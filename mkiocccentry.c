/*
 * mkiocccentry - form IOCCC entry compressed tarball
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla (with a healthy
 * overdose of chocolate :-) ) C.  Why?  Because this is an obfuscated C
 * contest.  But then why isn't this code obfuscated?  Because the IOCCC judges
 * prefer to write in robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell source and finger daemon source.
 * Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * OK, we do make use of shell scripts to help build and test
 * this repo: but who doesn't use a bit of shell scripting now and then?  :-)
 * Nevertheless, the core of building your IOCCC entry compressed tarball,
 * the code that you, the IOCCC contestant will use, is all written in
 * C because this is the IOCCC.
 *
 * If you do find a problem with this code, please let the judges know.
 * To contact the judges please see:
 *
 *      https://www.ioccc.org/contact.html
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Many thanks are due to a number of people who provided important
 * and valuable testing, suggestions, issue reports and GitHub pull
 * requests to this code.  Without their time and effort, this tool
 * would not work very well!
 *
 * Among the GitHub users we wish to thank include these fine developers
 * in alphabetical GitHub username order:
 *
 *	@ilyakurdyukov		Ilya Kurdyukov
 *	@SirWumpus		Anthony Howe
 *	@vog			Volker Diels-Grabsch
 *	@xexyl			Cody Boone Ferguson
 *
 * Copyright (c) 2021-2025 by Landon Curt Noll.  All Rights Reserved.
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


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <getopt.h>
#include <string.h>
#include <strings.h>	/* strcasecmp() */
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h> /* for WEXITSTATUS() */
#include <fcntl.h> /* for open() */
#include <fts.h>
#include <limits.h> /* PATH_MAX */

/*
 * mkiocccentry - form IOCCC entry compressed tarball
 */
#include "mkiocccentry.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (2)	/* number of required arguments on the command line */
#define CHUNK (39)              /* allocate CHUNK elements at a time */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg0 =
    "usage: %s [options] workdir topdir\n"
    "\noptions:\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level: (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode (def: loud :-) )\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-V\t\tprint version string and exit\n\n"
    "\t-W\t\tignore all warnings (this does NOT mean the judges will! :) )\n"
    "\t-E\t\texit non-zero after the first warning (def: do not)\n\n"
    "\t\t\t    NOTE: One cannot use both -W and -E.\n\n"
    "\t-y\t\tanswer yes to most questions (use with EXTREME caution!)\n";
static const char * const usage_msg1 =
    "\t-t tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-l ls\t\tpath to ls executable (def: %s)\n"
    "\t-T txzchk\tpath to txzchk executable (def: %s)\n"
    "\t-e\t\tentertainment mode\n"
    "\t-F fnamchk\tpath to fnamchk executable used by txzchk (def: %s)";
static const char * const usage_msg2 =
    "\t-C chkentry	path to chkentry executable (def: %s)\n";
static const char * const usage_msg3 =
    "\t-a answers\twrite answers to a file for easier updating of a submission\n"
    "\t-A answers\twrite answers file even if it already exists\n"
    "\t-i answers\tread answers from file previously written by -a|-A answers\n\n"
    "\t\t\t    NOTE: One cannot use both -a/-A answers and -i answers.\n"
    "\n"
    "\t-s seed\t\tGenerate and use pseudo-random answers, seeding with seed & 0x%08u (def: do not)\n"
    "\t-d\t\tAlias for -s %u\n"
    "\n"
    "\t\t\t    NOTE: Implies -y -E -A random_answers.seed and reads answers from random_answers.seed\n"
    "\t\t\t    NOTE: One cannot use -a/-A or -i with -s seed/-d.\n"
    "\n";
static const char * const usage_msg4 =
    "\tworkdir\t\tdirectory where the submission directory and tarball are formed\n"
    "\ttopdir\t\tdirectory with required, optional and extra files to be copied to tarball\n";
static const char * const usage_msg5 =
    "\n"
    "Exit codes:\n"
    "     0   all OK\n"
    "     1   -E and some warning was issued\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   invalid command line, invalid option or option missing an argument\n"
    " >= 10   internal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";

/*
 * globals
 */
static bool quiet = false;			/* true ==> quiet mode */
static bool need_confirm = true;	/* true ==> ask for confirmations */
static bool need_hints = true;		/* true ==> show hints */
static bool ignore_warnings = false;	/* true ==> ignore all warnings (this does NOT mean the judges will! :) */
static bool abort_on_warning = false;	/* true ==> exit non-zero after first warning */
static FILE *input_stream = NULL;	/* input file: stdin or answers file  */
static unsigned answers_errors;		/* > 0 ==> output errors on answers file */
static bool answer_yes = false;		/* true ==> -y used: always answer yes (use with EXTREME caution!) */
static bool entertain = false;          /* for -e mode for txzchk -e */
static bool silence_prompt = false;	/* true ==> do not display prompts */
static bool read_answers_flag_used = false;	/* true ==> -i read answers from answers file */
static bool seed_used = false;		/* true ==> -d or -s seed given */

/*
 * forward declarations
 */
static void usage(int exitcode, char const *program, char const *str) __attribute__((noreturn));
static bool noprompt_yes_or_no(void);
static bool append_unique_str(struct dyn_array *tbl, char *str);
static int filename_cmp(void const *a, void const *b);

int
main(int argc, char *argv[])
{
    char const *program = NULL;			/* our name */
    extern char *optarg;			/* option argument */
    extern int optind;				/* argv index of the next arg */
    struct timeval tp;				/* gettimeofday time value */
    char const *workdir = NULL;		/* where the submission directory and tarball are formed */
    char *prog_c = NULL;			/* path to prog.c */
    char *Makefile = NULL;		/* path to Makefile */
    char *remarks_md = NULL;		/* path to remarks.md */
    char **topdir = NULL;          /* directory from which files are to be copied to the workdir */
    char *tar = TAR_PATH_0;			/* path to tar executable that supports the -J (xz) option */
    char *ls = LS_PATH_0;			/* path to ls executable */
    char *txzchk = TXZCHK_PATH_0;		/* path to txzchk executable */
    char *fnamchk = FNAMCHK_PATH_0;		/* path to fnamchk executable */
    char *chkentry = CHKENTRY_PATH_0;		/* path to chkentry executable */
    char *answers = NULL;			/* path to the answers file (recording input given on stdin) */
    FILE *answerp = NULL;			/* file pointer to the answers file */
    char *submission_dir = NULL;		/* submission directory from which to form a compressed tarball */
    char *tarball_path = NULL;			/* path of the compressed tarball to form */
    int extra_count = 0;			/* number of extra files */
    struct info info;				/* data to form .info.json */
    struct auth auth;				/* data to form .auth.json */
    int author_count = 0;			/* number of authors */
    struct author *author_set = NULL;		/* list of authors */
    bool tar_flag_used = false;			/* true ==> -t /path/to/tar was given */
    bool ls_flag_used = false;			/* true ==> -l /path/to/ls was given */
    bool answers_flag_used = false;		/* true ==> -a write answers to answers file */
    bool overwrite_answers_flag_used = false;	/* true ==> don't prompt to overwrite answers if it already exists */
    bool txzchk_flag_used = false;		/* true ==> -T /path/to/txzchk was given */
    bool fnamchk_flag_used = false;		/* true ==> -F /path/to/fnamchk was given */
    bool chkentry_flag_used = false;		/* true ==> -C /path/to/chkentry was given */
    bool overwrite_answers = true;		/* true ==> overwrite answers file even if it already exists */
    RuleCount size;				/* rule_count() processing results */
    int ret;					/* libc return code */
    int i;

    /*
     * parse args
     */
    input_stream = stdin;	/* default to reading from standard in */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:qVt:l:a:i:A:WT:eF:C:yds:")) != -1) {
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
	case 'J': /* -J json_verbosity */
	     /*
             * parse json verbosity level
             */
            json_verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -J json_verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit 2 */
	    print("%s version: %s\n", MKIOCCCENTRY_BASENAME, MKIOCCCENTRY_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 't':		/* -t /path/to/tar */
	    tar = optarg;
	    tar_flag_used = true;
	    break;
	case 'l':		/* -l /path/to/ls */
	    ls = optarg;
	    ls_flag_used = true;
	    break;
	case 'A':		/* -A answers overwrite answers file */
	    answers = optarg;
	    overwrite_answers_flag_used = true;
	    answers_flag_used = true;
	    break;
	case 'a':		/* -a record_answers */
	    answers = optarg;
	    answers_flag_used = true;
	    break;
	case 'i':		/* -i input_recorded_answers */
	    answers = optarg;
	    read_answers_flag_used = true;
	    need_confirm = false;
	    need_hints = false;
	    silence_prompt = true;
	    break;
	case 'W':		/* -W ignores all warnings (this does NOT the judges will! :) ) */
	    ignore_warnings = true;
	    break;
	case 'E':		/* -E exit non-zero after first warning */
	    abort_on_warning = true;
	    break;
	case 'T':
	    txzchk_flag_used = true;
	    txzchk = optarg;
	    break;
        case 'e':
            entertain = true;
            break;
	case 'F':
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 'C':
	    chkentry_flag_used = true;
	    chkentry = optarg;
	    break;
	case 'y':
	    answer_yes = true;
	    need_confirm = false;
	    break;
	case 'd':		/* alias for -s DEFAULT_SEED */
	    answer_seed = DEFAULT_SEED;
	    seed_used = true;
	    /* set -y */
	    answer_yes = true;
	    /* set -i input_recorded_answers booleans except for read_answers_flag_used */
	    need_confirm = false;
	    need_hints = false;
	    silence_prompt = true;
	    /* set -E boolean */
	    abort_on_warning = true;
	    break;
	case 's':		/* set seed as seed & SEED_MASK */
	    /*
	     * parse seed
	     */
	    errno = 0;		/* pre-clear errno for warnp() */
	    answer_seed = (long)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		warnp(__func__, "invalid -s argument, must be an integer >= 0, disabling -s seed");
		answer_seed = NO_SEED;
	    } else if (answer_seed < 0) {
		warnp(__func__, "invalid -s argument, must be >= 0, disabling -s seed");
		answer_seed = NO_SEED;
	    } else {
		seed_used = true;
		/* set -y */
		answer_yes = true;
		/* set -i input_recorded_answers booleans except for read_answers_flag_used */
		need_confirm = false;
		need_hints = false;
		silence_prompt = true;
		/* set -E boolean */
		abort_on_warning = true;
	    }
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    check_invalid_option(program, i, optopt);
	    usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	}
    }
    /* must have at least the required number of args */
    if (argc - optind < REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    /*
     * guess where tar and ls utilities are located
     *
     * If the user did not give a -t, -c and/or -l /path/to/x path, then look at
     * the historic location for the utility.  If the historic location of the utility
     * isn't executable, look for an executable in the alternate location.
     *
     * On some systems where /usr/bin != /bin, the distribution made the mistake of
     * moving historic critical applications, look to see if the alternate path works instead.
     */
    find_utils(tar_flag_used, &tar, ls_flag_used, &ls,
	       txzchk_flag_used, &txzchk, fnamchk_flag_used, &fnamchk,
	       chkentry_flag_used, &chkentry);

    /*
     * check that conflicting answers file options are not used together
     */
    if (answers_flag_used && read_answers_flag_used) {
	err(3, __func__, "-a answers and -i answers cannot be used together"); /*ooo*/
	not_reached();
    }
    if (seed_used && ignore_warnings) {
	err(3, __func__, "-W cannot be used with with either -d or -s seed"); /*ooo*/
	not_reached();
    }
    if (seed_used && answers_flag_used) {
	err(3, __func__, "-a answers cannot be used with with either -d or -s seed"); /*ooo*/
	not_reached();
    }
    if (seed_used && overwrite_answers_flag_used) {
	err(3, __func__, "-A answers cannot be used with with either -d or -s seed"); /*ooo*/
	not_reached();
    }
    if (seed_used && read_answers_flag_used) {
	err(3, __func__, "-i answers cannot be used with with either -d or -s seed"); /*ooo*/
	not_reached();
    }
    if (ignore_warnings && abort_on_warning) {
	err(3, __func__, "-W and -E cannot be used with together"); /*ooo*/
	not_reached();
    }

    /*
     * if we are seeding (-s seed or -d), setup seed and answers file
     */
    if (seed_used) {

	int answer_len;	    /* length of the answers filename we will form */

	/*
	 * seed using random offset and srandom arg mask
	 *
	 * We add the non-zero SEED_OFFSET_BIAS value so that -s 0
	 * will not simply select the default 0 argument value.  Why?
	 * Well, why not?  :-)  Sure, someone could use -s 2147470909
	 * to get the default 0 argument value, but they'd have to really
	 * try. :-)
	 *
	 * BTW: There is nothing wrong with using the default 0 argument,
	 *	we are just having fun.
	 */
	srandom((unsigned)((answer_seed+SEED_OFFSET_BIAS) & SEED_MASK));

	/*
	 * form the answers filename based on seed
	 *
	 * We need to form the filename:
	 *
	 *	random_answers.answer_seed
	 */
	answer_len = (sizeof("random_answers.")-1) + SEED_DECIMAL_DIGITS + 1 + 1;   /* +1 for NULL, +1 for paranoia */
	answers = malloc((size_t)answer_len + 1);	/* +1 for paranoia */
	if (answers == NULL) {
	    err(3, __func__, "failed to malloc %d bytes for answers filename", answer_len + 1); /*ooo*/
	    not_reached();
	}
	answers[answer_len] = '\0';		/* paranoia */
	ret = snprintf(answers, answer_len, "random_answers.%u", (unsigned)answer_seed);
	if (ret <= 0) {
	    errp(3, __func__, "snprintf of random_answers.%u filename failed", (unsigned)answer_seed); /*ooo*/
	    not_reached();
	}

	/*
	 * load the random_answers.answer_seed file with randomly generated answers
	 */
	/* temporarily turn on -A random_answers.seed */
	overwrite_answers_flag_used = true;
	answers_flag_used = true;
	/* write random answers into random_answers.seed */
	generate_answers(answers);
	/* turn off -A random_answers.seed */
	overwrite_answers_flag_used = false;
	answers_flag_used = false;
	/* turn on -i random_answers.seed */
	read_answers_flag_used = true;
    }

    dbg(DBG_MED, "tar: %s", tar);
    dbg(DBG_MED, "ls: %s", ls);
    workdir = argv[optind];
    dbg(DBG_MED, "workdir: %s", workdir);
    topdir = argv + optind + REQUIRED_ARGS - 1;
    if (topdir == NULL) {
        err(245, __func__, "topdir is NULL");
        not_reached();
    }
    if (answers != NULL) {
        dbg(DBG_MED, "answers file: %s", answers);
    }
    if (seed_used) {
	dbg(DBG_MED, "pseudo random seed: %u", (unsigned)answer_seed);
    }

    /*
     * zeroize info
     */
    memset(&info, 0, sizeof(info));

    /*
     * record the time
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = gettimeofday(&tp, NULL);
    if (ret < 0) {
	errp(246, __func__, "gettimeofday failed");
	not_reached();
    }
    info.tstamp = tp.tv_sec;
    if ((time_t)(-1) > 0) {
	/* case: unsigned time_t */
	dbg(DBG_HIGH, "info.tstamp: %ju", (uintmax_t)info.tstamp);
    } else {
	/* case: signed time_t */
	dbg(DBG_HIGH, "info.tstamp: %jd", (intmax_t)info.tstamp);
    }
    info.usec = tp.tv_usec;
    dbg(DBG_HIGH, "info.usec: %jd", (intmax_t)info.usec);

    /*
     * XXX - for now we have to have no extra count - XXX
     */
    info.extra_count = extra_count;

    /*
     * Welcome
     */
    if (!quiet) {
	print("Welcome to mkiocccentry version: %s\n", MKIOCCCENTRY_VERSION);
    }

    /*
     * save our version
     */
    info.mkiocccentry_ver = MKIOCCCENTRY_VERSION;
    dbg(DBG_HIGH, "info.mkiocccentry_ver: %s", info.mkiocccentry_ver);

    /* warn about -y option */
    if (answer_yes) {
	para("",
	     "WARNING: you've chosen to answer yes to almost all prompts. If this was",
	     "unintentional, run the program again without specifying -y. We cannot",
	     "stress the importance of this enough! Well OK, we can over-stress most things",
	     "but you get the point. Do not use the -y option without EXTREME caution!",
	     "",
	     NULL);
    }

    /* if the user requested to ignore warnings, and now -E, then ignore this once and warn them :) */
    if (ignore_warnings) {
	para("",
	     "WARNING: You've chosen to ignore all warnings. While we will not show",
	     "you any additional warnings, you should note that The Judges will NOT",
	     "ignore warnings! If this was unintentional, run the program again",
	     "without specifying -W. We cannot stress the importance of this enough!",
	     "Well OK, we can over-stress most things, but you get the point.",
	     "Do not use the -W option!",
	     "",
	     NULL);
    }
    /*
     * environment sanity checks
     */
    if (!quiet) {
	para("", "Performing sanity checks on your environment ...", NULL);
    }
    mkiocccentry_sanity_chks(&info, workdir, tar, ls, txzchk, fnamchk, chkentry);
    if (!quiet) {
	para("... environment looks OK", "", NULL);
    }
    /*
     * if -a answers was specified and answers file exists, prompt user if they
     * want to overwrite it; if they don't tell them how to use it and abort.
     * Else it will be overwritten.
     */
    if (answers_flag_used && !overwrite_answers_flag_used && answers != NULL && strlen(answers) > 0 && exists(answers)) {
	overwrite_answers = yes_or_no("WARNING: The answers file already exists! Do you wish to overwrite it? [yn]");
	if (!overwrite_answers) {
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = printf("\nTo use the answers file, try:\n\n\t./mkiocccentry -i %s [...]\n\n", answers);
	    if (ret <= 0) {
		errp(247, __func__, "printf error telling the user how to use the answers file");
		not_reached();
	    }
	    err(248, __func__, "won't overwrite answers file");
	    not_reached();
	}
    }

    /*
     * check if we should read input from answers file
     */
    if (read_answers_flag_used && answers != NULL && strlen(answers) > 0) {
	if (!is_read(answers)) {
	    errp(249, __func__, "answers file not readable");
	    not_reached();
	}
	errno = 0;		/* pre-clear errno for errp() */
	answerp = fopen(answers, "r");
	if (answerp == NULL) {
	    errp(10, __func__, "cannot open answers file");
	    not_reached();
	}
	input_stream = answerp;
    }
    /*
     * obtain the IOCCC contest ID
     */
    info.ioccc_id = get_contest_id(&info.test_mode, &read_answers_flag_used);
    dbg(DBG_LOW, "Submission: IOCCC contest ID: %s", info.ioccc_id);

    /*
     * found the answer file header in stdin
     */
    if (read_answers_flag_used && answers == NULL) {
	answerp = stdin;
    }

    /*
     * obtain submit slot number
     */
    info.submit_slot = get_submit_slot(&info);
    dbg(DBG_LOW, "Submission: slot number: %d", info.submit_slot);

    /*
     * create submission directory
     */
    submission_dir = mk_submission_dir(workdir, info.ioccc_id, info.submit_slot, &tarball_path, info.tstamp, info.test_mode);
    errno = 0;
    info.tarball = strdup(tarball_path);
    if (info.tarball == NULL) {
	errp(11, __func__, "strdup() tarball path %s failed", tarball_path);
	not_reached();
    }

    dbg(DBG_MED, "Directory formed: %s", submission_dir);

    /*
     * if -a, open the answers file. We only do it after verifying that we can
     * make the submission directory because if we get input before and find out the
     * directory already exists then the answers file will have invalid data.
     */
    if (answers_flag_used && answers != NULL && strlen(answers) > 0) {

	errno = 0;			/* pre-clear errno for errp() */
	answerp = fopen(answers, "w");
	if (answerp == NULL) {
	    errp(12, __func__, "cannot create answers file: %s", answers);
	    not_reached();
	}
        ret = fprintf(answerp, "%s\n", MKIOCCCENTRY_ANSWERS_VERSION);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing header to the answers file");
	    ++answers_errors;
	}
    }

    /*
     * write the IOCCC id and submit slot number to the answers file
     */
    if (answerp != NULL && answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
        ret = fprintf(answerp, "%s\n", info.ioccc_id);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing IOCCC contest id to the answers file");
	    ++answers_errors;
	}
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(answerp, "%d\n", info.submit_slot);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing submit slot number to the answers file");
	    ++answers_errors;
	}
    }

    /*
     * scan and collect files in topdir, copying to the submission directory
     */
    collect_topdir_files(topdir, &info, submission_dir, &size);


    /*
     * obtain the title
     */
    info.title = get_title(&info);
    dbg(DBG_LOW, "Submission: title: %s", info.title);
    if (answerp != NULL && answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(answerp, "%s\n", info.title);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing title to the answers file");
	    ++answers_errors;
	}
    }

    /*
     * obtain the abstract
     */
    info.abstract = get_abstract(&info);
    dbg(DBG_LOW, "Submission: abstract: %s", info.abstract);
    if (answerp != NULL && answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(answerp, "%s\n", info.abstract);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing abstract to the answers file");
	    ++answers_errors;
	}
    }

    /*
     * obtain author information
     */
    author_count = get_author_info(&author_set);
    dbg(DBG_MED, "collected information on %d authors", author_count);

    /*
    * if we have an answers file, record the verified author information
    */
    if (answerp != NULL && !read_answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
        ret = fprintf(answerp, "%d\n", author_count);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing IOCCC author count to the answers file");
	    ++answers_errors;
	}

	/*
	 * write answers for each author to the answers file
	 */
	for (i = 0; i < author_count; i++) {
	    errno = 0;			/* pre-clear errno for warnp() */
	    ret = fprintf(answerp,
		"%s\n"	/* name */
		"%s\n"	/* location code */
		"%s\n"	/* email */
		"%s\n"	/* url */
		"%s\n"	/* alt_url */
		"%s\n"	/* mastodon handle */
		"%s\n"	/* GitHub */
		"%s\n"	/* affiliation */
		"%s\n"  /* past winning author */
		"%s\n"  /* author_handle */
		,
		author_set[i].name,
		author_set[i].location_code,
		author_set[i].email,
		author_set[i].url,
		author_set[i].alt_url,
		author_set[i].mastodon,
		author_set[i].github,
		author_set[i].affiliation,
		author_set[i].past_winning_author?"y":"n",
		author_set[i].author_handle);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author info to the answers file");
		++answers_errors;
	    }
	}
    }

    /*
     * write the .info.json file
     */
    if (!quiet) {
	para("", "Forming the .info.json file ...", NULL);
    }
    write_info(&info, submission_dir, chkentry, fnamchk);
    if (!quiet) {
	para("... completed the .info.json file.", "", NULL);
    }

    /*
     * form auth
     */
    form_auth(&auth, &info, author_count, author_set);

    /*
     * write the .auth.json file
     */
    if (!quiet) {
	para("", "Forming the .auth.json file ...", NULL);
    }
    write_auth(&auth, submission_dir, chkentry, fnamchk);
    if (!quiet) {
	para("... completed .auth.json file.", "", NULL);
    }

    /*
     * finalize the answers file: either write the final answers (if writing
     * answers) or read the answers EOF marker and then finally closing the
     * stream.
     */
    if (answerp != NULL) {
	if (read_answers_flag_used) {
	    char *linep = NULL;
	    char *line;
	    bool error = true;

	    line = readline_dup(&linep, true, NULL, answerp);
	    if (line != NULL) {
		if (line != NULL) {
		    error = strcmp(line, MKIOCCCENTRY_ANSWERS_EOF) != 0;
		    free(linep);
		    linep = NULL;
		} else {
		    error = false;
		}
	    }
	    if (error) {
	        errp(13, __func__, "expected ANSWERS_EOF marker at the end of the answers file");
	        not_reached();
	    }
	    input_stream = stdin;
	    if (line != NULL) {
		free(line);
		line = NULL;
	    }
	} else {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(answerp, "%s\n", MKIOCCCENTRY_ANSWERS_EOF);
	    if (ret <= 0) {
	        warnp(__func__, "fprintf error writing ANSWERS_EOF marker to the answers file");
		++answers_errors;
	    }
	}
	if (answers != NULL) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fclose(answerp);
	    if (ret != 0) {
	        warnp(__func__, "error in fclose to the answers file");
		++answers_errors;
	    }
	}
	answerp = NULL;
    }

    /*
     * remind the user about their answers file
     */
    if (answers != NULL) {
	if (need_hints) {
	    errno = 0;			/* pre-clear errno for warnp() */
	    ret = printf("\nTo more easily update this submission you can run:\n\n    ./mkiocccentry -i %s ...\n", answers);
	    if (ret <= 0) {
		warnp(__func__, "unable to tell user how to more easily update submission");
	    }
	}
    }

    /*
     * warn the user if there were I/O errors while writing the answers file
     */
    if (answers_flag_used) {
	if (answers_errors > 0) {
	    errno = 0;	/* pre-clear errno for warnp() */
	    ret = printf("Warning: There were %u I/O error%s on the answers file. Make SURE to verify that using the file\n"
			 "results in the proper input before re-uploading!\n",
			 answers_errors, answers_errors == 1 ? "" : "s" );
	    if (ret <= 0) {
		warnp(__func__, "unable to warn user that there were I/O errors on the answers file");
	    }
	}
    }

    /*
     * If the answers file (-i answers or -d or -s seed) was used, and there were warnings/problems
     * discovered with the submission that were overridden, warn the user.
     */
    if (read_answers_flag_used || seed_used) {
	if (info.empty_override ||
	    info.rule_2a_override ||
	    info.rule_2a_mismatch ||
	    info.rule_2b_override ||
	    info.nul_warning ||
	    info.trigraph_warning ||
	    info.wordbuf_warning ||
	    info.ungetc_warning ||
	    info.Makefile_override) {

	    do {
		if (!ignore_warnings) {
		    need_confirm = true;

		    if (info.empty_override) {
			warn_empty_prog();
		    }
		    if (info.rule_2a_override) {
			warn_rule_2a_size(&info, RULE_2A_BIG_FILE_WARNING, size);
		    }
		    if (info.rule_2a_mismatch) {
			warn_rule_2a_size(&info, RULE_2A_IOCCCSIZE_MISMATCH, size);
		    }
		    if (info.rule_2b_override) {
			warn_rule_2b_size(&info);
		    }
		    if (info.nul_warning) {
			warn_nul_chars();
		    }
		    if (info.trigraph_warning) {
			warn_trigraph();
		    }
		    if (info.wordbuf_warning) {
			warn_wordbuf();
		    }
		    if (info.ungetc_warning) {
			warn_ungetc();
		    }
		    if (info.Makefile_override) {
			warn_Makefile(&info);
		    }
		}
	    } while (0);
	}
    }

    /*
     * form the .txz file
     *
     * NOTE: this function will, if it successfully creates the tarball, run
     * txzchk on it, which will run fnamchk on it.
     */
    form_tarball(workdir, submission_dir, tarball_path, tar, ls, txzchk, fnamchk);

    /*
     * remind user various things e.g., to upload (unless in test mode)
     */
    remind_user(workdir, submission_dir, tar, tarball_path, info.test_mode, info.submit_slot);

    /*
     * free storage
     */
    if (submission_dir != NULL) {
	free(submission_dir);
	submission_dir = NULL;
    }
    if (tarball_path != NULL) {
	free(tarball_path);
	tarball_path = NULL;
    }
    free_info(&info);
    memset(&info, 0, sizeof(info));
    free_auth(&auth);
    memset(&auth, 0, sizeof(auth));

    if (prog_c != NULL) {
        free(prog_c);
        prog_c = NULL;
    }
    if (Makefile != NULL) {
        free(Makefile);
        Makefile = NULL;
    }

    if (remarks_md != NULL) {
        free(remarks_md);
        remarks_md = NULL;
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(0); /*ooo*/
}

