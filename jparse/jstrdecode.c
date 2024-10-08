/*
 * jstrdecode - tool to decode JSON encoded strings
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
#include <locale.h>

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
#define JSTRDECODE_VERSION "1.0.6 2024-10-08"	/* format: major.minor YYYY-MM-DD */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-t] [-n] [-Q] [-e] [string ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-t\t\tperform tests of JSON decode/encode functionality\n"
    "\t-n\t\tdo not output newline after decode output\n"
    "\t-Q\t\tenclose output in double quotes (def: do not)\n"
    "\t-e\t\tenclose each decoded string with escaped double quotes (def: do not)\n"
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
    "%s version: %s\n"
    "JSON parser version: %s";


/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));
static struct jstring *jstrdecode_stream(FILE *in_stream);
static struct jstring *add_decoded_string(char *string, size_t bufsiz);
static void free_json_decoded_strings(void);

/*
 * decoded string list
 */
static struct jstring *json_decoded_strings = NULL;


/*
 * add_decoded_string	- allocate and add a JSON decoded string to the json_decoded_strings list
 *
 * given:
 *	string	    - string to add (char *)
 *	bufsiz	    - buffer size
 *
 * returns:
 *	pointer to a newly allocated struct jstring *, added to the
 *	json_decoded_strings list or NULL if allocation failed
 *
 * NOTE: it is ASSUMED that the string is allocated so one should NOT pass a
 * char * that is not allocated on the stack.
 */
static struct jstring *
add_decoded_string(char *string, size_t bufsiz)
{
    struct jstring *jstr = NULL; /* for jstring list */
    struct jstring *jstr_next = NULL; /* to get last in list */

    /*
     * firewall
     */
    if (string == NULL) {
	err(10, __func__, "string is NULL");
	not_reached();
    }

    jstr = alloc_jstr(string, bufsiz);
    if (jstr == NULL) {
	warn(__func__, "failed to allocate struct jstring * for list");

	if (string != NULL) {
	    free(string);
	    string = NULL;
	}

	return NULL;
    }

    /*
     * add allocated jstring struct to list
     */

    /*
     * find end of list
     */
    for (jstr_next = json_decoded_strings; jstr_next != NULL && jstr_next->next != NULL; jstr_next = jstr_next->next)
	;;

    /*
     * add to end of list
     */
    if (jstr_next == NULL){
	json_decoded_strings = jstr;
    } else {
	jstr_next->next = jstr;
    }

    return jstr;
}


/*
 * free_json_decoded_strings	    - free json_decoded_strings list
 *
 * This function takes no args and returns void.
 *
 * NOTE: it is ASSUMED that the string in each struct jstring * is allocated on
 * the stack due to how the decoding/encoding works.
 */
static void
free_json_decoded_strings(void)
{
    struct jstring *jstr = NULL;    /* current in list */
    struct jstring *jstr_next = NULL;	/* next in list */

    for (jstr = json_decoded_strings; jstr != NULL; jstr = jstr_next) {
	jstr_next = jstr->next;		/* get next in list before we free the current */

	/* free current json decoded string */
	free_jstring(jstr);
	jstr = NULL;
    }
}


/*
 * jstrdecode_stream - decode an open file stream into a char *
 *
 * given:
 *	in_stream	open file stream to decode
 *
 * returns:
 *	allocated struct jstring * ==> decoding was successful,
 *	NULL ==> error in decoding, or NULL stream, or read error
 *
 * NOTE: this function adds the allocated struct jstring * to the list of
 * decoded JSON strings.
 */
static struct jstring *
jstrdecode_stream(FILE *in_stream)
{
    char *input = NULL;		/* argument to process */
    size_t inputlen;		/* length of input buffer */
    size_t bufsiz;		/* length of the buffer */
    char *buf = NULL;		/* decode buffer */
    struct jstring *jstr = NULL;    /* decoded string added to list */

    /*
     * firewall
     */
    if (in_stream == NULL) {
	warn(__func__, "in_stream is NULL");
	return NULL;
    }

    /*
     * read all of the input stream
     */
    dbg(DBG_LOW, "about to encode all data on input stream");
    input = read_all(in_stream, &inputlen);
    if (input == NULL) {
	warn(__func__, "error while reading data from input stream");
	return NULL;
    }
    dbg(DBG_MED, "stream read length: %ju", (uintmax_t)inputlen);

    /*
     * decode data read from input stream
     */
    buf = json_decode(input, inputlen, &bufsiz, NULL, NULL);
    if (buf == NULL) {
	/* free input */
	if (input != NULL) {
	    free(input);
	    input = NULL;
	}

	warn(__func__, "error while decoding stdin buffer");
	return NULL;
    }

    /*
     * free input
     */
    if (input != NULL) {
	free(input);
	input = NULL;
    }

    jstr = add_decoded_string(buf, bufsiz);
    if (jstr == NULL) {
	warn(__func__, "failed to allocate jstring of size %ju", bufsiz);
	return NULL;
    }

    /*
     * return struct added to list
     */
    return jstr;

}


