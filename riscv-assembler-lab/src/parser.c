#include "parser.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>

/* Trim leading and trailing whitespace in-place */
static char *trim(char *s)
{
    while (isspace((unsigned char)*s))
        s++;
    char *end = s + strlen(s);
    while (end > s && isspace((unsigned char)*(end - 1)))
        end--;
    *end = '\0';
    return s;
}

int parse_line(const char *line, ParsedLine *out)
{
    if (!line || !out)
        return -1;

    memset(out, 0, sizeof(*out));

    /* Copy so we can modify */
    char buf[512];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    /* Strip comments (# or ; ) */
    char *comment = strpbrk(buf, "#;");
    if (comment)
        *comment = '\0';

    char *p = trim(buf);
    if (*p == '\0')
        return 0; /* blank / comment-only line */

    /* Check for label: look for ':' before any whitespace or comma */
    char *colon = strchr(p, ':');
    if (colon)
    {
        /* Everything before ':' is the label */
        *colon = '\0';
        char *lbl = trim(p);
        strncpy(out->label, lbl, MAX_TOKEN_LEN - 1);
        p = trim(colon + 1);
        if (*p == '\0')
            return 0; /* label-only line */
    }

    /* Next token is the mnemonic */
    char *space = p;
    while (*space && !isspace((unsigned char)*space))
        space++;
    char saved = *space;
    *space = '\0';
    strncpy(out->mnemonic, p, MAX_TOKEN_LEN - 1);
    if (saved == '\0')
        return 0; /* mnemonic only, no operands */
    p = trim(space + 1);

    /* Split operands by comma */
    out->num_operands = 0;
    while (*p && out->num_operands < MAX_OPERANDS)
    {
        char *comma = strchr(p, ',');
        if (comma)
            *comma = '\0';

        /* Handle mem operands like 4(x2) — keep as single token */
        char *tok = trim(p);
        strncpy(out->operands[out->num_operands], tok, MAX_TOKEN_LEN - 1);
        out->num_operands++;

        if (!comma)
            break;
        p = trim(comma + 1);
    }

    return 0;
}