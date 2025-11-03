/*
 * txzchk - IOCCC submission tarball validation tool
 *
 * txzchk verifies that IOCCC submission tarballs conform to the IOCCC rules (no
 * feathers stuck in the tarballs :-) ).
 *
 * txzchk is invoked by mkiocccentry; txzchk in turn uses fnamchk to make
 * sure that the tarball was correctly named and formed. In other words txzchk
 * makes sure that the mkiocccentry tool was used and there was no screwing
 * around with the resultant tarball.
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson.  All Rights Reserved.
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
 * CODY BOONE FERGUSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
 * SHALL CODY BOONE FERGUSON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * This tool was written in 2022-2025 by Cody Boone Ferguson:
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
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
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
#include <sys/wait.h>	/* for WEXITSTATUS() */
#include <locale.h>

/*
 * txzchk - IOCCC tarball validation check tool
 */
#include "txzchk.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */

/*
 * static globals
 */
static intmax_t sum_check;			/* negative of previous sum */
static intmax_t count_check;			/* negative of previous count */
static char const *tarball_path = NULL;		/* the tarball (by path) being checked */
static char const *program = NULL;		/* our name */
static bool read_from_text_file = false;	/* true ==> assume tarball_path refers to a text file */
static char const *ext = "txz";			/* force extension in fnamchk to be this value */
static char const *tok_sep = " \t";		/* token separators for strtok_r */
static bool show_warnings = false;	        /* true ==> show warnings even if -q */
static bool entertain = false;			/* true ==> show entertaining messages */
static uintmax_t feathery = 3;			/* for entertain option */
static bool test_mode = false;                  /* true ==> use -t in fnamchk */


/*
 * txzchk specific structs
 */
static struct txz_line *txz_lines;		/* all of the read lines */
static struct tarball tarball;			/* all the information collected from tarball */
static struct txz_file *txz_files;		/* linked list of the files in the tarball */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-e] [-f feathers] [-w] [-V] [-t tar] [-F fnamchk] [-T] [-E ext] [-x] tarball_path\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level: (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )\n"
    "\t-e\t\tentertainment mode (def: be boring :-) )\n"
    "\t-f feathers\tdefine how many feathers is feathery (for -e)\n"
    "\t-w\t\talways show warning messages\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-t tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-F fnamchk\tpath to tool that checks if tarball_path is a valid compressed tarball\n"
    "\t\t\t    filename (def: %s)\n"
    "\t-T\t\tassume tarball_path is a text file with tar listing (for testing\n"
    "\t\t\t    different formats)\n"
    "\t-E ext\t\tchange extension to test (def: txz)\n"
    "\t-x\t\tforce fnamchk -t even if -T is not used\n"
    "\n"
    "\ttarball_path\tpath to an IOCCC compressed tarball\n"
    "\n"
    "Exit codes:\n"
    "     0   no feathers stuck in tarball :-)\n"
    "     1   tarball was successfully parsed :-) but there's at least one feather stuck in it :-(\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   invalid command line, invalid option or option missing an argument\n"
    " >= 10   internal error has occurred or unknown tar listing format has been encountered\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";


/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));

