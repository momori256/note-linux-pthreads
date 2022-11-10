#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

void* thread_entry(void* arg) {
  for (int i = 0; i < 3; ++i) {
    printf("thread: %ld\n", (long)pthread_self());
    sleep(1);
  }
  return NULL;
}

int main() {
  pthread_t pids[3];
  for (int i = 0; i < 3; ++i) {
    if (pthread_create(&pids[i], NULL, thread_entry, NULL)) {
      perror("pthread_create");
    }
  }

  for (int i = 0; i < 3; ++i) {
    if (pthread_join(pids[i], NULL)) {
      perror("pthead_join");
    }
  }
  return 0;
}
