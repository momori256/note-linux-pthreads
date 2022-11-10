#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

long long sum = 0;
pthread_mutex_t mutex;

typedef struct {
  int f, s;
} P;

void* f(void* p) {
  P* pair = (P*)p;
  for (int i = pair->f; i <= pair->s; ++i) {
    pthread_mutex_lock(&mutex);
    sum += i;
    pthread_mutex_unlock(&mutex);
  }
  free(p);
  return NULL;
}

int main() {
  pthread_mutex_init(&mutex, NULL);

  const int n = 1000;
  pthread_t pids[n];
  for (int i = 0; i < n; ++i) {
    P* p = (P*)malloc(sizeof(P));
    p->f = i * 100;
    p->s = p->f + 99;
    if (pthread_create(&pids[i], NULL, f, (void*)p)) {
      exit(1);
    }
  }

  for (int i = 0; i < n; ++i) {
    if (pthread_join(pids[i], NULL)) {
      exit(1);
    }
  }

  pthread_mutex_destroy(&mutex);

  printf("%lld, expected: %lld\n", sum,
         ((long long)(100 * n - 1) * (100 * n) / 2));
  return 0;
}
