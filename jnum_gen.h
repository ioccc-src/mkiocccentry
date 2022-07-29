/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jnum_gen - generate JSON number string conversion test data
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
 * Share and enjoy vomiting on the JSON spec! :-)
 */


#if !defined(INCLUDE_JNUM_GEN_H)
#    define  INCLUDE_JNUM_GEN_H


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
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

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
    "usage: %s [-h] [-v level] [-V] [-q] filename\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit 3\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\n"
    "\tfilename\tfile containing JSON number test cases\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "	 1\tfilename does not exist or is not a readable file\n"
    "    3\t-h and help string printed or -V and version string printed\n"
    "    4\tcommand line error\n"
    "    >=5\tinternal error\n"
    "\n"
    "jnum_gen version: %s\n";


/*
 * globals
 */
bool quiet = false;			/* true ==> quiet mode */

/*
 * function prototypes
 */
static void fpr_number(FILE *stream, struct json_number *item);
static void fpr_info(FILE *stream, bool sized, intmax_t value, char const *scomm, char const *vcomm);
static void fpr_uinfo(FILE *stream, bool sized, uintmax_t value, char const *scomm, char const *vcomm);
static void fpr_finfo(FILE *stream, bool sized, long double value, bool intval, char const *scomm,
		      char const *vcomm, char const *sintval);
static void usage(int exitcode, char const *prog, char const *str, int expected, int argc) __attribute__((noreturn));


#endif /* INCLUDE_JNUM_GEN_H */
