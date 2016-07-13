#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include "woo/log.h"
#include "woo/tcpserver.h"

namespace woo {

struct _server_thread_t;
typedef struct _tcp_server_t {
	struct _server_thread_t *thr_arr;
	int thr_num;
	int listen_sock;
	int epoll_fd;
	bool quit;
	struct timeval recv_to;
	struct timeval send_to;
	bool long_conn;
	pthread_mutex_t queue_mutex;
	pthread_cond_t queue_cond;
	std::queue<int> *queue;
	size_t max_queue_size;
} tcp_server_t;

tcp_server_t *tcp_server_create() {
	tcp_server_t *server;
	server = (tcp_server_t *)malloc(sizeof(tcp_server_t));
	if (! server) {
		return NULL;
	}
	server->listen_sock = -1;
	server->epoll_fd = -1;
	return server;
}

int safe_recv(int sock, char *buf, size_t len, int flags) {
	size_t recv_len = 0; 
	while (recv_len < len) {
		ssize_t ret = recv(sock, buf + recv_len, len - recv_len, flags);
		if (ret <= 0) {
			LOG_ERROR("recv ret %d had recv[%u] total[%u] errno[%d][%m]", ret, recv_len, 
					len, errno);
			return ret;
		} else {
			recv_len += ret;
		}
    }
	return recv_len;
}

int scgi_recv(void *handle_data, int sock, char *buf, size_t *data_len, size_t buf_size) {
	const int FIRST_RECV_LEN = 12;
	const char *CONTENT_LENGTH = "CONTENT_LENGTH";

	memset(buf, 0, FIRST_RECV_LEN + 1);
	ssize_t first_recved_len = recv(sock, buf, FIRST_RECV_LEN, MSG_DONTWAIT);
	if (first_recved_len < 0) {
		LOG_ERROR("scgi recv first [%d]bytes, ret[%zd] error[%d][%m]", FIRST_RECV_LEN,
				first_recved_len, errno);
		return -1;
	}
	if (! strchr(buf, ':')) {
		LOG_ERROR("scgi recv beg[%s], not contain':'", buf);
		return -1;
	}
	char *ptr = NULL;
	long head_len_len = 0;
	long head_len = strtol(buf, &ptr, 0);
	if (head_len < long(strlen(CONTENT_LENGTH) + 3)) {
		LOG_ERROR("head len[%lu] is too short", head_len);
		return -1;
	}
	if (*ptr == ':') {
		head_len_len = ptr - buf;
		++ ptr;
	} else {
		LOG_ERROR("scgi headlen[%s] not end with contain':'", buf);
		return -1;
	}

	uint32_t total_head_len = head_len_len + 2 + head_len;
	ssize_t nrecv;
	nrecv = safe_recv(sock, buf + first_recved_len, total_head_len - first_recved_len, 0);
	if (nrecv != ssize_t(total_head_len - first_recved_len)) {
		LOG_ERROR("recv ret[%zd] ne [%u]", nrecv, total_head_len - first_recved_len);
		return -1;
	}

	if (strcmp(CONTENT_LENGTH, ptr)) {
		LOG_ERROR("protocol error, first head[%s] not %s", ptr, CONTENT_LENGTH);
		return -1;
	}
	ptr += strlen(CONTENT_LENGTH);
	if (*ptr == '\0') {
		++ ptr;
	} else {
		LOG_ERROR("protocol error, first head[%s] ne %s", ptr, CONTENT_LENGTH);
		return -1;
	}
	if (buf[total_head_len - 1] != ',') {
		LOG_ERROR("protocol error, head[%c] not end with,", buf[total_head_len - 1]);
		return -1;
	}

	long body_len = strtol(ptr, &ptr, 0);

	nrecv = safe_recv(sock, buf + total_head_len, body_len, 0);
	if (nrecv != body_len) {
		LOG_ERROR("recv ret[%zd] ne [%lu]", nrecv, body_len);
		return -1;
	}
	*data_len = total_head_len + body_len;

	return 0;
}

int binary_recv(void *handle_data, int sock, char *buf, size_t *data_len, size_t buf_size) {
	binary_head_t *head = (binary_head_t *)buf;

	ssize_t ret = recv(sock, head, sizeof(binary_head_t), 0);
	if (ret != sizeof(binary_head_t)) {
		LOG_ERROR("recv %d errno[%d][%m]", ret, errno);
		return -1;
	}
	set_log_id(head->log_id);
	if (head->body_len + sizeof(binary_head_t) + 16 > buf_size)  {
		LOG_ERROR("body len[%u] is too large, buf_size[%zu]", head->body_len, buf_size);
		return -1;
	}

	ssize_t recv_body_len = safe_recv(sock, buf + sizeof(binary_head_t), head->body_len, 0);
	if (recv_body_len != ssize_t(head->body_len)) {
		LOG_ERROR("recv body len[%u] less to [%u]", recv_body_len, head->body_len);
		return -1;
	}
	*data_len = sizeof(binary_head_t) + head->body_len;

	return 0;
}

int default_send(int sock, char *buf, size_t data_len) {
	ssize_t ret = send(sock, buf, data_len, 0);
	if (ret != ssize_t(data_len)) {
		LOG_ERROR("send ret[%d] errno[%d][%m]", ret, errno);
		return -1;
	}
		
	return 0;
}

int setnoblock(int fd) {
	int opt;
	if ((opt = fcntl(fd, F_GETFL)) < 0) {
		return -1;
	}
	opt|=O_NONBLOCK;
	if(fcntl(fd, F_SETFL, opt) < 0) {
		return -1;
	}
	return 0;
}

typedef struct _server_thread_t {
	struct _tcp_server_t *server;
	recv_handle_t recv_handle;
	proc_handle_t proc_handle;
	void *handle_data;
	pthread_t thread_id;
	char *input_buf;
	char *output_buf;
	size_t input_buf_size;
	size_t output_buf_size;
} server_thread_t;

void *tcp_server_process(void* arg) {
	server_thread_t *thr = (server_thread_t *)arg;
	tcp_server_t *server = thr->server;
	//uint32_t body_len;
	uint32_t output_len;
	char *input_buf = thr->input_buf;
	size_t input_buf_size = thr->input_buf_size;
	char *output_buf = thr->output_buf;
	int sock;
	int ret;
	struct timeval tv_beg;
	struct timeval tv_end;
	int proc_msec;
	int recv_msec;
	int send_msec;
	struct epoll_event ev;
	char msg[128];
	size_t data_len;

	open_thread_log();

	while (! server->quit) {
		set_log_id(0);
		ret = pthread_mutex_lock(&server->queue_mutex);
		if (ret == 0) {
			//LOG_DEBUG("get lock from pop");
			while (server->queue->empty()) {
				//LOG_DEBUG("queue is empty, goto wait");
				//printf("queue is empty, goto wait\n");
				pthread_cond_wait(&server->queue_cond, &server->queue_mutex);
				//printf("wake from wait\n");
			}
			LOG_DEBUG("beg to get a sock from queue, %zu", server->queue->size());
			//printf("goto get a sock from queue, %zu\n", server->queue->size());
			sock = server->queue->front();
			server->queue->pop();
			LOG_DEBUG("have got a sock from queue, %zu", server->queue->size());
			//printf("have got a sock from queue, %zu\n", server->queue->size());
			pthread_mutex_unlock(&server->queue_mutex);
			//printf("unloc from queue\n");
		} else {
			LOG_ERROR("pop sock from queue, lock mutex error[%d]", ret);
			continue;
		}

		gettimeofday(&tv_beg, NULL);
		ret = thr->recv_handle(thr->handle_data, sock, input_buf, &data_len, input_buf_size);
		if (ret) {
			LOG_ERROR("recv %d", ret);
			close(sock);
			continue;
		}
		gettimeofday(&tv_end, NULL);
		recv_msec = (tv_end.tv_sec - tv_beg.tv_sec) * 1000 + (tv_end.tv_usec - tv_beg.tv_usec) / 1000;

		gettimeofday(&tv_beg, NULL);
		msg[0] = '\0';
		ret = thr->proc_handle(thr->handle_data, input_buf, data_len, output_buf, &output_len, msg, sizeof(msg));
		if (ret) {
			LOG_ERROR("handle %d", ret);
			close(sock);
			continue;
		}
		gettimeofday(&tv_end, NULL);
		proc_msec = (tv_end.tv_sec - tv_beg.tv_sec) * 1000 + (tv_end.tv_usec - tv_beg.tv_usec) / 1000;

		gettimeofday(&tv_beg, NULL);
		ret = default_send(sock, output_buf, output_len);
		if (ret) {
			LOG_ERROR("send %d", ret);
			close(sock);
			continue;
		}
		gettimeofday(&tv_end, NULL);
		send_msec = (tv_end.tv_sec - tv_beg.tv_sec) * 1000 + (tv_end.tv_usec - tv_beg.tv_usec) / 1000;

		if (server->long_conn) {
			ev.events = EPOLLIN;
			ev.data.fd = sock;
			if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0) {
				LOG_ERROR("after process, epoll add error: fd=%d", sock);
				close(sock);
			}
		} else {
			close(sock);
		}
		LOG_INFO("cost:(%u-%u-%u)ms %s", recv_msec, proc_msec, send_msec, msg);
		set_log_id(0);
	}
	return NULL;
}

