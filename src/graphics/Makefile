CC = gcc
CFLAGS = -I/opt/homebrew/opt/sdl2/include -I. -I../nn/ -Wall -Wextra -std=c11
LDFLAGS = -L/opt/homebrew/opt/sdl2/lib -lSDL2 -lm

NN_SRC = ../nn/src/data_loader.c ../nn/src/utils.c ../nn/src/model.c ../nn/src/model_io.c  ../nn/src/train.c 

EXEC_NAME = app

all: main

main: main.c utils.c $(NN_SRC) 
	$(CC) $(CFLAGS) main.c $(NN_SRC) utils.c -o $(EXEC_NAME) $(LDFLAGS)

clean:
	rm -f $(EXEC_NAME)
