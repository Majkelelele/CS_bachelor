CFLAGS = -Wall -Wextra -Wno-implicit-fallthrough -std=gnu17 -fPIC -O2
LDFLAGS = -shared -Wl,--wrap=malloc -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=reallocarray -Wl,--wrap=free -Wl,--wrap=strdup -Wl,--wrap=strndup

all: libseq.so

seq.o: seq.c seq.h
	gcc -c $(CFLAGS) seq.c -o seq.o

memory.o: memory_tests.c memory_tests.h
	gcc -c $(CFLAGS) memory_tests.c -o memory.o

libseq.so: seq.o memory.o
	gcc $(LDFLAGS) seq.o memory.o -o libseq.so

clean:
	rm libseq.so *.o

.PHONY: all clean