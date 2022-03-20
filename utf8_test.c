/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * utf8_test - test translate UTF-8 into POSIX portable filename and + chars
 *
 * "Because even POSIX needs an extra plus." :-)
 *
 * An author_handle, for an IOCCC winner, will be used to form
 * a winner_handle.  These winner_handle's will become part of a
 * JSON filename on the www.ioccc.org web site.  For this reason,
 * (and other reasons), we must restrict an author_handle to
 * only lower case POSIX portable filenames, with the addition of the +,
 * and restrictions on leading characters.
 *
 * The author_handle (and winner_handle) must fit the following
 * regular expression:
 *
 *	^[0-9a-z][0-9a-z._+-]*$
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
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>


/*
 * utf8_posix_map include
 */
#include "utf8_posix_map.h"


/*
 * IOCCC limits
 */
#include "limit_ioccc.h"


/*
 * definitions
 */
#define VERSION "1.0 2022-03-20"


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
bool msg_output_allowed = true;		/* false ==> disable output from msg() */
bool dbg_output_allowed = true;		/* false ==> disable output from dbg() */
bool warn_output_allowed = true;	/* false ==> disable output from warn() and warnp() */
bool err_output_allowed = true;		/* false ==> disable output from err() and errp() */
bool usage_output_allowed = true;	/* false ==> disable output from vfprintf_usage() */
static bool quiet = false;		/* true ==> only show errors, and warnings if -v > 0 */


/*
 * usage message
 *
 * The follow usage message came from an early draft of mkiocccentry.
 * This is just an example of usage: there is no mkiocccentry functionality here.
 */
static char const * const usage =
"usage: %s [-h] [-v level] [-q] name ...\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\t-q\t\tquiet mode, unless verbosity level > 0 (def: not quiet)\n"
"\n"
"\tname\t\tUTF-8 name to translate into POSIX portable filename and + chars\n"
"\n"
"Version: %s";


int
main(int argc, char *argv[])
{
    char *program = NULL;		/* our name */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char *name = NULL;			/* translated name argument */
    int ret;				/* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:q")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    /* exit(0); */
	    vfprintf_usage(0, stderr, usage, program, VERSION); /*ooo*/
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(1); */
		err(1, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno)); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    quiet = true;
	    break;
	default:
	    vfprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    /* exit(3); */
	    vfprintf_usage(3, stderr, usage, program, VERSION); /*ooo*/
	    not_reached();
	}
    }
    /* be warn(), warnp() and msg() quiet of -q and -v 0 */
    if (quiet == true && verbosity_level <= 0) {
	msg_output_allowed = false;
	warn_output_allowed = false;
    }
    /* must have 1 or more */
    if (argc-optind <= 0) {
	vfprintf_usage(DO_NOT_EXIT, stderr, "requires 1 or more arguments");
	/* exit(4); */
	vfprintf_usage(4, stderr, usage, program, VERSION); /*ooo*/
	not_reached();
    }

    /*
     * translate args, 1 at a time
     */
    for (i=optind; i < argc; ++i) {

	/*
	 * translate arg (as name) into default handle
	 */
	dbg(1, "UTF-8 name to translate: <%s>", argv[i]);
	name = default_handle(argv[i]);
	if (name == NULL) {
	    err(5, __func__, "default_handle(%s) returned NULL", argv[i]);
	    not_reached();
	}
	dbg(3, "translation into POSIX portable filename and + chars: <%s>", name);

	/*
	 * print translated name
	 */
	errno = 0;			/* pre-clear errno for errp() */
	ret = puts(name);
	if (ret == EOF) {
	    errp(6, __func__, "puts error");
	    not_reached();
	}

	/*
	 * free translation
	 */
	if (name != NULL) {
	    free(name);
	    name = NULL;
	}
    }

    /*
     * All Done!!! -- Jessica Noll, Age 2
     */
    exit(0); /*ooo*/
}
