#ifndef REGISTERS_H
#define REGISTERS_H

/* parse_register: convert "x0"-"x31" or ABI names to register number.
   Returns register number 0-31, or -1 on error. */
int parse_register(const char *name);

#endif /* REGISTERS_H */