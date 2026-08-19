#include "parser.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "file.h"

#define REPORT_ERROR(x) do {fprintf(stderr, "%s | %s | %d\n", x, __FILE__, __LINE__);} while (0)

#define CHAR_DIFF 32
#define MAX_TOK_LEN 10

static const int hash_table[256] = {
    [113] =  0, [170] =  1, [ 49] =  2, [ 55] =  3,
    [ 39] =  4, [229] =  5, [231] =  6, [101] =  7,
    [243] =  8, [107] =  9, [106] = 10, [250] = 11,
    [249] = 12, [105] = 13, [ 80] = 14, [225] = 15,
    [237] = 16, [119] = 17, [181] = 18, [233] = 19,
    [124] = 20, [255] = 21, [ 10] = 22, [241] = 23,
    [182] = 24, [248] = 25, [202] = 26, [254] = 27,
    [172] = 28, [203] = 29, [191] = 30, [ 30] = 31,
    [132] = 32, [154] = 33, [ 76] = 34, [ 34] = 35,
    [ 53] = 36
};

static const char* token_table[256] = {
    [113] = "ALO", [170] = "EXE", [ 49] = "PRI", [ 55] = "PRC",
    [ 39] = "PRS", [229] = "JMP", [231] = "JIF", [101] = "JNO",
    [243] = "JEQ", [107] = "JNE", [106] = "JGT", [250] = "JLT",
    [249] = "JGE", [105] = "JLE", [ 80] = "JAN", [225] = "JOR",
    [237] = "MOV", [119] = "ADD", [181] = "SUB", [233] = "MUL",
    [124] = "DIV", [255] = "MOD", [ 10] = "CMP", [241] = "MOR",
    [182] = "LES", [248] = "GTE", [202] = "LTE", [254] = "AND",
    [172] = "ORR", [203] = "NOT", [191] = "BAN", [ 30] = "BOR",
    [132] = "BXO", [154] = "BNO", [ 76] = "LSH", [ 34] = "RSH",
    [ 53] = "END",
};


static inline int is_tok_char(const char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z');
}

static inline int is_valid_char(const char c)
{
    return (c >= '0' && c <= '9') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z') ||
           (c == ' ') || (c == '\n');
}

static int parse_int(const char* string, const size_t length)
{
    int val = 0;
    for (size_t i = 0; i < length; ++i) {
        if (string[i] >= '0' && string[i] <= '9') {
            val = val * 10 + (string[i] - '0');
            continue;
        }
        return -1;
    }

    return val;
}

static int parse_instruction(const char* string, const size_t length)
{
    return 0;
}

int* parse_file(const char* filename, size_t* size)
{
    uint8_t* file = load_file(filename, size);
    size_t tok_count = 0;
    int* parsed = NULL;
    size_t parsed_count = 0;

    /**********************************************/
    // get tok count
    for (size_t i = 1; i < *size; ++i) {
        if (!is_valid_char((char)file[i])) {
            REPORT_ERROR("Invalid character");
            goto cleanup;
        }

        if (!is_tok_char((char)file[i]) && is_tok_char((char)file[i - 1]))
            ++tok_count;
    }
    /**********************************************/

    parsed = malloc(tok_count * (sizeof *parsed));
    if (parsed == NULL) {
        REPORT_ERROR("Failed allocation");
        goto cleanup;
    }

    /**********************************************/
    // parse tokens
    char token[MAX_TOK_LEN];
    size_t tok_len = 0;

    for (size_t i = 0; i < *size; ++i) {
         if (is_tok_char((char)file[i])) {
             if (tok_len >= MAX_TOK_LEN) {
                REPORT_ERROR("Long token");
                goto cleanup;
             }
             token[tok_len] = (char)file[i];
             ++tok_len;
             continue;
        }

        if (tok_len == 0) continue;

        const int value = parse_int(token, tok_len);
        if (parsed_count >= tok_count) {
            REPORT_ERROR("Out of bounds");
            goto cleanup;
        }
        if (value < 0) { // not an integer
            parsed[parsed_count++] = value;
        } else {
            parsed[parsed_count++] = parse_instruction(token, tok_len);
        }

        tok_len = 0;
    }
    /**********************************************/

cleanup:
    free(file);
    return parsed;
}
