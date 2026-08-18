#include "thythl.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_VARS 1000
#define PROG_START_LENGTH 100

#define REPORT_ERROR(x) do {fprintf(stderr, "%s | on line: %d | in file: %s\n", x, __LINE__, __FILE__);} while (0)

static Var* variables = NULL;

static int* program = NULL;
static size_t program_length = 0;

static char get_char(const int index)
{
    if (index >= program_length) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    return *((char*)variables[index].data);
}

int program_start(void)
{
    variables = calloc(MAX_VARS, sizeof(Var));
    if (variables == NULL) return 1;

    program = malloc(PROG_START_LENGTH * (sizeof *program));
    if (program == NULL) return 1;
    program_length = PROG_START_LENGTH;

    return 0;
}

int program_resize(void)
{
    program_length *= 2;
    int* new_program = realloc(program, program_length);
    if (new_program == NULL) return 1;
    program = new_program;

    return 0;
}


int program_run(void)
{
    for (size_t i = 0; i < program_length; ++i) {
        switch (program[i]) {
            case GOTO:
                i = program[i + 1];
            continue;
            case GOIF:
                if (get_char(program[i + 1]))
                    i = i + 2;
                else
                    i = program[i + 2];
            continue;

            case END: return 0;
            default:
                REPORT_ERROR("Invalid instruction");
            return 1;
        }
    }

    REPORT_ERROR("No END instruction");
    return 1;
}


void program_end(void)
{
    for (size_t i = 0; i < MAX_VARS; ++i)
        free(variables[i].data);

    free(variables);
    free(program);
    variables = NULL;
    program = NULL;
}


