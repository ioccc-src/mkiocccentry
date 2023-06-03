/*
 * jprint - JSON printer
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool is being developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * The JSON parser was co-developed in 2022 by Cody and Landon.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#include "jprint.h"

/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/*
 * static globals
 */
static bool quiet = false;				/* true ==> quiet mode */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg0 =
    "usage: %s [-h] [-V] [-v level] [-J level] [-e] [-Q] [-t type] [-q] [-j lvl] [-i count]\n"
    "\t\t[-N num] [-p {n,v,b}] [-b {t,number}] [-L] [-c] [-C] [-B] [-I {t,number}] [-j] [-E]\n"
    "\t\t[-I] [-S] [-g] file.json [name_arg ...]\n\n"
    "\t-h\t\tPrint help and exit\n"
    "\t-V\t\tPrint version and exit\n"
    "\t-v level\tVerbosity level (def: %d)\n"
    "\t-J level\tJSON verbosity level (def: %d)\n"
    "\t-e\t\tPrint JSON strings as encoded strings (def: decode JSON strings)\n"
    "\t-Q\t\tPrint JSON strings surrounded by double quotes (def: do not)\n"
    "\t-t type\t\tPrint only if JSON value matches one of the comma-separated\n"
    "\t\t\ttypes (def: simple):\n\n"
    "\t\t\t\tint\t\tinteger values\n"
    "\t\t\t\tfloat\t\tfloating point values\n"
    "\t\t\t\texp\t\texponential notation values\n"
    "\t\t\t\tnum\t\talias for int,float,exp\n"
    "\t\t\t\tbool\t\tboolean values\n"
    "\t\t\t\tstr\t\tstring values\n"
    "\t\t\t\tnull\t\tnull values\n"
    "\t\t\t\tsimple\t\talias for 'num,bool,str,null' (the default)\n"
    "\t\t\t\tobject\t\tJSON objects\n"
    "\t\t\t\tarray\t\tJSON array\n"
    "\t\t\t\tcompound\talias for object,array\n"
    "\t\t\t\tany\t\tany type of value\n\n";

static const char * const usage_msg1 =
    "\t-q\t\tQuiet mode (def: print stuff to stdout)\n\n"
    "\t-l lvl\t\tPrint values at specific JSON levels (def: any level, '0:')\n"
    "\t\t\tIf lvl is a number (e.g. '-l 3'), level must == number\n"
    "\t\t\tIf lvl is a number followed by : (e.g. '-l 3:'), level must be >= number\n"
    "\t\t\tIf lvl is a : followed by a number (e.g. '-l :3'), level must be <= number\n"
    "\t\t\tIf lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range\n\n"
    "\t-i count\tPrint up to count matches (def: print all matches)\n"
    "\t\t\tIf count is a number (e.g. '-i 3'), the matches must == number\n"
    "\t\t\tIf count is a number followed by : (e.g. '-i 3:'), matches must be >= number\n"
    "\t\t\tIf count is a : followed by a number (e.g. '-i :3'), matches must be <= number\n"
    "\t\t\tIf count is num:num (e.g. '-i 3:5'), matches must be inclusively in the range\n"
    "\t\t\tNOTE: when number < 0 it refers to the instance from last: -1 is last, -2 second to last ...\n\n"
    "\t-N num\t\tPrint only if there are only a given number of matches (def: do not limit)\n"
    "\t\t\tIf num is only a number (e.g. '-l 1'), there must be only that many matches\n"
    "\t\t\tIf num is a number followed by : (e.g. '-l 3:'), there must >= num matches\n"
    "\t\t\tIf num is a : followed by a number (e.g. '-i :3'), there must <= num matches\n"
    "\t\t\tIf num is num:num (e.g. '-i 3:5'), the number of matches must be inclusively in the range\n\n"
    "\t-p {n,v,b}\tprint JSON key, value or both (def: print JSON values)\n"
    "\t\t\tif the type of value does not match the -t type specification,\n"
    "\t\t\tthen the key, value or both are not printed.\n"
    "\t-p name\t\tAlias for '-p n'.\n"
    "\t-p value\tAlias for '-p v'.\n"
    "\t-p both\t\tAlias '-p v'.\n\n"
    "\t-b {t,number}\tprint between name and value (def: 1)\n"
    "\t\t\tprint a tab or spaces (i.e. '-b 4') between the name and value.\n"
    "\t\t\tUse of -b {t,number} without -j or -p b has no effect.\n"
    "\t-b tab\t\tAlias for '-b t'.\n\n"
    "\t-L\t\tPrint JSON levels, followed by tab (def: do not print levels).\n"
    "\t\t\tThe root (top) of the JSON document is defined as level 0.\n\n"
    "\t-c\t\tWhen printing -j both, separate name/value by a : (colon) (def: do not)\n"
    "\t\t\tNOTE: When -C is used with -b {t,number}, the same number of spaces or tabs\n"
    "\t\t\tseparate the name from the : (colon) AND a number of spaces or tabs\n"
    "\t\t\tand separate : (colon) from the value by the same.\n\n"
    "\t-C\t\tWhen printing JSON syntax, always print a comma after final line (def: do not).\n"
    "\t\t\tUse of -C without -j has no effect.\n\n"
    "\t-B\t\tWhen printing JSON syntax, start with a { line and end with a } line\n"
    "\t\t\tUse of -B without -j has no effect.\n\n"
    "\t-I {t,number}\tWhen printing JSON syntax, indent levels (i.e. '-I 4') (def: do not indent i.e. '-I 0')\n"
    "\t\t\tIndent levels by tab or spaces (i.e. '-t 4').\n"
    "\t\t\tUse of -I {t,number} without -j has no effect.\n"
    "\t-I tab\t\tAlias for '-I t'.\n\n"
    "\t-j\t\tPrint using JSON syntax (def: do not).\n"
    "\t\t\tImplies '-p b -b 1 -c -e -Q -I 4 -t any'.\n"
    "\t\t\tSubsequent use of -b {t,number} changes the printing between JSON tokens.\n"
    "\t\t\tSubsequent use of -I {t,number} changes how JSON is indented.\n"
    "\t\t\tSubsequent use of -t type will change which JSON values are printed.\n"
    "\t\t\tUse of -j conflicts with use of '-p {n,v}'.\n\n"
    "\t-E\t\tMatch the JSON encoded name (def: match the JSON decoded name).\n"
    "\t-I\t\tIgnore case of name (def: case matters).\n"
    "\t-S\t\tSubstrings are used to match (def: the full name must match).\n"
    "\t-g\t\tgrep-like extended regular expressions are used to match (def: name args are not regexps).\n"
    "\t\t\tTo match from the name beginning, start name_arg with '^'.\n"
    "\t\t\tTo match to the name end, end name_arg with '$'.\n"
    "\t\t\tTo match the entire name, enclose name_arg between '^' and '$'.\n"
    "\t\t\tUse of -g conflicts with -S.\n"
    "\n"
    "\tfile.json\tJSON file to parse\n"
    "\tname_arg\tJSON element to print\n"
    "jprint version: %s";


