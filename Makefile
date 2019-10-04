CC = gcc
CFLAGS = -Wall -O2
OBJECTS = dragonshell.o

.PHONY: all clean compress compile

all: dragonshell

clean:
	rm -f *.o dragonshell.tar.gz dragonshell

compress:
	tar -cvzf dragonshell.tar.gz Makefile dragonshell.c README.*

compile: dragonshell.c
	$(CC) $(CFLAGS) -c dragonshell.c -o dragonshell.o

dragonshell: $(OBJECTS)
	$(CC) -o dragonshell $(OBJECTS)
	echo "\nDone!\n"
