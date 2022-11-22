#include <pthread.h>

typedef struct {
  int fd;
} Item;

typedef struct {
  int n;
  Item* items;
  int head;
  int tail;
  pthread_mutex_t lock;
  pthread_cond_t can_push;
  pthread_cond_t can_pop;
} Queue;

Queue* queue_init(int n);

void queue_finalize(Queue* q);

int queue_push(Queue* q, int fd);

int queue_pop(Queue* q, Item* out);

int queue_wait_push(Queue* q, int msec);

int queue_wait_pop(Queue* q, int msec);

int queue_count(Queue* q);
