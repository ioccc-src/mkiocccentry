/*
 * pr_jparse_test - test print function calls
 *
 * Here "print function call" refers to functions such as:
 *
 *    printf(3), fprintf(3), dprintf(3),
 *    vprintf(3), vfprintf(3), vdprintf(3)
 *
 * We also test various print-like functions from util.c such as:
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
 * "Share and Enjoy!"
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
#include "../jparse.h"

/*
 * pr_jparse_test - test print function call
 */
#include "pr_jparse_test.h"

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
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version strings and exit\n"
    "\n"
    "Exit codes:\n"
    "\t0\t\tall is OK\n"
    "\t1\t\tsome unexpected I/O error was detected\n"
    "\t2\t\t-h and help string printed or -V and version strings printed\n"
    "\t3\t\tcommand line error\n"
    "\t>=10\t\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";


/*
 * static variables
 */
static FILE *devnull = NULL;	/* write stream to /dev/null */


/*
 * forward declarations
 */
static int notatty_test(void);
static int vprint_test(char const *fmt, ...);
static int pr_jparse_test(char const *fmt, char const *string);
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
    int pr_jparse_test_cnt = 0;	/* error count from pr_jparse_test() */
    int i;

    /*
     * open a write stream to /dev/null
     */
    errno = 0;			/* pre-clear errno */
    devnull = fopen("/dev/null", "w");
    if (devnull == NULL) {
	errp(10, __func__, "FATAL: cannot open /dev/null for writing");
	not_reached();
    }

    /*
     * firewall - paranoia
     */
    if (stdout == NULL) {
	err(11, __func__, "stdout is NULL");
	not_reached();
    }
    if (stderr == NULL) {
	err(12, __func__, "stderr is NULL");
	not_reached();
    }


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
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version strings and exit */
	    print("pr_jparse_test version %s\n", PR_JPARSE_TEST_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
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
     * notatty_test - test explicit writes to /dev/null
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run notatty_test()", __func__);
    notatty_test_cnt = notatty_test();
    if (notatty_test_cnt > 0) {
	fwarn(stderr, __func__, "notatty_test error count: %d", notatty_test_cnt);
    }

    /*
     * vprint_test - test vfprintf and related variable argument list print functions
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run vprint_test(fmt, string)", __func__);
    vprint_test_cnt = vprint_test("vprint_test: %s\n", "vstring");
    if (vprint_test_cnt > 0) {
	fwarn(stderr, __func__, "vprint_test error count: %d", vprint_test_cnt);
    }

    /*
     * pr_jparse_test - test various print functions
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run pr_jparse_test(fmt, string)", __func__);
    pr_jparse_test_cnt = pr_jparse_test("pr_jparse_test: %s\n", "string");
    if (pr_jparse_test_cnt > 0) {
	fwarn(stderr, __func__, "pr_jparse_test error count: %d", pr_jparse_test_cnt);
    }

    /*
     * para_test - test various paragraph functions
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run para_test(fmt, string)", __func__);
    para_test("para_test", "pstring");

    /*
     * pr_test - test fpr() and pr() related macros
     */
    fdbg(stderr, DBG_LOW, "in %s: about to run pr_test()", __func__);
    pr_test();

    /*
     * exit depending on error count
     */
    if (notatty_test_cnt > 0 || vprint_test_cnt > 0 || pr_jparse_test_cnt > 0) {
	fdbg(stderr, DBG_LOW, "One or more tests FAILED");
	exit(1); /*ooo*/
    }
    fdbg(stderr, DBG_LOW, "All tests PASSED");
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
    int fd2;			/* file descriptor returned by fileno() */

    /*
     * firewall - paranoia
     */
    if (devnull == NULL) {
	err(13, __func__, "devnull is NULL");
	not_reached();
    }
    if (stdout == NULL) {
	err(14, __func__, "stdout is NULL");
	not_reached();
    }
    if (stderr == NULL) {
	err(15, __func__, "stderr is NULL");
	not_reached();
    }

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
	fd2 = fileno(devnull);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(devnull): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on tty /dev/null: fileno(devnull): %d, failed", fd2);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on non-tty /dev/null: fileno(devnull): %d, failed", fd2);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for fprintf on non-tty /dev/null: "
			"fileno(devnull): %d", __func__, fd2);
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
    va_list ap2;		/* copy of variable argument list */
    int ret;			/* libc function return value */
    int saved_errno;		/* preserved errno */
    int err_cnt = 0;		/* number of errors detected */
    int fd;			/* file descriptor returned by fileno() */
    int fd2;			/* file descriptor returned by fileno() */

    /*
     * firewall - paranoia
     */
    if (devnull == NULL) {
	err(16, __func__, "devnull is NULL");
	not_reached();
    }
    if (stdout == NULL) {
	err(17, __func__, "stdout is NULL");
	not_reached();
    }
    if (stderr == NULL) {
	err(18, __func__, "stderr is NULL");
	not_reached();
    }
    if (fmt == NULL) {
	err(19, __func__, "fmt is NULL");
	not_reached();
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * test vprintf on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(devnull, fmt, ap2)", __func__);
    va_copy(ap2, ap);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(devnull, fmt, ap2);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	fd2 = fileno(devnull);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(devnull): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty fileno(devnull): %d, failed", fd2);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty fileno(devnull): %d, failed", fd2);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty "
			"fileno(devnull): %d", __func__, fd2);
	}
    }
    va_end(ap2);

    /*
     * test vfprintf on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(stdout, fmt, ap2)", __func__);
    va_copy(ap2, ap);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(stdout, fmt, ap2);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	fd2 = fileno(stdout);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(stdout): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty fileno(stdout): %d, failed", fd2);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty fileno(stdout): %d, failed", fd2);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty "
			"fileno(stdout): %d", __func__, fd2);
	}
    }
    va_end(ap2);

    /*
     * test vfprintf on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfprintf(stderr, fmt, ap2)", __func__);
    va_copy(ap2, ap);
    errno = 0;			/* pre-clear errno */
    ret = vfprintf(stderr, fmt, ap2);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	fd2 = fileno(stderr);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(stderr): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on tty stderr: fileno(devnull): %d, failed", fd2);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vfprintf on non-tty stderr: fileno(stderr): %d, failed", fd2);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vfprintf on non-tty "
			"fileno(stderr): %d", __func__, fd2);
	}
    }
    va_end(ap2);

    /*
     * test vdprintf on /dev/null
     */
    fd = fileno(devnull);
    fdbg(stderr, DBG_HIGH, "in %s: fileno(devnull): %d", __func__, fd);
    fdbg(stderr, DBG_MED, "in %s: about to call vdprintf(%d, fmt, ap2)", __func__, fd);
    va_copy(ap2, ap);
    errno = 0;			/* pre-clear errno */
    ret = vdprintf(fd, fmt, ap2);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	fd2 = fileno(devnull);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(devnull): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on tty fileno(devnull): %d, failed", fd);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on non-tty fileno(devnull): %d, failed", fd);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vdprintf on non-tty "
			"fileno(devnull): %d", __func__, fd);
	}
    }
    va_end(ap2);

    /*
     * test vdprintf on stdout
     */
    fd = fileno(stdout);
    fdbg(stderr, DBG_HIGH, "in %s: fileno(stdout): %d", __func__, fd);
    fdbg(stderr, DBG_MED, "in %s: about to call vdprintf(%d, fmt, ap2)", __func__, fd);
    va_copy(ap2, ap);
    errno = 0;			/* pre-clear errno */
    ret = vdprintf(fd, fmt, ap2);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	fd2 = fileno(stdout);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(stdout): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on tty fileno(stdout): %d, failed", fd);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on non-tty fileno(stdout): %d, failed", fd);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vdprintf on non-tty "
			"fileno(stdout): %d", __func__, fd);
	}
    }
    va_end(ap2);

    /*
     * test vdprintf on fileno(stderr)
     */
    fd = fileno(stderr);
    fdbg(stderr, DBG_HIGH, "in %s: fileno(stderr): %d", __func__, fd);
    fdbg(stderr, DBG_MED, "in %s: about to call vdprintf(%d, fmt, ap2)", __func__, fd);
    va_copy(ap2, ap);
    errno = 0;			/* pre-clear errno */
    ret = vdprintf(fd, fmt, ap2);
    if (ret <= 0 || errno != 0) {

	/*
	 * case: error on a TTY
	 */
	saved_errno = errno;	/* preserve errno in case isatty() changes it */
	fd2 = fileno(stderr);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(stderr): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on tty fileno(stderr): %d, failed", fd);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (saved_errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "vdprintf on non-tty fileno(stderr): %d, failed", fd);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for vdprintf on non-tty "
			"fileno(stderr): %d", __func__, fd);
	}
    }
    va_end(ap2);

    /*
     * test vfpr on /dev/null
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(devnull, __func__, fmt, ap2)", __func__);
    va_copy(ap2, ap);
    vfpr(devnull, __func__, fmt, ap2);
    va_end(ap2);

    /*
     * test vfpr on stdout
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(stdout, __func__, fmt, ap2)", __func__);
    va_copy(ap2, ap);
    vfpr(stdout, __func__, fmt, ap2);
    va_end(ap2);

    /*
     * test vfpr on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call vfpr(stderr, __func__, fmt, ap2)", __func__);
    va_copy(ap2, ap);
    vfpr(stderr, __func__, fmt, ap2);
    va_end(ap2);

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
 * pr_jparse_test - test various print functions
 *
 * given:
 *	fmt	format string
 *	string	one string to print
 *
 * returns:
 *	error count
 */
