#include "components.h"

uint16_t stack_pop(uint16_t stack[], size_t *stack_pointer)
{
    if (*stack_pointer == 0)
        return 0;
    *stack_pointer -= 1;
    return stack[*stack_pointer];
}

bool stack_push(uint16_t stack[], size_t *stack_pointer, size_t stack_size, uint16_t to_push)
{
    if (*stack_pointer == stack_size)
    {
        for (size_t counter = 0; counter < stack_size - 1; counter++)
        {
            stack[counter] = stack[counter + 1];
        }
        stack[*stack_pointer - 1] = to_push;
        return true;
    }
    stack[*stack_pointer] = to_push;
    *stack_pointer += 1;
    return false;
}

void machine_init(Machine *machine)
{
    machine->acc = 0;
    size_t i;
    for (i = 0; i < 16; i++)
        machine->flags[i] = false;
    for (i = 0; i < 65536; i++)
        machine->memory[i] = 0;
    machine->pc = 0;
    machine->pc_stack_pointer = 0;
    machine->stack_pointer = 0;
    for (i = 0; i < 4; i++)
        machine->registers[i] = 0;
}
