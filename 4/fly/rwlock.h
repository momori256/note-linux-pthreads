#ifndef RWLOCK_H
#define RWLOCK_H

#include <pthread.h>

typedef struct {
  int rd_cnt;
  pthread_mutex_t rd;
  pthread_mutex_t wr;
} RwLock;

void rwlock_init(RwLock* rw);

void rwlock_rd_lock(RwLock* rw);

void rwlock_rd_unlock(RwLock* rw);

void rwlock_wr_lock(RwLock* rw);

void rwlock_wr_unlock(RwLock* rw);

#endif
