/*
 * cpath - canonicalize a path
 *
 * "Not all those who wander are lost."
 *
 *      -- J.R.R. Tolkien
 *
 * Copyright (c) 2025 by Landon Curt Noll.  All rights reserved.
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
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "dbg/c_bool.h"
#include "dbg/c_compat.h"
#include "dbg/dbg.h"
#else
#include <c_bool.h>
#include <c_compat.h>
#include <dbg.h>
#endif

/*
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
#include "dyn_array/dyn_array.h"
#else
#include <dyn_array.h>
#endif

/*
 * pr - stdio helper library
 */
#if defined(INTERNAL_INCLUDE)
#include "pr/pr.h"
#else
#include <pr.h>
#endif

/*
 * file_util - common utility functions for file operations
 */
#if defined(INTERNAL_INCLUDE)
#include "soup/file_util.h"
#else
#include "file_util.h"
#endif


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define CPATH_BASENAME "cpath"
#define CPATH_VERSION "1.0.0 2025-09-28"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-m max_path] [-M max_file] [-d max_depth] [-r] [-l] [-s] [path ...]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "\t-m max_path\tmax canonicalized path length, 0 ==> no limit (def: 0)\n"
    "\t-M max_file\tmax length of any canonicalized path component, 0 ==> no limit (def: 0)\n"
    "\t-d max_depth\tmax canonicalized path depth where 0 is the topdir, 0 ==> no limit (def: 0)\n"
    "\n"
    "\t-r\t\tpath must be relative (def: absolute paths allowed)\n"
    "\t-l\t\tconvert to lower case (def: don't change the path case)\n"
    "\t-s\t\trequire canonicalized path components to be safe (def: don't check)\n"
    "\n"
    "\t\t\t    safe path components match: ^[0-9A-Za-z._][0-9A-Za-z._+-]*$]\n"
    "\n"
    "\tpath ...\tcanonicalize path args (def: read paths from stdin)\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\tcanonicalized path starts with /, and -r was used\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    "    4\tcanonicalized path exceeds -m max_path limit\n"
    "    5\tcanonicalized component path exceeds -M max_file limit\n"
    "    6\tcanonicalized depth exceeds -d max_depth limit\n"
    "    7\tcanonicalized is not safe, and -s was used\n"
    "    8\tuse of .. attempted to move above topdir\n"
    "    9\tpath is empty\n"
    " >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "pr library version: %s\n";


/*
 * static globals
 */
static int exit_code = 0;		/* exit with this code */

/*
 * forward declarations
 */