int tcp_server_open(tcp_server_t *server, const char *ip, int port,
		recv_handle_t recv_handle, proc_handle_t proc_handle, void **handle_data, 
		int thread_num, bool long_conn, 
		size_t input_buf_size, size_t output_buf_size, int recv_timeout, int send_timeout) {
	struct sockaddr_in listen_addr;
	struct epoll_event ev;
	int ret;

	server->recv_to.tv_usec = recv_timeout % 1000000;
	server->recv_to.tv_sec = recv_timeout / 1000000;
	server->send_to.tv_usec = send_timeout % 1000000;
	server->send_to.tv_sec = send_timeout / 1000000;
	server->max_queue_size = 256;
	server->queue = new std::queue<int>();
	if (! server->queue) {
		LOG_ERROR("create queue error");
		return -1;
	}

	int listen_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(-1 == listen_sock) {
		LOG_ERROR("can not create socket");
		return -1;
	}
	int optval = 1;
	if (setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval,sizeof(int)) < 0) {
		LOG_ERROR("can not set socket");
		return -1;
	}
	memset(&listen_addr, 0, sizeof(listen_addr));

	listen_addr.sin_family = AF_INET;
	listen_addr.sin_port = htons(port);
	if (ip) {
		listen_addr.sin_addr.s_addr = inet_addr(ip);
	} else {
		listen_addr.sin_addr.s_addr = INADDR_ANY;
	}

	if(-1 == bind(listen_sock,(struct sockaddr *)&listen_addr, sizeof(listen_addr))) {
		LOG_ERROR("error bind failed");
		close(listen_sock);
		return -1;
	}

	if(-1 == listen(listen_sock, 100)) {
		LOG_ERROR("error listen failed");
		close(listen_sock);
		return -1;
	}
	if (setnoblock(listen_sock)) {
		LOG_ERROR("set listen no block failed");
		close(listen_sock);
		return -1;
	}
	server->listen_sock = listen_sock;
	server->quit = false;
	server->long_conn = long_conn;

	server->epoll_fd = epoll_create(4096);
	ev.data.fd = listen_sock;
	ev.events = EPOLLIN;
	ret = epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, listen_sock, &ev);
	if (ret) {
		LOG_ERROR("epoll_ctl error errno[%d][%m]", errno);
		return -1;
	} 
	ret = pthread_mutex_init(&server->queue_mutex, NULL);
	if (ret) {
		LOG_ERROR("pthread_mutex_init ret[%d] errno[%d][%m]", ret, errno);
		return -1;
	}
	ret = pthread_cond_init(&server->queue_cond, NULL);
	if (ret) {
		LOG_ERROR("pthread_cond_init ret[%d] errno[%d][%m]", ret, errno);
		return -1;
	}

	server_thread_t *thr_arr = (server_thread_t *)malloc(sizeof(server_thread_t) * thread_num);
	if (! thr_arr) {
		LOG_DEBUG("create thr arr error");
		return -1;
	}
	server->thr_arr = thr_arr;
	server->thr_num= thread_num;
	if (! recv_handle) {
		recv_handle = binary_recv;
	}
	for (int i = 0; i < thread_num; ++ i) {
		thr_arr[i].server = server;
		thr_arr[i].recv_handle = recv_handle;
		thr_arr[i].proc_handle = proc_handle;
		thr_arr[i].output_buf_size = output_buf_size;
		thr_arr[i].input_buf_size = input_buf_size;
		thr_arr[i].input_buf = (char *)malloc(input_buf_size);
		if (! thr_arr[i].input_buf) {
			LOG_ERROR("malloc input buf error");
			return -1;
		}
		thr_arr[i].output_buf = (char *)malloc(output_buf_size);
		if (! thr_arr[i].output_buf) {
			LOG_ERROR("malloc output buf error");
			return -1;
		}
		if (handle_data) {
			thr_arr[i].handle_data = handle_data[i];
			//LOG_DEBUG("handle_data [%p]", handle_data[i]);
			//LOG_DEBUG("set handle_data [%p]", thr_arr[i].handle_data);
		} else {
			thr_arr[i].handle_data = NULL;
		}
	}
	for (int i = 0; i < thread_num; ++ i) {
		ret = pthread_create(&thr_arr[i].thread_id, NULL, tcp_server_process, thr_arr + i);
		if (ret) {
			LOG_ERROR("create thread error");
			return -1;
		}
	}

	return 0;
}

