#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Machine
{
    uint8_t memory[65536];
    uint16_t stack[16];
    size_t stack_pointer;
    uint16_t pc_stack[16];
    size_t pc_stack_pointer;
    uint16_t acc;
    uint16_t pc;
    uint16_t registers[4];
    bool flags[16];
} Machine;

uint16_t stack_pop(uint16_t stack[], size_t *stack_pointer);

bool stack_push(uint16_t stack[], size_t *stack_pointer, size_t stack_size, uint16_t to_push);

#endif
