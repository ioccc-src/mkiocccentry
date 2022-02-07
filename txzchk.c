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
#include <ctype.h>

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
#define TXZCHK_VERSION "0.4 2022-02-07"	/* use format: major.minor YYYY-MM-DD */


/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
int issues = 0;				/* issues with tarball found */
static bool quiet = false;		/* true ==> only show errors and warnings */

struct info {
    bool has_info_json;
    bool has_author_json;
    bool has_prog_c;
    bool has_remarks_md;
    bool has_Makefile;
    unsigned dot_files;
} info;

struct file {
    char *basename;
    char *filename;
    unsigned count;
    struct file *next;
};

struct file *files;

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
    "usage: %s [-h] [-v level] [-V] [-t tar] [-F fnamchk] txzpath\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "\t-t /path/to/tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-F fnamchk\t\tpath to tool that checks if filename.txz is a valid compressed tarball name\n"
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
static bool has_special_bits(char const *str);
static void add_file_to_list(struct file *file);
static unsigned check_files(void);
static void free_file_list(void);

int main(int argc, char **argv)
{
    char *program = NULL;		    /* our name */
    extern char *optarg;		    /* option argument */
    extern int optind;			    /* argv index of the next arg */
    char *tar = TAR_PATH_0;		    /* path to tar executable that supports the -J (xz) option */
    char *txzpath;			    /* txzpath argument to check */
    char *fnamchk = FNAMCHK_PATH_0;   	    /* path to fnamchk tool */
    bool fnamchk_flag_used = false;	    /* if -F option used */
    bool tar_flag_used = false;		    /* true ==> -t /path/to/tar was given */
    int ret;				    /* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:VqF:t:")) != -1) {
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
	case 'F':
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 't':
	    tar = optarg;
	    tar_flag_used = true;
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
    find_utils(tar_flag_used, &tar, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk);


    /*
     * environment sanity checks
     */
    if (!quiet) {
	para("", "Performing sanity checks on your environment ...", NULL);
    }
    sanity_chk(tar, fnamchk, txzpath);
    if (!quiet) {
	para("... environment looks OK", NULL);
    }

    /*
     * check the tarball
     */
    if (!quiet) {
	para("", "Performing checks on tarball ...", NULL);
    }
    issues = check_tarball(tar, fnamchk, txzpath);
    if (!quiet && !issues) {
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
    if (fnamchk == NULL) {
	fnamchk = "((NULL fnamchk))";
	warn(__func__, "\nin usage(): fnamchk was NULL, forcing it to be: %s\n", fnamchk);
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
     * fnamchk must be executable
     */
    if (!exists(fnamchk)) {
	fpara(stderr,
	      "",
	      "We cannot find fnamchk.",
	      "",
	      "This tool is required to test the tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    txzchk -F /path/to/fnamchk ...",
	      NULL);
	err(7, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }
    if (!is_file(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk, whilst it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    txzchk -F /path/to/fnamchk ...",
	      NULL);
	err(8, __func__, "fnamchk is not a file: %s", tar);
	not_reached();
    }
    if (!is_exec(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk, whilst it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the fnamchk program, or use another path:",
	      "",
	      "    txzchk -F /path/to/fnamchk ...",
	      NULL);
	err(9, __func__, "fnamchk is not an executable program: %s", tar);
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
	err(10, __func__, "txzpath does not exist: %s", txzpath);
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
	err(11, __func__, "txzpath is not a file: %s", txzpath);
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
	err(12, __func__, "txzpath is not readable: %s", txzpath);
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
 *
 *  Does not return on error.
 */
static int
check_tarball(char const *tar, char const *fnamchk, char const *txzpath)
{
    off_t size = 0; /* file size of tarball */
    off_t file_sizes = 0; /* accumulation of file sizes within the tarball */
    off_t current_file_size = 0; /* current file size */
    off_t rounded_file_size = 0; /* file sizes rounded up to 1024 multiple */
    unsigned line_num = 0; /* line number of tar output */
    char *cmd = NULL;	/* fnamchk and tar -tJvf */
    FILE *tar_stream = NULL; /* pipe for tar output */
    char *linep = NULL;		/* allocated line read from iocccsize */
    char *line_dup = NULL;	/* duplicated line from readline */
    ssize_t readline_len;	/* readline return length */
    int dir_count = 0;		/* number of directories detected */
    int ret;			/* libc function return */
    int exit_code;
    int i;
    struct file *file = NULL;

    /*
     * firewall
     */
    if (tar == NULL || fnamchk == NULL || txzpath == NULL) {
	err(13, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /* determine size of tarball */
    size = file_size(txzpath);
    /* report size (if too big) */
    if (size < 0) {
	err(14, __func__, "impossible error: sanity_chk() found tarball but file_size() did not");
	not_reached();
    }
    else if (size > MAX_TARBALL_LEN) {
	++issues;
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(15, __func__, "The compressed tarball: %s size: %lu > %ld",
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
    dbg(DBG_MED, "tarball %s size in bytes: %lu", txzpath, (unsigned long)size);

    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -tJvf %", tar, txzpath);
    if (cmd == NULL) {
	err(16, __func__, "failed to cmdprintf: tar -tJvf txzpath");
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
	errp(17, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(18, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the tar command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(19, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(20, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(21, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
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
	errp(22, __func__, "fflush(stdout) #0: error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(23, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * form pipe to the tar command
     */
    errno = 0;			/* pre-clear errno for errp() */
    tar_stream = popen(cmd, "r");
    if (tar_stream == NULL) {
	errp(24, __func__, "popen for reading failed for: %s", cmd);
	not_reached();
    }
    setlinebuf(tar_stream);

    /*
     * process all tar lines listed
     */
    do {
	char *p = NULL;

	/*
	 * count this line
	 */
	++line_num;

	/*
	 * read the next listing line
	 */
	readline_len = readline(&linep, tar_stream);
        if (readline_len < 0) {
	    dbg(DBG_HIGH, "reached EOF of tarball");
	    break;
	}
	dbg(DBG_VHIGH, "line %d: <%s>", line_num, linep);

	/*
	 * look for more than one directory
	 */
	if (*linep == 'd') {
	    ++dir_count;
	    if (dir_count > 1) {
		warn(__func__, "found more than one directory entry: %s", linep);
		++issues;
	    }

	/*
	 * look for non-directory non-regular non-hard-lined items
	 */
	} else if (*linep != '-') {
	    warn(__func__, "found a non-directory non-regular non-hard-lined item: %s", linep);
	    ++issues;
	}
	line_dup = strdup(linep);
	if (line_dup == NULL) {
	    err(25, __func__, "duplicating %s failed", linep);
	    not_reached();
	}
	/* extract each field, one at a time, to do various tests */
	p = strtok(linep, " \t");
	if (p == NULL) {
	    err(26, __func__, "NULL pointer encountered trying to parse line");
	    not_reached();
	}
	if (has_special_bits(p)) {
	    warn(__func__, "found special bits on line: %s", line_dup);
	    ++issues;
	}
	/* we don't need this next field */
	p = strtok(NULL, " \t");

	if (p == NULL) {
	    err(27, __func__, "NULL pointer encountered trying to parse line");
	    not_reached();
	}
	p = strtok(NULL, " \t");
	if (p == NULL) {
	    err(28, __func__, "NULL pointer encountered trying to parse line");
	    not_reached();
	}
	/*
	 * attempt to find !isdigit() chars (i.e. the tarball listing includes
	 * the owner of the files
	 */
	for (; p && *p && isdigit(*p); )
	    ++p; /* satisfy warnings */

	if (*p) {
	    warn(__func__, "found non-digit UID in file in line: %s", line_dup);
	    ++issues;
	}

	/*
	 * now do the same for group
	 */
	p = strtok(NULL, " \t");
	if (p == NULL) {
	    err(29, __func__, "NULL pointer encountered trying to parse line");
	    not_reached();
	}
	for (; p && *p && isdigit(*p); )
	    ++p; /* satisfy warnings */

	if (*p) {
	    warn(__func__, "found non-digit GID in file in line: %s", line_dup);
	    ++issues;
	}

	p = strtok(NULL, " \t");
	if (p == NULL) {
	    err(30, __func__, "NULL pointer encountered trying to parse line");
	    not_reached();
	}

	errno = 0;
	current_file_size = strtoll(p, NULL, 10);
	if (errno != 0) {
	    err(31, __func__, "trying to parse file size in tarball on line: %s, string: %s", line_dup, p);
	    not_reached();
	}
	file_sizes += current_file_size;

	/* 
	 * the next three fields we don't care about but we loop four times to
	 * get the following field which we do care about.
	 */
	for (i = 0; i < 4; ++i) {
	    p = strtok(NULL, " \t");
	    if (p == NULL) {
		err(32, __func__, "NULL pointer trying to parse line");
		not_reached();
	    }
	}
	/* p should now contain the filename. */
	errno = 0;
	file = calloc(1, sizeof *file);
	if (file == NULL) {
	    err(32, __func__, "unable to allocate a struct file *");
	    not_reached();
	}

	errno = 0;
	file->filename = strdup(p);
	if (!file->filename) {
	    err(33, __func__, "unable to strdup filename %s", p);
	    not_reached();
	}

	errno = 0;
	file->basename = strdup(base_name(p)?base_name(p):"");
	if (!file->basename || !strlen(file->basename)) {
	    err(34, __func__, "unable to strdup basename of filename %s", p);
	    not_reached();
	}

	/* 
	 * although we could check these in check_file() we check here because
	 * the add_file_to_list() function doesn't add the same file (basename)
	 * more than once: it simply increments the times it's been seen.
	 */
	if (current_file_size == 0) {
	    if (!strcmp(file->basename, ".author.json")) {
		++issues;
		warn(__func__, "found empty .author.json file");
	    }
	    else if (!strcmp(file->basename, ".info.json")) {
		++issues;
		warn(__func__, "found empty .info.json file");
	    }
	    else if (!strcmp(file->basename, "remarks.md")) {
		++issues;
		warn(__func__, "found empty remarks.md");
	    }
	    else if (!strcmp(file->basename, "Makefile")) {
		++issues;
		warn(__func__, "found empty Makefile");
	    }
	}
	if (strstr(file->filename, "../")) {
	    ++issues;
	    warn(__func__, "found file with ../ in the path: %s", file->filename);
	} else if (!strncmp(file->filename, "/", 1)) {
	    ++issues;
	    warn(__func__, "found absolute path %s", file->filename);
	}

	add_file_to_list(file);

	free(line_dup);
	line_dup = NULL;

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


    /* check the file list, reporting any issues */
    issues += check_files();

    /* report total file size */
    rounded_file_size = round_to_multiple(file_sizes, 1024);
    if (rounded_file_size > MAX_DIR_KSIZE) {
	warn(__func__, "accumulated size of all files %lld rounded up to multiple of 1024 %lld > %d", file_sizes, rounded_file_size, MAX_DIR_KSIZE);
	++issues;
    } else if (rounded_file_size < 0) {
	err(30, __func__, "accumulated file size < 0!");
	not_reached();
    } else if (!quiet) {
	printf("total size of files %lld rounded up to 1024 multiple: %lld OK\n", file_sizes, rounded_file_size);
    }



    /*
     * report issues found before running fnamchk so that it's easy to see how
     * many problems we found (if the fnamchk fails it errors out so this won't
     * be seen).
     */
    if (issues > 0) {
	fprintf(stderr, "txzchk found %u issue%s\n", issues, issues==1?"":"s");
    }

    /*
     * free cmd for next command
     */
    free(cmd);
    cmd = NULL;

    /* form command line to fnamchk */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% %", fnamchk, txzpath);
    if (cmd == NULL) {
	err(30, __func__, "failed to cmdprintf: fnamchk txzpath");
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
	errp(31, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(32, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the fnamchk command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(33, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(34, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(35, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }


    free(cmd);
    cmd = NULL;

    free_file_list();

    return issues;
}

/*
 * has_special_bits - determine if the string has any special bits
 *
 * given:
 *
 *	str	    - the string to test
 *
 * This function does not return on NULL pointer passed into the function.
 */
static bool
has_special_bits(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	err(36, __func__, "called with NULL arg(s)");
	not_reached();
    }

    return str[strspn(str, "-drwx")]!='\0';
}
/*
 * add_file_to_list - add a filename to the linked list 
 *
 * given:
 *
 *	file		    - pointer to struct file which should already have the name
 *
 * If the function finds this filename already in the list (basename!) it
 * increments the count and does not add it to the list; else it adds it to the
 * list with a count of 1.
 *
 * This function does not return on error.
 */
static void
add_file_to_list(struct file *file)
{
    struct file *ptr; /* used to iterate through list to find duplicate files */

    /*
     * firewall
     */
    if (file == NULL || !file->filename || !file->basename) {
	err(37, __func__, "called with NULL pointer(s)");
	not_reached();
    }

    for (ptr = files; ptr != NULL; ptr = ptr->next)
    {
	if (!strcmp(ptr->basename, file->basename)) {
	    dbg(DBG_MED, "incrementing count of filename %s", file->basename);
	    ptr->count++;
	    return;
	}
    }
    file->count++;
    /* lazily add to list */
    dbg(DBG_MED, "adding filename %s (basename %s) to list of files", file->filename, file->basename);
    file->next = files;
    files = file;
}

/* check_files	- check the files list, reporting issues (that aren't reported elsewhere)
 *
 * Returns the number of issues that _this_ function finds.
 */
static unsigned
check_files(void)
{
    unsigned issues = 0;
    struct file *file = NULL;

    for (file = files; file; file = file->next) {
	if (!strcmp(file->basename, ".info.json")) {
	    info.has_info_json = true;
	} else if (!strcmp(file->basename, ".author.json")) {
	    info.has_author_json = true;
	} else if (!strcmp(file->basename, "Makefile")) {
	    info.has_Makefile = true;
	} else if (!strcmp(file->basename, "prog.c")) {
	    info.has_prog_c = true;
	} else if (!strcmp(file->basename, "remarks.md")) {
	    info.has_remarks_md = true;
	} else if (*(file->basename) == '.' && strcmp(file->basename, ".info.json") && strcmp(file->basename, ".author.json")) {
	    ++issues;
	    warn(__func__, "found non .author.json and .info.json dot file %s", file->basename);
	    info.dot_files++;
	}

	if (file->count > 1) {
	    warn(__func__, "found a total of %u files with the name %s", file->count, file->basename);
	    issues += file->count - 1;
	}
    }

    /* determine if the required files are there */
    if (!info.has_info_json) {
	warn(__func__, "tarball has no .info.json file");
	++issues;
    }
    if (!info.has_author_json) {
	warn(__func__, "tarball has no .author.json file");
	++issues;
    }
    if (!info.has_prog_c) {
	warn(__func__, "tarball has no prog.c file");
	++issues;
    }
    if (!info.has_Makefile) {
	warn(__func__, "tarball has no Makefile");
	++issues;
    }
    if (!info.has_remarks_md) {
	warn(__func__, "tarball has no remarks.md file");
	++issues;
    }


    return issues;
}
/*
 * free_file_list - free the file linked list
 *
 */
static void
free_file_list(void)
{
    struct file *file, *next_file;

    for (file = files; file != NULL; file = next_file) {
	next_file = file->next;
	if (file->filename) {
	    free(file->filename);
	    file->filename = NULL;
	}
	if (file->basename) {
	    free(file->basename);
	    file->basename = NULL;
	}

	free(file);
	file = NULL;
    }
}