/*
 * has_ignored_dirname   - check if path has ignored dirname in it
 *
 * given:
 *
 *      path    - path to check
 *
 * NOTE: this function returns false if path is NULL.
 */
static
bool has_ignored_dirname(char const *path)
{
    size_t i = 0;
    /*
     * firewall
     */
    if (path == NULL) {
        return false;
    } else if (*path == '\0') {
        return false;
    }

    for (i = 0; ignored_dirnames[i] != NULL; ++i) {
        if (path_has_component(path, ignored_dirnames[i])) {
            return true;
        }
    }

    return false;
}

/*
 * append_unique_str - append string pointer to dynamic array if not already found
 *
 * Given a pointer to string, we search a dynamic array of pointers to strings.
 * If an exact match is found (i.e. the string is already in the dynamic array),
 * nothing is done other than to return false.  If no match is found, the pointer
 * to the string is appended to the dynamic array and we return true.
 *
 * given:
 *	tbl		dynamic array of pointers to strings
 *	str		string to search tbl and append if not already found
 *
 * returns:
 *	true		str was not already in dynamic array and has now been appended
 *	false		str was already in the dynamic array, table is unchanged
 *
 * NOTE: This function does not return if given NULL pointers or on other errors.
 */
static bool
append_unique_str(struct dyn_array *tbl, char *str)
{
    intmax_t unique_len = 0;	/* number of unique function name entries */
    char *u = NULL;		/* unique name pointer */
    intmax_t i;

    /*
     * firewall
     */
    if (tbl == NULL) {
	err(14, __func__, "tbl is NULL");
	not_reached();
    }
    if (str == NULL) {
	err(15, __func__, "str is NULL");
	not_reached();
    }

    /*
     * search tbl for the string
     *
     * NOTE: We realize calling the function with unique strings will
     *	     cause the execution time to grow as O(n^2).  However the
     *	     usual number of strings in a unique function name dynamic array
     *	     is almost certainly small.  Therefore we do not need to
     *	     employ a more optimized dynamic array search mechanism.
     */
    unique_len = dyn_array_tell(tbl);
    for (i=0; i < unique_len; ++i) {

	/* get next string pointer */
	u = dyn_array_value(tbl, char *, i);
	if (u == NULL) {	/* paranoia */
	    err(16, __func__, "found NULL pointer in function name dynamic array element: %ju", (uintmax_t)i);
	    not_reached();
	}

	/* look for match */
	if (strcmp(str, u) == 0) {
	    /* str found in function name dynamic array, not unique */
	    return false;
	}
    }

    /*
     * function name is unique, append to function name dynamic array
     */
    (void) dyn_array_append_value(tbl, &str);
    return true;	/* pointer to string was appended */
}

/*
 * filename_cmp - compare two semantic table entries
 *
 * Produce a semantic table that is in some canonical order.
 *
 * NOTE: Since the table is not very large, we do not need to
 *	 optimize lookups in this table.  It is better to reverse
 *	 sort by depth for human understanding.  This will help
 *	 put the more common nodes early in the table.
 *
 * given:
 *	a	pointer to first semantic table entry to compare
 *	b	pointer to second semantic table entry to compare
 *
 * returns:
 *      -1      a < b
 *      0       a == b
 *      1       a > b
 */
static int
filename_cmp(void const *a, void const *b)
{
    const char *first = NULL;	/* first entry to compare */
    const char *second = NULL;	/* second entry to compare */
    int cmp = 0;		/* byte string comparison */

    /*
     * firewall
     */
    if (a == NULL) {
	err(17, __func__, "a is NULL");
	not_reached();
    }
    if (b == NULL) {
	err(18, __func__, "b is NULL");
	not_reached();
    }
    first = (const char *)a;
    second = (const char *)b;

    if (first != NULL) {
	if (second != NULL) {
	    cmp = strcmp(first, second);
	    if (cmp > 0) {
		return 1;	/* first > second */
	    } else if (cmp < 0) {
		return -1;	/* first < second */
	    }
	} else {
	    return -1;	/* first < NULL second */
	}
    } else if (second != NULL) {
	return 1;	/* NULL first > second */
    }
    /* case: name matches */

    /*
     * entries match
     */
    return 0;
}



/*
 * collect_topdir_files
 *
 * Count total number of files in topdir, checking for sane relative paths with
 * mkiocccentry limits in mind.
 *
 * XXX - although this does copy files to their correct directories, not all
 * steps are implemented which means this function is subject to change. In the
 * comments below some things that will have to change are noted but there are
 * actually more as not all steps are even in the function (it might also be
 * good to break the function up into several but that can happen once
 * everything is implemented, unless it proves useful to do prior to finishing
 * it).
 *
 * When this function finds prog.c, Makefile or remarks.md, it uses the
 * check_prog_c, check_Makefile or check_remarks_md functions, which will copy
 * the file to the correct directory if all checks are okay. Later on these
 * checks will be done when looking at the files list.
 *
 * given:
 *      args            - the args (topdir)
 *      infop           - pointer to struct info
 *      submission_dir  - submission directory
 *      size            - pointer to RuleCount (for iocccsize check on prog.c)
 *
 * Returns:
 *      the total number of EXTRA files that were counted, not counting directories
 *
 * NOTE: if function is passed a NULL pointer we return 0.
 * NOTE: if an error is encountered traversing the path(s) it is an error.
 * NOTE: if the depth becomes too deep or if a filename length is too long or
 *       there are too many files it is an error.
 * NOTE: if we can't get a certain directory (absolute path or file descriptor)
 * or a change in directory fails or a function returns NULL it is an error.
 * NOTE: if any of prog.c, Makefile or remarks.md do not exist it is an error.
 * NOTE: this function does not return on error.
 */
static size_t
collect_topdir_files(char * const *args, struct info *infop, char const *submission_dir,
        RuleCount *size)
{
    struct dyn_array *ignored_files = NULL;     /* ignored files */
    struct dyn_array *files_list = NULL;        /* files to be added to tarball */
    struct dyn_array *directories = NULL;       /* directories seen */
    struct dyn_array *ignored_dirs = NULL;      /* ignored directories */
    struct dyn_array *forbidden_files = NULL;   /* forbidden files */
    char *filename = NULL;                      /* current filename (for arrays) */
    char *fname = NULL;                         /* filename can't be freed so we need another variable */
    char *p = NULL;                             /* temp value to print lists (arrays) */
    intmax_t len = 0;
    intmax_t i = 0;
    FTS *fts = NULL;
    FTSENT *item = NULL;
    size_t count = 0;                   /* total number of valid files */
    enum path_sanity sanity = PATH_OK;  /* assume path is okay first */
    char *path[] = { ".", NULL };
    int cwd = -1;		    /* current working directory */
    int ret;			    /* libc function return */
    int topdir = -1;                /* topdir */
    char topdir_path[PATH_MAX+1];   /* absolute path of topdir for copying files */
    char submit_path[PATH_MAX+1];   /* we need the absolute path to the submission directory for copying files */
    char *target_path = NULL;       /* target file path (absolute) */
    bool found_prog_c = false;      /* if prog.c found */
    bool found_Makefile = false;    /* if Makefile found */
    bool found_remarks_md = false;  /* if remarks.md found */
    bool yorn = false;              /* for prompts to ask user if everything is OK */

    /*
     * firewall
     */
    if (args == NULL || args[0] == NULL || infop == NULL || submission_dir == NULL || size == NULL) {
        return 0;
    }


    ignored_files = dyn_array_create(sizeof(char *), CHUNK, CHUNK, true);
    if (ignored_files == NULL) {
        err(19, __func__, "failed to create ignored files list array");
        not_reached();
    }
    files_list = dyn_array_create(sizeof(char *), CHUNK, CHUNK, true);
    if (files_list == NULL) {
        err(20, __func__, "failed to create files list array");
        not_reached();
    }
    directories = dyn_array_create(sizeof(char *), CHUNK, CHUNK, true);
    if (directories == NULL) {
        err(21, __func__, "failed to create directories list array");
        not_reached();
    }
    ignored_dirs = dyn_array_create(sizeof(char *), CHUNK, CHUNK, true);
    if (ignored_dirs == NULL) {
        err(22, __func__, "failed to create ignored directories list array");
        not_reached();
    }
    forbidden_files = dyn_array_create(sizeof(char *), CHUNK, CHUNK, true);
    if (forbidden_files == NULL) {
        err(23, __func__, "failed to create forbidden files list array");
        not_reached();
    }


    /*
     * note the current directory so we can restore it later
     */
    errno = 0;			/* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
	errp(24, __func__, "cannot open .");
	not_reached();
    }

    /*
     * change to submission directory so we can get its descriptor and absolute
     * path (for copying files)
     */
    errno = 0; /* pre-clear errno for errp() */
    if (chdir(submission_dir) != 0) {
        errp(25, __func__, "unable to change to submission directory");
        not_reached();
    }

    /*
     * get absolute path of submission directory for copying files
     */
    errno = 0; /* pre-clear errno for errp() */
    if (getcwd(submit_path, PATH_MAX) == NULL) {
        errp(26, __func__, "couldn't get absolute path of submission directory");
        not_reached();
    }

    /*
     * we need to move back to cwd for a moment
     */
    errno = 0; /* pre-clear errno for errp() */
    if (fchdir(cwd) != 0) {
        errp(27, __func__, "couldn't change back to cwd");
        not_reached();
    }

    /*
     * now we have to get to the directory specified before we can collect
     * files.
     *
     * NOTE: this is the topdir in the mkiocccentry command line.
     */
    errno = 0;      /* pre-clear errno for errp() */
    if (chdir(args[0]) != 0) {
        errp(28, __func__, "chdir(\"%s\") failed", args[0]);
        not_reached();
    }
    /*
     * note the topdir so we can restore it later, after the changing back to
     * the working directory below, so that when we read more files in the tree,
     * we are in the right location.
     */
    errno = 0;			/* pre-clear errno for errp() */
    topdir = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (topdir < 0) {
	errp(29, __func__, "cannot open topdir");
	not_reached();
    }

    /*
     * we now need to get the absolute path of the topdir for copying files
     * later on
     */
    errno = 0; /* pre-clear errno for errp() */
    if (getcwd(topdir_path, PATH_MAX) == NULL) {
        errp(30, __func__, "couldn't get absolute path of topdir");
        not_reached();
    }


    /*
     * now that we have changed to the correct directory and gathered everything
     * we need to copy files, we can traverse the tree.
     */
    errno = 0;      /* pre-clear errno for errp() */
    fts = fts_open(path, FTS_NOCHDIR | FTS_PHYSICAL, NULL);
    if (fts == NULL) {
        errp(31, __func__, "fts_open() for path returned NULL");
        not_reached();
    } else {
        while ((item = fts_read(fts)) != NULL) {
            bool ignored = has_ignored_dirname(item->fts_path + 2);
            bool forbidden = is_forbidden_filename(item->fts_path + 2);
            bool optional = is_optional_filename(item->fts_path + 2);

            if (ignored) {
                if (item->fts_info == FTS_D) {
                    /*
                     * we don't want to traverse below ignored directories
                     */
                    errno = 0;  /* pre-clear errno for errp() */
                    if (fts_set(fts, item, FTS_SKIP) != 0) {
                        errp(32, __func__, "fts_set() failed to set FTS_SKIP for %s", item->fts_path + 2);
                        not_reached();
                    }
                }
                continue;
            } else if (forbidden) {
                /*
                 * we have to add the path of the forbidden file to the
                 * forbidden files list
                 */
                errno = 0; /* pre-clear errno for errp() */
                filename = strdup(item->fts_path + 2);
                if (filename == NULL) {
                    errp(33, __func__, "strdup(\"%s\") failed", item->fts_path + 2);
                    not_reached();
                }
                append_unique_str(forbidden_files, filename);
                continue;
            }
            /*
             * NOTE: when traversing the directory "." the filenames found
             * under it will all start with "./". This is why the last
             * arg to sane_relative_path() is true: it allows the first
             * two characters to be "./" (this does NOT mean that ".//"
             * is okay and it does NOT mean that "././" is okay).
             */
            sanity = sane_relative_path(item->fts_path, MAX_PATH_LEN, MAX_FILENAME_LEN, MAX_PATH_DEPTH, true);
            if (sanity == PATH_ERR_NAME_TOO_LONG) {
                err(34, __func__, "%s: name too long: strlen(\"%s\"): %ju > %ju", item->fts_name + 2, item->fts_name,
                        (uintmax_t)strlen(item->fts_name), (uintmax_t)MAX_FILENAME_LEN);
                not_reached();
            }
            if (sanity == PATH_ERR_PATH_TOO_LONG) {
                err(35, __func__, "%s: path too long: strlen(\"%s\"): %ju > %ju", item->fts_path + 2, item->fts_path,
                        (uintmax_t)strlen(item->fts_path), (uintmax_t)MAX_FILENAME_LEN);
                not_reached();
            }
            if (sanity == PATH_ERR_PATH_TOO_DEEP) {
                err(36, __func__, "%s: path too deep: depth %ju > %ju", item->fts_path + 2,
                        (uintmax_t)count_dirs(item->fts_path), (uintmax_t)MAX_PATH_DEPTH);
                not_reached();
            }
            if (sanity == PATH_ERR_NOT_POSIX_SAFE && strcmp(item->fts_path, ".") != 0) {
                    errno = 0; /* pre-clear errno for errp() */
                    filename = strdup(item->fts_path + 2);
                    if (filename == NULL) {
                        errp(37, __func__, "strdup(\"%s\") failed", item->fts_path + 2);
                        not_reached();
                    }
                if (item->fts_info == FTS_F) {
                    append_unique_str(ignored_files, filename);
                    dbg(DBG_MED, "%s: not POSIX plus + safe chars only", item->fts_path);
                } else if (item->fts_info == FTS_D) {
                    append_unique_str(ignored_dirs, filename);
                    dbg(DBG_MED, "%s: not POSIX plus + safe chars only", item->fts_path);
                }
            }
            if (sanity == PATH_ERR_NOT_RELATIVE) {
                err(38, __func__, "%s: path not relative", item->fts_path + 2);
                not_reached();
            }
            if (sanity == PATH_ERR_UNKNOWN || sanity == PATH_ERR_PATH_IS_NULL ||
               sanity == PATH_ERR_PATH_EMPTY || sanity == PATH_ERR_MAX_PATH_LEN_0 ||
               sanity == PATH_ERR_MAX_DEPTH_0 || sanity == PATH_ERR_MAX_NAME_LEN_0) {
                    err(39, __func__, "%s: %s", item->fts_path + 2, path_sanity_error(sanity));
                    not_reached();
            }
            switch (item->fts_info) {
                case FTS_D:
                    {
                        if (sanity == PATH_OK) {
                            errno = 0; /* pre-clear errno for errp() */
                            filename = strdup(item->fts_path + 2);
                            if (filename == NULL) {
                                errp(40, __func__, "strdup(\"%s\") failed", item->fts_path + 2);
                                not_reached();
                            }
                            /*
                             * we need these for later
                             */
                            append_unique_str(directories, filename);
                        }
                        /*
                         * XXX - do we need to check for other conditions?
                         * Perhaps not as the errors are checked above but this
                         * will be determined later.
                         */
                    }
                    break;
                case FTS_F:
                    if (sanity == PATH_OK) {
                        dbg(DBG_LOW, "found sane relative path: %s", item->fts_path + 2);
                        errno = 0; /* pre-clear errno for errp() */
                        filename = strdup(item->fts_path + 2);
                        if (filename == NULL) {
                            errp(41, __func__, "strdup(\"%s\") failed", item->fts_path + 2);
                            not_reached();
                        }
                        append_unique_str(files_list, filename);

                        /*
                         * XXX - the check for prog.c, Makefile and remarks.md
                         * will be moved to after the traversing the tree but for
                         * now it is done here (as it is a work in progress)
                         */
                        if (!strcasecmp(item->fts_path + 2, PROG_C_FILENAME)) {
                            /*
                             * check prog.c
                             */
                            char *prog_c = NULL; /* path for check_prog_c */

                            /*
                             * we have to fake the prog.c path for
                             * check_prog_c()
                             */
                            prog_c = calloc_path(args[0], "prog.c");
                            if (prog_c == NULL) {
                                err(42, __func__, "prog_c is NULL");
                                not_reached();
                            }
                            errno = 0;      /* pre-clear errno for errp() */
                            ret = fchdir(cwd);
                            if (ret < 0) {
                                errp(43, __func__, "unable to fchdir(cwd)");
                                not_reached();
                            }
                            if (!quiet) {
                                para("", "Checking prog.c ...", NULL);
                            }
                            *size = check_prog_c(infop, submission_dir, prog_c);
                            if (!quiet) {
                                para("... completed prog.c check.", "", NULL);
                            }
                            if (prog_c != NULL) {
                                free(prog_c);
                                prog_c = NULL;
                            }
                            /*
                             * now switch back to the topdir for the rest of
                             * the files
                             */
                            errno = 0;          /* pre-clear errno for errp() */
                            ret = fchdir(topdir);
                            if (ret < 0) {
                                errp(44, __func__, "unable to fchdir(topdir)");
                                not_reached();
                            }

                            /*
                             * record prog.c found
                             */
                            found_prog_c = true;
                        } else if (!strcasecmp(item->fts_path + 2, MAKEFILE_FILENAME)) {
                            /*
                             * check Makefile
                             */
                            char *Makefile = NULL; /* path for check_Makefile */

                            /*
                             * we have to fake the Makefile path for
                             * check_Makefile()
                             */
                            Makefile = calloc_path(args[0], "Makefile");
                            if (Makefile == NULL) {
                                err(45, __func__, "Makefile is NULL");
                                not_reached();
                            }
                            errno = 0;      /* pre-clear errno for errp() */
                            ret = fchdir(cwd);
                            if (ret < 0) {
                                errp(46, __func__, "unable to fchdir(cwd)");
                                not_reached();
                            }

                            if (!quiet) {
                                para("Checking Makefile ...", NULL);
                            }
                            check_Makefile(infop, submission_dir, Makefile);
                            if (!quiet) {
                                para("... completed Makefile check.", "", NULL);
                            }
                            /*
                             * now switch back to the topdir
                             */
                            errno = 0;          /* pre-clear errno for errp() */
                            ret = fchdir(topdir);
                            if (ret < 0) {
                                errp(47, __func__, "unable to fchdir(topdir)");
                                not_reached();
                            }

                            /*
                             * record Makefile found
                             */
                            found_Makefile = true;
                        } else if (!strcasecmp(item->fts_path + 2, REMARKS_FILENAME)) {
                            /*
                             * check remarks.md
                             */
                            char *remarks_md = NULL; /* path for check_remarks_md() */

                            /*
                             * we have to fake the remarks.md path for
                             * check_remarks_md()
                             */
                            remarks_md = calloc_path(args[0], "remarks.md");
                            if (remarks_md == NULL) {
                                err(48, __func__, "remarks_md is NULL");
                                not_reached();
                            }
                            errno = 0;      /* pre-clear errno for errp() */
                            ret = fchdir(cwd);
                            if (ret < 0) {
                                errp(49, __func__, "unable to fchdir(cwd)");
                                not_reached();
                            }

                            if (!quiet) {
                                para("Checking remarks.md ...", NULL);
                            }
                            check_remarks_md(infop, submission_dir, remarks_md);
                            if (!quiet) {
                                para("... completed remarks.md check.", "", NULL);
                            }
                            /*
                             * now switch back to the topdir
                             */
                            errno = 0;          /* pre-clear errno for errp() */
                            ret = fchdir(topdir);
                            if (ret < 0) {
                                errp(50, __func__, "unable to fchdir(topdir)");
                                not_reached();
                            }

                            /*
                             * record remarks.md found
                             */
                            found_remarks_md = true;
                        } else if (!optional) {
                            /*
                             * we need the total number of extra files
                             */
                            ++count;
                        }
                    }
                    break;
                case FTS_DNR: /* directory not readable */
                    err(51, __func__, "directory not readable: %s", item->fts_path + 2);
                    not_reached();
                case FTS_ERR:
                    /*
                     * fake errno
                     */
                    errno = item->fts_errno;
                    errp(52, __func__, "encountered error reading path: %s", item->fts_path + 2);
                    not_reached();
                    break;
                case FTS_SL:
                case FTS_SLNONE:
                    errno = 0; /* pre-clear errno for errp() */
                    filename = strdup(item->fts_path + 2);
                    if (filename == NULL) {
                        errp(53, __func__, "strdup(\"%s\") failed", item->fts_path + 2);
                        not_reached();
                    }
                    /*
                     * we have to ignore symlinks but it's not an error to have
                     * them as they might be useful to some submitters
                     */
                    append_unique_str(ignored_files, filename);
                    break;
                default:
                    break;
            }
        }

        /*
         * close down stream
         */
        fts_close(fts);
        fts = NULL;
    }
    /*
     * now switch back to the original cwd
     */
    errno = 0;          /* pre-clear errno for errp() */
    ret = fchdir(cwd);
    if (ret < 0) {
        errp(54, __func__, "unable to fchdir(cwd)");
        not_reached();
    }

    /*
     * check that there are not too many extra files
     */
    if (count > MAX_EXTRA_FILE_COUNT) {
        err(55, __func__, "too many files: %ju > %ju", (uintmax_t)count, (uintmax_t)MAX_FILE_COUNT);
        not_reached();
    }

    /*
     * verify prog.c, Makefile and remarks.md have been found
     *
     * XXX - this will possibly be changed as well
     */
    if (!found_prog_c) {
        err(56, __func__, "prog.c not found in topdir %s", args[0]);
        not_reached();
    } else if (!found_Makefile) {
        err(57, __func__, "Makefile not found in topdir %s", args[0]);
        not_reached();
    } else if (!found_remarks_md) {
        err(58, __func__, "remarks.md not found in topdir %s", args[0]);
        not_reached();
    }

    /*
     * now sort the arrays
     */
    qsort(ignored_files->data, (size_t)dyn_array_tell(ignored_files), sizeof(char *), filename_cmp);
    qsort(files_list->data, (size_t)dyn_array_tell(files_list), sizeof(char *), filename_cmp);
    qsort(directories->data, (size_t)dyn_array_tell(directories), sizeof(char *), filename_cmp);
    qsort(ignored_dirs->data, (size_t)dyn_array_tell(ignored_dirs), sizeof(char *), filename_cmp);
    qsort(forbidden_files->data, (size_t)dyn_array_tell(forbidden_files), sizeof(char *), filename_cmp);

    /*
     * we need to show the user the list of ignored directories, if any
     */
    len = dyn_array_tell(ignored_dirs);
    if (len > 0) {
        para("",
                "The following is a list of directories that have been ignored:",
                "",
                NULL);

        for (i = 0; i < len; ++i) {
            p = dyn_array_value(ignored_dirs, char *, i);
            if (p == NULL) {
                err(59, __func__, "found NULL pointer in ignored dirname list, element: %ju", (uintmax_t)i);
                not_reached();
            }
            print("%s\n", p);
        }
        if (!answer_yes) {
            yorn = yes_or_no("Is this OK? [yn]");
            if (!yorn) {
                err(60, __func__, "aborting because user said ignored directories list is not OK");
                not_reached();
            }
        }
    }

    /*
     * we need to show any forbidden filenames to the user
     */
    len = dyn_array_tell(forbidden_files);
    if (len > 0) {
        para("",
                "The following is a list of forbidden filenames that have been ignored:",
                "",
                NULL);

        for (i = 0; i < len; ++i) {
            p = dyn_array_value(forbidden_files, char *, i);
            if (p == NULL) {
                err(61, __func__, "found NULL pointer in forbidden files list, element: %ju", (uintmax_t)i);
                not_reached();
            }
            print("%s\n", p);
        }
        /*
         * we don't ask them to confirm is as this is only informative
         */
    }

    /*
     * we need to show the user the list of ignored files, if any
     */
    len = dyn_array_tell(ignored_files);
    if (len > 0) {
        para("",
                "The following is a list of files that have been ignored:",
                "",
                NULL);

        for (i = 0; i < len; ++i) {
            p = dyn_array_value(ignored_files, char *, i);
            if (p == NULL) {
                err(62, __func__, "found NULL pointer in ignored files list, element: %ju", (uintmax_t)i);
                not_reached();
            }
            print("%s\n", p);
        }
        if (!answer_yes) {
            yorn = yes_or_no("Is this OK? [yn]");
            if (!yorn) {
                err(63, __func__, "aborting because user said ignored files list is not OK");
                not_reached();
            }
        }
    }

    /*
     * we need to show the user the list of all files
     */
    len = dyn_array_tell(files_list);
    if (len > 0) {
        para("",
                "The following is a list of files that will be added to the tarball:"
                "",
                NULL);

        for (i = 0; i < len; ++i) {
            p = dyn_array_value(files_list, char *, i);
            if (p == NULL) {
                err(64, __func__, "found NULL pointer in files list, element: %ju", (uintmax_t)i);
                not_reached();
            }
            print("%s\n", p);
        }
        if (!answer_yes) {
            yorn = yes_or_no("Is this OK? [yn]");
            if (!yorn) {
                err(65, __func__, "aborting because user said files list is not OK");
                not_reached();
            }
        }

        /*
         * make the necessary subdirectories, if any
         */
        len = dyn_array_tell(directories);
        if (len > 0) {
            /*
             * we have to get to the submission dir before we can make
             * directories
             */
            errno = 0;      /* pre-clear errno for errp() */
            if (chdir(submission_dir) != 0) {
                errp(66, __func__, "chdir(\"%s\") failed", submission_dir);
                not_reached();
            }

            for (i = 0; i < len; ++i) {
                /*
                 * make the directories under the submission directory
                 */
                p = dyn_array_value(directories, char *, i);
                if (p == NULL) {
                    err(67, __func__, "found NULL pointer in directories list");
                    not_reached();
                }
                /* create path */
                mkdirs(-1, p, 0755);
            }
        }

        /*
         * now we have to get to the topdir (where the files collected are)
         */
        errno = 0;
        if (fchdir(topdir) != 0) {
            errp(68, __func__, "cannot change to topdir");
            not_reached();
        }

        /*
         * XXX - this needs to be cleaned up but it now gets the files copied to
         * the right place with the right permissions.
         *
         * Since not all steps are even in yet this is okay as there will be
         * changes but now the files are copied to the directories.
         *
         * One thing that will change is that prog.c, Makefile and remarks.md
         * will be dealt with her rather than in the traversing of the topdir.
         *
         * Other steps are also missing.
         *
         * NOTE: the target path and the source path of all files must be
         * absolute as we cannot rely on relative paths, even if we require no
         * absolute paths in the tarball.
         */
        len = dyn_array_tell(files_list);
        for (i = 0; i < len; ++i) {
            p = dyn_array_value(files_list, char *, i);
            if (p == NULL) {
                err(69, __func__, "found NULL pointer in files list, element: %ju", (uintmax_t)i);
                not_reached();
            }
            /*
             * XXX - as we already copied these three files over we have to skip them.
             * Later on this will be done differently.
             */
            if (strcasecmp(p, "remarks.md") && strcasecmp(p, "prog.c") && strcasecmp(p, "Makefile")) {
                if (fname != NULL) {
                    free(fname);
                    fname = NULL;
                }
                /*
                 * we have to allocate the full path of the file under topdir.
                 * This is why we use calloc_path() with the absolute path of
                 * the topdir and the file. We need the absolute path because we
                 * cannot guarantee where the user has things in their system.
                 */
                fname = calloc_path(topdir_path, p);
                if (fname == NULL) {
                    err(70, __func__, "couldn't allocate path to copy");
                    not_reached();
                }
                if (target_path != NULL) {
                    free(target_path);
                    target_path = NULL;
                }

                /*
                 * we need the absolute target path because the topdir and work
                 * directory path can be anywhere.
                 */
                errno = 0; /* pre-clear errno for errp() */
                target_path = calloc(1, strlen(submit_path) + LITLEN("/") + strlen(p) + 1);
                if (target_path == NULL) {
                    errp(71, __func__, "failed to allocate target path for %s", p);
                    not_reached();
                }
                /*
                 * the target path is the absolute path of the submission
                 * directory + / + the filename
                 */
                errno = 0; /* pre-clear errno for errp() */
                ret = snprintf(target_path, strlen(submit_path) + 1 + strlen(p) + 1, "%s/%s", submit_path, p);
                if (ret <= 0) {
                    errp(72, __func__, "snprintf to form target path for %s failed", fname);
                    not_reached();
                }
                /*
                 * try.sh and try.alt.sh must be executable (0555)
                 */
                if (!strcasecmp(p, "try.sh") || !strcasecmp(p, "try.alt.sh")) {
                    copyfile(fname, target_path, false, 0555);
                } else {
                    /*
                     * other files must be mode 0444
                     */
                    copyfile(fname, target_path, false, 0444);
                }

                /*
                 * paranoia
                 */
                if (target_path != NULL) {
                    free(target_path);
                    target_path = NULL;
                }
                if (fname != NULL) {
                    free(fname);
                    fname = NULL;
                }
            }
        }
    }

    /*
     * now we have to switch back to the previous cwd (original) so that the
     * creation of .info.json and .auth.json do not fail.
     */
    errno = 0; /* pre-clear errno for errp() */
    if (fchdir(cwd) != 0) {
        errp(73, __func__, "failed to change to cwd");
        not_reached();
    }

    /*
     * free dynamic arrays
     */
    if (ignored_files != NULL) {
	dyn_array_free(ignored_files);
	ignored_files = NULL;
    }
    if (files_list != NULL) {
	dyn_array_free(files_list);
	files_list = NULL;
    }
    if (directories != NULL) {
	dyn_array_free(directories);
	directories = NULL;
    }
    if (ignored_dirs != NULL) {
	dyn_array_free(ignored_dirs);
	ignored_dirs = NULL;
    }
    if (forbidden_files != NULL) {
	dyn_array_free(forbidden_files);
	forbidden_files = NULL;
    }

    /*
     * free rest of allocated memory
     */
    if (fname != NULL) {
        free(fname);
        fname = NULL;
    }
    if (target_path != NULL) {
        free(target_path);
        target_path = NULL;
    }

    /*
     * close our temporary file descriptors
     */
    errno = 0; /* pre-clear errno for errp() */
    if (close(cwd) != 0) {
        errp(74, __func__, "failed to close(cwd)");
        not_reached();
    }
    errno = 0; /* pre-clear errno for errp() */
    if (close(topdir) != 0) {
        errp(75, __func__, "failed to close(topdir)");
        not_reached();
    }


    /*
     * total number of extra files
     */
    return count;
}

