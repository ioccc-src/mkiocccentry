/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * filenamechk - IOCCC compressed tarball filename sanity check tool
 *
 * Verify that the
 *
 * "Because even printf has a return value worth paying attention to." :-)
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


/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions
 */
#include "util.h"


/*
 * mkiocccentry version
 */
#define FILENAMECHECK_VERSION "0.1 2022-02-03"	/* use format: major.minor YYYY-MM-DD */


/*
 * definitions
 */
#define LITLEN(x) (sizeof(x)-1)	/* length of a literal string w/o the NUL byte */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] filepath\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "filepath\t\tpath to a IOCCC compressed tarball\n"
    "\n"
    "filenamechk version: %s\n";


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */


/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));


int
main(int argc, char *argv[])
{
    char *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    char *filepath;		/* filepath argument to check */
    char *filename;		/* basename of filepath to check */
    int ret;			/* libc return code */
    char *entry;		/* 1st .-separated token - entry */
    char *uuid;			/* 2nd .-separated token - test or UUID */
    size_t len;			/* UUID length */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    char guard;			/* scanf guard to catch excess amount of input */
    char *entry_str;		/* 3nd .-separated token - entry number */
    int entry_num;		/* 3nd .-separated token as a number */
    char *timestamp_str;	/* 4th .-separated token - timestamp */
    long timestamp;		/* 4th .-separated token as a timestamp */
    char *extension;		/* 5th .-separated token as a filename extension */
    int i;


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:V")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(1, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    errno = 0;		/* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(1, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno)); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", FILENAMECHECK_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", FILENAMECHECK_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	default:
	    usage(1, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(1, "wrong number of arguments", program); /*ooo*/
	not_reached();
    }
    filepath = argv[optind];
    dbg(DBG_LOW, "filepath: %s", filepath);

    /*
     * obtain the basename of the path to examine
     */
    filename = base_name(filepath);
    dbg(DBG_LOW, "filename: %s", filename);

    /*
     * 1st .-separated token must be entry
     */
    entry = strtok(filename, ".");
    if (entry == NULL) {
	err(2, __func__, "1st strtok() returned NULL");
	not_reached();
    }
    if (strcmp(entry, "entry") != 0) {
	err(3, __func__, "filename does not start with entry.: %s", filepath);
	not_reached();
    }
    dbg(DBG_LOW, "filename starts with entry.: %s", filename);

    /*
     * 2nd .-separated token must be test or a UUID
     */
    uuid = strtok(NULL, ".");
    if (uuid == NULL) {
	err(4, __func__, "nothing found after 1st .-separated token of entry");
	not_reached();
    }
    if (strcmp(uuid, "test") == 0) {
	dbg(DBG_LOW, "entry ID is test: %s", uuid);
    } else {
	len = strlen(uuid);
	if (len != UUID_LEN) {
	    err(5, __func__, "2nd .-separated token is neither test nor a UUID: %s", filepath);
	    not_reached();
	}
	ret = sscanf(uuid, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x%c", &a, &b, &version, &c, &variant, &d, &e, &f, &guard);
	if (ret != 8) {
	    err(6, __func__, "2nd .-separated non-test in not in UUID format: %s", filepath);
	    not_reached();
	}
	if (version != UUID_VERSION) {
	    err(7, __func__, "2nd .-separated UUID token version %x != %x: %s", version, UUID_VERSION, filepath);
	    not_reached();
	}
	if (variant != UUID_VARIANT) {
	    err(8, __func__, "2nd .-separated UUID token variant %x != %x: %s", variant, UUID_VARIANT, filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "entry ID is a valid UUID: %s", uuid);
    }

    /*
     * 3rd .-separated token must a valid entry number
     */
    entry_str = strtok(NULL, ".");
    if (entry_str == NULL) {
	err(9, __func__, "nothing found after 2rd .-separated token of entry.test nor entry.UUID");
	not_reached();
    }
    ret = sscanf(entry_str, "%d%c", &entry_num, &guard);
    if (ret != 1) {
	err(10, __func__, "3rd .-separated token: %s is not an entry number: %s", entry_str, filepath);
	not_reached();
    }
    if (entry_num < 0) {
	err(11, __func__, "3rd .-separated entry number: %d is < 0: %s", entry_num, filepath);
	not_reached();
    }
    if (entry_num > MAX_ENTRY_NUM) {
	err(12, __func__, "3rd .-separated entry number: %d is > %d: %s", entry_num, MAX_ENTRY_NUM, filepath);
	not_reached();
    }
    dbg(DBG_LOW, "entry number is valid: %d", entry_num);

    /*
     * 4th .-separated token must a valid timestamp
     */
    timestamp_str = strtok(NULL, ".");
    if (timestamp_str == NULL) {
	err(13, __func__, "nothing found after 3th .-separated token of entry number");
	not_reached();
    }
    ret = sscanf(timestamp_str, "%ld%c", &timestamp, &guard);
    if (ret != 1) {
	err(14, __func__, "4th .-separated token: %s is not a timestamp: %s", timestamp_str, filepath);
	not_reached();
    }
    if (timestamp < MIN_TIMESTAMP) {
	err(15, __func__, "4th .-separated timestamp: %ld is < %ld: %s", timestamp, (long)MIN_TIMESTAMP, filepath);
	not_reached();
    }
    dbg(DBG_LOW, "timestamp is valid: %ld", timestamp);

    /*
     * 5th .-separated token must be the filename extension
     */
    extension = strtok(NULL, ".");
    if (extension == NULL) {
	err(16, __func__, "nothing found after 4th .-separated token of timestamp");
	not_reached();
    }
    if (strcmp(extension, "txz") != 0) {
	err(17, __func__, "final 5th .-separated token filename extension: %s != txz: %s", extension, filepath);
	not_reached();
    }
    dbg(DBG_LOW, "filename extension is valid: %s", extension);

    /*
     * All is OK with the filepath - print entry directory basename
     */
    dbg(DBG_LOW, "filepath passes all checks: %s", filepath);
    errno = 0;		/* pre-clear errno for errp() */
    ret = printf("%s.%d\n", uuid, entry_num);
    if (ret <= 0) {
	errp(18, __func__, "printf of entry directory basename failed");
	not_reached();
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     *
     */
    exit(0); /*ooo*/
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
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, FILENAMECHECK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
