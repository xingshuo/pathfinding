#include "heapq.h"
#include "pathfind.h"

/********referred to the source code of libev/ev.c**********/
typedef Node* T;

#define T_value(node)       (node)->f
#define T_pos(node)    (node)->pos

#if USE_4HEAP

#define DHEAP 4
#define HEAP0 (DHEAP - 1) /* index of first element in heap */
#define HPARENT(k) ((((k) - HEAP0 - 1) / DHEAP) + HEAP0)
#define UPHEAP_DONE(p,k) ((p) == (k))

void
downheap (T *heap, int N, int k)
{
  T he = heap [k];
  T *E = heap + N + HEAP0;

  for (;;)
    {
      float minat;
      T *minpos;
      T *pos = heap + DHEAP * (k - HEAP0) + HEAP0 + 1;

      /* find minimum child */
      if (pos + DHEAP - 1 < E)
        {
          /* fast path */                               (minpos = pos + 0), (minat = T_value (*minpos));
          if (               T_value (pos [1]) < minat) (minpos = pos + 1), (minat = T_value (*minpos));
          if (               T_value (pos [2]) < minat) (minpos = pos + 2), (minat = T_value (*minpos));
          if (               T_value (pos [3]) < minat) (minpos = pos + 3), (minat = T_value (*minpos));
        }
      else if (pos < E)
        {
          /* slow path */                               (minpos = pos + 0), (minat = T_value (*minpos));
          if (pos + 1 < E && T_value (pos [1]) < minat) (minpos = pos + 1), (minat = T_value (*minpos));
          if (pos + 2 < E && T_value (pos [2]) < minat) (minpos = pos + 2), (minat = T_value (*minpos));
          if (pos + 3 < E && T_value (pos [3]) < minat) (minpos = pos + 3), (minat = T_value (*minpos));
        }
      else
        break;

      if (T_value (he) <= minat)
        break;

      heap [k] = *minpos;
      T_pos (*minpos) = k;

      k = minpos - heap;
    }

  heap [k] = he;
  T_pos (he) = k;
}

#else /* 4HEAP */

#define HEAP0 1
#define HPARENT(k) ((k) >> 1)
#define UPHEAP_DONE(p,k) (!(p))

void
downheap (T *heap, int N, int k)
{
  T he = heap [k];

  for (;;)
    {
      int c = k << 1;

      if (c >= N + HEAP0)
        break;

      c += c + 1 < N + HEAP0 && T_value (heap [c]) > T_value (heap [c + 1])
           ? 1 : 0;

      if (T_value (he) <= T_value (heap [c]))
        break;

      heap [k] = heap [c];
      T_pos (heap [k]) = k;
      
      k = c;
    }

  heap [k] = he;
  T_pos (he) = k;
}

#endif

void
upheap (T *heap, int k)
{
  T he = heap [k];

  for (;;)
    {
      int p = HPARENT (k);

      if (UPHEAP_DONE (p, k) || T_value (heap [p]) <= T_value (he))
        break;

      heap [k] = heap [p];
      T_pos (heap [k]) = k;
      k = p;
    }

  heap [k] = he;
  T_pos (he) = k;
}

void
adjustheap (T *heap, int N, int k)
{
  if (k > HEAP0 && T_value (heap [k]) <= T_value (heap [HPARENT (k)]))
    upheap (heap, k);
  else
    downheap (heap, N, k);
}

void
reheap (T *heap, int N)
{
  int i;
  for (i = 0; i < N; ++i)
    upheap (heap, i + HEAP0);
}
/***********************************************************/


HeapQueue*
create_queue(size_t capacity) {
  HeapQueue *q = malloc(sizeof(*q));
  if (!q)
    return NULL;
  q->data = malloc( (capacity+HEAP0) * sizeof(*(q->data)) );
  if (!q->data)
    return NULL;
  q->capacity = capacity;
  q->size = 0;
  return q;
}

void
reset_queue(HeapQueue* q) {
  if (q)
    q->size = 0;
}

void
delete_queue(HeapQueue* q) {
  if (q) {
    free(q->data);
    free(q);
  }
}

size_t
queue_size(HeapQueue* q) {
  return q->size;
}

void
queue_push(HeapQueue* q, void* e) {
  if (!q)
    return;
  if (q->size >= q->capacity)
    return;
  int pos = HEAP0 + q->size;
  q->data[pos] = e;
  q->size++;
  upheap((T*)q->data, pos);
}

void*
queue_pop(HeapQueue* q) {
    if (!q)
      return NULL;
    if (q->size < 1)
      return NULL;
    void* e = q->data[HEAP0];
    q->size--;
    q->data[HEAP0] = q->data[HEAP0+q->size];
    downheap ((T*)q->data, q->size, HEAP0);
    return e;
}

void
queue_adjust(HeapQueue* q, void* e) {
  adjustheap ((T*)q->data, q->size, ((T)(e))->pos);
}