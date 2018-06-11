#include "instructions.h"

static void inst_undef(Machine *machine)
{
    fprintf(stderr, "Call to undefined instruction, with pc=%u and memory[pc]=%u.\n", machine->pc, machine->memory[machine->pc]);
    machine->pc++;
}

static void inst_noop(Machine *machine)
{
    machine->pc++;
}

static void inst_halt(Machine *machine)
{
    machine->flags[FLAG_HALT] = true;
}

static void inst_lri(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    machine->acc = part1 | ((uint16_t)part2 << 8);
    machine->pc++;
}

static void inst_lr0(Machine *machine)
{
    machine->acc = machine->registers[FLAG_CARRY];
    machine->pc++;
}

static void inst_lr1(Machine *machine)
{
    machine->acc = machine->registers[1];
    machine->pc++;
}

static void inst_lr2(Machine *machine)
{
    machine->acc = machine->registers[2];
    machine->pc++;
}

static void inst_lr3(Machine *machine)
{
    machine->acc = machine->registers[3];
    machine->pc++;
}

static void inst_sr0(Machine *machine)
{
    machine->registers[0] = machine->acc;
    machine->pc++;
}

static void inst_sr1(Machine *machine)
{
    machine->registers[1] = machine->acc;
    machine->pc++;
}

static void inst_sr2(Machine *machine)
{
    machine->registers[2] = machine->acc;
    machine->pc++;
}

static void inst_sr3(Machine *machine)
{
    machine->registers[3] = machine->acc;
    machine->pc++;
}

static void inst_zero(Machine *machine)
{
    machine->acc = 0;
    machine->pc++;
}

static void inst_lrs(Machine *machine)
{
    machine->pc++;
    machine->acc = machine->memory[machine->pc];
    machine->pc++;
}

static void inst_add(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t input2 = stack_pop(machine->stack, &machine->stack_pointer);
    uint32_t result = input1 + input2;
    machine->acc = (uint16_t)result;
    machine->flags[FLAG_CARRY] = result > MAX_16_BIT_NUM;
    machine->pc++;
}

static void inst_addc(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t input2 = stack_pop(machine->stack, &machine->stack_pointer);
    uint32_t result = input1 + input2;
    if (machine->flags[FLAG_CARRY])
        result++;
    machine->acc = (uint16_t)result;
    machine->flags[FLAG_CARRY] = result > MAX_16_BIT_NUM;
    machine->pc++;
}

static void inst_sub(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t input2 = stack_pop(machine->stack, &machine->stack_pointer);
    int32_t result = input1 - input2;
    bool carry = false;
    if (result < 0)
    {
        carry = true;
        result += MAX_16_BIT_INDEX;
    }
    machine->acc = (uint16_t)result;
    machine->flags[FLAG_CARRY] = carry;
    machine->pc++;
}

static void inst_subc(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t input2 = stack_pop(machine->stack, &machine->stack_pointer);
    int32_t result = input1 - input2;
    if (machine->flags[FLAG_CARRY])
        result--;
    bool carry = false;
    if (result < 0)
    {
        carry = true;
        result += MAX_16_BIT_INDEX;
    }
    machine->acc = (uint16_t)result;
    machine->flags[FLAG_CARRY] = carry;
    machine->pc++;
}

static void inst_neg(Machine *machine)
{
    uint16_t input = stack_pop(machine->stack, &machine->stack_pointer);
    machine->acc = input ^ MAX_16_BIT_NUM;
    machine->acc++;
    machine->pc++;
}

static void inst_mul(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t input2 = stack_pop(machine->stack, &machine->stack_pointer);
    uint32_t result = input1 * input2;
    machine->acc = (uint16_t)result;
    machine->flags[FLAG_OVERFLOW] = result > MAX_16_BIT_NUM; //overflow
    machine->pc++;
}

static void inst_div(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t input2 = stack_pop(machine->stack, &machine->stack_pointer);
    machine->acc = input1 / input2;
    machine->pc++;
}

static void inst_addi(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    uint16_t input2 = part1 | ((uint16_t)part2 << 8);
    uint32_t result = input1 + input2;
    machine->acc = (uint16_t)result;
    machine->flags[FLAG_CARRY] = result > MAX_16_BIT_NUM;
    machine->pc++;
}

static void inst_subi(Machine *machine)
{
    uint16_t input1 = stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    uint16_t input2 = part1 | ((uint16_t)part2 << 8);
    int32_t result = input1 - input2;
    bool carry = false;
    if (result < 0)
    {
        carry = true;
        result += MAX_16_BIT_INDEX;
    }
    machine->acc = (uint16_t)result;
    machine->flags[FLAG_CARRY] = carry;
    machine->pc++;
}

static void inst_push(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, machine->acc);
    machine->pc++;
}

