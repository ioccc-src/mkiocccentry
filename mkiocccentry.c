/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * mkiocccentry - make an ioccc entry
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla C.
 * Why?  Because this is a obfuscated C contest.  But then why isn't
 * this code obfuscated?  Because the IOCCC judges prefer to write
 * in robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell source and finger daemon
 * source.  Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, let the judges know.
 * To contact the judges please see:
 *
 *      https://www.ioccc.org/judges.html
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Many thanks are due to a number of people who provided important
 * and valuable testing, suggestions, issue reports and GitHub pull
 * requests to this code.  Without their time and effort, this tool
 * would not work very well!
 *
 * Among the GitHub users we wish to thank include these fine developers
 * in alphabetical GitHub @user order:
 *
 *	@ilyakurdyukov		Ilya Kurdyukov
 *	@SirWumpus		Anthony Howe
 *	@vog			Volker Diels-Grabsch
 *	@xexyl			Cody Boone Ferguson
 *
 * Copyright (c) 2021,2022 by Landon Curt Noll.  All Rights Reserved.
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
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h> /* for WEXITSTATUS() */
#include <fcntl.h> /* for open() */


/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions
 */
#include "util.h"

/*
 * json - json structs
 */
#include "json.h"



/*
 * Answers file constants.
 *
 * Version of answers file.
 * Use format: MKIOCCCENTRY_ANSWERS-YYYY-major.minor
 *
 * The following is NOT the version of this mkiocccentry tool!
 */
#define MKIOCCCENTRY_ANSWERS_VER "MKIOCCCENTRY_ANSWERS-2022-0.0"
/* Answers file EOF marker */
#define MKIOCCCENTRY_ANSWERS_EOF "ANSWERS_EOF"


/*
 * definitions
 */
#define REQUIRED_ARGS (4)	/* number of required arguments on the command line */
#define ISO_3166_1_CODE_URL0 "https://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements"
#define ISO_3166_1_CODE_URL1 "https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2"
#define ISO_3166_1_CODE_URL2 "https://www.iso.org/obp/ui/#iso:pub:PUB500001:en"
#define ISO_3166_1_CODE_URL3 "https://www.iso.org/obp/ui/#search"
#define ISO_3166_1_CODE_URL4 "https://www.iso.org/glossary-for-iso-3166.html"
#define TAIL_TITLE_CHARS "abcdefghijklmnopqrstuvwxyz0123456789_+-"	/* [a-z0-9_+-] */
#define IOCCC_REGISTER_URL "https://register.ioccc.org/NOT/a/real/URL"	/* XXX - change to real URL when ready */
#define IOCCC_SUBMIT_URL "https://submit.ioccc.org/NOT/a/real/URL"	/* XXX - change to real URL when ready */
#define RULE_2A_BIG_FILE_WARNING (0)	/* warn that prog.c appears to be too big under Rule 2a */
#define RULE_2A_IOCCCSIZE_MISMATCH (1)	/* warn that prog.c iocccsize size differs from the file size */


/*
 * usage message
 *
 * Use the usage() function to print the these usage_msgX strings.
 */
static const char * const usage_msg0 =
    "usage: %s [options] work_dir prog.c Makefile remarks.md [file ...]\n"
    "\noptions:\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\t-W\t\t\tignore all warnings (this does NOT mean the judges will! :) )\n";

static const char * const usage_msg1 =
    "\t-t /path/to/tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-c /path/to/cp\t\tpath to cp executable (def: %s)\n"
    "\t-l /path/to/ls\t\tpath to ls executable (def: %s)\n"
    "\t-C /path/to/txzchk\tpath to txzchk executable (def: %s)\n"
    "\t-F /path/to/fnamchk\tpath to fnamchk executable used by txzchk (def: %s)";
static const char * const usage_msg2 =
    "\t-j /path/to/jinfochk	path to jinfochk executable used by txzchk (def: %s)\n"
    "\t-J /path/to/jauthchk	path to jauthchk executable used by txzchk (def: %s)\n";
static const char * const usage_msg3 =
    "\t-a answers\t\twrite answers to a file for easier updating of an entry\n"
    "\t-A answers\t\twrite answers file even if it already exists\n"
    "\t-i answers\t\tread answers from file previously written by -a|-A answers\n\n"
    "\t    NOTE: One cannot use both -a/-A answers and -i answers at the same time.\n"
    "\n"
    "\twork_dir\tdirectory where the entry directory and tarball are formed\n"
    "\tprog.c\t\tpath to the C source for your entry\n";
static const char * const usage_msg4 =
    "\n"
    "\tMakefile\tMakefile to build (make all) and cleanup (make clean & make clobber)\n"
    "\n"
    "\tremarks.md\tRemarks about your entry in markdown format\n"
    "\t\t\tNOTE: The following is a guide to markdown:\n"
    "\n"
    "\t\t\t    https://www.markdownguide.org/basic-syntax\n"
    "\n"
    "\t[file ...]\textra data files to include with your entry\n"
    "\n"
    "mkiocccentry version: %s\n";

/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
static bool need_confirm = true;	/* true ==> ask for confirmations */
static bool need_hints = true;		/* true ==> show hints */
static bool need_retry = true;
static bool ignore_warnings = false;	/* true ==> ignore all warnings (this does NOT mean the judges will! :) */
static FILE *input_stream = NULL;
static struct iocccsize size;	/* rule_count() processing results */


/*
 * forward declarations
 */
static void usage(int exitcode, char const *str, char const *program, char const *tar, char const *cp, char const *ls,
		  char const *txzchk, char const *fnamchk, char const *jinfochk, char const *jauthchk);
static void free_info(struct info *infop);
static void free_author_array(struct author *authorp, int author_count);
static void warn_empty_prog(char const *prog_c);
static void warn_rule2a_size(struct info *infop, char const *prog_c, int mode);
static void warn_high_bit(char const *prog_c);
static void warn_nul_chars(char const *prog_c);
static void warn_trigraph(char const *prog_c);
static void warn_wordbuf(char const *prog_c);
static void warn_ungetc(char const *prog_c);
static void warn_rule2b_size(struct info *infop, char const *prog_c);
static void check_prog_c(struct info *infop, char const *entry_dir, char const *cp, char const *prog_c);
static void sanity_chk(struct info *infop, char const *work_dir, char const *tar, char const *cp,
		       char const *ls, char const *txzchk, char const *fnamchk, char const *jinfochk, char const *jauthchk);
static char *prompt(char const *str, size_t *lenp);
static char *get_contest_id(struct info *infop, bool *testp, bool *read_answers_flag_used);
static int get_entry_num(struct info *infop);
static char *mk_entry_dir(char const *work_dir, char const *ioccc_id, int entry_num, char **tarball_path, time_t tstamp);
static bool inspect_Makefile(char const *Makefile, struct info *infop);
static void warn_Makefile(char const *Makefile, struct info *infop);
static void check_Makefile(struct info *infop, char const *entry_dir, char const *cp, char const *Makefile);
static void check_remarks_md(struct info *infop, char const *entry_dir, char const *cp, char const *remarks_md);
static void check_extra_data_files(struct info *infop, char const *entry_dir, char const *cp, int count, char **args);
static char const *lookup_location_name(char const *upper_code);
static bool yes_or_no(char const *question);
static char *get_title(struct info *infop);
static char *get_abstract(struct info *infop);
static int get_author_info(struct info *infop, char *ioccc_id, struct author **author_set);
static void verify_entry_dir(char const *entry_dir, char const *ls);
static bool json_putc(int const c, FILE *stream);
static bool json_fprintf_str(FILE *stream, char const *str);
static bool json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
				      char const *tail);
static bool json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
				    char const *tail);
static bool json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
				    char const *tail);
static char const * strnull(char const * const str);
static void write_info(struct info *infop, char const *entry_dir, bool test_mode, char const *jinfochk);
static void write_author(struct info *infop, int author_count, struct author *authorp, char const *entry_dir, char const *jauthchk);
static void form_tarball(char const *work_dir, char const *entry_dir, char const *tarball_path, char const *tar,
			 char const *ls, char const *txzchk, char const *fnamchk);
