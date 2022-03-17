/*
 * txzchk: the IOCCC tarball validation checker
 *
 * Invoked by mkiocccentry; txzchk in turn uses fnamchk to make sure that
 * the tarball was correctly named and formed (i.e. the mkiocccentry tool was
 * used).
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>	/* for strcasecmp() */
#include <ctype.h>
#include <stdint.h>

/*
 * Our header file - #includes the header files we need
 */
#include "txzchk.h"


int
main(int argc, char **argv)
{
    extern char *optarg;		    /* option argument */
    extern int optind;			    /* argv index of the next arg */
    char *tar = TAR_PATH_0;		    /* path to tar executable that supports the -J (xz) option */
    char *fnamchk = FNAMCHK_PATH_0;	    /* path to fnamchk tool */
    bool fnamchk_flag_used = false;	    /* if -F option used */
    bool tar_flag_used = false;		    /* true ==> -t /path/to/tar was given */
    int ret;				    /* libc return code */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVF:t:fT")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(0, "-h help mode", program);
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'q':
	    quiet = true;
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
	case 'f':
	    text_file_flag_used = true; /* don't rely on tar: just read file as if it was a text file */
	    break;
	case 'T':		/* -T (IOCCC toolkit release repository tag) */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", IOCCC_TOOLKIT_RELEASE);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing IOCCC toolkit release repository tag");
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	default:
	    usage(1, "invalid -flag", program); /*ooo*/
	    not_reached();
	 }
    }
    /* be warn(), warnp() and msg() quiet of -q and -v 0 */
    if (quiet && verbosity_level <= 0) {
	msg_output_allowed = false;
	warn_output_allowed = false;
    }
    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(1, "wrong number of arguments", program); /*ooo*/
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
	    errp(1, __func__, "printf error printing the welcome string");
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
     * If -f was used we don't actually need tar(1) so we test for that
     * specifically.
     */

    if (!text_file_flag_used)
	find_utils(tar_flag_used, &tar, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk, false, NULL, false, NULL);
    else
	find_utils(false, NULL, false, NULL, false, NULL, false, NULL, fnamchk_flag_used, &fnamchk, false, NULL, false, NULL);


    /*
     * environment sanity checks
     */
    if (!quiet) {
	para("", "Performing sanity checks on your environment ...", NULL);
    }
    txzchk_sanity_chks(tar, fnamchk);
    if (!quiet) {
	para("... environment looks OK", NULL);
    }

    /*
     * check the tarball
     */
    if (!quiet) {
	para("", "Performing checks on tarball ...", NULL);
    }
    txz_info.total_issues = check_tarball(tar, fnamchk);
    if (!quiet && !txz_info.total_issues) {
	para("All checks passed.", "", NULL);
    }

    show_txz_info(txzpath);

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(txz_info.total_issues != 0); /*ooo*/
}


/*
 * show_txz_info    - show information about tarball (if verbosity is >= medium)
 *
 * given:
 *
 *	txzpath	    - path to tarball we checked
 *
 * Returns void. Does not return on error.
 */
