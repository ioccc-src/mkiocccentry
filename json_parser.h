/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * ...and it is VERY incomplete! Right now all it does is read in strings and/or
 * files and attempts to parse them: parse as in the flex/bison parse: it does
 * not do anything beyond that. The grammar is not complete and there are no
 * actions yet.
 *
 * This is very much a work in progress that will be developed over some time.
 *
 */


#if !defined(INCLUDE_JSON_PARSER_H)
#    define  INCLUDE_JSON_PARSER_H



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
 * sanity - sanity checks on the IOCCC toolkit
 */
#include "sanity.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
bool msg_output_allowed = true;		/* false ==> disable output from msg() */
bool dbg_output_allowed = true;		/* false ==> disable output from dbg() */
bool warn_output_allowed = true;	/* false ==> disable output from warn() and warnp() */
bool err_output_allowed = true;		/* false ==> disable output from err() and errp() */
bool usage_output_allowed = true;	/* false ==> disable output from vfprintf_usage() */
bool output_newline = true;		/* true ==> -n not specified, output new line after each arg processed */
unsigned num_errors = 0;		/* > 0 number of errors encountered */
char const *program = NULL;	    /* our name */
static bool quiet = false;		    /* true ==> quiet mode */
static bool strict = false;		    /* true ==> be more restrictive on what's allowed */

/*
 * function prototypes
 */
void parse_file(char const *filename);
void parse_string(char const *string);
void print_newline(void);
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


void parse_file(char const *filename);
void parse_string(char const *string);

/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-T] [-s string] [-S] [file ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode, unless verbosity level > 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit 0\n"
    "\t-T\t\tshow IOCCC toolkit release repository tag\n"
    "\t-n\t\tdo not output newline after decode output\n"
    "\t-s\t\tread arg as a string\n"
    "\t-S\t\tdecode using strict mode (def: not strict)\n"
    "\n"
    "\t[file]\t\tread and parse file (def: parse stdin)\n"
    "\t\t\tNOTE: - means read from stdin\n"
    "\n"
    "jparse version: %s\n";


#endif /* INCLUDE_JSON_PARSER_H */
