/*
 * test_file_util - test common utility functions for file operations
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
#include <getopt.h>
#include <locale.h>
#include <fnmatch.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 * dyn_array - dynamic array facility
 */
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"

/*
 * canon_path - canonicalize paths
 */
#include "../cpath/cpath.h"

/*
 * file_util - common utility functions for file operations
 */
#include "../soup/file_util.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "../soup/version.h"

/*
 * jparse/util - common utility functions for the JSON parser and tools
 */
#include "../jparse/util.h"

/*
 * jparse - JSON library
 */
#include "../jparse/jparse.h"

/*
 * json_util - JSON util functions
 */
#include "../jparse/json_util.h"

/*
 * json_utf8 - jparse utf8 version
 */
#include "../jparse/json_utf8.h"

/*
 * version - official jparse API, parser and utils versions
 */
#include "../jparse/version.h"


/*
 * usage message
 *
 * test util usage message
 */
static char const * const usage =
"usage: %s [-h] [-v level] [-V] [-q]\n"
"\n"
"\t-h\t\tprint help message and exit\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\t-J level\tset JSON verbosity level (def level: 0)\n"
"\t-V\t\tprint version strings and exit\n"
"\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )\n"
"\n"
"Exit codes:\n"
"     0   all okay\n"
"     1   reserved for past use :-)\n"
"     2   -h and help string printed or -V and version string printed\n"
"     3   invalid command line, invalid option or option missing an argument\n"
" >= 10   internal error has occurred or unknown tar listing format has been encountered\n"
"\n"
"jparse util test version: %s\n"
"jparse utils version: %s\n"
"jparse UTF-8 version: %s\n"
"jparse library version: %s";


/*
 * static functions
 */
static void test_base_name(int test_num, char const *path, char const *expect);
static void test_dir_name(int test_num, char const *path, char const *expect);


/*
 * test_base_name - test base_name()
 *
 * given:
 *	test_num	    - test number
 *
 *	path		    - path to test (may be NULL)
 *	expect		    - expected basename
 */
static void
test_base_name(int test_num, char const *path, char const *expect)
{
    char *ret;		    /* malloced base_name() return */

    /*
     * firewall
     */
    if (expect == NULL) {
	err(12, __func__, "expect is NULL");
	not_reached();
    }

    /*
     * call base_name()
     */
    ret = base_name(path);

    /*
     * case: testing NULL path
     */
    if (path == NULL) {
	if (ret == NULL) {
	    err(13, __func__, "test[%d] base_name(\"((NULL))\"): retuned NULL", test_num);
	    not_reached();

	} else if (strcmp(ret, expect) != 0) {
	    err(14, __func__, "test[%d] failed: base_name(\"((NULL)\") return: \"%s\" != \"%s\"",
			      test_num, ret, expect);
	    not_reached();

	} else {
	    dbg(DBG_MED, "%s: test[%d] OK: base_name(\"((NULL)\") return: \"%s\"",
			 __func__, test_num, ret);
	}

    /*
     * case: testing non-NULL path
     */
    } else {
	if (ret == NULL) {
	    err(15, __func__, "%s: test[%d] base_name(\"%s\"): retuned NULL",
			      __func__, test_num, path);
	    not_reached();
	} else if (strcmp(ret, expect) != 0) {
	    err(16, __func__, "test[%d] failed: base_name(\"((NULL)\") return: \"%s\" != \"%s\"",
			      test_num, ret, expect);
	    not_reached();
	} else {
	    dbg(DBG_MED, "%s: test[%d] OK: base_name(\"%s\") "
			 "return: \"%s\"",
			 __func__, test_num, path, ret);
	}
    }

    /*
     * free base_name return
     */
    if (ret != NULL) {
        free(ret);
        ret = NULL;
    }
    return;
}


/*
 * test_dir_name - test dir_name()
 *
 * given:
 *	test_num	    - test number
 *
 *	path		    - path to test (may be NULL)
 *	expect		    - expected dirname
 */
static void
test_dir_name(int test_num, char const *path, char const *expect)
{
    char *ret;		    /* malloced dir_name() return */

    /*
     * firewall
     */
    if (expect == NULL) {
	err(17, __func__, "expect is NULL");
	not_reached();
    }

    /*
     * call dir_name()
     */
    ret = dir_name(path);

    /*
     * case: testing NULL path
     */
    if (path == NULL) {
	if (ret == NULL) {
	    err(18, __func__, "test[%d] dir_name(\"((NULL))\"): retuned NULL", test_num);
	    not_reached();

	} else if (strcmp(ret, expect) != 0) {
	    err(19, __func__, "test[%d] failed: dir_name(\"((NULL)\") return: \"%s\" != \"%s\"",
			      test_num, ret, expect);
	    not_reached();

	} else {
	    dbg(DBG_MED, "%s: test[%d] OK: dir_name(\"((NULL)\") return: \"%s\"",
			 __func__, test_num, ret);
	}

    /*
     * case: testing non-NULL path
     */
    } else {
	if (ret == NULL) {
	    err(20, __func__, "%s: test[%d] dir_name(\"%s\"): retuned NULL",
			      __func__, test_num, path);
	    not_reached();
	} else if (strcmp(ret, expect) != 0) {
	    err(21, __func__, "test[%d] failed: dir_name(\"%s\") return: \"%s\" != \"%s\"",
			      test_num, path, ret, expect);
	    not_reached();
	} else {
	    dbg(DBG_MED, "%s: test[%d] OK: dir_name(\"%s\") "
			 "return: \"%s\"",
			 __func__, test_num, path, ret);
	}
    }

    /*
     * free dir_name return
     */
    if (ret != NULL) {
        free(ret);
        ret = NULL;
    }
    return;
}


