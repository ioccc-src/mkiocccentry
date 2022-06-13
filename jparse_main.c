/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jparse_main - tool that parses a block of JSON input
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 */

#include <stdio.h>
#include <unistd.h>

/*
 * jparse_main - tool that parses a block of JSON input
 */
#include "jparse_main.h"


/*
 * static functions
 */
static void usage(int exitcode, char const *str, char const *prog);


int
main(int argc, char **argv)
{
    char const *program = NULL;	    /* our name */
    extern char *optarg;	    /* option argument */
    extern int optind;		    /* argv index of the next arg */
    bool string_flag_used = false;  /* true ==> -S string was used */
    bool valid_json = false;	    /* true ==> JSON parse was valid */
    int ret;			    /* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVs:J:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'J': /* -J json_verbosity_level */
	    /*
	     * parse json verbosity level
	     */
	    json_verbosity_level = parse_verbosity(program, optarg);

	    /*
	     * enable bison internal debugging if -J is verbose enough
	     */
	    if (json_dbg_allowed(JSON_DBG_VHIGH)) {
		ugly_debug = 1;	/* verbose bison debug on */
	    } else {
		ugly_debug = 0;	/* verbose bison debug off */
	    }
	    (void) json_dbg(JSON_DBG_VHIGH, __func__, "bison ugly_debug: %s",
						      ugly_debug ? "enabled" : "disabled");
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("JSON parser version: %s\n", JSON_PARSER_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing JSON parser version string: %s", JPARSE_VERSION);
	    }
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("jparse version: %s\n", JPARSE_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing jparse version string: %s", JPARSE_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 's':
	    /*
	     * So we don't trigger missing arg. Maybe there's another way but
	     * nothing is coming to my mind right now.
	     */
	    string_flag_used = true;
	    json_dbg(JSON_DBG_HIGH, __func__, "Calling parse_json(\"%s\", %ju, stderr):",
					      optarg, (uintmax_t)strlen(optarg));
	    /* parse arg as a block of json input */
	    parse_json(optarg, strlen(optarg), &valid_json);
	    if (valid_json) {
		errno = 0; /* pre-clear errno for warnp() */
		ret = printf("valid JSON\n");
		if (ret <= 0) {
		    warnp(__func__, "printf error printing valid JSON");
		}
	    }
	    break;
	default:
	    usage(2, "invalid -flag or missing option argument", program); /*ooo*/
	    not_reached();
	}
    }

    /*
     * case: process arguments on command line
     */
    if (argc - optind > 0) {

	/*
	 * process each argument in order
	 */
	for (i=optind; i < argc; ++i) {
	    parse_json_file(argv[i], &valid_json);
	    if (valid_json) {
		errno = 0; /* pre-clear errno for warnp() */
		ret = printf("valid JSON in file %s\n", argv[i]);
		if (ret <= 0) {
		    warnp(__func__, "printf error printing valid JSON");
		}
	    }
	}

    } else if (!string_flag_used) {
	usage(2, "-s string was not used and no file specified", program); /*ooo*/
	not_reached();
    }

    /*
     *  exit based on JSON parse success or failure
     */
    if (num_errors > 0 || valid_json == false) {
	exit(1); /*ooo*/
    }
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
 *
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
    fprintf_usage(exitcode, stderr, usage_msg, prog,
		  DBG_DEFAULT, json_verbosity_level, JSON_PARSER_VERSION, JPARSE_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
