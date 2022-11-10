#ifndef FLY_H
#define FLY_H

#include <pthread.h>

#include "rwlock.h"

typedef struct {
  char c;
  double x;
  double y;
  double v;
  // pthread_mutex_t mutex;
  RwLock rwlock;
} Fly;

void flies_init(int w, int h);
void flies_start();
void flies_destroy();
int flies_draw(int x, int y);
// void flies_move(int w, int h);

#endif
