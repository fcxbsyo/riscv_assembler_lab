# test_case_1.s  — 22 instructions, broad RV32I coverage
# Computes sum of integers 1..10 and stores result in memory

        addi  x2,  x0,  200    # stack pointer = 200
        addi  x1,  x0,  10     # x1 = 10  (loop counter / N)
        addi  x3,  x0,  0      # x3 = 0   (accumulator)
loop:
        add   x3,  x3,  x1     # sum += i
        addi  x1,  x1,  -1     # i--
        bne   x1,  x0,  loop   # if i != 0, repeat
        sw    x3,  0(x2)       # store result
        lw    x4,  0(x2)       # reload result
        addi  x5,  x0,  55     # expected value = 55
        beq   x4,  x5,  pass   # branch if correct
        addi  x6,  x0,  -1     # error code
        jal   x0,  done
pass:
        addi  x6,  x0,  1      # success code
done:
        xor   x7,  x4,  x5     # should be 0 if correct
        slt   x8,  x0,  x6     # x8 = 1 if x6 > 0
        and   x9,  x6,  x8     # x9 = x6 & x8
        or    x10, x6,  x7     # x10 = x6 | x7
        sltu  x11, x0,  x5     # x11 = 1 (x5 > 0 unsigned)
        sh    x6,  4(x2)       # store halfword
        lh    x12, 4(x2)       # load halfword
        sb    x6,  8(x2)       # store byte
        lb    x13, 8(x2)       # load byte