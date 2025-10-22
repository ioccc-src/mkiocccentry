/*
 * try_fts_walk - walk a directory tree using one of the static walk_rule sets
 *
 * "This is a serious journey, not a hobbit walking-party."
 *
 *	-- Gandalf to Pippin, the Fellowship of the Ring.
 *
 * Copyright (c) 2025 by Landon Curt Noll.  All Rights Reserved.
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
#include <unistd.h>
#include <libgen.h>

/*
 * location - location/country codes and set the IOCCC locale
 */
#include "../soup/location.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "../soup/limit_ioccc.h"

/*
 * walk - walk directory trees and tar listings
 */
#include "../soup/walk.h"


/*
 * definitions
 */
#define TRY_FTS_WALK_BASENAME "try_fts_walk"
/**/
#define TOPDIR "."		/* default topdir */


/*
 * walk_set numbers
 */
enum walk_set_num {
    WALK_SET_UNSET = -1,	/* not valid set, or walk_set number unset */
    WALK_SET_ALL = 0,		/* try all sets */
    WALK_SET_MKIOCCCENTRY = 1,	/* mkiocccentry walk_set */
    WALK_SET_TXZCHK = 2,	/* txzchk walk_set */
    WALK_SET_CHKSUBMIT = 3,	/* chksubmit walk_set */
    WALK_SET_CHKENTRY_CAPS = 4,	/* chkentry -S walk_set */
    WALK_SET_CHKENTRY_S = 5,	/* chkentry -s walk_set */
    WALK_SET_CHKENTRY_W = 6,	/* chkentry -w walk_set */
};
#define WALK_SET_DEFAULT (1)	/* default walk set number */

/*
 * type of path to simulate
 */
enum path_type {
    PATH_TYPE_UNSET = '?',	/* not valid type, or path type not set */
    PATH_TYPE_FILE = 'f',	/* simulate regular file */
    PATH_TYPE_DIR = 'd',	/* simulate directory */
    PATH_TYPE_SYMLINK = 's',	/* simulate symlink */
    PATH_TYPE_OTHER = 'o',	/* simulate other type (will simulate named pipe (fifo) */
};


/*
 * globals
 */
static char *program = "try_walk_set";		    /* our name */

/*
 * usage message
 */
static char const * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-q] [-T] [-o]\n"
    "\t\t\t[-m max_path] [-M max_depth] [-d max_depth]\n"
    "\t\t\t[-F max_file] [-D max_dir] [-S max_sym] [-O max_other] [-s set] topdir\n"
    "\n"
    "\t-h\t\tPrint help message and exit\n"
    "\t-v level\tSet verbosity level: (def level: 0)\n"
    "\t-V\t\tPrint version string and exit\n"
    "\t-q\t\tQuiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )\n"
    "\n"
    "\t-o\t\tkeep paths in original order (def: sort dynamic arrays by canonicalized path)\n"
    "\t-T\t\tprocess as a tarball listing\n"
    "\n"
    "\t-m max_path\tmax canonicalized path length, 0 ==> no limit (def: %d)\n"
    "\t-M max_file\tmax length of any canonicalized path component, 0 ==> no limit (def: %d)\n"
    "\t-d max_depth\tmax canonicalized path depth where 0 is the topdir, 0 ==> no limit (def: %d)\n"
    "\n"
    "\t-F max_file\tmax counted (non-free) files, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\t-D max_dir\tmax counted (non-free) directories, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\t-S max_sym\tmax counted (non-free) symlinks, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\t-O max_other\tmax counted (non-free) non-file/dir/symlinks, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\n"
    "\t-s set\t\twalk_set number to try (def: %d)\n"
    "\t\t\t    l - list sets and exit\n"
    "\t\t\t    0 - try all sets\n"
    "\t\t\t    1 - mkiocccentry walk_set\n"
    "\t\t\t    2 - txzchk walk_set\n"
    "\t\t\t    3 - chksubmit walk_set\n"
    "\t\t\t    4 - chkentry -S walk_set\n"
    "\t\t\t    5 - chkentry -s walk_set\n"
    "\t\t\t    6 - chkentry -w walk_set\n"
    "\n"
    "\ttopdir\tdirectory tree to walk\n"
    "\n"
    "Exit codes:\n"
    "     0   all is OK\n"
    "     1   some walk erorrs detected\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   invalid command line, invalid option or option missing an argument\n"
    "     4   invalid set number\n"
    "     5   invalid type letter\n"
    "     6   invalid size\n"
    "     7   canonicalization of path failed\n"
    " >= 10   internal error\n"
    "\n"
    "%s version: %s\n";

