/**
 * author: xingshuo
 * date: 2017-12-25
 */
#include "hash.h"

static inline slot *
mainposition(map *m , KEY_TYPE key){
    int hash = key & (m->size-1);
    return &m->slot_list[hash];
}

static void rehash(map *m);

static void
hash_insert(map* m, KEY_TYPE key, void* value){
    slot* s = mainposition(m, key);
    if (s->key == INVALID_KEY) {
        s->key = key;
        s->value = value;
        return;
    }
    slot* last = mainposition(m, s->key);
    if (last != s) { //s not in its hash pos, the head of hash link-list which s in is last
        while (last->next != s - m->slot_list) {
            assert(last->next >= 0);
            last = &m->slot_list[last->next];
        }
        KEY_TYPE tmp_key = s->key;
        void* tmp_val = s->value;
        last->next = s->next;
        s->key = key;
        s->value = value;
        s->next = -1;
        hash_insert(m, tmp_key, tmp_val);
        return;
    }
    while (m->lastfree >= 0) {
        slot * temp = &m->slot_list[m->lastfree--];
        if (temp->key == INVALID_KEY) {
            temp->key = key;
            temp->value = value;
            temp->next = s->next;
            s->next = (int)(temp - m->slot_list);
            return;
        }
    }
    rehash(m);
    hash_insert(m, key, value);
}

static void
rehash(map * m) {
    slot * old_slot = m->slot_list;
    int old_size = m->size;
    m->size = 2 * old_size;
    m->lastfree = m->size - 1;
    m->slot_list = malloc(m->size * sizeof(slot));
    int i;
    for (i=0;i<m->size;i++) {
        slot * s = &m->slot_list[i];
        s->key = INVALID_KEY;
        s->value = NULL;
        s->next = -1;
    }
    for (i=0;i<old_size;i++) {
        slot * s = &old_slot[i];
        if (s->key != INVALID_KEY) {
            hash_insert(m, s->key, s->value);
        }
    }
    free(old_slot);
}

void*
hash_add(map* m, KEY_TYPE key, void* value) {
    slot *s = mainposition(m, key);
    for (;;) {
        if (s->key == key) { //update
            if (value == s->value) {
                return NULL;
            }
            void* tmp = s->value;
            s->value = value;
            return tmp;
        }
        if (s->next < 0) {
            break;
        }
        s=&m->slot_list[s->next];
    }
    hash_insert(m, key, value);
    return NULL;
}

void*
hash_query(map* m, KEY_TYPE key) {
    slot *s = mainposition(m, key);
    for (;;) {
        if (s->key == key) {
            return s->value;
        }
        if (s->next < 0) {
            return NULL;
        }
        s=&m->slot_list[s->next];
    }
}

void*
hash_delete(map* m, KEY_TYPE key){
    slot* s = mainposition(m, key);
    slot* prev = NULL;
    for (;;) {
        if (s->key == key) {
            s->key = INVALID_KEY;
            void* tmp = s->value;
            s->value = NULL;
            if (prev) {
                if (prev->next > m->lastfree) {
                    m->lastfree = prev->next;
                }
                prev->next = s->next;
                s->next = -1;
            }else { //its head
                if (s->next >= 0) {
                    if (s->next > m->lastfree) {
                        m->lastfree = s->next;
                    }
                    slot* next = &m->slot_list[s->next];
                    *s = *next;
                    next->key = INVALID_KEY;
                    next->value = NULL;
                    next->next = -1;
                }
            }
            return tmp;
        }
        if (s->next < 0) {
            return NULL;
        }
        prev = s;
        s = &m->slot_list[s->next];
    }
}

static int
ceillog2(unsigned int x) { //fetch from lua source code
  static const unsigned char log_2[256] = {
    0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
    6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
    8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8
  };
  int l = 0;
  x--;
  while (x >= 256) { l += 8; x >>= 8; }
  return l + log_2[x];
}

map*
new_hashmap(int pre_size){
    assert(pre_size > 1);
    int lsize = ceillog2(pre_size);
    int size = 1<<lsize;
    map * m = malloc(sizeof(*m));
    m->size = size;
    m->lastfree = size - 1;
    m->slot_list = malloc(m->size * sizeof(slot));
    int i;
    for (i=0;i<m->size;i++) {
        slot * s = &m->slot_list[i];
        s->key = INVALID_KEY;
        s->next = -1;
        s->value = NULL;
    }
    return m;
}

void
delete_hashmap(map* m){
    free(m->slot_list);
    free(m);
}

void
trace_hashmap(map* m){
    printf("\033[0;34mtotal size:%d\033[0m\n", m->size);
    printf("\033[0;34mlastfree:%d\033[0m\n",m->lastfree);
    printf("\033[0;34mslot list:\033[0m\n");
    int i;
    for(i=0; i<m->size; i++) {
        if (m->slot_list[i].key == INVALID_KEY)
            printf("\033[0;34mpos:%d key:INVALID value:%p next:%d\033[0m\n", i, m->slot_list[i].value, m->slot_list[i].next);
        else
            printf("\033[0;34mpos:%d key:%lu value:%p next:%d\033[0m\n", i, m->slot_list[i].key, m->slot_list[i].value, m->slot_list[i].next);
    }
}