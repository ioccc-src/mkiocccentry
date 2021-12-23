/*
 * dbg - example of how to use usage(), dbg(), warn(), err(), errp()
 *
 * We show how to use the usage(), dbg(), warn(), and err() facility by
 * this stub from an early verison of iocccsize.  Compile with -DDBG_TEST
 * to build a standalone test program.
 *
 * Copyright (c) 2021 by Landon Curt Noll.  All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>


/*
 * definitions
 */
#define VERSION "0.1.20211223"
#define DBG_NONE (0)	// no debugging
#define DBG_LOW (1)	// minimal debugging
#define DBG_MED (3)	// somewhat more debugging
#define DBG_HIGH (5)	// verbose debugging
#define DBG_VHIGH (7)	// very verbose debugging
#define DBG_DEFAULT (DBG_NONE)	// default level of debugging


/*
 * usage message
 *
 * Use the usage() function to print the usage message.
 */
static char const *usage_msg =
"usage: %s [-h] [-v level] [-e errno] work_dir iocccsize_path [tar_path]\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-e errno\tsimilate setting of errno to cause errp() to be involed\n"
"\n"
"\twork_dir\tdirectory where the entry directory and tarball are formed\n"
"\tiocccsize_path\tpath to the iocccsize tool\n"
"\t\t\tNOTE: Source for the iocccsize tool may be found at:\n"
"\n"
"\t\t\t    https://www.ioccc.org/YYYY/iocccsize.c\n"
"\n"
"\t\t\twhere YYYY is the IOCCC contest year.\n"
"\ttar_path\tpath to tar tool that supports -cjvf (def: %s)\n"
"\n"
"NOTE: This is just a demo. Arguments are ignored and may be any value\n"
"\n"
"dbg_test version: %s\n";


/*
 * globals
 */
static char *program = NULL;			/* our name */
static int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
static char *work_dir = NULL;			/* where the entry directory and tarball are formed */
static char *tar_path = "/usr/bin/tar";		/* path to tar that supports -cjvf */
static char *iocccsize_path = NULL;		/* path to the iocccsize tool */


/*
 * forward declarctions
 */
static void usage(int exitcode, char const *name, char const *str);
static void dbg(int level, char const *fmt, ...);
static void warn(char const *name, char const *fmt, ...);
static void err(int exitcode, char const *name, char const *fmt, ...);
static void errp(int exitcode, char const *name, char const *fmt, ...);


