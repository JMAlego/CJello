#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include "components.h"
#include "instructions.h"

#define VERSION "CJello v0.1\n"
#define DESCRIPTION "CJello is the reference implementation of the Jello virtual machine in C.\n"
#define HELP "HELP, I need sombody. HELP, not just anybody. HELP, you know I need someone. HEEEELP!\n"


void debuginate(){
    printf("Running Debug Scripts...\n");



    printf("Debug Scripts Run.\n");
}


int main(int argc, char const *argv[])
{
    bool version = false;
    bool description = false;
    bool help = false;
    bool input_file_present = false;
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


    debuginate();


    if (version)
        printf(VERSION);
    if (description)
        printf(DESCRIPTION);
    if (help)
        printf(HELP);
    if (version || description || help)
        return 0;



    return 0;
}
