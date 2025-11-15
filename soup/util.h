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


/*
 * utility macros
 *
 * These will work for our purposes but the singular or plural one is in truth
 * much more complicated than what we're making it seem like.
 */
#define SINGULAR_OR_PLURAL(x) ((x)==1?"":"s")
#define Y_OR_IES(x) ((x)==1?"y":"ies")
#define IS_OR_ARE(x) ((x)==1?"is":"are")


/*
 * external function declarations
 */
extern bool sum_and_count(intmax_t value, intmax_t *sump, intmax_t *countp, intmax_t *sum_checkp, intmax_t *count_checkp);
extern char *str_dup(char const *str);
extern bool is_empty(char const *path);
extern char *cmdprintf(char const *format, ...);
extern char *vcmdprintf(char const *format, va_list ap);
extern int shell_cmd(char const *name, bool flush_stdin, bool abort_on_error, char const *format, ...);
extern FILE *pipe_open(char const *name, bool write_mode, bool abort_on_error, char const *format, ...);


#endif				/* INCLUDE_UTIL_H */
