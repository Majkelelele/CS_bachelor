CC     = gcc
CFLAGS = -Wall -Wextra -O2 -std=gnu17
LFLAGS =

.PHONY: all clean

TARGET1 = ppcbc
TARGET2 = ppcbs

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(TARGET1).o sum-common.o err.o common.o
$(TARGET2): $(TARGET2).o sum-common.o err.o common.o

# To są zależności wygenerowane automatycznie za pomocą polecenia `gcc -MM *.c`.



client_TCP.o: client_TCP.c sum-common.h err.h common.h protconst.h
client_UDP.o: client_UDP.c sum-common.h err.h common.h protconst.h
common.o: common.c err.h common.h sum-common.h protconst.h
err.o: err.c err.h
ppcbc.o: ppcbc.c sum-common.h err.h common.h client_TCP.c protconst.h \
 client_UDP.c
ppcbs.o: ppcbs.c sum-common.h err.h common.h server_TCP.c protconst.h \
 server_UDP.c
server_TCP.o: server_TCP.c sum-common.h err.h common.h protconst.h
server_UDP.o: server_UDP.c sum-common.h err.h common.h protconst.h
sum-common.o: sum-common.c sum-common.h

clean:
	rm -f $(TARGET1) $(TARGET2) *.o *~