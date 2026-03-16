# test_case_3.s — 106 instructions
# Comprehensive RV32I stress-test:
#   1. Fibonacci (iterative)
#   2. GCD (Euclidean)
#   3. Byte array checksum
#   4. Shift / bitwise cross-checks
#
# Venus: Settings -> Memory Configuration -> "Compact, Data at Address 0"
# This lets stores/loads work at low addresses just like our assembler expects.

# ════════════════════════════════════════════════════
# Setup: x2 = 1000 (scratch base)
# ════════════════════════════════════════════════════
        addi  x2,  x0,  1000

# ════════════════════════════════════════════════════
# Part 1 — Fibonacci: fib(10) = 55
# ════════════════════════════════════════════════════
        addi  x1,  x0,  0
        addi  x3,  x0,  1
        addi  x4,  x0,  10
        addi  x5,  x0,  2
fib_loop:
        add   x6,  x1,  x3
        addi  x1,  x3,  0
        addi  x3,  x6,  0
        addi  x5,  x5,  1
        blt   x5,  x4,  fib_loop
        add   x5,  x4,  x0
        add   x6,  x1,  x3
        addi  x1,  x3,  0
        addi  x3,  x6,  0
        sw    x3,   0(x2)
        addi  x7,  x0,  55
        beq   x3,  x7,  fib_ok
        addi  x8,  x0,  -1
        jal   x0,  gcd_start
fib_ok:
        addi  x8,  x0,  1

# ════════════════════════════════════════════════════
# Part 2 — GCD(48, 18) = 6
# ════════════════════════════════════════════════════
gcd_start:
        addi  x9,  x0,  48
        addi  x10, x0,  18
gcd_loop:
        beq   x10, x0,  gcd_done
        addi  x11, x9,  0
mod_loop:
        blt   x11, x10, mod_done
        sub   x11, x11, x10
        jal   x0,  mod_loop
mod_done:
        addi  x9,  x10, 0
        addi  x10, x11, 0
        jal   x0,  gcd_loop
gcd_done:
        sw    x9,   4(x2)
        addi  x12, x0,  6
        beq   x9,  x12, gcd_ok
        addi  x13, x0,  -1
        jal   x0,  csum_start
gcd_ok:
        addi  x13, x0,  1

# ════════════════════════════════════════════════════
# Part 3 — Byte checksum: 1+2+...+8 = 36
# Stored at x2+100 = address 1100
# ════════════════════════════════════════════════════
csum_start:
        addi  x14, x2,  100
        addi  x15, x0,  1
        sb    x15,  0(x14)
        addi  x15, x0,  2
        sb    x15,  1(x14)
        addi  x15, x0,  3
        sb    x15,  2(x14)
        addi  x15, x0,  4
        sb    x15,  3(x14)
        addi  x15, x0,  5
        sb    x15,  4(x14)
        addi  x15, x0,  6
        sb    x15,  5(x14)
        addi  x15, x0,  7
        sb    x15,  6(x14)
        addi  x15, x0,  8
        sb    x15,  7(x14)
        addi  x16, x0,  0
        addi  x17, x0,  0
        addi  x18, x14, 0
        addi  x19, x0,  8
csum_loop:
        lbu   x20, 0(x18)
        add   x16, x16, x20
        addi  x17, x17, 1
        addi  x18, x18, 1
        blt   x17, x19, csum_loop
        sw    x16,  8(x2)
        addi  x21, x0,  36
        beq   x16, x21, csum_ok
        addi  x22, x0,  -1
        jal   x0,  bit_start
csum_ok:
        addi  x22, x0,  1

# ════════════════════════════════════════════════════
# Part 4 — Shift and bitwise cross-checks
# ════════════════════════════════════════════════════
bit_start:
        addi  x23, x0,  12
        slli  x24, x23, 2
        srli  x25, x24, 2
        srai  x26, x24, 2
        beq   x25, x26, shift_ok
        addi  x27, x0,  -1
        jal   x0,  bit2
shift_ok:
        addi  x27, x0,  1
bit2:
        addi  x28, x0,  255
        andi  x29, x28, 15
        ori   x29, x29, 240
        xori  x29, x29, 255
        beq   x29, x0,  bit3
        addi  x30, x0,  -1
        jal   x0,  bit4
bit3:
        addi  x30, x0,  1
bit4:
        addi  x1,  x0,  5
        addi  x3,  x0,  10
        slt   x4,  x1,  x3
        slt   x5,  x3,  x1
        sltu  x6,  x1,  x3
        slti  x7,  x1,  10
        sltiu x8,  x1,  3
        and   x9,  x4,  x6
        or    x10, x4,  x5
        xor   x11, x4,  x5
        sub   x12, x3,  x1
        sw    x9,  12(x2)
        sw    x10, 16(x2)
        sw    x11, 20(x2)
        sw    x12, 24(x2)
        lw    x13, 12(x2)
        lw    x14, 16(x2)
        lh    x15, 20(x2)
        lhu   x16, 24(x2)
        lb    x17, 24(x2)
        lbu   x18, 24(x2)
        addi  x19, x0,  4
        jalr  x0,  x19, 0
end:
        jal   x0,  end