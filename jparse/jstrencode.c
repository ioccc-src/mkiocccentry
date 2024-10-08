/*
 * jstrencode - tool to encode a string for JSON
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
 * jstrencode - tool to encode a string for JSON
 */
#include "jstrencode.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */

/*
 * official jstrencode version
 */
#define JSTRENCODE_VERSION "1.1.3 2024-10-08"	/* format: major.minor YYYY-MM-DD */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-t] [-n] [-Q] [-e] [string ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level: (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-t\t\tperform tests of JSON decode/encode functionality\n"
    "\t-n\t\tdo not output newline after encode output (def: print final newline)\n"
    "\t-Q\t\tdo not encode double quotes that enclose the concatenation of args (def: do encode)\n"
    "\t-e\t\tdo not output double quotes that enclose each arg (def: do not remove)\n"
    "\n"
    "\t[string ...]\tencode the concatenation of string args (def: encode stdin)\n"
    "\t\t\tNOTE: - means read from stdin\n"
    "\n"
    "Exit codes:\n"
    "     0   encode successful\n"
    "     1   encode unsuccessful\n"
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
static struct jstring *jstrencode_stream(FILE *in_stream, bool skip_enclosing, bool ignore_first, bool remove_last);
static struct jstring *add_encoded_string(char *string, size_t bufsiz);
static void free_json_encoded_strings(void);

/*
 * encoded string list
 */
static struct jstring *json_encoded_strings = NULL;


/*
 * add_encoded_string	- allocate and add a JSON encoded string to the json_encoded_strings list
 *
 * given:
 *	string	    - string to add (char *)
 *	bufsiz	    - buffer size
 *
 * returns:
 *	pointer to a newly allocated struct jstring *, added to the
 *	json_encoded_strings list or NULL if allocation failed
 *
 * NOTE: it is ASSUMED that the string is allocated so one should NOT pass a
 * char * that is not allocated on the stack.
 */
static struct jstring *
add_encoded_string(char *string, size_t bufsiz)
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


    /*
     * allocate a struct jstring * for list
     */
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
    for (jstr_next = json_encoded_strings; jstr_next != NULL && jstr_next->next != NULL; jstr_next = jstr_next->next)
	;;

    /*
     * add to end of list
     */
    if (jstr_next == NULL){
	json_encoded_strings = jstr;
    } else {
	jstr_next->next = jstr;
    }

    return jstr;
}

/*
 * free_json_encoded_strings	    - free json_encoded_strings list
 *
 * This function takes no args and returns void.
 *
 * NOTE: it is ASSUMED that the string in each struct jstring * is allocated on
 * the stack due to how the decoding/encoding works.
 */
static void
free_json_encoded_strings(void)
{
    struct jstring *jstr = NULL;    /* current in list */
    struct jstring *jstr_next = NULL;	/* next in list */

    for (jstr = json_encoded_strings; jstr != NULL; jstr = jstr_next) {
	jstr_next = jstr->next;		/* get next in list before we free the current */

	/* free current json encoded string */
	free_jstring(jstr);
	jstr = NULL;
    }
}


/*
 * jstrencode_stream - encode an open file stream into a char *
 *
 * given:
 *	in_stream	open file stream to encode
 *	skip_enclosing	skip enclosing double quotes,
 *			    false ==> process all bytes in the block
 *	ignore_first	remove any leading double quote
 *			    false ==> do not remove
 *	remove_last	remove any final double quote
 *			    false ==> do not remove
 *
 * returns:
 *	allocated struct jstring * ==> encoding was successful,
 *	NULL ==> error in encoding, or NULL stream, or read error
 *
 * NOTE: this function adds the allocated struct jstring * to the list of
 * encoded JSON strings.
 */
