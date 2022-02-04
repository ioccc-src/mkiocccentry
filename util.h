/*
 * util - IOCCC entry common utility functions
 *
 * Copyright (c) 1989,1997,2018-2022 by Landon Curt Noll.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 */

#if !defined(INCLUDE_UTIL_H)
#    define INCLUDE_UTIL_H


/*
 * standard truth :-)
 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
/* have a C99 compiler - we should expect to have <stdbool.h> */
#include <stdbool.h>
#elif !defined(__cplusplus)
/* do not have a C99 compiler - fake a <stdbool.h> header file */
typedef unsigned char bool;
#undef true
#define true ((bool)(1))
#undef false
#define false ((bool)(0))
#endif

/*
 * external function declarations
 */
extern char *base_name(char const *path);
extern bool exists(char const *path);
extern bool is_file(char const *path);
extern bool is_exec(char const *path);
extern bool is_dir(char const *path);
extern bool is_read(char const *path);
extern bool is_write(char const *path);
extern ssize_t file_size(char const *path);
extern char *cmdprintf(char const *format, ...);

#endif				/* INCLUDE_UTIL_H */
