#include "registers.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* ABI name → register number mapping */
static const struct
{
    const char *name;
    int num;
} abi_map[] = {
    {"zero", 0}, {"ra", 1}, {"sp", 2}, {"gp", 3}, {"tp", 4}, {"t0", 5}, {"t1", 6}, {"t2", 7}, {"s0", 8}, {"fp", 8}, {"s1", 9}, {"a0", 10}, {"a1", 11}, {"a2", 12}, {"a3", 13}, {"a4", 14}, {"a5", 15}, {"a6", 16}, {"a7", 17}, {"s2", 18}, {"s3", 19}, {"s4", 20}, {"s5", 21}, {"s6", 22}, {"s7", 23}, {"s8", 24}, {"s9", 25}, {"s10", 26}, {"s11", 27}, {"t3", 28}, {"t4", 29}, {"t5", 30}, {"t6", 31}, {NULL, -1}};

int parse_register(const char *name)
{
    if (!name)
        return -1;

    /* Strip leading whitespace */
    while (isspace((unsigned char)*name))
        name++;

    /* x0 – x31 form */
    if (name[0] == 'x' || name[0] == 'X')
    {
        const char *digits = name + 1;
        if (*digits == '\0')
            return -1; /* bare "x" with no digits */
        char *end;
        long n = strtol(digits, &end, 10);
        if (*end == '\0' && end != digits && n >= 0 && n <= 31)
            return (int)n;
        return -1;
    }

    /* ABI names */
    for (int i = 0; abi_map[i].name; i++)
    {
        if (strcmp(name, abi_map[i].name) == 0)
            return abi_map[i].num;
    }

    return -1;
}