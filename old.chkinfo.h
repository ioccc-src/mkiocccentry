/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chkinfo - IOCCC JSON .info.json checker and validator. Invoked by
 * mkiocccentry after the .info.json file has been created but prior to forming
 * the .author.json file, validating it with chkauth and then forming the
 * tarball.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * NOTE: This tool and chkauth is (and are) very much a work(s) in progress and
 * as of 10 March 2022 it was decided that the parsing should be done via
 * flex(1) and bison(1) which will require some time and thought. In time the
 * two tools will be merged into one which can parse one or both of .info.json
 * and/or .author.json. This is because some fields MUST be the same value in
 * both files.
 *
 * Additionally there will likely be a jparse tool that will take a block of
 * memory from either stdin or a file and attempt to parse it as json.
 */


#if !defined(INCLUDE_CHKINFO_H)
#    define  INCLUDE_CHKINFO_H


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


struct manifest_file
{
    char *filename;	    /* filename of the file */
    size_t count;	    /* number of times this filename is in the list */

    struct manifest_file *next;	    /* the next in the list */
};

static struct manifest_file *manifest_files_list; /* list of files in the manifest: to detect if any filenames are duplicates */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-q] [-V] [-F fnamchk] [-t] [-W code] [-w] file\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
"\t-V\t\tprint version string and exit\n"
"\t-F fnamchk\tpath to fnamchk tool (def: %s)\n"
"\t-t\t\ttest mode: only issue warnings in some cases\n"
"\t-W code\t\tAdd code to the list of JSON error code to ignore\n"
"\t-w\t\tshow detailed warnings\n"
"\n"
"\tfile\t\tpath to a .info.json file\n"
"\n"
"exit codes:\n"
"\t0\t\tno errors or warnings detected\n"
"\t>0\t\tsome error(s) and/or warning(s) were detected\n"
"\n"
"chkinfo version: %s\n";


/*
 * globals
 */
bool quiet = false;			    /* true ==> quiet mode */
/**/
static char const *program = NULL;	    /* our name */
static char *program_basename = NULL;	    /* our basename */
static struct info info;		    /* .info.json struct */
static bool test = false;		    /* true ==> some tests are not performed */
static struct json_field *found_info_json_fields; /* list of fields specific to .info.json that have been found */
extern struct json_field info_json_fields[];

extern size_t SIZEOF_INFO_JSON_FIELDS_TABLE;
/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void chkinfo_sanity_chks(char const *file, char const *fnamchk);
static int check_info_json(char const *file, char const *fnamchk);
static struct json_field *add_found_info_json_field(char const *json_filename, char const *name, char const *val, int line_num);
static bool add_info_json_field(char const *json_filename, char *name, char *val, int line_num);
static int check_found_info_json_fields(char const *json_filename, bool test);
static void free_found_info_json_fields(void);
static struct manifest_file *add_manifest_file(char const *filename);
static void free_manifest_files_list(void);
static void free_manifest_file(struct manifest_file *file);


#endif /* INCLUDE_CHKINFO_H */
