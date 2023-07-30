/*
 * print_test - test print function call
 *
 * Here "print function call" refers to functions such as:
 *
 *    printf(3), fprintf(3), dprintf(3),
 *    vprintf(3), vfprintf(3), vdprintf(3)
 *
 * We also test various print-like functions from jparse/util.c such as:
 *
 *    fprint(), fprstr(), print(), prstr(), para(), fpara(), fpr(), vfpr()
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * This JSON parser was co-developed in 2022 by:
 *
 *	@xexyl
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

#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "../util.h"


/*
 * official print_test version
 */
#define PRINT_TEST_VERSION "1.0.1 2023-07-30"	/* format: major.minor YYYY-MM-DD */

/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command readline_buf */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V]\n"
    "\n"
    "\t-h\t\tPrint help message and exit\n"
    "\t-v level\tSet verbosity level (def level: %d)\n"
    "\t-V\t\tPrint version string and exit\n"
    "\n"
    "Exit codes:\n"
    "\t0\t\tall is OK\n"
    "\t1\t\tsome unexpected I/O error was detected\n"
    "\t2\t\t-h and help string printed or -V and version string printed\n"
    "\t3\t\tcommand line error\n"
    "\t>=10\t\tinternal error\n"
    "\n"
    "print_test version: %s";


/*
 * static variables
 */
static FILE *devnull = NULL;	/* write stream to /dev/null */


/*
 * forward declarations
 */
static int notatty_test(void);
static int vprint_test(char const *fmt, ...);
static int print_test(char const *fmt, char const *string);
static void para_test(char const *fmt, char const *string);
static void pr_test(void);
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    int arg_count = 0;		/* number of args to process */
    int notatty_test_cnt = 0;	/* error count from notatty_test() */
    int vprint_test_cnt = 0;	/* error count from vprint_test() */
    int print_test_cnt = 0;	/* error count from print_test() */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:V")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, ""); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", PRINT_TEST_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
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
    if (arg_count != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /*
     * open a write stream to /dev/null
     */
    fdbg(stderr, DBG_LOW, "in %s: about to open /dev/null for writing", __func__);
    errno = 0;			/* pre-clear errno */
    devnull = fopen("/dev/null", "w");
    if (devnull == NULL) {
	errp(10, __func__, "FATAL: cannot open /dev/null for writing");
	not_reached();
    }

    /*
     * notatty_test - test explicit writes to /dev/null
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run notatty_test", __func__);
    notatty_test_cnt = notatty_test();
    if (notatty_test_cnt > 0) {
	fwarn(stderr, __func__, "notatty_test error count: %d", notatty_test_cnt);
    }

    /*
     * vprint_test - test vfprintf and related variable argument list print functions
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run vprint_test", __func__);
    vprint_test_cnt = vprint_test("vprint_test: %s\n", "vstring");
    if (vprint_test_cnt > 0) {
	fwarn(stderr, __func__, "vprint_test error count: %d", vprint_test_cnt);
    }

    /*
     * print_test - test various print functions
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run print_test", __func__);
    print_test_cnt = print_test("print_test: %s\n", "string");
    if (print_test_cnt > 0) {
	fwarn(stderr, __func__, "print_test error count: %d", print_test_cnt);
    }

    /*
     * para_test - test various paragraph functions
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run para_test", __func__);
    para_test("para_test", "pstring");

    /*
     * pr_test - test fpr() and pr() related macros
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run pr_test", __func__);
    pr_test();

    /*
     * exit depending on error count
     */
    if (notatty_test_cnt > 0 || vprint_test_cnt > 0 || print_test_cnt > 0) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


/*
 * notatty_test - test fprintf print operations on a non-TTY
 *
 * In macOS, the first write to /dev/null via a function such as
 * fprintf(3) will generate an errno of ENOTTY.  Subsequent writes
 * to /dev/null via fprintf(3) will not generate such an error.
 *
 * returns:
 *	error count
 */
static int
notatty_test(void)
{
    int ret;			/* libc function return value */
    int saved_errno;		/* preserved errno */
    int err_cnt = 0;		/* number of errors detected */

    /*
     * test fprintf on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprintf(devnull, ...)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = fprintf(devnull, "writing to /dev/null should not produce any output\n");
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(devnull))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on non-tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for fprintf on non-tty /dev/null", __func__);
	   /* NOTE: this is not an error */
	}
    }

    /*
     * return the error count
     */
    return err_cnt;
}


/*
 * vprint_test - test vfprintf and related variable argument list print functions
 *
 * given:
 *	fmt	format string
 *	...	at lease one string to print
 *
 * returns:
 *	error count
 */
