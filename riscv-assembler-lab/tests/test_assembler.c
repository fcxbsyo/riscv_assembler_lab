#include "../src/assembler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

static void write_asm(const char *path, const char *src)
{
    FILE *f = fopen(path, "w");
    fputs(src, f);
    fclose(f);
}

static int read_hex(const char *path, unsigned *words, int max)
{
    FILE *f = fopen(path, "r");
    if (!f)
        return -1;
    int n = 0;
    while (n < max && fscanf(f, "%X", &words[n]) == 1)
        n++;
    fclose(f);
    return n;
}

int main(void)
{
    printf("=== test_assembler ===\n");

    unsigned words[64];
    int n;

    /* ── Test 1: single addi ── */
    write_asm("/tmp/t1.s", "addi x1, x0, 5\n");
    CHECK(assemble_file("/tmp/t1.s", "/tmp/t1.hex") == 0, "T1: assemble ok");
    n = read_hex("/tmp/t1.hex", words, 64);
    CHECK(n == 1, "T1: 1 instruction");
    CHECK(words[0] == 0x00500093, "T1: addi x1,x0,5 = 0x00500093");

    /* ── Test 2: label + branch ── */
    write_asm("/tmp/t2.s",
              "addi x1, x0, 5\n"
              "addi x2, x0, 5\n"
              "loop: beq x1, x2, done\n"
              "addi x1, x1, -1\n"
              "jal  x0, loop\n"
              "done:\n");
    CHECK(assemble_file("/tmp/t2.s", "/tmp/t2.hex") == 0, "T2: assemble ok");
    n = read_hex("/tmp/t2.hex", words, 64);
    CHECK(n == 5, "T2: 5 instructions");
    /* beq at PC=8, done at address=20 → offset=12 → 0x00208663 */
    CHECK(words[2] == 0x00208663, "T2: beq offset +12 = 0x00208663");

    /* ── Test 3: load/store ── */
    write_asm("/tmp/t3.s",
              "addi x2, x0, 100\n" /* sp = 100 */
              "addi x5, x0, 42\n"
              "sw   x5, 0(x2)\n"
              "lw   x6, 0(x2)\n");
    CHECK(assemble_file("/tmp/t3.s", "/tmp/t3.hex") == 0, "T3: assemble ok");
    n = read_hex("/tmp/t3.hex", words, 64);
    CHECK(n == 4, "T3: 4 instructions");
    CHECK(words[2] == 0x00512023, "T3: sw x5,0(x2)");
    CHECK(words[3] == 0x00012303, "T3: lw x6,0(x2)");

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}