static void remind_user(char const *work_dir, char const *entry_dir, char const *tar, char const *tarball_path, bool test_mode);


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    struct timeval tp;		/* gettimeofday time value */
    char const *work_dir = NULL;	/* where the entry directory and tarball are formed */
    char const *prog_c = NULL;		/* path to prog.c */
    char const *Makefile = NULL;	/* path to Makefile */
    char const *remarks_md = NULL;	/* path to remarks.md */
    char *tar = TAR_PATH_0;		/* path to tar executable that supports the -J (xz) option */
    char *cp = CP_PATH_0;		/* path to cp executable */
    char *ls = LS_PATH_0;		/* path to ls executable */
    char *txzchk = TXZCHK_PATH_0;	/* path to txzchk executable */
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    char *jauthchk = JAUTHCHK_PATH_0;	/* path to jauthchk executable */
    char *jinfochk = JINFOCHK_PATH_0;	/* path to jinfochk executable */
    char const *answers = NULL;		/* path to the answers file (recording input given on stdin) */
    FILE *answerp = NULL;		/* file pointer to the answers file */
    bool test_mode = false;		/* true ==> contest ID is test */
    char *entry_dir = NULL;		/* entry directory from which to form a compressed tarball */
    char *tarball_path = NULL;		/* path of the compressed tarball to form */
    int extra_count = 0;		/* number of extra files */
    char **extra_list = NULL;		/* list of extra files (if any) */
    struct info info;			/* data to form .info.json */
    int author_count = 0;		/* number of authors */
    struct author *author_set = NULL;	/* list of authors */
    bool tar_flag_used = false;		/* true ==> -t /path/to/tar was given */
    bool cp_flag_used = false;		/* true ==> -c /path/to/cp was given */
    bool ls_flag_used = false;		/* true ==> -l /path/to/ls was given */
    bool answers_flag_used = false;		/* true ==> -a write answers to answers file */
    bool read_answers_flag_used = false;		/* true ==> -i read answers from answers file */
    bool overwite_answers_flag_used = false;		/* true ==> don't prompt to overwrite answers if it already exists */
    bool txzchk_flag_used = false;	/* true ==> -C /path/to/txzchk was given */
    bool fnamchk_flag_used = false;		/* true ==> -F /path/to/fnamchk was given */
    bool jinfochk_flag_used = false;	/* true ==> -j /path/to/jinfochk was given */
    bool jauthchk_flag_used = false;	/* true ==> -J /path/to/jauthchk was given */
    bool overwrite_answers = true;	/* true ==> overwrite answers file even if it already exists */
    int ret;				/* libc return code */
    int i;

    /*
     * parse args
     */
    input_stream = stdin;	/* default to reading from standard in */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:Vt:c:l:a:i:A:WC:F:j:J:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(1, "-h help mode", program, TAR_PATH_0, CP_PATH_0, LS_PATH_0, TXZCHK_PATH_0, FNAMCHK_PATH_0, JINFOCHK_PATH_0, JAUTHCHK_PATH_0);
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    errno = 0;		/* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(2, __func__, "cannot parse -v arg: %s error: %s", optarg, strerror(errno));
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", MKIOCCCENTRY_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing version string: %s", MKIOCCCENTRY_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 't':		/* -t /path/to/tar */
	    tar = optarg;
	    tar_flag_used = true;
	    break;
	case 'c':		/* -c /path/to/cp */
	    cp = optarg;
	    cp_flag_used = true;
	    break;
	case 'l':		/* -l /path/to/ls */
	    ls = optarg;
	    ls_flag_used = true;
	    break;
	case 'A':		/* -A answers overwrite answers file */
	    answers = optarg;
	    overwite_answers_flag_used = true;
	    /* FALL THROUGH */
	case 'a':		/* -a record_answers */
	    answers = optarg;
	    answers_flag_used = true;
	    break;
	case 'i':		/* -i input_recorded_answers */
	    answers = optarg;
	    read_answers_flag_used = true;
	    need_confirm = false;
	    need_hints = false;
	    break;
	case 'W':		/* -W ignores all warnings (this does NOT the judges will! :) ) */
	    ignore_warnings = true;
	    break;
	case 'C':
	    txzchk_flag_used = true;
	    txzchk = optarg;
	    break;
	case 'F':
	    fnamchk_flag_used = true;
	    fnamchk = optarg;
	    break;
	case 'j':
	    jinfochk_flag_used = true;
	    jinfochk = optarg;
	    break;
	case 'J':
	    jauthchk_flag_used = true;
	    jauthchk = optarg;
	    break;
	default:
	    usage(3, "invalid -flag", program, TAR_PATH_0, CP_PATH_0, LS_PATH_0, TXZCHK_PATH_0, FNAMCHK_PATH_0,
		    JINFOCHK_PATH_0, JAUTHCHK_PATH_0); /*ooo*/
	    not_reached();
	 }
    }
    /* must have at least the required number of args */
    if (argc - optind < REQUIRED_ARGS) {
	usage(4, "wrong number of arguments", program, TAR_PATH_0, CP_PATH_0, LS_PATH_0, TXZCHK_PATH_0, FNAMCHK_PATH_0,
		JINFOCHK_PATH_0, JAUTHCHK_PATH_0); /*ooo*/
	not_reached();
    }

    /*
     * guess where tar, cp and ls utilities are located
     *
     * If the user did not give a -t, -c and/or -l /path/to/x path, then look at
     * the historic location for the utility.  If the historic location of the utility
     * isn't executable, look for an executable in the alternate location.
     *
     * On some systems where /usr/bin != /bin, the distribution made the mistake of
     * moving historic critical applications, look to see if the alternate path works instead.
     */
    find_utils(tar_flag_used, &tar, cp_flag_used, &cp, ls_flag_used, &ls, 
	    txzchk_flag_used, &txzchk, fnamchk_flag_used, &fnamchk,
	    jinfochk_flag_used, &jinfochk, jauthchk_flag_used, &jauthchk);

    /* check that conflicting answers file options are not used together */
    if (answers_flag_used && read_answers_flag_used) {
	err(1, __func__, "-a answers and -i answers cannot be used together"); /*ooo*/
	not_reached();
    }
    /* collect required the required args */
    extra_count = (argc - optind > REQUIRED_ARGS) ? argc - optind - REQUIRED_ARGS : 0;
    extra_list = argv + optind + REQUIRED_ARGS;
    dbg(DBG_LOW, "tar: %s", tar);
    dbg(DBG_LOW, "cp: %s", cp);
    dbg(DBG_LOW, "ls: %s", ls);
    work_dir = argv[optind];
    dbg(DBG_LOW, "work_dir: %s", work_dir);
    prog_c = argv[optind + 1];
    dbg(DBG_LOW, "prog.c: %s", prog_c);
    Makefile = argv[optind + 2];
    dbg(DBG_LOW, "Makefile: %s", Makefile);
    remarks_md = argv[optind + 3];
    dbg(DBG_LOW, "remarks: %s", remarks_md);
    dbg(DBG_LOW, "number of extra data file args: %d", extra_count);
    dbg(DBG_LOW, "answers file: %s", answers);

    /*
     * zerosize info
     */
    memset(&info, 0, sizeof(info));

    /*
     * record the time
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = gettimeofday(&tp, NULL);
    if (ret < 0) {
	errp(5, __func__, "gettimeofday failed");
	not_reached();
    }
    info.tstamp = tp.tv_sec;
    dbg(DBG_HIGH, "info.tstamp: %ld", (long)info.tstamp);
    info.usec = tp.tv_usec;
    dbg(DBG_HIGH, "infop->usec: %ld", (long)info.usec);

    /*
     * Welcome
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = printf("Welcome to mkiocccentry version: %s\n", MKIOCCCENTRY_VERSION);
    if (ret <= 0) {
	errp(6, __func__, "printf error printing the welcome string");
	not_reached();
    }

    /*
     * save our version
     */
    errno = 0;			/* pre-clear errno for errp() */
    info.mkiocccentry_ver = strdup(MKIOCCCENTRY_VERSION);
    if (info.mkiocccentry_ver == NULL) {
	errp(7, __func__, "cannot strdup version: %s", MKIOCCCENTRY_VERSION);
	not_reached();
    }

    /* if the user requested to ignore warnings, ignore this once and warn them :) */
    if (ignore_warnings) {
	para("",
	     "WARNING: You've chosen to ignore all warnings (except this warning! :) )!",
	     "If this was unintentional run this program again without the -W option.",
	     "Note that The Judges will NOT ignore warnings!",
	     NULL);
    }

    /*
     * environment sanity checks
     */
    para("", "Performing sanity checks on your environment ...", NULL);
    sanity_chk(&info, work_dir, tar, cp, ls, txzchk, fnamchk, jinfochk, jauthchk);
    para("... environment looks OK", "", NULL);

    /* if -a answers was specified and answers file exists, prompt user if they
     * want to overwrite it; if they don't tell them how to use it and abort.
     * Else it will be overwritten.
     */
    if (answers_flag_used && !overwite_answers_flag_used && answers != NULL && strlen(answers) > 0 && exists(answers)) {
	overwrite_answers = yes_or_no("WARNING: The answers file already exists! Do you wish to overwrite it? [yn]");
	if (!overwrite_answers) {
	    errno = 0;
	    ret = printf("\nTo use the answers file, try:\n\n\t./mkiocccentry -i %s [...]\n\n", answers);
	    if (ret <= 0) {
		errp(8, __func__, "printf error telling the user how to use the answers file");
		not_reached();
	    }
	    err(9, __func__, "won't overwrite answers file");
	    not_reached();
	}
    }

    /*
     * check if we should read input from answers file
     */
    if (read_answers_flag_used && answers != NULL && strlen(answers) > 0) {
	if (!is_read(answers)) {
	    errp(10, __func__, "cannot read answers file");
	    not_reached();
	}
	errno = 0;
	answerp = fopen(answers, "r");
	if (answerp == NULL) {
	    errp(11, __func__, "cannot open answers file");
	    not_reached();
	}
	input_stream = answerp;
    }
    /*
     * obtain the IOCCC contest ID
     */
    info.ioccc_id = get_contest_id(&info, &test_mode, &read_answers_flag_used);
    dbg(DBG_MED, "IOCCC contest ID: %s", info.ioccc_id);

    /*
     * found the answer file header in stdin
     */
    if (read_answers_flag_used && answers == NULL) {
	answerp = stdin;
    }

    /*
     * obtain entry number
     */
    info.entry_num = get_entry_num(&info);
    dbg(DBG_MED, "entry number: %d", info.entry_num);

    /*
     * create entry directory
     */
    entry_dir = mk_entry_dir(work_dir, info.ioccc_id, info.entry_num, &tarball_path, info.tstamp);
    info.tarball_path = tarball_path;
    dbg(DBG_LOW, "formed entry directory: %s", entry_dir);


    /*
     * if -a, open the answers file. We only do it after verifying that we can
     * make the entry directory because if we get input before and find out the
     * directory already exists then the answers file will have invalid data.
     */

    if (answers_flag_used && answers != NULL && strlen(answers) > 0) {

	errno = 0;			/* pre-clear errno for errp() */
	answerp = fopen(answers, "w");
	if (answerp == NULL) {
	    errp(12, __func__, "cannot create answers file: %s", answers);
	    not_reached();
	}
        ret = fprintf(answerp, "%s\n", MKIOCCCENTRY_ANSWERS_VER);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing header to the answers file");
	}
    }

    /*
     * write the IOCCC id and entry number to the answers file
     */
    if (answerp != NULL && answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
        ret = fprintf(answerp, "%s\n", info.ioccc_id);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing IOCCC contest id to the answers file");
	}
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(answerp, "%d\n", info.entry_num);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing entry number to the answers file");
	}
    }

    /*
     * check prog.c
     */
    para("", "Checking prog.c ...", NULL);
    check_prog_c(&info, entry_dir, cp, prog_c);
    para("... completed prog.c check.", "", NULL);

    /*
     * check Makefile
     */
    para("Checking Makefile ...", NULL);
    check_Makefile(&info, entry_dir, cp, Makefile);
    para("... completed Makefile check.", "", NULL);

    /*
     * check remarks.md
     */
    para("Checking remarks.md ...", NULL);
    check_remarks_md(&info, entry_dir, cp, remarks_md);
    para("... completed remarks.md check.", "", NULL);

    /*
     * check, if needed, extra data files
     */
    para("Checking extra data files ...", NULL);
    check_extra_data_files(&info, entry_dir, cp, extra_count, extra_list);
    para("... completed extra data files check.", "", NULL);

    /*
     * obtain the title
     */
    info.title = get_title(&info);
    dbg(DBG_LOW, "entry title: %s", info.title);
    if (answerp != NULL && answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(answerp, "%s\n", info.title);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing title to the answers file");
	}
    }

    /*
     * obtain the abstract
     */
    info.abstract = get_abstract(&info);
    dbg(DBG_LOW, "entry abstract: %s", info.abstract);
    if (answerp != NULL && answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(answerp, "%s\n", info.abstract);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing abstract to the answers file");
	}
    }

    /*
     * obtain author information
     */
    author_count = get_author_info(&info, info.ioccc_id, &author_set);
    dbg(DBG_LOW, "collected information on %d authors", author_count);

    /*
    * if we have an answers file, record the verified author information
    */
    if (answerp != NULL && !read_answers_flag_used) {
	errno = 0;			/* pre-clear errno for warnp() */
        ret = fprintf(answerp, "%d\n", author_count);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing IOCCC author count to the answers file");
	    info.answers_errors++;
	}

	/*
	 * write answers for each author to the answers file
	 */
	for (i = 0; i < author_count; i++) {
	    errno = 0;			/* pre-clear errno for warnp() */
	    ret = fprintf(answerp,
		"%s\n" "%s\n" "%s\n" "%s\n" "%s\n" "%s\n" "%s\n",
		author_set[i].name,
		author_set[i].location_code,
		author_set[i].email,
		author_set[i].url,
		author_set[i].twitter,
		author_set[i].github,
		author_set[i].affiliation);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error printing author info the answers file");
		info.answers_errors++;
	    }
	}
    }

    /*
     * write the .info.json file
     */
    para("", "Forming the .info.json file ...", NULL);
    write_info(&info, entry_dir, test_mode, jinfochk);
    para("... completed the .info.json file.", "", NULL);

    /*
     * write the .author.json file
     */
    para("", "Forming the .author.json file ...", NULL);
    write_author(&info, author_count, author_set, entry_dir, jauthchk);
    para("... completed .author.json file.", "", NULL);

    /*
     * finalize the answers file, writing final answers (if writing answers) and
     * then closing the stream.
     */
    if (answerp != NULL) {
	if (read_answers_flag_used) {
	    char *linep = NULL;
	    char *line;
	    bool error = true;

	    line = readline_dup(&linep, true, NULL, answerp);
	    if (linep != NULL) {
		error = strcmp(line, MKIOCCCENTRY_ANSWERS_EOF) != 0;
		free(linep);
	    }
	    if (error) {
	        errp(13, __func__, "expected ANSWERS_EOF marker at the end of the answers file");
	        not_reached();
	    }
	    input_stream = stdin;
	} else {
	    ret = fprintf(answerp, "%s\n", MKIOCCCENTRY_ANSWERS_EOF);
	    if (ret <= 0) {
	        warnp(__func__, "fprintf error writing ANSWERS_EOF marker to the answers file");
		info.answers_errors++;
	    }
	}
	if (answers != NULL) {
	    ret = fclose(answerp);
	    if (ret != 0) {
	        warnp(__func__, "error in fclose to the answers file");
	        info.answers_errors++;
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
	    ret = printf("\nTo more easily update this entry you can run:\n\n    ./mkiocccentry -i %s ...\n", answers);
	    if (ret <= 0) {
		warnp(__func__, "unable to tell user how to more easily update entry");
	    }
	}
    }

    /*
     * warn the user if there were I/O errors while writing the answers file
     */
    if (answers_flag_used) {
	if (info.answers_errors > 0) {
	    errno = 0;	/* pre-clear errno for warnp() */
	    ret = printf("Warning: There were %u I/O error%s on the answers file. Make SURE to verify that using the file\n"
			 "results in the proper input before re-uploading!\n",
			 info.answers_errors, info.answers_errors == 1 ? "" : "s" );
	    if (ret <= 0) {
		warnp(__func__, "unable to warn user that there were I/O errors on the answers file");
	    }
	}
    }

    /*
     * If the answers file (-i answers) was used, and there were warnings/problems
     * discovered with the entry that were overridden, warn the user.
     */
    if (read_answers_flag_used) {
	if (info.empty_override ||
	    info.rule_2a_override ||
	    info.rule_2a_mismatch ||
	    info.rule_2b_override ||
	    info.highbit_warning ||
	    info.nul_warning ||
	    info.trigraph_warning ||
	    info.wordbuf_warning ||
	    info.ungetc_warning ||
	    info.Makefile_override) {

	    do {
		if (!ignore_warnings) {
		    need_confirm = true;

		    if (info.empty_override) {
			warn_empty_prog(prog_c);
		    }
		    if (info.rule_2a_override) {
			warn_rule2a_size(&info, prog_c, RULE_2A_BIG_FILE_WARNING);
		    }
		    if (info.rule_2a_mismatch) {
			warn_rule2a_size(&info, prog_c, RULE_2A_IOCCCSIZE_MISMATCH);
		    }
		    if (info.rule_2b_override) {
			warn_rule2b_size(&info, prog_c);
		    }
		    if (info.highbit_warning) {
			warn_high_bit(prog_c);
		    }
		    if (info.nul_warning) {
			warn_nul_chars(prog_c);
		    }
		    if (info.trigraph_warning) {
			warn_trigraph(prog_c);
		    }
		    if (info.wordbuf_warning) {
			warn_wordbuf(prog_c);
		    }
		    if (info.ungetc_warning) {
			warn_ungetc(prog_c);
		    }
		    if (info.Makefile_override) {
			warn_Makefile(Makefile, &info);
		    }
		}
	    } while (0);
	}
    }

    /*
     * form the .txz file
     */
    form_tarball(work_dir, entry_dir, tarball_path, tar, ls, txzchk, fnamchk);

    /*
     * remind user various things e.g., to upload (unless in test mode)
     */
    remind_user(work_dir, entry_dir, tar, tarball_path, test_mode);

    /*
     * free storage
     */
    if (entry_dir != NULL) {
	free(entry_dir);
	entry_dir = NULL;
    }
    if (tarball_path != NULL) {
	free(tarball_path);
	tarball_path = NULL;
    }
    free_info(&info);
    memset(&info, 0, sizeof(info));
    if (author_set != NULL) {
	free_author_array(author_set, author_count);
	free(author_set);
	author_set = NULL;
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
 *      usage(3, "missing required argument(s), tar: %s cp: %s ls: %s", "/usr/bin/tar", "/bin/cp", "/bin/ls", "./txzchk");
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message
 *	program		our program name
 *	tar		path to the tar utility
 *	cp		path to tar cp utility
 *	ls		path to tar ls utility
 *	txzchk		path to the txzchk tool
 *	fnamchk		path to the fnamchk tool
 *	jinfochk	path to jinfochk tool
 *	jauthchk	path to jauthchk tool
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *str, char const *program, char const *tar, char const *cp, char const *ls,
      char const *txzchk, char const *fnamchk, char const *jinfochk, char const *jauthchk)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (program == NULL) {
	program = "((NULL program))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", program);
    }
    if (tar == NULL) {
	tar = "((NULL tar))";
	warn(__func__, "\nin usage(): tar was NULL, forcing it to be: %s\n", tar);
    }
    if (cp == NULL) {
	cp = "((NULL cp))";
	warn(__func__, "\nin usage(): cp was NULL, forcing it to be: %s\n", cp);
    }
    if (ls == NULL) {
	ls = "((NULL ls))";
	warn(__func__, "\nin usage(): ls was NULL, forcing it to be: %s\n", ls);
    }
    if (txzchk == NULL) {
	txzchk = "((NULL txzchk))";
	warn(__func__, "\nin usage(): txzchk was NULL, forcing it to be: %s\n", txzchk);
    }
    if (fnamchk == NULL) {
	fnamchk = "((NULL fnamchk))";
	warn(__func__, "\nin usage(): fnamchk was NULL, forcing it to be: %s\n", fnamchk);
    }
    if (jinfochk == NULL) {
	jinfochk = "((NULL jinfochk))";
	warn(__func__, "\nin usage(): jinfochk was NULL, forcing it to be: %s\n", jinfochk);
    }
    if (jauthchk == NULL) {
	jauthchk = "((NULL jauthchk))";
	warn(__func__, "\nin usage(): jauthchk was NULL, forcing it to be: %s\n", jauthchk);
    }



    /*
     * print the formatted usage stream
     */
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    vfprintf_usage(DO_NOT_EXIT, stderr, usage_msg0, program, DBG_DEFAULT);
    vfprintf_usage(DO_NOT_EXIT, stderr, usage_msg1, tar, cp, ls, txzchk, fnamchk);
    vfprintf_usage(DO_NOT_EXIT, stderr, usage_msg2, jinfochk, jauthchk);
    vfprintf_usage(DO_NOT_EXIT, stderr, "%s", usage_msg3);
    vfprintf_usage(exitcode, stderr, usage_msg4, MKIOCCCENTRY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * free_info - free info and related sub-elements
 *
 * given:
 *      infop   - pointer to info structure to free
 *
 * This function does not return.
 */
static void
free_info(struct info *infop)
{
    int i;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(14, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free version values
     */
    if (infop->mkiocccentry_ver != NULL) {
	free(infop->mkiocccentry_ver);
	infop->mkiocccentry_ver = NULL;
    }

    /*
     * free entry values
     */
    if (infop->ioccc_id != NULL) {
	free(infop->ioccc_id);
	infop->ioccc_id = NULL;
    }
    if (infop->title != NULL) {
	free(infop->title);
	infop->title = NULL;
    }
    if (infop->abstract != NULL) {
	free(infop->abstract);
	infop->abstract = NULL;
    }

    /*
     * free filenames
     */
    if (infop->prog_c != NULL) {
	free(infop->prog_c);
	infop->prog_c = NULL;
    }
    if (infop->Makefile != NULL) {
	free(infop->Makefile);
	infop->Makefile = NULL;
    }
    if (infop->remarks_md != NULL) {
	free(infop->remarks_md);
	infop->remarks_md = NULL;
    }
    if (infop->extra_file != NULL) {
	for (i = 0; i < infop->extra_count; ++i) {
	    if (infop->extra_file[i] != NULL) {
		free(infop->extra_file[i]);
		infop->extra_file[i] = NULL;
	    }
	}
	free(infop->extra_file);
	infop->extra_file = NULL;
    }

    /*
     * free time values
     */
    if (infop->epoch != NULL) {
	free(infop->epoch);
	infop->epoch = NULL;
    }
    if (infop->gmtime != NULL) {
	free(infop->gmtime);
	infop->gmtime = NULL;
    }
    return;
}


/*
 * free_author_array - free storage related to a struct author
 *
 * given:
 *      author_set      - pointer to a struct author array
 *      author_count    - length of author array
 */
static void
free_author_array(struct author *author_set, int author_count)
{
    int i;

    /*
     * firewall
     */
    if (author_set == NULL) {
	err(15, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(16, __func__, "author_count: %d < 0", author_count);
	not_reached();
    }

    /*
     * free elements of each array member
     */
    for (i = 0; i < author_count; ++i) {
	if (author_set[i].name != NULL) {
	    free(author_set[i].name);
	    author_set[i].name = NULL;
	}
	if (author_set[i].location_code != NULL) {
	    free(author_set[i].location_code);
	    author_set[i].location_code = NULL;
	}
	if (author_set[i].email != NULL) {
	    free(author_set[i].email);
	    author_set[i].email = NULL;
	}
	if (author_set[i].url != NULL) {
	    free(author_set[i].url);
	    author_set[i].url = NULL;
	}
	if (author_set[i].twitter != NULL) {
	    free(author_set[i].twitter);
	    author_set[i].twitter = NULL;
	}
	if (author_set[i].github != NULL) {
	    free(author_set[i].github);
	    author_set[i].github = NULL;
	}
	if (author_set[i].affiliation != NULL) {
	    free(author_set[i].affiliation);
	    author_set[i].affiliation = NULL;
	}
    }
    return;
}


/*
 * sanity_chk - perform basic sanity checks
 *
 * We perform basic sanity checks on paths and the IOCCC contest ID.
 *
 * given:
 *
 *      infop           - pointer to info structure
 *      work_dir        - where the entry directory and tarball are formed
 *      tar             - path to tar that supports the -J (xz) option
 *	cp		- path to the cp utility
 *	ls		- path to the ls utility
 *	txzchk		- path to txzchk tool
 *	fnamchk		- path to fnamchk tool
 *	jinfochk	- path to jinfochk tool
 *	jauthchk	- path to jauthchk tool
 *
 * NOTE: This function does not return on error or if things are not sane.
 */
static void
sanity_chk(struct info *infop, char const *work_dir, char const *tar, char const *cp, char const *ls,
	   char const *txzchk, char const *fnamchk, char const *jinfochk, char const *jauthchk)
{
    /*
     * firewall
     */
    if (infop == NULL || work_dir == NULL || tar == NULL || cp == NULL || ls == NULL ||
	txzchk == NULL || fnamchk == NULL || jinfochk == NULL || jauthchk == NULL) {
	err(17, __func__, "called with NULL arg(s)");
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
	      "    mkiocccentry -t /path/to/tar ...",
	      "",
	      "and/or install a tar program?  You can find the source for tar:",
	      "",
	      "    https://www.gnu.org/software/tar/",
	      "",
	      NULL);
	err(18, __func__, "tar does not exist: %s", tar);
	not_reached();
    }
    if (!is_file(tar)) {
	fpara(stderr,
	      "",
	      "The tar, while it exists, is not a file.",
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
	err(19, __func__, "tar is not a file: %s", tar);
	not_reached();
    }
    if (!is_exec(tar)) {
	fpara(stderr,
	      "",
	      "The tar, while it is a file, is not an executable.",
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
	err(20, __func__, "tar is not an executable program: %s", tar);
	not_reached();
    }

    /*
     * cp must be executable
     */
    if (!exists(cp)) {
	fpara(stderr,
	      "",
	      "We cannot find a cp program.",
	      "",
	      "A cp program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -c /path/to/cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(21, __func__, "cp does not exist: %s", cp);
	not_reached();
    }
    if (!is_file(cp)) {
	fpara(stderr,
	      "",
	      "The cp, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -c /path/to/cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(22, __func__, "cp is not a file: %s", cp);
	not_reached();
    }
    if (!is_exec(cp)) {
	fpara(stderr,
	      "",
	      "The cp, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the cp program, or use another path:",
	      "",
	      "    mkiocccentry -c /path/to/cp ...",
	      "",
	      "and/or install a cp program?  You can find the source for cp in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(23, __func__, "cp is not an executable program: %s", cp);
	not_reached();
    }

    /*
     * ls must be executable
     */
    if (!exists(ls)) {
	fpara(stderr,
	      "",
	      "We cannot find a ls program.",
	      "",
	      "A ls program is required to copy files into a directory under work_dir.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -l /path/to/ls ...",
	      "",
	      "and/or install a ls program?  You can find the source for ls in core utilities:",
	      "",
	      "    https://www.gnu.org/software/coreutils/",
	      "",
	      NULL);
	err(24, __func__, "ls does not exist: %s", ls);
	not_reached();
    }
    if (!is_file(ls)) {
	fpara(stderr,
	      "",
	      "The ls, while it exists, is not a file.",
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
	err(25, __func__, "ls is not a file: %s", ls);
	not_reached();
    }
    if (!is_exec(ls)) {
	fpara(stderr,
	      "",
	      "The ls, while it is a file, is not executable.",
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
	err(26, __func__, "ls is not an executable program: %s", ls);
	not_reached();
    }

    /*
     * txzchk must be executable
     */
    if (!exists(txzchk)) {
	fpara(stderr,
	      "",
	      "We cannot find a txzchk tool.",
	      "",
	      "A txzchk program performs a sanity check on the compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -C /path/to/txzchk ...",
	      "",
	      "and/or install the txzchk tool?  You can find the source for txzchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(27, __func__, "txzchk does not exist: %s", txzchk);
	not_reached();
    }
    if (!is_file(txzchk)) {
	fpara(stderr,
	      "",
	      "The txzchk tool, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -C /path/to/txzchk ...",
	      "",
	      "and/or install the txzchk tool?  You can find the source for txzchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(28, __func__, "txzchk is not a file: %s", txzchk);
	not_reached();
    }
    if (!is_exec(txzchk)) {
	fpara(stderr,
	      "",
	      "The txzchk tool, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the txzchk program, or use another path:",
	      "",
	      "    mkiocccentry -C /path/to/txzchk ...",
	      "",
	      "and/or install the txzchk tool?  You can find the source for txzchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(29, __func__, "txzchk is not an executable program: %s", txzchk);
	not_reached();
    }

    /*
     * fnamchk must be executable
     */
    if (!exists(fnamchk)) {
	fpara(stderr,
	      "",
	      "We cannot find a fnamchk tool.",
	      "",
	      "A fnamchk program performs a sanity check on the compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -F /path/to/fnamchk ...",
	      "",
	      "and/or install the fnamchk tool?  You can find the source for fnamchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(30, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }
    if (!is_file(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk tool, while it exists, is not a file.",
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
	err(31, __func__, "fnamchk is not a file: %s", fnamchk);
	not_reached();
    }
    if (!is_exec(fnamchk)) {
	fpara(stderr,
	      "",
	      "The fnamchk tool, while it is a file, is not executable.",
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
	err(32, __func__, "fnamchk is not an executable program: %s", fnamchk);
	not_reached();
    }

    /*
     * jinfochk must be executable
     */
    if (!exists(jinfochk)) {
	fpara(stderr,
	      "",
	      "We cannot find a jinfochk tool.",
	      "",
	      "A jinfochk program performs a sanity check on the compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -j /path/to/jinfochk ...",
	      "",
	      "and/or install the jinfochk tool?  You can find the source for jinfochk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(33, __func__, "jinfochk does not exist: %s", jinfochk);
	not_reached();
    }
    if (!is_file(jinfochk)) {
	fpara(stderr,
	      "",
	      "The jinfochk tool, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -j /path/to/jinfochk ...",
	      "",
	      "and/or install the jinfochk tool?  You can find the source for jinfochk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(34, __func__, "jinfochk is not a file: %s", jinfochk);
	not_reached();
    }
    if (!is_exec(jinfochk)) {
	fpara(stderr,
	      "",
	      "The jinfochk tool, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the jinfochk program, or use another path:",
	      "",
	      "    mkiocccentry -j /path/to/jinfochk ...",
	      "",
	      "and/or install the jinfochk tool?  You can find the source for jinfochk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(35, __func__, "jinfochk is not an executable program: %s", jinfochk);
	not_reached();
    }

    /*
     * jauthchk must be executable
     */
    if (!exists(jauthchk)) {
	fpara(stderr,
	      "",
	      "We cannot find a jauthchk tool.",
	      "",
	      "A jauthchk program performs a sanity check on the compressed tarball.",
	      "Perhaps you need to use:",
	      "",
	      "    mkiocccentry -J /path/to/jauthchk ...",
	      "",
	      "and/or install the jauthchk tool?  You can find the source for jauthchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(36, __func__, "jauthchk does not exist: %s", jauthchk);
	not_reached();
    }
    if (!is_file(jauthchk)) {
	fpara(stderr,
	      "",
	      "The jauthchk tool, while it exists, is not a file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    mkiocccentry -J /path/to/jauthchk ...",
	      "",
	      "and/or install the jauthchk tool?  You can find the source for jauthchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(37, __func__, "jauthchk is not a file: %s", jauthchk);
	not_reached();
    }
    if (!is_exec(jauthchk)) {
	fpara(stderr,
	      "",
	      "The jauthchk tool, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the jauthchk program, or use another path:",
	      "",
	      "    mkiocccentry -J /path/to/jauthchk ...",
	      "",
	      "and/or install the jauthchk tool?  You can find the source for jauthchk in the mkiocccentry GitHub repo:",
	      "",
	      "    https://github.com/ioccc-src/mkiocccentry",
	      "",
	      NULL);
	err(38, __func__, "jauthchk is not an executable program: %s", jauthchk);
	not_reached();
    }

    /*
     * work_dir must be a writable directory
     */
    if (!exists(work_dir)) {
	fpara(stderr,
	      "",
	      "The work_dir does not exist.",
	      "",
	      "You should either create work_dir, or use a different work_dir directory path on the command line.",
	      "",
	      NULL);
	err(39, __func__, "work_dir does not exist: %s", work_dir);
	not_reached();
    }
    if (!is_dir(work_dir)) {
	fpara(stderr,
	      "",
	      "While work_dir exists, it is not a directory.",
	      "",
	      "You should move or remove work_dir and them make a new work_dir directory, or use a different",
	      "work_dir directory path on the command line.",
	      "",
	      NULL);
	err(40, __func__, "work_dir is not a directory: %s", work_dir);
	not_reached();
    }
    if (!is_write(work_dir)) {
	fpara(stderr,
	      "",
	      "While the directory work_dir exists, it is not a writable directory.",
	      "",
	      "You should change the permission to make work_dir writable, or you move or remove work_dir and then",
	      "create a new writable directory, or use a different work_dir directory path on the command line.",
	      "",
	      NULL);
	err(41, __func__, "work_dir is not a writable directory: %s", work_dir);
	not_reached();
    }

    /*
     * obtain version string from iocccsize_version
     */
    infop->iocccsize_ver = iocccsize_version;
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
 *      malloced input string with newline and trailing whitespace removed
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
    char *buf;			/* malloced input string */

    /*
     * firewall
     */
    if (str == NULL) {
	err(42, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * prompt + :<space>
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fputs(str, stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(43, __func__, "error printing prompt string");
	    not_reached();
	} else if (feof(stdout)) {
	    err(44, __func__, "EOF while printing prompt string");
	    not_reached();
	} else {
	    errp(45, __func__, "unexpected fputs error printing prompt string");
	    not_reached();
	}
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fputs(": ", stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(46, __func__, "error printing :<space>");
	    not_reached();
	} else if (feof(stdout)) {
	    err(47, __func__, "EOF while writing :<space>");
	    not_reached();
	} else {
	    errp(48, __func__, "unexpected fputs error printing :<space>");
	    not_reached();
	}
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(49, __func__, "error flushing prompt to stdout");
	    not_reached();
	} else if (feof(stdout)) {
	    err(50, __func__, "EOF while flushing prompt to stdout");
	    not_reached();
	} else {
	    errp(51, __func__, "unexpected fflush error while flushing prompt to stdout");
	    not_reached();
	}
    }

    /*
     * read user input - return input length
     */
    buf = readline_dup(&linep, true, &len, input_stream);
    if (buf == NULL) {
	err(52, __func__, "EOF while reading prompt input");
	not_reached();
    }
    dbg(DBG_VHIGH, "received a %lu byte response", (unsigned long)len);

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
     * return malloced input buffer
     */
    return buf;
}


/*
 * get IOCCC ID or test
 *
 * This function will prompt the user for a contest ID, validate it and return it
 * as a malloced string.  If the contest ID is the special value "test", then
 * *testp will be set to true, otherwise it will be set to false.
 *
 * given:
 *      infop   - pointer to info structure
 *      testp   - pointer to boolean for test mode
 *
 * returns:
 *      malloced contest ID string
 *      *testp ==> contest ID is "test", else contest ID is a UUID.
 *
 * This function does not return on error or if the contest ID is malformed.
 */
static char *
get_contest_id(struct info *infop, bool *testp, bool *read_answers_flag_used)
{
    char *malloc_ret;		/* malloced return string */
    size_t len;			/* input string length */
    int ret;			/* libc function return */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    char guard;			/* scanf guard to catch excess amount of input */
    size_t i;
    bool seen_answers_header = false;

    /*
     * firewall
     */
    if (infop == NULL || testp == NULL) {
	err(53, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * explain contest ID
     */
    if (need_hints) {
	para("To submit entries to the IOCCC, you must a registered contestant and have received a",
	     "IOCCC contest ID (via email) shortly after you have been successfully registered.",
	     "If the IOCCC is open, you may register as a contestant. See:",
	     "",
	     NULL);
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "    %s\n", IOCCC_REGISTER_URL);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error printing IOCCC_REGISTER_URL");
	}
	para("",
	     "If you do not have an IOCCC contest ID and you with to test this program,",
	     "you may use the special contest ID:",
	     "",
	     "    test",
	     "",
	     "Note you will not be able to submit the resulting compressed tarball when using test.",
	     "",
	     NULL);
    }

    /*
     * keep asking for an entry number until we get a valid reply
     */
    do {

	/*
	 * prompt for the contest ID
	 */
	malloc_ret = prompt("Enter IOCCC contest ID or test", &len);
	if (!seen_answers_header && !strcmp(malloc_ret, MKIOCCCENTRY_ANSWERS_VER)) {
	    dbg(DBG_HIGH, "found answers header");
	    seen_answers_header = true;
	    *read_answers_flag_used = true;
	    need_retry = false;
	    need_confirm = false;
	    need_hints = false;

	    free(malloc_ret);
	    malloc_ret = prompt("", &len);
	}
	if (*read_answers_flag_used && !seen_answers_header) {
	    dbg(DBG_HIGH, "the IOCCC contest ID as entered is: %s", malloc_ret);
	    err(54, __func__, "didn't find the correct answers file header");
	    not_reached();
	}

	dbg(DBG_HIGH, "the IOCCC contest ID as entered is: %s", malloc_ret);
	ret = 0;		/* initialize paranoia */

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
	 *             xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx
	 *
	 * where 'x' is a hex character.  The 4 is the UUID version and the variant 1.
	 */
	if (len != UUID_LEN) {

	    /*
	     * reject improper input length
	     */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "\nIOCCC contest ID are %d characters in length, you entered %lu\n\n",
				  UUID_LEN, (unsigned long)len);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error while improper input length");
	    }
	    fpara(stderr,
		  "IOCCC contest IDs are of the form:",
		  "",
		  "    xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx",
		  "",
		  "where 'x' is a hex character, 4 is the UUID version and the variant 1.",
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
	/*
	 * convert to lower case
	 */
	for (i = 0; i < len; ++i) {
	    malloc_ret[i] = (char)tolower(malloc_ret[i]);
	}
	dbg(DBG_VHIGH, "converted the IOCCC contest ID to: %s", malloc_ret);
	ret = sscanf(malloc_ret, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x%c", &a, &b, &version, &c, &variant, &d, &e, &f, &guard);
	dbg(DBG_HIGH, "UUID version hex char: %1x", version);
	dbg(DBG_HIGH, "UUID variant hex char: %1x", variant);
	if (ret != 8 || version != UUID_VERSION || variant != UUID_VARIANT) {
	    fpara(stderr,
		  "",
		  "IOCCC contest IDs are version 4, variant 1 UUID as defined by RFC4122:",
		  "",
		  "    https://datatracker.ietf.org/doc/html/rfc4122#section-4.1.1",
		  "",
		  "They are of the form:",
		  "",
		  "    xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx",
		  "",
		  "where 'x' is a hex character, 4 is the UUID version and the variant 1.",
		  "",
		  "Your IOCCC contest ID is not a valid UUID.  Please check your the email you received",
		  "when you registered as an IOCCC contestant for the correct IOCCC contest ID.",
		  "",
		  NULL);
	}
    } while (ret != 8 && need_retry);

    /*
     * report on the result of the contest ID validation
     */
    if (ret != 8) {
	errp(55, __func__, "retry prompt is disabled");
	not_reached();
    }
    dbg(DBG_MED, "IOCCC contest ID is a UUID: %s", malloc_ret);

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
 * get_entry_num - obtain the entry number
 *
 * given:
 *      infop   - pointer to info structure
 *
 * returns:
 *      entry number >= 0 <= MAX_ENTRY_NUM
 */
static int
get_entry_num(struct info *infop)
{
    int entry_num;		/* entry number */
    char *entry_str;		/* entry number string */
    int ret;			/* libc function return */
    char guard;			/* scanf guard to catch excess amount of input */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(56, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * keep asking for an entry number until we get a valid reply
     */
    do {

	/*
	 * explain entry numbers
	 */
	if (need_hints) {
	    errno = 0;		/* pre-clear errno for errp() */
	    ret = printf("\nYou are allowed to submit up to %d entries to a given IOCCC.\n", MAX_ENTRY_NUM + 1);
	    if (ret <= 0) {
		errp(57, __func__, "printf error printing number of entries allowed");
		not_reached();
	    }
	    para("",
		 "As in C, Entry numbers start with 0.  If you are updating a previous entry, PLEASE",
		 "use the same entry number that you previously uploaded so we know which entry we",
		 "should replace. If this is your 1st entry to this given IOCCC, enter 0.",
		 "",
		 NULL);
	}

	/*
	 * ask for the entry number
	 */
	entry_str = prompt("Enter the entry number", NULL);

	/*
	 * check the entry number
	 */
	errno = 0;		/* pre-clear errno for warnp() */
	ret = sscanf(entry_str, "%d%c", &entry_num, &guard);
	if (ret != 1 || entry_num < 0 || entry_num > MAX_ENTRY_NUM) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "\nThe entry number must be a number from 0 thru %d, please re-enter.\n", MAX_ENTRY_NUM);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error while informing about the valid entry number range");
	    }
	    entry_num = -1;	/* invalidate input */
	}

	/*
	 * free storage
	 */
	if (entry_str != NULL) {
	    free(entry_str);
	    entry_str = NULL;
	}

    } while ((entry_num < 0 || entry_num > MAX_ENTRY_NUM) && need_retry);

    /*
     * report on the result of the entry number validation
     */
    if (entry_num < 0 || entry_num > MAX_ENTRY_NUM) {
	err(58, __func__, "retry prompt is disabled");
	not_reached();
    }
    dbg(DBG_MED, "IOCCC entry number is valid: %d", entry_num);

    /*
     * return the entry number
     */
    return entry_num;
}


/*
 * mk_entry_dir - make the entry directory
 *
 * Make a directory, under work_dir, from which the compressed tarball
 * will be formed.
 *
 * given:
 *      work_dir        - working directory under which the entry directory is formed
 *      ioccc_id        - IOCCC entry ID (or test)
 *      entry_num       - entry number
 *      tarball_path    - pointer to the malloced path to where the compressed tarball will be formed
 *      tstamp          - now as a timestamp
 *
 * returns:
 *      the path of the working directory
 *
 * This function does not return on error or if the entry directory cannot be formed.
 */
static char *
mk_entry_dir(char const *work_dir, char const *ioccc_id, int entry_num, char **tarball_path, time_t tstamp)
{
    size_t entry_dir_len;	/* length of entry directory */
    size_t tarball_len;		/* length of the compressed tarball path */
    char *entry_dir = NULL;	/* malloced entry directory path */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (work_dir == NULL || ioccc_id == NULL || tarball_path == NULL) {
	err(59, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (entry_num < 0 || entry_num > MAX_ENTRY_NUM) {
	err(60, __func__, "entry number: %d must >= 0 and <= %d", entry_num, MAX_ENTRY_NUM);
	not_reached();
    }

    /*
     * determine length of entry directory path
     */
    /*
     * work_dir/ioccc_id-entry
     */
    entry_dir_len = strlen(work_dir) + 1 + strlen(ioccc_id) + 1 + MAX_ENTRY_CHARS + 1 + 1;
    errno = 0;			/* pre-clear errno for errp() */
    entry_dir = (char *)malloc(entry_dir_len + 1);
    if (entry_dir == NULL) {
	errp(61, __func__, "malloc #0 of %lu bytes failed", (unsigned long)(entry_dir_len + 1));
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(entry_dir, entry_dir_len + 1, "%s/%s-%d", work_dir, ioccc_id, entry_num);
    if (ret <= 0) {
	errp(62, __func__, "snprintf to form entry directory failed");
	not_reached();
    }
    dbg(DBG_HIGH, "entry directory path: %s", entry_dir);

    /*
     * verify that the entry directory does not exist
     */
    if (exists(entry_dir)) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nentry directory already exists: %s\n", entry_dir);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error while informing that the entry directory already exists");
	}
	fpara(stderr,
	      "",
	      "You need to move that directory, or remove it, or use a different work_dir.",
	      "",
	      NULL);
	err(63, __func__, "entry directory exists: %s", entry_dir);
	not_reached();
    }
    dbg(DBG_HIGH, "entry directory path: %s", entry_dir);

    /*
     * make the entry directory
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = mkdir(entry_dir, 0755);
    if (ret < 0) {
	errp(64, __func__, "cannot mkdir %s with mode 0755", entry_dir);
	not_reached();
    }

    /*
     * form the compressed tarball path
     *
     * We assume timestamps will be values of 12 decimal digits or less in the future. :-)
     */
    tarball_len = LITLEN("entry.") + strlen(ioccc_id) + 1 + MAX_ENTRY_CHARS + LITLEN(".123456789012.txz") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    *tarball_path = (char *)malloc(tarball_len + 1);
    if (*tarball_path == NULL) {
	errp(65, __func__, "malloc #1 of %lu bytes failed", (unsigned long)(tarball_len + 1));
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(*tarball_path, tarball_len + 1, "entry.%s-%d.%ld.txz", ioccc_id, entry_num, (long)tstamp);
    if (ret <= 0) {
	errp(66, __func__, "snprintf to form compressed tarball path failed");
	not_reached();
    }
    dbg(DBG_HIGH, "compressed tarball path: %s", *tarball_path);

    /*
     * return entry directory
     */
    return entry_dir;
}


/*
 * warn_empty_prog - warn user that prog.c is empty
 *
 * given:
 *
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
warn_empty_prog(char const *prog_c)
{
    bool yorn = false;

    /*
     * firewall
     */
    if (prog_c == NULL) {
	err(67, __func__, "called with NULL arg(s)");
	not_reached();
    }

    dbg(DBG_MED, "prog.c: %s is empty", prog_c);
    if (need_confirm && !ignore_warnings) {
	fpara(stderr,
	  "WARNING: prog.c is empty.  An empty prog.c has been submitted before:",
	  "",
	  "    https://www.ioccc.org/years.html#1994_smr",
	  "",
	  "The guidelines indicate that we tend to dislike programs that:",
	  "",
	  "    * are rather similar to previous winners  :-(",
	  "",
	  "Perhaps you have a different twist on an empty prog.c than yet another",
	  "smallest self-replicating program.  If so, the you may proceed, although",
	  "we STRONGLY suggest that you put into your remarks.md file, why your",
	  "entry prog.c is not another smallest self-replicating program.",
	  "",
	  NULL);
	    yorn = yes_or_no("Are you sure you want to submit an empty prog.c file? [yn]");
	    if (!yorn) {
		err(68, __func__, "please fix your prog.c file: %s", prog_c);
		not_reached();
	    }
	    dbg(DBG_LOW, "user says that their empty prog.c: %s is OK", prog_c);
	}
}


/*
 * warn_rule2a_size - warn if prog.c is too large under Rule 2a
 *
 * given:
 *      infop           - pointer to info structure
 *      prog_c          - prog_c arg: given path to prog.c
 *      mode		- determines which warning to give
 *			  Either RULE_2A_BIG_FILE_WARNING or
 *				 RULE_2A_IOCCCSIZE_MISMATCH
 *
 * This function does not return on error.
 */
static void
warn_rule2a_size(struct info *infop, char const *prog_c, int mode)
{
    bool yorn = false;
    int ret;

    /*
     * firewall
     */
    if (infop == NULL || prog_c == NULL) {
	err(69, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * File is appears to be too big under Rule 2a warning
     */
    if (mode == RULE_2A_BIG_FILE_WARNING) {
	dbg(DBG_MED, "prog.c: %s size: %ld > Rule 2a size: %ld", prog_c,
		     (long)infop->rule_2a_size, (long)RULE_2A_SIZE);
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nWARNING: The prog.c %s size: %ld > Rule 2a maximum: %ld\n", prog_c,
		      (long)infop->rule_2a_size, (long)RULE_2A_SIZE);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c Rule 2a warning");
	}
	if (need_confirm && !ignore_warnings) {
	    fpara(stderr,
	      "If you are attempting some clever rule abuse, then we STRONGLY suggest that you",
	      "tell us about your rule abuse in your remarks.md file.  Be sure you have read the",
	      "\"ABUSING THE RULES\" section of the guidelines.  And more importantly, read rule 12!",
	      "",
	      NULL);
	    yorn = yes_or_no("Are you sure you want to submit such a large prog.c file? [yn]");
	    if (!yorn) {
		err(70, __func__, "please fix your prog.c file: %s", prog_c);
		not_reached();
	    }
	    dbg(DBG_LOW, "user says that their prog.c %s size: %ld > Rule 2a max size: %ld is OK", prog_c,
		(long)infop->rule_2a_size, (long)RULE_2A_SIZE);
	}

    /*
     * File size and iocccsize file size differ warning
     */
    } else if (mode == RULE_2A_BIG_FILE_WARNING) {
	if (need_confirm && !ignore_warnings) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "\nInteresting: prog.c: %s file size: %ld != rule_count function size: %ld\n"
				  "In order to avoid a possible Rule 2a violation, BE SURE TO CLEARLY MENTION THIS IN\n"
				  "YOUR remarks.md FILE!\n\n",
				  prog_c, (long)infop->rule_2a_size, (long)size.rule_2a_size);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error when printing prog.c file size and Rule 2a mismatch");
	    }
	    yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	    if (!yorn) {
		err(71, __func__, "please fix your prog.c file: %s", prog_c);
		not_reached();
	    }
	    dbg(DBG_LOW, "user says that prog.c %s size: %ld != rule_count function size: %ld is OK", prog_c,
		(long)infop->rule_2a_size, (long)size.rule_2a_size);
	}

    /*
     * invalid mode
     */
    } else {
	err(72, __func__, "invalid mode used: %d", mode);
	not_reached();
    }
    return;
}

/*
 * warn_high_bit - warn user that prog.c has high bit chars in prog.c
 *
 * given:
 *
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
warn_high_bit(char const *prog_c)
{
    int ret, yorn;

    /*
     * firewall
     */
    if (prog_c == NULL) {
	err(73, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * warn about high bit chars, if we are allowed
     */
    if (need_confirm && !ignore_warnings) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nprog_c: %s has character(s) with high bit set!\n"
			      "Be careful you don't violate rule 13!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c char_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(74, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s having character(s) with high bit is OK", prog_c);
    }
}


/*
 * warn_nul_chars - warn user that prog.c has high one or more NUL char(s)
 *
 * given:
 *
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
warn_nul_chars(char const *prog_c)
{
    int ret, yorn;

    /*
     * firewall
     */
    if (prog_c == NULL) {
	err(75, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * warn about NUL chars(s) if we are allowed
     */
    if (need_confirm && !ignore_warnings) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nprog_c: %s has NUL character(s)!\n"
			      "Be careful you don't violate rule 13!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c nul_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(76, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s having NUL character(s) is OK", prog_c);
    }
}


/*
 * warn_trigraph - warn user that prog.c has unknown or invalid trigraph(s)
 *
 * given:
 *
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
warn_trigraph(char const *prog_c)
{
    bool yorn = false;
    int ret;

    /*
     * firewall
     */
    if (prog_c == NULL) {
	err(77, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * warn the user about unknown or invalid trigraph(s), if we are allowed
     */
    if (need_confirm && !ignore_warnings) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(stderr, "\nprog_c: %s has unknown or invalid trigraph(s) found!\n"
			      "Is that a bug in, or a feature of your code?\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c trigraph_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(78, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s having unknown or invalid trigraph(s) is OK", prog_c);
    }
}


/*
 * warn_wordbuf - warn user that prog.c triggered a word buffer overflow
 *
 * given:
 *
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
warn_wordbuf(char const *prog_c)
{
    int ret, yorn;

    /*
     * firewall
     */
    if (prog_c == NULL) {
	err(79, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * warn the user about triggered a word buffer overflow in iocccsize, if we are allowed
     */
    if (need_confirm && !ignore_warnings) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nprog_c: %s triggered a word buffer overflow!\n"
			      "In order to avoid a possible Rule 2b violation, BE SURE TO CLEARLY MENTION THIS IN\n"
			      "YOUR remarks.md FILE!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c wordbuf_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(80, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s triggered a word buffer overflow is OK", prog_c);
    }
}


/*
 * warn_ungetc - warn user that prog.c triggered an ungetc error
 *
 * given:
 *
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
warn_ungetc(char const *prog_c)
{
    bool yorn = false;
    int ret;

    /*
     * firewall
     */
    if (prog_c == NULL) {
	err(81, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * warn the user abort iocccsize ungetc error, if we are allowed
     */
    if (need_confirm && !ignore_warnings) {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "\nprog_c: %s triggered a triggered an ungetc error: @SirWumpus goofed\n"
			      "In order to avoid a possible Rule 2b violation, BE SURE TO CLEARLY MENTION THIS IN\n"
			      "YOUR remarks.md FILE!\n\n", prog_c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf error when printing prog.c ungetc_warning");
	}
	yorn = yes_or_no("Are you sure you want to proceed? [yn]");
	if (!yorn) {
	    err(82, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that prog.c %s triggered an ungetc warning OK", prog_c);
    }
}


/*
 * warn_rule2b_size - warn user that prog.c triggered an ungetc error
 *
 * given:
 *
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
warn_rule2b_size(struct info *infop, char const *prog_c)
{
    int ret, yorn;

     /*
     * firewall
     */
    if (infop == NULL || prog_c == NULL) {
	err(83, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * warn the user about a possible Rule 2b violation, if we are allowed
     */
    if (need_confirm && !ignore_warnings) {
	errno = 0;
	ret = fprintf(stderr, "\nWARNING: The prog.c %s size: %lu > Rule 2b maximum: %lu\n", prog_c,
		      (unsigned long)infop->rule_2b_size, (unsigned long)RULE_2B_SIZE);
	if (ret <= 0) {
	    errp(84, __func__, "printf error printing prog.c size > Rule 2b maximum");
	    not_reached();
	}

	fpara(stderr,
	      "Unless you are attempting some clever rule abuse, then we STRONGLY suggest that you",
	      "tell us about your rule abuse in your remarks.md file.  Be sure you have read the",
	      "\"ABUSING THE RULES\" section of the guidelines.  And more importantly, read rule 12!",
	      "",
	      NULL);
	yorn = yes_or_no("Are you sure you want to submit such a large prog.c file? [yn]");
	if (!yorn) {
	    err(85, __func__, "please fix your prog.c file: %s", prog_c);
	    not_reached();
	}
	dbg(DBG_LOW, "user says that their prog.c %s size: %lu > Rule 2B max size: %lu is OK", prog_c,
	    (unsigned long)infop->rule_2b_size, (unsigned long)RULE_2B_SIZE);
    }
}


/*
 * check_prog_c - check prog_c arg and if OK, copy into entry_dir/prog.c
 *
 * Check if the prog_c argument is a readable file, and
 * if it is within the guidelines of iocccsize (or if the author overrides),
 * and if all is OK or overridden,
 * use cp to copy into entry_dir/prog.c.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      prog_c          - prog_c arg: given path to prog.c
 *
 * This function does not return on error.
 */
static void
check_prog_c(struct info *infop, char const *entry_dir, char const *cp, char const *prog_c)
{
    FILE *prog_stream;		/* prog.c open file stream */
    size_t prog_c_len;		/* length of the prog_c path */
    size_t entry_dir_len;	/* length of the entry_dir path */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || prog_c == NULL) {
	err(86, __func__, "called with NULL arg(s)");
	not_reached();
    }
    entry_dir_len = strlen(entry_dir);
    if (entry_dir_len <= 0) {
	err(87, __func__, "entry_dir arg is an empty string");
	not_reached();
    }
    prog_c_len = strlen(prog_c);
    if (prog_c_len <= 0) {
	err(88, __func__, "prog_c arg is an empty string");
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
	err(89, __func__, "prog.c does not exist: %s", prog_c);
	not_reached();
    }
    if (!is_file(prog_c)) {
	fpara(stderr,
	      "",
	      "The prog.c path, while it exists, is not a file.",
	      "",
	      NULL);
	err(90, __func__, "prog.c is not a file: %s", prog_c);
	not_reached();
    }
    if (!is_read(prog_c)) {
	fpara(stderr,
	      "",
	      "The prog.c, while it is a file, is not readable.",
	      "",
	      NULL);
	err(91, __func__, "prog.c is not a readable file: %s", prog_c);
	not_reached();
    }

    /*
     * warn if prog.c is empty
     */
    infop->rule_2a_size = file_size(prog_c);
    dbg(DBG_MED, "Rule 2a size: %ld", (long)infop->rule_2a_size);
    if (infop->rule_2a_size < 0) {
	err(92, __func__, "file_size error: %ld on prog_c: %s", (long)infop->rule_2a_size, prog_c);
	not_reached();
    } else if (infop->rule_2a_size == 0) {
	warn_empty_prog(prog_c);
	infop->empty_override = true;
	infop->rule_2a_override = false;

    /*
     * warn if prog.c is too large under Rule 2a
     */
    } else if (infop->rule_2a_size > RULE_2A_SIZE) {
	warn_rule2a_size(infop, prog_c, RULE_2A_BIG_FILE_WARNING);
	infop->empty_override = false;
	infop->rule_2a_override = true;
    } else {
	infop->empty_override = false;
	infop->rule_2a_override = false;
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
	errp(93, __func__, "failed to fopen: %s", prog_c);
	not_reached();
    }
    size = rule_count(prog_stream, 0);
    infop->rule_2b_size = size.rule_2b_size;
    dbg(DBG_MED, "prog.c: %s Rule 2b size: %lu", prog_c, (unsigned long)infop->rule_2b_size);
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(prog_stream);
    if (ret != 0) {
	errp(94, __func__, "failed to fclose: %s", prog_c);
	not_reached();
    }

    /*
     * sanity check on file size vs rule_count function size for Rule 2a
     */
    if (infop->rule_2a_size != size.rule_2a_size) {
	warn_rule2a_size(infop, prog_c, RULE_2A_IOCCCSIZE_MISMATCH);
	infop->rule_2a_mismatch = true;
    } else {
	infop->rule_2a_mismatch = false;
    }

    /*
     * sanity check on high bit character(s)
     */
    if (size.char_warning) {
	warn_high_bit(prog_c);
	infop->highbit_warning = true;
    } else {
	infop->highbit_warning = false;
    }

    /*
     * sanity check on NUL character(s)
     */
    if (size.nul_warning) {
	warn_nul_chars(prog_c);
	infop->nul_warning = true;
    } else {
	infop->nul_warning = false;
    }

    /*
     * sanity check on unknown tri-graph(s)
     */
    if (size.trigraph_warning) {
	warn_trigraph(prog_c);
	infop->trigraph_warning = true;
    } else {
	infop->trigraph_warning = false;
    }

    /*
     * sanity check on word buffer overflow
     */
    if (size.wordbuf_warning) {
	warn_wordbuf(prog_c);
	infop->wordbuf_warning = true;
    } else {
	infop->wordbuf_warning = false;
    }

    /*
     * sanity check on ungetc warning
     */
    if (size.ungetc_warning) {
	warn_ungetc(prog_c);
	infop->ungetc_warning = true;
    } else {
	infop->ungetc_warning = false;
    }

    /*
     * inspect the Rule 2b size
     */
    if (infop->rule_2b_size > RULE_2B_SIZE) {
	warn_rule2b_size(infop, prog_c);
	infop->rule_2b_override = true;
    } else {
	infop->rule_2b_override = false;
    }

    /*
     * copy prog.c under entry_dir
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -- % %/prog.c", cp, prog_c, entry_dir);
    if (cmd == NULL) {
	err(95, __func__, "failed to cmdprintf: cp prog_c entry_dir/prog.c");
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
	errp(96, __func__, "fflush(stdout) #2: error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(97, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the cp command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(98, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(99, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(100, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * save prog.c filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->prog_c = strdup("prog.c");
    if (infop->prog_c == NULL) {
	errp(101, __func__, "malloc #2 of %lu bytes failed", (unsigned long)(LITLEN("prog.c") + 1));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * inspect_Makefile - inspect the rule contents of Makefile
 *
 * Determine if the 1st rule contains all.  Determine if there is a clean rule.
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

    /*
     * firewall
     */
    if (Makefile == NULL || infop == NULL) {
	err(102, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * open Makefile
     */
    errno = 0;			/* pre-clear errno for errp() */
    stream = fopen(Makefile, "r");
    if (stream == NULL) {
	errp(103, __func__, "cannot open Makefile: %s", Makefile);
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
	 * skip line if there is no :
	 */
	p = strchr(line, ':');
	if (p == NULL) {

	    /*
	     * free storage
	     */
	    if (line != NULL) {
		free(line);
		line = NULL;
	    }

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
	for (p = strtok(line, " \t"); p != NULL; p = strtok(NULL, ":")) {

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
		     * all rule is in 1st rule line
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
	 */
	if (line != NULL) {
	    free(line);
	    line = NULL;
	}

    } while (!infop->first_rule_is_all || !infop->found_all_rule || !infop->found_clean_rule ||
	     !infop->found_clobber_rule || !infop->found_try_rule);

    /*
     * close Makefile
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(stream);
    if (ret < 0) {
	errp(104, __func__, "fclose error");
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
 *      Makefile          - Makefile arg: given path to Makefile
 *      infop		  - pointer to info struct
 *
 * This function does not return on error.
 */
static void
warn_Makefile(char const *Makefile, struct info *infop)
{
    bool yorn = false;
    int ret;

    /*
     * firewall
     */
    if (Makefile == NULL || infop == NULL) {
	err(105, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (need_confirm && !ignore_warnings) {
	/*
	 * report problem with Makefile
	 */
	fpara(stderr,
	      "",
	      "There are problems with the Makefile provided:",
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
		  "    The clobber role should depend on the clean rule, it could remove the entry's program,",
		  "    clean up after program execution (if needed), and restore the entire directory back",
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
	      "    all - compile the entry, must be the 1st entry",
	      "    clean - remove intermediate compilation files",
	      "    clobber - clean, remove compiled entry, restore to the original entry state",
	      "    try - invoke the entry at least once",
	      "",
	      "While this program's parser may have missed finding those Makefile rules,",
	      "chances are this file is not a proper Makefile under the IOCCC rules:",
	      "",
	      NULL);
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "    %s\n\n", Makefile);
	if (ret <= 0) {
	    warnp(__func__, "fprintf for Makefile error: %d", ret);
	}

	/*
	 * Ask if they want to submit it anyway
	 */
	yorn = yes_or_no("Do you still want to submit this Makefile in the hopes that it is OK? [yn]");
	if (!yorn) {
	    err(106, __func__, "Use a different Makefile or modify this file: %s", Makefile);
	    not_reached();
	}
    }
}


/*
 * check_Makefile - check Makefile arg and if OK, copy into entry_dir/Makefile
 *
 * Check if the Makefile argument is a readable file, and
 * if it has the proper rules (starting with all:),
 * use cp to copy into entry_dir/Makefile.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      Makefile        - Makefile arg: given path to Makefile
 *
 * This function does not return on error.
 */
static void
check_Makefile(struct info *infop, char const *entry_dir, char const *cp, char const *Makefile)
{
    off_t filesize = 0;		/* size of Makefile */
    int ret;			/* libc function return */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || Makefile == NULL) {
	err(107, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * Makefile must be a non-empty readable file
     */
    if (!exists(Makefile)) {
	fpara(stderr,
	      "",
	      "We cannot find the prog.c file.",
	      "",
	      NULL);
	err(108, __func__, "Makefile does not exist: %s", Makefile);
	not_reached();
    }
    if (!is_file(Makefile)) {
	fpara(stderr,
	       "",
	       "The Makefile path, while it exists, is not a file.",
	       "",
	       NULL);
	err(109, __func__, "Makefile is not a file: %s", Makefile);
	not_reached();
    }
    if (!is_read(Makefile)) {
	fpara(stderr,
	      "",
	      "The Makefile, while it is a file, is not readable.",
	      "",
	      NULL);
	err(110, __func__, "Makefile is not readable file: %s", Makefile);
	not_reached();
    }
    filesize = file_size(Makefile);
    if (filesize < 0) {
	err(111, __func__, "file_size error: %ld on Makefile  %s", (long)filesize, Makefile);
	not_reached();
    } else if (filesize == 0) {
	err(112, __func__, "Makefile cannot be empty: %s", Makefile);
	not_reached();
    }

    /*
     * scan Makefile for critical rules
     */
    if (!inspect_Makefile(Makefile, infop)) {
	warn_Makefile(Makefile, infop);
	infop->Makefile_override = true;
    } else {
	infop->Makefile_override = false;
    }

    /*
     * copy Makefile under entry_dir
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -- % %/Makefile", cp, Makefile, entry_dir);
    if (cmd == NULL) {
	err(113, __func__, "failed to cmdprintf: cp Makefile entry_dir/Makefile");
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
	errp(114, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(115, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the cp command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(116, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(117, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(118, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * save Makefile filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->Makefile = strdup("Makefile");
    if (infop->Makefile == NULL) {
	errp(119, __func__, "malloc #1 of %lu bytes failed", (unsigned long)(LITLEN("Makefile") + 1));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * check_remarks_md - check remarks_md arg and if OK, copy into entry_dir/Makefile
 *
 * Check if the remarks_md argument is a readable file, and
 * if it is not empty,
 * use cp to copy into entry_dir/remarks.md.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - the newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      remarks_md      - remarks_md arg: given path to author's remarks markdown file
 *
 * This function does not return on error.
 */
static void
check_remarks_md(struct info *infop, char const *entry_dir, char const *cp, char const *remarks_md)
{
    off_t filesize = 0;		/* size of remarks.md */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || remarks_md == NULL) {
	err(120, __func__, "called with NULL arg(s)");
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
	err(121, __func__, "remarks.md does not exist: %s", remarks_md);
	not_reached();
    }
    if (!is_file(remarks_md)) {
	fpara(stderr, "",
	      "The remarks.md path, while it exists, is not a file.",
	      "",
	      NULL);
	err(122, __func__, "remarks_md is not a file: %s", remarks_md);
	not_reached();
    }
    if (!is_read(remarks_md)) {
	fpara(stderr,
	      "",
	      "The remarks.md, while it is a file, is not readable.",
	      "",
	      NULL);
	err(123, __func__, "remarks_md is not readable file: %s", remarks_md);
	not_reached();
    }
    filesize = file_size(remarks_md);
    if (filesize < 0) {
	err(124, __func__, "file_size error: %ld on remarks_md %s", (long)filesize, remarks_md);
	not_reached();
    } else if (filesize == 0) {
	err(125, __func__, "remarks.md cannot be empty: %s", remarks_md);
	not_reached();
    }

    /*
     * copy remarks.md under entry_dir
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("% -- % %/remarks.md", cp, remarks_md, entry_dir);
    if (cmd == NULL) {
	err(126, __func__, "failed to cmdprintf: cp remarks.md entry_dir/remarks.md");
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
	errp(128, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(129, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the cp command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(130, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(131, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(132, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * save remarks_md filename
     */
    errno = 0;			/* pre-clear errno for errp() */
    infop->remarks_md = strdup("remarks.md");
    if (infop->remarks_md == NULL) {
	errp(133, __func__, "malloc #1 of %lu bytes failed", (unsigned long)(LITLEN("remarks.md") + 1));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * check_extra_data_files - check extra data files args and if OK, copy into entry_dir/Makefile
 *
 * Check if the check extra data files are readable, and
 * use cp to copy into entry_dir/remarks.md.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - newly created entry directory (by mk_entry_dir()) under work_dir
 *      cp              - cp utility path
 *      count           - number of extra data files arguments
 *      args            - pointer to an array of strings starting with 1st extra data file
 *
 * This function does not return on error.
 */
static void
check_extra_data_files(struct info *infop, char const *entry_dir, char const *cp, int count, char **args)
{
    char *base;			/* basename of extra data file */
    char *dest;			/* destination path of an extra data file */
    size_t base_len;		/* length of the basename of the data file */
    size_t dest_len;		/* length of the extra data file path */
    size_t entry_dir_len;	/* length of the entry_dir path */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    int ret;			/* libc function return */
    int i;
    size_t j;

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || cp == NULL || args == NULL) {
	err(134, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (count < 0) {
	err(135, __func__, "count :%d < 0", count);
	not_reached();
    }

    /*
     * save extra data file count
     */
    infop->extra_count = count;

    /*
     * allocate extra file arrays
     */
    errno = 0;			/* pre-clear errno for errp() */
    /* + 1 for trailing NULL */
    infop->extra_file = (char **)calloc((size_t)(count + 1), sizeof(char *));
    if (infop->extra_file == NULL) {
	errp(136, __func__, "calloc #0 of %d char * pointers failed", count + 1);
	not_reached();
    }

    /*
     * process all of the extra args
     */
    entry_dir_len = strlen(entry_dir);
    for (i = 0; i < count && args[i] != NULL; ++i) {

	/*
	 * extra data file must be a readable file
	 */
	dbg(DBG_HIGH, "processing extra data file %d: %s", i, args[i]);
	if (!exists(args[i])) {
	    fpara(stderr,
		  "",
		  "We cannot find an extra data file.",
		  "",
		  NULL);
	    err(137, __func__, "extra[%i] does not exist: %s", i, args[i]);
	    not_reached();
	}
	if (!is_file(args[i])) {
	    fpara(stderr,
		   "",
		   "The file, while it exists, is not a regular file.",
		   "",
		   NULL);
	    err(138, __func__, "extra[%i] is not a file: %s", i, args[i]);
	    not_reached();
	}
	if (!is_read(args[i])) {
	    fpara(stderr,
		  "",
		  "The file, while it is a file, is not readable.",
		  "",
		  NULL);
	    err(139, __func__, "extra[%i] is not readable file: %s", i, args[i]);
	    not_reached();
	}

	/*
	 * basename cannot be too long
	 */
	base = base_name(args[i]);
	dbg(DBG_VHIGH, "basename(%s): %s", args[i], base);
	base_len = strlen(base);
	if (base_len == 0) {
	    err(140, __func__, "basename of extra data file: %s has a length of 0", args[i]);
	    not_reached();
	} else if (base_len > MAX_BASENAME_LEN) {
	    fpara(stderr,
		  "",
		  "The basename of an extra file is too long.",
		  "",
		  NULL);
	    err(141, __func__, "basename of extra data file: %s is %lu characters an is > the limit: %lu",
			       args[i], (unsigned long)base_len, (unsigned long)MAX_BASENAME_LEN);
	    not_reached();
	}

	/*
	 * basename cannot begin with . nor - nor +
	 */
	if (base[0] == '.' || base[0] == '-' || base[0] == '+') {
	    fpara(stderr,
		  "",
		  "The first character is the basename of an extra file cannot be . nor - nor +",
		  "",
		  NULL);
	    err(142, __func__, "basename of extra data file: %s start with in invalid character: %s", args[i], base);
	    not_reached();
	}

	/*
	 * basename must only use POSIX Fully portable characters: A-Z a-z 0-9 . _ - and the + character
	 */
	for (j=0; j < base_len; ++j) {
	    if (!isascii(base[j]) ||
	        (!isalnum(base[j]) && base[j] != '.' && base[j] != '_' && base[j] != '-' && base[j] != '+')) {
		fpara(stderr,
		      "",
		      "The basename of an extra file can only consist of POSIX Fully portable characters and +:",
		      "",
		      "    A-Z a-z 0-9 . _ - +",
		      "",
		      NULL);
		err(143, __func__, "basename of %s character %lu is NOT a POSIX Fully portable character nor +",
				   args[i], (unsigned long)j);
		not_reached();
	    }
	}

	/*
	 * form destination path
	 */
	infop->extra_file[i] = base;
	dest_len = entry_dir_len + 1 + base_len + 1;
	errno = 0;		/* pre-clear errno for errp() */
	dest = (char *)malloc(dest_len + 1);
	if (dest == NULL) {
	    errp(144, __func__, "malloc #0 of %lu bytes failed", (unsigned long)(dest_len + 1));
	    not_reached();
	}
	ret = snprintf(dest, dest_len, "%s/%s", entry_dir, base);
	if (ret <= 0) {
	    errp(145, __func__, "snprintf #0 error: %d", ret);
	    not_reached();
	}
	dbg(DBG_VHIGH, "destination path: %s", dest);

	/*
	 * destination cannot exist
	 */
	if (exists(dest)) {
	    fpara(stderr,
		  "",
		  "extra data files cannot overwrite other files.",
		  "",
		  NULL);
	    err(146, __func__, "for extra file: %s destination already exists: %s", args[i], dest);
	    not_reached();
	}

	/*
	 * copy remarks_md under entry_dir
	 */
	cmd = cmdprintf("% -- % %", cp, args[i], dest);
	if (cmd == NULL) {
	    err(147, __func__, "failed to cmdprintf: cp extra_file[%d]: %s dest: %s", i, args[i], dest);
	    not_reached();
	}
	dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

	/*
	 * pre-flush to avoid system() buffered stdio issues
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fflush(stdout);
	if (ret < 0) {
	    errp(148, __func__, "fflush(stdout) error code: %d", ret);
	    not_reached();
	}
	clearerr(stderr);		/* pre-clear ferror() status */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fflush(stderr);
	if (ret < 0) {
	    errp(149, __func__, "fflush(stderr) #1: error code: %d", ret);
	    not_reached();
	}

	/*
	 * execute the cp command
	 */
	errno = 0;		/* pre-clear errno for errp() */
	exit_code = system(cmd);
	if (exit_code < 0) {
	    errp(150, __func__, "error calling system(%s)", cmd);
	    not_reached();
	} else if (exit_code == 127) {
	    errp(151, __func__, "execution of the shell failed for system(%s)", cmd);
	    not_reached();
	} else if (exit_code != 0) {
	    err(152, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	    not_reached();
	}

	/*
	 * free storage
	 */
	if (dest != NULL) {
	    free(dest);
	    dest = NULL;
	}
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
    }
    infop->extra_file[i] = NULL;
    return;
}


/*
 * lookup_location_name - convert a ISO 3166-1 Alpha-2 into a location name
 *
 * given:
 *      upper_code      - ISO 3166-1 Alpha-2 in UPPER CASE
 *
 * return:
 *      location name or NULL ==> unlisted code
 *
 * This function does not return on error.
 */
static char const *
lookup_location_name(char const *upper_code)
{
    struct location *p;		/* entry in the location table */

    /*
     * firewall
     */
    if (upper_code == NULL) {
	err(153, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * search location table for the code
     */
    for (p = &loc[0]; p->code != NULL && p->name != NULL; ++p) {
	if (strcmp(upper_code, p->code) == 0) {
	    dbg(DBG_VHIGH, "code %s name found: %s", p->code, p->name);
	    break;
	}
    }

    /*
     * return name or NULL
     */
    return p->name;
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
	err(154, __func__, "called with NULL arg(s)");
	not_reached();
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

    } while (response == NULL && need_retry);
    if (response == NULL) {
	errp(155, __func__, "retry prompt is disabled");
	not_reached();
    }

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
 * get_title - get the title of the entry
 *
 * Ask the user for an entry title, validate the response
 * and return the malloced title.
 *
 * given:
 *      infop   - pointer to info structure
 *
 * returns:
 *      malloced and validated title
 *
 * This function does not return on error.
 */
static char *
get_title(struct info *infop)
{
    char *title = NULL;		/* entry title to return or NULL */
    size_t len;			/* length of title */
    size_t span;		/* span of valid characters in title */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(156, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * inform the user of the title
     */
    if (need_hints) {
	para("An entry title is a short name using the [a-z0-9][a-z0-9_+-]* regex pattern.",
	      "",
	      "If your entry wins, the title might become the directory name of your entry.",
	      "Although the IOCCC judges might change the title for various reason.",
	      "",
	      "If you submitting more than one entry, please make your titles unique",
	      "amongst the entries that you submit to the current IOCCC.",
	      "",
	      NULL);
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fprintf(stderr, "You title must be between 1 and %d ASCII characters long.\n\n", MAX_TITLE_LEN);
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
	title = prompt("Enter a title for your entry", NULL);

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
	    ret = fprintf(stderr, "You title must be between 1 and %d ASCII characters long.\n\n", MAX_TITLE_LEN);
	    if (ret <= 0) {
		warnp(__func__, "fprintf #1 error: %d", ret);
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
	 * verify that the title starts with [a-z0-9]
	 */
	if (!isascii(title[0]) || (!islower(title[0]) && !isdigit(title[0]))) {
	    /*
	     * reject long title
	     */
	    fpara(stderr,
		  "",
		  "That title does not start with a lower case ASCII letter [a-z] or digit [0-9]:",
		  "",
		  NULL);

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
	 * verify that the title characters are from the valid character set
	 */
	span = strspn(title, TAIL_TITLE_CHARS);
	if (span != len) {

	    /*
	     * reject long title
	     */
	    fpara(stderr,
		  "",
		  "Your title contains invalid characters.  A title must match the following regex:",
		  "",
		  "    [a-z0-9][a-z0-9-]*",
		  "",
		  "That is, it must start with a lower case letter ASCII [a-z] or digit [0-9]",
		  "followed by zero or more lower case letters ASCII [a-z], digits [0-9],",
		  "- (ASCII dash), + (ASCII plus), or _ (ASCII underscore).",
		  "",
		  NULL);

	    /*
	     * free storage
	     */
	    if (title != NULL) {
		free(title);
		title = NULL;
	    }
	    continue;
	}
    } while (title == NULL && need_retry);

    /*
     * check the result of the title validation
     */
    if (title == NULL) {
	errp(157, __func__, "retry prompt is disabled");
	not_reached();
    }

    /*
     * returned malloced title
     */
    return title;
}


/*
 * get_abstract - get the abstract of the entry
 *
 * Ask the user for an entry abstract, validate the response
 * and return the malloced abstract.
 *
 * given:
 *      infop           - pointer to info structure
 *
 * returns:
 *      malloced and validated abstract
 *
 * This function does not return on error.
 */
static char *
get_abstract(struct info *infop)
{
    char *abstract = NULL;	/* entry abstract to return or NULL */
    size_t len;			/* length of abstract */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (infop == NULL) {
	err(158, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * inform the user of the abstract
     */
    para("",
	 "An entry abstract is 1-line summary of your entry.",
	 "",
	 NULL);

    /*
     * ask the question and obtain the response
     */
    do {

	/*
	 * obtain the reply
	 */
	abstract = prompt("Enter a 1-line abstract of your entry", NULL);

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
	    ret = fprintf(stderr, "You abstract must be between 1 and %d characters long.\n\n", MAX_ABSTRACT_LEN);
	    if (ret <= 0) {
		warnp(__func__, "fprintf error: %d", ret);
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
    } while (abstract == NULL && need_retry);

    /*
     * check the result of the abstract validation
     */
    if (abstract == NULL) {
	errp(159, __func__, "retry prompt is disabled");
	not_reached();
    }

    /*
     * returned malloced abstract
     */
    return abstract;
}


/*
 * get_author_info - obtain information on entry authors
 *
 * given:
 *      infop           - pointer to info structure
 *      ioccc_id        - IOCCC entry ID or test
 *      entry_num       - entry number
 *      author_set      - pointer to array of authors
 *
 * returns:
 *      number of authors
 *
 * This function does not return on error.
 */
static int
get_author_info(struct info *infop, char *ioccc_id, struct author **author_set_p)
{
    struct author *author_set = NULL;	/* allocated author set */
    int author_count = -1;		/* number of authors or -1 */
    char *author_count_str = NULL;	/* author count string */
    bool yorn = false;		/* response to a question */
    size_t len;			/* length of reply */
    int ret;			/* libc function return */
    char guard;			/* scanf guard to catch excess amount of input */
    char *p;
    int i;

    /*
     * firewall
     */
    if (infop == NULL || ioccc_id == NULL || author_set_p == NULL) {
	err(160, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * keep asking for an entry number until we get a valid reply
     */
    do {

	/*
	 * ask for author count
	 */
	author_count_str = prompt("\nEnter the number of authors of this entry", NULL);

	/*
	 * convert author_count_str to number
	 */
	ret = sscanf(author_count_str, "%d%c", &author_count, &guard);
	if (ret != 1 || author_count < 1 || author_count > MAX_AUTHORS) {
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = fprintf(stderr, "\nThe number of authors must a number from 1 thru %d, please re-enter.\n", MAX_AUTHORS);
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
	}

	/*
	 * free storage
	 */
	if (author_count_str != NULL) {
	    free(author_count_str);
	    author_count_str = NULL;
	}

    } while ((author_count < 1 || author_count > MAX_AUTHORS) && need_retry);

    if (author_count < 1 || author_count > MAX_AUTHORS) {
	errp(161, __func__, "retry prompt is disabled");
	not_reached();
    }

    dbg(DBG_HIGH, "will request information on %d authors", author_count);

    /*
     * allocate the author array
     */
    errno = 0;			/* pre-clear errno for errp() */
    author_set = (struct author *) malloc(sizeof(struct author) * (size_t)author_count);
    if (author_set == NULL) {
	errp(162, __func__, "malloc a struct author array of length: %d failed", author_count);
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
	     "We will now ask for information about the author(s) of this entry.",
	     "",
	     "Information that you supply, if your entry is selected as a winner,",
	     "will be published with your entry.",
	     "",
	     "Except for your name and location/country code, you can opt out of providing it,",
	     "(or if you don't have the thing we are asking for), by just pressing return.",
	     "",
	     "A name is required. If an author wishes to be anonymous, use a pseudo-name.",
	     "Keep in mind that if an author wins multiple years, or has won before, you might",
	     "want to be consistent and provide the same name or pseudo-name each time.",
	     "",
	     "We will ask for the location/country as a 2 character ISO 3166-1 Alpha-2 code.",
	     "",
	     "    See the following URLs for information on ISO 3166-1 Alpha-2 codes:",
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
	     "We will ask for the author(s) Email address. Press return if you don't want to provide it, or if don't have one.",
	     "We will ask for a home URL (starting with http:// or https://), or press return to skip, or if don't have one.",
	     "We will ask a twitter handle (must start with @), or press return to skip, or if don't have one.",
	     "We will ask a GitHub account (must start with @), or press return to skip, or if don't have one.",
	     "We will ask for an affiliation (company, school, org) of the author, or press return to skip, or if don't have one.",
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

	    /*
	     * reject empty author name
	     */
	    if (len <= 0) {

		/*
		 * issue rejection message
		 */
		fpara(stderr,
		      "",
		      "The author name cannot be empty, try again.  If they want to be anonymous, give a pseudo-name.",
		      "",
		      NULL);

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

		/*
		 * free storage
		 */
		if (author_set[i].name != NULL) {
		    free(author_set[i].name);
		    author_set[i].name = NULL;
		}
	    }
	} while (author_set[i].name == NULL && need_retry);
	if (author_set[i].name == NULL) {
	    errp(163, __func__, "retry prompt is disabled");
	    not_reached();
	}
	dbg(DBG_MED, "Author #%d Name %s", i, author_set[i].name);


	/*
	 * obtain author location/country code
	 */
	do {

	    /*
	     * request location/country code
	     */
	    author_set[i].location_code = NULL;
	    author_set[i].location_code = prompt("Enter author 2 character location/country code", &len);
	    dbg(DBG_VHIGH, "location/country code as entered: %s", author_set[i].location_code);

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
		ret = fprintf(stderr, "    %s\n\n", ISO_3166_1_CODE_URL0);
		if (ret <= 0) {
		    warnp(__func__, "fprintf while printing ISO 3166-1 CODE URL #0");
		}
		fpara(stderr,
		      "or from these Wikipedia / ISO web pages:",
		      "",
		      NULL);
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "    %s\n", ISO_3166_1_CODE_URL1);
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
		yorn = false;
		continue;

	    } else {

		/*
		 * valid location/country code - convert to upper case
		 */
		author_set[i].location_code[0] = (char)toupper(author_set[i].location_code[0]);
		author_set[i].location_code[1] = (char)toupper(author_set[i].location_code[1]);

		/*
		 * determine if code is known
		 */
		author_set[i].location_name = lookup_location_name(author_set[i].location_code);
		if (author_set[i].location_name == NULL) {

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
		    yorn = false;
		    continue;
		}

		/*
		 * verify the known location/country code
		 */
	        if (need_confirm) {
		    errno = 0;		/* pre-clear errno for warnp() */
		    ret = printf("The location/country code you entered is assigned to: %s\n", author_set[i].location_name);
		    if (ret <= 0) {
			warnp(__func__, "fprintf location/country code assignment");
		    }
		    yorn = yes_or_no("Is that location/country code correct? [yn]");

		    /*
		     * free storage if no (reenter location/country code)
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
	    }
	} while ((author_set[i].location_code == NULL || author_set[i].location_name == NULL || !yorn) &&
		 need_retry);
	if (author_set[i].location_code == NULL || author_set[i].location_name == NULL || !yorn) {
	    errp(164, __func__, "retry prompt is disabled");
	    not_reached();
	}
	dbg(DBG_MED, "Author #%d location/country: %s (%s)", i, author_set[i].location_code, author_set[i].location_name);

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
		dbg(DBG_VHIGH, "Email address withheld");
	    } else {
		dbg(DBG_VHIGH, "Email address: %s", author_set[i].email);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_EMAIL_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit Email address to %d characters\n\n", MAX_EMAIL_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing Email address length limit");
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

	    /*
	     * reject if no @ in the address
	     */
	    if (len > 0) {
		p = strchr(author_set[i].email, '@');
		if (p == NULL || author_set[i].email[0] == '@' || author_set[i].email[len - 1] == '@' ||
		    p != strrchr(author_set[i].email, '@')) {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "Email addresses must have only a single @ somewhere inside the string.",
			  "",
			  NULL);

		    /*
		     * free storage
		     */
		    if (author_set[i].email != NULL) {
			free(author_set[i].email);
			author_set[i].email = NULL;
		    }
		    continue;
		}
	    }
	} while (author_set[i].email == NULL && need_retry);
	if (author_set[i].email == NULL) {
	    errp(165, __func__, "retry prompt is disabled");
	    not_reached();
	}
	dbg(DBG_MED, "Author #%d Email: %s", i, author_set[i].email);

	/*
	 * ask for home URL
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
		dbg(DBG_VHIGH, "URL withheld");
	    } else {
		dbg(DBG_VHIGH, "URL: %s", author_set[i].url);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_URL_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we URLs to %d characters\n\n", MAX_URL_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing URL length limit");
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
			  "url addresses must begin with http:// or https:// followed by the rest of the home page URL",
			  "",
			  NULL);

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
	} while (author_set[i].url == NULL && need_retry);
	if (author_set[i].url == NULL) {
	    errp(166, __func__, "retry prompt is disabled");
	    not_reached();
	}
	dbg(DBG_MED, "Author #%d URL: %s", i, author_set[i].url);

	/*
	 * ask for twitter handle
	 */
	do {

	    /*
	     * request twitter handle
	     */
	    author_set[i].twitter = NULL;
	    author_set[i].twitter = prompt(need_hints ?
		"Enter author twitter handle, starting with @, or press return to skip" :
		"Enter author twitter handle", &len);
	    if (len == 0) {
		dbg(DBG_VHIGH, "Twitter handle not given");
	    } else {
		dbg(DBG_VHIGH, "Twitter handle: %s", author_set[i].twitter);
	    }

	    /*
	     * reject if too long
	     */
	    if (len > MAX_TWITTER_LEN) {

		/*
		 * issue rejection message
		 */
		errno = 0;		/* pre-clear errno for warnp() */
		ret = fprintf(stderr, "\nSorry ( tm Canada :-) ), we limit twitter handles,"
			      "starting with the @, to %d characters\n\n", MAX_TWITTER_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing twitter handle length limit");
		}

		/*
		 * free storage
		 */
		if (author_set[i].twitter != NULL) {
		    free(author_set[i].twitter);
		    author_set[i].twitter = NULL;
		}
		continue;
	    }

	    /*
	     * reject if no leading @, or if more than one @
	     */
	    if (len > 0) {
		p = strchr(author_set[i].twitter, '@');
		if (p == NULL || author_set[i].twitter[0] != '@' || p != strrchr(author_set[i].twitter, '@') ||
		    author_set[i].twitter[1] == '\0') {

		    /*
		     * issue rejection message
		     */
		    fpara(stderr,
			  "",
			  "Twitter handles must start with a @ and have no other @-signs.",
			  "",
			  NULL);

		    /*
		     * free storage
		     */
		    if (author_set[i].twitter != NULL) {
			free(author_set[i].twitter);
			author_set[i].twitter = NULL;
		    }
		    continue;
		}
	    }
	} while (author_set[i].twitter == NULL && need_retry);
	if (author_set[i].twitter == NULL) {
	    errp(167, __func__, "retry prompt is disabled");
	    not_reached();
	}
	dbg(DBG_MED, "Author #%d twitter: %s", i, author_set[i].twitter);

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
		dbg(DBG_VHIGH, "GitHub account not given");
	    } else {
		dbg(DBG_VHIGH, "GitHub account: %s", author_set[i].github);
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
			    "\nSorry ( tm Canada :-) ), we limit GitHub account names, starting with the @, to %d characters\n\n",
			    MAX_GITHUB_LEN);
		if (ret <= 0) {
		    warnp(__func__, "fprintf error while printing GitHub user length limit");
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
	} while (author_set[i].github == NULL && need_retry);
	if (author_set[i].github == NULL) {
	    errp(168, __func__, "retry prompt is disabled");
	    not_reached();
	}
	dbg(DBG_MED, "Author #%d GitHub: %s", i, author_set[i].github);

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
		dbg(DBG_VHIGH, "Affiliation not given");
	    } else {
		dbg(DBG_VHIGH, "Affiliation: %s", author_set[i].affiliation);
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

		/*
		 * free storage
		 */
		if (author_set[i].affiliation != NULL) {
		    free(author_set[i].affiliation);
		    author_set[i].affiliation = NULL;
		}
		continue;
	    }
	} while (author_set[i].affiliation == NULL && need_retry);
	if (author_set[i].affiliation == NULL) {
	    errp(169, __func__, "retry prompt is disabled");
	    not_reached();
	}
	dbg(DBG_MED, "Author #%d affiliation: %s", i, author_set[i].affiliation);

	/*
	 * verify the information for this author
	 */
	errno = 0;	/* pre-clear errno for errp() */
	if (printf("\nPlease verify the information about author #%d\n\n", i) <= 0 ||
	    printf("Name: %s\n", author_set[i].name) <= 0 ||
	    printf("Location/country code: %s (%s)\n", author_set[i].location_code, author_set[i].location_name) <= 0 ||
	    ((author_set[i].email[0] == '\0') ? printf("Email not given\n") :
						printf("Email: %s\n", author_set[i].email)) <= 0 ||
	    ((author_set[i].url[0] == '\0') ? printf("Url not given\n") :
					      printf("Url: %s\n", author_set[i].url)) <= 0 ||
	    ((author_set[i].twitter[0] == '\0') ? printf("Twitter handle not given\n") :
						  printf("Twitter handle: %s\n", author_set[i].twitter)) <= 0 ||
	    ((author_set[i].github[0] == '\0') ? printf("GitHub username not given\n") :
						 printf("GitHub username: %s\n", author_set[i].github)) <= 0 ||
	    ((author_set[i].affiliation[0] == '\0') ? printf("Affiliation not given\n\n") :
						      printf("Affiliation: %s\n\n", author_set[i].affiliation)) <= 0) {
	    errp(170, __func__, "error while printing author #%d information\n", i);
	    not_reached();
	}
	if (need_confirm) {
	    yorn = yes_or_no("Is that author information correct? [yn]");
	    if (!yorn) {
		/*
		 * reenter author information
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
 * verify_entry_dir - ask user to verify the contents of the entry directory
 *
 * given:
 *      entry_dir       - path to entry directory
 *      ls              - path to ls utility
 *
 * This function does not return on error.
 */
static void
verify_entry_dir(char const *entry_dir, char const *ls)
{
    int exit_code;		/* exit code from system(cmd) */
    bool yorn = false;		/* response to a question */
    char *cmd = NULL;		/* cd entry_dir && ls -l . */
    FILE *ls_stream;		/* pipe from iocccsize -V */
    char *linep = NULL;		/* allocated line read from iocccsize */
    ssize_t readline_len;	/* readline return length */
    int kdirsize;		/* number of kibibyte (2^10) blocks in entry directory */
    char guard;			/* scanf guard to catch excess amount of input */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (entry_dir == NULL || ls == NULL) {
	err(171, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * list the contents of the entry_dir
     */
    para("The following is a listing of the entry directory:",
	 "",
	 NULL);
    errno = 0;		/* pre-clear errno for warnp() */
    ret = printf("    %s\n", entry_dir);
    if (ret <= 0) {
	warnp(__func__, "printf error code: %d", ret);
    }
    para("",
	 "from which the xz tarball will be formed:",
	 "",
	 NULL);
    errno = 0;			/* pre-clear errno for errp() */
    cmd = cmdprintf("cd -- % && % -lak .", entry_dir, ls);
    if (cmd == NULL) {
	err(172, __func__, "failed to cmdprintf: cd entry_dir && ls -lak .");
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
	errp(173, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(174, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * execute the ls command
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(175, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(176, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(177, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
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
	errp(178, __func__, "fflush(stdout) #0: error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(179, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * form pipe to the ls command
     */
    errno = 0;			/* pre-clear errno for errp() */
    ls_stream = popen(cmd, "r");
    if (ls_stream == NULL) {
	errp(180, __func__, "popen for reading failed for: %s", cmd);
	not_reached();
    }

    errno = 0;
    ret = setvbuf(ls_stream, (char *)NULL, _IOLBF, 0);
    if (ret != 0) {
	warn(__func__, "setvbuf failed for %s", cmd);
    }

    /*
     * read the 1st line - contains the total kibibyte (2^10) block line
     */
    dbg(DBG_HIGH, "reading 1st line from popen(%s, r)", cmd);
    readline_len = readline(&linep, ls_stream);
    if (readline_len < 0) {
	err(181, __func__, "EOF while reading 1st line from ls: %s", ls);
	not_reached();
    } else {
	dbg(DBG_HIGH, "ls 1st line read length: %ld buffer: %s", (long)readline_len, linep);
    }

    /*
     * parse k-block line from ls
     */
    ret = sscanf(linep, "total %d%c", &kdirsize, &guard);
    if (ret != 1) {
	err(182, __func__, "failed to parse block line from ls: %s", linep);
	not_reached();
    }
    if (kdirsize <= 0) {
	err(183, __func__, "ls k block value: %d <= 0", kdirsize);
	not_reached();
    }
    if (kdirsize > MAX_DIR_KSIZE) {
	fpara(stderr,
	      "",
	      "The entry is too large.",
	      "",
	      NULL);
	errno = 0;			/* pre-clear errno for errp() */
	ret = fprintf(stderr, "The entry directory %s is %d kibibyte (1024 byte blocks) in length.\n"
			      "It must be <= %d kibibyte (1024 byte blocks).\n\n", entry_dir, kdirsize, MAX_DIR_KSIZE);
	if (ret <= 0) {
	    errp(184, __func__, "fprintf error while printing entry directory kibibyte sizes");
	    not_reached();
	}
	err(185, __func__, "The entry directory is too large: %s", entry_dir);
	not_reached();
    }
    dbg(DBG_LOW, "entry directory %s size in kibibyte (1024 byte blocks): %d", entry_dir, kdirsize);

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
     * ask the user to verify the list
     */
    yorn = yes_or_no("\nIs the above list a correct list of files in your entry? [yn]");
    if (!yorn) {
	fpara(stderr,
	      "",
	      "We suggest you remove the existing entry directory, and then",
	      "rerun this tool with the correct set of file arguments.",
	      NULL);
	err(186, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * free storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    if (linep != NULL) {
	free(linep);
	linep = NULL;
    }
    return;
}


/*
 * json_putc - print a character with JSON encoding
 *
 * JSON string encoding JSON string encoding.  We will encode as follows:
 *
 *     old		new
 *     --------------------
 *	"		\"
 *	/		\/
 *	\		\\
 *	<backspace>	\b
 *	<vertical tab>	\f
 *	<tab>		\t
 *	<enter>		\r
 *	<newline>	\n
 *	<		\u003C
 *	>		\u003E
 *	&		\uoo26
 *	%		\u0025
 *	\x80-\xff	\u0080 - \u00ff
 *
 * See:
 *
 *	https://developpaper.com/escape-and-unicode-encoding-in-json-serialization/
 *
 * given:
 *	stream	- string to print on
 *	c	- character to encode
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
static bool
json_putc(int const c, FILE *stream)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * case: \cFFFF encoding
     */
    errno = 0;			/* pre-clear errno for errp() */
    if ((c >= 0x80 && c <= 0xffff) || c == '<' || c == '>' || c == '&' || c == '%') {
	ret = fprintf(stream, "\\c%04X", c);
	if (ret <= 0) {
	    warnp(__func__, "fprintf #0 error in \\cFFFF encoding");
	    return false;
	}
        return true;
    }

    /*
     * case: \ escaped char
     */
    switch (c) {
    case '"':
	ret = fprintf(stream, "\\\"");
	break;
    case '/':
	ret = fprintf(stream, "\\/");
	break;
    case '\\':
	ret = fprintf(stream, "\\\\");
	break;
    case '\b':
	ret = fprintf(stream, "\\b");
	break;
    case '\f':
	ret = fprintf(stream, "\\f");
	break;
    case '\t':
	ret = fprintf(stream, "\\t");
	break;
    case '\r':
	ret = fprintf(stream, "\\r");
	break;
    case '\n':
	ret = fprintf(stream, "\\n");
	break;

    /*
     * case: un-escaped char
     */
    default:
	ret = fprintf(stream, "%c", c);
	break;
    }
    if (ret <= 0) {
	warnp(__func__, "fprintf #1 error");
	return false;
    }
    return true;
}


/*
 * json_fprintf_str - print a JSON string
 *
 * Print on stream:
 *
 * If str == NULL:
 *
 *	null
 *
 * else str != NULL:
 *
 *	str with JSON string encoding surrounded by "'s
 *
 * See:
 *
 *	https://www.json.org/json-en.html
 *
 * given:
 *	stream	- open file stream to print on
 *	str	- the string to JSON encode or NULL
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
static bool
json_fprintf_str(FILE *stream, char const *str)
{
    int ret;			/* libc function return */
    char const *p;

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * case: NULL
     */
    if (str == NULL) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(stream, "null");
	if (ret <= 0) {
	    warnp(__func__, "fprintf #0 error for null");
	    return false;
	}
	return true;
    }

    /*
     * print leading double-quote
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #0 error for leading double-quote");
	return false;
    }

    /*
     * print name, JSON encoded
     */
    for (p=str; *p != '\0'; ++p) {
	if (json_putc(*p, stream) != true) {
	    warnp(__func__, "json_putc #0 error");
	    return false;
	}
    }

    /*
     * print trailing double-quote
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #1 error for trailing double-quote");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_string - print name value (as a string) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded " middle "value_encoded" tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value string to JSON encode or NULL
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
static bool
json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
			  char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warn(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warn(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON encoded string
     */
    if (json_fprintf_str(stream, value) != true) {
	warn(__func__, "json_fprintf_str for value as a string");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warn(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_long - print name value (as a long integer) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle long_value tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as long
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
static bool
json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warnp(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON long
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%ld", value);
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a long");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_bool - print name value (as a boolean) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle true tail
 * or:
 *	lead "name_encoded" middle false tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as boolean
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
static bool
json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON boolean
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", (value) ? "true" : "false");
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a boolean");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * strnull - return NULL if string is empty
 *
 * given:
 *	str (NULL allowed)
 *
 * returns:
 *	str if str is NOT empty,
 *	else NULL
 */
static char const *
strnull(char const * const str)
{
    /*
     * if str is non-NULL and non-zero length, return str
     */
    if (str != NULL && strlen(str) > 0) {
	return str;
    }
    return NULL;
}


/*
 * write_info - create the .info.json file
 *
 * Form a simple JSON .info file describing the entry.
 *
 * given:
 *      infop           - pointer to info structure
 *      entry_dir       - path to entry directory
 *      test_mode       - true ==> test mode, do not upload
 *      jinfochk	- path to jinfochk tool
 *
 * returns:
 *	true
 *
 * This function does not return on error.
 */
static void
write_info(struct info *infop, char const *entry_dir, bool test_mode, char const *jinfochk)
{
    struct tm *timeptr;		/* localtime return */
    char *info_path;		/* path to .info.json file */
    size_t info_path_len;	/* length of path to .info.json */
    FILE *info_stream;		/* open write stream to the .info.json file */
    size_t asctime_len;		/* length of asctime() string without the trailing newline */
    size_t gmtime_len;		/* length of gmtime string (gmtime() + " UTC") */
    int ret;			/* libc function return */
    char **q;			/* extra filename array pointer */
    char *p;
    int i;
    char *cmd = NULL;		/* for jinfochk */
    int exit_code;		/* for system() */

    /*
     * firewall
     */
    if (infop == NULL || entry_dir == NULL || jinfochk == NULL) {
	err(187, __func__, "called with NULL arg(s)");
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
    errno = 0;			/* pre-clear errno for errp() */
    infop->epoch = strdup(TIMESTAMP_EPOCH);
    if (infop->epoch == NULL) {
	errp(188, __func__, "strdup of %s failed", TIMESTAMP_EPOCH);
	not_reached();
    }
    dbg(DBG_VVHIGH, "infop->epoch: %s", infop->epoch);

    /*
     * reset to UTC timezone
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = setenv("TZ", "UTC", 1);
    if (ret < 0) {
	errp(189, __func__, "cannot set TZ=UTC");
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    timeptr = localtime(&(infop->tstamp));
    if (timeptr == NULL) {
	errp(190, __func__, "localtime #1 returned NULL");
	not_reached();
    }
    if (timeptr->tm_zone == NULL) {
	err(191, __func__, "timeptr->tm_zone #1 is NULL");
	not_reached();
    }

    /*
     * ASCII UTC string
     */
    errno = 0;			/* pre-clear errno for errp() */
    p = asctime(timeptr);
    if (p == NULL) {
	errp(192, __func__, "asctime #1 returned NULL");
	not_reached();
    }
    asctime_len = strlen(p) - 1; /* -1 to remove trailing newline */
    gmtime_len = strlen(p) + 1 + LITLEN("UTC") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    infop->gmtime = (char *)calloc(gmtime_len + 1, 1);
    if (infop->gmtime == NULL) {
	errp(193, __func__, "calloc of %lu bytes failed", (unsigned long)gmtime_len + 1);
	not_reached();
    }
    (void) strncat(infop->gmtime, p, asctime_len);
    (void) strcat(infop->gmtime, " UTC");
    dbg(DBG_VVHIGH, "infop->gmtime: %s", infop->gmtime);

    /*
     * open .info.json for writing
     */
    info_path_len = strlen(entry_dir) + 1 + LITLEN(".info.json") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    info_path = (char *)malloc(info_path_len + 1);
    if (info_path == NULL) {
	errp(194, __func__, "malloc of %lu bytes failed", (unsigned long)info_path_len + 1);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(info_path, info_path_len, "%s/.info.json", entry_dir);
    if (ret <= 0) {
	errp(195, __func__, "snprintf #0 error: %d", ret);
	not_reached();
    }
    dbg(DBG_HIGH, ".info.json path: %s", info_path);
    errno = 0;			/* pre-clear errno for errp() */
    info_stream = fopen(info_path, "w");
    if (info_stream == NULL) {
	errp(196, __func__, "failed to open for writing: %s", info_path);
	not_reached();
    }

    /*
     * write leading part of info to the open .info.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(info_stream, "{\n") > 0 &&
	json_fprintf_value_string(info_stream, "\t", "IOCCC_info_version", " : ", INFO_VERSION, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "ioccc_contest", " : ", IOCCC_CONTEST, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "ioccc_year", " : ", (long)IOCCC_YEAR, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "mkiocccentry_version", " : ", infop->mkiocccentry_ver, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "iocccsize_version", " : ", infop->iocccsize_ver, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "IOCCC_contest_id", " : ", infop->ioccc_id, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "entry_num", " : ", (long)infop->entry_num, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "title", " : ", infop->title, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "abstract", " : ", infop->abstract, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "tarball", " : ", infop->tarball_path, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "rule_2a_size", " : ", (long)infop->rule_2a_size, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "rule_2b_size", " : ", (long)infop->rule_2b_size, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "empty_override", " : ", infop->empty_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "rule_2a_override", " : ", infop->rule_2a_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "rule_2a_mismatch", " : ", infop->rule_2a_mismatch, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "rule_2b_override", " : ", infop->rule_2b_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "highbit_warning", " : ", infop->highbit_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "nul_warning", " : ", infop->nul_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "trigraph_warning", " : ", infop->trigraph_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "wordbuf_warning", " : ", infop->wordbuf_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "ungetc_warning", " : ", infop->ungetc_warning, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "Makefile_override", " : ", infop->Makefile_override, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "first_rule_is_all", " : ", infop->first_rule_is_all, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "found_all_rule", " : ", infop->found_all_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "found_clean_rule", " : ", infop->found_clean_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "found_clobber_rule", " : ", infop->found_clobber_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "found_try_rule", " : ", infop->found_try_rule, ",\n") &&
	json_fprintf_value_bool(info_stream, "\t", "test_mode", " : ", test_mode, ",\n") &&
	fprintf(info_stream, "\t\"manifest\" : [\n") > 0;
    if (!ret) {
	errp(197, __func__, "fprintf error writing leading part of info to %s", info_path);
	not_reached();
    }

    /*
     * write mandatory files to the open .info.json file
     */
    ret = json_fprintf_value_string(info_stream, "\t\t{", "info_JSON", " : ", ".info.json", "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "author_JSON", " : ", ".author.json", "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "c_src", " : ", infop->prog_c, "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "Makefile", " : ", infop->Makefile, "},\n") &&
	  json_fprintf_value_string(info_stream, "\t\t{", "remarks", " : ", infop->remarks_md,
				    ((infop->extra_count > 0) ?  "},\n" : "}\n"));
    if (!ret) {
	errp(198, __func__, "fprintf error writing mandatory filename to %s", info_path);
	not_reached();
    }

    /*
     * write extra files to the open .info.json file
     */
    for (i=0, q=infop->extra_file; i < infop->extra_count && *q != NULL; ++i, ++q) {
        if (json_fprintf_value_string(info_stream, "\t\t{", "extra_file", " : ", *q,
				     (((i+1) < infop->extra_count) ? "},\n" : "}\n")) != true) {
	    errp(199, __func__, "fprintf error writing extra filename[%d] to %s", i, info_path);
	    not_reached();
	}
    }

    /*
     * write trailing part of info to the open .info.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(info_stream, "\t],\n") > 0 &&
	json_fprintf_value_long(info_stream, "\t", "formed_timestamp", " : ", (long)infop->tstamp, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "formed_timestamp_usec", " : ", (long)infop->usec, ",\n") &&
	json_fprintf_value_string(info_stream, "\t", "timestamp_epoch", " : ", infop->epoch, ",\n") &&
	json_fprintf_value_long(info_stream, "\t", "min_timestamp", " : ", MIN_TIMESTAMP, "\n") &&
	json_fprintf_value_string(info_stream, "\t", "formed_UTC", " : ", infop->gmtime, "\n") &&
	fprintf(info_stream, "}\n") > 0;
    if (!ret) {
	errp(200, __func__, "fprintf error writing trailing part of info to %s", info_path);
	not_reached();
    }

    /*
     * close the file prior to running jinfochk
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(info_stream);
    if (ret < 0) {
	errp(201, __func__, "fclose error");
	not_reached();
    }
    /*
     * form the jinfochk command
     */
    cmd = cmdprintf("% -q %", jinfochk, info_path);
    if (cmd == NULL) {
	err(202, __func__, "failed to cmdprintf: jinfochk %s", info_path);
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(203, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(204, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * perform the jinfochk which will indirectly show the user the tarball
     * contents
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(205, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(206, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(207, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    para("... all appears well with the .info.json file.", NULL);

    /*
     * free storage
     */
    if (info_path != NULL) {
	free(info_path);
	info_path = NULL;
    }

    free(cmd);
    cmd = NULL;


    return;
}


/*
 * write_author - create the .author.json file
 *
 * Form a simple JSON .author file describing the entry.
 *
 * given:
 *      infop           - pointer to info structure
 *      author_count    - length of the author structure array in elements
 *      authorp         - pointer to author structure array
 *      entry_dir       - path to entry directory
 *      jauthchk	- path to jauthchk tool
 *
 * This function does not return on error.
 */
static void
write_author(struct info *infop, int author_count, struct author *authorp, char const *entry_dir, char const *jauthchk)
{
    char *author_path;		/* path to .author.json file */
    size_t author_path_len;	/* length of path to .author.json */
    FILE *author_stream;	/* open write stream to the .author.json file */
    int ret;			/* libc function return */
    int i;
    int exit_code;		/* exit code from system(cmd) */
    char *cmd;			/* for jauthchk */

    /*
     * firewall
     */
    if (infop == NULL || authorp == NULL || entry_dir == NULL || jauthchk == NULL) {
	err(208, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count <= 0) {
	warn(__func__, "author_count %d <= 0", author_count);
    }

    /*
     * open .author.json for writing
     */
    author_path_len = strlen(entry_dir) + 1 + LITLEN(".author.json") + 1;
    errno = 0;			/* pre-clear errno for errp() */
    author_path = (char *)malloc(author_path_len + 1);
    if (author_path == NULL) {
	errp(209, __func__, "malloc of %lu bytes failed", (unsigned long)author_path_len + 1);
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = snprintf(author_path, author_path_len, "%s/.author.json", entry_dir);
    if (ret <= 0) {
	errp(210, __func__, "snprintf #0 error: %d", ret);
	not_reached();
    }
    dbg(DBG_HIGH, ".author.json path: %s", author_path);
    errno = 0;			/* pre-clear errno for errp() */
    author_stream = fopen(author_path, "w");
    if (author_stream == NULL) {
	errp(211, __func__, "failed to open for writing: %s", author_path);
	not_reached();
    }

    /*
     * write leading part of authorship to the open .author.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(author_stream, "{\n") > 0 &&
	json_fprintf_value_string(author_stream, "\t", "IOCCC_author_version", " : ", AUTHOR_VERSION, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "ioccc_contest", " : ", IOCCC_CONTEST, ",\n") &&
	json_fprintf_value_long(author_stream, "\t", "ioccc_year", " : ", (long)IOCCC_YEAR, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "mkiocccentry_version", " : ", infop->mkiocccentry_ver, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "IOCCC_contest_id", " : ", infop->ioccc_id, ",\n") &&
	json_fprintf_value_long(author_stream, "\t", "entry_num", " : ", (long)infop->entry_num, ",\n") &&
	fprintf(author_stream, "\t\"authors\" : [\n") > 0;
    if (!ret) {
	errp(212, __func__, "fprintf error writing leading part of authorship to %s", author_path);
	not_reached();
    }

    /*
     * write author info to the open .author.json file
     */
    for (i = 0; i < author_count; ++i) {
	errno = 0;		/* pre-clear errno for errp() */
	ret = fprintf(author_stream, "\t\t{\n") > 0 &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "name", " : ", authorp[i].name, ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "location_code", " : ", authorp[i].location_code, ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "location_name", " : ", authorp[i].location_name, ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "email", " : ", strnull(authorp[i].email), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "url", " : ", strnull(authorp[i].url), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "twitter", " : ", strnull(authorp[i].twitter), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "github", " : ", strnull(authorp[i].github), ",\n") &&
	    json_fprintf_value_string(author_stream, "\t\t\t", "affiliation", " : ", strnull(authorp[i].affiliation), ",\n") &&
	    json_fprintf_value_long(author_stream, "\t\t\t", "author_number", " : ", authorp[i].author_num, "\n") &&
	    fprintf(author_stream, "\t\t}%s\n", (((i + 1) < author_count) ? "," : "")) > 0;
	if (ret < 0) {
	    errp(213, __func__, "fprintf error writing author info to %s", author_path);
	    not_reached();
	}
    }

    /*
     * write trailing part of authorship to the open .author.json file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fprintf(author_stream, "\t],\n") > 0 &&
	json_fprintf_value_long(author_stream, "\t", "formed_timestamp", " : ", (long)infop->tstamp, ",\n") &&
	json_fprintf_value_long(author_stream, "\t", "formed_timestamp_usec", " : ", (long)infop->usec, ",\n") &&
	json_fprintf_value_string(author_stream, "\t", "timestamp_epoch", " : ", infop->epoch, ",\n") &&
	json_fprintf_value_long(author_stream, "\t", "min_timestamp", " : ", MIN_TIMESTAMP, "\n") &&
	json_fprintf_value_string(author_stream, "\t", "formed_UTC", " : ", infop->gmtime, "\n") &&
	fprintf(author_stream, "}\n") > 0;
    if (!ret) {
	errp(214, __func__, "fprintf error writing trailing part of authorship to %s", author_path);
	not_reached();
    }

    /*
     * close the file before checking it with jauthchk
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(author_stream);
    if (ret < 0) {
	errp(215, __func__, "fclose error");
	not_reached();
    }

    para("",
	"Checking the format of .author.json ...", NULL);

    /*
     * form the jauthchk command
     */
    cmd = cmdprintf("% -q %", jauthchk, author_path);
    if (cmd == NULL) {
	err(216, __func__, "failed to cmdprintf: jauthchk %s", author_path);
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(217, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(218, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * perform the jauthchk which will indirectly show the user the tarball
     * contents
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(219, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(220, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(221, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    para("... all appears well with the .author.json file.", NULL);


    /*
     * free storage
     */
    if (author_path != NULL) {
	free(author_path);
	author_path = NULL;
    }

    /* free cmd */
    free(cmd);
    cmd = NULL;

    return;
}


/*
 * form_tarball - form the compressed tarball
 *
 * Given the completed entry directory, form a compressed tar file for the user to submit.
 * Remind the user where to submit their compressed tarball file. The function
 * shows the listing of the tarball contents via the txzchk tool and the fnamchk tool.
 *
 * given:
 *      work_dir        - working directory under which the entry directory is formed
 *      entry_dir       - path to entry directory
 *      tarball_path    - path of the compressed tarball to form
 *      tar             - path to the tar utility
 *      ls              - path to ls utility
 *      txzchk		- path to txzchk tool
 *      fnamchk		- path to fnamchk tool
 *
 * This function does not return on error.
 */
static void
form_tarball(char const *work_dir, char const *entry_dir, char const *tarball_path, char const *tar,
	     char const *ls, char const *txzchk, char const *fnamchk)
{
    char *basename_entry_dir;	/* basename of the entry directory */
    char *basename_tarball_path;/* basename of tarball_path */
    char *cmd;			/* the tar command to form the compressed tarball */
    int exit_code;		/* exit code from system(cmd) */
    struct stat buf;		/* stat of the tarball */
    int ret;			/* libc function return */
    int cwd;			/* current working directory */

    /*
     * firewall
     */
    if (work_dir == NULL || entry_dir == NULL || tarball_path == NULL || tar == NULL || ls == NULL ||
        txzchk == NULL || fnamchk == NULL) {
	err(222, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * verify entry directory contents
     */
    verify_entry_dir(entry_dir, ls);
    dbg(DBG_LOW, "verified entry directory: %s", entry_dir);

    /*
     * note the current directory so we can restore it later, after the chdir(work_dir) below
     */
    errno = 0;			/* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
	errp(223, __func__, "cannot open .");
	not_reached();
    }

    /*
     * cd into the work_dir, just above the entry_dir and where the compressed tarball will be formed
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = chdir(work_dir);
    if (ret < 0) {
	errp(224, __func__, "cannot cd %s", work_dir);
	not_reached();
    }

    /*
     * form the tar create command
     *
     * IMPORTANT NOTE: The reason why we form a v7 tarball is that we do NOT want to
     *		       preserve user and group names (to help keep authors anonymous),
     *		       (modern flags to force a username/groupname are not very portable),
     *		       and we don't want special files, symlinks, etc.
     */
    basename_entry_dir = base_name(entry_dir);
    basename_tarball_path = base_name(tarball_path);
    cmd = cmdprintf("% --format=v7 -cJf % %", tar, basename_tarball_path, basename_entry_dir);
    if (cmd == NULL) {
	err(225, __func__, "failed to cmdprintf: tar --format=v7 -cJf tarball_path entry_dir");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(226, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(227, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * perform the tar create command
     */
    para("",
	 "About to run the tar command to form the compressed tarball ...",
	 "",
	 NULL);
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(228, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(229, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(230, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }

    /*
     * enforce the maximum size of the compressed tarball
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = stat(basename_tarball_path, &buf);
    if (ret != 0) {
	errp(231, __func__, "stat of the compressed tarball failed: %s", basename_tarball_path);
	not_reached();
    }
    if (buf.st_size > MAX_TARBALL_LEN) {
	fpara(stderr,
	      "",
	      "The compressed tarball exceeds the maximum allowed size, sorry.",
	      "",
	      NULL);
	err(232, __func__, "The compressed tarball: %s size: %lu > %ld",
		 basename_tarball_path, (unsigned long)buf.st_size, (long)MAX_TARBALL_LEN);
	not_reached();
    }

    /*
     * free command for next use
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }

    /*
     * switch back to the previous current directory
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fchdir(cwd);
    if (ret < 0) {
	errp(233, __func__, "cannot fchdir to the previous current directory");
	not_reached();
    }
    errno = 0;			/* pre-clear errno for errp() */
    ret = close(cwd);
    if (ret < 0) {
	errp(234, __func__, "close of previous current directory failed");
	not_reached();
    }

    /*
     * form the txzchk command
     */
    cmd = cmdprintf("% -q -F % %/../%", txzchk, fnamchk, entry_dir, basename_tarball_path);
    if (cmd == NULL) {
	err(235, __func__, "failed to cmdprintf: txzchk -q tarball_path");
	not_reached();
    }
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);

    /*
     * pre-flush to avoid system() buffered stdio issues
     */
    clearerr(stdout);	/* pre-clear ferror() status */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	errp(236, __func__, "fflush(stdout) error code: %d", ret);
	not_reached();
    }
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	errp(237, __func__, "fflush(stderr) #1: error code: %d", ret);
	not_reached();
    }

    /*
     * perform the txzchk which will indirectly show the user the tarball
     * contents
     */
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	errp(238, __func__, "error calling system(%s)", cmd);
	not_reached();
    } else if (exit_code == 127) {
	errp(239, __func__, "execution of the shell failed for system(%s)", cmd);
	not_reached();
    } else if (exit_code != 0) {
	err(240, __func__, "%s failed with exit code: %d", cmd, WEXITSTATUS(exit_code));
	not_reached();
    }
    para("",
	 "... the output above is the listing of the compressed tarball.",
	 "",
	 NULL);

    /*
     * free memory
     */
    if (basename_entry_dir != NULL) {
	free(basename_entry_dir);
	basename_entry_dir = NULL;
    }
    if (basename_tarball_path != NULL) {
	free(basename_tarball_path);
	basename_tarball_path = NULL;
    }
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }
    return;
}


/*
 * remind_user - remind the user to upload (if not in test mode)
 *
 * given:
 *      work_dir        - working directory under which the entry directory is formed
 *      entry_dir       - path to entry directory
 *      tar             - path to the tar utility
 *      tarball_path    - path of the compressed tarball to form
 *      test_mode       - true ==> test mode, do not upload
 *      answers		- path to the answers file (if specified)
 *      infop		- pointer to info structure
 */
static void
remind_user(char const *work_dir, char const *entry_dir, char const *tar, char const *tarball_path, bool test_mode)
{
    int ret;			/* libc function return */
    char *entry_dir_esc;
    char *work_dir_esc;

    /*
     * firewall
     */
    if (work_dir == NULL || entry_dir == NULL || tar == NULL || tarball_path == NULL) {
	err(241, __func__, "called with NULL arg(s)");
	not_reached();
    }

    entry_dir_esc = cmdprintf("%", entry_dir);
    if (entry_dir_esc == NULL) {
	err(242, __func__, "failed to cmdprintf: entry_dir");
	not_reached();
    }

    /*
     * tell user they can now remove entry_dir
     */
    para("Now that we have formed the compressed tarball file, you",
	 "can remove the entry directory we have formed by executing:",
	 "",
	 NULL);
    ret = printf("    rm -rf %s%s\n", entry_dir[0] == '-' ? "-- " : "", entry_dir_esc);
    if (ret <= 0) {
	errp(243, __func__, "printf #0 error");
	not_reached();
    }
    free(entry_dir_esc);

    work_dir_esc = cmdprintf("%", work_dir);
    if (work_dir_esc == NULL) {
	err(244, __func__, "failed to cmdprintf: work_dir");
	not_reached();
    }

    para("",
	 "If you are curious, you may examine the newly created compressed tarball",
	 "by running the following command:",
	 "",
	 NULL);
    ret = printf("    %s -Jtvf %s%s/%s\n", tar, work_dir[0] == '-' ? "./" : "", work_dir_esc, tarball_path);
    if (ret <= 0) {
	errp(245, __func__, "printf #2 error");
	not_reached();
    }
    free(work_dir_esc);

    /*
     * case: test mode
     */
    if (test_mode) {

	/*
	 * remind them that this is a test entry, not an official entry
	 */
	para("",
	     "As you entered an IOCCC contest ID of test, the compressed tarball",
	     "that was just formed CANNOT be used as an IOCCC entry.",
	     "",
	     NULL);

    /*
     * case: entry mode
     */
    } else {

	/*
	 * inform them of the compressed tarball file
	 */
	para("",
	     "Assuming that the IOCCC is still open, you may submit your entry",
	     "by uploading following compressed tarball file:",
	     "",
	     NULL);
	ret = printf("    %s/%s\n", work_dir, tarball_path);
	if (ret <= 0) {
	    errp(246, __func__, "printf #2 error");
	    not_reached();
	}
    }

    /*
     * case: test mode report
     */
    if (!test_mode) {
	para("",
	     "If the contest is still open, you may upload the above",
	     "tarball to the following submission URL:",
	     "",
	     NULL);
	ret = printf("    %s\n\n", IOCCC_SUBMIT_URL);
	if (ret < 0) {
	    errp(247, __func__, "printf #4 error");
	    not_reached();
	}
    }
    return;
}
