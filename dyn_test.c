/*
 * dyn_array_test - test the dynamic array facility
 *
 * Copyright (c) 2014,2015,2022 by Landon Curt Noll.  All Rights Reserved.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * Our header file - #includes the header files we need
 */
#include "dyn_test.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define CHUNK (1024)		/* allocate CHUNK elements at a time */


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    struct dyn_array *array;	/* dynatic array to test */
    double d;			/* test double */
    bool err = false;		/* true ==> test error found */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:V")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(3, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
#if defined(STAND_ALONE)
	    (void) printf("%s\n", DYN_TEST_VERSION);
#else /* STAND_ALONE */
	    print("%s\n", DYN_TEST_VERSION);
#endif /* STAND_ALONE */
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	default:
	    usage(4, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    if (argc - optind != REQUIRED_ARGS) {
	usage(4, "expected no arguments", program); /*ooo*/
	not_reached();
    }

    /*
     * create dynamic array
     */
    array = dyn_array_create(sizeof(double), CHUNK, CHUNK, "double test");

    /*
     * load a million doubles
     */
    for (d = 0.0; d < 1000000.0; d += 1.0) {
	dyn_array_append_value(array, &d);
    }

    /*
     * verify values
     */
    for (i = 0; i < 1000000; ++i) {
	if ((double) i != dyn_array_value(array, double, i)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i));
	    err = true;
	}
    }

    /*
     * exit based on the test result
     */
    if (err == true) {
	exit(1); /*ooo*/
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
 *	str		top level usage message
 *	program		our program name
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
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
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, DYN_TEST_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


#if defined(STAND_ALONE)
/*
 * parse_verbosity	- parse -v option for our tools
 *
 * given:
 *
 *	program		- the calling program e.g. txzchk, fnamchk, mkiocccentry etc.
 *	arg		- the optarg in the calling tool
 *
 * Returns the parsed verbosity.
 *
 * Returns DBG_NONE if passed NULL args or empty string.
 */
static int
parse_verbosity(char const *program, char const *arg)
{
    int verbosity;

    if (program == NULL || arg == NULL || !strlen(arg)) {
	return DBG_NONE;
    }

    /*
     * parse verbosity
     */
    errno = 0;		/* pre-clear errno for errp() */
    verbosity = (int)strtol(arg, NULL, 0);
    if (errno != 0) {
	errp(1, __func__, "%s: cannot parse -v arg: %s error: %s", program, arg, strerror(errno)); /*ooo*/
	not_reached();
    }

    return verbosity;
}
#endif /* STAND_ALONE */
