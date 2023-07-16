/*
 * jfmt - JSON printer
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

#include "jfmt.h"

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
    "usage: %s [-h] [-V] [-v level] [-J level] [-e] [-q] [-Q] [-t type] [-n count]\n"
    "\t\t[-N num] [-p {n,v,b}] [-b <num>{[t|s]}] [-L <num>{[t|s]}] [-P] [-C] [-B]\n"
    "\t\t[-I <num>{[t|s]}] [-j] [-E] [-i] [-s] [-g] [-c] [-m depth] [-K] [-Y type]\n"
    "\t\t[-o] [-r] [-S path] [-A args] file.json [name_arg ...]\n"
    "\n"
    "\t-h\t\tPrint help and exit\n"
    "\t-V\t\tPrint version and exit\n"
    "\t-v level\tVerbosity level (def: %d)\n"
    "\t-J level\tJSON verbosity level (def: %d)\n"
    "\n"
    "\t-e\t\tPrint JSON strings as encoded strings (def: decode JSON strings)\n"
    "\t-q\t\tQuiet mode (def: print stuff to stdout)\n"
    "\t-Q\t\tPrint JSON strings surrounded by double quotes (def: do not)\n"
    "\t-t type\t\tMatch only one of the these comma-separated types (def: simple):\n"
    "\n"
    "\t\t\t\tint\t\tinteger values\n"
    "\t\t\t\tfloat\t\tfloating point values\n"
    "\t\t\t\texp\t\texponential notation values\n"
    "\t\t\t\tnum\t\talias for 'int,float,exp'\n"
    "\t\t\t\tbool\t\tboolean values\n"
    "\t\t\t\tstr\t\tstring values\n"
    "\t\t\t\tnull\t\tnull values\n"
    "\t\t\t\tsimple\t\talias for 'int,float,exp,bool,str,null' (the default)\n"
    "\t\t\t\tmember\t\tJSON members\n"
    "\t\t\t\tobject\t\tJSON objects\n"
    "\t\t\t\tarray\t\tJSON arrays\n"
    "\t\t\t\tcompound\talias for 'object,array'\n"
    "\t\t\t\tany\t\tany type of value\n"
    "\n"
    "\t\t\tMatches are based on JSON names unless -Y is used in which case JSON values are matched.\n";

static const char * const usage_msg1 =
    "\t-l lvl\t\tPrint values at specific JSON levels (def: print any level)\n"
    "\n"
    "\t\t\tIf lvl is a number (e.g.: -l 3), level must == number.\n"
    "\t\t\tIf lvl is a number followed by : (e.g. '-l 3:'), level must be >= number.\n"
    "\t\t\tIf lvl is a : followed by a number (e.g. '-l :3'), level must be <= number.\n"
    "\t\t\tIf lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range.\n"
    "\n"
    "\t-n count\tPrint up to count matches (def: print all matches)\n"
    "\n"
    "\t\t\tIf count is a number (e.g. '-n 3'), the matches must == number\n"
    "\t\t\tIf count is a number followed by : (e.g. '-n 3:'), matches must be >= number.\n"
    "\t\t\tIf count is a : followed by a number (e.g. '-n :3'), matches must be <= number.\n"
    "\t\t\tIf count is num:num (e.g. '-n 3:5'), matches must be inclusively in the range.\n"
    "\t\t\tA number < 0 refers printing from last match (e.g. '-n -1' will print last match).\n"
    "\n"
    "\t-N num\t\tPrint only if there are only a given number of matches (def: do not limit)\n"
    "\n"
    "\t\t\tIf num is only a number (e.g. '-l 1'), there must be only that many matches.\n"
    "\t\t\tIf num is a number followed by : (e.g. '-l 3:'), there must >= num matches.\n"
    "\t\t\tIf num is a : followed by a number (e.g. '-n :3'), there must <= num matches.\n"
    "\t\t\tIf num is num:num (e.g. '-n 3:5'), the number of matches must be inclusively in the range.\n"
    "\n"
    "\t-p {n,v,b}\tPrint JSON key, value or both (def: print JSON values or JSON names if -Y is used)\n"
    "\t-p name\t\tAlias for '-p n'\n"
    "\t-p value\tAlias for '-p v'\n"
    "\t-p both\t\tAlias for '-p n,v'\n"
    "\n"
    "\t\t\tIt is an error to use -p n or -p v with -j.\n"
    "\n"
    "\t-b <num>[{t|s}]\tPrint specified number of tabs or spaces between JSON tokens printed via -j (def: 1 space)\n"
    "\t-b tab\t\tAlias for '-b 1t'\n"
    "\n"
    "\t\t\tNot specifying a character after the number implies spaces.\n"
    "\t\t\tThis option is only useful with -j or -p b or -p both.\n"
    "\t\t\tIt is an error to use -b without -p b or -p both.\n"
    "\n"
    "\t-L <num>[{t|s}]\tPrint JSON level (root is 0) followed by a number of tabs or spaces (def: don't print levels)\n"
    "\t-L tab\t\tAlias for: '-L 1t'\n"
    "\n"
    "\t\t\tTrailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces.\n"
    "\t\t\tNot specifying 's' or 't' implies spaces.\n";

static const char * const usage_msg2 =
    "\t-P\t\tWhen printing both i.e. '-p both', separate name/value by a : (colon) (def: do not)\n"
    "\n"
    "\t\t\tWhen -P is used with -b, the same whitespace is used round the : (colon).\n"
    "\n"
    "\t-C\t\tWhen printing JSON syntax, always print a comma after final line (def: do not)\n"
    "\n"
    "\t\t\tUse of -C without -j has no effect.\n"
    "\n"
    "\t-B\t\tWhen printing JSON syntax, start with a '{' line and end with a '}' line.\n"
    "\n"
    "\t\t\tUse of -B without -j has no effect.\n"
    "\t\t\tUse of both -B and -c is an error.\n"
    "\n"
    "\t-I <num>{[t|s]}\tWhen printing JSON syntax, indent levels (def: do not indent)\n"
    "\t-I tab\t\tAlias for '-I 1t'\n"
    "\n"
    "\t\t\tTrailing 't' implies indent with number of tabs whereas trailing 's' implies spaces.\n"
    "\t\t\tNot specifying 's' or 't' implies spaces.\n"
    "\n"
    "\t-j\t\tPrint using JSON syntax (def: do not)\n"
    "\n"
    "\t\t\tUse of -j implies '-p both -b 1s -e -Q -I 4 -t any'.\n"
    "\t\t\tSubsequent use of options after -j will change the effect of -j.\n"
    "\t\t\tUse of both -p and -j is an error.\n"
    "\n"
    "\t-E\t\tMatch the JSON encoded name (def: match the JSON decoded name)\n"
    "\t-i\t\tIgnore case of name (def: case matters)\n"
    "\t-s\t\tSubstrings are used to match (def: the full name must match)\n"
    "\t-g\t\tMatch using grep-like extended regular expressions (def: match strings or substrings if -s)\n"
    "\n"
    "\t\t\tTo match from the beginning, start name_arg with '^'.\n"
    "\t\t\tTo match to the end, end name_arg with '$'.\n"
    "\t\t\tUse of -g and -s is an error.\n"
    "\n"
    "\t-c\t\tOnly show count of matches found\n"
    "\n"
    "\t\t\tUse of -c with -B, -L, -j, or -I is an error.\n"
    "\n"
    "\t-m max_depth\tSet the maximum JSON level depth to max_depth (0 == infinite depth, def: %d)\n"
    "\n"
    "\t\t\tA 0 max_depth implies JSON_INFINITE_DEPTH: only safe with infinite variable size and RAM :-)\n"
    "\n"
    "\t-K\t\tRun tests on jfmt constraints\n";

static const char * const usage_msg3 =
    "\t-Y type\t\tSearch for a JSON value of a given comma-separated type (def: search for JSON names)\n"
    "\n"
    "\t\t\t\tint\tinteger values\n"
    "\t\t\t\tfloat\tfloating point values\n"
    "\t\t\t\texp\texponential notation values\n"
    "\t\t\t\tnum\talias for 'int,float,exp'\n"
    "\t\t\t\tbool\tboolean values\n"
    "\t\t\t\tstr\tstring values\n"
    "\t\t\t\tnull\tnull values\n"
    "\t\t\t\tsimple\talias for 'int,float,exp,bool,str,null'\n"
    "\n"
    "\t\t\tUse of -Y requires one and only one name_arg.\n"
    "\t\t\tUse of -Y changes the default from -p value to -p name.\n"
    "\n"
    "\t-o\t\tSearch by OR mode like grep -E 'foo|bar' (def: don't)\n"
    "\t-r\t\tSearch under anywhere (def: don't)\n"
    "\n"
    "\t-S path\t\tRun JSON check tool, path, with file.json arg, abort of non-zero exit (def: do not run)\n"
    "\t-A args\t\tRun JSON check tool with additional args passed to the tool after file.json (def: none)\n"
    "\n"
    "\t\t\tUse of -A requires use of -S.\n";

/*
 * NOTE: this next one should be the last number; if any additional usage message strings
 * have to be added the first additional one should be the number this is and this one
 * should be changed to be the final string before this one + 1. Similarly if a
 * string can be removed this one should have its number changed to be + 1 from
 * the last one before it.
 */
