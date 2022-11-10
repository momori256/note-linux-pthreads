#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "fly.h"

#define rep(i, n) for (int i = 0; i < (n); ++i)

#define W (32)
#define H (16)

void cur_move(int x, int y) { printf("\033[%d;%dH", x, y); }
void cur_clear() { printf("\033[2J"); }

static int s_stop = 0;

void* draw_field(void* arg) {
  (void)arg;

  while (!s_stop) {
    cur_move(0, 0);

    rep(y, H) {
      rep(x, W) {
        if (flies_draw(x, y)) {
          continue;
        }

        if ((x == 0 || x == W - 1) && (y == 0 || y == H - 1)) {
          putc('+', stdout);
        } else if (y == 0 || y == H - 1) {
          putc('-', stdout);
        } else if (x == 0 || x == W - 1) {
          putc('|', stdout);
        } else {
          putc(' ', stdout);
        }
      }
      putc('\n', stdout);
    }

    usleep(1 * 1000 * 1000 / 5);
  }

  return NULL;
}

int main() {
  flies_init(W, H);
  flies_start();

  cur_clear();
  pthread_t pid;
  if (pthread_create(&pid, NULL, draw_field, NULL)) {
    exit(1);
  }

  (void)fgetc(stdin);
  flies_destroy();
  s_stop = 1;

  return 0;
}
