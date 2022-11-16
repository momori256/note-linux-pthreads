#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_cond_t cond;
pthread_mutex_t mutex;

#define n (5)
int stop = 0;
int cnt = 0;
int scores[n] = {0};

int pthread_mutex_unlock_ex(pthread_mutex_t* mutex) {
  return pthread_mutex_unlock(mutex);
}

void* f(void* arg) {
  const int id = (int)arg;
  while (!stop) {
    printf("wait %d\n", id);
    if (pthread_cond_wait(&cond, &mutex)) {
      exit(1);
    }

    if (cnt < 0) {
      pthread_mutex_unlock_ex(&mutex);
      break;
    }

    if (cnt > 0) {
      printf("got %d\n", id);
      ++scores[id];
      --cnt;
      pthread_mutex_unlock_ex(&mutex);
      sleep(1);
    } else {
      printf("miss %d\n", id);
      pthread_mutex_unlock_ex(&mutex);
      sleep(2);
    }
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

  pthread_t pids[n];
  for (int i = 0; i < n; ++i) {
    if (pthread_create(&pids[i], NULL, f, (void*)i)) {
      exit(1);
    }
  }

  while (1) {
    scanf("%d", &cnt);
    if (cnt < 0) {
      stop = 1;
      break;
    }
    if (pthread_cond_broadcast(&cond)) {
      exit(1);
    }
  }

  if (pthread_cond_broadcast(&cond)) {
    exit(1);
  }

  for (int i = 0; i < n; ++i) {
    if (pthread_join(pids[i], NULL)) {
      exit(1);
    }
    printf("scores[%d] = %d\n", i, scores[i]);
  }
  return 0;
}
