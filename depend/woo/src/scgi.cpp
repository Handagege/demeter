#include "woo/scgi.h"
#include "woo/log.h"

namespace woo {

inline int hexchr2num(char c) {
	if ('0' <= c && '9' >= c) {
		return c - '0';
	} else if ('a' <= c && 'f' >= c) {
		return c - 'a' + 10;
	} else if ('A' <= c && 'F' >= c) {
		return c - 'A' + 10;
	} else {
		return -1;
	}
}

const char *
strpair_get(strptr_pair_t* pairs, size_t num, const char *name, const char *def_value) {
	for (size_t i = 0; i < num; ++ i) {
		if (strcmp(pairs[i].name, name) == 0) {
			return pairs[i].value;
		}
	}
	return def_value;
}

int 
parse_query_string(strptr_pair_t *params, size_t *num, size_t max_num, char *str, size_t len) {
	size_t i = 0;
	char *p = str;
	char *end = str + len;
	while (i < max_num && p < end) {
		params[i].name =  p;
		while(*p != '=' && *p != '&' && p < end) {
			p ++;
		}
		if (p == end || *p == '&') {
			return -1;
		}
		*p = '\0';
		//LOG_DEBUG("name:%s", params[i].name);
		params[i].value = ++ p;
		while (*p != '&' && p < end) {
			p ++;
		}
		if (*p == '&') {
			*p = '\0';
			++ p;
		}
		//LOG_DEBUG("escape value:%s", params[i].value);
		{
			char *q1 = (char *)params[i].value;
			const char *q2 = params[i].value;
			while (*q2) { 
				if (*q2 == '%') {
					if (*(q2+1) == '\0' || *(q2+2) == '\0') {
						return -1;
					}
					//LOG_DEBUG("str:%c%c", *(q2+1), *(q2+2));
					*q1 = (hexchr2num(*(q2+1))<<4) | hexchr2num(*(q2+2)); 
					//LOG_DEBUG("value:%02X", *q1);
					q2 += 3;
					++ q1;
				} else if (*q2 == '+') {
					*q1 = ' '; 
					++ q2;
					++ q1;
				} else {
					*q1 = *q2; 
					++ q2;
					++ q1;
				}
			}
			*q1 = '\0';
		}
		//LOG_DEBUG("unescape value:%s", params[i].value);
		++ i;
	}
	*num = i;
	return 0;
}

int parse_scgi_headers(strptr_pair_t *headers, size_t *num, size_t max_num, const char *str, size_t len) {
	size_t i = 0;
	const char *p = str;
	const char *end = str + len;
	while (i < max_num && *p && p < end) {
		headers[i].name =  p;
		//LOG_DEBUG("name:%s", headers[i].name);
		while(*p && p < end) {
			p ++;
		}
		if (p == end) {
			return -1;
		}
		headers[i].value = ++ p;
		//LOG_DEBUG("value:%s", headers[i].value);
		while(*p && p < end) {
			p ++;
		}
		if (p == end) {
			return -1;
		}
		++ p;
		++ i;
	}
	*num = i;
	return 0;
}

}
