/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jauthchk - IOCCC JSON .author.json checker and validator
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */

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
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * json - json file structs
 */
#include "json.h"


/*
 * jauthchk version
 */
#define JAUTHCHK_VERSION "0.1 2022-02-12"	/* use format: major.minor YYYY-MM-DD */


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
"usage: %s [-h] [-v level] [-V] [-q] file\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-V\t\tprint version string and exit\n"
"\t-q\t\tquiet mode\n"
"\n"
"\tfile\t\tpath to a .author.json file\n"
"\n"
"exit codes:\n"
"\n"
"\t0\t\tno errors or warnings detected\n"
"\t>0\t\tsome error(s) and/or warning(s) were detected\n"
"\n"
"jauthchk version: %s\n";


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	    /* debug level set by -v */
char const *program = NULL;		    /* our name */
static bool quiet = false;		    /* true ==> quiet mode */
static struct author author;

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void sanity_chk(char const *file);
static void check_author_json(char const *file);


int
main(int argc, char **argv)
{
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    char *file;		/* file argument to check */
    int ret;			/* libc return code */
    int i;


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vq")) != -1) {
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
	    ret = printf("%s\n", JAUTHCHK_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", JAUTHCHK_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
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
    file = argv[optind];
    dbg(DBG_LOW, "JSON file: %s", file);

    /*
     * Welcome
     */
    if (!quiet) {
	errno = 0;			/* pre-clear errno for errp() */
	ret = printf("Welcome to jauthchk version: %s\n", JAUTHCHK_VERSION);
	if (ret <= 0) {
	    errp(4, __func__, "printf error printing the welcome string");
	    not_reached();
	}
    /*
     * environment sanity checks
     */
	para("", "Performing sanity checks on your environment ...", NULL);
    }

    sanity_chk(file);
    if (!quiet) {
	para("... environment looks OK", "", NULL);
    }

    check_author_json(file);

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(author.issues != 0);
}

/*
 * sanity_chk - perform basic sanity checks
 *
 * We perform basic sanity checks on paths.
 *
 * given:
 *
 *      file        - path to JSON file to parse
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
sanity_chk(char const *file)
{
    /*
     * firewall
     */
    if (file == NULL) {
	err(3, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * file must be readable
     */
    if (!exists(file)) {
	fpara(stderr,
	      "",
	      "The JSON path specified does not exist. Perhaps you made a typo?",
	      "Please check the path and try again."
	      "",
	      "    jauthchk [options] <file>"
	      "",
	      NULL);
	err(10, __func__, "file does not exist: %s", file);
	not_reached();
    }
    if (!is_file(file)) {
	fpara(stderr,
	      "",
	      "The file specified, whilst it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    jauthchk [...] <file>",
	      "",
	      NULL);
	err(11, __func__, "file is not a file: %s", file);
	not_reached();
    }
    if (!is_read(file)) {
	fpara(stderr,
	      "",
	      "The JSON path, whilst it is a file, is not readable.",
	      "",
	      "We suggest you check the permissions on the path or use another path:",
	      "",
	      "    jauthchk [...] <file>"
	      "",
	      NULL);
	err(12, __func__, "file is not readable: %s", file);
	not_reached();
    }

    return;
}

/*
 * check_author_json  - check file as .author.json
 *
 * given:
 *
 *	file	-   path to the file to check
 *
 * Attempts to validate the file as .author.json, reporting any problems found.
 *
 * Function does not return on error.
 */
static void
check_author_json(char const *file)
{
    FILE *author_json;
    int ret;

    /*
     * firewall
     */
    if (file == NULL) {
	err(16, __func__, "passed NULL arg");
	not_reached();
    }
    author_json = fopen(file, "r");
    if (author_json == NULL) {
	err(17, __func__, "couldn't open %s", file);
	not_reached();
    }

    errno = 0;
    ret = fclose(author_json);
    if (ret != 0) {
	warnp(__func__, "error in fclose to .author.json file %s", file);
    }
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
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JAUTHCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
