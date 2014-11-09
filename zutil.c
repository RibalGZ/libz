/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2005, 2010, 2011, 2012 Jean-loup Gailly.
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include "zutil.h"
#include "gzguts.h"

const char * const z_errmsg[10] = {
"need dictionary",     /* Z_NEED_DICT       2  */
"stream end",          /* Z_STREAM_END      1  */
"",                    /* Z_OK              0  */
"file error",          /* Z_ERRNO         (-1) */
"stream error",        /* Z_STREAM_ERROR  (-2) */
"data error",          /* Z_DATA_ERROR    (-3) */
"insufficient memory", /* Z_MEM_ERROR     (-4) */
"buffer error",        /* Z_BUF_ERROR     (-5) */
"incompatible version",/* Z_VERSION_ERROR (-6) */
""};


const char * ZEXPORT zlibVersion(void)
{
    return ZLIB_VERSION;
}

unsigned long ZEXPORT zlibCompileFlags(void)
{
    unsigned long flags;

    flags = 0;
    switch ((int)(sizeof(unsigned int))) {
    case 2:     break;
    case 4:     flags += 1 << 0;        break;
    case 8:     flags += 2 << 0;        break;
    default:    flags += 3 << 0;
    }
    switch ((int)(sizeof(unsigned long))) {
    case 2:     break;
    case 4:     flags += 1 << 2;        break;
    case 8:     flags += 2 << 2;        break;
    default:    flags += 3 << 2;
    }
    switch ((int)(sizeof(void *))) {
    case 2:     break;
    case 4:     flags += 1 << 4;        break;
    case 8:     flags += 2 << 4;        break;
    default:    flags += 3 << 4;
    }
    switch ((int)(sizeof(off_t))) {
    case 2:     break;
    case 4:     flags += 1 << 6;        break;
    case 8:     flags += 2 << 6;        break;
    default:    flags += 3 << 6;
    }
#ifdef DEBUG
    flags += 1 << 8;
#endif
#ifdef ZLIB_WINAPI
    flags += 1 << 10;
#endif
    /* The operating system must have modern and secure printf functions. */
    flags += 0L << 24;
    flags += 0L << 25;
    flags += 0L << 26;
    return flags;
}

#ifdef DEBUG

#  ifndef verbose
#    define verbose 0
#  endif
int ZLIB_INTERNAL z_verbose = verbose;

/* TODO. This should be a const char *. */
void ZLIB_INTERNAL z_error(char *m)
{
    fprintf(stderr, "%s\n", m);
    exit(1);
}
#endif

/* exported to allow conversion of error code to string for compress() and
 * uncompress()
 */
const char * ZEXPORT zError(int err)
{
    return ERR_MSG(err);
}

void* ZLIB_INTERNAL zcalloc(void *opaque, unsigned int items, unsigned int size)
{
    (void) opaque;
    return calloc((size_t) items, (size_t) size);
}

void ZLIB_INTERNAL zcfree(void *opaque, void *ptr)
{
    (void) opaque;
    free(ptr);
}
