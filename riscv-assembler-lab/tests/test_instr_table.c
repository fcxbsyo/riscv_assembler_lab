#include "../src/instr_table.h"
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
    printf("=== test_instr_table ===\n");
    const InstrSpec *s;

    s = lookup_instr("add");
    CHECK(s != NULL, "add found");
    CHECK(s->fmt == FMT_R, "add: R-type");
    CHECK(s->opcode == 0x33, "add: opcode 0x33");
    CHECK(s->funct3 == 0x0, "add: funct3 0");
    CHECK(s->funct7 == 0x00, "add: funct7 0");

    s = lookup_instr("sub");
    CHECK(s && s->funct7 == 0x20, "sub: funct7 0x20");

    s = lookup_instr("addi");
    CHECK(s && s->fmt == FMT_I, "addi: I-type");
    CHECK(s && s->opcode == 0x13, "addi: opcode 0x13");

    s = lookup_instr("lw");
    CHECK(s && s->fmt == FMT_I, "lw: I-type");
    CHECK(s && s->opcode == 0x03, "lw: opcode 0x03");

    s = lookup_instr("sw");
    CHECK(s && s->fmt == FMT_S, "sw: S-type");

    s = lookup_instr("beq");
    CHECK(s && s->fmt == FMT_B, "beq: B-type");
    CHECK(s && s->opcode == 0x63, "beq: opcode 0x63");

    s = lookup_instr("jal");
    CHECK(s && s->fmt == FMT_J, "jal: J-type");

    s = lookup_instr("sra");
    CHECK(s && s->funct7 == 0x20, "sra: funct7 0x20");

    CHECK(lookup_instr("nop") == NULL, "nop → NULL (not in table)");
    CHECK(lookup_instr(NULL) == NULL, "NULL → NULL");

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}