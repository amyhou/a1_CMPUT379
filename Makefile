CC = gcc
CFLAGS = -Wall -02
OBJECTS = dragonshell.o

.PHONY: all clean

all: dragonshell

clean:
	rm *.o dragonshell

dragonshell.o: dragonshell.c
	$(CC) $(CFLAGS) -c dragonshell.c -o dragonshell.o

dragonshell: $(OBJECTS)
	$(CC) -o dragonshell $(OBJECTS)