/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	program		our program name
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
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = MKIOCCCENTRY_BASENAME;
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }


    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }

    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg0, prog, DBG_DEFAULT, JSON_DBG_DEFAULT);
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg1, TAR_PATH_0, LS_PATH_0, TXZCHK_PATH_0, FNAMCHK_PATH_0);
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg2, CHKENTRY_PATH_0);
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg3, (unsigned)SEED_MASK, (unsigned)(DEFAULT_SEED & SEED_MASK));
    fprintf_usage(DO_NOT_EXIT, stderr, usage_msg4);
    fprintf_usage(exitcode, stderr, usage_msg5, MKIOCCCENTRY_BASENAME, MKIOCCCENTRY_VERSION,
	    JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * mkiocccentry_sanity_chks - perform basic sanity checks
 *
 * We perform basic sanity checks on paths and the IOCCC contest ID as well as
 * the IOCCC toolkit tables.
 *
 * given:
 *
 *      infop           - pointer to info structure
 *      workdir        - where the submission directory and tarball are formed
 *      tar             - path to tar that supports the -J (xz) option
 *	ls		- path to the ls utility
 *	txzchk		- path to txzchk tool
 *	fnamchk		- path to fnamchk tool
 *	chkentry	- path to chkentry tool
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
mkiocccentry_sanity_chks(struct info *infop, char const *workdir, char const *tar,
	   char const *ls, char const *txzchk, char const *fnamchk, char const *chkentry)
{
    /*
     * firewall
     */
    if (infop == NULL || workdir == NULL || tar == NULL || ls == NULL ||
	txzchk == NULL || fnamchk == NULL || chkentry == NULL) {
	err(76, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * tar must be executable
     */
    if (!exists(tar)) {
	fpara(stderr,
	      "",
	      "We cannot find the tar program.",
	      "",
	      "A tar program that supports the -J (xz) option is required to build a compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(77, __func__, "tar does not exist: %s", tar);
	not_reached();
    }
    if (!is_file(tar)) {
	fpara(stderr,
	      "",
	      "The tar path, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(78, __func__, "tar is not a regular file: %s", tar);
	not_reached();
    }
    if (!is_exec(tar)) {
	fpara(stderr,
	      "",
	      "The tar path, while it is a file, is not an executable.",
	      "",
	      "We suggest you check the permissions on the tar program, or use another path:",
	      "",
	      "    mkiocccentry -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(79, __func__, "tar is not an executable program: %s", tar);
	not_reached();
    }

    /*
     * ls must be executable
     */
    if (!exists(ls)) {
	fpara(stderr,
	      "",
	      "We cannot find the ls program.",
	      "",
	      "The ls program is required to help you verify the correct files are included.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -l /path/to/ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(80, __func__, "ls does not exist: %s", ls);
	not_reached();
    }
    if (!is_file(ls)) {
	fpara(stderr,
	      "",
	      "The ls path, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -l /path/to/ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(81, __func__, "ls is not a regular file: %s", ls);
	not_reached();
    }
    if (!is_exec(ls)) {
	fpara(stderr,
	      "",
	      "The ls path, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the ls program, or use another path:",
	      "",
	      "    mkiocccentry -l /path/to/ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(82, __func__, "ls is not an executable program: %s", ls);
	not_reached();
    }

    /*
     * txzchk must be executable
     */
    if (!exists(txzchk)) {
	fpara(stderr,
	      "",
	      "We cannot find txzchk.",
	      "",
	      "The txzchk program performs sanity checks on the compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -T /path/to/txzchk ...",
	      "",
	      "and/or install the txzchk tool?  You can find the source for txzchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(83, __func__, "txzchk does not exist: %s", txzchk);
	not_reached();
    }
    if (!is_file(txzchk)) {
	fpara(stderr,
	      "",
	      "The txzchk path, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -T /path/to/txzchk ...",
	      "",
	      "and/or install the txzchk tool?  You can find the source for txzchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(84, __func__, "txzchk is not a regular file: %s", txzchk);
	not_reached();
    }
    if (!is_exec(txzchk)) {
	fpara(stderr,
	      "",
	      "The txzchk path, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the txzchk program, or use another path:",
	      "",
	      "    mkiocccentry -T /path/to/txzchk ...",
	      "",
	      "and/or install the txzchk tool?  You can find the source for txzchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(85, __func__, "txzchk is not an executable program: %s", txzchk);
	not_reached();
    }

    /*
     * fnamchk must be executable
     */
    if (!exists(fnamchk)) {
	fpara(stderr,
	      "",
	      "We cannot find the fnamchk tool.",
	      "",
	      "The fnamchk program performs sanity checks on the compressed tarball filename.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(86, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }
    if (!is_file(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk path, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(87, __func__, "fnamchk is not a regular file: %s", fnamchk);
	not_reached();
    }
    if (!is_exec(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk path, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the fnamchk program, or use another path:",
	      "",
	      "    mkiocccentry -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(88, __func__, "fnamchk is not an executable program: %s", fnamchk);
	not_reached();
    }

    /*
     * chkentry must be executable
     */
    if (!exists(chkentry)) {
	fpara(stderr,
	      "",
	      "We cannot find the chkentry tool.",
	      "",
	      "The chkentry program performs sanity checks on the JSON files.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -C /path/to/chkentry ...",
	      "",
	      "and/or install the chkentry tool?  You can find the source for chkentry in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(89, __func__, "chkentry does not exist: %s", chkentry);
	not_reached();
    }
    if (!is_file(chkentry)) {
	fpara(stderr,
	      "",
	      "The chkentry path, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -C /path/to/chkentry ...",
	      "",
	      "and/or install the chkentry tool?  You can find the source for chkentry in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(90, __func__, "chkentry is not a regular file: %s", chkentry);
	not_reached();
    }
    if (!is_exec(chkentry)) {
	fpara(stderr,
	      "",
	      "The chkentry path, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the chkentry program, or use another path:",
	      "",
	      "    mkiocccentry -C /path/to/chkentry ...",
	      "",
	      "and/or install the chkentry tool?  You can find the source for chkentry in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(91, __func__, "chkentry is not an executable program: %s", chkentry);
	not_reached();
    }

    /*
     * workdir must be a writable directory
     */
    if (!exists(workdir)) {
	fpara(stderr,
	      "",
	      "The workdir does not exist.",
	      "",
	      "You should either create workdir, or use a different workdir directory path on the command line.",
	      "",
	      NULL);
	err(92, __func__, "workdir does not exist: %s", workdir);
	not_reached();
    }
    if (!is_dir(workdir)) {
	fpara(stderr,
	      "",
	      "While workdir exists, it is not a directory.",
	      "",
	      "You should move or remove workdir and them make a new workdir directory, or use a different",
	      "workdir directory path on the command line.",
	      "",
	      NULL);
	err(93, __func__, "workdir is not a directory: %s", workdir);
	not_reached();
    }
    if (!is_write(workdir)) {
	fpara(stderr,
	      "",
	      "While the directory workdir exists, it is not a writable directory.",
	      "",
	      "You should change the permission to make workdir writable, or move or remove workdir and then",
	      "create a new writable directory, or use a different workdir directory path on the command line.",
	      "",
	      NULL);
	err(94, __func__, "workdir is not a writable directory: %s", workdir);
	not_reached();
    }

    /*
     * obtain version string from IOCCCSIZE_VERSION
     */
    infop->iocccsize_ver = IOCCCSIZE_VERSION;

    /* we also check that all the tables across the IOCCC toolkit are sane */
    ioccc_sanity_chks();

    return;
}


/*
 * prompt - prompt for a string
 *
 * Prompt a string, followed by :<space> on stdout and then read a line from stdin.
 * The line is stripped of the trailing newline and then of all trailing whitespace.
 *
 * given:
 *      str     - string to string followed by :<space>
 *      lenp    - pointer to where to put the length of the response,
 *                NULL ==> do not save length
 *
 *
 * returns:
 *      allocated input string with newline and trailing whitespace removed
 *	if lenp != NULL, *lenp will be set to the length of the response
 *
 * NOTE: This function will NOT return NULL.
 *
 * This function does not return on error.
 */
static char *
prompt(char const *str, size_t *lenp)
{
    char *linep = NULL;		/* readline_dup line buffer */
    int ret;			/* libc function return value */
    size_t len;			/* length of input */
    char *buf;			/* allocated input string */

    /*
     * firewall
     *
     * NOTE: As noted above, lenp can be NULL.
     */
    if (str == NULL) {
	err(95, __func__, "called with NULL str");
	not_reached();
    }

    /*
     * prompt + :<space> if silence_prompt is false
     */
    if (!silence_prompt) {
	clearerr(stdout);		/* pre-clear ferror() status */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fputs(str, stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(96, __func__, "error printing prompt string");
		not_reached();
	    } else if (feof(stdout)) {
		err(97, __func__, "EOF while printing prompt string");
		not_reached();
	    } else {
		errp(98, __func__, "unexpected fputs error printing prompt string");
		not_reached();
	    }
	}
	clearerr(stdout);		/* pre-clear ferror() status */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fputs(": ", stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(99, __func__, "error printing :<space>");
		not_reached();
	    } else if (feof(stdout)) {
		err(100, __func__, "EOF while writing :<space>");
		not_reached();
	    } else {
		errp(101, __func__, "unexpected fputs error printing :<space>");
		not_reached();
	    }
	}
	clearerr(stdout);		/* pre-clear ferror() status */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fflush(stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(102, __func__, "error flushing prompt to stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(103, __func__, "EOF while flushing prompt to stdout");
		not_reached();
	    } else {
		errp(104, __func__, "unexpected fflush error while flushing prompt to stdout");
		not_reached();
	    }
	}
    }

    /*
     * read user input - return input length
     */
    buf = readline_dup(&linep, true, &len, input_stream);
    if (buf == NULL) {
	err(105, __func__, "EOF while reading prompt input");
	not_reached();
    }
    dbg(DBG_VHIGH, "received a %ju byte response", (uintmax_t)len);

    /*
     * save length if requested
     */
    if (lenp != NULL) {
	*lenp = len;
    }

    /*
     * free storage
     */
    if (linep != NULL) {
	free(linep);
	linep = NULL;
    }

    /*
     * return allocated input buffer
     */
    return buf;
}


/*
 * get IOCCC ID or test
 *
 * This function will prompt the user for a contest ID, validate it and return it
 * as a allocated string.  If the contest ID is the special value "test", then
 * *testp will be set to true, otherwise it will be set to false.
 *
 * given:
 *      infop   - pointer to info structure
 *      testp   - pointer to boolean for test mode
 *
 * returns:
 *      allocated contest ID string
 *      *testp ==> contest ID is "test", else contest ID is a UUID.
 *
 * This function does not return on error or if the contest ID is malformed.
 */
static char *
get_contest_id(bool *testp, bool *read_answers_flag_used)
{
    char *malloc_ret;		/* allocated return string */
    size_t len;			/* input string length */
    bool valid = false;		/* true ==> IOCCC_contest_id is valid */
    bool seen_answers_header = false;

    /*
     * firewall
     */
    if (testp == NULL) {
	err(106, __func__, "called with NULL testp");
	not_reached();
    } else if (read_answers_flag_used == NULL) {
	err(107, __func__, "called with NULL read_answers_flag_used");
	not_reached();
    }

    /*
     * explain contest ID
     */
    if (need_hints) {
	show_registration_url();
	para("",
	     "If you do not have an IOCCC contest ID and you wish to test this program,",
	     "you may use the special contest ID:",
	     "",
	     "    test",
	     "",
	     "Note you will not be able to submit the resulting compressed tarball when using test.",
	     "",
	     NULL);
    }

    /*
     * keep asking for a submit slot number until we get a valid reply
     */
    do {

	/*
	 * prompt for the contest ID
	 */
	malloc_ret = prompt("Enter IOCCC contest ID or test", &len);
	if (!seen_answers_header && !strcmp(malloc_ret, MKIOCCCENTRY_ANSWERS_VERSION)) {
	    dbg(DBG_HIGH, "found answers header");
	    seen_answers_header = true;
	    *read_answers_flag_used = true;
	    need_confirm = false;
	    need_hints = false;

	    free(malloc_ret);
	    malloc_ret = prompt("", &len);
	}
	if (*read_answers_flag_used && !seen_answers_header) {
	    err(108, __func__, "didn't find the correct answers file header");
	    not_reached();
	}

	dbg(DBG_HIGH, "the IOCCC contest ID as entered is: %s", malloc_ret);

	/*
	 * case: IOCCC contest ID is test, quick return
	 */
	if (strcmp(malloc_ret, "test") == 0) {

	    /*
	     * report test mode
	     */
	    para("",
		 "IOCCC contest ID is test, entering test mode.",
		 NULL);
	    *testp = true;
	    return malloc_ret;
	}

	/*
	 * validate format of non-test contest ID
	 *
	 * The contest ID, if not "test" must be a UUID.  The UUID has the 36 character format:
	 *
	 *     xxxxxxxx-xxxx-4xxx-Nxxx-xxxxxxxxxxxx
	 *
	 * where 'x' is a hex character,  is the UUID version, and N is one of 8, 9, a, or b.
	 */
	valid = test_IOCCC_contest_id(malloc_ret);
	if (valid == false) {

	    /*
	     * reject invalid non-test contest ID
	     */
	    fpara(stderr,
		  "IOCCC contest IDs are in the form:",
		  "",
		  "    xxxxxxxx-xxxx-4xxx-Nxxx-xxxxxxxxxxxx",
		  "",
		  "where 'x' is a hex character, 4 is the UUID version, and N is one of 8, 9, a, or b.",
		  "",
		  NULL);

	    /*
	     * free storage
	     */
	    if (malloc_ret != NULL) {
		free(malloc_ret);
		malloc_ret = NULL;
	    }
	    continue;
	}
    } while (valid != true);

    /*
     * report on the result of the contest ID validation
     */
    dbg(DBG_MED, "Contest ID is a UUID: %s", malloc_ret);

    /*
     * report contest ID format is valid
     */
    para("",
	 "The format of the non-test IOCCC contest ID appears to be valid.",
	 NULL);
    *testp = false;		/* IOCCC contest ID is not test */

    /*
     * return IOCCC contest ID
     */
    return malloc_ret;
}


/*
 * get_submit_slot - obtain the submit slot number
 *
 * given:
 *      infop   - pointer to info structure
 *
 * returns:
 *      submit slot number >= 0 <= MAX_SUBMIT_SLOT
 */
static int
get_submit_slot(struct info *infop)
{
    int submit_slot;		/* submit slot number */
    char *submission_str;	/* submit slot number string */
    int ret;			/* libc function return */
    char guard;			/* scanf guard to catch excess amount of input */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(109, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * keep asking for a submit slot number until we get a valid reply
     */
    do {

	/*
	 * explain submit slot numbers
	 */
	if (need_hints) {
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = printf("\nYou are allowed to submit up to %d submissions to a given IOCCC.\n", MAX_SUBMIT_SLOT + 1);
	    if (ret <= 0) {
		errp(110, __func__, "printf error printing number of submissions allowed");
		not_reached();
	    }
	    para("",
		 "As in C, submit slot numbers start with 0.  If you are updating a",
                 "previous submission, PLEASE use the same submit slot number that",
                 "you previously uploaded so we know which submission we should replace.",
                 "If this is your first submission to this given IOCCC, enter 0.",
		 "",
		 NULL);
	}

	/*
	 * ask for the submit slot number
	 */
	submission_str = prompt("Enter the submit slot number", NULL);

	/*
	 * check the submit slot number
	 */
	errno = 0;		/* pre-clear errno for warnp() */
	ret = sscanf(submission_str, "%d%c", &submit_slot, &guard);
	if (ret != 1 || submit_slot < 0 || submit_slot > MAX_SUBMIT_SLOT) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "\nThe submit slot number must be a number from 0 through %d; please re-enter.\n",
		    MAX_SUBMIT_SLOT);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error while informing about the valid submit slot number range");
	    }
	    submit_slot = -1;	/* invalidate input */
	}

	/*
	 * free storage
	 */
	if (submission_str != NULL) {
	    free(submission_str);
	    submission_str = NULL;
	}

    } while (submit_slot < 0 || submit_slot > MAX_SUBMIT_SLOT);

    /*
     * report on the result of the submit slot number validation
     */
    dbg(DBG_MED, "IOCCC submit slot number is valid: %d", submit_slot);

    /*
     * return the submit slot number
     */
    return submit_slot;
}


/*
 * mk_submission_dir - make the submission directory
 *
 * Make a directory, under workdir, from which the compressed tarball
 * will be formed.
 *
 * given:
 *      workdir        - working directory under which the submission directory is formed
 *      ioccc_id        - IOCCC submission ID (or test)
 *      submit_slot     - submit slot number
 *      tarball_path    - pointer to the allocated path to where the compressed tarball will be formed
 *      tstamp          - now as a timestamp
 *      test_mode       - true ==> test mode, do not upload
 *
 * returns:
 *      the path of the working directory
 *
 * This function does not return on error or if the submission directory cannot be formed.
 */
static char *
mk_submission_dir(char const *workdir, char const *ioccc_id, int submit_slot,
	     char **tarball_path, time_t tstamp, bool test_mode)
{
    size_t submission_dir_len;	/* length of submission directory */
    char *submission_dir = NULL;	/* allocated submission directory path */
    bool test = false;		/* test result */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (workdir == NULL || ioccc_id == NULL || tarball_path == NULL) {
	err(111, __func__, "called with NULL arg(s)");
	not_reached();
    }
    test = test_submit_slot(submit_slot);
    if (test == false) {
	err(112, __func__, "submit slot number: %d must >= 0 and <= %d", submit_slot, MAX_SUBMIT_SLOT);
	not_reached();
    }

    /*
     * determine length of submission directory path
     */
    /*
     * workdir/ioccc_id-entry
     */
    submission_dir_len = strlen(workdir) + 1 + strlen(ioccc_id) + 1 + MAX_SUBMIT_SLOT_CHARS + 1 + 1;
    errno = 0;			/* pre-clear errno for errp() */
    submission_dir = (char *)malloc(submission_dir_len + 1);
    if (submission_dir == NULL) {
	errp(113, __func__, "malloc #0 of %ju bytes failed", (uintmax_t)(submission_dir_len + 1));
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(submission_dir, submission_dir_len + 1, "%s/%s-%d", workdir, ioccc_id, submit_slot);
    if (ret <= 0) {
	errp(114, __func__, "snprintf to form submission directory failed");
	not_reached();
    }
    dbg(DBG_HIGH, "submission directory path: %s", submission_dir);

    /*
     * verify that the submission directory does not exist
     */
    if (exists(submission_dir)) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nsubmission directory already exists: %s\n", submission_dir);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error while informing that the submission directory already exists");
	}
	fpara(stderr,
	      "",
	      "You need to move that directory, or remove it, or use a different workdir.",
	      "",
	      NULL);
	err(115, __func__, "submission directory exists: %s", submission_dir);
	not_reached();
    }
    dbg(DBG_HIGH, "submission directory path: %s", submission_dir);

    /*
     * make the submission directory
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = mkdir(submission_dir, 0755);
    if (ret < 0) {
	errp(116, __func__, "cannot mkdir %s with mode 0755", submission_dir);
	not_reached();
    }

    /*
     * form the compressed tarball path
     *
     * We assume timestamps will be values <= 12 decimal digits in the future. :-)
     */
    *tarball_path = form_tar_filename(ioccc_id, submit_slot, test_mode, tstamp);
    if (*tarball_path == NULL) {
	errp(117, __func__, "failed to form compressed tarball path");
	not_reached();
    }
    dbg(DBG_HIGH, "compressed tarball path: %s", *tarball_path);

    /*
     * return submission directory
     */
    return submission_dir;
}


/*
 * warn_empty_prog - warn user that prog.c is empty
 *
 * This function does not return on error.
 */
static void
warn_empty_prog(void)
{
    bool yorn = false;

    dbg(DBG_MED, "prog.c is empty");
    if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	fpara(stderr,
	  "WARNING: prog.c is empty.  An empty prog.c has been submitted before:",
	  "",
	  "    https://www.ioccc.org/years.html#1994_smr",
	  "",
	  "The guidelines indicate that we tend to dislike programs that:",
	  "",
	  "    * are rather similar to previous winning entries  :-(",
	  "",
	  "Perhaps you have a different twist on an empty prog.c than yet another",
	  "smallest self-replicating program.  If so, the you may proceed, although",
	  "we STRONGLY suggest that you put into your remarks.md file, towards",
          "the TOP, why your submission prog.c is not another smallest self-replicating",
          "program.",
	  "",
	  NULL);
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	}
	yorn = yes_or_no("Are you sure you want to submit an empty prog.c file? [yn]");
	if (!yorn) {
	    err(118, __func__, "please fix your prog.c file");
	    not_reached();
	}
	dbg(DBG_MED, "user says that their empty prog.c is OK");
    }
}


/*
 * warn_rule_2a_size - warn if prog.c is too large under Rule 2a
 *
 * given:
 *      infop           - pointer to info structure
 *      mode		- determines which warning to give
 *			  Either RULE_2A_BIG_FILE_WARNING or
 *				 RULE_2A_IOCCCSIZE_MISMATCH
 *
 * This function does not return on error.
 */
static void
warn_rule_2a_size(struct info *infop, int mode, RuleCount size)
{
    bool yorn = false;
    int ret;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(119, __func__, "called with NULL infop");
	not_reached();
    }

    /*
     * File appears to be too big under Rule 2a warning
     */
    if (mode == RULE_2A_BIG_FILE_WARNING) {
	dbg(DBG_MED, "prog.c size: %jd > Rule 2a size: %jd",
		     (intmax_t)infop->rule_2a_size, (intmax_t)RULE_2A_SIZE);
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nWARNING: The prog.c size: %jd > Rule 2a maximum: %jd\n",
		      (intmax_t)infop->rule_2a_size, (intmax_t)RULE_2A_SIZE);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c Rule 2a warning");
	}
	if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	    fpara(stderr,
	      "If you are attempting some clever rule abuse, then we STRONGLY",
              "suggest that you tell us about your rule abuse towards the TOP",
              "of your remarks.md file.  Be sure you have read the:",
              "\t\"ABUSING THE RULES\" section of the guidelines.",
              "\nAnd more importantly, read rule 12!",
	      "",
	      NULL);
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }
	    yorn = yes_or_no("Are you sure you want to submit such a large prog.c file? [yn]");
	    if (!yorn) {
		err(120, __func__, "please fix your prog.c file");
		not_reached();
	    }
	    dbg(DBG_MED, "user says that their prog.c size: %jd > Rule 2a max size: %jd is OK",
		(intmax_t)infop->rule_2a_size, (intmax_t)RULE_2A_SIZE);
	}

    /*
     * File size and iocccsize file size differ warning
     */
    } else if (mode == RULE_2A_BIG_FILE_WARNING) {
	if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "\nInteresting: prog.c file size: %jd != rule_count function size: %jd\n"
				  "In order to avoid a possible Rule 2a violation, BE SURE TO CLEARLY MENTION THIS IN\n"
				  "YOUR remarks.md FILE!\n\n",
				  (intmax_t)infop->rule_2a_size, (intmax_t)size.rule_2a_size);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error when printing prog.c file size and Rule 2a mismatch");
	    }
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }
	    yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	    if (!yorn) {
		err(121, __func__, "please fix your prog.c file");
		not_reached();
	    }
	    dbg(DBG_MED, "user says that prog.c size: %jd != rule_count function size: %jd is OK",
		(intmax_t)infop->rule_2a_size, (intmax_t)size.rule_2a_size);
	}

    /*
     * invalid mode
     */
    } else {
	err(122, __func__, "invalid mode passed to function: %d", mode);
	not_reached();
    }
    return;
}


/*
 * warn_nul_chars - warn user that prog.c has high one or more NUL char(s)
 *
 * given:
 *
 * This function does not return on error.
 */
static void
warn_nul_chars(void)
{
    int ret, yorn;


    /*
     * warn about NUL chars(s) if we are allowed
     */
    if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nprog.c has NUL character(s)!\n"
			      "Be careful you don't violate rule 13!\n\n");
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c nul_warning");
	}
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(123, __func__, "please fix your prog.c file");
	    not_reached();
	}
	dbg(DBG_MED, "user says that prog.c having NUL character(s) is OK");
    }
}


