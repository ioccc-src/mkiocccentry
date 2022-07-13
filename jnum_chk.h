/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jnum_chk - tool to check JSON number string conversions
 *
 * "Because the JSON co-founders minimalism is sub-minimal." :-)
 *
 * This JSON scanner was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and vomit on the JSON spec! :-)
 */


#if !defined(INCLUDE_JNUM_CHK_H)
#    define  INCLUDE_JNUM_CHK_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"


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
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] [-S]\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit 3\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-S\t\tstrict testing for all struct json_number elements\n"
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
static bool chk_test(int testnum, struct json_number *item, struct json_number *test, size_t len, bool strict);
static void check_val(bool *testp, char const *type, int testnum, bool size_a, bool size_b, intmax_t val_a, intmax_t val_b);
static void check_uval(bool *testp, char const *type, int testnum, bool size_a, bool size_b, uintmax_t val_a, uintmax_t val_b);
static void check_fval(bool *testp, char const *type, int testnum, bool size_a, bool size_b,
		       long double val_a, long double val_b, bool int_a, bool int_b, bool strict);
static void usage(int exitcode, char const *prog, char const *str, int expected, int argc) __attribute__((noreturn));


#endif /* INCLUDE_JNUM_CHK_H */
