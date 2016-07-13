#ifndef __woo_reactor_H__
#define __woo_reactor_H__

namespace woo {

struct reactor_t;
typedef struct reactor_t reactor_t;

typedef int (* reactor_event_handle_t) (reactor_t *reactor, void *handle_data, int handle_fd, int status);

reactor_t* reactor_create(int pool_thread_num);
int reactor_add(reactor_t *reactor, reactor_handle_t handle, void *handle_data, int handle_fd, int timeout);
int reactor_remove(reactor_t *reactor, int fd);
int reactor_run(reactor_t *reactor);
void reactor_quit(reactor_t *reactor);
void reactor_wait(reactor_t *reactor);
void reactor_destroy(reactor_t *reactor);

}

#endif
