#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"

#define rep(i, n) for (int i = 0; i < (n); ++i)
#define N (1000)
#define CNT (100)

static Queue *q;
static int stop = 0;
static int popped = 0;

void *enqueue(void *arg) {
  rep(i, CNT) {
    const int x = rand();
    while (1) {
      if (queue_wait_push(q, 100)) {
        continue;
      }
      if (!queue_push(q, x, x)) {
        break;
      }
    }
  }
  return NULL;
}

void *dequeue(void *arg) {
  Item item;
  while (1) {
    if (queue_wait_pop(q, 100)) {
      if (stop) {
        break;
      }
      continue;
    }
    if (queue_pop(q, &item)) {
      fprintf(stderr, "failed to pop\n");
    }
    ++popped;
    if (item.x != item.y) {
      fprintf(stderr, "mismatch %d, %d\n", (int)item.x, (int)item.y);
    }
  }
  return NULL;
}

/**
 * @brief 1000 threads push 10 items respectively.
 */
int main() {
  q = queue_init(N * CNT - 1000);
  pthread_t pids[N];
  rep(i, N) {
    if (pthread_create(&pids[i], NULL, enqueue, NULL)) {
      exit(1);
    }
  }

  pthread_t pid;
  if (pthread_create(&pid, NULL, dequeue, NULL)) {
    exit(1);
  }

  rep(i, N) {
    if (pthread_join(pids[i], NULL)) {
      exit(1);
    }
  }
  stop = 1;

  if (pthread_join(pid, NULL)) {
    exit(1);
  }

  printf("remaining: actual[%d], expected[%d]\n", queue_count(q), 0);
  printf("popped: actual[%d], expected[%d]\n", popped, N * CNT);
  queue_finalize(q);
  return 0;
}
