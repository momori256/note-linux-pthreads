#include "queue.h"

#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct timespec timespec;

void get_timespec_after(timespec* t, int msec);
void printe(const char* const file, int line, const char* const func,
            const char* const fmt, ...);

#define PRINTE(fmt, ...) \
  printe(__FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__)

/**
 * @brief Create and initialize a new queue.
 * @return New queue if succeeded, NULL
 * otherwise.
 */
Queue* queue_init(int n) {
  Queue* q = (Queue*)malloc(sizeof(Queue));
  if (!q) {
    return NULL;
  }
  q->head = 0;
  q->tail = 0;
  q->n = n + 1;
  q->items = (Item*)malloc(sizeof(Item) * q->n);
  if (q->items == NULL) {
    return NULL;
  }
  if (pthread_mutex_init(&q->lock, NULL)) {
    return NULL;
  }
  if (pthread_cond_init(&q->can_push, NULL) ||
      pthread_cond_init(&q->can_pop, NULL)) {
    return NULL;
  }
  return q;
}

/**
 * @brief Finalize the queue.
 */
void queue_finalize(Queue* q) {
  pthread_mutex_destroy(&q->lock);
  pthread_cond_destroy(&q->can_push);
  pthread_cond_destroy(&q->can_pop);
  free(q->items);
  free(q);
}

/**
 * @brief Push new item to the queue.
 * @return 0 is succeeded, 1 otherwise.
 */
int queue_push(Queue* q, int fd) {
  if (pthread_mutex_lock(&q->lock)) {
    PRINTE("lock");
    exit(1);
  }

  const int next_tail = (q->tail + 1) % q->n;
  if (next_tail == q->head) {
    if (pthread_mutex_unlock(&q->lock)) {
      PRINTE("unlock");
      exit(1);
    }
    return 1;
  }
  q->items[q->tail].fd = fd;
  q->tail = next_tail;

  if (pthread_mutex_unlock(&q->lock)) {
    PRINTE("unlock");
    exit(1);
  }
  if (pthread_cond_signal(&q->can_pop)) {
    PRINTE("signal");
    exit(1);
  }
  return 0;
}

/**
 * @brief Pop the head item of the queue.
 * @return 0 if succeeded, 1 otherwise.
 */
int queue_pop(Queue* q, Item* out) {
  if (pthread_mutex_lock(&q->lock)) {
    PRINTE("lock");
    exit(1);
  }

  if (q->head == q->tail) {
    if (pthread_mutex_unlock(&q->lock)) {
      PRINTE("unlock");
      exit(1);
    }
    return 1;
  }

  out->fd = q->items[q->head].fd;
  q->head = (q->head + 1) % q->n;

  if (pthread_mutex_unlock(&q->lock)) {
    PRINTE("unlock");
    exit(1);
  }
  if (pthread_cond_signal(&q->can_push)) {
    PRINTE("signal");
    exit(1);
  }
  return 0;
}

/**
 * @brief Wait up to msec for the queue to become pushable.
 * @return 0 if succeeded, 1 otherwise.
 */
int queue_wait_push(Queue* q, int msec) {
  if (pthread_mutex_lock(&q->lock)) {
    PRINTE("lock");
    exit(1);
  }
  const int next_tail = (q->tail + 1) % q->n;
  if (next_tail != q->head) {
    if (pthread_mutex_unlock(&q->lock)) {
      PRINTE("unlock");
      exit(1);
    }
    return 0;
  }

  timespec t;
  get_timespec_after(&t, msec);
  switch (pthread_cond_timedwait(&q->can_push, &q->lock, &t)) {
    case 0:
      if (pthread_mutex_unlock(&q->lock)) {
        PRINTE("unlock");
        exit(1);
      }
      return 0;
    case ETIMEDOUT:
      if (pthread_mutex_unlock(&q->lock)) {
        PRINTE("unlock");
        exit(1);
      }
      return 1;
    default:
      PRINTE("timedwait");
      exit(1);
  }
  return 0;
}

/**
 * @brief Wait up to msec for the queue to become popable.
 * @return 0 if succeeded, 1 otherwise.
 */
int queue_wait_pop(Queue* q, int msec) {
  if (pthread_mutex_lock(&q->lock)) {
    PRINTE("lock");
    exit(1);
  }
  if (q->head != q->tail) {
    if (pthread_mutex_unlock(&q->lock)) {
      PRINTE("unlock");
      exit(1);
    }
    return 0;
  }

  timespec t;
  get_timespec_after(&t, msec);
  switch (pthread_cond_timedwait(&q->can_pop, &q->lock, &t)) {
    case 0:
      if (pthread_mutex_unlock(&q->lock)) {
        PRINTE("unlock");
        exit(1);
      }
      return 0;
    case ETIMEDOUT:
      if (pthread_mutex_unlock(&q->lock)) {
        PRINTE("unlock");
        exit(1);
      }
      return 1;
    default:
      PRINTE("timedwait");
      exit(1);
  }
  return 0;
}

int queue_count(Queue* q) {
  if (q->head <= q->tail) {
    return q->tail - q->head;
  }
  return (q->tail + q->n) - q->head;
}

/**
 * @brief Get the timespec msec after from now.
 */
void get_timespec_after(timespec* t, int msec) {
  if (clock_gettime(CLOCK_REALTIME, t)) {
    PRINTE("clock_gettime");
    exit(1);
  }
  t->tv_sec += msec / 1000;
  t->tv_nsec += (msec % 1000) * 1000000;
  if (t->tv_nsec >= 1000000000) {
    ++t->tv_sec;
    t->tv_nsec -= 1000000000;
  }
}

void printe(const char* const file, int line, const char* const func,
            const char* const fmt, ...) {
  fprintf(stderr, "%s(%d): func[%s], err[%s], ", file, line, func,
          strerror(errno));

  va_list v;
  va_start(v, fmt);
  vfprintf(stderr, fmt, v);
  va_end(v);

  fprintf(stderr, "\n");
}
