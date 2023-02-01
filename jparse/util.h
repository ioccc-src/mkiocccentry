/*
 * util - entry common utility functions for the IOCCC toolkit
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
#    define  INCLUDE_UTIL_H


#include <sys/types.h>
#include <float.h> /* for long doubles */
#include <inttypes.h> /* uintmax_t and intmax_t and perhaps SIZE_MAX */
#include <string.h> /* for strcmp */


/*
 * standard truth :-)
 */
#if !defined(BOOL_IS_DEFINED)
#define BOOL_IS_DEFINED
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
 * dyn_array - dynamic array facility
 */
#include "../dyn_array/dyn_array.h"


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
#define INITIAL_BUF_SIZE (8192)	/* initial size of buffer allocated by read_all */
#define READ_ALL_CHUNK (65536)	/* grow this read_all by this amount when needed */
#define LLONG_MAX_BASE10_DIGITS (19) /* for string to int functions */
#define TBLLEN(x) (sizeof(x)/sizeof((x)[0]))	/* number of elements in an initialized table array */
#define UNUSED_ARG(x) (void)(x)			/* prevent compiler from complaining about an unused arg */

/*
 * byte as octet constants
 */
#define BITS_IN_BYTE (8)	    /* assume 8 bit bytes */
#define MAX_BYTE (0xff)		    /* maximum byte value */
#define BYTE_VALUES (MAX_BYTE+1)    /* number of different combinations of bytes */


/*
 * paths to utilities the IOCCC tools use (including our own tools fnamchk,
 * txzchk, chkentry, etc.)
 */
#define TAR_PATH_0 "/usr/bin/tar"		    /* historic path for tar */
#define TAR_PATH_1 "/bin/tar"			    /* alternate tar path for some systems where /usr/bin/tar != /bin/tar */
#define CP_PATH_0 "/bin/cp"			    /* historic path for cp */
#define CP_PATH_1 "/usr/bin/cp"			    /* alternate cp path for some systems where /bin/cp != /usr/bin/cp */
#define LS_PATH_0 "/bin/ls"			    /* historic path for ls */
#define LS_PATH_1 "/usr/bin/ls"			    /* alternate ls path for some systems where /bin/ls != /usr/bin/ls */
#define FNAMCHK_PATH_0 "./test_ioccc/fnamchk"	    /* default path to fnamchk tool */
#define FNAMCHK_PATH_1 "/usr/local/bin/fnamchk"	    /* default path to fnamchk tool if installed */
#define TXZCHK_PATH_0 "./txzchk"		    /* default path to txzchk tool */
#define TXZCHK_PATH_1 "/usr/local/bin/txzchk"	    /* default path to txzchk tool if installed */
#define CHKENTRY_PATH_0 "./chkentry"		    /* default path to chkentry tool */
#define CHKENTRY_PATH_1 "/usr/local/bin/chkentry"   /* default path to chkentry tool if installed */
#define JPARSE_PATH_0 "./jparse/jparse"		    /* default path to jparse */
#define JPARSE_PATH_1 "/usr/local/bin/jparse"	    /* default path to jparse tool if installed */


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
extern char *base_name(char const *path);
extern bool exists(char const *path);
extern bool is_file(char const *path);
extern bool is_exec(char const *path);
extern bool is_dir(char const *path);
extern bool is_read(char const *path);
extern bool is_write(char const *path);
extern bool is_open_stream(FILE *stream);
extern off_t file_size(char const *path);
extern char *cmdprintf(char const *format, ...);
extern char *vcmdprintf(char const *format, va_list ap);
extern int shell_cmd(char const *name, bool abort_on_error, char const *format, ...);
extern FILE *pipe_open(char const *name, bool abort_on_error, char const *format, ...);
extern void para(char const *line, ...);
extern void fpara(FILE * stream, char const *line, ...);
extern void vfpr(FILE *stream, char const *name, char const *fmt, va_list ap);
extern void fpr(FILE *stream, char const *name, char const *fmt, ...)
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void pr(char const *name, char const *fmt, ...)
	__attribute__((format(printf, 2, 3)));		/* 2=format 3=params */
extern ssize_t readline(char **linep, FILE * stream);
extern char *readline_dup(char **linep, bool strip, size_t *lenp, FILE * stream);
extern off_t round_to_multiple(off_t num, off_t multiple);
extern void jencchk(void);
extern void *read_all(FILE *stream, size_t *psize);
extern bool is_string(char const * const ptr, size_t len);
extern char const *strnull(char const * const str);
extern long string_to_long(char const *str);
extern long long string_to_long_long(char const *str);
extern intmax_t string_to_intmax(char const *str);
extern bool string_to_intmax2(char const *str, intmax_t *ret);
extern int string_to_int(char const *str);
extern unsigned long string_to_unsigned_long(char const *str);
extern unsigned long long string_to_unsigned_long_long(char const *str);
extern uintmax_t string_to_uintmax(char const *str);
extern long double string_to_float(char const *str);
extern int parse_verbosity(char const *program, char const *arg);
extern bool is_decimal(char const *ptr, size_t len);
extern bool is_decimal_str(char const *str, size_t *retlen);
extern bool posix_plus_safe(char const *str, bool lower_only, bool slash_ok, bool first);
extern void posix_safe_chk(char const *str, size_t len, bool *slash, bool *posix_safe,
			   bool *first_alphanum, bool *upper);
extern void clearerr_or_fclose(FILE *stream);
extern ssize_t fprint_line_buf(FILE *stream, const void *buf, size_t len, int start, int end);
extern ssize_t fprint_line_str(FILE *stream, char *str, size_t *retlen, int start, int end);
extern char *malloc_path(char const *dirname, char const *filename);

extern size_t count_char(char const *str, int ch);

/* find non-whitespace text */
extern size_t find_text(char const *ptr, size_t len, char **first);
extern size_t find_text_str(char const *str, char **first);

/* other utility functions */
extern bool sum_and_count(intmax_t value, intmax_t *sump, intmax_t *countp, intmax_t *sum_checkp, intmax_t *count_checkp);

/* for getopt() invalid option or missing option argument */
extern void check_invalid_option(char const *prog, int ch, int opt);

#endif				/* INCLUDE_UTIL_H */
