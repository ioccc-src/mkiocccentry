/*
 * pr_test - test the stdio helper library
 *
 * "Small acts of kindness can fill the world with light."
 *
 *	-- J.R.R. Tolkien
 *
 * Copyright (c) 2008-2025 by Landon Curt Noll and Cody Boone Ferguson.  All rights reserved.
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
 * The code in this from repo was copied out of the mkiocccentry toolkit repo:
 *
 *	https://github.com/ioccc-src/mkiocccentry
 *
 * and out of the jparse repo:
 *
 *	https://github.com/xexyl/jparse
 *
 * The origin of libpr dates back to code written by Landon Curt Noll around 2008.
 *
 * That 2008 code was copied into the jparse repo, and the mkiocccentry toolkit repo
 * by Landon Curt Noll.  While in the jparse repo, both Landon Curt Noll and
 * Cody Boone Ferguson added to and improved this code base.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

/*
 * libpr functions
 */
#include "pr.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define PR_TEST_BASENAME "pr_test"
#define PR_TEST_VERSION "1.1.1 2025-09-23"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\ttest suite failed\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    " >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "pr library version: %s\n";

const char *const pr_version = PR_VERSION;    /* library version format: major.minor YYYY-MM-DD */

/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str);

int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    bool opt_error = false;	/* fchk_inval_opt() return */
    int i;
    char *buf = NULL;           /* for read_all() test */
    char const *dirname = "foo";
    char const *filename = "bar";
    char *relpath = "foo";
    size_t comps = 0;                   /* number of directory components */
    char *name = NULL;                  /* for various tests */
    enum path_sanity sanity;
    size_t bytes = 0;
    struct stat in_st;                  /* verify copyfile() sets correct modes */
    struct stat out_st;                 /* output file stat(2) */
    bool dir_exists = false;            /* true ==> directory already exists (for testing modes) */
    FTSENT *ent = NULL;                 /* to test read_fts() */
    char *fname = NULL;           /* to test find_path() */
    int cwd = -1;                       /* to restore after read_fts() test */
    struct dyn_array *paths = NULL;     /* to test find_paths() */
    struct dyn_array *paths_found = NULL;   /* to test find_paths() */
    uintmax_t len = 0; /* length of arrays */
    uintmax_t j = 0; /* for arrays */
    intmax_t idx = 0; /* for find_path_in_array() */
    struct fts fts; /* for read_fts(), find_path() and find_paths() */
    off_t size1 = 0; /* for checking size_if_file() */
    off_t size2 = 0; /* for checking size_if_file() */


    /*
     * use default locale based on LANG
     */
    (void) setlocale(LC_ALL, "");

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
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    (void) printf("%s version: %s\n", PR_TEST_BASENAME, PR_TEST_VERSION);
	    (void) printf("libpr version: %s\n", pr_version);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, program, ""); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, __func__, "getopt() return: %c optopt: %c", (char)i, (char)optopt);
	    }
	    break;
	}
    }
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /*
     * before we do anything else reset fts!
     *
     * NOTE: on first use this will not touch any pointers and ONLY set them to
     * NULL so this MUST be called prior to using the FTS functions!
     *
     * NOTE: make SURE you call memset(&fts, 0, sizeof(struct fts)) first!
     */
    memset(&fts, 0, sizeof(struct fts));
    reset_fts(&fts, true, true); /* true, true: clear out ignore and match lists */

    /*
     * report on dbg state, if debugging
     */
    fdbg(stderr, DBG_MED, "verbosity_level: %d", verbosity_level);
    fdbg(stderr, DBG_MED, "msg_output_allowed: %s", booltostr(msg_output_allowed));
    fdbg(stderr, DBG_MED, "dbg_output_allowed: %s", booltostr(dbg_output_allowed));
    fdbg(stderr, DBG_MED, "warn_output_allowed: %s", booltostr(warn_output_allowed));
    fdbg(stderr, DBG_MED, "err_output_allowed: %s", booltostr(err_output_allowed));
    fdbg(stderr, DBG_MED, "usage_output_allowed: %s", booltostr(usage_output_allowed));
    fdbg(stderr, DBG_MED, "msg_warn_silent: %s", booltostr(msg_warn_silent));
    fdbg(stderr, DBG_MED, "msg() output: %s", msg_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "dbg(DBG_MED) output: %s", dbg_allowed(DBG_MED) ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "warn() output: %s", warn_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "err() output: %s", err_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "usage() output: %s", usage_allowed() ? "allowed" : "silenced");

    errno = 0; /* pre-clear errno for errp() */
    buf = calloc_path(dirname, filename);
    if (buf == NULL) {
	errp(28, __func__, "calloc_path(\"%s\", \"%s\") returned NULL", dirname, filename);
	not_reached();
    } else if (strcmp(buf, "foo/bar") != 0) {
	err(29, __func__, "buf: %s != %s/%s", buf, dirname, filename);
	not_reached();
    } else {
	fdbg(stderr, DBG_MED, "calloc_path(\"%s\", \"%s\"): returned %s", dirname, filename, buf);
    }

    /*
     * free buf
     */
    if (buf != NULL) {
	free(buf);
	buf = NULL;
    }

    /*
     * try calloc_path() again but without directory
     */
    dirname = NULL;
    errno = 0; /* pre-clear errno for errp() */
    buf = calloc_path(NULL, filename);
    if (buf == NULL) {
	errp(30, __func__, "calloc_path(NULL, \"%s\") returned NULL", filename);
	not_reached();
    } else if (strcmp(buf, "bar") != 0) {
	err(31, __func__, "buf: %s != %s", buf, filename);
	not_reached();
    } else {
	fdbg(stderr, DBG_MED, "calloc_path(NULL, \"%s\"): returned %s", filename, buf);
    }

    if (buf != NULL) {
	free(buf);
	buf = NULL;
    }

    /*
     * test the first relative path that we know is good
     */
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_OK) {
        err(32, __func__, "sane_relative_path(\"%s\", 99, 25, 4, fale): expected PATH_OK, got: %s",
                relpath, path_sanity_name(sanity)); /*coo*/
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_OK", relpath);
    }

    /*
     * test another sane path but with components
     */
    relpath = "foo/bar";
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_OK) {
        err(33, __func__, "sane_relative_path(\"%s\", 99, 25, 4, false): expected PATH_OK, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_OK", relpath);
    }

    /*
     * test empty path
     */
    relpath = "";
    sanity = sane_relative_path(relpath, 99, 25, 2, false);
    if (sanity != PATH_ERR_PATH_EMPTY) {
        err(34, __func__, "sane_relative_path(\"%s\", 99, 25, 2, false): expected PATH_ERR_PATH_EMPTY, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 2, false) == PATH_ERR_PATH_EMPTY", relpath);
    }

    /*
     * test path too long (path length > max_path_len)
     */
    relpath = "foo/bar/baz";
    sanity = sane_relative_path(relpath, 2, 99, 2, false);
    if (sanity != PATH_ERR_PATH_TOO_LONG) {
        err(35, __func__, "sane_relative_path(\"%s\", 2, 99, 2, false): expected PATH_ERR_PATH_TOO_LONG, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 2, 99, 2, false) == PATH_ERR_PATH_TOO_LONG", relpath);
    }

    /*
     * test max path len <= 0
     */
    relpath = "foo/bar/baz";
    sanity = sane_relative_path(relpath, 0, 25, 2, false);
    if (sanity != PATH_ERR_MAX_PATH_LEN_0) {
        err(36, __func__, "sane_relative_path(\"%s\", 0, 25, 2, false): expected PATH_ERR_MAX_PATH_LEN_0, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 0, 25, 2, false) == PATH_ERR_MAX_PATH_LEN_0", relpath);
    }

    /*
     * test max depth <= 0
     */
    relpath = "foo/bar/baz";
    sanity = sane_relative_path(relpath, 99, 25, 0, false);
    if (sanity != PATH_ERR_MAX_DEPTH_0) {
        err(37, __func__, "sane_relative_path(\"%s\", 99, 25, 0, false): expected PATH_ERR_MAX_DEPTH_0, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 0, false) == PATH_ERR_MAX_DEPTH_0", relpath);
    }

    /*
     * test path starting with '/' (not relative path)
     */
    relpath = "/foo";
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_ERR_NOT_RELATIVE) {
        err(38, __func__, "sane_relative_path(\"%s\", 99, 25, 4, false): expected PATH_ERR_NOT_RELATIVE, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_ERR_NOT_RELATIVE", relpath);
    }

    /*
     * test path with filename too long
     */
    relpath = "aequeosalinocalcalinoceraceoaluminosocupreovitriolic"; /* 52 letter word recognised by some */
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_ERR_NAME_TOO_LONG) {
        err(39, __func__, "sane_relative_path(\"%s\", 99, 25, 4, false): expected PATH_ERR_NAME_TOO_LONG, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_ERR_NAME_TOO_LONG", relpath);
    }

    /*
     * test max name length <= 0
     */
    relpath = "foo";
    sanity = sane_relative_path(relpath, 99, 0, 2, false);
    if (sanity != PATH_ERR_MAX_NAME_LEN_0) {
        err(40, __func__, "sane_relative_path(\"%s\", 99, 0, 2, false): expected PATH_ERR_MAX_NAME_LEN_0, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 0, 2, false) == PATH_ERR_MAX_NAME_LEN_0", relpath);
    }

    /*
     * test a path that's too deep (depth)
     */
    relpath = "foo/bar";
    sanity = sane_relative_path(relpath, 99, 25, 1, false);
    if (sanity != PATH_ERR_PATH_TOO_DEEP) {
        err(41, __func__, "sane_relative_path(\"%s\", 99, 25, 1, false): expected PATH_ERR_PATH_TOO_DEEP, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 1, false) == PATH_ERR_PATH_TOO_DEEP", relpath);
    }

    /*
     * test invalid path component
     */
    relpath = "foo/../";
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_ERR_NOT_POSIX_SAFE) {
        err(42, __func__, "sane_relative_path(\"%s\", 99, 25, 4, false): expected PATH_ERR_NOT_POSIX_SAFE, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_ERR_NOT_POSIX_SAFE", relpath);
    }

    /*
     * test another invalid path component
     */
    relpath = "foo/./";
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_ERR_NOT_POSIX_SAFE) {
        err(43, __func__, "sane_relative_path(\"%s\", 99, 25, 4, false): expected PATH_ERR_NOT_POSIX_SAFE, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_ERR_NOT_POSIX_SAFE", relpath);
    }

    /*
     * test another invalid path component
     */
    relpath = "./foo/";
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_ERR_NOT_POSIX_SAFE) {
        err(44, __func__, "sane_relative_path(\"%s\", 99, 25, 4, false): expected PATH_ERR_NOT_POSIX_SAFE, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_ERR_NOT_POSIX_SAFE", relpath);
    }

    /*
     * test path with number in it
     */
    relpath = "foo1";
    sanity = sane_relative_path(relpath, 99, 25, 4, false);
    if (sanity != PATH_OK) {
        err(45, __func__, "sane_relative_path(\"%s\", 99, 25, 4, false): expected PATH_OK, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 4, false) == PATH_OK", relpath);
    }

    /*
     * test max depth
     */
    relpath = "a/b/c/d";
    sanity = sane_relative_path(relpath, 99, 25, 3, false);
    if (sanity != PATH_ERR_PATH_TOO_DEEP) {
        err(46, __func__, "sane_relative_path(\"%s\", 99, 25, 3, false): expected PATH_ERR_PATH_TOO_DEEP, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 3, false) == PATH_ERR_PATH_TOO_DEEP", relpath);
    }

    /*
     * test relative path as "./foo" being valid
     */
    relpath = "./foo";
    sanity = sane_relative_path(relpath, 99, 25, 3, true);
    if (sanity != PATH_OK) {
        err(47, __func__, "sane_relative_path(\"%s\", 99, 25, 3, true): expected PATH_OK, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 3, true) == PATH_OK", relpath);
    }

    /*
     * test relative path as "./foo" NOT being valid
     */
    relpath = "./foo";
    sanity = sane_relative_path(relpath, 99, 25, 3, false);
    if (sanity != PATH_ERR_NOT_POSIX_SAFE) {
        err(48, __func__, "sane_relative_path(\"%s\", 99, 25, 3, false): expected PATH_ERR_NOT_POSIX_SAFE, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 3, false) == PATH_ERR_NOT_POSIX_SAFE", relpath);
    }

    /*
     * test relative path as ".//foo" NOT being valid (even with true
     * dot_slash_okay) because it's not a relative path (according to the rules
     * of sane_relative_path() at least)
     */
    relpath = ".//foo";
    sanity = sane_relative_path(relpath, 99, 25, 3, true);
    if (sanity != PATH_ERR_NOT_RELATIVE) {
        err(49, __func__, "sane_relative_path(\"%s\", 99, 25, 3, true): expected PATH_ERR_NOT_RELATIVE, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "sane_relative_path(\"%s\", 99, 25, 3, true) == PATH_ERR_NOT_RELATIVE", relpath);
    }

    /*
     * in case we ever use name before this in some test, we free it first
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * NOTE: when using dir_name() with a level of < 0 we obtain the first
     * directory only or, in the case of an empty string or a string without any
     * '/', the original string itself.
     */

    /*
     * test remove -1 levels of an empty string
     */
    relpath = "";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(50, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(51, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \".\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected empty string", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels of "//foo" (an absolute path with more than one
     * leading '/'): this should return "foo" itself too.
     */
    relpath = "//foo";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(52, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, relpath+2) != 0) {
        err(53, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"%s\"", relpath, name, relpath+2);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"%s\"", relpath, relpath+2);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels of "//foo//foo//" (an absolute path with more than one
     * leading '/' and additional components with // in between and at the end):
     * this should return "foo" itself too.
     */
    relpath = "//foo//foo//";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(54, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(55, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"foo\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"foo\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels of "/foo" (an absolute path): this should return
     * "foo" itself too.
     */
    relpath = "/foo";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(56, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(57, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"foo\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"foo\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels of "/foo//" (an absolute path with two trailing
     * '/'s): this should return "foo" itself too.
     */
    relpath = "/foo";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(58, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(59, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"foo\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"foo\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels of "///" (an absolute path of just three '/'s):
     * this should return just "/"
     */
    relpath = "///";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(60, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "/") != 0) {
        err(61, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"/\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"/\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels of "///." (an absolute path of just three '/'s
     * ending with a '.'): this should return just ".".
     */
    relpath = "///.";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(62, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(63, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \".\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \".\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels of "///../....//" (an invalid absolute path): this
     * should return "..".
     */
    relpath = "///../....//";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(64, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "..") != 0) {
        err(65, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"..\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"..\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }



    /*
     * test remove -1 levels (first directory) of "foo" (relative path): this
     * should return "foo" itself.
     */
    relpath = "foo";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(66, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, relpath) != 0) {
        err(67, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"%s\"", relpath, name, relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"%s\"", relpath, relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels (first directory) of "./foo" (relative path
     * starting with ./): this should return ".".
     */
    relpath = "./foo";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(68, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name,".") != 0) {
        err(69, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \".\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \".\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels (first directory) of "./foo//" (relative path
     * starting with ./ and ending with two '/'s): this should return "." too.
     */
    relpath = "./foo//";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(70, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(71, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \".\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \".\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels (first directory) of ".//foo/." (relative path
     * starting with .// and ending with two "/."): this should return "." too.
     */
    relpath = ".//foo/.";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(72, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(73, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \".\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \".\"", relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels (first directory) of "foo": this should return
     * "foo" itself.
     */
    relpath = "foo";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(74, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, relpath) != 0) {
        err(75, __func__, "dir_name(\"%s\", -1): returned \"%s\", expected \"%s\"", relpath, name, relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): got expected \"%s\"", relpath, relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }



    /*
     * test remove -1 levels (i.e. return the first directory name) of
     * "/foo/bar/baz/zab/rab/oof" (an absolute path).
     */
    relpath = "/foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(76, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(77, __func__, "dir_name(\"%s\", -1): returned %s, expected: foo", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): %s", relpath, name);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove -1 levels (i.e. return the first directory name) of
     * "foo/bar/baz/zab/rab/oof" (a relative path).
     */
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(78, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(79, __func__, "dir_name(\"%s\", -1): returned %s, expected: foo", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1): %s", relpath, name);
    }


    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test remove 0 levels: this should return the original string
     * ("foo/bar/baz/zab/rab/oof")
     */
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, 0);
    if (name == NULL) {
        err(80, __func__, "dir_name(\"%s\", 0): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, relpath) != 0) {
        err(81, __func__, "dir_name(\"%s\", 0): returned %s, expected: %s", relpath, name, relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", 0): %s", relpath, relpath);
    }

    /*
     * free memory
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * check remove one level: this should return "foo/bar/baz/zab/rab"
     */
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, 1);
    if (name == NULL) {
        err(82, __func__, "dir_name(\"%s\", 1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo/bar/baz/zab/rab") != 0) {
        err(83, __func__, "dir_name(\"%s\", 1): returned %s, expected: foo/bar/baz/zab/rab", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", 1) == foo/bar/baz/zab/rab", relpath);
    }

    /*
     * test remove two levels
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, 2);
    if (name == NULL) {
        err(84, __func__, "dir_name(\"%s\", 2): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo/bar/baz/zab") != 0) {
        err(85, __func__, "dir_name(\"%s\", 2): returned %s, expected: foo/bar/baz/zab", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", 2) == foo/bar/baz/zab", relpath);
    }

    /*
     * test remove three levels
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, 3);
    if (name == NULL) {
        err(86, __func__, "dir_name(\"%s\", 3): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo/bar/baz") != 0) {
        err(87, __func__, "dir_name(\"%s\", 3): returned %s, expected: foo/bar/baz", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", 3) == foo/bar/baz", relpath);
    }

    /*
     * test remove four levels
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, 4);
    if (name == NULL) {
        err(88, __func__, "dir_name(\"%s\", 4): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo/bar") != 0) {
        err(89, __func__, "dir_name(\"%s\", 4): returned %s, expected: foo/bar", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", 4) == foo/bar", relpath);
    }

    /*
     * test remove five levels
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, 5);
    if (name == NULL) {
        err(90, __func__, "dir_name(\"%s\", 5): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(91, __func__, "dir_name(\"%s\", 5): returned %s, expected: foo", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", 5) == foo", relpath);
    }

    /*
     * test special level counts: 6 and -1
     *
     * These should return the first component only.
     */

    /*
     * test remove six levels
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, 6);
    if (name == NULL) {
        err(92, __func__, "dir_name(\"%s\", 6): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(93, __func__, "dir_name(\"%s\", 6): returned %s, expected: foo", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", 6) == foo", relpath);
    }

    if (name != NULL) {
        free(name);
        name = NULL;
    }
    /*
     * test remove -1 levels
     */
    relpath = "foo/bar/baz/zab/rab/oof";
    name = dir_name(relpath, -1);
    if (name == NULL) {
        err(94, __func__, "dir_name(\"%s\", -1): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "foo") != 0) {
        err(95, __func__, "dir_name(\"%s\", -1): returned %s, expected: foo", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"%s\", -1) == foo", relpath);
    }

    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * test NULL path
     */
    relpath = NULL;
    name = dir_name(NULL, -1);
    if (name == NULL) {
        err(96, __func__, "dir_name(NULL, -1): returned NULL");
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(97, __func__, "dir_name(NULL, -1): returned %s, expected: \".\"", name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(NULL, -1) == \".\"");
    }

    /*
     * test empty path
     */
    relpath = "";
    name = dir_name("", -1);
    if (name == NULL) {
        err(98, __func__, "dir_name(\"\", -1): returned NULL");
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(99, __func__, "dir_name(\"\", -1): returned %s, expected: \".\"", name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "dir_name(\"\", -1) == \".\"");
    }


    /*
     * now we have to test the base_name() function
     */
    if (name != NULL) {
        free(name);
        name = NULL;
    }
    relpath = "foo/bar/baz/zab/rab/oof";
    name = base_name(relpath);
    if (name == NULL) {
        err(100, __func__, "base_name(\"%s\"): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, "oof") != 0) {
        err(101, __func__, "base_name(\"%s\"): returned %s, expected: oof", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "base_name(\"%s\") == oof", relpath);
    }

    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * try again but with a NULL string
     */
    relpath = NULL;
    name = base_name(NULL);
    if (name == NULL) {
        err(102, __func__, "base_name(NULL): returned NULL");
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(103, __func__, "base_name(NULL): returned %s, expected: \".\"", name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "base_name(NULL) == \".\"");
    }

    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * try again but with an empty string
     */
    relpath = "";
    name = base_name(relpath);
    if (name == NULL) {
        err(104, __func__, "base_name(\"%s\"): returned NULL", relpath);
        not_reached();
    } else if (strcmp(name, ".") != 0) {
        err(105, __func__, "base_name(\"%s\"): returned %s, expected: \".\"", relpath, name);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "base_name(\"%s\") == \".\"", relpath);
    }

    if (name != NULL) {
        free(name);
        name = NULL;
    }

    /*
     * now we have to count the number of directories in a path
     */
    relpath = "foo/bar/baz";
    comps = count_dirs(relpath);
    if (comps != 2) {
        err(106, __func__, "count_dirs(\"%s\"): %ju != 2", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 2", relpath);
    }

    /*
     * successive '/'s count as a single '/'
     */
    relpath = "foo//baz";
    comps = count_dirs(relpath);
    if (comps != 1) {
        err(107, __func__, "count_dirs(\"%s\"): %ju != 1", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 1", relpath);
    }

    /*
     * "///" counts as 1
     */
    relpath = "///";
    comps = count_dirs(relpath);
    if (comps != 1) {
        err(108, __func__, "count_dirs(\"%s\"): %ju != 1", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 1", relpath);
    }

    /*
     * "/" counts as 1
     */
    relpath = "/";
    comps = count_dirs(relpath);
    if (comps != 1) {
        err(109, __func__, "count_dirs(\"%s\"): %ju != 1", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 1", relpath);
    }

    /*
     * "foo///" counts as 1
     */
    relpath = "foo///";
    comps = count_dirs(relpath);
    if (comps != 1) {
        err(110, __func__, "count_dirs(\"%s\"): %ju != 1", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 1", relpath);
    }

    /*
     * "" counts as 0
     */
    relpath = "";
    comps = count_dirs(relpath);
    if (comps != 0) {
        err(111, __func__, "count_dirs(\"%s\"): %ju != 0", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 0", relpath);
    }

    /*
     * "foo/..//foo" counts as 2
     */
    relpath = "foo/..//foo";
    comps = count_dirs(relpath);
    if (comps != 2) {
        err(112, __func__, "count_dirs(\"%s\"): %ju != 2", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 2", relpath);
    }

    /*
     * "foo/..//foo/3/" counts as 4
     */
    relpath = "foo/..//foo/3/";
    comps = count_dirs(relpath);
    if (comps != 4) {
        err(113, __func__, "count_dirs(\"%s\"): %ju != 4", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 4", relpath);
    }

    /*
     * "foo/../foo" counts as 2
     */
    relpath = "foo/../foo";
    comps = count_dirs(relpath);
    if (comps != 2) {
        err(114, __func__, "count_dirs(\"%s\"): %ju != 2", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 2", relpath);
    }

    /*
     * "foo/../foo/3/" counts as 4
     */
    relpath = "foo/../foo/3/";
    comps = count_dirs(relpath);
    if (comps != 4) {
        err(115, __func__, "count_dirs(\"%s\"): %ju != 4", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 4", relpath);
    }

    /*
     * "foo//foo" counts as 1
     */
    relpath = "foo//foo";
    comps = count_dirs(relpath);
    if (comps != 1) {
        err(116, __func__, "count_dirs(\"%s\"): %ju != 1", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 1", relpath);
    }

    /*
     * "foo//foo/3/" counts as 3
     */
    relpath = "foo//foo/3/";
    comps = count_dirs(relpath);
    if (comps != 3) {
        err(117, __func__, "count_dirs(\"%s\"): %ju != 3", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 3", relpath);
    }

    /*
     * "foo/foo" counts as 1
     */
    relpath = "foo/foo";
    comps = count_dirs(relpath);
    if (comps != 1) {
        err(118, __func__, "count_dirs(\"%s\"): %ju != 1", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 1", relpath);
    }

    /*
     * "foo/foo/3/" counts as 3
     */
    relpath = "foo/foo/3/";
    comps = count_dirs(relpath);
    if (comps != 3) {
        err(119, __func__, "count_dirs(\"%s\"): %ju != 3", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_dirs(\"%s\") == 3", relpath);
    }

    /*
     * test count_comps() by removing all trailing delimiters
     */

    /*
     * "foo/bar,,," should count as 2
     */
    relpath = "foo,bar,,,";
    comps = count_comps(relpath, ',', true);
    if (comps != 2) {
        err(120, __func__, "count_comps(\"%s\", ',', true): %ju != 2", relpath, comps);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "count_comps(\"%s\", ',', true) == 2", relpath);
    }

    /*
     * check if "foo/bar/baz" has component "baz"
     */
    relpath = "foo/bar/baz";
    if (!path_has_component(relpath, "baz")) {
        err(121, __func__, "path_has_component(\"%s\", \"baz\") returned false: expected true", relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"baz\") == true", relpath);
    }

    /*
     * check if "foo/bar/baz" has component "bar"
     */
    relpath = "foo/bar/baz";
    if (!path_has_component(relpath, "bar")) {
        err(122, __func__, "path_has_component(\"%s\", \"bar\") returned false: expected true", relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"bar\") == true", relpath);
    }

    /*
     * check if "foo//bar/baz" has component "bar"
     */
    relpath = "foo//bar/baz";
    if (!path_has_component(relpath, "bar")) {
        err(123, __func__, "path_has_component(\"%s\", \"bar\") returned false: expected true", relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"bar\") == true", relpath);
    }

    /*
     * check if "foo//bar/baz//" has component "baz"
     */
    relpath = "foo//bar/baz";
    if (!path_has_component(relpath, "baz")) {
        err(124, __func__, "path_has_component(\"%s\", \"baz\") returned false: expected true", relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"baz\") == true", relpath);
    }

    /*
     * check if "foo//bar/.git//" has component ".git"
     */
    relpath = "foo//bar/.git//";
    if (!path_has_component(relpath, ".git")) {
        err(125, __func__, "path_has_component(\"%s\", \".git\") returned false: expected true", relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \".git\") == true", relpath);
    }


    /*
     * try one more time but this time make it a binary file
     */
    relpath = "pr.o";
    bytes = copyfile(relpath, "pr.copy.o", true, 0);
    fdbg(stderr, DBG_MED, "copyfile(\"%s\", \"pr_test.copy.c\", true, 0): %ju bytes", relpath, (uintmax_t)bytes);

    /*
     * make sure pr.o st_mode is the same as pr.copy.o
     */
    errno = 0; /* pre-clear errno for errp() */
    if (stat("pr.o", &in_st) != 0) {
        errp(126, __func__, "failed to stat pr.o");
        not_reached();
    }
    /*
     * now get copy stat info
     */
    errno = 0;  /* pre-clear errno for errp() */
    if (stat("pr.copy.o", &out_st) != 0) {
        errp(128, __func__, "failed to stat pr.copy.o");
        not_reached();
    }

    if (!is_mode("pr.copy.o", in_st.st_mode)) {
        err(129, __func__, "pr.o st_mode != pr.copy.o st_mode: %04o != %04o",
                in_st.st_mode & ~S_IFMT, out_st.st_mode & ~S_IFMT);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "copyfile() successfully copied st_mode to dest file: %04o == %04o",
                filemode("pr.o", true), filemode(relpath, true));
    }

    /*
     * make sure size_if_file() for both pr.o and pr_copy.o are > 0 and the
     * same size and are the same size as in_st.st_size and out_st.size.
     *
     * first obtain size by size_if_file() (of both source and copy):
     */
    size1 = size_if_file("pr.o");
    size2 = size_if_file("pr.copy.o");
    /*
     * NOTE: if the files did not exist, which we know they do, the
     * size_if_file() will return 0. We know the size MUST be > 0 so we check
     * for > 0 first.
     */
    if (size1 <= 0 || size1 != size2 || size1 != in_st.st_size || size1 != out_st.st_size) {
        err(130, __func__, "size_if_file(\"pr.o\") != size_if_file(\"pr.copy.o\") or <= or != stat(2) st_size!");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "size_if_file(\"pr.o\") == size_if_file(\"pr.copy.o\"), > 0 and both match stat(2) st_size");
    }

    /*
     * delete copied file
     */
    errno = 0;      /* pre-clear errno for errp() */
    if (unlink("pr.copy.o") != 0) {
        errp(131, __func__, "unable to delete file pr.copy.o");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "successfully deleted copied file pr_test.copy.c");
    }
    /*
     * now check that the size is empty
     */
    if (!is_empty("pr.copy.o")) {
        errp(132, __func__, "just deleted file pr.copy.o is not empty");
        not_reached();
    }


    /*
     * relpath is NOT allocated so don't free!
     */

    /*
     * make several subdirectories
     */
    relpath = "test_pr/a/b/c/d/e/f/g";
    /*
     * we need to know if this path already exists so we know whether or not to
     * test the modes
     */
    dir_exists = is_dir(relpath);
    mkdirs(-1, relpath, 0755);
    if (!exists(relpath)) {
        err(133, __func__, "%s does not exist", relpath);
        not_reached();
    } else if (!is_dir(relpath)) {
        err(134, __func__, "%s is not a directory", relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "%s is a directory", relpath);
    }

    /*
     * test that the mode was correctly set if directory didn't previously exist
     */
    if (!dir_exists) {
        if (!is_mode(relpath, 0755)) {
            err(135, __func__, "failed to set mode of %s to %o", relpath, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s is mode 07555", relpath);
        }
    }


    /*
     * make a single directory under test_pr/
     */
    relpath = "test_pr/b";
    /*
     * we need to know if this path already exists so we know whether or not to
     * test the modes
     */
    dir_exists = is_dir(relpath);
    mkdirs(-1, relpath, 0755);
    if (!exists(relpath)) {
        err(136, __func__, "%s does not exist", relpath);
        not_reached();
    } else if (!is_dir(relpath)) {
        err(137, __func__, "%s is not a directory", relpath);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "%s is a directory", relpath);
    }

    /*
     * test that the mode was correctly set if directory didn't previously exist
     */
    if (!dir_exists) {
        if (!is_mode(relpath, 0755)) {
            err(138, __func__, "failed to set mode of %s to %o", relpath, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s is mode 07555", relpath);
        }

        /*
         * also check that has_mode() works with the 0755 mode we set
         */
        if (!has_mode(relpath, 0755)){
            err(139, __func__, "%s does not have bits %o set", relpath, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s has bits %o in stat.st_mode",
                relpath, 0755);
        }

        /*
         * also check specific bits
         */
        if (!has_mode(relpath, S_IRWXU)) {
            err(140, __func__, "%s does not have bits %o set", relpath, S_IRWXU);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s has bits %o in stat.st_mode", relpath, S_IRWXU);
        }

        /*
         * check bits not set
         */
        if (has_mode(relpath, S_ISUID)) {
            err(141, __func__, "%s has set user id on execution", relpath);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s does not have set user id on execution", relpath);
        }
    }

    /*
     * Test is_chardev() on /dev/null. We will NOT test the other is_*()
     * functions as we don't want to mess with syscalls like mkfifo(2).
     *
     * And although /dev/null should be a character device we will not insist it
     * is.
     */
    if (is_chardev("/dev/null")) {
        fdbg(stderr, DBG_MED, "/dev/null is a character device");
        /*
         * test the file mode if verbose enough
         */
        fdbg(stderr, DBG_HIGH, "/dev/null is mode: %o", filemode("/dev/null", true));
    } else {
        fdbg(stderr, DBG_MED, "/dev/null is NOT a character device");
    }



    /*
     * test read_fts()
     *
     * NOTE: we will only show path names if debug level is very very high
     * as this would be annoying otherwise (due to there being A LOT of files in
     * test_pr/).
     *
     * We will show the filename (full path) and filetype as well (well - at
     * least for regular file, directory and symlink - we will ignore the other
     * kinds)
     *
     * We use logical == true so that broken symlinks can be detected.
     */
    fts.logical = true;
    ent = read_fts("", -1, &cwd, &fts);
    if (ent == NULL) {
        err(142, __func__, "read_fts() returned a NULL pointer on \"\"");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(143, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            switch (ent->fts_info) {
                case FTS_F:
                    fdbg(stderr, DBG_VVHIGH, "%s (file)", p);
                    if (is_file(p)) {
                        fdbg(stderr, DBG_VVVHIGH, "FTS_F %s: is_file(\"%s\") == true", p, p);
                    } else {
                        err(144, __func__, "%s reported as FTS_FILE !is_file(\"%s\")", p, p);
                        not_reached();
                    }
                    break;
                case FTS_D:
                    fdbg(stderr, DBG_VVHIGH, "%s (dir)", p);
                    if (is_dir(p)) {
                        fdbg(stderr, DBG_VVVHIGH, "FTS_D %s: is_dir(\"%s\") == true", p, p);
                    } else {
                        err(145, __func__, "%s reported as FTS_D !is_dir(\"%s\")", p, p);
                        not_reached();
                    }
                    break;
                case FTS_SL:
                    fdbg(stderr, DBG_VVHIGH, "%s (symlink)", p);
                    if (is_symlink(p)) {
                        fdbg(stderr, DBG_VVVHIGH, "FTS_D %s: is_symlink(\"%s\") == true", p, p);
                    } else {
                        err(146, __func__, "%s reported as FTS_SL !is_symlink(\"%s\")", p, p);
                        not_reached();
                    }
                    break;
                case FTS_SLNONE:
                    fdbg(stderr, DBG_VVHIGH, "%s (broken symlink)", p);
                    break;
                case FTS_NS:
                    err(147, __func__, "no stat(2) info available for path %s in tree", p);
                    not_reached();
                    break;
                case FTS_NSOK:
                    err(148, __func__, "encountered FTS_NSOK for path %s in tree: FTS_NOSTAT set!", p);
                    not_reached();
                    break;
            }
        } while ((ent = read_fts(NULL, -1, &cwd, &fts)) != NULL);
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * test read_fts() only finding directories
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* true, true: clear out ignore and match lists */
    fts.type = FTS_TYPE_DIR;
    ent = read_fts("", -1, &cwd, &fts);
    if (ent == NULL) {
        err(149, __func__, "read_fts() returned a NULL pointer on \"\" for directories");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(150, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            if (ent->fts_info != FTS_D && ent->fts_info != FTS_DP) {
                err(151, __func__, "%s is not a directory", p);
                not_reached();
            } else {
                fdbg(stderr, DBG_MED, "%s is a directory", p);
            }
        } while ((ent = read_fts(NULL, -1, &cwd, &fts)) != NULL);
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);
    /*
     * test read_fts() with fnmatch(3)
     */

    /*
     * reset fts
     */
    reset_fts(&fts, false, false); /* don't clear out ignore or match lists */
    fts.type = FTS_TYPE_FILE;
    fts.base = false; /* important */
    fts.match_case = false;
    fts.fn_ignore_flags = 0;
    append_path(&(fts.ignore), "pr*", true, false, true, false);
    ent = read_fts(".", -1, &cwd, &fts);
    if (ent == NULL) {
        err(152, __func__, "read_fts() returned a NULL pointer on \".\" for directories");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(153, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            if (ent->fts_info != FTS_F) {
                err(154, __func__, "%s is not a file", p);
                not_reached();
            } else if (!strncmp(p, "pr",4)) {
                err(155, __func__, "found file meant to be ignored: %s", p);
                not_reached();
            } else {
                fdbg(stderr, DBG_MED, "%s is a non-ignored file", p);
            }
        } while ((ent = read_fts(NULL, -1, &cwd, &fts)) != NULL);
    }
    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * test that FTS_NOSTAT is correctly unset in read_fts() even if we set it
     *
     * NOTE: we will only show path names if debug level is very very high
     * as this would be annoying otherwise.
     *
     * We will show the filename (full path) and filetype as well (well - at
     * least for regular file, directory and symlink - we will ignore the other
     * kinds).
     *
     * We use logical == true so that broken symlinks can be detected.
     */
    reset_fts(&fts, false, true); /* don't clear out ignore list but DO clear out match list */
    fts.options = FTS_NOSTAT | FTS_LOGICAL;
    fts.logical = true;

    /*
     * now test
     */
    ent = read_fts("", -1, &cwd, &fts);
    if (ent == NULL) {
        err(156, __func__, "read_fts() returned a NULL pointer on \"\"");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (ent == NULL) {
                err(157, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            switch (ent->fts_info) {
                case FTS_F:
                    fdbg(stderr, DBG_VVHIGH, "%s (file)", p);
                    break;
                case FTS_D:
                    fdbg(stderr, DBG_VVHIGH, "%s (dir)", p);
                    break;
                case FTS_SL:
                    fdbg(stderr, DBG_VVHIGH, "%s (symlink)", p);
                    break;
                case FTS_SLNONE:
                    fdbg(stderr, DBG_VVHIGH,  "%s (symlink with non-existent target)", p);
                    break;
                case FTS_NS:
                    err(158, __func__, "no stat(2) info available");
                    not_reached();
                    break;
                case FTS_NSOK:
                    err(159, __func__, "encountered FTS_NSOK for path %s in tree: FTS_NOSTAT set!", p);
                    not_reached();
                    break;
            }
        } while ((ent = read_fts(NULL, -1, &cwd, &fts)) != NULL);
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * look for a file called "pr_test" which should succeed because it's
     * not in the ignored list
     */
    /*
     * reset fts
     */
    fts.base = false;
    fts.seedot = false;
    fts.depth = 0;
    fts.count = 0;
    fts.match_case = true;
    fname = find_path("pr_test", NULL, -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(160, __func__, "found paths when none should have been found");
        not_reached();
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);



    /*
     * we need to test find_paths() now
     */
    /*
     * append strings to the array that we wish to find
     */
    /* filenames
     */
    append_path(&paths, "pr.o", true, false, true, false);
    append_path(&paths, "pr.c", true, false, true, false);
    append_path(&paths, "pr.h", true, false, true, false);
    append_path(&paths, "pr_test", true, false, true, false);
    /*
     * special checks: . and ..
     */
    append_path(&paths, ".", true, false, true, false);
    append_path(&paths, "..", true, false, true, false);
    /*
     * path that doesn't exist (or shouldn't)
     */
    append_path(&paths, "foobar", true, false, true, false);

    /*
     * test find_paths()
     */
    /*
     * reset fts
     */
    reset_fts(&fts, false, false); /* don't clear out ignore or match lists */
    fts.type = FTS_TYPE_ANY;
    fts.logical = false;
    fts.options = FTS_LOGICAL; /* it will be set to FTS_NOCHDIR | FTS_PHYSICAL */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(161, __func__, "didn't find any paths in the paths array");
        not_reached();
    }

    /*
     * look for pr_test
     */
    name = find_path_in_array("pr_test", paths, false, false, &idx);
    if (name == NULL) {
        /*
         * "pr_test" must exist
         */
        err(162, __func__, "didn't find \"pr_test\" in paths array");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "found pr_test at paths[%ju]", (uintmax_t)idx);
    }

    /*
     * show what we found
     */
    len = paths_in_array(paths_found);
    if (len > 0) {
        for (j = 0; j < len; ++j) {
            /* get next string pointer */
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {	/* paranoia */
                err(163, __func__, "found NULL pointer at paths_found[%ju]", (uintmax_t)j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", (uintmax_t)j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", (uintmax_t)j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", (uintmax_t)j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", (uintmax_t)j, name);
                }
            }
        }
    }

    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * test find_paths() with seedot == true
     */
    /*
     * reset fts again
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_ANY;
    fts.options = 0; /* will be set to FTS_NOCHDIR */
    fts.logical = false;
    fts.base = false;
    fts.seedot = true;
    fts.match_case = true;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(164, __func__, "didn't find any paths in the paths array");
        not_reached();
    }

    /*
     * show what we found
     */
    len = paths_in_array(paths_found);
    if (len > 0) {
        for (j = 0; j < len; ++j) {
            /* get next string pointer */
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {	/* paranoia */
                err(165, __func__, "found NULL pointer at paths_found[%ju]", (uintmax_t)j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", (uintmax_t)j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", (uintmax_t)j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", (uintmax_t)j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", (uintmax_t)j, name);
                }
            }
        }
    }
    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);



    /*
     * test find_paths() without basename, seedot and match_case
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.logical = false;
    fts.base = false;
    fts.seedot = false;
    fts.match_case = false;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(166, __func__, "didn't find any files in the paths array");
        not_reached();
    }

    /*
     * show what we found
     */
    len = paths_in_array(paths_found);
    if (len > 0) {
        for (j = 0; j < len; ++j) {
            /* get next string pointer */
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {	/* paranoia */
                err(167, __func__, "found NULL pointer at paths_found[%ju]", (uintmax_t)j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", (uintmax_t)j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", (uintmax_t)j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", (uintmax_t)j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", (uintmax_t)j, name);
                }
            }
        }
    }
    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * free paths array for special tests
     */
    free_paths_array(&paths, false);
    paths = NULL;

    /*
     * special test: only add "bad". Then make sure that this is in the ignored
     * list. Then try and find this path. It should return NULL.
     */
    append_path(&paths, "bad", true, false, true, false);

    /*
     * test find_paths() looking for every file called bad; this should fail.
     */

    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(168, __func__, "found paths when none should have been found");
        not_reached();
    }

    /*
     * test find_paths() looking for every file called pr.c; this should
     * succeed because it's not in the ignore list.
     */
    append_path(&paths, "pr.c", true, false, true, false);

    /*
     * free paths found
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL;

    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(169, __func__, "unable to find any path in the directory");
        not_reached();
    }


    /*
     * special test: this time we will look for every single file by making
     * an empty path string and FTS_TYPE_ANY
     */
    append_path(&paths, "", true, false, false, false);

    /*
     * test find_paths() looking for every file under pr_test/
     */
    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "test_pr", -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(170, __func__, "didn't find any paths in the paths array");
        not_reached();
    }

    /*
     * show what we found
     */
    len = paths_in_array(paths_found);
    if (len > 0) {
        /*
         * we'll count each type and then at the end we'll show how many of each
         * there are
         */
        size_t dirs = 0; /* directories */
        size_t files = 0; /* regular files */
        size_t symlinks = 0; /* symlinks */
        size_t others = 0; /* other file types */
        for (j = 0; j < len; ++j) {
            /* get next string pointer */
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {	/* paranoia */
                err(171, __func__, "found NULL pointer at paths_found[%ju]", (uintmax_t)j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", (uintmax_t)j, name);
                    ++dirs;
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", (uintmax_t)j, name);
                    ++files;
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", (uintmax_t)j, name);
                    ++symlinks;
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", (uintmax_t)j, name);
                    ++others;
                }
            }
        }
        fdbg(stderr, DBG_MED, "a total of %ju director%s in paths_found", (uintmax_t)dirs, dirs==1?"y":"ies");
        fdbg(stderr, DBG_MED, "a total of %ju file%s in paths_found", (uintmax_t)files, files==1?"":"s");
        fdbg(stderr, DBG_MED, "a total of %ju symlink%s in paths_found", (uintmax_t)symlinks, symlinks==1?"":"s");
        fdbg(stderr, DBG_MED, "a total of %ju other file type%s in paths_found", (uintmax_t)others,
                others==1?"":"s");
    }
    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);


    /*
     * special test: verify no sockets exist in the directory by setting path to
     * "" and setting FTS_TYPE_SOCK
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_SOCK;
    fts.logical = false;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(172, __func__, "found unexpected socket under cwd");
        not_reached();
    }

    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * special test: verify no character devices exist in the directory by setting path to
     * "" and setting FTS_TYPE_CHAR
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_CHAR;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(173, __func__, "found unexpected character device under cwd");
        not_reached();
    }

    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);


    /*
     * special test: verify no block devices exist in the directory by setting path to
     * "" and setting FTS_TYPE_BLOCK
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_BLOCK;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(174, __func__, "found unexpected block device under cwd");
        not_reached();
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);


    /*
     * special test: verify no FIFO exist in the directory by setting path to
     * "" and setting FTS_TYPE_FIFO
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_FIFO;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = false;
    paths_found = find_paths(paths, "", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(175, __func__, "found unexpected FIFO under cwd");
        not_reached();
    }

    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);


    /*
     * special test: verify no file types exist in the directory other than
     * files, directories and symlinks by setting the path to "" and masking
     * FTS_TYPE_ANY by ~(FTS_TYPE_FILE|FTS_TYPE_DIR|FTS_TYPE_SYMLINK).
     */
    /*
     * reset fts
     */
    fts.type = FTS_TYPE_ANY & ~(FTS_TYPE_FILE|FTS_TYPE_DIR|FTS_TYPE_SYMLINK);
    fts.base = true;
    fts.seedot = false;
    fts.match_case = false;
    paths_found = find_paths(paths, "", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        /*
         * first show the files
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {
                err(176, __func__, "found NULL pointer in paths_found (not file, directory or symlink) array");
                not_reached();
            }
            warn(__func__, "path is not a file, directory or symlink: %s", name);
        }
        /*
         * make it an error
         */
        err(177, __func__, "found unexpected file type under cwd");
        not_reached();
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * this next test is special in that we want to test case-sensitive and
     * case-insensitive of the find_paths() function on a specific file that we
     * will temporarily make
     */

    /*
     * first free the paths array
     */
    free_paths_array(&paths, false);
    paths = NULL; /* paranoia */
    /*
     * now append "foobar" to the array
     */
    append_path(&paths, "foobar", true, false, true, false);

    /*
     * create a file called foobar
     */
    relpath = "foobar";
    touch(relpath, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (!exists(relpath)) {
        err(178, __func__, "file %s does not exist after touch()", relpath);
        not_reached();
    }

    /*
     * check for "foobar" by case-sensitive
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_ANY; /* actual default */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        /*
         * first show the files
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {
                err(179, __func__, "found NULL pointer in paths_found (any) array");
                not_reached();
            }
            if (strcmp(relpath, name) != 0) {
                err(180, __func__, "found non-matching file in list: %s != %s", name, relpath);
                not_reached();
            }
            fdbg(stderr, DBG_MED, "found file %s as case-sensitive search", name);
        }
    } else {
        err(181, __func__, "couldn't find any file called \"%s\" as case-sensitive search", relpath);
        not_reached();
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * free array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * check for "foobar" by case-insensitive
     */
    relpath = "foobar"; /* paranoia */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_ANY;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = false;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        /*
         * first show the files
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {
                err(182, __func__, "found NULL pointer in paths_found (any) array");
                not_reached();
            }
            if (strcasecmp(relpath, name) != 0) {
                err(183, __func__, "found non-matching file in list: %s != %s", name, relpath);
                not_reached();
            }

            fdbg(stderr, DBG_MED, "found %s by case-insensitive search", name);
        }
    } else {
        err(184, __func__, "couldn't find any file called \"%s\" by case-insensitive search", relpath);
        not_reached();
    }

    /*
     * delete foobar
     */
    errno = 0;      /* pre-clear errno for errp() */
    if (unlink(relpath) != 0) {
        if (errno != ENOENT) {
            errp(185, __func__, "unable to delete file %s", relpath);
            not_reached();
        }
    } else {
        fdbg(stderr, DBG_MED, "successfully deleted created file %s", relpath);
    }
    /*
     * now check that the size is empty
     */
    if (!is_empty(relpath)) {
        errp(186, __func__, "just deleted file %s is not empty", relpath);
        not_reached();
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * free paths_found array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * this next test is special in that we want to test case-sensitive and
     * case-insensitive of the find_paths() function on a specific file that we
     * will temporarily make, only this time the file is a different case.
     */

    /*
     * now create a new file Foobar
     *
     * NOTE: we also use relpath here.
     */
    relpath = "Foobar";
    /*
     * create a file called Foobar
     */
    touch(relpath, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    /*
     * make sure it exists
     */
    if (!exists(relpath)) {
        err(187, __func__, "file %s does not exist", relpath);
        not_reached();
    }

    /*
     * free paths array
     *
     * We need to do this so that we can make sure that the array is empty.
     */
    free_paths_array(&paths, false);
    paths = NULL; /* paranoia */
    /*
     * now append "foobar" to the array
     */
    append_path(&paths, "foobar", true, false, true, false);

    /*
     * check for "foobar" by case-sensitive
     *
     * NOTE: we created a file Foobar not foobar.
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_ANY; /* actual default */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        /*
         * first show the files
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {
                err(188, __func__, "found NULL pointer in paths_found (any) array");
                not_reached();
            }
            fdbg(stderr, DBG_MED, "found file %s as case-sensitive search", name);
        }
    } else {
        /*
         * this can fail on some file systems but not all
         */
        fdbg(stderr, DBG_MED, "couldn't find any file called \"foobar\" as case-sensitive search");
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * check for "foobar" by case-insensitive.
     *
     * NOTE: we created Foobar, not foobar.
     */

    /*
     * free paths_found array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_ANY;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = false;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        /*
         * first show the files
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {
                err(189, __func__, "found NULL pointer in paths_found array");
                not_reached();
            }
            if (strcasecmp(relpath, name) != 0) {
                err(190, __func__, "found non-matching file: %s != %s", name, relpath);
                not_reached();
            }
            fdbg(stderr, DBG_MED, "found %s by case-insensitive search", name);
        }
    } else {
        /*
         * as it is a case-insensitive it should always succeed
         */
        err(191, __func__, "couldn't find any file called %s by case-insensitive search", relpath);
        not_reached();
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * one more test with this: this time we'll get the absolute path, using
     * find_path() (not find_paths())
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_ANY;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = false;
    fname = find_path(relpath, NULL, -1, NULL, true, &fts);
    if (fname != NULL) {
        fdbg(stderr, DBG_MED, "found %s at %s", relpath, fname);
        free(fname);
        fname = NULL;
    } else {
        err(192, __func__, "couldn't find %s in tree", relpath);
        not_reached();
    }


    /*
     * delete Foobar
     */
    errno = 0;      /* pre-clear errno for errp() */
    if (unlink(relpath) != 0) {
        if (errno != ENOENT) {
            errp(193, __func__, "unable to delete file %s", relpath);
            not_reached();
        }
    } else {
        fdbg(stderr, DBG_MED, "successfully deleted created file %s", relpath);
    }
    /*
     * now check that the size is empty
     */
    if (!is_empty(relpath)) {
        errp(194, __func__, "just deleted file %s is not empty", relpath);
        not_reached();
    }

    /*
     * free paths_found array
     */
    free_paths_array(&paths_found, false);
    paths_found = NULL; /* paranoia */

    /*
     * free paths array
     */
    free_paths_array(&paths, false);
    paths = NULL; /* paranoia */

    /*
     * clear out fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */

    /*
     * test resolve_path()
     */
    name = resolve_path("ls");
    if (name != NULL) {
        fdbg(stderr, DBG_MED, "resolved ls as: %s", name);
        free(name);
        name = NULL;
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(0); /*ooo*/
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
	warn(__func__, "\nstr was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT,
						     PR_TEST_BASENAME, PR_TEST_VERSION,
						     dyn_array_version);
    exit(exitcode); /*ooo*/
    not_reached();
}
