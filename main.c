#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define TOK_COUNT 29
#define HASH_SIZE 128

#include "parser.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Error: %s\n", argv[0]);
        return 1;
    }

    size_t size = 0;
    int* parsed_file = parse_file(argv[1], &size);



    free(parsed_file);
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

