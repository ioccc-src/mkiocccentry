/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * dbg - info, debug, warning, error and usage message facility
 *
 * See the following README in the dbg repo for more information:
 *
 *	https://github.com/lcn2/dbg/blob/master/README.md
 *
 * The non-static exported dbg interface functions have seven stages:
 *
 *	stage 0: determine if conditions allow the function to write (return or
 *	         exit as required)
 *
 *	stage 1: save errno so we can restore it before returning (unless we
 *	         must exit)
 *
 *	stage 2: stdarg variable argument list setup (unless va_list ap is the
 *		 last arg)
 *
 *	stage 3: firewall checks
 *
 *	stage 4: write actions as required by the function
 *
 *	stage 5: stdarg variable argument list cleanup
 *
 *	stage 6: restore previous errno value or exit
 *
 * The static foo_write() functions are the functions that actually write to the
 * stream. Each function has a non-static counterpart that calls the static
 * version during stage 4 of the process.
 *
 * NOTE: The functions prefixed with 'f' write to a FILE * passed to the
 * function to allow you to write to another stream other than stderr like
 * stdout or an open file.
 *
 * While you may be tempted to try and combine functions in this code, we
 * recommend against doing so: the more you make functions call other functions
 * the more complex you make a debugging, say with a tool such as lldb or gdb,
 * when you are trying to trace problems in your application.
 *
 * We chose to use static foo_write() functions in part because it made it
 * easier to set a breakpoint (say, in lldb of gdb) before writes occur.
 *
 * IMPORTANT WARNING: This code is widely used by a number of applications. A
 *		      great deal of care has gone in to making these debugging
 *		      facilities easy to code with, and much less likely to be
 *		      the source (pun intended) of bugs.
 *
 *		      We apologize in advance for any problems this code may
 *		      introduce. We would be happy to fix a general bug by
 *		      considering a pull request to the dbg repo.
 *
 * DBG repo: https://github.com/lcn2/dbg
 *
 *		      You may also report a bug in the form of an issue using
 *		      the above URL.
 *
 *		      Improvements -- including fixing typos in comments and
 *		      addressing compiler warning messages -- are very much
 *		      appreciated.
 *
 * ON PULL REQUESTS: Debugging an application program can be a frustrating
 *		     process for some people. If you feel you need to issue a
 *		     pull request to fix and/or improve this code, please keep
 *		     the more general debugging context in mind, and please take
 *		     extra care to test your proposed modifications so that you
 *		     don't complicate the debugging process for others.
 *
 *		     Please maintain the coding style of this code -- even if it
 *		     is not your style -- in your pull request.  A consistent
 *		     style is much easier to understand than a style patchwork.
 *		     A good example is testing booleans in the form of foo ==
 *		     false instead of !foo.
 *
 *		     We do welcome pull requests on this code. If you have any
 *		     questions on how best to form a pull request, or would like
 *		     some minor help in forming one, please consider asking by
 *		     opening an issue at dbg repo.
 *
 * Copyright (c) 1989,1997,2018-2022 by Landon Curt Noll.  All Rights Reserved.
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
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"


/*
 * global message control variables
 *
 * NOTE: These variables are initialized to defaults below.
 */
int verbosity_level = DBG_DEFAULT;	/* maximum debug level for debug messages */
bool msg_output_allowed = true;		/* false ==> disable informational messages */
bool dbg_output_allowed = true;		/* false ==> disable debug messages */
bool warn_output_allowed = true;	/* false ==> disable warning messages */
bool err_output_allowed = true;		/* false ==> disable error messages */
bool usage_output_allowed = true;	/* false ==> disable usage messages */
bool msg_warn_silent = false;		/* true ==> silence info & warnings if verbosity_level <= 0 */


#if defined(DBG_TEST)
#include <getopt.h>


/*
 * usage message
 *
 * The follow usage message came from an early draft of mkiocccentry. This is
 * just an example usage string: there is no mkiocccentry functionality here.
 */
static char const * const usage =
"usage: %s [-h] [-v level] [-V] [-q] [-e errno] foo bar [baz]\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
"\t-e errno\tsimulate setting of errno to cause errp() to be involved\n"
"\n"
"\tfoo\t\ta required arg\n"
"\tbar\t\tanother required arg\n"
"\tbaz\t\tan optional arg\n"
"\n"
"NOTE: This is just a demo. Arguments are ignored and may be of any value.\n"
"\n"
"Version: %s";
#endif /* DBG_TEST */


/*
 * static function declarations
 */
static void fmsg_write(FILE *stream, char const *caller, char const *fmt, va_list ap);
static void snmsg_write(char *str, size_t size, char const *caller, char const *fmt, va_list ap);
static void fdbg_write(FILE *stream, char const *caller, int level, char const *fmt, va_list ap);
static void sndbg_write(char *str, size_t size, char const *caller, int level, char const *fmt, va_list ap);
static void fwarn_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap);
static void snwarn_write(char *str, size_t size, char const *caller, char const *name, char const *fmt, va_list ap);
static void fwarnp_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap);
static void snwarnp_write(char *str, size_t size, char const *caller, char const *name, char const *fmt, va_list ap);
static void ferr_write(FILE *stream, int error_code, char const *caller,
		       char const *name, char const *fmt, va_list ap);
static void snerr_write(char *str, size_t size, int error_code, char const *caller,
			char const *name, char const *fmt, va_list ap);
static void ferrp_write(FILE *stream, int error_code, char const *caller,
			char const *name, char const *fmt, va_list ap);
static void snerrp_write(char *str, size_t size, int error_code, char const *caller,
			 char const *name, char const *fmt, va_list ap);
static void fusage_write(FILE *stream, int error_code, char const *caller, char const *fmt, va_list ap);