static void inst_pop(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_swap(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    machine->pc++;
}

static void inst_peak(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, machine->acc);
    machine->pc++;
}

static void inst_spill(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    machine->pc++;
}

static void inst_drop(Machine *machine)
{
    stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_under(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, machine->acc);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    machine->pc++;
}

static void inst_rotcw(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t third = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, third);
    machine->pc++;
}

static void inst_rotac(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t third = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, third);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    machine->pc++;
}

static void inst_dup(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    machine->pc++;
}

static void inst_pushi(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, part1 | ((uint16_t)part2 << 8));
    machine->pc++;
}

static void inst_rotcw4(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t third = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t fourth = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, third);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, fourth);
    machine->pc++;
}

static void inst_rotac4(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t third = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t fourth = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, fourth);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, third);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    machine->pc++;
}

static void inst_rotcw5(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t third = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t fourth = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t fifth = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, fifth);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, fourth);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, third);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    machine->pc++;
}

static void inst_rotac5(Machine *machine)
{
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t third = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t fourth = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t fifth = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, fourth);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, third);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, fifth);
    machine->pc++;
}

static void inst_jmpi(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    machine->pc = part1 | ((uint16_t)part2 << 8);
}

static void inst_jmpig(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];

    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);

    if (first > second)
        machine->pc = part1 | ((uint16_t)part2 << 8);
    else
        machine->pc++;
}

static void inst_jmpil(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];

    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);

    if (first < second)
        machine->pc = part1 | ((uint16_t)part2 << 8);
    else
        machine->pc++;
}

static void inst_jmpie(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];

    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);

    if (first == second)
        machine->pc = part1 | ((uint16_t)part2 << 8);
    else
        machine->pc++;
}

static void inst_jmpit(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];

    if (machine->flags[FLAG_TEST]) //test flag
        machine->pc = part1 | ((uint16_t)part2 << 8);
    else
        machine->pc++;
}

static void inst_jmp(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc = address;
}

static void inst_jmpgt(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);

    if (first > second)
        machine->pc = address;
    else
        machine->pc++;
}

static void inst_jmplt(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);

    if (first < second)
        machine->pc = address;
    else
        machine->pc++;
}

static void inst_jmpeq(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t first = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t second = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, second);
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, first);

    if (first == second)
        machine->pc = address;
    else
        machine->pc++;
}

static void inst_jmpt(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    if (machine->flags[FLAG_TEST]) //test flag
        machine->pc = address;
    else
        machine->pc++;
}

static void inst_pushp(Machine *machine)
{
    stack_push(machine->pc_stack, &machine->pc_stack_pointer, PC_STACK_SIZE, machine->pc);
    machine->pc++;
}

static void inst_popp(Machine *machine)
{
    machine->pc = stack_pop(machine->pc_stack, &machine->pc_stack_pointer);
}

static void inst_call(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    stack_push(machine->pc_stack, &machine->pc_stack_pointer, PC_STACK_SIZE, machine->pc);
    machine->pc = address;
}

static void inst_ret(Machine *machine)
{
    machine->pc = stack_pop(machine->pc_stack, &machine->pc_stack_pointer);
    machine->pc++;
}

static void inst_calli(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    stack_push(machine->pc_stack, &machine->pc_stack_pointer, PC_STACK_SIZE, machine->pc);
    machine->pc = part1 | ((uint16_t)part2 << 8);
}

static void inst_skip(Machine *machine)
{
    machine->pc += 2;
}

static void inst_load(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    uint8_t part1 = machine->memory[address];
    uint8_t part2 = machine->memory[address + 1];
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, part1 | (part2 << 8));
    machine->pc++;
}

static void inst_store(Machine *machine)
{
    uint16_t address = stack_pop(machine->stack, &machine->stack_pointer);
    uint16_t data = stack_pop(machine->stack, &machine->stack_pointer);
    uint8_t part1 = data & 0xff;
    uint8_t part2 = (data >> 8) & 0xff;
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, part1 | (part2 << 8));
    machine->memory[address] = part1;
    machine->memory[address + 1] = part2;
    machine->pc++;
}

static void inst_loadi(Machine *machine)
{
    machine->pc++;
    uint8_t apart1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t apart2 = machine->memory[machine->pc];
    uint16_t address = apart1 | (apart2 << 8);
    uint8_t part1 = machine->memory[address];
    uint8_t part2 = machine->memory[address + 1];
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, part1 | (part2 << 8));
    machine->pc++;
}

static void inst_storei(Machine *machine)
{
    machine->pc++;
    uint8_t apart1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t apart2 = machine->memory[machine->pc];
    uint16_t address = apart1 | (apart2 << 8);
    uint16_t data = stack_pop(machine->stack, &machine->stack_pointer);
    uint8_t part1 = data & 0xff;
    uint8_t part2 = (data >> 8) & 0xff;
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, part1 | (part2 << 8));
    machine->memory[address] = part1;
    machine->memory[address + 1] = part2;
    machine->pc++;
}

