/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 *
 * "Because most people become disappointed when someone gets their name wrong." :-)
 *
 * Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.
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
 * This tool was co-developed in 2022 by Cody Boone Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <locale.h>

/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 */
#include "fnamchk.h"

/*
 * location - location/country codes and set the IOCCC locale
 */
#include "../soup/location.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-E ext] [-t|-u] [-T] filepath\n"
    "\n"
    "\t-h\t\t\tprint help message and exit\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-q\t\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\t-E ext\t\t\tchange extension to test (def: txz)\n"
    "\t-t\t\t\tfilename must match test submit filename\n"
    "\t-u\t\t\tfilename must match real submit filename\n"
    "\t\t\t\t\tNOTE: -t and -u cannot be used together.\n"
    "\t-T\t\t\tignore timestamp check result\n\n"
    "\tfilepath\t\tpath to an IOCCC compressed tarball\n"
    "\n"
    "Exit codes:\n"
    "     0\t\tall OK\n"
    "     2\t\t-h and help string printed or -V and version string printed\n"
    "     3\t\tcommand line error\n"
    "     4\t\t\"submit.test-\" separated token length != %d\n"
    "     5\t\t\"submit.UUID-\" token length != %zu\n"
    "     >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";


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
    char *filepath;		/* filepath argument to check */
    char *filename;		/* basename of filepath to check */
    int ret;			/* libc return code */
    char *submit;		/* first '.' separated token - submit */
    char *uuid;			/* first '-' separated token - test or UUID */
    size_t len;			/* UUID length */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    char guard;			/* scanf guard to catch excess amount of input */
    int submit_slot;		/* third .-separated token as a number */
    char *timestamp_str;	/* fourth .-separated token - timestamp */
    intmax_t timestamp;		/* fifth .-separated token as a timestamp */
    char *extension;		/* sixth .-separated token as a filename extension */
    char *ext = "txz";		/* user supplied extension (def: txz): used for testing purposes only */
    bool test_mode = false;	/* true ==> force check to test if it's a test submit filename */
    char *saveptr = NULL;	/* for strtok_r() */
    bool ignore_timestamp = false; /* true ==> ignore timestamp check result (for testing purposes) */
    bool opt_error = false;	/* fchk_inval_opt() return */
    bool safe = true;		/* assume path is sane */
    enum path_sanity sanity = PATH_ERR_UNSET;	/* canon_path error or PATH_OK */
    int i;

    /* IOCCC requires use of C locale */
    set_ioccc_locale();

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:qVtTquE:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 2 */
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
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit 2 */
	    print("%s version: %s\n", FNAMCHK_BASENAME, FNAMCHK_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 't': /* force check to verify that the filename is a test submit filename */
	    test_mode = true;
	    break;
        case 'T': /* ignore timestamp check results (for txzchk test when the timestamp of the contest is updated) */
            ignore_timestamp = true;
            break;
	case 'E': /* force extension check to be optarg instead of "txz": used for txzchk test suite */
	    ext = optarg;
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
    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /* collect arg */
    filepath = canon_path(argv[optind], 0, 0, 0, &sanity, NULL, NULL, false, true, true);
    if (filepath == NULL) {
	err(3, program, "bogus filepath: %s error: %s", argv[optind], path_sanity_error(sanity)); /*ooo*/
	not_reached();
    }
    dbg(DBG_LOW, "filepath: %s", filepath);

    /*
     * obtain the basename of the path to examine
     */
    filename = base_name(filepath);
    if (filename == NULL) {
        err(55, __func__, "base_name(\"%s\") returned NULL", filepath);
        not_reached();
    }
    dbg(DBG_LOW, "filename: %s", filename);

    /*
     * first '.' separated token must be submit
     */
    submit = strtok_r(filename, ".", &saveptr);
    if (submit == NULL) {
	err(56, __func__, "first strtok_r() returned NULL");
	not_reached();
    }
    if (strcmp(submit, "submit") != 0) {
	err(57, __func__, "filename does not start with \"submit.\": %s", filepath);
	not_reached();
    }
    dbg(DBG_LOW, "filename starts with \"submit.\": %s", filename);

    /*
     * second '.' separated token must be test or a UUID
     */
    uuid = strtok_r(NULL, ".", &saveptr);
    if (uuid == NULL) {
	err(58, __func__, "nothing found after \"submit.\"");
	not_reached();
    }
    len = strlen(uuid);

    /*
     * parse a test-submit_slot IOCCC contest ID
     */
    if (strncmp(uuid, "test-", LITLEN("test-")) == 0) {
	/* if it starts as "test-" and -u was specified it's an error */
	if (!test_mode) {
	    err(59, __func__, "-t not specified with filename that starts as a test mode filename: %s", filepath);
            not_reached();
        }

	/*
	 * NOTE: we prevent seqcexit from modifying the exit code because the
	 * txzchk_test.sh script has a test file where it expects this error.
	 */
	if (len != LITLEN("test-")+MAX_SUBMIT_SLOT_CHARS) {
	    err(4, __func__, "\"submit.test-\" separated token length: %zu != %zu: %s", /*ooo*/
			     len, (LITLEN("test-")+MAX_SUBMIT_SLOT_CHARS), filepath);
	    not_reached();
	}
	ret = sscanf(uuid, "test-%d%c", &submit_slot, &guard);
	if (ret != 1) {
	    err(60, __func__, "submit_slot not found after \"test-\": %s", filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "submit ID is test: %s", uuid);
	if (submit_slot < 0) {
	    err(61, __func__, "submit_slot %d is < 0: %s", submit_slot, filepath);
	    not_reached();
	}
	if (submit_slot > MAX_SUBMIT_SLOT) {
	    err(62, __func__, "submit_slot %d is > %d: %s", submit_slot, MAX_SUBMIT_SLOT, filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "submit_slot %d is valid: %s", submit_slot, filepath);

    /*
     * parse a UUID-submit_slot IOCCC contest ID
     */
    } else {
	/*
	 * if -t is specified and we get here (filename does not start with
	 * "submit.test-") then it's an error.
	 */
	if (test_mode) {
	    err(63, __func__, "-t specified and filename does not start with \"submit.test-\": %s", filepath);
	    not_reached();
        }

	/*
	 * NOTE: we prevent seqcexit from modifying the exit code because the
	 * txzchk_test.sh script has a test file where it expects this error.
	 */
	if (len != UUID_LEN+1+MAX_SUBMIT_SLOT_CHARS) {
	    err(5, __func__, "\"submit.UUID-\" separated token length: %zu != %d: %s", /*ooo*/
			     len, (UUID_LEN+1+MAX_SUBMIT_SLOT_CHARS), filepath); /*ooo*/
	    not_reached();
	}
	ret = sscanf(uuid, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x-%d%c", &a, &b, &version, &c, &variant,
		&d, &e, &f, &submit_slot, &guard);
	if (ret != 9) {
	    err(64, __func__, "UUID-submit_slot not found after \"submit-\": %s", filepath);
	    not_reached();
	}
	if (version != UUID_VERSION) {
	    err(65, __func__, "UUID token version %x != %x: %s", version, UUID_VERSION, filepath);
	    not_reached();
	}
	if (variant != UUID_VARIANT_0 && variant != UUID_VARIANT_1 && variant != UUID_VARIANT_2 && variant != UUID_VARIANT_3) {
	    err(66, __func__, "UUID token variant %x not one of %x, %x, %x, %x: %s", variant, UUID_VARIANT_0,
                    UUID_VARIANT_1, UUID_VARIANT_2, UUID_VARIANT_3, filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "submit ID is a valid UUID: %s", uuid);
	if (submit_slot < 0) {
	    err(67, __func__, "submit_slot %d is < 0: %s", submit_slot, filepath);
	    not_reached();
	}
	if (submit_slot > MAX_SUBMIT_SLOT) {
	    err(68, __func__, "submit_slot %d is > %d: %s", submit_slot, MAX_SUBMIT_SLOT, filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "submit number is valid: %d", submit_slot);
    }

    /*
     * third '.' separated token must be a valid timestamp, unless
     * ignore_timestamp is true
     */
    timestamp_str = strtok_r(NULL, ".", &saveptr);
    if (timestamp_str == NULL) {
	err(69, __func__, "nothing found after second '.' separated token of submit number: %s", filepath);
	not_reached();
    }
    if (!ignore_timestamp) {
        ret = sscanf(timestamp_str, "%jd%c", &timestamp, &guard);
        if (ret != 1) {
            err(70, __func__, "timestamp not found after \"submit_slot.\": %s is not a timestamp: %s", timestamp_str, filepath);
            not_reached();
        }
        if (timestamp < MIN_TIMESTAMP) {
            err(71, __func__, "timestamp: %jd is < %jd: %s", timestamp, (intmax_t)MIN_TIMESTAMP, filepath);
            not_reached();
        }
        dbg(DBG_LOW, "timestamp is valid: %jd", timestamp);
    }

    /*
     * fourth .-separated token must be the filename extension
     */
    extension = strtok_r(NULL, ".", &saveptr);
    if (extension == NULL) {
	err(72, __func__, "nothing found after third '.' separated token of timestamp: %s", filepath);
	not_reached();
    }
    if (strcmp(extension, ext) != 0) {
	err(73, __func__, "extension %s != %s: %s", extension, ext, filepath);
	not_reached();
    }
    dbg(DBG_LOW, "filename extension is valid: %s", extension);

    /*
     * filepath must use only POSIX portable filename and + chars
     */
    safe = safe_str(filepath, true, true); /* ^[/0-9A-Za-z._+-]+$ */
    if (safe == false) {
	err(74, __func__, "filepath: safe_str(%s, true, true) is false", filepath);
	not_reached();
    }

    /*
     * filename must use only lower case POSIX portable filename and + chars
     */
    safe = safe_path_str(filename, false, false); /* ^[0-9a-z._][0-9a-z._+-]*$ */
    if (safe == false) {
	err(75, __func__, "basename: safe_path_str(%s, false, false) is false", filename);
	not_reached();
    }

    /*
     * All is OK with the filepath - print submit directory basename
     */
    dbg(DBG_LOW, "filepath passes all checks: %s", filepath);
    errno = 0;		/* pre-clear errno for errp() */
    ret = printf("%s\n", uuid);
    if (ret <= 0) {
	errp(76, __func__, "printf of submit directory basename failed");
	not_reached();
    }

    /*
     * free filename allocated from base_name()
     */
    if (filename != NULL) {
	free(filename);
	filename = NULL;
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     *
     */
    exit(0); /*ooo*/
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
	prog = FNAMCHK_BASENAME;
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, (UUID_LEN+1+MAX_SUBMIT_SLOT_CHARS),
	    (LITLEN("test-")+MAX_SUBMIT_SLOT_CHARS), FNAMCHK_BASENAME, FNAMCHK_VERSION,
	    JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
