/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jnum_gen - generate JSON number string conversion test data
 *
 * "Because the JSON co-founders minimalism is sub-minimal." :-)
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
#include "jnum_gen.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command readline_buf */
#define CHUNK (16)		/* allocate CHUNK elements at a time */


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    int arg_cnt = 0;		/* number of args to process */
    char *filename = NULL;	/* name of file containing test cases */
    FILE *stream = NULL;	/* test case open stream */
    char *readline_buf = NULL;	/* test case readline_buf buffer */
    ssize_t readline_len;	/* readline return length */
    int ret;			/* libc function return */
    int count = 0;		/* test count read from filename */
    char *p = NULL;		/* comment search */
    char *line = NULL;		/* allocated line with JSON number and/or comment */
    size_t len;			/* length of JSON test number */
    char *first = NULL;		/* start of JSON test number */
    struct json *node = NULL;	/* parsed JSON number */
    struct dyn_array *str_array = NULL;		/* dynamic array of test strings */
    struct dyn_array *result_array = NULL;	/* dynamic array of test results */
    bool moved = false;		/* true ==> realloc() moved data */
    int linenum = 0;		/* readline_buf number from filename */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vq")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(3, program, "-h help mode", -1, -1); /*ooo*/
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
	    print("%s", JNUM_CHK_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	default:
	    usage(4, program, "invalid -flag", -1, -1); /*ooo*/
	    not_reached();
	 }
    }
    arg_cnt = argc - optind;
    if (arg_cnt != REQUIRED_ARGS) {
	usage(4, program, "expected %d arguments, found: %d", REQUIRED_ARGS, arg_cnt); /*ooo*/
	not_reached();
    }
    filename = argv[optind];
    dbg(DBG_MED, "filename: %s", filename);

    /*
     * firewall - filename must be a readable file
     */
    if (is_file(filename) == false || is_read(filename) == false) {
	err(1, program, "filename is not readable file: %s", filename); /*ooo*/
	not_reached();
    }

    /*
     * open filename
     */
    errno = 0;		/* pre-clear errno for errp() */
    stream = fopen(filename, "r");
    if (stream == NULL) {
	err(10, program, "error in fopen(%s, \"r\")", filename);
	not_reached();
    }

    /*
     * create dynamic arrays for test strings
     */
    str_array = dyn_array_create(sizeof(char *), CHUNK, CHUNK, true);
    if (str_array == NULL) {
	err(11, program, "dyn_array_create() for str_array returned NULL");
	not_reached();
    }

    /*
     * create dynamic arrays for test results
     */
    result_array = dyn_array_create(sizeof(struct json *), CHUNK, CHUNK, true);
    if (result_array == NULL) {
	err(12, program, "dyn_array_create() for result_array returned NULL");
	not_reached();
    }

    /*
     * process lines from filename
     */
    while ((readline_len = readline(&readline_buf, stream)) >= 0) {

	/*
	 * form line from readline buffer
	 */
	errno = 0;		/* pre-clear errno for errp() */
	line = strdup(readline_buf);
	if (line == NULL) {
	    err(13, program, "strdup of readline buffer failed");
	    not_reached();
	}

	/*
	 * trim comments from line
	 */
	++linenum;
	dbg(DBG_VVHIGH, "linenum %d: <%s>", linenum, line);
	p = strchr(line, '#');
	if (p != NULL) {
	    /* strip comment from readline_buf */
	    *p = '\0';
	}

	/*
	 * look for text in the line to convert
	 */
	len = find_text_str(line, &first);
	if (len <= 0) {
	    /* found no JSON number string to test, ignore this readline_buf */
	    dbg(DBG_VHIGH, "only comment and/or whitespace found on line: %d", linenum);
	    continue;
	}

	/*
	 * save the test string for printing later
	 */
	moved = dyn_array_append_value(str_array, &line);
	if (moved == true) {
	     dbg(DBG_VHIGH, "FYI: dyn_array_append_value(str_array, readline_buf) realloc moved data, "
			    "count: %d readline_buf: %d", count, linenum);
	}

	/*
	 * convert the test string
	 */
	dbg(DBG_VVHIGH, "calling json_conv_number(<%*.*s>, %ju)", (int)len, (int)len, first, (uintmax_t)len);
	node = json_conv_number(first, len);
	if (node == NULL) {
	    err(14, program, "json_conv_number() returned NULL");
	    not_reached();
	}

	/*
	 * save the test result for printing later
	 */
	moved = dyn_array_append_value(result_array, &node);
	if (moved == true) {
	     dbg(DBG_VHIGH, "FYI: dyn_array_append_value(result_array, node) realloc moved data, "
			    "count: %d readline_buf: %d", count, linenum);
	}
	++count;	/* count another test */
    }

    /*
     * firewall - dynamic array size sanity check
     */
    dbg(DBG_MED, "found %d test cases in filename: %s", count, filename);
    if (dyn_array_tell(str_array) != count) {
	err(15, program, "expected %jd pointers, found %jd in str_array", (intmax_t)count, dyn_array_tell(str_array));
	not_reached();
    }
    if (dyn_array_tell(result_array) != count) {
	err(16, program, "expected %jd pointers, found %jd in result_array", (intmax_t)count, dyn_array_tell(result_array));
	not_reached();
    }

    /*
     * close filename
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fclose(stream);
    if (ret < 0) {
	err(17, program, "error in fclose");
	not_reached();
    }

    /*
     * output test count
     */
    print("#define TEST_COUNT (%d)\n\n", count);
    prstr("int const test_count = TEST_COUNT;\n\n");

    /*
     * output test strings
     */
     prstr("char *test_set[TEST_COUNT+1] = {\n");
    for (i=0; i < count; ++i) {
	line = dyn_array_value(str_array, char *, i);
	print("    \"%s\",\n", line);
    }
    prstr("    NULL\n");
    prstr("};\n\n");

    /*
     * output test results
     */
    prstr("struct json_number test_result[TEST_COUNT+1] = {\n");
    for (i=0; i < count; ++i) {

	/*
	 * print start of json_number structure
	 */
	line = dyn_array_value(str_array, char *, i);
	print("    /* test_result[%d]: %s */\n", i, line);
	prstr("    {\n");

	/*
	 * print bulk of json_number structure
	 */
	node = dyn_array_value(result_array, struct json *, i);
	if (node == NULL) {
	    err(18, program, "dyn_array_value() returned NULL");
	    not_reached();
	}
	if (node->type != JTYPE_NUMBER) {
	    err(19, program, "node->type for test %d: %d != %d", i, node->type, JTYPE_NUMBER);
	    not_reached();
	}
	fpr_number(stdout, &node->element.number);

	/*
	 * print end of json_number structure
	 */
	prstr("    },\n\n");
    }
    prstr("    /* MUST BE LAST */\n");
    prstr("    { false }\n");
    prstr("};\n");


    /*
     * free readline_buf buffers
     */
    for (i=0; i < count; ++i) {
	line = dyn_array_value(str_array, char *, i);
	if (line != NULL) {
	    free(line);
	    line = NULL;
	}
    }
    dyn_array_free(str_array);

    /*
     * free test results
     */
    for (i=0; i < count; ++i) {
	node = dyn_array_value(result_array, struct json *, i);
	if (node != NULL) {
	    free(node);
	    node = NULL;
	}
    }
    dyn_array_free(result_array);

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(0); /*ooo*/
}


