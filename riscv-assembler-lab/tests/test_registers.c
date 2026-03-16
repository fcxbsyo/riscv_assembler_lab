#include "../src/registers.h"
#include <stdio.h>
#include <assert.h>

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
    printf("=== test_registers ===\n");

    /* x-notation */
    CHECK(parse_register("x0") == 0, "x0  → 0");
    CHECK(parse_register("x1") == 1, "x1  → 1");
    CHECK(parse_register("x5") == 5, "x5  → 5");
    CHECK(parse_register("x31") == 31, "x31 → 31");
    CHECK(parse_register("x10") == 10, "x10 → 10");

    /* ABI names */
    CHECK(parse_register("zero") == 0, "zero → 0");
    CHECK(parse_register("ra") == 1, "ra   → 1");
    CHECK(parse_register("sp") == 2, "sp   → 2");
    CHECK(parse_register("a0") == 10, "a0   → 10");
    CHECK(parse_register("t0") == 5, "t0   → 5");
    CHECK(parse_register("s0") == 8, "s0   → 8");
    CHECK(parse_register("fp") == 8, "fp   → 8 (alias s0)");

    /* Invalid */
    CHECK(parse_register("x32") == -1, "x32 → -1 (out of range)");
    CHECK(parse_register("x") == -1, "x   → -1 (no number)");
    CHECK(parse_register("") == -1, "''  → -1 (empty)");
    CHECK(parse_register(NULL) == -1, "NULL → -1");
    CHECK(parse_register("foo") == -1, "foo → -1 (unknown)");

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}