#include "assembler.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input.s> [output.hex]\n", argv[0]);
        return 1;
    }

    const char *in = argv[1];
    char out_buf[512];
    const char *out;

    if (argc >= 3)
    {
        out = argv[2];
    }
    else
    {
        /* Default: replace extension with .hex */
        strncpy(out_buf, in, sizeof(out_buf) - 5);
        out_buf[sizeof(out_buf) - 5] = '\0';
        char *dot = strrchr(out_buf, '.');
        if (dot)
            *dot = '\0';
        strcat(out_buf, ".hex");
        out = out_buf;
    }

    int rc = assemble_file(in, out);
    if (rc == 0)
        printf("Assembled '%s' → '%s'\n", in, out);
    return rc;
}