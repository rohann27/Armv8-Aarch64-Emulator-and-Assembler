#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include <stdint.h>

#define MEMORY_SIZE (2*1024*1024)
#define ADDRESS_SIZE_BITS 21
#define INSTRUCTION_SIZE_BITS 32
#define NUM_REGISTERS 31
#define HALT_INSTR 2315255808 // 8a000000

/**
 * This structure contains the four main condition flags:
 * - N: Negative
 * - Z: Zero
 * - C: Carry
 * - V: Overflow
 */
typedef struct { 
    int N; 
    int Z; 
    int C; 
    int V; 
} PSTATE;

/**
 * Represents the complete state of the machine:
 * - General-purpose registers (X0 to X30, XZR)
 * - Program counter (`pc`)
 * - Condition flags (`pstate`)
 * - Memory space
 * - Halt status
 */
typedef struct {
    uint64_t registers[NUM_REGISTERS]; 
    uint64_t pc;
    PSTATE pstate;
    uint8_t memory[MEMORY_SIZE];
    int is_halted;
} STATE;

/**
 * Allocates and initializes a new machine state.
 *
 * All memory, registers, and flags are initialized to 0,
 * except for the Zero (Z) flag which is initialized to 1.
 *
 * @return Pointer to the newly allocated STATE structure.
 */
STATE *new_machine_state(void);

/**
 * Frees the memory allocated to aa previously initialised machine state.
 *
 * @param state Pointer to the machine state to be freed.
 */
void free_machine_state(
    STATE *state
);

/**
 * Returns the value stored in the given register. For 32-bit access,
 * only the lower 32 bits are returned. Access to register 31 always returns 0.
 *
 * @param state Pointer to the machine state.
 * @param index Register index (0 to 31).
 * @param is_32bit If non-zero, return only lower 32 bits of the register.
 * @return The value stored in the register (0 if register is XZR/WZR).
 */
uint64_t get_register(
    STATE *state, 
    int index, 
    int is_32bit
);

/**
 * Stores a value in the specified register. If the write is 32-bit, only
 * the lower 32 bits are written. Writes to register 31 (XZR/WZR) are ignored.
 *
 * @param state Pointer to the machine state.
 * @param index Register index (0 to 31).
 * @param value Value to be written.
 * @param is_32bit If non-zero, write only lower 32 bits of value.
 */
void set_register(
    STATE *state, 
    int index, 
    uint64_t value, 
    int is_32bit
);

// HELPER FUNCTIONS
void print_registers(
    STATE *state
);
void print_pstate(
    STATE *state
);
void print_memory(
    STATE *state
);

#endif
