/*
 * mkiocccentry - make an ioccc entry
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Why?  Because this is a C contest.  But then why isn't this
 * code obfuscated?  Because the IOCCC judges prefer to write
 * robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell and finger daemon.
 * Well, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, let the judges know.
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

/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>


/*
 * definitions
 */
#define VERSION "0.2 2021-12-29"	/* use format: major.minor YYYY-MM-DD */
#define REQUIRED_IOCCCSIZE_MAJVER (28)	/* iocccsize major version must match */
#define MIN_IOCCCSIZE_MINVER (2)	/* iocccsize minor version must be >= */
#define DBG_NONE (0)		/* no debugging */
#define DBG_LOW (1)		/* minimal debugging */
#define DBG_MED (3)		/* somewhat more debugging */
#define DBG_HIGH (5)		/* verbose debugging */
#define DBG_VHIGH (7)		/* very verbose debugging */
#define DBG_VVHIGH (9)		/* very very verbose debugging */
#define DBG_DEFAULT (DBG_NONE)	/* default level of debugging */
#define UUID_LEN (36)		/* characters in a UUID string */
#define UUID_VERSION (4)	/* version 4 - random UUID */
#define UUID_VARIANT (0xa)	/* variant 1 - encoded as 0xa */


/*
 * paragraph support
 *
 * FPARA(stream, "string","string2",...) calls fpara(stream, arg_count, "string","string2",...)
 * provided that all argument passed to PARA(...) are strings (char *).
 *
 * PARA("string","string2",...) is a macro for fpara(stdout, arg_count, "string","string2",...)
 * to print a paragraph on standard output.
 */
#define NARG(...) (sizeof((char *[]){__VA_ARGS__})/sizeof(char *))
#define FPARA(stream, ...) fpara(stream, NARG(__VA_ARGS__), __VA_ARGS__)
#define PARA(...) fpara(stdout, NARG(__VA_ARGS__), __VA_ARGS__)
#define MAX_SANE_PARA_LINES (100) /* sanity check limit on paragraph lines */


/*
 * standard truth :-)
 */
#if defined __STDC__ && defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
/* have a C99 compiler - we should expect to have <stdbool.h> */
# include <stdbool.h>
#else
/* do not have a C99 compiler - fake a <stdbool.h> header */
typedef unsigned char bool;
# undef true
# define true ((bool)(1))
# undef false
# define false ((bool)(0))
#endif


/*
 * usage message
 *
 * Use the usage() function to print the usage message.
 */
static char const *usage_msg =
"usage: %s [-h] [-v level] [-V] [-t tar] [-c cp] [-l ls] work_dir iocccsize\n"
"\n"
"\t-h\t\tprint help message and exit 0\n"
"\t-v level\tset verbosity level: (def level: %d)\n"
"\t-V\t\tprint version string and exit\n"
"\t-t tar\t\tpath to tar executable that supports -cjvf (def: %s)\n"
"\t-c cp\t\tpath to cp executable (def: %s)\n"
"\t-l ls\t\tpath to ls executable (def: %s)\n"
"\n"
"\twork_dir\tdirectory where the entry directory and tarball are formed\n"
"\tiocccsize\tpath to the iocccsize tool\n"
"\t\t\tNOTE: Source for the iocccsize tool may be found at:\n"
"\n"
"\t\t\t    https://www.ioccc.org/YYYY/iocccsize.c\n"
"\n"
"\t\t\twhere YYYY is the IOCCC contest year.\n"
"\n"
"mkiocccentry version: %s\n";


/*
 * globals
 */
static char *program = NULL;			/* our name */
static int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
static char *work_dir = NULL;			/* where the entry directory and tarball are formed */
static char *iocccsize = NULL;			/* path to the iocccsize tool */
static char *tar = "/usr/bin/tar";		/* path to tar executable that supports -cjvf */
static char *cp = "/bin/cp";			/* path to cp executable */
static char *ls = "/bin/ls";			/* path to ls executable */


/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str);
static void dbg(int level, char const *fmt, ...);
static void warn(char const *name, char const *fmt, ...);
static void err(int exitcode, char const *name, char const *fmt, ...);
static void errp(int exitcode, char const *name, char const *fmt, ...);
static bool exists(char const *path);
static bool is_file(char const *path);
static bool is_exec(char const *path);
static bool is_dir(char const *path);
static bool is_write(char const *path);
static ssize_t readline(char **linep, FILE *stream);
static char *readline_dup(char **linep, FILE *stream);
static void sanity_chk(char const *work_dir, char const *iocccsize, char const *tar);
static void fpara(FILE *stream, int nargs, ...);
static char *prompt(char *str, char **linep);
static char *get_contest_id(bool *testp);


