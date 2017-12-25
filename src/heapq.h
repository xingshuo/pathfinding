/**
 * author: xingshuo
 * date: 2017-12-25
 */
#ifndef _HEAPQ_H
#define _HEAPQ_H

#include "config.h"

typedef struct {
    size_t size;
    size_t capacity;
    void** data;
} HeapQueue;

HeapQueue* create_queue(size_t capacity);
void delete_queue(HeapQueue* q);
void reset_queue(HeapQueue* q);

size_t queue_size(HeapQueue* q);
void queue_push(HeapQueue* q, void* data);
void* queue_pop(HeapQueue* q);
void queue_adjust(HeapQueue* q, void* data);

#endif