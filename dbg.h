/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
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
#    define  INCLUDE_DBG_H


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>


/*
 * definitions
 */
#define DBG_VERSION "1.10 2022-05-01"		/* format: major.minor YYYY-MM-DD */


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
/* booltostr - convert a boolean to a string */
#if !defined(booltostr)
#define booltostr(x) ((x) ? "true" : "false")
#endif
/* strtobool - convert a string to a boolean */
#if !defined(strtobool)
#define strtobool(x) ((x) != NULL && !strcmp((x), "true"))
#endif



/*
 * backward compatibility
 *
 * Not all compilers support __attribute__ nor do they support __has_builtin.
 * For example, MSVC, TenDRA and Little C Compiler don't support __attribute__.
 * Early gcc does not support __attribute__.
 *
 * Not all compiles have __has_builtin
 *
 * __func__ is not supported prior to C99.
 */
#if !defined(__attribute__) && \
    (defined(__cplusplus) || !defined(__GNUC__)  || __GNUC__ == 2 && __GNUC_MINOR__ < 8)
#    define __attribute__(A)
#endif
#if !defined __has_builtin
#    define __has_builtin(x) 0
#endif
#if (__STDC_VERSION__ < 199901L)
#    define __func__ __FILE__
#endif


/*
 * not_reached
 *
 * In the old days of lint, one could give lint and friends a hint by
 * placing the token NOTREACHED immediately between opening and closing
 * comments.  Modern compilers do not honor such commented tokens
 * and instead rely on features such as __builtin_unreachable
 * and __attribute__((noreturn)).
 *
 * The not_reached will either yield a __builtin_unreachable() feature call,
 * or it will call abort from stdlib.
 */
#if __has_builtin(__builtin_unreachable)
#    define not_reached() __builtin_unreachable()
#else
#    define not_reached() abort()
#endif /* __has_builtin(__builtin_unreachable) */


#define DBG_NONE (0)		/* no debugging */
#define DBG_LOW (1)		/* minimal debugging */
#define DBG_MED (3)		/* somewhat more debugging */
#define DBG_HIGH (5)		/* verbose debugging */
#define DBG_VHIGH (7)		/* very verbose debugging */
#define DBG_VVHIGH (9)		/* very very verbose debugging */
#define DBG_VVVHIGH (11)	/* very very very verbose debugging */
#define DBG_VVVVHIGH (13)	/* very very very very verbose debugging */

#define DBG_DEFAULT (DBG_NONE)	/* default debugging level */

#define FORCED_EXIT (255)	/* exit(255) on bad exit code */
#define DO_NOT_EXIT (-1)	/* do not let fprintf_usage() exit */


/*
 * global debugging variables
 *
 * NOTE: These variables are initialized to defaults in dbg.c.
 */
extern int verbosity_level;		/* print debug messages <= verbosity_level */
extern bool msg_output_allowed;		/* false ==> disable output from msg(), vmsg() */
extern bool dbg_output_allowed;		/* false ==> disable output from dbg(), vdbg() */
extern bool warn_output_allowed;	/* false ==> disable output from warn(), vwarn(), warnp(), vwarnp */
extern bool err_output_allowed;		/* false ==> disable output from err(), verr(), errp(), verrp(), */
					/*				 werr(), vwerr(), werrp(), vwerrp() */
extern bool usage_output_allowed;	/* false ==> disable output from fprintf_usage(), vfprintf_usage() */
extern bool msg_warn_silent;		/* true ==> silence msg(), vmsg(), warn(), vwarn(), */
					/*				 warnp(), vwarnp() if verbosity_level == 0 */


/*
 * external function declarations
 */
extern bool msg(const char *fmt, ...) \
	__attribute__((format(printf, 1, 2)));		/* 1=format 2=params */
extern bool vmsg(char const *fmt, va_list ap);

extern bool dbg(int level, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern bool vdbg(int level, char const *fmt, va_list ap);

extern bool warn(const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern bool vwarn(char const *name, char const *fmt, va_list ap);

extern bool warnp(const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern bool vwarnp(char const *name, char const *fmt, va_list ap);

extern void err(int exitcode, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 3, 4))); /* 3=format 4=params */
extern void verr(int exitcode, char const *name, char const *fmt, va_list ap);

extern void errp(int exitcode, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 3, 4))); /* 3=format 4=params */
extern void verrp(int exitcode, char const *name, char const *fmt, va_list ap);

extern bool werr(int error_code, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4))); /* 3=format 4=params */
extern bool vwerr(int error_code, char const *name, char const *fmt, va_list ap);

extern bool werrp(int error_code, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4))); /* 3=format 4=params */
extern bool vwerrp(int error_code, char const *name, char const *fmt, va_list ap);

extern void fprintf_usage(int exitcode, FILE *stream, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vfprintf_usage(int exitcode, FILE *stream, char const *fmt, va_list ap);

extern void warn_or_err(int exitcode, const char *name, bool test, const char *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vwarn_or_err(int exitcode, const char *name, bool test, const char *fmt, va_list ap);

extern void warnp_or_errp(int exitcode, const char *name, bool test, const char *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vwarnp_or_errp(int exitcode, const char *name, bool test, const char *fmt, va_list ap);


#endif				/* INCLUDE_DBG_H */
