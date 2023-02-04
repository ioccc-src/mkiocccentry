/*
 * verge - determine if first version is greater or equal to the second version
 *
 * "Because too much minimalism can be sub-minimal." :-)
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
#include <ctype.h>

/*
 * verge - determine if first version is greater or equal to the second version
 */
#include "verge.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (2)	/* number of required arguments on the command line */

/*
 * official verge tool version
 */
#define VERGE_VERSION "1.0 2023-02-04"		/* format: major.minor YYYY-MM-DD */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] major.minor.patch-1 major.minor.patch-2\n"
    "\n"
    "\t-h\t\t\tprint help message and exit\n"
    "\t-v level\t\tset verbosity level (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "\tmajor.minor.patch-1\tfirst version  (example: 0.1.1)\n"
    "\tmajor.minor.patch-2\tsecond version (example: 1.3.2)\n"
    "\n"
    "Exit codes:\n"
    "     0   first version >= second version\n"
    "     1   first version < second version\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   command line error\n"
    "     4   first or second version string is an invalid version\n"
    "  >=10   internal error\n"
    "\n"
    "verge version: %s";


/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));

int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    int arg_count = 0;		/* number of args to process */
    char *ver1 = NULL;		/* first version string */
    char *ver2 = NULL;		/* second version string */
    int ver1_levels = 0;	/* number of version levels for first version string */
    int ver2_levels = 0;	/* number of version levels for second version string */
    intmax_t *vlevel1 = NULL;	/* allocated version levels from first version string */
    intmax_t *vlevel2 = NULL;	/* allocated version levels from second version string */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:V")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, ""); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", VERGE_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
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
	usage(3, program, "two args are required"); /*ooo*/
	not_reached();
    }
    ver1 = argv[optind];
    ver2 = argv[optind+1];
    dbg(DBG_LOW, "first version: <%s>", ver1);
    dbg(DBG_LOW, "second version: <%s>", ver2);

    /*
     * convert first version string
     */
    ver1_levels = (int)allocate_vers(ver1, &vlevel1);
    if (ver1_levels <= 0) {
	err(4, program, "first version string is invalid"); /*ooo*/
	not_reached();
    }

    /*
     * convert second version string
     */
    ver2_levels = (int)allocate_vers(ver2, &vlevel2);
    if (ver2_levels <= 0) {
	err(4, program, "second version string is invalid"); /*ooo*/
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
	    dbg(DBG_MED, "version 1 level %d: %jd > version 2 level %d: %jd",
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
	    dbg(DBG_MED, "version 1 level %d: %jd < version 2 level %d: %jd",
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
	    dbg(DBG_MED, "version 1 level %d: %jd == version 2 level %d: %jd",
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
 * allocate_vers - convert version string into a allocated array or version numbers
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
 * NOTE: This function does not return on allocation failures or NULL args.
 */
static size_t
allocate_vers(char *str, intmax_t **pvers)
{
    char *wstr = NULL;		/* working allocated copy of orig_str */
    char *wstr_start = NULL;	/* pointer to starting point of wstr */
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
	err(10, __func__, "NULL arg(s)");
	not_reached();
    }
    len = strlen(str);
    if (len <= 0) {
	dbg(DBG_MED, "version string is empty");
	return 0;
    }

    /*
     * duplicate str
     */
    errno = 0;		/* pre-clear errno for errp() */
    wstr = strdup(str);
    if (wstr == NULL) {
	errp(11, __func__, "cannot strdup: <%s>", str);
	not_reached();
    }
    wstr_start = wstr;

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
	/* free strdup()d string if != NULL */
	if (wstr_start != NULL) {
	    free(wstr_start);
	    wstr_start = NULL;
	}
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
     * we now have a string that starts and ends with digits
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
		/* free strdup()d string if != NULL */
		if (wstr_start != NULL) {
		    free(wstr_start);
		    wstr_start = NULL;
		}
		return 0;
	    }
	    dot = true;
	    ++dot_count;
	} else {
	    /* report invalid version string */
	    dbg(DBG_MED, "trimmed version string contains non-version character: wstr[%ju] = <%c>: <%s>",
			 (uintmax_t)i, wstr[i], wstr);
	    /* free strdup()d string if != NULL */
	    if (wstr_start != NULL) {
		free(wstr_start);
		wstr_start = NULL;
	    }
	    return 0;
	}
    }
    dbg(DBG_MED, "trimmed version string is in valid format: <%s>", wstr);
    dbg(DBG_HIGH, "trimmed version string has %ju '.'s in it: <%s>", (uintmax_t)dot_count, wstr);

    /*
     * we now know the version string is in valid format: ([0-9]+\.)*[0-9]+
     *
     * Allocate the array of dot_count+1 versions.
     */
    errno = 0;		/* pre-clear errno for errp() */
    *pvers = (intmax_t *)calloc(dot_count+1+1, sizeof(intmax_t));
    if (*pvers == NULL) {
	errp(12, __func__, "cannot calloc %ju intmax_ts", (uintmax_t)dot_count+1+1);
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
	(void) string_to_intmax(word, &(*pvers)[i]);
	dbg(DBG_VHIGH, "version level %ju: %jd", (uintmax_t)i, (*pvers)[i]);
    }

    /* we no longer need the duplicated string */
    if (wstr_start != NULL) {
	free(wstr_start);
	wstr_start = NULL;
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
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, VERGE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
