/*
 * dyn_array_test - test the dynamic array facility
 *
 * Copyright (c) 2014,2015,2022-2024 by Landon Curt Noll.  All Rights Reserved.
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
 * dyn_array_test - test the dynamic array facility
 */
#include "dyn_test.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define CHUNK (1024)		/* allocate CHUNK elements at a time */

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
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\ttest suite failed\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    " >=10\tinternal error\n"
    "\n"
    "dyn_array library version: %s\n"
    "dyn_test version: %s";


/*
 * forward declarations
 */
static void usage(int exitcode, char const *str, char const *prog) __attribute__((noreturn));


/*
 * dbg forward declarations
 *
 * NOTICE: What follows are declarations lifted from dbg.c in the dbg repo:
 *
 *	https://github.com/lcn2/dbg
 *
 * We reproduce it here so that one may run this test code here without having to have
 * access to the dbg repo source code.
 */
#if !defined(DBG_USE)

#  if !defined(DBG_NONE)
#  define DBG_NONE (0)		   /* no debugging */
#  endif

#  if !defined(DBG_DEFAULT)
#  define DBG_DEFAULT (DBG_NONE)  /* default debugging level */
#  endif

int verbosity_level = DBG_DEFAULT;	/* maximum debug level for debug messages */
bool dbg_output_allowed = true;		/* false ==> disable debug messages */
bool warn_output_allowed = true;	/* false ==> disable warning messages */
bool err_output_allowed = true;		/* false ==> disable error messages */
bool usage_output_allowed = true;	/* false ==> disable usage messages */
bool msg_warn_silent = false;		/* true ==> silence info & warnings if verbosity_level <= 0 */

static void fdbg_write(FILE *stream, char const *caller, int level, char const *fmt, va_list ap);
static void fwarn_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap);
static void ferrp_write(FILE *stream, int error_code, char const *caller,
			char const *name, char const *fmt, va_list ap);
static void fwarnp_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap);
static void fusage_write(FILE *stream, int error_code, char const *caller, char const *fmt, va_list ap);

bool dbg_allowed(int level);
bool warn_allowed(void);
bool err_allowed(void);
bool usage_allowed(void);
void dbg(int level, char const *fmt, ...);
void fwarn(FILE *stream, char const *name, char const *fmt, ...);
void warnp(char const *name, char const *fmt, ...);
void errp(int exitcode, char const *name, char const *fmt, ...);
void fprintf_usage(int exitcode, FILE *stream, char const *fmt, ...);
int parse_verbosity(char const *optarg);
#endif


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    struct dyn_array *array;	/* dynamic array to test */
    double d;			/* test double */
    bool error = false;		/* true ==> test error found */
    intmax_t len = 0;		/* length of the dynamic array */
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
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    (void) printf("%s\n", DYN_TEST_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case ':':
	    (void) fprintf(stderr, "%s: requires an argument -- %c\n\n", program, optopt);
	    (void) usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	case '?':
	    (void) fprintf(stderr, "%s: illegal option -- %c\n\n", program, optopt);
	    (void) usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	default:
	    usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	}
    }
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /*
     * create dynamic array
     */
    array = dyn_array_create(sizeof(double), CHUNK, CHUNK, true);

    /*
     * load a million doubles
     */
    for (d = 0.0; d < 1000000.0; d += 1.0) {
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "moved data after appending d: %f", d);
	}
    }

    /*
     * verify values
     */
    for (i = 0; i < 1000000; ++i) {
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i));
	    error = true;
	}
    }

    /*
     * verify size
     */
    len = dyn_array_tell(array);
    if (len != 1000000) {
	warn(__func__, "dyn_array_tell(array): %jd != %jd", len, (intmax_t)1000000);
	error = true;
    }

    /*
     * concatenate the array onto itself
     */
    if (dyn_array_concat_array(array, array)) {
	dbg(DBG_LOW, "moved data after concatenation");
    }

    /*
     * verify new size
     */
    len = dyn_array_tell(array);
    if (len != 2000000) {
	warn(__func__, "dyn_array_tell(array): %jd != %jd", len, (intmax_t)2000000);
	error = true;
    }

    /*
     * verify values again
     */
    for (i = 0; i < 1000000; ++i) {
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i));
	    error = true;
	}
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i+1000000)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i+1000000));
	    error = true;
	}
    }

    /*
     * free dynamic array
     */
    if (array != NULL) {
	dyn_array_free(array);
	array = NULL;
    }

    /*
     * exit based on the test result
     */
    if (error == true) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "missing required argument(s), program: %s");
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
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, dyn_array_version, DYN_TEST_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * NOTICE: What follows is code lifted from dbg.c in the dbg repo:
 *
 *	https://github.com/lcn2/dbg
 *
 * We reproduce it here so that one may run this test code here without having to have
 * access to the dbg repo source code.
 */


