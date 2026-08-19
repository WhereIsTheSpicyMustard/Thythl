#include <stdio.h>

#include "thythl.h"

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    if (program_start()) {
        printf("Error start\n");
        return 1;
    }
    if (program_run()) {
        printf("Error run\n");
        return 1;
    }
    program_end();

    return 0;
}
