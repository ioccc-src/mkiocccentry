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

/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */

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
    "\t\t[-N num] [-p {n,v,b}] [-b <num>{[t|s]}] [-L <num>{[t|s]}] [-P] [-C] [-B]\n"
    "\t\t[-I <num>{[t|s]} [-j] [-E] [-i] [-s] [-g] [-G regexp] [-c] [-m depth] [-K]\n"
    "\t\t[-Y type:value] [-S path] [-A args] [-o] file.json [name_arg ...]\n\n"
    "\t-h\t\tPrint help and exit\n"
    "\t-V\t\tPrint version and exit\n"
    "\t-v level\tVerbosity level (def: %d)\n"
    "\t-J level\tJSON verbosity level (def: %d)\n"
    "\n"
    "\t-e\t\tPrint JSON strings as encoded strings (def: decode JSON strings)\n"
    "\t-Q\t\tPrint JSON strings surrounded by double quotes (def: do not)\n"
    "\t-t type\t\tPrint only if JSON value matches one of the comma-separated\n"
    "\t\t\ttypes (def: simple):\n\n"
    "\t\t\t\tint\t\tinteger values\n"
    "\t\t\t\tfloat\t\tfloating point values\n"
    "\t\t\t\texp\t\texponential notation values\n"
    "\t\t\t\tnum\t\tAlias for: int,float,exp\n"
    "\t\t\t\tbool\t\tboolean values\n"
    "\t\t\t\tstr\t\tstring values\n"
    "\t\t\t\tnull\t\tnull values\n"
    "\t\t\t\tsimple\t\tAlias for: num,bool,str,null (the default)\n"
    "\t\t\t\tobject\t\tJSON objects\n"
    "\t\t\t\tarray\t\tJSON array\n"
    "\t\t\t\tcompound\tAlias for: object,array\n"
    "\t\t\t\tany\t\tany type of value\n";

static const char * const usage_msg1 =
    "\t-q\t\tQuiet mode (def: print stuff to stdout)\n\n"
    "\t-l lvl\t\tPrint values at specific JSON levels (def: any level: 0:)\n"
    "\t\t\tIf lvl is a number (e.g.: -l 3), level must == number\n"
    "\t\t\tIf lvl is a number followed by : (e.g.: -l 3:), level must be >= number\n"
    "\t\t\tIf lvl is a : followed by a number (e.g.: -l :3), level must be <= number\n"
    "\t\t\tIf lvl is num:num (e.g.: -l 3:5), level must be inclusively in the range\n\n"
    "\t-n count\tPrint up to count matches (def: print all matches)\n"
    "\t\t\tIf count is a number (e.g.: -n 3), the matches must == number\n"
    "\t\t\tIf count is a number followed by : (e.g.: -n 3:), matches must be >= number\n"
    "\t\t\tIf count is a : followed by a number (e.g.: -n :3), matches must be <= number\n"
    "\t\t\tIf count is num:num (e.g.: -n 3:5), matches must be inclusively in the range\n"
    "\t\t\tNOTE: when number < 0 it refers to up through matches - the positive max\n\n"
    "\t-N num\t\tPrint only if there are only a given number of matches (def: do not limit)\n"
    "\t\t\tIf num is only a number (e.g.: -l 1), there must be only that many matches\n"
    "\t\t\tIf num is a number followed by : (e.g.: -l 3:), there must >= num matches\n"
    "\t\t\tIf num is a : followed by a number (e.g.: -n :3), there must <= num matches\n"
    "\t\t\tIf num is num:num (e.g.: -n 3:5), the number of matches must be inclusively in the range\n\n"
    "\t-p {n,v,b}\tprint JSON key, value or both (def: print JSON values)\n"
    "\t\t\tIf the type of value does not match the -t type specification,\n"
    "\t\t\tthen the key, value or both are not printed\n"
    "\t\t\tNOTE: it is an error to use both -p and -j\n"
    "\t-p name\t\tAlias for: -p n\n"
    "\t-p value\tAlias for: -p v\n"
    "\t-p both\t\tAlias for: -p n,v\n\n"
    "\t-b <num>[{t|s}]\tPrint specified number of tabs or spaces between JSON tokens printed via -j (def: 1 space)\n"
    "\t\t\tNot specifying a character after the number implies spaces\n"
    "\t\t\tNOTE: -b without -j has no effect\n"
    "\t\t\tNOTE: it is an error to use -b [num]t without -p b\n"
    "\t-b tab\t\tAlias for: -b 1t\n\n"
    "\t-L <num>[{t|s}]\tPrint JSON level followed by specified number of tabs or spaces (def: don't print levels)\n"
    "\t\t\tTrailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces\n"
    "\t\t\tNot specifying 's' or 't' implies spaces but any other character is an error\n"
    "\t\t\tNOTE: the top JSON level is 0\n"
    "\t-L tab\t\tAlias for: -L 1t\n";

static const char * const usage_msg2 =
    "\t-P\t\tWhen printing '-p both', separate name/value by a : (colon) (def: do not)\n"
    "\t\t\tNOTE: When -C is used with -b {t,number}, the same number of spaces or tabs\n"
    "\t\t\tseparate the name from the : (colon) AND a number of spaces or tabs\n"
    "\t\t\tand separate : (colon) from the value by the same\n\n"
    "\t-C\t\tWhen printing JSON syntax, always print a comma after final line (def: do not)\n"
    "\t\t\tUse of -C without -j has no effect\n\n"
    "\t-B\t\tWhen printing JSON syntax, start with a { line and end with a } line\n"
    "\t\t\tUse of -B without -j has no effect\n\n"
    "\t-I <num>{[t|s]}\tWhen printing JSON syntax, indent levels (i.e.: -I 4) (def: don't indent i.e.: -I 0)\n"
    "\t\t\tTrailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces\n"
    "\t\t\tNot specifying 's' or 't' implies spaces but any other character is an error\n"
    "\t\t\tNOTE: the top JSON level is 0\n"
    "\t-I tab\t\tAlias for: -I 1t\n\n"
    "\t-j\t\tPrint using JSON syntax (def: do not)\n"
    "\t\t\tImplies: -p b -b 1 -e -Q -I 4 -t any\n"
    "\t\t\tSubsequent use of -b <num>{[t|s]} changes the printing between JSON tokens\n"
    "\t\t\tSubsequent use of -I <num>{[t|s]} changes how JSON is indented\n"
    "\t\t\tSubsequent use of -t type will change which JSON values are printed\n"
    "\t\t\tNOTE: it is an error to use both -p and -j\n\n"
    "\t-E\t\tMatch the JSON encoded name (def: match the JSON decoded name)\n"
    "\t-i\t\tIgnore case of name (def: case matters)\n"
    "\t-s\t\tSubstrings are used to match (def: the full name must match)\n\n"
    "\t-g\t\tgrep-like extended regular expressions are used to match (def: name args are not regexps)\n"
    "\t\t\tTo match from the name beginning, start name_arg with '^'\n"
    "\t\t\tTo match to the name end, end name_arg with '$'\n"
    "\t\t\tTo match the entire name, enclose name_arg between '^' and '$'\n"
    "\t\t\tNOTE: use of -g and -s is an error\n"
    "\t-G regex\t\tSpecify a pattern that is a regex irrespective of the name_args\n"
    "\t\t\tNOTE: use of -G does not conflict with G or -s\n\n"
    "\t-c\t\tOnly show count of matches found\n\n"
    "\t-m max_depth\tSet the maximum JSON level depth to max_depth, 0 ==> infinite depth (def: 256)\n"
    "\t\t\tNOTE: 0 implies JSON_INFINITE_DEPTH: only safe with infinite variable size and RAM :-)\n\n"
    "\t-K\t\tRun tests on jprint constraints\n";