/*
 * fpr_number - print the contents of struct json_number on a steam
 *
 * given:
 *	stream		open stream to print on
 *	item		pointer to struct json_number to print
 *
 * This function does not return on error.
 */
static void
fpr_number(FILE *stream, struct json_number *item)
{
    /*
     * firewall
     */
    if (stream == NULL) {
	err(20, __func__, "stream is NULL");
	not_reached();
    }
    if (item == NULL) {
	err(21, __func__, "item is NULL");
	not_reached();
    }

    /*
     * print bool converted
     */
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> able to convert JSON number string to some form of C value */\n\n",
		   booltostr(item->converted));

    /*
     * print JSON string
     */
    fprint(stream, "\t\"%s\",%s"
		   "/* allocated copy of the original allocated JSON number, NUL terminated */\n",
		   item->as_str,
		   (item->as_str_len <= 4 ? "\t\t" : "\t"));
    fprint(stream, "\t\"%s\",%s"
		   "/* first whitespace character */\n\n",
		   item->first,
		   (item->number_len <= 4 ? "\t\t" : "\t"));

    /*
     * print JSON string lengths
     */
    fprint(stream, "\t%ju,\t"
		   "\t/* length of as_str */\n",
		   (uintmax_t)item->as_str_len);
    fprint(stream, "\t%ju,\t"
		   "\t/* length of JSON number, w/o leading or trailing whitespace and NUL bytes */\n\n",
		   (uintmax_t)item->number_len);

    /*
     * print bool is_negative
     */
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> value < 0 */\n\n",
		   booltostr(item->is_negative));

    /*
     * print bool is_floating and is_e_notation
     */
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> as_str had a '.' in it such as 1.234, false ==> no '.' found */\n",
		   booltostr(item->is_floating));
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> e notation used such as 1e10, no e notation found */\n\n",
		   booltostr(item->is_e_notation));

    /*
     * print integer values
     */
    fprstr(stream, "\t/* integer values */\n");

    /*
     * print int8_t info
     */
    fpr_info(stream, item->int8_sized, item->as_int8,
	   "true ==> converted JSON integer to C int8_t",
	   "JSON integer value in int8_t form");

    /*
     * print uint8_t info
     */
    fpr_uinfo(stream, item->uint8_sized, item->as_uint8,
	    "true ==> converted JSON integer to C uint8_t",
	    "JSON integer value in uint8_t form");

    /*
     * print int16_t info
     */
    fpr_info(stream, item->int16_sized, item->as_int16,
	   "true ==> converted JSON integer to C int16_t",
	   "JSON integer value in int16_t form");

    /*
     * print uint16_t info
     */
    fpr_uinfo(stream, item->uint16_sized, item->as_uint16,
	    "true ==> converted JSON integer to C uint16_t",
	    "JSON integer value in uint16_t form");

    /*
     * print int32_t info
     */
    fpr_info(stream, item->int32_sized, item->as_int32,
	   "true ==> converted JSON integer to C int32_t",
	   "JSON integer value in int32_t form");

    /*
     * print uint32_t info
     */
    fpr_uinfo(stream, item->uint32_sized, item->as_uint32,
	    "true ==> converted JSON integer to C uint32_t",
	    "JSON integer value in uint32_t form");

    /*
     * print int64_t info
     */
    fpr_info(stream, item->int64_sized, item->as_int64,
	   "true ==> converted JSON integer to C int64_t",
	   "JSON integer value in int64_t form");

    /*
     * print uint64_t info
     */
    fpr_uinfo(stream, item->uint64_sized, item->as_uint64,
	    "true ==> converted JSON integer to C uint64_t",
	    "JSON integer value in uint64_t form");

    /*
     * print int info
     */
    fpr_info(stream, item->int_sized, item->as_int,
	   "true ==> converted JSON integer to C int",
	   "JSON integer value in int form");

    /*
     * print unsigned int info
     */
    fpr_uinfo(stream, item->uint_sized, item->as_uint,
	    "true ==> converted JSON integer to C unsigned int",
	    "JSON integer value in unsigned int form");

    /*
     * print long info
     */
    fpr_info(stream, item->long_sized, item->as_long,
	   "true ==> converted JSON integer to C long",
	   "JSON integer value in long form");

    /*
     * print unsigned long info
     */
    fpr_uinfo(stream, item->ulong_sized, item->as_ulong,
	    "true ==> converted JSON integer to C unsigned long",
	    "JSON integer value in unsigned long form");

    /*
     * print long long info
     */
    fpr_info(stream, item->longlong_sized, item->as_longlong,
	   "true ==> converted JSON integer to C long long",
	   "JSON integer value in long long form");

    /*
     * print unsigned long long info
     */
    fpr_uinfo(stream, item->ulonglong_sized, item->as_ulonglong,
	    "true ==> converted JSON integer to C unsigned long long",
	    "JSON integer value in unsigned long long form");

    /*
     * print ssize_t info
     */
    fpr_info(stream, item->ssize_sized, item->as_ssize,
	   "true ==> converted JSON integer to C ssize_t",
	   "JSON integer value in ssize_t form");

    /*
     * print size_t info
     */
    fpr_uinfo(stream, item->size_sized, item->as_size,
	    "true ==> converted JSON integer to C size_t",
	    "JSON integer value in size_t form");

    /*
     * print off_t info
     */
    fpr_info(stream, item->off_sized, item->as_off,
	   "true ==> converted JSON integer to C off_t",
	   "JSON integer value in off_t form");

    /*
     * print intmax_t info
     */
    fpr_info(stream, item->maxint_sized, item->as_maxint,
	   "true ==> converted JSON integer to C intmax_t",
	   "JSON integer value in intmax_t form");

    /*
     * print uintmax_t info
     */
    fpr_uinfo(stream, item->umaxint_sized, item->as_umaxint,
	    "true ==> converted JSON integer to C uintmax_t",
	    "JSON integer value in uintmax_t form");

    /*
     * print floating point values
     */
    fprstr(stream, "\n\t/* floating point values */\n");

    /*
     * print float info
     */
    fpr_finfo(stream, item->float_sized, item->as_float, item->as_float_int,
	   "true ==> converted JSON floating point to C float",
	   "JSON floating point value in float form",
	   "if float_sized == true, true ==> as_float is an integer");

    /*
     * print double info
     */
    fpr_finfo(stream, item->double_sized, item->as_double, item->as_double_int,
	    "true ==> converted JSON floating point to C double",
	    "JSON floating point value in double form",
	    "if double_sized == true, true ==> as_double is an integer");

    /*
     * print long double info
     */
    fpr_finfo(stream, item->longdouble_sized, item->as_longdouble, item->as_float_int,
	   "true ==> converted JSON floating point to C long double",
	   "JSON floating point value in long double form",
	   "if float_sized == true, true ==> as_float is an integer");
}


