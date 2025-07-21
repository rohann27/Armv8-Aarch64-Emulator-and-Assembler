#include <stdlib.h>
#include <stdio.h>
#include <stdint.h> 
#include <limits.h>
#include "utils.h"
#include "machine_state.h"
#include "utils.h"

uint32_t getRangeInt(
    uint32_t instrInt, 
    uint8_t l, uint8_t r
) {    
    instrInt >>= r; // Right shift instrInt r times
    
    // Create a number with l - r number of 1's. (From the right)
    // Ex/ ...0000111
    uint32_t andValue = 0;
    for (int i = 0; i < l - r; i++) {
        andValue++;
        andValue <<= 1;
    }
    andValue++;

    return (instrInt & andValue);
}

int checkSignedSumOverflow(
    long long a, 
    long long b,
    int is_32bit
) {
    if (is_32bit) {
        int a32 = (uint32_t)a; // Turn to uint_32 first to ensure we
        int b32 = (uint32_t)b; // Takes the last 32 bits

        if ((b32 > 0 && a32 > INT_MAX - b32) || (b32 < 0 && a32 < INT_MIN - b32)) {
            return 1; // Overflow
        }
    }
    else {
        if ((b > 0 && a > LLONG_MAX - b) || (b < 0 && a < LLONG_MIN - b))
            return 1; // Overflow
    }

    return 0; // No Overflow
}

int checkSignedSubOverflow(
    long long a, 
    long long b, 
    int is_32bit
) {
    b = -b;
    return checkSignedSumOverflow(a, b, is_32bit);
}

int checkUnsignedSumOverflow(
    uint64_t a, 
    uint64_t b, 
    int is_32bit
) {
    if (is_32bit) {
        uint32_t a32 = a;
        uint32_t b32 = b;

        if (a32 + b32 < a32 || a32 + b32 < b32)
            return 1; // Overflow
    }
    else {
        if (a + b < a || a + b < b)
            return 1; // Overflow
    }

    return 0; // No Overflow
}

int checkUnsignedSubOverflow(
    uint64_t a,
    uint64_t b, 
    int is_32bit
) {
    if (is_32bit) {
        uint32_t a32 = a;
        uint32_t b32 = b;

        if (b32 > a32)
            return 0; // Overflow
    }
    else {
        if (b > a)
            return 0; // Overflow
    }

    return 1; // No Overflow
}

uint64_t load_doubleword(
    STATE *state, 
    uint64_t addr
) {
    if (addr + 7 >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at address 0x%lx\n", addr);
        exit(1);
    }

    uint64_t value = 0;
    for (int i = 0; i < 8; i++) {
        value |= ((uint64_t)state->memory[addr + i]) << (8 * i);
    }
    return value;
}

uint32_t load_word(
    STATE *state, 
    uint64_t addr
) {
    if (addr + 3 >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at address 0x%lx\n", addr);
        exit(1);
    }

    uint32_t value = 0;
    for (int i = 0; i < 4; i++) {
        value |= ((uint32_t)state->memory[addr + i]) << (8 * i);
    }
    return value;
}

void store_doubleword(
    STATE *state, 
    uint64_t addr, 
    uint64_t value
) {
    if (addr + 7 >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at address 0x%lx\n", addr);
        exit(1);
    }

    for (int i = 0; i < 8; i++) {
        state->memory[addr + i] = (uint8_t)(value >> (8 * i));
    }
}

void store_word(
    STATE *state, 
    uint64_t addr, 
    uint32_t value
) {
    if (addr + 3 >= MEMORY_SIZE) {
        fprintf(stderr, "Memory access out of bounds at address 0x%lx\n", addr);
        exit(1);
    }

    for (int i = 0; i < 4; i++) {
        state->memory[addr + i] = (uint8_t)(value >> (8 * i));
    }
}

uint32_t fetch_next_instruction(
    STATE *state
) {
    // Load the next 4 elements of the array memory
   uint64_t pc = state->pc;

    if (pc + 3 >= MEMORY_SIZE) {
        fprintf(stderr, "Invalid PC value: 0x%lx\n", pc);
        exit(EXIT_FAILURE);
    }

    return  state->memory[pc] |
           (state->memory[pc + 1] << 8) |
           (state->memory[pc + 2] << 16) |
           (state->memory[pc + 3] << 24);  
}

//Arithmetic Helper, Shared by 1.4 and 1.5
void perform_arithmetic_op(
    STATE *state,
    uint32_t rd,
    uint64_t op1,
    uint64_t op2,
    uint32_t opc,
    int is_32bit
) {
    uint64_t result;

    if (opc == ADD || opc == ADDS) {
        result = op1 + op2;
    } else if (opc == SUB || opc == SUBS) {
        result = op1 - op2;
    } else {
        fprintf(stderr, "Invalid arithmetic opc: %d\n", opc);
        exit(EXIT_FAILURE);
    }

    if (rd != 31) {
        set_register(state, rd, result, is_32bit);
    }

    if (opc == ADDS || opc == SUBS) {
        state->pstate.N = (result >> (is_32bit ? 31 : 63)) & 1;
        state->pstate.Z = (result == 0);

        if (opc == ADDS) {
            state->pstate.C = 
                checkUnsignedSumOverflow(op1, op2, is_32bit);
            state->pstate.V = 
                checkSignedSumOverflow((int64_t)op1, (int64_t)op2, is_32bit);
        } else {
            state->pstate.C = 
                checkUnsignedSubOverflow(op1, op2, is_32bit);
            state->pstate.V = 
                checkSignedSubOverflow((int64_t)op1, (int64_t)op2, is_32bit);
        }
    }
}


int32_t sign_extend(uint32_t value, int bit_width) {
    if (value & (1u << (bit_width - 1))) {
        return value | (~0u << bit_width);
    } else {
        return value;
    }
}

// extends value to 64 bits
int64_t sign_extend_64(int64_t value, int bits) {
    int shift = 64 - bits;
    return (value << shift) >> shift;
}
