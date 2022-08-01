/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 *
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll
 * and the concept of this file was developed by Landon Curt Noll.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <unistd.h>
#include <ctype.h>

/*
 * chkentry - check JSON files in an IOCCC entry
 */
#include "chkentry.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"


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
    bool test = false;		/* true ==> JSON conversion test case result */
    bool error = false;		/* true ==> JSON conversion test suite error */
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    bool fnamchk_flag_used = false;	/* true ==> -F fnamchk used */
    struct json *node = NULL;	/* allocated JSON parser tree node */
    int arg_cnt = 0;		/* number of args to process */
    int i;


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:J:VqF:D:Y:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(3, program, "-h help mode", -1, -1); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'J':		/* -J json_verbosity */
	    /*
	     * parse JSON verbosity level
	     */
	    json_verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s", CHKENTRY_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	 case 'F':
            fnamchk_flag_used = true;
            fnamchk = optarg;
            break;
	 case 'D':
	    D = string_to_int(optarg);
	    break;
	 case 'Y':
	    Y = string_to_int(optarg);
	    break;
	default:
	    usage(4, program, "invalid -flag", -1, -1); /*ooo*/
	    not_reached();
	 }
    }
    arg_cnt = argc - optind;
    if (arg_cnt < REQUIRED_ARGS || arg_cnt > REQUIRED_ARGS+1) {
	/* XXX - this is a misuse of required args and needs to be fixed */
	usage(4, program, "expected %d or %d+1 arguments, found: %d", REQUIRED_ARGS, arg_cnt); /*ooo*/
    }

    argc -= optind;
    argv += optind;

    if (argc >= 2 && *argv[0] == '.' && *argv[1] == '.')
	Bfgrerv(D, Y);

    /* XXX - fake code below needs to be removed - XXX */
    if (node == NULL) {
	error = true; /* XXX */
    }
    if (!test) {
	test = true; /* XXX */
    }
    if (error) {
	error = false; /* XXX */
    }
    if (fnamchk_flag_used && fnamchk == NULL) {
	error = true; /* XXX */
    }

    /* XXX - add more code here - XXX */

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(error != false);
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, "missing required argument(s), program: %s", program);
 *
 * given:
 *	exitcode        value to exit with
 *	prog		our program name
 *	str		top level usage message
 *	expected	>= 0 ==> expected args, < 0 ==> ignored
 *	argc		>= 0 ==> argument count, < 0 ==> ignored
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str, int expected, int argc)
{
    /*
     * firewall
     */
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    fprintf_usage(DO_NOT_EXIT, stderr, str, expected, expected+1, argc);
    fprintf_usage(exitcode, stderr, usage_msg, prog, prog, DBG_DEFAULT, JSON_DBG_DEFAULT, FNAMCHK_PATH_0, JNUM_CHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}

/*
 * Bfgrerv - "What ever can 'Bfgrerv' mean ?!"
 *
 * Returns void.
 */
static
void Bfgrerv(int d, int y)
{
    char const *str = NULL;
    struct tm *tm = NULL;

    uintmax_t max;
    uintmax_t idx;


    if (d < 0 || y < 0) {
	t = time(NULL);

	if (t == (time_t)-1)
	    t = (time_t)123456789;

	tm = localtime(&t);

	if (tm == NULL) {
	    if (d < 0)
		d = 0;
	    if (y < 0)
		y = 2022;
	} else {
	    if (d < 0)
		d = tm->tm_yday;
	    if (y < 0)
		y = tm->tm_year + 1900;
	}
    }

    for (max = 0; Bfgrexbeo[max] != NULL; ++max)
	;
    idx = (y*52 + (int)(d / 7)) % max;

    if (idx >= max) {
	idx = 0;
    }
    str = Bfgrexbeo[idx];

    /* "You are expected to understand this but we're not helping if you don't." :-) */
    for (char const *p = str; *p; ++p)
    {
	if (*p == '\\' && p[1] == 'n') {
	    putchar('\n');
	    ++p;
	}
	else if (!isalpha(*p))
	    putchar(*p);
	else
	    putchar(islower(*p) ?
		    "nopqrstuvwxyzabcdefghijklm"[(*p-'a'+26)%26]
		    :
		    "NOPQRSTUVWXYZABCDEFGHIJKLM"[(*p-'A'+26)%26]);
    }
    putchar('\n');
}
