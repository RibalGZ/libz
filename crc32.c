/* crc32.c -- compute the CRC-32 of a data stream
 * Copyright (C) 1995-2006, 2010, 2011, 2012 Mark Adler
 * Copyright (C) 2015 Josiah Worcester
 * For conditions of distribution and use, see copyright notice in zlib.h
 *
 */

/* @(#) $Id$ */

#include <stdint.h>

#include "zutil.h"

/* Local functions for crc concatenation */
static unsigned long gf2_matrix_times(unsigned long *mat, unsigned long vec);
static void gf2_matrix_square(unsigned long *square, unsigned long *mat);
static unsigned long crc32_combine_(unsigned long crc1, unsigned long crc2, off_t len2);

/* ========================================================================
 * Tables of CRC-32s of all single-byte values.
 */
#include "crc32.h"

/* =========================================================================
 * This function can be used by asm versions of crc32()
 */
const z_crc_t * ZEXPORT get_crc_table(void)
{
    return (const z_crc_t *)crc_table;
}

/* =========================================================================
 * Implement crc32 using Intel's "slicing by 8" algorithm. Significantly
 * faster than most other common approachs on common CPUs at the time of
 * this writing.
 */
unsigned long ZEXPORT crc32(unsigned long crc,
                            const unsigned char *buf,
                            unsigned int len)
{
    if (buf == NULL) return 0UL;

    crc = crc ^ 0xffffffffUL;
    for (; len > 0 && (uintptr_t)buf & 7; len--, buf++)
        crc = crc_table[0][(crc & 0xff) ^ *buf] ^ (crc >> 8);
    for (; len >= 8; len -= 8, buf += 8) {
        crc = crc_table[7][(buf[0] ^ (crc      )) & 0xff]
            ^ crc_table[6][(buf[1] ^ (crc >> 8 )) & 0xff]
            ^ crc_table[5][(buf[2] ^ (crc >> 16)) & 0xff]
            ^ crc_table[4][(buf[3] ^ (crc >> 24)) & 0xff]
            ^ crc_table[3][buf[4]]
            ^ crc_table[2][buf[5]]
            ^ crc_table[1][buf[6]]
            ^ crc_table[0][buf[7]];
    }
    for (; len > 0; len--, buf++)
        crc = crc_table[0][(crc & 0xff) ^ *buf] ^ (crc >> 8);
    return crc ^ 0xffffffffUL;
}

#define GF2_DIM 32      /* dimension of GF(2) vectors (length of CRC) */

/* ========================================================================= */
static unsigned long gf2_matrix_times(unsigned long *mat,
                                      unsigned long vec)
{
    unsigned long sum;

    sum = 0;
    while (vec) {
        if (vec & 1)
            sum ^= *mat;
        vec >>= 1;
        mat++;
    }
    return sum;
}

/* ========================================================================= */
static void gf2_matrix_square(unsigned long *square,
                              unsigned long *mat)
{
    int n;

    for (n = 0; n < GF2_DIM; n++)
        square[n] = gf2_matrix_times(mat, mat[n]);
}

/* ========================================================================= */
static unsigned long crc32_combine_(unsigned long crc1,
                                    unsigned long crc2,
                                    off_t len2)
{
    int n;
    unsigned long row;
    unsigned long even[GF2_DIM];    /* even-power-of-two zeros operator */
    unsigned long odd[GF2_DIM];     /* odd-power-of-two zeros operator */

    /* degenerate case (also disallow negative lengths) */
    if (len2 <= 0)
        return crc1;

    /* put operator for one zero bit in odd */
    odd[0] = 0xedb88320UL;          /* CRC-32 polynomial */
    row = 1;
    for (n = 1; n < GF2_DIM; n++) {
        odd[n] = row;
        row <<= 1;
    }

    /* put operator for two zero bits in even */
    gf2_matrix_square(even, odd);

    /* put operator for four zero bits in odd */
    gf2_matrix_square(odd, even);

    /* apply len2 zeros to crc1 (first square will put the operator for one
       zero byte, eight zero bits, in even) */
    do {
        /* apply zeros operator for this bit of len2 */
        gf2_matrix_square(even, odd);
        if (len2 & 1)
            crc1 = gf2_matrix_times(even, crc1);
        len2 >>= 1;

        /* if no more bits set, then done */
        if (len2 == 0)
            break;

        /* another iteration of the loop with odd and even swapped */
        gf2_matrix_square(odd, even);
        if (len2 & 1)
            crc1 = gf2_matrix_times(odd, crc1);
        len2 >>= 1;

        /* if no more bits set, then done */
    } while (len2 != 0);

    /* return combined crc */
    crc1 ^= crc2;
    return crc1;
}

/* ========================================================================= */
unsigned long ZEXPORT crc32_combine(unsigned long crc1, unsigned long crc2, z_default_off_t len2)
{
    return crc32_combine_(crc1, crc2, len2);
}

unsigned long ZEXPORT crc32_combine64(unsigned long crc1, unsigned long crc2, off_t len2)
{
    return crc32_combine_(crc1, crc2, len2);
}
