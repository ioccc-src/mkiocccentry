/*
 * print_test - test print function call
 *
 * Here "print function call" refers to functions such as:
 *
 *    print(3), fprintf(3), dprintf(3),
 *    vprintf(3), vfprintf(3), vdprintf(3)
 *
 * We also test various print-like functions from jparse/util.c such as:
 *
 *    fprint(), fprstr(), print(), prstr(), para(), fpara(), fpr(), vfpr()
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Copyright (c) 2023 by Landon Curt Noll.  All Rights Reserved.
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
#include <stdarg.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "../util.h"


/*
 * official print_test version
 */
#define PRINT_TEST_VERSION "1.0 2023-07-28"	/* format: major.minor YYYY-MM-DD */

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
     * test vprintf to /dev/null
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
	    fwarnp(stderr, __func__, "fprintf on tty to /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on non-tty to /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for fprintf on non-tty to /dev/null", __func__);
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
     * test vprintf to /dev/null
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
	    fwarnp(stderr, __func__, "vfprintf on tty to /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty to /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty to /dev/null", __func__);
	   /* NOTE: this is not an error */
	}
    }

    /*
     * test vprintf to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(stdout, fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(stdout, fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(devnull))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty to stdout failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty to stdout failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty to stdout", __func__);
	   /* NOTE: this is not an error */
	}
    }

    /*
     * test vprintf to stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(stderr, fmt, ap)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(stderr, fmt, ap);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	if (isatty(fileno(devnull))) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty to stderr failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty to stderr failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty to stderr", __func__);
	   /* NOTE: this is not an error */
	}
    }

    /*
     * test vfpr to /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(devnull, __func__, fmt, ap)", __func__);
    vfpr(devnull, __func__, fmt, ap);

    /*
     * test vfpr to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(stdout, __func__, fmt, ap)", __func__);
    vfpr(stdout, __func__, fmt, ap);

    /*
     * test vfpr to stderr
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
     * test printf to /dev/null
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
	    fwarnp(stderr, __func__, "fprintf on tty to /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on non-tty to /dev/null failed");
	    ++err_cnt;	/* count this error */

	/*
	 * case: ignoring ENOTTY error on a non-TTY
	 */
	} else {

	   /*
	    * explain that we are ignoring ENOTTY errno on a non non-tty based stream
	    */
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty to /dev/null", __func__);
	   /* NOTE: this is not an error */
	}
    }

    /*
     * test printf to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprintf(stdout, fmt, string)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = fprintf(stdout, fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "fprintf to stdout failed");
	++err_cnt;	/* count this error */
    }

    /*
     * test printf to stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprintf(stderr, fmt, string)", __func__);
    errno = 0;			/* pre-clear errno */
    ret = fprintf(stderr, fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "fprintf to stderr failed");
	++err_cnt;	/* count this error */
    }

    /*
     * test pr to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call pr(__func__, fmt, string)", __func__);
    pr(__func__, fmt, string);

    /*
     * test fpr to /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpr(devnull, __func__, fmt, string)", __func__);
    fpr(devnull, __func__, fmt, string);

    /*
     * test fpr to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpr(stdout, __func__, fmt, string)", __func__);
    fpr(stdout, __func__, fmt, string);

    /*
     * test fpr to stderr
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
     * test para to /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call para(...)", __func__);
    para("test fpara to stdout",
         "next line is the fmt arg",
	 fmt,
	 "next line is the string arg",
	 string,
	 "last line of test para to stdout",
	 NULL  /* required end of string list */
	 );

    /*
     * test fpara to /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpara(devnull, ...)", __func__);
    fpara(devnull,
	  "test fpara to /dev/null",
          "next line is the fmt arg",
	  fmt,
	  "next line is the string arg",
	  string,
	  "last line of test fpara to /dev/null",
	 NULL  /* required end of string list */
	 );

    /*
     * test fpara to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpara(stdout, ...)", __func__);
    fpara(stdout,
	  "test fpara to stdout",
          "next line is the fmt arg",
	  fmt,
	  "next line is the string arg",
	  string,
	  "last line of test fpara to stdout",
	 NULL  /* required end of string list */
	 );

    /*
     * test fpara to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fpara(stderr, ...)", __func__);
    fpara(stderr,
	  "test fpara to stderr",
          "next line is the fmt arg",
	  fmt,
	  "next line is the string arg",
	  string,
	  "last line of test fpara to stderr",
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
     * test fprint to /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprint(devnull, ...)", __func__);

    /*
     * test fprstr to /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(devnull, ...)", __func__);
    fprstr(devnull, "testing fprstr to /dev/null\n");

    /*
     * test fprint to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprint(stdout, ...)", __func__);
    fprint(stdout, "testing fprint to stdout via: %s\n", __func__);

    /*
     * test fprstr to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(stdout, ...)", __func__);
    fprstr(stdout, "testing fprstr to stdout\n");

    /*
     * test print to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call print(...)", __func__);
    print("testing print to stdout via: %s\n", __func__);

    /*
     * test fprstr to stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(...)", __func__);
    prstr("testing prstr to stdout\n");

    /*
     * test fprint to stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprint(stderr, ...)", __func__);
    fprint(stderr, "testing fprint to stderr via: %s\n", __func__);

    /*
     * test fprstr to stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call fprstr(stderr, ...)", __func__);
    fprstr(stderr, "testing fprstr to stderr\n");
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
