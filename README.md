# RISC-V Assembler Lab

A two-pass assembler for a subset of the RV32I instruction set, written in C.  
Takes a `.s` assembly source file and produces a `.hex` machine code output file.

---

## Supported instructions

| Format            | Instructions                                                   |
| ----------------- | -------------------------------------------------------------- |
| R-type arithmetic | `add` `sub` `and` `or` `xor` `sll` `srl` `sra` `slt` `sltu`    |
| I-type arithmetic | `addi` `andi` `ori` `xori` `slli` `srli` `srai` `slti` `sltiu` |
| Loads             | `lb` `lbu` `lh` `lhu` `lw`                                     |
| Stores            | `sb` `sh` `sw`                                                 |
| Branches          | `beq` `bne` `blt` `bge` `bltu` `bgeu`                          |
| Jumps             | `jal` `jalr`                                                   |

---

## Project structure

```
riscv-assembler-lab/
├── Makefile
├── README.md
├── src/
│   ├── main.c              # CLI entry point
│   ├── assembler.c / .h    # Two-pass pipeline coordinator
│   ├── parser.c / .h       # Line tokeniser (label, mnemonic, operands)
│   ├── registers.c / .h    # Register name → number (x0–x31 + ABI names)
│   ├── instr_table.c / .h  # Instruction metadata table (opcode, funct3, funct7)
│   ├── symbols.c / .h      # Label symbol table (add, find, duplicate detection)
│   └── encode.c / .h       # Bit-field packers for R / I / S / B / U / J formats
├── tests/
│   ├── test_registers.c
│   ├── test_parser.c
│   ├── test_instr_table.c
│   ├── test_symbols.c
│   ├── test_encode.c
│   └── test_assembler.c
└── examples/
    ├── test_case_1.s        # 22 instructions — arithmetic, memory, branches
    ├── test_case_1.hex
    ├── test_case_2.s        # 52 instructions — bubble sort + bitwise checks
    ├── test_case_2.hex
    ├── test_case_3.s        # 108 instructions — Fibonacci, GCD, checksum, shifts
    ├── test_case_3.hex
    └── test_report.md       # Venus simulator vs assembler comparison
```

---

## Build

```bash
# Build the assembler binary
make assembler

# Run all 6 unit test suites (77 tests total)
make test

# Assemble all three example programs
make examples

# Clean all build artifacts
make clean
```

---

## Usage

```bash
./assembler <input.s> [output.hex]
```

- `input.s` — path to the RISC-V assembly source file
- `output.hex` — optional output path. If omitted, written to `<input>.hex`

**Example:**

```bash
./assembler examples/test_case_1.s
cat examples/test_case_1.hex
```

---

## Assembly syntax supported

```asm
# Comments start with #
# Labels end with :
# Register names: x0–x31 or ABI names (zero, ra, sp, a0–a7, etc.)

        addi  x1, x0, 10       # I-type: rd, rs1, immediate
        add   x3, x1, x2       # R-type: rd, rs1, rs2
        lw    x4, 8(x2)        # Load:   rd, imm(rs1)
        sw    x5, 0(x2)        # Store:  rs2, imm(rs1)
loop:
        bne   x1, x0, loop     # Branch: rs1, rs2, label
        jal   x0, done         # Jump:   rd, label
done:
```

---

## Module responsibilities

| Module          | Responsibility                                   |
| --------------- | ------------------------------------------------ |
| `main.c`        | Parse CLI arguments, call `assemble_file()`      |
| `assembler.c`   | Coordinate the two-pass pipeline                 |
| `parser.c`      | Break each line into label / mnemonic / operands |
| `registers.c`   | Convert register names to numbers 0–31           |
| `instr_table.c` | Look up opcode, funct3, funct7 by mnemonic       |
| `symbols.c`     | Store and resolve label addresses                |
| `encode.c`      | Pack fields into 32-bit machine words            |

---

## Two-pass algorithm

### Pass 1 — Build symbol table

```
PC = 0
for each line:
    if line has a label → symbols_add(label, PC)
    if line has an instruction → PC += 4
```

### Pass 2 — Encode instructions

```
PC = 0
for each line:
    parse_line()
    lookup_instr(mnemonic)
    resolve registers via parse_register()
    for branches/jumps: offset = symbols_find(label) - PC
    call encode_r / encode_i / encode_s / encode_b / encode_j
    write 32-bit word to output file
    PC += 4
```

