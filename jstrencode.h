#ifndef JSTRENCODE_H
#define JSTRENCODE_H

#ifdef JSTRENCODE_C


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions and definitions
 */
#include "util.h"


/*
 * JSON functions supporting mkiocccentry code
 */
#include "json.h"


/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"



/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-t] [-n] [string ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level: (def level: %d)\n"
    "\t-V\t\tprint version string and exit 0\n"
    "\t-t\t\tperform jencchk test on code JSON encode/decode functions\n"
    "\t-n\t\tdo not output newline after encode output\n"
    "\n"
    "\t[string ...]\tencode strings on command line (def: read stdin)\n"
    "\n"
    "jstrencode version: %s\n";


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */


/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


#endif /* JSTRENCODE_C */
#endif /* JSTRENCODE_H */
