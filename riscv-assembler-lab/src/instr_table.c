#include "instr_table.h"
#include <string.h>

/* Reference: RISC-V ISA Spec / CS61C reference card */
static const InstrSpec table[] = {
    /* ---- R-type -------------------------------------------------------- */
    {"add", FMT_R, 0x33, 0x0, 0x00},
    {"sub", FMT_R, 0x33, 0x0, 0x20},
    {"and", FMT_R, 0x33, 0x7, 0x00},
    {"or", FMT_R, 0x33, 0x6, 0x00},
    {"xor", FMT_R, 0x33, 0x4, 0x00},
    {"sll", FMT_R, 0x33, 0x1, 0x00},
    {"srl", FMT_R, 0x33, 0x5, 0x00},
    {"sra", FMT_R, 0x33, 0x5, 0x20},
    {"slt", FMT_R, 0x33, 0x2, 0x00},
    {"sltu", FMT_R, 0x33, 0x3, 0x00},

    /* ---- I-type arithmetic --------------------------------------------- */
    {"addi", FMT_I, 0x13, 0x0, 0x00},
    {"andi", FMT_I, 0x13, 0x7, 0x00},
    {"ori", FMT_I, 0x13, 0x6, 0x00},
    {"xori", FMT_I, 0x13, 0x4, 0x00},
    {"slti", FMT_I, 0x13, 0x2, 0x00},
    {"sltiu", FMT_I, 0x13, 0x3, 0x00},
    /* Shift immediates: funct7 embedded in immediate, handled in encode */
    {"slli", FMT_I, 0x13, 0x1, 0x00},
    {"srli", FMT_I, 0x13, 0x5, 0x00},
    {"srai", FMT_I, 0x13, 0x5, 0x20}, /* funct7=0x20 signals arithmetic */

    /* ---- I-type loads -------------------------------------------------- */
    {"lb", FMT_I, 0x03, 0x0, 0x00},
    {"lbu", FMT_I, 0x03, 0x4, 0x00},
    {"lh", FMT_I, 0x03, 0x1, 0x00},
    {"lhu", FMT_I, 0x03, 0x5, 0x00},
    {"lw", FMT_I, 0x03, 0x2, 0x00},

    /* ---- I-type jalr --------------------------------------------------- */
    {"jalr", FMT_I, 0x67, 0x0, 0x00},

    /* ---- S-type stores ------------------------------------------------- */
    {"sb", FMT_S, 0x23, 0x0, 0x00},
    {"sh", FMT_S, 0x23, 0x1, 0x00},
    {"sw", FMT_S, 0x23, 0x2, 0x00},

    /* ---- B-type branches ----------------------------------------------- */
    {"beq", FMT_B, 0x63, 0x0, 0x00},
    {"bne", FMT_B, 0x63, 0x1, 0x00},
    {"blt", FMT_B, 0x63, 0x4, 0x00},
    {"bge", FMT_B, 0x63, 0x5, 0x00},
    {"bltu", FMT_B, 0x63, 0x6, 0x00},
    {"bgeu", FMT_B, 0x63, 0x7, 0x00},

    /* ---- J-type -------------------------------------------------------- */
    {"jal", FMT_J, 0x6F, 0x0, 0x00},

    {NULL, 0, 0, 0, 0}};

const InstrSpec *lookup_instr(const char *mnemonic)
{
    if (!mnemonic)
        return NULL;
    for (int i = 0; table[i].mnemonic; i++)
    {
        if (strcmp(mnemonic, table[i].mnemonic) == 0)
            return &table[i];
    }
    return NULL;
}