int
main(int argc, char **argv)
{
    char *program = NULL;		/* our name */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char *buf = NULL;
    char const *dirname = "foo";
    char const *filename = "bar";
    /**/
    int32_t test_num = 0;		/* canon_path test number */
    /**/
    char *path = NULL;			/* path to process */
    char *expect = NULL;		/* expected process result */
    /**/
    char *name = NULL;                  /* for various tests */
    size_t bytes = 0;
    struct stat in_st;                  /* verify copyfile() sets correct modes */
    struct stat out_st;                 /* output file stat(2) */
    bool dir_exists = false;            /* true ==> directory already exists (for testing modes) */
    FTSENT *ent = NULL;                 /* to test read_fts() */
    char *fname = NULL;                 /* to test find_path() */
    int cwd = -1;                       /* to restore after read_fts() test */
    FILE *fp = NULL;                    /* to test find_path() */
    struct dyn_array *paths = NULL;     /* to test find_paths() */
    struct dyn_array *paths_found = NULL;   /* to test find_paths() */
    uintmax_t len = 0;                 /* length of arrays */
    intmax_t idx = 0;                  /* for find_path_in_array() */
    struct fts fts;		        /* for read_fts(), find_path() and find_paths() */
    off_t size1 = 0;			/* for checking size_if_file() */
    off_t size2 = 0;			/* for checking size_if_file() */
    struct json *tree = NULL;           /* check that the jparse.json file is valid JSON */
    int ret;
    int i;
    uintmax_t j = 0;                   /* for arrays */

    /*
     * use default locale based on LANG
     */
    (void) setlocale(LC_ALL, "");

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:Vqe:")) != -1) {
	switch (i) {
	case 'h':	/* -h - write help, to stderr and exit 0 */
	    fprintf_usage(2, stderr, usage, program, FILE_UTIL_TEST_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION,/*ooo*/
                    JPARSE_LIBRARY_VERSION);
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;			/* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, __func__, "cannot parse -v arg: %s", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'J':	/* -J verbosity */
	    /* parse JSON verbosity */
	    errno = 0;			/* pre-clear errno for errp() */
	    json_verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, __func__, "cannot parse -J arg: %s", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - write version strings and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("test_file_util version: %s\n", FILE_UTIL_TEST_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error writing test_file_util version string: %s", FILE_UTIL_TEST_VERSION);
	    }
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error writing jparse utils version string: %s", JPARSE_UTILS_VERSION);
	    }
            errno = 0;          /* pre-clear errno for warnp() */
	    ret = printf("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error writing jparse library version string: %s", JPARSE_LIBRARY_VERSION);
	    }
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case ':':
	    (void) fprintf(stderr, "%s: requires an argument -- %c\n\n", program, optopt);
	    fprintf_usage(3, stderr, usage, program, FILE_UTIL_TEST_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION,/*ooo*/
                    JPARSE_LIBRARY_VERSION);
	    not_reached();
	    break;
	case '?':
	    (void) fprintf(stderr, "%s: illegal option -- %c\n\n", program, optopt);
	    fprintf_usage(3, stderr, usage, program, FILE_UTIL_TEST_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION,/*ooo*/
                    JPARSE_LIBRARY_VERSION);
	    not_reached();
	    break;
	default:
	    fprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    fprintf_usage(3, stderr, usage, program, FILE_UTIL_TEST_VERSION, JPARSE_UTILS_VERSION,/*ooo*/
                    JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
	    not_reached();
	}
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
    fdbg(stderr, DBG_MED, "json_verbosity_level: %d", json_verbosity_level);
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
	errp(22, __func__, "calloc_path(\"%s\", \"%s\") returned NULL", dirname, filename);
	not_reached();
    } else if (strcmp(buf, "foo/bar") != 0) {
	err(23, __func__, "buf: %s != %s/%s", buf, dirname, filename);
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
	errp(24, __func__, "calloc_path(NULL, \"%s\") returned NULL", filename);
	not_reached();
    } else if (strcmp(buf, "bar") != 0) {
	err(25, __func__, "buf: %s != %s", buf, filename);
	not_reached();
    } else {
	fdbg(stderr, DBG_MED, "calloc_path(NULL, \"%s\"): returned %s", filename, buf);
    }

    if (buf != NULL) {
	free(buf);
	buf = NULL;
    }

    /*
     * now try parsing jparse.json as a JSON file
     */
    if (is_dir("./jparse")) {
	dirname = "./jparse";
    } else if (is_dir("../jparse")) {
	dirname = "../jparse";
    } else {
	dirname = ".";
    }
    filename = "jparse.json";
    tree = open_json_dir_file(dirname, filename);
    if (tree == NULL) {
        err(10, __func__, "jparse.json is invalid JSON"); /*ooo*/
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "%s/jparse.json is valid JSON", dirname);
        json_tree_free(tree, JSON_DEFAULT_MAX_DEPTH);
        free(tree);
        tree = NULL;
    }

    /*
     * base_name "foo"
     */
    test_num = 25;
    /**/
    path = "foo";
    expect = "foo";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "//foo//bar//"
     */
    test_num = 26;
    /**/
    path = "//foo//bar//";
    expect = "bar";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name NULL
     */
    test_num = 27;
    /**/
    path = NULL;
    expect = ".";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name empty string
     */
    test_num = 28;
    /**/
    path = "";
    expect = ".";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "./foo"
     */
    test_num = 29;
    /**/
    path = "./foo";
    expect = "foo";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "../foo"
     */
    test_num = 30;
    /**/
    path = "../foo";
    expect = "foo";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "/foo"
     */
    test_num = 31;
    /**/
    path = "/foo";
    expect = "foo";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "foo/bar/baz"
     */
    test_num = 32;
    /**/
    path = "foo/bar/baz";
    expect = "baz";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "/"
     */
    test_num = 33;
    /**/
    path = "/";
    expect = "/";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "."
     */
    test_num = 34;
    /**/
    path = ".";
    expect = ".";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * dir_name "foo"
     */
    test_num = 35;
    /**/
    path = "foo";
    expect = ".";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "//foo//bar//"
     */
    test_num = 36;
    /**/
    path = "//foo//bar//";
    expect = "/foo";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name NULL
     */
    test_num = 37;
    /**/
    path = NULL;
    expect = ".";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name empty string
     */
    test_num = 38;
    /**/
    path = "";
    expect = ".";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "./foo"
     */
    test_num = 39;
    /**/
    path = "./foo";
    expect = ".";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "../foo"
     */
    test_num = 40;
    /**/
    path = "../foo";
    expect = "..";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "/foo"
     */
    test_num = 41;
    /**/
    path = "/foo";
    expect = "/";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "foo/bar/baz"
     */
    test_num = 42;
    /**/
    path = "foo/bar/baz";
    expect = "foo/bar";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "/"
     */
    test_num = 43;
    /**/
    path = "/";
    expect = "/";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "."
     */
    test_num = 44;
    /**/
    path = ".";
    expect = ".";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "/foo//"
     */
    test_num = 45;
    /**/
    path = "/foo";
    expect = "/";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "///"
     */
    test_num = 46;
    /**/
    path = "///";
    expect = "/";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "///."
     */
    test_num = 47;
    /**/
    path = "///.";
    expect = "/";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "///../....//"
     */
    test_num = 48;
    /**/
    path = "///../....//";
    expect = "/";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "./foo//"
     */
    test_num = 49;
    /**/
    path = "./foo//";
    expect = ".";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name ".//foo/."
     */
    test_num = 50;
    /**/
    path = ".//foo/.";
    expect = ".";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * base_name "/foo/bar/baz/zab/rab/oof"
     */
    test_num = 51;
    /**/
    path = "/foo/bar/baz/zab/rab/oof";
    expect = "oof";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * base_name "foo/bar/baz/zab/rab/oof"
     */
    test_num = 52;
    /**/
    path = "foo/bar/baz/zab/rab/oof";
    expect = "oof";
    /**/
    test_base_name(test_num, path, expect);

    /*
     * dir_name "/foo/bar/baz/zab/rab/oof"
     */
    test_num = 53;
    /**/
    path = "/foo/bar/baz/zab/rab/oof";
    expect = "/foo/bar/baz/zab/rab";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * dir_name "foo/bar/baz/zab/rab/oof"
     */
    test_num = 54;
    /**/
    path = "foo/bar/baz/zab/rab/oof";
    expect = "foo/bar/baz/zab/rab";
    /**/
    test_dir_name(test_num, path, expect);

    /*
     * check if "foo/bar/baz" has component "baz"
     */
    path = "foo/bar/baz";
    if (!path_has_component(path, "baz")) {
        err(26, __func__, "path_has_component(\"%s\", \"baz\") returned false: expected true", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"baz\") == true", path);
    }

    /*
     * check if "foo/bar/baz" has component "bar"
     */
    path = "foo/bar/baz";
    if (!path_has_component(path, "bar")) {
        err(27, __func__, "path_has_component(\"%s\", \"bar\") returned false: expected true", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"bar\") == true", path);
    }

    /*
     * check if "foo//bar/baz" has component "bar"
     */
    path = "foo//bar/baz";
    if (!path_has_component(path, "bar")) {
        err(28, __func__, "path_has_component(\"%s\", \"bar\") returned false: expected true", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"bar\") == true", path);
    }

    /*
     * check if "foo//bar/baz//" has component "baz"
     */
    path = "foo//bar/baz";
    if (!path_has_component(path, "baz")) {
        err(29, __func__, "path_has_component(\"%s\", \"baz\") returned false: expected true", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \"baz\") == true", path);
    }

    /*
     * check if "foo//bar/.git//" has component ".git"
     */
    path = "foo//bar/.git//";
    if (!path_has_component(path, ".git")) {
        err(30, __func__, "path_has_component(\"%s\", \".git\") returned false: expected true", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "path_has_component(\"%s\", \".git\") == true", path);
    }
    path = calloc_path("test_ioccc", __FILE__);
    if (path != NULL) {
        errno = 0;  /* pre-clear errno for errp() */
        if (stat(path, &in_st) != 0) {
            errp(31, __func__, "couldn't stat file %s", path);
            not_reached();
        }
        bytes = copyfile(path, "test_file_util.copy.c", true, 0);
        fdbg(stderr, DBG_MED, "copyfile(\"%s\", \"test_file_util.copy.c\", true, 0): %zu bytes", path, bytes);
        errno = 0; /* pre-clear errno for errp() */
        if (stat("test_file_util.copy.c", &out_st) != 0) {
            errp(32, __func__, "couldn't stat file test_file_util.copy.c");
            not_reached();
        }

        if (!is_mode("test_file_util.copy.c", in_st.st_mode)) {
            err(33, __func__, "copyfile() failed to copy st_mode of file %s", path);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "copyfile() successfully copied st_mode of file %s: %04o == %04o", path,
                in_st.st_mode & ~S_IFMT, out_st.st_mode & ~S_IFMT);
        }

        /*
         * delete copied file
         */
        errno = 0;      /* pre-clear errno for errp() */
        if (unlink("test_file_util.copy.c") != 0) {
            errp(34, __func__, "unable to delete file test_file_util.copy.c");
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "sruccessfully deleted copied file test_file_util.copy.c");
        }

        /*
         * now check that the size is empty
         */
        if (!is_empty("test_file_util.copy.c")) {
            errp(35, __func__, "just deleted file test_file_util.copy.c is not empty");
            not_reached();
        }

        /*
         * free path
         */
        free(path);
        path = NULL;
    }

    /*
     * try again but this time with a mode
     */
    path = calloc_path("test_ioccc", __FILE__);
    if (path != NULL) {
        bytes = copyfile(path, "test_file_util.copy.c", false, S_IRUSR|S_IWUSR);
        fdbg(stderr, DBG_MED, "copyfile(\"%s\", \"test_file_util.copy.c\", false, S_IRUSR|S_IWUSR): %zu bytes",
		path, bytes);

        /*
         * verify that the st_mode is correct
         */
        errno = 0;  /* pre-clear errno for errp() */
        if (stat("test_file_util.copy.c", &out_st) != 0) {
            errp(36, __func__, "failed to stat test_file_util.copy.c");
            not_reached();
        }
        if ((out_st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) != (S_IRUSR|S_IWUSR)) {
            err(37, __func__, "copyfile() failed to set S_IRUSR|S_IWUSR on test_file_util.copy.c");
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "copyfile() successfully set S_IRUSR|S_IWUSR on test_file_util.copy.c");
        }

        /*
         * delete copied file
         */
        errno = 0;      /* pre-clear errno for errp() */
        if (unlink("test_file_util.copy.c") != 0) {
            errp(38, __func__, "unable to delete file test_file_util.copy.c");
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "successfully deleted copied file test_file_util.copy.c");
        }

        /*
         * now check that the size is empty
         */
        if (!is_empty("test_file_util.copy.c")) {
            errp(39, __func__, "just deleted file test_file_util.copy.c is not empty");
            not_reached();
        }

        /*
         * free path
         */
        free(path);
        path = NULL;
    }

    /*
     * try one more time but this time make it a binary file
     */
    path = calloc_path("test_ioccc", "test_file_util.o");
    bytes = copyfile(path, "test_file_util.copy.o", true, 0);
    fdbg(stderr, DBG_MED, "copyfile(\"%s\", \"test_file_util.copy.c\", true, 0): %zu bytes", path, bytes);

    /*
     * make sure test_file_util.o st_mode is the same as test_file_util.copy.o
     */
    errno = 0; /* pre-clear errno for errp() */
    if (stat("test_ioccc/test_file_util.o", &in_st) != 0) {
        errp(40, __func__, "failed to stat test_ioccc/test_file_util.o");
        not_reached();
    }

    /*
     * now get copy stat info
     */
    errno = 0;  /* pre-clear errno for errp() */
    if (stat("test_file_util.copy.o", &out_st) != 0) {
        errp(41, __func__, "failed to stat test_file_util.copy.o");
        not_reached();
    }

    if (!is_mode("test_file_util.copy.o", in_st.st_mode)) {
        err(42, __func__, "test_file_util.o st_mode != test_file_util.copy.o st_mode: %04o != %04o",
                in_st.st_mode & ~S_IFMT, out_st.st_mode & ~S_IFMT);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "copyfile() successfully copied st_mode to dest file: %04o == %04o",
                filemode("test_ioccc/test_file_util.o", true), filemode(path, true));
    }

    /*
     * make sure size_if_file() for both test_file_util.o and test_file_util_copy.o are > 0 and the
     * same size and are the same size as in_st.st_size and out_st.size.
     *
     * first obtain size by size_if_file() (of both source and copy):
     */
    size1 = size_if_file("test_ioccc/test_file_util.o");
    size2 = size_if_file("test_file_util.copy.o");

    /*
     * NOTE: if the files did not exist, which we know they do, the
     * size_if_file() will return 0. We know the size MUST be > 0 so we check
     * for > 0 first.
     */
    if (size1 <= 0 || size1 != size2 || size1 != in_st.st_size || size1 != out_st.st_size) {
        err(43, __func__, "size_if_file(\"file_util.o\") != size_if_file(\"test_file_util.copy.o\") or <= or != stat(2) st_size!");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "size_if_file(\"file_util.o\") == size_if_file(\"test_file_util.copy.o\"), "
			      "> 0 and both match stat(2) st_size");
    }

    /*
     * delete copied file
     */
    errno = 0;      /* pre-clear errno for errp() */
    if (unlink("test_file_util.copy.o") != 0) {
        errp(44, __func__, "unable to delete file test_file_util.copy.o");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "successfully deleted copied file test_file_util.copy.c");
    }

    /*
     * now check that the size is empty
     */
    if (!is_empty("test_file_util.copy.o")) {
        errp(45, __func__, "just deleted file test_file_util.copy.o is not empty");
        not_reached();
    }

    /*
     * free path
     */
    free(path);

    /*
     * make some a directory under test_ioccc/ with many subdirectories
     */
    path = "test_ioccc/aa/bb/c/d/e/f/g";

    /*
     * we need to know if this path already exists so we know whether or not to
     * test the modes
     */
    dir_exists = is_dir(path);
    mkdirs(-1, path, 0755);
    if (!exists(path)) {
        err(46, __func__, "%s does not exist", path);
        not_reached();
    } else if (!is_dir(path)) {
        err(47, __func__, "%s is not a directory", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "%s is a directory", path);
    }

    /*
     * test that the mode was correctly set if directory didn't previously exist
     */
    if (!dir_exists) {
        if (!is_mode(path, 0755)) {
            err(48, __func__, "failed to set mode of %s to %o", path, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s is mode 07555", path);
        }
    }

    /*
     * make a single directory under test_ioccc/
     */
    path = "test_ioccc/bb";

    /*
     * we need to know if this path already exists so we know whether or not to
     * test the modes
     */
    dir_exists = is_dir(path);
    mkdirs(-1, path, 0755);
    if (!exists(path)) {
        err(49, __func__, "%s does not exist", path);
        not_reached();
    } else if (!is_dir(path)) {
        err(50, __func__, "%s is not a directory", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "%s is a directory", path);
    }

    /*
     * test that the mode was correctly set if directory didn't previously exist
     */
    if (!dir_exists) {
        if (!is_mode(path, 0755)) {
            err(51, __func__, "failed to set mode of %s to %o", path, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s is mode 07555", path);
        }

        /*
         * also check that has_mode() works with the 0755 mode we set
         */
        if (!has_mode(path, 0755)){
            err(52, __func__, "%s does not have bits %o set", path, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s has bits %o in stat.st_mode",
                path, 0755);
        }

        /*
         * also check specific bits
         */
        if (!has_mode(path, S_IRWXU)) {
            err(53, __func__, "%s does not have bits %o set", path, S_IRWXU);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s has bits %o in stat.st_mode", path, S_IRWXU);
        }

        /*
         * check bits not set
         */
        if (has_mode(path, S_ISUID)) {
            err(54, __func__, "%s has set user id on execution", path);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s does not have set user id on execution", path);
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

/* XXX - start of code that will be replace by table drive walk code - XXX */
    /*
     * test read_fts()
     *
     * NOTE: we will only show path names if debug level is very very high
     * as this would be annoying otherwise (due to there being A LOT of files in
     * test_ioccc/).
     *
     * We will show the filename (full path) and filetype as well (well - at
     * least for regular file, directory and symlink - we will ignore the other
     * kinds)
     *
     * We use logical == true so that broken symlinks can be detected.
     */
    fts.logical = true;
    ent = read_fts("test_ioccc", -1, &cwd, &fts);
    if (ent == NULL) {
        err(55, __func__, "read_fts() returned a NULL pointer on \"test_ioccc\"");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(56, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            switch (ent->fts_info) {
                case FTS_F:
                    fdbg(stderr, DBG_VVHIGH, "%s (file)", p);
                    if (is_file(p)) {
                        fdbg(stderr, DBG_VVVHIGH, "FTS_F %s: is_file(\"%s\") == true", p, p);
                    } else {
                        err(57, __func__, "%s reported as FTS_FILE !is_file(\"%s\")", p, p);
                        not_reached();
                    }
                    break;
                case FTS_D:
                    fdbg(stderr, DBG_VVHIGH, "%s (dir)", p);
                    if (is_dir(p)) {
                        fdbg(stderr, DBG_VVVHIGH, "FTS_D %s: is_dir(\"%s\") == true", p, p);
                    } else {
                        err(58, __func__, "%s reported as FTS_D !is_dir(\"%s\")", p, p);
                        not_reached();
                    }
                    break;
                case FTS_SL:
                    fdbg(stderr, DBG_VVHIGH, "%s (symlink)", p);
                    if (is_symlink(p)) {
                        fdbg(stderr, DBG_VVVHIGH, "FTS_D %s: is_symlink(\"%s\") == true", p, p);
                    } else {
                        err(59, __func__, "%s reported as FTS_SL !is_symlink(\"%s\")", p, p);
                        not_reached();
                    }
                    break;
                case FTS_SLNONE:
                    fdbg(stderr, DBG_VVHIGH, "%s (broken symlink)", p);
                    break;
                case FTS_NS:
                    err(60, __func__, "no stat(2) info available for path %s in tree", p);
                    not_reached();
                    break;
                case FTS_NSOK:
                    err(61, __func__, "encountered FTS_NSOK for path %s in tree: FTS_NOSTAT set!", p);
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
    ent = read_fts("test_ioccc", -1, &cwd, &fts);
    if (ent == NULL) {
        err(62, __func__, "read_fts() returned a NULL pointer on \"test_ioccc\" for directories");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(63, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            if (ent->fts_info != FTS_D && ent->fts_info != FTS_DP) {
                err(64, __func__, "%s is not a directory", p);
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
    append_path(&(fts.ignore), "test*", true, false, true, false);
    ent = read_fts(".", -1, &cwd, &fts);
    if (ent == NULL) {
        err(65, __func__, "read_fts() returned a NULL pointer on \".\" for directories");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(66, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            if (ent->fts_info != FTS_F) {
                err(67, __func__, "%s is not a file", p);
                not_reached();
            } else if (!strncmp(p, "test",4)) {
                err(68, __func__, "found file meant to be ignored: %s", p);
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
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_FILE;
    fts.base = true; /* important */
    fts.match_case = false;
    fts.fn_match_flags = 0;
    append_path(&(fts.match), "test*", true, false, true, false);
    ent = read_fts(".", -1, &cwd, &fts);
    if (ent == NULL) {
        err(69, __func__, "read_fts() returned a NULL pointer on \".\" for directories");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(70, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            if (ent->fts_info != FTS_F) {
                err(71, __func__, "%s is not a file", p);
                not_reached();
            } else if (strncmp(p, "test", 4) != 0) {
                err(72, __func__, "found file that should not have matched: %s", p);
                not_reached();
            } else {
                fdbg(stderr, DBG_MED, "%s is a matched file", p);
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
     * as this would be annoying otherwise (due to there being A LOT of files in
     * test_ioccc/).
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
    ent = read_fts("test_ioccc", -1, &cwd, &fts);
    if (ent == NULL) {
        err(73, __func__, "read_fts() returned a NULL pointer on \"test_ioccc\"");
        not_reached();
    } else {
        do {
            char *p = fts_path(ent);
            if (ent == NULL) {
                err(74, __func__, "fts_path(ent) returned NULL");
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
                    err(75, __func__, "no stat(2) info available");
                    not_reached();
                    break;
                case FTS_NSOK:
                    err(76, __func__, "encountered FTS_NSOK for path %s in tree: FTS_NOSTAT set!", p);
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
     * reset fts again
     */
    reset_fts(&fts, false, false); /* don't clear out ignore or match list */
    fts.options = FTS_NOCHDIR; /* redundant */
    fts.logical = false;
    fts.type = FTS_TYPE_FILE;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;

    /*
     * now try and find a file called "jparse_test.sh"
     */
    fname = find_path("jparse_test.sh", NULL, -1, &cwd, false, &fts);
    if (fname != NULL) {
        fdbg(stderr, DBG_MED, "full path of jparse_test.sh: %s", fname);

        /*
         * try and open file from current directory
         */
        errno = 0; /* pre-clear errno for warnp() */
        fp = fopen(fname, "r");
        if (fp == NULL) {
            warnp(__func__, "failed to open %s for reading", fname);
        } else {
            fdbg(stderr, DBG_MED, "successfully opened %s for reading", fname);

            /*
             * now close it
             */
            errno = 0; /* pre-clear errno for warnp() */
            if (fclose(fp) != 0) {
                warnp(__func__, "failed to close %s", fname);
            } else {
                fdbg(stderr, DBG_MED, "successfully closed %s", fname);
            }
        }
    } else {
        warn(__func__, "couldn't find file jparse_test.sh: #0");
    }

    /*
     * reset fts
     */
    reset_fts(&fts, false, false); /* don't clear out ignore or match list */
    fts.options = FTS_NOCHDIR; /* redundant */
    fts.type = FTS_TYPE_FILE;
    fts.count = 1;
    fts.depth = 1;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;

    /*
     * now try and find a file called "jparse_test.sh" in "jparse/"
     */
    fname = find_path("jparse_test.sh", "jparse/test_jparse", -1, &cwd, false, &fts);
    if (fname != NULL) {

        /*
         * try and open file from (now) current directory
         */
        errno = 0; /* pre-clear errno for warnp() */
        fp = fopen(fname, "r");
        if (fp == NULL) {
            warnp(__func__, "failed to open %s for reading", fname);
        } else {
            fdbg(stderr, DBG_MED, "successfully opened %s for reading", fname);

            /*
             * now close it
             */
            errno = 0; /* pre-clear errno for warnp() */
            if (fclose(fp) != 0) {
                warnp(__func__, "failed to close %s", fname);
            } else {
                fdbg(stderr, DBG_MED, "successfully closed %s", fname);
            }
        }
    } else {
        warn(__func__, "couldn't find file jparse/jparse_test.sh");
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * reset fts
     */
    reset_fts(&fts, false, false); /* don't clear out ignore or match lists */
    fts.options = FTS_NOCHDIR; /* redundant */
    fts.type = FTS_TYPE_DIR;
    fts.count = 0;
    fts.depth = 0;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    /*
     * now try and find a directory called "bad" under the parent directory of
     * "test_ioccc". Since it is in the ignored list and the depth is 0 and the
     * fts.base is true this should NOT return NULL.
     */
    fname = find_path("bad", NULL, -1, &cwd, false, &fts);
    if (fname == NULL) {
        err(77, __func__, "unable to find directory \"bad\"");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "successfully ignored directories in ignore list");
    }

    /*
     * reset fts
     */
    reset_fts(&fts, false, false); /* don't clear out ignore or match lists */
    fts.options = FTS_NOCHDIR; /* redundant */
    fts.type = FTS_TYPE_DIR;
    fts.count = 0;
    fts.depth = 0;
    fts.base = false;
    fts.seedot = false;
    fts.match_case = true;

    /*
     * now try and find a directory called "bad" under the parent directory of
     * "test_ioccc". Since it is in the ignored list and the depth is 0 and the
     * fts.base is false this SHOULD return NULL.
     */
    fname = find_path("bad", NULL, -1, &cwd, false, &fts);
    if (fname != NULL) {
        err(78, __func__, "found ignored directory \"bad\"");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "successfully ignored directories in ignore list");
    }

    /*
     * look for a file called "bad" which should fail because we are ignoring it
     */
    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    fts.depth = 0;
    fname = find_path("bad", "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(79, __func__, "found paths when none should have been found");
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
     * look for a file called "test_ioccc" which should succeed because it's
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
    fname = find_path("test_ioccc", NULL, -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(80, __func__, "found paths when none should have been found");
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

    /*
     * filenames
     */
    append_path(&paths, "file_util.o", true, false, true, false);
    append_path(&paths, "file_util.c", true, false, true, false);
    append_path(&paths, "file_util.h", true, false, true, false);
    append_path(&paths, "test_file_util.c", true, false, true, false);
    append_path(&paths, "test_file_util.o", true, false, true, false);

    /*
     * directories
     */
    append_path(&paths, "test_ioccc", true, false, true, false);
    append_path(&paths, "test_ioccc/a", true, false, true, false);
    append_path(&paths, "test_ioccc/test_JSON", true, false, true, false);

    /*
     * special checks: . and ..
     */
    append_path(&paths, ".", true, false, true, false);
    append_path(&paths, "..", true, false, true, false);

    /*
     * path that doesn't exist (or shouldn't)
     */
    append_path(&paths, "foo.bar.baz", true, false, true, false);

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
        err(81, __func__, "didn't find any paths in the paths array");
        not_reached();
    }

    /*
     * look for test_ioccc
     */
    name = find_path_in_array("test_ioccc", paths, false, false, &idx);
    if (name == NULL) {
        /*
         * "test_ioccc" must exist
         */
        err(82, __func__, "didn't find \"test_ioccc\" in paths array");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "found test_ioccc at paths[%ju]", (uintmax_t)idx);
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
                err(83, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", j, name);
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
     * reset fts but this time without basename. This should make it so that the
     * directories in the ignored list are not actually ignored.
     */
    reset_fts(&fts, false, false); /* don't clear out ignore or match lists */
    fts.options = FTS_NOCHDIR; /* redundant */
    fts.type = FTS_TYPE_DIR;
    fts.count = 0;
    fts.depth = 0;
    fts.base = false;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(84, __func__, "didn't find any paths in the paths array");
        not_reached();
    } else {
        /*
         * make sure that the paths in the ignored list are actually correct
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            char *u = dyn_array_value(paths_found, char *, j);
            if (u == NULL) {
                err(85, __func__, "NULL found in paths_found[%ju]", j);
                not_reached();
            }
        }
    }

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
        err(86, __func__, "didn't find any paths in the paths array");
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
                err(87, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", j, name);
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
        err(88, __func__, "didn't find any files in the paths array");
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
                err(89, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", j, name);
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
     * test find_paths() with basename and depth of 2 and match_case
     */
    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.logical = false;
    fts.base = true;
    fts.depth = 2;
    fts.match_case = true;
    paths_found = find_paths(paths, NULL, -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(90, __func__, "didn't find any files in the paths array");
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
                err(91, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", j, name);
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
     * test find_paths() but this time from test_ioccc with basename and at
     * depth 1 and check as NULL (this will set it to check_fts_info)
     */

    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.depth = 1;
    fts.check = NULL;
    fts.logical = false;
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(92, __func__, "didn't find any files in the paths array");
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
                err(93, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", j, name);
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", j, name);
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", j, name);
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
        err(94, __func__, "found paths when none should have been found");
        not_reached();
    }

    /*
     * test find_paths() looking for every file called README.md; this should
     * succeed because it's not in the ignore list.
     */
    append_path(&paths, "README.md", true, false, true, false);

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
        err(95, __func__, "unable to find any path in the directory");
        not_reached();
    }

    /*
     * special test: this time we will look for every single file by making
     * an empty path string and FTS_TYPE_ANY
     */
    append_path(&paths, "", true, false, false, false);

    /*
     * test find_paths() looking for every file under test_ioccc/
     */

    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(96, __func__, "didn't find any paths in the paths array");
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
                err(97, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                    ++dirs;
                } else if (is_file(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a regular file", j, name);
                    ++files;
                } else if (is_symlink(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a symlink", j, name);
                    ++symlinks;
                } else {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s", j, name);
                    ++others;
                }
            }
        }
        fdbg(stderr, DBG_MED, "a total of %zu director%s in paths_found", dirs, dirs==1?"y":"ies");
        fdbg(stderr, DBG_MED, "a total of %zu file%s in paths_found", files, files==1?"":"s");
        fdbg(stderr, DBG_MED, "a total of %zu symlink%s in paths_found", symlinks, symlinks==1?"":"s");
        fdbg(stderr, DBG_MED, "a total of %zu other file type%s in paths_found", others, others==1?"":"s");
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
     * test find_paths() looking for directories under test_ioccc/a with a min
     * depth of 4 and a max depth of 5
     */

    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    fts.type = FTS_TYPE_DIR;
    fts.depth = 0;
    fts.min_depth = 4;
    fts.max_depth = 5;
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(98, __func__, "didn't find any paths in the paths array");
        not_reached();
    }

    /*
     * show what we found
     */
    len = paths_in_array(paths_found);
    if (len > 0) {
        size_t dirs = 0; /* directories */
        for (j = 0; j < len; ++j) {
            /* get next string pointer */
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {	/* paranoia */
                err(99, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                    ++dirs;
                }
            }
        }
        fdbg(stderr, DBG_MED, "a total of %zu director%s in paths_found under test_ioccc with min/max depth of 4/5",
            dirs, dirs==1?"y":"ies");
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
     * test find_paths() looking for directories under test_ioccc/a with a min
     * depth of 0 and a max depth of 3
     */

    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    fts.type = FTS_TYPE_DIR;
    fts.depth = 0;
    fts.min_depth = 0;
    fts.max_depth = 3;
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(100, __func__, "didn't find any paths in the paths array");
        not_reached();
    }

    /*
     * show what we found
     */
    len = paths_in_array(paths_found);
    if (len > 0) {
        size_t dirs = 0; /* directories */
        for (j = 0; j < len; ++j) {
            /* get next string pointer */
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {	/* paranoia */
                err(101, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                    ++dirs;
                }
            }
        }
        fdbg(stderr, DBG_MED, "a total of %zu director%s in paths_found under test_ioccc with min/max depth of 0/3",
            dirs, dirs==1?"y":"ies");
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
     * test find_paths() looking for directories under test_ioccc/a with a min
     * depth of 0 and a max depth of 0.
     *
     * NOTE: this disables the check.
     */
    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    fts.type = FTS_TYPE_DIR;
    fts.depth = 0;
    fts.min_depth = 0;
    fts.max_depth = 0;
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found == NULL) {
        err(102, __func__, "didn't find any paths in the paths array");
        not_reached();
    }

    /*
     * show what we found
     */
    len = paths_in_array(paths_found);
    if (len > 0) {
        size_t dirs = 0; /* directories */
        for (j = 0; j < len; ++j) {
            /* get next string pointer */
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {	/* paranoia */
                err(103, __func__, "found NULL pointer at paths_found[%ju]", j);
                not_reached();
            }

            /*
             * if dbg level DBG_MED is allowed, report what we found
             */
            if (dbg_allowed(DBG_MED)) {
                if (is_dir(name)) {
                    fdbg(stderr, DBG_MED, "paths_found[%ju]: %s is a directory", j, name);
                    ++dirs;
                }
            }
        }
        fdbg(stderr, DBG_MED, "a total of %zu director%s in paths_found under test_ioccc with min/max depth of 0/0",
            dirs, dirs==1?"y":"ies");
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
     * test find_paths() looking for directories under test_ioccc/a with a min
     * depth of 7 and a max depth of 3.
     *
     * NOTE: that is not a typo; we're testing bogus ranges.
     */

    /*
     * reset fts
     */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    fts.type = FTS_TYPE_DIR;
    fts.depth = 0;
    fts.min_depth = 7;
    fts.max_depth = 3;
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(104, __func__, "bogus range of min/max directories found directories");
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
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(105, __func__, "found unexpected socket under test_ioccc/");
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
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(106, __func__, "found unexpected character device under test_ioccc/");
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
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(107, __func__, "found unexpected block device under test_ioccc/");
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
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        err(108, __func__, "found unexpected FIFO under test_ioccc/");
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
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {

        /*
         * first show the files
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {
                err(109, __func__, "found NULL pointer in paths_found (not file, directory or symlink) array");
                not_reached();
            }
            warn(__func__, "path is not a file, directory or symlink: %s", name);
        }
        /*
         * make it an error
         */
        err(110, __func__, "found unexpected file type under test_ioccc/");
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
     * now append "foo.bar.baz" to the array
     */
    append_path(&paths, "foo.bar.baz", true, false, true, false);

    /*
     * create a file called foo.bar.baz
     */
    path = "foo.bar.baz";
    touch(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (!exists(path)) {
        err(111, __func__, "file %s does not exist after touch()", path);
        not_reached();
    }

    /*
     * check for "foo.bar.baz" by case-sensitive
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
                err(112, __func__, "found NULL pointer in paths_found (any) array");
                not_reached();
            }
            if (strcmp(path, name) != 0) {
                err(113, __func__, "found non-matching file in list: %s != %s", name, path);
                not_reached();
            }
            fdbg(stderr, DBG_MED, "found file %s as case-sensitive search", name);
        }
    } else {
        err(114, __func__, "couldn't find any file called \"%s\" as case-sensitive search", path);
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
     * check for "foo.bar.baz" by case-insensitive
     */
    path = "foo.bar.baz"; /* paranoia */
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
                err(115, __func__, "found NULL pointer in paths_found (any) array");
                not_reached();
            }
            if (strcasecmp(path, name) != 0) {
                err(116, __func__, "found non-matching file in list: %s != %s", name, path);
                not_reached();
            }

            fdbg(stderr, DBG_MED, "found %s by case-insensitive search", name);
        }
    } else {
        err(117, __func__, "couldn't find any file called \"%s\" by case-insensitive search", path);
        not_reached();
    }

    /*
     * delete foo.bar.baz
     */
    errno = 0;      /* pre-clear errno for errp() */
    if (unlink(path) != 0) {
        if (errno != ENOENT) {
            errp(118, __func__, "unable to delete file %s", path);
            not_reached();
        }
    } else {
        fdbg(stderr, DBG_MED, "successfully deleted created file %s", path);
    }
    /*
     * now check that the size is empty
     */
    if (!is_empty(path)) {
        errp(119, __func__, "just deleted file %s is not empty", path);
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
     * NOTE: we also use path here.
     */
    path = "Foo.bar.baz";

    /*
     * create a file called Foo.bar.baz
     */
    touch(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    /*
     * make sure it exists
     */
    if (!exists(path)) {
        err(120, __func__, "file %s does not exist", path);
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
     * now append "foobarbarbaz" to the array
     */
    append_path(&paths, "foo.bar.baz", true, false, true, false);

    /*
     * check for "foo.bar.baz" by case-sensitive
     *
     * NOTE: we created a file Foo.bar.baz not foo.bar.baz.
     */

    /*
     * reset fts
     */
    reset_fts(&fts, true, true); /* clear out ignore and match lists */
    fts.type = FTS_TYPE_ANY; /* actual default */
    fts.base = true;
    fts.seedot = false;
    fts.match_case = true;
    paths_found = find_paths(paths, "test_ioccc", -1, &cwd, false, &fts);
    if (paths_found != NULL) {
        /*
         * first show the files
         */
        len = paths_in_array(paths_found);
        for (j = 0; j < len; ++j) {
            name = dyn_array_value(paths_found, char *, j);
            if (name == NULL) {
                err(121, __func__, "found NULL pointer in paths_found (any) array");
                not_reached();
            }
            fdbg(stderr, DBG_MED, "found file %s as case-sensitive search", name);
        }
    } else {
        /*
         * this can fail on some file systems but not all
         */
        fdbg(stderr, DBG_MED, "couldn't find any file called \"foo.bar.baz\" as case-sensitive search");
    }

    /*
     * restore earlier directory that might have happened with read_fts()
     *
     * NOTE: this will close cwd so it will no longer be valid (that does not
     * mean it can't be reset)
     */
    (void) read_fts(NULL, -1, &cwd, NULL);

    /*
     * check for "foo.bar.baz" by case-insensitive.
     *
     * NOTE: we created Foo.bar.baz, not Foo.bar.baz.
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
                err(122, __func__, "found NULL pointer in paths_found array");
                not_reached();
            }
            if (strcasecmp(path, name) != 0) {
                err(123, __func__, "found non-matching file: %s != %s", name, path);
                not_reached();
            }
            fdbg(stderr, DBG_MED, "found %s by case-insensitive search", name);
        }
    } else {
        /*
         * as it is a case-insensitive it should always succeed
         */
        err(124, __func__, "couldn't find any file called %s by case-insensitive search", path);
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
    fname = find_path(path, NULL, -1, NULL, true, &fts);
    if (fname != NULL) {
        fdbg(stderr, DBG_MED, "found %s at %s", path, fname);
        free(fname);
        fname = NULL;
    } else {
        err(125, __func__, "couldn't find %s in tree", path);
        not_reached();
    }

    /*
     * delete Foo.bar.baz
     */
    errno = 0;      /* pre-clear errno for errp() */
    if (unlink(path) != 0) {
        if (errno != ENOENT) {
            errp(126, __func__, "unable to delete file %s", path);
            not_reached();
        }
    } else {
        fdbg(stderr, DBG_MED, "successfully deleted created file %s", path);
    }

    /*
     * now check that the size is empty
     */
    if (!is_empty(path)) {
        errp(128, __func__, "just deleted file %s is not empty", path);
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
/* XXX - end of code that will be replace by table drive walk code - XXX */

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(0);	/*ooo*/
}
