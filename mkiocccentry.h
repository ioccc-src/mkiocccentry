/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * mkiocccentry - make an ioccc entry
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla C.
 * Why?  Because this is an obfuscated C contest.  But then why isn't
 * this code obfuscated?  Because the IOCCC judges prefer to write
 * in robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell source and finger daemon
 * source.  Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, please let the judges know.
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


#if !defined(INCLUDE_MKIOCCCENTRY_H)
#    define  INCLUDE_MKIOCCCENTRY_H


/*
 * util - utility functions and variable types (like bool)
 */
#include "util.h"

/*
 * JSON - JSON structures and functions
 */
#include "json_entry.h"

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * location table
 */
#include "location.h"

/*
 * UTF-8 -> POSIX map
 */
#include "utf8_posix_map.h"

/*
 * IOCCC tables sanity checkers
 */
#include "sanity.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * iocccsize use
 */
#include "iocccsize.h"

/*
 * official versions
 */
#include "version.h"

/*
 * definitions
 *
 * NOTE: MKIOCCCENTRY_ANSWERS_VERSION and MKIOCCCENTRY_ANSWERS_EOF must be defined
 *	 in mkiocccentry.c and not here.
 */
#define REQUIRED_ARGS (4)	/* number of required arguments on the command line */
#define ISO_3166_1_CODE_URL0 "https://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements"
#define ISO_3166_1_CODE_URL1 "https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2"
#define ISO_3166_1_CODE_URL2 "https://www.iso.org/obp/ui/#iso:pub:PUB500001:en"
#define ISO_3166_1_CODE_URL3 "https://www.iso.org/obp/ui/#search"
#define ISO_3166_1_CODE_URL4 "https://www.iso.org/glossary-for-iso-3166.html"
#define IOCCC_REGISTER_URL "https://register.ioccc.org/NOT/a/real/URL"	/* XXX - change to real URL when ready */
#define IOCCC_SUBMIT_URL "https://submit.ioccc.org/NOT/a/real/URL"	/* XXX - change to real URL when ready */
/* NOTE: The next two are for the warn_rule_2a_size() function. Do **NOT** change these values! */
#define RULE_2A_BIG_FILE_WARNING (0)	/* warn that prog.c appears to be too big under Rule 2a */
#define RULE_2A_IOCCCSIZE_MISMATCH (1)	/* warn that prog.c iocccsize size differs from the file size */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg0 =
    "usage: %s [options] work_dir prog.c Makefile remarks.md [file ...]\n"
    "\noptions:\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level: (def level: %d)\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-W\t\tignore all warnings (this does NOT mean the judges will! :) )\n";
static const char * const usage_msg1 =
    "\t-t tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-c cp\t\tpath to cp executable (def: %s)\n"
    "\t-l ls\t\tpath to ls executable (def: %s)\n"
    "\t-T txzchk\tpath to txzchk executable (def: %s)\n"
    "\t-F fnamchk\tpath to fnamchk executable used by txzchk (def: %s)";
static const char * const usage_msg2 =
    "\t-j jinfochk	path to jinfochk executable used by txzchk (def: %s)\n"
    "\t-J jauthchk	path to jauthchk executable used by txzchk (def: %s)\n";
static const char * const usage_msg3 =
    "\t-a answers\twrite answers to a file for easier updating of an entry\n"
    "\t-A answers\twrite answers file even if it already exists\n"
    "\t-i answers\tread answers from file previously written by -a|-A answers\n\n"
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
bool quiet = false;			/* true ==> quiet mode */
/**/
static bool need_confirm = true;	/* true ==> ask for confirmations */
static bool need_hints = true;		/* true ==> show hints */
static bool ignore_warnings = false;	/* true ==> ignore all warnings (this does NOT mean the judges will! :) */
static FILE *input_stream = NULL;	/* input file: stdin or answers file  */
static unsigned answers_errors;		/* > 0 ==> output errors on answers file */


/*
 * forward declarations
 */
static void usage(int exitcode, char const *str, char const *program);
static void warn_empty_prog(char const *prog_c);
static void warn_rule_2a_size(struct info *infop, char const *prog_c, int mode, RuleCount size);
static void warn_high_bit(char const *prog_c);
static void warn_nul_chars(char const *prog_c);
static void warn_trigraph(char const *prog_c);
static void warn_wordbuf(char const *prog_c);
static void warn_ungetc(char const *prog_c);
static void warn_rule_2b_size(struct info *infop, char const *prog_c);
static RuleCount check_prog_c(struct info *infop, char const *entry_dir, char const *cp, char const *prog_c);
static void mkiocccentry_sanity_chks(struct info *infop, char const *work_dir, char const *tar, char const *cp,
		       char const *ls, char const *txzchk, char const *fnamchk, char const *jinfochk, char const *jauthchk);
static char *prompt(char const *str, size_t *lenp);
static char *get_contest_id(bool *testp, bool *read_answers_flag_used);
static int get_entry_num(struct info *infop);
static char *mk_entry_dir(char const *work_dir, char const *ioccc_id, int entry_num, char **tarball_path, time_t tstamp);
static bool inspect_Makefile(char const *Makefile, struct info *infop);
static void warn_Makefile(char const *Makefile, struct info *infop);
static void check_Makefile(struct info *infop, char const *entry_dir, char const *cp, char const *Makefile);
static void check_remarks_md(struct info *infop, char const *entry_dir, char const *cp, char const *remarks_md);
static void check_extra_data_files(struct info *infop, char const *entry_dir, char const *cp, int count, char **args);
static bool yes_or_no(char const *question);
static char *get_title(struct info *infop);
static char *get_abstract(struct info *infop);
static int get_author_info(struct info *infop, char *ioccc_id, struct author **author_set);
static void verify_entry_dir(char const *entry_dir, char const *ls);
static void write_info(struct info *infop, char const *entry_dir, char const *jinfochk, char const *fnamchk);
static void write_author(struct info *infop, int author_count, struct author *authorp, char const *entry_dir, char const *jauthchk);
static void form_tarball(char const *work_dir, char const *entry_dir, char const *tarball_path, char const *tar,
			 char const *ls, char const *txzchk, char const *fnamchk);
static void remind_user(char const *work_dir, char const *entry_dir, char const *tar, char const *tarball_path, bool test_mode);


#endif /* INCLUDE_MKIOCCCENTRY_H */
