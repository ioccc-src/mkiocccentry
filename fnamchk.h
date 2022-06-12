/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 *
 * "Because even fprintf has a return value worth paying attention to." :-)
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


#if !defined(INCLUDE_FNAMCHK_H)
#    define  INCLUDE_FNAMCHK_H


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
 * utf8_posix_map include
 */
#include "utf8_posix_map.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-E ext] [-t|-u] [-e] filepath\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-q\t\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\t-E ext\t\t\tchange extension to test (def: txz)\n"
    "\t-t\t\t\tfilename must match test entry filename\n"
    "\t-u\t\t\tfilename must match real entry filename\n"
    "\t-e\t\t\tsuppress error messages\n"
    "\n"
    "\t\tNOTE: -t and -u cannot be used together.\n\n"
    "\tfilepath\t\tpath to an IOCCC compressed tarball\n"
    "\n"
    "fnamchk version: %s\n";


/*
 * globals
 */


/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


#endif /* INCLUDE_FNAMCHK_H */
