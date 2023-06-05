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
    "\t\t[-N num] [-p {n,v,b}] [-b {t,number}] [-L] [-T] [-C] [-B] [-I {t,number}] [-j] [-E]\n"
    "\t\t[-I] [-S] [-g] [-c] [-m depth] file.json [name_arg ...]\n\n"
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
    "\t-p both\t\tAlias for '-p n,v'.\n\n"
    "\t-b {t,number}\tprint between name and value (def: 1)\n"
    "\t\t\tprint a tab or spaces (i.e. '-b 4') between the name and value.\n"
    "\t\t\tUse of -b {t,number} without -j or -p b has no effect.\n"
    "\t-b tab\t\tAlias for '-b t'.\n\n"
    "\t-L\t\tPrint JSON levels, followed by tab (def: do not print levels).\n"
    "\t\t\tThe root (top) of the JSON document is defined as level 0.\n\n";

static const char * const usage_msg2 =
    "\t-T\t\tWhen printing '-j both', separate name/value by a : (colon) (def: do not)\n"
    "\t\t\tNOTE: When -C is used with -b {t,number}, the same number of spaces or tabs\n"
    "\t\t\tseparate the name from the : (colon) AND a number of spaces or tabs\n"
    "\t\t\tand separate : (colon) from the value by the same.\n\n"
    "\t-C\t\tWhen printing JSON syntax, always print a comma after final line (def: do not).\n"
    "\t\t\tUse of -C without -j has no effect.\n\n"
    "\t-B\t\tWhen printing JSON syntax, start with a { line and end with a } line\n"
    "\t\t\tUse of -B without -j has no effect.\n\n"
    "\t-I {t,number}\tWhen printing JSON syntax, indent levels (i.e. '-I 4') (def: do not indent i.e. '-I 0')\n"
    "\t\t\tIndent levels by tab or spaces (i.e. '-I 4').\n"
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
    "\t-c\t\tOnly show count of matches found\n"
    "\t-m max_depth\tSet the maximum JSON level depth to max_depth, 0 ==> infinite depth (def: 256)\n"
    "\t\t\tNOTE: max_depth of 0 implies use of JSON_INFINITE_DEPTH: use this with extreme caution.\n";

/*
 * NOTE: this next one should be the last number; if any additional usage message strings
 * have to be added the first additional one should be the number this is and this one
 * should be changed to be the final string before this one + 1. Similarly if a
 * string can be removed this one should have its number changed to be + 1 from
 * the last one before it.
 */
static const char * const usage_msg3 =
    "\tfile.json\tJSON file to parse (- indicates stdin)\n"
    "\tname_arg\tJSON element to print\n\n"
    "\tExit codes:\n"
    "\t\t0\tall is OK, file is valid JSON, match(s) found or no name_arg given\n"
    "\t\t1\tfile is valid JSON, name_arg given but no matches found\n"
    "\t\t2\t-h and help string printed or -V and version string printed\n"
    "\t\t3\tinvalid command line, invalid option or option missing an argument\n"
    "\t\t4\tfile does not exist, not a file, or unable to read the file\n"
    "\t\t5\tfile contents is not valid JSON\n\n"
    "jprint version: %s";

/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));
static uintmax_t parse_types_option(char *optarg);

