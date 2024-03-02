/*
 * dbg - info, debug, warning, error, and usage message facility
 *
 * Copyright (c) 1989,1997,2018-2024 by Landon Curt Noll.  All Rights Reserved.
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
#define DBG_VERSION "3.0 2023-08-05"		/* format: major.minor YYYY-MM-DD */


/*
 * standard truth :-)
 */
#if !defined(BOOL_IS_DEFINED)
#define BOOL_IS_DEFINED
#if !defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
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
 * Not all compilers have __has_builtin
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
#define DBG_VVVVVHIGH (15)	/* very very very very very verbose debugging */

#define DBG_DEFAULT (DBG_NONE)	/* default debugging level */
#define DBG_INVALID (-1)	/* invalid debug level - returned by parse_verbosity() on error */

#define FORCED_EXIT (255)	/* exit(255) on bad exit code */
#define DO_NOT_EXIT (-1)	/* do not let the usage printing function exit */


/*
 * global message control variables
 *
 * NOTE: These variables are initialized to defaults in dbg.c.
 */
extern int verbosity_level;		/* maximum debug level for debug messages */
extern bool msg_output_allowed;		/* false ==> disable informational messages */
extern bool dbg_output_allowed;		/* false ==> disable debug messages */
extern bool warn_output_allowed;	/* false ==> disable warning messages */
extern bool err_output_allowed;		/* false ==> disable error messages */
extern bool usage_output_allowed;	/* false ==> disable usage messages */
extern bool msg_warn_silent;		/* true ==> silence info & warnings if verbosity_level <= 0 */
extern const char *const dbg_version;	/* library version format: major.minor YYYY-MM-DD */


/*
 * external function declarations
 */

extern bool msg_allowed(void);
extern bool dbg_allowed(int level);
extern bool warn_allowed(void);
extern bool err_allowed(void);
extern bool usage_allowed(void);

extern void msg(const char *fmt, ...) \
	__attribute__((format(printf, 1, 2)));		/* 1=format 2=params */
extern void vmsg(char const *fmt, va_list ap);
extern void fmsg(FILE *stream, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void vfmsg(FILE *stream, char const *fmt, va_list ap);
extern void snmsg(char *str, size_t size, char const *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vsnmsg(char *str, size_t size, char const *fmt, va_list ap);

extern void dbg(int level, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void vdbg(int level, char const *fmt, va_list ap);
extern void fdbg(FILE *stream, int level, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vfdbg(FILE *stream, int level, char const *fmt, va_list ap);
extern void sndbg(char *str, size_t size, int level, char const *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vsndbg(char *str, size_t size, int level, char const *fmt, va_list ap);

extern void warn(const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void vwarn(char const *name, char const *fmt, va_list ap);
extern void fwarn(FILE *stream, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vfwarn(FILE *stream, char const *name, char const *fmt, va_list ap);
extern void snwarn(char *str, size_t size, char const *name, char const *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vsnwarn(char *str, size_t size, char const *name, char const *fmt, va_list ap);

extern void warnp(const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void vwarnp(char const *name, char const *fmt, va_list ap);
extern void fwarnp(FILE *stream, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vfwarnp(FILE *stream, char const *name, char const *fmt, va_list ap);
extern void snwarnp(char *str, size_t size, char const *name, char const *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vsnwarnp(char *str, size_t size, char const *name, char const *fmt, va_list ap);

extern void err(int exitcode, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 3, 4))); /* 3=format 4=params */
extern void verr(int exitcode, char const *name, char const *fmt, va_list ap) \
	   __attribute__((noreturn));
extern void ferr(int exitcode, FILE *stream, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 4, 5))); /* 4=format 5=params */
extern void vferr(int exitcode, FILE *stream, char const *name, char const *fmt, va_list ap) \
	   __attribute__((noreturn));

extern void errp(int exitcode, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 3, 4))); /* 3=format 4=params */
extern void verrp(int exitcode, char const *name, char const *fmt, va_list ap) \
	   __attribute__((noreturn));
extern void ferrp(int exitcode, FILE *stream, const char *name, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 4, 5))); /* 4=format 5=params */
extern void vferrp(int exitcode, FILE *stream, char const *name, char const *fmt, va_list ap) \
	   __attribute__((noreturn));

extern void werr(int error_code, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vwerr(int error_code, char const *name, char const *fmt, va_list ap);
extern void fwerr(int error_code, FILE *stream, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vfwerr(int error_code, FILE *stream, char const *name, char const *fmt, va_list ap);
extern void snwerr(int error_code, char *str, size_t size, char const *name, char const *fmt, ...) \
	__attribute__((format(printf, 5, 6)));		/* 5=format 6=params */
extern void vsnwerr(int error_code, char *str, size_t size, char const *name, char const *fmt, va_list ap);

extern void werrp(int error_code, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vwerrp(int error_code, char const *name, char const *fmt, va_list ap);
extern void fwerrp(int error_code, FILE *stream, const char *name, const char *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vfwerrp(int error_code, FILE *stream, char const *name, char const *fmt, va_list ap);
extern void snwerrp(int error_code, char *str, size_t size, char const *name, char const *fmt, ...) \
	__attribute__((format(printf, 5, 6)));		/* 5=format 6=params */
extern void vsnwerrp(int error_code, char *str, size_t size, char const *name, char const *fmt, va_list ap);

extern void warn_or_err(int exitcode, const char *name, bool warning, const char *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vwarn_or_err(int exitcode, const char *name, bool warning,
			 const char *fmt, va_list ap);
extern void fwarn_or_err(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, ...) \
	__attribute__((format(printf, 5, 6)));		/* 5=format 6=params */
extern void vfwarn_or_err(int exitcode, FILE *stream, const char *name, bool warning,
			 const char *fmt, va_list ap);

extern void warnp_or_errp(int exitcode, const char *name, bool warning, const char *fmt, ...) \
	__attribute__((format(printf, 4, 5)));		/* 4=format 5=params */
extern void vwarnp_or_errp(int exitcode, const char *name, bool warning,
			 const char *fmt, va_list ap);
extern void fwarnp_or_errp(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, ...) \
	__attribute__((format(printf, 5, 6)));		/* 5=format 6=params */
extern void vfwarnp_or_errp(int exitcode, FILE *stream, const char *name, bool warning,
			 const char *fmt, va_list ap);

extern void printf_usage(int exitcode, const char *fmt, ...) \
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern void vprintf_usage(int exitcode, char const *fmt, va_list ap);
extern void fprintf_usage(int exitcode, FILE *stream, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void vfprintf_usage(int exitcode, FILE *stream, char const *fmt, va_list ap);

extern int parse_verbosity(char const *optarg);

#endif				/* INCLUDE_DBG_H */
