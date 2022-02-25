/*
 * jauthchk - IOCCC JSON .author.json checker and validator
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 */


#if !defined(INCLUDE_JAUTHCHK_H)
#    define  INCLUDE_JAUTHCHK_H


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
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-V] [-T] [-q] [-s] [-F fnamchk] [-t] file\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-V\t\tprint version string and exit\n"
"\t-T\t\t\tshow IOCCC toolset chain release repository tag\n"
"\t-q\t\tquiet mode\n"
"\t-s\t\tstrict mode: be more strict on what is allowed (def: not strict)\n"
"\t-F /path/to/fnamchk\tpath to fnamchk tool (def: %s)\n"
"\t-t\t\t\ttest mode: only issue warnings in some cases\n"
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
int verbosity_level = DBG_DEFAULT;		/* debug level set by -v */
char const *program = NULL;			/* our name */
char *program_basename = NULL;			/* our basename */
static bool quiet = false;			/* true ==> quiet mode */
static struct author author;			/* the .author.json struct */
static bool strict = false;			/* true ==> disallow anything before/after the '{' and '}' */
static bool test = false;			/* true ==> issue warnings instead of errors in some cases (N.B.: not yet used) */
static struct json_field *found_author_json_fields;	/* list of fields specific to .author.json found */
/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void sanity_chk(char const *file, char const *fnamchk);
static int check_author_json(char const *file, char const *fnamchk);
static struct json_field *add_found_author_json_field(char const *field, char const *value);
static void free_found_author_json_fields(void);

#endif /* INCLUDE_JAUTHCHK_H */