int
main(int argc, char *argv[])
{
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    bool test_mode = false;		/* true ==> contest ID is test */
    char *ioccc_id = NULL;		/* IOCCC contest ID */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vt:c:l:")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    usage(0, __FUNCTION__, "-h help mode:\n");
	    /*NOTREACHED*/
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;
	    verbosity_level = strtol(optarg, NULL, 0);
	    if (errno != 0) {
		err(1, __FUNCTION__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		/*NOTREACHED*/
	    }
	    break;
	case 'V':	/* -V - print version and exit */
	    printf("%s\n", VERSION);
	    exit(0); /*ooo*/
	    /*NOTREACHED*/
	    break;
	case 't':	/* -t tar */
	    tar = optarg;
	    break;
	case 'c':	/* -c cp */
	    tar = optarg;
	    break;
	case 'l':	/* -l ls */
	    tar = optarg;
	    break;
	default:
	    usage(2, __FUNCTION__, "invalid -flag");
	    /*NOTREACHED*/
	}
    }
    /* must have 2 args */
    switch (argc-optind) {
    case 2:
    	break;
    default:
	usage(3, __FUNCTION__, "requires 2 arguments");
	/*NOTREACHED*/
    	break;
    }
    /* collect required args */
    work_dir = argv[optind];
    dbg(DBG_LOW, "work_dir: %s", work_dir);
    iocccsize = argv[optind+1];
    dbg(DBG_LOW, "iocccsize: %s", iocccsize);
    dbg(DBG_LOW, "tar: %s", tar);
    dbg(DBG_LOW, "cp: %s", cp);
    dbg(DBG_LOW, "ls: %s", ls);

    /*
     * welcome
     */
    printf("Welcome to mkiocccentry version: %s\n", VERSION);

    /*
     * environment sanity checks
     */
    PARA("", "Performing santiy checks on your environment ...");
    sanity_chk(work_dir, iocccsize, tar);
    PARA("... environment looks OK", "");

    /*
     * obtain the IOCCC contest ID
     */
    ioccc_id = get_contest_id(&test_mode);

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    warn(__FUNCTION__, "XXX - code is NOT complete");	/* XXX - remove when code complete */
    exit(0); /*ooo*/
}


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
    if (tar == NULL) {
	tar = "((NULL tar))";
	warn(__FUNCTION__, "\nin usage(): tar was NULL, forcing it to be: %s\n", tar);
    }

    /*
     * print the formatted usage stream
     */
    ret = fprintf(stderr, "%s\n", str);
    if (ret < 0) {
	warn(__FUNCTION__, "\nin usage(): fprintf #0 returned error: %d\n", ret);
    }
    ret = fprintf(stderr, usage_msg, program, DBG_DEFAULT, tar, cp, ls, VERSION);
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
 * 	err(exit_code, __FUNCTION__, "bad foobar: %s", message);
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


/*
 * exists - if a path exists
 *
 * This function tests if a path exists.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists,
 *	false ==> path does not exist
 */
static bool
exists(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(4, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);
    return true;
}


/*
 * is_file - if a path is a file
 *
 * This function tests if a path exists and is a regular file.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and is a regular file,
 *	false ==> path does not exist OR is not a regular file
 */
