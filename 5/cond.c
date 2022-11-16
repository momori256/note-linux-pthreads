#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_cond_t cond;
pthread_mutex_t mutex;

void* f(void* arg) {
  const int id = (int)arg;
  printf("wait %d\n", id);
  if (pthread_cond_wait(&cond, &mutex)) {
    exit(1);
  }
  printf("got %d\n", id);
  if (pthread_mutex_unlock(&mutex)) {
    exit(1);
  }
  return NULL;
}

int main() {
  if (pthread_mutex_init(&mutex, NULL)) {
    exit(1);
  }
  if (pthread_cond_init(&cond, NULL)) {
    exit(1);
  }

  const int n = 5;
  pthread_t pids[n];
  for (int i = 0; i < n; ++i) {
    if (pthread_create(&pids[i], NULL, f, (void*)i)) {
      exit(1);
    }
  }

  for (int i = 0; i < n; ++i) {
    sleep(1);
    printf("main\n");
    if (pthread_cond_signal(&cond)) {
      exit(1);
    }
  }

  for (int i = 0; i < n; ++i) {
    if (pthread_join(pids[i], NULL)) {
      exit(1);
    }
  }
  return 0;
}