int main(int argc, char **argv)
{
    char const *program = NULL;		/* our name */
    extern char *optarg;
    extern int optind;
    struct json *json_tree = NULL;	/* json tree */
    bool is_stdin = false;		/* reading from stdin */
    bool is_valid = true;		/* if file is valid json */
    bool match_found = false;		/* true if a pattern is specified and there is a match */
    bool pattern_specified = false;	/* true if a pattern was specified */
    FILE *json_file = NULL;		/* file pointer for json file */
    bool encode_strings = false;	/* -e used */
    bool quote_strings = false;		/* -Q used */
    uintmax_t type = JPRINT_TYPE_SIMPLE;/* -t type used */
    uintmax_t max_matches = 0;		/* -i count specified - don't show more than this many matches */
    uintmax_t min_matches = 0;		/* -N count specified - minimum matches required */
    char const *print_type = NULL;	/* -p type specified */
    uintmax_t num_spaces = 0;		/* -b specified */
    bool print_json_levels = false;	/* -L specified */
    bool print_colons = false;		/* -T specified */
    bool print_commas = false;		/* -C specified */
    bool print_braces = false;		/* -B specified */
    uintmax_t indent_level = 0;		/* -I specified */
    bool print_syntax = false;		/* -j used, will imply -p b -b 1 -c -e -Q -I 4 -t any */
    bool match_encoded = false;		/* -E used, match encoded name */
    bool substrings_okay = false;	/* -S used, matching substrings okay */
    bool use_regexps = false;		/* -g used, allow grep-like regexps */
    bool count_only = false;		/* -c used, only show count */
    uintmax_t max_depth = JSON_DEFAULT_MAX_DEPTH; /* max depth to traverse set by -m depth */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hVv:J:eQt:qj:i:N:p:b:LTCBI:jEISgcm:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, "");	/*ooo*/
	    not_reached();
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", JPRINT_VERSION);
	    exit(2);		/*ooo*/
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
	case 'e':
	    encode_strings = true;
	    break;
	case 'Q':
	    quote_strings = true;
	    break;
	case 't':
	    type = parse_types_option(optarg);
	    break;
	case 'i':
	    if (!string_to_uintmax(optarg, &max_matches)) {
		err(3, "jprint", "couldn't parse -i count"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'N':
	    if (!string_to_uintmax(optarg, &min_matches)) {
		err(3, "jprint", "couldn't parse -N count"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'p':
	    /* XXX the type of this variable might have to change and in any
	     * event must be parsed.
	     */
	    print_type = optarg;
	    break;
	case 'b':
	    /* XXX - is this the right idea ? - XXX */
	    if (!string_to_uintmax(optarg, &num_spaces)) {
		err(3, "jprint", "couldn't parse -b spaces"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'L':
	    print_json_levels = true;
	    break;
	case 'T':
	    print_colons = true;
	    break;
	case 'C':
	    print_commas = true;
	    break;
	case 'B':
	    print_braces = true;
	    break;
	case 'I':
	    if (!string_to_uintmax(optarg, &indent_level)) {
		err(3, "jprint", "couldn't parse -I indent_level"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'j':
	    /* TODO need to set the options of -p b -b 1 -c -e -Q -I 4 -t any */
	    print_syntax = true;
	    break;
	case 'E':
	    match_encoded = true;
	    break;
	case 'S':
	    substrings_okay = true;
	    break;
	case 'g':   /* allow grep-like ERE */
	    use_regexps = true;
	    break;
	case 'c':
	    count_only = true;
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'm': /* set maximum depth to traverse json tree */
	    if (!string_to_uintmax(optarg, &max_depth)) {
		err(3, "jprint", "couldn't parse -m depth"); /*ooo*/
		not_reached();
	    }
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

    /* must have at least REQUIRED_ARGS args */
    if (argc - optind < REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    argc -= optind;
    argv += optind;

    /* if *argv[0] != '-' we will attempt to read from a regular file */
    if (*argv[0] != '-') {
        /* check that first arg exists and is a regular file */
	if (!exists(argv[0])) {
	    err(4, "jprint", "%s: file does not exist", argv[0]); /*ooo*/
	    not_reached();
	} else if (!is_file(argv[0])) {
	    err(4, "jprint", "%s: not a regular file", argv[0]); /*ooo*/
	    not_reached();
	} else if (!is_read(argv[0])) {
	    err(4, "jprint", "%s: unreadable file", argv[0]); /*ooo*/
	    not_reached();
	}

	errno = 0; /* pre-clear errno for errp() */
	json_file = fopen(argv[0], "r");
	if (json_file == NULL) {
	    errp(4, "jprint", "%s: could not open for reading", argv[0]); /*ooo*/
	    not_reached();
	}
    } else { /* *argv[0] == '-', read from stdin */
	is_stdin = true;
	json_file = stdin;
    }

    json_tree = parse_json_stream(json_file, argv[0], &is_valid);
    if (!is_valid) {
	if (json_file != stdin) {
	    fclose(json_file);  /* close file prior to exiting */
	    json_file = NULL;   /* set to NULL even though we're exiting as a safety precaution */
	}

	err(5, "jprint", "%s invalid JSON", argv[0]); /*ooo*/
	not_reached();
    }

    /* close the JSON file if not stdin */
    if (json_file != stdin) {
	fclose(json_file);
	json_file = NULL;
    }

    /* this will change to a debug message at a later time */
    msg("valid JSON");

    /* the debug level will be increased at a later time */
    dbg(DBG_NONE, "maximum depth to traverse: %ju%s", max_depth, (max_depth == 0?" (no limit)":
		max_depth==JSON_DEFAULT_MAX_DEPTH?" (default)":""));

    /* TODO process name_args */
    for (i = 1; argv[i] != NULL; ++i) {
	pattern_specified = true;

	/*
	 * XXX either change the debug level or remove this message once
	 * processing is complete
	 */
	dbg(DBG_NONE,"pattern requested: %s", argv[i]);
	/*
	 * XXX if matches found we set the boolean match_found to true to
	 * indicate exit code of 0 but currently no matches are checked. In
	 * other words in the future this setting of match_found will not always
	 * happen.
	 */
	match_found = true;
    }

    /*
     * XXX remove this informative message or change debug level once processing
     * is implemented.
     */
    if (!pattern_specified) {
	dbg(DBG_NONE,"no pattern requested");
    }

    /* free tree */
    json_tree_free(json_tree, max_depth);

    if (match_found) {
	exit(0); /*ooo*/
    } else {
	/*
	 * exit with 1 due to no pattern requested OR no matches found
	 */
	exit(1); /*ooo*/
    }
}

static uintmax_t
parse_types_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */

    uintmax_t type = JPRINT_TYPE_SIMPLE; /* default is simple: num, bool, str and null */

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    }

    /*
     * Go through comma-separated list of types, setting each as a bitvector
     *
     * NOTE: the way this is done might change if it proves there is a better
     * way (and there might be - I've thought of a number of ways already).
     */
    for (p = strtok_r(optarg, ",", &saveptr); p; p = strtok_r(NULL, ",", &saveptr)) {
	if (!strcmp(p, "int")) {
	    type |= JPRINT_TYPE_INT;
	} else if (!strcmp(p, "float")) {
	    type |= JPRINT_TYPE_FLOAT;
	} else if (!strcmp(p, "exp")) {
	    type |= JPRINT_TYPE_EXP;
	} else if (!strcmp(p, "num")) {
	    type |= JPRINT_TYPE_NUM;
	} else if (!strcmp(p, "bool")) {
	    type |= JPRINT_TYPE_BOOL;
	} else if (!strcmp(p, "str")) {
	    type |= JPRINT_TYPE_STR;
	} else if (!strcmp(p, "null")) {
	    type |= JPRINT_TYPE_NULL;
	} else if (!strcmp(p, "object")) {
	    type |= JPRINT_TYPE_OBJECT;
	} else if (!strcmp(p, "array")) {
	    type |= JPRINT_TYPE_ARRAY;
	} else if (!strcmp(p, "simple")) {
	    type |= JPRINT_TYPE_SIMPLE;
	} else if (!strcmp(p, "compound")) {
	    type |= JPRINT_TYPE_COMPOUND;
	} else if (!strcmp(p, "any")) {
	    type |= JPRINT_TYPE_ANY;
	} else {
	    /* unknown type */
	    err(11, __func__, "unknown type '%s'", p);
	    not_reached();
	}
    }

    return type;
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
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg1);
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg2);
    fprintf_usage(exitcode, stderr, usage_msg3, JPRINT_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
