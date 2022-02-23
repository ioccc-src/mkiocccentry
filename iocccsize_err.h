/*
 * iocccsize_err.h - debug, warning and error compat routines
 */


#if !defined(INCLUDE_IOCCCSIZE_ERR_H)
#    define  INCLUDE_IOCCCSIZE_ERR_H


#ifdef __cplusplus
extern "C" {
#endif


#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#if defined(MKIOCCCENTRY_USE)


#include "dbg.h"

#define iocccsize_warnx(...)	warn(__func__, __VA_ARGS__)

#define iocccsize_errx(ex, ...) { \
	err((ex), __func__, __VA_ARGS__); \
}

#define iocccsize_dbg(level, ...)   dbg(((level)+1), __VA_ARGS__)


#else /* MKIOCCCENTRY_USE */


/*
 * backward compatibility
 *
 * Not all compilers support __attribute__ nor do they support __has_builtin.
 * For example, MSVC, TenDRA and Little C Compiler doesn't support __attribute__.
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


#define iocccsize_warnx(...) { \
	(void) fprintf(stderr, __VA_ARGS__); \
	(void) fputc('\n', stderr); \
}

#define iocccsize_errx(ex, ...) { \
	(void) fprintf(stderr, __VA_ARGS__); \
	(void) fputc('\n', stderr); \
	exit(ex); \
}

#define iocccsize_dbg(level, ...) { \
	if (verbosity_level > (level)) { \
		(void) fprintf(stderr, __VA_ARGS__); \
		(void) fputc('\n', stderr); \
	} \
}


#endif /* MKIOCCCENTRY_USE */


#ifdef  __cplusplus
}
#endif


#endif /* INCLUDE_IOCCCSIZE_ERR_H */
