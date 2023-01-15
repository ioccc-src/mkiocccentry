/*
 * txzchk - IOCCC tarball validation tool
 *
 * txzchk verifies that the tarball does not have any feathers stuck in it (i.e.
 * the tarball conforms to the IOCCC tarball rules). Invoked by mkiocccentry;
 * txzchk in turn uses fnamchk to make sure that the tarball was correctly named
 * and formed. In other words txzchk makes sure that the mkiocccentry tool was
 * used and there was no screwing around with the resultant tarball.
 *
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Dedicated to:
 *
 *	The many poor souls who have been tarred and feathered:
 *
 *	    "Because sometimes people throw feathers on tar." :-(
 *
 *	And to my wonderful Mum and my dear cousin and friend Dani:
 *
 *	    "Because even feathery balls of tar need some love." :-)
 *
 * Disclaimer:
 *
 *	No pitman or coal mines were harmed in the making of this tool and
 *	neither were any pine trees or birch trees. Whether the fact no coal
 *	mines were harmed is a good or bad thing might be debatable but
 *	nevertheless none were harmed. :-) More importantly, no tar pits -
 *	including the La Brea Tar Pits - were disturbed in the making of this
 *	tool. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>	/* strcasecmp() */
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

static intmax_t sum_check;			/* negative of previous sum */
static intmax_t count_check;			/* negative of previous count */

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
    while ((i = getopt(argc, argv, "hv:qVF:t:TE:w")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 2 */
	    usage(2, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'q':   /* -q - enable quiet mode unless -w specified */
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit 2 */
	    print("%s\n", TXZCHK_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 'F':   /* -F fnamchk - specify path to fnamchk tool */
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 't':   /* -t tar - specify path to tar (perhaps to tar and feather :-) ) */
	    tar = optarg;
	    tar_flag_used = true;
	    break;
	case 'T':
	    read_from_text_file = true; /* -T - text file mode - don't rely on tar: just read file as if it was a text file */
	    break;
	case 'E':   /* -E ext - specify extension (used with -T for test suite) */
	    ext = optarg;
	    break;
	case 'w':   /* -w - always show warnings - important for test suite */
	    always_show_warnings = true;
	    break;
	default:
	    usage(3, "invalid -flag", program); /*ooo*/
	    not_reached();
	}
    }

    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, "wrong number of arguments", program); /*ooo*/
	not_reached();
    }
    txzpath = argv[optind];
    dbg(DBG_LOW, "txzpath: %s", txzpath);

    if (always_show_warnings) {
	warn_output_allowed = true;
	msg_warn_silent = false;
    }

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

    if (!read_from_text_file) {
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

    tarball.total_feathers = check_tarball(tar, fnamchk);
    if (!quiet && !tarball.total_feathers) {
	para("No feathers stuck in tarball.", NULL);
    }
    show_tarball_info(txzpath);

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    if (tarball.total_feathers != 0) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


/*
 * show_tarball_info    - show information about tarball (if verbosity is >= DBG_MED)
 *
 * given:
 *
 *	txzpath	    - path to tarball we checked
 *
 * Returns void. Does not return on error.
 */
static void
show_tarball_info(char const *txzpath)
{
    /*
     * firewall
     */
    if (txzpath == NULL) {
	err(10, __func__, "passed NULL arg");
	not_reached();
    }

    if (verbosity_level >= DBG_MED) {
	/* show information about tarball */
	para("", "The following information about the tarball was collected:", NULL);


	dbg(DBG_MED, "%s %s a .info.json", txzpath, has_does_not_have(tarball.has_info_json));
	dbg(DBG_HIGH, "%s %s an empty .info.json", txzpath, has_does_not_have(tarball.empty_info_json));
	dbg(DBG_HIGH, "%s .info.json size is %jd", txzpath, (intmax_t)tarball.info_json_size);
	dbg(DBG_MED, "%s %s a .auth.json", txzpath, has_does_not_have(tarball.has_auth_json));
	dbg(DBG_HIGH, "%s %s an empty .auth.json", txzpath, has_does_not_have(tarball.empty_auth_json));
	dbg(DBG_HIGH, "%s .auth.json size is %jd", txzpath, (intmax_t)tarball.auth_json_size);
	dbg(DBG_MED, "%s %s a prog.c", txzpath, has_does_not_have(tarball.has_prog_c));
	dbg(DBG_HIGH, "%s %s an empty prog.c", txzpath, has_does_not_have(tarball.empty_prog_c));
	dbg(DBG_HIGH, "%s prog.c size is %jd", txzpath, (intmax_t)tarball.prog_c_size);
	dbg(DBG_MED, "%s %s a remarks.md", txzpath, has_does_not_have(tarball.has_remarks_md));
	dbg(DBG_HIGH, "%s %s an empty remarks.md", txzpath, has_does_not_have(tarball.empty_remarks_md));
	dbg(DBG_HIGH, "%s remarks.md size is %jd", txzpath, (intmax_t)tarball.remarks_md_size);
	dbg(DBG_MED, "%s %s a Makefile", txzpath, has_does_not_have(tarball.has_Makefile));
	dbg(DBG_HIGH, "%s %s an empty Makefile", txzpath, has_does_not_have(tarball.empty_Makefile));
	dbg(DBG_HIGH, "%s Makefile size is %jd", txzpath, (intmax_t)tarball.Makefile_size);
	dbg(DBG_MED, "%s tarball size is %jd according to stat(2)", txzpath, (intmax_t)tarball.size);
	dbg(DBG_MED, "%s total file size is %jd", txzpath, (intmax_t)tarball.files_size);
	dbg(DBG_HIGH, "%s shrunk in files size %ju time%s", txzpath, tarball.files_size_shrunk,
		singular_or_plural(tarball.files_size_shrunk));
	dbg(DBG_HIGH, "%s went below 0 in all files size %ju time%s", txzpath, tarball.negative_files_size,
		singular_or_plural(tarball.negative_files_size));
	dbg(DBG_HIGH, "%s went above max files size %ju %ju time%s", txzpath,
		(uintmax_t)MAX_SUM_FILELEN, (uintmax_t)tarball.files_size_too_big,
		singular_or_plural(tarball.files_size_too_big));
	dbg(DBG_MED, "%s has %ju file%s", txzpath, tarball.total_files-tarball.abnormal_files,
		tarball.total_files-tarball.abnormal_files == 1?"":"s");

	if (tarball.correct_directory < tarball.total_files) {
	    dbg(DBG_MED, "%s has %ju incorrect director%s", txzpath, tarball.total_files - tarball.correct_directory,
		    tarball.total_files - tarball.correct_directory == 1 ? "y":"ies");
	} else {
	    dbg(DBG_MED, "%s has 0 incorrect directories", txzpath);
	}

	dbg(DBG_MED, "%s has %ju invalid dot file%s", txzpath, tarball.invalid_dot_files,
		singular_or_plural(tarball.invalid_dot_files));
	dbg(DBG_MED, "%s has %ju file%s named '.'", txzpath, tarball.named_dot, singular_or_plural(tarball.named_dot));
	dbg(DBG_MED, "%s has %ju file%s with at least one unsafe char", txzpath, tarball.unsafe_chars,
		singular_or_plural(tarball.unsafe_chars));
	if (tarball.total_feathers > 0) {
	    dbg(DBG_VHIGH, "%s has %ju feather%s stuck in tarball :-(", txzpath, tarball.total_feathers,
		    singular_or_plural(tarball.total_feathers));
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
    if ((tar == NULL && !read_from_text_file) || fnamchk == NULL || txzpath == NULL) {
	err(11, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * if text file flag not used tar must be executable
     */
    if (!read_from_text_file)
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
	    err(12, __func__, "tar does not exist: %s", tar);
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
	    err(13, __func__, "tar is not a regular file: %s", tar);
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
	    err(14, __func__, "tar is not an executable program: %s", tar);
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
	err(15, __func__, "fnamchk does not exist: %s", fnamchk);
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
	err(16, __func__, "fnamchk is not a regular file: %s", fnamchk);
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
	err(17, __func__, "fnamchk is not an executable program: %s", fnamchk);
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
	err(18, __func__, "txzpath does not exist: %s", txzpath);
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
	err(19, __func__, "txzpath is not a regular file: %s", txzpath);
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
	err(20, __func__, "txzpath is not readable: %s", txzpath);
	not_reached();
    }

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
 * Report feathers stuck in the current tarball.
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
	err(21, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * identify if file is an allowed '.' file
     *
     * NOTE: Files that are allowed to begin with '.' must also be lower case.
     * In other words if any of the letters in INFO_JSON_FILENAME or
     * AUTH_JSON_FILENAME are upper case the file is an invalid dot file.
     *
     * Yes there's a certain irony that the macro names for these filenames are
     * all upper case and so we're checking for lower case by using upper case
     * but this _is_ part of the IOCCC so why not have a bit of confusion and
     * irony? :-) If this unduly bothers you you can just call the upper case
     * lower case or the lower case upper case but not upper case lower case and
     * lower case upper case! :-) Alternatively you can just deal with the
     * irony.
     */
    if (!strcmp(file->basename, INFO_JSON_FILENAME) || !strcmp(file->basename, AUTH_JSON_FILENAME)) {
	allowed_dot_file = true;
    }

    /*
     * filename must use only POSIX portable filename and + chars plus /
     */
    if (!posix_plus_safe(file->filename, false, true, false)) {
	++tarball.total_feathers; /* report it once and consider it only one feather */
	++tarball.unsafe_chars;
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
	 * it's not AUTH_JSON_FILENAME and not INFO_JSON_FILENAME then it's an
	 * invalid dot file; if it's ONLY '.' it counts as BOTH an invalid dot
	 * file AND a file called just '.' (which would likely be a directory
	 * but is abuse nonetheless).
	 */
	if (*(file->basename) == '.') {
	    ++tarball.total_feathers;
	    warn("txzchk", "%s: found non %s and %s dot file %s",
			   txzpath, AUTH_JSON_FILENAME, INFO_JSON_FILENAME, file->basename);
	    tarball.invalid_dot_files++;

	    /* check for files called '.' without anything after the dot */
	    if (file->basename[1] == '\0') {
		++tarball.total_feathers;
		++tarball.named_dot;
		warn("txzchk", "%s: found file called '.' in path %s", txzpath, file->filename);
	    }
	}

	/*
	 * basename must use only POSIX portable filename and + chars
	 */
	if (!posix_plus_safe(file->basename, false, false, true)) {
	    ++tarball.total_feathers; /* report it once and consider it only one feather */
	    ++tarball.unsafe_chars;
	    warn(__func__, "%s: file basename does not match regexp ^[0-9A-Za-z][0-9A-Za-z._+-]*$: %s",
			   txzpath, file->basename);
	}
    }

    /* check the dirs in the path */
    check_directories(file, dir_name, txzpath);
}


/*
 * check_file_size - if file is required record size and (depending on file) report invalid size
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
check_file_size(char const *txzpath, off_t size, struct txz_file *file)
{
    /*
     * firewall
     */

    if (txzpath == NULL || file == NULL) {
	err(22, __func__, "called with NULL arg(s)");
	not_reached();
    } else if (file->basename == NULL || file->filename == NULL) {
	err(23, __func__, "file->basename == NULL || file->filename == NULL which should never happen");
	not_reached();
    }

    if (size == 0) {
	if (!strcmp(file->basename, AUTH_JSON_FILENAME)) {
	    ++tarball.total_feathers;
	    warn("txzchk", "%s: found empty %s file", txzpath, AUTH_JSON_FILENAME);
	    tarball.empty_auth_json = true;
	} else if (!strcmp(file->basename, INFO_JSON_FILENAME)) {
	    ++tarball.total_feathers;
	    tarball.empty_info_json = true;
	    warn("txzchk", "%s: found empty %s file", txzpath, INFO_JSON_FILENAME);
	} else if (!strcmp(file->basename, "remarks.md")) {
	    ++tarball.total_feathers;
	    tarball.empty_remarks_md = true;
	    warn("txzchk", "%s: found empty remarks.md", txzpath);
	} else if (!strcmp(file->basename, "Makefile")) {
	    ++tarball.total_feathers;
	    tarball.empty_Makefile = true;
	    warn("txzchk", "%s: found empty Makefile", txzpath);
	} else if (!strcmp(file->basename, "prog.c")) {
	    /* this is NOT a feather: it's only for informational purposes! */
	    tarball.empty_prog_c = true;
	}
    } else {
	/* record size of required files for informational purposes */
	if (!strcmp(file->basename, AUTH_JSON_FILENAME)) {
	    tarball.auth_json_size = size;
	} else if (!strcmp(file->basename, INFO_JSON_FILENAME)) {
	    tarball.info_json_size = size;
	} else if (!strcmp(file->basename, "remarks.md")) {
	    tarball.remarks_md_size = size;
	} else if (!strcmp(file->basename, "Makefile")) {
	    tarball.Makefile_size = size;
	} else if (!strcmp(file->basename, "prog.c")) {
	    tarball.prog_c_size = size;
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

    /*
     * Now go through the files list to verify the required files are there and
     * also to detect any additional feathers stuck in the tarball (or issues in
     * the text file).
     */
    for (file = txz_files; file != NULL; file = file->next) {
	if (file->basename == NULL) {
	    err(24, __func__, "found NULL file->basename in txz_files list");
	    not_reached();
	} else if (file->filename == NULL) {
	    err(25, __func__, "found NULL file->filename in txz_files list");
	    not_reached();
	}
	if (!strcmp(file->basename, INFO_JSON_FILENAME)) {
	    tarball.has_info_json = true;
	} else if (!strcmp(file->basename, AUTH_JSON_FILENAME)) {
	    tarball.has_auth_json = true;
	} else if (!strcmp(file->basename, MAKEFILE_FILENAME)) {
	    tarball.has_Makefile = true;
	} else if (!strcmp(file->basename, PROG_C_FILENAME)) {
	    tarball.has_prog_c = true;
	} else if (!strcmp(file->basename, REMARKS_FILENAME)) {
	    tarball.has_remarks_md = true;
	}

	if (dir_name != NULL && tarball.correct_directory) {
	    if (strncmp(file->filename, dir_name, strlen(dir_name))) {
		warn("txzchk", "%s: found directory change in filename %s", txzpath, file->filename);
		++tarball.total_feathers;
	    }
	}

	if (file->count > 1) {
	    warn("txzchk", "%s: found a total of %ju files with the name %s", txzpath, file->count, file->basename);
	    tarball.total_feathers += file->count - 1;
	}
    }

    /* determine if the required files are there */
    if (!tarball.has_info_json) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no .info.json found", txzpath);
    }
    if (!tarball.has_auth_json) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no .auth.json found", txzpath);
    }
    if (!tarball.has_prog_c) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no prog.c found", txzpath);
    }
    if (!tarball.has_Makefile) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no Makefile found", txzpath);
    }
    if (!tarball.has_remarks_md) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no remarks.md found", txzpath);
    }
    if (tarball.correct_directory < tarball.total_files) {
	++tarball.total_feathers;
	warn("txzchk", "%s: not all files in correct directory", txzpath);
    }

    /*
     * Report total number of non .auth.json and .info.json dot files.
     * Don't increment the number of feathers as this was done in
     * check_txz_file().
     */
    if (tarball.invalid_dot_files > 0) {
	warn("txzchk", "%s: found a total of %ju invalidly named dot file%s",
		       txzpath, tarball.invalid_dot_files, tarball.invalid_dot_files==1?"":"s");
    }

    /*
     * report total feathers found
     */
    if (tarball.total_feathers > 0) {
	warn("txzchk", "%s: found %ju feather%s stuck in the tarball",
		       txzpath, tarball.total_feathers, tarball.total_feathers==1?"":"s");
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
 * Issues a warning for every violation specific to directories (and
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
	err(26, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /* check that there is a directory */
    if (strchr(file->filename, '/') == NULL && strcmp(file->filename, ".")) {
	warn("txzchk", "%s: no directory found in filename %s", txzpath, file->filename);
	++tarball.total_feathers;
    }
    if (strstr(file->filename, "..")) /* check for '..' in path */ {
	/*
	 * Note that this check does NOT detect a file in the form of "../.file"
	 * but since the basename of each file is checked in check_txz_file() this
	 * is okay.
	 */
	++tarball.total_feathers;
	warn("txzchk", "%s: found file with '..' in the path: %s", txzpath, file->filename);
    }
    if (*(file->filename) == '/') {
	++tarball.total_feathers;
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
	++tarball.total_feathers;
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
	    ++tarball.total_feathers;
	} else {
	    /* This file is in the right directory */
	    tarball.correct_directory++;
	}
    }
}


/*
 * parse_linux_txz_line - parse linux tar output
 *
 * given:
 *
 *	p		- pointer to current field in line
 *	linep		- the line we're parsing
 *	line_dup	- duplicated line
 *	dir_name	- directory name retrieved from fnamchk or NULL if it failed
 *	txzpath		- the tarball path
 *	saveptr		- pointer to char * to save context between each strtok_r() call
 *	normal_file	- true ==> normal file, check size and number of files
 *	sum		- pointer to sum for sum_and_count() (which we use in count_and_sum())
 *	count		- pointer to count for sum_and_count() (which we use in count_and_sum())
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_txz_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error.
 */
static void
parse_linux_txz_line(char *p, char *linep, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr,
		    bool normal_file, intmax_t *sum, intmax_t *count)
{
    intmax_t length = 0; /* file size */
    struct txz_file *file = NULL;   /* allocated struct of file info */
    int i = 0;
    bool test = false;		    /* tests related to size and max number of files */

    /*
     * firewall
     */
    if (p == NULL || linep == NULL || line_dup == NULL || txzpath == NULL || saveptr == NULL ||
	sum == NULL || count == NULL) {
	err(27, __func__, "called with NULL arg(s)");
	not_reached();
    }

    for ( ; *p && isdigit(*p) && *p != '/'; ) {
	++p;
    }

    if (*p != '/') {
	warn("txzchk", "found non-numerical UID in line %s", line_dup);
	++tarball.total_feathers;
	p = strchr(p, '/');
    }
    if (p == NULL) {
	warn("txzchk", "encountered NULL pointer when parsing line %s", line_dup);
	++tarball.total_feathers;
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
	++tarball.total_feathers;
    }
    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++tarball.total_feathers;
	return;
    }

    test = string_to_intmax2(p, &length);
    if (!test) {
	warn("txzchk", "%s: trying to parse file size in on line: <%s>: token: <%s>", txzpath, line_dup, p);
	++tarball.total_feathers;

	/*
	 * we still have to add to the total number of files before we return to
	 * next line but only if it's a normal file
	 */
	if (normal_file) {
	    count_and_sum(txzpath, sum, count, length);
	}
	msg("skipping to next line due to inability to parse file size");
	return;
    }

    /* add to total number of files and total size if it's a normal file */
    if (normal_file) {
	count_and_sum(txzpath, sum, count, length);
    }

    /*
     * the next two fields we don't care about but we loop three times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 3; ++i) {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line", txzpath);
	    msg("skipping to next line");
	    ++tarball.total_feathers;
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_txz_file(p, length);
    if (file == NULL) {
	err(28, __func__, "alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p != NULL) {
	    warn("txzchk", "%s: bogus field found after filename: %s", txzpath, p);
	    ++tarball.total_feathers;
	}
    } while (p != NULL);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_file_size(txzpath, length, file);

    /* checks on this specific file */
    check_txz_file(txzpath, dir_name, file);

    add_txz_file_to_list(file);
}