static void
show_txz_info(char const *txzpath)
{
    /*
     * firewall
     */
    if (txzpath == NULL) {
	err(2, __func__, "passed NULL arg");
	not_reached();
    }
    if (verbosity_level >= DBG_MED) {
	/* show information about tarball */
	para("", "The following information about the tarball was collected:", NULL);

	dbg(DBG_MED, "txzchk: %s: has .info.json:\t\t%d", txzpath, txz_info.has_info_json);
	dbg(DBG_HIGH, "txzchk: %s: empty .info.json:\t\t%d", txzpath, txz_info.empty_info_json);
	dbg(DBG_MED, "txzchk: %s: has .author.json:\t\t%d", txzpath, txz_info.has_author_json);
	dbg(DBG_HIGH, "txzchk: %s: empty .author.json:\t\t%d", txzpath, txz_info.empty_author_json);
	dbg(DBG_MED, "txzchk: %s: has prog.c:\t\t\t%d", txzpath, txz_info.has_prog_c);
	dbg(DBG_HIGH, "txzchk: %s: empty prog.c:\t\t\t%d", txzpath, txz_info.empty_prog_c);
	dbg(DBG_MED, "txzchk: %s: has remarks.md:\t\t\t%d", txzpath, txz_info.has_remarks_md);
	dbg(DBG_HIGH, "txzchk: %s: empty remarks.md:\t\t%d", txzpath, txz_info.empty_remarks_md);
	dbg(DBG_MED, "txzchk: %s: has Makefile:\t\t\t%d", txzpath, txz_info.has_Makefile);
	dbg(DBG_HIGH, "txzchk: %s: empty Makefile:\t\t\t%d", txzpath, txz_info.empty_Makefile);
	dbg(DBG_MED, "txzchk: %s: size:\t\t\t\t%jd", txzpath, (intmax_t)txz_info.size);
	dbg(DBG_MED, "txzchk: %s: size of all files:\t\t%jd", txzpath, (intmax_t)txz_info.file_sizes);
	dbg(DBG_MED, "txzchk: %s: rounded files size:\t\t%jd", txzpath, (intmax_t)txz_info.rounded_file_size);
	dbg(DBG_MED, "txzchk: %s: total files:\t\t\t%d", txzpath, txz_info.total_files);
	dbg(DBG_MED, "txzchk: %s: incorrect directory found:\t%d", txzpath, txz_info.correct_directory != txz_info.total_files);
	dbg(DBG_MED, "txzchk: %s: invalid dot files found:\t\t%d", txzpath, txz_info.dot_files);
	dbg(DBG_MED, "txzchk: %s: files named '.':\t\t\t%d", txzpath, txz_info.named_dot);
	dbg(DBG_MED, "txzchk: %s: files with invalid chars:\t%u", txzpath, txz_info.invalid_chars);
	dbg(DBG_VHIGH, "txzchk: %s: issues found:\t\t\t%d", txzpath, txz_info.total_issues);

    }
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
usage(int exitcode, char const *str, char const *prog)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn("txzchk", "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "txzchk";
	warn("txzchk", "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, TAR_PATH_0, FNAMCHK_PATH_0, TXZCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * txzchk_sanity_chks - perform basic sanity checks
 *
 * We perform basic sanity checks on paths and the IOCCC contest ID as well as
 * the IOCCC toolkit tables. Note that these tables are not used in txzchk but
 * to make sure everything is sane in every tool we make these checks.
 *
 * given:
 *
 *      tar             - path to tar that supports the -J (xz) option
 *	fnamchk		- path to the fnamchk utility
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
txzchk_sanity_chks(char const *tar, char const *fnamchk)
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
		  "A tar program that supports the -J (xz) option is required to test the compressed tarball.",
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
		  "The tar, while it exists, is not a file.",
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
		  "The tar, while it is a file, is not executable.",
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
	      "The fnamchk, while it exists, is not a file.",
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
	      "The fnamchk, while it is a file, is not executable.",
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
	      "The file specified, while it exists, is not a regular file.",
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
	      "The tarball path, while it is a file, is not readable.",
	      "",
	      "We suggest you check the permissions on the path or use another path:",
	      "",
	      "    txzchk [...] <txzpath>"
	      "",
	      NULL);
	err(12, __func__, "txzpath is not readable: %s", txzpath);
	not_reached();
    }

    /* we also check that all the tables across the IOCCC toolkit are sane */
    ioccc_sanity_chks();

    return;
}


