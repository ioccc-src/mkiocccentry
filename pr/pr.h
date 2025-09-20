/*
 * pr - stdio helper library
 *
 * "Small acts of kindness can fill the world with light."
 *
 *	-- J.R.R. Tolkien
 *
 * Copyright (c) 2008-2025 by Landon Curt Noll and Cody Boone Ferguson.  All rights reserved.
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
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * The code in this from repo was copied out of the mkiocccentry toolkit repo:
 *
 *	https://github.com/ioccc-src/mkiocccentry
 *
 * and out of the jparse repo:
 *
 *	https://github.com/xexyl/jparse
 *
 * The origin of libpr dates back to code written by Landon Curt Noll around 2008.
 *
 * That 2008 code was copied into the jparse repo, and the mkiocccentry toolkit repo
 * by Landon Curt Noll.  While in the jparse repo, both Landon Curt Noll and
 * Cody Boone Ferguson added to and improved this code base.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_PR_H)
#    define  INCLUDE_PR_H


/*
 * system includes
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <fcntl.h>


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/c_bool.h"
#include "../dbg/c_compat.h"
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"
#else
#include <c_bool.h>
#include <c_compat.h>
#include <dbg.h>
#include <dyn_array.h>
#endif


/*
 * official version
 */
#define PR_VERSION "1.0.0 2025-09-19"    /* format: major.minor YYYY-MM-DD */


/*
 * definitions
 */
#define LITLEN(x) (sizeof(x)-1)	/* length of a literal string w/o the NUL byte */
#define INITIAL_BUF_SIZE (8192)	/* initial size of buffer allocated by read_all */
#define READ_ALL_CHUNK (65536)	/* grow this read_all by this amount when needed */


/*
 * fpr() and pr() related macros
 */
#define fprint(stream, fmt, ...) fpr((stream), __func__, (fmt), __VA_ARGS__)
#define fprstr(stream, fmt) fpr((stream), __func__, (fmt))
/**/
#define print(fmt, ...) pr(__func__, (fmt), __VA_ARGS__)
#define prstr(fmt) pr(__func__, (fmt))


/*
 * global variables
 */
extern const char *const pr_version;	/* library version format: major.minor YYYY-MM-DD */


/*
 * external function declarations
 */
extern bool chk_stdio_printf_err(FILE *stream, int ret);
extern void para(char const *line, ...);
extern void fpara(FILE * stream, char const *line, ...);
extern void vfpr(FILE *stream, char const *name, char const *fmt, va_list ap);
extern void fpr(FILE *stream, char const *name, char const *fmt, ...)
        __attribute__((format(printf, 3, 4)));          /* 3=format 4=params */
extern void pr(char const *name, char const *fmt, ...)
        __attribute__((format(printf, 2, 3)));          /* 2=format 3=params */
extern ssize_t readline(char **linep, FILE * stream);
extern char *readline_dup(char **linep, bool strip, size_t *lenp, FILE * stream);
extern void *read_all(FILE *stream, size_t *psize);
extern void clearerr_or_fclose(FILE *stream);
extern ssize_t fprint_line_buf(FILE *stream, const void *buf, size_t len, int start, int end);
extern ssize_t fprint_line_str(FILE *stream, char *str, size_t *retlen, int start, int end);
extern FILE *open_dir_file(char const *dir, char const *file);


#endif				/* INCLUDE_PR_H */
