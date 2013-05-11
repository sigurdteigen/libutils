#include "rb-tree.h"

#include "alloc.h"

#include <stdlib.h>
#include <assert.h>

typedef struct _RBNode RBNode;

struct _RBNode
{
    void *key;
    void *value;
    bool red;
    RBNode *parent;
    RBNode *left;
    RBNode *right;
};

struct _RBTree
{
    void *(*key_copy)(const void *key);
    int (*key_compare)(const void *a, const void *b);
    void (*key_destroy)(void *key);

    void *(*value_copy)(const void *key);
    int (*value_compare)(const void *a, const void *b);
    void (*value_destroy)(void *key);

    struct _RBNode *root;
    struct _RBNode *nil;
    unsigned int size;
};

struct _RBTreeIterator
{
    const RBTree *tree;
    RBNode *curr;
};

static int pointer_compare(const void *a, const void *b)
{
    return ((const char *)a) - ((const char *)b);
}

static void noop_destroy(void *a)
{
    return;
}

static void *noop_copy(const void *a)
{
    return (void *)a;
}

static RBNode *node_new(RBTree *tree, RBNode *parent, bool red, const void *key, const void *value)
{
    RBNode *node = xmalloc(sizeof(RBNode));

    node->parent = parent;
    node->red = red;
    node->key = tree->key_copy(key);
    node->value = tree->value_copy(value);
    node->left = tree->nil;
    node->right = tree->nil;

    return node;
}

static void node_destroy(RBTree *tree, RBNode *node)
{
    if (node)
    {
        tree->key_destroy(node->key);
        tree->value_destroy(node->value);
        free(node);
    }
}


RBTree *rbtree_new(void *(*key_copy)(const void *key),
                  int (*key_compare)(const void *a, const void *b),
                  void (*key_destroy)(void *key),
                  void *(*value_copy)(const void *value),
                  int (*value_compare)(const void *a, const void *b),
                  void (*value_destroy)(void *value))
{
    assert(!(key_copy && key_destroy) || (key_copy && key_destroy));
    assert(!(value_copy && value_destroy) || (value_copy && value_destroy));

    RBTree *t = xmalloc(sizeof(RBTree));

    t->key_copy = key_copy ? key_copy : noop_copy;
    t->key_compare = key_compare ? key_compare : pointer_compare;
    t->key_destroy = key_destroy ? key_destroy : noop_destroy;

    t->value_copy = value_copy ? value_copy : noop_copy;
    t->value_compare = value_compare ? value_compare : pointer_compare;
    t->value_destroy = value_destroy ? value_destroy : noop_destroy;

    t->nil = xcalloc(1, sizeof(RBNode));
    t->nil->key = t->nil->value = NULL;
    t->nil->red = false;
    t->nil->parent = t->nil->left = t->nil->right = t->nil;

    t->root = xcalloc(1, sizeof(RBNode));
    t->root->key = t->root->value = NULL;
    t->root->red = false;
    t->root->parent = t->root->left = t->root->right = t->nil;

    t->size = 0;

    return t;
}

static void tree_destroy(RBTree *tree, RBNode *x)
{
    if (x != tree->nil)
    {
        tree_destroy(tree, x->left);
        tree_destroy(tree, x->right);
        node_destroy(tree, x);
    }
}

bool rbtree_equal(const void *_a, const void *_b)
{
    const RBTree *a = _a, *b = _b;

    if (a == b)
    {
        return true;
    }
    if (a == NULL || b == NULL)
    {
        return false;
    }
    if (a->key_compare != b->key_compare || a->value_compare != b->value_compare)
    {
        return false;
    }
    if (rbtree_size(a) != rbtree_size(b))
    {
        return false;
    }

    RBTreeIterator *it_a = rbtree_iterator_new(a);
    RBTreeIterator *it_b = rbtree_iterator_new(b);

    void *a_key, *a_val, *b_key, *b_val;
    while (rbtree_iterator_next(it_a, &a_key, &a_val)
           && rbtree_iterator_next(it_b, &b_key, &b_val))
    {
        if (a->key_compare(a_key, b_key) != 0
            || b->value_compare(a_val, b_val))
        {
            rbtree_iterator_destroy(it_a);
            rbtree_iterator_destroy(it_b);
            return false;
        }
    }

    rbtree_iterator_destroy(it_a);
    rbtree_iterator_destroy(it_b);
    return true;

}