int
main(int argc, char **argv)
{
    extern char *optarg;		    /* option argument */
    extern int optind;			    /* argv index of the next arg */
    char *tar = TAR_PATH_0;		    /* path to tar executable that supports the -J (xz) option */
    char *fnamchk = FNAMCHK_PATH_0;	    /* path to fnamchk tool */
    int i;
    bool found_tar = false;                     /* for find_utils */
    bool found_fnamchk = false;                 /* for find_utils */
    bool opt_error = false;			/* fchk_inval_opt() return */
    enum path_sanity sanity = PATH_ERR_UNSET;	/* canon_path error or PATH_OK */

    /* IOCCC requires use of C locale */
    set_ioccc_locale();

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:qVF:t:TE:wef:x")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 2 */
	    usage(2, program, ""); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit 2 */
	    print("%s version: %s\n", TXZCHK_BASENAME, TXZCHK_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 'F': /* -F fnamchk - specify path to fnamchk tool */
	    fnamchk = optarg;
	    break;
	case 't': /* -t tar - specify path to tar (perhaps to tar and feather :-) ) */
	    tar = optarg;
	    break;
	case 'T': /* -T - text (test) file mode - don't rely on tar: just read file as if it was a text file */
	    read_from_text_file = true;
	    break;
	case 'E': /* -E ext - specify extension (used with -T for test suite) */
	    ext = optarg;
	    break;
	case 'w': /* -w - always show warnings */
	    show_warnings = true;
	    break;
	case 'e': /* whee! */
	    entertain = true;
	    break;
	case 'f': /* how many feathers is feathery? */
	    if (!string_to_uintmax(optarg, &feathery)) {
		usage(3, program, "invalid -f feathers"); /*ooo*/
		not_reached();
	    }
	    break;
        case 'x':   /* when mkiocccentry UUID is "test" we need this */
            test_mode = true;
            break;
	case ':': /* option requires an argument */
	case '?': /* illegal option */
	default:  /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, program, ""); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, TXZCHK_BASENAME, "is %s: getopt() return: %c optopt: %c", __func__, (char)i, (char)optopt);
	    }
	    break;
	}
    }

    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }
    /* IMPORTANT: canon_path() MUST use a false "lower_case" arg!  See the path_in_item_array() function. */
    tarball_path = canon_path(argv[optind], 0, 0, 0, &sanity, NULL, NULL, false, false, true, true, NULL);
    if (tarball_path == NULL) {
	err(3, program, "bogus tarball path: %s error: %s", argv[optind], path_sanity_error(sanity)); /*ooo*/
	not_reached();
    }
    dbg(DBG_MED, "tarball path: %s", tarball_path);
    dbg(DBG_MED, "fnamchk test mode: %s", booltostr(test_mode));
    dbg(DBG_MED, "entertainment mode: %s", booltostr(entertain));

    /* if -w used then we always show warnings from warn() */
    if (show_warnings) {
	warn_output_allowed = true;
	msg_warn_silent = false;
    }

    if (entertain) {
	/*
	 * Welcome
	 */
	print("Welcome to txzchk version: %s\n", TXZCHK_VERSION);
    }

    /*
     * environment sanity checks
     */
    if (entertain) {
	para("", "Checking if your environment is full of tar ...", NULL);
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

    if (!read_from_text_file) {
        /*
         * we need tar
         */
        find_utils(&found_tar, &tar, NULL, NULL, NULL, NULL, &found_fnamchk, &fnamchk,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    } else {
        /*
         * we do not need tar
         */
        find_utils(NULL, NULL, NULL, NULL, NULL, NULL, &found_fnamchk, &fnamchk,
            NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
    }

    /* additional sanity checks */
    txzchk_sanity_chks(tar, fnamchk);
    if (entertain) {
	para("... environment looks tarry.", NULL);
    }

    /*
     * check the tarball
     */
    if (entertain) {
	para("", "Looking for feathers in tarball ...", NULL);
    }

    tarball.total_feathers = check_tarball(tar, fnamchk);
    if (entertain) {
	if (!tarball.total_feathers) {
	    para("No feathers stuck in tarball.", NULL);
	} else {
	    if (tarball.total_feathers >= feathery) {
		para("\n... looks like someone has been throwing feathers",
		     "about, because that is quite a feathery ball of tar!", NULL);
	    }
	}
    }
    show_tarball_info(tarball_path);

    /*
     * we need to free the paths to the tools
     */
    if (tar != NULL && !read_from_text_file && found_tar) {
        free(tar);
        tar = NULL;
    }
    if (fnamchk != NULL && found_fnamchk) {
        free(fnamchk);
        fnamchk = NULL;
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    if (tarball.total_feathers > 0) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


/*
 * show_tarball_info - show information about tarball (if verbosity is >= DBG_MED)
 *
 * given:
 *
 *	tarball_path	    - path to tarball we checked
 *
 * Returns void. Does not return on error.
 */
static void
show_tarball_info(char const *tarball_path)
{
    /*
     * firewall
     */
    if (tarball_path == NULL) {
	err(10, __func__, "passed NULL tarball path");
	not_reached();
    } else if (*tarball_path == '\0') {
        err(11, __func__, "passed empty tarball path string");
        not_reached();
    }

    if (verbosity_level >= DBG_MED) {
	/* show information about tarball */
	para("", "The following information about the tarball was collected:", NULL);
        dbg(DBG_MED, "tarball size: %lld", (long long)tarball.size);
        dbg(DBG_MED, "total file size: %lld", (long long)tarball.total_size);
        dbg(DBG_MED, "total file size shrunk %lld time%s", (long long)tarball.files_size_shrunk,
                SINGULAR_OR_PLURAL(tarball.files_size_shrunk));
        dbg(DBG_MED, "total number of files with invalid permissions: %lld", (long long)tarball.invalid_perms);
        dbg(DBG_MED, "total number of executable files: %lld\n", (long long)tarball.total_exec_files);
	if (tarball.total_feathers > 0) {
	    dbg(DBG_VHIGH, "%s has %ju feather%s stuck in tarball :-(", tarball_path, tarball.total_feathers,
		    SINGULAR_OR_PLURAL(tarball.total_feathers));
	} else {
	    dbg(DBG_VHIGH, "%s has 0 feathers stuck in tarball :-)", tarball_path);
	}
    }
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(2, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	prog		our program name
 *	str		top level usage message
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(TXZCHK_BASENAME, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }

    if (prog == NULL) {
	prog = TXZCHK_BASENAME;
	warn(TXZCHK_BASENAME, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, TAR_PATH_0, FNAMCHK_PATH_0,
	    TXZCHK_BASENAME, TXZCHK_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
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
 * NOTE: this function does not return on error or if things are not sane.
 */
static void
txzchk_sanity_chks(char const *tar, char const *fnamchk)
{
    /*
     * firewall
     */
    if ((tar == NULL && !read_from_text_file) || fnamchk == NULL || tarball_path == NULL) {
	err(12, __func__, "called with NULL arg(s)");
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
		  "We cannot find tar.",
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
	    err(13, __func__, "tar does not exist: %s", tar);
	    not_reached();
	}

	if (!is_file(tar)) {
	    fpara(stderr,
		  "",
		  "The tar path, while it exists, is not a regular file.",
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
	    err(14, __func__, "tar is not a regular file: %s", tar);
	    not_reached();
	}

	if (!is_exec(tar)) {
	    fpara(stderr,
		  "",
		  "The tar path, while it is a file, is not executable.",
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
	    err(15, __func__, "tar is not an executable program: %s", tar);
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
	err(16, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }

    if (!is_file(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk path, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    txzchk -F /path/to/fnamchk [...]",
	      NULL);
	err(17, __func__, "fnamchk is not a regular file: %s", fnamchk);
	not_reached();
    }

    if (!is_exec(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk path, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the fnamchk program, or use another path:",
	      "",
	      "    txzchk -F /path/to/fnamchk [...]",
	      NULL);
	err(18, __func__, "fnamchk is not an executable program: %s", fnamchk);
	not_reached();
    }


    /*
     * tarball_path must be readable
     */
    if (!exists(tarball_path)) {
	fpara(stderr,
	      "",
	      "The tarball path specified does not exist. Perhaps you made a typo?",
	      "Please check the path and try again."
	      "",
	      "    txzchk [options] <tarball_path>"
	      "",
	      NULL);
	err(19, __func__, "tarball_path does not exist: %s", tarball_path);
	not_reached();
    }

    if (!is_file(tarball_path)) {
	fpara(stderr,
	      "",
	      "The file specified, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    txzchk [...] <tarball_path>",
	      "",
	      NULL);
	err(20, __func__, "tarball_path is not a regular file: %s", tarball_path);
	not_reached();
    }

    if (!is_read(tarball_path)) {
	fpara(stderr,
	      "",
	      "The tarball path, while it is a file, is not readable.",
	      "",
	      "We suggest you check the permissions on the path or use another path:",
	      "",
	      "    txzchk [...] <tarball_path>"
	      "",
	      NULL);
	err(21, __func__, "tarball_path is not readable: %s", tarball_path);
	not_reached();
    }

    return;
}



/*
 * check_all_txz_files - check txz_files list after parsing tarball (or text file)
 *
 * This function does a few sanity checks but primarily it uses the walk code,
 * recording the step of each file and verifying everything is okay.
 *
 * Returns void. Ignores empty files (though these should not be in the list at
 * all).
 *
 * Does not return on NULL filenames or basenames (neither of which should ever
 * happen).
 */
static void
check_all_txz_files(void)
{
    struct txz_file *file;  /* to iterate through files list */
    struct walk_stat wstat; /* record information and stats about a complete walk */
    bool process = false;   /* true ==> process item, false ==> ignore item */
    bool dup = false;       /* true ==> attempt to record a duplicate canonical path */
    bool walk_ok = false;    /* true ==> no walk errors found, false ==> some walk errors found */
    char const *path = NULL;    /* a path from a tarball line listing */
    char const *cpath = NULL;       /* canonicalised path arg as a string */
    enum path_sanity sanity = PATH_ERR_UNSET;       /* canonicalize_path() error code, or PATH_OK */
    size_t path_len = 0;            /* canonicalised path length */
    int_least32_t deep = -1;        /* canonicalised stack depth */

    /*
     * init walk code
     */
    memset(&wstat, 0, sizeof(wstat));
    init_walk_stat(&wstat, ".", &walk_txzchk, TXZCHK_BASENAME, MAX_PATH_LEN, MAX_FILENAME_LEN, MAX_PATH_DEPTH, true);

    /*
     * Now go through the files list to verify the required files are there and
     * also to detect any additional feathers stuck in the tarball (or issues in
     * the text file).
     */
    for (file = txz_files; file != NULL; file = file->next) {
	if (file->basename == NULL) {
	    err(22, __func__, "found NULL file->basename in txz_files list");
	    not_reached();
	} else if (file->filename == NULL) {
	    err(23, __func__, "found NULL file->filename in txz_files list");
	    not_reached();
        }
        check_directory(file, file->dirname, tarball_path);

        /*
         * first find the first '/'. If NULL warn about it and skip file
         */
        path = strchr(file->filename, '/');
        if (path == NULL) {
            warn(TXZCHK_BASENAME, "no directory found in filename: %s", file->filename);
            ++tarball.total_feathers;
            continue;
        } else {
            /*
             * first canonicalise the path
             */
            cpath = canonicalize_path(&wstat, path + 1, &sanity, &path_len, &deep);
            if (cpath == NULL) {
                warn(TXZCHK_BASENAME, "canonicalize_path had an internal failure and returned NULL");
                ++tarball.total_feathers;
                continue;
            }
        }
        /* process the path, size, and st_mode from the tarball listing line */
        process = record_step(&wstat, cpath, file->length, file->mode, &dup, NULL);
        if (dup) {
            warn(TXZCHK_BASENAME, "file %s is a duplicate file", cpath != NULL?cpath:file->filename);
            ++tarball.total_feathers;
        } else if (process) {
            dbg(DBG_MED, "txzchk: file %s has been successfully processed", cpath!=NULL?cpath:file->filename);
        }
        if (cpath != NULL) {
            free((void *)cpath);
            cpath = NULL;
        }
    }

    walk_ok = chk_walk(&wstat, stderr, MAX_EXTRA_FILE_COUNT, MAX_EXTRA_DIR_COUNT,
                       NO_COUNT, NO_COUNT, true);
    if (walk_ok) {
        dbg(DBG_MED, "all okay walking tarball: %s", tarball_path);
    } else {
        if (entertain) {
            warn(TXZCHK_BASENAME, "encountered one or more feathers in tar pit: %s", tarball_path);
        } else {
            warn(TXZCHK_BASENAME, "encountered an error walking directory in tarball: %s", tarball_path);
        }
        ++tarball.total_feathers;
    }

    /*
     * free the walk_stat
     */
    free_walk_stat(&wstat);

    if (tarball.total_size > MAX_SUM_FILELEN) {
        warn(TXZCHK_BASENAME, "total length of tarball contents is too long: %lld > limit: %lld", (long long)tarball.total_size,
            (long long) MAX_SUM_FILELEN);
        ++tarball.total_feathers;
    } else {
        dbg(DBG_LOW, "total length of tarball size: %lld <= limit: %lld", (long long)tarball.total_size,
            (long long) MAX_SUM_FILELEN);
    }

    /*
     * report total feathers found
     */
    if (tarball.total_feathers > 0) {
	warn(TXZCHK_BASENAME, "%s: found %ju feather%s stuck in the tarball",
	    tarball_path, tarball.total_feathers, tarball.total_feathers==1?"":"s");
    }
}

/*
 * check_directory - directory specific checks on this _file_
 *
 * given:
 *
 *	file		- file structure
 *	dirname	        - the directory expected (or NULL if fnamchk fails)
 *	tarball_path	- the tarball path
 *
 * Issues a warning if the expected (if fnamchk did not fail i.e. dirname !=
 * NULL) directory name in the file is not correct (i.e. the top level directory
 * != dirname). If fnamchk did fail it is also warned against.
 *
 * Does not return on error.
 */
static void
check_directory(struct txz_file *file, char const *dirname, char const *tarball_path)
{
    /*
     * firewall
     */
    if (tarball_path == NULL || file == NULL || file->filename == NULL) {
	err(24, __func__, "passed NULL arg(s)");
	not_reached();
    }

    if (dirname != NULL && *dirname != '\0')
    {
	if (strncmp(file->filename, dirname, strlen(dirname))) {
	    warn(TXZCHK_BASENAME, "%s: found incorrect top level directory in filename %s", tarball_path, file->filename);
	    ++tarball.total_feathers;
	} else {
	    /* This file has the right top level directory */
            dbg(DBG_HIGH, "%s: correct directory %s for file %s", tarball_path, dirname, file->filename);
	}
    } else if (!test_mode) {
        warn(TXZCHK_BASENAME, "%s: found incorrect top level directory in filename %s", tarball_path, file->filename);
        ++tarball.total_feathers;
    }
}

/*
 * count_and_sum - wrapper to sum_and_count (util.c) related checks
 *
 * given:
 *
 *	tarball_path	- path to the tarball being checked for feathers
 *	sum		- corresponds to the sum pointer in sum_and_count()
 *	count		- corresponds to the count pointer in sum_and_count()
 *	length		- corresponds to the length in sum_and_count()
 */
static void
count_and_sum(char const *tarball_path, intmax_t *sum, intmax_t *count, intmax_t length)
{
    bool test = false;	    /* status of various tests */

    /*
     * firewall
     */
    if (tarball_path == NULL) {
	err(25, __func__, "tarball_path is NULL");
	not_reached();
    } else if (sum == NULL) {
	err(26, __func__, "sum is NULL");
	not_reached();
    } else if (count == NULL) {
	err(27, __func__, "count is NULL");
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
    tarball.total_size = *sum;

    /* check for negative total file length */
    if (*sum < 0) {
	++tarball.total_feathers;
	++tarball.negative_files_size;
	warn(TXZCHK_BASENAME, "%s: total file size went below 0: %jd", tarball_path, *sum);
	if (*sum < tarball.previous_files_size) {
	    ++tarball.files_size_shrunk;
	    warn(TXZCHK_BASENAME, "%s: total files size %jd < previous file size %lld", tarball_path, *sum,
		(long long)tarball.previous_files_size);
	}
    }
    /* check for sum of total file lengths being too big */
    if (*sum > MAX_SUM_FILELEN) {
        if (tarball.previous_files_size <= MAX_SUM_FILELEN) {
            ++tarball.total_feathers;
            ++tarball.files_size_too_big;
        }
	warn(TXZCHK_BASENAME, "%s: total file size too big: %jd > %jd", tarball_path,
	    *sum, (intmax_t)MAX_SUM_FILELEN);
    }
    /* update the previous files size */
    tarball.previous_files_size = *sum;

    /* check for no or negative file count */
    if (*count <= 0) {
	++tarball.total_feathers;
	++tarball.invalid_files_count;
	warn(TXZCHK_BASENAME, "%s: files count <= 0: %jd", tarball_path, *count);
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
 *	dirname	        - directory name retrieved from fnamchk or NULL if it failed
 *	tarball_path	- the tarball path
 *	saveptr		- pointer to char * to save context between each strtok_r() call
 *	isfile	        - true ==> normal file, check size and number of files
 *	sum		- pointer to sum for sum_and_count() (which we use in count_and_sum())
 *	count		- pointer to count for sum_and_count() (which we use in count_and_sum())
 *	isdir           - true ==> is a directory
 *	perms           - permission line of file
 *	isexec          - if executable bit (+x) found in permissions
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_txz_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error although the word error is used
 * loosely here.
 */
static void
parse_linux_txz_line(char *p, char *linep, char *line_dup, char const *dirname,
	char const *tarball_path, char **saveptr, bool isfile, intmax_t *sum, intmax_t *count,
        bool isdir, char *perms, bool isexec)
{
    intmax_t length = 0; /* file size */
    struct txz_file *file = NULL;   /* allocated struct of file info */
    int i = 0;
    bool test = false;		    /* tests related to size and max number of files */

    /*
     * firewall
     */
    if (p == NULL || linep == NULL || line_dup == NULL || tarball_path == NULL || saveptr == NULL ||
	sum == NULL || count == NULL || perms == NULL) {
	err(28, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /* verify that this is a UID, not a username */
    for ( ; *p && isascii(*p) && isdigit(*p) && *p != '/'; ) {
	++p;
    }

    if (*p != '/') {
	warn(TXZCHK_BASENAME, "found non-numerical UID in line %s", line_dup);
	++tarball.total_feathers;
	p = strchr(p, '/');
    }
    if (p == NULL) {
	err(29, __func__, "txzchk: encountered NULL pointer when parsing line %s", line_dup);
        not_reached();
    }
    ++p;

    /*
     * now do the same for group id
     */
    for ( ; *p && isascii(*p) && isdigit(*p); ) {
	++p; /* satisfy frivolous warning (give loop a body instead of having the loop do the actual action) */
    }

    if (*p) {
	warn(TXZCHK_BASENAME, "found non-numerical GID in file in line %s", line_dup);
	++tarball.total_feathers;
    }
    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	err(30, __func__, "%s: NULL pointer encountered trying to parse line", tarball_path);
	not_reached();
    }

    test = string_to_intmax(p, &length);
    if (!test) {
	warn(TXZCHK_BASENAME, "%s: trying to parse file size in on line: <%s>: token: <%s>", tarball_path, line_dup, p);
	++tarball.total_feathers;

	/*
	 * we still have to add to the total number of files before we return to
	 * next line but only if it's a normal file
	 */
	if (isfile) {
	    count_and_sum(tarball_path, sum, count, length);
	}
	if (verbosity_level) {
	    msg("skipping to next line due to inability to parse file size");
	}
	return;
    } else if (length < 0) {
        ++tarball.total_feathers;
        warn(TXZCHK_BASENAME, "in tarball: %s: length %lld < 0", tarball_path, (long long)length);
    } else if (isfile) {
        /* add to total number of files and total size if it's a normal file */
	count_and_sum(tarball_path, sum, count, length);
    }

    /*
     * the next two fields we don't care about but we loop three times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 3; ++i) {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p == NULL) {
            err(31, __func__, "%s: NULL pointer trying to parse line", tarball_path);
            not_reached();
	}
    }

    /* p should now contain the filename. */
    file = alloc_txz_file(p, dirname, perms, isdir, isfile, isexec, length);
    if (file == NULL) {
	err(32, __func__, "alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p != NULL) {
	    warn(TXZCHK_BASENAME, "%s: bogus field found after filename: %s", tarball_path, p);
	    ++tarball.total_feathers;
	}
    } while (p != NULL);

    /* checks on this specific file */
    check_txz_file(tarball_path, dirname, file);

    add_txz_file_to_list(file);
}


/*
 * parse_bsd_txz_line - parse macOS/BSD tar output
 *
 * given:
 *
 *	p		- pointer to current field in line
 *	linep		- the line we're parsing
 *	line_dup	- duplicated line
 *	dirname	        - directory name retrieved from fnamchk or NULL if it failed
 *	tarball_path	- the tarball path
 *	saveptr		- pointer to char * to save context between each strtok_r() call
 *	isfile	        - true ==> normal file, check size and number of files
 *	sum		- pointer to sum for sum_and_count() (which we use in count_and_sum())
 *	count		- pointer to count for sum_and_count() (which we use in count_and_sum())
 *	isdir           - true ==> is a directory
 *	perms           - permission string
 *	isexec          - executable bit found (+x)
 *
 * If everything goes okay the line will be completely parsed and the calling
 * function (parse_txz_line()) will return to its caller (parse_all_lines()) which
 * will in turn read the next line.
 *
 * This function does not return on error although the word error is used
 * loosely here.
 */
static void
parse_bsd_txz_line(char *p, char *linep, char *line_dup, char const *dirname,
	char const *tarball_path, char **saveptr,
		    bool isfile, intmax_t *sum, intmax_t *count, bool isdir,
                    char *perms, bool isexec)
{
    intmax_t length = 0; /* file size */
    struct txz_file *file = NULL;   /* allocated struct of file info */
    int i = 0;
    bool test = false;		    /* tests related to size and max number of files */

    /*
     * firewall
     */
    if (p == NULL || linep == NULL || line_dup == NULL || tarball_path == NULL || saveptr == NULL ||
	sum == NULL || count == NULL) {
	err(33, __func__, "called with NULL arg(s)");
	not_reached();
    }

    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
        err(34, __func__, "txzchk: %s: NULL pointer encountered trying to parse line", tarball_path);
        not_reached();
    }

    /*
     * attempt to find !isdigit() chars (i.e. verify the tarball listing includes
     * the UID, not the user name).
     */
    for (; p && *p && isascii(*p) && isdigit(*p); ) {
	++p; /* satisfy frivolous warning (give loop a body instead of having the loop do the actual action) */
    }

    if (*p) {
	warn(TXZCHK_BASENAME, "%s: found non-numerical UID in file in line %s", tarball_path, line_dup);
	++tarball.total_feathers;
    }

    /*
     * now do the same for group
     */
    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
        err(35, __func__, "txzchk: %s: NULL pointer encountered trying to parse line", tarball_path);
        not_reached();
    }

    /*
     * attempt to find !isdigit() chars (i.e. verify the tarball listing includes
     * the GID of the file, not group name).
     */
    for (; p && *p && isascii(*p) && isdigit(*p); ) {
	++p; /* satisfy frivolous warnings */
    }

    if (*p) {
	warn(TXZCHK_BASENAME, "%s: found non-numerical GID in file in line: %s", tarball_path, line_dup);
	++tarball.total_feathers;
    }

    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	err(36, __func__, "txzchk: %s: NULL pointer encountered trying to parse line", tarball_path);
        not_reached();
    }

    test = string_to_intmax(p, &length);
    if (!test) {
	warn(TXZCHK_BASENAME, "%s: trying to parse file size in on line: <%s>: token: <%s>", tarball_path, line_dup, p);
	++tarball.total_feathers;

	/*
	 * we still have to add to the total number of files before we return to
	 * next line but only if it's a normal file
	 */
	if (isfile) {
	    count_and_sum(tarball_path, sum, count, length);
	}
	if (verbosity_level) {
	    msg("skipping to next line due to inability to parse file size");
	}
	return;
    } else if (length < 0) {
        ++tarball.total_feathers;
        warn(TXZCHK_BASENAME, "in tarball: %s: length %lld < 0", tarball_path, (long long)length);
    } else if (isfile) {
        /* add to total number of files and total size if it's a normal file */
	count_and_sum(tarball_path, sum, count, length);
    }

    /*
     * the next three fields we don't care about but we loop four times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 4; ++i) {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p == NULL) {
	    err(37, __func__, "txzchk: %s: NULL pointer trying to parse line", tarball_path);
            not_reached();
	}
    }
    /* p should now contain the filename. */
    file = alloc_txz_file(p, dirname, perms, isdir, isfile, isexec, length);
    if (file == NULL) {
	err(38, __func__, "txzchk: alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p != NULL) {
	    warn(TXZCHK_BASENAME, "%s: bogus field found after filename: %s", tarball_path, p);
	    ++tarball.total_feathers;
	}
    } while (p != NULL);

    /* checks on this specific file */
    check_txz_file(tarball_path, dirname, file);

    add_txz_file_to_list(file);
}

/*
 * check_txz_file - checks on the current filename only
 *
 * given:
 *
 *	tarball_path	- the tarball (or text file) we're processing
 *	dirname	- the directory name (if fnamchk passed - else NULL)
 *	file		- txz_file structure
 *
 * Report feathers stuck in the current tarball.
 *
 * Returns void. Does not return on error.
 *
 */
static void
check_txz_file(char const *tarball_path, char const *dirname, struct txz_file *file)
{
    /*
     * firewall
     */
    if (tarball_path == NULL || file == NULL || file->basename == NULL || file->filename == NULL) {
	err(39, __func__, "passed NULL arg(s)");
	not_reached();
    }

    if (has_special_bits(file) || file->mode == 0) {
        dbg(DBG_MED, "file %s: has invalid perms: %s", file->filename, file->perms);
        ++tarball.total_feathers;
    }

    /* check the dirs in the path */
    check_directory(file, dirname, tarball_path);
}



/*
 * has_special_bits - determine if the file permission has any special bits
 *
 * given:
 *
 *	file	    - the file (as a struct txz_file) to test
 *
 * This function does not return on NULL pointers (file itself or required
 * pointers in the struct).
 */
static bool
has_special_bits(struct txz_file *file)
{
    /*
     * firewall
     */
    if (file == NULL) {
	err(40, __func__, "called with NULL file");
	not_reached();
    }
    if (file->filename == NULL) {
        err(41, __func__, "file->filename is NULL");
        not_reached();
    }
    if (file->perms == NULL) {
        err(42, __func__, "file->perms is NULL");
        not_reached();
    }

    if (file->isdir) {
        if (strcmp(file->perms, "drwxr-xr-x") != 0) {
	    warn(TXZCHK_BASENAME, "directory with incorrect permissions found: %s: %s != drwxr-xr-x", file->filename, file->perms);
            ++tarball.invalid_perms;
            return true;
        }
    } else if (is_executable_filename(file->basename)) {
        ++tarball.total_exec_files;
        if (strcmp(file->perms, "-r-xr-xr-x") != 0) {
            warn(TXZCHK_BASENAME, "found executable filename %s that does not match mode 0555: %s != -r-xr-xr-x",
                    file->filename, file->perms);
            /*
             * NOTE: the caller will increment the tarball.total_feathers so do
             * NOT do it here.
             */
            return true;
        }
    /*
     * if we get here we have to verify that the file is a specific
     * permission too, namely read only (-r--r--r--).
     */
    } else if (strcmp(file->perms, "-r--r--r--") != 0) {
        warn(TXZCHK_BASENAME, "found non-executable non-directory file %s with wrong permissions: %s != -r--r--r-- (0444)",
                file->filename, file->perms);
        ++tarball.invalid_perms;
        /*
         * NOTE: the caller will increment the tarball.total_feathers so do
         * NOT do it here.
         */
        return true;
    }

    /*
     * all good
     */
    return false;
}
/*
 * parse_txz_line - parse a line in the tarball listing
 *
 * given:
 *
 *	linep		-   line to parse
 *	line_dup	-   pointer to the duplicated line
 *	dirname	-   the directory name reported by fnamchk or NULL if it failed
 *	tarball_path	-   the tarball path
 *	sum		-   corresponds to sum pointer in sum_and_count()
 *	count		-   corresponds to count pointer in sum_and_count()
 *
 *  Function updates tarball.total_feathers, tarball.files_size and dir_count. Returns void.
 *
 *  Function does not return on error.
 */
static void
parse_txz_line(char *linep, char *line_dup, char const *dirname, char const *tarball_path, intmax_t *sum, intmax_t *count)
{
    char *p = NULL; /* each field in the line extracted from strtok_r() */
    char *saveptr = NULL; /* for strtok_r() context */
    bool isfile = false; /* normal file counts against file size and count */
    bool isdir = false;       /* if it's a directory */
    bool isexec = false;      /* if executable bit found */
    char *perms = NULL;     /* permissions string */

    /*
     * firewall
     */
    if (linep == NULL || line_dup == NULL || tarball_path == NULL ||
	    sum == NULL || count == NULL) {
	err(43, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * look for non-directory non-regular non-hard-linked items
     */
    if (*linep != '-' && *linep != 'd') {
	warn(TXZCHK_BASENAME, "%s: found a non-directory non-regular non-hard-linked item: %s",
	    tarball_path, linep);
	++tarball.total_feathers;
    } else {
        /*
         * record if it's a directory or not
         */
        if (*linep == 'd') {
            isdir = true;
        } else {
            isfile = true; /* we have to count this as a normal file */
        }
    }

    /* extract each field, one at a time, to do various tests */
    p = strtok_r(linep, tok_sep, &saveptr);
    if (p == NULL) {
	err(44, __func__, "txzchk: %s: NULL pointer encountered trying to parse line", tarball_path);
        not_reached();
    }

    /*
     * save permission bits
     */
    errno = 0;      /* pre-clear errno for errp() */
    perms = strdup(p);
    if (perms == NULL) {
        errp(45, __func__, "txzchk: failed to strdup permissions string");
        not_reached();
    }

    /*
     * now that we have the permissions string we have to check for +x bit.
     *
     * We make the assumption that it's in the right place because it does not
     * actually matter. If it has any executable bit we check for a specific
     * permission string in the has_special_bits() function and if it does not
     * match or it's not a file that is allowed to be +x then it is an error.
     */
    if (strchr(perms, 'x') != NULL) {
        isexec = true;
    } else {
        isexec = false;
    }


    /*
     * we have to check this next field for a '/': this will tell us whether to
     * parse it for linux or for macOS (and presumably BSD).
     */
    p = strtok_r(NULL, tok_sep, &saveptr);
    if (p == NULL) {
	err(46, __func__, "txzchk: %s: NULL pointer encountered trying to parse line", tarball_path);
        not_reached();
    }
    if (strchr(p, '/') != NULL) {
	/* found linux output */
	parse_linux_txz_line(p, linep, line_dup, dirname, tarball_path, &saveptr, isfile, sum, count, isdir, perms, isexec);
    } else {
	/* assume macOS/BSD output */
	parse_bsd_txz_line(p, linep, line_dup, dirname, tarball_path, &saveptr, isfile, sum, count, isdir, perms, isexec);
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
    char *dirname = NULL;	/* line read from fnamchk (directory name) */
    ssize_t readline_len;	/* readline return length */
    int ret;			/* libc function return */
    int exit_code;		/* shell command exit code */
    bool fnamchk_okay = false;    /* true ==> fnamchk passed */

    /*
     * firewall
     */
    if ((!read_from_text_file && tar == NULL) || fnamchk == NULL || tarball_path == NULL)
    {
	err(47, __func__, "called with NULL arg(s)");
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
     *
     */
    if (read_from_text_file) {
        /*
         * In test mode (-T) we must ignore the timestamp failing. This is
         * important because otherwise the test script (txzchk_test.sh) would
         * fail due to the timestamp being updated.
         *
         * In test mode (-T) we MUST give fnamchk(1) the -T option (ignore
         * timestamp failure) and if we were given -x we must use -t.
         */
        if (dbg_allowed(DBG_MED)) {
            if (test_mode) {
                dbg(DBG_MED, "about to execute: %s -t -T -v 3 -E %s -- %s", fnamchk, ext, tarball_path);
                exit_code = shell_cmd(__func__, false, true, "% -t -T -v 3 -E % -- %", fnamchk, ext, tarball_path);
            } else {
                dbg(DBG_MED, "about to execute: %s -T -v 3 -E %s -- %s", fnamchk, ext, tarball_path);
                exit_code = shell_cmd(__func__, false, true, "% -T -v 3 -E % -- %", fnamchk, ext, tarball_path);
            }
        } else {
            if (test_mode) {
                exit_code = shell_cmd(__func__, false, true, "% -t -T -E % -- % >/dev/null", fnamchk, ext, tarball_path);
            } else {
                exit_code = shell_cmd(__func__, false, true, "% -T -E % -- % >/dev/null", fnamchk, ext, tarball_path);
            }
        }
    } else {
        if (dbg_allowed(DBG_MED)) {
            if (test_mode) {
                dbg(DBG_MED, "about to execute: %s -t -v 5 -E %s -- %s", fnamchk, ext, tarball_path);
                exit_code = shell_cmd(__func__, false, true, "% -t -v 3 -E % -- %", fnamchk, ext, tarball_path);
            } else {
                dbg(DBG_MED, "about to execute: %s -v 5 -E %s -- %s", fnamchk, ext, tarball_path);
                exit_code = shell_cmd(__func__, false, true, "% -v 3 -E % -- %", fnamchk, ext, tarball_path);
            }
        } else {
            if (test_mode) {
                exit_code = shell_cmd(__func__, false, true, "% -t -E % -- % >/dev/null", fnamchk, ext, tarball_path);
            } else {
                exit_code = shell_cmd(__func__, false, true, "% -E % -- % >/dev/null", fnamchk, ext, tarball_path);
            }
        }
    }
    if (exit_code != 0) {
	warn(TXZCHK_BASENAME, "%s: %s %s failed with exit code: %d", tarball_path, fnamchk, tarball_path, WEXITSTATUS(exit_code));
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
     * with the submit slot number and directory.
     */
    if (fnamchk_okay) {
	/*
	 * form pipe to the fnamchk command
         *
         * If test mode is enabled we will also pass the -T option to fnamchk: to
         * ignore the timestamp test failing. This is important because when a new
         * contest opens the timestamp will be updated and that means that the tests
         * will fail!
         */
        if (read_from_text_file) {
            /*
             * In text file mode (-T) we must ignore the timestamp failing. This
             * is important because otherwise the test script (txzchk_test.sh)
             * would fail due to the timestamp being updated.
             *
             * In text file mode (-T) we MUST give fnamchk(1) the -T option (ignore
             * timestamp failure); if we're given -x we MUST give fnamchk -t.
             */
            if (test_mode) {
                errno = 0; /* pre-clear errno for errp() */
                fnamchk_stream = pipe_open(__func__, false, true, "% -t -T -E % -- %", fnamchk, ext, tarball_path);
                if (fnamchk_stream == NULL) {
                    errp(48, __func__, "popen for reading failed for: %s -- %s", fnamchk, tarball_path);
                    not_reached();
                }
            } else {
                errno = 0; /* pre-clear errno for errp() */
                fnamchk_stream = pipe_open(__func__, false, true, "% -T -E % -- %", fnamchk, ext, tarball_path);
                if (fnamchk_stream == NULL) {
                    errp(49, __func__, "popen for reading failed for: %s -- %s", fnamchk, tarball_path);
                    not_reached();
                }
            }
        } else {
            /*
             * In real mode (not reading from a text file) we MUST check the timestamp so
             * we must NOT use the -T option to fnamchk. Depending on whether we
             * were given -x or not we will give fnamchk -t (we were given -x).
             */
            errno = 0; /* pre-clear errno for errp() */
            if (test_mode) {
                fnamchk_stream = pipe_open(__func__, false, true, "% -t -E % -- %", fnamchk, ext, tarball_path);
            } else {
                fnamchk_stream = pipe_open(__func__, false, true, "% -E % -- %", fnamchk, ext, tarball_path);
            }
            if (fnamchk_stream == NULL) {
                errp(50, __func__, "popen for reading failed for: %s -- %s", fnamchk, tarball_path);
                not_reached();
            }
        }

	/*
	 * read the output from the fnamchk command
	 */
	readline_len = readline(&dirname, fnamchk_stream);
	if (readline_len < 0) {
	    warn(TXZCHK_BASENAME, "%s: unexpected EOF from fnamchk", tarball_path);
	}

	/*
	 * close down pipe
	 */
	errno = 0;		/* pre-clear errno for warnp() */
	ret = pclose(fnamchk_stream);
	if (ret < 0) {
	    warnp(TXZCHK_BASENAME, "in %s: %s: pclose error on fnamchk stream", __func__, tarball_path);
	}

	fnamchk_stream = NULL;

	if (dirname == NULL || *dirname == '\0') {
	    err(51, __func__, "txzchk: unexpected NULL pointer from fnamchk -- %s", tarball_path);
	    not_reached();
	}
    }

    /* determine size of tarball */
    tarball.size = file_size(tarball_path);
    /* report size if too big */
    if (tarball.size < 0) {
	err(52, __func__, "%s: impossible error: txzchk_sanity_chks() found tarball but file_size() did not", tarball_path);
	not_reached();
    } else if (tarball.size > MAX_TARBALL_LEN) {
	++tarball.total_feathers;
        fpara(stderr,
              "",
              "The compressed tarball exceeds the maximum allowed size, sorry.",
              "",
              NULL);
	    warn(TXZCHK_BASENAME, "%s: the compressed tarball size %lld > %d",
				  tarball_path, (long long)tarball.size, MAX_TARBALL_LEN);
    } else if (verbosity_level) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = printf("txzchk: %s size of %lld bytes OK\n", tarball_path, (long long) tarball.size);
	if (ret <= 0) {
	    warnp(TXZCHK_BASENAME, "unable to tell user how big the tarball %s is", tarball_path);
	}
    }
    dbg(DBG_MED, "txzchk: %s size in bytes: %lld", tarball_path, (long long)tarball.size);

    /*
     * if txzchk -T we need to open it as a text file: for test mode
     */
    if (read_from_text_file) {
	errno = 0;		/* pre-clear errno for warnp() */
	input_stream = fopen(tarball_path, "r");
	if (input_stream == NULL)
	{
	    errp(53, __func__, "fopen of %s failed", tarball_path);
	    not_reached();
	}
	errno = 0;		/* pre-clear errno for warnp() */
	ret = setvbuf(input_stream, (char *)NULL, _IOLBF, 0);
	if (ret != 0)
	    warnp(TXZCHK_BASENAME, "is %s: setvbuf failed for %s", __func__, tarball_path);

    } else {
	/*
	 * case: -T was not passed to txzchk so we have to execute tar: if we
         * cannot get a tarball listing it is an error and we abort; else we
         * open a pipe to read the output of the command.
         */

	/*
	 * first execute the tar command
	 */
	if (verbosity_level) {
	    exit_code = shell_cmd(__func__, false, true, "% -tJvf %", tar, tarball_path);
	} else {
	    exit_code = shell_cmd(__func__, false, true, "% -tJvf % >/dev/null", tar, tarball_path);
	}
	if (exit_code != 0) {
	    err(54, __func__, "%s -tJvf %s failed with exit code: %d",
			      tar, tarball_path, WEXITSTATUS(exit_code));
	    not_reached();
	}

	/* now open a pipe to tar command (tar -tJvf) to read from */
        errno = 0; /* pre-clear errno for errp() */
	input_stream = pipe_open(__func__, false, true, "% -tJvf %", tar, tarball_path);
	if (input_stream == NULL) {
	    errp(55, __func__, "popen for reading failed for: %s -tJvf %s",
			      tar, tarball_path);
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
	    dbg(DBG_HIGH, "reached EOF of tarball %s", tarball_path);
	    break;
	} else if (readline_len == 0) {
	    dbg(DBG_HIGH, "found empty line in tarball %s", tarball_path);
	    continue;
	}

	/*
	 * scan for embedded NUL bytes (before end of line)
	 *
	 */
	errno = 0;		/* pre-clear errno for warnp() */
	p = (char *)memchr(linep, 0, (size_t)readline_len);
	if (p != NULL) {
	    ++tarball.total_feathers;
	    warnp(TXZCHK_BASENAME, "found NUL before end of line");
	    if (verbosity_level) {
		msg("skipping to next line");
	    }
	    /* free the allocated memory */
	    if (linep != NULL) {
		free(linep);
		linep = NULL;
	    }
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
		warnp(TXZCHK_BASENAME, "in %s: unable to printf line from text file", __func__);
	}

	/* free the allocated memory */
	if (linep != NULL) {
	    free(linep);
	    linep = NULL;
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
	warnp(TXZCHK_BASENAME, "in %s: %s: %s error on tar stream",
			       __func__, tarball_path, read_from_text_file?"fclose":"pclose");
    }
    input_stream = NULL;

    /*
     * now parse the lines, reporting any feathers stuck in the tarball that
     * have to be detected while parsing
     */
    parse_all_txz_lines(dirname, tarball_path);

    /*
     * check files list and report any additional feathers stuck in the tarball
     */
    check_all_txz_files();

    /* free the files list */
    free_txz_files_list();

    /* free txz_lines list */
    free_txz_lines();

    /* free the allocated memory */
    if (dirname != NULL) {
	free(dirname);
	dirname = NULL;
    }
    if (linep != NULL) {
	free(linep);
	linep = NULL;
    }

    return tarball.total_feathers;
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
	err(56, __func__, "passed NULL str");
	not_reached();
    }

    errno = 0;
    line = calloc(1, sizeof *line);
    if (line == NULL) {
	errp(57, __func__, "unable to allocate struct txz_line *");
	not_reached();
    }

    errno = 0;
    line->line = strdup(str);
    if (line->line == NULL) {
	errp(58, __func__, "unable to strdup string '%s' for lines list", str);
	not_reached();
    }
    line->line_num = line_num;

    dbg(DBG_VHIGH, "adding line %s to lines list", line->line);
    line->next = txz_lines;
    txz_lines = line;
}


/*
 * parse_all_txz_lines - parse all tar lines
 *
 * Parse the txz_lines list and report any feathers stuck in the tarball. After
 * all the lines have been parsed additional tests will be performed.
 *
 * given:
 *
 *	dirname	- directory name as reported by fnamchk (can be NULL if
 *			  fnamchk failed to validate directory)
 *	tarball_path	- the tarball that is being read
 *
 * This function returns void.
 *
 * This function does not return on error.
 */
static void
parse_all_txz_lines(char const *dirname, char const *tarball_path)
{
    struct txz_line *line = NULL;	/* for txz_lines list */
    char *line_dup = NULL;	/* strdup()d line */
    intmax_t sum = 0;		/* sum for sum_and_count() checks */
    intmax_t count = 0;		/* count for sum_and_count() checks */

    /*
     * firewall
     */
    if (tarball_path == NULL) {
	err(59, __func__, "passed NULL tarball_path");
	not_reached();
    }

    for (line = txz_lines; line != NULL; line = line->next) {
	if (line->line == NULL) {
	    warn(TXZCHK_BASENAME, "encountered NULL string on line %ju", line->line_num);
	    ++tarball.total_feathers;
	    continue;
	}

	line_dup = strdup(line->line);
	if (line_dup == NULL) {
	    err(60, __func__, "%s: duplicating %s failed", tarball_path, line->line);
	    not_reached();
	}

	parse_txz_line(line->line, line_dup, dirname, tarball_path, &sum, &count);
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
 *	path	    - file path
 *	dirname    - directory name from fnamchk or NULL if fnamchk failed
 *	perms       - permissions string
 *	isdir       - true ==> is a directory
 *	isfile      - true ==> is a regular file
 *	isexec      - true ==> executable bit (+x) found
 *	length	    - length of file as calculated by string_to_intmax (validating
 *		      size of files will be done later)
 *
 * Returns the newly allocated struct txz_file * with the file information. The
 * function does NOT add it to the list!
 *
 * This function does not return on error.
 */
static struct txz_file *
alloc_txz_file(char const *path, char const *dirname, char *perms, bool isdir, bool isfile, bool isexec, intmax_t length)
{
    struct txz_file *file;  /* the file structure */

    /*
     * firewall
     */
    if (path == NULL) {
	err(61, __func__, "passed NULL path");
	not_reached();
    }
    if (perms == NULL) {
        err(62, __func__, "passed NULL perms");
        not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    file = calloc(1, sizeof *file);
    if (file == NULL) {
	errp(63, __func__, "%s: unable to allocate a struct txz_file *", tarball_path);
	not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    file->filename = strdup(path);
    if (!file->filename) {
	errp(64, __func__, "%s: unable to strdup filename %s", tarball_path, path);
	not_reached();
    }

    /*
     * get root directory name of this file (i.e. up to the first '/')
     */
    file->top_dirname = dir_name(path);
    if (file->top_dirname == NULL || *(file->top_dirname) == '\0') {
	err(65, __func__, "%s: unable to strdup top dirname of filename %s", tarball_path, path);
	not_reached();
    }

    /*
     * if fnamchk did not fail make copy of dirname
     */
    if (dirname != NULL) {
        errno = 0; /* pre-clear errno for errp() */
        file->dirname = strdup(dirname);
        if (file->dirname == NULL) {
            err(66, __func__, "%s: unable to strdup dirname %s", tarball_path, dirname);
            not_reached();
        }
    } else {
        file->dirname = NULL;
    }

    /*
     * get basename of file
     */
    file->basename = base_name(path);
    if (file->basename == NULL || *(file->basename) == '\0') {
	err(67, __func__, "%s: unable to strdup basename of filename %s", tarball_path, path);
	not_reached();
    }

    /*
     * NOTE: perms was already strdup()d so we don't do that here
     */
    file->perms = perms;
    /*
     * convert perms into a mode_t
     */
    file->mode = get_mode(path, file->perms);

    /* record the length */
    file->length = length;

    /*
     * record if a directory
     */
    file->isdir = isdir;

    /*
     * record if regular file (i.e. first char in mode/perms is '-')
     */
    file->isfile = isfile;

    /*
     * record if +x
     */
    file->isexec = isexec;

    return file;
}

/*
 * get_mode - convert tar(1) mode output into a mode_t
 *
 * given:
 *
 *      filename    - filename
 *      perms       - permissions string
 *
 * This function does not return on a NULL string.
 *
 * Returns a mode_t corresponding to the file's permission string.
 */
mode_t get_mode(char const *filename, char const *perms)
{
    mode_t mode = 0;       /* mode_t to return */
    char const *s = NULL;     /* perms string in file */

    if (filename == NULL) {
        warn(TXZCHK_BASENAME, "in %s: filename is NULL", __func__);
        ++tarball.total_feathers;
        return 0;
    }
    if (perms == NULL) {
	warn(TXZCHK_BASENAME, "in %s: perms for file '%s' is NULL", __func__, filename);
        ++tarball.total_feathers;
        return 0;
    }

    if (strlen(perms) < 10) {
        warn(TXZCHK_BASENAME, "in %s: file %s has NULL or too short perms string", __func__, filename);
        ++tarball.total_feathers;
        return 0;
    }

    s = perms;
    /*
     * first the type
     */
    switch (*s) {
        case '-':
            mode |= S_IFREG;
            break;
        case 'd':
            mode |= S_IFDIR;
            break;
        case 'l':
            mode |= S_IFLNK;
            break;
        case 'b':
            mode |= S_IFBLK;
            break;
        case 'c':
            mode |= S_IFCHR;
            break;
        case 'p':
            mode |= S_IFIFO;
            break;
        case 's':
            mode |= S_IFSOCK;
            break;
        default:
            warn(TXZCHK_BASENAME, "unknown type '%c' in permission: %s: file: %s", *s, s, filename);
            ++tarball.total_feathers;
            break;
    }

    /*
     * Yes we could use a loop and check by index but we're more explicit here.
     */

    /*
     * user perms
     */
    if (s[1] == 'r') {
        mode |= S_IRUSR;
    } else if (s[1] != '-') {
        warn(TXZCHK_BASENAME, "user read permission '%c' in file %s invalid", s[1], filename);
        ++tarball.total_feathers;
    }
    if (s[2] == 'w') {
        mode |= S_IWUSR;
    } else if (s[2] != '-') {
        warn(TXZCHK_BASENAME, "user write permission '%c' in file %s invalid", s[2], filename);
        ++tarball.total_feathers;
    }
    if (s[3] == 'x' || s[3] == 's') {
        mode |= S_IXUSR;
    }
    if (s[3] == 's' || s[3] == 'S') {
        mode |= S_ISUID;
    }
    if (s[3] != 's' && s[3] != 'S' && s[3] != 'x' && s[3] != '-') {
        warn(TXZCHK_BASENAME, "user exec permission '%c' in file %s invalid", s[3], filename);
        ++tarball.total_feathers;
    }


    /*
     * group
     */
    if (s[4] == 'r') {
        mode |= S_IRGRP;
    } else if (s[4] != '-') {
        warn(TXZCHK_BASENAME, "group read permission '%c' in file %s invalid", s[4], filename);
        ++tarball.total_feathers;
    }
    if (s[5] == 'w') {
        mode |= S_IWGRP;
    } else if (s[5] != '-') {
        warn(TXZCHK_BASENAME, "group write permission '%c' in file %s invalid", s[5], filename);
        ++tarball.total_feathers;
    }
    if (s[6] == 'x' || s[6] == 's') {
        mode |= S_IXGRP;
    }
    if (s[6] == 's' || s[6] == 'S') {
        mode |= S_ISGID;
    }
    if (s[6] != 's' && s[6] != 'S' && s[6] != 'x' && s[6] != '-') {
        warn(TXZCHK_BASENAME, "group exec permission '%c' in file %s invalid", s[6], filename);
        ++tarball.total_feathers;
    }

    /*
     * everyone else
     */
    if (s[7] == 'r') {
        mode |= S_IROTH;
    } else if (s[7] != '-') {
        warn(TXZCHK_BASENAME, "other read permission '%c' in file %s invalid", s[7], filename);
        ++tarball.total_feathers;
    }
    if (s[8] == 'w') {
        mode |= S_IWOTH;
    } else if (s[8] != '-') {
        warn(TXZCHK_BASENAME, "other write permission '%c' in file %s invalid", s[8], filename);
        ++tarball.total_feathers;
    }
    if (s[9] == 'x' || s[9] == 't') {
        mode |= S_IXOTH;
    }
    if (s[9] == 't' || s[9] == 'T') {
        mode |= S_ISVTX;
    }
    if (s[9] != 's' && s[9] != 'S' && s[9] != 'x' && s[9] != '-') {
        warn(TXZCHK_BASENAME, "other exec permission '%c' in file %s invalid", s[9], filename);
        ++tarball.total_feathers;
    }

    return mode;
}


/*
 * add_txz_file_to_list - add a filename to the linked list
 *
 * given:
 *
 *	file		    - pointer to struct txz_file which should already have the name
 *
 * If the function finds this filename already in the list (full path!) it
 * increments the count and does not add it to the list; else it adds it to the
 * list with a count of 1.
 *
 * This function does not return on error.
 */
static void
add_txz_file_to_list(struct txz_file *txzfile)
{
    /*
     * firewall
     */
    if (txzfile == NULL || txzfile->filename == NULL || txzfile->basename == NULL) {
	err(68, __func__, "called with NULL pointer(s)");
	not_reached();
    }

    /* lazily add to list */
    dbg(DBG_VHIGH, "adding filename %s (basename %s) to list of files", txzfile->filename, txzfile->basename);
    txzfile->next = txz_files;
    txz_files = txzfile;
}


/*
 * free_txz_file        - free a struct txz_file
 *
 * given:
 *      file        - pointer to pointer to a txz_file to free
 *
 * This function does not return on a NULL pointer.
 * This function should set the pointer in the _calling_ function to NULL but
 * the caller might still be wise to set it to NULL just in case something funny
 * goes on.
 */
static void
free_txz_file(struct txz_file **file)
{
    /*
     * firewall
     */
    if (file == NULL || *file == NULL) {
        err(69, __func__, "file is NULL");
        not_reached();
    }

    if ((*file)->filename != NULL) {
        free((*file)->filename);
        (*file)->filename = NULL;
    }
    if ((*file)->basename != NULL) {
        free((*file)->basename);
        (*file)->basename = NULL;
    }

    if ((*file)->perms != NULL) {
        free((*file)->perms);
        (*file)->perms = NULL;
    }

    /*
     * free the struct itself
     */
    free(*file);
    *file = NULL;
}


/*
 * free_txz_files_list  - free the txz_files linked list
 */
static void
free_txz_files_list(void)
{
    struct txz_file *file, *next_file;

    for (file = txz_files; file != NULL; file = next_file)
    {
	next_file = file->next;
        free_txz_file(&file);
        file = NULL; /* redundant but it's safe */
    }

    txz_files = NULL;
}
