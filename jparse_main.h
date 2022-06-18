/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jparse_main - tool that parses a block of JSON input
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * NOTE: This is _very incomplete_ and right now all it does is reads from either
 * stdin, the command line or a file, running yyparse() (actually ugly_parse())
 * on it. The grammar is incomplete, there are some things that are allowed that
 * are not valid JSON and many other things need to be done.
 *
 * This is very much a work in progress!
 */


#if !defined(INCLUDE_JPARSE_MAIN_H)
#    define  INCLUDE_JPARSE_MAIN_H


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * jparse - JSON parser
 */
#include "jparse.h"

/*
 * official jparse version
 */
#define JPARSE_VERSION "0.8 2022-06-12"		/* format: major.minor YYYY-MM-DD */

/*
 * static functions
 */
static void usage(int exitcode, char const *str, char const *prog);

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-q] [-V] [-s string] [file ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t\t\tNOTE: You must specify this option before -s as -s is processed immediately\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit 0\n"
    "\t-s\t\tread arg as a string\n"
    "\n"
    "\t[file]\t\tread and parse file\n"
    "\t\t\tNOTE: - means read from stdin\n"
    "\n"
    "JSON parser version: %s\n"
    "jparse version: %s\n";


#endif /* INCLUDE_JPARSE_MAIN_H */
