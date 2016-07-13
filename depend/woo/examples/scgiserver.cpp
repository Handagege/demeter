#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ext/hash_set>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include "woo/log.h"
#include "woo/tcpserver.h"
#include "woo/scgi.h"

int req_handle(void *handle_data, char *input, uint32_t input_len, char *output, uint32_t *output_len, char *msg, size_t msg_size) {
	const int MAX_HEADER_NUM = 32;
	const int MAX_PARAM_NUM = 16;
	const int MAX_QUERY_STRING_LEN = 2048;
	woo::strptr_pair_t headers[MAX_HEADER_NUM];
	woo::strptr_pair_t params[MAX_PARAM_NUM];
	char query_string[MAX_QUERY_STRING_LEN];
	size_t header_num;
	size_t param_num;

	LOG_DEBUG("input:%s", input);

	char *headers_str = NULL;
	uint32_t headers_len = strtoul(input, &headers_str, 0);
	++ headers_str;

	int ret = woo::parse_scgi_headers(headers, &header_num, MAX_HEADER_NUM, 
			headers_str, headers_len);
	if (ret < 0) {
		strcpy(output, "Status: 200 OK\r\n" "Content-Type: text/plain\r\n" "\r\n" "Parse Headers Error.");
		*output_len = strlen(output);
		return 0;
	}

	const char *query_str = strpair_get(headers, header_num, "QUERY_STRING");
	if (! query_str) {
		strcpy(output, "Status: 200 OK\r\n" "Content-Type: text/plain\r\n" "\r\n" "Query String Error.");
		*output_len = strlen(output);
		return 0;
	}
	strncpy(query_string, query_str, MAX_QUERY_STRING_LEN);

	//parse_query_string will modify query_str
	LOG_DEBUG("query string:%s", query_string);
	ret = parse_query_string(params, &param_num, MAX_PARAM_NUM, 
			query_string, strlen(query_string));
	if (ret) {
		strcpy(output, "Status: 200 OK\r\n" "Content-Type: text/plain\r\n" "\r\n" "Parse Param Error.");
		*output_len = strlen(output);
		return 0;
	}

	strcpy(output, "Status: 200 OK\r\n" "Content-Type: text/plain\r\n" "\r\n");
	strcat(output, "headers:\r\n");
	for (size_t i = 0; i < header_num; ++ i) {
		strcat(output, headers[i].name);
		strcat(output, ":");
		strcat(output, headers[i].value);
		strcat(output, "\r\n");
	}
	strcat(output, "\r\nparams:\r\n");
	for (size_t i = 0; i < param_num; ++ i) {
		strcat(output, params[i].name);
		strcat(output, ":");
		strcat(output, params[i].value);
		strcat(output, "\r\n");
	}
	
	*output_len = strlen(output);
	LOG_DEBUG("output:%s", output);
	return 0;
}

typedef struct _conf_t {
	char log_path[PATH_MAX];
	char ip[32];
	int port;
	bool long_conn;
	int thread_num;
	size_t recv_buf_size;
	size_t send_buf_size;
	int recv_to;
	int send_to;
} conf_t;

conf_t g_conf;

int main(int argc, char ** argv) {
	signal(SIGPIPE, SIG_IGN);

	if (argc < 2) {
		printf("usage:%s port \n", argv[0]);
		return -1;
	}

	strncpy(g_conf.ip, "0.0.0.0", sizeof(g_conf.ip));
	g_conf.port = atoi(argv[1]);
	g_conf.thread_num = 6;
	g_conf.long_conn = false;
	strncpy(g_conf.log_path, "./scgiserver.log", sizeof(g_conf.log_path));
   	g_conf.recv_buf_size = 1024*1000;
   	g_conf.send_buf_size = 1024*1000;
	g_conf.recv_to = 1000000;
   	g_conf.send_to = 1000000;

	woo::open_log(g_conf.log_path, 1024 *100, "ewidt", "ewi");

	woo::tcp_server_t *server = woo::tcp_server_create();
	woo::tcp_server_open(server, g_conf.ip, g_conf.port, woo::scgi_recv, req_handle, NULL, 
			g_conf.thread_num, g_conf.long_conn, 
			g_conf.recv_buf_size, g_conf.send_buf_size, g_conf.recv_to, g_conf.send_to);
	woo::tcp_server_run(server);
	woo::tcp_server_wait(server);
	woo::tcp_server_destroy(server);

	return 0;
}
