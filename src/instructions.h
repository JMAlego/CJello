#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include "components.h"

void (*const instructions[256])(Machine *);

void call_instruction(uint8_t instruction, Machine *machine);

#endif
