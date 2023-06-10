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
    "usage: %s [-h] [-V] [-v level] [-J level] [-e] [-Q] [-t type] [-q] [-n count]\n"
    "\t\t[-N num] [-p {n,v,b}] [-b <num>{[t|s]}] [-L <num>{[t|s]}] [-T] [-C] [-B]\n"
    "\t\t[-I <num>{[t|s]} [-j] [-E] [-i] [-S] [-g] [-c] [-m depth] [-K] file.json [name_arg ...]\n\n"
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
    "\t\t\t\tany\t\tany type of value\n";

static const char * const usage_msg1 =
    "\t-q\t\tQuiet mode (def: print stuff to stdout)\n\n"
    "\t-l lvl\t\tPrint values at specific JSON levels (def: any level, '0:')\n"
    "\t\t\tIf lvl is a number (e.g. '-l 3'), level must == number\n"
    "\t\t\tIf lvl is a number followed by : (e.g. '-l 3:'), level must be >= number\n"
    "\t\t\tIf lvl is a : followed by a number (e.g. '-l :3'), level must be <= number\n"
    "\t\t\tIf lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range\n\n"
    "\t-n count\tPrint up to count matches (def: print all matches)\n"
    "\t\t\tIf count is a number (e.g. '-n 3'), the matches must == number\n"
    "\t\t\tIf count is a number followed by : (e.g. '-n 3:'), matches must be >= number\n"
    "\t\t\tIf count is a : followed by a number (e.g. '-n :3'), matches must be <= number\n"
    "\t\t\tIf count is num:num (e.g. '-n 3:5'), matches must be inclusively in the range\n"
    "\t\t\tNOTE: when number < 0 it refers to up through matches - the positive max\n\n"
    "\t-N num\t\tPrint only if there are only a given number of matches (def: do not limit)\n"
    "\t\t\tIf num is only a number (e.g. '-l 1'), there must be only that many matches\n"
    "\t\t\tIf num is a number followed by : (e.g. '-l 3:'), there must >= num matches\n"
    "\t\t\tIf num is a : followed by a number (e.g. '-n :3'), there must <= num matches\n"
    "\t\t\tIf num is num:num (e.g. '-n 3:5'), the number of matches must be inclusively in the range\n\n"
    "\t-p {n,v,b}\tprint JSON key, value or both (def: print JSON values)\n"
    "\t\t\tif the type of value does not match the -t type specification,\n"
    "\t\t\tthen the key, value or both are not printed.\n"
    "\t-p name\t\tAlias for '-p n'.\n"
    "\t-p value\tAlias for '-p v'.\n"
    "\t-p both\t\tAlias for '-p n,v'.\n\n"
    "\t-b <num>[{t|s}]\tPrint specified number of tabs or spaces between JSON tokens printed via -j (def: 1 space)\n"
    "\t\t\tNot specifying a character after the number implies spaces.\n"
    "\t\t\tNOTE: -b without -j has no effect.\n"
    "\t\t\tNOTE: it is an error to use -b [num]t without -p b\n"
    "\t-b tab\t\tAlias for '-b 1t'.\n\n"
    "\t-L <num>[{t|s}]\tPrint JSON level followed by specified number of tabs or spaces (def: don't print levels)\n"
    "\t\t\tTrailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces. Not\n"
    "\t\t\tspecifying 's' or 't' implies spaces but any other character is an error.\n"
    "\t\t\tNOTE: the top JSON level is 0.\n"
    "\t-L tab\t\tAlias for '-L 1t'.\n";

