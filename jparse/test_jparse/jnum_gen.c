/*
 * jnum_gen - generate JSON number string conversion test data
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
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
#include <math.h>
#include <stdint.h>
#include <inttypes.h>
#include <locale.h>

/*
 * jnum_gen - generate JSON number string conversion test data
 */
#include "jnum_gen.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command readline_buf */
#define CHUNK (16)		/* allocate CHUNK elements at a time */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-q] filename\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version strings and exit\n"
    "\t-q\t\tset quiet mode (def: loud :-) )\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\n"
    "\tfilename\tfile containing JSON number test cases\n"
    "\n"
    "Exit codes:\n"
    "\t0\t\tall is OK\n"
    "\t1\t\tfilename does not exist or is not a readable file\n"
    "\t2\t\t-h and help string printed or -V and version strings printed\n"
    "\t3\t\tcommand line error\n"
    "\t>=10\t\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";

/*
 * globals
 */
static bool quiet = false;			/* true ==> quiet mode */

/*
 * forward declarations
 */
static bool is_file(char const *path);
static bool is_read(char const *path);
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    int arg_count = 0;		/* number of args to process */
    char *filename = NULL;	/* name of file containing test cases */
    FILE *stream = NULL;	/* test case open stream */
    char *readline_buf = NULL;	/* test case readline_buf buffer */
    int ret;			/* libc function return */
    intmax_t count = 0;		/* test count read from filename */
    char *p = NULL;		/* comment search */
    char *line = NULL;		/* allocated line with JSON number and/or comment */
    size_t len;			/* length of JSON test number */
    char *first = NULL;		/* start of JSON test number */
    struct json *node = NULL;	/* parsed JSON number */
    struct dyn_array *str_array = NULL;		/* dynamic array of test strings */
    struct dyn_array *result_array = NULL;	/* dynamic array of test results */
    bool moved = false;		/* true ==> realloc() moved data */
    intmax_t linenum = 0;	/* readline_buf number from filename */
    int i;

    /*
     * use default locale based on LANG
     */
    (void) setlocale(LC_ALL, "");

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:Vq")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
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
	case 'V':		/* -V - print version strings and exit */
	    print("%s version: %s\n", JNUM_GEN_BASENAME, JNUM_GEN_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
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
    arg_count = argc - optind;
    if (arg_count != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
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
	errp(10, program, "error in fopen(%s, \"r\")", filename);
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
    while (readline(&readline_buf, stream) >= 0) {

	/*
	 * form line from readline buffer
	 */
	errno = 0;		/* pre-clear errno for errp() */
	line = strdup(readline_buf);
	if (line == NULL) {
	    errp(13, program, "strdup of readline buffer failed");
	    not_reached();
	}

	/*
	 * trim comments from line
	 */
	++linenum;
	dbg(DBG_VVHIGH, "linenum %jd: <%s>", linenum, line);
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
	    dbg(DBG_VHIGH, "only comment and/or whitespace found on line: %jd", linenum);
	    continue;
	}

	/*
	 * save the test string for printing later
	 */
	moved = dyn_array_append_value(str_array, &line);
	if (moved == true) {
	     dbg(DBG_VHIGH, "FYI: dyn_array_append_value(str_array, readline_buf) realloc moved data, "
			    "count: %jd readline_buf: %jd", count, linenum);
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
			    "count: %jd readline_buf: %jd", count, linenum);
	}
	++count;	/* count another test */
    }

    /*
     * firewall - dynamic array size sanity check
     */
    dbg(DBG_MED, "found %jd test cases in filename: %s", count, filename);
    if (dyn_array_tell(str_array) != count) {
	err(15, program, "expected %jd pointers, found %jd in str_array", count, dyn_array_tell(str_array));
	not_reached();
    }
    if (dyn_array_tell(result_array) != count) {
	err(16, program, "expected %jd pointers, found %jd in result_array", count, dyn_array_tell(result_array));
	not_reached();
    }

    /*
     * close filename
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fclose(stream);
    if (ret < 0) {
	errp(17, program, "error in fclose");
	not_reached();
    }

    /*
     * output test count
     */
    print("#define TEST_COUNT (%jd)\n\n", count);
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
	    err(19, program, "node->type for test %d: %s != %s", i, json_type_name(node->type),
		    json_type_name(JTYPE_NUMBER));
	    not_reached();
	}
	fpr_number(stdout, &node->item.number);

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
    str_array = NULL;

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
    result_array = NULL;

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(0); /*ooo*/
}


/* NOTE: The following function is a static duplicate from from mkicccentry toolkit */
/*
 * is_file - if a path is a file
 *
 * This function tests if a path exists and is a regular file.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a regular file,
 *      false ==> path does not exist OR is not a regular file
 */
