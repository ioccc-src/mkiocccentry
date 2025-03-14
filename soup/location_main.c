/*
 * location_main - lookup ISO 3166 codes, location names or print the table
 *
 * "Because there is an I in IOCCC." :-)
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
 * This tool was co-developed in 2022-2025 by Cody Boone Ferguson and Landon
 * Curt Noll:
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


#include <unistd.h>
#include <locale.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * location - location/country codes
 */
#include "location.h"

/*
 * official location version
 */
#define LOCATION_VERSION "1.0.4 2024-05-22"		/* format: major.minor YYYY-MM-DD */


/*
 * usage message
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-n] [-s] [-a] [location...]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "\t-c\t\tUse common name (def: official name)\n"
    "\n"
    "\t-n\t\tlocation arg(s) are name(s) to search for (def: location arg(s) are ISO 3166 code(s))\n"
    "\t\t\t\tNOTE: use of -n requires a location arg\n"
    "\n"
    "\t-s\t\tSearch by substrings instead of exact match.\n"
    "\t\t\t\tNOTE: use of -s requires a location arg\n"
    "\n"
    "\t-a\t\tShow all matches if searching by substrings\n"
    "\n"
    "\tlocation\tISO 3166 code (or location name if -n) to print (def: print all codes and names)\n"
    "\n"
    "Exit codes:\n"
    "    0\tlocation arg(s) found in table\n"
    "    1\tone or more location arg(s) not found\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    "    >=4\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";

/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


int
main(int argc, char **argv)
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    bool substrings = false;	/* search location as a substring */
    bool show_all = false;	/* show all matches if -s */
    int arg_count = 0;		/* number of args to process */
    bool name_flag = false;	/* false ==> search for ISO 3166 code, true ==> search for location name */
    const char *ret = NULL;	/* table lookup return or NULL */
    struct location *l = NULL;	/* location table entry */
    struct location *location = NULL; /* for saving location in table if -a */
    size_t idx = 0;		/* location to start searching table at */
    bool found = false;		/* true if nothing found */
    bool use_common = false;	/* true ==> use common name, false ==> use official name */
    int i;

    /* IOCCC requires use of C locale */
    (void) setlocale(LC_ALL, "C");

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:Vcnsa")) != -1) {
        switch (i) {
        case 'h':
	    usage(2, program, ""); /*ooo*/
	    not_reached();
            break;
        case 'v':
            /*
             * parse verbosity
             */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
            break;
        case 'V':               /* -V - print version and exit */
            print("%s version: %s\n", LOCATION_BASENAME, LOCATION_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
            exit(2); /*ooo*/
            not_reached();
            break;
	case 'c':
	    use_common = true;
            break;
	case 'n':
	    name_flag = true;
            break;
	case 's':
	    substrings = true;
	    break;
	case 'a':
	    show_all = true;
	    break;
        case ':':   /* option requires an argument */
        case '?':   /* illegal option */
        default:    /* anything else but should not actually happen */
	    check_invalid_option(program, i, optopt);
	    usage(3, program, ""); /*ooo*/
            not_reached();
            break;
        }
    }
    arg_count = argc - optind;
    if (arg_count < 0) {
	err(4, __func__, "%s: arg_count: %d < 0", program, arg_count);
	not_reached();
    } else if (arg_count == 0 && name_flag == true) {
	err(3, __func__, "%s: use of -n requires location arg(s)", program); /*ooo*/
	not_reached();
    } else if (arg_count == 0 && substrings) {
	err(3, __func__, "%s: use of -s requires location arg(s)", program); /*ooo*/
	not_reached();
    }


    /*
     * paranoia - sanity check table
     */
    check_location_table();

    /*
     * case: print entire table
     */
    if (arg_count == 0) {

	/*
	 * print each table entry
	 */
	if (use_common) {
	    for (l=loc; l->code != NULL && l->common_name != NULL; ++l) {
		(void) printf("%s\t%s\n", l->code, l->common_name);
	    }
	} else {
	    for (l=loc; l->code != NULL && l->name != NULL; ++l) {
		(void) printf("%s\t%s\n", l->code, l->name);
	    }
	}

    /*
     * case: search for table entries
     */
    } else {

	/*
	 * process each command line argument
	 */
	for (i=optind; i < argc; ++i) {

	    /* for each arg, set location to NULL to not mess up previous pass */
	    location = NULL;
	    /* also reset idx */
	    idx = 0;

	    /*
	     * case: -n: scan for name(s)
	     */
	    if (name_flag) {

		/*
		 * search for location name given location code
		 */
		/* case: -a, show all */
		if (show_all) {
		    ret = lookup_location_code_r(argv[i], &idx, &location, substrings, use_common);
		    do {
			if (ret != NULL) {
			    found = true;
			    /* print what was found */
			    if (use_common) {
				if (verbosity_level > 0 && location != NULL && location->common_name) {
				    (void) printf("%s ==> ", location->common_name);
				}
			    } else {
				if (verbosity_level > 0 && location != NULL && location->name) {
				    (void) printf("%s ==> ", location->name);
				}
			    }
			    (void) printf("%s\n", ret);
			    ret = lookup_location_code_r(argv[i], &idx, &location, substrings, use_common);
			}
		    } while (ret != NULL);
		/* case: no -a, only show first */
		} else {
		    ret = lookup_location_code_r(argv[i], NULL, NULL, substrings, use_common);

		    /*
		     * print location code if found
		     */
		    if (ret == NULL) {
			err(1, __func__, "location name not found for: %s", argv[i]); /*ooo*/
			not_reached();
		    }
		    found = true;
		    /* print what was found */
		    (void) printf("%s\n", ret);
		}
	    /*
	     * case: scan for code(s)
	     */
	    } else {

		/* case: -a, show all */
		if (show_all) {
		    ret = lookup_location_name_r(argv[i], &idx, &location, substrings, use_common);
		    do {
			if (ret != NULL) {
			    found = true;
			    /* print what was found */
			    if (verbosity_level > 0 && location != NULL && location->code != NULL) {
				(void) printf("%s ==> ", location->code);
			    }
			    (void) printf("%s\n", ret);
			    ret = lookup_location_name_r(argv[i], &idx, &location, substrings, use_common);
			}
		    } while (ret != NULL);
		/* case: no -a, only show first */
		} else {

		    /*
		     * search for location code given location name
		     */
		    ret = lookup_location_name_r(argv[i], NULL, &location, substrings, use_common);

		    /*
		     * print location code if found
		     */
		    if (ret != NULL) {
			found = true;

			/* print what was found */
			if (verbosity_level > 0 && location != NULL && location->code != NULL) {
			    (void) printf("%s ==> ", location->code);
			}
			(void) printf("%s\n", ret);
		    }
		}
	    }
	}
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(found?0:1); /*ooo*/
}

/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(2, program, "wrong number of arguments");
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
	warn("location", "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }

    if (prog == NULL) {
	prog = LOCATION_BASENAME;
	warn("location", "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, LOCATION_BASENAME, LOCATION_VERSION,
	    JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
