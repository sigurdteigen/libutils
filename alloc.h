#ifndef LIBUTILS_ALLOC_H
#define LIBUTILS_ALLOC_H

#include <stddef.h>
#include <stdarg.h>

void *xcalloc(size_t nmemb, size_t size);
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);

void *xmemdup(const void *ptr, size_t size);
void *xmemcpy(void *dst, const void *src, size_t size);

#endif
