#ifndef LIBUTILS_RB_TREE_H
#define LIBUTILS_RB_TREE_H

#include <stdbool.h>

typedef struct _RBTree RBTree;
typedef struct _RBTreeIterator RBTreeIterator;

RBTree *rbtree_new(void *(*key_copy)(const void *key),
                   int (*key_compare)(const void *a, const void *b),
                   void (*key_destroy)(void *key),
                   void *(*value_copy)(const void *key),
                   int (*value_compare)(const void *a, const void *b),
                   void (*value_destroy)(void *key));

bool rbtree_equal(const void *a, const void *b);

void rbtree_destroy(void *rb_tree);

bool rbtree_put(RBTree *tree, const void *key, const void *value);
void *rbtree_get(const RBTree *tree, const void *key);
bool rbtree_remove(RBTree *tree, const void *key);
void rbtree_clear(RBTree *tree);
unsigned int rbtree_size(const RBTree *tree);

RBTreeIterator *rbtree_iterator_new(const RBTree *tree);
bool rbtree_iterator_next(RBTreeIterator *iter, void **key, void **value);
void rbtree_iterator_destroy(void *_rb_iter);

#endif
