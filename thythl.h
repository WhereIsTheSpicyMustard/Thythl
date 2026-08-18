#ifndef THYTHL_H
#define THYTHL_H

#include <stddef.h>

typedef unsigned int uint;

typedef enum Type {
    BOOL,
    INT,
    FLOAT,
    STRING,
    LIST,
    FUNC,
} Type;

typedef enum Instruction {
    GOTO,
    GOIF,
    END,
} Instruction;

typedef struct Var {
    void* data;
    Type type;
} Var;

typedef struct Bool {
    char data;
} Bool;

typedef struct Int {
    long long data;
} Int;

typedef struct Float {
    double data;
} Float;

typedef struct String {
    char* data;
    size_t length;
} String;

typedef struct List {
    Var* data;
    size_t length;
} List;

typedef struct Func {
    Instruction* data;
    size_t length;
} Func;

int program_start(void);
int program_resize(void);
int program_run(void);
void program_end(void);

#endif
