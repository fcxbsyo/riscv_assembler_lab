#include "../src/parser.h"
#include <stdio.h>
#include <string.h>

static int passed = 0, failed = 0;

#define CHECK(expr, desc)                \
    do                                   \
    {                                    \
        if (expr)                        \
        {                                \
            printf("[PASS] %s\n", desc); \
            passed++;                    \
        }                                \
        else                             \
        {                                \
            printf("[FAIL] %s\n", desc); \
            failed++;                    \
        }                                \
    } while (0)

int main(void)
{
    printf("=== test_parser ===\n");
    ParsedLine pl;

    /* Simple R-type */
    parse_line("add x1, x2, x3", &pl);
    CHECK(strcmp(pl.mnemonic, "add") == 0, "mnemonic: add");
    CHECK(pl.num_operands == 3, "3 operands");
    CHECK(strcmp(pl.operands[0], "x1") == 0, "op0: x1");
    CHECK(strcmp(pl.operands[2], "x3") == 0, "op2: x3");

    /* Label + instruction */
    parse_line("loop: beq x1, x2, done", &pl);
    CHECK(strcmp(pl.label, "loop") == 0, "label: loop");
    CHECK(strcmp(pl.mnemonic, "beq") == 0, "mnemonic: beq");
    CHECK(pl.num_operands == 3, "3 operands");

    /* Label-only line */
    parse_line("start:", &pl);
    CHECK(strcmp(pl.label, "start") == 0, "label-only: start");
    CHECK(pl.mnemonic[0] == '\0', "no mnemonic on label-only");

    /* Comment stripping */
    parse_line("addi x1, x0, 5  # set x1=5", &pl);
    CHECK(strcmp(pl.mnemonic, "addi") == 0, "comment stripped");
    CHECK(pl.num_operands == 3, "3 operands after strip");

    /* Blank line */
    parse_line("   ", &pl);
    CHECK(pl.mnemonic[0] == '\0', "blank line → no mnemonic");

    /* Memory operand */
    parse_line("lw x5, 8(x2)", &pl);
    CHECK(strcmp(pl.mnemonic, "lw") == 0, "lw mnemonic");
    CHECK(strcmp(pl.operands[1], "8(x2)") == 0, "mem operand 8(x2)");

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}