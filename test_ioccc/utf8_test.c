/*
 * utf8_test - test translate UTF-8 into POSIX portable filename and + chars
 *
 * "Because even POSIX needs an extra plus." :-)
 *
 * An author_handle, if the submission is selected by the
 * IOCCC judges to win the IOCCC, will be used to form a
 * JSON filename under the author directory on the IOCCC
 * website.  For this and other reasons, we must restrict an
 * author_handle to use only POSIX portable filenames, and we
 * must restrict the leading character to be ASCII alphabetic,
 * in addition to other character restrictions.
 *
 * The author_handle must fit the following regular expression:
 *
 *	^[0-9A-Za-z][0-9A-Za-z._+-]*$*
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
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>


/*
 * utf8_posix_map - translate UTF-8 into POSIX portable filename and + chars
 */
#include "../soup/utf8_posix_map.h"

/*
 * jparse - the JSON parser
 */
#include "../jparse/jparse.h"

/*
 * jparse/version - the JSON parser version
 */
#include "../jparse/version.h"


/*
 * definitions
 */
#define UTF8_TEST_VERSION "1.3 2023-02-04"
#define UTF8_TEST_BASENAME "utf8_test"


/*
 * globals
 */

/*
 * usage message
 *
 * The follow usage message came from an early draft of mkiocccentry.
 * This is just an example of usage: there is no mkiocccentry functionality here.
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static char const * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-q] name ...\n"
    "\n"
    "\t-h\t\tPrint help message and exit\n"
    "\t-v level\tSet verbosity level: (def level: 0)\n"
    "\t-V\t\tPrint version string and exit\n"
    "\t-q\t\tQuiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\n"
    "\tname\t\tUTF-8 name to translate into POSIX portable filename and + chars\n"
    "\n"
    "Exit codes:\n"
    "     0   all is OK\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   invalid command line, invalid option or option missing an argument\n"
    " >= 10   internal error\n"
    "\n"
    "%s version: %s\n"
    "JSON parser version: %s";

/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


int
main(int argc, char *argv[])
{
    char *program = NULL;		/* our name */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char *name = NULL;			/* translated name argument */
    int ret;				/* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:Vq")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    usage(2, program, "");
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':	/* -V - print version and exit */
            print("%s version: %s\n", UTF8_TEST_BASENAME, UTF8_TEST_VERSION);
	    print("JSON parser version: %s\n", JSON_PARSER_VERSION);
            exit(2); /*ooo*/
            not_reached();
            break;
	case 'q':
	    msg_warn_silent = true;
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
    /* must have 1 or more */
    if (argc-optind <= 0) {
	usage(3, program, "wrong number of arguments");	/*ooo*/
	not_reached();
    }

    /*
     * translate args, 1 at a time
     */
    for (i=optind; i < argc; ++i) {

	/*
	 * translate arg (as name) into default handle
	 */
	dbg(1, "UTF-8 name to translate: <%s>", argv[i]);
	name = default_handle(argv[i]);
	if (name == NULL) {
	    err(10, __func__, "default_handle(%s) returned NULL", argv[i]); /*ooo*/
	    not_reached();
	}
	dbg(3, "translation into POSIX portable filename and + chars: <%s>", name);

	/*
	 * print translated name
	 */
	errno = 0;			/* pre-clear errno for errp() */
	ret = puts(name);
	if (ret == EOF) {
	    errp(3, __func__, "puts error");
	    not_reached();
	}

	/*
	 * free translation
	 */
	if (name != NULL) {
	    free(name);
	    name = NULL;
	}
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(0); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(2, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	prog		our program name
 *	str		top level usage message
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
	prog = UTF8_TEST_BASENAME;
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

    fprintf_usage(exitcode, stderr, usage_msg, prog, UTF8_TEST_BASENAME, UTF8_TEST_VERSION, JSON_PARSER_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
