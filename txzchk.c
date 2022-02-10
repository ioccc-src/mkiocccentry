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
#define TXZCHK_VERSION "0.41 2022-02-08"    /* use format: major.minor YYYY-MM-DD */


/*
 * globals
 */
char const *program = NULL;		    /* our name */
int verbosity_level = DBG_DEFAULT;	    /* debug level set by -v */
static int total_issues = 0;		    /* total number of issues with tarball found */
static bool quiet = false;		    /* true ==> only show errors and warnings */
static bool text_file_flag_used = false;    /* true ==> assume txzpath is a text file */
char const *txzpath = NULL;		    /* the current tarball being checked */

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

struct line {
    char *line;
    int line_num;
    struct line *next;
};

struct line *lines;

/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */



/*
 * usage message
 *
 * Use the usage() function to print the usage_msgX strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-t tar] [-F fnamchk] [-T] txzpath\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "\t-t /path/to/tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-F /path/to/fnamchk\tpath to tool that checks if txzpath is a valid compressed tarball name\n"
    "\t\t\t\tfilename (def: %s)\n\n"
    "\t-T\t\t\tassume txzpath is a text file with tar listing (for testing different formats)\n\n"
    "\ttxzpath\t\t\tpath to an IOCCC compressed tarball\n"
    "\n"
    "txzchk version: %s\n";

/*
 * forward declarations
 */
static void usage(int exitcode, char const *name, char const *str, char const *tar, char const *fnamchk) __attribute__((noreturn));
static void sanity_chk(char const *tar, char const *fnamchk);
static void parse_line(char *linep, char *line_dup, char const *dir_name, char const *txzpath, off_t *file_sizes, int *dir_count);
static void parse_linux_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, off_t *file_sizes);
static void parse_bsd_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, off_t *file_sizes);
static unsigned check_tarball(char const *tar, char const *fnamchk);
static void check_empty_file(char const *txzpath, off_t file_size, struct file *file);
static void check_file(char const *txzpath, char *p, char const *dir_name, struct file *file);
static void check_all_files(off_t file_sizes);
static bool has_special_bits(char const *str);
static void add_line(char const *str, int line_num);
static void parse_all_lines(char const *dir_name, char const *txzpath, off_t *file_sizes);
static void free_lines(void);
static struct file *alloc_file(char const *p);
static void add_file_to_list(struct file *file);
static void free_file_list(void);

