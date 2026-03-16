#include "../src/symbols.h"
#include <stdio.h>

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
    printf("=== test_symbols ===\n");
    SymbolTable tbl;
    symbols_init(&tbl);

    CHECK(tbl.count == 0, "init: count=0");

    CHECK(symbols_add(&tbl, "start", 0) == 0, "add 'start' → ok");
    CHECK(symbols_add(&tbl, "loop", 12) == 0, "add 'loop' → ok");
    CHECK(symbols_add(&tbl, "done", 24) == 0, "add 'done' → ok");

    CHECK(symbols_find(&tbl, "start") == 0, "find 'start' → 0");
    CHECK(symbols_find(&tbl, "loop") == 12, "find 'loop'  → 12");
    CHECK(symbols_find(&tbl, "done") == 24, "find 'done'  → 24");

    CHECK(symbols_find(&tbl, "missing") == -1, "missing → -1");

    /* Duplicate detection */
    CHECK(symbols_add(&tbl, "loop", 99) == -1, "duplicate 'loop' → -1");
    /* Count must not have grown */
    CHECK(tbl.count == 3, "count still 3 after dup");

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}