static void process_sanity(char const *cpath, enum path_sanity sanity, size_t path_len, int32_t deep);
static void usage(int exitcode, char const *prog, char const *str);


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    bool opt_error = false;	/* fchk_inval_opt() return */
    size_t max_path_len = 0;	/* max canonicalized path length, 0 ==> no limit */
    size_t max_filename_len = 0; /* max length of each component of path, 0 ==> no limit */
    int32_t max_depth = 0;	/* max canonicalized path depth where 0 is the topdir, 0 ==> no limit */
    bool only_relative = false;	/* true ==> true ==> path from "/" (slash) NOT allowed, false ==> absolute paths allowed */
    bool any_case = true;	/* true ==> don't change path case, false ==> convert UPPER CASE to lower case */
    bool safe_chk = false;	/* true ==> safety test each canonical path component, false ==> don't check */
    size_t path_len = 0;     /* full path length */
    int32_t deep = 0;         /* dynamic array stack depth */
    enum path_sanity sanity = PATH_OK;	/* canon_path() error code, or PATH_OK */
    char *line = NULL;		/* stdin readline() buffer */
    ssize_t readline_ret = 0;	/* return from readline() */
    char *cpath = NULL;		/* malloced canonicalized path */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:Vm:M:d:rls")) != -1) {
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
	case 'V':		/* -V - print version and exit */
	    (void) printf("%s version: %s\n", CPATH_BASENAME, CPATH_VERSION);
	    (void) printf("libdbg version: %s\n", dbg_version);
	    (void) printf("libdyn_alloc version: %s\n", dyn_array_version);
	    (void) printf("libpr version: %s\n", pr_version);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 'm':
	    errno = 0;
	    max_path_len = (size_t) strtoumax(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -m %s into a size_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'M':
	    errno = 0;
	    max_filename_len = (size_t) strtoumax(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -M %s into a size_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'd':
	    errno = 0;
	    max_depth = (int32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -d %s into a int32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'r':
	    only_relative = true;
	    break;
	case 'l':
	    any_case = false;
	    break;
	case 's':
	    safe_chk = true;
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, program, ""); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, program, "getopt() return: %c optopt: %c", (char)i, (char)optopt);
	    }
	    break;
	}
    }
    argc -= optind;
    argv += optind;

    /*
     * debugging - print conditions
     */
    if (max_path_len > 0) {
	dbg(DBG_MED, "max canonicalized path length: %zu", max_path_len);
    } else {
	dbg(DBG_MED, "max canonicalized path length is unlimited");
    }
    if (max_filename_len > 0) {
	dbg(DBG_MED, "max length of each component: %zu", max_filename_len);
    } else {
	dbg(DBG_MED, "max length of each component is unlimited");
    }
    if (max_depth > 0) {
	dbg(DBG_MED, "max canonicalized path depth: %d", max_depth);
    } else {
	dbg(DBG_MED, "max canonicalized path depth is unlimited");
    }
    dbg(DBG_MED, "absolute paths: %s", (only_relative ? "disallowed" : "allowed"));
    dbg(DBG_MED, "conversion of paths to lower case: %s", (any_case ? "disabled" : "enabled"));
    dbg(DBG_MED, "safety test each canonical path component: %s", (safe_chk ? "enabled" : "disabled"));

    /*
     * case: no path args, read lines from stdin
     */
    if (argc <= 0) {

	/*
	 * read paths from stdin and canonicalize until EOF
	 */
	do {

	    /*
	     * read the stdin line
	     */
	    readline_ret = readline(&line, stdin);
	    if (readline_ret > 0) {
		dbg(DBG_HIGH, "readline(&line, stdin) returned: %ld bytes", readline_ret);
	    } else {
		dbg(DBG_HIGH, "readline(&line, stdin) error, returned: %ld", readline_ret);
		break;
	    }

	    /*
	     * canonicalize the line
	     */
	    cpath = canon_path(line, max_path_len, max_filename_len, max_depth,
			       &sanity, &path_len, &deep,
			       only_relative, any_case, safe_chk);

	    /*
	     * print canonicalized path and perform debugging on canon_path() results
	     */
	    process_sanity(cpath, sanity, path_len, deep);

	    /* free stdin line buffer */
	    if (line != NULL) {
		free(line);
		line = NULL;
	    }

	    /* free canonical path */
	    if (cpath != NULL) {
		free(cpath);
		cpath = NULL;
	    }

	} while (readline_ret > 0);

    /*
     * case: process command line args
     */
    } else {

	for (line = argv[0]; argc > 0; ++argv, --argc, line = argv[0]) {

	    /*
	     * canonicalize the line
	     */
	    cpath = canon_path(line, max_path_len, max_filename_len, max_depth,
			       &sanity, &path_len, &deep,
			       only_relative, any_case, safe_chk);

	    /*
	     * print canonicalized path and perform debugging on canon_path() results
	     */
	    process_sanity(cpath, sanity, path_len, deep);

	    /* free canonical path */
	    if (cpath != NULL) {
		free(cpath);
		cpath = NULL;
	    }
	}
    }

    /*
     * exit as requested
     */
    exit(exit_code); /*ooo*/
}


/*
 * process_sanity - print canonicalized path and perform debugging on canon_path() results
 *
 * given:
 *	cpath	    canonicalized path
 *	sanity	    canon_path() error code, or PATH_OK
 *	path_len    full path length
 *	deep	    dynamic array stack depth
 */
static void
process_sanity(char const *cpath, enum path_sanity sanity, size_t path_len, int32_t deep)
{

    /*
     * output based on sanity return
     */
    switch (sanity) {
    case PATH_OK:

	/* debugging - print results */
	dbg(DBG_MED, "canonicalized path length: %zu", path_len);
	dbg(DBG_MED, "canonicalized path depth: %d", deep);

	/* print canonical path on stdout */
	if (cpath == NULL) {
	    warn("%s: canon_path sanity is PATH_OK, but cpath returned NULL", CPATH_BASENAME);
	    prstr("\n");	/* print empty line if cpath is NULL */
	} else {
	    print("%s\n", cpath);
	}
	break;

    default:
	dbg(DBG_LOW, "canon_path error return: %s", path_sanity_name(sanity));
	dbg(DBG_MED, "canon_path error: %s", path_sanity_error(sanity));
	prstr("\n");	/* print empty line on canon_path error */
    }

    /*
     * if needed, set exit code
     */
    switch (sanity) {
    case PATH_OK:
	break;
    case PATH_ERR_PATH_TOO_LONG:
	exit_code = 4;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_NOT_RELATIVE:
	exit_code = 1;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_NAME_TOO_LONG:
	exit_code = 5;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_PATH_TOO_DEEP:
	exit_code = 6;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_NOT_POSIX_SAFE:
	exit_code = 7;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_DOTDOT_OVER_TOPDIR:
	exit_code = 8;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_PATH_EMPTY:
	exit_code = 9;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_PATH_IS_NULL:
	exit_code = 10;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_MALLOC:
	exit_code = 11;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_NULL_COMPONENT:
	exit_code = 12;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    case PATH_ERR_WRONG_LEN:
	exit_code = 13;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    default:
	exit_code = 14;
	warn(CPATH_BASENAME, "setting exit code to: %d, error: %s", exit_code, path_sanity_error(sanity));
	break;
    }
    return;
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "missing required argument(s), program: %s");
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
	warn(CPATH_BASENAME, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(CPATH_BASENAME, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT,
						     CPATH_BASENAME, CPATH_VERSION,
						     dyn_array_version);
    exit(exitcode); /*ooo*/
    not_reached();
}
