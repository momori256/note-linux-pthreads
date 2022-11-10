#include "rwlock.h"

#include <pthread.h>
#include <stdlib.h>

static int die() {
  exit(1);
  return 1;
}

void rwlock_init(RwLock* rw) {
  rw->rd_cnt = 0;
  (void)(!pthread_mutex_init(&rw->rd, NULL) || die());
  (void)(!pthread_mutex_init(&rw->wr, NULL) || die());
}

void rwlock_rd_lock(RwLock* rw) {
  (void)(!pthread_mutex_lock(&rw->rd) || die());
  if (rw->rd_cnt == 0) {
    (void)(!pthread_mutex_lock(&rw->wr) || die());
  }
  ++rw->rd_cnt;
  (void)(!pthread_mutex_unlock(&rw->rd) || die());
}

void rwlock_rd_unlock(RwLock* rw) {
  (void)(!pthread_mutex_lock(&rw->rd) || die());
  --rw->rd_cnt;
  if (rw->rd_cnt == 0) {
    (void)(!pthread_mutex_unlock(&rw->wr) || die());
  }
  (void)(!pthread_mutex_unlock(&rw->rd) || die());
}

void rwlock_wr_lock(RwLock* rw) {
  while (1) {
    (void)(!pthread_mutex_lock(&rw->rd) || die());
    if (rw->rd_cnt == 0) {
      break;
    }
    (void)(!pthread_mutex_unlock(&rw->rd) || die());
  }
  (void)(!pthread_mutex_lock(&rw->wr) || die());
}

void rwlock_wr_unlock(RwLock* rw) {
  (void)(!pthread_mutex_unlock(&rw->rd) || die());
  (void)(!pthread_mutex_unlock(&rw->wr) || die());
}
