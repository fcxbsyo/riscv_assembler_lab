#include "encode.h"

/* Helper: mask to n bits */
#define MASK(n) ((1u << (n)) - 1u)
#define BIT(x, i) (((unsigned)(x) >> (i)) & 1u)

/* R-type:  funct7[31:25] | rs2[24:20] | rs1[19:15] | funct3[14:12] | rd[11:7] | opcode[6:0] */
uint32_t encode_r(unsigned opcode, unsigned funct3, unsigned funct7,
                  unsigned rd, unsigned rs1, unsigned rs2)
{
    return ((funct7 & MASK(7)) << 25) |
           ((rs2 & MASK(5)) << 20) |
           ((rs1 & MASK(5)) << 15) |
           ((funct3 & MASK(3)) << 12) |
           ((rd & MASK(5)) << 7) |
           (opcode & MASK(7));
}

/* I-type:  imm[11:0][31:20] | rs1[19:15] | funct3[14:12] | rd[11:7] | opcode[6:0]
   Special case: slli/srli/srai embed funct7 in imm[11:5]. Caller sets imm appropriately. */
uint32_t encode_i(unsigned opcode, unsigned funct3,
                  unsigned rd, unsigned rs1, int imm)
{
    unsigned uimm = (unsigned)imm & MASK(12);
    return (uimm << 20) |
           ((rs1 & MASK(5)) << 15) |
           ((funct3 & MASK(3)) << 12) |
           ((rd & MASK(5)) << 7) |
           (opcode & MASK(7));
}

/* S-type:  imm[11:5][31:25] | rs2[24:20] | rs1[19:15] | funct3[14:12] | imm[4:0][11:7] | opcode[6:0] */
uint32_t encode_s(unsigned opcode, unsigned funct3,
                  unsigned rs1, unsigned rs2, int imm)
{
    unsigned uimm = (unsigned)imm & MASK(12);
    return (((uimm >> 5) & MASK(7)) << 25) |
           ((rs2 & MASK(5)) << 20) |
           ((rs1 & MASK(5)) << 15) |
           ((funct3 & MASK(3)) << 12) |
           ((uimm & MASK(5)) << 7) |
           (opcode & MASK(7));
}

/* B-type immediate encoding:
   inst[31]   = imm[12]
   inst[30:25]= imm[10:5]
   inst[11:8] = imm[4:1]
   inst[7]    = imm[11]  */
uint32_t encode_b(unsigned opcode, unsigned funct3,
                  unsigned rs1, unsigned rs2, int imm)
{
    unsigned u = (unsigned)imm;
    return (BIT(u, 12) << 31) |
           (((u >> 5) & MASK(6)) << 25) |
           ((rs2 & MASK(5)) << 20) |
           ((rs1 & MASK(5)) << 15) |
           ((funct3 & MASK(3)) << 12) |
           (((u >> 1) & MASK(4)) << 8) |
           (BIT(u, 11) << 7) |
           (opcode & MASK(7));
}

/* U-type:  imm[31:12] | rd[11:7] | opcode[6:0] */
uint32_t encode_u(unsigned opcode, unsigned rd, int imm)
{
    return ((unsigned)imm & 0xFFFFF000u) |
           ((rd & MASK(5)) << 7) |
           (opcode & MASK(7));
}

/* J-type immediate encoding:
   inst[31]    = imm[20]
   inst[30:21] = imm[10:1]
   inst[20]    = imm[11]
   inst[19:12] = imm[19:12]  */
uint32_t encode_j(unsigned opcode, unsigned rd, int imm)
{
    unsigned u = (unsigned)imm;
    return (BIT(u, 20) << 31) |
           (((u >> 1) & MASK(10)) << 21) |
           (BIT(u, 11) << 20) |
           (((u >> 12) & MASK(8)) << 12) |
           ((rd & MASK(5)) << 7) |
           (opcode & MASK(7));
}