static void inst_or(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) |
                   stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_and(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) &
                   stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_xor(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) ^
                   stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_nand(Machine *machine)
{
    machine->acc = (stack_pop(machine->stack, &machine->stack_pointer) &
                    stack_pop(machine->stack, &machine->stack_pointer)) ^
                   0xffff;
    machine->pc++;
}

static void inst_nor(Machine *machine)
{
    machine->acc = (stack_pop(machine->stack, &machine->stack_pointer) |
                    stack_pop(machine->stack, &machine->stack_pointer)) ^
                   0xffff;
    machine->pc++;
}

static void inst_not(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) ^ 0xffff;
    machine->pc++;
}

static void inst_lsft0(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) << 1;
    machine->pc++;
}

static void inst_rsft0(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) >> 1;
    machine->pc++;
}

static void inst_lsft1(Machine *machine)
{
    machine->acc = (stack_pop(machine->stack, &machine->stack_pointer) << 1) | 0x1;
    machine->pc++;
}

static void inst_rsft1(Machine *machine)
{
    machine->acc = (stack_pop(machine->stack, &machine->stack_pointer) >> 1) | (0x1 << 15);
    machine->pc++;
}

static void inst_ori(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    uint16_t data = part1 | ((uint16_t)part2 << 8);
    machine->acc = data | stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_andi(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    uint16_t data = part1 | ((uint16_t)part2 << 8);
    machine->acc = data & stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_xori(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    uint16_t data = part1 | ((uint16_t)part2 << 8);
    machine->acc = data ^ stack_pop(machine->stack, &machine->stack_pointer);
    machine->pc++;
}

static void inst_nandi(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    uint16_t data = part1 | ((uint16_t)part2 << 8);
    machine->acc = (data & stack_pop(machine->stack, &machine->stack_pointer)) ^ 0xffff;
    machine->pc++;
}

static void inst_nori(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    uint16_t data = part1 | ((uint16_t)part2 << 8);
    machine->acc = (data | stack_pop(machine->stack, &machine->stack_pointer)) ^ 0xffff;
    machine->pc++;
}

static void inst_inc(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) + 1;
    machine->pc++;
}

static void inst_inc2(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) + 2;
    machine->pc++;
}

static void inst_inc3(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) + 3;
    machine->pc++;
}

static void inst_inc4(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) + 4;
    machine->pc++;
}

static void inst_dec(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) - 1;
    machine->pc++;
}

static void inst_dec2(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) - 2;
    machine->pc++;
}

static void inst_dec3(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) - 3;
    machine->pc++;
}

static void inst_dec4(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) - 4;
    machine->pc++;
}

static void inst_incp(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) + 1);
    machine->pc++;
}

static void inst_inc2p(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) + 2);
    machine->pc++;
}

static void inst_inc3p(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) + 3);
    machine->pc++;
}

static void inst_inc4p(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) + 4);
    machine->pc++;
}

static void inst_decp(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) - 1);
    machine->pc++;
}

static void inst_dec2p(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) - 2);
    machine->pc++;
}

static void inst_dec3p(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) - 3);
    machine->pc++;
}

static void inst_dec4p(Machine *machine)
{
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, stack_pop(machine->stack, &machine->stack_pointer) - 4);
    machine->pc++;
}

static void inst_test(Machine *machine)
{
    machine->flags[FLAG_TEST] = machine->flags[FLAG_CARRY] | machine->flags[FLAG_OVERFLOW];
    machine->pc++;
}

static void inst_testc(Machine *machine)
{
    machine->flags[FLAG_TEST] = machine->flags[FLAG_CARRY];
    machine->pc++;
}

static void inst_testo(Machine *machine)
{
    machine->flags[FLAG_TEST] = machine->flags[FLAG_OVERFLOW];
    machine->pc++;
}

static void inst_tsast(Machine *machine)
{
    machine->flags[FLAG_TEST] = stack_pop(machine->stack, &machine->stack_pointer) != 0;
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, 1);
    machine->pc++;
}

static void inst_tsainc(Machine *machine)
{
    uint16_t data = stack_pop(machine->stack, &machine->stack_pointer);
    machine->flags[FLAG_TEST] = data != 0;
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, data + 1);
    machine->pc++;
}

static void inst_tsadec(Machine *machine)
{
    uint16_t data = stack_pop(machine->stack, &machine->stack_pointer);
    machine->flags[FLAG_TEST] = data != 0;
    stack_push(machine->stack, &machine->stack_pointer, STACK_SIZE, data - 1);
    machine->pc++;
}

static void inst_tsastr(Machine *machine)
{
    machine->flags[FLAG_TEST] = machine->registers[0] != 0;
    machine->registers[0] = 1;
    machine->pc++;
}