static const char * const usage_msg3 =
    "\t-Y type\t\tSearch for a JSON value as written in file.json (def: search for JSON names)\n"
    "\t\t\tNOTE: Type is one or more comma separated of:\n"
    "\n"
    "\t\t\t\tint\tinteger values\n"
    "\t\t\t\tfloat\tfloating point values\n"
    "\t\t\t\texp\texponential notation values\n"
    "\t\t\t\tnum\tAlias for: int,float,exp\n"
    "\t\t\t\tbool\tboolean values\n"
    "\t\t\t\tstr\tstring values\n"
    "\t\t\t\tnull\tnull values\n"
    "\t\t\t\tsimple\tAlias for: num,bool,str,null\n\n"
    "\t\t\tNOTE: -Y Requires one and only one name_arg\n\n"
    "\t-S path\t\tRun JSON check tool, path, with file.json arg, abort of non-zero exit (def: do not run)\n"
    "\t-A args\t\tRun JSON check tool with additional args passed to the tool after file.json (def: none)\n"
    "\t\t\tNOTE: use of -A requires use of -S\n\n"
    "\t-o\t\twrite entire file to stdout if valid JSON\n"
    "\t\t\tNOTE: use of -o with patterns specified is an error.\n";

/*
 * NOTE: this next one should be the last number; if any additional usage message strings
 * have to be added the first additional one should be the number this is and this one
 * should be changed to be the final string before this one + 1. Similarly if a
 * string can be removed this one should have its number changed to be + 1 from
 * the last one before it.
 */
static const char * const usage_msg4 =
    "\tfile.json\tJSON file to parse (- ==> read from stdin)\n"
    "\tname_arg\tsearch file.json for JSON name(s) (or one value if -Y) (def: match all)\n\n"
    "Exit codes:\n"
    "    0\tall is OK: file is valid JSON, match(es) found or no name_arg given OR test mode OK\n"
    "    1\tfile is valid JSON, name_arg given but no matches found\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tinvalid command line, invalid option or option missing an argument\n"
    "    4\tfile does not exist, not a file, or unable to read the file\n"
    "    5\tfile contents is not valid JSON\n"
    "    6\ttest mode failed\n"
    "    7\tJSON check tool failed\n"
    " >=10\tinternal error\n\n"
    "JSON parser version: %s\n"
    "jprint version: %s";

/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));
static struct jprint *alloc_jprint(void);

