/*
 * dyn_array_test - test the dynamic array facility
 *
 * Copyright (c) 2014,2015,2022-2023 by Landon Curt Noll.  All Rights Reserved.
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
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * dyn_array_test - test the dynamic array facility
 */
#include "dyn_test.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define CHUNK (1024)		/* allocate CHUNK elements at a time */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\ttest suite failed\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    " >=10\tinternal error\n"
    "\n"
    "dyn_array library version: %s\n"
    "dyn_test version: %s";


/*
 * forward declarations
 */
#if !defined(DBG_USE)
static int parse_verbosity(char const *optarg);
#endif
static void usage(int exitcode, char const *str, char const *prog) __attribute__((noreturn));

int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    struct dyn_array *array;	/* dynamic array to test */
    double d;			/* test double */
    bool error = false;		/* true ==> test error found */
    intmax_t len = 0;		/* length of the dynamic array */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:V")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, ""); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    (void) printf("%s\n", DYN_TEST_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case ':':
	    (void) fprintf(stderr, "%s: requires an argument -- %c\n\n", program, optopt);
	    (void) usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	case '?':
	    (void) fprintf(stderr, "%s: illegal option -- %c\n\n", program, optopt);
	    (void) usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	default:
	    usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	}
    }
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /*
     * create dynamic array
     */
    array = dyn_array_create(sizeof(double), CHUNK, CHUNK, true);

    /*
     * load a million doubles
     */
    for (d = 0.0; d < 1000000.0; d += 1.0) {
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "moved data after appending d: %f", d);
	}
    }

    /*
     * verify values
     */
    for (i = 0; i < 1000000; ++i) {
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i));
	    error = true;
	}
    }

    /*
     * verify size
     */
    len = dyn_array_tell(array);
    if (len != 1000000) {
	warn(__func__, "dyn_array_tell(array): %jd != %jd", len, (intmax_t)1000000);
	error = true;
    }

    /*
     * concatenate the array onto itself
     */
    if (dyn_array_concat_array(array, array)) {
	dbg(DBG_LOW, "moved data after concatenation");
    }

    /*
     * verify new size
     */
    len = dyn_array_tell(array);
    if (len != 2000000) {
	warn(__func__, "dyn_array_tell(array): %jd != %jd", len, (intmax_t)2000000);
	error = true;
    }

    /*
     * verify values again
     */
    for (i = 0; i < 1000000; ++i) {
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i));
	    error = true;
	}
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i+1000000)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i+1000000));
	    error = true;
	}
    }

    /*
     * free dynamic array
     */
    if (array != NULL) {
	dyn_array_free(array);
	array = NULL;
    }

    /*
     * exit based on the test result
     */
    if (error == true) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


#if !defined(DBG_USE)
/*
 * parse_verbosity - parse -v optarg for our tools
 *
 * given:
 *	optarg		verbosity string, must be an integer >= 0
 *
 * returns:
 *	parsed verbosity or DBG_INVALID on conversion error
 */
int
parse_verbosity(char const *optarg)
{
    int verbosity = DBG_NONE;	/* parsed verbosity or DBG_NONE */

    /*
     * firewall
     */
    if (optarg == NULL) {
	return DBG_INVALID;
    }

    /*
     * parse verbosity
     */
    errno = 0;		/* pre-clear errno for warnp() */
    verbosity = (int)strtol(optarg, NULL, 0);
    if (errno != 0) {
	return DBG_INVALID;
    }
    if (verbosity < 0) {
	return DBG_INVALID;
    }
    return verbosity;
}
#endif /* !defined(DBG_USE) */


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "missing required argument(s), program: %s");
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
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, dyn_array_version, DYN_TEST_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