static void inst_tsainr(Machine *machine)
{
    machine->flags[FLAG_TEST] = machine->registers[0] != 0;
    machine->registers[0]++;
    machine->pc++;
}

static void inst_tsader(Machine *machine)
{
    machine->flags[FLAG_TEST] = machine->registers[0] != 0;
    machine->registers[0]--;
    machine->pc++;
}

static void inst_lsftb(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) << 8;
    machine->pc++;
}

static void inst_rsftb(Machine *machine)
{
    machine->acc = stack_pop(machine->stack, &machine->stack_pointer) >> 8;
    machine->pc++;
}

static void inst_prn(Machine *machine)
{
    uint8_t popped = stack_pop(machine->stack, &machine->stack_pointer) & LOWER_BYTE;
    printf("%c", popped);
    machine->pc++;
}

static void inst_prni(Machine *machine)
{
    machine->pc++;
    uint8_t data = machine->memory[machine->pc];
    printf("%c", data);
    machine->pc++;
}

static void inst_prn2(Machine *machine)
{
    uint16_t popped = stack_pop(machine->stack, &machine->stack_pointer);
    uint8_t part1 = popped & LOWER_BYTE;
    uint8_t part2 = (popped >> 8) & LOWER_BYTE;
    printf("%c%c", part1, part2);
    machine->pc++;
}

static void inst_prn2i(Machine *machine)
{
    machine->pc++;
    uint8_t part1 = machine->memory[machine->pc];
    machine->pc++;
    uint8_t part2 = machine->memory[machine->pc];
    printf("%c%c", part1, part2);
    machine->pc++;
}

static void inst_dump8(Machine *machine)
{
    uint8_t popped = stack_pop(machine->stack, &machine->stack_pointer) & LOWER_BYTE;
    printf("0x%02x", popped);
    machine->pc++;
}

static void inst_dump16(Machine *machine)
{
    uint16_t popped = stack_pop(machine->stack, &machine->stack_pointer);
    printf("0x%04x", popped);
    machine->pc++;
}

void (*const instructions[256])(Machine *) = {
    inst_noop, //0x00
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_halt,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_lri, //0x10
    inst_lr0,
    inst_lr1,
    inst_lr2,
    inst_lr3,
    inst_sr0,
    inst_sr1,
    inst_sr2,
    inst_sr3,
    inst_zero,
    inst_lrs,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_add, //0x20
    inst_addc,
    inst_sub,
    inst_subc,
    inst_neg,
    inst_mul,
    inst_div,
    inst_addi,
    inst_subi,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_push, //0x30
    inst_pop,
    inst_swap,
    inst_peak,
    inst_spill,
    inst_drop,
    inst_under,
    inst_rotcw,
    inst_rotac,
    inst_dup,
    inst_pushi,
    inst_rotcw4,
    inst_rotac4,
    inst_rotcw5,
    inst_rotac5,
    inst_undef,
    inst_jmpi, //0x40
    inst_jmpig,
    inst_jmpil,
    inst_jmpie,
    inst_jmpit,
    inst_jmp,
    inst_jmpgt,
    inst_jmplt,
    inst_jmpeq,
    inst_jmpt,
    inst_pushp,
    inst_popp,
    inst_call,
    inst_ret,
    inst_calli,
    inst_skip,
    inst_load, //0x50
    inst_store,
    inst_loadi,
    inst_storei,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_or, //0x60
    inst_and,
    inst_xor,
    inst_nand,
    inst_nor,
    inst_not,
    inst_lsft0,
    inst_rsft0,
    inst_lsft1,
    inst_rsft1,
    inst_ori,
    inst_andi,
    inst_xori,
    inst_nandi,
    inst_nori,
    inst_undef,
    inst_inc, //0x70
    inst_inc2,
    inst_inc3,
    inst_inc4,
    inst_dec,
    inst_dec2,
    inst_dec3,
    inst_dec4,
    inst_incp,
    inst_inc2p,
    inst_inc3p,
    inst_inc4p,
    inst_decp,
    inst_dec2p,
    inst_dec3p,
    inst_dec4p,
    inst_test, //0x80
    inst_testc,
    inst_testo,
    inst_tsast,
    inst_tsainc,
    inst_tsadec,
    inst_tsastr,
    inst_tsainr,
    inst_tsader,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_lsftb, //0x90
    inst_rsftb,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef, //0xa0
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef, //0xb0
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef, //0xc0
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef, //0xd0
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef, //0xe0
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_prn, //0xf0
    inst_prni,
    inst_prn2,
    inst_prn2i,
    inst_dump8,
    inst_dump16,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef,
    inst_undef};

void call_instruction(uint8_t instruction, Machine *machine)
{
    instructions[instruction](machine);
}
