#include "woo/config.h"
#include "woo/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace woo {

char *rtrim(char *s) {
	char *back = s + strlen(s) - 1;
	while (isspace(*back)) {
		-- back; 
	}
	*(back + 1) = '\0';
	return s;
}

Conf *
conf_load(const char *path) {
	char buf[1024];
	FILE *fp = fopen(path, "r");
	if (! fp) {
		LOG_ERROR("open conf[%s] error", path);
		return NULL;
	}
	std::map<std::string, std::string> *conf =
		new std::map<std::string, std::string>();
	if (! conf) {
		fclose(fp);
		LOG_ERROR("new conf obj error");
		return NULL;
	}
	while (fgets(buf, sizeof(buf), fp)) {
		char *key = buf;
		while (*key && isspace(*key)) {
			++ key;
		}
		if (*key == '#') {
			continue;
		}
		char *delim = strchr(key, ':');
		if (! delim) {
			continue;
		}
		*delim = '\0';
		rtrim(key);
		char *value = delim + 1;
		while (*value && isspace(*value)) {
			++ value;
		}
		rtrim(value);
		//LOG_DEBUG("key:%s, value:%s", key, value);

		conf->insert(std::make_pair(std::string(key), std::string(value)));
	}
	fclose(fp);
	return conf;
}

void 
conf_destroy( Conf *conf) {
	delete conf;
}

const char *
conf_get_str(Conf *conf, const char *name, const char *def_value) {
	std::map<std::string, std::string>::iterator it;
	it = conf->find(name);
	if (it == conf->end()) {
		return def_value;
	}
	return it->second.c_str();
}

int
conf_get_int(Conf *conf, const char *name, int def_value) {
	const char *str;
	str = conf_get_str(conf, name, NULL);
	if (NULL == str) {
		return def_value;
	}
	return atoi(str);
}

long
conf_get_long(Conf *conf, const char *name, int def_value) {
	const char *str;
	str = conf_get_str(conf, name, NULL);
	if (NULL == str) {
		return def_value;
	}
	return strtol(str, NULL, 0);
}

int 
conf_get_server_array(Conf *conf, const char *server_name, ServerArray &server_arr) {
	int server_num;
	char name[128];
	snprintf(name, sizeof(name), "%s_num", server_name);
   	server_num = conf_get_int(conf, name, -1);
	if (server_num < 0) {
		LOG_ERROR("get %s[%d] error", name, server_num);
		return -1;
	}
	for (int i = 0; i < server_num; ++ i) {
		inet_socket_addr_t addr;
		snprintf(name, sizeof(name), "%s_%d_ip", server_name, i);
		const char *ip = conf_get_str(conf, name, NULL); 
		if (! ip) {
			LOG_ERROR("get %s error", name);
			return -1;
		}
		snprintf(name, sizeof(name), "%s_%d_port", server_name, i);
		int port = conf_get_int(conf, name, -1); 
		if (port <= 0) {
			LOG_ERROR("get %s error", name);
			return -1;
		}
		strncpy(addr.ip, ip, sizeof(addr.ip));
		addr.port = port;
		server_arr.push_back(addr);
	}

	return 0;
}

}
