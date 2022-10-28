/*
 * jstrdecode - tool to JSON decode JSON encoded strings
 *
 * "JSON: when a minimal design falls below a critical minimum." :-)
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
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * jstrdecode - tool to JSON decode JSON encoded strings
 */
#include "jstrdecode.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */


/*
 * jstrdecode_stream - decode an open file stream onto another open file stream
 *
 * given:
 *	in_stream	open file steam to decode
 *	out_stream	open file where to write encoded data
 *	write_quote	true ==> output enclosing quotes
 *
 * returns:
 *	true ==> encoding was successful,
 *	false ==> error in encoding, or NULL stream, or read error
 */
static bool
jstrdecode_stream(FILE *in_stream, FILE *out_stream, bool write_quote)
{
    char *input;		/* argument to process */
    size_t inputlen;		/* length of input buffer */
    size_t outputlen;		/* length of write of decode buffer */
    size_t bufsiz;		/* length of the buffer */
    char *buf;			/* decode buffer */
    bool success = true;	/* true ==> encoding OK, false ==> error while encoding */
    int ret = 0;		/* libc function return code */

    /*
     * firewall
     */
    if (in_stream == NULL) {
	warn(__func__, "in_stream is NULL");
	return false;
    }
    if (out_stream == NULL) {
	warn(__func__, "out_stream is NULL");
	return false;
    }

    /*
     * read all of the input stream
     */
    dbg(DBG_LOW, "about to encode all data on input stream");
    input = read_all(in_stream, &inputlen);
    if (input == NULL) {
	warn(__func__, "error while reading data from input stream");
	return false;
    }
    dbg(DBG_MED, "stream read length: %ju", (uintmax_t)inputlen);

    /*
     * decode data read from input stream
     */
    buf = json_decode(input, inputlen, &bufsiz);
    if (buf == NULL) {
	warn(__func__, "error while encoding stdin buffer");
	success = false;

    /*
     * print decode buffer
     */
    } else {

	/*
	 * write starting quote if requested
	 */
	if (write_quote == true) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fputc('"', out_stream);
	    if (ret != '"') {
		warnp(__func__, "fputc of starting quote returned error");
		success = false;
	    }
	}

	/*
	 * write decoded data
	 */
	dbg(DBG_MED, "decode length: %ju", (uintmax_t)bufsiz);
	errno = 0;		/* pre-clear errno for warnp() */
	outputlen = fwrite(buf, 1, bufsiz, out_stream);
	if (outputlen != bufsiz) {
	    warnp(__func__, "error: write of %ju bytes of data: returned: %ju",
			    (uintmax_t)bufsiz, (uintmax_t)outputlen);
	    success = false;
	}
	dbg(DBG_MED, "fwrite length: %ju", (uintmax_t)outputlen);

	/*
	 * write ending quote if requested
	 */
	if (write_quote == true) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fputc('"', out_stream);
	    if (ret != '"') {
		warnp(__func__, "fputc of ending quote returned error");
		success = false;
	    }
	}
    }

    /*
     * free buffer
     */
    if (buf == NULL) {
	free(buf);
	buf = NULL;
    }

    /*
     * return encoding status
     */
    return success;
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
    bool success = true;	/* true ==> encoding OK, false ==> error while encoding */
    bool nloutput = true;	/* true ==> output newline after JSON decode */
    bool write_quote = false;	/* true ==> output enclosing quotes */
    int ret;			/* libc return code */
    int i;


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVtnQ")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 2 */
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
	case 'V':		/* -V - print version and exit 2 */
	    print("%s\n", JSTRDECODE_VERSION);
	    exit(2); /*ooo*/
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
	case 'Q':
	    write_quote = true;
	    break;
	default:
	    usage(3, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    dbg(DBG_LOW, "enclose in quotes: %s", booltostr(write_quote));
    dbg(DBG_LOW, "newline output: %s", booltostr(nloutput));
    dbg(DBG_LOW, "silence warnings: %s", booltostr(msg_warn_silent));

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
		success = jstrdecode_stream(stdin, stdout, write_quote);
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
		    success = false;

		/*
		 * write starting quote if requested
		 */
		if (write_quote == true) {
		    errno = 0;		/* pre-clear errno for warnp() */
		    ret = fputc('"', stdout);
		    if (ret != '"') {
			warnp(__func__, "fputc for starting quote returned error");
			success = false;
		    }
		}

		/*
		 * print decoded buffer
		 */
		} else {
		    dbg(DBG_MED, "decode length: %ju", (uintmax_t)bufsiz);
		    errno = 0;		/* pre-clear errno for warnp() */
		    outputlen = fwrite(buf, 1, bufsiz, stdout);
		    if (outputlen != bufsiz) {
			warnp(__func__, "error: write of %ju bytes of arg: %d returned: %ju",
					(uintmax_t)bufsiz, i-optind, (uintmax_t)outputlen);
			success = false;
		    }
		}

		/*
		 * write ending quote if requested
		 */
		if (write_quote == true) {
		    errno = 0;		/* pre-clear errno for warnp() */
		    ret = fputc('"', stdout);
		    if (ret != '"') {
			warnp(__func__, "fputc for ending quote returned error");
			success = false;
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
	success = jstrdecode_stream(stdin, stdout, write_quote);
    }

    /*
     * unless -n, output newline
     */
    if (nloutput == true) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = putchar('\n');
	if (ret != '\n') {
	    warnp(__func__, "error while writing final newline");
	    success = false;
	}
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    if (success == false) {
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
