/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because the JSON co-founders minimalism is sub-minimal." :-)
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
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
 * official versions
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
    bool strict = false;	/* true ==> strict testing for all struct integer element, implies -t -q */
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
	    errno = 0;		/* pre-clear errno for warnp() */
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
	usage(4, program, "expected %d or %d+1 arguments, found: %d", REQUIRED_ARGS, arg_cnt); /*ooo*/
    }
    dbg(DBG_MED, "strict mode: %s", (strict == true) ? "enabled" : "disabled");

    /* XXX - fake code below needs to be removed - XXX */
    if (node == NULL) {
	error = true; /* XXX */
    }
    if (test == false) {
	test = true; /* XXX */
    }
    if (error == true) {
	error = false; /* XXX */
    }
    if (strict == true) {
	strict = false; /* XXX */
    }
    if (fnamchk_flag_used == true && fnamchk == NULL) {
	error = true; /* XXX */
    }

    /* XXX - add more code here - XXX */

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    if (error == true) {
	if (strict == true) {
	    exit(2); /*ooo*/
	} else {
	    exit(1); /*ooo*/
	}
    }
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
