/*
 * util - common utility functions
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_UTIL_H)
#    define  INCLUDE_UTIL_H

#include <float.h>      /* for long doubles */
#include <inttypes.h>   /* uintmax_t and intmax_t and perhaps SIZE_MAX */
#include <string.h>     /* for strcmp */
#include <sys/types.h>  /* various things */
#include <sys/stat.h>   /* for stat(2) and others */
#include <fts.h>        /* FTS and FTSENT */
#include <limits.h>	/* for CHAR_BIT */


/*
 * dbg - info, debug, warning, error, and usage message facility
 * dyn_array - dynamic array facility
 */
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"

#if 0	/* XXX - pre-IOCCC29: remove the obsolete #if block below - XXX */
/*
 * byte as octet constants
 */
#if !defined(CHAR_BIT)
# define CHAR_BIT (8)		/* paranoia - in case limits.h is very old */
#endif

/*
 * definitions
 */
#define LITLEN(x) (sizeof(x)-1)	/* length of a literal string w/o the NUL byte */
#define TBLLEN(x) (sizeof(x)/sizeof((x)[0]))	/* number of elements in an initialized table array */
#define UNUSED_ARG(x) (void)(x)			/* prevent compiler from complaining about an unused arg */

/*
 * invalid exit codes (values < 0) that may be returned by shell_cmd()
 */
#define EXIT_CALLOC_FAILED (-2)		/* invalid exit code - calloc() failure */
#define EXIT_SYSTEM_FAILED (-3)		/* invalid exit code - system() failed - returned exit 127 */
#define EXIT_FFLUSH_FAILED (-4)		/* invalid exit code - fflush() failed */
#define EXIT_NULL_ARGS (-5)		/* invalid exit code - function called with a NULL arg */


/*
 * fpr() and pr() related macros
 */
#define fprint(stream, fmt, ...) fpr((stream), __func__, (fmt), __VA_ARGS__)
#define fprstr(stream, fmt) fpr((stream), __func__, (fmt))
/**/
#define print(fmt, ...) pr(__func__, (fmt), __VA_ARGS__)
#define prstr(fmt) pr(__func__, (fmt))

/*
 * bit macros
 */
#define IS_SET(flag,bit)  ((flag) & (bit))
#define SET_BIT(var,bit)  ((var) |= ((uintmax_t)bit))
#define REMOVE_BIT(var,bit)  ((var) &= ~((uintmax_t)bit))
#define TOGGLE_BIT(var,bit) ((var)) = ((var)) ^ ((uintmax_t)bit)

#define LSET_BIT(var,bit)  ((var) | ((uintmax_t)bit))
#define LREMOVE_BIT(var,bit)  ((var) & ~((uintmax_t)bit))
#define LTOGGLE_BIT(var,bit) ((var) ^ ((uintmax_t)bit))
#endif	/* XXX - pre-IOCCC29: remove the obsolete #if block above - XXX */

/*
 * external function declarations
 */
extern char *str_dup(char const *str);
/* XXX - pre-IOCCC29: remove the obsolete function below - XXX */
extern bool sum_and_count(intmax_t value, intmax_t *sump, intmax_t *countp, intmax_t *sum_checkp, intmax_t *count_checkp);
extern bool is_empty(char const *path);
extern char *cmdprintf(char const *format, ...);
extern char *vcmdprintf(char const *format, va_list ap);
extern int shell_cmd(char const *name, bool flush_stdin, bool abort_on_error, char const *format, ...);
extern FILE *pipe_open(char const *name, bool write_mode, bool abort_on_error, char const *format, ...);

#endif				/* INCLUDE_UTIL_H */
