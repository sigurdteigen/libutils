#ifndef LIBUTILS_SEQ_H
#define LIBUTILS_SEQ_H

typedef struct Seq_ Seq;

Seq *seq_new(unsigned int initial_capacity, void (*item_destroy)(void*));
void seq_destroy(Seq *seq);

unsigned int seq_length(const Seq *seq);
void *seq_at(const Seq *seq, unsigned int index);

void seq_append(Seq *seq, void *item);

#endif
