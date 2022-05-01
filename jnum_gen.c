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
	    print("%s\n", JNUM_CHK_VERSION);
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
	err(10, program, "dyn_array_create() for str_array returned NULL");
	not_reached();
    }

    /*
     * create dynamic arrays for test results
     */
    result_array = dyn_array_create(sizeof(struct json *), CHUNK, CHUNK, true);
    if (result_array == NULL) {
	err(10, program, "dyn_array_create() for result_array returned NULL");
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
	    err(10, program, "strdup of readline buffer failed");
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
	    err(10, program, "json_conv_number() returned NULL");
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
	err(10, program, "expected %jd pointers, found %jd in str_array", (intmax_t)count, dyn_array_tell(str_array));
	not_reached();
    }
    if (dyn_array_tell(result_array) != count) {
	err(10, program, "expected %jd pointers, found %jd in result_array", (intmax_t)count, dyn_array_tell(result_array));
	not_reached();
    }

    /*
     * close filename
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fclose(stream);
    if (ret < 0) {
	err(10, program, "error in fclose");
	not_reached();
    }

    /* XXX - add code here to print test output - XXX */

    /*
     * free readline_buf buffers
     */
    for (i=0; i < count; ++i) {
	readline_buf = dyn_array_value(str_array, char *, i);
	if (readline_buf != NULL) {
	    free(readline_buf);
	    readline_buf = NULL;
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