---

## Instruction encoding formats

```
R-type: [ funct7 | rs2 | rs1 | funct3 | rd  | opcode ]
         31    25  24 20  19 15  14  12  11  7   6    0

I-type: [ imm[11:0]      | rs1 | funct3 | rd  | opcode ]
         31          20   19 15  14  12  11  7   6    0

S-type: [ imm[11:5] | rs2 | rs1 | funct3 | imm[4:0] | opcode ]

B-type: [ imm[12|10:5] | rs2 | rs1 | funct3 | imm[4:1|11] | opcode ]

J-type: [ imm[20|10:1|11|19:12] | rd | opcode ]
```

---

## Unit test results

```
test_registers    17/17  ✅
test_parser       14/14  ✅
test_instr_table  17/17  ✅
test_symbols      10/10  ✅
test_encode        9/9   ✅
test_assembler    10/10  ✅
──────────────────────────
Total             77/77  ✅
```

---

## Venus verification report

All three test cases verified against [Venus RISC-V Simulator](https://venus.cs61c.org).  
Procedure: paste `.s` into Venus Editor → Assemble → compare Memory dump with `.hex` file.

> **Note for test_case_3.s:** Settings → Memory Configuration → **"Compact, Data at Address 0"**

### Summary

| Test case       | Instructions | Words matched | Result          |
| --------------- | ------------ | ------------- | --------------- |
| `test_case_1.s` | 22           | 22/22         | ✅ PASS         |
| `test_case_2.s` | 52           | 52/52         | ✅ PASS         |
| `test_case_3.s` | 108          | 108/108       | ✅ PASS         |
| **Total**       | **182**      | **182/182**   | ✅ **ALL PASS** |

### Test Case 1 — 22 instructions

| PC       | Instruction      | Our assembler | Venus      | Match |
| -------- | ---------------- | ------------- | ---------- | ----- |
| 00000000 | addi x2, x0, 200 | `0C800113`    | `0C800113` | ✅    |
| 00000004 | addi x1, x0, 10  | `00A00093`    | `00A00093` | ✅    |
| 00000008 | addi x3, x0, 0   | `00000193`    | `00000193` | ✅    |
| 0000000C | add x3, x3, x1   | `001181B3`    | `001181B3` | ✅    |
| 00000010 | addi x1, x1, -1  | `FFF08093`    | `FFF08093` | ✅    |
| 00000014 | bne x1, x0, loop | `FE009CE3`    | `FE009CE3` | ✅    |
| 00000018 | sw x3, 0(x2)     | `00312023`    | `00312023` | ✅    |
| 0000001C | lw x4, 0(x2)     | `00012203`    | `00012203` | ✅    |
| 00000020 | addi x5, x0, 55  | `03700293`    | `03700293` | ✅    |
| 00000024 | beq x4, x5, pass | `00520663`    | `00520663` | ✅    |
| 00000028 | addi x6, x0, -1  | `FFF00313`    | `FFF00313` | ✅    |
| 0000002C | jal x0, done     | `0080006F`    | `0080006F` | ✅    |
| 00000030 | addi x6, x0, 1   | `00100313`    | `00100313` | ✅    |
| 00000034 | xor x7, x4, x5   | `005243B3`    | `005243B3` | ✅    |
| 00000038 | slt x8, x0, x6   | `00602433`    | `00602433` | ✅    |
| 0000003C | and x9, x6, x8   | `008374B3`    | `008374B3` | ✅    |
| 00000040 | or x10, x6, x7   | `00736533`    | `00736533` | ✅    |
| 00000044 | sltu x11, x0, x5 | `005035B3`    | `005035B3` | ✅    |
| 00000048 | sh x6, 4(x2)     | `00611223`    | `00611223` | ✅    |
| 0000004C | lh x12, 4(x2)    | `00411603`    | `00411603` | ✅    |
| 00000050 | sb x6, 8(x2)     | `00610423`    | `00610423` | ✅    |
| 00000054 | lb x13, 8(x2)    | `00810683`    | `00810683` | ✅    |

**22/22 ✅**

### Test Case 2 — 52 instructions

| PC       | Instruction          | Our assembler | Venus      | Match |
| -------- | -------------------- | ------------- | ---------- | ----- |
| 00000000 | addi x2, x0, 400     | `19000113`    | `19000113` | ✅    |
| 00000004 | addi x1, x0, 8       | `00800093`    | `00800093` | ✅    |
| 00000008 | sw x1, 0(x2)         | `00112023`    | `00112023` | ✅    |
| 0000000C | addi x1, x0, 7       | `00700093`    | `00700093` | ✅    |
| 00000010 | sw x1, 4(x2)         | `00112223`    | `00112223` | ✅    |
| 00000014 | addi x1, x0, 6       | `00600093`    | `00600093` | ✅    |
| 00000018 | sw x1, 8(x2)         | `00112423`    | `00112423` | ✅    |
| 0000001C | addi x1, x0, 5       | `00500093`    | `00500093` | ✅    |
| 00000020 | sw x1, 12(x2)        | `00112623`    | `00112623` | ✅    |
| 00000024 | addi x1, x0, 4       | `00400093`    | `00400093` | ✅    |
| 00000028 | sw x1, 16(x2)        | `00112823`    | `00112823` | ✅    |
| 0000002C | addi x1, x0, 3       | `00300093`    | `00300093` | ✅    |
| 00000030 | sw x1, 20(x2)        | `00112A23`    | `00112A23` | ✅    |
| 00000034 | addi x1, x0, 2       | `00200093`    | `00200093` | ✅    |
| 00000038 | sw x1, 24(x2)        | `00112C23`    | `00112C23` | ✅    |
| 0000003C | addi x1, x0, 1       | `00100093`    | `00100093` | ✅    |
| 00000040 | sw x1, 28(x2)        | `00112E23`    | `00112E23` | ✅    |
| 00000044 | addi x10, x0, 7      | `00700513`    | `00700513` | ✅    |
| 00000048 | addi x11, x0, 0      | `00000593`    | `00000593` | ✅    |
| 0000004C | addi x12, x2, 0      | `00010613`    | `00010613` | ✅    |
| 00000050 | lw x13, 0(x12)       | `00062683`    | `00062683` | ✅    |
| 00000054 | lw x14, 4(x12)       | `00462703`    | `00462703` | ✅    |
| 00000058 | bge x13, x14, noswap | `00E6D663`    | `00E6D663` | ✅    |
| 0000005C | sw x14, 0(x12)       | `00E62023`    | `00E62023` | ✅    |
| 00000060 | sw x13, 4(x12)       | `00D62223`    | `00D62223` | ✅    |
| 00000064 | addi x11, x11, 1     | `00158593`    | `00158593` | ✅    |
| 00000068 | addi x12, x12, 4     | `00460613`    | `00460613` | ✅    |
| 0000006C | blt x11, x10, inner  | `FEA5C2E3`    | `FEA5C2E3` | ✅    |
| 00000070 | addi x10, x10, -1    | `FFF50513`    | `FFF50513` | ✅    |
| 00000074 | bne x10, x0, outer   | `FC051AE3`    | `FC051AE3` | ✅    |
| 00000078 | addi x15, x0, 5      | `00500793`    | `00500793` | ✅    |
| 0000007C | addi x16, x0, 0      | `00000813`    | `00000813` | ✅    |
| 00000080 | addi x17, x2, 0      | `00010893`    | `00010893` | ✅    |
| 00000084 | lw x18, 0(x17)       | `0008A903`    | `0008A903` | ✅    |
| 00000088 | beq x18, x15, found  | `00F90E63`    | `00F90E63` | ✅    |
| 0000008C | addi x16, x16, 1     | `00180813`    | `00180813` | ✅    |
| 00000090 | addi x17, x17, 4     | `00488893`    | `00488893` | ✅    |
| 00000094 | addi x19, x0, 8      | `00800993`    | `00800993` | ✅    |
| 00000098 | blt x16, x19, search | `FF3846E3`    | `FF3846E3` | ✅    |
| 0000009C | addi x20, x0, -1     | `FFF00A13`    | `FFF00A13` | ✅    |
| 000000A0 | jal x0, sdone        | `0080006F`    | `0080006F` | ✅    |
| 000000A4 | addi x20, x0, 1      | `00100A13`    | `00100A13` | ✅    |
| 000000A8 | andi x21, x20, 1     | `001A7A93`    | `001A7A93` | ✅    |
| 000000AC | ori x22, x20, 0      | `000A6B13`    | `000A6B13` | ✅    |
| 000000B0 | xori x23, x20, 0     | `000A4B93`    | `000A4B93` | ✅    |
| 000000B4 | slli x24, x20, 2     | `002A1C13`    | `002A1C13` | ✅    |
| 000000B8 | srli x25, x24, 2     | `002C5C93`    | `002C5C93` | ✅    |
| 000000BC | srai x26, x24, 2     | `402C5D13`    | `402C5D13` | ✅    |
| 000000C0 | slti x27, x20, 2     | `002A2D93`    | `002A2D93` | ✅    |
| 000000C4 | sltiu x28, x20, 10   | `00AA3E13`    | `00AA3E13` | ✅    |
| 000000C8 | sub x29, x25, x26    | `41AC8EB3`    | `41AC8EB3` | ✅    |
| 000000CC | xor x30, x21, x27    | `01BACF33`    | `01BACF33` | ✅    |

**52/52 ✅**

### Test Case 3 — 108 instructions

| Line | Our assembler | Venus      | Match |
| ---- | ------------- | ---------- | ----- |
| 1    | `3E800113`    | `3E800113` | ✅    |
| 2    | `00000093`    | `00000093` | ✅    |
| 3    | `00100193`    | `00100193` | ✅    |
| 4    | `00A00213`    | `00A00213` | ✅    |
| 5    | `00200293`    | `00200293` | ✅    |
| 6    | `00308333`    | `00308333` | ✅    |
| 7    | `00018093`    | `00018093` | ✅    |
| 8    | `00030193`    | `00030193` | ✅    |
| 9    | `00128293`    | `00128293` | ✅    |
| 10   | `FE42C8E3`    | `FE42C8E3` | ✅    |
| 11   | `000202B3`    | `000202B3` | ✅    |
| 12   | `00308333`    | `00308333` | ✅    |
| 13   | `00018093`    | `00018093` | ✅    |
| 14   | `00030193`    | `00030193` | ✅    |
| 15   | `00312023`    | `00312023` | ✅    |
| 16   | `03700393`    | `03700393` | ✅    |
| 17   | `00718663`    | `00718663` | ✅    |
| 18   | `FFF00413`    | `FFF00413` | ✅    |
| 19   | `0080006F`    | `0080006F` | ✅    |
| 20   | `00100413`    | `00100413` | ✅    |
| 21   | `03000493`    | `03000493` | ✅    |
| 22   | `01200513`    | `01200513` | ✅    |
| 23   | `02050063`    | `02050063` | ✅    |
| 24   | `00048593`    | `00048593` | ✅    |
| 25   | `00A5C663`    | `00A5C663` | ✅    |
| 26   | `40A585B3`    | `40A585B3` | ✅    |
| 27   | `FF9FF06F`    | `FF9FF06F` | ✅    |
| 28   | `00050493`    | `00050493` | ✅    |
| 29   | `00058513`    | `00058513` | ✅    |
| 30   | `FE5FF06F`    | `FE5FF06F` | ✅    |
| 31   | `00912223`    | `00912223` | ✅    |
| 32   | `00600613`    | `00600613` | ✅    |
| 33   | `00C48663`    | `00C48663` | ✅    |
| 34   | `FFF00693`    | `FFF00693` | ✅    |
| 35   | `0080006F`    | `0080006F` | ✅    |
| 36   | `00100693`    | `00100693` | ✅    |
| 37   | `06410713`    | `06410713` | ✅    |
| 38   | `00100793`    | `00100793` | ✅    |
| 39   | `00F70023`    | `00F70023` | ✅    |
| 40   | `00200793`    | `00200793` | ✅    |
| 41   | `00F700A3`    | `00F700A3` | ✅    |
| 42   | `00300793`    | `00300793` | ✅    |
| 43   | `00F70123`    | `00F70123` | ✅    |
| 44   | `00400793`    | `00400793` | ✅    |
| 45   | `00F701A3`    | `00F701A3` | ✅    |
| 46   | `00500793`    | `00500793` | ✅    |
| 47   | `00F70223`    | `00F70223` | ✅    |
| 48   | `00600793`    | `00600793` | ✅    |
| 49   | `00F702A3`    | `00F702A3` | ✅    |
| 50   | `00700793`    | `00700793` | ✅    |
| 51   | `00F70323`    | `00F70323` | ✅    |
| 52   | `00800793`    | `00800793` | ✅    |
| 53   | `00F703A3`    | `00F703A3` | ✅    |
| 54   | `00000813`    | `00000813` | ✅    |
| 55   | `00000893`    | `00000893` | ✅    |
| 56   | `00070913`    | `00070913` | ✅    |
| 57   | `00800993`    | `00800993` | ✅    |
| 58   | `00094A03`    | `00094A03` | ✅    |
| 59   | `01480833`    | `01480833` | ✅    |
| 60   | `00188893`    | `00188893` | ✅    |
| 61   | `00190913`    | `00190913` | ✅    |
| 62   | `FF38C8E3`    | `FF38C8E3` | ✅    |
| 63   | `01012423`    | `01012423` | ✅    |
| 64   | `02400A93`    | `02400A93` | ✅    |
| 65   | `01580663`    | `01580663` | ✅    |
| 66   | `FFF00B13`    | `FFF00B13` | ✅    |
| 67   | `0080006F`    | `0080006F` | ✅    |
| 68   | `00100B13`    | `00100B13` | ✅    |
| 69   | `00C00B93`    | `00C00B93` | ✅    |
| 70   | `002B9C13`    | `002B9C13` | ✅    |
| 71   | `002C5C93`    | `002C5C93` | ✅    |
| 72   | `402C5D13`    | `402C5D13` | ✅    |
| 73   | `01AC8663`    | `01AC8663` | ✅    |
| 74   | `FFF00D93`    | `FFF00D93` | ✅    |
| 75   | `0080006F`    | `0080006F` | ✅    |
| 76   | `00100D93`    | `00100D93` | ✅    |
| 77   | `0FF00E13`    | `0FF00E13` | ✅    |
| 78   | `00FE7E93`    | `00FE7E93` | ✅    |
| 79   | `0F0EEE93`    | `0F0EEE93` | ✅    |
| 80   | `0FFECE93`    | `0FFECE93` | ✅    |
| 81   | `000E8663`    | `000E8663` | ✅    |
| 82   | `FFF00F13`    | `FFF00F13` | ✅    |
| 83   | `0080006F`    | `0080006F` | ✅    |
| 84   | `00100F13`    | `00100F13` | ✅    |
| 85   | `00500093`    | `00500093` | ✅    |
| 86   | `00A00193`    | `00A00193` | ✅    |
| 87   | `0030A233`    | `0030A233` | ✅    |
| 88   | `0011A2B3`    | `0011A2B3` | ✅    |
| 89   | `0030B333`    | `0030B333` | ✅    |
| 90   | `00A0A393`    | `00A0A393` | ✅    |
| 91   | `0030B413`    | `0030B413` | ✅    |
| 92   | `006274B3`    | `006274B3` | ✅    |
| 93   | `00526533`    | `00526533` | ✅    |
| 94   | `005245B3`    | `005245B3` | ✅    |
| 95   | `40118633`    | `40118633` | ✅    |
| 96   | `00912623`    | `00912623` | ✅    |
| 97   | `00A12823`    | `00A12823` | ✅    |
| 98   | `00B12A23`    | `00B12A23` | ✅    |
| 99   | `00C12C23`    | `00C12C23` | ✅    |
| 100  | `00C12683`    | `00C12683` | ✅    |
| 101  | `01012703`    | `01012703` | ✅    |
| 102  | `01411783`    | `01411783` | ✅    |
| 103  | `01815803`    | `01815803` | ✅    |
| 104  | `01810883`    | `01810883` | ✅    |
| 105  | `01814903`    | `01814903` | ✅    |
| 106  | `00400993`    | `00400993` | ✅    |
| 107  | `00098067`    | `00098067` | ✅    |
| 108  | `0000006F`    | `0000006F` | ✅    |

**108/108 ✅**

---

## References

- RISC-V ISA Specification — [riscv.org](https://riscv.org/technical/specifications/)
- CS61C RISC-V Reference Card
- Venus RISC-V Simulator — [venus.cs61c.org](https://venus.cs61c.org)
