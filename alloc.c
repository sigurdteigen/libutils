#include "alloc.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#define CHECK_RETURN(pre, op)\
    assert(pre);\
    void *res = op;\
    assert(res && #op);\
    return res

void *xcalloc(size_t nmemb, size_t size)
{
    CHECK_RETURN(nmemb > 0 && size > 0, calloc(nmemb, size));
}

void *xmalloc(size_t size)
{
    CHECK_RETURN(size > 0, malloc(size));
}

void *xrealloc(void *ptr, size_t size)
{
    CHECK_RETURN(size > 0, realloc(ptr, size));
}

void *xmemdup(const void *ptr, size_t size)
{
    void *copy = xmalloc(size);
    CHECK_RETURN(size > 0, memcpy(copy, ptr, size));
}

void *xmemcpy(void *dst, const void *src, size_t size)
{
    CHECK_RETURN(size > 0, memcpy(dst, src, size));
}
