#ifndef STRDUP_H
#define STRDUP_H

#include <string.h>

static inline char *strdup(const char *source) {
	int size = source ? (strlen(source)+1) : 0;
	char *dest = malloc(size);
	if (dest) {
		memcpy(dest, source, size);
	}
	return dest;
}

#endif
