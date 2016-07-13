#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "woo/log.h"

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

namespace woo {

static const char LOG_LEVEL_STR[][8] = {
	"ERROR",
	"WARN",
	"INFO",
	"TRACE",
	"DEBUG"
};


typedef struct _log_file_t {
	int fd;
	time_t sw_time;
} log_file_t;

typedef struct _log_data_t {
	int is_open;
	log_file_t file[2];
	int idx;
	int created_key;
	pthread_key_t log_key;
	size_t buf_size;
	char path[128];
	int filter_level;
	int flush_level;
	int64_t log_id;
	pthread_mutex_t mutex;
} log_data_t;

typedef struct _log_thead_data_t {
	int64_t log_id;
	size_t len;
	size_t size;
	char buf[0];
} log_thread_data_t;

static log_data_t log_data;

int log_write(const char *buf, size_t len);

void close_thread_log(void *arg) {
	log_thread_data_t *ptr = (log_thread_data_t *)arg;
	log_write(ptr->buf, ptr->len);
	ptr->len = 0;
	free(ptr);
}

int level_str2int(const char *str) {
	int level = 0;
	for (uint32_t i = 0; i < sizeof(LOG_LEVEL_STR) / sizeof(LOG_LEVEL_STR[0]); ++ i) {
		if (strchr(str, LOG_LEVEL_STR[i][0]) || strchr(str, tolower(LOG_LEVEL_STR[i][0])) ) {
			level |= 1 << i;
		}
	}
	return level;
}

int open_log(const char *path, size_t buf_size, const char * write_level, const char *flush_level) {
	strncpy(log_data.path, path, sizeof(log_data.path));
	int fd;

	if (pthread_mutex_init(&log_data.mutex, NULL) != 0) {
		return -1;
	}

	log_data.filter_level = ~ (level_str2int(write_level));
	log_data.flush_level = level_str2int(flush_level);
	fd = open(log_data.path, O_CREAT|O_APPEND|O_WRONLY, 0644);
	if (fd < 0) {
		fprintf(stderr, "open log file[%s] error\n", log_data.path);
		return -1;
	}

	log_data.file[0].fd = fd;
	log_data.file[1].fd = -1;
	log_data.idx = 0;
	log_data.is_open = 1;
	log_data.buf_size = buf_size;

	if (pthread_key_create(&log_data.log_key, close_thread_log) < 0) {
		log(LOG_LEVEL_WARN, 1, "create pthread key error\n");
	}
	log_data.created_key = 1;

	return 0;
}

void close_log() {
	pthread_mutex_destroy(&log_data.mutex);
}

int open_thread_log() {
	if (! log_data.created_key) {
		fprintf(stderr, "pthread key not created for thread\n");
		return -1;
	}
	size_t size = log_data.buf_size;
	if (size < 1024 * 128) {
		size = 1024 * 128;
	}
	log_thread_data_t *ptr;
	if ((ptr = (log_thread_data_t *)pthread_getspecific(log_data.log_key)) == NULL) {
		ptr = (log_thread_data_t *)malloc(size + sizeof(log_thread_data_t));
		if (! ptr) {
			log(LOG_LEVEL_WARN, 1, "malloc pthread key buf error\n");
			return -1;
		}
		ptr->log_id = 0;
		ptr->len = 0;
		ptr->size = size;

		if (pthread_setspecific(log_data.log_key, ptr)) {
			log(LOG_LEVEL_WARN, 1, "set pthread key buf error\n");
			return -1;
		}
	}
	return 0;
}


int
log_write(const char *buf, size_t len) {
	if (! log_data.is_open) {
		return ::write(2, buf, len);
	}
	int fd;
	struct stat file_stat;
	time_t cur_time = time(NULL);
	if (log_data.file[1 - log_data.idx].fd >= 0 && cur_time > log_data.file[1 - log_data.idx].sw_time + 5) {
		if (pthread_mutex_trylock(&log_data.mutex) == 0) {
			if (log_data.file[1 - log_data.idx].fd >= 0 && cur_time > log_data.file[1 - log_data.idx].sw_time + 5) {
				close(log_data.file[1 - log_data.idx].fd);
				log_data.file[1 - log_data.idx].fd = -1;
			}
			pthread_mutex_unlock(&log_data.mutex);
		}
	}

	if (log_data.file[1 - log_data.idx].fd < 0) {
		if (stat(log_data.path, &file_stat) < 0) {
			if (pthread_mutex_trylock(&log_data.mutex) == 0) {
				if (log_data.file[1 - log_data.idx].fd < 0) {
					if (stat(log_data.path, &file_stat) < 0) {
						fd = open(log_data.path, O_CREAT|O_APPEND|O_WRONLY, 0644);
						if (fd >= 0) {
							log_data.file[1 - log_data.idx].fd = fd;
							log_data.idx = 1 - log_data.idx;
							log_data.file[1 - log_data.idx].sw_time = time(NULL);
						}
					}
				}
				pthread_mutex_unlock(&log_data.mutex);
			}
		}
	}

	fd = log_data.file[log_data.idx].fd;
	return write(fd, buf, len);
}

static int
log_write(int level, const char *fmt, va_list ap) {
	struct timeval tv;
	struct tm tm_res;
	char buf[1024 * 2];

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm_res);
	int n1 =  snprintf(buf, sizeof(buf), "%s [%04d-%02d-%02d %02d:%02d:%02d.%03d] [logid:%" PRId64 "] ", 
			LOG_LEVEL_STR[level], 1900 + tm_res.tm_year, 1 + tm_res.tm_mon, tm_res.tm_mday,
			tm_res.tm_hour, tm_res.tm_min, tm_res.tm_sec, int(tv.tv_usec / 1000),
			log_data.log_id);

