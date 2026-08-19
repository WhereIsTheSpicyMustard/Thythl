#include <stdio.h>
#include <stdint.h>

#define TOK_COUNT 31

// #include "parser.h"

static char* tokens[TOK_COUNT] = {
    "ALO",
    "EXE",
    "PRI",
    "PRC",
    "PRS",
    "JMP",
    "JIF",
    "JNO",
    "JEQ",
    "JNE",
    "MOV",
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "CMP",
    "MOR",
    "LES",
    "GTE",
    "LTE",
    "AND",
    "ORR",
    "NOT",
    "BAN",
    "BOR",
    "BXO",
    "BNO",
    "LSH",
    "RSH",
    "END",
};

/*
// 256 all commands
static int hash_function(const char key[3])
{
    uint64_t hash = 0xa953ce2d300ac2b7UL;

    for (int i = 0; i < 3; ++i) {
        hash ^= ((uint64_t)key[i]) + 0xdf5267745fe8559dUL;
        hash ^= (hash >> 11) + 0xdf34cd14a4a8abdbUL;
        hash ^= (hash << 7) + 0xbd059a1c204ef59bUL;
    }

    return (int)(hash & 255);
}




// 128 ver
static int hash_function(const char key[3])
{
uint64_t hash = 0x8cecead31040d20fUL;

for (int i = 0; i < 3; ++i) {
    hash ^= ((uint64_t)key[i]) + 0xc8df051a2fda7a19UL;
    hash ^= ((3 * hash) << 5) ^ (((0x8cecead31040d20fUL * hash) >> 11) + 0xdef3894720645a97UL);
    hash ^= ((3 * hash) >> 3) ^ (((5 * hash) << 7) + 0x907df0938574231bUL);
    hash ^= (hash >> 11) + 0xfb170cd939c3b799UL;
    }

    return (int)(hash & 127);
    }

*/

static int hash_function(const char key[3])
{
    uint64_t hash = 0x8cecead31040d20fUL;

    for (int i = 0; i < 3; ++i) {
        hash ^= ((uint64_t)key[i]) + 0xe7fd1acc4b12a691UL;
        hash ^= (hash >> 11) + 0xabb907d73a33df51UL;
        hash ^= (hash >> 3) + 0x9b3184a6fe49a657UL;
        hash ^= (hash << 5) + 0x86a34435d50c10b1UL;
    }

    return (int)(hash & 127);
}

static int arr[256] = {0};

int main(int argc, char* argv[])
{
    if (argc != 1) {
        printf("%s\n", argv[0]);
        return 1;
    }


    for (int i = 0; i < 256; ++i) {
        arr[i] = -1;
    }

    for (int i = 0; i < TOK_COUNT; ++i) {
        int index = hash_function(tokens[i]);
        printf("%d\n", index);
        if (arr[index] != -1) {
            printf("duplicate\n");
        }
        arr[hash_function(tokens[i])] = i;
        // printf("[%3d] = \"%s\", ", hash_function(tokens[i]), tokens[i]);
        // if ((i + 1) % 4 == 0) printf("\n");
    }
    printf("\n");

    // size_t size = 0;
    // parse_file(argv[1], &size);

    // if (program_start()) {
    //     printf("Error start\n");
    //     return 1;
    // }
    // if (program_run()) {
    //     printf("Error run\n");
    //     return 1;
    // }
    // program_end();

    return 0;
}


/*


*/