static int
pr_jparse_test(char const *fmt, char const *string)
{
    int ret;			/* libc function return value */
    int saved_errno;		/* preserved errno */
    int err_cnt = 0;		/* number of errors detected */
    int fd;			/* file descriptor returned by fileno() */
    int fd2;			/* file descriptor returned by fileno() */

    /*
     * firewall - paranoia
     */
    if (devnull == NULL) {
	err(20, __func__, "devnull is NULL");
	not_reached();
    }
    if (stdout == NULL) {
	err(21, __func__, "stdout is NULL");
	not_reached();
    }
    if (stderr == NULL) {
	err(22, __func__, "stderr is NULL");
	not_reached();
    }
    if (fmt == NULL) {
	err(23, __func__, "fmt is NULL");
	not_reached();
    }
    if (string == NULL) {
	err(24, __func__, "string is NULL");
	not_reached();
    }

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
	fd2 = fileno(devnull);
	fdbg(stderr, DBG_HIGH, "in %s: fileno(devnull): %d", __func__, fd2);
	if (isatty(fd2)) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on tty fileno(devnull): %d, failed", fd2);
	    ++err_cnt;	/* count this error */

	/*
	 * case: non-ENOTTY error on a non-TTY
	 */
	} else if (errno != ENOTTY) {

	    errno = saved_errno;	/* restore errno in case isatty() changed it */
	    fwarnp(stderr, __func__, "fprintf on non-tty fileno(devnull): %d, failed", fd2);
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
	   fdbg(stderr, DBG_HIGH, "in %s: ignoring ENOTTY errno for fprintf on non-tty "
			"fileno(devnull): %d", __func__, fd2);
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
    fd = fileno(stdout);
    fdbg(stderr, DBG_HIGH, "in %s: fileno(stdout): %d", __func__, fd);
    fdbg(stderr, DBG_MED, "in %s: about to call dprintf(%d, fmt, ap)", __func__, fd);
    errno = 0;			/* pre-clear errno */
    ret = dprintf(fd, fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "dprintf on fileno(stdout): %d, failed", fd);
	++err_cnt;	/* count this error */
    }

    /*
     * test dprintf on stderr
     */
    fdbg(stderr, DBG_MED, "in %s: about to call dprintf(fileno(stderr), fmt, string)", __func__);
    fd = fileno(stderr);
    fdbg(stderr, DBG_HIGH, "in %s: fileno(stderr): %d", __func__, fd);
    fdbg(stderr, DBG_MED, "in %s: about to call dprintf(%d, fmt, ap)", __func__, fd);
    errno = 0;			/* pre-clear errno */
    ret = dprintf(fd, fmt, string);
    if (ret <= 0 || errno != 0) {
	fwarnp(stderr, __func__, "dprintf on fileno(stderr): %d, failed", fd);
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
     * firewall - paranoia
     */
    if (devnull == NULL) {
	err(25, __func__, "devnull is NULL");
	not_reached();
    }
    if (stdout == NULL) {
	err(26, __func__, "stdout is NULL");
	not_reached();
    }
    if (stderr == NULL) {
	err(27, __func__, "stderr is NULL");
	not_reached();
    }
    if (fmt == NULL) {
	err(28, __func__, "fmt is NULL");
	not_reached();
    }
    if (string == NULL) {
	err(29, __func__, "string is NULL");
	not_reached();
    }

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
     * firewall - paranoia
     */
    if (devnull == NULL) {
	err(30, __func__, "devnull is NULL");
	not_reached();
    }
    if (stdout == NULL) {
	err(31, __func__, "stdout is NULL");
	not_reached();
    }
    if (stderr == NULL) {
	err(32, __func__, "stderr is NULL");
	not_reached();
    }

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
     * firewall - paranoia
     */
    if (devnull == NULL) {
	err(33, __func__, "devnull is NULL");
	not_reached();
    }
    if (stdout == NULL) {
	err(34, __func__, "stdout is NULL");
	not_reached();
    }
    if (stderr == NULL) {
	err(35, __func__, "stderr is NULL");
	not_reached();
    }

    /*
     * firewall
     */
    if (prog == NULL) {
	prog = PR_JPARSE_TEST_BASENAME;
	fwarn(stderr, __func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	fwarn(stderr, __func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, PR_JPARSE_TEST_BASENAME,
	    PR_JPARSE_TEST_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
