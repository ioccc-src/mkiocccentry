#ifndef JAUTHCHK_H
#define JAUTHCHK_H

#ifdef JAUTHCHK_C


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions and definitions
 */
#include "util.h"


/*
 * json - json file structs
 */
#include "json.h"


/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/*
 * jauthchk version
 */
#define JAUTHCHK_VERSION "0.5 2022-02-19"	/* use format: major.minor YYYY-MM-DD */



/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-V] [-q] [-s] [-F fnamchk] file\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-V\t\tprint version string and exit\n"
"\t-q\t\tquiet mode\n"
"\t-s\t\tstrict mode: be more strict on what is allowed (def: not strict)\n"
"\t-F /path/to/fnamchk\tpath to fnamchk tool (def: %s)\n"
"\n"
"\tfile\t\tpath to a .author.json file\n"
"\n"
"exit codes:\n"
"\t0\t\tno errors or warnings detected\n"
"\t>0\t\tsome error(s) and/or warning(s) were detected\n"
"\n"
"jauthchk version: %s\n";


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	    /* debug level set by -v */
char const *program = NULL;		    /* our name */
static bool quiet = false;		    /* true ==> quiet mode */
static struct author author;		    /* the .author.json struct */
static bool strict = false;		    /* true ==> disallow anything before/after the '{' and '}' */

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void sanity_chk(char const *file, char const *fnamchk);
static void check_author_json(char const *file, char const *fnamchk);


#endif /* JAUTHCHK_C */
#endif /* JAUTHCHK_H */