/*
 * fmsg_write - write a message to a stream
 *
 * Write a formatted message to a stream.  The message is followed by
 * a newline and then the stream is flushed.
 *
 * given:
 *	stream	open stream on which to write
 *	caller	name of the calling function (__func__, for example)
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
fmsg_write(FILE *stream, char const *caller, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

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
     * write message to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret <= 0) {
	warnp(caller, "\nin %s(stream, %s, %s, ap): vfprintf error\n", __func__, caller, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %s, ap): fputc error\n",
		      __func__, caller, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %s, ap): fflush error\n",
		      __func__, caller, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snmsg_write - copy an informational message string into a buffer
 *
 * Copy a formatted message string into a fixed sized buffer.
 *
 * If the string is too large, the string will be truncated and warning will be issued.
 *
 * Unlike fmsg_write(), this function does NOT add a final newline to str.
 *
 * The str[size-1] byte is set to NUL (if str != NULL && size > 0) in paranoia.
 * I.e., str is always NUL terminated unless str == NULL or size == 0.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *	caller	name of the calling function (__func__, for example)
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 * NOTE: This function does nothing (just returns) if a size <= 1.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
snmsg_write(char *str, size_t size, char const *caller, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - just return if given a NULL ptr
     */
    if (str == NULL || caller == NULL || fmt == NULL) {
	return;
    }
    if (size > 0) {
	str[size-1] = '\0';	/* paranoia */
    }
    if (size <= 1) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write message to string
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vsnprintf(str, size, fmt, ap);
    if ((size_t)ret >= size) {
	warnp(caller, "\nin %s(str, %zu, %s, %s, ap): vsnprintf returned: %d\n",
		      __func__, size, caller, fmt, ret);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


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
 * sndbg_write - write a diagnostic message string into a buffer
 *
 * Copy a formatted debug diagnostic message string into a fixed sized buffer.
 *
 * If the string is too large, the string will be truncated and warning will be issued.
 *
 * Unlike fdbg_write(), this function does NOT add a final newline to str.
 *
 * The str[size-1] byte is set to NUL (if str != NULL && size > 0) in paranoia.
 * I.e., str is always NUL terminated unless str == NULL or size == 0.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *	caller	name of the calling function
 *	level	debug level
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
sndbg_write(char *str, size_t size, char const *caller, int level, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int ret2;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - just return if given a NULL ptr
     */
    if (str == NULL || caller == NULL || fmt == NULL) {
	return;
    }
    if (size > 0) {
	str[size-1] = '\0';	/* paranoia */
    }
    if (size <= 1) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write debug header to string
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = snprintf(str, size, "debug[%d]: ", level);
    if ((size_t)ret >= size) {
	warnp(caller, "\nin %s(str, %zu, %s, %d, %s, ap): snprintf returned: %d\n",
		      __func__, size, caller, level, fmt, ret);
    }

    /*
     * write diagnostic to string
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret2 = vsnprintf(str+ret, size-ret, fmt, ap);
    if ((size_t)ret2 >= size-ret) {
	warnp(caller, "\nin %s(str, %zu, %s, %s, ap): "
		      "snprintf returned: %d vsnprintf returned: %d\n",
		      __func__, size, caller, fmt, ret, ret2);
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
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 */
static void
fwarn_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

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
	(void) fprintf(stderr, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): vfprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fputc returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fflush(stream);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fflush returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snwarn_write - copy a warning message string into a buffer
 *
 * Copy a formatted warning message string into a fixed sized buffer.
 *
 * If the string is too large, the string will be truncated and warning will be issued.
 *
 * Unlike fwarn_write(), this function does NOT add a final newline to str.
 *
 * The str[size-1] byte is set to NUL (if str != NULL && size > 0) in paranoia.
 * I.e., str is always NUL terminated unless str == NULL or size == 0.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *	caller	name of the calling function
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 */
static void
snwarn_write(char *str, size_t size, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int ret2;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - just return if given a NULL ptr
     */
    if (str == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }
    if (size > 0) {
	str[size-1] = '\0';	/* paranoia */
    }
    if (size <= 1) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write warning header to string
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = snprintf(str, size, "Warning: %s: ", name);
    if ((size_t)ret >= size) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(str, %zu, %s, %s, %s, ap): snprintf returned: %d\n",
			       caller, __func__, size, caller, name, fmt, ret);
    }

    /*
     * write warning to string
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret2 = vsnprintf(str+ret, size-ret, fmt, ap);
    if ((size_t)ret2 >= size-ret) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(str, %zu, %s, %s, %s, ap): "
			       "snprintf returned: %d vsnprintf returned: %d\n",
			       caller, __func__, size, caller, name, fmt, ret, ret2);
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
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 */
static void
fwarnp_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

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
	(void) fprintf(stderr, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): vfprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write errno details plus newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, ": errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in vwarnp(%s, %s, ap): fprintf with errno returned error: %s\n",
			       caller, name, fmt, strerror(errno));
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fflush(stream);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fflush returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snwarnp_write - copy a warning message string with errno details to a buffer
 *
 * Copy a formatted warning message string with errno details to a fixed sized buffer.
 *
 * If the string is too large, the string will be truncated and warning will be issued.
 *
 * Unlike fwarnp_write(), this function does NOT add a final newline to str.
 *
 * The str[size-1] byte is set to NUL (if str != NULL && size > 0) in paranoia.
 * I.e., str is always NUL terminated unless str == NULL or size == 0.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *	caller	name of the calling function
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 */
static void
snwarnp_write(char *str, size_t size, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int ret2;		/* libc function return code */
    int ret3;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - just return if given a NULL ptr
     */
    if (str == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }
    if (size > 0) {
	str[size-1] = '\0';	/* paranoia */
    }
    if (size <= 1) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write warning header to string
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = snprintf(str, size, "Warning: %s: ", name);
    if ((size_t)ret >= size) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(str, %zu, %s, %s, %s, ap): snprintf returned: %d\n",
			       caller, __func__, size, caller, name, fmt, ret);
    }

    /*
     * write warning to string
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret2 = vsnprintf(str+ret, size-ret, fmt, ap);
    if ((size_t)ret2 >= size-ret) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(str, %zu, %s, %s, %s, ap): "
			       "snprintf returned: %d vsnprintf returned: %d\n",
			       caller, __func__, size, caller, name, fmt, ret, ret2);
    }

    /*
     * write errno details plus newline to string
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret3 = snprintf(str+ret+ret2, size-ret-ret2, ": errno[%d]: %s", saved_errno, strerror(saved_errno));
    if ((size_t)ret3 >= size-ret-ret2) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(str, %zu, %s, %s, %s, ap): "
			       "snprintf returned: %d vsnprintf returned: %d 2nd snprintf returned: %d\n",
			       caller, __func__, size, caller, name, fmt, ret, ret2, ret3);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * ferr_write - write an error diagnostic, to a stream
 *
 * Write a formatted an error diagnostic to a stream.
 *
 * given:
 *	stream		open stream on which to write
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the error diagnostic
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
ferr_write(FILE *stream, int error_code, char const *caller,
	   char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

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
     * write error diagnostic header to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stderr, "ERROR[%d]: %s: ", error_code, name);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf error\n",
			       __func__, caller, error_code, name, fmt);
    }

    /*
     * write error diagnostic warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): vfprintf error\n",
			       __func__, caller, error_code, name, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fputc error\n",
			       __func__, caller, error_code, name, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
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
 * snerr_write - copy an error diagnostic string into a buffer
 *
 * Copy a error diagnostic string into a fixed sized buffer.
 *
 * If the string is too large, the string will be truncated and warning will be issued.
 *
 * Unlike ferr_write(), this function does NOT add a final newline to str.
 *
 * The str[size-1] byte is set to NUL (if str != NULL && size > 0) in paranoia.
 * I.e., str is always NUL terminated unless str == NULL or size == 0.
 *
 * given:
 *	str		pointer to buffer in which to write a message
 *	size		size of buffer, including space for a final NUL byte
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the error diagnostic
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
snerr_write(char *str, size_t size, int error_code, char const *caller,
	    char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int ret2;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - just return if given a NULL ptr
     */
    if (str == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }
    if (size > 0) {
	str[size-1] = '\0';	/* paranoia */
    }
    if (size <= 1) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write error diagnostic header to string
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = snprintf(str, size, "ERROR[%d]: %s: ", error_code, name);
    if ((size_t)ret >= size) {
	warnp(caller, "\nin %s(str, %zu, %s, %d, %s, %s, ap): "
		      "snprintf returned: %d",
		      __func__, size, caller, error_code, name, fmt, ret);
    }

    /*
     * write error diagnostic to string
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret2 = vsnprintf(str+ret, size-ret, fmt, ap);
    if ((size_t)ret2 >= size-ret) {
	warnp(caller, "\nin %s(str, %zu, %s, %d, %s, %s, ap): "
		      "snprintf returned: %d vsnprintf returned: %d\n",
		      __func__, size, caller, error_code, name, fmt, ret, ret2);
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
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
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
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stderr, "ERROR[%d]: %s: ", error_code, name);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf #0 error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * write error diagnostic warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): vfprintf error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * write errno details plus newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, ": errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf #1 error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
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
 * snerrp_write - copy an error diagnostic string with errno details to a buffer
 *
 * Copy an error diagnostic string with errno details into a fixed sized buffer.
 *
 * If the string is too large, the string will be truncated and warning will be issued.
 *
 * Unlike ferrp_write(), this function does NOT add a final newline to str.
 *
 * The str[size-1] byte is set to NUL (if str != NULL && size > 0) in paranoia.
 * I.e., str is always NUL terminated unless str == NULL or size == 0.
 *
 * given:
 *	str		pointer to buffer in which to write a message
 *	size		size of buffer, including space for a final NUL byte
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the error diagnostic
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
snerrp_write(char *str, size_t size, int error_code, char const *caller,
	     char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int ret2;		/* libc function return code */
    int ret3;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - just return if given a NULL ptr
     */
    if (str == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }
    if (size > 0) {
	str[size-1] = '\0';	/* paranoia */
    }
    if (size <= 1) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write error diagnostic header to string
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = snprintf(str, size, "ERROR[%d]: %s: ", error_code, name);
    if ((size_t)ret >= size) {
	warnp(caller, "\nin %s(str, %zu, %s, %d, %s, %s, ap): "
		      "snprintf returned: %d",
		      __func__, size, caller, error_code, name, fmt, ret);
    }

    /*
     * write error diagnostic to string
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret2 = vsnprintf(str+ret, size-ret, fmt, ap);
    if ((size_t)ret2 >= size-ret) {
	warnp(caller, "\nin %s(str, %zu, %s, %d, %s, %s, ap): "
		      "snprintf returned: %d vsnprintf returned: %d\n",
		      __func__, size, caller, error_code, name, fmt, ret, ret2);
    }

    /*
     * write errno details plus newline to string
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret3 = snprintf(str+ret+ret2, size-ret-ret2, ": errno[%d]: %s", saved_errno, strerror(saved_errno));
    if ((size_t)ret3 >= size-ret-ret2) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in %s(str, %zu, %s, %s, %s, ap): "
			       "snprintf returned: %d vsnprintf returned: %d 2nd snprintf returned: %d\n",
			       caller, __func__, size, caller, name, fmt, ret, ret2, ret3);
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
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): vfprintf error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fputc error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
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
 * msg_allowed - determine if generic messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
msg_allowed(void)
{
    /*
     * determine generic messages are allowed
     */
    if (msg_output_allowed == false || (msg_warn_silent == true && verbosity_level <= 0)) {
	return false;
    }
    return true;
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
 * msg - write a generic message, to stderr
 *
 * given:
 *      fmt     printf format
 *      ...
 *
 * Example:
 *
 *      msg("foobar information");
 *      msg("foo = %d\n", foo);
 */
