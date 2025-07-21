#ifndef BITWISESHIFTS_H
#define BITWISESHIFTS_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

/**
 * @enum shift_type
 * @brief Enumerates the types of bitwise shift operations.
 *
 * These operations are used to shift or rotate bits in a 32-bit or 64-bit operand.
 */

typedef enum {
    LSL, /**< Logical Shift Left: shifts bits to the left, filling with 0s from the right. */
    LSR, /**< Logical Shift Right: shifts bits to the right, filling with 0s from the left. */
    ASR, /**< Arithmetic Shift Right: shifts bits to the right, preserving the sign bit (MSB). */
    ROR /**< Rotate Right: rotates bits right, wrapping lower bits to the upper bits. */
} shift_type;

/**
 * @brief Performs a bitwise shift or rotate operation on a 32-bit or 64-bit value.
 *
 * This function supports logical left/right shifts, arithmetic right shifts,
 * and right rotates. It handles 32-bit and 64-bit modes by masking and preserving
 * the correct number of bits.
 *
 * @param value The value to shift or rotate.
 * @param shift_type The type of shift operation (LSL, LSR, ASR, ROR).
 * @param shift_amount The number of bit positions to shift or rotate.
 * @param is64bit Non-zero if operating on 64-bit data; zero for 32-bit mode.
 * @return The shifted or rotated value with correct bit width applied.
 */

uint64_t bitwise_shift(
    uint64_t value, 
    int shift_type, 
    int shift_amount, 
    int is_64bit
);

#endif 
