/*
 * location_main - lookup ISO 3166 codes, location names or print the table
 *
 * "Because there is an I in IOCCC." :-)
 *
 * Copyright (c) 2023,2024 by Landon Curt Noll.  All Rights Reserved.
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


#include <unistd.h>

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
#define LOCATION_VERSION "1.0.3 2024-03-03"		/* format: major.minor YYYY-MM-DD */


/*
 * usage message
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-n] [-s] [-a] [location]\n"
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
    "location version: %s\n";



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

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:Vcnsa")) != -1) {
        switch (i) {
        case 'h':
	    fprintf_usage(2, stderr, usage_msg, program, DBG_DEFAULT, LOCATION_VERSION); /*ooo*/
	    not_reached();
            break;
        case 'v':
            /*
             * parse verbosity
             */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		warn(program, "invalid -v optarg");
		fprintf_usage(3, stderr, usage_msg, program, DBG_DEFAULT, LOCATION_VERSION); /*ooo*/
		not_reached();
	    }
            break;
        case 'V':               /* -V - print version and exit */
            (void) printf("%s\n", LOCATION_VERSION);
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
	    fprintf_usage(3, stderr, usage_msg, program, DBG_DEFAULT, LOCATION_VERSION); /*ooo*/
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
     * All Done!!! - Jessica Noll, age 2
     */
    exit(found?0:1); /*ooo*/
}
