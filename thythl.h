#ifndef THYTHL_H
#define THYTHL_H

#include <stddef.h>

typedef unsigned int uint;

typedef enum Type {
    NONE = 0,
    BOOL,
    INT,
    FLOAT,
    STRING,
    LIST,
    FUNC,
} Type;

typedef enum Instruction {
    GOTO, // jump
    GOIF, // if

    END,
} Instruction;

typedef struct Var {
    int* data;
    size_t length; // in bytes
    Type type;
} Var;

int program_start(void);
int program_run(void);
void program_end(void);

#endif
