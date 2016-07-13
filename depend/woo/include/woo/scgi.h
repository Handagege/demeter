#ifndef __woo_scgi_H__
#define __woo_scgi_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace woo {

typedef struct _strptr_pair_t {
	const char *name;
	const char *value;
} strptr_pair_t;

const char *
strpair_get(strptr_pair_t* pairs, size_t num, const char *name, const char *def_value = NULL);

int 
parse_scgi_headers(strptr_pair_t *headers, size_t *num, size_t max_num, const char *str, size_t len);

int 
parse_query_string(strptr_pair_t *params, size_t *num, size_t max_num, char *str, size_t len);

}

#endif
