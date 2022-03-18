/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jparse - tool that parses a block of JSON input
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * NOTE: This is _very incomplete_ and right now all it does is reads from either
 * stdin, the command line or a file. It does not parse anything yet as the JSON
 * parser is not made yet. All this does now is check that it's a C string and
 * print it out if it is. It's an error if it's not a C string.
 *
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
#include "jparse.h"

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
    size_t length;		/* length of input string or file */
    FILE *input_stream = NULL;	/* file to read */
    size_t outputlen;		/* length of write of file or input string */
    bool output_newline = true;	/* true ==> output newline after writing input to stdout */
    bool strict = false;	/* true ==> strict mode (currently unused: this is for when a JSON parser is added) */
    bool is_stdin = false;	/* true ==> argv[i] is '-' */
    int ret;			/* libc return code */
    int i;


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVnsT")) != -1) {
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
	    quiet = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", JPARSE_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JPARSE_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'T':		/* -T (IOCCC toolkit release repository tag) */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", IOCCC_TOOLKIT_RELEASE);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing IOCCC toolkit release repository tag");
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'n':
	    output_newline = false;
	    break;
	case 's':
	    strict = true;
	    break;
	default:
	    usage(2, "invalid -flag", program); /*ooo*/
	    not_reached();
	}
    }
    /* warn(), warnp() and msg() are quiet if -q and -v 0 */
    if (quiet && verbosity_level <= 0) {
	msg_output_allowed = false;
	warn_output_allowed = false;
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
	     * Obtain argument string.
	     *
	     * NOTE: If it's the name of a file we read from the file instead.
	     * If it's "-" we set the file to stdin.
	     */
	    dbg(DBG_LOW, "processing arg: %d: <%s>", i-optind, argv[i]);
	    is_stdin = !strcmp(argv[i], "-");

	    errno = 0;
	    input_stream = is_stdin ? stdin : fopen(argv[i], "r");
	    if (input_stream != NULL) {
		if (input_stream == stdin) {
		    dbg(DBG_LOW, "about to read all data from stdin");
		} else {
		    dbg(DBG_LOW, "about to read all data from file %s", argv[i]);
		}
		/*
		 * Read in the file (stdin or otherwise) entirely before
		 * anything else.
		 */
		input = read_all(input_stream, &length);

		if (input_stream == stdin) {
		    /*
		     * If it's stdin we have to clear the EOF and error flags:
		     * this is because if the user specifies - again they would
		     * get an error like:
		     *
		     *	    Warning: read_all: EOF found at start of reading stream
		     *	    FATAL[35]: main: error reading data in file -
		     *
		     * But that's an error itself.
		     */
		    clearerr(stdin);
		} else {
		    /*
		     * If it's not stdin we close the stream as we no longer need
		     * it: we either read in all the data or there was an error but
		     * we close the file before we check for NULL input (meaning
		     * read_all() failed).
		     */
		    errno = 0;
		    ret = fclose(input_stream);
		    if (ret != 0) {
			warnp(__func__, "error in fclose on file %s", argv[i]);
		    }
		    input_stream = NULL;
		}
		if (input == NULL) {
		    err(35, __func__, "error reading data in file %s", argv[i]);
		    not_reached();
		}
		/* scan for embedded NUL bytes (before EOF) */
		if (!is_string(input, length + 1)) {
		    err(36, __func__, "found NUL byte before EOF: %s", argv[i]);
		    not_reached();
		}
		dbg(DBG_MED, "file length: %ju", (uintmax_t)length);
	    } else if (errno == ENOENT) {
		/*
		 * Only if the file does not exist do we read it as a string
		 * itself.
		 */
		dbg(DBG_LOW, "about to read all string from arg %d", i-optind);
		input = argv[i];
		length = strlen(input);
		/*
		 * Scan for embedded NUL bytes before EOF: this is probably not
		 * needed when reading from the command line but we do it just
		 * in case there's something funny going on.
		 */
		if (!is_string(input, length + 1)) {
		    err(37, __func__, "found NUL byte before end of string");
		    not_reached();
		}
		dbg(DBG_MED, "arg length: %ju", (uintmax_t)length);
	    } else {
		/*
		 * File does exist but we couldn't open it: report error and
		 * exit.
		 */
		err(38, __func__, "couldn't open file %s", argv[i]);
		not_reached();
	    }

	    /*
	     * If we get here just print out the string or file to stdout.
	     */
	    errno = 0;		/* pre-clear errno for warnp() */
	    outputlen = fwrite(input, 1, length, stdout);
	    if (outputlen != length) {
		err(39, __func__, "error: write of %ju bytes of arg: %d returned: %ju",
				(uintmax_t)length, i-optind, (uintmax_t)outputlen);
		not_reached();
	    }

	    /*
	     * we also print a newline if -n not specified
	     */
	    print_newline(output_newline);

	    /*
	     * free string
	     */
	    if (input == NULL) {
		free(input);
		input = NULL;
	    }
	}

    /*
     * case: process data from stdin
     */
    } else {
	/*
	 * read all of stdin
	 */
	dbg(DBG_LOW, "about to read all data from stdin");
	input = read_all(stdin, &length);
	if (input == NULL) {
	    err(40, __func__, "error while reading data in stdin");
	    not_reached();
	}
	dbg(DBG_MED, "stdin read length: %ju", (uintmax_t)length);

	/*
	 * Warn if string data contains an embedded NUL byte.
	 *
	 * NOTE: The read_all will ensure that at least one extra byte
	 *	 will have been allocated and set to NUL. Thus in order
	 *	 to correctly check if data contains an embedded NUL byte,
	 *	 we MUST check for a length of length+1!
	 */
	if (!is_string(input, length+1)) {
	    err(41, __func__, "data contains an embedded NUL byte");
	    not_reached();
	}

	dbg(DBG_MED, "string length: %ju", (uintmax_t)length);
	errno = 0;		/* pre-clear errno for warnp() */
	outputlen = fwrite(input, 1, length, stdout);
	if (outputlen != length) {
	    err(42, __func__, "error: write of %ju bytes of stdin data: returned: %ju",
			    (uintmax_t)length, (uintmax_t)outputlen);
	    not_reached();
	}
	dbg(DBG_MED, "stdout write length: %ju", (uintmax_t)outputlen);

	/*
	 * unless -n specified we output a newline after each arg processed
	 */
	print_newline(output_newline);

	/*
	 * free string
	 */
	if (input == NULL) {
	    free(input);
	    input = NULL;
	}
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(0); /*ooo*/
}

/* print_newline	- prints a newline to stdout
 *
 * given:
 *
 *
 *	output_newline	- true ==> print out a newline after output
 *
 * NOTE: We have this in a function because the same code is used every time we
 * print something out as long as -n was not specified.
 */
static void
print_newline(bool output_newline)
{
    int ret;

    if (output_newline) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = putchar('\n');
	if (ret != '\n') {
	    err(43, __func__, "error while writing newline");
	    not_reached();
	}
    }
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
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JPARSE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
