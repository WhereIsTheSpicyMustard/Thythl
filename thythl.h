#ifndef THYTHL_H
#define THYTHL_H

#include <stddef.h>

typedef enum Instruction {
    ALO,  // allocate
    EXE,  // execute
    PRI,  // print as int
    PRC,  // print as char
    PRS,  // print as string
    JMP,  // goto
    JIF,  // JMP if x != 0
    JNO,  // JMP if x == 0
    MOV,  // =
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
} Var;

int program_start(void);
int program_run(void);
void program_end(void);

#endif



