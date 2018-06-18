NAME=out/bmp
CFLAGS=-o $(NAME)
SRCS=src/main.cpp src/bitmap.cpp
CC=g++

all: main

main: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS)
clean:
	rm -rf src/*.o
	rm -rf $(NAME) 
	rm -f out.bmp