static bool
is_file(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(5, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if path is a regular file
     */
    if ((buf.st_mode & S_IFMT) != S_IFREG) {
	dbg(DBG_HIGH, "path %s is not a regular file, file type: %07o", path, buf.st_mode & S_IFMT);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a regular file", path);
    return true;
}


/*
 * is_exec - if a path is executable
 *
 * This function tests if a path exists and we have permissions to execute it.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and we have executable access,
 *	false ==> path does not exist OR is not executable OR
 *		  we don't have permission to execute it
 */
static bool
is_exec(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(6, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, X_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not executable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is executable", path);
    return true;
}


/*
 * is_dir - if a path is a directory
 *
 * This function tests if a path exists and is a directory.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and is a directory,
 *	false ==> path does not exist OR is not a directory
 */
static bool
is_dir(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(7, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if path is a regular directory
     */
    if ((buf.st_mode & S_IFMT) != S_IFDIR) {
	dbg(DBG_HIGH, "path %s is not a directory, file type: %07o", path, buf.st_mode & S_IFMT);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a directory", path);
    return true;
}


/*
 * is_write - if a path is writable
 *
 * This function tests if a path exists and we have permissions to write it.
 *
 * given:
 *
 *	path	- the path to test
 *
 * returns:
 *	true ==> path exists and we have write access,
 *	false ==> path does not exist OR is not writable OR
 *		  we don't have permission to write it
 */
static bool
is_write(char const *path)
{
    int ret;		/* return code holder */
    struct stat buf;	/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(8, __FUNCTION__, "exists called with NULL path");
	/*NOTREACHED*/
    }

    /*
     * test for existence of path
     */
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %d", path, ret);
	return false;
    }
    dbg(DBG_VHIGH, "path %s size: %ld", path, buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, W_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not writable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is writable", path);
    return true;
}


/*
 * readline - read a line from a stream
 *
 * Read a line from an open stream.  Malloc or realloc the line
 * buffer as needed.  Remove the trailing newline that was read.
 *
 * given:
 *	linep	- malloced line buffer (may be realloced) or ptr to NULL
 *		  NULL ==> getline() will malloc() the linep buffer
 *		  else ==> getline() might realloc() the linep buffer
 *	stream - file stream to read from
 *
 * returns:
 *	number of characters in the line with newline removed
 *
 * This function does not return on error.
 */
static ssize_t
readline(char **linep, FILE *stream)
{
    size_t linecap;	/* allocated capacity of linep buffer */
    ssize_t ret;	/* getline return and our modified size return */
    char *p;		/* printer to NUL */

    /*
     * firewall
     */
    if (linep == NULL) {
	err(9, __FUNCTION__, "linep is NULL");
	/*NOTREACHED*/
    }
    if (stream == NULL) {
	err(10, __FUNCTION__, "stream is NULL");
	/*NOTREACHED*/
    }

    /*
     * read the line
     */
    clearerr(stream);
    errno = 0;	/* pre-clear errno for errp() */
    ret = getline(linep, &linecap, stream);
    if (ret < 0) {
	if (feof(stream)) {
	    errp(11, __FUNCTION__, "EOF found while reading line");
	    /*NOTREACHED*/
	} else {
	    errp(12, __FUNCTION__, "getline() error");
	    /*NOTREACHED*/
	}
    }
    /* paranoia */
    if (*linep == NULL) {
	err(13, __FUNCTION__, "*linep is NULL after getline()");
	/*NOTREACHED*/
    }

    /*
     * scan for embedded NUL bytes (before end of line)
     *
     * We could use memchr() but
     */
    errno = 0;	/* pre-clear errno for errp() */
    p = memchr(*linep, 0, ret);
    if (p != NULL) {
	errp(14, __FUNCTION__, "found NUL before end of line");
	/*NOTREACHED*/
    }

    /*
     * process trailing newline or lack there of
     */
    if ((*linep)[ret-1] != '\n') {
	err(15, __FUNCTION__, "line does not end in newline");
	/*NOTREACHED*/
    }
    (*linep)[ret-1] = '\0';	/* clear newline */
    --ret;
    dbg(DBG_VVHIGH, "read %d bytes + newline into %d byte buffer", ret, linecap);

    /*
     * return length of line without the trailing newline
     */
    return ret;
}


/*
 * readline_dup - read a line from a stream and duplicate to a malloced buffer.
 *
 * given:
 *	linep	- malloced line buffer (may be realloced) or ptr to NULL
 *		  NULL ==> getline() will malloc() the linep buffer
 *		  else ==> getline() might realloc() the linep buffer
 *	stream - file stream to read from
 *
 * returns:
 *	malloced buffer containing the input without a trailing newline
 *
 * NOTE: This function will NOT return NULL.
 *
 * This function does not return on error.
 */
static char *
readline_dup(char **linep, FILE *stream)
{
    ssize_t len;	/* getline return and our modified size return */
    char *ret;		/* malloced input */

    /*
     * firewall
     */
    if (linep == NULL) {
	err(16, __FUNCTION__, "linep is NULL");
	/*NOTREACHED*/
    }
    if (stream == NULL) {
	err(17, __FUNCTION__, "stream is NULL");
	/*NOTREACHED*/
    }

    /*
     * read the line
     */
    len = readline(linep, stream);
    dbg(DBG_VVHIGH, "readline returned %d", len);

    /*
     * duplicate the line
     */
    errno = 0;	/* pre-clear errno for errp() */
    ret = strdup(*linep);
    if (ret == NULL) {
	err(18, __FUNCTION__, "strdup of read line of %d bytes failed", ret);
	/*NOTREACHED*/
    }

    /*
     * return the malloced string
     */
    return ret;
}


/*
 * sanity_chk - perform basic firewall sanity checks
 *
 * We perform basic sanity checks on paths and the IOCCC contest ID.
 *
 * given:
 *
 *	work_dir - where the entry directory and tarball are formed
 *	iocccsize - path to the iocccsize tool
 *	tar - path to tar that supports -cjvf
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
sanity_chk(char const *work_dir, char const *iocccsize, char const *tar)
{
    FILE *iocccsize_stream;	/* pipe from iocccsize -V */
    char *popen_cmd;		/* iocccsize -V */
    int popen_cmd_len;		/* length of iocccsize buffer */
    char *linep = NULL;		/* allocated line read from iocccsize */
    int exit_code;		/* exit code from system(iocccsize -V) */
    int major_ver;		/* iocccsize major version */
    int minor_ver;		/* iocccsize minor version */
    int year;			/* iocccsize release year */
    int month;			/* iocccsize release month */
    int day;			/* iocccsize release day */
    int ret;			/* libc function return value */

    /*
     * firewall
     */
    if (work_dir == NULL || iocccsize == NULL || tar == NULL) {
	err(19, __FUNCTION__, "called with NULL arg");
	/*NOTREACHED*/
    }

    /*
     * tar must be executable
     */
    if (! exists(tar)) {
	FPARA(stderr,
	      "",
	      "We cannot find a tar program.",
	      "",
	      "A tar program that supports -cjvf is required to build an compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -t tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "");
	err(20, __FUNCTION__, "tar does not exist: %s", tar);
	/*NOTREACHED*/
    }
    if (! is_file(tar)) {
	FPARA(stderr,
	      "",
	      "The tar, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "");
	err(21, __FUNCTION__, "tar is not a file: %s", tar);
	/*NOTREACHED*/
    }
    if (! is_exec(tar)) {
	FPARA(stderr,
	      "",
	      "The tar, while it is a file, is not execurable.",
	      "",
	      "We suggest you check the permissions on the tar program, or use another path:",
	      "",
	      "    mkiocccentry -t tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "");
	err(22, __FUNCTION__, "tar is not executable program: %s", tar);
	/*NOTREACHED*/
    }

    /*
     * cp must be executable
     */
    if (! exists(cp)) {
	FPARA(stderr,
	      "",
	      "We cannot find a cp program.",
	      "",
	      "A cp program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -c cp ...",
	      "",
	      "and/or install a cp program?  You can find the sorce for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "");
	err(23, __FUNCTION__, "cp does not exist: %s", cp);
	/*NOTREACHED*/
    }
    if (! is_file(cp)) {
	FPARA(stderr,
	      "",
	      "The cp, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t cp ...",
	      "",
	      "and/or install a cp program?  You can find the sorce for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/cp/",
	      "");
	err(24, __FUNCTION__, "cp is not a file: %s", cp);
	/*NOTREACHED*/
    }
    if (! is_exec(cp)) {
	FPARA(stderr,
	      "",
	      "The cp, while it is a file, is not execurable.",
	      "",
	      "We suggest you check the permissions on the cp program, or use another path:",
	      "",
	      "    mkiocccentry -t cp ...",
	      "",
	      "and/or install a cp program?  You can find the sorce for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/cp/",
	      "");
	err(25, __FUNCTION__, "cp is not executable program: %s", cp);
	/*NOTREACHED*/
    }

    /*
     * ls must be executable
     */
    if (! exists(ls)) {
	FPARA(stderr,
	      "",
	      "We cannot find a ls program.",
	      "",
	      "A ls program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -c ls ...",
	      "",
	      "and/or install a ls program?  You can find the sorce for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "");
	err(26, __FUNCTION__, "ls does not exist: %s", ls);
	/*NOTREACHED*/
    }
    if (! is_file(ls)) {
	FPARA(stderr,
	      "",
	      "The ls, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t ls ...",
	      "",
	      "and/or install a ls program?  You can find the sorce for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/ls/",
	      "");
	err(27, __FUNCTION__, "ls is not a file: %s", ls);
	/*NOTREACHED*/
    }
    if (! is_exec(ls)) {
	FPARA(stderr,
	      "",
	      "The ls, while it is a file, is not execurable.",
	      "",
	      "We suggest you check the permissions on the ls program, or use another path:",
	      "",
	      "    mkiocccentry -t ls ...",
	      "",
	      "and/or install a ls program?  You can find the sorce for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/ls/",
	      "");
	err(28, __FUNCTION__, "ls is not executable program: %s", ls);
	/*NOTREACHED*/
    }

    /*
     * iocccsize (iocccsize) must be executable
     */
    if (! exists(iocccsize)) {
	FPARA(stderr,
	      "",
	      "The iocccsize file does not exist.",
	      "",
	      "Perhaps you need to supply a different path?",
	      "");
	err(29, __FUNCTION__, "iocccsize does not exist: %s", iocccsize);
	/*NOTREACHED*/
    }
    if (! is_file(iocccsize)) {
	FPARA(stderr,
	      "",
	      "The iocccsize file, while it exits, is not a file.",
	      "",
	      "We suggest you check the permissions on the iocccsize.",
	      "");
	err(30, __FUNCTION__, "iocccsize is not a file: %s", iocccsize);
	/*NOTREACHED*/
    }
    if (! is_exec(iocccsize)) {
	FPARA(stderr,
	      "",
	      "The iocccsize file, while it is a file, is not execurable.",
	      "",
	      "We suggest you check the permissions on the iocccsize.",
	      "");
	err(31, __FUNCTION__, "iocccsize is not executable program: %s", iocccsize);
	/*NOTREACHED*/
    }

    /*
     * work_dir must be a writable direcrtory
     */
    if (! exists(work_dir)) {
	FPARA(stderr,
	      "",
	      "The work_dir does not exist.",
	      "",
	      "You should either create work_dir, or use a different work_dir directory path on the command line.",
	      "");
	err(32, __FUNCTION__, "work_dir does not exist: %s", work_dir);
	/*NOTREACHED*/
    }
    if (! is_dir(work_dir)) {
	FPARA(stderr,
	      "",
	      "While work_dir exists, it is not a directory.",
	      "",
	      "You should move or remove work_dir and them make a new work_dir directory, or use a different",
	      "work_dir directory path on the command line.",
	      "");
	err(33, __FUNCTION__, "work_dir is not a directory: %s", work_dir);
	/*NOTREACHED*/
    }
    if (! is_write(work_dir)) {
	FPARA(stderr,
	      "",
	      "While the diectory work_dir exists, it is not a writable directory.",
	      "",
	      "You should change the permission to make work_dir writable, or you move or remove work_dir and then",
	      "create a new writable directory, or use a different work_dir directory path on the command line.",
	      "");
	err(34, __FUNCTION__, "work_dir is not a writable directory: %s", work_dir);
	/*NOTREACHED*/
    }

    /*
     * be sure iocccsize version is OK
     *
     * The tool:
     *
     *    iocccsize -V
     *
     * will print the version string and exit.  The version string is of the form:
     *
     *    major.minor YYYY-MM-DD
     *
     * For this code to accept iocccsize, the major version must match REQUIRED_IOCCCSIZE_MAJVER
     * AND the minor version must be >= MIN_IOCCCSIZE_MINVER.
     */
    popen_cmd_len = strlen(iocccsize) + sizeof(" -V >/dev/null 2>&1") + 1;
    errno = 0;	/* pre-clear errno for errp() */
    popen_cmd = malloc(popen_cmd_len + 1);
    if (popen_cmd == NULL) {
	errp(35, __FUNCTION__, "malloc #0 failed");
	/*NOTREACHED*/
    }
    errno = 0;	/* pre-clear errno for errp() */
    ret = snprintf(popen_cmd, popen_cmd_len, "%s -V >/dev/null 2>&1", iocccsize);
    if (ret < 0) {
	errp(36, __FUNCTION__, "snprintf error: %d", ret);
	/*NOTREACHED*/
    }
    /* try running iocccsize -V to see if we can execute it */
    dbg(DBG_MED, "testing if %s supports -V", iocccsize);
    errno = 0;	/* pre-clear errno for errp() */
    exit_code = system(popen_cmd);
    if (exit_code < 0) {
	errp(37, __FUNCTION__, "error calling system(\"%s\")", popen_cmd);
	/*NOTREACHED*/
    } else if (exit_code == 127) {
	errp(38, __FUNCTION__, "execution of the shell failed for system(\"%s\")", popen_cmd);
	/*NOTREACHED*/
    } else if (exit_code == 2) {
	err(39, __FUNCTION__, "%s appears to be too old to support -V", iocccsize);
	/*NOTREACHED*/
    } else if (exit_code != 0) {
	err(40, __FUNCTION__, "%s failed with exit code: %d", popen_cmd, exit_code);
	/*NOTREACHED*/
    }

    /*
     * obtain version string from iocccsize -V
     */
    errno = 0;	/* pre-clear errno for errp() */
    ret = snprintf(popen_cmd, popen_cmd_len, "%s -V", iocccsize);
    if (ret < 0) {
	errp(41, __FUNCTION__, "snprintf error: %d", ret);
	/*NOTREACHED*/
    }
    /* pre-flush to avoid popen() buffered stdio issues */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(42, __FUNCTION__, "fflush(stdout); error code: %d", ret);
	/*NOTREACHED*/
    }
    clearerr(stderr);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(43, __FUNCTION__, "fflush(stderr); error code: %d", ret);
	/*NOTREACHED*/
    }
    dbg(DBG_MED, "reading version string from %s -V", iocccsize);
    errno = 0;	/* pre-clear errno for errp() */
    iocccsize_stream = popen(popen_cmd, "r");
    if (iocccsize_stream == NULL) {
	errp(44, __FUNCTION__, "popen for reading failed for: %s", popen_cmd);
	/*NOTREACHED*/
    }
    /* read the 1st line - should contain the iocccsize version */
    (void) readline(&linep, iocccsize_stream);
    dbg(DBG_HIGH, "version line read: %s", linep);
    /* close down pipe */
    (void) fclose(iocccsize_stream);
    ret = sscanf(linep, "%d.%d %d-%d-%d", &major_ver, &minor_ver, &year, &month, &day);
    if (ret != 5) {
	err(45, __FUNCTION__, "iocccsize -V version string is mal-formed: %s", linep);
	/*NOTREACHED*/
    }
    dbg(DBG_MED, "iocccsize version: %d.%d", major_ver, minor_ver);
    dbg(DBG_HIGH, "iocccsize release year: %d month: %d day: %d", year, month, day);
    if (major_ver != REQUIRED_IOCCCSIZE_MAJVER) {
	err(46, __FUNCTION__, "iocccsize major version: %d != required major version: %d", major_ver, REQUIRED_IOCCCSIZE_MAJVER);
	/*NOTREACHED*/
    }
    if (major_ver != REQUIRED_IOCCCSIZE_MAJVER) {
	err(47, __FUNCTION__, "iocccsize major version: %d != required major version: %d", major_ver, REQUIRED_IOCCCSIZE_MAJVER);
	/*NOTREACHED*/
    }
    if (minor_ver < MIN_IOCCCSIZE_MINVER) {
	err(48, __FUNCTION__, "iocccsize minor version: %d < minimum minor version: %d", minor_ver, MIN_IOCCCSIZE_MINVER);
	/*NOTREACHED*/
    }
    dbg(DBG_LOW, "good iocccsize version: %s", linep);
    return;
}


/*
 * fpara - print a paragraph of lines to an open stream
 *
 * Print a collection of strings with newlines added after each string.
 * This function is intended to be invoked via the PARA() macro
 * in order to pass as the 1st argument, the number of strings passed.
 *
 * Example:
 *	FPARA(stderr, "line 1", "line 2", "", "prev line 3 was an empty line");
 *	PARA("line 1", "line 2", "", "prev line 3 was an empty line");
 *
 * The PARA(...) macro is the same as FPARA(stdout, ...).
 *
 * given:
 *	stream	open file stream to print a paragraph onto
 *	nargd	number of lines in the paragraph
 *	...	strings as paragaph lines without trailing newlines
 *
 * This function does not return on error.
 */
static void
fpara(FILE *stream, int nargs, ...)
{
    va_list ap;	/* stdarg block */
    int ret;	/* libc function return value */
    int fd;	/* stream as a file descriptor or -1 */
    int i;

    /*
     * stdarg setup
     */
    va_start(ap, nargs);

    /*
     * stream sanity check
     */
    if (stream == NULL) {
	err(49, __FUNCTION__, "stream is NULL");
	/*NOTREACHED*/
    }
    clearerr(stream);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    /* this may not always catch a bogus or un-opened stream, but try anyway */
    fd = fileno(stream);
    if (fd < 0) {
	errp(50, __FUNCTION__, "fileno on stream returned: %d < 0", fd);
	/*NOTREACHED*/
    }
    clearerr(stream);	/* paranoia */

    /*
     * paragraph count sanity check
     */
    dbg(DBG_VVHIGH, "request to print a %d line paragraph", nargs);
    if (nargs < 0) {
	err(51, __FUNCTION__, "para called with a negative number of lines: %d < 0", nargs);
	/*NOTREACHED*/
    } else if (nargs == 0) {
	/* nothing to do */
	va_end(ap);
	return;
    } else if (nargs > MAX_SANE_PARA_LINES) {
	err(52, __FUNCTION__, "para called with an absurd number of lines: %d < %d", nargs, MAX_SANE_PARA_LINES);
	/*NOTREACHED*/
    }

    /*
     * print paragraph strings followed by newlines
     */
    for (i=0; i < nargs; ++i) {

	/*
	 * print the string
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;	/* pre-clear errno for errp() */
	ret = fputs(va_arg(ap, char *), stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(53, __FUNCTION__, "error writing paragraph to a stream");
		/*NOTREACHED*/
	    } else if (feof(stream)) {
		errp(54, __FUNCTION__, "EOF while writing paragraph to a stream");
		/*NOTREACHED*/
	    } else {
		errp(55, __FUNCTION__, "unexpected fputs error writing paragraph to a stream");
		/*NOTREACHED*/
	    }
	}

	/*
	 * print the newline
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;	/* pre-clear errno for errp() */
    	ret = fputc('\n', stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(56, __FUNCTION__, "error writing newline to a stream");
		/*NOTREACHED*/
	    } else if (feof(stream)) {
		errp(57, __FUNCTION__, "EOF while writing newline to a stream");
		/*NOTREACHED*/
	    } else {
		errp(58, __FUNCTION__, "unexpected fputc error newline a stream");
		/*NOTREACHED*/
	    }
	}
    }

    /*
     * stdarg cleanup
     */
    va_end(ap);

    /*
     * flush the paragraph onto the stream
     */
    clearerr(stream);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stream);
    if (ret == EOF) {
	if (ferror(stream)) {
	    errp(59, __FUNCTION__, "error flushing stream");
	    /*NOTREACHED*/
	} else if (feof(stream)) {
	    errp(60, __FUNCTION__, "EOF while flushing stream");
	    /*NOTREACHED*/
	} else {
	    errp(61, __FUNCTION__, "unexpected fflush error while flushing stream");
	    /*NOTREACHED*/
	}
    }
    return;
}


/*
 * prompt - prompt for a string
 *
 * Prompt a string, followed by :<space> on stdout and then read a line from stdin.
 *
 * given:
 *	str	- string to string followed by :<space>
 *	linep	- malloced line buffer (may be realloced) or ptr to NULL
 *		  NULL ==> getline() will malloc() the linep buffer
 *		  else ==> getline() might realloc() the linep buffer
 *
 *
 * returns:
 *	malloced input string
 *
 * NOTE: This function will NOT return NULL.
 *
 * This function does not return on error.
 */
static char *
prompt(char *str, char **linep)
{
    int ret;		/* libc function return value */
    char *buf;		/* malloced input string */

    /*
     * firewall
     */
    if (str == NULL) {
	err(62, __FUNCTION__, "str is NULL");
	/*NOTREACHED*/
    }
    if (linep == NULL) {
	err(63, __FUNCTION__, "linep is NULL");
	/*NOTREACHED*/
    }

    /*
     * prompt + :<space>
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fputs(str, stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(64, __FUNCTION__, "error printing prompt string");
	    /*NOTREACHED*/
	} else if (feof(stdout)) {
	    errp(65, __FUNCTION__, "EOF while printing prompt string");
	    /*NOTREACHED*/
	} else {
	    errp(66, __FUNCTION__, "unexpected fputs error printing prompt string");
	    /*NOTREACHED*/
	}
    }
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fputs(": ", stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(67, __FUNCTION__, "error printing :<space>");
	    /*NOTREACHED*/
	} else if (feof(stdout)) {
	    errp(68, __FUNCTION__, "EOF while writing :<space>");
	    /*NOTREACHED*/
	} else {
	    errp(69, __FUNCTION__, "unexpected fputs error printing :<space>");
	    /*NOTREACHED*/
	}
    }
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;	/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(70, __FUNCTION__, "error flushing prompt to stdout");
	    /*NOTREACHED*/
	} else if (feof(stdout)) {
	    errp(71, __FUNCTION__, "EOF while flushing prompt to stdout");
	    /*NOTREACHED*/
	} else {
	    errp(72, __FUNCTION__, "unexpected fflush error while flushing prompt to stdout");
	    /*NOTREACHED*/
	}
    }

    /*
     * read user input - return input length
     */
    buf = readline_dup(linep, stdin);
    if (buf == NULL) {
	errp(73, __FUNCTION__, "readline_dup returned NULL");
	/*NOTREACHED*/
    }

    /*
     * return malloced input buffer
     */
    return buf;
}