/*
 * count_and_sum    - wrapper to sum_and_count (util.c) related checks
 *
 * given:
 *
 *	txzpath		- path to the tarball being checked for feathers
 *	sum		- corresponds to the sum pointer in sum_and_count()
 *	count		- corresponds to the count pointer in sum_and_count()
 *	length		- corresponds to the length in sum_and_count()
 */
static void
count_and_sum(char const *txzpath, intmax_t *sum, intmax_t *count, intmax_t length)
{
    bool test = false;	    /* status of various tests */

    /*
     * firewall
     */
    if (txzpath == NULL) {
	err(29, __func__, "txzpath is NULL");
	not_reached();
    } else if (sum == NULL) {
	err(30, __func__, "sum is NULL");
	not_reached();
    } else if (count == NULL) {
	err(31, __func__, "count is NULL");
	not_reached();
    }
    test = sum_and_count(length, sum, count, &sum_check, &count_check);
    if (!test) {
	/*
	 * sum_and_count() will have reported the issue so we don't report anything
	 * specially. We do however increase the number of feathers.
	 */
	++tarball.total_feathers;
    }

    /* update the tarball files size total */
    tarball.files_size = *sum;

    /* check for negative total file length */
    if (*sum < 0) {
	++tarball.total_feathers;
	++tarball.negative_files_size;
	warn("txzchk", "%s: total file size went below 0: %jd", txzpath, *sum);
	if (*sum < tarball.previous_files_size) {
	    ++tarball.files_size_shrunk;
	    warn("txzchk", "%s: total files size %jd < previous file size %jd", txzpath, *sum,
		    (intmax_t)tarball.previous_files_size);
	}
    }
    /* check for sum of total file lengths being too big */
    if (*sum > MAX_SUM_FILELEN) {
	++tarball.total_feathers;
	++tarball.files_size_too_big;
	warn("txzchk", "%s: total file size too big: %jd > %jd", txzpath, *sum, (intmax_t)MAX_SUM_FILELEN);
    }
    /* update the previous files size */
    tarball.previous_files_size = *sum;

    /* check for no or negative file count */
    if (*count <= 0) {
	++tarball.total_feathers;
	++tarball.invalid_files_count;
	warn("txzchk", "%s: files count <= 0: %jd", txzpath, *count);
    }
    /* check for too many files */
    if (*count - (intmax_t)tarball.abnormal_files > MAX_FILE_COUNT) {
	++tarball.total_feathers;
	++tarball.invalid_files_count;
	warn("txzchk", "%s: too many files: %jd > %jd", txzpath,
		*count - (intmax_t)tarball.abnormal_files, (intmax_t)MAX_FILE_COUNT);
    }
}
/*
 * parse_bsd_txz_line - parse macOS/BSD tar output
 *
 * given:
 *
 *	p		- pointer to current field in line
 *	linep		- the line we're parsing
 *	line_dup	- duplicated line
 *	dir_name	- directory name retrieved from fnamchk or NULL if it failed
 *	txzpath		- the tarball path
 *	saveptr		- pointer to char * to save context between each strtok_r() call
 *	normal_file	- true ==> normal file, check size and number of files
 *	sum		- pointer to sum for sum_and_count() (which we use in count_and_sum())
 *	count		- pointer to count for sum_and_count() (which we use in count_and_sum())
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_txz_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error.
 */