static const char * const usage_msg2 =
    "\t-T\t\tWhen printing '-p both', separate name/value by a : (colon) (def: do not)\n"
    "\t\t\tNOTE: When -C is used with -b {t,number}, the same number of spaces or tabs\n"
    "\t\t\tseparate the name from the : (colon) AND a number of spaces or tabs\n"
    "\t\t\tand separate : (colon) from the value by the same.\n\n"
    "\t-C\t\tWhen printing JSON syntax, always print a comma after final line (def: do not).\n"
    "\t\t\tUse of -C without -j has no effect.\n\n"
    "\t-B\t\tWhen printing JSON syntax, start with a { line and end with a } line\n"
    "\t\t\tUse of -B without -j has no effect.\n\n"
    "\t-I <num>{[t|s]}\tWhen printing JSON syntax, indent levels (i.e. '-I 4') (def: don't indent i.e. '-I 0')\n"
    "\t\t\tTrailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces. Not\n"
    "\t\t\tspecifying 's' or 't' implies spaces but any other character is an error.\n"
    "\t\t\tNOTE: the top JSON level is 0.\n"
    "\t-I tab\t\tAlias for '-I 1t'.\n\n"
    "\t-j\t\tPrint using JSON syntax (def: do not).\n"
    "\t\t\tImplies '-p b -b 1 -e -Q -I 4 -t any'.\n"
    "\t\t\tSubsequent use of -b {t,number} changes the printing between JSON tokens.\n"
    "\t\t\tSubsequent use of -I {t,number} changes how JSON is indented.\n"
    "\t\t\tSubsequent use of -t type will change which JSON values are printed.\n"
    "\t\t\tUse of -j conflicts with use of '-p {n,v}'.\n\n"
    "\t-E\t\tMatch the JSON encoded name (def: match the JSON decoded name).\n"
    "\t-i\t\tIgnore case of name (def: case matters).\n"
    "\t-S\t\tSubstrings are used to match (def: the full name must match).\n"
    "\t-g\t\tgrep-like extended regular expressions are used to match (def: name args are not regexps).\n"
    "\t\t\tTo match from the name beginning, start name_arg with '^'.\n"
    "\t\t\tTo match to the name end, end name_arg with '$'.\n"
    "\t\t\tTo match the entire name, enclose name_arg between '^' and '$'.\n"
    "\t\t\tNOTE: Use of -g and -S is an error.\n"
    "\t-c\t\tOnly show count of matches found\n"
    "\t-m max_depth\tSet the maximum JSON level depth to max_depth, 0 ==> infinite depth (def: 256)\n"
    "\t\t\tNOTE: max_depth of 0 implies use of JSON_INFINITE_DEPTH: use this with extreme caution.\n"
    "\t-K\t\tRun tests on jprint constraints\n";

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
    "\t\t0\tall is OK: file is valid JSON, match(es) found or no name_arg given OR test mode OK\n"
    "\t\t1\tfile is valid JSON, name_arg given but no matches found\n"
    "\t\t2\t-h and help string printed or -V and version string printed\n"
    "\t\t3\tinvalid command line, invalid option or option missing an argument\n"
    "\t\t4\tfile does not exist, not a file, or unable to read the file\n"
    "\t\t5\tfile contents is not valid JSON\n"
    "\t\t6\ttest mode failed\n\n"
    "jprint version: %s";

/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));

