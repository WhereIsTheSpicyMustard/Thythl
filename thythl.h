#ifndef THYTHL_H
#define THYTHL_H

#include <stddef.h>

typedef unsigned int uint;

typedef enum Type {
    NONE = 0,
    INT,
    STRING,
    FUNC,
} Type;

typedef enum Instruction {
    JMP,  // goto
    JIF,  // JMP if x != 0
    JNO,  // JMP if x == 0
    JEQ,  // JMP if ==
    JNE,  // JMP if !=
    JGT,  // JMP if >
    JLT,  // JMP if <
    JGE,  // JMP if >=
    JLE,  // JMP if <=
    JAN,  // JMP if &&
    JOR,  // JMP if ||
    SET,  // =
    ADD,  // +
    SUB,  // -
    MUL,  // *
    DIV,  // /
    MOD,  // %
    CMP,  // ==
    MOR,  // >
    LES,  // <
    GTE,  // >=
    LTE,  // <=
    AND,  // &&
    ORR,  // ||
    NOT,  // !
    BAN,  // &
    BOR,  // |
    BXO,  // ^
    BNO,  // ~
    LSH,  // <<
    RSH,  // >>
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