/*
 * get content ID or test
 *
 * This function will prompt the user for a contest ID, validate it and return it
 * as a malloced string.  If the contest ID is the special value "test", then
 * *testp will be set to true, otherwise it will be set to false.
 *
 * given:
 *	testp - pointer to boolean for test mode
 *
 * returns:
 *	malloced contest ID string
 *	*testp == true ==> contest ID is "test", else contest ID is a UUID.
 *
 * This function does not return on error or if the contest ID is malformed.
 */
static char *
get_contest_id(bool *testp)
{
    char *linep = NULL;		/* allocated line read from iocccsize */
    char *malloc_ret;		/* mallocted return string */
    size_t len;			/* input string length */
    int ret;			/* libc fuction return */
    int a, b, c, d, e, f;	/* parts of the UUID string */
    int version = 0;		/* UUID version hex character */
    int variant = 0;		/* UUID variant hex character */
    int i;

    /*
     * firewall
     */
    if (testp == NULL) {
	err(74, __FUNCTION__, "testp is NULL");
	/*NOTREACHED*/
    }

    /*
     * explain contest ID
     */
    PARA("To submit entries to the IOCCC, you must a registered contestant and have received a",
    	 "IOCCC contest ID (via email) shortly after you have been successfully registered.",
	 "If the IOCCC os open, you may register as a contestant. See:",
	 "",
	 "    file:///Users/chongo/bench/ioccc/ioccc-src/winner/index.html#enter",
	 "",
	 "If you do not have an IOCCC contest ID and you with to test this program,"
	 "you may use the special contest ID:"
	 "",
	 "    test",
	 "",
	 "Note you will not be able to submit the resulting compressed tarball when using test.",
	 "");

    /*
     * prompt for the contest ID
     */
    malloc_ret = prompt("Enter IOCCC contest ID or test", &linep);
    dbg(DBG_HIGH, "the IOCCC contest ID as entered is: %s", malloc_ret);

    /*
     * case: IOCCC contest ID is test, quick return
     */
    if (strcmp(malloc_ret, "test") == 0) {
	PARA("",
	     "IOCCC contst ID is test, entering test mode.",
	     "");
	*testp = true;
    	return malloc_ret;
    }

    /*
     * validate format of non-test contest ID
     *
     * The contest ID, if not "test" must be a UUID.  The UUID has the 36 character format:
     *
     *		xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx
     *
     * where 'x' is a hex character.  The 4 is the UUID version and a the variant 1.
     */
    len = strlen(malloc_ret);
    if (len != UUID_LEN) {
	err(75, __FUNCTION__, "IOCCC contest ID are %d characters in length, you entered %d", UUID_LEN, len);
	/*NOTREACHED*/
    }
    /* convert to lower case */
    for (i=0; i < len; ++i) {
	malloc_ret[i] = tolower(malloc_ret[i]);
    }
    dbg(DBG_VHIGH, "converted the IOCCC contest ID to: %s", malloc_ret);
    ret = sscanf(malloc_ret, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x", &a, &b, &version, &c, &variant, &d, &e, &f);
    dbg(DBG_HIGH, "UUID version hex char: %1x", version);
    dbg(DBG_HIGH, "UUID variant hex char: %1x", variant);
    if (ret != 8) {
	FPARA(stderr,
	      "",
	      "IOCCC contest IDs are version 4, variant 1 UUID as defined by RFC4122:",
	      "",
	      "    https://datatracker.ietf.org/doc/html/rfc4122#section-4.1.1",
	      "",
	      "Your IOCCC contest ID is not a valid UUID.  Please check your the email you received",
	      "when you registered as an IOCCC contestant for the correct IOCCC contest ID.",
	      "");
	err(76, __FUNCTION__, "malfiored IOCCC contest ID");
	/*NOTREACHED*/
    }
    dbg(DBG_MED, "IOCCC contest ID is a UUID: %s", malloc_ret);

    /*
     * report contest ID format is valid
     */
    PARA("",
	 "The format of the non-test IOCCC contest ID appears to be valid.",
	 "");
    *testp = false;	/* IOCCC contest ID is not test */

    /*
     * return IOCCC contest ID
     */
    return malloc_ret;
}
