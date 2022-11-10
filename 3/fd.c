#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define rep(i, n) for (int i = 0; i < (n); ++i)

void* f(void* p) {
  char buf[10] = {0};
  ssize_t r = read(STDIN_FILENO, buf, sizeof(buf) - 1);
  printf("%d: %s\n", (int)p, buf);
  return NULL;
}

int main() {
  const int n = 2;
  pthread_t pids[n];
  rep(i, n) {
    if (pthread_create(&pids[i], NULL, f, (void*)i)) {
      exit(1);
    }
  }

  rep(i, n) {
    if (pthread_join(pids[i], NULL)) {
      exit(1);
    }
  }
  return 0;
}
