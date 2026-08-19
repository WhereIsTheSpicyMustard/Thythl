#ifndef THYTHL_H
#define THYTHL_H

#include <stddef.h>

// potential additions that avoid hash collisions:
// DAL, UAL, UNA, DST, CLS, RET

typedef enum Instruction {
    ALO,  // allocate
    DAL,  // de-allocate
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
    SET,
} Instruction;

typedef struct Var {
    int* data;
    size_t length; // in bytes
} Var;

int program_start(int* prog, const size_t size);
int program_run(void);
void program_end(void);

#endif



