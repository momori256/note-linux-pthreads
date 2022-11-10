#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_key_t key;

void f() {
  int n = (int)pthread_getspecific(key);
  printf("%d\n", n);
}

void *thread_entry(void *p) {
  pthread_setspecific(key, p);
  f();
  return NULL;
}

int main() {
  pthread_key_create(&key, NULL);
  pthread_t pids[3];

  for (int i = 0; i < 3; ++i) {
    if (pthread_create(&pids[i], NULL, thread_entry, (void *)i)) {
      exit(1);
    }
  }

  for (int i = 0; i < 3; ++i) {
    if (pthread_join(pids[i], NULL)) {
      exit(1);
    }
  }
  return 0;
}
