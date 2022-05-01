/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * debug - debug, warning and error reporting facility
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


#include "dbg.h"		/* debug, warning, error and usage macros */


/*
 * set global debugging defaults
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
bool msg_output_allowed = true;		/* false ==> disable output from msg() */
bool dbg_output_allowed = true;		/* false ==> disable output from dbg() */
bool warn_output_allowed = true;	/* false ==> disable output from warn() and warnp() */
bool err_output_allowed = true;		/* false ==> disable output from err() and errp() */
bool usage_output_allowed = true;	/* false ==> disable output from fprintf_usage() */
bool msg_warn_silent = false;		/* true ==> silence msg(), warn(), warnp() if verbosity_level == 0 */


#if defined(DBG_TEST)
#include <getopt.h>

/*
 * definitions
 */
#define VERSION "1.9 2022-05-01"


/*
 * usage message
 *
 * The follow usage message came from an early draft of mkiocccentry.
 * This is just an example of usage: there is no mkiocccentry functionality here.
 */
static char const * const usage =
"usage: %s [-h] [-v level] [-q] [-e errno] foo bar [baz]\n"
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
 * msg - print a generic message
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
    va_list ap;			/* argument pointer */
    int saved_errno;	/* errno at function start */

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (fmt == NULL) {
	warn(__func__, "NULL fmt given to msg()");
	fmt = "((NULL fmt))";
    }

    /*
     * print the message, if allowed
     */
    vmsg(fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * vmsg - print a generic message
 *
 * given:
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *      vmsg("foobar information", ap);
 *      vmsg("foo = %d\n", ap);
 */
void
vmsg(char const *fmt, va_list ap)
{
    int ret;			/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * firewall
     */
    if (fmt == NULL) {
	warn(__func__, "NULL fmt given to vmsg()");
	fmt = "((NULL fmt))";
    }

    /*
     * print the message, if allowed
     */
    if (msg_output_allowed == true && (msg_warn_silent == false || verbosity_level > 0)) {
	ret = vfprintf(stderr, fmt, ap);
	if (ret <= 0) {
	    fprintf(stderr, "[%s vfprintf returned error: %d]", __func__, ret);
	}
	fputc('\n', stderr);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * dbg - print debug message if we are verbose enough
 *
 * given:
 *	level	print message if >= verbosity level
 *	fmt	printf format
 *	...
 *
 * Example:
 *
 *	dbg(1, "foobar information: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
dbg(int level, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int saved_errno;	/* errno at function start */

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin dbg(%d, ...): NULL fmt, forcing use of: %s\n", level, fmt);
    }

    /*
     * print the debug message if allowed and allowed by the verbosity level
     */
    vdbg(level, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * vdbg - print debug message if we are verbose enough
 *
 * given:
 *	level	print message if >= verbosity level
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	vdbg(1, "foobar information: %d", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
vdbg(int level, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * firewall
     */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin vdbg(%d, ...): NULL fmt, forcing use of: %s\n", level, fmt);
    }

    /*
     * print the debug message if allowed and allowed by the verbosity level
     */
    if (dbg_output_allowed) {
	if (level <= verbosity_level) {
	    errno = 0;
	    ret = fprintf(stderr, "debug[%d]: ", level);
	    if (ret < 0) {
		warn(__func__, "\nin vdbg(%d, %s ...): fprintf returned error: %s\n",
			       level, fmt, strerror(errno));
	    }

	    errno = 0;
	    ret = vfprintf(stderr, fmt, ap);
	    if (ret < 0) {
		warn(__func__, "\nin vdbg(%d, %s ...): vfprintf returned error: %s\n",
			       level, fmt, strerror(errno));
	    }

	    errno = 0;
	    ret = fputc('\n', stderr);
	    if (ret != '\n') {
		warn(__func__, "\nin vdbg(%d, %s ...): fputc returned error: %s\n",
			       level, fmt, strerror(errno));
	    }

	    errno = 0;
	    ret = fflush(stderr);
	    if (ret < 0) {
		warn(__func__, "\nin vdbg(%d, %s ...): fflush returned error: %s\n",
			       level, fmt, strerror(errno));
	    }
	}
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * warn - issue a warning message
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	warn(__func__, "unexpected foobar: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
warn(char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int saved_errno;	/* errno at function start */

    if (!warn_output_allowed) {
	/* if warn output not allowed return without doing anything */
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * silence warn() if -q and -v 0
     */
    if (msg_warn_silent == true && verbosity_level <= 0) {
	warn_output_allowed = false;
    }

    /*
     * NOTE: We cannot use warn because this is the warn function!
     */

    /*
     * print the warning, if allowed
     */
    vwarn(name, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * vwarn - issue a warning message using a fmt / va_list pair
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	vwarn(__func__, "unexpected foobar: %d", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
vwarn(char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    if (!warn_output_allowed) {
	/* if warn output not allowed return without doing anything */
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * silence warn() if -q and -v 0
     */
    if (msg_warn_silent == true && verbosity_level <= 0) {
	warn_output_allowed = false;
    }

    /*
     * NOTE: We cannot use warn because this is the warn function!
     */

    /*
     * print the warning, if allowed
     */
    if (warn_output_allowed == true && (msg_warn_silent == false || verbosity_level > 0)) {

	/*
	 * firewall
	 */
	if (name == NULL) {
	    name = "((NULL name))";
	    (void) fprintf(stderr, "\nWarning: in vwarn(): called with NULL name, forcing name: %s\n", name);
	}
	if (fmt == NULL) {
	    fmt = "((NULL fmt))";
	    (void) fprintf(stderr, "\nWarning: in vwarn(): called with NULL fmt, forcing fmt: %s\n", fmt);
	}

	/*
	 * issue the warning, if allowed
	 */
	errno = 0;
	ret = fprintf(stderr, "Warning: %s: ", name);
	if (ret < 0) {
	    (void) fprintf(stderr, "\nWarning: in vwarn(%s, %s, ...): fprintf returned error: %s\n",
				   name, fmt, strerror(errno));
	}

	errno = 0;
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    (void) fprintf(stderr, "\nWarning: in vwarn(%s, %s, ...): vfprintf returned error: %s\n",
				   name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    (void) fprintf(stderr, "\nWarning: in vwarn(%s, %s, ...): fputc returned error: %s\n",
				   name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fflush(stderr);
	if (ret < 0) {
	    (void) fprintf(stderr, "\nWarning: in vwarn(%s, %s, ...): fflush returned error: %s\n",
				   name, fmt, strerror(errno));
	}
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * warnp - issue a warning message with errno information
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
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
warnp(char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int saved_errno;	/* errno at function start */

    if (!warn_output_allowed) {
	/* if warn output is not allowed return without doing anything */
	return;
    }
    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * silence warn() if -q and -v 0
     */
    if (msg_warn_silent == true && verbosity_level <= 0) {
	warn_output_allowed = false;
    }

    /*
     * NOTE: We cannot use warn because this is the warn function!
     */

    /*
     * print the warning, if allowed
     */
    vwarnp(name, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * vwarnp - issue a warning message with errno information using a fmt / va_list pair
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	vwarnp(__func__, "unexpected foobar: %d", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
vwarnp(char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    if (!warn_output_allowed) {
	/* if warn output is not allowed return without doing anything */
	return;
    }
    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * silence warn() if -q and -v 0
     */
    if (msg_warn_silent == true && verbosity_level <= 0) {
	warn_output_allowed = false;
    }

    /*
     * NOTE: We cannot use warn because this is the warn function!
     */

    /*
     * print the warning, if allowed
     */
    if (warn_output_allowed == true && (msg_warn_silent == false || verbosity_level > 0)) {


	/* firewall */
	if (name == NULL) {
	    name = "((NULL name))";
	    (void) fprintf(stderr, "\nWarning: in vwarnp(): called with NULL name, forcing name: %s\n", name);
	}
	if (fmt == NULL) {
	    fmt = "((NULL fmt))";
	    (void) fprintf(stderr, "\nWarning: in vwarnp(): called with NULL fmt, forcing fmt: %s\n", fmt);
	}

	errno = 0;
	ret = fprintf(stderr, "Warning: %s: ", name);
	if (ret < 0) {
	    (void) fprintf(stderr, "\nWarning: in vwarnp(%s, %s, ...): fprintf returned error: %s\n",
				   name, fmt, strerror(errno));
	}

	errno = 0;
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    (void) fprintf(stderr, "\nWarning: in vwarnp(%s, %s, ...): vfprintf returned error: %s\n",
				   name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fprintf(stderr, ": errno[%d]: %s\n", saved_errno, strerror(saved_errno));
	if (ret < 0) {
	    (void) fprintf(stderr, "\nWarning: in vwarnp(%s, %s, ...): fprintf with errno returned error: %s\n",
				   name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fflush(stderr);
	if (ret < 0) {
	    (void) fprintf(stderr, "\nWarning: in vwarnp(%s, %s, ...): fflush returned error: %s\n",
				   name, fmt, strerror(errno));
	}
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * err - issue a fatal error message and exit
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
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
void
err(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the FATAL error, if allowed
     */
    verr(exitcode, name, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * verr - issue a fatal error message and exit
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the error
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	verr(1, __func__, "bad foobar: %s", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
void
verr(int exitcode, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */

    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin verr(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin verr(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin verr(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin verr(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the FATAL error, if allowed
     */
    if (err_output_allowed) {
	errno = 0;
	ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
	if (ret < 0) {
	    warn(__func__, "\nin verr(%d, %s, %s, ...): fprintf returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    warn(__func__, "\nin verr(%d, %s, %s, ...): vfprintf returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    warn(__func__, "\nin verr(%d, %s, %s, ...): fputc returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fflush(stderr);
	if (ret < 0) {
	    warn(__func__, "\nin verr(%d, %s, %s, ...): fflush returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}
    }

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * errp - issue a fatal error message with errno information and exit
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
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
void
errp(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__func__, "\nin errp(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin errp(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin errp(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin errp(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the FATAL error, if allowed
     */
    verrp(exitcode, name, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * verrp - issue a fatal error message with errno information and exit
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	verrp(1, __func__, "bad foobar: %s", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
void
verrp(int exitcode, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno value when called */

    /*
     * save errno in case we need it for strerror()
     */
    saved_errno = errno;

    /* firewall */
    if (exitcode < 0) {
	warn(__func__, "\nin verrp(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin verrp(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin verrp(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin verrp(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the FATAL error, if allowed
     */
    if (err_output_allowed) {
	errno = 0;
	ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
	if (ret < 0) {
	    warn(__func__, "\nin verrp(%d, %s, %s, ...): fprintf #0 returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    warn(__func__, "\nin verrp(%d, %s, %s, ...): vfprintf returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
	if (ret < 0) {
	    warn(__func__, "\nin verrp(%d, %s, %s, ...): fprintf #1  returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    warn(__func__, "\nin verrp(%d, %s, %s, ...): fputc returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fflush(stderr);
	if (ret < 0) {
	    warn(__func__, "\nin verrp(%d, %s, %s, ...): fflush returned error: %s\n",
			   exitcode, name, fmt, strerror(errno));
	}
    }

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * werr - issue an error message as a warning
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
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
werr(int error_code, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (error_code < 0) {
	warn(__func__, "\nin werr(): called with error_code <0: %d\n", error_code);
	error_code = 255;
	warn(__func__, "\nin werr(): forcing error code: %d\n", error_code);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin werr(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin werr(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the error if allowed
     */
    vwerr(error_code, name, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);
}


/*
 * vwerr - issue an error message as a warning
 *
 * given:
 *	error_code	error code
 *	name		name of function issuing the error
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	vwerr(1, __func__, "bad foobar: %s", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
vwerr(int error_code, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */

    /*
     * firewall
     */
    if (error_code < 0) {
	warn(__func__, "\nin vwerr(): called with error_code <0: %d\n", error_code);
	error_code = 255;
	warn(__func__, "\nin vwerr(): forcing error code: %d\n", error_code);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin vwerr(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin vwerr(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the error if allowed
     */
    if (err_output_allowed) {
	errno = 0;
	ret = fprintf(stderr, "ERROR[%d]: %s: ", error_code, name);
	if (ret < 0) {
	    warn(__func__, "\nin vwerr(%d, %s, %s, ...): fprintf returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    warn(__func__, "\nin vwerr(%d, %s, %s, ...): vfprintf returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    warn(__func__, "\nin vwerr(%d, %s, %s, ...): fputc returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fflush(stderr);
	if (ret < 0) {
	    warn(__func__, "\nin vwerr(%d, %s, %s, ...): fflush returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}
    }
    return;
}


/*
 * werrp - issue an error message with errno information
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
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
werrp(int error_code, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /* firewall */
    if (error_code < 0) {
	warn(__func__, "\nin werrp(): called with error_code <0: %d\n", error_code);
	error_code = 255;
	warn(__func__, "\nin werrp(): forcing exit code: %d\n", error_code);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin werrp(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin werrp(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the error, if allowed
     */
    vwerrp(error_code, name, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);
}


/*
 * vwerrp - issue an error message with errno information
 *
 * given:
 *	error_code	error code
 *	name		name of function issuing the warning
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	vwerrp(1, __func__, "bad foobar: %s", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
vwerrp(int error_code, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno value when called */

    /*
     * save errno in case we need it for strerror()
     */
    saved_errno = errno;

    /* firewall */
    if (error_code < 0) {
	warn(__func__, "\nin vwerrp(): called with error_code <0: %d\n", error_code);
	error_code = 255;
	warn(__func__, "\nin vwerrp(): forcing exit code: %d\n", error_code);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin vwerrp(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin vwerrp(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the error, if allowed
     */
    if (err_output_allowed) {
	errno = 0;
	ret = fprintf(stderr, "ERROR[%d]: %s: ", error_code, name);
	if (ret < 0) {
	    warn(__func__, "\nin vwerrp(%d, %s, %s, ...): fprintf #0 returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    warn(__func__, "\nin vwerrp(%d, %s, %s, ...): vfprintf returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
	if (ret < 0) {
	    warn(__func__, "\nin vwerrp(%d, %s, %s, ...): fprintf #1  returned error: %s\n",
			    error_code, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    warn(__func__, "\nin vwerrp(%d, %s, %s, ...): fputc returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}

	errno = 0;
	ret = fflush(stderr);
	if (ret < 0) {
	    warn(__func__, "\nin vwerrp(%d, %s, %s, ...): fflush returned error: %s\n",
			   error_code, name, fmt, strerror(errno));
	}
    }
    return;
}


/*
 * fprintf_usage - print command line usage and perhaps exit
 *
 * given:
 *	exitcode	- >= 0, exit with this code
 *			  < 0, just return
 *	stream		- stream to print on
 *	fmt		- format of the usage message
 *	...		- potential args for usage message
 */
void
fprintf_usage(int exitcode, FILE *stream, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int saved_errno;	/* errno at function start */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL stream, will use stderr");
	stream = stderr;
    }
    if (fmt == NULL) {
	fmt = "no usage message given";
	warn(__func__, "called with NULL fmt, will use: %s", fmt);
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * issue the usage message, if allowed
     */
    vfprintf_usage(exitcode, stream, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * terminate with exit code if exitcode >= 0
     */
    if (exitcode >= 0) {
	exit(exitcode);
	not_reached();
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * vfprintf_usage - print command line usage and perhaps exit
 *
 * given:
 *	exitcode	- >= 0, exit with this code
 *			  < 0, just return
 *	stream		- stream to print on
 *	fmt		format of the warning
 *	ap		va_list
 */
void
vfprintf_usage(int exitcode, FILE *stream, char const *fmt, va_list ap)
{
    int saved_errno;	/* errno at function start */
    int ret;		/* libc function return code */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL stream, will use stderr");
	stream = stderr;
    }
    if (fmt == NULL) {
	fmt = "no usage message given";
	warn(__func__, "called with NULL fmt, will use: %s", fmt);
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * issue the usage message, if allowed
     */
    if (usage_output_allowed == true) {
	ret = vfprintf(stream, fmt, ap);
	if (ret < 0) {
	    (void) fprintf(stream, "\nWarning: in vfprintf_usage(%d, stream, %s ...): vfprintf error: %d\n",
				   exitcode, fmt, ret);
	}
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    (void) fprintf(stream, "\nWarning: in vfprintf_usage(%d, stream, %s ...): fputc error: %d\n",
				   exitcode, fmt, ret);
	}
	ret = fflush(stream);
	if (ret < 0) {
	    (void) fprintf(stream, "\nWarning: in vfprintf_usage(%d, stream, %s ...): fflush error: %d\n",
				   exitcode, fmt, ret);
	}
    }

    /*
     * terminate with exit code if exitcode >= 0
     */
    if (exitcode >= 0) {
	exit(exitcode);
	not_reached();
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * warn_or_err - issue a warning or an error depending on test
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	test		true ==> call warn(), false ==> call err()
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	warn_or_err(1, __func__, true, "bad foobar: %s", message);
 *
 * NOTE: This function does not return if test == false.
 */
void
warn_or_err(int exitcode, const char *name, bool test, const char *fmt, ...)
{
    va_list ap;		/* argument pointer */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue a warning or error message
     */
    vwarn_or_err(exitcode, name, test, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * return for the true case
     */
    return;
}


/*
 * vwarn_or_err - issue a warning or an error depending on test
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	test		true ==> call warn(), false ==> call err()
 *	fmt	format of the warning
 *	ap	va_list
 *
 * Example:
 *
 *	vwarn_or_err(1, __func__, true, "bad foobar: %s", ap);
 *
 * NOTE: This function does not return if test == false.
 */
void
vwarn_or_err(int exitcode, const char *name, bool test, const char *fmt, va_list ap)
{
    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * case: true: call warn()
     */
    if (test == true) {

	/*
	 * issue a warning as the calling function
	 */
	vwarn(name, fmt, ap);

    /*
     * case: false: call err()
     */
    } else {

	/*
	 * issue error message as the calling function and exit
	 */
	verr(exitcode, name, fmt, ap);
	not_reached();
    }

    /*
     * return for the true case
     */
    return;
}


/*
 * warnp_or_errp - issue a warning or an error depending on test
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	test		true ==> call warn(), false ==> call err()
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	warnp_or_errp(1, __func__, true, "bad foobar: %s", message);
 *
 * NOTE: This function does not return if test == false.
 */
void
warnp_or_errp(int exitcode, const char *name, bool test, const char *fmt, ...)
{
    va_list ap;		/* argument pointer */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue a warning or error message
     */
    vwarnp_or_errp(exitcode, name, test, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * return for the true case
     */
    return;
}


/*
 * warnp_or_errp - issue a warning or an error depending on test
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	test		true ==> call warn(), false ==> call err()
 *	fmt		format of the warning
 *	ap		va_list
 *
 * Example:
 *
 *	vwarnp_or_errp(1, __func__, true, "bad foobar: %s", ap);
 *
 * NOTE: This function does not return if test == false.
 */
void
vwarnp_or_errp(int exitcode, const char *name, bool test, const char *fmt, va_list ap)
{
    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * case: true: call warnp()
     */
    if (test == true) {

	/*
	 * issue a warning as the calling function
	 */
	vwarnp(name, fmt, ap);

    /*
     * case: false: call errp()
     */
    } else {

	/*
	 * issue error message as the calling function and exit
	 */
	verrp(exitcode, name, fmt, ap);
	not_reached();
    }

    /*
     * return for the true case
     */
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
    char const *baz = NULL;	/* path to the iocccsize tool */
    int forced_errno;			/* -e errno setting */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qe:")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    /* exit(0); */
	    fprintf_usage(0, stderr, usage, program, VERSION); /*ooo*/
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(1); */
		err(1, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno)); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'e':	/* -e errno - force errno */
	    /* parse errno */
	    errno = 0;
	    forced_errno = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(2); */
		err(2, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno)); /*ooo*/
		not_reached();
	    }
	    errno = forced_errno;	/* simulate errno setting */
	    break;
	default:
	    fprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    /* exit(3); */
	    fprintf_usage(3, stderr, usage, program, VERSION); /*ooo*/
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
	fprintf_usage(4, stderr, usage, program, VERSION); /*ooo*/
	not_reached();
	break;
    }
    /* collect required args */
    foo = argv[optind];
    dbg(DBG_LOW, "foo: %s", foo);
    baz = argv[optind+1];
    dbg(DBG_LOW, "baz: %s", baz);
    dbg(DBG_LOW, "bar: %s", bar);

    /*
     * report on dbg state, if debugging
     */
    dbg(DBG_MED, "verbosity_level: %d", verbosity_level);
    dbg(DBG_MED, "msg_output_allowed: %s", t_or_f(msg_output_allowed));
    dbg(DBG_MED, "dbg_output_allowed: %s", t_or_f(dbg_output_allowed));
    dbg(DBG_MED, "warn_output_allowed: %s", t_or_f(warn_output_allowed));
    dbg(DBG_MED, "err_output_allowed: %s", t_or_f(err_output_allowed));
    dbg(DBG_MED, "usage_output_allowed: %s", t_or_f(usage_output_allowed));
    dbg(DBG_MED, "msg_warn_silent: %s", t_or_f(msg_warn_silent));
    dbg(DBG_MED, "msg() output: %s",
	(msg_output_allowed == true && (msg_warn_silent == false || verbosity_level > 0)) ?
	"allowed" : "silenced");
    dbg(DBG_MED, "warn() output: %s",
	(warn_output_allowed == true && (msg_warn_silent == false || verbosity_level > 0)) ?
	"allowed" : "silenced");

    /*
     * simulate warnings
     */
    warn(program, "simulated call to warn()");
    warnp(program, "simulated call to warnp()");
    warn_or_err(129, program, true, "simulated call to warn_or_err(129, %s, true, ...)", program); /*ooo*/
    warnp_or_errp(130, program, true, "simulated call to warnp_or_errp129, %s, true, ...)", program); /*ooo*/

    /*
     * simulate an error
     */
    if (errno != 0) {
	/* exit(5); */
	errp(5, __func__, "simulated error, foo: %s bar: %s", foo, baz); /*ooo*/
    }
    /* exit(6); */
    err(6, __func__, "simulated error, foo: %s bar: %s", foo, baz); /*ooo*/
    not_reached();
}
#endif /* DBG_TEST */
