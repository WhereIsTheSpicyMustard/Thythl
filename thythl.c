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

    if (var_pool[index].data == NULL) {
        REPORT_ERROR("Uninitialized memory read");
        return 0;
    }

    return *(var_pool[index].data);
}

static int var_get_at(const int var_index, size_t data_index)
{
    if (var_index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }

    if (var_pool[var_index].data == NULL) {
        REPORT_ERROR("Uninitialized memory read");
        return 0;
    }

    if (data_index >= var_pool[var_index].length) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    return var_pool[var_index].data[data_index];
}

static int* var_get_data(const int index)
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return NULL;
    }
    return var_pool[index].data;
}

static size_t var_get_length(const int index)
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    return var_pool[index].length;
}

static int var_set(const int index, const int value)
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    if (var_pool[index].data == NULL) return 1;
    // printf("Edit var[%d]: %d -> %d\n", index, *(var_pool[index].data), value);
    *(var_pool[index].data) = value;
    return 0;
}

// returns the index in vars, negative on error
// maybe replace with hashmap instead of linear search
static int var_alloc(const int index, const size_t size) // size is number of ints
{
    if (index >= MAX_VARS) {
        REPORT_ERROR("Array out of bounds");
        return 0;
    }
    if (var_pool[index].data != NULL) return 1;
    var_pool[index].data = calloc(size, sizeof(int));
    if (var_pool[index].data == NULL) return 1;
    var_pool[index].length = size;
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
}

static int program_resize(void)
{
    program_length = (program_length * 2) + 1; // ensures the size cannot be 0
    int* new_program = realloc(program, program_length);
    if (new_program == NULL) return 1;
    program = new_program;

    return 0;
}

static int program_execute(int* prog, size_t size)
{
    for (int i = 0; (size_t)i < size; ++i) {
        printf("PROGRAM > %d\n", i);
        switch (prog[i]) {
        case ALO: // index, size
            if (var_alloc(prog[i + 1], (size_t)prog[i + 2])) {
                REPORT_ERROR("Error alocating var");
                return 1;
            }
            JUMP(i + 3);
        continue;
        case DAL: // index
            var_free(prog[i + 1]);
            JUMP(i + 2);
        continue;
        case EXE: // index
            if (program_execute(
                var_get_data(prog[i + 1]),
                var_get_length(prog[i + 1]))
            ) return 1;
            JUMP(i + 2);
        continue;
        case PRI: // index
            printf("    PRINT I > %d\n", var_get(prog[i + 1]));
            JUMP(i + 2);
        continue;
        case PRC: // index
            printf("    PRINT C > %c\n", (char)var_get(prog[i + 1]));
            JUMP(i + 2);
        continue;
        case PRS: // index
            printf("    PRINT S > ");
            for (size_t j = 0; j < var_get_length(prog[i + 1]); ++j)
                printf("%c", (char)var_get_at(prog[i + 1], j));
            printf("\n");
            JUMP(i + 2);
        continue;
        case JMP: // goto [i + 1]
            JUMP(prog[i + 1]);
        continue;
        case JIF: // if [i + 1] != 0 {goto [i + 2]} else {goto i + 3}
            if (var_get(prog[i + 1])) {
                JUMP(prog[i + 2]);
            } else {
                JUMP(i + 3);
            }
        continue;
        case SET:
            var_set(prog[i + 1], prog[i + 2]);
            JUMP(i + 3);
        continue;
        case MOV:
            var_set(prog[i + 1], var_get(prog[i + 2]));
            JUMP(i + 3);
        continue;
        case ADD:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) + var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case SUB:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) - var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case MUL:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) * var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case DIV:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) / var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case MOD:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) % var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case CMP:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) == var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case MOR:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) > var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case LES:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) < var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case GTE:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) >= var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case LTE:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) <= var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case AND:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) && var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case ORR:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) || var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case NOT:
            var_set(prog[i + 1], !var_get(prog[i + 2]));
            JUMP(i + 3);
        continue;
        case BAN:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) & var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case BOR:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) | var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case BXO:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) ^ var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case BNO:
            var_set(prog[i + 1], ~var_get(prog[i + 2]));
            JUMP(i + 3);
        continue;
        case LSH:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) << var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;
        case RSH:
            var_set(prog[i + 1],
            var_get(prog[i + 2]) >> var_get(prog[i + 3]));
            JUMP(i + 4);
        continue;

        case END: printf("PROGRAM > END\n");           return 0;
        default:
            REPORT_ERRORI("Invalid instruction on line: ", i);
            return 1;
        }
    }

    REPORT_ERROR("No END instruction");
    return 1;
}

int program_start(int* prog, const size_t size)
{
    program = malloc(size * (sizeof *program));
    if (program == NULL) return 1;
    program_length = size;

    memcpy(program, prog, size * (sizeof *program));

    var_pool = calloc(MAX_VARS, (sizeof(*var_pool)));
    if (var_pool == NULL) {
        program_end();
        return 1;
    }

    return 0;
}

int program_run(void)
{
    return program_execute(program, program_length);
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