int main(int argc, char **argv)
{
    char const *program = NULL;		/* our name */
    extern char *optarg;
    extern int optind;
    struct jprint *jprint = NULL;	/* struct of all our options and other things */
    struct jprint_pattern *pattern = NULL; /* iterate through patterns list to search for matches */
    FILE *json_file = NULL;		/* file pointer for json file */
    FILE *tool_stream = NULL;		/* for -S path */
    char *file_contents = NULL;		/* file contents in full */
    size_t len = 0;			/* length of file contents */
    struct json *json_tree;		/* json tree */
    bool is_valid = false;		/* if file is valid json */
    char *tool_path = NULL;		/* -S path specified */
    char *tool_args = NULL;		/* -A args for -S path specified */
    int exit_code = 0;			/* exit code for -S path execution */
    int i;

    jprint = alloc_jprint();		/* allocate our struct jprint * */
    /*
     * the alloc_jprint() will never return a NULL pointer but check just in
     * case
     */
    if (jprint == NULL) {
	err(16, "jprint", "failed to allocate jprint struct");
	not_reached();
    }

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hVv:J:l:eQt:qjn:N:p:b:L:PCBI:jEiS:m:cg:G:KY:sA:o")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    free_jprint(&jprint);
	    usage(2, program, "");	/*ooo*/
	    not_reached();
	    break;
	case 'V':		/* -V - print version and exit */
	    free_jprint(&jprint);
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
	    jprint->levels_constrained = true;
	    jprint_parse_number_range("-l", optarg, &jprint->jprint_levels);
	    break;
	case 'e':
	    jprint->encode_strings = true;
	    dbg(DBG_NONE, "-e specified, will encode strings");
	    break;
	case 'Q':
	    jprint->quote_strings = true;
	    dbg(DBG_NONE, "-Q specified, will quote strings");
	    break;
	case 't':
	    jprint->type = jprint_parse_types_option(optarg);
	    break;
	case 'n':
	    jprint_parse_number_range("-n", optarg, &jprint->jprint_max_matches);
	    jprint->max_matches_requested = true;
	    break;
	case 'N':
	    jprint_parse_number_range("-N", optarg, &jprint->jprint_min_matches);
	    jprint->min_matches_requested = true;
	    break;
	case 'p':
	    jprint->print_type_option = true;
	    jprint->print_type = jprint_parse_print_option(optarg);
	    break;
	case 'b':
	    jprint_parse_st_tokens_option(optarg, &jprint->num_token_spaces, &jprint->print_token_tab);
	    break;
	case 'L':
	    jprint->print_json_levels = true; /* print JSON levels */
	    jprint_parse_st_level_option(optarg, &jprint->num_level_spaces, &jprint->print_level_tab);
	    break;
	case 'P':
	    jprint->print_colons = true;
	    dbg(DBG_NONE, "-P specified, will print colons");
	    break;
	case 'C':
	    jprint->print_final_comma = true;
	    dbg(DBG_NONE, "-C specified, will print final comma");
	    break;
	case 'B':
	    jprint->print_braces = true;
	    dbg(DBG_NONE, "-B specified, will print braces");
	    break;
	case 'I':
	    jprint_parse_st_indent_option(optarg, &jprint->indent_level, &jprint->indent_tab);
	    break;
	case 'i':
	    jprint->case_insensitive = true; /* make case cruel :-) */
	    dbg(DBG_NONE, "-i specified, making matches case-insensitive");
	    break;
	case 'j':
	    jprint->print_syntax = true;
	    dbg(DBG_NONE, "-j, implying -p both");
	    jprint->print_type = jprint_parse_print_option("both");
	    dbg(DBG_NONE, "-j, implying -b 1");
	    jprint_parse_st_tokens_option("1", &jprint->num_token_spaces, &jprint->print_token_tab);
	    dbg(DBG_NONE, "-j, implying -e -Q");
	    jprint->encode_strings = true;
	    jprint->quote_strings = true;
	    dbg(DBG_NONE, "-j, implying -t any");
	    jprint->type = jprint_parse_types_option("any");
	    break;
	case 'E':
	    jprint->match_encoded = true;
	    dbg(DBG_NONE, "-E specified, will match encoded strings, not decoded strings");
	    break;
	case 's':
	    jprint->substrings_okay = true;
	    dbg(DBG_NONE, "-s specified, will match substrings");
	    break;
	case 'g':   /* allow grep-like ERE */
	    jprint->use_regexps = true;
	    dbg(DBG_NONE, "-g specified, name_args will be regexps");
	    break;
	case 'G': /* this pattern is a regexp but the name_args will be a normal pattern unless -g specified */
	    jprint->explicit_regexp = true;
	    if (add_jprint_pattern(jprint, true, false, optarg) == NULL) {
		free_jprint(&jprint);
		err(17, __func__, "failed to add regexp '%s' to patterns list", optarg);
		not_reached();
	    }
	    break;
	case 'c':
	    jprint->count_only = true;
	    dbg(DBG_NONE, "-c specified, will only show count of matches");
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'm': /* set maximum depth to traverse json tree */
	    if (!string_to_uintmax(optarg, &jprint->max_depth)) {
		free_jprint(&jprint);
		err(3, "jprint", "couldn't parse -m depth"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'K': /* run test code */
	    if (!jprint_run_tests()) {
		free_jprint(&jprint);
		exit(6); /*ooo*/
	    }
	    else {
		free_jprint(&jprint);
		exit(0); /*ooo*/
	    }
	    break;
	case 'Y':
	    /*
	     * Why is this option -Y? Why not Y? Because Y, that's why! Why
	     * besides, all the other good options were already taken. :-)
	     * Specifically the letter Y has a V in it and V would have been the
	     * obvious choice but both v and V are taken. This is why you'll
	     * have to believe us when we tell you that this is a good enough
	     * reason why! :-)
	     */
	    jprint->search_value = true;
	    jprint->type = jprint_parse_value_type_option(optarg);
	    break;
	case 'S':
	    /* -S path to tool */
	    tool_path = optarg;
	    dbg(DBG_NONE, "set tool path to: '%s'", tool_path);
	    break;
	case 'A':
	    /*
	     * -A args to tool. Requires use of -S. */
	    tool_args = optarg;
	    dbg(DBG_NONE, "set tool args to: '%s'", tool_args);
	    break;
	case 'o': /* -o, print entire file if valid JSON. Incompatible with patterns to search for. */
	    jprint->print_entire_file = true;
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

    /* use of -g conflicts with -s and is an error. -G and -s do not conflict. */
    if (jprint->use_regexps && jprint->substrings_okay) {
	free_jprint(&jprint);
	err(3, "jprint", "cannot use both -g and -s"); /*ooo*/
	not_reached();
    }

    /* check that both -j and -s were not used */
    if (jprint->print_syntax && jprint->substrings_okay) {
	free_jprint(&jprint);
	err(3, "jprint", "cannot use both -j and -s"); /*ooo*/
	not_reached();
    }

    /* check that if -b [num]t is used then -p both is true */
    if (jprint->print_token_tab && !jprint_print_name_value(jprint->print_type)) {
	free_jprint(&jprint);
	err(3, "jparse", "use of -b [num]t cannot be used without -p both"); /*ooo*/
	not_reached();
    }

    /*
     * check that -j and -p are not used together.
     *
     * NOTE: this means check if -p was explicitly used: the default is -p v but
     * -j conflicts with it and since -j enables a number of options it is
     * easier to just make it an error.
     */
    if (jprint->print_type_option && jprint->print_syntax) {
	free_jprint(&jprint);
	err(3, "jparse", "cannot use -j and explicit -p together"); /*ooo*/
	not_reached();
    }

    /* run specific sanity checks */
    jprint_sanity_chks(jprint, tool_path, tool_args);

    /* shift argc and argv for further processing */
    argc -= optind;
    argv += optind;

    /* must have at least one arg */
    if (argv[0] == NULL) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /* if argv[0] != "-" we will attempt to read from a regular file */
    if (strcmp(argv[0], "-") != 0) {
        /* check that first arg exists and is a readable regular file */
	if (!exists(argv[0])) {
	    free_jprint(&jprint);
	    err(4, "jprint", "%s: file does not exist", argv[0]); /*ooo*/
	    not_reached();
	} else if (!is_file(argv[0])) {
	    free_jprint(&jprint);
	    err(4, "jprint", "%s: not a regular file", argv[0]); /*ooo*/
	    not_reached();
	} else if (!is_read(argv[0])) {
	    free_jprint(&jprint);
	    err(4, "jprint", "%s: unreadable file", argv[0]); /*ooo*/
	    not_reached();
	}

	errno = 0; /* pre-clear errno for errp() */
	json_file = fopen(argv[0], "r");
	if (json_file == NULL) {
	    free_jprint(&jprint);
	    errp(4, "jprint", "%s: could not open for reading", argv[0]); /*ooo*/
	    not_reached();
	}
    } else { /* argv[0] is "-": will read from stdin */
	jprint->is_stdin = true;
	json_file = stdin;
    }

    /* the debug level will be increased at a later time */
    dbg(DBG_NONE, "maximum depth to traverse: %ju%s", jprint->max_depth, (jprint->max_depth == 0?" (no limit)":
		jprint->max_depth==JSON_DEFAULT_MAX_DEPTH?" (default)":""));

    if (jprint->search_value && argc != 2 && jprint->number_of_patterns != 1) {
	free_jprint(&jprint);
	err(18, "jprint", "-Y requires exactly one name_arg");
	not_reached();
    } else if (!jprint->search_value && argv[1] == NULL) {
	jprint->print_entire_file = true;   /* technically this boolean is redundant */
    }

    for (i = 1; argv[i] != NULL; ++i) {
	jprint->pattern_specified = true;

	if (add_jprint_pattern(jprint, jprint->use_regexps, jprint->substrings_okay, argv[i]) == NULL) {
	    err(19, __func__, "failed to add pattern (substrings %s) '%s' to patterns list",
		    jprint->substrings_okay?"OK":"ignored", argv[i]);
	    not_reached();
	}
    }

    if (jprint->search_value && jprint->number_of_patterns != 1) {
	/*
	 * special handling to make sure that if -Y is specified then only -G
	 * foo or one arg is specified after the file
	 */
	free_jprint(&jprint);
	err(20, "jprint", "-Y requires exactly one name_arg");
	not_reached();
    }


    /* run tool if -S used */
    if (tool_path != NULL) {
	/* try running via shell_cmd() first */
	if (tool_args != NULL) {
	    dbg(DBG_MED, "about to execute: %s %s %s >/dev/null 2>&1", tool_path, tool_args, argv[0]);
	    exit_code = shell_cmd(__func__, true, "% % -- %", tool_path, tool_args, argv[0]);
	} else {
	    dbg(DBG_MED, "about to execute: %s %s >/dev/null 2>&1", tool_path, argv[0]);
	    exit_code = shell_cmd(__func__, true, "% %", tool_path, argv[0]);
	}
	if(exit_code != 0) {
	    free_jprint(&jprint);
	    if (tool_args != NULL) {
		err(7, "jprint", "JSON check tool '%s' with args '%s' failed with exit code: %d",/*ooo*/
			tool_path, tool_args, exit_code);
	    } else {
		err(7, "jprint", "JSON check tool '%s' without args failed with exit code: %d",/*ooo*/
			tool_path, exit_code);
	    }
	    not_reached();
	}
	/* now open a write-only pipe */
	if (tool_args != NULL) {
	    tool_stream = pipe_open(__func__, true, true, "% % %", tool_path, tool_args, argv[0]);
	} else {
	    tool_stream = pipe_open(__func__, true, true, "% %", tool_path, argv[0]);
	}
	if (tool_stream == NULL) {
	    free_jprint(&jprint);
	    if (tool_args != NULL) {
		err(7, "jprint", "opening pipe to JSON check tool '%s' with args '%s' failed", tool_path, tool_args); /*ooo*/
	    } else {
		err(7, "jprint", "opening pipe to JSON check tool '%s' without args failed", tool_path); /*ooo*/
	    }
	    not_reached();
	} else {
	    /* XXX - process stream - XXX */
	}
    }

    /*
     * read in entire file BEFORE trying to parse it as json as the parser
     * function will close the file if not stdin
     */
    file_contents = read_all(json_file, &len);
    if (file_contents == NULL) {
	err(4, "jprint", "could not read in file: %s", argv[0]); /*ooo*/
	not_reached();
    }
    /* clear EOF status and rewind for parse_json_stream() */
    clearerr(json_file);
    rewind(json_file);

    json_tree = parse_json_stream(json_file, argv[0], &is_valid);
    if (!is_valid) {
	if (json_file != stdin) {
	    fclose(json_file);  /* close file prior to exiting */
	    json_file = NULL;   /* set to NULL even though we're exiting as a safety precaution */
	}

	/* free our jprint struct */
	free_jprint(&jprint);
	err(5, "jprint", "%s invalid JSON", argv[0]); /*ooo*/
	not_reached();
    }

    dbg(DBG_MED, "valid JSON");

    if (jprint->patterns != NULL && jprint->print_entire_file) {
	free_jprint(&jprint);
	err(3, "jprint", "printing the entire file not supported when searching for a pattern");/*ooo*/
	not_reached();
    }
    if (jprint->patterns != NULL && !jprint->print_entire_file) {
	/* TODO process name_args */
	for (pattern = jprint->patterns; pattern != NULL; pattern = pattern->next) {
	    if (pattern->pattern != NULL && *pattern->pattern) {
		/*
		 * XXX if matches found we set the boolean match_found to true to
		 * indicate exit code of 0 but currently no matches are checked. In
		 * other words in the future this setting of match_found will not always
		 * happen.
		 */
		jprint->match_found = true;

		if (pattern->use_regexp) {
		    dbg(DBG_NONE, "searching for %s regexp '%s'", pattern->use_value?"value":"name", pattern->pattern);
		} else {
		    dbg(DBG_NONE, "searching for %s %s '%s' (substrings %s)", pattern->use_value?"value":"name",
			pattern->use_regexp?"regexp":"pattern", pattern->pattern,
			pattern->use_substrings?"OK":"ignored");
		}
	    }
	}
    } else {
	/*
	 * XXX remove this informative message or change debug level once processing
	 * above is implemented.
	 */
	dbg(DBG_NONE,"no pattern requested or -o, will print entire file");
	if (file_contents != NULL) {
	    fpr(stdout, "jprint", "%s", file_contents);
	}
    }

    jprint_json_tree_search(jprint, json_tree, jprint->max_depth);
    jprint_print_matches(jprint);

    /* free tree */
    json_tree_free(json_tree, jprint->max_depth);

    /* All Done!!! -- Jessica Noll, Age 2 */
    if (jprint->match_found || !jprint->pattern_specified || jprint->print_entire_file) {
	free_jprint(&jprint);	/* free jprint struct */
	exit(0); /*ooo*/
    } else {
	free_jprint(&jprint);	/* free jprint struct */
	/*
	 * exit with 1 due to no pattern found
	 */
	exit(1); /*ooo*/
    }
}

/* alloc_jprint	    - allocate and clear out a struct jprint *
 *
 * This function returns a newly allocated and cleared struct jprint *.
 *
 * This function will never return a NULL pointer.
 */
struct jprint *
alloc_jprint(void)
{
    /* allocate our struct jprint */
    struct jprint *jprint = calloc(1, sizeof *jprint);

    /* verify jprint != NULL */
    if (jprint == NULL) {
	err(21, "jprint", "failed to allocate jprint struct");
	not_reached();
    }

    /* clear everything out explicitly even after calloc() */
    jprint->is_stdin = false;			/* if it's stdin */
    jprint->match_found = false;		/* true if a pattern is specified and there is a match */
    jprint->case_insensitive = false;		/* true if -i, case-insensitive */
    jprint->pattern_specified = false;		/* true if a pattern was specified */
    jprint->encode_strings = false;		/* -e used */
    jprint->quote_strings = false;		/* -Q used */
    jprint->type = JPRINT_TYPE_SIMPLE;		/* -t type used */

    /* number range options, see struct in jprint_util.h for details */

    /* max matches number range */
    jprint->jprint_max_matches.number = 0;
    jprint->jprint_max_matches.exact = false;
    jprint->jprint_max_matches.range.min = 0;
    jprint->jprint_max_matches.range.max = 0;
    jprint->jprint_max_matches.range.less_than_equal = false;
    jprint->jprint_max_matches.range.greater_than_equal = false;
    jprint->jprint_max_matches.range.inclusive = false;
    jprint->max_matches_requested = false;

    /* min matches number range */
    jprint->jprint_min_matches.number = 0;
    jprint->jprint_min_matches.exact = false;
    jprint->jprint_min_matches.range.min = 0;
    jprint->jprint_min_matches.range.max = 0;
    jprint->jprint_min_matches.range.less_than_equal = false;
    jprint->jprint_min_matches.range.greater_than_equal = false;
    jprint->jprint_min_matches.range.inclusive = false;
    jprint->min_matches_requested = false;

    /* levels number range */
    jprint->jprint_levels.number = 0;
    jprint->jprint_levels.exact = false;
    jprint->jprint_levels.range.min = 0;
    jprint->jprint_levels.range.max = 0;
    jprint->jprint_levels.range.less_than_equal = false;
    jprint->jprint_levels.range.greater_than_equal = false;
    jprint->jprint_levels.range.inclusive = false;
    jprint->levels_constrained = false;

    jprint->print_type = JPRINT_PRINT_VALUE;		/* -p type specified */
    jprint-> print_type_option = false;			/* -p explicitly used */
    jprint->num_token_spaces = 0;			/* -b specified number of spaces or tabs */
    jprint->print_token_tab = false;			/* -b tab (or -b <num>[t]) specified */
    jprint->print_json_levels = false;			/* -L specified */
    jprint->num_level_spaces = 0;			/* number of spaces or tab for -L */
    jprint->print_level_tab = false;			/* -L tab option */
    jprint->print_colons = false;			/* -P specified */
    jprint->print_final_comma = false;			/* -C specified */
    jprint->print_braces = false;			/* -B specified */
    jprint->indent_level = 0;				/* -I specified */
    jprint->indent_tab = false;				/* -I <num>[{t|s}] specified */
    jprint->print_syntax = false;			/* -j used, will imply -p b -b 1 -c -e -Q -I 4 -t any */
    jprint->match_encoded = false;			/* -E used, match encoded name */
    jprint->substrings_okay = false;			/* -s used, matching substrings okay */
    jprint->use_regexps = false;			/* -g used, allow grep-like regexps */
    jprint->explicit_regexp = false;			/* -G used, will not allow -Y */
    jprint->count_only = false;				/* -c used, only show count */
    jprint->print_entire_file = false;			/* no name_arg specified */
    jprint->max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */

    jprint->search_value = false;			/* -Y search by value, not name. Uses print type */

    /* finally the linked list of patterns */
    jprint->patterns = NULL;
    jprint->number_of_patterns = 0;

    return jprint;
}

/*
 * add_jprint_match
 *
 * Add jprint pattern match to the jprint struct pattern match list.
 *
 * given:
 *
 *	jprint		- pointer to the jprint struct
 *	pattern		- the pattern that matched
 *	level		- the depth or level for the -l / -L options (level 0 is top of tree)
 *
 * NOTE: this function will not return if any of the pointers are NULL including
 * the pointers in the pattern struct.
 *
 * Returns a pointer to the newly allocated struct jprint_match * that was
 * added to the jprint matched patterns list.
 */
struct jprint_match *
add_jprint_match(struct jprint *jprint, struct jprint_pattern *pattern, char *value, uintmax_t level)
{
    struct jprint_match *match = NULL;
    struct jprint_match *tmp = NULL;

    /*
     * firewall
     */
    if (jprint == NULL) {
	err(22, __func__, "passed NULL jprint struct");
	not_reached();
    }

    if (pattern == NULL) {
	err(23, __func__, "passed NULL pattern");
	not_reached();
    } else if (pattern->pattern == NULL) {
	err(24, __func__, "pattern->pattern is NULL");
	not_reached();
    }

    if (value == NULL) {
	err(25, __func__, "value is NULL");
	not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    match = calloc(1, sizeof *match);
    if (match == NULL) {
	errp(26, __func__, "unable to allocate struct jprint_match *");
	not_reached();
    }

    /* duplicate the name (pattern->pattern) */
    errno = 0; /* pre-clear errno for errp() */
    match->name = strdup(pattern->pattern);
    if (match->name == NULL) {
	errp(27, __func__, "unable to strdup string '%s' for match list", pattern->pattern);
	not_reached();
    }

    /* duplicate the value */
    errno = 0; /* pre-clear errno for errp() */
    match->value = strdup(value);
    if (match->value == NULL) {
	errp(28, __func__, "unable to strdup value string '%s' for match list", value);
	not_reached();
    }
    /* set level of the match for -l / -L options */
    match->level = level;

    /* record the pattern that was matched */
    match->pattern = pattern; /* DO NOT FREE THIS */

    /* set which match number this is, incrementing the pattern's total matches */
    match->number = pattern->matches_found++;

    dbg(DBG_NONE, "adding match '%s' to pattern '%s' to match list",
	    jprint->search_value?match->value:match->name, pattern->pattern);

    for (tmp = pattern->matches; tmp && tmp->next; tmp = tmp->next)
	; /* on its own line to silence useless and bogus warning -Wempty-body */

    if (!tmp) {
	pattern->matches = match;
    } else {
	tmp->next = match;
    }

    return match;
}

/* free_jprint_matches_list   - free matches list in a struct jprint_pattern *
 *
 * given:
 *
 *	pattern	    - the jprint pattern
 *
 * If the jprint patterns match list is empty this function will do nothing.
 *
 * NOTE: this function does not return on a NULL pattern.
 */
void
free_jprint_matches_list(struct jprint_pattern *pattern)
{
    struct jprint_match *match = NULL; /* to iterate through matches list */
    struct jprint_match *next_match = NULL; /* next in list */

    if (pattern == NULL) {
	err(29, __func__, "passed NULL pattern struct");
	not_reached();
    }

    for (match = pattern->matches; match != NULL; match = next_match) {
	next_match = match->next;
	if (match->name) {
	    free(match->name);
	    match->name = NULL;
	}

	if (match->value) {
	    free(match->value);
	    match->value = NULL;
	}

	/* DO NOT FREE match->pattern! */
	free(match);
	match = NULL;
    }

    pattern->matches = NULL;
}



/*
 * add_jprint_pattern
 *
 * Add jprint pattern to the jprint struct pattern list.
 *
 * given:
 *
 *	jprint		- pointer to the jprint struct
 *	use_regexp	- whether to use regexp or not
 *	use_substrings	- if -s was specified, make this a substring match
 *	str		- the pattern to be added to the list
 *
 * NOTE: this function will not return if jprint is NULL. If str is NULL
 * this function will not return but if str is empty it will add an empty
 * string to the list. However the caller will usually check that it's not empty
 * prior to calling this function.
 *
 * Returns a pointer to the newly allocated struct jprint_pattern * that was
 * added to the jprint patterns list.
 *
 * Duplicate patterns will not be added (case sensitive).
 */
struct jprint_pattern *
add_jprint_pattern(struct jprint *jprint, bool use_regexp, bool use_substrings, char *str)
{
    struct jprint_pattern *pattern = NULL;
    struct jprint_pattern *tmp = NULL;

    /*
     * firewall
     */
    if (jprint == NULL) {
	err(30, __func__, "passed NULL jprint struct");
	not_reached();
    }
    if (str == NULL) {
	err(31, __func__, "passed NULL str");
	not_reached();
    }

    /*
     * first make sure the pattern is not already added to the list as the same
     * type
     */
    for (pattern = jprint->patterns; pattern != NULL; pattern = pattern->next) {
	if (pattern->pattern && !strcmp(pattern->pattern, str) && pattern->use_regexp == use_regexp) {
	    return pattern;
	}
    }
    /*
     * XXX either change the debug level or remove this message once
     * processing is complete
     */
    if (use_regexp) {
	dbg(DBG_NONE,"%s regex requested: '%s'", jprint->search_value?"value":"name", str);
    } else {
	dbg(DBG_NONE,"%s pattern requested: '%s'", jprint->search_value?"value":"name", str);
    }

    errno = 0; /* pre-clear errno for errp() */
    pattern = calloc(1, sizeof *pattern);
    if (pattern == NULL) {
	errp(32, __func__, "unable to allocate struct jprint_pattern *");
	not_reached();
    }

    errno = 0;
    pattern->pattern = strdup(str);
    if (pattern->pattern == NULL) {
	errp(33, __func__, "unable to strdup string '%s' for patterns list", str);
	not_reached();
    }

    pattern->use_regexp = use_regexp;
    pattern->use_value = jprint->search_value;
    pattern->use_substrings = use_substrings;
    /* increment how many patterns have been specified */
    ++jprint->number_of_patterns;
    /* let jprint know that a pattern was indeed specified */
    jprint->pattern_specified = true;
    pattern->matches_found = 0; /* 0 matches found at first */

    dbg(DBG_NONE, "adding %s pattern '%s' to patterns list", pattern->use_value?"value":"name", pattern->pattern);

    for (tmp = jprint->patterns; tmp && tmp->next; tmp = tmp->next)
	; /* on its own line to silence useless and bogus warning -Wempty-body */

    if (!tmp) {
	jprint->patterns = pattern;
    } else {
	tmp->next = pattern;
    }

    return pattern;
}

/* free_jprint_patterns_list	- free patterns list in a struct jprint *
 *
 * given:
 *
 *	jprint	    - the jprint struct
 *
 * If the jprint patterns list is empty this function will do nothing.
 *
 * NOTE: this function does not return on a NULL jprint.
 *
 * NOTE: this function calls free_jprint_matches_list() on all the patterns
 * prior to freeing the pattern itself.
 */
void
free_jprint_patterns_list(struct jprint *jprint)
{
    struct jprint_pattern *pattern = NULL; /* to iterate through patterns list */
    struct jprint_pattern *next_pattern = NULL; /* next in list */

    if (jprint == NULL) {
	err(34, __func__, "passed NULL jprint struct");
	not_reached();
    }

    for (pattern = jprint->patterns; pattern != NULL; pattern = next_pattern) {
	next_pattern = pattern->next;

	/* first free any matches */
	free_jprint_matches_list(pattern);

	/* now free the pattern string itself */
	if (pattern->pattern) {
	    free(pattern->pattern);
	    pattern->pattern = NULL;
	}

	/* finally free the pattern and set to NULL for the next pass */
	free(pattern);
	pattern = NULL;
    }

    jprint->patterns = NULL;
}

/*
 * free_jprint	    - free jprint struct
 *
 * given:
 *
 *	jprint	    - a struct jprint **
 *
 * This function will do nothing other than warn on NULL pointer (even though
 * it's safe to free a NULL pointer though if jprint itself was NULL it would be
 * an error to dereference it).
 *
 * We pass a struct jprint ** so that in the caller jprint can be set to NULL to
 * remove the need to repeatedly set it to NULL each time this function is
 * called. This way we remove the need to do more than just call this function.
 */
void
free_jprint(struct jprint **jprint)
{
    if (jprint == NULL || *jprint == NULL) {
	warn(__func__, "passed NULL struct jprint ** or *jprint is NULL");
	return;
    }

    free_jprint_patterns_list(*jprint); /* free patterns list first */

    free(*jprint);
    *jprint = NULL;
}

/* jprint_sanity_chks	- sanity checks on jprint
 *
 * given:
 *
 *	jprint	    - pointer to our jprint struct
 *	tool_path   - path to tool if -S specified
 *	tool_args   - args to tool_path
 *
 * This function runs any important checks on the jprint internal state.
 * If passed a NULL pointer or anything is not sane this function will not
 * return.
 *
 * This function will not always run tests: it depends on the options specified
 * at the command line.
 */
void
jprint_sanity_chks(struct jprint *jprint, char const *tool_path, char const *tool_args)
{
    /* firewall */
    if (jprint == NULL) {
	err(35, __func__, "NULL jprint");
	not_reached();
    }

    /*
     * if -S specified then we need to verify that the tool is a regular
     * executable file
     */
    if (tool_path != NULL) {
	if (!exists(tool_path)) {
	    free_jprint(&jprint);
	    err(3, __func__, "jprint tool path does not exist: %s", tool_path);/*ooo*/
	    not_reached();
	} else if (!is_file(tool_path)) {
	    free_jprint(&jprint);
	    err(3, __func__, "jprint tool not a regular file: %s", tool_path); /*ooo*/
	    not_reached();
	} else if (!is_exec(tool_path)) {
	    free_jprint(&jprint);
	    err(3, __func__, "jprint tool not an executable file: %s", tool_path); /*ooo*/
	    not_reached();
	}
    }

    if (tool_args != NULL) {
	if (tool_path == NULL) {
	    /* it is an error if -A args specified without -S path */
	    free_jprint(&jprint);
	    err(3, __func__, "-A used without -S"); /*ooo*/
	    not_reached();
	} else if (tool_args == NULL || *tool_args == '\0') {
	    /*
	     * tool args should never be NULL but might be empty. The question
	     * is whether or not empty args should actually be an error. It
	     * shouldn't hurt if they are but for now it's an error.
	     */
	    free_jprint(&jprint);
	    err(3, __func__, "-A args NULL or empty"); /*ooo*/
	    not_reached();
	}
    }

    /* all good */
    return;
}

/*
 * jprint_json_search
 *
 * Print information about a JSON node, depending on the booleans in struct
 * jprint if the tree node matches the name or value in any pattern in the
 * struct json.
 *
 * given:
 *	jprint	    pointer to our struct jprint
 *	node	    pointer to a JSON parser tree node to try and match
 *	is_value    whether node is a value or name
 *	depth	    current tree depth (0 ==> top of tree)
 *	...	    extra args are ignored, required extra args if <=
 *		    json_verbosity_level:
 *
 *			stream		stream to print on
 *			json_dbg_lvl	print message if JSON_DBG_FORCED
 *					OR if <= json_verbosity_level
 *
 * Example use - print a JSON parse tree
 *
 *	jprint_json_search(node, true, depth, JSON_DBG_MED);
 *	jprint_json_search(node, false, depth, JSON_DBG_FORCED;
 *	jprint_json_search(node, false, depth, JSON_DBG_MED);
 *
 * While the ... variable arg are ignored, we need to declare
 * then in order to be in vcallback form for use by json_tree_walk().
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if jprint == NULL or node == NULL.
 */
void
jprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, ...)
{
    va_list ap;		/* variable argument list */

    /* firewall */
    if (jprint == NULL || node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, depth);

    /*
     * call va_list argument list function
     */
    vjprint_json_search(jprint, node, is_value, depth, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);
    return;
}


/*
 * vjprint_json_search
 *
 * Search for matches in a JSON node, depending on the booleans in struct
 * jprint if the tree node matches the name or value in any pattern in the
 * struct json.
 *
 * This is a variable argument list interface to jprint_json_search().
 *
 * See jprint_json_tree_search() to go through the entire tree.
 *
 * given:
 *	jprint	    pointer to our struct json
 *	node	    pointer to a JSON parser tree node to free
 *	is_value    boolean to indicate if this is a value or name
 *	depth	    current tree depth (0 ==> top of tree)
 *	ap	    variable argument list, required ap args:
 *
 *			json_dbg_lvl	print message if JSON_DBG_FORCED
 *					OR if <= json_verbosity_level
 *
 * NOTE: This function does nothing if jprint == NULL or jprint->patterns ==
 * NULL or if none of the names/values match any of the patterns or node ==
 * NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 *
 * NOTE: this function is incomplete and does not do everything correctly. Any
 * problems will be fixed in a future commit.
 */
void
vjprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, va_list ap)
{
    FILE *stream = NULL;	/* stream to print on */
    int json_dbg_lvl = JSON_DBG_DEFAULT;	/* JSON debug level if json_dbg_used */
    struct jprint_pattern *pattern = NULL; /* to iterate through patterns list */
    va_list ap2;		/* copy of va_list ap */

    /*
     * firewall - nothing to do for NULL jprint or NULL patterns list or NULL node
     */
    if (jprint == NULL || jprint->patterns == NULL || node == NULL) {
	return;
    }

    /*
     * duplicate va_list ap
     */
    va_copy(ap2, ap);

    /*
     * obtain the stream, json_dbg_used, and json_dbg args
     */
    stream = stdout;
    if (stream == NULL) {
	va_end(ap2); /* stdarg variable argument list clean up */
	return;
    }
    json_dbg_lvl = va_arg(ap2, int);

    /*
     * NOTE: why is it that we return if we're searching by value and it is a
     * value or if it's not a value and we're searching by name? Because as I
     * understand it at this time if we're searching for values we want to print
     * the name and not the value. The '-p' option will change this behaviour
     * anyway so this might have to change but at this time for simple JSON
     * files using -Y will add only names and not using it will add only values.
     * The pattern should itself have the pattern that matched (though currently
     * only basic matching is in - regexp not implemented yet) which is set to
     * the name in the match (it might be that it should be renamed as it isn't
     * necessarily always a name but this will be decided later). Note that this
     * only prints the match found regardless of the -p option. That will be
     * fixed later.
     */
    if ((jprint->search_value && is_value) || (!is_value && !jprint->search_value)) {
	va_end(ap2); /* stdarg variable argument list clean up */
	return;
    }

    /* if level is okay go through each pattern and print any matches */
    if (!jprint->levels_constrained || jprint_number_in_range(depth, jprint->number_of_patterns, &jprint->jprint_levels))
    {
	for (pattern = jprint->patterns; pattern != NULL; pattern = pattern->next) {
	    /* XXX: for each pattern we have to find a match and then add it to
	     * the matches list of that pattern. After that we can go through
	     * the matches found and print out the matches as desired by the
	     * user. We will not add matches if the constraints do not allow it.
	     */

		/*
		 * if there is a match found add it to the matches list
		 *
		 * XXX - for now everything is added as no matching is done yet - XXX
		 */
		switch (node->type) {

		case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
		    break;

		case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
		    {
			struct json_number *item = &(node->item.number);

			if (!strcmp(pattern->pattern, item->as_str)) {
			    if (add_jprint_match(jprint, pattern, item->as_str, depth) == NULL) {
				err(36, __func__, "adding match '%s' to pattern failed", item->as_str);
				not_reached();
			    }
			}
		    }
		    break;

		case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
		    {
			struct json_string *item = &(node->item.string);

			if (!strcmp(pattern->pattern, item->as_str)) {
			    if (add_jprint_match(jprint, pattern, item->as_str, depth) == NULL) {
				err(37, __func__, "adding match '%s' to pattern failed", item->as_str);
				not_reached();
			    }
			}
		    }
		    break;

		case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
		    {
			struct json_boolean *item = &(node->item.boolean);

			if (!strcmp(pattern->pattern, item->as_str)) {
			    if (add_jprint_match(jprint, pattern, item->as_str, depth) == NULL) {
				err(38, __func__, "adding match '%s' to pattern failed", item->as_str);
				not_reached();
			    }
			}
		    }
		    break;

		case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
		    {
			struct json_null *item = &(node->item.null);

			if (!strcmp(pattern->pattern, item->as_str)) {
			    if (add_jprint_match(jprint, pattern, item->as_str, depth) == NULL) {
				err(39, __func__, "adding match '%s' to pattern failed", item->as_str);
				not_reached();
			    }
			}
		    }
		    break;

		case JTYPE_MEMBER:	/* JSON item is a member - see struct json_member */
		    {
			struct json_member *item = &(node->item.member);

			/* XXX - fix to check for match of the member and add to
			 * the matches list if it fits within constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		case JTYPE_OBJECT:	/* JSON item is a { members } - see struct json_object */
		    {
			struct json_object *item = &(node->item.object);

			/* XXX - fix to check for match of the object and add to
			 * the matches list if it fits within constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		case JTYPE_ARRAY:	/* JSON item is a [ elements ] - see struct json_array */
		    {
			struct json_array *item = &(node->item.array);

			/* XXX - fix to check for match of the array and add it
			 * to the matches list if it fits within the constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values - see struct json_elements */
		    {
			struct json_elements *item = &(node->item.elements);

			/* XXX - fix to check for match of the element and add it
			 * to the matches list if it fits within the constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		default:
		    break;
		}
	    }
    }
    /*
     * stdarg variable argument list clean up
     */
    va_end(ap2);
    return;
}


/*
 * jprint_json_tree_search - print lines for an entire JSON parse tree.
 *
 * This function uses the jprint_json_tree_walk() interface to walk
 * the JSON parse tree and print requested information about matching nodes.
 *
 * given:
 *	jprint	    pointer to our struct jprint
 *	node	    pointer to a JSON parser tree node to free
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	...	extra args are ignored, required extra args:
 *
 *		json_dbg_lvl   print message if JSON_DBG_FORCED
 *			       OR if <= json_verbosity_level
 *
 * Example uses - print an entire JSON parse tree
 *
 *	jprint_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_FORCED);
 *	jprint_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_FORCED);
 *	jprint_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_MED);
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if jprint == NULL, jprint->patterns == NULL
 * or node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 *
 * NOTE: this function is a wrapper to jprint_json_tree_walk() with the callback
 * vjprint_json_search().
 */
void
jprint_json_tree_search(struct jprint *jprint, struct json *node, unsigned int max_depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (jprint == NULL || jprint->patterns == NULL || node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, max_depth);

    /*
     * walk the JSON parse tree
     */
    jprint_json_tree_walk(jprint, node, max_depth, false, 0, vjprint_json_search, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);
    return;
}

/*
 * jprint_json_tree_walk - walk a JSON parse tree calling a function on each node in va_list form
 *
 * This is the va_list form of json_tree_walk().
 *
 * Walk a JSON parse tree, Depth-first Post-order (LRN) order.  See:
 *
 *	https://en.wikipedia.org/wiki/Tree_traversal#Post-order,_LRN
 *
 * Example use - walk an entire JSON parse tree, looking for matches and
 * printing requested information on those matches.
 *
 *	jprint_json_tree_walk(jprint, tree, JSON_DEFAULT_MAX_DEPTH, 0, json_free);
 *
 * given:
 *	node	    pointer to a JSON parse tree
 *	is_value    if it's a value or name
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	depth	    current tree depth (0 ==> top of tree)
 *	vcallback   function to operate JSON parse tree node in va_list form
 *	ap	    variable argument list
 *
 * The vcallback() function must NOT call va_arg() nor call va_end() on the
 * va_list argument directly.  Instead they must call va_copy() and then use
 * va_arg() and va_end() on the va_list copy.
 *
 * Although in C ALL functions are pointers which means one can call foo()
 * as foo() or (*foo)() we use the latter format for the callback function
 * to make it clearer that it is in fact a function that's passed in so
 * that we can use this function to do more than one thing. This is also
 * why we call it callback and not something else.
 *
 * If max_depth is >= 0 and the tree depth > max_depth, then this function return.
 * In this case it will NOT operate on the node, or will be descend and further
 * into the tree.
 *
 * NOTE: This function warns but does not do anything if an arg is NULL.
 *
 * NOTE: this function might be incomplete or does something that is incorrect.
 * If this is the case it will be fixed in a future update.
 */
void
jprint_json_tree_walk(struct jprint *jprint, struct json *node, bool is_value, unsigned int max_depth, unsigned int depth,
		void (*vcallback)(struct jprint *, struct json *, bool, unsigned int, va_list), va_list ap)
{
    int i;

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return ;
    }
    if (vcallback == NULL) {
	warn(__func__, "vcallback is NULL");
	return ;
    }

    /*
     * do nothing if we are too deep
     */
    if (max_depth != JSON_INFINITE_DEPTH && depth > max_depth) {
	warn(__func__, "tree walk descent stopped, tree depth: %u > max_depth: %u", depth, max_depth);
	return;
    }

    /* if depth is 0 it can't be a value */
    if (depth == 0) {
	is_value = false;
    }

    /*
     * walk based on type of node
     */
    switch (node->type) {

    case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
	/* perform function operation on this terminal parse tree node, all of
	 * which have to be a value */
	(*vcallback)(jprint, node, true, depth, ap);
	break;

    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */

	/* perform function operation on this terminal parse tree node */
	(*vcallback)(jprint, node, is_value, depth, ap);
	break;

    case JTYPE_MEMBER:	/* JSON item is a member */
	{
	    struct json_member *item = &(node->item.member);

	    /* perform function operation on JSON member name (left branch) node */
	    jprint_json_tree_walk(jprint, item->name, false, max_depth, depth+1, vcallback, ap);

	    /* perform function operation on JSON member value (right branch) node */
	    jprint_json_tree_walk(jprint, item->value, true, max_depth, depth+1, vcallback, ap);
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth, ap);
	break;

    case JTYPE_OBJECT:	/* JSON item is a { members } */
	{
	    struct json_object *item = &(node->item.object);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jprint_json_tree_walk(jprint, item->set[i], is_value, max_depth, depth+1, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth, ap);
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] */
	{
	    struct json_array *item = &(node->item.array);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jprint_json_tree_walk(jprint, item->set[i], true, max_depth, depth+1, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth, ap);
	break;

    case JTYPE_ELEMENTS:	/* JSON items is zero or more JSON values */
	{
	    struct json_elements *item = &(node->item.elements);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jprint_json_tree_walk(jprint, item->set[i], true, max_depth, depth+1, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth, ap);
	break;

    default:
	warn(__func__, "node type is unknown: %d", node->type);
	/* nothing we can traverse */
	break;
    }
    return;
}

/* jprint_print_matches	    - print all matches found
 *
 * given:
 *
 *	jprint	    - our struct jprint with patterns list
 *
 * NOTE: this function will not return if jprint is NULL.
 *
 * NOTE: this function will only warn on empty patterns list.
 *
 * NOTE: if any pointer in a match is NULL this function will not return as it
 * indicates incorrect behaviour in the program as it should never have got this
 * far in the first place.
 */
void
jprint_print_matches(struct jprint *jprint)
{
    struct jprint_pattern *pattern = NULL;  /* to iterate through patterns list */
    struct jprint_match *match = NULL;	    /* to iterate through matches of each pattern in the patterns list */

    /* firewall */
    if (jprint == NULL) {
	err(40, __func__, "jprint is NULL");
	not_reached();
    } else if (jprint->patterns == NULL) {
	warn(__func__, "empty patterns list");
	return;
    }

    for (pattern = jprint->patterns; pattern != NULL; pattern = pattern->next) {
	for (match = pattern->matches; match != NULL; match = match->next) {
	    /* if the name of the match is NULL it is a fatal error */
	    if (match->name == NULL) {
		err(41, __func__, "match->name is NULL");
		not_reached();
	    } else if (*match->name == '\0') {
		/* warn on empty name for now and then go to next match */
		warn(__func__, "empty match name");
		continue;
	    }

	    if (match->value == NULL) {
		err(42, __func__, "match '%s' has NULL value", match->name);
		not_reached();
	    } else if (*match->value == '\0') {
		/* for now we only warn on empty value */
		warn(__func__, "empty value for match '%s'", match->name);
		continue;
	    }

	    /* print the match if constraints allow it
	     *
	     * XXX - add final constraint checks
	     *
	     * XXX - This is buggy in some cases. This must be fixed.
	     */
	    if (jprint_print_name_value(jprint->print_type)) {
		print("%s\n", match->name);
		print("%s\n", match->value);
	    } else if (jprint_print_name(jprint->print_type)) {
		print("%s\n", match->name);
	    } else if (jprint_print_value(jprint->print_type)) {
		print("%s\n", match->value);
	    }
	    /*
	     * XXX: more functions will have to be added to print the values
	     * and currently the struct jprint_match struct is a work in
	     * progress. More will have to be added like the JSON type that
	     * matched (this includes not only the jprint type but the JSON
	     * type).
	     */

	}
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
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg3);
    fprintf_usage(exitcode, stderr, usage_msg4, json_parser_version, JPRINT_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
