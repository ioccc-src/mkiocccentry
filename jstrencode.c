/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jstrencode - JSON encode strings
 *
 * "JSON: when a minimal design falls below a critical minimum." :-)
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
#include <string.h>

/*
 * Our header file - #includes the header files we need
 */
#include "jstrencode.h"

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
    char *input;		/* argument to process */
    size_t inputlen;		/* length of input buffer */
    char *buf;			/* encode buffer */
    size_t bufsiz;		/* length of the buffer */
    size_t outputlen;		/* length of write of encode buffer */
    bool error = false;		/* true ==> error while performing JSON encode */
    bool nloutput = true;	/* true ==> output newline after JSON encode */
    int ret;			/* libc return code */
    int i;


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVtn")) != -1) {
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
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", JSTRENCODE_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JSTRENCODE_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 't':		/* -t - validate the contents of the jenc[] table */
	    printf("%s: Beginning jencchk test on code JSON encode/decode functions ...\n", program);
	    jencchk();
	    printf("%s: ... passed JSON encode/decode test\n", program);
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'n':
	    nloutput = false;
	    break;
	default:
	    usage(2, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }

    /*
     * case: process arguments on command line
     */
    if (argc - optind > 0) {

	/*
	 * process each argument in order
	 */
	for (i=optind; i < argc; ++i) {

	    /*
	     * obtain argument string
	     */
	    input = argv[i];
	    inputlen = strlen(input);
	    dbg(DBG_LOW, "processing arg: %d: <%s>", i-optind, input);
	    dbg(DBG_MED, "arg length: %ju", (uintmax_t)inputlen);

	    /*
	     * encode
	     */
	    buf = malloc_json_encode_str(input, &bufsiz);
	    if (buf == NULL) {
		warn(__func__, "error while encoding processing arg: %d", i-optind);
		error = true;

	    /*
	     * print encode buffer
	     */
	    } else {
		dbg(DBG_MED, "encode length: %ju", (uintmax_t)bufsiz);
		errno = 0;		/* pre-clear errno for warnp() */
		outputlen = fwrite(buf, 1, bufsiz, stdout);
		if (outputlen != bufsiz) {
		    warnp(__func__, "error: write of %ju bytes of arg: %d returned: %ju",
				    (uintmax_t)bufsiz, i-optind, (uintmax_t)outputlen);
		    error = true;
		}
	    }

	    /*
	     * free buffer
	     */
	    if (buf == NULL) {
		free(buf);
		buf = NULL;
	    }
	}

    /*
     * case: process data on stdin
     */
    } else {

	/*
	 * read all of stdin
	 */
	dbg(DBG_LOW, "about to encode all data on stdin");
	input = read_all(stdin, &inputlen);
	if (input == NULL) {
	    warn(__func__, "error while reading data in stdin");
	    error = true;
	}
	dbg(DBG_MED, "stdin read length: %ju", (uintmax_t)inputlen);

	/*
	 * encode data read from stdin
	 */
	buf = malloc_json_encode(input, inputlen, &bufsiz);
	if (buf == NULL) {
	    warn(__func__, "error while encoding stdin buffer");
	    error = true;

	/*
	 * print encode buffer
	 */
	} else {
	    dbg(DBG_MED, "encode length: %ju", (uintmax_t)bufsiz);
	    errno = 0;		/* pre-clear errno for warnp() */
	    outputlen = fwrite(buf, 1, bufsiz, stdout);
	    if (outputlen != bufsiz) {
		warnp(__func__, "error: write of %ju bytes of stdin data: returned: %ju",
			        (uintmax_t)bufsiz, (uintmax_t)outputlen);
		error = true;
	    }
	    dbg(DBG_MED, "stdout write length: %ju", (uintmax_t)outputlen);
	}

	/*
	 * free buffer
	 */
	if (buf == NULL) {
	    free(buf);
	    buf = NULL;
	}
    }

    /*
     * unless -n, output newline
     */
    if (nloutput == true) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = putchar('\n');
	if (ret != '\n') {
	    warnp(__func__, "error while writing final newline");
	    error = true;
	}
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    if (error == true) {
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
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JSTRENCODE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
