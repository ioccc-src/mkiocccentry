/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chkauth - IOCCC JSON .author.json checker and validator. Invoked by
 * mkiocccentry after the .author.json file has been created but prior to
 * forming the tarball. The .author.json file is not formed if the chkinfo tool
 * fails to validate the .info.json file.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * NOTE: This tool and chkinfo is (and are) very much a work(s) in progress and
 * as of 10 March 2022 it was decided that the parsing should be done via
 * flex(1) and bison(1) which will require some time and thought. In time the
 * two tools will be merged into one which can parse one or both of .author.json
 * and/or .info.json. This is because some fields MUST be the same value in
 * both files.
 *
 * Additionally there will likely be a jparse tool that will take a block of
 * memory from either stdin or a file and attempt to parse it as json.
 */



#if !defined(INCLUDE_CHKAUTH_H)
#    define  INCLUDE_CHKAUTH_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "entry_util.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * sanity - perform common IOCCC sanity checks
 */
#include "sanity.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * chk_util - support chkinfo and chkauth services
 */
#include "chk_util.h"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-V] [-q] [-F fnamchk] [-t] [-W code] [-w] file\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-q\t\tquiet mode (def: not quiet)\n"
"\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
"\t-V\t\tprint version string and exit\n"
"\t-F fnamchk\tpath to fnamchk tool (def: %s)\n"
"\t-t\t\ttest mode: only issue warnings in some cases\n"
"\t-W code\t\tAdd code to the list of JSON error code to ignore\n"
"\t-w\t\tshow detailed warnings\n"
"\n"
"\tfile\t\tpath to a .author.json file\n"
"\n"
"exit codes:\n"
"\t0\t\tno errors or warnings detected\n"
"\t>0\t\tsome error(s) and/or warning(s) were detected\n"
"\n"
"chkauth version: %s\n";


/*
 * globals
 */
static char const *program = NULL;			/* our name */
static char *program_basename = NULL;			/* our basename */
static bool test = false;				/* true ==> some tests are not performed */
static struct json_field *found_author_json_fields;	/* list of fields specific to .author.json found */
extern struct json_field author_json_fields[];
extern size_t SIZEOF_AUTHOR_JSON_FIELDS_TABLE;		/* number of elements in the author_json_fields table */

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void chkauth_sanity_chks(char const *file, char const *fnamchk);
static int check_author_json(char const *file, char const *fnamchk);
static struct json_field *add_found_author_json_field(char const *json_filename, char const *name, char const *val, int line_num);
static bool add_author_json_field(char const *file, char *name, char *val, int line_num);
static int check_found_author_json_fields(char const *json_filename, bool test);
static void free_found_author_json_fields(void);


#endif /* INCLUDE_CHKAUTH_H */
