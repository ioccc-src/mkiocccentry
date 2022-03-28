/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jint - convert JSON integer strings
 *
 * "Because the JSON co-founders flawed minimalism is sub-minimal." :-)
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
#include "jint.h"

/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    char *input;		/* argument to process */
    size_t inputlen;		/* length of argument string */
    size_t retlen;		/* length of the string given to malloc_json_conv_int_str() */
    bool error = false;		/* true ==> JSON integer conversion test suite error */
    bool test_mode = false;	/* true ==> perform JSON integer conversion test suite */
    bool strict = false;	/* true ==> JSON decode in strict mode */
    struct integer *ival = NULL;	/* malloc_json_conv_int_str() return */
    int arg_cnt = 0;		/* number of args to process */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:VqtS")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(3, "-h help mode", program); /*ooo*/
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
	    print("%s\n", JINT_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    break;
	case 't':		/* -t - validate the contents of the jenc[] table */
	    test_mode = true;
	    break;
	case 'S':		/* -S - strict mode */
	    test_mode = true;	/* -S implies -t */
	    strict = true;
	    break;
	default:
	    usage(4, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    /* be warn(), warnp() and msg() quiet of -q and -v 0 */
    if (quiet == true && verbosity_level <= 0) {
	msg_output_allowed = false;
	warn_output_allowed = false;
    }
    arg_cnt = argc - optind;
    if (test_mode == true && arg_cnt != 0) {
	usage(4, "without -t, no args allowed", program); /*ooo*/
	not_reached();
    } else if (test_mode == false && arg_cnt < REQUIRED_ARGS) {
	usage(4, "without -t, requires at least 1 argument", program); /*ooo*/
	not_reached();
    }
    dbg(3, "test mode: %s", (test_mode == true) ? "enabled" : "disabled");
    dbg(3, "strict mode: %s", (strict == true) ? "enabled" : "disabled");

    /*
     * case: test node
     */
    if (test_mode == true) {
	err(5, __func__, "test node not yet implemented - XXX");
	not_reached();
    }

    /*
     * case: non-test mode, output beginning of test_set[], unless -q
     */
    if (quiet == false) {
	print("#define TEST_COUNT (%d)\n\n", arg_cnt);
	prstr("char test_set[TEST_COUNT+1] = {\n");
    }

    /*
     * first output the test string array
     */
    for (i=optind; i < argc; ++i) {

	/*
	 * obtain argument string
	 */
	input = argv[i];

	/*
	 * output test_set element for this arg, unless -q
	 */
	if (quiet == false) {
	    print("    \"%s\",\n", input);
	}
    }

    /*
     * output end if test_set[], unless -q
     */
    if (quiet == false) {
	prstr("    NULL\n");
	prstr("}\n");
    }

    /*
     * output beginning of test_result[], unless -q
     */
    if (quiet == false) {
	prstr("\n");
	prstr("struct integer test_result[TEST_COUNT] = {\n");
    }

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
	 * Convert the JSON integer string
	 *
	 * We call the malloc_json_conv_int_str() interface, which in
	 * turn calls the malloc_json_conv_int() interface in order
	 * to check the inputlen vs *retlen value.
	 */
	ival = malloc_json_conv_int_str(input, &retlen);
	if (ival == NULL) {
	    err(6, __func__, "malloc_json_conv_int_str() is not supposed to return NULL!");
	    not_reached();
	}
	if (inputlen != retlen) {
	    err(7, __func__, "inputlen: %ju != retlen: %ju", (uintmax_t)inputlen, (uintmax_t)retlen);
	    not_reached();
	}

	/*
	 * output struct integer element, unless -q
	 */
	if (quiet == false) {

	    /*
	     * use a comment to remind us of the value that produced this struct integer element
	     */
	    print("    /* test_result[%d]: \"%s\" */\n", i-optind, input);
	    prstr("    {\n");

	    /*
	     * output as_str
	     */
	    print("\t/* malloced JSON integer string trimmed if needed, that was converted */\n"
		  "\t\"%s\",\n\n", input);

	    /*
	     * print bool converted and bool is_negative
	     */
	    print("\t%s,\t/* true ==> able to convert JSON integer to some form of C integer type */\n",
		  ival->converted ? "true" : "false");
	    print("\t%s,\t/* true ==> integer < 0 */\n",
		  ival->is_negative ? "true" : "false");

	    /*
	     * print int8_t info
	     */
	    prinfo(ival->int8_sized, ival->as_int8,
	           "true ==> converted JSON integer to C int8_t",
		   "JSON integer in int8_t form");

	    /*
	     * print uint8_t info
	     */
	    pruinfo(ival->uint8_sized, ival->as_uint8,
	            "true ==> converted JSON integer to C uint8_t",
		    "JSON integer in uint8_t form");

	    /*
	     * print int16_t info
	     */
	    prinfo(ival->int16_sized, ival->as_int16,
	           "true ==> converted JSON integer to C int16_t",
		   "JSON integer in int16_t form");

	    /*
	     * print uint16_t info
	     */
	    pruinfo(ival->uint16_sized, ival->as_uint16,
	            "true ==> converted JSON integer to C uint16_t",
		    "JSON integer in uint16_t form");

	    /*
	     * print int32_t info
	     */
	    prinfo(ival->int32_sized, ival->as_int32,
	           "true ==> converted JSON integer to C int32_t",
		   "JSON integer in int32_t form");

	    /*
	     * print uint32_t info
	     */
	    pruinfo(ival->uint32_sized, ival->as_uint32,
	            "true ==> converted JSON integer to C uint32_t",
		    "JSON integer in uint32_t form");

	    /*
	     * print int64_t info
	     */
	    prinfo(ival->int64_sized, ival->as_int64,
	           "true ==> converted JSON integer to C int64_t",
		   "JSON integer in int64_t form");

	    /*
	     * print uint64_t info
	     */
	    pruinfo(ival->uint64_sized, ival->as_uint64,
	            "true ==> converted JSON integer to C uint64_t",
		    "JSON integer in uint64_t form");

	    /*
	     * print int info
	     */
	    prinfo(ival->int_sized, ival->as_int,
	           "true ==> converted JSON integer to C int",
		   "JSON integer in int form");

	    /*
	     * print unsigned int info
	     */
	    pruinfo(ival->uint_sized, ival->as_uint,
	            "true ==> converted JSON integer to C unsigned int",
		    "JSON integer in unsigned int form");

	    /*
	     * print long info
	     */
	    prinfo(ival->long_sized, ival->as_long,
	           "true ==> converted JSON integer to C long",
		   "JSON integer in long form");

	    /*
	     * print unsigned long info
	     */
	    pruinfo(ival->ulong_sized, ival->as_ulong,
	            "true ==> converted JSON integer to C unsigned long",
		    "JSON integer in unsigned long form");

	    /*
	     * print long long info
	     */
	    prinfo(ival->longlong_sized, ival->as_longlong,
	           "true ==> converted JSON integer to C long long",
		   "JSON integer in long long form");

	    /*
	     * print unsigned long long info
	     */
	    pruinfo(ival->ulonglong_sized, ival->as_ulonglong,
	            "true ==> converted JSON integer to C unsigned long long",
		    "JSON integer in unsigned long long form");

	    /*
	     * print ssize_t info
	     */
	    prinfo(ival->ssize_sized, ival->as_ssize,
	           "true ==> converted JSON integer to C ssize_t",
		   "JSON integer in ssize_t form");

	    /*
	     * print size_t info
	     */
	    pruinfo(ival->size_sized, ival->as_size,
	            "true ==> converted JSON integer to C size_t",
		    "JSON integer in size_t form");

	    /*
	     * print off_t info
	     */
	    prinfo(ival->off_sized, ival->as_off,
	           "true ==> converted JSON integer to C off_t",
		   "JSON integer in off_t form");

	    /*
	     * print intmax_t info
	     */
	    prinfo(ival->maxint_sized, ival->as_maxint,
	           "true ==> converted JSON integer to C intmax_t",
		   "JSON integer in intmax_t form");

	    /*
	     * print uintmax_t info
	     */
	    pruinfo(ival->umaxint_sized, ival->as_umaxint,
	            "true ==> converted JSON integer to C uintmax_t",
		    "JSON integer in uintmax_t form");

	    /*
	     * close struct integer element with }
	     */
	    prstr("    },\n\n");
	}

	/*
	 * free buffer
	 */
	if (ival == NULL) {
	    free(ival);
	    ival = NULL;
	}
    }

    /*
     * output end if test_result[], unless -q
     */
    if (quiet == false) {

	/*
	 * close test_result[] array
	 */
	prstr("}\n");
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    if (error == true) {
	exit(8);
    }
    exit(0); /*ooo*/
}


/*
 * prinfo - print information about a struct integer signed element
 *
 * given:
 *	sized	- boolean indicating if the value was set
 *	value	- value to be printed as an intmax_t
 *	scomm	- comment relating to the sized boolean
 *	vcomm	- comment relating to the value as an intmax_t
 *
 * NOTE: This function does not return on error.
 */
static void
prinfo(bool sized, intmax_t value, char const *scomm, char const *vcomm)
{
    /*
     * firewall
     */
    if (scomm == NULL || vcomm == NULL) {
	err(9, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * case: sized is true - value to print
     */
    prstr("\n");
    if (sized == true) {
	print("\ttrue,\t/* %s */\n", scomm);
	print("\t%jd,\t/* %s */\n", value, vcomm);

    /*
     * case: sized is false - no value to print
     */
    } else {
	print("\tfalse,\t/* %s */\n", scomm);
	print("\t0,\t/* no %s */\n", vcomm);

    }
    return;
}


/*
 * pruinfo - print information about a struct integer unsigned element
 *
 * given:
 *	sized	- boolean indicating if the value was set
 *	value	- value to be printed as an intmax_t
 *	scomm	- comment relating to the sized boolean
 *	vcomm	- comment relating to the value as an intmax_t
 *
 * NOTE: This function does not return on error.
 */
static void
pruinfo(bool sized, uintmax_t value, char const *scomm, char const *vcomm)
{
    /*
     * firewall
     */
    if (scomm == NULL || vcomm == NULL) {
	err(10, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * case: sized is true - value to print
     */
    prstr("\n");
    if (sized == true) {
	print("\ttrue,\t/* %s */\n", scomm);
	print("\t%ju,\t/* %s */\n", value, vcomm);

    /*
     * case: sized is false - no value to print
     */
    } else {
	print("\tfalse,\t/* %s */\n", scomm);
	print("\t0,\t/* no %s */\n", vcomm);

    }
    return;
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
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JINT_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