int main(int argc, char **argv)
{
    char const *program = NULL;		/* our name */
    char ch = -1;			/* placeholder for some sscanf() calls */
    extern char *optarg;
    extern int optind;
    struct json *json_tree = NULL;	/* json tree */
    bool is_stdin = false;		/* reading from stdin */
    bool is_valid = true;		/* if file is valid json */
    bool match_found = false;		/* true if a pattern is specified and there is a match */
    bool case_insensitive = false;	/* true if -i, case-insensitive */
    bool pattern_specified = false;	/* true if a pattern was specified */
    FILE *json_file = NULL;		/* file pointer for json file */
    bool encode_strings = false;	/* -e used */
    bool quote_strings = false;		/* -Q used */
    uintmax_t type = JPRINT_TYPE_SIMPLE;/* -t type used */
    struct jprint_number jprint_max_matches = { 0 }; /* -n count specified */
    struct jprint_number jprint_min_matches = { 0 }; /* -N count specified */
    struct jprint_number jprint_levels = { 0 }; /* -l level specified */
    uintmax_t print_type = JPRINT_PRINT_VALUE;	/* -p type specified */
    uintmax_t num_token_spaces = 0;	/* -b specified number of spaces or tabs */
    bool print_token_tab = false;	/* -b tab (or -b <num>[t]) specified */
    bool print_json_levels = false;	/* -L specified */
    uintmax_t num_level_spaces = 0;	/* number of spaces or tab for -L */
    bool print_level_tab = false;	/* -L tab option */
    bool print_colons = false;		/* -T specified */
    bool print_commas = false;		/* -C specified */
    bool print_braces = false;		/* -B specified */
    uintmax_t indent_level = 0;		/* -I specified */
    bool indent_tab = false;		/* -I t{,ab} specified */
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
    while ((i = getopt(argc, argv, ":hVv:J:l:eQt:qjn:N:p:b:L:TCBI:jEiSgcm:K")) != -1) {
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
	case 'l':
	    jprint_parse_number_range("-l", optarg, &jprint_levels);
	    break;
	case 'e':
	    encode_strings = true;
	    break;
	case 'Q':
	    quote_strings = true;
	    break;
	case 't':
	    type = jprint_parse_types_option(optarg);
	    break;
	case 'n':
	    jprint_parse_number_range("-n", optarg, &jprint_max_matches);
	    break;
	case 'N':
	    jprint_parse_number_range("-N", optarg, &jprint_min_matches);
	    break;
	case 'p':
	    print_type = jprint_parse_print_option(optarg);
	    break;
	case 'b':
	    jprint_parse_st_tokens_option(optarg, &num_token_spaces, &print_token_tab);
	    break;
	case 'L':
	    if (sscanf(optarg, "%ju%c", &num_level_spaces, &ch) == 2) {
		if (ch == 't') {
		    print_level_tab = true;
		    dbg(DBG_NONE, "will print %ju tab%s after levels", num_level_spaces, num_level_spaces==1?"":"s");
		} else if (ch == 's') {
		    print_level_tab = false; /* ensure it's false in case specified previously */
		    dbg(DBG_NONE, "will print %jd space%s after level", num_level_spaces, num_level_spaces==1?"":"s");
		} else {
		    err(4, __func__, "syntax error for -L");
		    not_reached();
		}
	    } else if (!strcmp(optarg, "tab")) {
		    print_level_tab = true;
		    num_level_spaces = 1;
		    dbg(DBG_NONE, "will print %ju tab%s after levels", num_level_spaces, num_level_spaces==1?"":"s");
	    } else if (!string_to_uintmax(optarg, &num_level_spaces)) {
		err(3, "jprint", "couldn't parse -L spaces"); /*ooo*/
		not_reached();
	    } else {
		print_level_tab = false; /* ensure it's false in case specified previously */
		dbg(DBG_NONE, "will print %jd space%s after level", num_level_spaces, num_level_spaces==1?"":"s");
	    }
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
	    jprint_parse_st_indent_option(optarg, &indent_level, &indent_tab);
	    break;
	case 'i':
	    case_insensitive = true; /* make case cruel :-) */
	    break;
	case 'j':
	    /* TODO still need to set the options of -t any */
	    print_syntax = true;
	    dbg(DBG_NONE, "implying -p both");
	    print_type = jprint_parse_print_option("both");
	    dbg(DBG_NONE, "implying -b 1");
	    jprint_parse_st_tokens_option("1", &num_token_spaces, &print_token_tab);
	    dbg(DBG_NONE, "implying -e -Q");
	    encode_strings = true;
	    quote_strings = true;
	    dbg(DBG_NONE, "implying -I 4");
	    jprint_parse_st_indent_option("4", &indent_level, &indent_tab);
	    dbg(DBG_NONE, "implying -t any");
	    type = jprint_parse_types_option("any");
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
	case 'K': /* run test code */
	    if (!jprint_run_tests()) {
		exit(6); /*ooo*/
	    }
	    else {
		exit(0); /*ooo*/
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


    /*
     * check for conflicting options prior to changing argc and argv so that the
     * user will know to correct the options before being told that they have
     * the wrong number of arguments (if they do).
     */

    /* use of -g conflicts with -S is an error */
    if (use_regexps && substrings_okay) {
	err(3, "jprint", "cannot use both -g and -S"); /*ooo*/
	not_reached();
    }

    /* check that if -b [num]t is used then both -p both */
    if (print_token_tab && !jprint_print_name_value(print_type)) {
	err(3, "jparse", "use of -b [num]t cannot be used without -p both"); /*ooo*/
	not_reached();
    }

    argc -= optind;
    argv += optind;

    /* must have at least REQUIRED_ARGS args */
    if (argc < REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }


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

    if (match_found || !pattern_specified) {
	exit(0); /*ooo*/
    } else {
	/*
	 * exit with 1 due to no pattern requested OR no matches found
	 */
	exit(1); /*ooo*/
    }
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "wrong number of arguments");;
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
