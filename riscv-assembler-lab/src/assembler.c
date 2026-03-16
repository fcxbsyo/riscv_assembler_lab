#include "assembler.h"
#include "parser.h"
#include "registers.h"
#include "instr_table.h"
#include "symbols.h"
#include "encode.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 4096
#define LINE_BUF 512

/* ──────────────────────────────────────────────────────────────────────────
   Helper: parse a memory operand of the form  imm(reg)  or  reg
   Returns 0 on success, fills *reg_num and *imm.
   ────────────────────────────────────────────────────────────────────────── */
static int parse_mem_operand(const char *op, int *reg_num, int *imm)
{
    char buf[MAX_TOKEN_LEN];
    strncpy(buf, op, MAX_TOKEN_LEN - 1);
    buf[MAX_TOKEN_LEN - 1] = '\0';

    char *lparen = strchr(buf, '(');
    if (lparen)
    {
        /* imm(reg) form */
        *lparen = '\0';
        char *rparen = strchr(lparen + 1, ')');
        if (rparen)
            *rparen = '\0';
        *imm = (int)strtol(buf, NULL, 0);
        *reg_num = parse_register(lparen + 1);
        return (*reg_num >= 0) ? 0 : -1;
    }
    /* Plain register, no immediate */
    *imm = 0;
    *reg_num = parse_register(buf);
    return (*reg_num >= 0) ? 0 : -1;
}

/* ──────────────────────────────────────────────────────────────────────────
   Encode one parsed instruction into a 32-bit word.
   pc   = current program counter (byte address)
   syms = symbol table (for branch/jump targets)
   Returns encoded word, or 0xFFFFFFFF on error.
   ────────────────────────────────────────────────────────────────────────── */
static uint32_t encode_instr(const ParsedLine *pl, unsigned pc,
                             const SymbolTable *syms, int line_no)
{
    const InstrSpec *spec = lookup_instr(pl->mnemonic);
    if (!spec)
    {
        fprintf(stderr, "line %d: unknown instruction '%s'\n",
                line_no, pl->mnemonic);
        return 0xFFFFFFFF;
    }

    int rd, rs1, rs2, imm, addr;

    switch (spec->fmt)
    {

    case FMT_R:
        /* add rd, rs1, rs2 */
        if (pl->num_operands < 3)
            goto bad_ops;
        rd = parse_register(pl->operands[0]);
        rs1 = parse_register(pl->operands[1]);
        rs2 = parse_register(pl->operands[2]);
        if (rd < 0 || rs1 < 0 || rs2 < 0)
            goto bad_reg;
        return encode_r(spec->opcode, spec->funct3, spec->funct7,
                        rd, rs1, rs2);

    case FMT_I:
        if (spec->opcode == 0x03)
        {
            /* Load: lw rd, imm(rs1) */
            if (pl->num_operands < 2)
                goto bad_ops;
            rd = parse_register(pl->operands[0]);
            if (rd < 0)
                goto bad_reg;
            if (parse_mem_operand(pl->operands[1], &rs1, &imm) < 0)
                goto bad_reg;
        }
        else if (spec->opcode == 0x67)
        {
            /* jalr rd, rs1, imm  OR  jalr rd, imm(rs1) */
            if (pl->num_operands < 2)
                goto bad_ops;
            rd = parse_register(pl->operands[0]);
            if (rd < 0)
                goto bad_reg;
            if (pl->num_operands == 2)
            {
                /* jalr rd, imm(rs1) */
                if (parse_mem_operand(pl->operands[1], &rs1, &imm) < 0)
                    goto bad_reg;
            }
            else
            {
                rs1 = parse_register(pl->operands[1]);
                imm = (int)strtol(pl->operands[2], NULL, 0);
                if (rs1 < 0)
                    goto bad_reg;
            }
        }
        else if (spec->funct3 == 0x1 ||
                 (spec->funct3 == 0x5 && spec->opcode == 0x13))
        {
            /* Shift immediate: slli/srli/srai  rd, rs1, shamt */
            if (pl->num_operands < 3)
                goto bad_ops;
            rd = parse_register(pl->operands[0]);
            rs1 = parse_register(pl->operands[1]);
            if (rd < 0 || rs1 < 0)
                goto bad_reg;
            int shamt = (int)strtol(pl->operands[2], NULL, 0) & 0x1F;
            /* srai: set bit 30 of the 32-bit instruction → bit 10 of imm field */
            if (spec->funct7 == 0x20)
                imm = (1 << 10) | shamt;
            else
                imm = shamt;
        }
        else
        {
            /* addi, andi, ori, xori, slti, sltiu */
            if (pl->num_operands < 3)
                goto bad_ops;
            rd = parse_register(pl->operands[0]);
            rs1 = parse_register(pl->operands[1]);
            if (rd < 0 || rs1 < 0)
                goto bad_reg;
            imm = (int)strtol(pl->operands[2], NULL, 0);
        }
        return encode_i(spec->opcode, spec->funct3, rd, rs1, imm);

    case FMT_S:
        /* sw rs2, imm(rs1) */
        if (pl->num_operands < 2)
            goto bad_ops;
        rs2 = parse_register(pl->operands[0]);
        if (rs2 < 0)
            goto bad_reg;
        if (parse_mem_operand(pl->operands[1], &rs1, &imm) < 0)
            goto bad_reg;
        return encode_s(spec->opcode, spec->funct3, rs1, rs2, imm);

    case FMT_B:
        /* beq rs1, rs2, label */
        if (pl->num_operands < 3)
            goto bad_ops;
        rs1 = parse_register(pl->operands[0]);
        rs2 = parse_register(pl->operands[1]);
        if (rs1 < 0 || rs2 < 0)
            goto bad_reg;
        addr = symbols_find(syms, pl->operands[2]);
        if (addr < 0)
        {
            /* Try numeric literal */
            char *endp;
            long lit = strtol(pl->operands[2], &endp, 0);
            if (*endp == '\0')
                addr = (int)lit;
            else
            {
                fprintf(stderr, "line %d: undefined label '%s'\n",
                        line_no, pl->operands[2]);
                return 0xFFFFFFFF;
            }
        }
        imm = addr - (int)pc;
        return encode_b(spec->opcode, spec->funct3, rs1, rs2, imm);

    case FMT_J:
        /* jal rd, label */
        if (pl->num_operands < 2)
            goto bad_ops;
        rd = parse_register(pl->operands[0]);
        if (rd < 0)
            goto bad_reg;
        addr = symbols_find(syms, pl->operands[1]);
        if (addr < 0)
        {
            char *endp;
            long lit = strtol(pl->operands[1], &endp, 0);
            if (*endp == '\0')
                addr = (int)lit;
            else
            {
                fprintf(stderr, "line %d: undefined label '%s'\n",
                        line_no, pl->operands[1]);
                return 0xFFFFFFFF;
            }
        }
        imm = addr - (int)pc;
        return encode_j(spec->opcode, rd, imm);

    default:
        fprintf(stderr, "line %d: unsupported format\n", line_no);
        return 0xFFFFFFFF;
    }

bad_ops:
    fprintf(stderr, "line %d: wrong number of operands for '%s'\n",
            line_no, pl->mnemonic);
    return 0xFFFFFFFF;
bad_reg:
    fprintf(stderr, "line %d: bad register in '%s'\n",
            line_no, pl->mnemonic);
    return 0xFFFFFFFF;
}

