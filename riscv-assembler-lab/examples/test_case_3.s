# test_case_3.s  — 122 instructions
# Comprehensive RV32I stress-test:
#   1. Fibonacci (iterative)
#   2. GCD (Euclidean)
#   3. Byte array checksum
#   4. Shift / bitwise arithmetic cross-checks

        addi  x2,  x0,  1000   # stack / scratch base

# ════════════════════════════════════════════════════
# Part 1 — Fibonacci: compute fib(10) = 55
# ════════════════════════════════════════════════════
        addi  x1,  x0,  0      # fib(0) = 0
        addi  x3,  x0,  1      # fib(1) = 1
        addi  x4,  x0,  10     # n = 10
        addi  x5,  x0,  2      # i = 2
fib_loop:
        add   x6,  x1,  x3     # f_new = f0 + f1
        addi  x1,  x3,  0      # f0 = f1
        addi  x3,  x6,  0      # f1 = f_new
        addi  x5,  x5,  1      # i++
        blt   x5,  x4,  fib_loop
        add   x5,  x4,  x0     # final extra step to reach fib(10)
        add   x6,  x1,  x3
        addi  x1,  x3,  0
        addi  x3,  x6,  0
        sw    x3,   0(x2)      # store fib(10)
        addi  x7,  x0,  55
        beq   x3,  x7,  fib_ok
        addi  x8,  x0,  -1     # fib error
        jal   x0,  gcd_start
fib_ok:
        addi  x8,  x0,  1      # fib ok

# ════════════════════════════════════════════════════
# Part 2 — GCD(48, 18) = 6
# ════════════════════════════════════════════════════
gcd_start:
        addi  x9,  x0,  48     # a = 48
        addi  x10, x0,  18     # b = 18
gcd_loop:
        beq   x10, x0,  gcd_done
        # r = a mod b  (a = a - b while a >= b)
        addi  x11, x9,  0      # r = a
mod_loop:
        blt   x11, x10, mod_done
        sub   x11, x11, x10
        jal   x0,  mod_loop
mod_done:
        addi  x9,  x10, 0      # a = b
        addi  x10, x11, 0      # b = r
        jal   x0,  gcd_loop
gcd_done:
        sw    x9,   4(x2)      # store gcd result
        addi  x12, x0,  6
        beq   x9,  x12, gcd_ok
        addi  x13, x0,  -1
        jal   x0,  csum_start
gcd_ok:
        addi  x13, x0,  1

# ════════════════════════════════════════════════════
# Part 3 — byte checksum over 8 bytes stored at 200
# Values: 1,2,3,4,5,6,7,8  → sum = 36
# ════════════════════════════════════════════════════
csum_start:
        addi  x14, x0,  200    # byte array base
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

        addi  x16, x0,  0      # sum = 0
        addi  x17, x0,  0      # i = 0
        addi  x18, x14, 0      # ptr
        addi  x19, x0,  8      # count
csum_loop:
        lbu   x20, 0(x18)      # load unsigned byte
        add   x16, x16, x20
        addi  x17, x17, 1
        addi  x18, x18, 1
        blt   x17, x19, csum_loop
        sw    x16,  8(x2)      # store checksum
        addi  x21, x0,  36
        beq   x16, x21, csum_ok
        addi  x22, x0,  -1
        jal   x0,  bit_start
csum_ok:
        addi  x22, x0,  1

# ════════════════════════════════════════════════════
# Part 4 — shift and bitwise cross-checks
# ════════════════════════════════════════════════════
bit_start:
        addi  x23, x0,  12     # 0b00001100
        slli  x24, x23, 2      # << 2 = 48
        srli  x25, x24, 2      # >> 2 = 12  (logical)
        srai  x26, x24, 2      # >> 2 = 12  (arithmetic)
        beq   x25, x26, shift_ok
        addi  x27, x0,  -1
        jal   x0,  bit2
shift_ok:
        addi  x27, x0,  1
bit2:
        addi  x28, x0,  0xFF
        andi  x29, x28, 0x0F   # 0x0F
        ori   x29, x29, 0xF0   # 0xFF
        xori  x29, x29, 0xFF   # 0x00
        beq   x29, x0,  bit3
        addi  x30, x0,  -1
        jal   x0,  bit4
bit3:
        addi  x30, x0,  1
bit4:
        # slt / sltu checks
        addi  x1,  x0,  5
        addi  x3,  x0,  10
        slt   x4,  x1,  x3     # 1 (5 < 10)
        slt   x5,  x3,  x1     # 0
        sltu  x6,  x1,  x3     # 1
        slti  x7,  x1,  10     # 1
        sltiu x8,  x1,  3      # 0
        and   x9,  x4,  x6     # 1
        or    x10, x4,  x5     # 1
        xor   x11, x4,  x5     # 1
        sub   x12, x3,  x1     # 5
        # store summary flags
        sw    x9,  12(x2)
        sw    x10, 16(x2)
        sw    x11, 20(x2)
        sw    x12, 24(x2)
        # load back and verify
        lw    x13, 12(x2)
        lw    x14, 16(x2)
        lh    x15, 20(x2)
        lhu   x16, 24(x2)
        lb    x17, 24(x2)
        lbu   x18, 24(x2)
        # Final jalr self-loop escape
        addi  x19, x0,  4
        jalr  x0,  x19, 0
end:
        jal   x0,  end