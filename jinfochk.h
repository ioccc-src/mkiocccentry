/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jinfochk - IOCCC JSON .info.json checker and validator. Invoked by
 * mkiocccentry after the .info.json file has been created but prior to forming
 * the .author.json file, validating it with jauthchk and then forming the
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
 * NOTE: This tool and jauthchk is (and are) very much a work(s) in progress and
 * as of 10 March 2022 it was decided that the parsing should be done via
 * flex(1) and bison(1) which will require some time and thought. In time the
 * two tools will be merged into one which can parse one or both of .info.json
 * and/or .author.json. This is because some fields MUST be the same value in
 * both files.
 *
 * Additionally there will likely be a jparse tool that will take a block of
 * memory from either stdin or a file and attempt to parse it as json.
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
 * sanity - sanity checks on the IOCCC toolkit
 */
#include "sanity.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

struct manifest_file {
    char *filename;	    /* filename of the file */
    size_t count;	    /* number of times this filename is in the list */

    struct manifest_file *next;	    /* the next in the list */
};

static struct manifest_file *manifest_files_list; /* list of files in the manifest: to detect if any filenames are duplicates */

/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-q] [-V] [-T] [-s] [-F fnamchk] [-t] [-W code] [-w] ... file\n"
"\n"
"\t-h\t\t\tprint help message and exit 0\n"
"\t-v level\t\tset verbosity level: (def level: %d)\n"
"\t-q\t\t\tquiet mode, unless verbosity level > 0 (def: not quiet)\n"
"\t-V\t\t\tprint version string and exit\n"
"\t-T\t\t\tshow IOCCC toolkit chain release repository tag\n"
"\t-s\t\t\tstrict mode: be more strict on what is allowed (def: not strict)\n"
"\t-F /path/to/fnamchk\tpath to fnamchk tool (def: %s)\n"
"\t-t\t\t\ttest mode: only issue warnings in some cases\n"
"\t-W code\t\t\tAdd code to the list of JSON error code to ignore\n"
"\t-w\t\t\tshow detailed warnings\n"
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
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
bool msg_output_allowed = true;		/* false ==> disable output from msg() */
bool dbg_output_allowed = true;		/* false ==> disable output from dbg() */
bool warn_output_allowed = true;	/* false ==> disable output from warn() and warnp() */
bool err_output_allowed = true;		/* false ==> disable output from err() and errp() */
bool usage_output_allowed = true;	/* false ==> disable output from vfprintf_usage() */
static char const *program = NULL;	    /* our name */
static char *program_basename = NULL;	    /* our basename */
static bool quiet = false;		    /* true ==> quiet mode */
static struct info info;		    /* .info.json struct */
static bool strict = false;		    /* true ==> disallow anything before/after the '{' and '}' in the file */
static bool test = false;		    /* true ==> some tests are not performed */
static struct json_field *found_info_json_fields; /* list of fields specific to .info.json that have been found */
extern struct json_field info_json_fields[];

extern size_t SIZEOF_INFO_JSON_FIELDS_TABLE;
/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void jinfochk_sanity_chks(char const *file, char const *fnamchk);
static int check_info_json(char const *file, char const *fnamchk);
static struct json_field *add_found_info_json_field(char const *name, char const *val, int line_num);
static int get_info_json_field(char const *file, char *name, char *val, int line_num);
static int check_found_info_json_fields(char const *file, bool test);
static void free_found_info_json_fields(void);
static struct manifest_file *add_manifest_file(char const *filename);
static void free_manifest_files_list(void);
static void free_manifest_file(struct manifest_file *file);


#endif /* INCLUDE_JINFOCHK_H */