/*
 * warn_trigraph - warn user that prog.c has unknown or invalid trigraph(s)
 *
 * given:
 *
 * This function does not return on error.
 */
static void
warn_trigraph(void)
{
    bool yorn = false;
    int ret;

    /*
     * warn the user about unknown or invalid trigraph(s), if we are allowed
     */
    if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nprog.c has unknown or invalid trigraph(s) found!\n"
			      "Is that a bug in, or a feature of your code?\n\n");
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c trigraph_warning");
	}
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(124, __func__, "please fix your prog.c file");
	    not_reached();
	}
	dbg(DBG_MED, "user says that prog.c having unknown or invalid trigraph(s) is OK");
    }
}


/*
 * warn_wordbuf - warn user that prog.c triggered a word buffer overflow
 *
 * given:
 *
 * This function does not return on error.
 */
static void
warn_wordbuf(void)
{
    int ret, yorn;

    /*
     * warn the user about triggered a word buffer overflow in iocccsize, if we are allowed
     */
    if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nprog.c triggered a word buffer overflow!\n"
			      "In order to avoid a possible Rule 2b violation, BE SURE TO CLEARLY MENTION THIS IN\n"
			      "YOUR remarks.md FILE!\n\n");
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c wordbuf_warning");
	}
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(125, __func__, "please fix your prog.c file");
	    not_reached();
	}
	dbg(DBG_MED, "user says that prog.c triggering a word buffer overflow is OK");
    }
}


/*
 * warn_ungetc - warn user that prog.c triggered an ungetc error
 *
 * given:
 *
 * This function does not return on error.
 */
static void
warn_ungetc(void)
{
    bool yorn = false;
    int ret;

    /*
     * warn the user abort iocccsize ungetc error, if we are allowed
     */
    if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nprog.c triggered an ungetc error: @SirWumpus goofed\n"
			      "In order to avoid a possible Rule 2b violation, BE SURE TO CLEARLY MENTION THIS IN\n"
			      "YOUR remarks.md FILE!\n\n");
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c ungetc_warning");
	}
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(126, __func__, "please fix your prog.c file");
	    not_reached();
	}
	dbg(DBG_MED, "user says that prog.c triggering an ungetc warning OK");
    }
}


/*
 * warn_rule_2b_size - warn user that prog.c triggered an ungetc error
 *
 * given:
 *
 * This function does not return on error.
 */
static void
warn_rule_2b_size(struct info *infop)
{
    int ret, yorn;

     /*
     * firewall
     */
    if (infop == NULL) {
	err(128, __func__, "called with NULL infop");
	not_reached();
    }

    /*
     * warn the user about a possible Rule 2b violation, if we are allowed
     */
    if (abort_on_warning || (need_confirm && !ignore_warnings && !answer_yes)) {
	errno = 0;
	ret = fprintf(stderr, "\nWARNING: The prog.c size: %ju > Rule 2b maximum: %ju\n",
		      (uintmax_t)infop->rule_2b_size, (uintmax_t)RULE_2B_SIZE);
	if (ret <= 0) {
	    errp(129, __func__, "printf error printing prog.c size > Rule 2b maximum");
	    not_reached();
	}

	fpara(stderr,
	      "If you are attempting some clever rule abuse, then we",
              "STRONGLY suggest that you tell us about your rule abuse",
              "towards the TOP of your remarks.md file.  Be sure you have",
              "read the \"ABUSING THE RULES\" section of the guidelines.",
              "\nAnd more importantly, read rule 12!",
	      "",
	      NULL);
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	}
	yorn = yes_or_no("Are you sure you want to submit such a large prog.c file? [yn]");
	if (!yorn) {
	    err(130, __func__, "please fix your prog.c file");
	    not_reached();
	}
	dbg(DBG_MED, "user says that their prog.c size: %ju > Rule 2B max size: %ju is OK",
	    (uintmax_t)infop->rule_2b_size, (uintmax_t)RULE_2B_SIZE);
    }
}


/*
 * check_prog_c - check prog_c arg and if OK, copy into submission_dir/prog.c
 *
 * Check if the prog_c argument is a readable file, and
 * if it is within the guidelines of iocccsize (or if the author overrides),
 * and if all is OK or overridden, use copyfile() to copy into submission_dir/prog.c.
 *
 * given:
 *      infop           - pointer to info structure
 *      submission_dir  - newly created submission directory (by mk_submission_dir()) under workdir
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static RuleCount
check_prog_c(struct info *infop, char const *submission_dir, char const *prog_c)
{
    FILE *prog_stream;		/* prog.c open file stream */
    size_t prog_c_len;		/* length of the prog_c path */
    size_t submission_dir_len;	/* length of the submission_dir path */
    int ret;			/* libc function return */
    RuleCount size;		/* rule_count() processing results */
    char *path = NULL;          /* complete path of submission_dir/prog.c */

    /*
     * firewall
     */
    if (infop == NULL || submission_dir == NULL || prog_c == NULL) {
	err(131, __func__, "called with NULL arg(s)");
	not_reached();
    }
    submission_dir_len = strlen(submission_dir);
    if (submission_dir_len <= 0) {
	err(132, __func__, "submission_dir arg is an empty string");
	not_reached();
    }

    /*
     * prog_c filename length MUST be > 0 && <= MAX_FILENAME_LEN!
     */
    prog_c_len = strlen(prog_c);
    if (!test_filename_len(prog_c)) {
	err(133, __func__, "prog_c filename length: %ju: is not > 0 && <= %ju", (uintmax_t)prog_c_len, (uintmax_t)MAX_FILENAME_LEN);
	not_reached();
    }

    /*
     * prog.c must be a readable file
     */
    if (!exists(prog_c)) {
	fpara(stderr,
	      "",
	      "We cannot find the prog.c file.",
	      "",
	      NULL);
	err(134, __func__, "prog.c does not exist: %s", prog_c);
	not_reached();
    }
    if (!is_file(prog_c)) {
	fpara(stderr,
	      "",
	      "The prog.c path, while it exists, is not a regular file.",
	      "",
	      NULL);
	err(135, __func__, "prog.c is not a regular file: %s", prog_c);
	not_reached();
    }
    if (!is_read(prog_c)) {
	fpara(stderr,
	      "",
	      "The prog.c path, while it is a file, is not readable.",
	      "",
	      NULL);
	err(136, __func__, "prog.c is not a readable file: %s", prog_c);
	not_reached();
    }

    /*
     * determine Rule 2b for prog.c
     *
     * If the size returned is outside of the allowed range, the user will
     * be asked if they want to proceed.  We will allow it if the user insists
     * on proceeding, or of iocccsize says the size is allowed under Rule 2a.
     */
    errno = 0;			/* pre-clear errno for errp() */
    prog_stream = fopen(prog_c, "r");
    if (prog_stream == NULL) {
	errp(137, __func__, "failed to fopen: %s", prog_c);
	not_reached();
    }
    size = rule_count(prog_stream);
    infop->rule_2b_size = size.rule_2b_size;
    dbg(DBG_MED, "prog.c: %s Rule 2b size: %ju", prog_c, (uintmax_t)infop->rule_2b_size);
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(prog_stream);
    if (ret != 0) {
	errp(138, __func__, "failed to fclose: %s", prog_c);
	not_reached();
    }

    /*
     * warn if prog.c is empty
     */
    infop->rule_2a_size = file_size(prog_c);
    dbg(DBG_MED, "Rule 2a size: %jd", (intmax_t)infop->rule_2a_size);
    if (infop->rule_2a_size < 0) {
	err(139, __func__, "file_size error: %jd on prog.c: %s", (intmax_t)infop->rule_2a_size, prog_c);
	not_reached();
    } else if (infop->rule_2a_size == 0 || infop->rule_2b_size == 0) {
	warn_empty_prog();
	infop->empty_override = true;
	infop->rule_2a_override = false;

    /*
     * warn if prog.c is too large under Rule 2a
     */
    } else if (infop->rule_2a_size > RULE_2A_SIZE) {
	warn_rule_2a_size(infop, RULE_2A_BIG_FILE_WARNING, size);
	infop->empty_override = false;
	infop->rule_2a_override = true;
    } else {
	infop->empty_override = false;
	infop->rule_2a_override = false;
    }

    /*
     * sanity check on file size vs rule_count function size for Rule 2a
     */
    if (infop->rule_2a_size != size.rule_2a_size) {
	warn_rule_2a_size(infop, RULE_2A_IOCCCSIZE_MISMATCH, size);
	infop->rule_2a_mismatch = true;
    } else {
	infop->rule_2a_mismatch = false;
    }

    /*
     * now that we allow all UTF-8 in prog.c, we force highbit_warning off
     */
    infop->highbit_warning = false;

    /*
     * sanity check on NUL character(s)
     */
    if (size.nul_warning) {
	warn_nul_chars();
	infop->nul_warning = true;
    } else {
	infop->nul_warning = false;
    }

    /*
     * sanity check on unknown trigraph(s)
     */
    if (size.trigraph_warning) {
	warn_trigraph();
	infop->trigraph_warning = true;
    } else {
	infop->trigraph_warning = false;
    }

    /*
     * sanity check on word buffer overflow
     */
    if (size.wordbuf_warning) {
	warn_wordbuf();
	infop->wordbuf_warning = true;
    } else {
	infop->wordbuf_warning = false;
    }

    /*
     * sanity check on ungetc warning
     */
    if (size.ungetc_warning) {
	warn_ungetc();
	infop->ungetc_warning = true;
    } else {
	infop->ungetc_warning = false;
    }

    /*
     * inspect the Rule 2b size
     */
    if (infop->rule_2b_size > RULE_2B_SIZE) {
	warn_rule_2b_size(infop);
	infop->rule_2b_override = true;
    } else {
	infop->rule_2b_override = false;
    }

    /*
     * get full path
     */
    path = calloc_path(submission_dir, "prog.c");
    if (path == NULL) {
        err(140, __func__, "calloc_path(\"%s\", \"prog.c\") failed", submission_dir);
        not_reached();
    }

    /*
     * copy prog.c under submission_dir
     */
    copyfile(prog_c, path, false, S_IRUSR | S_IRGRP | S_IROTH);

    /*
     * free path
     */
    if (path != NULL) {
        free(path);
        path = NULL;
    }



    /*
     * save prog.c filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->prog_c = strdup("prog.c");
    if (infop->prog_c == NULL) {
	errp(141, __func__, "malloc #2 of %ju bytes failed", (uintmax_t)(LITLEN("prog.c") + 1));
	not_reached();
    }
    return size;
}


/*
 * inspect_Makefile - inspect the rule contents of Makefile
 *
 * Determine if the first rule contains all.  Determine if there is a clean rule.
 * Determine if there is a clobber rule.  Determine if there is a try rule.
 *
 * NOTE: This is a simplistic Makefile line parser.  It is possible to
 *       fool the line scanner and to evade rule detection due to use of
 *       Makefile variables, line continuation, conditional Gnu-make controls, etc.
 *
 * given:
 *      Makefile  - path to Makefile
 *      infop	  - pointer to info struct
 *
 * returns:
 *      true ==> the rule set in Makefile is OK,
 *      false ==> the Makefile has an issue
 *
 * This function does not return on error.
 */
