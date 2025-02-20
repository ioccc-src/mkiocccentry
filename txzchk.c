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
#include <sys/wait.h>	/* for WEXITSTATUS() */

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
    "\t-F fnamchk\tpath to tool that checks if tarball_path is a valid compressed tarball name\n"
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
    bool fnamchk_flag_used = false;	    /* if -F option used */
    bool tar_flag_used = false;		    /* true ==> -t /path/to/tar was given */
    int i;

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
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 't': /* -t tar - specify path to tar (perhaps to tar and feather :-) ) */
	    tar = optarg;
	    tar_flag_used = true;
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
	    check_invalid_option(program, i, optopt);
	    usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	}
    }

    /* must have the exact required number of args */
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }
    tarball_path = argv[optind];
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
     *
     * If -T was used we don't actually need tar(1) so we test for that
     * specifically.
     */

    if (!read_from_text_file) {
	find_utils(tar_flag_used, &tar, false, NULL, false, NULL,
		   fnamchk_flag_used, &fnamchk, false, NULL, false, NULL);
    } else {
	find_utils(false, NULL, false, NULL, false, NULL,
		   fnamchk_flag_used, &fnamchk, false, NULL, false, NULL);
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
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    if (tarball.total_feathers != 0) {
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

	dbg(DBG_MED, "%s %s a .info.json", tarball_path, HAS_DOES_NOT_HAVE(tarball.has_info_json));
	dbg(DBG_HIGH, "%s %s an empty .info.json", tarball_path, HAS_DOES_NOT_HAVE(tarball.empty_info_json));
	dbg(DBG_HIGH, "%s .info.json size is %jd", tarball_path, (intmax_t)tarball.info_json_size);
	dbg(DBG_MED, "%s %s a .auth.json", tarball_path, HAS_DOES_NOT_HAVE(tarball.has_auth_json));
	dbg(DBG_HIGH, "%s %s an empty .auth.json", tarball_path, HAS_DOES_NOT_HAVE(tarball.empty_auth_json));
	dbg(DBG_HIGH, "%s .auth.json size is %jd", tarball_path, (intmax_t)tarball.auth_json_size);
	dbg(DBG_MED, "%s %s a prog.c", tarball_path, HAS_DOES_NOT_HAVE(tarball.has_prog_c));
	dbg(DBG_HIGH, "%s %s an empty prog.c", tarball_path, HAS_DOES_NOT_HAVE(tarball.empty_prog_c));
	dbg(DBG_HIGH, "%s prog.c size is %jd", tarball_path, (intmax_t)tarball.prog_c_size);
	dbg(DBG_MED, "%s %s a remarks.md", tarball_path, HAS_DOES_NOT_HAVE(tarball.has_remarks_md));
	dbg(DBG_HIGH, "%s %s an empty remarks.md", tarball_path, HAS_DOES_NOT_HAVE(tarball.empty_remarks_md));
	dbg(DBG_HIGH, "%s remarks.md size is %jd", tarball_path, (intmax_t)tarball.remarks_md_size);
	dbg(DBG_MED, "%s %s a Makefile", tarball_path, HAS_DOES_NOT_HAVE(tarball.has_Makefile));
	dbg(DBG_HIGH, "%s %s an empty Makefile", tarball_path, HAS_DOES_NOT_HAVE(tarball.empty_Makefile));
	dbg(DBG_HIGH, "%s Makefile size is %jd", tarball_path, (intmax_t)tarball.Makefile_size);
	dbg(DBG_MED, "%s tarball size is %jd according to stat(2)", tarball_path, (intmax_t)tarball.size);
	dbg(DBG_MED, "%s total file size is %jd", tarball_path, (intmax_t)tarball.files_size);
	dbg(DBG_HIGH, "%s shrunk in files size %ju time%s", tarball_path, tarball.files_size_shrunk,
		SINGULAR_OR_PLURAL(tarball.files_size_shrunk));
	dbg(DBG_HIGH, "%s went below 0 in all files size %ju time%s", tarball_path, tarball.negative_files_size,
		SINGULAR_OR_PLURAL(tarball.negative_files_size));
	dbg(DBG_HIGH, "%s went above max files size %ju %ju time%s", tarball_path,
		(uintmax_t)MAX_SUM_FILELEN, (uintmax_t)tarball.files_size_too_big,
		SINGULAR_OR_PLURAL(tarball.files_size_too_big));

	if (tarball.correct_directories < tarball.total_files) {
	    dbg(DBG_MED, "%s has %ju incorrect director%s", tarball_path, tarball.total_files - tarball.correct_directories,
		    tarball.total_files - tarball.correct_directories == 1 ? "y":"ies");
	} else {
	    dbg(DBG_MED, "%s has 0 incorrect directories", tarball_path);
	}

	dbg(DBG_MED, "%s has %ju invalid dot file%s", tarball_path, tarball.invalid_dot_files,
		SINGULAR_OR_PLURAL(tarball.invalid_dot_files));
	dbg(DBG_MED, "%s has %ju file%s named '.'", tarball_path, tarball.named_dot, SINGULAR_OR_PLURAL(tarball.named_dot));
	dbg(DBG_MED, "%s has %ju file%s with at least one unsafe char", tarball_path, tarball.unsafe_chars,
		SINGULAR_OR_PLURAL(tarball.unsafe_chars));
        if (tarball.invalid_filename_lengths > 0) {
            dbg(DBG_MED, "%s has %ju oversized or undersized filename%s", tarball_path, tarball.invalid_filename_lengths,
                    SINGULAR_OR_PLURAL(tarball.invalid_filename_lengths));
        }

        dbg(DBG_MED, "%s has %ju extra director%s", tarball_path, tarball.directories,
                tarball.directories == 1 ? "y":"ies");
        if (MAX_EXTRA_DIR_COUNT > 0) {
            if (tarball.directories > MAX_EXTRA_DIR_COUNT) {
                dbg(DBG_MED, "%s has %ju extra director%s > max %ju: %ju - %ju == %ju",
                        tarball_path,
                        (uintmax_t)(tarball.directories - MAX_EXTRA_DIR_COUNT),
                        (uintmax_t)(tarball.directories - MAX_EXTRA_DIR_COUNT) == 1 ? "y" : "ies",
                        (uintmax_t)MAX_EXTRA_DIR_COUNT, (uintmax_t)tarball.directories, (uintmax_t)MAX_EXTRA_DIR_COUNT,
                        (uintmax_t)(tarball.directories - MAX_EXTRA_DIR_COUNT));
            } else if (tarball.directories < MAX_EXTRA_DIR_COUNT) {
                dbg(DBG_MED, "%s has %ju fewer extra director%s than max %ju: %ju - %ju == %ju", tarball_path,
                    (uintmax_t)(MAX_EXTRA_DIR_COUNT-tarball.directories),
                    (uintmax_t)(MAX_EXTRA_DIR_COUNT-tarball.directories) == 1 ? "y" : "ies",
                    (uintmax_t)MAX_EXTRA_DIR_COUNT, (uintmax_t)MAX_EXTRA_DIR_COUNT, (uintmax_t)tarball.directories,
                    (uintmax_t)MAX_EXTRA_DIR_COUNT - (uintmax_t)tarball.directories);
            } else if (tarball.directories == MAX_EXTRA_DIR_COUNT) {
                dbg(DBG_MED, "%s has the same number of extra directories than max %ju: %ju - %ju == %ju", tarball_path,
                    (uintmax_t)MAX_EXTRA_DIR_COUNT, (uintmax_t)tarball.directories, (uintmax_t)MAX_EXTRA_DIR_COUNT,
                    (uintmax_t)(tarball.directories - MAX_EXTRA_DIR_COUNT));
            }
        }

        dbg(DBG_MED, "%s has a total of %ju invalid directory name%s", tarball_path, (uintmax_t)tarball.invalid_dirnames,
                SINGULAR_OR_PLURAL(tarball.invalid_dirnames));
        dbg(DBG_MED, "%s has a total of %ju directory depth error%s", tarball_path, (uintmax_t)tarball.depth_errors,
                SINGULAR_OR_PLURAL(tarball.depth_errors));
        dbg(DBG_MED, "%s has a total of %ju invalid director%s", tarball_path, (uintmax_t)tarball.invalid_directories,
                SINGULAR_OR_PLURAL(tarball.invalid_directories));

        dbg(DBG_MED, "%s has %ju extra filename%s", tarball_path, tarball.extra_filenames,
                SINGULAR_OR_PLURAL(tarball.extra_filenames));
        dbg(DBG_MED, "%s has %ju required filename%s", tarball_path, tarball.required_filenames,
                SINGULAR_OR_PLURAL(tarball.required_filenames));
        dbg(DBG_MED, "%s has %ju forbidden filename%s", tarball_path, tarball.forbidden_filenames,
                SINGULAR_OR_PLURAL(tarball.forbidden_filenames));
        dbg(DBG_MED, "%s has %ju file%s with invalid permissions", tarball_path, tarball.invalid_perms,
                SINGULAR_OR_PLURAL(tarball.invalid_perms));
        dbg(DBG_MED, "%s has %ju executable file%s", tarball_path, tarball.total_exec_files,
                SINGULAR_OR_PLURAL(tarball.total_exec_files));

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
	warn("txzchk", "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }

    if (prog == NULL) {
	prog = TXZCHK_BASENAME;
	warn("txzchk", "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
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
 * NOTE: This function does not return on error or if things are not sane.
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
    bool allowed_dot_file = false;	/* true ==> basename is an allowed '.' file */
    enum path_sanity sanity = PATH_OK;  /* assume path is okay */

    /*
     * firewall
     */
    if (tarball_path == NULL || file == NULL || file->basename == NULL || file->filename == NULL) {
	err(22, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * identify if file is an allowed '.' file
     *
     * NOTE: files that are allowed to begin with '.' must also be lower case.
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
    if (count_dirs(file->filename) == 1 && (!strcmp(file->basename, INFO_JSON_FILENAME) ||
                !strcmp(file->basename, AUTH_JSON_FILENAME))) {
	allowed_dot_file = true;
        dbg(DBG_VHIGH, "%s: is an allowed dot file", file->filename);
    } else {
        dbg(DBG_VHIGH, "%s: is not an allowed dot file", file->filename);
    }

    if (has_special_bits(file)) {
        dbg(DBG_MED, "file->filename: %s: has invalid perms: %s", file->filename, file->perms);
        ++tarball.total_feathers;
    }

    /*
     * case: basename is NOT allowed to begin with a dot.
     */
    if (!allowed_dot_file) {
        /*
         * filename must use only POSIX portable filename and + chars plus /
         *
         * NOTE: we check for sane relative paths here only if the file is not a
         * valid dot file. This is because if it is a valid dot file the
         * sane_relative_path() will still flag it as invalid as it has a '.' in
         * it.
         */
        sanity = sane_relative_path(file->filename, MAX_PATH_LEN, MAX_FILENAME_LEN, MAX_PATH_DEPTH, false);
        if (sanity != PATH_OK) {
            ++tarball.total_feathers; /* report it once and consider it only one feather */
            ++tarball.unsafe_chars;
            warn(__func__, "%s: file \"%s\" is not a sane relative path", tarball_path, file->filename);
        } else {
            dbg(DBG_VHIGH, "%s: file is a sane relative path: %s", tarball_path, file->filename);
        }

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
	    warn("txzchk", "%s: found non %s and %s dot file: %s",
			   tarball_path, AUTH_JSON_FILENAME, INFO_JSON_FILENAME, file->basename);
	    tarball.invalid_dot_files++;

	    /* check for files called '.' without anything after the dot */
	    if (file->basename[1] == '\0') {
		++tarball.total_feathers;
		++tarball.named_dot;
		warn("txzchk", "%s: found file called '.' in path %s", tarball_path, file->filename);
	    }
	}
    }

    /* check the dirs in the path */
    check_directory(file, dirname, tarball_path);
}


/*
 * check_file_size - if file is required record size and (depending on file) report invalid size
 *
 * given:
 *
 *	tarball_path	- the tarball (or text file) we're checking
 *	size		- size of the file
 *	file		- the struct txz_file we're checking
 *
 * Returns void.
 *
 * Does not return on error (NULL pointers passed in).
 */
static void
check_file_size(char const *tarball_path, off_t size, struct txz_file *file)
{
    /*
     * firewall
     */

    if (tarball_path == NULL || file == NULL) {
	err(23, __func__, "called with NULL arg(s)");
	not_reached();
    } else if (file->basename == NULL || file->filename == NULL) {
	err(24, __func__, "file->basename == NULL || file->filename == NULL which should never happen");
	not_reached();
    }

    if (size == 0) {
	if (!strcmp(file->basename, AUTH_JSON_FILENAME)) {
	    ++tarball.total_feathers;
	    warn("txzchk", "%s: found empty %s file", tarball_path, AUTH_JSON_FILENAME);
	    tarball.empty_auth_json = true;
	} else if (!strcmp(file->basename, INFO_JSON_FILENAME)) {
	    ++tarball.total_feathers;
	    tarball.empty_info_json = true;
	    warn("txzchk", "%s: found empty %s file", tarball_path, INFO_JSON_FILENAME);
	} else if (!strcmp(file->basename, "remarks.md")) {
	    ++tarball.total_feathers;
	    tarball.empty_remarks_md = true;
	    warn("txzchk", "%s: found empty remarks.md", tarball_path);
	} else if (!strcmp(file->basename, "Makefile")) {
	    ++tarball.total_feathers;
	    tarball.empty_Makefile = true;
	    warn("txzchk", "%s: found empty Makefile", tarball_path);
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
 *	dirname	- fnamchk result (if passed - else NULL)
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
check_all_txz_files(void)
{
    struct txz_file *file;  /* to iterate through files list */
    size_t len = 0;         /* length of each filename */
    size_t dirs = 0;        /* number of dirs in each filename */
    bool forbidden = false; /* true ==> filename is a forbidden name */
    bool required = false;  /* true ==> filename is required */
    bool optional = false;  /* true ==> filename is optional file */

    /*
     * Now go through the files list to verify the required files are there and
     * also to detect any additional feathers stuck in the tarball (or issues in
     * the text file).
     */
    for (file = txz_files; file != NULL; file = file->next) {
	if (file->basename == NULL) {
	    err(25, __func__, "found NULL file->basename in txz_files list");
	    not_reached();
	} else if (file->filename == NULL) {
	    err(26, __func__, "found NULL file->filename in txz_files list");
	    not_reached();
        }

        /*
         * always reset bools to false
         */
        forbidden = false;
        required = false;
        optional = false;

        len = strlen(file->basename);
        if (len <= 0) {
            warn("txzchk", "%s: filename length 0 for: %s", tarball_path, file->basename);
            ++tarball.invalid_filename_lengths;
            ++tarball.total_feathers;
            continue;
        } else if (!test_filename_len(file->basename)) {
            warn("txzchk", "%s: filename length: %ju not in range of > 0 && <= MAX_FILENAME_LEN %ju", file->basename,
                    (uintmax_t)len, (uintmax_t)MAX_FILENAME_LEN);
            ++tarball.invalid_filename_lengths;
            ++tarball.total_feathers;
        }

	if (file->count > 1) {
	    warn("txzchk", "%s: found a total of %ju files with the name %s in the same directory", tarball_path,
                    file->count, file->basename);
	    tarball.total_feathers += file->count - 1;
	}

        /*
         * directories have specific (extra) checks
         */
        if (file->isdir) {
            /*
             * here we have to determine if it is a required or optional
             * filename. If it is it is not allowed!
             *
             * NOTE: if someone wishes to put these names as a directory in a
             * subdirectory that is perfectly fine and this is why we check the
             * full name and not the basename.
             */
            if (is_mandatory_filename(file->filename) || is_optional_filename(file->filename)) {
	        warn("txzchk", "%s: directory name %s not allowed", tarball_path, file->filename);
                ++tarball.invalid_dirnames;
                ++tarball.total_feathers;
            }
        }
        /*
         * obtain number of directories in filename to check for specific files
         */
        dirs = count_dirs(file->filename);
        /*
         * if we only have one directory we have to check for specific required
         * files. It is not an error if it does not match as it could be the
         * next file in the list; we simply need to know if THIS file is the one
         * we are checking.
         */
        if (dirs <= 0) {
            if (file->isfile) {
                /*
                 * if 0 directories everything is invalid
                 */
                ++tarball.forbidden_filenames;
                /*
                 * it's also an extra filename
                 */
                ++tarball.extra_filenames;
            } else {
                ++tarball.invalid_directories; /* invalid directory */
            }
            ++tarball.total_feathers;
        } else if (dirs == 1) {
            /*
             * the top level submission directory
             */
            if (!strcmp(file->basename, INFO_JSON_FILENAME)) {
                tarball.has_info_json = true;
                required = true;
            } else if (!strcmp(file->basename, AUTH_JSON_FILENAME)) {
                tarball.has_auth_json = true;
                required = true;
            } else if (!strcmp(file->basename, MAKEFILE_FILENAME)) {
                tarball.has_Makefile = true;
                required = true;
            } else if (!strcmp(file->basename, PROG_C_FILENAME)) {
                tarball.has_prog_c = true;
                required = true;
            } else if (!strcmp(file->basename, REMARKS_FILENAME)) {
                tarball.has_remarks_md = true;
                required = true;
            } else {
                /*
                 * here we know it's not one of the five required files so we
                 * have to do other checks.
                 */

                /*
                 * ensure the booleans are false
                 */
                forbidden = false;
                optional = false;
                required = false; /* note it cannot be required here */

                /*
                 * in the case it's not one of the required filenames, we have
                 * to check for forbidden filenames
                 */
                if (is_forbidden_filename(file->basename)) {
                    warn("txzchk", "%s: filename %s (basename %s) not allowed", tarball_path, file->filename, file->basename);
                    ++tarball.forbidden_filenames;
                    ++tarball.total_feathers;
                    forbidden = true;
                }
                /*
                 * we need to count optional filenames too, assuming it's not a
                 * forbidden filename (meaning it couldn't be an optional
                 * filename)
                 */
                if (!forbidden && is_optional_filename(file->filename)) {
                    optional = true;
                    ++tarball.optional_filenames;
                }

                /*
                 * if we don't have an optional or required filename then we
                 * have to increment the extra filename count
                 */
                if (!optional && !required) {
                    dbg(DBG_HIGH, "%s: extra filename: %s", tarball_path, file->basename);
                    ++tarball.extra_filenames;
                }
            }

            /*
             * we need to make sure we record the number of required files
             */
            if (required) {
                ++tarball.required_filenames;
            }
        } else if (dirs > 1) { /* subdirectory */
            /*
             * ensure the booleans are false
             */
            forbidden = false;
            required = false;
            optional = false;

            /*
             * why dirs + 1 for max depth?
             *
             * Because the way count_dirs() works is by the count_comps()
             * function which considers the delimiting character (in this case
             * '/') so that if one has a file:
             *
             *      foo/bar/baz
             *
             * it's two directories but if one has:
             *
             *      foo/bar/baz/
             *
             * it's three directories (see the comments in jparse/util.c for
             * more details). However with the IOCCC it's a matter of
             * depth, not the number of directories, so we have to do + 1.
             */
            if (dirs + 1 > MAX_PATH_DEPTH) {
                warn("txzchk", "%s: depth too deep: %ju > %ju", file->filename, (uintmax_t)(dirs+1),
                        (uintmax_t)MAX_PATH_DEPTH);
                ++tarball.depth_errors;
                ++tarball.total_feathers;
            }
            if (!strcmp(file->basename, INFO_JSON_FILENAME) ||
                !strcmp(file->basename, AUTH_JSON_FILENAME)) {
                    ++tarball.forbidden_filenames;
                    ++tarball.invalid_dot_files;
                    ++tarball.total_feathers;
                    forbidden = true;
                    warn("txzchk", "%s (basename %s) is an invalid dot file", file->filename, file->basename);
            } else if (is_forbidden_filename(file->basename)) {
                ++tarball.forbidden_filenames;
                ++tarball.total_feathers;
                forbidden = true;
                warn("txzchk", "%s (basename %s) is a forbidden filename", file->filename, file->basename);
            } else {
                /*
                 * we need to count optional filenames too.
                 *
                 * NOTE: in subdirectories one may have certain filenames not
                 * allowed in the top level directory. Some are still forbidden,
                 * however, like any dot file and README.md (both checked
                 * above).
                 */
                if (is_optional_filename(file->filename)) {
                    optional = true;
                    ++tarball.optional_filenames;
                }

                /*
                 * if not in top level directory (depth > 1) we count this as an
                 * extra filename
                 */
                ++tarball.extra_filenames;
            }
        }
    }

    /* determine if the required files are there */
    if (!tarball.has_info_json) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no .info.json found", tarball_path);
    }
    if (!tarball.has_auth_json) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no .auth.json found", tarball_path);
    }
    if (!tarball.has_prog_c) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no prog.c found", tarball_path);
    }
    if (!tarball.has_Makefile) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no Makefile found", tarball_path);
    }
    if (!tarball.has_remarks_md) {
	++tarball.total_feathers;
	warn("txzchk", "%s: no remarks.md found", tarball_path);
    }
    if (tarball.correct_directories < tarball.total_files) {
	++tarball.total_feathers;
	warn("txzchk", "%s: not all files in correct directory", tarball_path);
    }

    /*
     * Report total number of non .auth.json and .info.json dot files.
     * Don't increment the number of feathers as this was done in
     * check_txz_file().
     */
    if (tarball.invalid_dot_files > 0) {
	warn("txzchk", "%s: found a total of %ju invalidly named dot file%s",
	    tarball_path, tarball.invalid_dot_files, tarball.invalid_dot_files==1?"":"s");
    }

    /*
     * if MAX_EXTRA_DIR_COUNT > 0 and directories > MAX_EXTRA_DIR_COUNT then it is an issue.
     */
    if (MAX_EXTRA_DIR_COUNT > 0 && tarball.directories > MAX_EXTRA_DIR_COUNT) {
        warn("txzchk", "%s: %ju extra directories > max %ju: %ju > %ju", tarball_path, (uintmax_t)tarball.directories,
                (uintmax_t)MAX_EXTRA_DIR_COUNT, (uintmax_t)tarball.directories, (uintmax_t)MAX_EXTRA_DIR_COUNT);
        ++tarball.directories;
        ++tarball.total_feathers;
    }

    /*
     * report total feathers found
     */
    if (tarball.total_feathers > 0) {
	warn("txzchk", "%s: found %ju feather%s stuck in the tarball",
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
	err(27, __func__, "passed NULL arg(s)");
	not_reached();
    }

    if (dirname != NULL && *dirname != '\0')
    {
	if (strncmp(file->filename, dirname, strlen(dirname))) {
	    warn("txzchk", "%s: found incorrect top level directory in filename %s", tarball_path, file->filename);
	    ++tarball.total_feathers;
	} else {
	    /* This file has the right top level directory */
	    tarball.correct_directories++;
	}
    } else if (!test_mode) {
        warn("txzchk", "%s: found incorrect top level directory in filename %s", tarball_path, file->filename);
        ++tarball.total_feathers;
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
 *	dirname	- directory name retrieved from fnamchk or NULL if it failed
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
 * This function does not return on error.
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
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", tarball_path);
	if (verbosity_level) {
	    msg("skipping to next line");
	}
	++tarball.total_feathers;
	return;
    }

    test = string_to_intmax(p, &length);
    if (!test) {
	warn("txzchk", "%s: trying to parse file size in on line: <%s>: token: <%s>", tarball_path, line_dup, p);
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
    }

    /* add to total number of files and total size if it's a normal file */
    if (isfile) {
	count_and_sum(tarball_path, sum, count, length);
    }

    /*
     * the next two fields we don't care about but we loop three times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 3; ++i) {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line", tarball_path);
	    if (verbosity_level) {
		msg("skipping to next line");
	    }
	    ++tarball.total_feathers;
	    return;
	}
    }
    /* p should now contain the filename. */
    file = alloc_txz_file(p, dirname, perms, isdir, isfile, isexec, length);
    if (file == NULL) {
	err(29, __func__, "alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p != NULL) {
	    warn("txzchk", "%s: bogus field found after filename: %s", tarball_path, p);
	    ++tarball.total_feathers;
	}
    } while (p != NULL);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (full path) more
     * than once: it simply increments the times it's been seen.
     */
    check_file_size(tarball_path, length, file);

    /* checks on this specific file */
    check_txz_file(tarball_path, dirname, file);

    add_txz_file_to_list(file);
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
	err(30, __func__, "tarball_path is NULL");
	not_reached();
    } else if (sum == NULL) {
	err(31, __func__, "sum is NULL");
	not_reached();
    } else if (count == NULL) {
	err(32, __func__, "count is NULL");
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
	warn("txzchk", "%s: total file size went below 0: %jd", tarball_path, *sum);
	if (*sum < tarball.previous_files_size) {
	    ++tarball.files_size_shrunk;
	    warn("txzchk", "%s: total files size %jd < previous file size %jd", tarball_path, *sum,
		(intmax_t)tarball.previous_files_size);
	}
    }
    /* check for sum of total file lengths being too big */
    if (*sum > MAX_SUM_FILELEN) {
	++tarball.total_feathers;
	++tarball.files_size_too_big;
	warn("txzchk", "%s: total file size too big: %jd > %jd", tarball_path,
	    *sum, (intmax_t)MAX_SUM_FILELEN);
    }
    /* update the previous files size */
    tarball.previous_files_size = *sum;

    /* check for no or negative file count */
    if (*count <= 0) {
	++tarball.total_feathers;
	++tarball.invalid_files_count;
	warn("txzchk", "%s: files count <= 0: %jd", tarball_path, *count);
    }
    /* check for too many files */
    if (*count - (intmax_t)tarball.extra_filenames > MAX_EXTRA_FILE_COUNT) {
	++tarball.total_feathers;
	++tarball.invalid_files_count;
	warn("txzchk", "%s: too many files: %jd > %jd", tarball_path,
	    *count - (intmax_t)tarball.extra_filenames, (intmax_t)MAX_EXTRA_FILE_COUNT);
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
 *	dirname	- directory name retrieved from fnamchk or NULL if it failed
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
 * This function does not return on error.
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
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", tarball_path);
	if (verbosity_level) {
	    msg("skipping to next line");
	}
	++tarball.total_feathers;
	return;
    }

    /*
     * attempt to find !isdigit() chars (i.e. verify the tarball listing includes
     * the UID, not the user name).
     */
    for (; p && *p && isdigit(*p); ) {
	++p; /* satisfy frivolous warning (give loop a body instead of having the loop do the actual action) */
    }

    if (*p) {
	warn("txzchk", "%s: found non-numerical UID in file in line %s", tarball_path, line_dup);
	++tarball.total_feathers;
    }

    /*
     * now do the same for group
     */
    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", tarball_path);
	if (verbosity_level) {
	    msg("skipping to next line");
	}
	++tarball.total_feathers;
	return;
    }

    /*
     * attempt to find !isdigit() chars (i.e. verify the tarball listing includes
     * the GID of the file, not group name).
     */
    for (; p && *p && isdigit(*p); ) {
	++p; /* satisfy frivolous warnings */
    }

    if (*p) {
	warn("txzchk", "%s: found non-numerical GID in file in line: %s", tarball_path, line_dup);
	++tarball.total_feathers;
    }

    p = strtok_r(NULL, tok_sep, saveptr);
    if (p == NULL) {
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", tarball_path);
	if (verbosity_level) {
	    msg("skipping to next line");
	}
	++tarball.total_feathers;
	return;
    }

    test = string_to_intmax(p, &length);
    if (!test) {
	warn("txzchk", "%s: trying to parse file size in on line: <%s>: token: <%s>", tarball_path, line_dup, p);
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
    }

    /* add to total number of files and total size if it's a normal file */
    if (isfile) {
	count_and_sum(tarball_path, sum, count, length);
    }

    /*
     * the next three fields we don't care about but we loop four times to
     * get the following field which we _do_ care about.
     */
    for (i = 0; i < 4; ++i) {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p == NULL) {
	    warn("txzchk", "%s: NULL pointer trying to parse line", tarball_path);
	    if (verbosity_level) {
		msg("skipping to next line");
	    }
	    ++tarball.total_feathers;
	    return;
	}
    }
    /* p should now contain the filename. */
    file = alloc_txz_file(p, dirname, perms, isdir, isfile, isexec, length);
    if (file == NULL) {
	err(34, __func__, "alloc_txz_file() returned NULL");
	not_reached();
    }

    do {
	p = strtok_r(NULL, tok_sep, saveptr);
	if (p != NULL) {
	    warn("txzchk", "%s: bogus field found after filename: %s", tarball_path, p);
	    ++tarball.total_feathers;
	}
    } while (p != NULL);

    /*
     * although we could check these later we check here because the
     * add_txz_file_to_list() function doesn't add the same file (basename) more
     * than once: it simply increments the times it's been seen.
     */
    check_file_size(tarball_path, length, file);

    /* checks on this specific file */
    check_txz_file(tarball_path, dirname, file);

    add_txz_file_to_list(file);
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
	err(35, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * look for non-directory non-regular non-hard-linked items
     */
    if (*linep != '-' && *linep != 'd') {
	warn("txzchk", "%s: found a non-directory non-regular non-hard-linked item: %s",
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
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", tarball_path);
	if (verbosity_level) {
	    msg("skipping to next line");
	}
	++tarball.total_feathers;
	return;
    }

    /*
     * save permission bits
     */
    errno = 0;      /* pre-clear errno for errp() */
    perms = strdup(p);
    if (perms == NULL) {
        errp(36, __func__, "failed to strdup permissions string");
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
	warn("txzchk", "%s: NULL pointer encountered trying to parse line", tarball_path);
	if (verbosity_level) {
	    msg("skipping to next line");
	}
	++tarball.total_feathers;
	return;
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
	err(37, __func__, "called with NULL arg(s)");
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
                exit_code = shell_cmd(__func__, false, true, "% -T -v 3 -E % -- %", fnamchk, ext, tarball_path);
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
                exit_code = shell_cmd(__func__, false, true, "% -v 3 -E % -- %", fnamchk, ext, tarball_path);
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
	warn("txzchk", "%s: %s %s failed with exit code: %d", tarball_path, fnamchk, tarball_path, WEXITSTATUS(exit_code));
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
                fnamchk_stream = pipe_open(__func__, false, true, "% -t -T -E % -- %", fnamchk, ext, tarball_path);
                if (fnamchk_stream == NULL) {
                    err(38, __func__, "popen for reading failed for: %s -- %s", fnamchk, tarball_path);
                    not_reached();
                }
            } else {
                fnamchk_stream = pipe_open(__func__, false, true, "% -T -E % -- %", fnamchk, ext, tarball_path);
                if (fnamchk_stream == NULL) {
                    err(39, __func__, "popen for reading failed for: %s -- %s", fnamchk, tarball_path);
                    not_reached();
                }
            }
        } else {
            /*
             * In real mode (not reading from a text file) we MUST check the timestamp so
             * we must NOT use the -T option to fnamchk. Depending on whether we
             * were given -x or not we will give fnamchk -t (we were given -x).
             */
            if (test_mode) {
                fnamchk_stream = pipe_open(__func__, false, true, "% -t -E % -- %", fnamchk, ext, tarball_path);
            } else {
                fnamchk_stream = pipe_open(__func__, false, true, "% -E % -- %", fnamchk, ext, tarball_path);
            }
            if (fnamchk_stream == NULL) {
                err(40, __func__, "popen for reading failed for: %s -- %s", fnamchk, tarball_path);
                not_reached();
            }
        }

	/*
	 * read the output from the fnamchk command
	 */
	readline_len = readline(&dirname, fnamchk_stream);
	if (readline_len < 0) {
	    warn("txzchk", "%s: unexpected EOF from fnamchk", tarball_path);
	}

	/*
	 * close down pipe
	 */
	errno = 0;		/* pre-clear errno for warnp() */
	ret = pclose(fnamchk_stream);
	if (ret < 0) {
	    warnp(__func__, "%s: pclose error on fnamchk stream", tarball_path);
	}

	fnamchk_stream = NULL;

	if (dirname == NULL || *dirname == '\0') {
	    err(41, __func__, "txzchk: unexpected NULL pointer from fnamchk -- %s", tarball_path);
	    not_reached();
	}
    }

    /* determine size of tarball */
    tarball.size = file_size(tarball_path);
    /* report size if too big */
    if (tarball.size < 0) {
	err(42, __func__, "%s: impossible error: txzchk_sanity_chks() found tarball but file_size() did not", tarball_path);
	not_reached();
    } else if (tarball.size > MAX_TARBALL_LEN) {
	++tarball.total_feathers;
	    fpara(stderr,
		  "",
		  "The compressed tarball exceeds the maximum allowed size, sorry.",
		  "",
		  NULL);
	    warn("txzchk", "%s: the compressed tarball size %jd > %jd",
		     tarball_path, (intmax_t)tarball.size, (intmax_t)MAX_TARBALL_LEN);
    } else if (verbosity_level) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = printf("txzchk: %s size of %jd bytes OK\n", tarball_path, (intmax_t) tarball.size);
	if (ret <= 0)
	    warnp("txzchk", "unable to tell user how big the tarball %s is", tarball_path);
    }
    dbg(DBG_MED, "txzchk: %s size in bytes: %jd", tarball_path, (intmax_t)tarball.size);

    /*
     * if txzchk -T we need to open it as a text file: for test mode
     */
    if (read_from_text_file) {
	errno = 0;		/* pre-clear errno for warnp() */
	input_stream = fopen(tarball_path, "r");
	if (input_stream == NULL)
	{
	    errp(43, __func__, "fopen of %s failed", tarball_path);
	    not_reached();
	}
	errno = 0;		/* pre-clear errno for warnp() */
	ret = setvbuf(input_stream, (char *)NULL, _IOLBF, 0);
	if (ret != 0)
	    warnp(__func__, "setvbuf failed for %s", tarball_path);

    } else {
	/*
	 * case: -T was not passed to txzchk so we have to execute tar: if we
         * cannot get a tarball listing it is an error and we abort; else we
         * open a pipe to read the output of the command.
         */

	/*
	 * first execute the tar command
	 */
	errno = 0;			/* pre-clear errno for errp() */
	if (verbosity_level) {
	    exit_code = shell_cmd(__func__, false, true, "% -tJvf %", tar, tarball_path);
	} else {
	    exit_code = shell_cmd(__func__, false, true, "% -tJvf % >/dev/null", tar, tarball_path);
	}
	if (exit_code != 0) {
	    errp(44, __func__, "%s -tJvf %s failed with exit code: %d",
			      tar, tarball_path, WEXITSTATUS(exit_code));
	    not_reached();
	}

	/* now open a pipe to tar command (tar -tJvf) to read from */
	input_stream = pipe_open(__func__, false, true, "% -tJvf %", tar, tarball_path);
	if (input_stream == NULL) {
	    err(45, __func__, "popen for reading failed for: %s -tJvf %s",
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
	    warnp("txzchk", "found NUL before end of line");
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
		warnp(__func__, "unable to printf line from text file");
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
	warnp(__func__, "%s: %s error on tar stream", tarball_path, read_from_text_file?"fclose":"pclose");
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
	err(46, __func__, "called with NULL file");
	not_reached();
    }
    if (file->filename == NULL) {
        err(47, __func__, "file->filename is NULL");
        not_reached();
    }
    if (file->perms == NULL) {
        err(48, __func__, "file->perms is NULL");
        not_reached();
    }

    if (file->isdir) {
        if (strcmp(file->perms, "drwxr-xr-x") != 0) {
	    warn("txzchk", "directory with incorrect permissions found: %s: %s != drwxr-xr-x", file->filename, file->perms);
            ++tarball.invalid_perms;
            return true;
        }
    } else if (file->isexec) {
        ++tarball.total_exec_files;
        if (count_dirs(file->filename) != 1 || (!is_executable_filename(file->basename))) {
            ++tarball.invalid_perms;
            warn("txzchk", "found executable file that is in the wrong directory or the wrong filename: %s", file->filename);
            /*
             * NOTE: the caller will increment the tarball.total_feathers so do
             * NOT do it here.
             */
            return true;
        } else if (strcmp(file->perms, "-r-xr-xr-x") != 0) {
            /*
             * we know that the directory count is 1 (submission directory
             * itself) and we know it is supposed to be 0555 but this is not so
             * we flag it
             */
            ++tarball.invalid_perms;
            warn("txzchk", "found valid executable %s with wrong permissions: %s (0555) != -r-xr-xr-x",
                    file->filename, file->perms);
            /*
             * NOTE: the caller will increment the tarball.total_feathers so do
             * NOT do it here.
             */
            return true;
        }
    } else {
        /*
         * if we get here we have to verify that the file is a specific
         * permission too, namely read only (-r--r--r--).
         */
        if (strcmp(file->perms, "-r--r--r--") != 0) {
            warn("txzchk", "found non-executable non-directory file %s with wrong permissions: %s != -r--r--r-- (0444)",
                    file->filename, file->perms);
            ++tarball.invalid_perms;
            /*
             * NOTE: the caller will increment the tarball.total_feathers so do
             * NOT do it here.
             */
            return true;
        }
    }

    /*
     * all good
     */
    return false;
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
	err(49, __func__, "passed NULL str");
	not_reached();
    }

    errno = 0;
    line = calloc(1, sizeof *line);
    if (line == NULL) {
	errp(50, __func__, "unable to allocate struct txz_line *");
	not_reached();
    }

    errno = 0;
    line->line = strdup(str);
    if (line->line == NULL) {
	errp(51, __func__, "unable to strdup string '%s' for lines list", str);
	not_reached();
    }
    line->line_num = line_num;

    dbg(DBG_VHIGH, "adding line %s to lines list", line->line);
    line->next = txz_lines;
    txz_lines = line;
}


/*
 * parse_all_txz_lines - parse all tar txz lines
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
	err(52, __func__, "passed NULL tarball_path");
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
	    err(53, __func__, "%s: duplicating %s failed", tarball_path, line->line);
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
    char *dir = NULL;       /* we need the full directory name temporarily if isdir == true */

    /*
     * firewall
     */
    if (path == NULL) {
	err(54, __func__, "passed NULL path");
	not_reached();
    }
    if (perms == NULL) {
        err(55, __func__, "passed NULL perms");
        not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    file = calloc(1, sizeof *file);
    if (file == NULL) {
	errp(56, __func__, "%s: unable to allocate a struct txz_file *", tarball_path);
	not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    file->filename = strdup(path);
    if (!file->filename) {
	errp(57, __func__, "%s: unable to strdup filename %s", tarball_path, path);
	not_reached();
    }

    /*
     * get basename of file
     */
    file->basename = base_name(path);
    if (file->basename == NULL || *(file->basename) == '\0') {
	err(58, __func__, "%s: unable to strdup basename of filename %s", tarball_path, path);
	not_reached();
    }

    /*
     * get root directory name of this file (i.e. up to the first '/')
     */
    file->top_dirname = dir_name(path, -1);
    if (file->top_dirname == NULL || *(file->top_dirname) == '\0') {
	err(59, __func__, "%s: unable to strdup top dirname of filename %s", tarball_path, path);
	not_reached();
    }
    if (isdir && dirname != NULL) {
        /* in this case we need to compare the top dirname with the name of this
         * path _after_ the trailing '/'s are removed, in order to determine if
         * this is an extra directory or the top level directory (the submission
         * directory in mkiocccentry terms)
         */
        dir = dir_name(path, 0);
        if (dir == NULL) {
            err(60, __func__, "error extracting full directory name of: %s", path);
            not_reached();
        }

        /*
         * redundant check
         */
        if (dir != NULL) {
            if (strcasecmp(dir, file->top_dirname) != 0 && strcasecmp(dirname, file->top_dirname) == 0) {
                ++tarball.directories;
                dbg(DBG_MED, "found extra directory: %s", dir);
            }
            /*
             * now we need to free the full directory name
             */
            free(dir);
            dir = NULL;
        }
    }

    /*
     * NOTE: perms was already strdup()d so we don't do that here
     */
    file->perms = perms;

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
    struct txz_file *file; /* used to iterate through list to find duplicate files */

    /*
     * firewall
     */
    if (txzfile == NULL || !txzfile->filename || !txzfile->basename) {
	err(61, __func__, "called with NULL pointer(s)");
	not_reached();
    }

    /* always increment total files count */
    ++tarball.total_files;

    for (file = txz_files; file != NULL; file = file->next) {
	if (!strcasecmp(file->filename, txzfile->filename)) {
	    dbg(DBG_MED, "incrementing count of filename %s", txzfile->filename);
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
        err(62, __func__, "file is NULL");
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

    if ((*file)->top_dirname != NULL) {
        free((*file)->top_dirname);
        (*file)->top_dirname = NULL;
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
