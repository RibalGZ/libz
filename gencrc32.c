#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "zutil.h"

#define TBLS 8

int main(int argc, char* argv[])
{
    const char *out_path = NULL;
    unsigned long tab[8][256];
    int i, j;
    unsigned long c;

    if (2 <= argc) {
        if (!(freopen(out_path = argv[1], "w", stdout))) {
            fprintf(stderr, "%s: %s: %s\n", argv[0], out_path, strerror(errno));
            return 1;
        }
    }

    for (i = 0; i < 256; i++) {
        c = i;
        for (j = 0; j < 8; j++) {
            c = (c >> 1) ^ (c & 1 ? 0xedb88320UL : 0);
        }
        tab[0][i] = c;
    }

    for (i = 0; i < 256; i++) {
        c = tab[0][i];
        for (j = 1; j < 8; j++) {
            c = (c >> 8) ^ tab[0][c & 0xff];
            tab[j][i] = c;
        }
    }

    if (out_path)
        printf("/* %s -- tables for rapid CRC calculation\n", out_path);
    else
        printf("/* tables for rapid CRC calculation\n");
    printf(" * Generated automatically by %s\n */\n\n", argv[0]);

    printf("static const z_crc_t crc_table[8][256] =\n");
    printf("{\n");
    for (i = 0; i < 8; i++) {
        printf("  {\n    ");
        for (j = 0; j < 256; j++) {
            const char *out_str = ", ";
            if (j == 255)
                out_str = "\n";
            else if(((j + 1) % 5) == 0)
                out_str = ",\n    ";
            printf("0x%08lxUL%s", tab[i][j], out_str);
        }
        printf("  }%s\n", i != 7 ? "," : "");
    }
    printf("};\n");

    if (ferror(stdout) || fflush(stdout) == EOF) {
        if (!out_path)
            out_path = "<stdout>";
        fprintf(stderr, "%s: %s: %s\n", argv[0], out_path, strerror(errno));
        return 1;
    }

    return 0;
}
