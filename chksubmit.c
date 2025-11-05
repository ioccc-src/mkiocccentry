/*
 * chksubmit - check submission directory via chkentry -S
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * Copyright (c) 2025 by Cody Boone Ferguson and Landon Curt Noll.
 * All Rights Reserved.
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
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This tool and the JSON parser were co-developed in 2022-2025 by Cody Boone
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
 * The concept of the JSON semantics tables was developed by Landon Curt Noll.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

/*
 * chksubmit - run chkentry -S
 */
#include "chksubmit.h"


#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/*
 * globals
 */
static bool quiet = false;		    /* true ==> quiet mode */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-C chkentry] [-q] submission_dir\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: 0)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-C chkentry\tpath to chkentry\n"
    "\t-q\t\tquiet mode (def: loud :-) )\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\n"
    "\tsubmission_dir\tsubmission directory to be checked\n"
    "\n"
    "Exit codes:\n"
    "    0\t\tall is OK\n"
    "    1\t\tchkentry exited 1 due to some submission directory check failure\n"
    "    1\t\ta JSON file is not valid JSON, a semantics test failed or some other test failed\n"
    "    2\t\t-h and help string printed or -V and version string printed\n"
    "    3\t\tcommand line error\n"
    "    >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";


/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));
static void chksubmit_sanity_chks(char const *chkentry);
/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message
 *	prog		our program name
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
	prog = CHKSUBMIT_BASENAME;
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog,
	    CHKSUBMIT_BASENAME, CHKSUBMIT_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * chksubmit_sanity_chks - perform basic sanity checks
 *
 * We perform basic sanity checks on chkentry path.
 *
 * given:
 *
 *      chkentry        - path to chkentry
 *
 * NOTE: this function does not return on error or if things are not sane.
 *
 * NOTE: it is probably wisest to let find_utils() do its thing rather than
 * specify the path with -C.
 */
static void
chksubmit_sanity_chks(char const *chkentry)
{
    /*
     * firewall
     */
    if (chkentry == NULL) {
	err(12, __func__, "called with NULL chkentry");
	not_reached();
    }

    if (!exists(chkentry)) {
        fpara(stderr,
              "",
              "We cannot find chkentry.",
              "",
              "The chkentry program is required to run this tool.",
              "",
              "Perhaps you need to use:",
              "",
              "    chksubmit -C /path/to/chkentry [...]",
              "",
              "or install the mkiocccentry toolkit? You can find the mkiocccentry toolkit at:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
              "",
              NULL);
        err(13, __func__, "chkentry does not exist: %s", chkentry);
        not_reached();
    }
    if (!is_file(chkentry)) {
        fpara(stderr,
              "",
              "The chkentry path, while it exists, is not a regular file.",
              "",
              "Perhaps you need to use another path:",
              "",
              "    chksubmit -C /path/to/chkentry [...]",
              "",
              "or install the mkiocccentry toolkit?",
              "",
              NULL);
        err(14, __func__, "chkentry is not a regular file: %s", chkentry);
        not_reached();
    }

    if (!is_exec(chkentry)) {
        fpara(stderr,
              "",
              "The chkentry path, while it is a file, is not executable.",
              "",
              "We suggest you check the permissions on the chkentry program, or use another path:",
              "",
              "    chksubmit -C /path/to/chkentry [...]",
              "",
              "or install the mkiocccentry toolkit.",
              "",
              NULL);
        err(15, __func__, "chkentry is not an executable program: %s", chkentry);
        not_reached();
    }

    return;
}


int
main(int argc, char *argv[])
{
    char const *program = NULL;		/* our name */
    char *submission_dir = NULL;        /* directory from which files are to be checked */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    int exit_code = 0;                  /* return value of chkentry -S */
    bool found_chkentry = false;        /* for find_utils */
    char *chkentry = NULL;              /* for find_utils */
    char v_str[INT_DECIMAL_SIZE+1+1];	/* verbosity level as a string + NUL + 1 for paranoia */
    bool opt_error = false;		/* fchk_inval_opt() return */
    int i = 0;

    /* IOCCC requires use of C locale */
    set_ioccc_locale();

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:VqC:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, "");	/*ooo*/
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
	    print("%s version: %s\n", CHKSUBMIT_BASENAME, CHKSUBMIT_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    exit(2);		/*ooo*/
	    not_reached();
	    break;
        case 'C': /* -C chkentry */
            chkentry = optarg;
            break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
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

    /*
     * we need chkentry if the user did not specify the path
     */
    if (chkentry == NULL) {
        find_utils(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &found_chkentry, &chkentry,
                   NULL, NULL, NULL, NULL, NULL, NULL);
    }

    /*
     * verify chkentry is an executable program
     */
    chksubmit_sanity_chks(chkentry);

    argc -= optind;
    argv += optind;
    switch (argc) {
    case 1:
        submission_dir = argv[0];
	break;
    case 2:
        if (!strcmp(argv[0], ".") && !strcmp(argv[1], ".")) {
            vrergfB(-1, -1); /* Easter egg */
            not_reached();
        }
        /*fallthrough*/
    default:
	usage(3, program, "wrong number of arguments");	/*ooo*/
	not_reached();
	break;
    }

    /*
     * debugging
     */
    if (dbg_allowed(DBG_MED)) {
        if (quiet) {
            dbg(DBG_MED, "about to execute: %s -S -v %d -q -- %s", chkentry, verbosity_level, submission_dir);
        } else {
            dbg(DBG_MED, "about to execute: %s -S -v %d -- %s", chkentry, verbosity_level, submission_dir);
        }
    }

    /*
     * execute chkentry -S [-v level] [-q]
     */
    if (snprintf(v_str, sizeof(v_str)-2, "%d", verbosity_level) <= 0) {
	err(16, program, "failed to convert int: %d into decimal", i);
	not_reached();
    }
    v_str[sizeof(v_str)-1] = '\0';	/* paranoia */
    if (quiet) {
	if (is_dbg_enabled) {
	    exit_code = shell_cmd(__func__, false, true, "% -S -v % -q -- %", chkentry, v_str, submission_dir);
	} else {
	    exit_code = shell_cmd(__func__, false, true, "% -S -q -- %", chkentry, submission_dir);
	}
    } else {
	if (is_dbg_enabled) {
	    exit_code = shell_cmd(__func__, false, true, "% -S -v % -- %", chkentry, v_str, submission_dir);
	} else {
	    exit_code = shell_cmd(__func__, false, true, "% -S -- %", chkentry, submission_dir);
	}
    }

    /*
     * free storage
     */
    if (chkentry != NULL && found_chkentry) {
        free(chkentry);
        chkentry = NULL;
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     *
     */
    if (exit_code != 0) {
        exit(1); /*ooo*/
        not_reached();
    }
    exit(0); /*ooo*/
}
