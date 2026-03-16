#ifndef PARSER_H
#define PARSER_H

#define MAX_OPERANDS 4
#define MAX_TOKEN_LEN 64

typedef struct
{
    char label[MAX_TOKEN_LEN];    /* empty string if no label */
    char mnemonic[MAX_TOKEN_LEN]; /* empty string if label-only line */
    char operands[MAX_OPERANDS][MAX_TOKEN_LEN];
    int num_operands;
} ParsedLine;

/* parse_line: parse one line of assembly into a ParsedLine.
   Returns 0 on success, -1 on error. */
int parse_line(const char *line, ParsedLine *out);

#endif /* PARSER_H */