static void
parse_bsd_txz_line(char *p, char *linep, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr,
		    bool normal_file, intmax_t *sum, intmax_t *count)
{
    intmax_t length = 0; /* file size */
    struct txz_file *file = NULL;   /* allocated struct of file info */
    int i = 0;
    bool test = false;		    /* tests related to size and max number of files */

    /*
     * firewall
     */
    if (p == NULL || linep == NULL || line_dup == NULL || txzpath == NULL || saveptr == NULL ||
	sum == NULL || count == NULL) {
	err(32, __func__, "called with NULL arg(s)");
	not_reached();
    }

    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++tarball.total_feathers;
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
	++tarball.total_feathers;
    }

    /*
     * now do the same for group
     */
    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++tarball.total_feathers;
	return;
    }
    for (; p && *p && isdigit(*p); ) {
	++p; /* satisfy frivolous warnings */
    }

    if (*p) {
	warn("txzchk", "%s: found non-numerical GID in file in line: %s", txzpath, line_dup);
	++tarball.total_feathers;
    }

    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++tarball.total_feathers;
	return;
    }

    test = string_to_intmax2(p, &length);
    if (!test) {
	warn("txzchk", "%s: trying to parse file size in on line: <%s>: token: <%s>", txzpath, line_dup, p);
	++tarball.total_feathers;

	/*
	 * we still have to add to the total number of files before we return to
	 * next line but only if it's a normal file
	 */
	if (normal_file) {
	    count_and_sum(txzpath, sum, count, length);
	}
	msg("skipping to next line due to inability to parse file size");
	return;
    }

    /* add to total number of files and total size if it's a normal file */
    if (normal_file) {
	count_and_sum(txzpath, sum, count, length);
    }

    /*
     * the next three fields we don't care about but we loop four times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 4; ++i) {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line", txzpath);
	    msg("skipping to next line");
	    ++tarball.total_feathers;
	    return;
	}
    }

    /* p should now contain the filename. */
    file = alloc_txz_file(p, length);
    if (file == NULL) {
	err(33, __func__, "alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p != NULL) {
	    warn("txzchk", "%s: bogus field found after filename: %s", txzpath, p);
	    ++tarball.total_feathers;
	}
    } while (p != NULL);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_file_size(txzpath, length, file);

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
 *	sum		-   corresponds to sum pointer in sum_and_count()
 *	count		-   corresponds to count pointer in sum_and_count()
 *
 *  Function updates tarball.total_feathers, tarball.files_size and dir_count. Returns void.
 *
 *  Function does not return on error.
 */
