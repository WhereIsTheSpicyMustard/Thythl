#ifndef THYTHL_H
#define THYTHL_H

#include <stdint.h>
#include <stddef.h>

typedef enum Type {
    INT,
    FLOAT,
    STRING,
    LIST,
    FUNC,
} Type;

typedef enum Inst {

} Inst;

typedef struct Instruction {

} Instruction;

typedef struct Var {
    void* data;
    Type type;
} Var;

typedef struct Int {
    uint64_t data;
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



#endif