static bool
inspect_Makefile(char const *Makefile, struct info *infop)
{
    FILE *stream;		/* open file stream */
    int ret;			/* libc function return */
    char *linep = NULL;		/* allocated line read from iocccsize */
    char *line;			/* Makefile line to parse */
    int rulenum = 0;		/* current rule number */
    char *p;
    char *saveptr = NULL;	/* for strtok_r */

    /*
     * firewall
     */
    if (Makefile == NULL || infop == NULL) {
	err(142, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * open Makefile
     */
    errno = 0;			/* pre-clear errno for errp() */
    stream = fopen(Makefile, "r");
    if (stream == NULL) {
	errp(143, __func__, "cannot open Makefile: %s", Makefile);
	not_reached();
    }

    /*
     * process lines until EOF
     */
    do {

	/*
	 * process the next line
	 */
	line = readline_dup(&linep, true, NULL, stream);
	if (line == NULL) {
	    break;
	}

	/*
	 * free storage
	 */
	if (linep != NULL) {
	    free(linep);
	    linep = NULL;
	}

	/*
	 * trim off comments
	 */
	p = strchr(line, '#');
	if (p != NULL) {
	    /*
	     * trim off comment
	     */
	    *p = '\0';
	}

        /*
         * trim off '=' as some variables can have a ':' which can confuse the
         * checks below
         */
        p = strchr(line, '=');
        if (p != NULL) {
            /*
             * trim off variable
             */
            *p = '\0';
        }

        /*
         * skip lines starting with a '.' as those are special make rules
         */
        if (*line == '.') {
            /*
             * NOTE: due to a bug or mis-feature in GitHub's advanced security
             * bot we cannot explicitly check for NULL line before freeing which
             * means that if someone messed with line here it could be a
             * problem. This is because of the dereference above.
             *
             * We know it is not a short-circuiting not being understood because
             * we tried it both ways.
             *
             * Now the pedant would point out it is safe to free a NULL pointer
             * but in this repo we always check for != NULL before freeing
             * anyway. However since we have to dereference line we cannot
             * (because of GitHub) check for NULL before freeing it. That does
             * mean that if someone ended up setting it to NULL by mistake or
             * for some other reason, here, and if we did not add the check back
             * (which might or might not make GitHub complain), it would end up
             * freeing a NULL pointer. But of course it would not be a problem
             * if it was NULL; rather it would be a problem if it was freed or
             * set to some other location, and then dereferenced.
             *
             * Of course the fact we do pass it to strchr() means we don't
             * technically need to check for line != NULL above but here we
             * cannot check before freeing because GitHub will issue a
             * complaint; if we do not check for != NULL GitHub does not
             * complain, suggesting that it simply does not know what it is
             * doing.
             *
             * TL;DR: the fact we have to dereference line prevents us from
             * checking line for != NULL due to GitHub's advanced security bot
             * issuing a complaint about a redundant check, even though it's
             * perfectly safe.
             *
             * This is why we do not check for NULL here prior to freeing,
             * unlike the rest of the repo.
             */

            /*
             * free storage
             */
            free(line);
            line = NULL;

            /*
             * built-in Makefile rule
             */
            continue;
        }
	/*
	 * skip line if there is no :
	 */
	p = strchr(line, ':');
	if (p == NULL) {

	    /*
	     * free storage
             *
             * We can't check for != NULL because of GitHub.
	     */
            free(line);
            line = NULL;

	    /*
	     * non-: line
	     */
	    continue;

	/*
	 * we appear to have a make rule line
	 */
	} else {
	    /* trim off : and later dependency text */
	    *p = 0;
	}
	++rulenum;

	/*
	 * split the line into whitespace separated tokens
	 */
	for (p = strtok_r(line, " \t", &saveptr); p != NULL; p = strtok_r(NULL, ":", &saveptr)) {

	    /*
	     * detect all rule
	     */
	    dbg(DBG_VHIGH, "rulenum[%d]: token: %s", rulenum, p);
	    if (!infop->found_all_rule && strcmp(p, "all") == 0) {
		/*
		 * first all rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: all token found", rulenum);
		infop->found_all_rule = true;
		if (rulenum == 1) {
		    /*
		     * all rule is in first rule line
		     */
		    infop->first_rule_is_all = true;
		    break;
		}

	    /*
	     * detect clean rule
	     */
	    } else if (!infop->found_clean_rule && strcmp(p, "clean") == 0) {
		/*
		 * first clean rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: clean token found", rulenum);
		infop->found_clean_rule = true;

	    /*
	     * detect clobber rule
	     */
	    } else if (!infop->found_clobber_rule && strcmp(p, "clobber") == 0) {
		/*
		 * first clobber rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: clobber token found", rulenum);
		infop->found_clobber_rule = true;

	    /*
	     * detect try rule
	     */
	    } else if (!infop->found_try_rule && strcmp(p, "try") == 0) {
		/*
		 * first try rule found
		 */
		dbg(DBG_HIGH, "rulenum[%d]: try token found", rulenum);
		infop->found_try_rule = true;
	    }
	}

	/*
	 * free storage
         *
         * We can't check for != NULL because of GitHub.
	 */
        free(line);
        line = NULL;

    } while (!infop->first_rule_is_all || !infop->found_all_rule || !infop->found_clean_rule ||
	     !infop->found_clobber_rule || !infop->found_try_rule);

    /*
     * close Makefile
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(stream);
    if (ret < 0) {
	errp(144, __func__, "fclose error");
	not_reached();
    }

    /*
     * free storage
     */
    if (line != NULL) {
	free(line);
	line = NULL;
    }

    /*
     * if our parse of Makefile was successful
     */
    if (infop->first_rule_is_all && infop->found_all_rule && infop->found_clean_rule &&
	infop->found_clobber_rule && infop->found_try_rule) {
	dbg(DBG_MED, "Makefile appears to pass");
	return true;
    }

    return false;
}


/*
 * warn_Makefile - warn user that Makefile is invalid/incomplete
 *
 * given:
 *
 *      infop		  - pointer to info struct
 *
 * This function does not return on error.
 */
static void
warn_Makefile(struct info *infop)
{
    bool yorn = false;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(145, __func__, "called with NULL infop");
	not_reached();
    }
    if (need_confirm && (!answer_yes || seed_used)) {

	/*
	 * report problem with Makefile
	 */
	fpara(stderr,
	      "",
	      "At least one problem was detected with the Makefile provided:",
	      "",
	      NULL);
	if (!infop->first_rule_is_all) {
	    fpara(stderr, "The all rule appears to not be the first (default) rule.",
		  "",
		  NULL);
	}
	if (!infop->found_all_rule) {
	    fpara(stderr,
		  "  The Makefile appears to not have an all rule.",
		  "    The all rule should make your compiled/built program.",
		  "",
		  NULL);
	}
	if (!infop->found_clean_rule) {
	    fpara(stderr,
		  "  The Makefile appears to not have a clean rule.",
		  "    The clean rule should remove any intermediate build files.",
		  "    For example, remove .o files and other intermediate build files .",
		  "    The clean rule should NOT remove compiled/built program built by the all rule.",
		  "",
		  NULL);
	}
	if (!infop->found_clobber_rule) {
	    fpara(stderr,
		  "  The Makefile appears to not have a clobber rule.",
		  "    The clobber rule should restore the directory to the original submission state.",
		  "    The clobber rule should depend on the clean rule and should remove the submission's program,",
		  "    clean up after program execution (if needed) and restore the entire directory back",
		  "    to the original submission state.",
		  "",
		  NULL);
	}
	if (!infop->found_try_rule) {
	    fpara(stderr,
		  "  The Makefile appears to not have a try rule.",
		  "    The try rule should execute the program with suggested arguments (if any needed).",
		  "    The program may be executed more than once if such examples are informative.",
		  "	   The try rule should depend on the all rule.",
		  "",
		  NULL);
	}

	/*
	 * Explain again what is needed in a Makefile
	 */
	fpara(stderr,
	      "Makefiles must have the following Makefile rules:",
	      "",
	      "    all - compile the submission, must be the first submission",
	      "    clean - remove intermediate compilation files",
	      "    clobber - clean, remove compiled submission, restore to the original submission state",
	      "    try - invoke the submission at least once",
	      "",
	      "While this program's parser may have missed finding those Makefile rules,",
	      "chances are this file is not a proper Makefile under the IOCCC rules.",
	      "",
	      NULL);

	/*
	 * Ask if they want to submit it anyway unless -y
	 */
	if (!answer_yes) {
	    yorn = yes_or_no("Do you still want to submit this Makefile in the hopes that it is OK? [yn]");
	    if (!yorn) {
		err(146, __func__, "Use a different Makefile or modify your Makefile");
		not_reached();
	    }
	}
    }
}


/*
 * check_Makefile - check Makefile arg and if OK, copy into submission_dir/Makefile
 *
 * Check if the Makefile argument is a readable file, and
 * if it has the proper rules (starting with all:), use copyfile() to copy into
 * submission_dir/Makefile.
 *
 * given:
 *      infop           - pointer to info structure
 *      submission_dir  - newly created submission directory (by mk_submission_dir()) under workdir
 *      Makefile        - Makefile arg: given path to Makefile
 *
 * This function does not return on error.
 */
static void
check_Makefile(struct info *infop, char const *submission_dir, char const *Makefile)
{
    off_t filesize = 0;		/* size of Makefile */
    size_t makefile_filename_len;	/* length of the Makefile path */
    char *path = NULL;          /* full path of Makefile (submission_dir/Makefile) */

    /*
     * firewall
     */
    if (infop == NULL || submission_dir == NULL || Makefile == NULL) {
	err(147, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * Makefile filename length MUST be > 0 && <= MAX_FILENAME_LEN!
     */
    makefile_filename_len = strlen(Makefile);
    if (!test_filename_len(Makefile)) {
	err(148, __func__, "Makefile filename length: %ju: is not > 0 && <= %ju", (uintmax_t)makefile_filename_len,
                (uintmax_t)MAX_FILENAME_LEN);
	not_reached();
    }

    /*
     * Makefile must be a non-empty readable file
     */
    if (!exists(Makefile)) {
	fpara(stderr,
	      "",
	      "We cannot find the Makefile.",
	      "",
	      NULL);
	err(149, __func__, "Makefile does not exist: %s", Makefile);
	not_reached();
    }
    if (!is_file(Makefile)) {
	fpara(stderr,
	       "",
	       "The Makefile path, while it exists, is not a regular file.",
	       "",
	       NULL);
	err(150, __func__, "Makefile is not a regular file: %s", Makefile);
	not_reached();
    }
    if (!is_read(Makefile)) {
	fpara(stderr,
	      "",
	      "The Makefile path, while it is a file, is not readable.",
	      "",
	      NULL);
	err(151, __func__, "Makefile is not readable file: %s", Makefile);
	not_reached();
    }
    filesize = file_size(Makefile);
    if (filesize < 0) {
	err(152, __func__, "file_size error: %jd on Makefile  %s", (intmax_t)filesize, Makefile);
	not_reached();
    } else if (filesize == 0) {
	err(153, __func__, "Makefile cannot be empty: %s", Makefile);
	not_reached();
    }

    /*
     * scan Makefile for critical rules
     */
    if (!inspect_Makefile(Makefile, infop)) {
	warn_Makefile(infop);
	infop->Makefile_override = true;
    } else {
	infop->Makefile_override = false;
    }

    /*
     * get full path
     */
    path = calloc_path(submission_dir, "Makefile");
    if (path == NULL) {
        err(154, __func__, "calloc_path(\"%s\", \"Makefile\") failed", submission_dir);
        not_reached();
    }


    /*
     * copy Makefile under submission_dir
     */
    copyfile(Makefile, path, false, S_IRUSR | S_IRGRP | S_IROTH);

    /*
     * free path
     */
    if (path != NULL) {
        free(path);
        path = NULL;
    }

    /*
     * save Makefile filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->Makefile = strdup("Makefile");
    if (infop->Makefile == NULL) {
	errp(155, __func__, "malloc #1 of %ju bytes failed", (uintmax_t)(LITLEN("Makefile") + 1));
	not_reached();
    }
    return;
}


/*
 * check_remarks_md - check remarks_md arg and if OK, copy into submission_dir/Makefile
 *
 * Check if the remarks_md argument is a readable file, and
 * if it is not empty, use copyfile() to copy into submission_dir/remarks.md.
 *
 * given:
 *      infop           - pointer to info structure
 *      submission_dir  - the newly created submission directory (by mk_submission_dir()) under workdir
 *      remarks_md      - remarks_md arg: given path to author's remarks markdown file
 *
 * This function does not return on error.
 */
static void
check_remarks_md(struct info *infop, char const *submission_dir, char const *remarks_md)
{
    off_t filesize = 0;		/* size of remarks.md */
    size_t remarks_filename_len;	/* length of the remarks.md path */
    char *path = NULL;          /* full path of file (submission_dir/remarks.md) */

    /*
     * firewall
     */
    if (infop == NULL || submission_dir == NULL || remarks_md == NULL) {
	err(156, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * remarks.md filename length MUST be > 0 && < MAX_FILENAME_LEN
     */
    remarks_filename_len = strlen(remarks_md);
    if (!test_filename_len(remarks_md)) {
	err(157, __func__, "remarks_md filename length: %ju: is not > 0 && <= %ju", (uintmax_t)remarks_filename_len,
                (uintmax_t)MAX_FILENAME_LEN);
	not_reached();
    }


    /*
     * remarks_md must be a non-empty readable file
     */
    if (!exists(remarks_md)) {
	fpara(stderr,
	       "",
	       "We cannot find the remarks.md file.",
	       "",
	       NULL);
	err(158, __func__, "remarks.md does not exist: %s", remarks_md);
	not_reached();
    }
    if (!is_file(remarks_md)) {
	fpara(stderr, "",
	      "The remarks.md path, while it exists, is not a regular file.",
	      "",
	      NULL);
	err(159, __func__, "remarks.md is not a regular file: %s", remarks_md);
	not_reached();
    }
    if (!is_read(remarks_md)) {
	fpara(stderr,
	      "",
	      "The remarks.md path, while it is a file, is not readable.",
	      "",
	      NULL);
	err(160, __func__, "remarks.md is not readable file: %s", remarks_md);
	not_reached();
    }
    filesize = file_size(remarks_md);
    if (filesize < 0) {
	err(161, __func__, "file_size error: %jd on remarks.md %s", (intmax_t)filesize, remarks_md);
	not_reached();
    } else if (filesize == 0) {
	err(162, __func__, "remarks.md cannot be empty: %s", remarks_md);
	not_reached();
    }

    /*
     * get full path
     */
    path = calloc_path(submission_dir, "remarks.md");
    if (path == NULL) {
        err(163, __func__, "calloc_path(\"%s\", \"remarks.md\") failed", submission_dir);
        not_reached();
    }

    /*
     * copy remarks.md under submission_dir
     */
    copyfile(remarks_md, path, false, S_IRUSR | S_IRGRP | S_IROTH);

    /*
     * free path
     */
    if (path != NULL) {
        free(path);
        path = NULL;
    }

    /*
     * save remarks_md filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->remarks_md = strdup("remarks.md");
    if (infop->remarks_md == NULL) {
	errp(164, __func__, "malloc #1 of %ju bytes failed", (uintmax_t)(LITLEN("remarks.md") + 1));
	not_reached();
    }

    return;
}


/*
 * yes_or_no - determine if input is yes or no
 *
 * given:
 *      question        - string to prompt for a question
 *
 * returns:
 *      true ==> input is yes in some form,
 *      false ==> input is not yes
 */
static bool
yes_or_no(char const *question)
{
    char *response;		/* response to the question */
    char *p;

    /*
     * firewall
     */
    if (question == NULL) {
	err(165, __func__, "called with NULL question");
	not_reached();
    }

    /*
     * if -y, return true
     */
    if (answer_yes) {
	/* return yes */
	return true;
    }

    /*
     * ask the question and obtain the response
     */
    do {
	response = prompt(question, NULL);

	/*
	 * convert response to lower case
	 */
	for (p = response; *p != '\0'; ++p) {
	    if (isascii(*p) && isalpha(*p)) {
		*p = (char)tolower(*p);
	    }
	}

	/*
	 * check for a valid reply
	 */
	if (strcmp(response, "y") == 0 || strcmp(response, "yes") == 0) {

	    /*
	     * free storage
	     */
	    if (response != NULL) {
		free(response);
		response = NULL;
	    }

	    /*
	     * return yes
	     */
	    return true;

	} else if (strcmp(response, "n") == 0 || strcmp(response, "no") == 0) {

	    /*
	     * free storage
	     */
	    if (response != NULL) {
		free(response);
		response = NULL;
	    }

	    /*
	     * return no
	     */
	    return false;
	}

	/*
	 * reject response and ask again
	 */
	fpara(stderr,
	      "Please enter either y (yes) or n (no)",
	      NULL);

	/*
	 * free storage
	 */
	if (response != NULL) {
	    free(response);
	    response = NULL;
	}

    } while (response == NULL);

    /*
     * free storage
     */
    if (response != NULL) {
	free(response);
	response = NULL;
    }

    /*
     * should not get here - but assume no if we do
     */
    return false;
}


/*
 * get_title - get the title of the submission
 *
 * Ask the user for an submission title, validate the response
 * and return the allocated title.
 *
 * given:
 *      infop   - pointer to info structure
 *
 * returns:
 *      allocated and validated title
 *
 * This function does not return on error.
 */
static char *
get_title(struct info *infop)
{
    char *title = NULL;		/* submission title to return or NULL */
    size_t len;			/* length of title */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(166, __func__, "called with NULL infop");
	not_reached();
    }

    /*
     * inform the user of the title
     */
    if (need_hints) {
	para("A submission title is a short name using the [a-z0-9][a-z0-9._+-]* regex pattern.",
	      "",
	      "If your submission wins, the title might become the directory name of the winning entry,",
	      "although the IOCCC judges might change the title for various reason.",
	      "",
	      "If you have more than one submission to submit, please make your titles unique",
	      "amongst the submissions that you submit to the current IOCCC.",
	      "",
	      NULL);
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "Your title must be between 1 and %d ASCII characters long.\n\n", MAX_TITLE_LEN);
	if (ret <= 0) {
	    warnp(__func__, "fprintf #0 error: %d", ret);
	}
    }

    /*
     * ask the question and obtain the response
     */
    do {

	/*
	 * obtain the reply
	 */
	title = prompt("Enter a title for your submission", NULL);

	/*
	 * title cannot be empty
	 */
	len = strlen(title);
	if (len == 0) {

	    /*
	     * reject empty title
	     */
	    fpara(stderr,
		  "",
		  "The title cannot be an empty string.",
		  "",
		  NULL);
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;

	/*
	 * title cannot be too long
	 */
	} else if (len > MAX_TITLE_LEN) {

	    /*
	     * reject long title
	     */
	    fpara(stderr,
		  "",
		  "That title is too long.",
		  "",
		  NULL);
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "Your title must be between 1 and %d ASCII characters long.\n\n", MAX_TITLE_LEN);
	    if (ret <= 0) {
		warnp(__func__, "fprintf #1 error: %d", ret);
	    }
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;
	}

	/*
	 * verify that the title has only POSIX portable filename and + chars
	 */
	if (posix_plus_safe(title, true, false, true) == false) {

	    /*
	     * reject invalid chars in title
	     */
	    fpara(stderr,
		  "",
		  "Your title contains invalid characters. A title must match the following regex:",
		  "",
		  "    ^[0-9a-z][0-9a-z._+-]*$",
		  "",
		  "That is, it must start with a lower case letter ASCII [a-z] or digit [0-9]",
		  "followed by zero or more lower case letters ASCII [a-z], digits [0-9],",
		  "- (ASCII dash), + (ASCII plus), or _ (ASCII underscore).",
		  "",
		  NULL);
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;
	}
    } while (title == NULL);


    /*
     * returned allocated title
     */
    return title;
}


/*
 * get_abstract - get the abstract of the submission
 *
 * Ask the user for a submission abstract, validate the response
 * and return the allocated abstract.
 *
 * given:
 *      infop           - pointer to info structure
 *
 * returns:
 *      allocated and validated abstract
 *
 * This function does not return on error.
 */
static char *
get_abstract(struct info *infop)
{
    char *abstract = NULL;	/* submission abstract to return or NULL */
    size_t len;			/* length of abstract */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(167, __func__, "called with NULL infp");
	not_reached();
    }

    /*
     * inform the user of the abstract
     */
    if (!silence_prompt) {
	para("",
	     "A submission abstract is 1-line summary of your submission.",
	     "",
	     NULL);
    }

    /*
     * ask the question and obtain the response
     */
    do {

	/*
	 * obtain the reply
	 */
	abstract = prompt("Enter a 1-line abstract of your submission", NULL);

	/*
	 * abstract cannot be empty
	 */
	len = strlen(abstract);
	if (len == 0) {

	    /*
	     * reject empty abstract
	     */
	    fpara(stderr,
		  "",
		  "The abstract cannot be an empty string.",
		  "",
		  NULL);
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }

	    /*
	     * free storage
	     */
	    if (abstract != NULL) {
		free(abstract);
		abstract = NULL;
	    }
	    continue;

	/*
	 * abstract cannot be too long
	 */
	} else if (len > MAX_ABSTRACT_LEN) {

	    /*
	     * reject long abstract
	     */
	    fpara(stderr,
		  "",
		  "That abstract is too long.",
		  "",
		  NULL);
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "Your abstract must be between 1 and %d characters long.\n\n", MAX_ABSTRACT_LEN);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error: %d", ret);
	    }
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }

	    /*
	     * free storage
	     */
	    if (abstract != NULL) {
		free(abstract);
		abstract = NULL;
	    }
	    continue;
	}
    } while (abstract == NULL);

    /*
     * returned allocated abstract
     */
    return abstract;
}


/*
 * noprompt_yes_or_no - read yes or no without a prompt
 *
 * Unlike the yes_or_no() function, this function does NOT issue a prompt.
 *
 * Unlike the yes_or_no() function, this function does NOT attempt to
 * re-ask is the input not valid.  Unless the input is "y" or "yes"
 * regardless of the case, this function will return false, even
 * with a read error.
 *
 * returns:
 *      true ==> input is yes in some form,
 *      false ==> input is not yes or there was a read error.
 *
 * NOTE: The -y (answer_yes) as no impact on this function as
 *	 the yes or no input will be read regardless.
 */
