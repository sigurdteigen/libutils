#include "rb-tree.h"

#include "alloc.h"
#include "seq.h"

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmockery.h>
#include <stdlib.h>

static void *int_copy(const void *_a)
{
    return xmemdup(_a, sizeof(int));
}

static int int_compare(const void *_a, const void *_b)
{
    const int *a = _a, *b = _b;
    return *a - *b;
}

static RBTree *int_tree_new(void)
{
    return rbtree_new(int_copy, int_compare, free, int_copy, int_compare, free);
}

static void test_new_destroy(void **state)
{
    RBTree *t = int_tree_new();
    rbtree_destroy(t);
}

static void test_put_overwrite(void **state)
{
    RBTree *t = int_tree_new();

    int a = 42;
    assert_false(rbtree_put(t, &a, &a));
    int *r = rbtree_get(t, &a);
    assert_int_equal(a, *r);

    assert_true(rbtree_put(t, &a, &a));
    r = rbtree_get(t, &a);
    assert_int_equal(a, *r);

    rbtree_destroy(t);
}

static void test_put_remove(void **state)
{
    RBTree *t = int_tree_new();

    int a = 42;
    assert_false(rbtree_put(t, &a, &a));
    int *r = rbtree_get(t, &a);
    assert_int_equal(a, *r);

    assert_true(rbtree_remove(t, &a));
    r = rbtree_get(t, &a);
    assert_true(r == NULL);

    assert_false(rbtree_remove(t, &a));
    r = rbtree_get(t, &a);
    assert_true(r == NULL);

    rbtree_destroy(t);
}

static void test_put_remove_inorder(void **state)
{
    RBTree *t = int_tree_new();
    for (int i = 0; i < 20000; i++)
    {
        rbtree_put(t, &i, &i);
    }

    for (int i = 0; i < 20000; i++)
    {
        int *r = rbtree_get(t, &i);
        assert_int_equal(i, *r);
    }

    for (int i = 0; i < 20000; i++)
    {
        rbtree_remove(t, &i);
    }

    rbtree_destroy(t);
}

static void test_iterate(void **state)
{
    Seq *nums = seq_new(20, free);
    srand(0);
    for (int i = 0; i < 20; i++)
    {
        int k = rand() % 1000;
        seq_append(nums, xmemdup(&k, sizeof(int)));
    }

    RBTree *t = int_tree_new();
    for (unsigned int i = 0; i < seq_length(nums); i++)
    {
        rbtree_put(t, seq_at(nums, i), seq_at(nums, i));
    }

    RBTreeIterator *it = rbtree_iterator_new(t);
    int last = -1;
    void *_r = NULL;
    int i = 0;
    while (rbtree_iterator_next(it, &_r, NULL))
    {
        int *r = _r;
        assert_true(*r >= last);
        last = *r;
        i++;
    }
    rbtree_iterator_destroy(it);

    assert_int_equal(i, 20);

    seq_destroy(nums);
    rbtree_destroy(t);
}

static void test_put_remove_random(void **state)
{
    Seq *nums = seq_new(20, free);
    srand(0);
    for (int i = 0; i < 20; i++)
    {
        int k = rand() % 1000;
        seq_append(nums, xmemdup(&k, sizeof(int)));
    }

    RBTree *t = int_tree_new();
    for (unsigned int i = 0; i < seq_length(nums); i++)
    {
        rbtree_put(t, seq_at(nums, i), seq_at(nums, i));
    }

    for (unsigned int i = 0; i < seq_length(nums); i++)
    {
        int *k = seq_at(nums, i);
        int *r = rbtree_get(t, k);
        assert_int_equal(*k, *r);
    }

    for (unsigned int i = 0; i < seq_length(nums); i++)
    {
        rbtree_remove(t, seq_at(nums, i));
    }

    seq_destroy(nums);
    rbtree_destroy(t);
}


int main()
{
    const UnitTest tests[] =
    {
        unit_test(test_new_destroy),
        unit_test(test_put_overwrite),
        unit_test(test_put_remove),
        unit_test(test_put_remove_inorder),
        unit_test(test_iterate),
        unit_test(test_put_remove_random)
    };

    return run_tests(tests);
}

