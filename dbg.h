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
#include <stdbool.h>
#include <errno.h>
#include <syslog.h>

extern int verbosity_level;		/* print debug messages <= verbosity_level */

/*
 * DEBUG_LINT - if defined, debug calls turn into fprintf to stderr calls
 *
 * The purpose of DEBUG_LINT is to let the C compiler do a fprintf format
 * argument count and type check against the debug function.  With DEBUG_LINT,
 * the debug macros are "faked" as best as we can.
 *
 * NOTE: Use of DEBUG_LINT is intended ONLY for static analysis (ala classic lint)
 * with compiler warnings.  DEBUG_LINT works best with -Wall.  In particular, it
 * won't help of you disable warnings that DEBUG_LINT would otherwise generate.
 *
 * When using DEBUG_LINT, consider compiling with:
 *
 *      -Wall -Werror -pedantic
 *
 * with at least the c11 standard.  As in:
 *
 *      -std=c11 -Wall -Werror -pedantic
 *
 * During DEBUG_LINT, output will be written to stderr.  These macros assume
 * that stderr is unbuffered.
 *
 * No error checking is performed by fprintf and fputc to stderr.  Such errors will overwrite
 * errno making the calls that perror print incorrect error messages.
 *
 * The DEBUG_LINT assumes the following global variables is declared elsewhere:
 *
 *      long int verbosity_level;            // dbg() verbosity cutoff level
 *
 * The DEBUG_LINT assumes the following file is included somewhere above the include of this file:
 *
 *      #include <stdlib.h>
 *
 * The DEBUG_LINT only works with compilers newer than 199901L (c11 or newer).
 * Defining DEBUG_LINT on an older compiler will be ignored in this file.
 * However when it comes to compiling debug.c, nothing will happen resulting
 * in a link error.  This is a feature, not a bug.  It tells you that your
 * compiler is too old to make use of DEBUG_LINT so don't use it.
 *
 * IMPORTANT NOTE:
 *
 * Executing code with DEBUG_LINT enabled is NOT recommended.
 * It might work, but don't count in it!
 *
 *  You are better off defining DEBUG_LINT in CFLAGS with, say:
 *
 *      -std=c11 -Wall -Werror -pedantic
 *
 * At a minimum, fix warnings (that turn into compiler errors) related to fprintf()
 * calls * until the program compiles.  For better results, fix ALL warnings as some
 * of those warnings may indicate the presence of bugs in your code including but
 * not limited to the use of debug functions.
 */

#    if defined(DEBUG_LINT) && __STDC_VERSION__ >= 199901L

#        define msg(...) fprintf(stderr, __VA_ARGS__)
#        define dbg(level, ...) \
		    ((verbosity_level >= (level)) ? \
			(fprintf(stderr, "Debug[%d]: ", (level)), \
			 printf(__VA_ARGS__)) : \
		     true)
#        define warn(name, ...) \
		    (fprintf(stderr, "Warning: %s: ", (name)), \
		     fprintf(stderr, __VA_ARGS__))
#        define warnp(name, ...) \
		    (fprintf(stderr, "Warning: %s: ", (name)), \
		     fprintf(stderr, __VA_ARGS__), \
		     fputc('\n', stderr), \
		     perror(__func__))
#        define err(exitcode, name, ...) \
		    (fprintf(stderr, "FATAL[%d]: %s: ", (exitcode), (name)), \
		     fprintf(stderr, __VA_ARGS__), \
		     exit(exitcode))
#        define errp(exitcode, name, ...) \
		    (fprintf(stderr, "FATAL[%d]: %s: ", (exitcode), (name)), \
		     fprintf(stderr, __VA_ARGS__), \
		     fputc('\n', stderr), \
		     perror(__func__), \
		     exit(exitcode))
#	 define vfprintf_usage(exitcode, stream, fmt, ...) \
		    (vfprintf((stream), (fmt), ...), \
		     (((exitcode) >= 0) ? exit(exitcode) : 1))

#    else			/* DEBUG_LINT && __STDC_VERSION__ >= 199901L */

extern void msg(const char *fmt, ...);
extern void dbg(int level, const char *fmt, ...);
extern void warn(const char *name, const char *fmt, ...);
extern void warnp(const char *name, const char *fmt, ...);
extern void err(int exitcode, const char *name, const char *fmt, ...);
extern void errp(int exitcode, const char *name, const char *fmt, ...);
extern void vfprintf_usage(int exitcode, FILE *stream, const char *fmt, ...);

#    endif			/* DEBUG_LINT && __STDC_VERSION__ >= 199901L */

#    define DBG_NONE (0)	/* no debugging */
#    define DBG_LOW (1)		/* minimal debugging */
#    define DBG_MED (3)		/* somewhat more debugging */
#    define DBG_HIGH (5)	/* verbose debugging */
#    define DBG_VHIGH (7)	/* very verbose debugging */
#    define DBG_VVHIGH (9)	/* very very verbose debugging */
#    define DBG_VVVHIGH (11)	/* very very very verbose debugging */
#    define FORCED_EXIT (255)	/* exit(255) on bad exit code */

#endif				/* INCLUDE_DBG_H */
