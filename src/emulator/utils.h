#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "machine_state.h"

// TODO: needs comments
#define ADD  0
#define ADDS 1
#define SUB  2
#define SUBS 3

/**
 * Extracts a range of bits from a 32-bit integer.
 *
 * Given a 32-bit instruction, this function extracts the bits between
 * positions `l` (MSB side) and `r` (LSB side) (inclusive).
 *
 * @param instrInt The 32-bit integer to extract bits from
 * @param l The index of the MSB in the range
 * @param r The index of the LSB in the range
 * @return The extracted bitfield as a 32-bit unsigned integer
 */
uint32_t getRangeInt(
    uint32_t instrInt, 
    uint8_t l, 
    uint8_t r
);

/**
 * Checks for signed overflow in a sum.
 *
 * Determines if the sum of `a + b` causes signed overflow for either
 * 32-bit or 64-bit integers.
 *
 * @param a First operand.
 * @param b Second operand.
 * @param is_32bit Set to 1 if checking for 32-bit overflow, 0 for 64-bit.
 * @return 1 if overflow occurred, 0 otherwise.
 */
int checkSignedSumOverflow(
    long long a, 
    long long b, 
    int is_32bit
);

/**
 * Checks for signed overflow in a subtraction.
 *
 * Determines if the subtraction `a - b` causes signed overflow for either
 * 32-bit or 64-bit integers.
 *
 * @param a Minuend
 * @param b Subtrahend
 * @param is_32bit Set to 1 if checking for 32-bit overflow, 0 for 64-bit.
 * @return 1 if overflow occurred, 0 otherwise.
 */
int checkSignedSubOverflow(
    long long a, 
    long long b, 
    int is_32bit
);

/**
 * Checks for unsigned overflow in a sum.
 *
 * Detects if adding two unsigned integers causes overflow in either 32-bit
 * or 64-bit mode.
 *
 * @param a First operand.
 * @param b Second operand.
 * @param is_32bit Set to 1 for 32-bit check, 0 for 64-bit.
 * @return 1 if overflow occurred, 0 otherwise.
 */
int checkUnsignedSumOverflow(
    uint64_t a, 
    uint64_t b, 
    int is_32bit
);

/**
 * Checks for unsigned overflow in a subtraction.
 *
 * Verifies if the subtraction `a - b` underflows for unsigned integers
 * in either 32-bit or 64-bit mode.
 *
 * @param a Minuend.
 * @param b Subtrahend.
 * @param is_32bit Set to 1 for 32-bit check, 0 for 64-bit.
 * @return 1 if no underflow occurred, 0 if underflow.
 */
int checkUnsignedSubOverflow(
    uint64_t a, 
    uint64_t b, 
    int is_32bit
);

int getSign(
    long long n, 
    int is32bit
);

/**
 * Sign-extends a given value to 32-bit signed integer.
 *
 * @param value The unsigned value to be sign-extended.
 * @param bit_width The bit width of the original value.
 * @return The sign-extended 32-bit signed integer.
 */
int32_t sign_extend(
    uint32_t value, 
    int bit_width
);

/**
 * Sign-extends a value to 64 bits.
 *
 * @param value The value to be sign-extended.
 * @param bits The number of significant bits in `value`.
 * @return The 64-bit sign-extended integer.
 */
int64_t sign_extend_64(
    int64_t value, 
    int bits
);

/**
 * Loads a 64-bit (double word) value from memory.
 *
 * Reads 8 consecutive bytes from memory starting at the specified address
 * and assembles them into a 64-bit little-endian value.
 *
 * @param state Pointer to the machine state.
 * @param addr Memory address to load from.
 * @return The 64-bit value loaded from memory.
 */
uint64_t load_doubleword(
    STATE *state, 
    uint64_t addr
);

/**
 * Loads a 32-bit (word) value from memory.
 *
 * Reads 4 consecutive bytes from memory starting at the specified address
 * and assembles them into a 32-bit little-endian value.
 *
 * @param state Pointer to the machine state.
 * @param addr Memory address to load from.
 * @return The 32-bit value loaded from memory.
 */
uint32_t load_word(
    STATE *state, 
    uint64_t addr
);

/**
 * Stores a 64-bit (doubleword) value to memory.
 *
 * Writes a 64-bit value to memory starting at the specified address
 * in little-endian order.
 *
 * @param state Pointer to the machine state.
 * @param addr Memory address to store to.
 * @param value The 64-bit value to store.
 */
void store_doubleword(
    STATE *state, 
    uint64_t addr, 
    uint64_t value
);

/**
 * Stores a 32-bit (word) value to memory.
 *
 * Writes a 32-bit value to memory starting at the specified address
 * in little-endian order.
 *
 * @param state Pointer to the machine state.
 * @param addr Memory address to store to.
 * @param value The 32-bit value to store.
 */
void store_word(
    STATE *state, 
    uint64_t addr, 
    uint32_t value
);

/**
 * Fetches the next 32-bit instruction from memory.
 *
 * Reads the next 4 bytes from memory at `pc` and
 * returns the assembled 32-bit instruction in little-endian format.
 *
 * @param state Pointer to the machine state.
 * @return The 32-bit instruction fetched from memory.
 */
uint32_t fetch_next_instruction(
    STATE *state
);

// TODO: needs comments
void print_machine_state(
    STATE *state, 
    FILE *fout
);

void perform_arithmetic_op(
    STATE *state,
    uint32_t rd,
    uint64_t op1,
    uint64_t op2,
    uint32_t opc,
    int is_32bit
);

#endif
