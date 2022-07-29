/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jstrdecode - tool to JSON decode JSON encoded strings
 *
 * "JSON: when a minimal design falls below a critical minimum." :-)
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


#if !defined(INCLUDE_JSTRDECODE_H)
#    define  INCLUDE_JSTRDECODE_H


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
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-t] [-n] [-Q] [string ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit 0\n"
    "\t-t\t\tperform jencchk test on code JSON decode/encode functions\n"
    "\t-n\t\tdo not output newline after decode output\n"
    "\t-Q\t\tenclose output in quotes (def: do not)\n"
    "\n"
    "\t[string ...]\tdecode strings on command line (def: read stdin)\n"
    "\t\t\tNOTE: - means read from stdin\n"
    "\n"
    "jstrdecode version: %s\n";


/*
 * globals
 */


/*
 * function prototypes
 */
static bool jstrdecode_stream(FILE *in_stream, FILE *out_stream, bool write_quote);
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


#endif /* INCLUDE_JSTRDECODE_H */
