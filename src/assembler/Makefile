CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic\
	-I. -I../emulator

# output folders
OUT_DIR := ../../out/assembler
OBJ_DIR := $(OUT_DIR)/objects

SRC := assemble.c parser.c storing_labels.c symbol_table.c tokens.c encoding_functions.c instruction_representation.c assemble_utils.c
EXT_SRC := ../emulator/bitwise_shifts.c

OBJ := $(SRC:%.c=$(OBJ_DIR)/%.o)
EXT_OBJ := $(OBJ_DIR)/bitwise_shifts.o


# targets
ASSEMBLE_EXE  := $(OUT_DIR)/assemble


.SUFFIXES: .c .o

.PHONY: all clean

all: $(ASSEMBLE_EXE)

$(ASSEMBLE_EXE): $(OBJ) $(EXT_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(EXT_OBJ): $(EXT_SRC) | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure output folders exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) -r $(OUT_DIR)
	
