/*
 * jparse_main - main function for the JSON parser demo tool
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */

/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <unistd.h>

/*
 * jparse_main - tool that parses a block of JSON input
 */
#include "jparse_main.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


int
main(int argc, char **argv)
{
    char const *program = NULL;	    /* our name */
    extern char *optarg;	    /* option argument */
    extern int optind;		    /* argv index of the next arg */
    bool string_flag_used = false;  /* true ==> -S string was used */
    bool valid_json = false;	    /* true ==> JSON parse was valid */
    struct json *tree = NULL;	    /* JSON parse tree or NULL */
    int arg_count = 0;		    /* number of args to process */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVsJ:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'J': /* -J json_verbosity_level */
	    /*
	     * parse json verbosity level
	     */
	    json_verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", JPARSE_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 's':
	    string_flag_used = true;
	    break;
	default:
	    usage(3, "invalid -flag or missing option argument", program); /*ooo*/
	    not_reached();
	}
    }
    arg_count = argc - optind;
    if (arg_count != REQUIRED_ARGS) {
	usage(3, "wrong number of arguments", program); /*ooo*/
	not_reached();
    }

    /*
     * case: process -s arg
     */
    if (string_flag_used == true) {

	/* parse arg as a block of json input */
	dbg(DBG_HIGH, "Calling parse_json(\"%s\", %ju, &valid_json):",
		      argv[argc-1], (uintmax_t)strlen(argv[argc-1]));
	tree = parse_json(argv[argc-1], strlen(argv[argc-1]), &valid_json);

    /*
     * case: process file arg
     */
    } else {

	/* parse arg as a json filename */
	dbg(DBG_HIGH, "Calling parse_json_file(\"%s\", &valid_json):", argv[argc-1]);
	filename = argv[argc-1];
	tree = parse_json_file(filename, &valid_json);
    }

    if (tree == NULL) {
	warn(program, "JSON parse tree is NULL");
    }
    /*
     * free the JSON parse tree
     */
    else {
	json_tree_free(tree, JSON_INFINITE_DEPTH);
	free(tree);
	tree = NULL;
    }

    /*
     * firewall - JSON parser must have returned a valid JSON parse tree
     */
    if (valid_json == false) {
	++num_errors;
	err(1, program, "invalid JSON"); /*ooo*/
	not_reached();
    }

    if (num_errors > 0) {
	msg("invalid JSON");
	exit(1); /*ooo*/
    }
    msg("valid JSON");
    exit(0); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, "missing required argument(s), program: %s", program);
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message
 *	program		our program name
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 *
 */
static void
usage(int exitcode, char const *str, char const *prog)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog,
		  DBG_DEFAULT, json_verbosity_level, JSON_PARSER_VERSION, JPARSE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
