# test_case_2.s  — 52 instructions
# Bubble-sort 8 integers stored in memory, then verify with linear search

        addi  x2,  x0,  400    # base address for array

        # Initialise 8 words: 8 7 6 5 4 3 2 1
        addi  x1,  x0,  8
        sw    x1,   0(x2)
        addi  x1,  x0,  7
        sw    x1,   4(x2)
        addi  x1,  x0,  6
        sw    x1,   8(x2)
        addi  x1,  x0,  5
        sw    x1,  12(x2)
        addi  x1,  x0,  4
        sw    x1,  16(x2)
        addi  x1,  x0,  3
        sw    x1,  20(x2)
        addi  x1,  x0,  2
        sw    x1,  24(x2)
        addi  x1,  x0,  1
        sw    x1,  28(x2)

        # Bubble sort: outer loop x10 = n-1 .. 1
        addi  x10, x0,  7      # outer count
outer:
        addi  x11, x0,  0      # inner index j = 0
        addi  x12, x2,  0      # pointer p = base
inner:
        lw    x13, 0(x12)      # a[j]
        lw    x14, 4(x12)      # a[j+1]
        bge   x13, x14, noswap # if a[j] <= a[j+1], skip
        # swap
        sw    x14, 0(x12)
        sw    x13, 4(x12)
noswap:
        addi  x11, x11, 1
        addi  x12, x12, 4
        blt   x11, x10, inner  # j < outer_count
        addi  x10, x10, -1
        bne   x10, x0,  outer

        # Verify: scan for value 5 (should be at index 4, offset 16)
        addi  x15, x0,  5      # search key
        addi  x16, x0,  0      # index
        addi  x17, x2,  0      # pointer
search:
        lw    x18, 0(x17)
        beq   x18, x15, found
        addi  x16, x16, 1
        addi  x17, x17, 4
        addi  x19, x0,  8
        blt   x16, x19, search
        addi  x20, x0,  -1     # not found
        jal   x0,  sdone
found:
        addi  x20, x0,  1      # found

        # Bitwise checks on result
sdone:
        andi  x21, x20, 1      # isolate bit 0
        ori   x22, x20, 0      # copy
        xori  x23, x20, 0      # copy
        slli  x24, x20, 2      # << 2
        srli  x25, x24, 2      # >> 2 (logical)
        srai  x26, x24, 2      # >> 2 (arithmetic)
        slti  x27, x20, 2      # x27 = 1 if x20 < 2
        sltiu x28, x20, 10     # x28 = 1 if x20 < 10 (unsigned)
        sub   x29, x25, x26    # should be 0
        xor   x30, x21, x27    # should be 0 if both 1