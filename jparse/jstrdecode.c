/*
 * jstrdecode - tool to JSON decode JSON encoded strings
 *
 * "JSON: when a minimal design falls below a critical minimum." :-)
 *
 * This JSON parser was co-developed in 2022 by:
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
 * official jstrdecode version
 */
#define JSTRDECODE_VERSION "1.0 2023-02-04"	/* format: major.minor YYYY-MM-DD */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-t] [-n] [-Q] [string ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-t\t\tperform jencchk test on code JSON decode/encode functions\n"
    "\t-n\t\tdo not output newline after decode output\n"
    "\t-Q\t\tenclose output in quotes (def: do not)\n"
    "\n"
    "\t[string ...]\tdecode strings on command line (def: read stdin)\n"
    "\t\t\tNOTE: - means read from stdin\n"
    "\n"
    "Exit codes:\n"
    "     0   decode successful\n"
    "     1   decode unsuccessful\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   invalid command line, invalid option or option missing an argument\n"
    " >= 10   internal error\n"
    "\n"
    "jstrdecode version: %s";


/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


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
    char *input = NULL;		/* argument to process */
    size_t inputlen;		/* length of input buffer */
    size_t outputlen;		/* length of write of decode buffer */
    size_t bufsiz;		/* length of the buffer */
    char *buf = NULL;		/* decode buffer */
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
    buf = json_decode(input, inputlen, &bufsiz, NULL);
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
    if (buf != NULL) {
	free(buf);
	buf = NULL;
    }

    /*
     * free input
     */
    if (input != NULL) {
	free(input);
	input = NULL;
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
    while ((i = getopt(argc, argv, ":hv:qVtnQ")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 2 */
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
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit 2 */
	    print("%s\n", JSTRDECODE_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 't':		/* -t - validate the contents of the jenc[] table */
	    print("%s: Beginning jencchk test on code JSON decode/decode functions ...\n", program);
	    jencchk();
	    print("%s: ... passed JSON decode/decode test\n", program);
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'n':
	    nloutput = false;
	    break;
	case 'Q':
	    write_quote = true;
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
 *      usage(3, program, "wrong number of arguments");
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

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JSTRDECODE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
