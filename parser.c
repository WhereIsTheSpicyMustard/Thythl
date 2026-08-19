#include "parser.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "file.h"

#define REPORT_ERROR(x) do {fprintf(stderr, "%s | %s | %d\n", x, __FILE__, __LINE__);} while (0)

#define CHAR_DIFF 32
#define MAX_TOK_LEN 10
#define HASH_SIZE 127

// hash of token -> enum value
static const int hash_table[HASH_SIZE] = {
    [  0] = 18, [  2] =  3, [  8] = 15, [ 10] = 25, [ 19] = 17, [ 20] =  7,
    [ 23] = 13, [ 24] =  1, [ 31] = 14, [ 38] = 12, [ 39] = 28, [ 44] = 27,
    [ 46] =  8, [ 47] =  2, [ 49] = 21, [ 50] = 26, [ 57] = 19, [ 60] = 16,
    [ 65] = 11, [ 66] =  6, [ 68] = 20, [ 70] = 29, [ 92] = 22, [ 95] = 30,
    [ 97] =  5, [ 99] =  4, [108] = 10, [118] = 23, [122] =  0, [124] =  9,
    [125] = 24
};

// hash of token -> token string
static const char* const token_table[HASH_SIZE] = {
    [  0] = "GTE",[  2] = "PRI",[  8] = "CMP",[ 10] = "BXO",
    [ 19] = "LES",[ 20] = "JIF",[ 23] = "DIV",[ 24] = "DAL",
    [ 31] = "MOD",[ 38] = "MUL",[ 39] = "RSH",[ 44] = "LSH",
    [ 46] = "JNO",[ 47] = "EXE",[ 49] = "ORR",[ 50] = "BNO",
    [ 57] = "LTE",[ 60] = "MOR",[ 65] = "SUB",[ 66] = "JMP",
    [ 68] = "AND",[ 70] = "END",[ 92] = "NOT",[ 95] = "SET",
    [ 97] = "PRS",[ 99] = "PRC",[108] = "ADD",[118] = "BAN",
    [122] = "ALO",[124] = "MOV",[125] = "BOR",
};

static inline void print_token(char token[3])
{
    printf("%c%c%c\n", token[0], token[1], token[2]);
}

// token string -> hash of token
static int hash_function(const uint8_t* const key)
{
    const uint64_t hash =
    (((uint64_t)key[0] << 13) + 0x92aaebaadfdfe300UL) ^
    (((uint64_t)key[1] << 23) + 0xb8016a2e00bed809UL) ^
    (((uint64_t)key[2] << 25) + 0xc2a16d719235e88fUL);

    return (int)(hash % 127);
}

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

static int parse_int(const char* const string, const size_t length)
{
    uint64_t val = 0;
    for (size_t i = 0; i < length; ++i) {
        if (string[i] >= '0' && string[i] <= '9') {
            val = val * 10 + (uint64_t)(string[i] - '0');
            if (val > INT32_MAX) {
                REPORT_ERROR("Integer overflow");
                return -1;
            }
            continue;
        }
        return -1;
    }

    return (int)val;
}

// compares 3 char string
static int str_is_equal(const char* const a, const char* const b)
{
    for (int i = 0; i < 3; ++i) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

static int parse_instruction(char* token)
{
    const int hash = hash_function((uint8_t*)token);
    if (!str_is_equal(token_table[hash], token)) {
        REPORT_ERROR("Invalid token");
        print_token(token);
        return -1;
    }
    return hash_table[hash];
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
             if (token[tok_len] <= 'z' && token[tok_len] >= 'a') {
                 token[tok_len] -= CHAR_DIFF; // convert to uppercase
             }
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
            if (tok_len != 3) {
                REPORT_ERROR("Invalid token");
                print_token(token);
                goto cleanup;
            }
            parsed[parsed_count] = parse_instruction(token);
            if (parsed[parsed_count] < 0) goto cleanup;
            ++parsed_count;
        } else {
            parsed[parsed_count++] = value;
        }

        tok_len = 0;
    }
    /**********************************************/

    *size = tok_count;
    free(file);
    return parsed;
cleanup:
    free(file);
    free(parsed);
    return NULL;
}
