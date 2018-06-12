#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "components.h"
#include "instructions.h"

#define VERSION "CJello v0.1\n"
#define DESCRIPTION "CJello is the reference implementation of the Jello virtual machine in C.\n"
#define HELP "Usage: cjello [options] file...\nOptions:\n  -v, --version  Displays a version message\n\
  -h, --help     Displays this help message\n  -d, --debug    Enables debug messages\n\nUse `-` to take input from stdin.\n"

int main(int argc, char const *argv[])
{
    bool version = false;
    bool description = false;
    bool help = false;
    bool input_file_present = false;
    bool debug_mode = false;
    bool read_from_stdin = false;
    const char *input_file;

    if (argc < 2)
    {
        version = true;
        description = true;
        help = true;
        input_file_present = false;
    }
    else
    {
        for (int arg_count = 1; arg_count < argc; arg_count++)
        {
            if (!version && (strncmp("-v", argv[arg_count], 3) == 0 || strncmp("--version", argv[arg_count], 10) == 0))
            {
                version = true;
            }
            else if (!help && (strncmp("-h", argv[arg_count], 3) == 0 || strncmp("--help", argv[arg_count], 7) == 0))
            {
                version = true;
                description = true;
                help = true;
            }
            else if (!debug_mode && (strncmp("-d", argv[arg_count], 3) == 0 || strncmp("--debug", argv[arg_count], 8) == 0))
            {
                debug_mode = true;
            }
            else if (!read_from_stdin && strncmp("-", argv[arg_count], 2) == 0)
            {
                read_from_stdin = true;
            }
            else if (!input_file_present && strncmp("-", argv[arg_count], 1) != 0)
            {
                input_file_present = true;
                input_file = argv[arg_count];
            }
            else
            {
                version = true;
                description = true;
                help = true;
            }
        }
    }

    if (version)
        printf(VERSION);
    if (description)
        printf(DESCRIPTION);
    if (help)
        printf(HELP);
    if (version || description || help)
        return 0;

    Machine machine;
    machine_init(&machine);

    size_t read_bytes = 0;

    if (input_file_present)
    {
        FILE *fp;
        if (NULL == (fp = fopen(input_file, "rb")))
        {
            fputs("Unable to open input file.\n", stderr);
            return 1;
        }

        read_bytes = fread(machine.memory, 1, MEMORY_SIZE, fp);

        fclose(fp);
    }
    else if (read_from_stdin)
    {
        read_bytes = fread(machine.memory, 1, MEMORY_SIZE, stdin);
    }

    if (debug_mode)
        printf("Loaded %zu bytes from file.\n", read_bytes);

    if (read_bytes == 0)
    {
        fputs("No input.\n", stderr);
        return 2;
    }

    while (!machine.flags[FLAG_HALT])
    {
        if (debug_mode)
            printf("Call to 0x%x at 0x%x\n", machine.memory[machine.pc], machine.pc);
        call_instruction(machine.memory[machine.pc], &machine);
    }

    return 0;
}
