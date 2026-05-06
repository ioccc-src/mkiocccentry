/*
 * jval - use XPath for JSON to analyse, transform and selectively extract data from JSON
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * Copyright (c) 2026 by Cody Boone Ferguson and Landon Curt Noll. All
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
 * This JSON parser, library and tools were co-developed in 2022-2026 by Cody Boone
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
#include <unistd.h>
#include <locale.h>
#include <string.h>

/*
 * jval - XXX - fill out - XXX
 */
#include "jparse_main.h"


/*
 * definitions
 *
 */
#define REQUIRED_ARGS (2)	/* number of required arguments on the command line */

/*
 * usage message
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-q] [-V] [-use-jparse] file.json pattern\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )\n"
    "\t-V\t\tprint version strings and exit\n"
    "\n"
    "\t-use-jparse\tDo nothing: verify this tool came from the jparse toolset.\n"
    "\n"
    "\tfile.json\tJSON input file\n"
    "\t\t\tNOTE: if file.json is - (dash) read from standard input.\n"
    "\t\t\tNOTE: if file.json is . (dot) do not read any file, perform an internal test\n"
    "\t\t\tand exit accordingly.\n"
    "\tpattern\t\tXPath Expression\n"
    "\n"
    "Exit codes:\n"
    "    0\tJSON is valid\n"
    "    1\tJSON is invalid\n"
    "    2\t-h and help string printed or -V and version strings printed\n"
    "    3\tcommand line error\n"
    "    >=4\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";


/*
 * static functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


int
main(int argc, char **argv)
{
    char const *program = NULL;	    /* our name */
    char const *json_path = NULL;   /* file.json */
    char const *pattern = NULL;     /* XPath pattern */
    extern char *optarg;	    /* option argument */
    extern int optind;		    /* argv index of the next arg */
    bool valid_json = false;	    /* true ==> JSON parse was valid */
    int exit_code = 0;              /* exit code depends on if any JSON is invalid */
    struct json *tree = NULL;	    /* JSON parse tree or NULL */
    bool opt_error = false;		/* fchk_inval_opt() return */
    int i;

    /*
     * use default locale based on LANG
     */
    (void) setlocale(LC_ALL, "");

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:qVJ:u:")) != -1) {
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
        case 'u':
            if (strcmp(optarg, "se-jparse")) {
                usage(3, program, "invalid command line"); /*ooo*/
                not_reached();
            }
            break;
	case 'J': /* -J json_verbosity_level */
	    /*
	     * parse json verbosity level
	     */
	    json_verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -J json_verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version strings and exit */
	    print("%s version: %s\n", JPARSE_BASENAME, JPARSE_TOOL_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
            if (optarg) abort();
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
    if (argc - optind < REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    json_path = argv[optind];
    pattern = argv[optind + 1];

    if (json_path == NULL || *json_path == '\0') {
	usage(3, program, "json_path NULL or empty"); /*ooo*/
	not_reached();
    }
    if (pattern == NULL || *pattern == '\0') {
	usage(3, program, "pattern NULL or empty"); /*ooo*/
	not_reached();
    }
    dbg(DBG_HIGH, "Calling parse_json_file(\"%s\", &valid_json):", json_path);
    tree = parse_json_file(json_path, &valid_json);
    if (tree == NULL || !valid_json) {
        warn(program, "JSON parse tree is NULL for file: %s", json_path);
        exit_code = 1;
    }
    /*
     * free the JSON parse tree
     */
    else {
        dbg(DBG_HIGH, "%s is valid JSON", strcmp(json_path,"-")?json_path:"stdin");
        /*
         * XXX - write the XPath code here (or call a function to do so), before
         * freeing the tree - XXX
         */
        json_tree_free(tree, JSON_INFINITE_DEPTH);
        free(tree);
        tree = NULL;
    }

    /*
     * firewall - JSON parser must have returned a valid JSON parse tree
     */
    if (exit_code != 0) {
	err(1, program, "invalid JSON"); /*ooo*/
	not_reached();
    }

    if (verbosity_level > 0) {
	msg("valid JSON");
    }

    exit(0); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program,  "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	program		our program name
 *	str		top level usage message
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 *
 */
static void
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = JPARSE_BASENAME;
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog,
		  DBG_DEFAULT, JSON_DBG_DEFAULT, JPARSE_BASENAME, JPARSE_TOOL_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION,
                  JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
