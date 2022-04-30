/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jnum_chk - check JSON number string conversions
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


#if !defined(INCLUDE_JNUM_CHK_H)
#    define  INCLUDE_JNUM_CHK_H


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions and definitions
 */
#include "util.h"


/*
 * JSON functions supporting mkiocccentry code
 */
#include "json.h"


/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-q] [-S]\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit 2\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-S\t\tstrict testing for all struct number elements\n"
    "\t\t\t    (def: test only 8, 16, 32, 64 bit signed and unsigned integer types)\n"
    "\t\t\t    (def: test floating point with match to only 1 part in 4194304)\n"
    "\n"
    "\tNOTE: The -S mode is for information purposes only, and may fail\n"
    "\t      on your system due to hardware and/or other system differences.\n"
    "\t      The IOCCC mkiocccentry repo does not need -S to pass in order\n"
    "\t      to be able to create a valid IOCCC entry compressed tarball.\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\twithout -S given and JSON number conversion test suite failed\n"
    "    2\t-S given and JSON number conversion test suite failed\n"
    "    3\t-h and help string printed or -V and version string printed\n"
    "    4\tcommand line error\n"
    "    >=5\tinternal error\n"
    "\n"
    "jnum_chk version: %s\n";


/*
 * globals
 */
bool quiet = false;				/* true ==> quiet mode */

/*
 * externals
 */
extern int const test_count;			/* number of tests to perform */
extern char *test_set[];			/* test strings */
extern struct json_number test_result[];	/* struct integer conversions of test strings */

/*
 * function prototypes
 */
static bool chk_test(struct json *node, struct json_number *test, size_t len, bool strict);
static void usage(int exitcode, char const *prog, char const *str, int expected, int argc) __attribute__((noreturn));


#endif /* INCLUDE_JNUM_CHK_H */
