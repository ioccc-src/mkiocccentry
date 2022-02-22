/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
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
#    define  INCLUDE_UTIL_H


#include <sys/types.h>


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
 * location/country codes
 */
struct location {
    const char * const code;		/* ISO 3166-1 Alpha-2 Code */
    const char * const name;		/* name (short name lower case) */
};

/*
 * definitions
 */
#define LITLEN(x) (sizeof(x)-1)	/* length of a literal string w/o the NUL byte */
#define INITIAL_BUF_SIZE (8192)	/* initial size of buffer allocated by read_all */
#define READ_ALL_CHUNK (65536)	/* grow this read_all by this amount when needed */
#define TAIL_TITLE_CHARS "abcdefghijklmnopqrstuvwxyz0123456789_+-"	/* [a-z0-9_+-] */

/*
 * paths to utilities the IOCCC tools use (including our own tools fnamchk,
 * txzchk, jinfochk and jauthchk)
 */
#define TAR_PATH_0 "/usr/bin/tar"		    /* historic path for tar */
#define TAR_PATH_1 "/bin/tar"			    /* alternate tar path for some systems where /usr/bin/tar != /bin/tar */
#define CP_PATH_0 "/bin/cp"			    /* historic path for cp */
#define CP_PATH_1 "/usr/bin/cp"			    /* alternate cp path for some systems where /bin/cp != /usr/bin/cp */
#define LS_PATH_0 "/bin/ls"			    /* historic path for ls */
#define LS_PATH_1 "/usr/bin/ls"			    /* alternate ls path for some systems where /bin/ls != /usr/bin/ls */
#define FNAMCHK_PATH_0 "./fnamchk"		    /* default path to fnamchk tool */
#define FNAMCHK_PATH_1 "/usr/local/bin/fnamchk"	    /* default path to fnamchk tool if installed */
#define TXZCHK_PATH_0 "./txzchk"		    /* default path to txzchk tool */
#define TXZCHK_PATH_1 "/usr/local/bin/txzchk"	    /* default path to txzchk tool if installed */
#define JAUTHCHK_PATH_0 "./jauthchk"		    /* default path to jauthchk tool */
#define JAUTHCHK_PATH_1 "/usr/local/bin/jauthchk"   /* default path to jauthchk tool if installed */
#define JINFOCHK_PATH_0 "./jinfochk"		    /* default path to jinfochk tool */
#define JINFOCHK_PATH_1 "/usr/local/bin/jinfochk"   /* default path to jinfochk tool if installed */

/*
 * global variables
 */
extern struct location loc[];		/* location/country codes */

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
extern off_t file_size(char const *path);
extern char *cmdprintf(char const *format, ...);
extern void para(char const *line, ...);
extern void fpara(FILE * stream, char const *line, ...);
extern ssize_t readline(char **linep, FILE * stream);
extern char *readline_dup(char **linep, bool strip, size_t *lenp, FILE * stream);
extern void find_utils(bool tar_flag_used, char **tar, bool cp_flag_used, char **cp,
		       bool ls_flag_used, char **ls, bool txzchk_flag_used, char **txzchk,
		       bool fnamchk_flag_used, char **fnamchk, bool jinfochk_flag_used,
		       char **jinfochk, bool jauthchk_flag_used, char **jauthchk);
extern off_t round_to_multiple(off_t num, off_t multiple);
extern char *malloc_json_encode(char const *ptr, size_t len, size_t *retlen);
extern char *malloc_json_str(char const *str, size_t *retlen);
extern void jencchk(void);
extern void *read_all(FILE *stream, size_t *psize);
extern char const * strnull(char const * const str);
extern long string_to_long(char const *str);
extern long long string_to_long_long(char const *str);
extern int string_to_int(char const *str);
extern unsigned long string_to_unsigned_long(char const *str);
extern unsigned long long string_to_unsigned_long_long(char const *str);
extern bool valid_contest_id(char *str);

#endif				/* INCLUDE_UTIL_H */
