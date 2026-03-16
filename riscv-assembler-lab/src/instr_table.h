#ifndef INSTR_TABLE_H
#define INSTR_TABLE_H

typedef enum
{
    FMT_R,
    FMT_I,
    FMT_S,
    FMT_B,
    FMT_U,
    FMT_J
} InstrFmt;

typedef struct
{
    const char *mnemonic;
    InstrFmt fmt;
    unsigned opcode; /* 7-bit opcode */
    unsigned funct3; /* 3-bit funct3 (0 if unused) */
    unsigned funct7; /* 7-bit funct7 (0 if unused) */
} InstrSpec;

/* lookup_instr: find instruction spec by mnemonic.
   Returns pointer to static entry, or NULL if not found. */
const InstrSpec *lookup_instr(const char *mnemonic);

#endif /* INSTR_TABLE_H */