/*
 * static functions
 */
static void usage(int exitcode, char const *str) __attribute__((noreturn));
static bool test_walk(FILE *out,
		      char const *topdir, struct walk_set *set, char *context,
		      size_t max_path_len, size_t max_filename_len, int32_t max_depth,
		      int32_t max_file, int32_t max_dir, int32_t max_sym, int32_t max_other,
		      bool sort_cpath);


int
main(int argc, char *argv[])
{
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    /**/
    char *topdir = TOPDIR;			    /* simulated topdir */
    /**/
    size_t max_path_len = MAX_PATH_LEN;		    /* max canonicalized path length, 0 ==> no limit */
    size_t max_filename_len = MAX_FILENAME_LEN;	    /* max length of each component of path, 0 ==> no limit */
    int32_t max_depth = MAX_PATH_DEPTH;		    /* max canonicalized path depth where 0 is the topdir, 0 ==> no limit */
    bool sort_cpath = true;			    /* true ==> dynamic arrays by canonicalized path, false ==> do not sort */
    /**/
    int32_t max_file = MAX_EXTRA_FILE_COUNT;	    /* max counted files, 0 ==> unlimited, -1 ==> none */
    int32_t max_dir = MAX_EXTRA_DIR_COUNT;	    /* max counted directories, 0 ==> unlimited, -1 ==> none */
    int32_t max_sym = ANY_COUNTED;		    /* max counted symlinks, 0 ==> unlimited, -1 ==> none */
    int32_t max_other = NO_COUNTED;		    /* max counted non-file/dir/symlinks, 0 ==> unlimited, -1 ==> none */
    /**/
    bool opt_error = false;		/* fchk_inval_opt() return */
    int set_num = WALK_SET_DEFAULT;	/* set arg as a number */
    bool walk_ok = true;		/* true ==> no errors found, false ==> some errors found */
    int exit_code = 0;			/* exit with this code */
    int i;

    /* IOCCC requires use of C locale */
    set_ioccc_locale();

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:VqoTm:M:d:s:F:D:S:O:")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    usage(2, NULL);
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':	/* -V - print version and exit */
            print("%s version: %s\n", TRY_FTS_WALK_BASENAME, TRY_FTS_WALK_VERSION);
            exit(2); /*ooo*/
            not_reached();
            break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'o':
	    sort_cpath = false;
	    break;
	case 'T':
	    err(3, program, "-T used, however tarball list processing it NOT yet supported"); /*ooo*/
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
	case 's':
	    if (strcmp(optarg, "l") == 0) {
		(void) printf("Allowed Walk Sets\n"
			      "\n"
			      "0 - try all sets\n"
			      "1 - mkiocccentry walk_set\n"
			      "2 - txzchk walk_set\n"
			      "3 - chksubmit walk_set\n"
			      "4 - chkentry -S walk_set\n"
			      "5 - chkentry -s walk_set\n"
			      "6 - chkentry -w walk_set\n");
		exit(0); /*ooo*/
		break;
	    }
	    errno = 0;
	    set_num = (int) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, __func__, "cannot convert set: %s into a set number", optarg);
		not_reached();
	    }
	    break;
	case 'F':
	    errno = 0;
	    max_file = (int32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -F %s into a int32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'D':
	    errno = 0;
	    max_dir = (int32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -S %s into a int32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'S':
	    errno = 0;
	    max_sym = (int32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -S %s into a int32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'O':
	    errno = 0;
	    max_depth = (int32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -O %s into a int32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, "invalid command line option"); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, __func__, "getopt() return: %c optopt: %c", (char)i, (char)optopt);
	    }
	    break;
	}
    }
    /* advance command line beyond options */
    argc -= optind;
    argv += optind;
    /* must have 3 args */
    if (argc != 1) {
	warn(__func__, "expected 1 arg, found: %d", argc);
	usage(3, NULL); /*ooo*/
	not_reached();
    }
    topdir = argv[0];
    /* parse set_num */
    switch (set_num) {
    case WALK_SET_ALL:
	dbg(DBG_MED, "set number WALK_SET_ALL: %d", set_num);
	break;
    case WALK_SET_MKIOCCCENTRY:
	dbg(DBG_MED, "set number WALK_SET_MKIOCCCENTRY: %d", set_num);
	break;
    case WALK_SET_TXZCHK:
	dbg(DBG_MED, "set number WALK_SET_TXZCHK: %d", set_num);
	break;
    case WALK_SET_CHKSUBMIT:
	dbg(DBG_MED, "set number WALK_SET_CHKSUBMIT: %d", set_num);
	break;
    case WALK_SET_CHKENTRY_CAPS:
	dbg(DBG_MED, "set number WALK_SET_CHKENTRY_CAPS: %d", set_num);
	break;
    case WALK_SET_CHKENTRY_S:
	dbg(DBG_MED, "set number WALK_SET_CHKENTRY_S: %d", set_num);
	break;
    case WALK_SET_CHKENTRY_W:
	dbg(DBG_MED, "set number WALK_SET_CHKENTRY_W: %d", set_num);
	break;
    default:
	err(4,  __func__, "unknown set number: %d", set_num); /*ooo*/
	not_reached();
    }

    /*
     * mkiocccentry walk_set WALK_SET_MKIOCCCENTRY or WALK_SET_ALL
     */
    if (set_num == WALK_SET_MKIOCCCENTRY || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_MKIOCCCENTRY */
	walk_ok = test_walk(stdout,
			    topdir, &walk_mkiocccentry, "mkiocccentry",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * txzchk walk_set WALK_SET_TXZCHK or WALK_SET_ALL
     */
    if (set_num == WALK_SET_TXZCHK || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_TXZCHK */
	walk_ok = test_walk(stdout,
			    topdir, &walk_txzchk, "txzchk",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * chksubmit walk_set WALK_SET_CHKSUBMIT or WALK_SET_ALL
     */
    if (set_num == WALK_SET_CHKSUBMIT || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_TXZCHK */
	walk_ok = test_walk(stdout,
			    topdir, &walk_chksubmit, "chksubmit",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * chkentry -S walk_set WALK_SET_CHKENTRY_CAPS or WALK_SET_ALL
     */
    if (set_num == WALK_SET_CHKENTRY_CAPS || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_CHKENTRY_CAPS */
	walk_ok = test_walk(stdout,
			    topdir, &walk_chkentry_S, "chkentry -S",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * chkentry -s walk_set WALK_SET_CHKENTRY_S or WALK_SET_ALL
     */
    if (set_num == WALK_SET_CHKENTRY_S || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_CHKENTRY_S */
	walk_ok = test_walk(stdout,
			    topdir, &walk_chkentry_s, "chkentry -s",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * chkentry -w walk_set (WALK_SET_CHKENTRY_W) or WALK_SET_ALL
     */
    if (set_num == WALK_SET_CHKENTRY_W || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_CHKENTRY_S */
	walk_ok = test_walk(stdout,
			    topdir, &walk_chkentry_w, "chkentry -w",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(exit_code); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(2, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message if str is non-NULL and not an empty string
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *str)
{
    /*
     * print the formatted usage stream
     */
    if (str != NULL && str[0] != '\0') {
	(void) fprintf(stderr, "%s: %s\n", TRY_FTS_WALK_BASENAME, str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, TRY_FTS_WALK_BASENAME,
			    MAX_PATH_LEN, MAX_FILENAME_LEN, MAX_PATH_DEPTH,
			    ANY_COUNTED, NO_COUNTED, MAX_EXTRA_FILE_COUNT,
			    ANY_COUNTED, NO_COUNTED, MAX_EXTRA_DIR_COUNT,
			    ANY_COUNTED, NO_COUNTED, ANY_COUNTED,
			    ANY_COUNTED, NO_COUNTED, NO_COUNTED,
			    WALK_SET_DEFAULT,
			    TRY_FTS_WALK_BASENAME, TRY_FTS_WALK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * test_walk - test walking a directory tree while recording steps in a walk_set
 *
 * given:
 *	out		    - stream to print walk_stat details on
 *
 *	topdir		    - simulated topdir
 *	set		    - pointer to walk_set being used
 *	context		    - context (tool and options) for debugging purposes
 *
 *      max_path_len        - max canonicalized path length, 0 ==> no limit
 *      max_filename_len    - max length of each component of path, 0 ==> no limit
 *      max_depth           - max depth of subdirectory tree, 0 ==> no limit, <0 ==> reserved for future use
 *
 *	max_file	    - max counted files, 0 ==> unlimited, -1 ==> none
 *	max_dir		    - max counted directories, 0 ==> unlimited, -1 ==> none
 *	max_sym		    - max counted symlinks, 0 ==> unlimited, -1 ==> none
 *	max_other	    - max counted non-file/dir/symlinks, 0 ==> unlimited, -1 ==> none
 *
 *      sort_cpath	    - true ==> dynamic arrays by canonicalized path, false ==> do not sort
 *
 * returns:
 *	true ==> no walk errors found
 *	false ==> some walk errors found
 */
static bool
test_walk(FILE *out,
	  char const *topdir, struct walk_set *set, char *context,
	  size_t max_path_len, size_t max_filename_len, int32_t max_depth,
	  int32_t max_file, int32_t max_dir, int32_t max_sym, int32_t max_other,
	  bool sort_cpath)
{
    struct walk_stat wstat;	    /* walk_stat being processed */
    bool walk_ok = true;	    /* true ==> no walk errors found, false ==> some walk errors found */

    /*
     * firewall
     */
    if (out == NULL) {
	err(4,  __func__, "out is NULL");
	not_reached();
    }
    if (set == NULL) {
	err(5,  __func__, "set is NULL");
	not_reached();
    }
    if (context == NULL) {
	err(6,  __func__, "context is NULL");
	not_reached();
    }
    if (topdir == NULL) {
	err(7,  __func__, "topdir is NULL");
	not_reached();
    }

    /*
     * initialize a walk_stat
     */
    init_walk_stat(&wstat, topdir, set, context,
		   max_path_len, max_filename_len, max_depth,
		   false);

    /*
     * walk a file system tree, recording steps
     */
    walk_ok = fts_walk(&wstat);
    if (walk_ok == false) {
	dbg(DBG_MED, "%s: fts_walk() failed", __func__);
	return false;
    }

    /*
     * sort walk_stat arrays by canonicalized path in a case independent way unless -o
     */
    if (sort_cpath) {
	dbg(DBG_MED, "%s: sorting walk_stat arrays by canonicalized path", __func__);
	sort_walk_istat(&wstat);
    }

    /*
     * print walk_stat details on stdout if at least -v 1
     */
    if (dbg_allowed(DBG_HIGH)) {
	fprintf_walk_stat(out, &wstat);
    }

    /*
     * end talk and check if the walk was successful
     */
    walk_ok = chk_walk(&wstat, stderr, max_file, max_dir, max_sym, max_other, true);

    /*
     * free the walk_stat
     */
    free_walk_stat(&wstat);

    /*
     * exit if walk was not successful
     */
    if (walk_ok) {
	dbg(DBG_LOW, "%s walk was successful for: %s", context, topdir);
    } else {
	dbg(DBG_LOW, "%s some walk errors were detected for: %s", context, topdir);
    }
    return walk_ok;
}
