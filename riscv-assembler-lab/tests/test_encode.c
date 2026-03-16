#include "../src/encode.h"
#include <stdio.h>
#include <stdint.h>

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
    printf("=== test_encode ===\n");
    uint32_t w;

    /* ── R-type ──────────────────────────────────────────────────────── */
    /* add x1, x2, x3  → 0x003100B3 */
    w = encode_r(0x33, 0x0, 0x00, 1, 2, 3);
    printf("  add x1,x2,x3  = 0x%08X  (expect 0x003100B3)\n", w);
    CHECK(w == 0x003100B3, "encode_r: add x1,x2,x3");

    /* sub x5, x6, x7  → 0x407302B3 */
    w = encode_r(0x33, 0x0, 0x20, 5, 6, 7);
    printf("  sub x5,x6,x7  = 0x%08X  (expect 0x407302B3)\n", w);
    CHECK(w == 0x407302B3, "encode_r: sub x5,x6,x7");

    /* ── I-type ──────────────────────────────────────────────────────── */
    /* addi x1, x0, 5  → 0x00500093 */
    w = encode_i(0x13, 0x0, 1, 0, 5);
    printf("  addi x1,x0,5  = 0x%08X  (expect 0x00500093)\n", w);
    CHECK(w == 0x00500093, "encode_i: addi x1,x0,5");

    /* addi x1, x0, -1 → 0xFFF00093 */
    w = encode_i(0x13, 0x0, 1, 0, -1);
    printf("  addi x1,x0,-1 = 0x%08X  (expect 0xFFF00093)\n", w);
    CHECK(w == 0xFFF00093, "encode_i: addi x1,x0,-1");

    /* lw x5, 0(x2)    → 0x00012283 */
    w = encode_i(0x03, 0x2, 5, 2, 0);
    printf("  lw x5,0(x2)   = 0x%08X  (expect 0x00012283)\n", w);
    CHECK(w == 0x00012283, "encode_i: lw x5,0(x2)");

    /* ── S-type ──────────────────────────────────────────────────────── */
    /* sw x5, 0(x2)    → 0x00512023 */
    w = encode_s(0x23, 0x2, 2, 5, 0);
    printf("  sw x5,0(x2)   = 0x%08X  (expect 0x00512023)\n", w);
    CHECK(w == 0x00512023, "encode_s: sw x5,0(x2)");

    /* sw x5, 4(x2)    → 0x00512223 */
    w = encode_s(0x23, 0x2, 2, 5, 4);
    printf("  sw x5,4(x2)   = 0x%08X  (expect 0x00512223)\n", w);
    CHECK(w == 0x00512223, "encode_s: sw x5,4(x2)");

    /* ── B-type ──────────────────────────────────────────────────────── */
    /* beq x1, x2, +8  → offset=8 → 0x00208463 */
    w = encode_b(0x63, 0x0, 1, 2, 8);
    printf("  beq x1,x2,+8  = 0x%08X  (expect 0x00208463)\n", w);
    CHECK(w == 0x00208463, "encode_b: beq x1,x2,+8");

    /* ── J-type ──────────────────────────────────────────────────────── */
    /* jal x1, +4      → 0x004000EF */
    w = encode_j(0x6F, 1, 4);
    printf("  jal x1,+4     = 0x%08X  (expect 0x004000EF)\n", w);
    CHECK(w == 0x004000EF, "encode_j: jal x1,+4");

    printf("\nResults: %d passed, %d failed\n", passed, failed);
    return failed ? 1 : 0;
}