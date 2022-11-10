CFLAGS := -g -Wall -std=c99

a.out: main.o fly.o rwlock.o
	gcc $(CFLAGS) -lpthread -lm main.o fly.o rwlock.o

main.o: main.c
	gcc $(CFLAGS) -c -D_DEFAULT_SOURCE main.c

fly.o: fly.c fly.h rwlock.o
	gcc $(CFLAGS) -c -D_DEFAULT_SOURCE fly.c

rwlock.o: rwlock.c rwlock.h
	gcc $(CFLAGS) -c -D_DEFAULT_SOURCE rwlock.c

clean:
	rm *.o *.out