/*
 * fpr_info - print information about a struct json_number signed element
 *
 * given:
 *	stream	- open file stream to print on
 *	sized	- boolean indicating if the value was set
 *	value	- value to be printed as an intmax_t
 *	scomm	- comment relating to the sized boolean
 *	vcomm	- comment relating to the value as an intmax_t
 *
 * NOTE: This function does not return on error.
 */
static void
fpr_info(FILE *stream, bool sized, intmax_t value, char const *scomm, char const *vcomm)
{
    /*
     * firewall
     */
    if (stream == NULL || scomm == NULL || vcomm == NULL) {
	err(22, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * case: sized is true - value to print
     */
    fprstr(stream,"\n");
    if (sized == true) {
	fprint(stream, "\ttrue,\t\t/* %s */\n", scomm);
	fprint(stream, "\t%jd,%s/* %s */\n",
		       value,
		       (value < -99999 || value > 999999) ? "\t" : "\t\t",
		       vcomm);

    /*
     * case: sized is false - no value to print
     */
    } else {
	fprint(stream, "\tfalse,\t\t/* %s */\n", scomm);
	fprint(stream, "\t0,\t\t/* no %s */\n", vcomm);

    }
    return;
}


/*
 * fpr_uinfo - print information about a struct json_number unsigned element
 *
 * given:
 *	stream	- open file stream to print on
 *	sized	- boolean indicating if the value was set
 *	value	- value to be printed as an intmax_t
 *	scomm	- comment relating to the sized boolean
 *	vcomm	- comment relating to the value as an intmax_t
 *
 * NOTE: This function does not return on error.
 */
static void
fpr_uinfo(FILE *stream, bool sized, uintmax_t value, char const *scomm, char const *vcomm)
{
    /*
     * firewall
     */
    if (stream == NULL || scomm == NULL || vcomm == NULL) {
	err(23, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * case: sized is true - value to print
     */
    fprstr(stream,"\n");
    if (sized == true) {
	fprint(stream, "\ttrue,\t\t/* %s */\n", scomm);
	fprint(stream, "\t%ju,%s/* %s */\n",
		       value,
		       (value > 999999) ? "\t" : "\t\t",
		       vcomm);

    /*
     * case: sized is false - no value to print
     */
    } else {
	fprint(stream, "\tfalse,\t\t/* %s */\n", scomm);
	fprint(stream, "\t0,\t\t/* no %s */\n", vcomm);

    }
    return;
}


/*
 * fpr_finfo - print information about a struct json_number signed element
 *
 * given:
 *	stream	- open file stream to print on
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
fpr_finfo(FILE *stream, bool sized, long double value, bool intval, char const *scomm, char const *vcomm, char const *sintval)
{
    /*
     * firewall
     */
    if (stream == NULL || scomm == NULL || vcomm == NULL || sintval == NULL) {
	err(24, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * case: sized is true - value to print
     */
    fprstr(stream,"\n");
    if (sized == true) {
	fprint(stream, "\ttrue,\t\t/* %s */\n", scomm);
	fprint(stream, "\t%.22Lg,%s/* %s */\n",
		       value,
		       (value <= -100000.0 || value >= 1000000.0) ? "\t" : "\t\t",
		       vcomm);
	fprint(stream, "\t%s,\t\t/* %s */\n", booltostr(intval), sintval);

    /*
     * case: sized is false - no value to print
     */
    } else {
	fprint(stream, "\tfalse,\t\t/* %s */\n", scomm);
	fprint(stream, "\t0,\t\t/* no %s */\n", vcomm);
	fprint(stream, "\tfalse,\t\t/* %s */\n", sintval);

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
 *	prog		our program name
 *	str		top level usage message
 *	expected	>= 0 ==> expected args, < 0 ==> ignored
 *	argc		>= 0 ==> argument count, < 0 ==> ignored
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str, int expected, int argc)
{
    /*
     * firewall
     */
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    if (argc >= 0 && expected >= 0) {
	fprintf_usage(DO_NOT_EXIT, stderr, str, expected, argc);
    } else {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JNUM_CHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
