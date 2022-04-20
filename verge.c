/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * verge - determine if first version is >= second version
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
#include <string.h>
#include <ctype.h>

/*
 * Our header file - #includes the header files we need
 */
#include "verge.h"

/*
 * definitions
 */
#define REQUIRED_ARGS (2)	/* number of required arguments on the command line */


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    int arg_cnt = 0;		/* number of args to process */
    char *ver1 = NULL;		/* first version string */
    char *ver2 = NULL;		/* second version string */
    int ver1_levels = 0;	/* number of version levels for first version string */
    int ver2_levels = 0;	/* number of version levels for second version string */
    long *vlevel1 = NULL;	/* allocated version levels from first version string */
    long *vlevel2 = NULL;	/* allocated version levels from second version string */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:V")) != -1) {
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
	    print("%s\n", VERGE_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	default:
	    usage(4, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    arg_cnt = argc - optind;
    if (arg_cnt != REQUIRED_ARGS) {
	usage(4, "two args are required", program); /*ooo*/
	not_reached();
    }
    ver1 = argv[optind];
    ver2 = argv[optind+1];
    dbg(DBG_LOW, "first version: <%s>", ver1);
    dbg(DBG_LOW, "second version: <%s>", ver2);

    /*
     * convert first version string
     */
    ver1_levels = malloc_vers(ver1, &vlevel1);
    if (ver1_levels <= 0) {
	err(2, program, "first version string is invalid"); /*ooo*/
	not_reached();
    }

    /*
     * convert second version string
     */
    ver2_levels = malloc_vers(ver2, &vlevel2);
    if (ver2_levels <= 0) {
	err(2, program, "second version string is invalid"); /*ooo*/
	not_reached();
    }

    /*
     * compare versions
     */
    for (i=0; i < ver1_levels && i < ver2_levels; ++i) {

	/*
	 * compare both versions at a given level (i)
	 */
	if (vlevel1[i] > vlevel2[i]) {

	    /* ver1 > ver2 */
	    dbg(DBG_MED, "version 1 level %d: %ld > version 2 level %d: %ld",
			  i, vlevel1[i], i, vlevel2[i]);
	    dbg(DBG_LOW, "%s > %s", ver1, ver2);
	    /* free memory */
	    if (vlevel1 != NULL) {
		free(vlevel1);
		vlevel1 = NULL;
	    }
	    if (vlevel2 != NULL) {
		free(vlevel2);
		vlevel2 = NULL;
	    }
	    /* report ver1 > ver2 */
	    exit(0); /*ooo*/

	} else if (vlevel1[i] < vlevel2[i]) {

	    /* ver1 < ver2 */
	    dbg(DBG_MED, "version 1 level %d: %ld < version 2 level %d: %ld",
			  i, vlevel1[i], i, vlevel2[i]);
	    dbg(DBG_LOW, "%s < %s", ver1, ver2);
	    /* free memory */
	    if (vlevel1 != NULL) {
		free(vlevel1);
		vlevel1 = NULL;
	    }
	    if (vlevel2 != NULL) {
		free(vlevel2);
		vlevel2 = NULL;
	    }
	    /* report ver1 < ver2 */
	    exit(1); /*ooo*/

	} else {

	    /* versions match down to this level */
	    dbg(DBG_MED, "version 1 level %d: %ld == version 2 level %d: %ld",
			  i, vlevel1[i], i, vlevel2[i]);
	}
    }
    dbg(DBG_MED, "versions match down to level: %d",
		 (ver1_levels > ver2_levels) ? ver2_levels : ver1_levels);

    /*
     * free memory
     */
    if (vlevel1 != NULL) {
	free(vlevel1);
	vlevel1 = NULL;
    }
    if (vlevel2 != NULL) {
	free(vlevel2);
	vlevel2 = NULL;
    }

    /*
     * ver1 matches ver2 to the extent that they share the same level
     *
     * The presence of sub-levels will determine the final comparison
     */
    if (ver1_levels < ver2_levels) {

	dbg(DBG_MED, "version 1 level count: %d < version level count: %d",
		     ver1_levels, ver2_levels);
	dbg(DBG_LOW, "%s < %s", ver1, ver2);
	/* report ver1 < ver2 */
	exit(1); /*ooo*/

    } else if (ver1_levels > ver2_levels) {

	dbg(DBG_MED, "version 1 level count: %d > version level count: %d",
		     ver1_levels, ver2_levels);
	dbg(DBG_LOW, "%s > %s", ver1, ver2);
	/* report ver1 > ver2 */
	exit(0); /*ooo*/

    }

    /*
     * versions match
     */
    dbg(DBG_MED, "version 1 level count: %d == version level count: %d",
		 ver1_levels, ver2_levels);
    dbg(DBG_LOW, "%s == %s", ver1, ver2);
    /* report ver1 == ver2 */
    exit(0); /*ooo*/
}


/*
 * malloc_vers - convert version string into a allocated array or version numbers
 *
 * given:
 *	ver	version string
 *	pvers	pointer to allocated array of versions
 *
 * returns:
 *	> 0  ==> number of version integers in allocated array of versions
 *	0 <= ==> string was not a valid version string,
 *		 array of versions not allocated
 *
 * NOTE: This function does not return on malloc failure or arg error.
 */
static size_t
malloc_vers(char *str, long **pvers)
{
    char *wstr = NULL;		/* working allocated copy of orig_str */
    size_t len;			/* length of version string */
    bool dot = false;		/* true ==> previous character was dot */
    size_t dot_count = 0;	/* number of .'s in version string */
    char *word = NULL;		/* token */
    char *brkt;			/* last arg for strtok_r() */
    size_t i;

    /*
     * firewall
     */
    if (str == NULL || pvers == NULL) {
	err(5, __func__, "NULL arg(s)");
	not_reached();
    }
    len = strlen(str);
    if (len <= 0) {
	dbg(DBG_MED, "version string was an empty string");
	return 0;
    }

    /*
     * duplicate str
     */
    errno = 0;		/* pre-clear errno for errp() */
    wstr = strdup(str);
    if (wstr == NULL) {
	errp(6, __func__, "cannot strdup: <%s>", str);
	not_reached();
    }

    /*
     * trim leading non-digits
     */
    for (i=0; i < len; ++i) {
	if (isascii(wstr[i]) && isdigit(wstr[i])) {
	    /* stop on first digit */
	    break;
	}
    }
    if (i == len) {
	/* report invalid version string */
	dbg(DBG_MED, "version string contained no digits: <%s>", wstr);
	return 0;
    }
    wstr += i;
    len -= i;

    /*
     * trim at and beyond any whitespace
     */
    for (i=0; i < len; ++i) {
	if (isascii(wstr[i]) && isspace(wstr[i])) {
	    wstr[i] = '\0';
	    len = i;
	    break;
	}
    }

    /*
     * trim trailing non-digits
     */
    for (i=len-1; i > 0; --i) {
	if (isascii(wstr[i]) && isdigit(wstr[i])) {
	    /* stop on first digit */
	    break;
	}
    }
    wstr[i+1] = '\0';
    len = i;

    /*
     * we now have a string that start and ends with digits
     *
     * Inspect the remaining string for digits and '.'s only.
     * Also reject string if we find more than 2 '.'s in a row.
     */
    dbg(DBG_HIGH, "trimmed version string: <%s>", wstr);
    for (i=0; i < len; ++i) {
	if (isascii(wstr[i]) && isdigit(wstr[i])) {
	    dot = false;
	} else if (wstr[i] == '.') {
	    if (dot == true) {
		/* report invalid version string */
		dbg(DBG_MED, "trimmed version string contains 2 dots in a row: <%s>", wstr);
		return 0;
	    }
	    dot = true;
	    ++dot_count;
	} else {
	    /* report invalid version string */
	    dbg(DBG_MED, "trimmed version string contains non-version character: wstr[%ju] = <%c>: <%s>",
			 (uintmax_t)i, wstr[i], wstr);
	    return 0;
	}
    }
    dbg(DBG_MED, "trimmed version string is in valid format: <%s>", wstr);
    dbg(DBG_HIGH, "trimmed version string has %ju .'s in it: <%s>", (uintmax_t)dot_count, wstr);

    /*
     * we now know the version string is in valid format: ([0-9]+\.)*[0-9]+
     *
     * Allocate the array of dot_count+1 versions.
     */
    errno = 0;		/* pre-clear errno for errp() */
    *pvers = (long *)calloc(dot_count+1+1, sizeof(long));
    if (*pvers == NULL) {
	errp(7, __func__, "cannot calloc %ju longs", (uintmax_t)dot_count+1+1);
	not_reached();
    }

    /*
     * tokenize version string using '.' delimiters
     */
    for (i=0, word=strtok_r(wstr, ".", &brkt);
         i <= dot_count && word != NULL;
	 ++i, word=strtok_r(NULL, ".", &brkt)) {
	/* word is the next version string - convert to integer */
	dbg(DBG_VVHIGH, "version level %ju word: <%s>", (uintmax_t)i, word);
	(*pvers)[i] = string_to_long(word);
	dbg(DBG_VHIGH, "version level %ju: %ld", (uintmax_t)i, (*pvers)[i]);
    }

    /*
     * return number of version levels
     */
    return dot_count+1;
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
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, VERGE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