/*
 * check_txz_file - checks on the current file only
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
check_txz_file(char const *txzpath, char *p, char const *dir_name, struct txz_file *file)
{
    bool allowed_dot_file = false;	/* true ==> basename is an allowed . file */

    /*
     * firewall
     */
    if (txzpath == NULL || p == NULL || file == NULL || file->basename == NULL || file->filename == NULL) {
	err(13, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * identify if file is an allowed '.' file
     *
     * NOTE: Files that are allowed to begin with '.' must also be lower case.
     * In other words if any of the letters in ".info.json" or ".author.json"
     * are upper case the file is an invalid dot file.
     */
    if (!strcmp(file->basename, ".info.json") || !strcmp(file->basename, ".author.json")) {
	allowed_dot_file = true;
    }

    /*
     * filename must use only POSIX portable filename and + chars plus /
     */
    if (!posix_plus_safe(file->filename, false, true, false)) {
	++txz_info.total_issues; /* report it once and consider it only one issue */
	++txz_info.invalid_chars;
	warn(__func__, "%s: file does not match regexp ^[/0-9a-z][/0-9a-z._+-]*$: %s",
		       txzpath, file->filename);
    }

    /*
     * case: basename is NOT allowed to begin with a dot.
     */
    if (!allowed_dot_file) {
	/*
	 * Check for dot files but note that a basename of only '.' also counts
	 * as a filename with just '.': so if the file starts with a '.' and
	 * it's not ".author.json" and not ".info.json" then it's an invalid dot
	 * file; if it's ONLY '.' it counts as BOTH an invalid dot file AND a
	 * file called just '.' (which would likely be a directory but is abuse
	 * nonetheless).
	 */
	if (*(file->basename) == '.') {
	    ++txz_info.total_issues;
	    warn("txzchk", "%s: found non .author.json and .info.json dot file %s", txzpath, file->basename);
	    txz_info.dot_files++;

	    /* check for files called '.' without anything after the dot */
	    if (file->basename[1] == '\0') {
		++txz_info.total_issues;
		++txz_info.named_dot;
		warn("txzchk", "%s: found file called '.' in path %s", txzpath, file->filename);
	    }
	}

	/*
	 * basename must use only POSIX portable filename and + chars
	 */
	if (!posix_plus_safe(file->basename, false, false, true)) {
	    ++txz_info.total_issues; /* report it once and consider it only one issue */
	    ++txz_info.invalid_chars;
	    warn(__func__, "%s: file basename does not match regexp ^[0-9A-Za-z][0-9A-Za-z._+-]*$: %s",
			   txzpath, file->basename);
	}
    }

    /* check the dirs in the path */
    check_directories(file, dir_name, txzpath);
}


/*
 * check_empty_file - if file is empty, check which file it is and report it
 *
 * given:
 *
 *	txzpath		- the tarball (or text file) we're checking
 *	size		- size of the file
 *	file		- the struct txz_file we're checking
 *
 * Returns void.
 *
 * Does not return on error (NULL pointers passed in).
 */
static void
check_empty_file(char const *txzpath, off_t size, struct txz_file *file)
{
    /*
     * firewall
     */

    if (txzpath == NULL || file == NULL) {
	err(14, __func__, "called with NULL arg(s)");
	not_reached();
    } else if (file->basename == NULL || file->filename == NULL) {
	err(15, __func__, "file->basename == NULL || file->filename == NULL which should never happen");
	not_reached();
    }

    if (size == 0) {
	if (!strcmp(file->basename, ".author.json")) {
	    ++txz_info.total_issues;
	    warn("txzchk", "%s: found empty .author.json file", txzpath);
	    txz_info.empty_author_json = true;
	}
	else if (!strcmp(file->basename, ".info.json")) {
	    ++txz_info.total_issues;
	    txz_info.empty_info_json = true;
	    warn("txzchk", "%s: found empty .info.json file", txzpath);
	}
	else if (!strcmp(file->basename, "remarks.md")) {
	    ++txz_info.total_issues;
	    txz_info.empty_remarks_md = true;
	    warn("txzchk", "%s: found empty remarks.md", txzpath);
	}
	else if (!strcmp(file->basename, "Makefile")) {
	    ++txz_info.total_issues;
	    txz_info.empty_Makefile = true;
	    warn("txzchk", "%s: found empty Makefile", txzpath);
	}
	else if (!strcmp(file->basename, "prog.c")) {
	    /* this is NOT an issue: it's only for debugging information! */
	    txz_info.empty_prog_c = true;
	}
    }
}


/*
 * check_all_txz_files - check txz_files list after parsing tarball (or text file)
 *
 * given:
 *
 *	dir_name	- fnamchk result (if passed - else NULL)
 *
 * Reports any additional issues found in the tarball (or text file).
 *
 * Returns void. Ignores empty files (though these should not be in the list at
 * all).
 *
 * Does not return on NULL filenames or basenames (neither of which should ever
 * happen).
 */
static void
check_all_txz_files(char const *dir_name)
{
    struct txz_file *file; /* to iterate through files list */

    /* report total file size */
    if (txz_info.file_sizes < 0) {
	err(16, __func__, "%s: total size of all files < 0!", txzpath);
	not_reached();
    } else if (txz_info.file_sizes == 0) {
	warn("txzchk", "%s: total size of all files == 0", txzpath);
	++txz_info.total_issues;
    }
    txz_info.rounded_file_size = round_to_multiple(txz_info.file_sizes, 1024);
    if (txz_info.rounded_file_size < 0) {
	err(17, __func__, "%s: total size of all files rounded up to multiple of 1024 < 0!", txzpath);
	not_reached();
    } else if (txz_info.rounded_file_size > MAX_DIR_KSIZE) {
	warn("txzchk", "%s: total size of files %jd rounded up to multiple of 1024 %jd > %d",
		       txzpath, (intmax_t)txz_info.file_sizes,
		       (intmax_t) txz_info.rounded_file_size, MAX_DIR_KSIZE);
	++txz_info.total_issues;
    } else if (!quiet) {
	printf("txzchk: %s total size of files %jd rounded up to 1024 multiple: %jd OK\n",
		txzpath, (intmax_t) txz_info.file_sizes, (intmax_t) txz_info.rounded_file_size);
    }


    /*
     * Now go through the files list to verify the required files are there and
     * also to detect any additional issues.
     */
    for (file = txz_files; file != NULL; file = file->next) {
	if (file->basename == NULL) {
	    err(18, __func__, "found NULL file->basename in txz_files list");
	    not_reached();
	} else if (file->filename == NULL) {
	    err(19, __func__, "found NULL file->filename in txz_files list");
	    not_reached();
	}
	if (!strcmp(file->basename, ".info.json")) {
	    txz_info.has_info_json = true;
	} else if (!strcmp(file->basename, ".author.json")) {
	    txz_info.has_author_json = true;
	} else if (!strcmp(file->basename, "Makefile")) {
	    txz_info.has_Makefile = true;
	} else if (!strcmp(file->basename, "prog.c")) {
	    txz_info.has_prog_c = true;
	} else if (!strcmp(file->basename, "remarks.md")) {
	    txz_info.has_remarks_md = true;
	}
	if (dir_name != NULL && txz_info.correct_directory) {
	    if (strncmp(file->filename, dir_name, strlen(dir_name))) {
		warn("txzchk", "%s: found directory change in filename %s", txzpath, file->filename);
		++txz_info.total_issues;
	    }
	}

	if (file->count > 1) {
	    warn("txzchk", "%s: found a total of %u files with the name %s", txzpath, file->count, file->basename);
	    txz_info.total_issues += file->count - 1;
	}
    }

    /* determine if the required files are there */
    if (!txz_info.has_info_json) {
	++txz_info.total_issues;
	warn("txzchk", "%s: no .info.json found", txzpath);
    }
    if (!txz_info.has_author_json) {
	++txz_info.total_issues;
	warn("txzchk", "%s: no .author.json found", txzpath);
    }
    if (!txz_info.has_prog_c) {
	++txz_info.total_issues;
	warn("txzchk", "%s: no prog.c found", txzpath);
    }
    if (!txz_info.has_Makefile) {
	++txz_info.total_issues;
	warn("txzchk", "%s: no Makefile found", txzpath);
    }
    if (!txz_info.has_remarks_md) {
	++txz_info.total_issues;
	warn("txzchk", "%s: no remarks.md found", txzpath);
    }
    if (txz_info.correct_directory < txz_info.total_files) {
	++txz_info.total_issues;
	warn("txzchk", "%s: not all files in correct directory", txzpath);
    }

    /*
     * Report total number of non .author.json and .info.json dot files.
     * Don't increment the number of issues as this was done in
     * check_txz_file().
     */
    if (txz_info.dot_files > 0) {
	warn("txzchk", "%s: found a total of %u unacceptable dot file%s", txzpath, txz_info.dot_files, txz_info.dot_files==1?"":"s");
    }

    /*
     * report total issues found
     */
    if (txz_info.total_issues > 0) {
	warn("txzchk", "%s: found %u issue%s", txzpath, txz_info.total_issues, txz_info.total_issues==1?"":"s");
    }
}


/*
 * check_directories - directory specific checks on the file
 *
 * given:
 *
 *	file		- file structure
 *	dir_name	- the directory expected (or NULL if fnamchk fails)
 *	txzpath		- the tarball path
 *
 * Issues a warning for every violations specific to directories (and
 * subdirectories).
 *
 * Does not return on error.
 */
static void
check_directories(struct txz_file *file, char const *dir_name, char const *txzpath)
{
    unsigned dir_count = 0; /* number of directories in the path */
    int prev = '\0';
    int first = '\0';
    int i;

    /*
     * firewall
     */
    if (txzpath == NULL || file == NULL || file->filename == NULL) {
	err(20, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /* check that there is a directory */
    if (strchr(file->filename, '/') == NULL && strcmp(file->filename, ".")) {
	warn("txzchk", "%s: no directory found in filename %s", txzpath, file->filename);
	++txz_info.total_issues;
    }
    if (strstr(file->filename, "..")) { /* check for '..' in path */
	/*
	 * Note that this check does NOT detect a file in the form of "../.file"
	 * but since the basename of each file is checked in check_txz_file() this
	 * is okay.
	 */
	++txz_info.total_issues;
	warn("txzchk", "%s: found file with .. in the path: %s", txzpath, file->filename);
    }
    if (*(file->filename) == '/') {
	++txz_info.total_issues;
	warn("txzchk", "%s: found absolute path %s", txzpath, file->filename);
    }

    /*
     * Check the path to see if there are any subdirectories. The way this is
     * done is counting the number of '/' but done carefully: for example the
     * path test-1//prog.c would not count as two directories but just one.
     *
     * Another example: ..// would be counted as one directory but it still has
     * ../ so that would have been detected above.
     *
     * It does this by saving the previous character: if it was also a '/' then
     * it's not counted as another directory: the first one will be counted
     * however since there wasn't one before it.
     *
     * We don't count the first character of the path because a path like:
     * /test-3/ would be counted as two directories but it's actually only one.
     * Well it kind of is two but / is special and it would still trigger an
     * absolute path warning - at least from a text file (tar strips it off) -
     * because it starts with a '/'.
     *
     * Note that the path /test-3 would trigger a warning that it's not in the
     * correct directory because it's an absolute directory. However since tar
     * strips the initial '/'s this would probably not get flagged.
     *
     * Note also that if the tar output does not have a trailing '/' in a
     * directory entry itself it would not count as another directory. However
     * since we also check for more than one 'd' line in the output it would
     * trigger more than one directory in the tarball.
     *
     * We keep track of two previous characters. The reason is that
     * 'test-3/././file' should count as only one directory but previously
     * (first version of this) it detected more than one directory because the
     * '.' was not considered. Notice that the path 'test-3/.././file' will
     * trigger both '../' in the path as well as more than one directory.
     */
    first = file->filename[0];
    prev = file->filename[1];
    for (i = 1; file->filename[i]; ++i) {
	if (file->filename[i] == '/' && prev != '/' && first != '.') {
	    ++dir_count;
	}
	first = prev;
	prev = file->filename[i];
    }
    if (dir_count > 1) {
	++txz_info.total_issues;
	warn("txzchk", "%s: found more than one directory in path %s", txzpath, file->filename);
    }
    /*
     * Now we have to run some tests on the directory name which we obtained
     * from fnamchk earlier on - but only if fnamchk did not return an
     * error! If it did we'll report other issues but we won't check
     * directory names (at least the directory name expected in the
     * tarball).
     */
    if (dir_name != NULL && strlen(dir_name) > 0) {
	if (strncmp(file->filename, dir_name, strlen(dir_name))) {
	    warn("txzchk", "%s: found incorrect directory in filename %s", txzpath, file->filename);
	    ++txz_info.total_issues;
	}
	else {
	    /* This file is in the right directory */
	    txz_info.correct_directory++;
	}
    }
}


/*
 * parse_linux_txz_line - parse linux tar output
 *
 * given:
 *
 *	p	    - pointer to current field in line
 *	linep	    - the line we're parsing
 *	line_dup    - duplicated line
 *	dir_name    - directory name retrieved from fnamchk or NULL if it failed
 *	txzpath	    - the tarball path
 *	saveptr	    - pointer to char * to save context between each strtok_r() call
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_txz_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error.
 */
static void
parse_linux_txz_line(char *p, char *linep, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr)
{
    off_t current_file_size = 0;
    struct txz_file *file = NULL;
    int i = 0;

    /*
     * firewall
     */

    if (p == NULL || linep == NULL || line_dup == NULL || txzpath == NULL || saveptr == NULL) {
	err(21, __func__, "called with NULL arg(s)");
	not_reached();
    }

    for ( ; *p && isdigit(*p) && *p != '/'; )
	++p;

    if (*p != '/') {
	warn("txzchk", "found non-numerical UID in line %s", line_dup);
	++txz_info.total_issues;
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
	++txz_info.total_issues;
    }
    p = strtok_r(NULL, " \t", saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }

    errno = 0;
    current_file_size = strtoll(p, NULL, 10);
    if (errno != 0) {
	warnp("txzchk", "%s: trying to parse file size in on line: %s, string: %s, reading next line", txzpath, line_dup, p);
    } else if (current_file_size < 0) {
	warn("txzchk", "%s: file size < 0", txzpath);
	++txz_info.total_issues;
    }
    txz_info.file_sizes += current_file_size;

    /*
     * the next two fields we don't care about but we loop three times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 3; ++i) {
	p = strtok_r(NULL, " \t", saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line, reading next line", txzpath);
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_txz_file(p);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_empty_file(txzpath, current_file_size, file);

    /* checks on this specific file */
    check_txz_file(txzpath, p, dir_name, file);

    add_txz_file_to_list(file);
}


/*
 * parse_bsd_txz_line - parse macOS/BSD tar output
 *
 * given:
 *
 *	p	    - pointer to current field in line
 *	linep	    - the line we're parsing
 *	line_dup    - duplicated line
 *	dir_name    - directory name retrieved from fnamchk or NULL if it failed
 *	txzpath	    - the tarball path
 *	saveptr	    - pointer to char * to save context between each strtok_r() call
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_txz_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error.
 */
static void
parse_bsd_txz_line(char *p, char *linep, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr)
{
    off_t current_file_size = 0;
    struct txz_file *file = NULL;
    int i = 0;

    /*
     * firewall
     */

    if (p == NULL || linep == NULL || line_dup == NULL || txzpath == NULL || saveptr == NULL) {
	err(22, __func__, "called with NULL arg(s)");
	not_reached();
    }

    p = strtok_r(NULL, " \t", saveptr);
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
	++txz_info.total_issues;
    }

    /*
     * now do the same for group
     */
    p = strtok_r(NULL, " \t", saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }
    for (; p && *p && isdigit(*p); )
	++p; /* satisfy warnings */

    if (*p) {
	warn("txzchk", "%s: found non-numerical GID in file in line: %s", txzpath, line_dup);
	++txz_info.total_issues;
    }

    p = strtok_r(NULL, " \t", saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }

    errno = 0;
    current_file_size = strtoll(p, NULL, 10);
    if (errno != 0) {
	warnp("txzchk", "%s: trying to parse file size in on line: %s, string: %s, reading next line", txzpath, line_dup, p);
    }
    txz_info.file_sizes += current_file_size;

    /*
     * the next three fields we don't care about but we loop four times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 4; ++i) {
	p = strtok_r(NULL, " \t", saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line, reading next line", txzpath);
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_txz_file(p);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_empty_file(txzpath, current_file_size, file);

    /* checks on this specific file */
    check_txz_file(txzpath, p, dir_name, file);

    add_txz_file_to_list(file);
}


/*
 * parse_txz_line  - parse a line in the tarball listing
 *
 * given:
 *
 *	linep		-   line to parse
 *	line_dup	-   pointer to the duplicated line
 *	dir_name	-   the dir name reported by fnamchk or NULL if it failed
 *	txzpath		-   the tarball path
 *	dir_count	-   pointer to dir_count (from check_tarball())
 *
 *  Function updates txz_info.total_issues, txz_info.file_sizes and dir_count. Returns void.
 *
 *  Function does not return on error.
 */
static void
parse_txz_line(char *linep, char *line_dup, char const *dir_name, char const *txzpath, int *dir_count)
{
    char *p = NULL; /* each field in the line extracted from strtok_r() */
    char *saveptr = NULL; /* for strtok_r() context */

    /*
     * firewall
     */

    if (linep == NULL || line_dup == NULL || txzpath == NULL || dir_count == NULL) {
	err(23, __func__, "called with NULL arg(s)");
	not_reached();
    }
    /*
     * look for more than one directory
     */
    if (*linep == 'd') {
	++(*dir_count);
	if (*dir_count > 1) {
	    warn("txzchk", "%s: found more than one directory entry: %s", txzpath, linep);
	    ++txz_info.total_issues;
	}

    /*
     * look for non-directory non-regular non-hard-lined items
     */
    } else if (*linep != '-') {
	warn("txzchk", "%s: found a non-directory non-regular non-hard-lined item: %s", txzpath, linep);
	++txz_info.total_issues;
    }
    /* extract each field, one at a time, to do various tests */
    p = strtok_r(linep, " \t", &saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }
    if (has_special_bits(p)) {
	warn("txzchk", "%s: found special bits on line: %s", txzpath, line_dup);
	++txz_info.total_issues;
    }
    /*
     * we have to check this next field for a '/': this will tell us whether to
     * parse it for linux or for macOS/BSD.
     */
    p = strtok_r(NULL, " \t", &saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line, reading next line", txzpath);
	return;
    }
    if (strchr(p, '/') != NULL) {
	/* found linux output */
	parse_linux_txz_line(p, linep, line_dup, dir_name, txzpath, &saveptr);
    } else {
	/* assume macOS/BSD output */
	parse_bsd_txz_line(p, linep, line_dup, dir_name, txzpath, &saveptr);
    }
}


/*
 * check_tarball - perform tests on tarball, validating it for the IOCCC
 *
 * given:
 *
 *	tar		- path to executable tar program (if -f was not
 *			  specified)
 *	fnamchk		- path to fnamchk tool
 *
 *
 * returns:
 *
 *	number of total number of issues found (txz_info.total_issues).
 *
 * NOTE: Does not return on error.
 */
static unsigned
check_tarball(char const *tar, char const *fnamchk)
{
    unsigned line_num = 0; /* line number of tar output */
    char *cmd = NULL;	/* fnamchk and tar -tJvf */
    FILE *input_stream = NULL; /* pipe for tar output (or if -f specified read as a text file) */
    FILE *fnamchk_stream = NULL; /* pipe for fnamchk output */
    char *linep = NULL;		/* allocated line read from tar (or text file) */
    char *dir_name = NULL;	/* line read from fnamchk (directory name) */
    ssize_t readline_len;	/* readline return length */
    int ret;			/* libc function return */
    int exit_code;		/* shell command exit code */
    bool fnamchk_okay = false;    /* true ==> fnamchk passed */

    /*
     * firewall
     */
    if ((!text_file_flag_used && tar == NULL) || fnamchk == NULL || txzpath == NULL) {
	err(24, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * First of all we have to run fnamchk on the tarball: this is important
     * because we have to know the actual directory name the files should be in
     * within the tarball which we use in checks on the directory (and any
     * additional directories in the tarball).
     */

    /*
     * execute the fnamchk command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = shell_cmd(__func__, true, "% %", fnamchk, txzpath);
    if (exit_code != 0) {
	warnp("txzchk", "%s: %s %s failed with exit code: %d", txzpath, fnamchk, txzpath, WEXITSTATUS(exit_code));
	++txz_info.total_issues;
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
	fnamchk_stream = pipe_open(__func__, true, "% -- %", fnamchk, txzpath);
	if (fnamchk_stream == NULL) {
	    err(25, __func__, "popen for reading failed for: %s -- %s", fnamchk, txzpath);
	    not_reached();
	}

	/*
	 * read the output from the fnamchk command
	 */
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
	    err(26, __func__, "txzchk: %s: unexpected NULL pointer from %s", txzpath, cmd);
	    not_reached();
	}
    }

    /* determine size of tarball */
    txz_info.size = file_size(txzpath);
    /* report size if too big or !quiet */
    if (txz_info.size < 0) {
	err(27, __func__, "%s: impossible error: txzchk_sanity_chks() found tarball but file_size() did not", txzpath);
	not_reached();
    }
    else if (txz_info.size > MAX_TARBALL_LEN) {
	++txz_info.total_issues;
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(28, __func__, "%s: The compressed tarball size %jd > %jd",
		 txzpath, (intmax_t)txz_info.size, (intmax_t)MAX_TARBALL_LEN);
	not_reached();
    }
    else if (!quiet) {
	errno = 0;
	ret = printf("txzchk: %s size of %jd bytes OK\n", txzpath, (intmax_t) txz_info.size);
	if (ret <= 0) {
	    warnp("txzchk", "unable to tell user how big the tarball %s is", txzpath);
	}
    }
    dbg(DBG_MED, "txzchk: %s size in bytes: %jd", txzpath, (intmax_t)txz_info.size);

    /*
     * free cmd for tar (if -f wasn't specified) command
     */
    free(cmd);
    cmd = NULL;

    if (text_file_flag_used) {
	input_stream = fopen(txzpath, "r");
	errno = 0;
	if (input_stream == NULL) {
	    errp(29, __func__, "fopen of %s failed", txzpath);
	    not_reached();
	}
	errno = 0;
	ret = setvbuf(input_stream, (char *)NULL, _IOLBF, 0);
	if (ret != 0) {
	    warnp(__func__, "setvbuf failed for %s", txzpath);
	}

    } else { /* if -f not specified we have to do more to set up input stream */

	/*
	 * execute the tar command
	 */
	errno = 0;			/* pre-clear errno for errp() */
	exit_code = shell_cmd(__func__, true, "% -tJvf %", tar, txzpath);
	if (exit_code != 0) {
	    errp(30, __func__, "%s -tJvf %s failed with exit code: %d",
			      tar, txzpath, WEXITSTATUS(exit_code));
	    not_reached();
	}

	/*
	 * If we get here -f was not specified so open pipe to tar command.
	 */
	input_stream = pipe_open(__func__, true, "% -tJvf %", tar, txzpath);
	if (input_stream == NULL) {
	    err(31, __func__, "popen for reading failed for: %s -tJvf %s",
			      tar, txzpath);
	    not_reached();
	}
    }

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
	    warnp("txzchk", "found NUL before end of line, reading next line");
	    continue;
	}
	dbg(DBG_VHIGH, "line %d: %s", line_num, linep);


	/*
	 * add line to list (to parse once the list of files has been shown to
	 * the user).
	 */
	add_txz_line(linep, line_num);

	if (text_file_flag_used) {
	    errno = 0;
	    ret = printf("%s\n", linep);
	    if (ret <= 0) {
		warnp(__func__, "unable to printf line from text file");
	    }
	}


    } while (readline_len >= 0);
    /*
     * free cmd (it'll be NULL if -f was specified but this is safe)
     */
    free(cmd);
    cmd = NULL;

    /*
     * now parse the lines, reporting any issue that have to be done while parsing
     */
    parse_all_txz_lines(dir_name, txzpath);

    /*
     * check files list and report any additional issues
     */
    check_all_txz_files(dir_name);

    /* free the files list */
    free_txz_files_list();

    /* free txz_lines list */
    free_txz_lines();

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

    return txz_info.total_issues;
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
	err(32, __func__, "called with NULL arg(s)");
	not_reached();
    }

    return str[strspn(str, "-drwx")]!='\0';
}


