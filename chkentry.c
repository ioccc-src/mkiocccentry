/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 *
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll
 * and the concept of this file was developed by Landon Curt Noll.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <unistd.h>

/*
 * chkentry - check JSON files in an IOCCC entry
 */
#include "chkentry.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    bool test = false;		/* true ==> JSON conversion test case result */
    bool error = false;		/* true ==> JSON conversion test suite error */
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    bool fnamchk_flag_used = false;	/* true ==> -F fnamchk used */
    struct json *node = NULL;	/* allocated JSON parser tree node */
    int arg_cnt = 0;		/* number of args to process */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:J:VqF:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(3, program, "-h help mode", -1, -1); /*ooo*/
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
	    print("%s", CHKENTRY_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	 case 'F':
            fnamchk_flag_used = true;
            fnamchk = optarg;
            break;
	default:
	    usage(4, program, "invalid -flag", -1, -1); /*ooo*/
	    not_reached();
	 }
    }
    arg_cnt = argc - optind;
    if (arg_cnt < REQUIRED_ARGS || arg_cnt > REQUIRED_ARGS+1) {
	/* XXX - the %d+1 arguments is a misfeature that should be fixed in usage() */
	usage(4, program, "expected %d or %d+1 arguments, found: %d", REQUIRED_ARGS, arg_cnt); /*ooo*/
    }

    /* XXX - fake code below needs to be removed - XXX */
    if (node == NULL) {
	error = true; /* XXX */
    }
    if (!test) {
	test = true; /* XXX */
    }
    if (error) {
	error = false; /* XXX */
    }
    if (fnamchk_flag_used && fnamchk == NULL) {
	error = true; /* XXX */
    }

    /* XXX - add more code here - XXX */

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(error != false);
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, "missing required argument(s), program: %s", program);
 *
 * given:
 *	exitcode        value to exit with
 *	prog		our program name
 *	str		top level usage message
 *	expected	>= 0 ==> expected args, < 0 ==> ignored
 *	argc		>= 0 ==> argument count, < 0 ==> ignored
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str, int expected, int argc)
{
    /*
     * firewall
     */
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    fprintf_usage(DO_NOT_EXIT, stderr, str, expected, expected+1, argc);
    fprintf_usage(exitcode, stderr, usage_msg, prog, prog, DBG_DEFAULT, JSON_DBG_DEFAULT, FNAMCHK_PATH_0, JNUM_CHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
