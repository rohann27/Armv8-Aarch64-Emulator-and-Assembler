#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "bitwise_shifts.h"

uint64_t bitwise_shift(
    uint64_t value, 
    int shift_type, 
    int shift_amount, 
    int is_64bit
) {
    int sign_bit; // Gets sign
    uint64_t mask; // Calculates the correct mask depending if 32 or 64 bit
    int width; // Helper variable used for ASR and ROR

    // Sets up whether we are dealing with 32 or 64 bit

    if (is_64bit) {
        mask = 0xFFFFFFFFFFFFFFFFULL;
        sign_bit = (value >> 63) & 1;
        width = 64;
    }
    else {
        mask = 0xFFFFFFFFULL;
        sign_bit = (value >> 31) & 1;
        width = 32;
    }

    // Bitwise and with mask so that we have the correct number of bits 

    value &= mask;

    // Select the correct type of bitwise shift, LSL, LSR, ASR, ROR

    switch (shift_type) {
        case LSL:
            value = (value << shift_amount);
            break;
        case LSR:
            value = (value >> shift_amount);
            break;
        case ASR: 
            value = (value >> shift_amount); // Sign bit == 0 -> LSR
            if (sign_bit) {
            uint64_t fix = ((1ULL << shift_amount) - 1) << (width - shift_amount);
            value = value | fix;
            }
            break;
        case ROR:
            shift_amount %= width;
            value = (value << (width - shift_amount)) | (value >> shift_amount);
            break;
        default:
        fprintf(stderr, "Invalid shift type\n");
        exit(EXIT_FAILURE);
    }

    return value & mask;

}
