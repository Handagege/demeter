#include <sys/epoll.h>
#include <pthread.h>

namespace woo {

typedef struct _reactor_event_data_t {
	void *handle_data;
	reactor_event_handle_t handle;
	uint32_t timeout;
	int fd;
	int flag;
} reactor_event_data_t;

typedef struct reactor_t {
	int epoll_fd;
	int max_event_num;
	struct epoll_event *events;
	int pool_thread_num;
	pthread_t *pool_thread;
	bool quit;
	pthread_mutex_t queue_mutex;
	pthread_cond_t queue_cond;
	std::queue<reactor_event_data_t> *queue;
	size_t max_queue_size;
} reactor_t;

reactor_t* reactor_create(int pool_thread_num) {
	reactor_t *reactor = (reactor_t *)malloc(sizeof(reactor_t));
	if (! reactor) {
		return NULL;
	}
	reactor->epoll_fd = epoll_create(4096);
	if (epoll_fd  < 0) {
		return NULL;
	}
	reactor->pool_thread_num = pool_thread_num;
	reactor->pool_thread = (pthread_t *)malloc(sizeof(pthread_t) * pool_thread_num);
	if (! reactor->pool_thread) {
		return NULL;
	}
	reactor->quit = false;
	reactor->max_event_num = 128;
	reactor->events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * reactor->max_event_num);
	if (! reactor->events) {
		return NULL;
	}

	reactor->max_queue_size = 256;
	reactor->queue = new std::queue<int>();
	if (! reactor->queue) {
		LOG_ERROR("create queue error");
		return NULL;
	}

	ret = pthread_mutex_init(&reactor->queue_mutex, NULL);
	if (ret) {
		LOG_ERROR("pthread_mutex_init ret[%d] errno[%d][%m]", ret, errno);
		return NULL;
	}

	ret = pthread_cond_init(&reactor->queue_cond, NULL);
	if (ret) {
		LOG_ERROR("pthread_cond_init ret[%d] errno[%d][%m]", ret, errno);
		return NULL;
	}

	return reactor;
}

int reactor_add(reactor_t *reactor, reactor_handle_t handle, void *arg, int fd, int timeout) {
	reactor_event_data_t *event_data = malloc(sizeof(reactor_event_data_t));
	if (! event_data) {
		return -1;
	}	
	event_data->fd = fd;
	event_data->timeout = timeout;
	event_data->handle = handle;

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.ptr = event_data;
	if (epoll_ctl(reactor->epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
		fprintf(stderr, "epoll set insertion error: fd=%d\n",
				client);
		return -1;
	}
	return 0;
}

int reactor_remove(reactor_t *reactor, int fd) {
	struct epoll_event ev;
	return epoll_ctl(reactor->epoll_fd, EPOLL_CTL_DEL, fd, &ev);
}

int reactor_quit(reactor_t *reactor) {
	reactor->quit = true;
}

int reactor_destroy(reactor_t *reactor) {
	free(reactor->events);
	reactor->events = NULL;
	free(reactor->pool_thread);
	reactor->pool_thread = NULL;
	delete reactor->queue;
	reactor->queue = NULL;
	free(reactor);
}

static
void *reactor_pool_thread(void *arg) {
	while (! server->quit) {
		ret = pthread_mutex_lock(&server->queue_mutex);
		if (ret == 0) {
			while (server->queue->empty()) {
				LOG_DEBUG("queue is empty, goto wait");
				pthread_cond_wait(&server->queue_cond, &server->queue_mutex);
			}
			event = server->queue->front();
			server->queue->pop();
			pthread_mutex_unlock(&server->queue_mutex);
		} else {
			LOG_ERROR("pop sock from queue, lock mutex error[%d]", ret);
			continue;
		}

		ret = event->hanlde(reactor, event->fd, event->handle_data);
		if (ret) {
			close(event->fd);
		}

		if (event->flag & REACTOR_EVENT_KEEP_CONN) {
			ev.events = EPOLLIN;
			ev.data.fd = sock;
			if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
				LOG_ERROR("after process, epoll add error: fd=%d", sock);
				close(sock);
			}
		} else {
			close(event->fd);
		}
	}
}

void reactor_wait(reactor_t *reactor) {
	void *thread_ret;
	for (int i = 0; i < reactor->pool_thread_num; ++ i) {
		pthread_join(reactor->pool_thread[i], &thread_ret);
	}
}

int reactor_run(reactor_t *reactor) {
	int nfds;
	int i;
	int ret;

	for (int i = 0; i < pool_thread_num; ++ i) {
		ret = pthread_create(reactor->pool_thread + i, NULL, reactor_pool_thread, reactor);
		if (ret) {
			LOG_ERROR("pthread_create error");
			return -1;
		}
	}

	while (! reactor->quit) {
		nfds = epoll_wait(reactor->epoll_fd, reactor->events, reactor->max_event_num, 20);
		for (i = 0; i < nfds; ++ i) {
			if (events[i].data.ptr->flag & REACTOR_EVENT_MAIN_THREAD_EXEC) {
				if (! (events[i].data.ptr->flag & REACTOR_KEEP_CONN)) {
					memset(&ev, 0, sizeof(ev));
					if (epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &ev) < 0) {
						LOG_ERROR("epoll del error: fd=%d", events[n].data.fd);
						return -1;
					}
				}
				ptr = events[i].data.ptr;
				ptr->hanlde(reactor, ptr->handle_data, ptr->fd);
				if (! (events[i].data.ptr->flag & REACTOR_EVENT_KEEP_CONN)) {
					close(ptr->fd);
				}
			} else {
				memset(&ev, 0, sizeof(ev));
				if (epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, &ev) < 0) {
					LOG_ERROR("epoll del error: fd=%d", events[n].data.fd);
					return -1;
				}
				ret = pthread_mutex_lock(&server->queue_mutex);
				if (ret == 0) {
					LOG_DEBUG("queue size[%zu]", server->queue->size());
					if (server->queue->size() >= server->max_queue_size) {
						pthread_mutex_unlock(&server->queue_mutex);
						close(events[n].data.fd);
						LOG_ERROR("queue size[%zu] is full", server->queue->size());
					} else {
						queue->push_back(ptr);
						pthread_cond_broadcast(&server->queue_cond);
						pthread_mutex_unlock(&server->queue_mutex);
					}
				} else {
					LOG_ERROR("push sock to queue lock mutext error[%d]", ret);
				}
			}
		}
		//timeout queue
	}
}

}
