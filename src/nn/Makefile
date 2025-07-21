# Compiler and flags
CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic\
	-I. 

LDFLAGS = -lm
	
# Directories
SRC_DIR = src
OBJ_DIR = build
BIN_DIR = bin

# Files
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/model.c $(SRC_DIR)/data_loader.c $(SRC_DIR)/train.c $(SRC_DIR)/evaluate.c $(SRC_DIR)/model_io.c $(SRC_DIR)/utils.c
OBJ = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))
TARGET = $(BIN_DIR)/mnist_nn

# Default target
$(TARGET): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile each source file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
