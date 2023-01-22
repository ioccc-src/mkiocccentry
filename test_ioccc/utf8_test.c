/*
 * utf8_test - test translate UTF-8 into POSIX portable filename and + chars
 *
 * "Because even POSIX needs an extra plus." :-)
 *
 * An author_handle, for an IOCCC winner, will be used to form
 * a winner_handle.  These winner handles will become part of a
 * JSON filename on the www.ioccc.org website.  For this reason,
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
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>


/*
 * utf8_posix_map - translate UTF-8 into POSIX portable filename and + chars
 */
#include "../soup/utf8_posix_map.h"


/*
 * definitions
 */
#define VERSION "1.1 2022-10-17"


/*
 * globals
 */


/*
 * usage message
 *
 * The follow usage message came from an early draft of mkiocccentry.
 * This is just an example of usage: there is no mkiocccentry functionality here.
 */
static char const * const usage =
"usage: %s [-h] [-v level] [-V] [-q] name ...\n"
"\n"
"\t-h\t\tprint help message and exit\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\t-V\t\tprint version string and exit\n"
"\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
"\n"
"\tname\t\tUTF-8 name to translate into POSIX portable filename and + chars\n"
"\n"
"Exit codes:\n"
"     0   all is OK\n"
"     2   -h and help string printed or -V and version string printed\n"
"     3   invalid command line, invalid option or option missing an argument\n"
" >= 10   internal error\n"
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
    while ((i = getopt(argc, argv, "hv:Vq")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    fprintf_usage(2, stderr, usage, program, VERSION); /*ooo*/
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'V':	/* -V - print version and exit */
            print("%s\n", VERSION);
            exit(2); /* ooo */
            not_reached();
            break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	default:
	    fprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    fprintf_usage(3, stderr, usage, program, VERSION); /*ooo*/
	    not_reached();
	}
    }
    /* must have 1 or more */
    if (argc-optind <= 0) {
	fprintf_usage(DO_NOT_EXIT, stderr, "requires 1 or more arguments");
	fprintf_usage(3, stderr, usage, program, VERSION); /*ooo*/
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
	    err(10, __func__, "default_handle(%s) returned NULL", argv[i]); /*ooo*/
	    not_reached();
	}
	dbg(3, "translation into POSIX portable filename and + chars: <%s>", name);

	/*
	 * print translated name
	 */
	errno = 0;			/* pre-clear errno for errp() */
	ret = puts(name);
	if (ret == EOF) {
	    errp(3, __func__, "puts error");
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
