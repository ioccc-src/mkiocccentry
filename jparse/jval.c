/*
 * jval - JSON value printer
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

#include "jval.h"

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
    "usage:\t%s [-h] [-V] [-v level] [-J level] [-q] [-L <num>{[t|s]}] [-t type] [-l lvl]\n"
    "\t[-Q] [-D] [-d] [-i] [-s] [-f] [-c] [-C] [-g] [-e] [-n op=num] [-S op=str] [-o ofile]\n"
    "\t[-m max_depth] [-K] file.json [arg ...]\n"
    "\n"
    "\t-h\t\tPrint help and exit\n"
    "\t-V\t\tPrint version and exit\n"
    "\t-v level\tVerbosity level (def: %d)\n"
    "\t-J level\tJSON verbosity level (def: %d)\n"
    "\t-q\t\tQuiet mode (def: print stuff to stdout)\n"
    "\n"
    "\t-L <num>[{t|s}]\tPrint JSON level (root is 0) followed by a number of tabs or spaces (def: don't print levels)\n"
    "\t-L tab\t\tAlias for: '-L 1t'\n"
    "\n"
    "\t\t\tTrailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces.\n"
    "\t\t\tNot specifying 's' or 't' implies spaces.\n"
    "\n"
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
    "\t\t\t\tany\t\tany of the above types\n"
    "\n"
    "\t-l lvl\t\tPrint values at specific JSON levels (def: print any level)\n"
    "\n"
    "\t\t\tIf lvl is a number (e.g.: -l 3), level must == number.\n"
    "\t\t\tIf lvl is a number followed by : (e.g. '-l 3:'), level must be >= number.\n"
    "\t\t\tIf lvl is a : followed by a number (e.g. '-l :3'), level must be <= number.\n"
    "\t\t\tIf lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range.\n"

    "\t-Q\t\tPrint JSON strings surrounded by double quotes (def: do not)\n"
    "\t-D\t\tPrint JSON strings as decoded strings (def: print JSON strings as encoded strings)\n"
    "\t-d\t\tMatch the JSON decoded values (def: match as given in the JSON document)\n"
    "\t-i\t\tInvert match: print values that do not match (def: print values that do match)\n"
    "\t-s\t\tMatch substrings (def: match entire values)\n"
    "\t-f\t\tFold case (def: case matters when matching strings)\n"
    "\t-c\t\tPrint total match count only (def: print values)\n"
    "\n"
    "\t\t\tUsing -c with either -C or -L is an error.\n"
    "\n"
    "\t-C\t\tPrint match count followed by matched value (def: print values)\n"
    "\n"
    "\t\t\tUsing -C with either -c or -L is an error.\n"
    "\n"
    "\t-g\t\tMatch using grep-like extended regular expressions (def: match strings or substrings if -s)\n"
    "\n"
    "\t\t\tTo match from the beginning, start arg with '^'.\n"
    "\t\t\tTo match to the end, end arg with '$'.\n"
    "\t\t\tUsing -g with -s is an error.\n"
    "\n"
    "\t-e\t\tPrint JSON strings as encoded strings (def: decode JSON strings)\n"
    "\n"
    "\t-n op=num\tMatch if numeric op with num is true (def: do not)\n"
    "\n"
    "\t\t\top may be one of: eq, lt, le, gt, ge\n"
    "\n"
    "\t-S op=str\n"
    "\t\t\top may be one of: eq, lt, le, gt, ge\n"
    "\n"
    "\t-o ofile\tWrite to ofile (def: stdout)\n"
    "\t-m max_depth\tSet the maximum JSON level depth to max_depth (0 == infinite depth, def: %d)\n"
    "\n"
    "\t\t\tA 0 max_depth implies JSON_INFINITE_DEPTH: only safe with infinite variable size and RAM :-)\n"
    "\n"
    "\t-K\t\tRun tests on jval constraints\n"
    "\n"
    "\tfile.json\tJSON file to parse (- ==> read from stdin)\n"
    "\targ\t\tmatch arg(s)\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK and file is valid JSON\n"
    "    1\terror writing to ofile\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tinvalid command line, invalid option or option missing an argument\n"
    "    4\tfile does not exist, not a file, or unable to read the file\n"
    "    5\tfile contents is not valid JSON\n"
    "    6\ttest mode failed\n"
    " >=10\tinternal error\n"
    "\n"
    "JSON parser version: %s\n"
    "jval version: %s";

/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


int
main(int argc, char **argv)
{
    char const *program = NULL;		/* our name */
    extern char *optarg;
    extern int optind;
    struct jval *jval = NULL;	/* struct of all our options and other things */
    size_t len = 0;			/* length of file contents */
    bool is_valid = false;		/* if file is valid json */
    int exit_code = 0;			/* for the end */
    int i;

    jval = alloc_jval();		/* allocate our struct jval * */
    /*
     * the alloc_jval() will never return a NULL pointer but check just in
     * case
     */
    if (jval == NULL) {
	err(23, "jval", "failed to allocate jval struct");
	not_reached();
    }

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hVv:J:qL:t:l:QDdisfcCgen:S:o:m:K")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    free_jval(&jval);
	    usage(2, program, "");	/*ooo*/
	    not_reached();
	    break;
	case 'V':		/* -V - print version and exit */
	    free_jval(&jval);
	    print("%s\n", JVAL_VERSION);
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
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'L':
	    jval->print_json_levels = true; /* print JSON levels */
	    jval_parse_st_level_option(optarg, &jval->num_level_spaces, &jval->print_level_tab);
	    break;
	case 't':
	    jval->json_types_specified = true;
	    jval->json_types = jval_parse_types_option(optarg);
	    break;
	case 'l':
	    jval->levels_constrained = true;
	    jval_parse_number_range("-l", optarg, false, &jval->jval_levels);
	    break;
	case 'Q':
	    jval->quote_strings = true;
	    dbg(DBG_LOW, "-Q specified, will quote strings");
	    break;
	case 'D': /* -D - print decoded strings */
	    jval->print_decoded = true;
	    break;
	case 'd': /* -d - match decoded */
	    jval->match_decoded = true;
	    break;
	case 'i':
	    jval->invert_matches = true; /* show non-matches */
	    break;
	case 's':
	    jval->match_substrings = true;
	    dbg(DBG_LOW, "-s specified, will match substrings");
	    break;
	case 'f':
	    jval->ignore_case = true; /* make case cruel :-) */
	    dbg(DBG_LOW, "-i specified, making matches case-insensitive");
	    break;
	case 'c':
	    jval->count_only = true;
	    dbg(DBG_LOW, "-c specified, will only show count of matches");
	    break;
	case 'C':
	    jval->count_and_show_values = true;
	    break;
	case 'g':   /* allow grep-like ERE */
	    jval->use_regexps = true;
	    dbg(DBG_LOW, "-g specified, name_args will be regexps");
	    break;
	case 'e':
	    jval->encode_strings = true;
	    dbg(DBG_LOW, "-e specified, will encode strings");
	    break;
	case 'n': /* -n op=num */
	    jval->num_cmp_used = true;
	    jval_parse_cmp_op(jval, "n", optarg, &jval->num_cmp);
	    break;
	case 'S': /* -S op=str */
	    jval->string_cmp_used = true;
	    jval_parse_cmp_op(jval, "S", optarg, &jval->string_cmp);
	    break;
	case 'o': /* search with OR mode */
	    if (strcmp(optarg, "-")) { /* check if we will write to stdout */
		jval->outfile_not_stdout = true;
	    }
	    jval->outfile_path = optarg;
	    break;
	case 'm': /* set maximum depth to traverse json tree */
	    if (!string_to_uintmax(optarg, &jval->max_depth)) {
		free_jval(&jval);
		err(3, "jval", "couldn't parse -m depth"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'K': /* run test code */
	    if (!jval_run_tests()) {
		free_jval(&jval);
		exit(6); /*ooo*/
	    }
	    else {
		free_jval(&jval);
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
     * the wrong number of arguments (if they do). Not everything can be checked
     * prior to doing this though.
     */

    /* run specific sanity checks on options etc. */
    jval->json_file = jval_sanity_chks(jval, program, &argc, &argv);


    /*
     * jval_sanity_chks() should never return a NULL FILE * but we check
     * anyway
     */
    if (jval->json_file == NULL) {
	/*
	 * NOTE: don't make this exit code 3 as it's an internal error if the
	 * jval_sanity_chks() returns a NULL pointer.
	 */
	err(24, "jval", "could not open regular readable file");
	not_reached();
    }

    /*
     * Read in entire file BEFORE trying to parse it as json as the parser
     * function will close the file if not stdin.
     *
     * NOTE: why doesn't the jval_sanity_chks() function do this? Because this
     * is not so much about a sane environment as much as being unable to
     * continue after verifying the command line is correct.
     */
    jval->file_contents = read_all(jval->json_file, &len);
    if (jval->file_contents == NULL) {
	err(4, "jval", "could not read in file: %s", argv[0]); /*ooo*/
	not_reached();
    }
    /* clear EOF status and rewind for parse_json_stream() */
    clearerr(jval->json_file);
    rewind(jval->json_file);

    jval->json_tree = parse_json_stream(jval->json_file, argv[0], &is_valid);
    if (!is_valid || jval->json_tree == NULL) {
	if (jval->json_file != stdin) {
	    fclose(jval->json_file);  /* close file prior to exiting */
	    jval->json_file = NULL;   /* set to NULL even though we're exiting as a safety precaution */
	}

	/* free our jval struct */
	free_jval(&jval);
	err(5, "jval", "%s invalid JSON", argv[0]); /*ooo*/
	not_reached();
    }

    dbg(DBG_MED, "valid JSON");

    /* only if we get here can we try and open the output file */
    if (jval->outfile_not_stdout && jval->outfile_path != NULL && *jval->outfile_path != '\0') {
	/*
	 * we know the output file doesn't exist but the real question is can we
	 * open it for writing to make it JSON ?
	 */
	jval->outfile = fopen(jval->outfile_path, "w");
	if (jval->outfile == NULL) {
	    err(1, __func__, "couldn't open output file: %s", jval->outfile_path);/*ooo*/
	    not_reached();
	}
    }

    /* XXX - implement core of the tool, for now just print file to out file - XXX */
    fpr(jval->outfile?jval->outfile:stdout, "jval", "%s", jval->file_contents);

    /* free tree */
    json_tree_free(jval->json_tree, jval->max_depth);

    if (jval != NULL) {
	free_jval(&jval);	/* free jval struct */
    }

    exit(exit_code); /*ooo*/
}

/* jval_sanity_chks	- sanity checks on jval tool options
 *
 * given:
 *
 *	jval	    - pointer to our jval struct
 *	program	    - program name
 *	argc	    - pointer to argc from main()
 *	argv	    - pointer to argv from main()
 *
 * This function runs any important checks on the jval internal state. It also
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
 * NOTE: jval->check_tool_path and jval->check_tool_args can be NULL.
 *
 * NOTE: this function must be in jval.c, not jval_util.h, because it uses
 * the usage() function which needs to be in this file.
 */
FILE *
jval_sanity_chks(struct jval *jval, char const *program, int *argc, char ***argv)
{
    /* firewall */
    if (jval == NULL) {
	err(25, __func__, "NULL jval");
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
    if (jval->use_regexps && jval->match_substrings) {
	free_jval(&jval);
	err(3, __func__, "cannot use both -g and -s"); /*ooo*/
	not_reached();
    }

    /*
     * use of -c with -C or -L is an error and use of -C with -c or -L is an
     * error
     */
    if (jval->count_only || jval->print_json_levels || jval->count_and_show_values) {
	if (jval->count_and_show_values && jval->count_only) {
	    err(3, __func__, "cannot use -c and -C together"); /*ooo*/
	    not_reached();
	}
	if (jval->print_json_levels && jval->count_only) {
	    err(3, __func__, "cannot use -L and -c together"); /*ooo*/
	    not_reached();
	}
	if (jval->print_json_levels && jval->count_and_show_values) {
	    err(3, __func__, "cannot use -L and -C together"); /*ooo*/
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
	/*
	 * first check that if -o was used that the out file is not the same as
	 * the file.json file! Do this in a case-insensitive way for file
	 * systems that don't distinguish file.json from FILE.JSON.
	 *
	 * NOTE: do NOT open this file: we won't do that until we know that the
	 * input JSON is valid.
	 */
	if (jval->outfile_not_stdout && jval->outfile_path != NULL && *jval->outfile_path != '\0') {
	    if (!strcasecmp((*argv)[0], jval->outfile_path)) {
		free_jval(&jval);
		err(3, __func__, "-o ofile is the same as JSON file"); /*ooo*/
		not_reached();
	    }
	}

        /* check that the path exists and is a regular readable file */
	if (!exists((*argv)[0])) {
	    free_jval(&jval);
	    err(4, __func__, "%s: file does not exist", (*argv)[0]); /*ooo*/
	    not_reached();
	} else if (!is_file((*argv)[0])) {
	    free_jval(&jval);
	    err(4, __func__, "%s: not a regular file", (*argv)[0]); /*ooo*/
	    not_reached();
	} else if (!is_read((*argv)[0])) {
	    free_jval(&jval);
	    err(4, __func__, "%s: unreadable file", (*argv[0])); /*ooo*/
	    not_reached();
	}

	errno = 0; /* pre-clear errno for errp() */
	jval->json_file = fopen((*argv)[0], "r");
	if (jval->json_file == NULL) {
	    free_jval(&jval);
	    errp(4, __func__, "%s: could not open for reading", (*argv)[0]); /*ooo*/
	    not_reached();
	}
    } else { /* argv[0] is "-": will read from stdin */
	jval->is_stdin = true;
	jval->json_file = stdin;
    }

    dbg(DBG_LOW, "maximum depth to traverse: %ju%s", jval->max_depth, (jval->max_depth == 0?" (no limit)":
		jval->max_depth==JSON_DEFAULT_MAX_DEPTH?" (default)":""));

    /*
     * final processing: some options require the use of others but they are not
     * an error if they not used together; the one simply has no effect. Also
     * once options are parsed we have to check args and verify some things
     * after that.
     */

    /*
     * if -o file specified and not stdout (not '-') then we have to verify that
     * it does not already exist and that we can open it for writing
     *
     * NOTE: there is a slight risk that between the time we have checked if the
     * file exists and when we want to open it to write it later that it exists
     */
    if (jval->outfile_not_stdout && jval->outfile_path != NULL && *jval->outfile_path != '\0') {
	if (exists(jval->outfile_path)) {
	    err(3, __func__, "-o ofile: %s already exists: will not overwrite", jval->outfile_path); /*ooo*/
	    not_reached();
	}
    }
    /*
     * parse args first
     *
     * XXX - implement this function if necessary
     */
    parse_jval_args(jval, *argv);

    /* all good: return the (presumably) json FILE * */
    return jval->json_file;
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
    fprintf_usage(exitcode, stderr, usage_msg0, prog, DBG_DEFAULT, JSON_DBG_DEFAULT,
	    JSON_DEFAULT_MAX_DEPTH,json_parser_version, JVAL_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