/*
 * usage - print usage to stderr
 *
 * Example:
 *	usage(3, __FUNCTION__, "missing required argument(s)");
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
usage(int exitcode, char const *name, char const *str)
{
    int ret;		/* return code holder */

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__FUNCTION__, "\nin usage(): program was NULL, forcing it to be: %s\n", program);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__FUNCTION__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (program == NULL) {
	program = "((NULL program))";
	warn(__FUNCTION__, "\nin usage(): program was NULL, forcing it to be: %s\n", program);
    }
    if (tar_path == NULL) {
	tar_path = "((NULL tar_path))";
	warn(__FUNCTION__, "\nin usage(): tar_path was NULL, forcing it to be: %s\n", tar_path);
    }

    /*
     * print the formatted usage stream
     */
    ret = fprintf(stderr, "%s\n", str);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin usage(): fprintf #0 returned error: %d\n", ret);
    }
    ret = fprintf(stderr, usage_msg, program, DBG_DEFAULT, tar_path, VERSION);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin usage(): fprintf #1 returned error: %d\n", ret);
    }

    /*
     * exit
     */
    exit(exitcode);
    /*NOTREACHED*/
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
 * 	dbg(1, "foobar information");
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
dbg(int level, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */

    /* start the var arg setup and fetch our first arg */
    va_start(ap, fmt);

    /* firewall */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__FUNCTION__, "\nin dbg(%d, %s ...): NULL fmt, forcing use of: %d\n", level, fmt);
    }

    /* 
     * print the debug message if allowed by the verbosity level
     */
    if (level <= verbosity_level) {
	ret = fprintf(stderr, "debug[%d]: ", level);
	if (ret < 0) {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): fprintf returned error: %d\n", level, fmt, ret);
	}
	ret = vfprintf(stderr, fmt, ap);
	if (ret < 0) {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): vfprintf returned error: %d\n", level, fmt, ret);
	}
	ret = fputc('\n', stderr);
	if (ret != '\n') {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): fputc returned error: %d\n", level, fmt, ret);
	}
	ret = fflush(stderr);
	if (ret < 0) {
	    warn(__FUNCTION__, "\nin dbg(%d, %s, %s ...): fflush returned error: %d\n", level, fmt, ret);
	}
    }

    /* clean up stdarg stuff */
    va_end(ap);
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
 * 	warn(__FUNCTION__, "unexpected foobar: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
warn(char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */

    /* start the var arg setup and fetch our first arg */
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
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fputc returned error: %d\n", name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in warn(%s, %s ...): fflush returned error: %d\n", name, fmt, ret);
    }

    /* clean up stdarg stuff */
    va_end(ap);
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
 * This function does not return.
 *
 * Example:
 *
 * 	err(1, __FUNCTION__, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
err(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */

    /* start the var arg setup and fetch our first arg */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__FUNCTION__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__FUNCTION__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__FUNCTION__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__FUNCTION__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /* issue the FATAL error */
    ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): vfprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fputc returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fflush returned error: %d\n", exitcode, name, fmt, ret);
    }

    /* clean up stdarg stuff */
    va_end(ap);

    /* terminate */
    exit(exitcode);
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
 * This function does not return.
 *
 * Example:
 *
 * 	errp(1, __FUNCTION__, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
errp(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* return code holder */
    int saved_errno;	/* errno value when called */

    /* save errno in case we need it for strerror() */
    saved_errno = errno;

    /* start the var arg setup and fetch our first arg */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__FUNCTION__, "\nin err(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__FUNCTION__, "\nin err(): forcing exit code: %d\n", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__FUNCTION__, "\nin err(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__FUNCTION__, "\nin err(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }

    /* issue the FATAL error */
    ret = fprintf(stderr, "FATAL[%d]: %s: ", exitcode, name);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fprintf #0 returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): vfprintf returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fprintf #1  returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fputc returned error: %d\n", exitcode, name, fmt, ret);
    }
    ret = fflush(stderr);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin err(%d, %s, %s ...): fflush returned error: %d\n", exitcode, name, fmt, ret);
    }

    /* clean up stdarg stuff */
    va_end(ap);

    /* terminate */
    exit(exitcode);
}


#if defined(DBG_TEST)
int
main(int argc, char *argv[])
{
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    int forced_errno;			/* -e errno setting */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:e:")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    usage(0, __FUNCTION__, "-h help mode:\n");
	    /* exit(0); */
	    /*NOTREACHED*/
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;
	    verbosity_level = strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(1); */
		err(1, __FUNCTION__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		/*NOTREACHED*/
	    }
	    break;
	case 'e':	/* -e errno - force errno */
	    /* parse verbosity */
	    errno = 0;
	    forced_errno = strtol(optarg, NULL, 0);
	    if (errno != 0) {
		/* exit(1); */
		err(1, __FUNCTION__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		/*NOTREACHED*/
	    }
	    errno = forced_errno;	/* simulate errno setting */
	    break;
	default:
	    usage(2, __FUNCTION__, "invalid -flag");
	    /* exit(2); */
	    /*NOTREACHED*/
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
	/* exit(3) */
	usage(3, __FUNCTION__, "requires 2 or 3 arguments");
	/*NOTREACHED*/
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
	errp(4, __FUNCTION__, "simulated error, work_dir: %s iocccsize_path: %s", work_dir, iocccsize_path);
	/* exit(4); */
    }
    err(5, __FUNCTION__, "simulated error, work_dir: %s iocccsize_path: %s", work_dir, iocccsize_path);
    /* exit(5); */
    /*NOTREACHED*/

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(0);
}
#endif /* DBG_TEST */
