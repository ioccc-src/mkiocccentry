/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jinfochk - IOCCC JSON .info.json checker and validator
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */

#ifndef JINFOCHK_H
#define JINFOCHK_H

#ifdef JINFOCHK_C
/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */

/*
 * jinfochk version
 */
#define JINFOCHK_VERSION "0.5 2022-02-19"	/* use format: major.minor YYYY-MM-DD */

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


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
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-V] [-q] [-s] [-F fnamchk] file\n"
"\n"
"\t-h\t\t\tprint help message and exit 0\n"
"\t-v level\t\tset verbosity level: (def level: %d)\n"
"\t-V\t\t\tprint version string and exit\n"
"\t-q\t\t\tquiet mode\n"
"\t-s\t\t\tstrict mode: be more strict on what is allowed (def: not strict)\n"
"\t-F /path/to/fnamchk\tpath to fnamchk tool (def: %s)\n"
"\n"
"\tfile\t\t\tpath to a .info.json file\n"
"\n"
"exit codes:\n"
"\t0\t\tno errors or warnings detected\n"
"\t>0\t\tsome error(s) and/or warning(s) were detected\n"
"\n"
"jinfochk version: %s\n";


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	    /* debug level set by -v */
char const *program = NULL;		    /* our name */
static bool quiet = false;		    /* true ==> quiet mode */
static struct info info;		    /* .info.json struct */
static bool strict = false;		    /* true ==> disallow anything before/after the '{' and '}' in the file */

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void sanity_chk(char const *file, char const *fnamchk);
static void check_info_json(char const *file, char const *fnamchk);


#endif /* JINFOCHK_C */
#endif /* JINFOCHK_H */
