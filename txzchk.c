/*
 * txzchk: the IOCCC tarball validation checker
 *
 * Invoked by mkiocccentry; txzchk in turn uses fnamchk to make sure that
 * the tarball was correctly named and formed (i.e. the mkiocccentry tool was
 * used).
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions and definitions
 */
#include "util.h"


/*
 * txzchk version
 */
#define TXZCHK_VERSION "0.1 2022-02-05"	/* use format: major.minor YYYY-MM-DD */


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
int issues = 0;				/* issues with tarball found */
static bool quiet = false;		/* true ==> only show errors and warnings */

/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */



/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-t tar] [-f fnamchk] txzpath\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "\t-t /path/to/tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n\n"
    "\t-f fnamchk\t\tpath to tool that checks if filename.txz is a valid compressed tarball\n"
    "\t\t\t\tfilename (def: %s)\n\n"
    "\ttxzpath\t\t\tpath to an IOCCC compressed tarball\n"
    "\n"
    "txzchk version: %s\n";

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str, char const *tar, char const *fnamchk) __attribute__((noreturn));
static void sanity_chk(char const *tar, char const *fnamchk, char const *txzpath);
static int check_tarball(char const *tar, char const *fnamchk, char const *txzpath);

int main(int argc, char **argv)
{
    char *program = NULL;		    /* our name */
    extern char *optarg;		    /* option argument */
    extern int optind;			    /* argv index of the next arg */
    char *tar = TAR_PATH_0;		    /* path to tar executable that supports the -J (xz) option */
    char *txzpath;			    /* txzpath argument to check */
    char *fnamchk = FNAMCHK_PATH_0;   	    /* path to fnamchk tool */
    bool fnamchk_flag_used = true;	    /* if -f option used */
    bool t_flag_used = false;		    /* true ==> -t /path/to/tar was given */
    int ret;				    /* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vqf:t:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(0, "-h help mode", program, TAR_PATH_0, FNAMCHK_PATH_0);
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    errno = 0;		/* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(1, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", TXZCHK_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", TXZCHK_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 'f':
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 't':
	    tar = optarg;
	    t_flag_used = true;
	    break;
	case 'q':
	    quiet = true;
	    break;
	default:
	    usage(1, "invalid -flag", program, TAR_PATH_0, TXZCHK_PATH_0); /*ooo*/
	    not_reached();
	 }
    }
    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(1, "wrong number of arguments", program, TAR_PATH_0, TXZCHK_PATH_0); /*ooo*/
	not_reached();
    }
    txzpath = argv[optind];
    dbg(DBG_LOW, "txzpath: %s", txzpath);

    if (!quiet) {
	/*
	 * Welcome
	 */
	errno = 0;			/* pre-clear errno for errp() */
	ret = printf("Welcome to txzchk version: %s\n", TXZCHK_VERSION);
	if (ret <= 0) {
	    errp(2, __func__, "printf error printing the welcome string");
	    not_reached();
	}
    }

    /*
     * guess where tar is
     *
     * If the user did not give a -t /path/to/tar then look at the historic
     * location for the utility.  If the historic location of the utility isn't
     * executable, look for an executable in the alternate location.
     *
     * On some systems where /usr/bin != /bin, the distribution made the mistake of
     * moving historic critical applications, look to see if the alternate path works instead.
     */
    find_utils(t_flag_used, &tar, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk);


    /*
     * environment sanity checks
     */
    if (!quiet) {
	para("", "Performing sanity checks on your environment ...", NULL);
    }
    sanity_chk(tar, fnamchk, txzpath);
    if (!quiet) {
	para("... environment looks OK", "", NULL);
    }

    /*
     * check the tarball
     */
    if (!quiet) {
	para("", "Performing checks on tarball ...", NULL);
    }
    issues = check_tarball(tar, fnamchk, txzpath);
    if (!quiet) {
	para("All checks passed.", "", NULL);
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
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
 */
static void
usage(int exitcode, char const *str, char const *prog, char const *tar, char const *fnamchk)
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
    if (tar == NULL) {
	tar = "((NULL tar))";
	warn(__func__, "\nin usage: tar was NULL, forcing it to be: %s\n", tar);
    }

    /*
     * print the formatted usage stream
     */
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, tar, fnamchk, TXZCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * sanity_chk - perform basic sanity checks
 *
 * We perform basic sanity checks on paths.
 *
 * given:
 *
 *      tar             - path to tar that supports the -J (xz) option
 *	fnamchk		- path to the fnamchk utility
 *	txzpath		- path to txz tarball to check
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
sanity_chk(char const *tar, char const *fnamchk, char const *txzpath)
{
    /*
     * firewall
     */
    if (tar == NULL || fnamchk == NULL || txzpath == NULL) {
	err(3, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * tar must be executable
     */
    if (!exists(tar)) {
	fpara(stderr,
	      "",
	      "We cannot find a tar program.",
	      "",
	      "A tar program that supports the -J (xz) option is required to build an compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    txzchk -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(4, __func__, "tar does not exist: %s", tar);
	not_reached();
    }
    if (!is_file(tar)) {
	fpara(stderr,
	      "",
	      "The tar, whilst it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    txzchk -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(5, __func__, "tar is not a file: %s", tar);
	not_reached();
    }
    if (!is_exec(tar)) {
	fpara(stderr,
	      "",
	      "The tar, whilst it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the tar program, or use another path:",
	      "",
	      "    txzchk -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(6, __func__, "tar is not an executable program: %s", tar);
	not_reached();
    }

    /*
     * txzpath must be readable
     */
    if (!exists(txzpath)) {
	fpara(stderr,
	      "",
	      "The tarball path specified does not exist. Perhaps you made a typo?",
	      "Please check the path and try again."
	      "",
	      "    txzchk [options] <txzpath>"
	      "",
	      NULL);
	err(7, __func__, "txzpath does not exist: %s", txzpath);
	not_reached();
    }
    if (!is_file(txzpath)) {
	fpara(stderr,
	      "",
	      "The file specified, whilst it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    txzchk [...] <txzpath>",
	      "",
	      NULL);
	err(8, __func__, "txzpath is not a file: %s", txzpath);
	not_reached();
    }
    if (!is_read(txzpath)) {
	fpara(stderr,
	      "",
	      "The tarball path, whilst it is a file, is not readable.",
	      "",
	      "We suggest you check the permissions on the path or use another path:",
	      "",
	      "    txzchk [...] <txzpath>"
	      "",
	      NULL);
	err(9, __func__, "txzpath is not readable: %s", txzpath);
	not_reached();
    }

    return;
}


/*
 * check_tarball - perform tests on tarball, validating it for the IOCCC
 *
 * given:
 *
 *	tar		- path to executable tar program
 *	fnamchk		- path to fnamchk tool
 *	txzpath		- path to tarball to check
 *
 *  Returns the number of issues found.
 */
static int
check_tarball(char const *tar, char const *fnamchk, char const *txzpath)
{
    off_t size = 0; /* file size of tarball */
    unsigned line_num = 0; /* line number of tar output */
    char *cmd = NULL;	/* tar -tJvf */
    FILE *tar_stream = NULL; /* pipe for tar output */
    char *linep = NULL;		/* allocated line read from iocccsize */
    ssize_t readline_len;	/* readline return length */
    int ret;			/* libc function return */
    int exit_code;

    /*
     * firewall
     */
    if (tar == NULL || fnamchk == NULL || txzpath == NULL) {
	err(10, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /* determine size of tarball */
    size = file_size(txzpath);
    /* report size (if too big) */
    if (size < 0) {
	err(11, __func__, "impossible error: sanity_chk found tarball but file_size() did not");
	not_reached();
    }
    else if (size > MAX_TARBALL_LEN) {
	++issues;
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(12, __func__, "The compressed tarball: %s size: %lu > %ld",
		 txzpath, (unsigned long)size, (long)MAX_TARBALL_LEN);
	not_reached();
    }
    else if (!quiet) {
	errno = 0;
	ret = printf("tarball %s size of %d bytes OK\n", txzpath, (int) size);
	if (ret <= 0) {
	    warn(__func__, "unable to tell user how big the tarball is");
	}
    }
    dbg(DBG_MED, "tarball %s size in bytes: %d", txzpath, (int)size);

    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -tJvf %", tar, txzpath);
    if (cmd == NULL) {
	err(13, __func__, "failed to cmdprintf: tar -tJvf txzpath");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(14, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(15, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the tar command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(16, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(17, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(18, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * pre-flush to avoid popen() buffered stdio issues
     */
    dbg(DBG_HIGH, "about to perform: popen(%s, r)", cmd);
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(19, __func__, "fflush(stdout) #0: error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(20, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * form pipe to the tar command
     */
    errno = 0;			/* pre-clear errno for errp() */
    tar_stream = popen(cmd, "r");
    if (tar_stream == NULL) {
	errp(21, __func__, "popen for reading failed for: %s", cmd);
	not_reached();
    }
    setlinebuf(tar_stream);

    /*
     * read the 1st line - should contain the directory name
     */
    dbg(DBG_HIGH, "reading 1st line from popen(%s, r)", cmd);
    readline_len = readline(&linep, tar_stream);
    if (readline_len < 0) {
	err(22, __func__, "EOF while reading 1st line from tar: %s", tar);
	not_reached();
    } else {
	dbg(DBG_HIGH, "tar 1st line read length: %ld buffer: %s", (long)readline_len, linep);
	if (strncmp(linep, "drwxr-xr-x", 10) != 0) {
	    if (*linep == 'd') {
		err(23, __func__, "although the first file in the tarball is a directory, the permissions are incorrect\n");
	    } else {
		err(24, __func__, "first entry in the tarball is not a directory\n");
	    }
	    ++issues;
	}
    }

    ++line_num;

    do {
	readline_len = readline(&linep, tar_stream);
	if (readline_len < 0) {
	    dbg(DBG_VVHIGH, "reached EOF of tarball");
	} else {
	    /* perform various tests */
	    if (*linep != '-') {
		err(25, __func__, "found entry in tarball that's not a regular file\n");
		++issues;
	    }
	}
	++line_num;
    } while (readline_len >= 0);

    /*
     * close down pipe
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret = pclose(tar_stream);
    if (ret < 0) {
	warnp(__func__, "pclose error on tar stream");
    }
    tar_stream = NULL;

    if (issues > 0) {
	fprintf(stderr, "check_tarball() found %u issue%s\n", issues, issues==1?"":"s");
    }

    return issues;
}
