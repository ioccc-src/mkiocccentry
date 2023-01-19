/*
 * verge - determine if first version is greater or equal to the second version
 *
 * "Because too much minimalism can be sub-minimal." :-)
 *
 * This JSON parser was co-developed in 2022 by:
 *
 *	@xexyl
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


#if !defined(INCLUDE_VERGE_H)
#    define  INCLUDE_VERGE_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] major.minor.patch-1 major.minor.patch-2\n"
    "\n"
    "\t-h\t\t\tprint help message and exit\n"
    "\t-v level\t\tset verbosity level (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "\tmajor.minor.patch-1\tfirst version  (example: 0.1.1)\n"
    "\tmajor.minor.patch-2\tsecond version (example: 1.3.2)\n"
    "\n"
    "Exit codes:\n"
    "     0   first version >= second version\n"
    "     1   first version < second version\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   command line error\n"
    "     4   first or second version string is an invalid version\n"
    "  >=10   internal error\n"
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
static size_t allocate_vers(char *str, long **pvers);
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


#endif /* INCLUDE_VERGE_H */
