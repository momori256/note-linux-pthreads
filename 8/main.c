#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE (200112)
#endif

#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "queue.h"

#define BACKLOG (10)
#define PORT "54321"
#define BUF_SIZE (100)

Queue *q = NULL;
int stop = 0;

void *worker(void *arg);

int create_bind_fd();
int create_server_fd();

/**
 * Accept connections and give them to workers.
 */
int main() {
  const int sfd = create_server_fd();

  q = queue_init(BACKLOG);
  if (!q) {
    fprintf(stderr, "queue init\n");
    exit(1);
  }

  pthread_t pids[BACKLOG];
  for (int i = 0; i < BACKLOG; ++i) {
    if (pthread_create(&pids[i], NULL, worker, (void *)(intptr_t)i)) {
      perror("phread_create");
      exit(1);
    }
  }

  while (!stop) {
    const int afd = accept(sfd, NULL, NULL);
    if (afd == -1) {
      perror("accept");
      exit(1);
    }
    if (queue_push(q, afd)) {
      fprintf(stderr, "can not push\n");
    }
  }

  for (int i = 0; i < BACKLOG; ++i) {
    if (pthread_join(pids[i], NULL)) {
      perror("phread_join");
      exit(1);
    }
  }
  return 0;
}

void *worker(void *arg) {
  intptr_t id = (intptr_t)arg;
  while (!stop) {
    if (queue_wait_pop(q, 100)) {
      continue;
    }
    Item item;
    if (queue_pop(q, &item)) {
      continue;
    }

    const int sfd = item.fd;
    char buf[BUF_SIZE];
    const ssize_t sz = read(sfd, buf, sizeof(buf) / sizeof(buf[0]));
    if (sz == -1) {
      perror("read");
      exit(1);
    }
    sleep(3);  // for the sake of clarity.
    write(sfd, buf, sz);
    close(sfd);
    printf("[%ld] received: %s", id, buf);
    if (strcmp(buf, "stop\r\n") == 0) {
      stop = 1;
      break;
    }
  }
  return NULL;
}

int create_bind_fd() {
  typedef struct addrinfo addrinfo;

  addrinfo hints = {
      .ai_family = AF_INET,
      .ai_socktype = SOCK_STREAM,
      .ai_flags = AI_PASSIVE,
      .ai_protocol = 0,
  };

  addrinfo *head;
  const int r = getaddrinfo(NULL, PORT, &hints, &head);
  if (r) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(r));
    exit(1);
  }

  int sfd = -1;
  for (addrinfo *p = head; p != NULL; p = p->ai_next) {
    sfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sfd == -1) {
      continue;
    }
    int val = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (bind(sfd, p->ai_addr, p->ai_addrlen) == 0) {
      break;
    }
    close(sfd);
  }
  freeaddrinfo(head);
  return sfd;
}

int create_server_fd() {
  const int sfd = create_bind_fd();
  if (listen(sfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  return sfd;
}