static bool
noprompt_yes_or_no(void)
{
    char *linep = NULL;		/* readline_dup line buffer */
    size_t len;			/* length of input */
    char *response;		/* yes or no response */
    char *p;

    /*
     * read user input - return input length
     */
    errno = 0;		/* pre-clear errno for warnp() */
    response = readline_dup(&linep, true, &len, input_stream);
    if (response == NULL) {

	/*
	 * case: readline_dup error
	 */
	warnp(__func__, "readline_dup returned NULL, assuming a no answer");
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on first warning"); /*ooo*/
            not_reached();
	}
	dbg(DBG_HIGH, "due to readline_dup error, returning false");
	return false;
    }

    /*
     * examine response length
     */
    dbg(DBG_VVHIGH, "received a %ju byte response", (uintmax_t)len);
    if (len <= 0) {

	/*
	 * free storage
	 */
	if (response != NULL) {
	    free(response);
	    response = NULL;
	}

	/*
	 * report empty response
	 */
	warn(__func__, "readline_dup returned empty string, assuming a no answer");
	if (abort_on_warning) {
	    err(1, __func__, "-E forcing exit on first warning"); /*ooo*/
            not_reached();
	}
	dbg(DBG_HIGH, "due to readline_dup error, returning false");
	return false;
    }

    /*
     * convert response to lower case
     */
    for (p = response; *p != '\0'; ++p) {
	if (isascii(*p) && isalpha(*p)) {
	    *p = (char)tolower(*p);
	}
    }
    dbg(DBG_VHIGH, "response converted into lower case is: \"%s\"", response);

    /*
     * check for a valid reply
     */
    if (strcmp(response, "y") == 0 || strcmp(response, "yes") == 0) {

	/*
	 * free storage
	 */
	if (response != NULL) {
	    free(response);
	    response = NULL;
	}

	/*
	 * return yes
	 */
	dbg(DBG_HIGH, "yes read, returning true");
	return true;

    } else if (strcmp(response, "n") == 0 || strcmp(response, "no") == 0) {

	/*
	 * free storage
	 */
	if (response != NULL) {
	    free(response);
	    response = NULL;
	}

	/*
	 * return no
	 */
	dbg(DBG_HIGH, "no read, returning false");
	return false;
    }

    /*
     * free storage
     */
    if (response != NULL) {
	free(response);
	response = NULL;
    }

    /*
     * invalid response
     */
    warn(__func__, "response was neither yes nor no");
    if (abort_on_warning) {
	err(1, __func__, "-E forcing exit on first warning"); /*ooo*/
        not_reached();
    }

    /*
     * should not get here - but assume no if we do
     */
    dbg(DBG_HIGH, "due to invalid response, returning false");
    return false;
}


/*
 * get_author_info - obtain information on submission authors
 *
 * given:
 *      author_set      - pointer to array of authors
 *
 * returns:
 *      number of authors
 *
 * This function does not return on error.
 */
static int
get_author_info(struct author **author_set_p)
{
    struct author *author_set = NULL;	/* allocated author set */
    int author_count = -1;		/* number of authors or -1 */
    char *author_count_str = NULL;	/* author count string */
    bool yorn = false;		/* response to a question */
    size_t len;			/* length of reply */
    int ret;			/* libc function return */
    char guard;			/* scanf guard to catch excess amount of input */
    char *def_handle = NULL;	/* default author handle computed from name */
    bool pass = false;		/* true ==> passed test */
    char *p, *last_at;
    int i = 0;
    int j = 0;

    /*
     * firewall
     */
    if (author_set_p == NULL) {
	err(168, __func__, "called with NULL author_set_p");
	not_reached();
    }

    /*
     * keep asking for an author number until we get a valid reply
     */
    do {

	/*
	 * ask for author count
	 */
	author_count_str = prompt("\nEnter the number of authors of this submission", NULL);

	/*
	 * convert author_count_str to number
	 */
	ret = sscanf(author_count_str, "%d%c", &author_count, &guard);
	if (ret != 1 || test_author_count(author_count) == false) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "\nThe number of authors must be a number from 1 through %d; please re-enter.\n", MAX_AUTHORS);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error #0 while printing author number range");
	    }
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "If you happen to have more than %d authors, we ask that you pick\n", MAX_AUTHORS);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error #1 while printing author number range");
	    }
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "just %d authors and mention the remainder of the authors in the remarks file.\n", MAX_AUTHORS);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error #2 while printing author number range");
	    }
	    author_count = -1;	/* invalidate input */
	    if (abort_on_warning) {
		err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
	    }
	}

	/*
	 * free storage
	 */
	if (author_count_str != NULL) {
	    free(author_count_str);
	    author_count_str = NULL;
	}

    } while ((author_count < 1 || author_count > MAX_AUTHORS));

    dbg(DBG_HIGH, "will request information on %d authors", author_count);

    /*
     * allocate the author array
     */
    errno = 0;			/* pre-clear errno for errp() */
    author_set = (struct author *) malloc(sizeof(struct author) * (size_t)author_count);
    if (author_set == NULL) {
	errp(169, __func__, "malloc a struct author array of length: %d failed", author_count);
	not_reached();
    }

    /*
     * pre-zeroize the author array
     */
    memset(author_set, 0, sizeof(struct author) * (size_t)author_count);

    /*
     * inform the user about the author information we need to collect
     */
    if (need_hints) {
	para("",
	     "We will now ask for information about the author(s) of this submission.",
	     "",
	     "Information that you supply, if your submission is selected as a winning entry,",
	     "will be published with your submission.",
	     "",
	     "Except for your name and location/country code, you can opt out of providing it,",
	     "(or if you don't have the thing we are asking for), by just pressing return.",
	     "",
	     "A name is required. If an author wishes to be anonymous, use a pseudonym.",
	     "Keep in mind that if an author wins multiple years, or has won before, you might",
	     "want to be consistent and provide the same name or pseudonym each time.",
	     "",
	     "We will ask for the location/country as a 2 character ISO 3166-1 Alpha-2 code.",
	     "",
	     "If you wish your location/country to be anonymous, use the code: XX",
	     "",
	     "See the following URLs for information on ISO 3166-1 Alpha-2 codes:",
	     "",
	     NULL);
	errno = 0;		/* pre-clear errno for warnp() */
	ret = puts(ISO_3166_1_CODE_URL0);
	if (ret < 0) {
	    warnp(__func__, "puts error printing ISO 3166-1 URL0");
	}
	errno = 0;		/* pre-clear errno for warnp() */
	ret = puts(ISO_3166_1_CODE_URL1);
	if (ret < 0) {
	    warnp(__func__, "puts error printing ISO 3166-1 URL1");
	}
	errno = 0;		/* pre-clear errno for warnp() */
	ret = puts(ISO_3166_1_CODE_URL2);
	if (ret < 0) {
	    warnp(__func__, "puts error printing ISO 3166-1 URL2");
	}
	errno = 0;		/* pre-clear errno for warnp() */
	ret = puts(ISO_3166_1_CODE_URL3);
	if (ret < 0) {
	    warnp(__func__, "puts error printing ISO 3166-1 URL3");
	}
	errno = 0;		/* pre-clear errno for warnp() */
	ret = puts(ISO_3166_1_CODE_URL4);
	if (ret < 0) {
	    warnp(__func__, "puts error printing ISO 3166-1 URL4");
	}
	para("",
	     "We will ask for the author(s) Email address. Press return if you don't want to provide it.",
	     "We will ask for a home URL (starting with http:// or https://). Each author may provide",
	     "up to two URLs. Press return to skip.",
	     "We will ask a Mastodon handle (must start with @), or press return to skip.",
	     "We will ask a GitHub account (must start with @), or press return to skip.",
	     "We will ask for an affiliation (company, school, group) of the author.",
	     "We will ask if you have won the IOCCC before. Your answer will not affect your chances of winning.",
	     "We will ask you for an author handle. You should select the default unless you have won the IOCCC before.",
	     NULL);
    }

    /*
     * collect information on authors
     */
    for (i = 0; i < author_count; ++i) {

	/*
	 * announce author number
	 */
	errno = 0;		/* pre-clear errno for warnp() */
	ret = printf("\nEnter information for author #%d\n\n", i);
	if (ret <= 0) {
	    warnp(__func__, "printf error printing author number");
	}
	author_set[i].author_num = i;

	/*
	 * obtain author name
	 */
	do {

	    /*
	     * prompt for the author name
	     */
	    author_set[i].name = NULL;
	    author_set[i].name = prompt("Enter author name", &len);
	    dbg(DBG_MED, "read: author[%d] name: %s", i, author_set[i].name);

	    /*
	     * reject empty author name
	     */
	    if (len <= 0) {

		/*
		 * issue rejection message
		 */
		fpara(stderr,
		      "",
		      "The author name cannot be empty, try again.  If they want to be anonymous, give a pseudonym.",
		      "",
		      NULL);
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].name != NULL) {
		    free(author_set[i].name);
		    author_set[i].name = NULL;
		}

	    /*
	     * reject if name is too long
	     */
	    } else if (len > MAX_NAME_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit names to %d characters\n\n", MAX_NAME_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while reject name that is too long");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].name != NULL) {
		    free(author_set[i].name);
		    author_set[i].name = NULL;
		}

	    /*
	     * reject if author name is a duplicate of a previous author name
	     */
	    } else if (i > 0) {
		for (j=0; j < i; ++j) {
		    if (strcmp(author_set[i].name, author_set[j].name) == 0) {

			/*
			 * issue rejection message
			 */
			errno = 0;		/* pre-clear errno for warnp() */
			ret = fprintf(stderr, "\nauthor #%d name duplicates previous author #%d name", i, j);
			if (ret <= 0) {
			    warnp(__func__, "fprintf error while reject duplicate name");
			}
			if (abort_on_warning) {
			    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
			}

			/*
			 * free storage
			 */
			if (author_set[i].name != NULL) {
			    free(author_set[i].name);
			    author_set[i].name = NULL;
			}
			break;
		    }
		}
	    }
	} while (author_set[i].name == NULL);
	dbg(DBG_LOW, "Submission: author[%d] name: %s", i, author_set[i].name);

	/*
	 * obtain author location/country code
	 */
	do {

	    /*
	     * request location/country code
	     */
	    author_set[i].location_code = NULL;
	    author_set[i].location_code = prompt("Enter author 2 character location/country code (XX for anonymous)", &len);
	    dbg(DBG_MED, "read: author[%d] location code: %s", i, author_set[i].location_code);

	    /*
	     * inspect code input
	     */
	    if (len != 2 ||
		!isascii(author_set[i].location_code[0]) || !isalpha(author_set[i].location_code[0]) ||
		!isascii(author_set[i].location_code[1]) || !isalpha(author_set[i].location_code[1])) {

		/*
		 * provide more help on location/country codes
		 */
		fpara(stderr,
		      "",
		      "Location/country codes are two letters.",
		      "",
		      "For ISO 3166-1 2 character codes, see: the Alpha-2 code column of:",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "%s\n\n", ISO_3166_1_CODE_URL0);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #0");
		}
		fpara(stderr,
		      "or from these Wikipedia / ISO web pages:",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "%s\n", ISO_3166_1_CODE_URL1);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #1");
		}
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL2);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #2");
		}
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL3);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #3");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].location_code != NULL) {
		    free(author_set[i].location_code);
		    author_set[i].location_code = NULL;
		}

		/*
		 * discard this invalid location/country code input
		 */
		author_set[i].location_name = NULL;
		author_set[i].common_name = NULL;
		yorn = false;
		continue;

	    }

	    /*
	     * force ASCII alpha code into canonical UPPER CASE code
	     */
	    author_set[i].location_code[0] = (char)toupper(author_set[i].location_code[0]);
	    author_set[i].location_code[1] = (char)toupper(author_set[i].location_code[1]);

	    /*
	     * determine if code is known
	     */
	    if (test_location_code(author_set[i].location_code) == false) {

		/*
		 * provide more help on location/country codes
		 */
		fpara(stderr,
		      "",
		      "That is not a known location/country code.",
		      "",
		      "For ISO 3166-1 2 character codes, see: the Alpha-2 code column of:",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL0);
		if (ret <= 0) {
		    warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #0");
		}
		fpara(stderr,
		      "or from these Wikipedia / ISO web pages:",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL1);
		if (ret <= 0) {
		    warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #1");
		}
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL2);
		if (ret <= 0) {
		    warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #2");
		}
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL3);
		if (ret <= 0) {
		    warnp(__func__, "fprintf when printing ISO 3166-1 CODE URL #3");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].location_code != NULL) {
		    free(author_set[i].location_code);
		    author_set[i].location_code = NULL;
		}

		/*
		 * discard this invalid location/country code input
		 */
		author_set[i].location_name = NULL;
		author_set[i].common_name = NULL;
		yorn = false;
		continue;
	    }
	    author_set[i].location_name = lookup_location_name(author_set[i].location_code, false);
	    author_set[i].common_name = lookup_location_name(author_set[i].location_code, true);

	    /*
	     * verify the known location/country code
	     */
	    if (need_confirm && !answer_yes) {
		errno = 0;		/* pre-clear errno for warnp() */
		ret = printf("The location/country code you entered is assigned to: %s (%s)\n",
			     author_set[i].location_name, author_set[i].common_name);
		if (ret <= 0) {
		    warnp(__func__, "fprintf location/country code assignment");
		}
		yorn = yes_or_no("Is that location/country code correct? [yn]");

		/*
		 * free storage if no (re-enter location/country code)
		 */
		if (!yorn) {
		    if (author_set[i].location_code != NULL) {
			free(author_set[i].location_code);
			author_set[i].location_code = NULL;
		    }
		}
	    } else {
		yorn = true;
	    }
	} while (author_set[i].location_code == NULL ||
		 author_set[i].location_name == NULL ||
		 author_set[i].common_name == NULL ||
		 !yorn);
	dbg(DBG_LOW, "Submission: author[%d] location: %s", i, author_set[i].location_code);
	dbg(DBG_LOW, "\t      location code: %s == %s (%s)",
		     author_set[i].location_code, author_set[i].location_name, author_set[i].common_name);

	/*
	 * ask for Email address
	 */
	do {

	    /*
	     * request Email address
	     */
	    author_set[i].email = NULL;
	    author_set[i].email = prompt(need_hints ?
		"Enter author email address, or press return to skip" :
		"Enter author email address", &len);
	    if (len == 0) {
		dbg(DBG_MED, "read: author[%d] Email address withheld", i);
	    } else {
		dbg(DBG_MED, "read: author[%d] Email: %s", i, author_set[i].email);
	    }

	    /*
	     * sanity check the Email address
	     */
	    pass = test_email(author_set[i].email);
	    if (pass == false) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit Email address to %d characters\n", MAX_EMAIL_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing Email address length limit");
		}
		fpara(stderr,
		      "and we require that Email addresses must only a single @ somewhere inside the string.",
		      "",
		      NULL);
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].email != NULL) {
		    free(author_set[i].email);
		    author_set[i].email = NULL;
		}
		continue;
	    }
	} while (author_set[i].email == NULL);
	if (len == 0) {
	    dbg(DBG_LOW, "Submission: author[%d] Email address withheld", i);
	} else {
	    dbg(DBG_LOW, "Submission: author[%d] Email address: %s", i, author_set[i].email);
	}

	/*
	 * ask for main URL
	 */
	do {

	    /*
	     * request URL
	     */
	    author_set[i].url = NULL;
	    author_set[i].url =
		prompt(need_hints ?
		    "Enter author home page URL (starting with http:// or https://), or press return to skip" :
		    "Enter author home page URL", &len);
	    if (len == 0) {
		dbg(DBG_MED, "read: author[%d] URL withheld", i);
	    } else {
		dbg(DBG_MED, "read: author[%d] URL: %s", i, author_set[i].url);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_URL_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit URLs to %d characters\n\n", MAX_URL_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing URL length limit");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].url != NULL) {
		    free(author_set[i].url);
		    author_set[i].url = NULL;
		}
		continue;
	    }

	    /*
	     * if it starts with http:// or https:// and has more characters, it is OK
	     */
	    if (len > 0) {
		if (((strncmp(author_set[i].url, "http://", LITLEN("http://")) == 0) &&
		     (author_set[i].url[LITLEN("http://")] != '\0')) ||
		    ((strncmp(author_set[i].url, "https://", LITLEN("https://")) == 0) &&
		     (author_set[i].url[LITLEN("https://")] != '\0'))) {

		    /*
		     * URL appears to in valid form
		     */
		    break;

		/*
		 * reject if it does not start with http:// or https://
		 */
		} else if (len > 0) {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "URLs must begin with http:// or https:// followed by the rest of the home page URL",
			  "",
			  NULL);
		    if (abort_on_warning) {
			err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		    }

		    /*
		     * free storage
		     */
		    if (author_set[i].url != NULL) {
			free(author_set[i].url);
			author_set[i].url = NULL;
		    }
		    continue;
		}
	    }
	} while (author_set[i].url == NULL);
	if (len == 0) {
	    dbg(DBG_LOW, "Submission: author[%d] URL withheld", i);
	} else {
	    dbg(DBG_LOW, "Submission: author[%d] URL: %s", i, author_set[i].url);
	}

	/*
	 * ask for Alt URL
	 */
	do {

	    /*
	     * request URL
	     */
	    author_set[i].alt_url = NULL;
	    author_set[i].alt_url =
		prompt(need_hints ?
		    "Enter second URL (starting with http:// or https://), or press return to skip" :
		    "Enter second URL", &len);
	    if (len == 0) {
		dbg(DBG_MED, "read: author[%d] Alt URL withheld", i);
	    } else {
		dbg(DBG_MED, "read: author[%d] Alt URL: %s", i, author_set[i].alt_url);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_URL_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit URLs to %d characters\n\n", MAX_URL_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing URL length limit");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].alt_url != NULL) {
		    free(author_set[i].alt_url);
		    author_set[i].alt_url = NULL;
		}
		continue;
	    }

	    /*
	     * if it starts with http:// or https:// and has more characters, it is OK
	     */
	    if (len > 0) {
		if (((strncmp(author_set[i].alt_url, "http://", LITLEN("http://")) == 0) &&
		     (author_set[i].alt_url[LITLEN("http://")] != '\0')) ||
		    ((strncmp(author_set[i].alt_url, "https://", LITLEN("https://")) == 0) &&
		     (author_set[i].alt_url[LITLEN("https://")] != '\0'))) {

		    /*
		     * URL appears to in valid form
		     */
		    break;

		/*
		 * reject if it does not start with http:// or https://
		 */
		} else if (len > 0) {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "URLs must begin with http:// or https:// followed by the rest of the home page URL",
			  "",
			  NULL);
		    if (abort_on_warning) {
			err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		    }

		    /*
		     * free storage
		     */
		    if (author_set[i].alt_url != NULL) {
			free(author_set[i].alt_url);
			author_set[i].alt_url = NULL;
		    }
		    continue;
		}
	    }
	} while (author_set[i].alt_url == NULL);
	if (len == 0) {
	    dbg(DBG_LOW, "Submission: author[%d] Alt URL withheld", i);
	} else {
	    dbg(DBG_LOW, "Submission: author[%d] Alt URL: %s", i, author_set[i].alt_url);
	}

	/*
	 * ask for mastodon handle
	 */
	do {

	    /*
	     * request mastodon handle
	     */
	    author_set[i].mastodon = NULL;
	    author_set[i].mastodon = prompt(need_hints ?
		"Enter author Mastodon handle, starting with @, or press return to skip" :
		"Enter author Mastodon handle", &len);
	    if (len == 0) {
		dbg(DBG_MED, "read: author[%d] Mastodon handle withheld", i);
	    } else {
		dbg(DBG_MED, "read: author[%d] Mastodon handle: %s", i, author_set[i].mastodon);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_MASTODON_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit Mastodon handles to %d "
			"characters, starting with the @\n\n", MAX_MASTODON_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing mastodon handle length limit");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].mastodon != NULL) {
		    free(author_set[i].mastodon);
		    author_set[i].mastodon = NULL;
		}
		continue;
	    }

	    /*
	     * reject if no leading @ or there's not a second @
	     */
	    if (len > 0) {
		p = strchr(author_set[i].mastodon, '@');
		last_at = strrchr(author_set[i].mastodon, '@');
		if (author_set[i].mastodon[0] != '@' || p == NULL || last_at == NULL ||
		    last_at == p || count_char(author_set[i].mastodon, '@') != 2 || author_set[i].mastodon[1] == '\0' ||
		    strstr(author_set[i].mastodon, "@@") != NULL || author_set[i].mastodon[len-1] == '@') {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "Mastodon handles must start with a @, must have one other @ that's not adjacent to",
			  "the first and is not the last character in the handle.",
			  "",
			  NULL);
		    if (abort_on_warning) {
			err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		    }

		    /*
		     * free storage
		     */
		    if (author_set[i].mastodon != NULL) {
			free(author_set[i].mastodon);
			author_set[i].mastodon = NULL;
		    }
		    continue;
		}
	    }
	} while (author_set[i].mastodon == NULL);
	if (len == 0) {
	    dbg(DBG_LOW, "Submission: author[%d] Mastodon handle withheld", i);
	} else {
	    dbg(DBG_LOW, "Submission: author[%d] Mastodon handle: %s", i, author_set[i].mastodon);
	}

	/*
	 * ask for GitHub account
	 */
	do {

	    /*
	     * request GitHub account
	     */
	    author_set[i].github = NULL;
	    author_set[i].github = prompt(need_hints ?
		"Enter author GitHub account, starting with @, or press return to skip" :
		"Enter author GitHub account", &len);
	    if (len == 0) {
		dbg(DBG_MED, "read: author[%d] GitHub account withheld", i);
	    } else {
		dbg(DBG_MED, "read: author[%d] GitHub account: %s", i, author_set[i].github);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_GITHUB_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret =
		    fprintf(stderr,
			    "\nSorry ( tm Canada :-) ), we limit GitHub account names to %d characters after the 1st @\n\n",
			    MAX_GITHUB_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing GitHub user length limit");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].github != NULL) {
		    free(author_set[i].github);
		    author_set[i].github = NULL;
		}
		continue;
	    }

	    /*
	     * reject if no leading @, or if more than one @
	     */
	    if (len > 0) {
		p = strchr(author_set[i].github, '@');
		if (p == NULL || author_set[i].github[0] != '@' || p != strrchr(author_set[i].github, '@')
		    || author_set[i].github[1] == '\0') {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "GitHub accounts must start with a @ and have no other @-signs.",
			  "",
			  NULL);
		    if (abort_on_warning) {
			err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		    }

		    /*
		     * free storage
		     */
		    if (author_set[i].github != NULL) {
			free(author_set[i].github);
			author_set[i].github = NULL;
		    }
		    continue;
		}
	    }
	} while (author_set[i].github == NULL);
	if (len == 0) {
	    dbg(DBG_LOW, "Submission: author[%d] GitHub account withheld", i);
	} else {
	    dbg(DBG_LOW, "Submission: author[%d] GitHub account: %s", i, author_set[i].github);
	}

	/*
	 * ask for affiliation
	 */
	do {

	    /*
	     * request affiliation
	     */
	    author_set[i].affiliation = NULL;
	    author_set[i].affiliation = prompt(need_hints ?
		"Enter author affiliation, or press return to skip" :
		"Enter author affiliation", &len);
	    if (len == 0) {
		dbg(DBG_MED, "read: author[%d] Affiliation withheld", i);
	    } else {
		dbg(DBG_MED, "read: author[%d] Affiliation: %s", i, author_set[i].affiliation);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_AFFILIATION_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret =
		    fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit affiliation names to %d characters\n\n",
			    MAX_AFFILIATION_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing affiliation length limit");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].affiliation != NULL) {
		    free(author_set[i].affiliation);
		    author_set[i].affiliation = NULL;
		}
		continue;
	    }
	} while (author_set[i].affiliation == NULL);
	if (len == 0) {
	    dbg(DBG_LOW, "Submission: author[%d] Affiliation withheld", i);
	} else {
	    dbg(DBG_LOW, "Submission: author[%d] Affiliation: %s", i, author_set[i].affiliation);
	}

	/*
	 * ask if they are a past IOCCC winning author
	 */
	if (need_hints) {
	    para("",
	        "Please note: your next answer will not affect your chances of winning the IOCCC;",
		"we just need to know if you are a past IOCCC winning author in case you do win.",
		"This will simply help us identify all of your winning entries on the IOCCC website.",
		"",
		NULL);
	}
	if (seed_used) {
	    author_set[i].past_winning_author = noprompt_yes_or_no();
	} else {
	    author_set[i].past_winning_author = yes_or_no("Are you a past IOCCC winning author? [yn]");
	}
	dbg(DBG_LOW, "Submission: author[%d] Past winner: %s", i, (author_set[i].past_winning_author ? "true" : "false"));

	/*
	 * ask for IOCCC author handle
	 */
	do {

	    /*
	     * past IOCCC winning author extra prompt
	     */
	    if (author_set[i].past_winning_author == true) {

		/*
		 * explain to the past IOCCC winning author
		 */
		if (need_hints) {
		    para("",
		         "As a self-declared past IOCCC winning author, we recommend that you enter your",
			 "IOCCC winner handle instead of just pressing return, unless of course, the",
			 "below mentioned default IOCCC author handle happens to be your IOCCC winner handle. :-)",
			 "",
			 "If you do not know your IOCCC winner handle, then read the following FAQ entry:",
			 "",
			 NULL);
		    print("    %s\n", IOCCC_FIND_AUTHOR_HANDLE);
		    para("",
			 "By entering your IOCCC winner handle, you will help us match up this submission",
			 "on the website should you happen to win (again) with this submission.",
			 NULL);
		}
	    }

	    /*
	     * determine the default author handle from the name so we can use it in a prompt
	     */
	    def_handle = default_handle(author_set[i].name);
	    if (def_handle == NULL) {
		err(170, __func__, "default_handle() returned NULL!");
		not_reached();
	    }
	    dbg(DBG_VHIGH, "default IOCCC author handle: <%s>", def_handle);
	    if (need_hints) {
		errno = 0;		/* pre-clear errno for warnp() */
		ret = printf("\nThe default IOCCC author handle for author #%d is:\n\n    %s\n\n", i, def_handle);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing default IOCCC author handle");
		}
	    }

	    /*
	     * request IOCCC author handle
	     */
	    author_set[i].author_handle =
	        prompt("Enter author's IOCCC author handle, or press return to use the default", &len);

	    /*
	     * case: accept default IOCCC author handle
	     */
	    if (len == 0) {
		/* note accepting the default IOCCC author handle */
		dbg(DBG_VHIGH, "will use default IOCCC author handle");
		author_set[i].default_handle = true;

		/* free storage of prompt */
		if (author_set[i].author_handle != NULL) {
		    free(author_set[i].author_handle);
		    author_set[i].author_handle = NULL;
		}

		/* use default IOCCC author handle */
		author_set[i].author_handle = def_handle;

	    /*
	     * case: accept user input
	     */
	    } else {

		/* note using user supplied IOCCC author handle */
		dbg(DBG_VHIGH, "will use IOCCC author handle entered by user");
		author_set[i].default_handle = false;

		/* free storage of unused default IOCCC author handle */
		if (def_handle != NULL) {
		    free(def_handle);
		    def_handle = NULL;
		}
	    }
	    dbg(DBG_MED, "read: author[%d] handle: %s", i, author_set[i].author_handle);

	    /*
	     * reject if handle is invalid
	     */
	    if (test_author_handle(author_set[i].author_handle) == false) {

		/*
		 * issue rejection message
		 */
		fpara(stderr,
		      "",
		      "The IOCCC author handle must match the following regular expression:",
		      "",
		      "     ^[0-9A-Za-z][0-9A-Za-z._+-]*$",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nThe IOCCC author handle is limited to %d characters\n\n", MAX_HANDLE);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing IOCCC author handle length limit");
		}
		if (abort_on_warning) {
		    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
		}

		/*
		 * free storage
		 */
		if (author_set[i].author_handle != NULL) {
		    free(author_set[i].author_handle);
		    author_set[i].author_handle = NULL;
		}
		continue;
	    }

	    /*
	     * reject if author handle is a duplicate of a previous author handle
	     */
	    if (i > 0) {
		for (j=0; j < i; ++j) {
		    if (strcmp(author_set[i].author_handle, author_set[j].author_handle) == 0) {

			/*
			 * issue rejection message
			 */
			errno = 0;		/* pre-clear errno for warnp() */
			ret = fprintf(stderr, "\nauthor #%d author_handle duplicates previous author #%d author_handle", i, j);
			if (ret <= 0) {
			    warnp(__func__, "fprintf error while reject duplicate author_handle");
			}
			if (abort_on_warning) {
			    err(1, __func__, "-E forcing exit on 1st warning"); /*ooo*/
			}

			/*
			 * free storage
			 */
			if (author_set[i].author_handle != NULL) {
			    free(author_set[i].author_handle);
			    author_set[i].author_handle = NULL;
			}
			break;
		    }
		}
	    }
	} while (author_set[i].author_handle == NULL);
	dbg(DBG_LOW, "Submission: author[%d] IOCCC author handle: %s", i, author_set[i].author_handle);

	/*
	 * verify the information for this author
	 */
	errno = 0;	/* pre-clear errno for errp() */
	if (printf("\nPlease verify the information about author #%d\n\n", i) <= 0 ||
	    printf("Name: %s\n", author_set[i].name) <= 0 ||
	    printf("Location/country code: %s %s (%s)\n",
		   author_set[i].location_code, author_set[i].location_name, author_set[i].common_name) <= 0 ||
	    ((author_set[i].email[0] == '\0') ? printf("Email not given\n") :
						printf("Email: %s\n", author_set[i].email)) <= 0 ||
	    ((author_set[i].url[0] == '\0') ? printf("URL not given\n") :
					      printf("URL: %s\n", author_set[i].url)) <= 0 ||
	    ((author_set[i].alt_url[0] == '\0') ? printf("Alt URL not given\n") :
					      printf("Alt URL: %s\n", author_set[i].alt_url)) <= 0 ||
	    ((author_set[i].mastodon[0] == '\0') ? printf("Mastodon handle not given\n") :
						  printf("Mastodon handle: %s\n", author_set[i].mastodon)) <= 0 ||
	    ((author_set[i].github[0] == '\0') ? printf("GitHub username not given\n") :
						 printf("GitHub username: %s\n", author_set[i].github)) <= 0 ||
	    ((author_set[i].affiliation[0] == '\0') ? printf("Affiliation not given\n") :
						      printf("Affiliation: %s\n", author_set[i].affiliation)) <= 0 ||
	    ((author_set[i].past_winning_author == true) ? printf("Author claims to be a past IOCCC winning author\n") :
						   printf("Author claims to not be a past IOCCC winning author\n")) <= 0 ||
	    ((author_set[i].default_handle == true) ? printf("Default IOCCC author handle was accepted\n") :
						      printf("IOCCC author handle was manually entered\n"))  <= 0 ||
	    ((author_set[i].author_handle[0] == '\0') ? printf("IOCCC author handle\n\n") :
						        printf("IOCCC author handle: %s\n\n", author_set[i].author_handle)) <= 0) {
	    errp(171, __func__, "error while printing author #%d information\n", i);
	    not_reached();
	}
	if (need_confirm) {
	    yorn = yes_or_no("Is that author information correct? [yn]");
	    if (!yorn) {
		/*
		 * re-enter author information
		 */
		--i;
		continue;
	    }
	}
    }

    /*
     * store author set
     */
    *author_set_p = author_set;

    /*
     * return the author count
     */
    return author_count;
}