/* ──────────────────────────────────────────────────────────────────────────
   assemble_file
   ────────────────────────────────────────────────────────────────────────── */
int assemble_file(const char *in_path, const char *out_path)
{
    FILE *fin = fopen(in_path, "r");
    if (!fin)
    {
        perror(in_path);
        return 1;
    }

    /* Read all lines */
    char lines[MAX_LINES][LINE_BUF];
    int nlines = 0;
    while (nlines < MAX_LINES && fgets(lines[nlines], LINE_BUF, fin))
        nlines++;
    fclose(fin);

    SymbolTable syms;
    symbols_init(&syms);

    /* ── PASS 1: collect labels ── */
    unsigned pc = 0;
    for (int i = 0; i < nlines; i++)
    {
        ParsedLine pl;
        if (parse_line(lines[i], &pl) < 0)
            continue;
        if (pl.label[0])
            symbols_add(&syms, pl.label, pc);
        if (pl.mnemonic[0])
            pc += 4;
    }

    /* ── PASS 2: encode ── */
    FILE *fout = fopen(out_path, "w");
    if (!fout)
    {
        perror(out_path);
        return 1;
    }

    pc = 0;
    int errors = 0;
    for (int i = 0; i < nlines; i++)
    {
        ParsedLine pl;
        if (parse_line(lines[i], &pl) < 0)
            continue;
        if (!pl.mnemonic[0])
            continue;

        uint32_t word = encode_instr(&pl, pc, &syms, i + 1);
        if (word == 0xFFFFFFFF)
        {
            errors++;
        }
        else
        {
            fprintf(fout, "%08X\n", word);
        }
        pc += 4;
    }

    fclose(fout);
    if (errors)
        fprintf(stderr, "%d error(s) during assembly.\n", errors);
    return errors ? 1 : 0;
}