/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 *
 * "Because even fprintf has a return value worth paying attention to." :-)
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
#include <stdint.h>

/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 */
#include "fnamchk.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */



int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    char *filepath;		/* filepath argument to check */
    char *filename;		/* basename of filepath to check */
    int ret;			/* libc return code */
    char *entry;		/* first '.' separated token - entry */
    char *uuid;			/* first '-' separated token - test or UUID */
    size_t len;			/* UUID length */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    char guard;			/* scanf guard to catch excess amount of input */
    int entry_num;		/* third .-separated token as a number */
    char *timestamp_str;	/* fourth .-separated token - timestamp */
    intmax_t timestamp;		/* fifth .-separated token as a timestamp */
    char *extension;		/* sixth .-separated token as a filename extension */
    char *ext = "txz";		/* user supplied extension (def: txz): used for testing purposes only */
    int i;
    bool test_mode = false;	/* true ==> force check to test if it's a test entry filename */
    bool uuid_mode = false;	/* true ==> force check to test if it's a UUID entry filename */


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVtquE:e")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(1, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", FNAMCHK_VERSION);
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 't': /* force check to verify that the filename is a test entry filename */
	    test_mode = true;
	    break;
	case 'u': /* force check to verify that the filename is a UUID (real) entry filename */
	    uuid_mode = true;
	    break;
	case 'E': /* force extension check to be optarg instead of "txz": used for test suite for txzchk */
	    ext = optarg;
	    break;
	case 'e': /* suppress error messages */
	    msg_warn_silent = true;
	    err_output_allowed = false;
	    break;
	default:
	    usage(1, "invalid -flag", program); /*ooo*/
	    not_reached();
	    break;
	 }
    }
    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(1, "wrong number of arguments", program); /*ooo*/
	not_reached();
    }

    /* -t and -u cannot be used together as the tests they enable conflict */
    if (test_mode && uuid_mode) {
	err(1, __func__, "-t and -u cannot be used together");
	not_reached();
    }

    /* collect arg */
    filepath = argv[optind];
    dbg(DBG_LOW, "filepath: %s", filepath);

    /*
     * obtain the basename of the path to examine
     */
    filename = base_name(filepath);
    dbg(DBG_LOW, "filename: %s", filename);

    /*
     * first .-separated token must be entry
     */
    entry = strtok(filename, ".");
    if (entry == NULL) {
	err(2, __func__, "first strtok() returned NULL");
	not_reached();
    }
    if (strcmp(entry, "entry") != 0) {
	err(3, __func__, "filename does not start with \"entry.\": %s", filepath);
	not_reached();
    }
    dbg(DBG_LOW, "filename starts with \"entry.\": %s", filename);

    /*
     * second '.' separated token must be test or a UUID
     */
    uuid = strtok(NULL, ".");
    if (uuid == NULL) {
	err(4, __func__, "nothing found after first '.'  separated token of entry");
	not_reached();
    }
    len = strlen(uuid);

    /*
     * parse a test-entry_num IOCCC contest ID
     */
    if (strncmp(uuid, "test-", LITLEN("test-")) == 0) {
	/* if it starts as "test-" and -u was specified it's an error */
	if (uuid_mode) {
	    err(5, __func__, "-u specified and entry starts as a test mode filename");
	    not_reached();
	}
	if (len != LITLEN("test-")+MAX_ENTRY_CHARS) {
	    err(6, __func__, "second '-' separated token length: %ju != %ju: %s",
			     (uintmax_t)len, (uintmax_t)(LITLEN("test-")+MAX_ENTRY_CHARS), filepath);
	    not_reached();
	}
	ret = sscanf(uuid, "test-%d%c", &entry_num, &guard);
	if (ret != 1) {
	    err(7, __func__, "second '.' separated non-test not in test-entry_number format: %s", filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "entry ID is test: %s", uuid);
	if (entry_num < 0) {
	    err(8, __func__, "third '.' separated entry number: %d is < 0: %s", entry_num, filepath);
	    not_reached();
	}
	if (entry_num > MAX_ENTRY_NUM) {
	    err(9, __func__, "second '-' separated entry number: %d is > %d: %s", entry_num, MAX_ENTRY_NUM, filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "entry number is valid: %d", entry_num);

    /*
     * parse a UUID-entry_num IOCCC contest ID
     */
    } else {
	/*
	 * if -t is specified and we get here (filename does not start with
	 * "entry.test-" then it's an error.
	 */
	if (test_mode) {
	    err(10, __func__, "-t specified and entry does not start with \"entry.test-\"");
	    not_reached();
	}

	if (len != UUID_LEN+1+MAX_ENTRY_CHARS) {
	    err(11, __func__, "second '-' separated token length: %ju != %ju: %s",
			     (uintmax_t)len, (uintmax_t)(UUID_LEN+1+MAX_ENTRY_CHARS), filepath);
	    not_reached();
	}
	ret = sscanf(uuid, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x-%d%c", &a, &b, &version, &c, &variant, &d, &e, &f, &entry_num, &guard);
	if (ret != 9) {
	    err(12, __func__, "second '.' separated non-test not in UUID-entry_number format: %s", filepath);
	    not_reached();
	}
	if (version != UUID_VERSION) {
	    err(13, __func__, "second '.' separated UUID token version %x != %x: %s", version, UUID_VERSION, filepath);
	    not_reached();
	}
	if (variant != UUID_VARIANT) {
	    err(14, __func__, "second '.' separated UUID token variant %x != %x: %s", variant, UUID_VARIANT, filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "entry ID is a valid UUID: %s", uuid);
	if (entry_num < 0) {
	    err(15, __func__, "third '.' separated entry number: %d is < 0: %s", entry_num, filepath);
	    not_reached();
	}
	if (entry_num > MAX_ENTRY_NUM) {
	    err(16, __func__, "second '-' separated entry number: %d is > %d: %s", entry_num, MAX_ENTRY_NUM, filepath);
	    not_reached();
	}
	dbg(DBG_LOW, "entry number is valid: %d", entry_num);
    }

    /*
     * third '.' separated token must be a valid timestamp
     */
    timestamp_str = strtok(NULL, ".");
    if (timestamp_str == NULL) {
	err(17, __func__, "nothing found after second '.' separated token of entry number");
	not_reached();
    }
    ret = sscanf(timestamp_str, "%jd%c", &timestamp, &guard);
    if (ret != 1) {
	err(18, __func__, "third '.' separated token: %s is not a timestamp: %s", timestamp_str, filepath);
	not_reached();
    }
    if (timestamp < MIN_TIMESTAMP) {
	err(19, __func__, "third '.' separated timestamp: %jd is < %jd: %s", timestamp, (intmax_t)MIN_TIMESTAMP, filepath);
	not_reached();
    }
    dbg(DBG_LOW, "timestamp is valid: %jd", timestamp);

    /*
     * fourth .-separated token must be the filename extension
     */
    extension = strtok(NULL, ".");
    if (extension == NULL) {
	err(20, __func__, "nothing found after third '.' separated token of timestamp");
	not_reached();
    }
    if (strcmp(extension, ext) != 0) {
	err(21, __func__, "final fourth '.' separated token filename extension: %s != %s: %s", extension, ext, filepath);
	not_reached();
    }
    dbg(DBG_LOW, "filename extension is valid: %s", extension);

    /*
     * filepath must use only POSIX portable filename and + chars /
     */
    if (posix_plus_safe(filepath, false, true, false) == false) {
	err(22, __func__, "filepath: posix_plus_safe(%s, false, true, false) is false", filepath);
	not_reached();
    }

    /*
     * filename must use only lower case POSIX portable filename and + chars
     */
    if (posix_plus_safe(filename, true, false, true) == false) {
	err(23, __func__, "basename: posix_plus_safe(%s, true, false, true) is false", filename);
	not_reached();
    }

    /*
     * All is OK with the filepath - print entry directory basename
     */
    dbg(DBG_LOW, "filepath passes all checks: %s", filepath);
    errno = 0;		/* pre-clear errno for errp() */
    ret = printf("%s\n", uuid);
    if (ret <= 0) {
	errp(24, __func__, "printf of entry directory basename failed");
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
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, FNAMCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
