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
    int_least32_t test_num = 0;		/* canon_path test number */
    /**/
    char *path = NULL;			/* path to process */
    char *expect = NULL;		/* expected process result */
    /**/
    size_t bytes = 0;
    struct stat in_st;                  /* verify copyfile() sets correct modes */
    struct stat out_st;                 /* output file stat(2) */
    bool dir_exists = false;            /* true ==> directory already exists (for testing modes) */
    off_t size1 = 0;			/* for checking size_if_file() */
    off_t size2 = 0;			/* for checking size_if_file() */
    struct json *tree = NULL;           /* check that the jparse.json file is valid JSON */
    int ret;
    int i;

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
     * try again but this time with a mode
     */
    path = calloc_path("test_ioccc", __FILE__);
    if (path != NULL) {

	/*
	 * pre-remove test_file_util.o
	 */
	if (unlink("test_file_util.copy.c") != 0) {
	    fdbg(stderr, DBG_HIGH, "no test_file_util.copy.c to delete: this is OK");
	} else {
	    fdbg(stderr, DBG_MED, "successfully deleted copied file test_file_util.copy.c");
	}

	/*
	 * form a new test_file_util.copy.c
	 */
        bytes = copyfile(path, "test_file_util.copy.c", false, S_IRUSR|S_IWUSR);
        fdbg(stderr, DBG_MED, "copyfile(\"%s\", \"test_file_util.copy.c\", false, S_IRUSR|S_IWUSR): %zu bytes",
		path, bytes);

        /*
         * verify that the st_mode is correct
         */
        errno = 0;  /* pre-clear errno for errp() */
        if (stat("test_file_util.copy.c", &out_st) != 0) {
            errp(26, __func__, "failed to stat test_file_util.copy.c");
            not_reached();
        }
        if ((out_st.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO)) != (S_IRUSR|S_IWUSR)) {
            err(27, __func__, "copyfile() failed to set S_IRUSR|S_IWUSR on test_file_util.copy.c");
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "copyfile() successfully set S_IRUSR|S_IWUSR on test_file_util.copy.c");
        }

        /*
         * delete copied file
         */
        errno = 0;      /* pre-clear errno for errp() */
        if (unlink("test_file_util.copy.c") != 0) {
            errp(28, __func__, "unable to delete file test_file_util.copy.c");
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "successfully deleted copied file test_file_util.copy.c");
        }

        /*
         * now check that the size is empty
         */
        if (!is_empty("test_file_util.copy.c")) {
            errp(29, __func__, "just deleted file test_file_util.copy.c is not empty");
            not_reached();
        }

        /*
         * free path
         */
        free(path);
        path = NULL;
    }

    /*
     * pre-remove test_file_util.o
     */
    if (unlink("test_file_util.copy.o") != 0) {
	fdbg(stderr, DBG_HIGH, "no test_file_util.copy.o to delete: this is OK");
    } else {
	fdbg(stderr, DBG_MED, "successfully deleted copied file test_file_util.copy.o");
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
        errp(30, __func__, "failed to stat test_ioccc/test_file_util.o");
        not_reached();
    }

    /*
     * now get copy stat info
     */
    errno = 0;  /* pre-clear errno for errp() */
    if (stat("test_file_util.copy.o", &out_st) != 0) {
        errp(31, __func__, "failed to stat test_file_util.copy.o");
        not_reached();
    }

    if (!is_mode("test_file_util.copy.o", in_st.st_mode)) {
        err(32, __func__, "test_file_util.o st_mode != test_file_util.copy.o st_mode: %04o != %04o",
                in_st.st_mode & ~S_IFMT, out_st.st_mode & ~S_IFMT);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "copyfile() successfully copied st_mode to dest file: %04o == %04o",
                filemode("test_ioccc/test_file_util.o"), filemode(path));
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
        err(33, __func__, "size_if_file(\"file_util.o\") != size_if_file(\"test_file_util.copy.o\") or <= or != stat(2) st_size!");
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
        errp(34, __func__, "unable to delete file test_file_util.copy.o");
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "successfully deleted copied file test_file_util.copy.c");
    }

    /*
     * now check that the size is empty
     */
    if (!is_empty("test_file_util.copy.o")) {
        errp(35, __func__, "just deleted file test_file_util.copy.o is not empty");
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
        err(36, __func__, "%s does not exist", path);
        not_reached();
    } else if (!is_dir(path)) {
        err(37, __func__, "%s is not a directory", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "%s is a directory", path);
    }

    /*
     * test that the mode was correctly set if directory didn't previously exist
     */
    if (!dir_exists) {
        if (!is_mode(path, 0755)) {
            err(38, __func__, "failed to set mode of %s to %o", path, 0755);
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
        err(39, __func__, "%s does not exist", path);
        not_reached();
    } else if (!is_dir(path)) {
        err(40, __func__, "%s is not a directory", path);
        not_reached();
    } else {
        fdbg(stderr, DBG_MED, "%s is a directory", path);
    }

    /*
     * test that the mode was correctly set if directory didn't previously exist
     */
    if (!dir_exists) {
        if (!is_mode(path, 0755)) {
            err(41, __func__, "failed to set mode of %s to %o", path, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s is mode 07555", path);
        }

        /*
         * also check that has_mode() works with the 0755 mode we set
         */
        if (!has_mode(path, 0755)){
            err(42, __func__, "%s does not have bits %o set", path, 0755);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s has bits %o in stat.st_mode",
                path, 0755);
        }

        /*
         * also check specific bits
         */
        if (!has_mode(path, S_IRWXU)) {
            err(43, __func__, "%s does not have bits %o set", path, S_IRWXU);
            not_reached();
        } else {
            fdbg(stderr, DBG_MED, "%s has bits %o in stat.st_mode", path, S_IRWXU);
        }

        /*
         * check bits not set
         */
        if (has_mode(path, S_ISUID)) {
            err(44, __func__, "%s has set user id on execution", path);
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
        fdbg(stderr, DBG_HIGH, "/dev/null is mode: %o", filemode("/dev/null"));
    } else {
        fdbg(stderr, DBG_MED, "/dev/null is NOT a character device");
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(0);	/*ooo*/
}
