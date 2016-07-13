#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "woo/binaryclient.h"
#include "woo/log.h"

int main(int argc, char **argv) {
	if (argc < 4) {
		printf("%s host port req_body [log_id]\n", argv[0]);
		return 1;
	}
	const char *host = argv[1];
	int port = atoi(argv[2]);
	const char *req = argv[3];
	uint32_t log_id = 333333333;
	if (argc > 4) {
		log_id = strtoul(argv[4], NULL, 0);
	}
	const size_t BUF_SIZE = 1024*1000;
	char *buf = (char *)malloc(BUF_SIZE);
	if (! buf) {
		return -1;
	}
	woo::binary_client_t *cli = woo::binary_client_create(host, port, 1000000, 1000000);
	uint32_t data_len;
	printf("req:%s\n", req);
	ssize_t ret = woo::binary_client_talk(cli, req, strlen(req),
		buf, &data_len, BUF_SIZE, log_id);
	if (ret) {
		LOG_ERROR("talk to [%s:%d] error", host, port);
		return -1;
	}
	buf[data_len] = '\0';
	printf("rsep:%s\n", buf);

	return 0;
}
