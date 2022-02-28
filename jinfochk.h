/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jinfochk - IOCCC JSON .info.json checker and validator
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 */


#if !defined(INCLUDE_JINFOCHK_H)
#    define  INCLUDE_JINFOCHK_H


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


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
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-V] [-T] [-q] [-s] [-F fnamchk] [-t] file\n"
"\n"
"\t-h\t\t\tprint help message and exit 0\n"
"\t-v level\t\tset verbosity level: (def level: %d)\n"
"\t-V\t\t\tprint version string and exit\n"
"\t-T\t\t\tshow IOCCC toolset chain release repository tag\n"
"\t-q\t\t\tquiet mode\n"
"\t-s\t\t\tstrict mode: be more strict on what is allowed (def: not strict)\n"
"\t-F /path/to/fnamchk\tpath to fnamchk tool (def: %s)\n"
"\t-t\t\t\ttest mode: only issue warnings in some cases\n"
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
char *program_basename = NULL;		    /* our basename */
static bool quiet = false;		    /* true ==> quiet mode */
static struct info info;		    /* .info.json struct */
static bool strict = false;		    /* true ==> disallow anything before/after the '{' and '}' in the file */
static bool test = false;		    /* true ==> some tests are not performed */
static struct json_field *found_info_json_fields; /* list of fields specific to .info.json that have been found */
extern struct json_field info_json_fields[];

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void sanity_chk(char const *file, char const *fnamchk);
static int check_info_json(char const *file, char const *fnamchk);
static struct json_field *add_found_info_json_field(char const *name, char const *val);
static int get_info_json_field(char const *file, char *name, char *val);
static int check_found_info_json_fields(char const *file, bool test);
static void check_info_json_fields_table(void);
static void free_found_info_json_fields(void);


#endif /* INCLUDE_JINFOCHK_H */
