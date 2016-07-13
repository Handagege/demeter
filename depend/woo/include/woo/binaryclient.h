#ifndef __woo_binary_client__h__
#define __woo_binary_client__h__

#include <sys/time.h>
#include <stdint.h>

namespace woo {

struct _binary_client_t;
typedef struct _binary_client_t binary_client_t;

binary_client_t*
binary_client_create(const char *host, int port, int send_to, int recv_to);

void
binary_client_destroy(binary_client_t *client);

ssize_t 
binary_client_send(binary_client_t *client, const char *data, uint32_t data_len, uint32_t log_id);

ssize_t 
binary_client_recv(binary_client_t *client, char *buf, uint32_t *data_len, uint32_t buf_size, uint32_t *log_id);

ssize_t 
binary_client_talk(binary_client_t *client, const char *req, uint32_t req_len,
		char *buf, uint32_t *data_len, uint32_t buf_size, uint32_t log_id);

ssize_t 
safe_send(int s, const char *buf, size_t len, int flags);

}

#endif