static const char * const usage_msg4 =
    "\tfile.json\tJSON file to parse (- ==> read from stdin)\n"
    "\tname_arg\tsearch file.json for JSON name(s) (or one value if -Y) (def: match all)\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK: file is valid JSON, match(es) found or no name_arg given OR test mode OK\n"
    "    1\tfile is valid JSON, name_arg given but no matches found\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tinvalid command line, invalid option or option missing an argument\n"
    "    4\tfile does not exist, not a file, or unable to read the file\n"
    "    5\tfile contents is not valid JSON\n"
    "    6\ttest mode failed\n"
    "    7\tJSON check tool failed\n"
    " >=10\tinternal error\n"
    "\n"
    "JSON parser version: %s\n"
    "jfmt version: %s";

/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));

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
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg2, JSON_DEFAULT_MAX_DEPTH);
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg3);
    fprintf_usage(exitcode, stderr, usage_msg4, json_parser_version, JFMT_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


int
main(int argc, char **argv)
{
    char const *program = NULL;		/* our name */
    extern char *optarg;
    extern int optind;
    struct jfmt *jfmt = NULL;	/* struct of all our options and other things */
    struct jfmt_pattern *pattern = NULL; /* iterate through patterns list to search for matches */
    size_t len = 0;			/* length of file contents */
    bool is_valid = false;		/* if file is valid json */
    int exit_code = 0;			/* for the end */
    int i;

    jfmt = alloc_jfmt();		/* allocate our struct jfmt * */
    /*
     * the alloc_jfmt() will never return a NULL pointer but check just in
     * case
     */
    if (jfmt == NULL) {
	err(23, "jfmt", "failed to allocate jfmt struct");
	not_reached();
    }

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hVv:J:l:eQt:qjn:N:p:b:L:PCBI:jEim:cg:KY:sorS:A:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    free_jfmt(&jfmt);
	    usage(2, program, "");	/*ooo*/
	    not_reached();
	    break;
	case 'V':		/* -V - print version and exit */
	    free_jfmt(&jfmt);
	    print("%s\n", JFMT_VERSION);
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
	    jfmt->levels_constrained = true;
	    jfmt_parse_number_range("-l", optarg, false, &jfmt->jfmt_levels);
	    break;
	case 'e':
	    jfmt->encode_strings = true;
	    dbg(DBG_LOW, "-e specified, will encode strings");
	    break;
	case 'Q':
	    jfmt->quote_strings = true;
	    dbg(DBG_LOW, "-Q specified, will quote strings");
	    break;
	case 't':
	    jfmt->json_types_specified = true;
	    jfmt->json_types = jfmt_parse_types_option(optarg);
	    break;
	case 'n':
	    jfmt_parse_number_range("-n", optarg, true, &jfmt->jfmt_max_matches);
	    jfmt->max_matches_requested = true;
	    break;
	case 'N':
	    jfmt_parse_number_range("-N", optarg, false, &jfmt->jfmt_min_matches);
	    jfmt->min_matches_requested = true;
	    break;
	case 'p':
	    jfmt->print_json_types_option = true;
	    jfmt->print_json_types = jfmt_parse_print_option(optarg);
	    break;
	case 'b':
	    jfmt->print_token_spaces = true;
	    jfmt_parse_st_tokens_option(optarg, &jfmt->num_token_spaces, &jfmt->print_token_tab);
	    break;
	case 'L':
	    jfmt->print_json_levels = true; /* print JSON levels */
	    jfmt_parse_st_level_option(optarg, &jfmt->num_level_spaces, &jfmt->print_level_tab);
	    break;
	case 'P':
	    jfmt->print_colons = true;
	    dbg(DBG_LOW, "-P specified, will print colons");
	    break;
	case 'C':
	    jfmt->print_final_comma = true;
	    dbg(DBG_LOW, "-C specified, will print final comma");
	    break;
	case 'B':
	    jfmt->print_braces = true;
	    dbg(DBG_LOW, "-B specified, will print braces");
	    break;
	case 'I':
	    jfmt->indent_levels = true;
	    jfmt_parse_st_indent_option(optarg, &jfmt->indent_spaces, &jfmt->indent_tab);
	    break;
	case 'i':
	    jfmt->ignore_case = true; /* make case cruel :-) */
	    dbg(DBG_LOW, "-i specified, making matches case-insensitive");
	    break;
	case 'j':
	    jfmt->print_syntax = true;
	    dbg(DBG_LOW, "-j, implying -p both");
	    jfmt->print_json_types = jfmt_parse_print_option("both");
	    dbg(DBG_LOW, "-j, implying -b 1");
	    jfmt_parse_st_tokens_option("1", &jfmt->num_token_spaces, &jfmt->print_token_tab);
	    dbg(DBG_LOW, "-j, implying -e -Q");
	    jfmt->encode_strings = true;
	    jfmt->quote_strings = true;
	    dbg(DBG_LOW, "-j, implying -t any");
	    /* don't set jfmt->json_types_specified as that's for explicit use of -t */
	    jfmt->json_types = jfmt_parse_types_option("any");
	    break;
	case 'E':
	    jfmt->match_encoded = true;
	    dbg(DBG_LOW, "-E specified, will match encoded strings, not decoded strings");
	    break;
	case 's':
	    jfmt->use_substrings = true;
	    dbg(DBG_LOW, "-s specified, will match substrings");
	    break;
	case 'g':   /* allow grep-like ERE */
	    jfmt->use_regexps = true;
	    dbg(DBG_LOW, "-g specified, name_args will be regexps");
	    break;
	case 'c':
	    jfmt->count_only = true;
	    dbg(DBG_LOW, "-c specified, will only show count of matches");
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'm': /* set maximum depth to traverse json tree */
	    if (!string_to_uintmax(optarg, &jfmt->max_depth)) {
		free_jfmt(&jfmt);
		err(3, "jfmt", "couldn't parse -m depth"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'K': /* run test code */
	    if (!jfmt_run_tests()) {
		free_jfmt(&jfmt);
		exit(6); /*ooo*/
	    }
	    else {
		free_jfmt(&jfmt);
		exit(0); /*ooo*/
	    }
	    break;
	case 'Y':
	    /*
	     * Why is this option -Y? Why not Y? Because Y, that's why Y! Why
	     * besides, all the other good options were already taken. :-)
	     * Specifically the letter Y has a V in it and V would have been the
	     * obvious choice but both v and V are taken. This is why you'll
	     * have to believe us when we tell you that this is a good enough
	     * reason why Y! :-)
	     */
	    jfmt->search_value = true;
	    jfmt->json_types = jfmt_parse_value_type_option(optarg);
	    break;
	case 'o': /* search with OR mode */
	    jfmt->search_or_mode = true;
	    break;
	case 'r': /* search under anywhere */
	    jfmt->search_anywhere = true;  /* Why is -o before -r? To spell out OR when -o itself is OR! :-) */
	    break;
	case 'S':
	    /* -S path to tool */
	    jfmt->check_tool_specified = true;
	    jfmt->check_tool_path = optarg;
	    dbg(DBG_LOW, "set tool path to: '%s'", jfmt->check_tool_path);
	    break;
	case 'A':
	    /*
	     * -A args to tool. Requires use of -S. */
	    jfmt->check_tool_args = optarg;
	    dbg(DBG_LOW, "set tool args to: '%s'", jfmt->check_tool_args);
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
     * the wrong number of arguments (if they do). Not everything can be checked
     * prior to doing this though.
     */

    /* run specific sanity checks on options etc. */
    jfmt->json_file = jfmt_sanity_chks(jfmt, program, &argc, &argv);


    /*
     * jfmt_sanity_chks() should never return a NULL FILE * but we check
     * anyway
     */
    if (jfmt->json_file == NULL) {
	/*
	 * NOTE: don't make this exit code 3 as it's an internal error if the
	 * jfmt_sanity_chks() returns a NULL pointer.
	 */
	err(24, "jfmt", "could not open regular readable file");
	not_reached();
    }

    /* Before we can process the -S option, if it specified (which we know is
     * sane), we have to read in the JSON file (either stdin or otherwise) and
     * then verify that the JSON is valid.
     *
     * Read in entire file BEFORE trying to parse it as json as the parser
     * function will close the file if not stdin.
     *
     * NOTE: why doesn't the jfmt_sanity_chks() function do this? Because this
     * is not so much about a sane environment as much as being unable to
     * continue after verify the command line is correct.
     */
    jfmt->file_contents = read_all(jfmt->json_file, &len);
    if (jfmt->file_contents == NULL) {
	err(4, "jfmt", "could not read in file: %s", argv[0]); /*ooo*/
	not_reached();
    }
    /* clear EOF status and rewind for parse_json_stream() */
    clearerr(jfmt->json_file);
    rewind(jfmt->json_file);

    /* run -S tool */
    run_jfmt_check_tool(jfmt, argv);

    jfmt->json_tree = parse_json_stream(jfmt->json_file, argv[0], &is_valid);
    if (!is_valid || jfmt->json_tree == NULL) {
	if (jfmt->json_file != stdin) {
	    fclose(jfmt->json_file);  /* close file prior to exiting */
	    jfmt->json_file = NULL;   /* set to NULL even though we're exiting as a safety precaution */
	}

	/* free our jfmt struct */
	free_jfmt(&jfmt);
	err(5, "jfmt", "%s invalid JSON", argv[0]); /*ooo*/
	not_reached();
    }

    dbg(DBG_MED, "valid JSON");


    /* search for any patterns */
    jfmt_json_tree_search(jfmt, jfmt->json_tree, jfmt->max_depth);

    /* report, if debug level high enough, what will be searched for. */
    if (jfmt->patterns != NULL && !jfmt->print_entire_file) {
	for (pattern = jfmt->patterns; pattern != NULL; pattern = pattern->next) {
	    if (pattern->pattern != NULL && *pattern->pattern) {

		if (pattern->use_regexp) {
		    dbg(DBG_LOW, "searching for %s regexp '%s'", pattern->use_value?"value":"name", pattern->pattern);
		} else {
		    dbg(DBG_LOW, "searching for %s %s '%s' (substrings %s)", pattern->use_value?"value":"name",
			pattern->use_regexp?"regexp":"pattern", pattern->pattern,
			pattern->use_substrings?"OK":"ignored");
		}
	    }
	}
    }

    /*
     * we can't have this in the sanity checks function very easily as we don't
     * want to read in the entire contents from that function
     *
     * XXX - printing the entire file is incorrect here as it needs to print it
     * according to the options - XXX
     */
    if (!jfmt->print_entire_file || jfmt->count_only) {
	jfmt_print_matches(jfmt);
    } else if (jfmt->file_contents != NULL) {
	dbg(DBG_MED, "no pattern requested and no -c, will print entire file");
	fpr(stdout, "jfmt", "%s", jfmt->file_contents);
    }

    /* free tree */
    json_tree_free(jfmt->json_tree, jfmt->max_depth);

    /* All Done!!! -- Jessica Noll, Age 2 */
    if (jfmt->match_found || !jfmt->pattern_specified || jfmt->print_entire_file) {
	exit_code = 0;
    } else {
	exit_code = 1;
    }
    if (jfmt != NULL) {
	free_jfmt(&jfmt);	/* free jfmt struct */
    }

    exit(exit_code); /*ooo*/
}

/* jfmt_sanity_chks	- sanity checks on jfmt tool options
 *
 * given:
 *
 *	jfmt	    - pointer to our jfmt struct
 *	program	    - program name
 *	argc	    - pointer to argc from main()
 *	argv	    - pointer to argv from main()
 *
 * This function runs any important checks on the jfmt internal state. It also
 * runs checks that a file arg is specified and that the right number of options
 * are specified done after options are parsed.
 *
 * If passed a NULL pointer or anything is not sane this function will not
 * return.
 *
 * This function returns a FILE *, the file to read the json from. It will not
 * return if this cannot be done (i.e. it will never return a NULL pointer
 * though main() still checks to be defensive).
 *
 * NOTE: this function does NOT check for valid JSON.
 *
 * NOTE: jfmt->check_tool_path and jfmt->check_tool_args can be NULL.
 *
 * NOTE: this function must be in jfmt.c, not jfmt_util.h, because it uses
 * the usage() function which needs to be in this file.
 */
FILE *
jfmt_sanity_chks(struct jfmt *jfmt, char const *program, int *argc, char ***argv)
{
    /* firewall */
    if (jfmt == NULL) {
	err(25, __func__, "NULL jfmt");
	not_reached();
    } else if (argc == NULL) {
	err(26, __func__, "NULL argc");
	not_reached();
    } else if (argv == NULL || *argv == NULL || **argv == NULL) {
	err(27, __func__, "NULL argv");
	not_reached();
    } else if (program == NULL) {
	err(28, __func__, "NULL program");
	not_reached();
    }

    /*
     * first check for invalid option combinations which if any found it is a
     * command line error.
     */

    /* use of -g conflicts with -s and is an error. */
    if (jfmt->use_regexps && jfmt->use_substrings) {
	free_jfmt(&jfmt);
	err(3, __func__, "cannot use both -g and -s"); /*ooo*/
	not_reached();
    }

    /* check that if -b [num]t is used then -p both is true */
    if (jfmt->print_token_tab && !jfmt_print_name_value(jfmt->print_json_types)) {
	free_jfmt(&jfmt);
	err(3, "jparse", "use of -b [num]t cannot be used without printing both name and value"); /*ooo*/
	not_reached();
    }

    /*
     * check that if -j was used that printing both name and value is used. -j
     * does this but it's possible the user explicitly used -p after -j but if
     * they did not specify 'b' or 'both' it is an error.
     */
    if (jfmt->print_syntax && !jfmt_print_name_value(jfmt->print_json_types)) {
	free_jfmt(&jfmt);
	err(3, "jparse", "cannot use -j without printing both name and value"); /*ooo*/
	not_reached();
    }


    /* use of -c with any of any of -B, -L, -j and -I is an error */
    if (jfmt->count_only) {
	if (jfmt->print_braces) {
	    err(3, __func__, "cannot use -B and -c together"); /*ooo*/
	    not_reached();
	}
	if (jfmt->print_json_levels) {
	    err(3, __func__, "cannot use -L and -c together"); /*ooo*/
	    not_reached();
	}
	if (jfmt->print_syntax) {
	    err(3, __func__, "cannot use -j and -c together"); /*ooo*/
	    not_reached();
	}
	if (jfmt->indent_levels) {
	    err(3, __func__, "cannot use -I and -c together"); /*ooo*/
	    not_reached();
	}
    }

    /*
     * shift argc and argv for further processing. They're a pointer to those in
     * main() so we have to dereference them here because main() also requires
     * that they are shifted.
     */
    (*argc) -= optind;
    (*argv) += optind;

    /* must have at least one arg */
    if ((*argv)[0] == NULL) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /* if argv[0] != "-" we will attempt to open a regular readable file */
    if (strcmp((*argv)[0], "-") != 0) {
        /* check that the path exists and is a regular readable file */
	if (!exists((*argv)[0])) {
	    free_jfmt(&jfmt);
	    err(4, __func__, "%s: file does not exist", (*argv)[0]); /*ooo*/
	    not_reached();
	} else if (!is_file((*argv)[0])) {
	    free_jfmt(&jfmt);
	    err(4, __func__, "%s: not a regular file", (*argv)[0]); /*ooo*/
	    not_reached();
	} else if (!is_read((*argv)[0])) {
	    free_jfmt(&jfmt);
	    err(4, __func__, "%s: unreadable file", (*argv[0])); /*ooo*/
	    not_reached();
	}

	errno = 0; /* pre-clear errno for errp() */
	jfmt->json_file = fopen((*argv)[0], "r");
	if (jfmt->json_file == NULL) {
	    free_jfmt(&jfmt);
	    errp(4, __func__, "%s: could not open for reading", (*argv)[0]); /*ooo*/
	    not_reached();
	}
    } else { /* argv[0] is "-": will read from stdin */
	jfmt->is_stdin = true;
	jfmt->json_file = stdin;
    }

    dbg(DBG_LOW, "maximum depth to traverse: %ju%s", jfmt->max_depth, (jfmt->max_depth == 0?" (no limit)":
		jfmt->max_depth==JSON_DEFAULT_MAX_DEPTH?" (default)":""));

    if (jfmt->search_value && *argc != 2 && jfmt->number_of_patterns != 1) {
	free_jfmt(&jfmt);
	err(29, __func__, "-Y requires exactly one name_arg");
	not_reached();
    } else if (!jfmt->search_value && (*argv)[1] == NULL && !jfmt->count_only) {
	jfmt->print_entire_file = true;   /* technically this boolean is redundant */
    }


    /*
     * if -S specified then we need to verify that the tool is a regular
     * executable file
     */
    if (jfmt->check_tool_path != NULL) {
	if (!exists(jfmt->check_tool_path)) {
	    err(3, __func__, "jfmt tool path does not exist: %s", jfmt->check_tool_path);/*ooo*/
	    not_reached();
	} else if (!is_file(jfmt->check_tool_path)) {
	    err(3, __func__, "jfmt tool not a regular file: %s", jfmt->check_tool_path); /*ooo*/
	    not_reached();
	} else if (!is_exec(jfmt->check_tool_path)) {
	    err(3, __func__, "jfmt tool not an executable file: %s", jfmt->check_tool_path); /*ooo*/
	    not_reached();
	}
    }

    /*
     * if -A args is specified then we must have an -S tool as well */
    if (jfmt->check_tool_args != NULL) {
	if (jfmt->check_tool_path == NULL) {
	    /* it is an error if -A args specified without -S path */
	    free_jfmt(&jfmt);
	    err(3, __func__, "-A used without -S"); /*ooo*/
	    not_reached();
	} else if (jfmt->check_tool_args == NULL) {
	    free_jfmt(&jfmt);
	    err(3, __func__, "-A args NULL"); /*ooo*/
	    not_reached();
	}
    }

    /*
     * final processing: some options require the use of others but they are not
     * an error if they not used together; the one simply has no effect. Also
     * once options are parsed we have to check name_args and verify some things
     * after that.
     */

    /* without -j, -B has no effect */
    if (jfmt->print_braces && !jfmt->print_syntax) {
	jfmt->print_braces = false;
    }

    /* without -j, -C has no effect */
    if (jfmt->print_final_comma && !jfmt->print_syntax) {
	jfmt->print_final_comma = false;
    }

    /* parse name_args first */
    parse_jfmt_name_args(jfmt, *argv);

    /* now verify final options that require looking at name_args first */
    if (jfmt->search_value && jfmt->number_of_patterns != 1) {
	/*
	 * special handling to make sure that if -Y is specified then only one
	 * arg is specified after the file
	 */
	free_jfmt(&jfmt);
	err(3, __func__, "-Y requires exactly one name_arg"); /*ooo*/
	not_reached();
    }

    if (jfmt->count_only && jfmt->patterns == NULL) {
	err(3, __func__, "use of -c without any patterns is an error"); /*ooo*/
	not_reached();
    }

    /*
     * verify that if patterns list is not NULL that we're not printing the
     * entire file
     */
    if (jfmt->patterns != NULL && jfmt->print_entire_file) {
	free_jfmt(&jfmt);
	err(3, __func__, "printing the entire file not supported when searching for a pattern");/*ooo*/
	not_reached();
    }

    /* all good: return the (presumably) json FILE * */
    return jfmt->json_file;
}
