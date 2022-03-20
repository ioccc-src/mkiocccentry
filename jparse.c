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

/* parse_file	    - parses file file
 *
 * given:
 *
 *	filename    - filename to parse
 *
 * If filename is NULL or the filename is not a readable file (or is empty) or
 * if the file is not a C string or if read_all() returns NULL the function
 * warns but does nothing else.
 *
 * NOTE: Until the JSON parser is written this only prints the string out.
 */
static void
parse_file(char const *filename)
{
    size_t length = 0;		/* bytes read if file is a regular file and can be read */
    size_t outputlen = 0;	/* number of bytes sent to stdout */
    FILE *input_stream = NULL;	/* FILE * that refers to filename if filename is a regular readable file */
    char *input = NULL;		/* data of read in file */
    bool is_stdin = false;	/* true if reading from stdin (filename == "-") */
    int ret;

    /*
     * firewall
     */
    if (filename == NULL) {
	/* this should actually never happen */
	warn(__func__, "passed NULL filename");
	++num_errors;
	return;
    } else if (*filename == '\0') { /* strlen(filename) == 0 */
	warn(__func__, "passed empty filename");
	++num_errors;
	return;
    }

    is_stdin = !strcmp(filename, "-");
    if (!is_stdin && !is_file(filename)) {
	warn(__func__, "passed filename that's not actually a file: %s", filename);
	++num_errors;
	return;
    } else if (!is_stdin && !is_read(filename)) {
	warn(__func__, "passed filename not readable file: %s", filename);
	++num_errors;
	return;
    }

    errno = 0;
    input_stream = is_stdin ? stdin : fopen(filename, "r");
    if (input_stream == NULL) {
	warnp(__func__, "couldn't open file %s, ignoring", filename);
	++num_errors;
	return;
    }

    dbg(DBG_MED, "about to read from %s", is_stdin?"stdin":filename);
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
	    warnp(__func__, "error in fclose on file %s", filename);
	}
	input_stream = NULL;
    }

    if (input == NULL) {
	warn(__func__, "error while reading data from %s", is_stdin?"stdin":filename);
	++num_errors;
	return;
    }
    dbg(DBG_MED, "%s read length: %ju", is_stdin?"stdin":filename, (uintmax_t)length);

    if (!is_string(input, length + 1)) {
	warn(__func__, "found NUL byte before EOF");
	++num_errors;
	return;
    }

    /*
     * If we get here just write the file's contents to stdout.
     */
    errno = 0;		/* pre-clear errno for warnp() */
    outputlen = fwrite(input, 1, length, stdout);
    if (outputlen != length) {
	warnp(__func__, "error: write of %ju bytes of file %s returned: %ju",
			(uintmax_t)length, filename, (uintmax_t)outputlen);
	++num_errors;
    }

    dbg(DBG_MED, "%s write length: %ju", is_stdin?"stdin":filename, (uintmax_t)outputlen);

    if (input != NULL) {
	/*
	 * We KNOW input != NULL and it's okay to free a NULL pointer but we
	 * check anyway.
	 */
	free(input);
	input = NULL;
    }

    print_newline();
}

/* parse_string	    - parse string as a JSON block
 *
 * given:
 *
 *	string	    - the string to parse as JSON
 *
 * NOTE: Until the JSON parser is written this only writes the string to stdout.
 */
static void
parse_string(char const *string)
{
    size_t length = string != NULL ? strlen(string) : 0; /* length of string */
    size_t outputlen = 0; /* number of bytes sent to stdout */

    /*
     * firewall
     */
    if (string == NULL) {
	/* this should never happen */
	warn(__func__, "passed NULL string");
	return;
    } else if (*string == '\0') /* strlen(string) == 0 */ {
	warn(__func__, "passed empty string");
	return;
    }

    /*
     * If we get here just write the string to stdout.
     */
    errno = 0;		/* pre-clear errno for warnp() */
    outputlen = fwrite(string, 1, length, stdout);
    if (outputlen != length) {
	warnp(__func__, "error: write of %ju bytes of string %s returned: %ju",
			(uintmax_t)length, string, (uintmax_t)outputlen);
	++num_errors;
    }

    print_newline();
}

/* print_newline	- prints a newline to stdout if -n not specified
 *
 * NOTE: We have this in a function because the same code is used every time we
 * print something out as long as -n was not specified.
 */
static void
print_newline(void)
{
    int ret;

    if (output_newline) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = putchar('\n');
	if (ret != '\n') {
	    errp(43, __func__, "error while writing newline");
	    not_reached();
	}
    }
}

int
main(int argc, char **argv)
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    bool strict = false;	/* true ==> strict mode (currently unused: this is for when a JSON parser is added) */
    bool string_flag_used = false; /* -S string was used */
    int ret;			/* libc return code */
    int i;


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVnSTs:")) != -1) {
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
		warnp(__func__, "printf error printing IOCCC toolkit release repository tag: %s", IOCCC_TOOLKIT_RELEASE);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'n':
	    output_newline = false;
	    break;
	case 'S':
	    /*
	     * XXX currently this is unused as json parsing is not done yet and
	     * this is why the compiler flags for jparse.c has (for now) include
	     * -Wno-unused-but-set-variable.
	     */
	    strict = true;
	    dbg(DBG_MED, "enabling strict mode");
	    break;
	case 's':
	    /*
	     * So we don't trigger missing arg. Maybe there's another way but
	     * nothing is coming to my mind right now.
	     */
	    string_flag_used = true;

	    /* parse arg as a string */
	    parse_string(optarg);
	    /*
	     * XXX Rather than having an option to disable strict mode so that
	     * in the same invocation we can test some strings in strict mode
	     * and some not strict after each string is parsed the strict mode
	     * is disabled so that so that another -s has to be specified prior
	     * to the string. This does mean that if you want strict parsing of
	     * files and you specify the -s option then you must have -S after
	     * the string args.
	     *
	     * But the question is: should it be this way or should it be
	     * another design choice? For example should there be an option that
	     * specifically disables strict mode so that one can not worry about
	     * having to specify -s repeatedly? I think it might be better this
	     * way but I'm not sure what letter should do it. Perhaps -x? If we
	     * didn't use -S for strict it could be S but we do so that won't
	     * work.
	     */
	    dbg(DBG_MED, "disabling strict mode");
	    strict = false;
	    break;
	default:
	    usage(2, "invalid -flag", program); /*ooo*/
	    not_reached();
	}
    }

    /* perform IOCCC sanity checks */
    ioccc_sanity_chks();

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
	    parse_file(argv[i]);
	}

    } else if (!string_flag_used) {
	usage(2, "no file specified", program); /*ooo*/
	not_reached();
    }


    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(num_errors != 0); /*ooo*/
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
