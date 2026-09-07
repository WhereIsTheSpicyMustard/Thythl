#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "parser.h"
#include "thythl.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Error: %s\n", argv[0]);
        return 1;
    }

    size_t size;
    int* parsed_file = parse_file(argv[1], &size);
    if (parsed_file == NULL) {
        printf("Error parsing file\n");
        return 1;
    }

    if (program_start(parsed_file, size)) {
        goto cleanup;
    }

    if (program_run()) {
        goto cleanup;
    }
cleanup:
    program_end();
    free(parsed_file);
    return 0;
}

