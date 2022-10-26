/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll.
 * The concept of the JSON semantics table, which this tool will use, was
 * developed by Landon Curt Noll.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


#if !defined(INCLUDE_CHKENTRY_H)
#    define  INCLUDE_CHKENTRY_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * jparse - the parser
 */
#include "jparse.h"

/*
 * json_sem - JSON semantics support
 */
#include "json_sem.h"

/*
 * chk_sem_info - check .info.json semantics
 */
#include "chk_sem_info.h"

/*
 * chk_sem_auth - check .author.json semantics
 */
#include "chk_sem_auth.h"

/*
 * foo - bar
 */
#include "foo.h"

/*
 * sanity - because foo is insane (because Cody Boone Ferguson is a bit insane! :-) ) :-)
 *
 * Actually we need this because we use it to find the path to fnamchk but it
 * can't be denied that foo is insane :-)
 *
 * NB: sanity.h will include files that we need so we could actually get rid of
 * including util.h and dbg.h but if sanity.h ever changes this would be a
 * problem so we will still include them above.
 */
#include "sanity.h"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] entry_dir\n"
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] info.json author.json\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\n"
    "entry_dir\tIOCCC entry directory with .info.json and author.json files\n"
    "info.json\tcheck info.json file, . ==> skip IOCCC info check\n"
    "author.json\tcheck author.json file, . ==> skip IOCCC author check\n"
    "\n"
    "Exit codes:\n"
    "     0   all is OK\n"
    "     1   JSON semantic check failed\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   command line error\n"
    "  >=10   internal error\n"
    "\n"
    "chkentry version: %s\n";


/*
 * globals
 */
bool quiet = false;				/* true ==> quiet mode */


/*
 * externals
 */

/*
 * function prototypes
 */

#endif /* INCLUDE_CHKENTRY_H */