void
msg(char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions function to write, return if not
     */
    allowed = msg_allowed();
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
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the message
     */
    fmsg_write(stderr, __func__, fmt, ap);

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
 * vmsg - write a generic message, to stderr, in va_list form
 *
 * given:
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *      vmsg(__func__, "foobar information", ap);
 *      vmsg(__func__, "foo = %d\n", ap);
 */
void
vmsg(char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions function to write, return if not
     */
    allowed = msg_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the message
     */
    fmsg_write(stderr, __func__, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fmsg - write a generic message, to a stream
 *
 * given:
 *	stream	open stream to use
 *      fmt     printf format
 *      ...
 *
 * Example:
 *
 *      fmsg(stderr, "foobar information");
 *      fmsg(stderr, "foo = %d\n", foo);
 */
void
fmsg(FILE *stream, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions function to write, return if not
     */
    allowed = msg_allowed();
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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the message
     */
    fmsg_write(stream, __func__, fmt, ap);

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
 * vfmsg - write a generic message, to a stream, in va_list form
 *
 * given:
 *	stream	open stream to use
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *      vfmsg(stderr, __func__, "foobar information", ap);
 *      vfmsg(stderr, __func__, "foo = %d\n", ap);
 */
void
vfmsg(FILE *stream, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions function to write, return if not
     */
    allowed = msg_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the message
     */
    fmsg_write(stream, __func__, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snmsg - copy a generic message string into a buffer
 *
 * Copy an informational message string into a fixed sized buffer.
 *
 * Unlike msg(), this function does NOT add a final newline to str.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *      fmt     printf format
 *      ...
 *
 * Example:
 *
 *      snmsg(buf, BUFSIZ, "foobar information");
 *      snmsg(buf, BUFSIZ, "foo = %d\n", foo);
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 * NOTE: This function does nothing (just returns) if a size <= 1.
 */
void
snmsg(char *str, size_t size, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions function to write, return if not
     */
    allowed = msg_allowed();
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
    if (str == NULL) {
	return;
    }
    if (fmt == NULL) {
	return;
    }

    /*
     * stage 4: write the message
     */
    snmsg_write(str, size, __func__, fmt, ap);

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
 */
void
dbg(int level, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
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
 * vdbg - write a verbosity level allowed debug message, to stderr, in va_list form
 *
 * given:
 *	level	write message if >= verbosity level
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *	vdbg(1, "foobar information: %d", ap);
 */
void
vdbg(int level, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the diagnostic
     */
    fdbg_write(stderr, __func__, level, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fdbg - write a verbosity level allowed debug message, to a stream
 *
 * given:
 *	stream	open stream to use
 *	level	write message if >= verbosity level
 *	fmt	printf format
 *	...
 *
 * Example:
 *
 *	fdbg(stderr, 1, "foobar information: %d", value);
 */
void
fdbg(FILE *stream, int level, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the diagnostic
     */
    fdbg_write(stream, __func__, level, fmt, ap);

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
 * vfdbg - write a verbosity level allowed debug message, to a stream, in va_list form
 *
 * given:
 *	stream	open stream to use
 *	level	write message if >= verbosity level
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *	vfdbg(stream, 1, "foobar information: %d", ap);
 */
void
vfdbg(FILE *stream, int level, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the diagnostic
     */
    fdbg_write(stream, __func__, level, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * sndbg - Copy a verbosity level allowed debug message string into a buffer
 *
 * Copy a debug message string, if the verbosity level is high enough,
 * into a fixed sized buffer.
 *
 * Unlike dbg(), this function does NOT add a final newline to str.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *	level	write message if >= verbosity level
 *	fmt	printf format
 *	...
 *
 * Example:
 *
 *	sndbg(buf, BUFSIZ, DBG_VHIGH, "foobar information: %d", value);
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 * NOTE: This function does nothing (just returns) if a size <= 1.
 */
void
sndbg(char *str, size_t size, int level, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (str == NULL) {
	return;
    }
    if (fmt == NULL) {
	return;
    }

    /*
     * stage 4: write the diagnostic
     */
    sndbg_write(str, size, __func__, level, fmt, ap);

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
 * warn - write a warning message, to stderr
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	warn(__func__, "unexpected foobar: %d", value);
 */
void
warn(char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
	(void) fprintf(stderr, "\nWarning: %s: name is NULL, forcing name to be: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: fmt is NULL, forcing fmt to be: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning
     */
    fwarn_write(stderr, __func__, name, fmt, ap);

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
 * vwarn - write a warning message, to stderr, in va_list form
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *	vwarn(__func__, "unexpected foobar: %d", ap);
 */
void
vwarn(char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: name is NULL, forcing name to be: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: fmt is NULL, forcing fmt to be: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning
     */
    fwarn_write(stderr, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

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
 *	fwarn(strerr, __func__, "unexpected foobar: %d", value);
 */
void
fwarn(FILE *stream, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (stream == NULL) {
	stream = stderr;
	(void) fprintf(stream, "\nWarning: %s: called with NULL stream, will use stderr\n",
			       __func__);
    }
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
 * vfwarn - write a warning message, to a stream, in va_list form
 *
 * given:
 *	stream	open stream to use
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *	vfwarn(stderr, __func__, "unexpected foobar: %d", ap);
 */
void
vfwarn(FILE *stream, char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	(void) fprintf(stream, "\nWarning: %s: called with NULL stream, will use stderr\n",
			       __func__);
    }
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

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snwarn - copy a warning message string into a buffer
 *
 * Copy a warning message string into a fixed sized buffer.
 *
 * Unlike warn(), this function does NOT add a final newline to str.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	snwarn(buf, BUFSIZ, __func__, "whey value: %d", value);
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 * NOTE: This function does nothing (just returns) if a size <= 1.
 */
void
snwarn(char *str, size_t size, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (str == NULL) {
	return;
    }
    if (name == NULL) {
	return;
    }
    if (fmt == NULL) {
	return;
    }

    /*
     * stage 4: write the warning
     */
    snwarn_write(str, size, __func__, name, fmt, ap);

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
 */
void
warnp(char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
	(void) fprintf(stderr, "\nWarning: %s: in vwarnp(): called with NULL name, forcing name: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in vwarnp(): called with NULL fmt, forcing fmt: %s\n",
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
 * vwarnp - write a warning message with errno details, to stderr, in va_list form
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *	vwarnp(__func__, "unexpected foobar: %d", ap);
 */
void
vwarnp(char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in vwarnp(): called with NULL name, forcing name: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: in vwarnp(): called with NULL fmt, forcing fmt: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning with errno details
     */
    fwarnp_write(stderr, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwarnp - write a warning message with errno details, to a stream
 *
 * given:
 *	stream	open stream to use
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	fwarnp(stderr, __func__, "unexpected foobar: %d", value);
 */
void
fwarnp(FILE *stream, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (stream == NULL) {
	stream = stderr;
	(void) fprintf(stream, "\nWarning: %s: called with NULL stream, will use stderr\n",
			       __func__);
    }
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in vwarnp(): called with NULL name, forcing name: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in vwarnp(): called with NULL fmt, forcing fmt: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning with errno details
     */
    fwarnp_write(stream, __func__, name, fmt, ap);

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
 * vfwarnp - write a warning message with errno details, to a stream, in va_list form
 *
 * given:
 *	stream	open stream to use
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * Example:
 *
 *	vfwarnp(stderr, __func__, "unexpected foobar: %d", ap);
 */
void
vfwarnp(FILE *stream, char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	(void) fprintf(stream, "\nWarning: %s: called with NULL stream, will use stderr\n",
			       __func__);
    }
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in vwarnp(): called with NULL name, forcing name: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in vwarnp(): called with NULL fmt, forcing fmt: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning with errno details
     */
    fwarnp_write(stream, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snwarnp - copy a warning message string with errno details into a buffer
 *
 * Copy warning message string that includes errno details into a fixed sized buffer.
 *
 * Unlike warnp(), this function does NOT add a final newline to str.
 *
 * given:
 *	str	pointer to buffer in which to write a message
 *	size	size of buffer, including space for a final NUL byte
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	snwarnp(buf, BUFSIZ, __func__, "unexpected foobar: %d", value);
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 * NOTE: This function does nothing (just returns) if a size <= 1.
 */
void
snwarnp(char *str, size_t size, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (str == NULL) {
	return;
    }
    if (name == NULL) {
	return;
    }
    if (fmt == NULL) {
	return;
    }

    /*
     * stage 4: write the warning
     */
    snwarnp_write(str, size, __func__, name, fmt, ap);

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
 * err - write a fatal error message before exiting, to stderr
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	err(1, __func__, "bad foobar: %s", message);
 *
 * This function does not return.
 */
void
err(int exitcode, char const *name, char const *fmt, ...)
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
	warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stderr, exitcode, __func__, name, fmt, ap);

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
 * verr - write a fatal error message before exiting, to stderr, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	verr(1, __func__, "bad foobar: %s", ap);
 *
 * This function does not return.
 */
void
verr(int exitcode, char const *name, char const *fmt, va_list ap)
{
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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (exitcode < 0) {
	warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stderr, exitcode, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: do not restore errno, just exit
     */
    exit(exitcode);
    not_reached();
}


/*
 * ferr - write a fatal error message before exiting, to a stream
 *
 * given:
 *	exitcode	value to exit with
 *	stream		open stream to use
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	ferr(1, stderr, __func__, "bad foobar: %s", message);
 *
 * This function does not return.
 */
void
ferr(int exitcode, FILE *stream, char const *name, char const *fmt, ...)
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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (exitcode < 0) {
	fwarn(stream, __func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	fwarn(stream, __func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stream, exitcode, __func__, name, fmt, ap);

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
 * vferr - write a fatal error message before exiting, to a stream, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	stream		open stream to use
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vferr(1, stderr, __func__, "bad foobar: %s", ap);
 *
 * This function does not return.
 */
void
vferr(int exitcode, FILE *stream, char const *name, char const *fmt, va_list ap)
{
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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (exitcode < 0) {
	fwarn(stream, __func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	fwarn(stream, __func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stream, exitcode, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: do not restore errno, just exit
     */
    exit(exitcode);
    not_reached();
}


/*
 * errp - write a fatal error message with errno details before exiting, to stderr
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
	warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stderr, exitcode, __func__, name, fmt, ap);

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
 * verrp - write a fatal error message with errno details before exiting, to stderr, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	verrp(1, __func__, "bad foobar: %s", ap);
 *
 * This function does not return.
 */
void
verrp(int exitcode, char const *name, char const *fmt, va_list ap)
{
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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (exitcode < 0) {
	warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stderr, exitcode, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: do not restore errno, just exit
     */
    exit(exitcode);
    not_reached();
}


/*
 * ferrp - write a fatal error message with errno details before exiting, to a stream
 *
 * given:
 *	exitcode	value to exit with
 *	stream		open stream to use
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	ferrp(1, stderr, __func__, "bad foobar: %s", message);
 *
 * This function does not return.
 */
void
ferrp(int exitcode, FILE *stream, char const *name, char const *fmt, ...)
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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (exitcode < 0) {
	fwarn(stream, __func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	fwarn(stream, __func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stream, exitcode, __func__, name, fmt, ap);

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
 * vferrp - write a fatal error message with errno details before exiting, to a stream, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	stream		open stream to use
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vferrp(1, stderr, __func__, "bad foobar: %s", ap);
 *
 * This function does not return.
 */
void
vferrp(int exitcode, FILE *stream, char const *name, char const *fmt, va_list ap)
{
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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (exitcode < 0) {
	fwarn(stream, __func__, "\nexitcode < 0: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    } else if (exitcode > 255) {
	fwarn(stream, __func__, "\nexitcode > 255: %d\n", exitcode);
	exitcode = 255;
	fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stream, exitcode, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: do not restore errno, just exit
     */
    exit(exitcode);
    not_reached();
}


/*
 * werr - write an error message w/o exiting, to stderr
 *
 * given:
 *	error_code	error code
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	werr(1, __func__, "bad foobar: %s", message);
 *
 * This function writes the same message as err() but without
 * bounds checking on error_code and without calling exit().
 */
void
werr(int error_code, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
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
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stderr, error_code, __func__, name, fmt, ap);

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
 * vwerr - write an error message w/o exiting, to stderr, in va_list form
 *
 * given:
 *	error_code	error code
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vwerr(1, __func__, "bad foobar: %s", ap);
 *
 * This function writes the same message as verr() but without
 * bounds checking on error_code and without calling exit().
 */
void
vwerr(int error_code, char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin vwerr(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin vwerr(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stderr, error_code, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwerr - write an error message w/o exiting, to a stream
 *
 * given:
 *	error_code	error code
 *	stream		open stream to use
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	fwerr(1, stderr, __func__, "bad foobar: %s", message);
 *
 * This function writes the same message as err() but without
 * bounds checking on error_code and without calling exit().
 */
void
fwerr(int error_code, FILE *stream, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stream, error_code, __func__, name, fmt, ap);

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
 * vfwerr - write an error message w/o exiting, to a stream, in va_list form
 *
 * given:
 *	error_code	error code
 *	stream		open stream to use
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vfwerr(1, stderr, __func__, "bad foobar: %s", ap);
 *
 * This function writes the same message as verr() but without
 * bounds checking on error_code and without calling exit().
 */
void
vfwerr(int error_code, FILE *stream, char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nin vwerr(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nin vwerr(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * stage 4: write error diagnostic
     */
    ferr_write(stream, error_code, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snwerr - copy an error message string into a buffer
 *
 * Copy an error message string into a fixed sized buffer.
 *
 * Unlike werr(), this function does NOT add a final newline to str.
 *
 * given:
 *	error_code	error code
 *	str		pointer to buffer in which to write a message
 *	size		size of buffer, including space for a final NUL byte
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	snwerr(123, buf, BUFSIZ, __func__, "invalid whey value: %d", value);
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 * NOTE: This function does nothing (just returns) if a size <= 1.
 */
void
snwerr(int error_code, char *str, size_t size, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (str == NULL) {
	return;
    }
    if (name == NULL) {
	return;
    }
    if (fmt == NULL) {
	return;
    }

    /*
     * stage 4: write error diagnostic
     */
    snerr_write(str, size, error_code, __func__, name, fmt, ap);

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
 * werrp - write an error message with errno details w/o exiting, to stderr
 *
 * given:
 *	error_code	error code
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	werrp(1, __func__, "bad foobar: %s", message);
 *
 * This function writes the same message as verrp() but without
 * bounds checking on error_code and without calling exit().
 */
void
werrp(int error_code, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
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
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stderr, error_code, __func__, name, fmt, ap);

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
 * vwerrp - write an error message with errno info w/o exiting, to stderr, in va_list form
 *
 * given:
 *	error_code	error code
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vwerrp(1, __func__, "bad foobar: %s", ap);
 *
 * This function writes the same message as werrp() but without
 * bounds checking on error_code and without calling exit().
 */
void
vwerrp(int error_code, char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stderr, error_code, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwerrp - write an error message with errno details w/o exiting, to a stream
 *
 * given:
 *	error_code	error code
 *	stream		open stream to use
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	fwerrp(1, stderr, __func__, "bad foobar: %s", message);
 *
 * This function writes the same message as verrp() but without
 * bounds checking on error_code and without calling exit().
 */
void
fwerrp(int error_code, FILE *stream, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stream, error_code, __func__, name, fmt, ap);

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
 * vfwerrp - write an error message with errno details w/o exiting, to a stream, in va_list form
 *
 * given:
 *	error_code	error code
 *	stream		open stream to use
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vfwerrp(1, stderr, __func__, "bad foobar: %s", ap);
 *
 * This function writes the same message as werrp() but without
 * bounds checking on error_code and without calling exit().
 */
void
vfwerrp(int error_code, FILE *stream, char const *name, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    ferrp_write(stream, error_code, __func__, name, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * snwerrp - copy an error message string with errno details into a buffer
 *
 * Copy an error message that includes errno details into a fixed string.
 *
 * Unlike werrp(), this function does NOT add a final newline to str.
 *
 * given:
 *	error_code	error code
 *	str		pointer to buffer in which to write a message
 *	size		size of buffer, including space for a final NUL byte
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	snwerrp(123, buf, BUFSIZ, __func__, "unexpected foobar: %d", value);
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 * NOTE: This function does nothing (just returns) if a size <= 1.
 */
void
snwerrp(int error_code, char *str, size_t size, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (str == NULL) {
	return;
    }
    if (name == NULL) {
	return;
    }
    if (fmt == NULL) {
	return;
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    snerrp_write(str, size, error_code, __func__, name, fmt, ap);

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
 * warn_or_err - write a warning or error message before exiting, depending on an arg,
 *		 to stderr
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	warn_or_err(1, __func__, true, "bad foobar: %s", message);
 *
 * NOTE: This function does not return if warning == false.
 */
void
warn_or_err(int exitcode, const char *name, bool warning, const char *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
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
    if (warning == false) {
	if (exitcode < 0) {
	    warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: issue a warning or error message
     */
    if (warning == true) {
	fwarn_write(stderr, __func__, name, fmt, ap);
    } else {
	ferr_write(stderr, exitcode, __func__, name, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * vwarn_or_err - write a warning or error message before exiting, depending on an arg,
 *		  to stderr, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vwarn_or_err(1, __func__, true, "bad foobar: %s", ap);
 *
 * NOTE: This function does not return if warning == false.
 */
void
vwarn_or_err(int exitcode, const char *name, bool warning,
	     const char *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (warning == false) {
	if (exitcode < 0) {
	    warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: issue a warning or error message
     */
    if (warning == true) {
	fwarn_write(stderr, __func__, name, fmt, ap);
    } else {
	ferr_write(stderr, exitcode, __func__, name, fmt, ap);
    }

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * fwarn_or_err - write a warning or error message before exiting, depending on an arg,
 *		  to a stream
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	fwarn_or_err(1, stderr, __func__, true, "bad foobar: %s", message);
 *
 * NOTE: This function does not return if warning == false.
 */
void
fwarn_or_err(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (warning == false) {
	if (exitcode < 0) {
	    fwarn(stream, __func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    fwarn(stream, __func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: issue a warning or error message
     */
    if (warning == true) {
	fwarn_write(stream, __func__, name, fmt, ap);
    } else {
	ferr_write(stream, exitcode, __func__, name, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * vfwarn_or_err - write a warning or error message before exiting, depending on an arg,
 *		   to a stream, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	stream		open stream to use
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vwarn_or_err(1, stderr, __func__, true, "bad foobar: %s", ap);
 *
 * NOTE: This function does not return if warning == false.
 */
void
vfwarn_or_err(int exitcode, FILE *stream, const char *name, bool warning,
	      const char *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (warning == false) {
	if (exitcode < 0) {
	    fwarn(stderr, __func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    fwarn(stderr, __func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stderr, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stderr, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: issue a warning or error message
     */
    if (warning == true) {
	fwarn_write(stream, __func__, name, fmt, ap);
    } else {
	ferr_write(stream, exitcode, __func__, name, fmt, ap);
    }

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * warnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *		   w/errno details, to stderr
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	warnp_or_errp(1, __func__, true, "bad foobar: %s", message);
 *
 * NOTE: This function does not return if warning == false.
 */
void
warnp_or_errp(int exitcode, const char *name, bool warning, const char *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
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
    if (warning == false) {
	if (exitcode < 0) {
	    warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write warning or an error w/errno diagnostic
     */
    if (warning == true) {
	fwarnp_write(stderr, __func__, name, fmt, ap);
    } else {
	ferrp_write(stderr, exitcode, __func__, name, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * vwarnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *		    w/errno details, to stderr, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vwarnp_or_errp(1, __func__, true, "bad foobar: %s", ap);
 *
 * NOTE: This function does not return if warning == false.
 */
void
vwarnp_or_errp(int exitcode, const char *name, bool warning,
	       const char *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    if (warning == false) {
	if (exitcode < 0) {
	    warn(__func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    warn(__func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    warn(__func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write warning or an error w/errno diagnostic
     */
    if (warning == true) {
	fwarnp_write(stderr, __func__, name, fmt, ap);
    } else {
	ferrp_write(stderr, exitcode, __func__, name, fmt, ap);
    }

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * fwarnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *		    w/errno details, to a stream
 *
 * given:
 *	exitcode	value to exit with
 *	stream		open stream to use
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	fwarnp_or_errp(1, stderr, __func__, true, "bad foobar: %s", message);
 *
 * NOTE: This function does not return if warning == false.
 */
void
fwarnp_or_errp(int exitcode, FILE *stream, const char *name, bool warning, const char *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (warning == false) {
	if (exitcode < 0) {
	    fwarn(stream, __func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    fwarn(stream, __func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write warning or an error w/errno diagnostic
     */
    if (warning == true) {
	fwarnp_write(stderr, __func__, name, fmt, ap);
    } else {
	ferrp_write(stream, exitcode, __func__, name, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * vfwarnp_or_errp - write a warning or error message before exiting, depending on an arg,
 *		     w/errno details, to a stream, in va_list form
 *
 * given:
 *	exitcode	value to exit with
 *	stream		open stream to use
 *	name		name of function issuing the warning
 *	warning		true ==> write a warning, false ==> error message before exiting
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * Example:
 *
 *	vwarnp_or_errp(1, __func__, true, "bad foobar: %s", ap);
 *
 * NOTE: This function does not return if warning == false.
 */
void
vfwarnp_or_errp(int exitcode, FILE *stream, const char *name, bool warning,
	        const char *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool w_allowed = false;	/* true ==> warning output is allowed */
    bool e_allowed = false;	/* true ==> err output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, return or exit if not
     */
    if (warning == true) {
	w_allowed = warn_allowed();
        if (w_allowed == false) {
	    return;
	}
    } else {
	e_allowed = err_allowed();
	if (e_allowed == false) {
	    exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	    not_reached();
	}
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (warning == false) {
	if (exitcode < 0) {
	    fwarn(stream, __func__, "\nexitcode < 0: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
	} else if (exitcode > 255) {
	    fwarn(stream, __func__, "\nexitcode > 255: %d\n", exitcode);
	    exitcode = 255;
	    fwarn(stream, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	fwarn(stream, __func__, "\nname is NULL, forcing name to be: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write warning or an error w/errno diagnostic
     */
    if (warning == true) {
	fwarnp_write(stream, __func__, name, fmt, ap);
    } else {
	ferrp_write(stream, exitcode, __func__, name, fmt, ap);
    }

    /* stage 5: stdarg variable argument list cleanup is not required */

    /*
     * stage 6: restore errno if warning, else exit
     */
    if (warning == true) {
	errno = saved_errno;
    } else {
	exit(exitcode);
	not_reached();
    }
    return;
}


/*
 * printf_usage - write command line usage and perhaps exit, to stderr
 *
 * given:
 *	exitcode	- >= 0, exit with this code
 *			  < 0, just return
 *	fmt		- format of the usage message
 *	...		- potential args for usage message
 */
void
printf_usage(int exitcode, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
	fmt = "((NULL fmt))";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write command line usage
     */
    fusage_write(stderr, exitcode, __func__, fmt, ap);

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
 * vprintf_usage - write command line usage and perhaps exit, to stderr, in va_list form
 *
 * given:
 *	exitcode	>= 0, exit with this code
 *			  < 0, just return
 *	fmt		format of the warning
 *	ap		variable argument list
 */
void
vprintf_usage(int exitcode, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (fmt == NULL) {
	fmt = "no usage message given";
	warn(__func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write command line usage
     */
    fusage_write(stderr, exitcode, __func__, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

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
 * fprintf_usage - write command line usage and perhaps exit, to a stream
 *
 * given:
 *	exitcode	- >= 0, exit with this code
 *			  < 0, just return
 *	stream		- stream to write on
 *	fmt		- format of the usage message
 *	...		- potential args for usage message
 */
void
fprintf_usage(int exitcode, FILE *stream, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (fmt == NULL) {
	fmt = "no usage message given";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write command line usage
     */
    fusage_write(stream, exitcode, __func__, fmt, ap);

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
 * vfprintf_usage - write command line usage and perhaps exit, to a stream, in va_list form
 *
 * given:
 *	exitcode	>= 0, exit with this code
 *			  < 0, just return
 *	stream		stream to write on
 *	fmt		format of the warning
 *	ap		variable argument list
 */
void
vfprintf_usage(int exitcode, FILE *stream, char const *fmt, va_list ap)
{
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

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

    /* stage 2: stdarg variable argument list setup is not required */

    /*
     * stage 3: firewall checks
     */
    if (stream == NULL) {
	stream = stderr;
	fwarn(stream, __func__, "called with NULL stream, will use stderr");
    }
    if (fmt == NULL) {
	fmt = "no usage message given";
	fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write command line usage
     */
    fusage_write(stream, exitcode, __func__, fmt, ap);

    /* stage 5: stdarg variable argument list cleanup is not required */

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


#if defined(DBG_TEST)
int
main(int argc, char *argv[])
{
    char *program = NULL;		/* our name */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char const *foo = NULL;		/* where the entry directory and tarball are formed */
    char const *bar = "/usr/bin/tar";	/* path to tar that supports -cjvf */
    char const *baz = NULL;		/* path to the iocccsize tool */
    char buf[BUFSIZ+1];			/* message buffer */
    int forced_errno = 0;		/* -e errno setting */
    int ret;
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vqe:")) != -1) {
	switch (i) {
	case 'h':	/* -h - write help, to stderr and exit 0 */
	    fprintf_usage(0, stderr, usage, program, DBG_VERSION); /*ooo*/
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;			/* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(1, __func__, "cannot parse -v arg: %s", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'e':	/* -e errno - force errno */
	    /* parse errno */
	    errno = 0;			/* pre-clear errno for errp() */
	    forced_errno = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(2, __func__, "cannot parse -v arg: %s", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - write version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", DBG_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error writing version string: %s", DBG_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	default:
	    fprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    fprintf_usage(3, stderr, usage, program, DBG_VERSION); /*ooo*/
	    not_reached();
	}
    }
    /* must have two or three args */
    switch (argc-optind) {
    case 2:
	break;
    case 3:
	bar = argv[optind+2];
	break;
    default:
	fprintf_usage(DO_NOT_EXIT, stderr, "requires two or three arguments");
	/* exit(4); */
	fprintf_usage(4, stderr, usage, program, DBG_VERSION); /*ooo*/
	not_reached();
	break;
    }
    /* collect required args */
    foo = argv[optind];
    dbg(DBG_LOW, "foo: %s", foo);
    baz = argv[optind+1];
    dbg(DBG_LOW, "baz: %s", baz);
    dbg(DBG_LOW, "bar: %s", bar);
    dbg(DBG_LOW, "errno: %d", errno);

    /*
     * report on dbg state, if debugging
     */
    fdbg(stderr, DBG_MED, "verbosity_level: %d", verbosity_level);
    fdbg(stderr, DBG_MED, "msg_output_allowed: %s", booltostr(msg_output_allowed));
    fdbg(stderr, DBG_MED, "dbg_output_allowed: %s", booltostr(dbg_output_allowed));
    fdbg(stderr, DBG_MED, "warn_output_allowed: %s", booltostr(warn_output_allowed));
    fdbg(stderr, DBG_MED, "err_output_allowed: %s", booltostr(err_output_allowed));
    fdbg(stderr, DBG_MED, "usage_output_allowed: %s", booltostr(usage_output_allowed));
    fdbg(stderr, DBG_MED, "msg_warn_silent: %s", booltostr(msg_warn_silent));
    fdbg(stderr, DBG_MED, "msg() output: %s", msg_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "dbg(DBG_MED) output: %s", dbg_allowed(DBG_MED) ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "warn() output: %s", warn_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "err() output: %s", err_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "usage() output: %s", usage_allowed() ? "allowed" : "silenced");

    /*
     * simulate warnings
     */
    warn(program, "simulated call to warn()");

    errno = forced_errno;	/* simulate errno setting */
    snwarn(buf, BUFSIZ, program, "simulated call to snwarn()");
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stderr, "%s\n", buf);
    if (ret <= 0) {
	warnp(__func__, "printf error snwarn() string");
    }

    warnp(program, "simulated call to warnp()");

    errno = forced_errno;	/* simulate errno setting */
    snwarnp(buf, BUFSIZ, program, "simulated call to snwarnp()");
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stderr, "%s\n", buf);
    if (ret <= 0) {
	warnp(__func__, "printf error snwarnp() string");
    }

    errno = forced_errno;	/* simulate errno setting */
    warn_or_err(129, program, true,
		"simulated call to warn_or_err(129, %s, true, ...)", program); /*ooo*/

    errno = forced_errno;	/* simulate errno setting */
    warnp_or_errp(130, program,
		  true, "simulated call to warnp_or_errp(130, %s, true, ...)", program); /*ooo*/

    errno = forced_errno;	/* simulate errno setting */
    fwarn(stderr, program, "simulated call to fwarn()");

    errno = forced_errno;	/* simulate errno setting */
    fwarnp(stderr, program, "simulated call to fwarnp()");

    errno = forced_errno;	/* simulate errno setting */
    fwarn_or_err(131, stderr, program, true,
		"simulated call to fwarn_or_err(131, %s, true, ...)", program); /*ooo*/

    errno = forced_errno;	/* simulate errno setting */
    fwarnp_or_errp(132, stderr, program,
		  true, "simulated call to fwarnp_or_errp(132, %s, true, ...)", program); /*ooo*/

    errno = forced_errno;	/* simulate errno setting */
    snwerr(133, buf, BUFSIZ, program, "simulated call to snwerr(133, %s, ...)", program); /*ooo*/
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stderr, "%s\n", buf);
    if (ret <= 0) {
	warnp(__func__, "printf error snwerr() string");
    }

    errno = forced_errno;	/* simulate errno setting */
    snwerrp(134, buf, BUFSIZ, program, "simulated call to snwerrp(134, %s, ...)", program); /*ooo*/
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stderr, "%s\n", buf);
    if (ret <= 0) {
	warnp(__func__, "printf error snwerrp() string");
    }

    /*
     * simulate an error
     */
    errno = forced_errno;	/* simulate errno setting */
    if (errno != 0) {
	/* exit(5); */
	errp(5, __func__, "simulated error, foo: %s bar: %s", foo, baz); /*ooo*/
    }
    /* exit(6); */
    err(6, __func__, "simulated error, foo: %s bar: %s", foo, baz); /*ooo*/
    not_reached();
}
#endif /* DBG_TEST */