/*
 * add_txz_line - add line to txz_lines list
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
add_txz_line(char const *str, int line_num)
{
    struct txz_line *line;

    /*
     * firewall
     */
    if (str == NULL) {
	err(33, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    line = calloc(1, sizeof *line);
    if (line == NULL) {
	errp(34, __func__, "unable to allocate struct txz_line *");
	not_reached();
    }

    errno = 0;
    line->line = strdup(str);
    if (line->line == NULL) {
	errp(35, __func__, "unable to strdup string '%s' for lines list", str);
	not_reached();
    }
    line->line_num = line_num;

    dbg(DBG_MED, "adding line %s to lines list", line->line);
    line->next = txz_lines;
    txz_lines = line;
}


/*
 * parse_all_txz_lines - parse txz_lines, reporting any issues found
 *
 * given:
 *
 *	dir_name    - directory name as reported by fnamchk (can be NULL if
 *		      fnamchk failed to validate directory)
 *	txzpath	    - the tarball that is being read
 *
 * This function returns void.
 *
 * This function does not return on error.
 */
static void
parse_all_txz_lines(char const *dir_name, char const *txzpath)
{
    struct txz_line *line = NULL;	/* for txz_lines list */
    char *line_dup = NULL;	/* strdup()d line */
    int dir_count = 0;		/* number of directories detected */

    /*
     * firewall
     */
    if (txzpath == NULL) {
	err(36, __func__, "passed NULL arg");
	not_reached();
    }

    for (line = txz_lines; line != NULL; line = line->next) {
	if (line->line == NULL) {
	    warn("txzchk", "encountered NULL string on line %d", line->line_num);
	    continue;
	}
	line_dup = strdup(line->line);
	if (line_dup == NULL) {
	    err(37, __func__, "%s: duplicating %s failed", txzpath, line->line);
	    not_reached();
	}

	parse_txz_line(line->line, line_dup, dir_name, txzpath, &dir_count);
	free(line_dup);
	line_dup = NULL;
    }
}


/*
 * free_txz_lines - free txz_lines list
 *
 * The purpose of the txz_lines list is so that we can show the list of files in
 * the tarball together without interspersing it with any warnings. Thus we show
 * the files list, adding each line to the list in the process, and then after
 * that we can iterate through the lines and show any warnings. After that we
 * report any issues that haven't been reported yet (some warnings have to be
 * issued while parsing the lines).
 *
 * This function returns void.
 */
static void
free_txz_lines(void)
{
    struct txz_line *line, *next_line;

    for (line = txz_lines; line != NULL; line = next_line) {
	next_line = line->next;
	if (line->line) {
	    free(line->line);
	    line->line = NULL;
	}

	free(line);
	line = NULL;
    }

    txz_lines = NULL;
}


/*
 * alloc_txz_file - allocate a struct txz_file *
 *
 * given:
 *
 *	p	- file path
 *
 * Returns the newly allocated struct txz_file * with the file information. The
 * function does NOT add it to the list!
 *
 * This function does not return on error.
 */
static struct txz_file *
alloc_txz_file(char const *p)
{
    struct txz_file *file; /* the file structure */

    /*
     * firewall
     */
    if (p == NULL) {
	err(38, __func__, "passed NULL path");
	not_reached();
    }
    errno = 0;
    file = calloc(1, sizeof *file);
    if (file == NULL) {
	errp(39, __func__, "%s: unable to allocate a struct txz_file *", txzpath);
	not_reached();
    }

    errno = 0;
    file->filename = strdup(p);
    if (!file->filename) {
	errp(40, __func__, "%s: unable to strdup filename %s", txzpath, p);
	not_reached();
    }

    errno = 0;
    file->basename = strdup(base_name(p)?base_name(p):"");
    if (!file->basename || !strlen(file->basename)) {
	errp(41, __func__, "%s: unable to strdup basename of filename %s", txzpath, p);
	not_reached();
    }

    return file;
}


/*
 * add_txz_file_to_list - add a filename to the linked list
 *
 * given:
 *
 *	file		    - pointer to struct txz_file which should already have the name
 *
 * If the function finds this filename already in the list (basename!) it
 * increments the count and does not add it to the list; else it adds it to the
 * list with a count of 1.
 *
 * This function does not return on error.
 */
static void
add_txz_file_to_list(struct txz_file *txzfile)
{
    struct txz_file *file; /* used to iterate through list to find duplicate files */

    /*
     * firewall
     */
    if (txzfile == NULL || !txzfile->filename || !txzfile->basename) {
	err(42, __func__, "called with NULL pointer(s)");
	not_reached();
    }

    /* always increment total files count */
    ++txz_info.total_files;

    for (file = txz_files; file != NULL; file = file->next)
    {
	if (!strcasecmp(file->basename, txzfile->basename)) {
	    dbg(DBG_MED, "incrementing count of filename %s", txzfile->basename);
	    file->count++;
	    return;
	}
    }
    txzfile->count++;
    /* lazily add to list */
    dbg(DBG_MED, "adding filename %s (basename %s) to list of files", txzfile->filename, txzfile->basename);
    txzfile->next = txz_files;
    txz_files = txzfile;
}


/*
 * free_txz_files_list - free the file linked list
 */
static void
free_txz_files_list(void)
{
    struct txz_file *file, *next_file;

    for (file = txz_files; file != NULL; file = next_file) {
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

    txz_files = NULL;
}
