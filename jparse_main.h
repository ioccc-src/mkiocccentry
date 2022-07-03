/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jparse_main - tool that parses a block of JSON input
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 */


#if !defined(INCLUDE_JPARSE_MAIN_H)
#    define  INCLUDE_JPARSE_MAIN_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * jparse - JSON parser demo tool
 */
#include "jparse.h"

/*
 * official jparse version
 */
#define JPARSE_VERSION "0.9 2022-07-02"		/* format: major.minor YYYY-MM-DD */


/*
 * static functions
 */
static void usage(int exitcode, char const *str, char const *prog);

/*
 * usage message
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-q] [-V] [-s] arg\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit 0\n"
    "\t-s\t\targ is a string (def: arg is a filename)\n"
    "\n"
    "\targ\t\tparse JSON for string (if -s), file (w/o -s), or stdin (if arg is -)\n"
    "\n"
    "JSON parser version: %s\n"
    "jparse version: %s\n";


#endif /* INCLUDE_JPARSE_MAIN_H */