/*
 * verify_submission_dir - ask user to verify the contents of the submission directory
 *
 * given:
 *      submission_dir      - path to submission directory
 *      ls                  - path to ls utility
 *
 * This function does not return on error.
 */
static void
verify_submission_dir(char const *submission_dir, char const *ls)
{
    int exit_code;		/* exit code from shell_cmd() */
    bool yorn = false;		/* response to a question */
    FILE *ls_stream;		/* pipe from iocccsize -V */
    char *linep = NULL;		/* allocated line read from iocccsize */
    ssize_t readline_len;	/* readline return length */
    int kdirsize = 0;		/* number of kibibyte (2^10) blocks in submission directory */
    int size = 0;               /* incremental kibibyte per directory */
    char guard;			/* scanf guard to catch excess amount of input */
    int ret;			/* libc function return */
    size_t i = 0;               /* number of lines read by ls */

    /*
     * firewall
     */
    if (submission_dir == NULL || ls == NULL) {
	err(172, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * list the contents of the submission_dir
     */
    para("The following is a listing of the submission directory:",
	 "",
	 NULL);
    errno = 0;		/* pre-clear errno for warnp() */
    ret = printf("    %s\n", submission_dir);
    if (ret <= 0) {
	warnp(__func__, "printf error code: %d", ret);
    }
    para("",
	 "from which the xz tarball will be formed:",
	 "",
	 NULL);
    dbg(DBG_HIGH, "about to perform: cd -- %s && %s -lakR .", submission_dir, ls);
    exit_code = shell_cmd(__func__, false, true, "cd -- % && % -lakR .", submission_dir, ls);
    if (exit_code != 0) {
	err(173, __func__, "cd -- %s && %s -lakR . failed with exit code: %d",
			   submission_dir, ls, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * open pipe to the ls command
     */
    dbg(DBG_HIGH, "about to popen: cd -- %s && %s -lakR .", submission_dir, ls);
    ls_stream = pipe_open(__func__, false, true, "cd -- % && % -lakR .", submission_dir, ls);
    if (ls_stream == NULL) {
	err(174, __func__, "popen filed for: cd -- %s && %s -lakR .", submission_dir, ls);
	not_reached();
    }

    /*
     * read successive lines, looking for total kibibytes (2^10) block lines
     */
    kdirsize = 0;
    i = 0;
    while ((readline_len = readline(&linep, ls_stream)) >= 0) {
	dbg(DBG_HIGH, "ls line read length: %jd buffer: %s", (intmax_t)readline_len, linep);
        /*
         * parse k-block lines from ls
         */
        ret = sscanf(linep, "total %d%c", &size, &guard);
        if (ret == 1) {
            ++i;
            kdirsize += size;
        }
        /*
         * free storage
         */
        if (linep != NULL) {
            free(linep);
            linep = NULL;
        }
    }

    /*
     * no line was read at all
     */
    if (readline_len < 0 && i == 0) {
	err(175, __func__, "EOF while reading output of ls: %s", ls);
	not_reached();
    }
    /*
     * lines were read from ls but nothing correct was found
     */
    if (i == 0) {
        err(176, __func__, "found no k-block line in ls output");
        not_reached();
    }
    if (kdirsize <= 0) {
	err(177, __func__, "ls k-block value: %d <= 0", kdirsize);
	not_reached();
    }
    dbg(DBG_MED, "Directory %s size in kibibyte (1024 byte blocks): %d", submission_dir, kdirsize);

    /*
     * close down pipe
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret = pclose(ls_stream);
    if (ret < 0) {
	warnp(__func__, "pclose error on ls stream");
    }
    ls_stream = NULL;

    /*
     * if either -d or -s seed are used, temporarily turn on prompting
     */
    if (seed_used) {
	silence_prompt = false;

    /*
     * however, if -i input_recorded_answers is used, force prompting
     */
    } else if (read_answers_flag_used) {
	silence_prompt = false;
    }

    /*
     * if -i input_recorded_answers
     */
    if (!silence_prompt) {

	/*
	 * ask to verify submission file list
	 */
	yorn = yes_or_no("\nIs the above list a correct list of files in your submission? [yn]");
	if (!yorn) {
	    fpara(stderr,
		  "",
		  "We suggest you remove the existing submission directory, and then",
		  "rerun this tool with the correct set of file arguments.",
		  NULL);
	    err(178, __func__, "user rejected listing of submission_dir: %s", submission_dir);
	    not_reached();
	}
    }

    /*
     * if either -d or -s seed are used, silence prompting again
     */
    if (seed_used) {
	silence_prompt = true;

    /*
     * and of input_recorded_answers was used, silence prompting and turn back on -y
     */
    } else if (read_answers_flag_used) {
	silence_prompt = true;
    }

    /*
     * free storage
     */
    if (linep != NULL) {
	free(linep);
	linep = NULL;
    }
    return;
}


/*
 * write_info - create the .info.json file
 *
 * Form a simple JSON .info file describing the submission.
 *
 * given:
 *      infop           - pointer to info structure
 *      submission_dir  - path to submission directory
 *      chkentry	- path to chkentry tool
 *      fnamchk		- path to fnamchk tool
 *
 * returns:
 *	true
 *
 * This function does not return on error.
 */
static void
write_info(struct info *infop, char const *submission_dir, char const *chkentry, char const *fnamchk)
{
    struct tm *timeptr;		/* localtime return */
    char *info_path;		/* path to .info.json file */
    size_t info_path_len;	/* length of path to .info.json */
    FILE *info_stream;		/* open write stream to the .info.json file */
    size_t strftime_ret;	/* length of strftime() string without the trailing newline */
    size_t utctime_len;		/* length of utctime string (utctime() + " UTC") */
    int ret;			/* libc function return */
    char **q;			/* extra filename array pointer */
    int exit_code;		/* exit code from shell_cmd() */
    int i;
    int fd = -1;

    /*
     * firewall
     */
    if (infop == NULL || submission_dir == NULL || chkentry == NULL || fnamchk == NULL) {
	err(179, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (infop->extra_count < 0) {
	warn(__func__, "extra_count %d < 0", infop->extra_count);
    }


    /*
     * fill out time information in the info structure
     */

    /*
     * timestamp epoch
     */
    infop->epoch = TIMESTAMP_EPOCH;
    dbg(DBG_VVHIGH, "infop->epoch: %s", infop->epoch);

    /*
     * reset to UTC time zone
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = setenv("TZ", "UTC", 1);
    if (ret < 0) {
	errp(180, __func__, "cannot set TZ=UTC");
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    timeptr = gmtime(&(infop->tstamp));
    if (timeptr == NULL) {
	errp(181, __func__, "gmtime returned NULL");
	not_reached();
    }

    /*
     * allocate ASCII UTC string
     */
    utctime_len = MAX_TIMESTAMP_LEN + 1;    /* + 1 for trailing NUL byte */
    errno = 0;			/* pre-clear errno for errp() */
    infop->utctime = (char *)calloc(utctime_len + 1, sizeof(char)); /* + 1 for paranoia padding */
    if (infop->utctime == NULL) {
	errp(182, __func__, "calloc of %ju bytes failed", (uintmax_t)utctime_len + 1);
	not_reached();
    }

    /*
     * fill out ASCII UTC string for time now
     *
     * The ASCII UTC string is of the form:
     *
     * example:	    Thu Jan 01 00:00:00 1970 UTC
     * format:	    %a  %b  %d %H %M %S %Y
     */
    errno = 0;			/* pre-clear errno for errp() */
    strftime_ret = strftime(infop->utctime, utctime_len, "%a %b %d %H:%M:%S %Y UTC", timeptr);
    if (strftime_ret == 0) {
	errp(183, __func__, "strftime returned 0");
	not_reached();
    }
    dbg(DBG_VHIGH, "infop->utctime: %s", infop->utctime);

    /*
     * open .info.json for writing
     */
    info_path_len = strlen(submission_dir) + 1 + LITLEN(INFO_JSON_FILENAME) + 1;
    errno = 0;			/* pre-clear errno for errp() */
    info_path = (char *)malloc(info_path_len + 1);
    if (info_path == NULL) {
	errp(184, __func__, "malloc of %ju bytes failed", (uintmax_t)info_path_len + 1);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(info_path, info_path_len, "%s/%s", submission_dir, INFO_JSON_FILENAME);
    if (ret <= 0) {
	errp(185, __func__, "snprintf #0 error: %d", ret);
	not_reached();
    }
    dbg(DBG_HIGH, ".info.json path: %s", info_path);
    errno = 0;			/* pre-clear errno for errp() */
    info_stream = fopen(info_path, "w");
    if (info_stream == NULL) {
	errp(186, __func__, "failed to open for writing: %s", info_path);
	not_reached();
    }

    /*
     * obtain file descriptor for fchmod()
     */
    errno = 0; /* pre-clear errno for errp() */
    fd = open(info_path, O_WRONLY|O_CLOEXEC, S_IRWXU);
    if (fd < 0) {
        errp(187, __func__, "failed to obtain file descriptor for: %s", info_path);
        not_reached();
    }

    /*
     * write leading part of info to the open .info.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(info_stream, "{\n") > 0 &&
	json_fprintf_value_string(info_stream, "    ", JSON_PARSING_DIRECTIVE_NAME, " : ", JSON_PARSING_DIRECTIVE_VALUE, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "IOCCC_info_version", " : ", INFO_VERSION, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "IOCCC_contest", " : ", IOCCC_CONTEST, ",\n") &&
	json_fprintf_value_long(info_stream, "    ", "IOCCC_year", " : ", (long)IOCCC_YEAR, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "mkiocccentry_version", " : ", MKIOCCCENTRY_VERSION, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "iocccsize_version", " : ", infop->iocccsize_ver, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "chkentry_version", " : ", CHKENTRY_VERSION, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "fnamchk_version", " : ", FNAMCHK_VERSION, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "txzchk_version", " : ", TXZCHK_VERSION, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "IOCCC_contest_id", " : ", infop->ioccc_id, ",\n") &&
	json_fprintf_value_long(info_stream, "    ", "submit_slot", " : ", (long)infop->submit_slot, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "title", " : ", infop->title, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "abstract", " : ", infop->abstract, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "tarball", " : ", infop->tarball, ",\n") &&
	json_fprintf_value_long(info_stream, "    ", "rule_2a_size", " : ", (long)infop->rule_2a_size, ",\n") &&
	json_fprintf_value_long(info_stream, "    ", "rule_2b_size", " : ", (long)infop->rule_2b_size, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "empty_override", " : ", infop->empty_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "rule_2a_override", " : ", infop->rule_2a_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "rule_2a_mismatch", " : ", infop->rule_2a_mismatch, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "rule_2b_override", " : ", infop->rule_2b_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "highbit_warning", " : ", infop->highbit_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "nul_warning", " : ", infop->nul_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "trigraph_warning", " : ", infop->trigraph_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "wordbuf_warning", " : ", infop->wordbuf_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "ungetc_warning", " : ", infop->ungetc_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "Makefile_override", " : ", infop->Makefile_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "first_rule_is_all", " : ", infop->first_rule_is_all, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "found_all_rule", " : ", infop->found_all_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "found_clean_rule", " : ", infop->found_clean_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "found_clobber_rule", " : ", infop->found_clobber_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "found_try_rule", " : ", infop->found_try_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "    ", "test_mode", " : ", infop->test_mode, ",\n") &&
	fprintf(info_stream, "    \"manifest\" : [\n") > 0;
    if (!ret) {
	errp(188, __func__, "fprintf error writing leading part of info to %s", info_path);
	not_reached();
    }

    /*
     * write mandatory files to the open .info.json file
     */
    ret =                   fprintf(info_stream, "        {\n") > 0 &&
	  json_fprintf_value_string(info_stream, "            ", "info_JSON", " : ", INFO_JSON_FILENAME, "\n") &&
			    fprintf(info_stream, "        },\n") > 0 &&
			    fprintf(info_stream, "        {\n") > 0 &&
	  json_fprintf_value_string(info_stream, "            ", "auth_JSON", " : ", AUTH_JSON_FILENAME, "\n") &&
			    fprintf(info_stream, "        },\n") > 0 &&
			    fprintf(info_stream, "        {\n") > 0 &&
	  json_fprintf_value_string(info_stream, "            ", "c_src", " : ", infop->prog_c, "\n") &&
			    fprintf(info_stream, "        },\n") > 0 &&
			    fprintf(info_stream, "        {\n") > 0 &&
	  json_fprintf_value_string(info_stream, "            ", "Makefile", " : ", infop->Makefile, "\n") &&
			    fprintf(info_stream, "        },\n") > 0 &&
			    fprintf(info_stream, "        {\n") > 0 &&
	  json_fprintf_value_string(info_stream, "            ", "remarks", " : ", infop->remarks_md, "\n") &&
			    fprintf(info_stream, "        }%s\n", (infop->extra_count > 0) ?  "," : "") > 0;
    if (!ret) {
	errp(189, __func__, "fprintf error writing mandatory filename to %s", info_path);
	not_reached();
    }

    /*
     * write extra files to the open .info.json file
     */
    for (i=0, q=infop->extra_file; i < infop->extra_count && *q != NULL; ++i, ++q) {
	ret =                   fprintf(info_stream, "        {\n") > 0 &&
              json_fprintf_value_string(info_stream, "            ", "extra_file", " : ", *q, "\n") &&
			        fprintf(info_stream, "        }%s\n", ((i+1) < infop->extra_count) ?  "," : "") > 0;
	if (!ret) {
	    errp(190, __func__, "fprintf error writing extra filename[%d] to %s", i, info_path);
	    not_reached();
	}
    }

    /*
     * write trailing part of info to the open .info.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(info_stream, "    ],\n") > 0 &&
	json_fprintf_value_time_t(info_stream, "    ", "formed_timestamp", " : ", infop->tstamp, ",\n") &&
	json_fprintf_value_long(info_stream, "    ", "formed_timestamp_usec", " : ", (long)infop->usec, ",\n") &&
	json_fprintf_value_string(info_stream, "    ", "timestamp_epoch", " : ", TIMESTAMP_EPOCH, ",\n") &&
	json_fprintf_value_long(info_stream, "    ", "min_timestamp", " : ", MIN_TIMESTAMP, "\n") &&
	fprintf(info_stream, "}\n") > 0;
    if (!ret) {
	errp(191, __func__, "fprintf error writing trailing part of info to %s", info_path);
	not_reached();
    }

    /*
     * close the file prior to running chkentry
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(info_stream);
    if (ret < 0) {
	errp(192, __func__, "fclose error");
	not_reached();
    }


    /*
     * verify .info.json
     */
    if (!quiet) {
	para("",
	    "Checking the format of .info.json ...", NULL);
    }
    dbg(DBG_HIGH, "about to perform: %s -q -- . %s", chkentry, info_path);
    exit_code = shell_cmd(__func__, false, true, "% -q -- . %", chkentry, info_path);
    if (exit_code != 0) {
	err(193, __func__, "%s -q -- . %s failed with exit code: %d",
			   chkentry, info_path, WEXITSTATUS(exit_code));
	not_reached();
    }
    if (!quiet) {
	para("... all appears well with the .info.json file.", NULL);
    }

    /*
     * set read only for user, group and others
     */
    errno = 0;      /* pre-clear errno for errp() */
    ret = fchmod(fd, S_IRUSR | S_IRGRP | S_IROTH);
    if (ret != 0) {
        err(194, __func__, "chmod(2) failed to set user, group and other read-only on %s", info_path);
        not_reached();
    }



    /*
     * free storage
     */
    if (info_path != NULL) {
	free(info_path);
	info_path = NULL;
    }
    return;
}


/*
 * form_auth - fill out struct auth using common data from struct info plus author structure array
 *
 * given:
 *      authp           - pointer to auth structure to fill out
 *	infop		- pointer to info structure
 *      author_count    - length of the author structure array in elements
 *      authorp         - pointer to author structure array
 */
static void
form_auth(struct auth *authp, struct info *infop, int author_count, struct author *authorp)
{
    /*
     * firewall
     */
    if (authp == NULL || infop == NULL || authorp == NULL) {
	err(195, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (infop->ioccc_id == NULL) {
	err(196, __func__, "infop->ioccc_id is NULL");
	not_reached();
    }
    if (infop->tarball == NULL) {
	err(197, __func__, "infop->tarball is NULL");
	not_reached();
    }
    if (infop->utctime == NULL) {
	err(198, __func__, "infop->utctime is NULL");
	not_reached();
    }
    memset(authp, 0, sizeof(*authp));

    /*
     * copy over information before the authors array
     */
    /* copy over file format strings as compiled in compiled in constants */
    authp->no_comment = infop->no_comment;
    authp->auth_version = AUTH_VERSION;
    authp->IOCCC_contest = infop->IOCCC_contest;
    /* contest year */
    authp->year = infop->year;
    /* copy over IOCCC tool versions as compiled in compiled in constants */
    authp->mkiocccentry_ver = infop->mkiocccentry_ver;
    authp->chkentry_ver = infop->chkentry_ver;
    authp->fnamchk_ver = infop->fnamchk_ver;
    authp->iocccsize_ver = infop->iocccsize_ver;
    authp->txzchk_ver = infop->txzchk_ver;
    /* copy over submission information */
    errno = 0;			/* pre-clear errno for errp() */
    authp->ioccc_id = strdup(infop->ioccc_id);
    if (authp->ioccc_id == NULL) {
	errp(199, __func__, "strdup() ioccc_id path %s failed", infop->ioccc_id);
	not_reached();
    }
    authp->submit_slot = infop->submit_slot;
    errno = 0;			/* pre-clear errno for errp() */
    authp->tarball = strdup(infop->tarball);
    if (authp->tarball == NULL) {
	errp(200, __func__, "strdup() tarball path %s failed", infop->tarball);
	not_reached();
    }
    /* copy over test or non-test mode */
    authp->test_mode = infop->test_mode;

    /*
     * setup author set
     */
    authp->author_count = author_count;
    authp->author = authorp;

    /*
     * copy over information after the authors array
     */
    authp->tstamp = infop->tstamp;
    authp->usec = infop->usec;
    authp->epoch = infop->epoch;
    authp->min_stamp = infop->min_stamp;
    errno = 0;			/* pre-clear errno for errp() */
    authp->utctime = strdup(infop->utctime);
    if (authp->utctime == NULL) {
	errp(201, __func__, "strdup() utctime path %s failed", infop->utctime);
	not_reached();
    }
    return;
}


/*
 * write_auth - create the .auth.json file
 *
 * Form a simple JSON .author file describing the submission.
 *
 * given:
 *      authp           - pointer to auth structure
 *      submission_dir  - path to submission directory
 *      chkentry	- path to chkentry tool
 *      fnamchk		- path to fnamchk tool
 *
 * This function does not return on error.
 */
static void
write_auth(struct auth *authp, char const *submission_dir, char const *chkentry, char const *fnamchk)
{
    char *auth_path;		/* path to .auth.json file */
    size_t auth_path_len;	/* length of path to .auth.json */
    FILE *auth_stream;	/* open write stream to the .auth.json file */
    int ret;			/* libc function return */
    int exit_code;		/* exit code from shell_cmd() */
    int i;
    int fd = -1;

    /*
     * firewall
     */
    if (authp == NULL || submission_dir == NULL || chkentry == NULL || fnamchk == NULL) {
	err(202, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (authp->author_count <= 0) {
	err(203, __func__, "author_count %d <= 0", authp->author_count);
	not_reached();
    } else if (authp->author_count > MAX_AUTHORS) {
	err(204, __func__, "author count %d > max authors %d", authp->author_count, MAX_AUTHORS);
	not_reached();
    }


    /*
     * open .auth.json for writing
     */
    auth_path_len = strlen(submission_dir) + 1 + LITLEN(AUTH_JSON_FILENAME) + 1;
    errno = 0;			/* pre-clear errno for errp() */
    auth_path = (char *)malloc(auth_path_len + 1);
    if (auth_path == NULL) {
	errp(205, __func__, "malloc of %ju bytes failed", (uintmax_t)auth_path_len + 1);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(auth_path, auth_path_len, "%s/%s", submission_dir, AUTH_JSON_FILENAME);
    if (ret <= 0) {
	errp(206, __func__, "snprintf #0 error: %d", ret);
	not_reached();
    }
    dbg(DBG_HIGH, ".auth.json path: %s", auth_path);
    errno = 0;			/* pre-clear errno for errp() */
    auth_stream = fopen(auth_path, "w");
    if (auth_stream == NULL) {
	errp(207, __func__, "failed to open for writing: %s", auth_path);
	not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    fd = open(auth_path, O_WRONLY|O_CLOEXEC, S_IRWXU);
    if (fd < 0) {
        err(208, __func__, "failed to obtain file descriptor for: %s", auth_path);
        not_reached();
    }

    /*
     * write leading part of authorship to the open .auth.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(auth_stream, "{\n") > 0 &&
	json_fprintf_value_string(auth_stream, "    ", JSON_PARSING_DIRECTIVE_NAME, " : ", JSON_PARSING_DIRECTIVE_VALUE, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "IOCCC_auth_version", " : ", AUTH_VERSION, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "IOCCC_contest", " : ", IOCCC_CONTEST, ",\n") &&
	json_fprintf_value_long(auth_stream, "    ", "IOCCC_year", " : ", (long)IOCCC_YEAR, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "mkiocccentry_version", " : ", MKIOCCCENTRY_VERSION, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "chkentry_version", " : ", CHKENTRY_VERSION, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "fnamchk_version", " : ", FNAMCHK_VERSION, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "IOCCC_contest_id", " : ", authp->ioccc_id, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "tarball", " : ", authp->tarball, ",\n") &&
	json_fprintf_value_long(auth_stream, "    ", "submit_slot", " : ", (long)authp->submit_slot, ",\n") &&
	json_fprintf_value_long(auth_stream, "    ", "author_count", " : ", (long)authp->author_count, ",\n") &&
	json_fprintf_value_bool(auth_stream, "    ", "test_mode", " : ", authp->test_mode, ",\n") &&
	fprintf(auth_stream, "    \"authors\" : [\n") > 0;
    if (!ret) {
	errp(209, __func__, "fprintf error writing leading part of authorship to %s", auth_path);
	not_reached();
    }

    /*
     * write author info to the open .auth.json file
     *
     * NOTE: We do not add location_name to the .auth.json file as location names can change over time.
     */
    for (i = 0; i < authp->author_count; ++i) {
	struct author *ap = &(authp->author[i]);
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(auth_stream, "        {\n") > 0 &&
	    json_fprintf_value_string(auth_stream, "            ", "name", " : ", ap->name, ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "location_code", " : ", ap->location_code, ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "email", " : ", strnull(ap->email), ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "url", " : ", strnull(ap->url), ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "alt_url", " : ", strnull(ap->alt_url), ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "mastodon", " : ", strnull(ap->mastodon), ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "github", " : ", strnull(ap->github), ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "affiliation", " : ", strnull(ap->affiliation), ",\n") &&
	    json_fprintf_value_bool(auth_stream, "            ", "past_winning_author", " : ", ap->past_winning_author, ",\n") &&
	    json_fprintf_value_bool(auth_stream, "            ", "default_handle", " : ", ap->default_handle, ",\n") &&
	    json_fprintf_value_string(auth_stream, "            ", "author_handle", " : ", strnull(ap->author_handle), ",\n") &&
	    json_fprintf_value_long(auth_stream, "            ", "author_number", " : ", ap->author_num, "\n") &&
	    fprintf(auth_stream, "        }%s\n", (((i + 1) < authp->author_count) ? "," : "")) > 0;
	if (ret == false) {
	    errp(210, __func__, "fprintf error writing author %d info to %s", i, auth_path);
	    not_reached();
	}
    }

    /*
     * write trailing part of authorship to the open .auth.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(auth_stream, "    ],\n") > 0 &&
	json_fprintf_value_time_t(auth_stream, "    ", "formed_timestamp", " : ", authp->tstamp, ",\n") &&
	json_fprintf_value_long(auth_stream, "    ", "formed_timestamp_usec", " : ", (long)authp->usec, ",\n") &&
	json_fprintf_value_string(auth_stream, "    ", "timestamp_epoch", " : ", authp->epoch, ",\n") &&
	json_fprintf_value_long(auth_stream, "    ", "min_timestamp", " : ", MIN_TIMESTAMP, "\n") &&
	fprintf(auth_stream, "}\n") > 0;
    if (!ret) {
	errp(211, __func__, "fprintf error writing trailing part of authorship to %s", auth_path);
	not_reached();
    }

    /*
     * close the file before checking it with chkentry
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(auth_stream);
    if (ret < 0) {
	errp(212, __func__, "fclose error");
	not_reached();
    }



    /*
     * verify .auth.json
     */
    if (!quiet) {
	para("",
	    "Checking the format of .auth.json ...", NULL);
    }
    dbg(DBG_HIGH, "about to perform: %s -q -- %s .", chkentry, auth_path);
    exit_code = shell_cmd(__func__, false, true, "% -q -- % .", chkentry, auth_path);
    if (exit_code != 0) {
	err(213, __func__, "%s -q -- %s . failed with exit code: %d",
			   chkentry, auth_path, WEXITSTATUS(exit_code));
	not_reached();
    }
    if (!quiet) {
	para("... all appears well with the .auth.json file.", NULL);
    }

    /*
     * set read only for user, group and others
     */
    errno = 0;      /* pre-clear errno for errp() */
    ret = fchmod(fd, S_IRUSR | S_IRGRP | S_IROTH);
    if (ret != 0) {
        err(214, __func__, "chmod(2) failed to set user, group and other read-only on %s", auth_path);
        not_reached();
    }

    /*
     * close descriptor
     */
    errno = 0; /* pre-clear for errp() */
    ret = close(fd);
    if (ret < 0) {
        errp(215, __func__, "close(fd) failed");
        not_reached();
    }


    /*
     * free storage
     */
    if (auth_path != NULL) {
	free(auth_path);
	auth_path = NULL;
    }
    return;
}


/*
 * form_tarball - form the compressed tarball
 *
 * Given the completed submission directory, form a compressed tarball for the user to submit.
 * Remind the user where to submit their compressed tarball file. The function
 * shows the listing of the tarball contents via the txzchk tool and the fnamchk tool.
 *
 * given:
 *      workdir        - working directory under which the submission directory is formed
 *      submission_dir  - path to submission directory
 *      tarball_path    - path of the compressed tarball to form
 *      tar             - path to the tar utility
 *      ls              - path to ls utility
 *      txzchk		- path to txzchk tool
 *      fnamchk		- path to fnamchk tool
 *
 * This function does not return on error.
 */
static void
form_tarball(char const *workdir, char const *submission_dir, char const *tarball_path, char const *tar,
	     char const *ls, char const *txzchk, char const *fnamchk)
{
    char *basename_submission_dir;	/* basename of the submission directory */
    char *basename_tarball_path;/* basename of tarball_path */
    int exit_code;		/* exit code from shell_cmd() */
    struct stat buf;		/* stat of the tarball */
    int cwd = -1;		/* current working directory */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (workdir == NULL || submission_dir == NULL || tarball_path == NULL || tar == NULL || ls == NULL ||
        txzchk == NULL || fnamchk == NULL) {
	err(216, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * verify submission directory contents
     */
    verify_submission_dir(submission_dir, ls);
    dbg(DBG_MED, "verified submission directory: %s", submission_dir);

    /*
     * note the current directory so we can restore it later, after the chdir(workdir) below
     */
    errno = 0;			/* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
	errp(217, __func__, "cannot open .");
	not_reached();
    }

    /*
     * cd into the workdir, just above the submission_dir and where the compressed tarball will be formed
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = chdir(workdir);
    if (ret < 0) {
	errp(218, __func__, "cannot cd %s", workdir);
	not_reached();
    }

    /*
     * perform the tar create command
     *
     * IMPORTANT NOTE: The reason why we form a v7 tarball is that we do NOT want to
     *		       preserve user and group names (to help keep authors anonymous),
     *		       (modern flags to force a username/groupname are not very portable),
     *		       and we don't want special files, symlinks, etc.
     */
    if (!quiet) {
	para("",
	     "About to run the tar command to form the compressed tarball ...",
	     "",
	     NULL);
    } else {
	para("", NULL);
    }

    basename_submission_dir = base_name(submission_dir);
    basename_tarball_path = base_name(tarball_path);
    dbg(DBG_HIGH, "about to perform: %s --format=v7 -cJf %s -- %s",
		   tar, basename_tarball_path, basename_submission_dir);
    exit_code = shell_cmd(__func__, false, true, "% --format=v7 -cJf % -- %",
				    tar, basename_tarball_path, basename_submission_dir);
    if (exit_code != 0) {
	err(219, __func__, "%s --format=v7 -cJf %s -- %s failed with exit code: %d",
			   tar, basename_tarball_path, basename_submission_dir, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * enforce the maximum size of the compressed tarball
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = stat(basename_tarball_path, &buf);
    if (ret != 0) {
	errp(220, __func__, "stat of the compressed tarball failed: %s", basename_tarball_path);
	not_reached();
    }
    if (buf.st_size > MAX_TARBALL_LEN) {
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(221, __func__, "The compressed tarball: %s size: %ju > %jd",
		 basename_tarball_path, (uintmax_t)buf.st_size, (intmax_t)MAX_TARBALL_LEN);
	not_reached();
    }

    /*
     * switch back to the previous current directory
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fchdir(cwd);
    if (ret < 0) {
	errp(222, __func__, "cannot fchdir to the previous current directory");
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = close(cwd);
    if (ret < 0) {
	errp(223, __func__, "close of previous current directory failed");
	not_reached();
    }

    /*
     * perform the txzchk which will indirectly show the user the tarball contents
     */
    if (entertain) {
        dbg(DBG_HIGH, "about to perform: %s -e -w -v 1 -F %s -- %s/../%s",
                      txzchk, fnamchk, submission_dir, basename_tarball_path);
        exit_code = shell_cmd(__func__, false, true, "% -e -w -v 1 -F % -- %/../%",
                                              txzchk, fnamchk, submission_dir, basename_tarball_path);
        if (exit_code != 0) {
            err(224, __func__, "%s -e -w -v 1 -F %s -- %s/../%s failed with exit code: %d",
                               txzchk, fnamchk, submission_dir, basename_tarball_path, WEXITSTATUS(exit_code));
            not_reached();
        }

    } else {
        dbg(DBG_HIGH, "about to perform: %s -w -v 1 -F %s -- %s/../%s",
                      txzchk, fnamchk, submission_dir, basename_tarball_path);
        exit_code = shell_cmd(__func__, false, true, "% -w -v 1 -F % -- %/../%",
                                              txzchk, fnamchk, submission_dir, basename_tarball_path);
        if (exit_code != 0) {
            err(225, __func__, "%s -w -v 1 -F %s -- %s/../%s failed with exit code: %d",
                               txzchk, fnamchk, submission_dir, basename_tarball_path, WEXITSTATUS(exit_code));
            not_reached();
        }
    }
    para("",
	 "... the output above is the listing of the compressed tarball.",
	 "",
	 NULL);


    /*
     * free memory
     */
    if (basename_submission_dir != NULL) {
	free(basename_submission_dir);
	basename_submission_dir = NULL;
    }
    if (basename_tarball_path != NULL) {
	free(basename_tarball_path);
	basename_tarball_path = NULL;
    }
    return;
}


/*
 * remind_user - remind the user to upload (if not in test mode)
 *
 * given:
 *      workdir        - working directory under which the submission directory is formed
 *      submission_dir  - path to submission directory
 *      tar             - path to the tar utility
 *      tarball_path    - path of the compressed tarball to form
 *      test_mode       - true ==> test mode, do not upload
 *      answers		- path to the answers file (if specified)
 *      slot_number     - slot number of submission
 */
static void
remind_user(char const *workdir, char const *submission_dir, char const *tar, char const *tarball_path,
        bool test_mode, int slot_number)
{
    int ret;			/* libc function return */
    char *submission_dir_esc;
    char *workdir_esc;

    /*
     * firewall
     */
    if (workdir == NULL || submission_dir == NULL || tar == NULL || tarball_path == NULL) {
	err(226, __func__, "called with NULL arg(s)");
	not_reached();
    }

    submission_dir_esc = cmdprintf("%", submission_dir);
    if (submission_dir_esc == NULL) {
	err(227, __func__, "failed to cmdprintf: submission_dir");
	not_reached();
    }

    /*
     * tell user they can now remove submission_dir
     */
    para("Now that we have formed the compressed tarball file, you",
	 "can remove the submission directory we have formed by executing:",
	 "",
	 NULL);
    ret = printf("    rm -rf %s%s\n", submission_dir[0] == '-' ? "-- " : "", submission_dir_esc);
    if (ret <= 0) {
	errp(228, __func__, "printf #0 error");
	not_reached();
    }
    free(submission_dir_esc);

    workdir_esc = cmdprintf("%", workdir);
    if (workdir_esc == NULL) {
	err(229, __func__, "failed to cmdprintf: workdir");
	not_reached();
    }

    para("",
	 "If you are curious, you may examine the newly created compressed tarball",
	 "by running the following command:",
	 "",
	 NULL);
    ret = printf("    %s -Jtvf %s%s/%s\n", tar, workdir[0] == '-' ? "./" : "", workdir_esc, tarball_path);
    if (ret <= 0) {
	errp(230, __func__, "printf #2 error");
	not_reached();
    }
    free(workdir_esc);

    /*
     * case: test mode
     */
    if (test_mode) {

	/*
	 * remind them that this is a test submission, not an official submission
	 */
	para("",
	     "As you entered an IOCCC contest ID of 'test', the compressed tarball",
	     "that was just formed CANNOT be used as an IOCCC submission.",
	     "",
	     "Please do NOT upload the compressed tarball to the IOCCC submit server!",
	     "", "",
	     NULL);

    /*
     * case: -d or -s seed
     */
    } else if (seed_used) {

	/*
	 * remind them that this is seed test, not an official submission
	 */
	para("",
	     "As you used either -d or -s seed, the compressed tarball",
	     "that was just formed CANNOT be used as an IOCCC submission.",
	     "",
	     "Please do NOT upload the compressed tarball to the IOCCC submit server!",
	     "", "",
	     NULL);

    /*
     * case: submission mode
     */
    } else {

	/*
	 * inform them of the compressed tarball file
	 */
	show_registration_url();
	show_submit_url(workdir, tarball_path, slot_number);
    }
    return;
}


/*
 * show_registration_url - print registration URL
 */
static void
show_registration_url(void)
{
    int ret;			/* libc function return */

    /*
     * print information about how to register for the IOCCC
     */
    para("",
	 "To submit to the IOCCC, you must be a registered contestant",
         "and have received an IOCCC contest ID (a UUID) and initial",
         "password (via email). To do so, please visit:",
	 "",
	 NULL);
    errno = 0;		/* pre-clear errno for warnp() */
    ret = printf("    %s\n", IOCCC_REGISTER_URL);
    if (ret <= 0) {
	err(231, __func__, "printf error printing IOCCC_REGISTER_URL");
	not_reached();
    }
    para("",
         "Please also see the FAQ on how to enter the IOCCC:",
         "",
         NULL);
    errno = 0;		/* pre-clear errno for warnp() */
    ret = printf("    %s\n", IOCCC_REGISTER_FAQ_URL);
    if (ret <= 0) {
	err(232, __func__, "printf error printing IOCCC register FAQ URL");
	not_reached();
    }
    para("",
        "...and the register, password change and submit help pages:",
        "",
        NULL);
    errno = 0;		/* pre-clear errno for warnp() */
    ret = printf("    %s\n    %s\n    %s\n", IOCCC_REGISTER_INFO_URL, IOCCC_PW_CHANGE_INFO_URL, IOCCC_SUBMIT_INFO_URL);
    if (ret <= 0) {
	err(233, __func__, "printf error printing IOCCC_REGISTER_INFO_URL, IOCCC_PW_CHANGE_INFO_URL and IOCCC_SUBMIT_INFO_URL");
	not_reached();
    }

    para("",
        "Please note that you may ONLY register when the contest is in",
        "a PENDING or OPEN status! For the contest status see:",
        "",
        NULL);
    ret = printf("    %s\n", IOCCC_STATUS_URL);
    if (ret < 0) {
	errp(234, __func__, "printf error printing IOCCC status URL");
	not_reached();
    }

}


/*
 * show_submit_url
 *
 * Tell the user where they may submit their submission if the contest is still
 * open.
 *
 * given:
 *	    workdir	    - work directory
 *	    tarball_path    - path to the submission tarball
 *	    submit_slot     - submit slot for submission
 *
 * NOTE: if either workdir or tarball_path is NULL we will do nothing.
 */
static void
show_submit_url(char const *workdir, char const *tarball_path, int slot_number)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (workdir == NULL || tarball_path == NULL) {
	return;
    }


    ret = printf("\nWhen the contest is open (see https://www.ioccc.org/status.html),\n"
        "after you have registered, you must upload into slot %d:\n\n\t%s/%s\n", slot_number,
        workdir, tarball_path);
    if (ret <= 0) {
	errp(235, __func__, "printf error printing tarball path and slot number");
	not_reached();
    }
    para("",
        "to the IOCCC submit server:",
        "",
        NULL);

    ret = printf("    %s\n", IOCCC_SUBMIT_URL);
    if (ret < 0) {
	errp(236, __func__, "printf error printing IOCCC submit URL");
	not_reached();
    }

    para("",
        "For more information, see the FAQ on how to enter at:",
        "",
        NULL);

     ret = printf("    %s\n", IOCCC_ENTER_FAQ_URL);
    if (ret < 0) {
	errp(237, __func__, "printf error printing IOCCC enter FAQ URL");
	not_reached();
    }
}
