CC=gcc
CFLAGS=-std=c89 -w -s -O3 -fexpensive-optimizations -fomit-frame-pointer
LIBS=-lm -lGLEW -lglfw -lGL -lpthread -lportaudio -lsndfile 
LIBS_MUTE=-lm -lGLEW -lglfw -lGL
OUTPUT_NAME=ss

all:
	$(CC) -c $(CFLAGS) *.c
	$(CC) *.o $(LIBS) -o $(OUTPUT_NAME)
	rm -f *.o

mute:
	$(CC) -c -DMUTE $(CFLAGS) *.c
	$(CC) *.o $(LIBS_MUTE) -o $(OUTPUT_NAME)
	rm -f *.o

clean:
	rm -f *.o
	rm -f space_ship