#if !defined(DBG_USE)
/*
 * fdbg_write - write a diagnostic message to a stream
 *
 * Write a formatted debug diagnostic message to a stream. The diagnostic is followed by
 * a newline and then the stream is flushed.
 *
 * given:
 *	stream	open stream on which to write
 *	caller	name of the calling function
 *	level	debug level
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
fdbg_write(FILE *stream, char const *caller, int level, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write debug header
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stream, "debug[%d]: ", level);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fprintf error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * write diagnostic to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): vfprintf error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fputc error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fflush error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwarn_write - write a warning to a stream
 *
 * Write a warming message to a stream. The diagnostic is followed by
 * a newline and then the stream is flushed.
 *
 * given:
 *	stream	open stream on which to write
 *	caller	name of the calling function
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 */
static void
fwarn_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;			/* libc function return code */
    int saved_errno;		/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write warning header to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, "Warning: %s: ", name);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): vfprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fputc returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fflush(stream);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fflush returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * ferrp_write - write an error diagnostic with errno details, to a stream
 *
 * Write a formatted an error diagnostic with errno details to a stream.
 *
 * given:
 *	stream		open stream on which to write
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the error diagnostic
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed a other NULL pointers.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
ferrp_write(FILE *stream, int error_code, char const *caller,
	    char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */


    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write error diagnostic warning header to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stream, "ERROR[%d]: %s: ", error_code, name);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf #0 error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * write error diagnostic warning to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): vfprintf error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * write errno details plus newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stream, ": errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf #1 error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fflush error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwarnp_write - write a warning message with errno details, to a stream
 *
 * Write a warming message with errno info to a stream. The diagnostic is followed by
 * a newline and then the stream is flushed.
 *
 * given:
 *	stream	open stream on which to write
 *	caller	name of the calling function
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed other NULL pointers.
 */
static void
fwarnp_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;			/* libc function return code */
    int saved_errno;		/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write warning header to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, "Warning: %s: ", name);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): vfprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write errno details plus newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, ": errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in vwarnp(%s, %s, ap): fprintf with errno returned error: %s\n",
			       caller, name, fmt, strerror(errno));
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fflush(stream);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fflush returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fusage_write - write the usage message, to a stream
 *
 * Write a formatted the usage message to a stream. Checks for write
 * errors and call warnp() with a write error diagnostic.
 *
 * given:
 *	stream		open stream on which to write
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the usage message
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just exits).
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
fusage_write(FILE *stream, int error_code, char const *caller, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write the usage message to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): vfprintf error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fputc error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fflush error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * dbg_allowed - determine if verbosity level allows for debug messages are allowed
 *
 * given:
 *	level	write message if >= verbosity level
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
dbg_allowed(int level)
{
    /*
     * determine if verbosity level allows for debug messages
     */
    if (dbg_output_allowed == false || level > verbosity_level) {
	return false;
    }
    return true;
}


/*
 * warn_allowed - determine if warning messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
warn_allowed(void)
{
    /*
     * determine if warning messages are allowed
     */
    if (warn_output_allowed == false || (msg_warn_silent == true && verbosity_level <= 0)) {
	return false;
    }
    return true;
}


/*
 * err_allowed - determine if fatal error messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
err_allowed(void)
{
    /*
     * determine if fatal error messages are allowed
     */
    if (err_output_allowed == false) {
	return false;
    }
    return true;
}


/*
 * usage_allowed - determine if command line usage messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
usage_allowed(void)
{
    /*
     * determine if conditions allow command line usage messages
     */
    if (usage_output_allowed == false) {
	return false;
    }
    return true;
}


/*
 * dbg - write a verbosity level allowed debug message, to stderr
 *
 * given:
 *	level	write message if >= verbosity level
 *	fmt	printf format
 *	...
 *
 * Example:
 *
 *	dbg(1, "foobar information: %d", value);
 *
 * NOTE: If stderr is NULL, this function does nothing (just returns).
 */