	int n2 =  vsnprintf(buf + n1, sizeof(buf) - n1, fmt, ap);

	if (n2 + n1 > sizeof(buf) - 1) {
		buf[sizeof(buf) - 2] = '\n';
		return log_write(buf, sizeof(buf) - 1);
	} else {
		return log_write(buf, n1 + n2);
	}	
}

void
set_log_id(int64_t log_id) {
	log_thread_data_t *ptr = NULL;
	if (log_data.created_key) {
		ptr = (log_thread_data_t *)pthread_getspecific(log_data.log_key);
		if (ptr) {
			ptr->log_id = log_id;
			return;
		}
	}
	log_data.log_id = log_id;
}

int
log(int level, int flush, char *fmt, ...) {
	va_list ap;
	int ret;
	struct timeval tv;
	struct tm tm_res;

	log_thread_data_t *ptr = NULL;
	if ((1<<level) & log_data.filter_level) {
		return 0;
	}

	if (log_data.created_key) {
		ptr = (log_thread_data_t *)pthread_getspecific(log_data.log_key);
	}

	if (! ptr) {
		va_start(ap, fmt); 
		ret = log_write(level, fmt, ap);
		va_end(ap);
		return ret;
	}

	if (ptr->len + 1024 * 32 > ptr->size ) {
		log_write(ptr->buf, ptr->len);
		ptr->len = 0;
	}
	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm_res);

	ret =  snprintf(ptr->buf + ptr->len, ptr->size - ptr->len, "%s [%04d-%02d-%02d %02d:%02d:%02d.%03d] [logid:%"PRId64"] ", 
			LOG_LEVEL_STR[level], 1900 + tm_res.tm_year, 1 + tm_res.tm_mon, tm_res.tm_mday,
			tm_res.tm_hour, tm_res.tm_min, tm_res.tm_sec, int(tv.tv_usec / 1000),
			ptr->log_id);

	ptr->len += ret;

	va_start(ap, fmt); 
	ret =  vsnprintf(ptr->buf + ptr->len, ptr->size - ptr->len, fmt, ap);
	va_end(ap);
	ptr->len += ret;

	if (flush || ( (1<<level) & log_data.flush_level)) {
		log_write(ptr->buf, ptr->len);
		ptr->len = 0;
	}

	return 0;
}

}
