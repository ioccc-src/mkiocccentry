/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because the JSON co-founders minimalism is sub-minimal." :-)
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


#if !defined(INCLUDE_CHKENTRY_H)
#    define  INCLUDE_CHKENTRY_H


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * util - utility functions and definitions
 */
#include "util.h"


/*
 * non-strict match to 1 part in MATCH_PRECISION
 */
#define MATCH_PRECISION ((long double)(1<<22))


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] [-F fnamchk] entry_dir\n"
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] [-F fnamchk] info.JSON author.JSON\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit 3\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-F fnamchk\tpath to fnamchk tool (def: %s)\n"
    "\n"
    "entry_dir\tIOCCC entry directory with .info.JSON and author.JSON files\n"
    "info.JSON\tcheck info.JSON file, . ==> skip IOCCC info check\n"
    "author.JSON\tcheck author.JSON file, . ==> skip IOCCC author check\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    3\t-h and help string printed or -V and version string printed\n"
    "    4\tcommand line error\n"
    "    >=5\tinternal error\n"
    "\n"
    "chkentry version: %s\n";


/*
 * globals
 */
bool quiet = false;				/* true ==> quiet mode */

/*
 * externals
 */

/*
 * function prototypes
 */
static void usage(int exitcode, char const *prog, char const *str, int expected, int argc) __attribute__((noreturn));


#endif /* INCLUDE_CHKENTRY_H */