void rbtree_destroy(void *rb_tree)
{
    RBTree *tree = rb_tree;
    if (tree)
    {
        tree_destroy(tree, tree->root->left);
        free(tree->root);
        free(tree->nil);
        free(tree);
    }
}

static void rotate_left(RBTree *tree, RBNode *x)
{
    assert(!tree->nil->red);

    RBNode *y = x->right;
    x->right = y->left;

    if (y->left != tree->nil)
    {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x == x->parent->left)
    {
        x->parent->left = y;
    }
    else
    {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;

    assert(!tree->nil->red);
}

static void rotate_right(RBTree *tree, RBNode *y)
{
    assert(!tree->nil->red);

    RBNode *x = y->left;
    y->left = x->right;

    if (x->right != tree->nil)
    {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;

    assert(!tree->nil->red);
}

typedef struct
{
    bool replaced;
    RBNode *node;
} InsertResult;

static void put_fix(RBTree *tree, RBNode *z)
{
    while (z->parent->red)
    {
        if (z->parent == z->parent->parent->left)
        {
            RBNode *y = z->parent->parent->right;
            if (y->red)
            {
                // case 1
                z->parent->red = false;
                y->red = false;
                z->parent->parent->red = true;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->right)
                {
                    // case 2
                    z = z->parent;
                    rotate_left(tree, z);
                }

                // case 3
                z->parent->red = false;
                z->parent->parent->red = true;
                rotate_right(tree, z->parent->parent);
            }
        }
        else
        {
            RBNode *y = z->parent->parent->left;
            if (y->red)
            {
                // case 1
                z->parent->red = false;
                y->red = false;
                z->parent->parent->red = true;
                z = z->parent->parent;
            }
            else
            {
                if (z == z->parent->left)
                {
                    // case 2
                    z = z->parent;
                    rotate_right(tree, z);
                }

                // case 3
                z->parent->red = false;
                z->parent->parent->red = true;
                rotate_left(tree, z->parent->parent);
            }
        }
    }

    tree->root->left->red = false;

    assert(!tree->nil->red);
    assert(!tree->root->red);
}


bool rbtree_put(RBTree *tree, const void *key, const void *value)
{
    RBNode *y = tree->root;
    RBNode *x = tree->root->left;

    while (x != tree->nil)
    {
        y = x;
        int cmp = tree->key_compare(key, x->key);
        if (cmp == 0)
        {
            tree->value_destroy(x->value);
            x->value = tree->value_copy(value);
            return true;
        }
        x = (cmp < 0) ? x->left : x->right;
    }

    RBNode *z = node_new(tree, y, true, key, value);

    if (y == tree->root || tree->key_compare(z->key, y->key) < 0)
    {
        y->left = z;
    }
    else
    {
        y->right = z;
    }

    put_fix(tree, z);
    tree->size++;

    return false;
}

static RBNode *node_next(const RBTree *tree, const RBNode *node)
{
    if (node->right != tree->nil)
    {
        RBNode *curr;
        for (curr = node->right; curr->left != tree->nil; curr = curr->left);
        return curr;
    }
    else
    {
        RBNode *curr;
        for (curr = node->parent; node == curr->right; node = curr, curr = curr->parent);
        return (curr != tree->root) ? curr : tree->nil;
    }
}

static RBNode *node_get(const RBTree *tree, const void *key)
{
    assert(!tree->nil->red);
    RBNode *curr = tree->root->left;

    while (curr != tree->nil)
    {
        int cmp = tree->key_compare(key, curr->key);
        if (cmp == 0)
        {
            return curr;
        }
        else if (cmp < 0)
        {
            curr = curr->left;
        }
        else
        {
            curr = curr->right;
        }
    }

    assert(!tree->nil->red);
    return curr;
}

void *rbtree_get(const RBTree *tree, const void *key)
{
    RBNode *node = node_get(tree, key);
    return node != tree->nil ? node->value : NULL;
}


void remove_fix(RBTree *tree, RBNode *x)
{
    assert(!tree->nil->red);

    RBNode *root = tree->root->left;
    RBNode *w;

    while (x != root && !x->red)
    {
        if (x == x->parent->left)
        {
            w = x->parent->right;
            if (w->red)
            {
                w->red = false;
                x->parent->red = true;
                rotate_left(tree, x->parent);
                w = x->parent->right;
            }

            if (!w->left->red && !w->right->red)
            {
                w->red = true;
                x = x->parent;
            }
            else
            {
                if (!w->right->red)
                {
                    w->left->red = false;
                    w->red = true;
                    rotate_right(tree, w);
                    w = x->parent->right;
                }
                w->red = x->parent->red;
                x->parent->red = false;
                w->right->red = false;
                rotate_left(tree, x->parent);
                x = root;
            }
        }
        else
        {
            w = x->parent->left;
            if (w->red)
            {
                w->red = false;
                x->parent->red = true;
                rotate_right(tree, x->parent);
                w = x->parent->left;
            }

            if (!w->left->red && !w->right->red)
            {
                w->red = true;
                x = x->parent;
            }
            else
            {
                if (!w->left->red)
                {
                    w->right->red = false;
                    w->red = true;
                    rotate_left(tree, w);
                    w = x->parent->left;
                }

                w->red = x->parent->red;
                x->parent->red = false;
                w->left->red = false;
                rotate_right(tree, x->parent);
                x = root;
            }
        }
    }

    x->red = false;
    assert(!tree->nil->red);
}

bool rbtree_remove(RBTree *tree, const void *key)
{
    assert(!tree->nil->red);

    RBNode *z = node_get(tree, key);
    if (z == tree->nil)
    {
        return false;
    }

    RBNode *y = ((z->left == tree->nil) || (z->right == tree->nil)) ? z : node_next(tree, z);
    RBNode *x = (y->left == tree->nil) ? y->right : y->left;

    x->parent = y->parent;
    if (tree->root == x->parent)
    {
        tree->root->left = x;
    }
    else
    {
        if (y == y->parent->left)
        {
            y->parent->left = x;
        }
        else
        {
            y->parent->right = x;
        }
    }

    if (z != y)
    {
        assert(y != tree->nil);
        assert(!tree->nil->red);

        if (!y->red)
        {
            remove_fix(tree, x);
        }

        y->left = z->left;
        y->right = z->right;
        y->parent = z->parent;
        y->red = z->red;
        z->left->parent = y;
        z->right->parent = y;

        if (z == z->parent->left)
        {
            z->parent->left = y;
        }
        else
        {
            z->parent->right = y;
        }
        node_destroy(tree, z);
    }
    else
    {
        if (!y->red)
        {
            remove_fix(tree, x);
        }
        node_destroy(tree, y);
    }

    assert(!tree->nil->red);

    tree->size--;
    return true;
}

void clear_recursive(RBTree *tree, RBNode *node)
{
    if (node == tree->nil)
    {
        return;
    }

    clear_recursive(tree, node->left);
    clear_recursive(tree, node->right);

    node_destroy(tree, node);
}

void rbtree_clear(RBTree *tree)
{
    assert(tree);

    clear_recursive(tree, tree->root);
}

unsigned int rbtree_size(const RBTree *tree)
{
    return tree->size;
}

RBTreeIterator *rbtree_iterator_new(const RBTree *tree)
{
    RBTreeIterator *iter = malloc(sizeof(RBTreeIterator));

    iter->tree = tree;
    for (iter->curr = iter->tree->root; iter->curr->left != tree->nil; iter->curr = iter->curr->left);

    return iter;
}

bool iterator_peek(RBTreeIterator *iter, void **key, void **value)
{
    if (iter->curr == iter->tree->nil)
    {
        return false;
    }

    if (key)
    {
        *key = iter->curr->key;
    }

    if (value)
    {
        *value = iter->curr->value;
    }

    return true;
}

bool rbtree_iterator_next(RBTreeIterator *iter, void **key, void **value)
{
    if (iterator_peek(iter, key, value))
    {
        iter->curr = node_next(iter->tree, iter->curr);
        return true;
    }
    else
    {
        return false;
    }
}

void rbtree_iterator_destroy(void *_rb_iter)
{
    free(_rb_iter);
}
