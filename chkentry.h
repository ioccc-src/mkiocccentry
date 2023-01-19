/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * This tool and the JSON parser were co-developed in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * The concept of the JSON semantics tables was developed by Landon Curt Noll.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_CHKENTRY_H)
#    define  INCLUDE_CHKENTRY_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg/dbg.h"

/*
 * jparse - the parser
 */
#include "jparse/jparse.h"

/*
 * soup - some delicious IOCCC soup recipes :-)
 */
#include "soup/soup.h"

/*
 * foo - bar
 */
#include "soup/foo.h"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] entry_dir\n"
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] info.json auth.json\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\n"
    "\tentry_dir\tIOCCC entry directory with .info.json and auth.json files\n"
    "\tinfo.json\tcheck info.json file, . ==> skip IOCCC .info.json style check\n"
    "\tauth.json\tcheck auth.json file, . ==> skip IOCCC .auth.json style check\n"
    "\n"
    "Exit codes:\n"
    "     0   all is OK\n"
    "     1   JSON files(s) are valid JSON, however one or more semantic checks failed\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   command line error\n"
    "     4   file(s) were not valid JSON, no semantic checks were performed\n"
    "  >=10   internal error\n"
    "\n"
    "chkentry version: %s\n";


/*
 * globals
 */
static bool quiet = false;				/* true ==> quiet mode */


/*
 * externals
 */

/*
 * function prototypes
 */

#endif /* INCLUDE_CHKENTRY_H */
