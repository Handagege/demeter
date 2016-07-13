#ifndef __woo_tcp_server__h__
#define __woo_tcp_server__h__

#include <sys/time.h>
#include <pthread.h>
#include <stdint.h>
#include <queue>

namespace woo {

struct _tcp_server_t;
typedef struct _tcp_server_t tcp_server_t;

typedef int (*recv_handle_t)(void *handle_data, int sock, char *buf, size_t *data_len, 
		size_t buf_size);
typedef int (*proc_handle_t)(void *handle_data, char *input, uint32_t input_len, 
		char *output, uint32_t *output_len, char *msg, size_t msg_size);

tcp_server_t *tcp_server_create();
int tcp_server_open(tcp_server_t *server, const char *ip, int port,
		recv_handle_t recv, proc_handle_t proc, void **handle_data, int thread_num, 
		bool long_conn, size_t input_buf_size, size_t output_buf_size, int recv_timeout, int send_timeout);
int tcp_server_run(tcp_server_t *server);
int tcp_server_wait(tcp_server_t *server);
void tcp_server_stop(tcp_server_t *server);
int tcp_server_destroy(tcp_server_t *server);

typedef struct _binary_head_t {
	uint32_t body_len;
	uint32_t log_id;
    char clent[6];
    uint8_t reserved;
    uint8_t flags;
} binary_head_t;

int scgi_recv(void *handle_data, int sock, char *buf, size_t *data_len, size_t buf_size);
int binary_recv(void *handle_data, int sock, char *buf, size_t *data_len, size_t buf_size);

int safe_recv(int sock, char *buf, size_t len, int flags);
}

#endif
