CC      ?= gcc
CFLAGS  ?= -std=c17 -g\
	-D_POSIX_SOURCE -D_DEFAULT_SOURCE\
	-Wall -Werror -pedantic

# output folders
OUT_DIR := ../../out/emulator
OBJ_DIR := $(OUT_DIR)/objects

# targets
EMULATE_EXE  := $(OUT_DIR)/emulate


.SUFFIXES: .c .o

.PHONY: all clean

all: $(EMULATE_EXE)

$(EMULATE_EXE): $(OBJ_DIR)/emulate.o $(OBJ_DIR)/ioutils.o $(OBJ_DIR)/machine_state.o $(OBJ_DIR)/utils.o $(OBJ_DIR)/single_data_transfer.o $(OBJ_DIR)/branch_instructions.o $(OBJ_DIR)/data_proc.o $(OBJ_DIR)/bitwise_shifts.o $(OBJ_DIR)/dp_register.o 
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/emulate.o: emulate.c emulate.h ioutils.h machine_state.h utils.h single_data_transfer.h data_proc.h branch_instructions.h dp_register.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/ioutils.o: ioutils.c ioutils.h machine_state.h utils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/machine_state.o: machine_state.c machine_state.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/utils.o: utils.c utils.h machine_state.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/single_data_transfer.o: single_data_transfer.c single_data_transfer.h machine_state.h utils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/branch_instructions.o: branch_instructions.c machine_state.h utils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/data_proc.o: data_proc.c data_proc.h machine_state.h utils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/bitwise_shifts.o: bitwise_shifts.c bitwise_shifts.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/dp_register.o: dp_register.c dp_register.h machine_state.h bitwise_shifts.h utils.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@


# Ensure output folders exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	$(RM) -r $(OUT_DIR)
	
