/*
 * jprint - JSON printer
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool is being developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * The JSON parser was co-developed in 2022 by Cody and Landon.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#include "jprint.h"

/*
 * definitions
 */
#define REQUIRED_ARGS (2)	/* number of required arguments on the command line */


/*
 * static globals
 */
static bool quiet = false;				/* true ==> quiet mode */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-V] [-v level] [-J level] [-e] [-Q] [-t type] [-q]\n"
    "\t\t[-j lvl] [-i cnt] [-N num] [-p {n,v,b}] [-b {t,number}] [-L]\n"
    "\t\t[-c] [-C] [-B] [-I {t,number}] [-j] [-E] [-I] [-S] [-g]\n"
    "\t\tfile.json [name_arg ...]\n"
    "\n"
    "\t-h\t\tprint help and exit\n"
    "\t-V\t\tprint version and exit\n"
    "\t-v level\tverbosity level (def: %d)\n"
    "\t-J level\tJSON verbosity level (def: %d)\n"
    "\t-e\t\tprint JSON strings as encoded strings (def: decode JSON strings)\n"
    "\t-Q\t\tprint JSON strings surrounded by double quotes (def: do not)\n"
    "\n"
    "\tfile.json\tJSON file to parse\n"
    "\tname_arg\tJSON element to print\n"
    "jprint version: %s";


/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));

int main(int argc, char **argv)
{
    char const *program = NULL;		/* our name */
    extern char *optarg;
    extern int optind;
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:Vq")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, "");	/*ooo*/
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
	    print("%s\n", JPRINT_VERSION);
	    exit(2);		/*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
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

    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    argc -= optind;
    argv += optind;


    exit(0);
}

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
	prog = "((NULL prog))";
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
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JSON_DBG_DEFAULT, JPRINT_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
