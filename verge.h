/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * verge - determine if first version is >= second version
 *
 * "Because the JSON co-founders flawed minimalism is sub-minimal." :-)
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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


#if !defined(INCLUDE_VERGE_H)
#    define  INCLUDE_VERGE_H


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions and definitions
 */
#include "util.h"


/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * usage message
 *
 * Use the usage() function to print the these usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] major.minor.patch-1 major.minor.patch-2\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 3\n"
    "\t-v level\t\tset verbosity level (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit 3\n"
    "\n"
    "\tmajor.minor.patch-1\tfirst version  (example: 0.1.1)\n"
    "\tmajor.minor.patch-2\tsecond version (example: 1.3.2)\n"
    "\n"
    "Exit codes:\n"
    "    0\tfirst version >= second version\n"
    "    1\tfirst version < second version\n"
    "    2\tfirst and/or second version is an invalid version\n"
    "    3\t-h and help string printed or -V and version string printed\n"
    "    4\tcommand line error\n"
    "    >=5\tinternal error\n"
    "\n"
    "verge version: %s\n";


/*
 * globals
 */

/*
 * externals
 */

/*
 * function prototypes
 */
static size_t malloc_vers(char *str, long **pvers);
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


#endif /* INCLUDE_VERGE_H */
