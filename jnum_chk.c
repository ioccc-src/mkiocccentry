/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jnum_chk - check JSON number string conversions
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
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>

/*
 * Our header file - #includes the header files we need
 */
#include "jnum_chk.h"

/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    bool test = false;		/* true ==> JSON conversion test case result */
    bool error = false;		/* true ==> JSON conversion test suite error */
    bool strict = false;	/* true ==> strict testing for all struct integer element, implies -t -q */
    struct json *node = NULL;	/* allocated JSON parser tree node */
    size_t len = 0;		/* length of str */
    int arg_cnt = 0;		/* number of args to process */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:VqS")) != -1) {
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
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    print("%s\n", JNUM_CHK_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'S':		/* -S - strict mode */
	    strict = true;
	    break;
	default:
	    usage(4, program, "invalid -flag", -1, -1); /*ooo*/
	    not_reached();
	 }
    }
    arg_cnt = argc - optind;
    if (arg_cnt != REQUIRED_ARGS) {
	usage(4, program, "expected %d arguments, found: %d", REQUIRED_ARGS, arg_cnt); /*ooo*/
    }
    dbg(DBG_MED, "strict mode: %s", (strict == true) ? "enabled" : "disabled");

    /*
     * process each test case
     */
    for (i=0; i < test_count; ++i) {

	/*
	 * convert the test string
	 */
	node = json_conv_number_str(test_set[i], &len);
	if (node == NULL) {
	    warn(__func__, "json_conv_number_str(%s, &%ju) returned NULL",
			   test_set[i], (uintmax_t)len);
	    error = true;
	    break;
	}

	/*
	 * compare conversion with test case
	 */
	test = chk_test(node, &test_result[i], len, strict);
	if (test == false) {
	    warn(__func__, "json_conv_number_str(%s, &%ju) failed",
			   test_set[i], (uintmax_t)len);
	    error = true;
	}

	/*
	 * free converted node
	 */
	json_conv_free(node);
	if (node != NULL) {
	    free(node);
	    node = NULL;
	}
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    if (error == true) {
	exit(5);
    }
    exit(0); /*ooo*/
}


/*
 * chk_test = check a given test
 *
 * When strict == true, we will compare every node->element.number element
 * with every *test element.  When strict == false, we will only compare
 * select struct json_number elements.
 *
 * given:
 *	node	- pointer to struct json converted JSON number
 *	test	- pointer to test suite struct json_number
 *	len	- converted JSON number length
 *	strict	- true ==> compare all struct json_member elements,
 *		  false ==> compare only select struct json_member elements
 *
 * returns:
 *	true ==> node->element.number matches *test
 *	false ==> node->element.number does NOT match *test, or
 *		  NULL pointer given, or len is incorrect
 */
static bool
chk_test(struct json *node, struct json_number *test, size_t len, bool strict)
{
    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return false;
    }
    if (test == NULL) {
	warn(__func__, "test is NULL");
	return false;
    }
    if (len != test->number_len) {
	warn(__func__, "len: %ju != test->number_len: %ju", len, test->number_len);
	return false;
    }

    /* XXX - add code here - XXX */

    return strict;	/* XXX - return the actual test result */
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
    if (argc >= 0 && expected >= 0) {
	fprintf_usage(DO_NOT_EXIT, stderr, str, expected, argc);
    } else {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JNUM_CHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