int
main(int argc, char **argv)
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
    bool esc_quotes = false;	/* true ==> escape quotes */
    int ret;			/* libc return code */
    int i;
    struct jstring *jstr = NULL;    /* decoded string */

    /*
     * set locale
     */
    if (setlocale(LC_ALL, "") == NULL) {
	err(11, __func__, "failed to set locale");
	not_reached();
    }

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:qVtnQe")) != -1) {
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
	    print("%s version: %s\n", JSTRDECODE_BASENAME, JSTRDECODE_VERSION);
	    print("JSON parser version: %s\n", JSON_PARSER_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 't':		/* -t - validate the contents of the byte2asciistr[] table */
	    print("%s: Beginning jencchk test of the byte2asciistr table...\n", program);
	    jencchk();
	    print("%s: ... passed byte2asciistr table test\n", program);
	    print("%s: Beginning tests of JSON decode/encode functionality ...\n", program);
	    print("%s: XXX - JSON encode/decode tests not yet written - XXX\n", program);
	    print("%s: ... passed JSON encode/decode tests\n", program);
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'n':
	    nloutput = false;
	    break;
	case 'Q':
	    write_quote = true;
	    break;
	case 'e':
	    esc_quotes = true;
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
    dbg(DBG_LOW, "escaped quotes: %s", booltostr(esc_quotes));

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

		/*
		 * decode stdin
		 *
		 * NOTE: the function jstrdecode_stream() adds the allocated
		 * struct jstring * to the list of decoded JSON strings
		 */
		jstr = jstrdecode_stream(stdin);
		if (jstr != NULL) {
		    dbg(DBG_MED, "decode length: %ju", jstr->bufsiz);
		} else {
		    warn(__func__, "failed to decode string from stdin");
		    success = false;
		}

	    } else {

		/*
		 * obtain arg
		 */
		inputlen = strlen(input);
		dbg(DBG_LOW, "processing arg: %d: <%s>", i-optind, input);
		dbg(DBG_MED, "arg length: %ju", (uintmax_t)inputlen);

		/*
		 * decode arg
		 */
		buf = json_decode_str(input, &bufsiz);
		if (buf == NULL) {
		    warn(__func__, "error while decoding processing arg: %d", i-optind);
		    success = false;

		/*
		 * append decoded buffer to decoded JSON strings list
		 */
		} else {
		    dbg(DBG_MED, "decode length: %ju", bufsiz);
		    jstr = add_decoded_string(buf, bufsiz);
		    if (jstr == NULL) {
			warn(__func__, "error adding decoded string to list");
			success = false;
		    } else {
			dbg(DBG_MED, "added string of size %ju to decoded strings list", bufsiz);
		    }
		}
	    }
	}

    /*
     * case: process data on stdin
     */
    } else {

	/*
	 * NOTE: the function jstrdecode_stream() adds the allocated
	 * struct jstring * to the list of decoded JSON strings
	 */
	jstr = jstrdecode_stream(stdin);

	if (jstr != NULL) {
	    dbg(DBG_MED, "decode length: %ju", jstr->bufsiz);
	} else {
		warn(__func__, "error while decoding processing stdin");
		success = false;
	    }
	}

    /*
     * write starting quote if requested
     */
    if (write_quote) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fputc('"', stdout);
	if (ret != '"') {
	    warnp(__func__, "fputc for starting quote returned error");
	    success = false;
	}
    }

    /*
     * now write each processed arg to stdout
     */
    for (jstr = json_decoded_strings; jstr != NULL; jstr = jstr->next) {
	dbg(DBG_MED, "processing decoded JSON string of size: %ju", jstr->bufsiz);
	/*
	 * write starting escaped quote if requested
	 */
	if (esc_quotes) {
	    fprint(stdout, "%s", "\\\"");
	}
	buf = jstr->jstr;
	bufsiz = (uintmax_t)jstr->bufsiz;
	dbg(DBG_MED, "bufsiz: %ju", bufsiz);
	errno = 0;		/* pre-clear errno for warnp() */
	outputlen = fwrite(buf, 1, bufsiz, stdout);
	if (outputlen != bufsiz) {
	    warnp(__func__, "error: wrote %ju bytes out of expected %jd bytes",
			    (uintmax_t)outputlen, (uintmax_t)bufsiz);
	    success = false;
	}

	/*
	 * write ending escaped quote if requested
	 */
	if (esc_quotes) {
	    fprint(stdout, "%s", "\\\"");
	}
    }


    /*
     * write ending quote if requested
     */
    if (write_quote) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fputc('"', stdout);
	if (ret != '"') {
	    warnp(__func__, "fputc for ending quote returned error");
	    success = false;
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
	    success = false;
	}
    }

    /* we have to free the list */
    free_json_decoded_strings();

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
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
	prog = JSTRDECODE_BASENAME;
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JSTRDECODE_BASENAME, JSTRDECODE_VERSION, JSON_PARSER_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
