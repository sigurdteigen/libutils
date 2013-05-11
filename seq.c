#include "seq.h"

#include "alloc.h"

#include <stdlib.h>
#include <assert.h>

struct Seq_
{
    void **data;
    unsigned int length;
    unsigned int capacity;
    void (*item_destroy)(void *);
};

static const unsigned int EXPAND_FACTOR = 2;

Seq *seq_new(unsigned int initial_capacity, void (*item_destroy)(void *))
{
    Seq *seq = xmalloc(sizeof(Seq));

    if (initial_capacity == 0)
    {
        initial_capacity = 1;
    }

    seq->capacity = initial_capacity;
    seq->length = 0;
    seq->data = xmalloc(sizeof(void *) * initial_capacity);
    seq->item_destroy = item_destroy;

    return seq;
}

static void destroy_range(Seq *seq, unsigned int start, unsigned int end)
{
    assert(start < seq->length);
    assert(end < seq->length);
    assert(start <= end);

    if (seq->item_destroy)
    {
        for (unsigned int i = start; i <= end; i++)
        {
            seq->item_destroy(seq->data[i]);
        }
    }
}

void seq_destroy(Seq *seq)
{
    if (seq)
    {
        if (seq->length > 0)
        {
            destroy_range(seq, 0, seq->length - 1);
        }

        free(seq->data);
        free(seq);
    }
}

static void maybe_expand(Seq *seq)
{
    assert(seq->length <= seq->capacity);

    if (seq->length == seq->capacity)
    {
        seq->capacity *= EXPAND_FACTOR;
        seq->data = xrealloc(seq->data, sizeof(void *) * seq->capacity);
    }
}

inline void *seq_at(const Seq *seq, unsigned int index)
{
    return seq->data[index];
}

inline unsigned int seq_length(const Seq *seq)
{
    return seq->length;
}

void seq_append(Seq *seq, void *item)
{
    maybe_expand(seq);

    seq->data[seq->length] = item;
    (seq->length)++;
}
