/*
 * txzchk - IOCCC tarball validation tool
 *
 * txzchk verifies that the tarball does not have any feathers stuck in it (i.e.
 * the tarball conforms to the IOCCC tarball rules). Invoked by mkiocccentry;
 * txzchk in turn uses fnamchk to make sure that the tarball was correctly named
 * and formed. In other words txzchk makes sure that the mkiocccentry tool was
 * used and there was no screwing around with the resultant tarball.
 *
 * Dedicated to:
 *
 *	The many poor souls who have been tarred and feathered:
 *
 *	    "Because sometimes people add feathers to tar." :-(
 *
 *	And to my wonderful Mum and my dear cousin and friend Dani:
 *
 *	    "Because even feathery balls of tar need some love." :-)
 *
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>	/* for strcasecmp() */
#include <ctype.h>
#include <stdint.h>

/*
 * txzchk - IOCCC tarball validation check tool
 */
#include "txzchk.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


int
main(int argc, char **argv)
{
    extern char *optarg;		    /* option argument */
    extern int optind;			    /* argv index of the next arg */
    char *tar = TAR_PATH_0;		    /* path to tar executable that supports the -J (xz) option */
    char *fnamchk = FNAMCHK_PATH_0;	    /* path to fnamchk tool */
    bool fnamchk_flag_used = false;	    /* if -F option used */
    bool tar_flag_used = false;		    /* true ==> -t /path/to/tar was given */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:qVF:t:TE:e")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 3 */
	    usage(2, "-h help mode", program); /*ooo*/
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
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", TXZCHK_VERSION);
	    exit(2); /*ooo*/
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
	case 'T':
	    text_file_flag_used = true; /* don't rely on tar: just read file as if it was a text file */
	    break;
	case 'E':
	    ext = optarg;
	    break;
	case 'e': /* suppress error messages */
	    quiet = true;
	    msg_warn_silent = true;
	    err_output_allowed = false;
	    suppress_error_messages = true;
	    break;
	default:
	    usage(2, "invalid -flag", program); /*ooo*/
	    not_reached();
	}
    }

    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(2, "wrong number of arguments", program); /*ooo*/
	not_reached();
    }
    txzpath = argv[optind];
    dbg(DBG_LOW, "txzpath: %s", txzpath);

    if (!quiet) {
	/*
	 * Welcome
	 */
	print("Welcome to txzchk version: %s\n", TXZCHK_VERSION);
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

    if (!text_file_flag_used) {
	find_utils(tar_flag_used, &tar, false, NULL, false, NULL, false, NULL,
		   fnamchk_flag_used, &fnamchk, false, NULL);
    } else {
	find_utils(false, NULL, false, NULL, false, NULL, false, NULL,
		   fnamchk_flag_used, &fnamchk, false, NULL);
    }

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

    txz_info.total_feathers = check_tarball(tar, fnamchk);
    if (!quiet && !txz_info.total_feathers) {
	para("No feathers stuck in tarball.", "", NULL);
    }
    show_txz_info(txzpath);

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(txz_info.total_feathers != 0); /*ooo*/
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


	dbg(DBG_MED, "%s %s a .info.json", txzpath, has_does_not_have(txz_info.has_info_json));
	dbg(DBG_HIGH, "%s %s an empty .info.json", txzpath, has_does_not_have(txz_info.empty_info_json));
	dbg(DBG_MED, "%s %s a .author.json", txzpath, has_does_not_have(txz_info.has_author_json));
	dbg(DBG_HIGH, "%s %s an empty .author.json", txzpath, has_does_not_have(txz_info.empty_author_json));
	dbg(DBG_MED, "%s %s a prog.c", txzpath, has_does_not_have(txz_info.has_prog_c));
	dbg(DBG_HIGH, "%s %s an empty prog.c", txzpath, has_does_not_have(txz_info.empty_prog_c));
	dbg(DBG_MED, "%s %s a remarks.md", txzpath, has_does_not_have(txz_info.has_remarks_md));
	dbg(DBG_HIGH, "%s %s an empty remarks.md", txzpath, has_does_not_have(txz_info.empty_remarks_md));
	dbg(DBG_MED, "%s %s a Makefile", txzpath, has_does_not_have(txz_info.has_Makefile));
	dbg(DBG_HIGH, "%s %s an empty Makefile", txzpath, has_does_not_have(txz_info.empty_Makefile));
	dbg(DBG_MED, "%s file size is %jd according to stat(2)", txzpath, (intmax_t)txz_info.size);
	dbg(DBG_MED, "%s total files size is %jd", txzpath, (intmax_t)txz_info.file_sizes);
	dbg(DBG_HIGH, "%s shrunk in files size %ju time%s", txzpath, txz_info.files_size_shrunk,
		singular_or_plural(txz_info.files_size_shrunk));
	dbg(DBG_HIGH, "%s went below 0 in all files size %ju time%s", txzpath, txz_info.negative_files_size,
		singular_or_plural(txz_info.negative_files_size));
	dbg(DBG_HIGH, "%s went above max files size %d %ju time%s", txzpath,
		MAX_DIR_KSIZE, (uintmax_t)txz_info.files_size_too_big, singular_or_plural(txz_info.files_size_too_big));
	dbg(DBG_MED, "%s rounded files size is %jd", txzpath, (intmax_t)txz_info.rounded_file_size);
	dbg(DBG_HIGH, "%s rounded files size shrunk %ju time%s", txzpath, (uintmax_t)txz_info.rounded_files_size_shrunk,
		singular_or_plural(txz_info.rounded_files_size_shrunk==1));
	dbg(DBG_HIGH, "%s rounded files size went below 0 %ju time%s", txzpath, (uintmax_t)txz_info.negative_files_size,
		singular_or_plural(txz_info.negative_files_size==1));
	dbg(DBG_HIGH, "%s rounded files size went above max %d %ju time%s", txzpath,
		MAX_DIR_KSIZE, (uintmax_t)txz_info.rounded_files_size_too_big,
		singular_or_plural(txz_info.rounded_files_size_too_big));
	dbg(DBG_MED, "%s has %ju files", txzpath, txz_info.total_files);

	if (txz_info.correct_directory < txz_info.total_files) {
	    dbg(DBG_MED, "%s has %ju incorrect director%s", txzpath, txz_info.total_files - txz_info.correct_directory,
		    txz_info.total_files - txz_info.correct_directory == 1 ? "y":"ies");
	} else {
	    dbg(DBG_MED, "%s has no incorrect directory", txzpath);
	}

	dbg(DBG_MED, "%s has %ju invalid dot file%s", txzpath, txz_info.dot_files, singular_or_plural(txz_info.dot_files));
	dbg(DBG_MED, "%s has %ju file%s named '.'", txzpath, txz_info.named_dot, singular_or_plural(txz_info.named_dot));
	dbg(DBG_MED, "%s has %ju file%s with one or more unsafe chars", txzpath, txz_info.unsafe_chars,
		singular_or_plural(txz_info.unsafe_chars));
	if (txz_info.total_feathers > 0) {
	    dbg(DBG_VHIGH, "%s has %ju feather%s stuck in tarball :-(", txzpath, txz_info.total_feathers,
		    singular_or_plural(txz_info.total_feathers));
	} else {
	    dbg(DBG_VHIGH, "%s has 0 feathers stuck in tarball :-)", txzpath);
	}
    }
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(2, "missing required argument(s), program: %s", program);
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
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, TAR_PATH_0, FNAMCHK_PATH_0, TXZCHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * txzchk_sanity_chks - perform basic sanity checks
 *
 * We perform basic sanity checks on paths as well as some of the IOCCC tables.
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
    if (!text_file_flag_used)
    {
	if (!exists(tar)) {
	    fpara(stderr,
		  "",
		  "We cannot find a tar program.",
		  "",
		  "A tar program that supports the -J (xz) option is required to test the compressed tarball.",
		  "Perhaps you need to use:",
		  "",
		  "    txzchk -t /path/to/tar [...]",
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
		  "The tar, while it exists, is not a regular file.",
		  "",
		  "Perhaps you need to use another path:",
		  "",
		  "    txzchk -t /path/to/tar [...]",
		  "",
		  "and/or install a tar program?  You can find the source for tar:",
		  "",
		  "    https://www.gnu.org/software/tar/",
		  "",
		  NULL);
	    err(5, __func__, "tar is not a regular file: %s", tar);
	    not_reached();
	}

	if (!is_exec(tar)) {
	    fpara(stderr,
		  "",
		  "The tar, while it is a file, is not executable.",
		  "",
		  "We suggest you check the permissions on the tar program, or use another path:",
		  "",
		  "    txzchk -t /path/to/tar [...]",
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
	      "    txzchk -F /path/to/fnamchk [...]",
	      NULL);
	err(7, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }

    if (!is_file(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    txzchk -F /path/to/fnamchk [...]",
	      NULL);
	err(8, __func__, "fnamchk is not a regular file: %s", fnamchk);
	not_reached();
    }

    if (!is_exec(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the fnamchk program, or use another path:",
	      "",
	      "    txzchk -F /path/to/fnamchk [...]",
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
	err(11, __func__, "txzpath is not a regular file: %s", txzpath);
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
 *	dir_name	- the directory name (if fnamchk passed - else NULL)
 *	file		- file structure
 *
 * Report feathers stuck in the current file's path.
 *
 * Returns void. Does not return on error.
 *
 */
static void
check_txz_file(char const *txzpath, char const *dir_name, struct txz_file *file)
{
    bool allowed_dot_file = false;	/* true ==> basename is an allowed '.' file */

    /*
     * firewall
     */
    if (txzpath == NULL || file == NULL || file->basename == NULL || file->filename == NULL) {
	err(13, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * identify if file is an allowed '.' file
     *
     * NOTE: Files that are allowed to begin with '.' must also be lower case.
     * In other words if any of the letters in INFO_JSON_FILENAME or
     * AUTHOR_JSON_FILENAME are upper case the file is an invalid dot file.
     */
    if (!strcmp(file->basename, INFO_JSON_FILENAME) || !strcmp(file->basename, AUTHOR_JSON_FILENAME)) {
	allowed_dot_file = true;
    }

    /*
     * filename must use only POSIX portable filename and + chars plus /
     */
    if (!posix_plus_safe(file->filename, false, true, false)) {
	++txz_info.total_feathers; /* report it once and consider it only one feather */
	++txz_info.unsafe_chars;
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
	 * it's not AUTHOR_JSON_FILENAME and not INFO_JSON_FILENAME then it's an
	 * invalid dot file; if it's ONLY '.' it counts as BOTH an invalid dot
	 * file AND a file called just '.' (which would likely be a directory
	 * but is abuse nonetheless).
	 */
	if (*(file->basename) == '.') {
	    ++txz_info.total_feathers;
	    warn("txzchk", "%s: found non %s and %s dot file %s",
			   txzpath, AUTHOR_JSON_FILENAME, INFO_JSON_FILENAME, file->basename);
	    txz_info.dot_files++;

	    /* check for files called '.' without anything after the dot */
	    if (file->basename[1] == '\0') {
		++txz_info.total_feathers;
		++txz_info.named_dot;
		warn("txzchk", "%s: found file called '.' in path %s", txzpath, file->filename);
	    }
	}

	/*
	 * basename must use only POSIX portable filename and + chars
	 */
	if (!posix_plus_safe(file->basename, false, false, true)) {
	    ++txz_info.total_feathers; /* report it once and consider it only one feather */
	    ++txz_info.unsafe_chars;
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
	if (!strcmp(file->basename, AUTHOR_JSON_FILENAME)) {
	    ++txz_info.total_feathers;
	    warn("txzchk", "%s: found empty %s file", txzpath, AUTHOR_JSON_FILENAME);
	    txz_info.empty_author_json = true;
	} else if (!strcmp(file->basename, INFO_JSON_FILENAME)) {
	    ++txz_info.total_feathers;
	    txz_info.empty_info_json = true;
	    warn("txzchk", "%s: found empty %s file", txzpath, INFO_JSON_FILENAME);
	} else if (!strcmp(file->basename, "remarks.md")) {
	    ++txz_info.total_feathers;
	    txz_info.empty_remarks_md = true;
	    warn("txzchk", "%s: found empty remarks.md", txzpath);
	} else if (!strcmp(file->basename, "Makefile")) {
	    ++txz_info.total_feathers;
	    txz_info.empty_Makefile = true;
	    warn("txzchk", "%s: found empty Makefile", txzpath);
	} else if (!strcmp(file->basename, "prog.c")) {
	    /* this is NOT a feather: it's only for debugging information! */
	    txz_info.empty_prog_c = true;
	}
    }
}

/* convert_file_size	- converts number string to off_t
 *
 * This function is to remove duplicate code as this code is used in more than
 * one function.
 *
 * given:
 *	current_file_size   - pointer to off_t to store the file size in
 *	p		    - the token extracted from strtok_r() (number
 *			      string)
 *
 * Returns: true if conversion succeeded; false if it fails.
 *
 * NOTE: This function does not return on NULL pointers (which should actually
 * never happen).
 */
static bool
convert_file_size(off_t *current_file_size, char *p)
{
    /*
     * firewall
     */
    if (current_file_size == NULL) {
	err(16, __func__, "passed NULL current_file_size");
	not_reached();
    } else if (p == NULL) {
	err(17, __func__, "passed NULL number string");
	not_reached();
    }
    errno = 0;
    *current_file_size = strtoimax(p, NULL, 10);

    /* the conversion fail will be reported in the calling function so we don't do it here */
    if (errno != 0) {
	return false;

    } else if (*current_file_size < 0) {
	warn("txzchk", "%s: file size < 0: %jd", txzpath, (intmax_t)*current_file_size);
	++txz_info.total_feathers;
    }
    txz_info.file_sizes += *current_file_size;

    /* false: check new totals but don't print rounded file size if OK */
    check_txz_files_size(false);

    return true;
}

/* check_txz_files_size	    - check current total file size sum
 *
 * This function is used after each line is parsed. It's important because if a
 * file takes one or both of the sizes (file_sizes or rounded_file_sizes) to >
 * the max or < 0 or 0 then it's an issue that needs to be flagged. We have
 * booleans so that we only warn of this once.
 *
 * given:
 *
 *	show_rounded_size	- true ==> show rounded file size if all is okay
 */
static void
check_txz_files_size(bool show_rounded_size)
{
    /* check total file size */
    if (txz_info.file_sizes < txz_info.previous_files_size) {
	warn("txzchk", "%s: total size of all files < previous size: %jd < %jd",
		txzpath,
		(intmax_t)txz_info.file_sizes,
		(intmax_t) txz_info.previous_files_size);
	++txz_info.total_feathers;
    }
    /* update previous to be the current value */
    txz_info.previous_files_size = txz_info.file_sizes;

    if (txz_info.file_sizes < 0) {
	warn("txzchk", "%s: total file sizes < 0: %jd!", txzpath, (intmax_t)txz_info.file_sizes);
	++txz_info.total_feathers;
	/*
	 * if the previous total size >= 0 we increment the number of times size
	 * < 0
	 */
	if (txz_info.previous_files_size >= 0) {
	    ++txz_info.negative_files_size;
	}
    } else if (txz_info.file_sizes == 0) {
	warn("txzchk", "%s: total size of all files == 0", txzpath);
	++txz_info.total_feathers;
    } else if (txz_info.file_sizes > MAX_DIR_KSIZE) {
	/* only warn first time */
	if (txz_info.files_size_too_big == 0) {
	    warn("txzchk", "%s: total size of files %jd > %d",
		       txzpath, (intmax_t)txz_info.file_sizes,
		       MAX_DIR_KSIZE);
	}

	++txz_info.total_feathers;
	++txz_info.files_size_too_big;
    }

    txz_info.rounded_file_size = round_to_multiple(txz_info.file_sizes, 1024);
    if (txz_info.rounded_file_size < txz_info.previous_rounded_file_size) {
	if (txz_info.rounded_files_size_shrunk == 0) /* only warn first time */
	    warn("txzchk", "%s: total size of all files rounded up to multiple of 1024 < previous size: %jd < %jd",
		    txzpath, (intmax_t)txz_info.rounded_file_size, (intmax_t) txz_info.previous_rounded_file_size);
	++txz_info.total_feathers;
	++txz_info.rounded_files_size_shrunk;
    }

    /* update previous to be the current value */
    txz_info.previous_rounded_file_size = txz_info.rounded_file_size;

    if (txz_info.rounded_file_size < 0) {
	warn("txzchk", "%s: total size of all files rounded up to multiple of 1024 < 0: %jd!",
		txzpath, (intmax_t) txz_info.rounded_file_size);
	++txz_info.total_feathers;
    } else if (txz_info.rounded_file_size == 0) {
	warn("txzchk", "%s: total size of all files rounded up to multiple of 1024 == 0!",
		txzpath);
	++txz_info.total_feathers;
    } else if (txz_info.rounded_file_size > MAX_DIR_KSIZE) {
	if (txz_info.rounded_files_size_too_big == 0) /* only warn first time */
		warn("txzchk", "%s: total size of files %jd rounded up to multiple of 1024 %jd > %d",
		       txzpath, (intmax_t)txz_info.file_sizes, (intmax_t) txz_info.rounded_file_size,
		       MAX_DIR_KSIZE);
	++txz_info.total_feathers;
	++txz_info.rounded_files_size_too_big;
    } else if (!quiet && show_rounded_size) {
	printf("txzchk: %s total size of files %jd rounded up to 1024 multiple: %jd OK\n",
		txzpath, (intmax_t) txz_info.file_sizes, (intmax_t) txz_info.rounded_file_size);
    }
}

/*
 * check_all_txz_files - check txz_files list after parsing tarball (or text file)
 *
 * given:
 *
 *	dir_name	- fnamchk result (if passed - else NULL)
 *
 * Reports any additional feathers stuck in the tarball (or issues in the text
 * file).
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

    check_txz_files_size(true); /* true: show rounded file size if all OK */
    /*
     * Now go through the files list to verify the required files are there and
     * also to detect any additional feathers stuck in the tarball (or issues in
     * the text file).
     */
    for (file = txz_files; file != NULL; file = file->next) {
	if (file->basename == NULL) {
	    err(18, __func__, "found NULL file->basename in txz_files list");
	    not_reached();
	} else if (file->filename == NULL) {
	    err(19, __func__, "found NULL file->filename in txz_files list");
	    not_reached();
	}
	if (!strcmp(file->basename, INFO_JSON_FILENAME)) {
	    txz_info.has_info_json = true;
	} else if (!strcmp(file->basename, AUTHOR_JSON_FILENAME)) {
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
		++txz_info.total_feathers;
	    }
	}

	if (file->count > 1) {
	    warn("txzchk", "%s: found a total of %ju files with the name %s", txzpath, file->count, file->basename);
	    txz_info.total_feathers += file->count - 1;
	}
    }

    /* determine if the required files are there */
    if (!txz_info.has_info_json) {
	++txz_info.total_feathers;
	warn("txzchk", "%s: no .info.json found", txzpath);
    }
    if (!txz_info.has_author_json) {
	++txz_info.total_feathers;
	warn("txzchk", "%s: no .author.json found", txzpath);
    }
    if (!txz_info.has_prog_c) {
	++txz_info.total_feathers;
	warn("txzchk", "%s: no prog.c found", txzpath);
    }
    if (!txz_info.has_Makefile) {
	++txz_info.total_feathers;
	warn("txzchk", "%s: no Makefile found", txzpath);
    }
    if (!txz_info.has_remarks_md) {
	++txz_info.total_feathers;
	warn("txzchk", "%s: no remarks.md found", txzpath);
    }
    if (txz_info.correct_directory < txz_info.total_files) {
	++txz_info.total_feathers;
	warn("txzchk", "%s: not all files in correct directory", txzpath);
    }

    /*
     * Report total number of non .author.json and .info.json dot files.
     * Don't increment the number of feathers as this was done in
     * check_txz_file().
     */
    if (txz_info.dot_files > 0) {
	warn("txzchk", "%s: found a total of %ju unacceptable dot file%s",
		       txzpath, txz_info.dot_files, txz_info.dot_files==1?"":"s");
    }

    /*
     * report total feathers found
     */
    if (txz_info.total_feathers > 0) {
	warn("txzchk", "%s: found %ju feather%s stuck in the tarball",
		       txzpath, txz_info.total_feathers, txz_info.total_feathers==1?"":"s");
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
    uintmax_t dir_count = 0; /* number of directories in the path */
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
	++txz_info.total_feathers;
    }
    if (strstr(file->filename, "..")) /* check for '..' in path */ {
	/*
	 * Note that this check does NOT detect a file in the form of "../.file"
	 * but since the basename of each file is checked in check_txz_file() this
	 * is okay.
	 */
	++txz_info.total_feathers;
	warn("txzchk", "%s: found file with '..' in the path: %s", txzpath, file->filename);
    }
    if (*(file->filename) == '/') {
	++txz_info.total_feathers;
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
	++txz_info.total_feathers;
	warn("txzchk", "%s: found more than one directory in path %s", txzpath, file->filename);
    }

    /*
     * Now we have to run some tests on the directory name which we obtained
     * from fnamchk earlier on - but only if fnamchk did not return an
     * error! If it did we'll report other feathers/issues but we won't check
     * directory names (at least the directory name expected in the
     * tarball).
     */
    if (dir_name != NULL && *dir_name != '\0')
    {
	if (strncmp(file->filename, dir_name, strlen(dir_name))) {
	    warn("txzchk", "%s: found incorrect directory in filename %s", txzpath, file->filename);
	    ++txz_info.total_feathers;
	} else {
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

    for ( ; *p && isdigit(*p) && *p != '/'; ) {
	++p;
    }

    if (*p != '/') {
	warn("txzchk", "found non-numerical UID in line %s", line_dup);
	++txz_info.total_feathers;
	p = strchr(p, '/');
    }
    if (p == NULL) {
	warn("txzchk", "encountered NULL pointer when parsing line %s", line_dup);
	++txz_info.total_feathers;
	return;
    }
    ++p;

    /*
     * now do the same for group id
     */
    for ( ; *p && isdigit(*p); ) {
	++p; /* satisfy frivolous warning (give loop a body instead of having the loop do the actual action) */
    }

    if (*p) {
	warn("txzchk", "found non-numerical GID in file in line %s", line_dup);
	++txz_info.total_feathers;
    }
    p = strtok_r(NULL, " \t", saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++txz_info.total_feathers;
	return;
    }

    if (!convert_file_size(&current_file_size, p)) {
	warnp("txzchk", "%s: trying to parse file size in on line: <%s>: token: <%s>", txzpath, line_dup, p);
	msg("skipping to next line");
	++txz_info.total_feathers;
	return;
    }

    /*
     * the next two fields we don't care about but we loop three times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 3; ++i) {
	p = strtok_r(NULL, " \t", saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line", txzpath);
	    msg("skipping to next line");
	    ++txz_info.total_feathers;
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_txz_file(p);
    if (file == NULL) {
	err(22, __func__, "alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, " \t", saveptr);
	if (p != NULL) {
	    warn("txzchk", "%s: bogus field found after filename: %s", txzpath, p);
	    ++txz_info.total_feathers;
	}
    } while (p != NULL);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_empty_file(txzpath, current_file_size, file);

    /* checks on this specific file */
    check_txz_file(txzpath, dir_name, file);

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
	err(23, __func__, "called with NULL arg(s)");
	not_reached();
    }

    p = strtok_r(NULL, " \t", saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++txz_info.total_feathers;
	return;
    }

    /*
     * attempt to find !isdigit() chars (i.e. the tarball listing includes
     * the owner name of the file).
     */
    for (; p && *p && isdigit(*p); ) {
	++p; /* satisfy frivolous warning (give loop a body instead of having the loop do the actual action) */
    }

    if (*p) {
	warn("txzchk", "%s: found non-numerical UID in file in line %s", txzpath, line_dup);
	++txz_info.total_feathers;
    }

    /*
     * now do the same for group
     */
    p = strtok_r(NULL, " \t", saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++txz_info.total_feathers;
	return;
    }
    for (; p && *p && isdigit(*p); ) {
	++p; /* satisfy frivolous warnings */
    }

    if (*p) {
	warn("txzchk", "%s: found non-numerical GID in file in line: %s", txzpath, line_dup);
	++txz_info.total_feathers;
    }

    p = strtok_r(NULL, " \t", saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++txz_info.total_feathers;
	return;
    }

    if (!convert_file_size(&current_file_size, p)) {
	warnp("txzchk", "%s: trying to parse file size in on line: <%s>: token: <%s>", txzpath, line_dup, p);
	msg("skipping to next line");
	++txz_info.total_feathers;
	return;
    }

    /*
     * the next three fields we don't care about but we loop four times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 4; ++i) {
	p = strtok_r(NULL, " \t", saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line", txzpath);
	    msg("skipping to next line");
	    ++txz_info.total_feathers;
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_txz_file(p);
    if (file == NULL) {
	err(24, __func__, "alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, " \t", saveptr);
	if (p != NULL) {
	    warn("txzchk", "%s: bogus field found after filename: %s", txzpath, p);
	    ++txz_info.total_feathers;
	}
    } while (p != NULL);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_empty_file(txzpath, current_file_size, file);

    /* checks on this specific file */
    check_txz_file(txzpath, dir_name, file);

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
 *  Function updates txz_info.total_feathers, txz_info.file_sizes and dir_count. Returns void.
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
	err(25, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * look for more than one directory
     */
    if (*linep == 'd') {
	++(*dir_count);
	if (*dir_count > 1) {
	    warn("txzchk", "%s: found more than one directory entry: %s", txzpath, linep);
	    ++txz_info.total_feathers;
	}

    /*
     * look for non-directory non-regular non-hard-lined items
     */
    } else if (*linep != '-') {
	warn("txzchk", "%s: found a non-directory non-regular non-hard-lined item: %s", txzpath, linep);
	++txz_info.total_feathers;
    }

    /* extract each field, one at a time, to do various tests */
    p = strtok_r(linep, " \t", &saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++txz_info.total_feathers;
	return;
    }

    if (has_special_bits(p)) {
	warn("txzchk", "%s: found special bits on line: %s", txzpath, line_dup);
	++txz_info.total_feathers;
    }

    /*
     * we have to check this next field for a '/': this will tell us whether to
     * parse it for linux or for macOS/BSD.
     */
    p = strtok_r(NULL, " \t", &saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++txz_info.total_feathers;
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
 *	tar		- path to executable tar program (if -T was not
 *			  specified)
 *	fnamchk		- path to fnamchk tool
 *
 *
 * returns:
 *
 *	total number of feathers/issues found (txz_info.total_feathers).
 *
 * NOTE: Does not return on error.
 */
static uintmax_t
check_tarball(char const *tar, char const *fnamchk)
{
    uintmax_t line_num = 0; /* line number of tar output */
    FILE *input_stream = NULL; /* pipe for tar output (or if -T specified read as a text file) */
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
    if ((!text_file_flag_used && tar == NULL) || fnamchk == NULL || txzpath == NULL)
    {
	err(26, __func__, "called with NULL arg(s)");
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
    if (suppress_error_messages) {
	exit_code = shell_cmd(__func__, true, "% -E % -e -- %", fnamchk, ext, txzpath);

    } else {
	exit_code = shell_cmd(__func__, true, "% -E % -- %", fnamchk, ext, txzpath);
    }

    if (exit_code != 0) {
	warn("txzchk", "%s: %s %s failed with exit code: %d", txzpath, fnamchk, txzpath, WEXITSTATUS(exit_code));
	++txz_info.total_feathers;
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
     * still can detect other feathers/issues; we just won't detect feathers
     * with the entry number and directory.
     */
    if (fnamchk_okay) {

	/*
	 * form pipe to the fnamchk command
	 */
	fnamchk_stream = pipe_open(__func__, true, "% -E % -- %", fnamchk, ext, txzpath);
	if (fnamchk_stream == NULL) {
	    err(27, __func__, "popen for reading failed for: %s -- %s", fnamchk, txzpath);
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

	if (dir_name == NULL || *dir_name == '\0') {
	    err(28, __func__, "txzchk: unexpected NULL pointer from fnamchk -- %s", txzpath);
	    not_reached();
	}
    }

    /* determine size of tarball */
    txz_info.size = file_size(txzpath);
    /* report size if too big or !quiet */
    if (txz_info.size < 0) {
	err(29, __func__, "%s: impossible error: txzchk_sanity_chks() found tarball but file_size() did not", txzpath);
	not_reached();
    } else if (txz_info.size > MAX_TARBALL_LEN) {
	++txz_info.total_feathers;
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(30, __func__, "%s: The compressed tarball size %jd > %jd",
		 txzpath, (intmax_t)txz_info.size, (intmax_t)MAX_TARBALL_LEN);
	not_reached();
    } else if (!quiet) {
	errno = 0;
	ret = printf("txzchk: %s size of %jd bytes OK\n", txzpath, (intmax_t) txz_info.size);
	if (ret <= 0)
	    warnp("txzchk", "unable to tell user how big the tarball %s is", txzpath);
    }
    dbg(DBG_MED, "txzchk: %s size in bytes: %jd", txzpath, (intmax_t)txz_info.size);

    if (text_file_flag_used) {
	input_stream = fopen(txzpath, "r");
	errno = 0;
	if (input_stream == NULL)
	{
	    errp(31, __func__, "fopen of %s failed", txzpath);
	    not_reached();
	}
	errno = 0;
	ret = setvbuf(input_stream, (char *)NULL, _IOLBF, 0);
	if (ret != 0)
	    warnp(__func__, "setvbuf failed for %s", txzpath);

    } else {
	/*
	 * -T was not specified so we have to execute tar: if it fails it's an
	 * error and we abort; else we open a pipe to read the output of the
	 * command.
	 */

	/*
	 * first execute the tar command
	 */
	errno = 0;			/* pre-clear errno for errp() */
	exit_code = shell_cmd(__func__, true, "% -tJvf %", tar, txzpath);
	if (exit_code != 0) {
	    errp(32, __func__, "%s -tJvf %s failed with exit code: %d",
			      tar, txzpath, WEXITSTATUS(exit_code));
	    not_reached();
	}

	/* now open a pipe to tar command (tar -tJvf) to read from */
	input_stream = pipe_open(__func__, true, "% -tJvf %", tar, txzpath);
	if (input_stream == NULL) {
	    err(33, __func__, "popen for reading failed for: %s -tJvf %s",
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
	    warnp("txzchk", "found NUL before end of line");
	    msg("skipping to next line");
	    continue;
	}
	dbg(DBG_VHIGH, "line %ju: %s", line_num, linep);


	/*
	 * add line to list (to parse once the list of files has been shown to
	 * the user).
	 */
	add_txz_line(linep, line_num);

	if (text_file_flag_used) {
	    errno = 0;
	    ret = printf("%s\n", linep);
	    if (ret <= 0)
		warnp(__func__, "unable to printf line from text file");
	}


    } while (readline_len >= 0);

    /*
     * close down pipe
     */
    errno = 0;		/* pre-clear errno for errp() */
    if (text_file_flag_used) {
	ret = fclose(input_stream);
    } else {
	ret = pclose(input_stream);
    }
    if (ret < 0) {
	warnp(__func__, "%s: %s error on tar stream", txzpath, text_file_flag_used?"fclose":"pclose");
    }
    input_stream = NULL;


    /*
     * now parse the lines, reporting any feathers stuck in the tarball that
     * have to be detected while parsing
     */
    parse_all_txz_lines(dir_name, txzpath);

    /*
     * check files list and report any additional feathers stuck in the tarball
     */
    check_all_txz_files(dir_name);

    /* free the files list */
    free_txz_files_list();

    /* free txz_lines list */
    free_txz_lines();

    return txz_info.total_feathers;
}


/*
 * has_special_bits - determine if the file permission has any special bits
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
	err(34, __func__, "called with NULL arg(s)");
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
	err(35, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    line = calloc(1, sizeof *line);
    if (line == NULL) {
	errp(36, __func__, "unable to allocate struct txz_line *");
	not_reached();
    }

    errno = 0;
    line->line = strdup(str);
    if (line->line == NULL) {
	errp(37, __func__, "unable to strdup string '%s' for lines list", str);
	not_reached();
    }
    line->line_num = line_num;

    dbg(DBG_MED, "adding line %s to lines list", line->line);
    line->next = txz_lines;
    txz_lines = line;
}


/*
 * parse_all_txz_lines
 *
 * Parse the txz_lines list and report any feathers stuck in the tarball. After
 * all the lines have been parsed additional tests will be performed.
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
	err(38, __func__, "passed NULL arg");
	not_reached();
    }

    for (line = txz_lines; line != NULL; line = line->next) {
	if (line->line == NULL) {
	    warn("txzchk", "encountered NULL string on line %d", line->line_num);
	    ++txz_info.total_feathers;
	    continue;
	}

	line_dup = strdup(line->line);
	if (line_dup == NULL) {
	    err(39, __func__, "%s: duplicating %s failed", txzpath, line->line);
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
 * report any feathers stuck in the tarball that haven't been reported yet (some
 * warnings have to be issued while parsing the lines so those were already
 * reported).
 *
 * Once the lines are all parsed we must free the list.
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
 *	path	- file path
 *
 * Returns the newly allocated struct txz_file * with the file information. The
 * function does NOT add it to the list!
 *
 * This function does not return on error.
 */
static struct txz_file *
alloc_txz_file(char const *path)
{
    struct txz_file *file; /* the file structure */

    /*
     * firewall
     */
    if (path == NULL) {
	err(40, __func__, "passed NULL path");
	not_reached();
    }
    errno = 0;
    file = calloc(1, sizeof *file);
    if (file == NULL) {
	errp(41, __func__, "%s: unable to allocate a struct txz_file *", txzpath);
	not_reached();
    }

    errno = 0;
    file->filename = strdup(path);
    if (!file->filename) {
	errp(42, __func__, "%s: unable to strdup filename %s", txzpath, path);
	not_reached();
    }

    file->basename = base_name(path);
    if (!file->basename || *(file->basename) == '\0') {
	err(43, __func__, "%s: unable to strdup basename of filename %s", txzpath, path);
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
	err(44, __func__, "called with NULL pointer(s)");
	not_reached();
    }

    /* always increment total files count */
    ++txz_info.total_files;

    for (file = txz_files; file != NULL; file = file->next) {
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
 * free_txz_files_list - free the txz_files linked list
 */
static void
free_txz_files_list(void)
{
    struct txz_file *file, *next_file;

    for (file = txz_files; file != NULL; file = next_file)
    {
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
