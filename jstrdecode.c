/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jstrdecode - JSON decode JSON encoded strings
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
#include "jstrdecode.h"

/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */


/*
 * jstrdecode_stdin	- decodes stdin
 *
 *
 * XXX This function should probably be jstrdecode_file() and decode any file.
 */
static
bool jstrdecode_stdin(void)
{
    bool error = false;
    char *input;		/* argument to process */
    size_t inputlen;		/* length of input buffer */
    size_t outputlen;		/* length of write of decode buffer */
    size_t bufsiz;		/* length of the buffer */
    char *buf;			/* decode buffer */

    /*
     * read all of stdin
     */
    dbg(DBG_LOW, "about to decode all data on stdin");
    input = read_all(stdin, &inputlen);
    if (input == NULL) {
	warn(__func__, "error while reading data in stdin");
	error = true;
    }
    dbg(DBG_MED, "stdin read length: %ju", (uintmax_t)inputlen);

    /*
     * warn if encode data contains an embedded NUL byte
     *
     * NOTE: The read_all will ensure that at least one extra byte
     *	 will have been allocated and set to NUL.  Thus in order
     *	 to correctly check if data contains an embedded NUL byte,
     *	 we MUST check for a length of inputlen+1!
     */
    if (is_string(input, inputlen+1) == false) {
	warn(__func__, "encoded data that was read contains a NUL byte");
	error = true;
    }

    /*
     * decode data read from stdin
     */
    buf = json_decode(input, inputlen, &bufsiz);
    if (buf == NULL) {
	warn(__func__, "error while encoding stdin buffer");
	error = true;

    /*
     * print decode buffer
     */
    } else {
	dbg(DBG_MED, "decode length: %ju", (uintmax_t)bufsiz);
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

    return error;
}

int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    char *input;		/* argument to process */
    size_t inputlen;		/* length of input buffer */
    char *buf;			/* decode buffer */
    size_t bufsiz;		/* length of the buffer */
    size_t outputlen;		/* length of write of decode buffer */
    bool error = false;		/* true ==> error while performing JSON decode */
    bool nloutput = true;	/* true ==> output newline after JSON decode */
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
	    ret = printf("%s\n", JSTRDECODE_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JSTRDECODE_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 't':		/* -t - validate the contents of the jenc[] table */
	    printf("%s: Beginning jencchk test on code JSON decode/decode functions ...\n", program);
	    jencchk();
	    printf("%s: ... passed JSON decode/decode test\n", program);
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
	    if (!strcmp(input, "-")) {
		/* decode stdin */
		error = jstrdecode_stdin();
	    } else {
		inputlen = strlen(input);
		dbg(DBG_LOW, "processing arg: %d: <%s>", i-optind, input);
		dbg(DBG_MED, "arg length: %ju", (uintmax_t)inputlen);

		/*
		 * decode
		 */
		buf = json_decode_str(input, &bufsiz);
		if (buf == NULL) {
		    warn(__func__, "error while encoding processing arg: %d", i-optind);
		    error = true;

		/*
		 * print decode buffer
		 */
		} else {
		    dbg(DBG_MED, "decode length: %ju", (uintmax_t)bufsiz);
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
	}

    /*
     * case: process data on stdin
     */
    } else {
	error = jstrdecode_stdin();
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
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JSTRDECODE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
