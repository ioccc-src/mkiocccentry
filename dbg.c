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


#if defined(DBG_TEST)
#include <getopt.h>

/*
 * definitions
 */
#define VERSION "1.2 2022-01-19"


/*
 * globals
 */
char *program = NULL;			/* our name */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */


/*
 * usage message
 *
 * The follow usage message came from an early draft of mkiocccentry.
 * This is just an example of usage: there is no mkiocccentry functionality here.
 */
char const *usage =
"usage: %s [-h] [-v level] [-e errno] work_dir iocccsize_path [tar_path]\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\t-e errno\tsimulate setting of errno to cause errp() to be involved\n"
"\n"
"\twork_dir\ta required arg\n"
"\tiocccsize_path\tanother required arg\n"
"\ttar_path\tan optional arg\n"
"\n"
"NOTE: This is just a demo. Arguments are ignored and may be of any value.\n"
"\n"
"Version: %s";
#endif /* DBG_TEST */


#if !defined(DEBUG_LINT)

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
msg(const char *fmt, ...)
{
    va_list ap;			/* argument pointer */
    int ret;			/* libc function return code */
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
	warn(__func__, "NULL fmt given to debug");
	fmt = "((NULL fmt))";
    }

    /*
     * print the message
     */
    ret = vfprintf(stderr, fmt, ap);
    if (ret <= 0) {
	fprintf(stderr, "[%s vfprintf returned error: %d]", __func__, ret);
    }
    fputc('\n', stderr);

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
 * dbg - print debug message if we are verbose enough
 *
 * given:
 * 	level	print message if >= verbosity level
 * 	fmt	printf format
 * 	...
 *
 * Example:
 *
 * 	dbg(1, "foobar information: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
dbg(int level, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
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
     * print the debug message if allowed by the verbosity level
     */
    if (level <= verbosity_level) {
	ret = fprintf(stderr, "debug[%d]: ", level);
	if (ret < 0) {
	    warn(__func__, "\nin dbg(%d, %s ...): fprintf returned error: %d\n", level, fmt, ret);
	}
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    warn(__func__, "\nin dbg(%d, %s ...): vfprintf returned error: %d\n", level, fmt, ret);
	}
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    warn(__func__, "\nin dbg(%d, %s ...): fputc returned error: %d\n", level, fmt, ret);
	}
	ret = fflush(stderr);
	if (ret < 0) {
	    warn(__func__, "\nin dbg(%d, %s ...): fflush returned error: %d\n", level, fmt, ret);
	}
    }

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
 * warn - issue a warning message
 *
 * given:
 * 	name	name of function issuing the warning
 * 	fmt	format of the warning
 * 	...	optional format args
 *
 * Example:
 *
 * 	warn(__func__, "unexpected foobar: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
warn(char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
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
     * NOTE: We cannot use warn because this is the warn function!
     */

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	(void) fprintf(stderr, "\nWarning: in warn(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	(void) fprintf(stderr, "\nWarning: in warn(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /*
     * issue the warning
     */
    ret = fprintf(stderr, "Warning: %s: ", name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fprintf returned error: %d\n", name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): vfprintf returned error: %d\n", name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fputc returned error: %d\n", name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fflush returned error: %d\n", name, fmt, ret);
    }

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
 * warnp - issue a warning message with errno information
 *
 * given:
 * 	name	name of function issuing the warning
 * 	fmt	format of the warning
 * 	...	optional format args
 *
 * Example:
 *
 * 	warnp(__func__, "unexpected foobar: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
warnp(char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
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
     * NOTE: We cannot use warn because this is the warn function!
     */

    /* firewall */
    if (name == NULL) {
	name = "((NULL name))";
	(void) fprintf(stderr, "\nWarning: in warn(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	(void) fprintf(stderr, "\nWarning: in warn(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /* issue the warning */
    ret = fprintf(stderr, "Warning: %s: ", name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fprintf returned error: %d\n", name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): vfprintf returned error: %d\n", name, fmt, ret);
    }
    ret = fprintf(stderr, "errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fprintf with errno returned error: %d\n", name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fflush returned error: %d\n", name, fmt, ret);
    }

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
 * err - issue a fatal error message and exit
 *
 * given:
 * 	exitcode	value to exit with
 * 	name		name of function issuing the warning
 * 	fmt		format of the warning
 * 	...		optional format args
 *
 * Example:
 *
 * 	err(1, __func__, "bad foobar: %s", message);
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
    int ret;		/* libc function return code */

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
     * issue the FATAL error
     */
    ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
    if (ret < 0) {
	warn(__func__, "\nin err(%d, %s, %s ...): fprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	warn(__func__, "\nin err(%d, %s, %s ...): vfprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	warn(__func__, "\nin err(%d, %s, %s ...): fputc returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	warn(__func__, "\nin err(%d, %s, %s ...): fflush returned error: %d\n", exitcode, name, fmt, ret);
    }

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
 * errp - issue a fatal error message with errno information and exit
 *
 * given:
 * 	exitcode	value to exit with
 * 	name		name of function issuing the warning
 * 	fmt		format of the warning
 * 	...		optional format args
 *
 * Example:
 *
 * 	errp(1, __func__, "bad foobar: %s", message);
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
    int ret;		/* libc function return code */
    int saved_errno;	/* errno value when called */

    /*
     * save errno in case we need it for strerror()
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /* firewall */
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
     * issue the FATAL error
     */
    ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
    if (ret < 0) {
	warn(__func__, "\nin err(%d, %s, %s ...): fprintf #0 returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	warn(__func__, "\nin err(%d, %s, %s ...): vfprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	warn(__func__, "\nin err(%d, %s, %s ...): fprintf #1  returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	warn(__func__, "\nin err(%d, %s, %s ...): fputc returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	warn(__func__, "\nin err(%d, %s, %s ...): fflush returned error: %d\n", exitcode, name, fmt, ret);
    }

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
 * vfprintf_usage - print command line usage and perhaps exit
 *
 * given:
 * 	exitcode	- >= 0, exit with this code
 *			  < 0, just return
 *	stream		- stream to print on
 * 	fmt		- format of the usage message
 *	...		- potential args for usage message
 */
void
vfprintf_usage(int exitcode, FILE *stream, const char *fmt, ...)
{
    va_list ap;		/* argument pointer */
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
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * issue the usage message
     */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stream, "\nWarning: in vfprintf_usage(%d, stream, %s ...): vfprintf error: %d\n", exitcode, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stream, "\nWarning: in vfprintf_usage(%d, stream, %s ...): fputc error: %d\n", exitcode, fmt, ret);
    }
    ret = fflush(stream);
    if (ret < 0) {
	(void) fprintf(stream, "\nWarning: in vfprintf_usage(%d, stream, %s ...): fflush error: %d\n", exitcode, fmt, ret);
    }

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

#endif				/* DEBUG_LINT */


#if defined(DBG_TEST)
int
main(int argc, char *argv[])
{
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    const char *work_dir = NULL;		/* where the entry directory and tarball are formed */
    const char *tar_path = "/usr/bin/tar";	/* path to tar that supports -cjvf */
    const char *iocccsize_path = NULL;	/* path to the iocccsize tool */
    int forced_errno;			/* -e errno setting */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:e:")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    /* exit(0); */
	    vfprintf_usage(0, stderr, usage, program, VERSION);
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;
	    verbosity_level = strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(1); */
		err(1, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		not_reached();
	    }
	    break;
	case 'e':	/* -e errno - force errno */
	    /* parse verbosity */
	    errno = 0;
	    forced_errno = strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(2); */
		err(2, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		not_reached();
	    }
	    errno = forced_errno;	/* simulate errno setting */
	    break;
	default:
	    vfprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    /* exit(3); */
	    vfprintf_usage(3, stderr, usage, program, VERSION);
	    not_reached();
	}
    }
    /* must have 2 or 3 args */
    switch (argc-optind) {
    case 2:
	break;
    case 3:
	tar_path = argv[optind+2];
	break;
    default:
	vfprintf_usage(DO_NOT_EXIT, stderr, "requires 2 or 3 arguments");
	/* exit(4); */
	vfprintf_usage(4, stderr, usage, program, VERSION);
	not_reached();
	break;
    }
    /* collect required args */
    work_dir = argv[optind];
    dbg(DBG_LOW, "work_dir: %s", work_dir);
    iocccsize_path = argv[optind+1];
    dbg(DBG_LOW, "iocccsize_path: %s", iocccsize_path);
    dbg(DBG_LOW, "tar_path: %s", tar_path);

    /*
     * simulate an error
     */
    if (errno != 0) {
	/* exit(5); */
	errp(5, __func__, "simulated error, work_dir: %s iocccsize_path: %s", work_dir, iocccsize_path);
    }
    /* exit(6); */
    err(6, __func__, "simulated error, work_dir: %s iocccsize_path: %s", work_dir, iocccsize_path);
    not_reached();

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(0);
}
#endif /* DBG_TEST */
