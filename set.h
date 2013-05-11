#ifndef LIBUTILS_SET_H
#define LIBUTILS_SET_H

#include <stdbool.h>
#include <stddef.h>

typedef void *Set;
typedef void *SetIterator;

Set *set_new(void *(*copy)(const void *), int (*compare)(const void *, const void *), void (*destroy)(void *));
bool set_equal(const void *a, const void *b);
void set_destroy(void *set);

bool set_add(Set *set, void *element);
bool set_contains(const Set *set, const void *element);
bool set_remove(Set *set, const void *element);
void set_clear(Set *set);
size_t set_size(const Set *set);

SetIterator *set_iterator_new(const Set *set);
void *set_iterator_next(SetIterator *iter);
void set_iterator_destroy(void *iter);


#endif
