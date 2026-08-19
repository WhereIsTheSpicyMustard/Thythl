#include "thythl.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_VARS 1000
#define PROG_START_LENGTH 100

#define REPORT_ERROR(x) do {fprintf(stderr, "%s | on line: %d | in file: %s\n", x, __LINE__, __FILE__);} while (0)
#define REPORT_ERRORI(x, y) do {fprintf(stderr, "%s%d | on line: %d | in file: %s\n", x, y, __LINE__, __FILE__);} while (0)

#define JUMP(x) i = x - 1

static Var* var_pool = NULL;

static int* program = NULL;
static size_t program_length = 0;

static int var_get(const int index)
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
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
static int var_alloc(const int index, const Type t, const size_t Size) // size is number of ints
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    if (var_pool[index].data != NULL) return 1;
    var_pool[index].data = malloc(Size * sizeof(int));
    if (var_pool[index].data == NULL) return 1;
    var_pool[index].length = Size;
    var_pool[index].type = t;
    return 0;
}

static void var_free(const int index)
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return;
    }
    free(var_pool[index].data);
    var_pool[index].data = NULL;
    var_pool[index].length = 0;
    var_pool[index].type = NONE;
}

static int var_set(const int index, const int value)
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    if (var_pool[index].data == NULL) return 1;
    printf("Edit var[%d]: %d -> %d\n", index, *(var_pool[index].data), value);
    *(var_pool[index].data) = value;
    return 0;
}

static void test(void)
{

    for (int i = 0; i < PROG_START_LENGTH; ++i)
        program[i] = END;

    int var1 = 0;
    int var2 = 1;
    int var3 = 2;

    if (var_alloc(var1, INT, 1)) {
        REPORT_ERROR("Failed to allocate var");
        return;
    }
    if (var_alloc(var2, INT, 1)) {
        REPORT_ERROR("Failed to allocate var");
        return;
    }
    if (var_alloc(var3, INT, 1)) {
        REPORT_ERROR("Failed to allocate var");
        return;
    }


    if (var_set(var1, 2)) {
        REPORT_ERROR("Failed to allocate var");
        return;
    }
    if (var_set(var2, -1)) {
        REPORT_ERROR("Failed to allocate var");
        return;
    }

    // TEST
    program[0] = JIF;
    program[1] = var1; // points to a variable
    program[2] = 10;
    program[3] = ADD;
    program[4] = var1; // dest
    program[5] = var1;
    program[6] = var2;
    program[7] = JMP;
    program[8] = 0;

    program[10] = END;
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

    test();

    return 0;
}

int program_run(void)
{
    for (int i = 0; (size_t)i < program_length; ++i) {
        printf("PROGRAM > %d\n", i);
        switch (program[i]) {
            case JMP: // goto [i + 1]
                JUMP(program[i + 1]);
            continue;
            case JIF: // if [i + 1] != 0 {goto [i + 2]} else {goto i + 3}
                if (var_get(program[i + 1])) {
                    JUMP(program[i + 2]);
                } else {
                    JUMP(i + 3);
                }
            continue;
            case JNO: // if [i + 1] != 0 {goto i + 3} else {goto [i + 2]}
                if (var_get(program[i + 1])) {
                    JUMP(i + 3);
                } else {
                    JUMP(program[i + 2]);
                }
            continue;
            case JEQ:
                if (var_get(program[i + 1]) == var_get(program[i + 2])) {
                    JUMP(i + 4);
                } else {
                    JUMP(program[i + 3]);
                }
            continue;
            case JNE:
                if (var_get(program[i + 1]) != var_get(program[i + 2])) {
                    JUMP(i + 4);
                } else {
                    JUMP(program[i + 3]);
                }
            continue;
            case SET:
                var_set(program[i + 1], var_get(program[i + 2]));
                JUMP(i + 3);
            continue;
            case ADD:
                var_set(program[i + 1],
                var_get(program[i + 2]) + var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case SUB:
                var_set(program[i + 1],
                var_get(program[i + 2]) - var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case MUL:
                var_set(program[i + 1],
                var_get(program[i + 2]) * var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case DIV:
                var_set(program[i + 1],
                var_get(program[i + 2]) / var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case MOD:
                var_set(program[i + 1],
                var_get(program[i + 2]) % var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case CMP:
                var_set(program[i + 1],
                var_get(program[i + 2]) == var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case MOR:
                var_set(program[i + 1],
                var_get(program[i + 2]) > var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case LES:
                var_set(program[i + 1],
                var_get(program[i + 2]) < var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case GTE:
                var_set(program[i + 1],
                var_get(program[i + 2]) >= var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case LTE:
                var_set(program[i + 1],
                var_get(program[i + 2]) <= var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case AND:
                var_set(program[i + 1],
                var_get(program[i + 2]) && var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case ORR:
                var_set(program[i + 1],
                var_get(program[i + 2]) || var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case NOT:
                var_set(program[i + 1], !var_get(program[i + 2]));
                JUMP(i + 3);
            continue;
            case BAN:
                var_set(program[i + 1],
                var_get(program[i + 2]) & var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case BOR:
                var_set(program[i + 1],
                var_get(program[i + 2]) | var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case BXO:
                var_set(program[i + 1],
                var_get(program[i + 2]) ^ var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case BNO:
                var_set(program[i + 1], ~var_get(program[i + 2]));
                JUMP(i + 3);
            continue;
            case LSH:
                var_set(program[i + 1],
                var_get(program[i + 2]) << var_get(program[i + 3]));
                JUMP(i + 4);
            continue;
            case RSH:
                var_set(program[i + 1],
                var_get(program[i + 2]) >> var_get(program[i + 3]));
                JUMP(i + 4);
            continue;

            case END: printf("PROGRAM > END\n");           return 0;
            default:  REPORT_ERROR("Invalid instruction"); return 1;
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