static struct jstring *
jstrencode_stream(FILE *in_stream, bool skip_enclosing, bool ignore_first, bool remove_last)
{
    char *orig_input = NULL;	/* argument to process */
    char *input = NULL;		/* possibly updated orig_input */
    size_t inputlen = 0;	/* length of input buffer */
    char *buf = NULL;		/* encode buffer */
    size_t bufsiz;		/* length of the buffer */
    struct jstring *jstr = NULL; /* for jstring list */

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
    dbg(DBG_LOW, "about to encode data on input stream");
    orig_input = read_all(in_stream, &inputlen);
    if (orig_input == NULL) {
	warn(__func__, "error while reading data from input stream");
	return NULL;
    }
    dbg(DBG_MED, "stream read length: %ju", (uintmax_t)inputlen);
    input = orig_input;
    dbg(DBG_HIGH, "stream data is: <%s>", input);

    /*
     * case: we need to remove BOTH a leading and a trailing double quote
     */
    if (ignore_first == true && remove_last == true && inputlen > 1 &&
	input[0] == '"' && input[inputlen-1] == '"') {

	/*
	 * remove both the leading and a trailing double quotes
	 */
	dbg(DBG_HIGH, "removing leading and trailing double quotes from stream");
	input[inputlen-1] = '\0';
	--inputlen;
	++input;
	--inputlen;
	dbg(DBG_VHIGH, "stream read length changed to: %ju", (uintmax_t)inputlen);
	dbg(DBG_VHIGH, "stream data changed to: <%s>", input);

    /*
     * case: we might need to remove either the leading or trailing double quotes, but not both
     */
    } else {

	/*
	 * remove, if needed, a leading double quote
	 */
	if (ignore_first == true && inputlen > 0 && input[0] == '"') {

	    /*
	     * remove leading double quote
	     */
	    dbg(DBG_HIGH, "removing leading double quote from stream");
	    ++input;
	    --inputlen;
	    dbg(DBG_VHIGH, "stream read length is now: %ju", (uintmax_t)inputlen);
	    dbg(DBG_VHIGH, "stream data is now: <%s>", input);

	/*
	 * remove, if needed, a trailing double quote
	 */
	} else if (remove_last == true && inputlen > 0 && input[inputlen-1] == '"') {

	    /*
	     * remove trailing double quote
	     */
	    dbg(DBG_HIGH, "removing trailing double quote from stream");
	    input[inputlen-1] = '\0';
	    --inputlen;
	    dbg(DBG_VHIGH, "stream read length changed to: %ju", (uintmax_t)inputlen);
	    dbg(DBG_VHIGH, "stream data changed to: <%s>", input);
	}
    }

    /*
     * encode data read from input stream
     */
    buf = json_encode(input, inputlen, &bufsiz, skip_enclosing);
    if (buf == NULL) {
	/* free input */
	if (input != NULL) {
	    free(input);
	    input = NULL;
	}

	warn(__func__, "error while encoding buffer");
	return NULL;
    }

    /*
     * free input
     */
    if (orig_input != NULL) {
	free(orig_input);
	orig_input = NULL;
	input = NULL; /* paranoia */
    }

    jstr = add_encoded_string(buf, bufsiz);
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
    size_t inputlen = 0;	/* length of input buffer */
    char *buf;			/* encode buffer */
    size_t bufsiz;		/* length of the buffer */
    size_t outputlen;		/* length of write of encode buffer */
    bool success = true;	/* true ==> encoding OK, false ==> error while encoding */
    bool nloutput = true;	/* true ==> output newline after JSON encode */
    bool skip_concat_quotes = false;	/* true ==> skip enclosing quotes around the arg concatenation */
    bool skip_each = false;	/* true ==> skip enclosing quotes around each arg */
    int ret;			/* libc return code */
    int i;
    struct jstring *jstr = NULL;    /* to iterate through list */

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
	    print("%s version: %s\n", JSTRENCODE_BASENAME, JSTRENCODE_VERSION);
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
	    skip_concat_quotes = true;
	    break;
	case 'e':
	    skip_each = true;
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
    dbg(DBG_LOW, "-Q: skip double quotes that enclose the arg concatenation: %s", booltostr(skip_concat_quotes));
    dbg(DBG_LOW, "-e: skip double quotes that enclose each arg: %s", booltostr(skip_each));
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

		/*
		 * encode stdin
		 */
		dbg(DBG_LOW, "encoding from stdin");

		/*
		 * NOTE: the function jstrencode_stream() adds the allocated
		 * struct jstring * to the list of encoded JSON strings
		 */
		jstr = jstrencode_stream(stdin, skip_each,
					 (skip_concat_quotes && i == optind),
					 (skip_concat_quotes && i == argc-1));
		if (!jstr) {
		    warn(__func__, "failed to encode string from stdin");
		    success = false;
		} else {
		    dbg(DBG_LOW, "encoded buf size: %ju", (uintmax_t)jstr->bufsiz);
		}

	    } else {

		/*
		 * obtain arg
		 */
		inputlen = strlen(input);
		dbg(DBG_LOW, "processing arg: %d: <%s>", i-optind, input);
		dbg(DBG_MED, "arg length: %ju", (uintmax_t)inputlen);
	    }

	    /*
	     * case: we need to remove BOTH a leading and a trailing double quote
	     */
	    if (skip_concat_quotes == true && i == optind && i == argc-1 && inputlen > 1) {

		/*
		 * remove both the leading and a trailing double quotes
		 */
		if (input[0] == '"' && input[inputlen-1] == '"') {
		    dbg(DBG_HIGH, "removing leading and trailing double quotes from arg[%d]", i-optind);
		    input[inputlen-1] = '\0';
		    --inputlen;
		    ++input;
		    --inputlen;
		    dbg(DBG_VHIGH, "arg is now: %d: <%s>", i-optind, input);
		    dbg(DBG_VHIGH, "arg length is now: %ju", (uintmax_t)inputlen);
		}

	    /*
	     * case: we might need to remove either the leading or trailing double quotes, but not both
	     */
	    } else {

		/*
		 * remove, if needed, a leading double quote
		 */
		if (skip_concat_quotes == true && i == optind && inputlen > 0 && input[0] == '"') {

		    /*
		     * remove leading double quote
		     */
		    dbg(DBG_HIGH, "removing leading double quote from arg[%d]", i-optind);
		    ++input;
		    --inputlen;
		    dbg(DBG_VHIGH, "arg is now: %d: <%s>", i-optind, input);
		    dbg(DBG_VHIGH, "arg length is now: %ju", (uintmax_t)inputlen);
		}

		/*
		 * remove, if needed, a trailing double quote
		 */
		if (skip_concat_quotes == true && i == argc-1 && inputlen > 0 && input[inputlen-1] == '"') {

		    /*
		     * remove trailing double quote
		     */
		    dbg(DBG_HIGH, "removing trailing double quote from arg[%d]", i-optind);
		    input[inputlen-1] = '\0';
		    --inputlen;
		    dbg(DBG_VHIGH, "arg is now: %d: <%s>", i-optind, input);
		    dbg(DBG_VHIGH, "arg length is now: %ju", (uintmax_t)inputlen);
		}
	    }

	    /*
	     * encode arg
	     */
	    buf = json_encode_str(input, &bufsiz, skip_each);
	    if (buf == NULL) {
		warn(__func__, "error while encoding processing arg: %d", i-optind);
		success = false;

	    /*
	     * append encoded buffer to encoded JSON strings list
	     */
	    } else {
		dbg(DBG_MED, "encode length: %ju", bufsiz);
		jstr = add_encoded_string(buf, bufsiz);
		if (jstr == NULL) {
		    warn(__func__, "error adding encoded string to list");
		    success = false;
		} else {
		    dbg(DBG_MED, "added string of size %ju to encoded strings list", bufsiz);
		}
	    }
	}

    /*
     * case: process data on stdin
     */
    } else {

	/*
	 * read all of stdin
	 */
	dbg(DBG_LOW, "encoding from stdin");
	/*
	 * NOTE: jstrencode_stream() adds the allocated struct jstring * to the
	 * encoded JSON strings list
	 */
	jstr = jstrencode_stream(stdin, skip_concat_quotes, skip_each, skip_each);
	if (jstr != NULL) {
	    dbg(DBG_MED, "encode length: %ju", jstr->bufsiz);
	} else {
	    warn(__func__, "error while encoding processing stdin");
	    success = false;
	}
    }

    /*
     * now write each processed arg to stdout
     */
    for (jstr = json_encoded_strings; jstr != NULL; jstr = jstr->next) {
	if (jstr->jstr != NULL) {
	    buf = jstr->jstr;
	    bufsiz = (uintmax_t)jstr->bufsiz;
	    errno = 0;		/* pre-clear errno for warnp() */
	    outputlen = fwrite(buf, 1, bufsiz, stdout);
	    if (outputlen != bufsiz) {
		warnp(__func__, "error: wrote %ju bytes out of expected %ju bytes",
			    (uintmax_t)outputlen, (uintmax_t)bufsiz);
		success = false;
	    }
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
    free_json_encoded_strings();

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
	prog = JSTRENCODE_BASENAME;
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JSTRENCODE_BASENAME, JSTRENCODE_VERSION, JSON_PARSER_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
