/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jfloat - convert JSON floating point strings
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
#include <math.h>

/*
 * Our header file - #includes the header files we need
 */
#include "jfloat.h"

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
    size_t retlen;		/* length of the string given to calloc_json_conv_float_str() */
    bool error = false;		/* true ==> JSON floating point conversion test suite error */
    bool test_mode = false;	/* true ==> perform JSON floating point conversion test suite */
    bool strict = false;	/* true ==> JSON decode in strict mode */
    struct json *node = NULL;		/* malloced JSON parser tree node */
    struct json_floating *item = NULL;	/* floating element in JSON parser tree node */
    int arg_cnt = 0;		/* number of args to process */
#if defined(JFLOAT_TEST_ENABLED)
    char *test = NULL;		/* test string */
    bool test_error = false;	/* true ==> current JSON floating point conversion test error */
#endif /* JFLOAT_TEST_ENABLED */
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
	    print("%s\n", JFLOAT_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 't':		/* -t - validate the contents of the jenc[] table */
	    test_mode = true;
	    quiet = true;	/* -t implies -q */
	    msg_warn_silent = true;
	    break;
	case 'S':		/* -S - strict mode */
	    test_mode = true;	/* -S implies -t */
	    quiet = true;	/* -S implies -q */
	    msg_warn_silent = true;
	    strict = true;
	    break;
	default:
	    usage(4, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    arg_cnt = argc - optind;
    if (test_mode == true && arg_cnt != 0) {
	usage(4, "without -t, no args allowed", program); /*ooo*/
	not_reached();
    } else if (test_mode == false && arg_cnt < REQUIRED_ARGS) {
	usage(4, "without -t, requires at least one argument", program); /*ooo*/
	not_reached();
    }
    dbg(DBG_MED, "test mode: %s", (test_mode == true) ? "enabled" : "disabled");
    dbg(DBG_MED, "strict mode: %s", (strict == true) ? "enabled" : "disabled");

#if defined(JFLOAT_TEST_ENABLED)
    /*
     * case: test node
     */
    if (test_mode == true) {

	/*
	 * process all test cases
	 */
	for (i=0; i < test_count; ++i) {

	    /*
	     * firewall - test string cannot be NULL
	     */
	    test = test_set[i];
	    if (test == NULL) {
		err(10, __func__, "test_set[%d] == NULL", i);
		not_reached();
	    }
	    inputlen = strlen(test);
	    test_error = false;
	    dbg(DBG_HIGH, "test[%d] string: <%s>", i, test);

	    /*
	     * convert test into struct json_floating
	     */
	    node = calloc_json_conv_float_str(test, &retlen);
	    if (node == NULL) {
		err(11, __func__, "calloc_json_conv_float_str() is not supposed to return NULL!");
		not_reached();
	    }
	    item = &(node->element.floating);
	    if (inputlen != retlen) {
		err(12, __func__, "test[%d] inputlen: %ju != retlen: %ju",
				 i, (uintmax_t)inputlen, (uintmax_t)retlen);
		not_reached();
	    }

	    /*
	     * perform tests for both strict and non-strict testing
	     */

	    /*
	     * test string lengths
	     */
	    if (test_result[i].orig_len != inputlen) {
		dbg(DBG_VHIGH, "test_result[%d].orig_len: %ju != inputlen: %ju",
			     i, (uintmax_t)test_result[i].orig_len, (uintmax_t)inputlen);
		test_error = true;
	    } else {
		dbg(DBG_VVHIGH, "test_result[%d].orig_len: %ju == inputlen: %ju",
			     i, (uintmax_t)test_result[i].orig_len, (uintmax_t)inputlen);
	    }
	    if (test_result[i].as_str_len != retlen) {
		dbg(DBG_VHIGH, "test_result[%d].as_str_len: %ju != retlen: %ju",
			     i, (uintmax_t)test_result[i].as_str_len, (uintmax_t)retlen);
		test_error = true;
	    } else {
		dbg(DBG_VVHIGH, "test_result[%d].as_str_len: %ju == retlen: %ju",
			     i, (uintmax_t)test_result[i].as_str_len, (uintmax_t)retlen);
	    }

	    /* test: top level booleans */
	    if (test_result[i].converted != item->converted) {
		dbg(DBG_VHIGH, "test_result[%d].converted: %d != item.converted: %d",
			     i, test_result[i].converted, item->converted);
		test_error = true;
	    } else {
		dbg(DBG_VVHIGH, "test_result[%d].converted: %d == item.converted: %d",
			        i, test_result[i].converted, item->converted);
	    }
	    if (test_result[i].is_negative != item->is_negative) {
		dbg(DBG_VHIGH, "test_result[%d].is_negative: %d != item.is_negative: %d",
			     i, test_result[i].is_negative, item->is_negative);
		test_error = true;
	    } else {
		dbg(DBG_VVHIGH, "test_result[%d].is_negative: %d == item.is_negative: %d",
			        i, test_result[i].is_negative, item->is_negative);
	    }

	    /* test: float */
	    check_val(&test_error, "float", i,
				   test_result[i].float_sized, item->float_sized,
				   test_result[i].as_float, item->as_float,
				   test_result[i].as_float_int, item->as_float_int, strict);

	    /* test: double */
	    check_val(&test_error, "double", i,
				    test_result[i].double_sized, item->double_sized,
				    test_result[i].as_double, item->as_double,
				    test_result[i].as_double_int, item->as_double_int, strict);

	    /* test: long double */
	    check_val(&test_error, "longdouble", i,
				   test_result[i].longdouble_sized, item->longdouble_sized,
				   test_result[i].as_longdouble, item->as_longdouble,
				   test_result[i].as_longdouble_int, item->as_longdouble_int, strict);

	    /*
	     * if this test failed, force non-zero exit
	     */
	    if (test_error == true) {
		dbg(DBG_MED, "test %d failed", i);
		error = true;
	    } else {
		dbg(DBG_HIGH, "test %d passed", i);
	    }
	}

	/*
	 * exit according to test results
	 */
	if (error == true) {
	    dbg(DBG_LOW, "some tests FAILED");
	    if (strict == true) {
		err(2, __func__, "some test(s) failed in strict mode"); /*ooo*/
		not_reached();
	    }
	    err(1, __func__, "some test(s) failed in non-strict mode"); /*ooo*/
	    not_reached();
	}
	dbg(DBG_LOW, "all tests PASSED");
	exit(0); /*ooo*/
    }
#endif /* JFLOAT_TEST_ENABLED */

    /*
     * case: non-test mode, output beginning of test_set[], unless -q
     */
    if (quiet == false) {
	print("#define TEST_COUNT (%d)\n\n", arg_cnt);
	prstr("int const test_count = TEST_COUNT;\n\n");
	prstr("char *test_set[TEST_COUNT+1] = {\n");
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
	prstr("};\n");
    }

    /*
     * output beginning of test_result[], unless -q
     */
    if (quiet == false) {
	prstr("\n");
	prstr("struct json_floating test_result[TEST_COUNT] = {\n");
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
	 * Convert the JSON floating point string
	 *
	 * We call the calloc_json_conv_float_str() interface, which in
	 * turn calls the calloc_json_conv_float() interface in order
	 * to check the inputlen vs *retlen value.
	 */
	node = calloc_json_conv_float_str(input, &retlen);
	if (node == NULL) {
	    err(13, __func__, "calloc_json_conv_flot_str() is not supposed to return NULL!");
	    not_reached();
	}
	item = &(node->element.floating);
	if (inputlen != retlen) {
	    err(14, __func__, "inputlen: %ju != retlen: %ju", (uintmax_t)inputlen, (uintmax_t)retlen);
	    not_reached();
	}

	/*
	 * output struct json_floating element, unless -q
	 */
	if (quiet == false) {

	    /*
	     * use a comment to remind us of the value that produced this struct json_floating element
	     */
	    print("    /* test_result[%d]: \"%s\" */\n", i-optind, input);
	    prstr("    {\n");

	    /*
	     * output as_str
	     */
	    print("\t/* malloced JSON floating point string, whitespace trimmed if needed */\n"
		  "\t\"%s\",\n\n", input);

	    /*
	     * print JSON string lengths
	     */
	    print("\t%ju,\t/* length of original JSON floating point string */\n", (uintmax_t)inputlen);
	    print("\t%ju,\t/* length of as_str */\n\n", (uintmax_t)inputlen);

	    /*
	     * print bool converted and bool is_negative
	     */
	    print("\t%s,\t/* true ==> able to convert JSON floating point to some form of C floating point type */\n",
		  item->converted ? "true" : "false");
	    print("\t%s,\t/* true ==> value < 0 */\n",
		  item->is_negative ? "true" : "false");

	    /*
	     * print float info
	     */
	    prinfo(item->float_sized, item->as_float, item->as_float_int,
	           "true ==> converted JSON floating point to C float",
		   "JSON floating point value in float form",
		   "if float_sized == true, true ==> as_float is an integer");

	    /*
	     * print double info
	     */
	    prinfo(item->double_sized, item->as_double, item->as_double_int,
	            "true ==> converted JSON floating point to C double",
		    "JSON floating point value in double form",
		    "if double_sized == true, true ==> as_double is an integer");

	    /*
	     * print long double info
	     */
	    prinfo(item->longdouble_sized, item->as_longdouble, item->as_float_int,
	           "true ==> converted JSON floating point to C long double",
		   "JSON floating point value in long double form",
		   "if float_sized == true, true ==> as_float is an integer");

	    /*
	     * close struct json_floating element with }
	     */
	    prstr("    },\n\n");
	}

	/*
	 * free buffer
	 */
	if (item != NULL) {
	    free(item);
	    item = NULL;
	}
    }

    /*
     * output end if test_result[], unless -q
     */
    if (quiet == false) {

	/*
	 * close test_result[] array
	 */
	prstr("};\n");
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    if (error == true) {
	exit(15);
    }
    exit(0); /*ooo*/
}


#if defined(JFLOAT_TEST_ENABLED)
/*
 * check_val - test an aspect of conversion into struct json_floating for signed value
 *
 * If the reference matches the converted for a given test aspect, this
 * function just returns, else *testp is false.
 *
 * given:
 *	testp	- pointer to set to false if aspect of conversion fails
 *	type	- test aspect name type
 *	testnum	- test number
 *	size_a	- reference (test_result[i]) conversion into type boolean
 *	size_b	- converted (item->) type boolean
 *	val_a	- signed reference (test_result[i]) conversion into type value
 *	val_b	- signed reference (item->) conversion into type value
 *	int_a	- reference (test_result[i]) is an integer
 *	int_b	- reference (item->) is an integer
 *	strict	- test for strict match
 *
 * NOTE: This function will warn in error.
 */
static void
check_val(bool *testp, char const *type, int testnum, bool size_a, bool size_b,
	 long double val_a, long double val_b, bool int_a, bool int_b, bool strict)
{
    long double diff;		/* absolute difference between val_a and val_b */
    long double diff_part;	/* absolute difference between val_a and val_b as one part of val_a */

    /*
     * firewall
     */
    if (testp == NULL) {
	warn(__func__, "testp is NULL");
	return;
    }
    if (type == NULL) {
	warn(__func__, "type is NULL");
	return;
    }

    /*
     * compare booleans
     */
    if (size_a != size_b) {
	dbg(DBG_VHIGH, "test_result[%d].%s_sized: %s != item->%s_sized: %s",
		       testnum,
		       type, size_a ? "true" : "false",
		       type, size_b ? "true" : "false");
	*testp = true; /* test failed */
    } else {
	dbg(DBG_VVHIGH, "test_result[%d].%s_sized: %s == item->%s_sized: %s",
		        testnum,
		        type, size_a ? "true" : "false",
		        type, size_b ? "true" : "false");
    }
    if (int_a != int_b) {
	dbg(DBG_VHIGH, "test_result[%d].%s_int: %s != item->%s_int: %s",
		       testnum,
		       type, int_a ? "true" : "false",
		       type, int_b ? "true" : "false");
	*testp = true; /* test failed */
    } else {
	dbg(DBG_VVHIGH, "test_result[%d].%s_int: %s == item->%s_int: %s",
		        testnum,
		        type, int_a ? "true" : "false",
		        type, int_b ? "true" : "false");
    }

    /*
     * compare values only if size_a is true
     */
    if (size_a == true) {

	/*
	 * case strict: test for exact match
	 */
	if (strict == true) {
	    if (val_a != val_b) {
		dbg(DBG_VHIGH, "test_result[%d].as_%s: %.22Lg != item->as_%s: %.22Lg",
			       testnum,
			       type, val_a,
			       type, val_b);
		*testp = true; /* test failed */
	    } else {
		dbg(DBG_VVHIGH, "test_result[%d].as_%s: %.22Lg == item->as_%s: %.22Lg",
				testnum,
				type, val_a,
				type, val_b);
	    }

	/*
	 * case non-strict: test for match to one part in MATCH_PRECISION
	 */
	} else {

	    /* determine the difference */
	    diff = fabsl(val_a - val_b);

	    /* determine the difference as part of the whole */
	    if (diff != 0.0) {
		diff_part = val_a / diff;
	    } else {
		diff_part = 0.0;	/* exact match */
	    }

	    /* compare difference as part of the whole */
	    if (diff_part <= MATCH_PRECISION) {
		dbg(DBG_VVHIGH, "test_result[%d].as_%s: %.22Lg similar to item->as_%s: %.22Lg diff precision: %.22Lg",
				testnum,
				type, val_a,
				type, val_b, diff_part);

	    } else {
		dbg(DBG_VHIGH, "test_result[%d].as_%s: %.22Lg not similar to item->as_%s: %.22Lg diff precision: %.22Lg",
			       testnum,
			       type, val_a,
			       type, val_b, diff_part);
	    }
	}
    }
    return;
}
#endif /* JFLOAT_TEST_ENABLED */


/*
 * prinfo - print information about a struct json_floating signed element
 *
 * given:
 *	sized	- boolean indicating if the value was set
 *	value	- value to be printed as an intmax_t
 *	intval	- boolean indicating if the value an integer
 *	scomm	- comment relating to the sized boolean
 *	vcomm	- comment relating to the value as an intmax_t
 *	sintval	- comment relating to the intval boolean
 *
 * NOTE: This function does not return on error.
 */
static void
prinfo(bool sized, long double value, bool intval, char const *scomm, char const *vcomm, char const *sintval)
{
    /*
     * firewall
     */
    if (scomm == NULL || vcomm == NULL || sintval == NULL) {
	err(16, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * case: sized is true - value to print
     */
    prstr("\n");
    if (sized == true) {
	print("\ttrue,\t/* %s */\n", scomm);
	print("\t%.22Lg,\t/* %s */\n", value, vcomm);
	print("\t%s,\t/* %s */\n", intval ? "true" : "false", sintval);

    /*
     * case: sized is false - no value to print
     */
    } else {
	print("\tfalse,\t/* %s */\n", scomm);
	print("\t0,\t/* no %s */\n", vcomm);
	print("\tfalse,\t/* %s */\n", sintval);

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
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JFLOAT_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
