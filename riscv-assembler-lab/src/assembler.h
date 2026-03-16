#ifndef ASSEMBLER_H
#define ASSEMBLER_H

/* assemble_file: read assembly from in_path, write hex machine code to out_path.
   Returns 0 on success, non-zero on error. */
int assemble_file(const char *in_path, const char *out_path);

#endif /* ASSEMBLER_H */