/*
 * dbg - debug, warning and error reporting facility
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

#if !defined(INCLUDE_DBG_H)
#    define INCLUDE_DBG_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>


/*
 * NOTREACHED
 *
 * In the old days of lint, one could give lint and friends a hint by
 * placing the token NOTREACHED immediately between opening and closing
 * comments.  Modern compilers do not honor such commented tokens
 * and instead rely on features such as __builtin_unreachable
 * and __attribute__((noreturn)).
 *
 * The NOTREACHED will either yield a __builtin_unreachable() feature call,
 * or it will call abort from stdlib.
 */
#if !defined __has_builtin
#    define __has_builtin(x) 0
#endif
#if __has_builtin(__builtin_unreachable)
#    define NOTREACHED __builtin_unreachable()
#else
#    define NOTREACHED abort()
#endif /* __has_builtin(__builtin_unreachable) */


#define DBG_NONE (0)		/* no debugging */
#define DBG_LOW (1)		/* minimal debugging */
#define DBG_MED (3)		/* somewhat more debugging */
#define DBG_HIGH (5)		/* verbose debugging */
#define DBG_VHIGH (7)		/* very verbose debugging */
#define DBG_VVHIGH (9)		/* very very verbose debugging */
#define DBG_VVVHIGH (11)	/* very very very verbose debugging */

#define DBG_DEFAULT (DBG_NONE)	/* default debugging level */

#define FORCED_EXIT (255)	/* exit(255) on bad exit code */
#define DO_NOT_EXIT (-1)		/* do not let vfprintf_usage() exit */


/*
 * external variables
 */
extern int verbosity_level;		/* print debug messages <= verbosity_level */


/*
 * external function declaractions
 */
extern void msg(const char *fmt, ...) \
	__attribute__((format(printf, 1, 2)));		/* 1=format 2=params */
extern void dbg(int level, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void warn(const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void warnp(const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void err(int exitcode, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 3, 4)));	/* 3=format 4=params */
extern void errp(int exitcode, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 3, 4)));	/* 3=format 4=params */
extern void vfprintf_usage(int exitcode, FILE *stream, const char *fmt, ...) \
	 __attribute__((format(printf, 3, 4)));					/* 3=format 4=params */

#endif				/* INCLUDE_DBG_H */
