CFLAGS := -Wall -std=c99 -g -c
FLAGS := -Wall -std=c99 -g
MACROS := -D_POSIX_C_SOURCE=199309L

a.out: main.o queue.o
	gcc $(FLAGS) $(MACROS) main.c queue.c

main.o: main.c queue.o
	gcc $(CFLAGS) main.c

queue.o: queue.c queue.h
	gcc $(CFLAGS) $(MACROS) queue.c

clean:
	rm *.o
