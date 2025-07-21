#ifndef EMULATOR_H
#define EMULATOR_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

// (op == 10 || op == 11) for branch
// (op == 8 || op == 9) for dp imm
// (op == 13 || op == 5) for dp reg
// op0 & 5) == 4 for loads and stores
#define IS_BRANCH(op0) ((op0) == 10 || (op0) == 11)
#define IS_DP_IMM(op0) ((op0) == 8  || (op0) == 9)
#define IS_DP_REG(op0) ((op0) == 13 || (op0) == 5)
#define IS_LOAD_STORE(op0) (((op0) & 5) == 4)
#define PC_INCREMENT 4

#endif