static int
vprint_test(char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int ret;			/* libc function return value */
    int saved_errno;		/* preserved errno */
    int err_cnt = 0;		/* number of errors detected */

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * test vprintf on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(devnull, fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(devnull, fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(devnull))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty /dev/null", __func__);
	   /* NOTE: this is not an error */
	}
    }

    /*
     * test vfprintf on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(stdout, fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(stdout, fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(stdout))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty stdout failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty stdout failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    *
	    * NOTE: this is not an error
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty stdout", __func__);
	}
    }

    /*
     * test vfprintf on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(stderr, fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(stderr, fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(stderr))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty stderr failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty stderr failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    *
	    * NOTE: this is not an error
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty stderr", __func__);
	}
    }

    /*
     * test vprintf on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vdprintf(fileno(devnull), fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vdprintf(fileno(devnull), fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(devnull))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on non-tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vdprintf on non-tty /dev/null", __func__);
	   /* NOTE: this is not an error */
	}
    }


    /*
     * test vdprintf on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vdprintf(stdout, fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vdprintf(fileno(stdout), fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(stdout))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on tty fileno(stdout) failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on non-tty fileno(stdout) failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    *
	    * NOTE: this is not an error
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vdprintf on non-tty fileno(stdout)", __func__);
	}
    }

    /*
     * test vdprintf on fileno(stderr)
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vdprintf(fileno(stderr), fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vdprintf(fileno(stderr), fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(stderr))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on tty fileno(stderr) failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on non-tty fileno(stderr) failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    *
	    * NOTE: this is not an error
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vdprintf on non-tty fileno(stderr)", __func__);
	}
    }


    /*
     * test vfpr on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(devnull, __func__, fmt, ap)", __func__);
    vfpr(devnull, __func__, fmt, ap);

    /*
     * test vfpr on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(stdout, __func__, fmt, ap)", __func__);
    vfpr(stdout, __func__, fmt, ap);

    /*
     * test vfpr on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(stderr, __func__, fmt, ap)", __func__);
    vfpr(stderr, __func__, fmt, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * return the error count
     */
    return err_cnt;
}


/*
 * print_test - test various print functions
 *
 * given:
 *	fmt	format string
 *	string	one string to print
 *
 * returns:
 *	error count
 */
static int
print_test(char const *fmt, char const *string)
{
    int ret;			/* libc function return value */
    int saved_errno;		/* preserved errno */
    int err_cnt = 0;		/* number of errors detected */

    /*
     * test fprintf on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprintf(devnull, fmt, string)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = fprintf(devnull, fmt, string);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(devnull))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on non-tty /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    *
	    * NOTE: this is not an error.
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty to /dev/null", __func__);
	}
    }

    /*
     * test fprintf on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprintf(stdout, fmt, string)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = fprintf(stdout, fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "fprintf on stdout failed");
	++err_cnt;	/* count this error */
    }

    /*
     * test fprintf on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprintf(stderr, fmt, string)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = fprintf(stderr, fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "fprintf on stderr failed");
	++err_cnt;	/* count this error */
    }

    /*
     * test dprintf on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call dprintf(fileno(stdout), fmt, string)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = dprintf(fileno(stdout), fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "dprintf on fileno(stdout) failed");
	++err_cnt;	/* count this error */
    }

    /*
     * test dprintf on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call dprintf(fileno(stderr), fmt, string)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = dprintf(fileno(stderr), fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "dprintf on stderr failed");
	++err_cnt;	/* count this error */
    }


    /*
     * test pr on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call pr(__func__, fmt, string)", __func__);
    pr(__func__, fmt, string);

    /*
     * test fpr on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpr(devnull, __func__, fmt, string)", __func__);
    fpr(devnull, __func__, fmt, string);

    /*
     * test fpr on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpr(stdout, __func__, fmt, string)", __func__);
    fpr(stdout, __func__, fmt, string);

    /*
     * test fpr on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpr(stderr, __func__, fmt, string)", __func__);
    fpr(stderr, __func__, fmt, string);

    /*
     * return the error count
     */
    return err_cnt;
}


/*
 * para_test - test various paragraph functions
 *
 * given:
 *	fmt	format string
 *	string	one string to print
 */
static void
para_test(char const *fmt, char const *string)
{
    /*
     * test para on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call para(...)", __func__);
    para("test fpara on stdout",
         "next line is the fmt arg",
	 fmt,
	 "next line is the string arg",
	 string,
	 "last line of test para on stdout",
	 NULL  /* required end of string list */
	 );

    /*
     * test fpara on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpara(devnull, ...)", __func__);
    fpara(devnull,
	  "test fpara on /dev/null",
          "next line is the fmt arg",
	  fmt,
	  "next line is the string arg",
	  string,
	  "last line of test fpara on /dev/null",
	 NULL  /* required end of string list */
	 );

    /*
     * test fpara on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpara(stdout, ...)", __func__);
    fpara(stdout,
	  "test fpara on stdout",
          "next line is the fmt arg",
	  fmt,
	  "next line is the string arg",
	  string,
	  "last line of test fpara on stdout",
	 NULL  /* required end of string list */
	 );

    /*
     * test fpara on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpara(stderr, ...)", __func__);
    fpara(stderr,
	  "test fpara on stderr",
          "next line is the fmt arg",
	  fmt,
	  "next line is the string arg",
	  string,
	  "last line of test fpara on stderr",
	 NULL  /* required end of string list */
	 );

    return;
}


/*
 * pr_test - test fpr() and pr() related macros
 */
static void
pr_test(void)
{
    /*
     * test fprint on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprint(devnull, ...)", __func__);
    fprint(devnull, "testing fprint on /dev/null via: %s\n", __func__);

    /*
     * test fprstr on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(devnull, ...)", __func__);
    fprstr(devnull, "testing fprstr on /dev/null\n");

    /*
     * test fprint on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprint(stdout, ...)", __func__);
    fprint(stdout, "testing fprint on stdout via: %s\n", __func__);

    /*
     * test fprstr on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(stdout, ...)", __func__);
    fprstr(stdout, "testing fprstr on stdout\n");

    /*
     * test print on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call print(...)", __func__);
    print("testing print on stdout via: %s\n", __func__);

    /*
     * test prstr on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(...)", __func__);
    prstr("testing prstr on stdout\n");

    /*
     * test fprintf on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprint(stderr, ...)", __func__);
    fprint(stderr, "testing fprint on stderr via: %s\n", __func__);

    /*
     * test fprstr on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(stderr, ...)", __func__);
    fprstr(stderr, "testing fprstr on stderr\n");
    return;
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
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, PRINT_TEST_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
