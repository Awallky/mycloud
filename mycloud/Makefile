CFLAGS = -Wall
CPPFLAGS = -Wall

all: server mcput mcget mcdel mclist

csapp.h:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/include/csapp.h

csapp.c:
	wget http://csapp.cs.cmu.edu/2e/ics2/code/src/csapp.c

csapp.o: csapp.h csapp.c
	gcc -c $(CFLAGS) csapp.h csapp.c	

mycloud.o: mycloud.h mycloud.c
	gcc -c $(CFLAGS) mycloud.h mycloud.c

server: server.c csapp.o 
	gcc $(CFLAGS) server.c csapp.o -lpthread -o ./Test/server
	cp ./Test/server ..

mcput: mcput.c csapp.o mycloud.o
	gcc $(CFLAGS) mcput.c csapp.o mycloud.o -lpthread -o mcput

mcget: mcget.c csapp.o mycloud.o
	gcc $(CFLAGS) mcget.c csapp.o mycloud.o -lpthread -o mcget

mcdel: mcdel.c csapp.o mycloud.o
	gcc $(CFLAGS) mcdel.c csapp.o mycloud.o -lpthread -o mcdel

mclist: mclist.c csapp.o mycloud.o
	gcc $(CFLAGS) mclist.c csapp.o mycloud.o -lpthread -o mclist

.PHONY: clean
clean:
	/bin/rm -rf csapp.h csapp.c *.o server mcput mcget mcdel mclist ./Test/server