/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));

int main(int argc, char **argv)
{
    char const *program = NULL;		/* our name */
    extern char *optarg;
    extern int optind;
    struct json *json_tree = NULL;	/* json tree */
    bool is_valid = true;		/* if file is valid json */
    FILE *json_file = NULL;		/* file pointer for json file */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:Vq")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, "");	/*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'J':		/* -J json_verbosity */
	    /*
	     * parse JSON verbosity level
	     */
	    json_verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", JPRINT_VERSION);
	    exit(2);		/*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    check_invalid_option(program, i, optopt);
	    usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	}
    }

    /* must have the exact required number of args */
    if (argc - optind < REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    argc -= optind;
    argv += optind;

    /* check that first arg exists and is a regular file */
    if (!exists(argv[0])) {
	err(4, "jprint", "%s: file does not exist", argv[0]); /*ooo*/
	not_reached();
    } else if (!is_file(argv[0])) {
	err(5, "jprint", "%s: not a regular file", argv[0]); /*ooo*/
	not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    json_file = fopen(argv[0], "r");
    if (json_file == NULL) {
	errp(6, "jprint", "%s: could not open for reading", argv[0]); /*ooo*/
	not_reached();
    }

    json_tree = parse_json_stream(json_file, argv[0], &is_valid);
    if (!is_valid) {
	fclose(json_file);  /* close file prior to exiting */
	json_file = NULL;   /* set to NULL even though we're exiting as a safety precaution */

	err(7, "jprint", "%s not valid JSON", argv[0]); /*ooo*/
	not_reached();
    }

    /* close the JSON file */
    fclose(json_file);
    json_file = NULL;

    /*
     * XXX this will probably change to a dbg() message when a name_arg is
     * specified but it can probably stay msg() if no name_arg specified. This
     * is TBD at a later date.
     */
    msg("valid JSON");

    if (argv[1] != NULL) {
	/* TODO process name_args */

	msg("\npattern requested: %s", argv[1]);
	/*
	 * XXX if matches found exit 0 but currently no matches checked. In
	 * other words in the future the call to exit() here will only be called
	 * if a pattern is matched: otherwise the exit(1) below will be called.
	 */
	exit(0); /*ooo*/
    }

    /*
     * XXX remove this informative message once processing is implemented
     */
    msg("\nno pattern requested");
    /*
     * exit with 1 due to no pattern requested OR no matches found
     */
    exit(1); /*ooo*/
}

/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message
 *	prog		our program name
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg0, prog, DBG_DEFAULT, JSON_DBG_DEFAULT);
    fprintf_usage(exitcode, stderr, usage_msg1, JPRINT_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
