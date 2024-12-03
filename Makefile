CFLAGS=-c -g -Wall -Werror 
CC=gcc

all: send receive

send: send.o
	$(CC) -o send send.o

recieve: receive.o
	$(CC) -o receive receive.o

clean:
	rm -f *.o send receive

%.o: %.c
	$(CC) $(CFLAGS) -o $@ $<