static void
parse_txz_line(char *linep, char *line_dup, char const *dir_name, char const *txzpath, int *dir_count,
	       intmax_t *sum, intmax_t *count)
{
    char *p = NULL; /* each field in the line extracted from strtok_r() */
    char *saveptr = NULL; /* for strtok_r() context */
    bool normal_file = false; /* normal file counts against file size and count */

    /*
     * firewall
     */

    if (linep == NULL || line_dup == NULL || txzpath == NULL || dir_count == NULL ||
	    sum == NULL || count == NULL) {
	err(34, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * look for more than one directory
     */
    if (*linep == 'd') {
	++(*dir_count);
	if (*dir_count > 1) {
	    warn("txzchk", "%s: found more than one directory entry: %s", txzpath, linep);
	    ++tarball.total_feathers;
	}
	++tarball.abnormal_files; /* we need this for the sum_and_count() checks on total number of files */
    /*
     * look for non-directory non-regular non-hard-linked items
     */
    } else if (*linep != '-') {
	warn("txzchk", "%s: found a non-directory non-regular non-hard-linked item: %s", txzpath, linep);
	++tarball.total_feathers;
	++tarball.abnormal_files; /* we need this for the sum_and_count() checks on total number of files */
    } else {
	normal_file = true; /* we have to count this as a normal file */
    }

    /* extract each field, one at a time, to do various tests */
    p = strtok_r(linep, tok_sep, &saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++tarball.total_feathers;
	return;
    }

    if (has_special_bits(p)) {
	warn("txzchk", "%s: found special bits on line: %s", txzpath, line_dup);
	++tarball.total_feathers;
    }

    /*
     * we have to check this next field for a '/': this will tell us whether to
     * parse it for linux or for macOS (and presumably BSD).
     */
    p = strtok_r(NULL, tok_sep, &saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", txzpath);
	msg("skipping to next line");
	++tarball.total_feathers;
	return;
    }
    if (strchr(p, '/') != NULL) {
	/* found linux output */
	parse_linux_txz_line(p, linep, line_dup, dir_name, txzpath, &saveptr, normal_file, sum, count);
    } else {
	/* assume macOS/BSD output */
	parse_bsd_txz_line(p, linep, line_dup, dir_name, txzpath, &saveptr, normal_file, sum, count);
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
 *	total number of feathers/issues found (tarball.total_feathers).
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
    if ((!read_from_text_file && tar == NULL) || fnamchk == NULL || txzpath == NULL)
    {
	err(35, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * First of all we have to run fnamchk on the tarball: this is important
     * because we have to know the actual directory name the files should be in
     * within the tarball which we use in checks on the directory (and any
     * additional directories in the tarball).
     */

    /*
     * execute the fnamchk command.
     *
     * We redirect stdout to /dev/null so that the user doesn't have to see the
     * output of the command. We require reading it but the user doesn't need to
     * see it. We could enable it with certain verbosity_level values but this
     * seems like additional code that's only unnecessary. This especially is so
     * because if someone wants the directory name required they can just use
     * fnamchk on it directly with the appropriate options (for example -E txt
     * would tell it to expect the extension txt instead of txz).
     */
    if (dbg_allowed(DBG_HIGH)) {
	dbg(DBG_MED, "about to execute: %s -v 5 -E %s -- %s", fnamchk, ext, txzpath);
	errno = 0;			/* pre-clear errno for errp() */
	exit_code = shell_cmd(__func__, true, "% -v 5 -E % -- %", fnamchk, ext, txzpath);
    } else {
	dbg(DBG_MED, "about to execute: %s -E %s -- %s >/dev/null", fnamchk, ext, txzpath);
	errno = 0;			/* pre-clear errno for errp() */
	exit_code = shell_cmd(__func__, true, "% -E % -- % >/dev/null", fnamchk, ext, txzpath);
    }
    if (exit_code != 0) {
	warn("txzchk", "%s: %s %s failed with exit code: %d", txzpath, fnamchk, txzpath, WEXITSTATUS(exit_code));
	++tarball.total_feathers;
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
	    err(36, __func__, "popen for reading failed for: %s -- %s", fnamchk, txzpath);
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
	errno = 0;		/* pre-clear errno for warnp() */
	ret = pclose(fnamchk_stream);
	if (ret < 0) {
	    warnp(__func__, "%s: pclose error on fnamchk stream", txzpath);
	}

	fnamchk_stream = NULL;

	if (dir_name == NULL || *dir_name == '\0') {
	    err(37, __func__, "txzchk: unexpected NULL pointer from fnamchk -- %s", txzpath);
	    not_reached();
	}
    }

    /* determine size of tarball */
    tarball.size = file_size(txzpath);
    /* report size if too big or !quiet */
    if (tarball.size < 0) {
	err(38, __func__, "%s: impossible error: txzchk_sanity_chks() found tarball but file_size() did not", txzpath);
	not_reached();
    } else if (tarball.size > MAX_TARBALL_LEN) {
	++tarball.total_feathers;
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(39, __func__, "%s: The compressed tarball size %jd > %jd",
		 txzpath, (intmax_t)tarball.size, (intmax_t)MAX_TARBALL_LEN);
	not_reached();
    } else if (!quiet) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = printf("txzchk: %s size of %jd bytes OK\n", txzpath, (intmax_t) tarball.size);
	if (ret <= 0)
	    warnp("txzchk", "unable to tell user how big the tarball %s is", txzpath);
    }
    dbg(DBG_MED, "txzchk: %s size in bytes: %jd", txzpath, (intmax_t)tarball.size);

    if (read_from_text_file) {
	input_stream = fopen(txzpath, "r");
	errno = 0;
	if (input_stream == NULL)
	{
	    errp(40, __func__, "fopen of %s failed", txzpath);
	    not_reached();
	}
	errno = 0;		/* pre-clear errno for warnp() */
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
	    errp(41, __func__, "%s -tJvf %s failed with exit code: %d",
			      tar, txzpath, WEXITSTATUS(exit_code));
	    not_reached();
	}

	/* now open a pipe to tar command (tar -tJvf) to read from */
	input_stream = pipe_open(__func__, true, "% -tJvf %", tar, txzpath);
	if (input_stream == NULL) {
	    err(42, __func__, "popen for reading failed for: %s -tJvf %s",
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
	errno = 0;		/* pre-clear errno for warnp() */
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

	/*
	 * if we're reading from a text file and verbosity level > 0 then we
	 * print the line too.
	 */
	if (read_from_text_file && verbosity_level > 0) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", linep);
	    if (ret <= 0)
		warnp(__func__, "unable to printf line from text file");
	}


    } while (readline_len >= 0);

    /*
     * close down pipe
     */
    errno = 0;		/* pre-clear errno for warnp() */
    if (read_from_text_file) {
	ret = fclose(input_stream);
    } else {
	ret = pclose(input_stream);
    }
    if (ret < 0) {
	warnp(__func__, "%s: %s error on tar stream", txzpath, read_from_text_file?"fclose":"pclose");
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

    return tarball.total_feathers;
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
	err(43, __func__, "called with NULL arg(s)");
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
add_txz_line(char const *str, uintmax_t line_num)
{
    struct txz_line *line;

    /*
     * firewall
     */
    if (str == NULL) {
	err(44, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    line = calloc(1, sizeof *line);
    if (line == NULL) {
	errp(45, __func__, "unable to allocate struct txz_line *");
	not_reached();
    }

    errno = 0;
    line->line = strdup(str);
    if (line->line == NULL) {
	errp(46, __func__, "unable to strdup string '%s' for lines list", str);
	not_reached();
    }
    line->line_num = line_num;

    dbg(DBG_VHIGH, "adding line %s to lines list", line->line);
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
    intmax_t sum = 0;		/* sum for sum_and_count() checks */
    intmax_t count = 0;		/* count for sum_and_count() checks */

    /*
     * firewall
     */
    if (txzpath == NULL) {
	err(47, __func__, "passed NULL arg");
	not_reached();
    }

    for (line = txz_lines; line != NULL; line = line->next) {
	if (line->line == NULL) {
	    warn("txzchk", "encountered NULL string on line %ju", line->line_num);
	    ++tarball.total_feathers;
	    continue;
	}

	line_dup = strdup(line->line);
	if (line_dup == NULL) {
	    err(48, __func__, "%s: duplicating %s failed", txzpath, line->line);
	    not_reached();
	}

	parse_txz_line(line->line, line_dup, dir_name, txzpath, &dir_count, &sum, &count);
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
 *	length	- length of file as calculated by string_to_intmax2 (validating
 *		  size of files will be done later)
 *
 * Returns the newly allocated struct txz_file * with the file information. The
 * function does NOT add it to the list!
 *
 * This function does not return on error.
 */
static struct txz_file *
alloc_txz_file(char const *path, intmax_t length)
{
    struct txz_file *file; /* the file structure */

    /*
     * firewall
     */
    if (path == NULL) {
	err(49, __func__, "passed NULL path");
	not_reached();
    }
    errno = 0;
    file = calloc(1, sizeof *file);
    if (file == NULL) {
	errp(50, __func__, "%s: unable to allocate a struct txz_file *", txzpath);
	not_reached();
    }

    errno = 0;
    file->filename = strdup(path);
    if (!file->filename) {
	errp(51, __func__, "%s: unable to strdup filename %s", txzpath, path);
	not_reached();
    }

    file->basename = base_name(path);
    if (!file->basename || *(file->basename) == '\0') {
	err(52, __func__, "%s: unable to strdup basename of filename %s", txzpath, path);
	not_reached();
    }

    /* also record the length */
    file->length = length;

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
	err(53, __func__, "called with NULL pointer(s)");
	not_reached();
    }

    /* always increment total files count */
    ++tarball.total_files;

    for (file = txz_files; file != NULL; file = file->next) {
	if (!strcasecmp(file->basename, txzfile->basename)) {
	    dbg(DBG_MED, "incrementing count of filename %s", txzfile->basename);
	    file->count++;
	    return;
	}
    }
    txzfile->count++;
    /* lazily add to list */
    dbg(DBG_VHIGH, "adding filename %s (basename %s) to list of files", txzfile->filename, txzfile->basename);
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
