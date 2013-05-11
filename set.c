#include "set.h"

#include "rb-tree.h"

#include <assert.h>

Set *set_new(void *(*copy)(const void *), int (*compare)(const void *, const void *), void (*destroy)(void *))
{
    return (Set*)rbtree_new(copy, compare, destroy, NULL, NULL, NULL);
}

bool set_equal(const void *a, const void *b)
{
    return rbtree_equal(a, b);
}

void set_destroy(void *set)
{
    rbtree_destroy(set);
}

bool set_add(Set *set, void *element)
{
    assert(element);
    return rbtree_put((RBTree *)set, element, element);
}

bool set_contains(const Set *set, const void *element)
{
    return rbtree_get((const RBTree *)set, element) == element;
}

bool set_remove(Set *set, const void *element)
{
    return rbtree_remove((RBTree *)set, element);
}

void set_clear(Set *set)
{
    rbtree_clear((RBTree *)set);
}

size_t set_size(const Set *set)
{
    return rbtree_size((const RBTree *)set);
}

SetIterator *set_iterator_new(const Set *set)
{
    return (SetIterator*)rbtree_iterator_new((RBTree *)set);
}

void *set_iterator_next(SetIterator *iter)
{
    void *element = NULL;
    if (rbtree_iterator_next((RBTreeIterator*)iter, &element, NULL))
    {
        return element;
    }
    else
    {
        return NULL;
    }
}

void set_iterator_destroy(void *iter)
{
    rbtree_iterator_destroy(iter);
}
