#include "fly.h"

#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "rwlock.h"

// private decl.

static int rand_between(int l, int h);
static double drand_between(double l, double h);
static double pi();

static void fly_init(Fly* fly, int w, int h);
static int fly_draw(Fly* const fly, int x, int y);
static void* fly_move(void* arg);

#define rep(i, n) for (int i = 0; i < (n); ++i)
#define NFLY (16)

static Fly flies[NFLY];
static pthread_t pids[NFLY];
static int s_w;
static int s_h;
static int s_stop = 0;

// public impl.

void flies_init(int w, int h) {
  s_w = w;
  s_h = h;
  srand(time(NULL));
  rep(i, NFLY) { fly_init(&flies[i], w, h); }
}

void flies_start() {
  rep(i, NFLY) {
    if (pthread_create(&pids[i], NULL, fly_move, (void*)&flies[i])) {
      exit(1);
    }
  }
}

void flies_destroy() {
  s_stop = 1;
  rep(i, NFLY) {
    if (pthread_join(pids[i], NULL)) {
      exit(1);
    }
  }
}

int flies_draw(int x, int y) {
  rep(i, NFLY) {
    Fly* const fly = &flies[i];
    if (fly_draw(fly, x, y)) {
      return 1;
    }
  }
  return 0;
}

// private impl.

static int fly_draw(Fly* const fly, int x, int y) {
  rwlock_rd_lock(&fly->rwlock);
  const int draw = (int)fly->x == x && (int)fly->y == y;
  rwlock_rd_unlock(&fly->rwlock);

  if (draw) {
    putc(fly->c, stdout);
  }
  return draw;
}

static void* fly_move(void* arg) {
  Fly* fly = (Fly*)arg;
  while (!s_stop) {
    rwlock_wr_lock(&fly->rwlock);

    fly->x += cos(fly->v);
    fly->y += sin(fly->v);

    if (fly->x < 1) {
      fly->x = 1;
      fly->v = atan2(sin(fly->v), -cos(fly->v));
    } else if (fly->x > s_w - 2) {
      fly->x = s_w - 2;
      fly->v = atan2(sin(fly->v), -cos(fly->v));
    }

    if (fly->y < 1) {
      fly->y = 1;
      fly->v = atan2(-sin(fly->v), cos(fly->v));
    } else if (fly->y > s_h - 2) {
      fly->y = s_h - 2;
      fly->v = atan2(-sin(fly->v), cos(fly->v));
    }

    rwlock_wr_unlock(&fly->rwlock);
    usleep(1 * 1000 * 1000 / 5);
  }
  return NULL;
}

static void fly_init(Fly* fly, int w, int h) {
  const char* const s = "@*.ox";
  fly->c = s[rand_between(0, strlen(s) - 1)];
  fly->x = drand_between(1, w - 2);
  fly->y = drand_between(1, h - 2);
  fly->v = drand_between(0, 2 * pi());
  rwlock_init(&fly->rwlock);
}

static int rand_between(int l, int h) {
  const int r = rand();
  return l + (double)r / RAND_MAX * (h - l) + 0.5;
}

static double drand_between(double l, double h) {
  const int r = rand();
  return l + (double)r / RAND_MAX * (h - 1);
}

static double pi() { return atan(1) * 4; }
