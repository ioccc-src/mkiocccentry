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
    "usage:\t%s [-h] [-V] [-v level] [-J level] [-q] [-L <num>{[t|s]}] [-I <num>{[t|s]}]\n"
    "\t[-l lvl] [-m depth] [-K] [-o ofile] file.json\n"
    "\n"
    "\t-h\t\tPrint help and exit\n"
    "\t-V\t\tPrint version and exit\n"
    "\t-v level\tVerbosity level (def: %d)\n"
    "\t-J level\tJSON verbosity level (def: %d)\n"
    "\t-q\t\tSuppress some of the output (def: show more info)\n"
    "\n"
    "\t-L <num>[{t|s}]\tPrint JSON level followed by a number of tabs or spaces (def: don't print levels)\n"
    "\t-L tab\t\tAlias for: '-L 1t'\n"
    "\n"
    "\t\t\tTrailing 't' implies <num> tabs whereas trailing 's' implies <num> spaces.\n"
    "\t\t\tNot specifying 's' or 't' implies spaces.\n"
    "\n"
    "\t-I <num>{[t|s]}\tWhen printing JSON syntax, indent levels (def: indent with 4 spaces)\n"
    "\t-I tab\t\tAlias for '-I 1t'\n"
    "\n"
    "\t\t\tTrailing 't' implies indent with number of tabs whereas trailing 's' implies spaces.\n"
    "\t\t\tNot specifying 's' or 't' implies spaces.\n"
    "\n"
    "\t-l lvl\t\tPrint specific JSON levels (def: print at any level)\n"
    "\n"
    "\t\t\tIf lvl is a number (e.g.: -l 3), level must == number.\n"
    "\t\t\tIf lvl is a number followed by : (e.g. '-l 3:'), level must be >= number.\n"
    "\t\t\tIf lvl is a : followed by a number (e.g. '-l :3'), level must be <= number.\n"
    "\t\t\tIf lvl is num:num (e.g. '-l 3:5'), level must be inclusively in the range.\n"
    "\n"
    "\t-m max_depth\tSet the maximum JSON level depth to max_depth (0 == infinite depth, def: %d)\n"
    "\n"
    "\t\t\tA 0 max_depth implies JSON_INFINITE_DEPTH: only safe with infinite variable size and RAM :-)\n"
    "\n"
    "\t-K\t\tRun test mode\n"
    "\n"
    "\t-o ofile\tOutput formatted JSON to ofile (def: stdout, same as '-')\n"
    "\n"
    "\tfile.json\tJSON file to parse (- ==> read from stdin)\n"
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
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg0, prog, DBG_DEFAULT, JSON_DBG_DEFAULT,
	    JSON_DEFAULT_MAX_DEPTH, JSON_PARSER_VERSION, JFMT_VERSION);
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
    while ((i = getopt(argc, argv, ":hVv:J:qL:I:l:m:Ko:")) != -1) {
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
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'L':
	    jfmt->common.print_json_levels = true; /* print JSON levels */
	    json_util_parse_st_level_option(optarg, &jfmt->common.num_level_spaces, &jfmt->common.print_level_tab);
	    break;
	case 'I':
	    jfmt->common.indent_levels = true;
	    json_util_parse_st_indent_option(optarg, &jfmt->common.indent_spaces, &jfmt->common.indent_tab);
	    break;
	case 'l':
	    jfmt->common.levels_constrained = true;
	    json_util_parse_number_range("-l", optarg, false, &jfmt->common.json_util_levels);
	    break;
	case 'm': /* set maximum depth to traverse json tree */
	    if (!string_to_uintmax(optarg, &jfmt->common.max_depth)) {
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
	case 'o': /* search with OR mode */
	    if (strcmp(optarg, "-")) { /* check if we will write to stdout */
		jfmt->common.outfile_not_stdout = true;
	    }
	    jfmt->common.outfile_path = optarg;
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
    jfmt->common.json_file = jfmt_sanity_chks(jfmt, program, &argc, &argv);


    /*
     * jfmt_sanity_chks() should never return a NULL FILE * but we check
     * anyway
     */
    if (jfmt->common.json_file == NULL) {
	/*
	 * NOTE: don't make this exit code 3 as it's an internal error if the
	 * jfmt_sanity_chks() returns a NULL pointer.
	 */
	err(24, "jfmt", "could not open regular readable file");
	not_reached();
    }

    /*
     * Read in contents of file.
     *
     * NOTE: why doesn't the jfmt_sanity_chks() function do this? Because this
     * is not so much about a sane environment as much as being unable to
     * continue after verify the command line is correct.
     */
    jfmt->common.file_contents = read_all(jfmt->common.json_file, &len);
    if (jfmt->common.file_contents == NULL) {
	err(4, "jfmt", "could not read in file: %s", argv[0]); /*ooo*/
	not_reached();
    }

    jfmt->common.json_tree = parse_json(jfmt->common.file_contents, strlen(jfmt->common.file_contents),
	    jfmt->common.json_file_path, &is_valid);
    if (!is_valid || jfmt->common.json_tree == NULL) {
	if (jfmt->common.json_file != stdin) {
	    fclose(jfmt->common.json_file);  /* close file prior to exiting */
	    jfmt->common.json_file = NULL;   /* set to NULL even though we're exiting as a safety precaution */
	}

	/* free our jfmt struct */
	free_jfmt(&jfmt);
	err(5, "jfmt", "%s invalid JSON", argv[0]); /*ooo*/
	not_reached();
    }

    dbg(DBG_MED, "valid JSON");

    /* only if we get here can we try and open the output file */
    if (jfmt->common.outfile_not_stdout && jfmt->common.outfile_path != NULL && *jfmt->common.outfile_path != '\0') {
	/*
	 * we know the output file doesn't exist but the real question is can we
	 * open it for writing to make it JSON ?
	 */
	jfmt->common.outfile = fopen(jfmt->common.outfile_path, "w");
	if (jfmt->common.outfile == NULL) {
	    err(1, __func__, "couldn't open output file: %s", jfmt->common.outfile_path);/*ooo*/
	    not_reached();
	}
    }

    /* XXX - change this to format the file - XXX */
    fprintf(jfmt->common.outfile?jfmt->common.outfile:stdout, "%s", jfmt->common.file_contents);

    /* free tree */
    json_tree_free(jfmt->common.json_tree, jfmt->common.max_depth);

    /* All Done!!! -- Jessica Noll, Age 2 */
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
     * shift argc and argv for processing. They're a pointer to those in
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
    /* check that file path is not an empty string */
    if (*(*argv)[0] == '\0') {
	usage(3, program, "empty file path");/*ooo*/
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
	if (jfmt->common.outfile_not_stdout && jfmt->common.outfile_path != NULL && *jfmt->common.outfile_path != '\0') {
	    if (!strcasecmp((*argv)[0], jfmt->common.outfile_path)) {
		free_jfmt(&jfmt);
		err(3, __func__, "-o ofile is the same as JSON file"); /*ooo*/
		not_reached();
	    }
	}
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
	jfmt->common.json_file_path = (*argv)[0];
	jfmt->common.json_file = fopen(jfmt->common.json_file_path, "r");
	if (jfmt->common.json_file == NULL) {
	    free_jfmt(&jfmt);
	    errp(4, __func__, "%s: could not open for reading", (*argv)[0]); /*ooo*/
	    not_reached();
	}
    } else { /* argv[0] is "-": will read from stdin */
	jfmt->common.json_file_path = "-";
	jfmt->common.is_stdin = true;
	jfmt->common.json_file = stdin;
    }

    /*
     * if -o file specified and not stdout (not '-') then we have to verify that
     * it does not already exist and that we can open it for writing
     *
     * NOTE: there is a slight risk that between the time we have checked if the
     * file exists and when we want to open it to write it later that it exists
     */
    if (jfmt->common.outfile_not_stdout && jfmt->common.outfile_path != NULL && *jfmt->common.outfile_path != '\0') {
	if (exists(jfmt->common.outfile_path)) {
	    err(3, __func__, "-o ofile already exists: will not overwrite"); /*ooo*/
	    not_reached();
	}
    }

    dbg(DBG_LOW, "maximum depth to traverse: %ju%s", jfmt->common.max_depth, (jfmt->common.max_depth == 0?" (no limit)":
		jfmt->common.max_depth==JSON_DEFAULT_MAX_DEPTH?" (default)":""));


    /* XXX - should it be an error if any more args are specified ? - XXX */

    /* all good: return the (presumably) json FILE * */
    return jfmt->common.json_file;
}