void
dbg(int level, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - do nothing if stderr is NULL
     */
    if (stderr == NULL) {
	return;
    }

    /*
     * stage 0: determine if conditions allow function to write, return if not
     */
    allowed = dbg_allowed(level);
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stderr, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the diagnostic
     */
    fdbg_write(stderr, __func__, level, fmt, ap);

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwarn - write a warning message, to a stream
 *
 * given:
 *	stream	open stream to use
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	fwarn(stderr, __func__, "unexpected foobar: %d", value);
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 */
void
fwarn(FILE *stream, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - if stream is NULL, try stderr, unless that is also NULL
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	    stream = stderr;
        } else {
	    return;
	}
    }

    /*
     * stage 0: determine if conditions allow function to write, return if not
     */
    allowed = warn_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: name is NULL, forcing name to be: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: fmt is NULL, forcing fmt to be: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning
     */
    fwarn_write(stream, __func__, name, fmt, ap);

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * warnp - write a warning message with errno details, to stderr
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	warnp(__func__, "unexpected foobar: %d", value);
 *
 * NOTE: If stderr is NULL, this function does nothing (just returns).
 */
void
warnp(char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - do nothing if stderr is NULL
     */
    if (stderr == NULL) {
	return;
    }

    /*
     * stage 0: determine if conditions allow function to write, return if not
     */
    allowed = warn_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: called with NULL name, forcing name: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: called with NULL fmt, forcing fmt: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning with errno details
     */
    fwarnp_write(stderr, __func__, name, fmt, ap);

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * errp - write a fatal error message with errno details to stderr before exiting
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	errp(1, __func__, "bad foobar: %s", message);
 *
 * This function does not return.
 *
 * NOTE: If stderr is NULL, this function does nothing (just exits).
 */
void
errp(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
    if (allowed == false) {
	exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	not_reached();
    }

    /* stage 1: we will not return so we do not need to save errno */

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (exitcode < 0) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nexitcode < 0: %d\n", exitcode);
	}
	exitcode = 255;
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    } else if (exitcode > 255) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nexitcode > 255: %d\n", exitcode);
	}
	exitcode = 255;
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nname is NULL, forcing name to be: %s", name);
	}
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
	}
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    if (stderr != NULL) {
	ferrp_write(stderr, exitcode, __func__, name, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: do not restore errno, just exit
     */
    exit(exitcode);
    not_reached();
}


/*
 * fprintf_usage - write command line usage to a stream and, depending on exitcode, exit
 *
 * given:
 *	exitcode	- >= 0, exit with this code
 *			  < 0, just return
 *	stream		- stream to write on
 *	fmt		- format of the usage message
 *	...		- potential args for usage message
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just exits).
 */
void
fprintf_usage(int exitcode, FILE *stream, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * stage 0: determine if conditions allow function to write, exit or return as required
     */
    allowed = usage_allowed();
    if (allowed == false) {
	if (exitcode >= 0) {
	    exit(exitcode);
	    not_reached();
	}
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (fmt == NULL) {
	fmt = "no usage message given";
	if (stream != NULL) {
	    fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
	}
    }

    /*
     * stage 4: write command line usage
     */
    if (stream != NULL) {
	fusage_write(stream, exitcode, __func__, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: exit if exitcode >= 0, else restore errno
     */
    if (exitcode >= 0) {
	exit(exitcode);
	not_reached();
    } else {
	errno = saved_errno;
    }
    return;
}


/*
 * parse_verbosity - parse -v optarg for our tools
 *
 * given:
 *	optarg		verbosity string, must be an integer >= 0
 *
 * returns:
 *	parsed verbosity or DBG_INVALID on conversion error
 */
int
parse_verbosity(char const *optarg)
{
    int verbosity = DBG_NONE;	/* parsed verbosity or DBG_NONE */

    /*
     * firewall
     */
    if (optarg == NULL) {
	return DBG_INVALID;
    }

    /*
     * parse verbosity
     */
    errno = 0;		/* pre-clear errno for warnp() */
    verbosity = (int)strtol(optarg, NULL, 0);
    if (errno != 0) {
	return DBG_INVALID;
    }
    if (verbosity < 0) {
	return DBG_INVALID;
    }
    return verbosity;
}
#endif /* !defined(DBG_USE) */