int tcp_server_run(tcp_server_t *server) {
	const int MAX_EVENTS = 128;
	struct epoll_event ev, events[MAX_EVENTS];
	int nfds;
	int n;
	struct sockaddr_in client_addr;
	socklen_t addrlen ;
	int sock;
	int ret;

	while (! server->quit) {
		nfds = epoll_wait(server->epoll_fd, events, MAX_EVENTS, 20);
		for (n = 0; n < nfds; ++n) {
			if(events[n].data.fd == server->listen_sock) {
				addrlen = sizeof(client_addr);
				sock = accept(server->listen_sock, (struct sockaddr *) &client_addr,
						&addrlen);
				LOG_TRACE("accept a connection from[%s]", inet_ntoa(client_addr.sin_addr));
				if(sock < 0){
					LOG_ERROR("accept connection error");
					continue;
				}
				if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &server->recv_to, sizeof(server->recv_to)) < 0) {
					LOG_ERROR("set sock recv timeout error");
					close(sock);
					continue;
				}
				if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &server->send_to, sizeof(server->send_to)) < 0) {
					LOG_ERROR("set sock recv timeout error");
					close(sock);
					continue;
				}
				ev.events = EPOLLIN;
				ev.data.fd = sock;
				if (epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0) {
					LOG_ERROR("epoll add error: fd=%d", sock);
					close(sock);
					continue;
				}
			} else {
				//LOG_DEBUG("a sock is ready for read");
				memset(&ev, 0, sizeof(ev));
				if (epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, events[n].data.fd, &ev) < 0) {
					LOG_ERROR("epoll del error: fd=%d", events[n].data.fd);
					return -1;
				}
				ret = pthread_mutex_lock(&server->queue_mutex);
				if (ret == 0) {
					//LOG_DEBUG("before push queue size[%zu]", server->queue->size());
					if (server->queue->size() >= server->max_queue_size) {
						pthread_mutex_unlock(&server->queue_mutex);
						close(events[n].data.fd);
						LOG_ERROR("queue size[%zu] is full", server->queue->size());
					} else {
						server->queue->push(events[n].data.fd);
						LOG_DEBUG("after push queue size[%zu]", server->queue->size());
						//pthread_cond_signal(&server->queue_cond);
						pthread_cond_broadcast(&server->queue_cond);
						pthread_mutex_unlock(&server->queue_mutex);
					}
				} else {
					LOG_ERROR("push sock to queue lock mutext error[%d]", ret);
				}
			}
		}
	}
	return 0;
}

int tcp_server_wait(tcp_server_t *server) {
	for (int i = 0; i < server->thr_num; ++ i) {
		pthread_join(server->thr_arr[i].thread_id, NULL);
	}
	return 0;
}

void tcp_server_stop(tcp_server_t *server) {
	server->quit = true;
}

int tcp_server_destroy(tcp_server_t *server) {
	pthread_cond_destroy(&server->queue_cond);
	pthread_mutex_destroy(&server->queue_mutex);
	return 0;
}

}
