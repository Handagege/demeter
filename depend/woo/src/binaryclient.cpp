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
#include "woo/binaryclient.h"

namespace woo {

/*
int connect_to (int sfd, struct sockaddr *addr, int addrlen, struct timeval *timeout) {
	struct timeval sv;
	int svlen = sizeof(sv);
	int ret;

	if (!timeout)
		return connect (sfd, addr, addrlen);
	if (getsockopt (sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&sv, &svlen) < 0)
		return -1;
	if (setsockopt (sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)timeout, sizeof(timeout)) < 0)
		return -1;
	ret = connect (sfd, addr, addrlen);
	setsockopt (sfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&sv, sizeof(sv));
	return ret;
}
*/

struct _binary_client_t {
	char host[128];
	int port;
	int sock;
	struct timeval send_to;
	struct timeval recv_to;
};


binary_client_t*
binary_client_create(const char *host, int port, int send_to, int recv_to) {
	binary_client_t *client = (binary_client_t *)calloc(1, sizeof(binary_client_t));
	if (! client) {
		return NULL;
	}
	client->sock = -1;
	strncpy(client->host, host, sizeof(client->host));
	client->port = port;
	client->send_to.tv_usec = send_to % 1000000;
	client->send_to.tv_sec = send_to / 1000000;
	client->recv_to.tv_usec = recv_to % 1000000;
	client->recv_to.tv_sec = recv_to / 1000000;
	return client;
}

ssize_t 
safe_send(int s, const char *buf, size_t len, int flags) {
	size_t c = 0;
	while (c < len) {
		ssize_t n = ::send(s, buf + c, len - c , flags);
		if (n <= 0) {
			return -1;
		}
		c += n;
	}
	return c;
}

void
binary_client_destroy(binary_client_t *client) {
	if (client->sock >= 0) {
		close(client->sock);
		client->sock = -1;
	}
}

ssize_t 
binary_client_send(binary_client_t *client, const char *data, uint32_t data_len, uint32_t log_id) {
	if (client->sock < 0) {
		struct sockaddr_in addr;
		int sock = socket(AF_INET, SOCK_STREAM, 0);
		if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, 
					&client->recv_to, sizeof(client->recv_to)) < 0) {
			LOG_ERROR("set sock recv timeout error");
			close(sock);
			sock = -1;
			return -1;
		}
		if (setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, 
					&client->send_to, sizeof(client->send_to)) < 0) {
			LOG_ERROR("set sock recv timeout error");
			close(sock);
			sock = -1;
			return -1;
		}
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = inet_addr(client->host);
		addr.sin_port = htons(client->port);
		int ret = connect(sock, (const struct sockaddr *)&addr, sizeof(addr));
		if (ret) {
			close(sock);
			sock = -1;
			return -1;
		}
		client->sock = sock;
	}
	binary_head_t head;
	memset(&head, 0, sizeof(head));
	head.log_id = log_id;
	head.body_len = data_len;
	ssize_t n = safe_send(client->sock, (const char *)&head, sizeof(head), 0);
	if (n != sizeof(head)) {
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	n = safe_send(client->sock, data, data_len, 0);
	if (n != data_len) {
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	return 0;
}

ssize_t 
binary_client_recv(binary_client_t *client, char *buf, uint32_t *data_len, 
		uint32_t buf_size, uint32_t *log_id) {
	if (client->sock < 0) {
		return -1;
	}
	binary_head_t head;
	ssize_t ret = recv(client->sock, &head, sizeof(binary_head_t), 0);
	if (ret != sizeof(binary_head_t)) {
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	if (log_id) {
		*log_id = head.log_id;
	}
	if (head.body_len + sizeof(binary_head_t) > buf_size)  {
		close(client->sock);
		client->sock = -1;
		return -1;
	}

	ssize_t recv_body_len = safe_recv(client->sock, buf, head.body_len, 0);
	if (recv_body_len != ssize_t(head.body_len)) {
		close(client->sock);
		client->sock = -1;
		return -1;
	}
	*data_len = head.body_len;

	return 0;
}

ssize_t 
binary_client_talk(binary_client_t *client, const char *req, uint32_t req_len,
		char *buf, uint32_t *data_len, uint32_t buf_size, uint32_t log_id) {
	ssize_t	ret = binary_client_send(client, req, req_len, log_id);
	if (ret) {
		return -1;
	}
	return binary_client_recv(client, buf, data_len, buf_size, NULL);
}

}
