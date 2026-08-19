#include "parser.h"

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "file.h"

#define REPORT_ERROR(x) do {fprintf(stderr, "%s | %s | %d\n", x, __FILE__, __LINE__);} while (0)

#define CHAR_DIFF 32
#define MAX_TOK_LEN 10
#define HASH_SIZE 128

// hash of token -> enum value
static const int hash_table[HASH_SIZE] = {
    [  0] = 17, [  2] = 2,  [  8] = 14, [ 10] = 24, [ 19] = 16,
    [ 20] = 6,  [ 23] = 12, [ 31] = 13, [ 38] = 11, [ 39] = 27,
    [ 44] = 26, [ 46] = 7,  [ 47] = 1,  [ 49] = 20, [ 50] = 25,
    [ 57] = 18, [ 60] = 15, [ 65] = 10, [ 66] = 5,  [ 68] = 19,
    [ 70] = 28, [ 92] = 21, [ 97] = 4,  [ 99] = 3,  [108] = 9,
    [118] = 22, [122] = 0,  [124] = 8,  [125] = 23,
};

// hash of token -> token string
static const char* const token_table[HASH_SIZE] = {
    [  0] = "GTE", [  2] = "PRI", [  8] = "CMP", [ 10] = "BXO",
    [ 19] = "LES", [ 20] = "JIF", [ 23] = "DIV", [ 31] = "MOD",
    [ 38] = "MUL", [ 39] = "RSH", [ 44] = "LSH", [ 46] = "JNO",
    [ 47] = "EXE", [ 49] = "ORR", [ 50] = "BNO", [ 57] = "LTE",
    [ 60] = "MOR", [ 65] = "SUB", [ 66] = "JMP", [ 68] = "AND",
    [ 70] = "END", [ 92] = "NOT", [ 97] = "PRS", [ 99] = "PRC",
    [108] = "ADD", [118] = "BAN", [122] = "ALO", [124] = "MOV",
    [125] = "BOR",
};

// token string -> hash of token
static int hash_function(const char key[3])
{
    const uint64_t a = (uint8_t)key[0];
    const uint64_t b = (uint8_t)key[1];
    const uint64_t c = (uint8_t)key[2];

    uint64_t hash = (0xca4b195ebe20bb27UL +
    ((a << 13) +    0xc85fd24c21bf27d9UL)) ^
    ((b << 23) +    0xb8016a2e00bed809UL) ^
    ((c << 25) +    0xc2a16d719235e88fUL);

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

// compares 3 char string
static int str_is_equal(const char* const a, const char* const b)
{
    for (int i = 0; i < 3; ++i) {
        if (a[i] != b[i]) return 0;
    }
    return 1;
}

static int parse_instruction(const char* const token)
{
    const int hash = hash_function(token);
    if (!str_is_equal(token_table[hash], token)) {
        REPORT_ERROR("Invalid token");
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
             ++tok_len;
             continue;
        }

        if (tok_len == 0) continue;


        // ==========================
        // TEST =====================
        char ptoken[MAX_TOK_LEN + 1];
        for (size_t j = 0; j < tok_len; ++j) {
            ptoken[j] = token[j];
        }
        ptoken[tok_len] = '\0';
        printf("%s\n", ptoken);
        // TEST =====================
        // ==========================



        const int value = parse_int(token, tok_len);
        if (parsed_count >= tok_count) {
            REPORT_ERROR("Out of bounds");
            goto cleanup;
        }
        if (value < 0) { // not an integer
            if (tok_len != 3) {
                REPORT_ERROR("Invalid token");
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

cleanup:
    free(file);
    return parsed;
}