static bool
is_file(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(20, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);

    /*
     * test if path is a regular file
     */
    if (!S_ISREG(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a regular file", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a regular file", path);
    return true;
}


/* NOTE: The following function is a static duplicate from from mkicccentry toolkit */
/*
 * is_read - if a path is readable
 *
 * This function tests if a path exists and we have permissions to read it.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and we have read access,
 *      false ==> path does not exist OR is not read OR
 *                we don't have permission to read it
 */
static bool
is_read(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(21, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, R_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not readable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is readable", path);
    return true;
}


/* NOTE: The following function is a static duplicate from from mkicccentry toolkit */
/*
 * fpr_number - print the contents of struct json_number on a stream
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
	err(22, __func__, "stream is NULL");
	not_reached();
    }
    if (item == NULL) {
	err(23, __func__, "item is NULL");
	not_reached();
    }

    /*
     * print bool parsed and converted
     */
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> able to parse JSON number string */\n",
		   booltostr(item->parsed));
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> able to convert JSON number string to some form of C value */\n",
		   booltostr(item->converted));
    fprstr(stream, "\n");

    /*
     * print JSON string
     */
    fprint(stream, "\t\"%s\",%s"
		   "/* allocated copy of the original allocated JSON number, NUL terminated */\n",
		   item->as_str,
		   (item->as_str_len <= 4 ? "\t\t" : "\t"));
    fprint(stream, "\t\"%s\",%s"
		   "/* first whitespace character */\n",
		   item->first,
		   (item->number_len <= 4 ? "\t\t" : "\t"));
    fprstr(stream, "\n");

    /*
     * print JSON string lengths
     */
    fprint(stream, "\t%ju,\t"
		   "\t/* length of as_str */\n",
		   (uintmax_t)item->as_str_len);
    fprint(stream, "\t%ju,\t"
		   "\t/* length of JSON number, w/o leading or trailing whitespace and NUL bytes */\n",
		   (uintmax_t)item->number_len);
    fprstr(stream, "\n");

    /*
     * print bool is_negative
     */
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> value < 0 */\n",
		   booltostr(item->is_negative));
    fprstr(stream, "\n");

    /*
     * print bool is_floating and is_e_notation and is_integer
     */
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> as_str had a '.' in it such as 1.234, false ==> no '.' found */\n",
		   booltostr(item->is_floating));
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> e notation used such as 1e10, false ==> no e notation found */\n",
		   booltostr(item->is_e_notation));
    fprint(stream, "\t%s,\t"
		   "\t/* true ==> integer conversion success, false ==> no integer conversion */\n",
		   booltostr(item->is_integer));
    fprstr(stream, "\n");

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
    fprstr(stream, "\n");
    fprstr(stream, "\t/* floating point values */\n");

    /*
     * print float info
     */
    fpr_finfo(stream, item->float_sized, (long double)item->as_float, item->as_float_int,
	   "true ==> converted JSON floating point to C float",
	   "JSON floating point value in float form",
	   "if float_sized == true, true ==> as_float is an integer", "");

    /*
     * print double info
     */
    fpr_finfo(stream, item->double_sized, (long double)item->as_double, item->as_double_int,
	    "true ==> converted JSON floating point to C double",
	    "JSON floating point value in double form",
	    "if double_sized == true, true ==> as_double is an integer", "");

    /*
     * print long double info
     */
    fpr_finfo(stream, item->longdouble_sized, item->as_longdouble, item->as_float_int,
	   "true ==> converted JSON floating point to C long double",
	   "JSON floating point value in long double form",
	   "if float_sized == true, true ==> as_float is an integer", "L");
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
	err(24, __func__, "NULL arg(s)");
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
	err(25, __func__, "NULL arg(s)");
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
fpr_finfo(FILE *stream, bool sized, long double value, bool intval, char const *scomm, char const *vcomm, char const *sintval,
	char const *suffix)
{
    /*
     * firewall
     */
    if (stream == NULL || scomm == NULL || vcomm == NULL || sintval == NULL || suffix == NULL) {
	err(26, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * case: sized is true - value to print
     */
    fprstr(stream,"\n");
    if (sized == true) {
	fprint(stream, "\ttrue,\t\t/* %s */\n", scomm);
	fprint(stream, "\t%.22Lg%s,%s/* %s */\n",
		       value, suffix,
		       (islessequal(value, -100000.0L) ||
			isgreaterequal(value, 1000000.0L)) ? "\t" : "\t\t",
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
    if (prog == NULL) {
	prog = JNUM_GEN_BASENAME;
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JNUM_GEN_BASENAME, JNUM_GEN_VERSION,
	    JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
