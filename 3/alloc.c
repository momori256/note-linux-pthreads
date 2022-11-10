#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *thread_entry(void *p) {
  char *s = (char *)p;
  s[0] = toupper(s[0]);
  return NULL;
}

int main() {
  pthread_t pid;

  char *s = (char *)malloc(sizeof(char) * 10);
  strcpy(s, "abcde");

  char t[] = "wxyz";

  if (pthread_create(&pid, NULL, thread_entry, (void *)s)) {
    exit(1);
  }
  if (pthread_join(pid, NULL)) {
    exit(1);
  }

  if (pthread_create(&pid, NULL, thread_entry, (void *)t)) {
    exit(1);
  }
  if (pthread_join(pid, NULL)) {
    exit(1);
  }

  printf("%s\n%s\n", s, t);
  return 0;
}
