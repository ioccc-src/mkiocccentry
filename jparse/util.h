/*
 * util - common utility functions for the JSON parser and tools
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


#if !defined(INCLUDE_JPARSE_UTIL_H)
#    define  INCLUDE_JPARSE_UTIL_H

#include <float.h>      /* for long doubles */
#include <inttypes.h>   /* uintmax_t and intmax_t and perhaps SIZE_MAX */
#include <string.h>     /* for strcmp */
#include <sys/types.h>  /* various things */
#include <sys/stat.h>   /* for stat(2) and others */
#include <fts.h>        /* FTS and FTSENT */
#include <fnmatch.h>    /* for fnmatch(3) (for ignored paths - if desired) */
#include <limits.h>	/* for CHAR_BIT */

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
  #include "../dbg/c_bool.h"
  #include "../dbg/c_compat.h"
  #include "../dbg/dbg.h"
#elif defined(INTERNAL_INCLUDE_2)
  #include "../dbg/c_bool.h"
  #include "../dbg/c_compat.h"
  #include "../dbg/dbg.h"
#else
  #include <c_bool.h>
  #include <c_compat.h>
  #include <dbg.h>
#endif

/*
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
  #include "../dyn_array/dyn_array.h"
#elif defined(INTERNAL_INCLUDE_2)
  #include "../dyn_array/dyn_array.h"
#else
  #include <dyn_array.h>
#endif

/*
 * pr - stdio helper library
 */
#if defined(INTERNAL_INCLUDE)
  #include "../pr/pr.h"
#elif defined(INTERNAL_INCLUDE_2)
  #include "../pr/pr.h"
#else
  #include <pr.h>
#endif


/*
 * byte as octet constants
 */
#if !defined(CHAR_BIT)
# define CHAR_BIT (8)		/* paranoia - in case limits.h is very old */
#endif
#define BITS_IN_BYTE (CHAR_BIT)	    /* assume 8 bit bytes */
#define MAX_BYTE (0xff)		    /* maximum byte value */
#define BYTE_VALUES (MAX_BYTE+1)    /* number of different combinations of bytes */

/*
 * off_t MAX and MIN
 */
#if !defined(OFF_MAX)
#define OFF_MAX (~((off_t)1 << (sizeof(off_t) * BITS_IN_BYTE - 1)))
#endif /* OFF_MAX */
#if !defined(OFF_MIN)
#define OFF_MIN (((off_t)1 << (sizeof(off_t) * BITS_IN_BYTE - 1)))
#endif /* OFF_MIN */

/*
 * size_t MAX and MIN
 */
#if !defined(SIZE_MAX)
#define SIZE_MAX (~((size_t)0))
#endif /* SIZE_MAX */
#if !defined(SIZE_MIN)
#define SIZE_MIN ((size_t)(0))
#endif /* SIZE_MIN */

/*
 * ssize_t MAX and MIN
 */
#if !defined(SSIZE_MAX)
#define SSIZE_MAX (~((ssize_t)1 << (sizeof(ssize_t) * BITS_IN_BYTE - 1)))
#endif /* SSIZE_MAX */
#if !defined(SSIZE_MIN)
#define SSIZE_MIN (((ssize_t)1 << (sizeof(ssize_t) * BITS_IN_BYTE - 1)))
#endif /* SSIZE_MIN */

/*
 * definitions
 */
#define LITLEN(x) (sizeof(x)-1)	/* length of a literal string w/o the NUL byte */
#define TBLLEN(x) (sizeof(x)/sizeof((x)[0]))	/* number of elements in an initialized table array */
#define UNUSED_ARG(x) (void)(x)			/* prevent compiler from complaining about an unused arg */


/*
 * fpr() and pr() related macros
 */
#define fprint(stream, fmt, ...) fpr((stream), __func__, (fmt), __VA_ARGS__)
#define fprstr(stream, fmt) fpr((stream), __func__, (fmt))
/**/
#define print(fmt, ...) pr(__func__, (fmt), __VA_ARGS__)
#define prstr(fmt) pr(__func__, (fmt))


/*
 * utility macros for find_text functions
 */
#define is_all_text(buf, len) (find_text((buf), (len), NULL) == (len))
#define is_all_text_str(str) (is_all_text((str), strlen(str)))
#define is_all_whitespace(buf, len) (find_text((buf), (len), NULL) == 0)
#define is_all_whitespace_str(str) (is_all_whitespace((str), strlen(str)))

/*
 * non-strict floating match to 1 part in MATCH_PRECISION
 */
#define MATCH_PRECISION ((long double)(1<<22))


/*
 * external function declarations
 */
extern bool is_string(char const * const ptr, size_t len);
extern char const *strnull(char const * const str);
extern bool string_to_intmax(char const *str, intmax_t *ret);
extern bool string_to_uintmax(char const *str, uintmax_t *ret);
extern bool is_decimal(char const *ptr, size_t len);
extern bool is_decimal_str(char const *str, size_t *retlen);
extern bool is_floating_notation(char const *ptr, size_t len);
extern bool is_floating_notation_str(char const *str, size_t *retlen);
extern bool is_e_notation(char const *str, size_t len);
extern bool is_e_notation_str(char const *str, size_t *retlen);

/* find non-whitespace text */
extern size_t find_text(char const *ptr, size_t len, char **first);
extern size_t find_text_str(char const *str, char **first);

#endif				/* INCLUDE_JPARSE_UTIL_H */
