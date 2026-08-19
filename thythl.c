#include "thythl.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_VARS 1000
#define PROG_START_LENGTH 100

#define REPORT_ERROR(x) do {fprintf(stderr, "%s | on line: %d | in file: %s\n", x, __LINE__, __FILE__);} while (0)
#define REPORT_ERRORI(x, y) do {fprintf(stderr, "%s%d | on line: %d | in file: %s\n", x, y, __LINE__, __FILE__);} while (0)

static Var* var_pool = NULL;

static int* program = NULL;
static size_t program_length = 0;

static int get_bool(const int index)
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    if (var_pool[index].type != BOOL) {
        REPORT_ERRORI("Invalid type: ", var_pool[index].type);
        return 0;
    }
    return *(var_pool[index].data);
}

static int program_resize(void)
{
    program_length = (program_length * 2) + 1; // ensures the size cannot be 0
    int* new_program = realloc(program, program_length);
    if (new_program == NULL) return 1;
    program = new_program;

    return 0;
}

// returns the index in vars, negative on error
// maybe replace with hashmap instead of linear search
static int var_alloc(const size_t Size, const Type t) // size is number of ints
{
    for (int i = 0; i < MAX_VARS; ++i) {
        if (var_pool[i].data != NULL) continue;
        var_pool[i].data = malloc(Size * sizeof(int));
        if (var_pool[i].data == NULL) return -1;
        var_pool[i].length = Size;
        var_pool[i].type = t;
        return i;
    }

    return -1;
}

static void var_free(const int index)
{
    free(var_pool[index].data);
    var_pool[index].data = NULL;
    var_pool[index].length = 0;
    var_pool[index].type = NONE;
}

int program_start(void)
{
    program = malloc(PROG_START_LENGTH * (sizeof *program));
    if (program == NULL) return 1;
    program_length = PROG_START_LENGTH;

    var_pool = calloc(MAX_VARS, (sizeof(*var_pool)));
    if (var_pool == NULL) {
        program_end();
        return 1;
    }

    return 0;
}


int program_run(void)
{
    for (size_t i = 0; i < program_length; ++i) {
        switch (program[i]) {
            case GOTO:
                i = (size_t)program[i + 1];
            continue;
            case GOIF:
                if (get_bool(program[i + 1]))
                    i = i + 2;
                else
                    i = (size_t)program[i + 2];
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
    if (var_pool != NULL) {
        for (int i = 0; i < MAX_VARS; ++i) {
            free(var_pool[i].data);
        }
    }

    free(var_pool);
    free(program);
    program = NULL;
    var_pool = NULL;
}


