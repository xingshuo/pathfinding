/**
 * author: xingshuo
 * date: 2017-12-25
 */
#ifndef _HASH_H
#define _HASH_H
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define INVALID_KEY (~0)
#define KEY_TYPE uint64_t

typedef struct {
    KEY_TYPE key;
    void* value;
    int next;
} slot;

typedef struct {
    int size;
    int lastfree;
    slot* slot_list;
} map;

typedef map HASH_MAP;

map* new_hashmap(int);
void delete_hashmap(map*);
void trace_hashmap(map*);
void* hash_add(map*, KEY_TYPE, void*);
void* hash_delete(map*, KEY_TYPE);
void* hash_query(map*, KEY_TYPE);

#endif 