int main(int argc, char **argv)
{
    extern char *optarg;		    /* option argument */
    extern int optind;			    /* argv index of the next arg */
    char *tar = TAR_PATH_0;		    /* path to tar executable that supports the -J (xz) option */
    char *fnamchk = FNAMCHK_PATH_0;   	    /* path to fnamchk tool */
    bool fnamchk_flag_used = false;	    /* if -F option used */
    bool tar_flag_used = false;		    /* true ==> -t /path/to/tar was given */
    int ret;				    /* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:VqF:t:T")) != -1) {
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
	case 'T':
	    text_file_flag_used = true; /* don't rely on tar: just read file as if it was a text file */
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
     *
     * If -T was used we don't actually need tar(1) so we test for that
     * specifically.
     */

    if (!text_file_flag_used)
	find_utils(tar_flag_used, &tar, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk);
    else
	find_utils(false, NULL, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk);


    /*
     * environment sanity checks
     */
    if (!quiet) {
	para("", "Performing sanity checks on your environment ...", NULL);
    }
    sanity_chk(tar, fnamchk);
    if (!quiet) {
	para("... environment looks OK", NULL);
    }

    /*
     * check the tarball
     */
    if (!quiet) {
	para("", "Performing checks on tarball ...", NULL);
    }
    total_issues = check_tarball(tar, fnamchk);
    if (!quiet && !total_issues) {
	para("All checks passed.", "", NULL);
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(total_issues != 0); /*ooo*/
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
	warn("txzchk", "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn("txzchk", "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (tar == NULL) {
	tar = "((NULL tar))";
	warn("txzchk", "\nin usage: tar was NULL, forcing it to be: %s\n", tar);
    }
    if (fnamchk == NULL) {
	fnamchk = "((NULL fnamchk))";
	warn("txzchk", "\nin usage(): fnamchk was NULL, forcing it to be: %s\n", fnamchk);
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
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
sanity_chk(char const *tar, char const *fnamchk)
{
    /*
     * firewall
     */
    if ((tar == NULL && !text_file_flag_used) || fnamchk == NULL || txzpath == NULL) {
	err(3, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * if text file flag not used tar must be executable
     */
    if (!text_file_flag_used) {
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
	err(8, __func__, "fnamchk is not a file: %s", fnamchk);
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
	err(9, __func__, "fnamchk is not an executable program: %s", fnamchk);
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

/* check_file		- checks on the current file only
 *
 * given:
 *
 *	txzpath		- the tarball (or text file) we're processing
 *	p		- the path
 *	dir_name	- the directory name (if fnamchk passed - else NULL)
 *	file		- file structure
 *
 * Report issues on the current file's path.
 *
 * Returns void. Does not return on error.
 *
 */
static void
check_file(char const *txzpath, char *p, char const *dir_name, struct file *file)
{
    /*
     * firewall
     */
    if (txzpath == NULL || p == NULL || file == NULL || file->basename == NULL || file->filename == NULL) {
	err(35, __func__, "passed NULL arg(s)");
	not_reached();
    }

    if (*(file->basename) == '.' && strcmp(file->basename, ".info.json") && strcmp(file->basename, ".author.json")) {
	++total_issues;
	warn("txzchk", "%s: found non .author.json and .info.json dot file %s", txzpath, file->basename);
	info.dot_files++;
    }

    if (strstr(file->filename, "../")) {
	/*
	 * note that this check does NOT detect a file in the form of
	 * "../.file" but since the basename of each file is checked above
	 * this is okay.
	 */
	++total_issues;
	warn("txzchk", "%s: found file with ../ in the path: %s", txzpath, file->filename);
    }
    if (*(file->filename) == '/') {
	++total_issues;
	warn("txzchk", "%s: found absolute path %s", txzpath, file->filename);
    }

    /*
     * Now we have to run some tests on the directory name which we obtained
     * from fnamchk earlier on - but only if fnamchk did not return an
     * error! If it did we'll report other issues but we won't check
     * directory names (at least the directory name expected in the
     * tarball).
     */
    if (dir_name != NULL && strlen(dir_name) > 0) {
	if (strncmp(p, dir_name, strlen(dir_name))) {
	    warn("txzchk", "%s: found incorrect directory in filename %s", txzpath, p);
	    ++total_issues;
	}
    }
    if (strchr(p, '/') == NULL) {
	warn("txzchk", "%s: no directory found in filename %s", txzpath, p);
	++total_issues;
    }
}
/*
 * check_empty_file	- if file is empty, check which file it is and report it
 *
 * given:
 *
 *	txzpath		- the tarball (or text file) we're checking
 *	file_size	- size of the file
 *	file		- the struct file we're checking
 *
 * Returns void.
 *
 * Does not return on error (NULL pointers passed in).
 */
static void
check_empty_file(char const *txzpath, off_t file_size, struct file *file)
{
    /*
     * firewall
     */

    if (txzpath == NULL || file == NULL || file->basename == NULL || file->filename == NULL) {
	err(123, __func__, "called with NULL arg(s)");
	not_reached();
    }

    if (file_size == 0) {
	if (!strcmp(file->basename, ".author.json")) {
	    ++total_issues;
	    warn("txzchk", "%s: found empty .author.json file", txzpath);
	}
	else if (!strcmp(file->basename, ".info.json")) {
	    ++total_issues;
	    warn("txzchk", "%s: found empty .info.json file", txzpath);
	}
	else if (!strcmp(file->basename, "remarks.md")) {
	    ++total_issues;
	    warn("txzchk", "%s: found empty remarks.md", txzpath);
	}
	else if (!strcmp(file->basename, "Makefile")) {
	    ++total_issues;
	    warn("txzchk", "%s: found empty Makefile", txzpath);
	}
    }
}

/*
 * check_all_files		- check files list after parsing tarball (or text file)
 *
 * given:
 *
 *	file_sizes	- total size of all files
 *
 * Reports any additional issues found in the tarball (or text file).
 *
 * Returns void. Ignores empty files (though these should not be in the list at
 * all).
 *
 */
static void
check_all_files(off_t file_sizes)
{
    struct file *file; /* to iterate through files list */
    off_t rounded_file_size = 0; /* file sizes rounded up to 1024 multiple */

    /* report total file size */
    if (file_sizes < 0) {
	err(32, __func__, "%s: total size of all files < 0!", txzpath);
	not_reached();
    } else if (file_sizes == 0) {
	warn("txzchk", "%s: total size of all files == 0", txzpath);
	++total_issues;
    }
    rounded_file_size = round_to_multiple(file_sizes, 1024);
    if (rounded_file_size < 0) {
	err(30, __func__, "%s: total size of all files rounded up to multiple of 1024 < 0!", txzpath);
	not_reached();
    } else if (rounded_file_size > MAX_DIR_KSIZE) {
	warn("txzchk", "%s: total size of files %lu rounded up to multiple of 1024 %lu > %d", txzpath, (unsigned long) file_sizes,
		(unsigned long) rounded_file_size, MAX_DIR_KSIZE);
	++total_issues;
    } else if (!quiet) {
	printf("txzchk: %s total size of files %lu rounded up to 1024 multiple: %lu OK\n", txzpath, (unsigned long) file_sizes,
		(unsigned long) rounded_file_size);
    }


    /*
     * now go through the files list and detect any additional issues
     */
    for (file = files; file != NULL; file = file->next) {
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
	}
	if (file->count > 1) {
	    warn("txzchk", "%s: found a total of %u files with the name %s", txzpath, file->count, file->basename);
	    total_issues += file->count - 1;
	}
    }

    /* determine if the required files are there */
    if (!info.has_info_json) {
	warn("txzchk", "%s: no .info.json found", txzpath);
	++total_issues;
    }
    if (!info.has_author_json) {
	warn("txzchk", "%s: no .author.json found", txzpath);
	++total_issues;
    }
    if (!info.has_prog_c) {
	warn("txzchk", "%s: no prog.c found", txzpath);
	++total_issues;
    }
    if (!info.has_Makefile) {
	warn("txzchk", "%s: no Makefile found", txzpath);
	++total_issues;
    }
    if (!info.has_remarks_md) {
	warn("txzchk", "%s: no remarks.md found", txzpath);
	++total_issues;
    }

    /*
     * Report total number of non .author.json and .info.json dot files.
     * Don't increment the number of issues as this was done when iterating
     * through the linked list above.
     */

    if (info.dot_files > 0) {
	warn("txzchk", "%s: found a total of %u unacceptable dot files", txzpath, info.dot_files);
    }

    /*
     * report total issues found
     */
    if (total_issues > 0) {
	warn("txzchk", "%s: found %u issue%s", txzpath, total_issues, total_issues==1?"":"s");
    }
}

/*
 * parse_linux_line	- parse linux tar output
 *
 * given:
 *
 *	p	    - pointer to current field in line
 *	linep	    - the line we're parsing
 *	line_dup    - duplicated line
 *	dir_name    - directory name retrieved from fnamchk or NULL if it failed
 *	txzpath	    - the tarball path
 *	file_sizes  - pointer to the total file sizes of the tarball
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error.
 */
static void
parse_linux_line(char *p, char *linep, char *line_dup, char const *dir_name, char const *txzpath, off_t *file_sizes)
{
    off_t current_file_size = 0;
    struct file *file = NULL;
    int i = 0;

    /*
     * firewall
     */

    if (p == NULL || linep == NULL || line_dup == NULL || txzpath == NULL || file_sizes == NULL) {
	err(122, __func__, "called with NULL arg(s)");
	not_reached();
    }

    for ( ; *p && isdigit(*p) && *p != '/'; )
	++p;

    if (*p != '/') {
	warn("txzchk", "found non-numerical UID in line %s", line_dup);
	++total_issues;
	p = strchr(p, '/');
    }
    if (p == NULL) {
	warn("txzchk", "encountered NULL pointer when parsing line %s", line_dup);
	return;
    }
    ++p;

    /*
     * now do the same for group id
     */
    for ( ; *p && isdigit(*p); ) {
	++p; /* satisfy warning */
    }

    if (*p) {
	warn("txzchk", "found non-numerical GID in file in line %s", line_dup);
	++total_issues;
    }
    p = strtok(NULL, " \t");
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }

    errno = 0;
    current_file_size = strtoll(p, NULL, 10);
    if (errno != 0) {
	warn("txzchk", "%s: trying to parse file size in on line: %s, string: %s, reading next line", txzpath, line_dup, p);
    } else if (current_file_size < 0) {
	warn("txzchk", "%s: file size < 0", txzpath);
	++total_issues;
    }
    *file_sizes += current_file_size;

    /*
     * the next two fields we don't care about but we loop three times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 3; ++i) {
	p = strtok(NULL, " \t");
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line, reading next line", txzpath);
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_file(p);

    /*
     * although we could check these later we check here because the
     * add_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_empty_file(txzpath, current_file_size, file);

    /* checks on this specific file */
    check_file(txzpath, p, dir_name, file);

    add_file_to_list(file);
}
/*
 * parse_bsd_line	- parse macOS/BSD tar output
 *
 * given:
 *
 *	p	    - pointer to current field in line
 *	linep	    - the line we're parsing
 *	line_dup    - duplicated line
 *	dir_name    - directory name retrieved from fnamchk or NULL if it failed
 *	txzpath	    - the tarball path
 *	file_sizes  - pointer to the total file sizes of the tarball
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error.
 */
static void
parse_bsd_line(char *p, char *linep, char *line_dup, char const *dir_name, char const *txzpath, off_t *file_sizes)
{
    off_t current_file_size = 0;
    struct file *file = NULL;
    int i = 0;

    /*
     * firewall
     */

    if (p == NULL || linep == NULL || line_dup == NULL || txzpath == NULL || file_sizes == NULL) {
	err(122, __func__, "called with NULL arg(s)");
	not_reached();
    }

    p = strtok(NULL, " \t");
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }
    /*
     * attempt to find !isdigit() chars (i.e. the tarball listing includes
     * the owner name of the file).
     */
    for (; p && *p && isdigit(*p); )
	++p; /* satisfy warnings */

    if (*p) {
	warn("txzchk", "%s: found non-numerical UID in file in line %s", txzpath, line_dup);
	++total_issues;
    }

    /*
     * now do the same for group
     */
    p = strtok(NULL, " \t");
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }
    for (; p && *p && isdigit(*p); )
	++p; /* satisfy warnings */

    if (*p) {
	warn("txzchk", "%s: found non-numerical GID in file in line: %s", txzpath, line_dup);
	++total_issues;
    }

    p = strtok(NULL, " \t");
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }

    errno = 0;
    current_file_size = strtoll(p, NULL, 10);
    if (errno != 0) {
	warn("txzchk", "%s: trying to parse file size in on line: %s, string: %s, reading next line", txzpath, line_dup, p);
    }
    *file_sizes += current_file_size;

    /*
     * the next three fields we don't care about but we loop four times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 4; ++i) {
	p = strtok(NULL, " \t");
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line, reading next line", txzpath);
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_file(p);

    /*
     * although we could check these later we check here because the
     * add_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_empty_file(txzpath, current_file_size, file);

    /* checks on this specific file */
    check_file(txzpath, p, dir_name, file);

    add_file_to_list(file);
}
/*
 * parse_line	 - parse a line in the tarball listing
 *
 * given:
 *
 *	linep		-   line to parse
 *	line_dup	-   pointer to the duplicated line
 *	dir_name	-   the dir name reported by fnamchk or NULL if it failed
 *	txzpath		-   the tarball path
 *	file_sizes	-   pointer to file_sizes (from check_tarball())
 *	dir_count	-   pointer to dir_count (from check_tarball())
 *
 *  Function updates total_issues, file_sizes and dir_count. Returns void.
 *
 *  Function does not return on error.
 */
static void
parse_line(char *linep, char *line_dup, char const *dir_name, char const *txzpath, off_t *file_sizes, int *dir_count)
{
    char *p = NULL;

    /*
     * firewall
     */

    if (linep == NULL || line_dup == NULL || txzpath == NULL || file_sizes == NULL || dir_count == NULL) {
	err(33, __func__, "called with NULL arg(s)");
	not_reached();
    }
    /*
     * look for more than one directory
     */
    if (*linep == 'd') {
	++(*dir_count);
	if (*dir_count > 1) {
	    warn("txzchk", "%s: found more than one directory entry: %s", txzpath, linep);
	    ++total_issues;
	}

    /*
     * look for non-directory non-regular non-hard-lined items
     */
    } else if (*linep != '-') {
	warn("txzchk", "%s: found a non-directory non-regular non-hard-lined item: %s", txzpath, linep);
	++total_issues;
    }
    /* extract each field, one at a time, to do various tests */
    p = strtok(linep, " \t");
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }
    if (has_special_bits(p)) {
	warn("txzchk", "%s: found special bits on line: %s", txzpath, line_dup);
	++total_issues;
    }
    /*
     * we have to check this next field for a '/': this will tell us whether to
     * parse it for linux or for macOS/BSD.
     */
    p = strtok(NULL, " \t");
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }
    if (strchr(p, '/') != NULL) {
	/* found linux output */
	parse_linux_line(p, linep, line_dup, dir_name, txzpath, file_sizes);
    } else {
	/* assume macOS/BSD output */
	parse_bsd_line(p, linep, line_dup, dir_name, txzpath, file_sizes);
    }
}

/*
 * check_tarball - perform tests on tarball, validating it for the IOCCC
 *
 * given:
 *
 *	tar		- path to executable tar program (if -T was not
 *			  specified)
 *	fnamchk		- path to fnamchk tool
 *
 *  Returns the number of total number of issues found (total_issues).
 *
 *  Does not return on error.
 */
static unsigned
check_tarball(char const *tar, char const *fnamchk)
{
    off_t size = 0; /* file size of tarball */
    off_t file_sizes = 0; /* accumulation of file sizes within the tarball */
    unsigned line_num = 0; /* line number of tar output */
    char *cmd = NULL;	/* fnamchk and tar -tJvf */
    FILE *input_stream = NULL; /* pipe for tar output (or if -T specified read as a text file) */
    FILE *fnamchk_stream = NULL; /* pipe for fnamchk output */
    char *linep = NULL;		/* allocated line read from tar (or text file) */
    char *dir_name = NULL;	/* line read from fnamchk (directory name) */
    ssize_t readline_len;	/* readline return length */
    int ret;			/* libc function return */
    int exit_code;
    bool fnamchk_okay = false;    /* true ==> fnamchk passed */

    /*
     * firewall
     */
    if ((!text_file_flag_used && tar == NULL) || fnamchk == NULL || txzpath == NULL) {
	err(13, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * First of all we have to run fnamchk on the tarball: this is important
     * because we have to know the actual directory name the files should be in
     * within the tarball and we cannot use strtok(3) on the strdup()'d strings
     * (since we then can't free them) later on so we have to act on the path in
     * the initial loop when discovering the filenames.
     */

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
	errp(33, __func__, "%s: error calling system(%s)", txzpath, cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(34, __func__, "%s: execution of the shell failed for system(%s)", txzpath, cmd);
	not_reached();
    } else if (exit_code != 0) {
	warn("txzchk", "%s: %s failed with exit code: %d", txzpath, cmd, WEXITSTATUS(exit_code));
	++total_issues;
    } else {
	fnamchk_okay = true;
    }

    /*
     * If fnamchk went okay we have to retrieve the directory name that's
     * expected and compare it to what's in the filenames of the tarball. Since
     * it's the only output we shouldn't have to loop at all. If the output
     * format of the tool changes this must also change!
     *
     * Note that the reason we don't exit if fnamchk reports an error is we
     * still can detect other issues; we just won't detect issues with the entry
     * number and directory.
     */
    if (fnamchk_okay) {

	/*
	 * form pipe to the fnamchk command
	 */
	errno = 0;			/* pre-clear errno for errp() */
	fnamchk_stream = popen(cmd, "r");
	if (fnamchk_stream == NULL) {
	    errp(24, __func__, "popen for reading failed for: %s", cmd);
	    not_reached();
	}
	setvbuf(fnamchk_stream, (char *)NULL, _IOLBF, 0);

	readline_len = readline(&dir_name, fnamchk_stream);
	if (readline_len < 0) {
	    warn("txzchk", "%s: unexpected EOF from fnamchk", txzpath);
	}

	/*
	 * close down pipe
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = pclose(fnamchk_stream);
	if (ret < 0) {
	    warnp(__func__, "%s: pclose error on fnamchk stream", txzpath);
	}
	fnamchk_stream = NULL;

	if (dir_name == NULL || !strlen(dir_name)) {
	    err(40, __func__, "txzchk: %s: unexpected NULL pointer from %s", txzpath, cmd);
	    not_reached();
	}
    }

    /* determine size of tarball */
    size = file_size(txzpath);
    /* report size (if too big) */
    if (size < 0) {
	err(14, __func__, "%s: impossible error: sanity_chk() found tarball but file_size() did not", txzpath);
	not_reached();
    }
    else if (size > MAX_TARBALL_LEN) {
	++total_issues;
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(15, __func__, "%s: The compressed tarball size %lu > %ld",
		 txzpath, (unsigned long)size, (long)MAX_TARBALL_LEN);
	not_reached();
    }
    else if (!quiet) {
	errno = 0;
	ret = printf("txzchk: %s size of %lu bytes OK\n", txzpath, (unsigned long) size);
	if (ret <= 0) {
	    warn("txzchk", "unable to tell user how big the tarball %s is", txzpath);
	}
    }
    dbg(DBG_MED, "txzchk: %s size in bytes: %lu", txzpath, (unsigned long)size);

    /*
     * free cmd for tar (if -T wasn't specified) command
     */
    free(cmd);
    cmd = NULL;

    if (text_file_flag_used) {
	input_stream = fopen(txzpath, "r");
	errno = 0;
	if (input_stream == NULL) {
	    errp(122, __func__, "fopen of %s failed", txzpath);
	    not_reached();
	}
    }
    else { /* if -T not specified we have to do more to set up input stream */

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
	 * If we get here -T was not specified so open pipe to tar command.
	 */
	errno = 0;			/* pre-clear errno for errp() */
	input_stream = popen(cmd, "r");
	if (input_stream == NULL) {
	    errp(24, __func__, "popen for reading failed for: %s", cmd);
	    not_reached();
	}
    }
    setvbuf(input_stream, (char *)NULL, _IOLBF, 0);

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
	readline_len = readline(&linep, input_stream);
        if (readline_len < 0) {
	    dbg(DBG_HIGH, "reached EOF of tarball %s", txzpath);
	    break;
	} else if (readline_len == 0) {
	    dbg(DBG_HIGH, "found empty line in tarball %s", txzpath);
	    continue;
	}

	/*
	 * scan for embedded NUL bytes (before end of line)
	 *
	 */
	errno = 0;			/* pre-clear errno for errp() */
	p = (char *)memchr(linep, 0, (size_t)readline_len);
	if (p != NULL) {
	    warn("txzchk", "found NUL before end of line, reading next line");
	    continue;
	}
	dbg(DBG_VHIGH, "line %d: %s", line_num, linep);


	/*
	 * add line to list (to parse once the list of files has been shown to
	 * the user.
	 */
	add_line(linep, line_num);

	if (text_file_flag_used) {
	    errno = 0;
	    ret = printf("%s\n", linep);
	    if (ret <= 0) {
		warnp(__func__, "unable to printf line from text file");
	    }
	}


    } while (readline_len >= 0);
    /*
     * free cmd (it'll be NULL if -T was specified but this is safe)
     */
    free(cmd);
    cmd = NULL;

    /* now parse the lines, reporting any issue that have to be done whilst
     * parsing.
     */
    parse_all_lines(dir_name, txzpath, &file_sizes);

    /*
     * check files list and report any additional issues
     */
    check_all_files(file_sizes);

    /* free the files list */
    free_file_list();

    /* free lines list */
    free_lines();

    /*
     * close down pipe
     */
    errno = 0;		/* pre-clear errno for errp() */
    if (text_file_flag_used)
	ret = fclose(input_stream);
    else
	ret = pclose(input_stream);
    if (ret < 0) {
	warnp(__func__, "%s: %s error on tar stream", txzpath, text_file_flag_used?"fclose":"pclose");
    }
    input_stream = NULL;

    return total_issues;
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
 * add_line	    - add line to lines list
 *
 * given:
 *
 *	str	    - line to add to the list
 *	line_num    - line number
 *
 * The purpose of this function is that we can show the tarball listing in one
 * go and then after that we can parse this so that any warnings are not
 * interspersed with the tarball list thus keeping the output cleaner.
 *
 * This function returns void.
 */
static void
add_line(char const *str, int line_num)
{
    struct line *line;

    /*
     * firewall
     */
    if (str == NULL) {
	err(35, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    line = calloc(1, sizeof *line);
    if (line == NULL) {
	err(36, __func__, "unable to allocate struct line *");
	not_reached();
    }

    errno = 0;
    line->line = strdup(str);
    if (line->line == NULL) {
	err(37, __func__, "unable to strdup string '%s' for lines list", str);
	not_reached();
    }
    line->line_num = line_num;

    dbg(DBG_MED, "adding line %s to lines list", line->line);
    line->next = lines;
    lines = line;
}

/*
 * parse_all_lines	    - parse lines, reporting any issues found
 *
 * given:
 *
 *	dir_name    - directory name as reported by fnamchk (can be NULL if
 *		      fnamchk failed to validate directory)
 *	txzpath	    - the tarball that is being read
 *	file_sizes  - pointer to file size accumulation
 *
 * This function returns void.
 *
 * This function does not return on error.
 */
static void
parse_all_lines(char const *dir_name, char const *txzpath, off_t *file_sizes)
{
    struct line *line = NULL;	/* for lines list */
    char *line_dup = NULL;	/* strdup()d line */
    int dir_count = 0;		/* number of directories detected */

    /*
     * firewall
     */
    if (txzpath == NULL || file_sizes == NULL) {
	err(35, __func__, "passed NULL arg(s)");
	not_reached();
    }

    for (line = lines; line; line = line->next) {
	if (line->line == NULL) {
	    warn("txzchk", "encountered NULL string on line %d", line->line_num);
	    continue;
	}
	line_dup = strdup(line->line);
	if (line_dup == NULL) {
	    err(25, __func__, "%s: duplicating %s failed", txzpath, line->line);
	    not_reached();
	}

	parse_line(line->line, line_dup, dir_name, txzpath, file_sizes, &dir_count);
	free(line_dup);
	line_dup = NULL;
    }
}
/*
 * free_lines	    - free lines list
 *
 * The purpose of the lines list is so that we can show the list of files in the
 * tarball together without interspersing it with any warnings. Thus we show the
 * files list, adding each line to the list in the process, and then after that
 * we can iterate through the lines and show any warnings. After that we report
 * any issues that haven't been reported yet (some warnings have to be issued
 * whilst parsing the lines).
 *
 * This function returns void.
 */
static void
free_lines(void)
{
    struct line *line, *next_line;

    for (line = lines; line != NULL; line = next_line) {
	next_line = line->next;
	if (line->line) {
	    free(line->line);
	    line->line = NULL;
	}

	free(line);
	line = NULL;
    }

    lines = NULL;
}

/* alloc_file	- allocate a struct file *
 *
 * given:
 *
 *	p	- file path
 *
 * Returns the newly allocated struct file * with the file information. The
 * function does NOT add it to the list!
 *
 * This function does not return on error.
 */
static struct file *
alloc_file(char const *p)
{
    struct file *file; /* the file structure */

    /*
     * firewall
     */
    if (p == NULL) {
	err(35, __func__, "passed NULL path");
	not_reached();
    }
    errno = 0;
    file = calloc(1, sizeof *file);
    if (file == NULL) {
	err(32, __func__, "%s: unable to allocate a struct file *", txzpath);
	not_reached();
    }

    errno = 0;
    file->filename = strdup(p);
    if (!file->filename) {
	err(33, __func__, "%s: unable to strdup filename %s", txzpath, p);
	not_reached();
    }

    errno = 0;
    file->basename = strdup(base_name(p)?base_name(p):"");
    if (!file->basename || !strlen(file->basename)) {
	err(34, __func__, "%s: unable to strdup basename of filename %s", txzpath, p);
	not_reached();
    }

    return file;
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

    files = NULL;
}
