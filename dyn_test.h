/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * dyn_array - dynamic array facility
 *
 * Copyright (c) 2014,2015,2022 by Landon Curt Noll.  All Rights Reserved.
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


#if !defined(INCLUDE_DYN_TEST_H)
#    define  INCLUDE_DYN_TEST_H


/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * official versions
 */
#define DYN_TEST_VERSION "1.7 2022-07-04"

/*
 * dyn_array - dynamic array facility
 */
#include "dyn_array.h"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V]\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit 2\n"
    "\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\ttest suite failed\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    "    >=4\tinternal error\n"
    "\n"
    "dyn_array library version: %s\n"
    "dyn_test version: %s\n";


/*
 * external allocation functions
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


#endif		/* INCLUDE_DYN_TEST_H */
