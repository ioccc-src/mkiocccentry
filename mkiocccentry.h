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


#ifndef MKIOCCCENTRY_H
#define MKIOCCCENTRY_H

#ifdef MKIOCCCENTRY_C

/*
 * util - utility functions and variable types (like bool)
 */
#include "util.h"

/*
 * JSON - JSON structures and functions
 */
#include "json.h"

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


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
const char * const usage_msg0 =
    "usage: %s [options] work_dir prog.c Makefile remarks.md [file ...]\n"
    "\noptions:\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\t-W\t\t\tignore all warnings (this does NOT mean the judges will! :) )\n";

const char * const usage_msg1 =
    "\t-t /path/to/tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-c /path/to/cp\t\tpath to cp executable (def: %s)\n"
    "\t-l /path/to/ls\t\tpath to ls executable (def: %s)\n"
    "\t-C /path/to/txzchk\tpath to txzchk executable (def: %s)\n"
    "\t-F /path/to/fnamchk\tpath to fnamchk executable used by txzchk (def: %s)";
const char * const usage_msg2 =
    "\t-j /path/to/jinfochk	path to jinfochk executable used by txzchk (def: %s)\n"
    "\t-J /path/to/jauthchk	path to jauthchk executable used by txzchk (def: %s)\n";
const char * const usage_msg3 =
    "\t-a answers\t\twrite answers to a file for easier updating of an entry\n"
    "\t-A answers\t\twrite answers file even if it already exists\n"
    "\t-i answers\t\tread answers from file previously written by -a|-A answers\n\n"
    "\t    NOTE: One cannot use both -a/-A answers and -i answers at the same time.\n"
    "\n"
    "\twork_dir\tdirectory where the entry directory and tarball are formed\n"
    "\tprog.c\t\tpath to the C source for your entry\n";
const char * const usage_msg4 =
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
bool need_confirm = true;	/* true ==> ask for confirmations */
bool need_hints = true;		/* true ==> show hints */
bool need_retry = true;
bool ignore_warnings = false;	/* true ==> ignore all warnings (this does NOT mean the judges will! :) */
FILE *input_stream = NULL;
struct iocccsize size;	/* rule_count() processing results */


/*
 * forward declarations
 */
void usage(int exitcode, char const *str, char const *program, char const *tar, char const *cp, char const *ls,
		  char const *txzchk, char const *fnamchk, char const *jinfochk, char const *jauthchk);
void free_info(struct info *infop);
void free_author_array(struct author *authorp, int author_count);
void warn_empty_prog(char const *prog_c);
void warn_rule2a_size(struct info *infop, char const *prog_c, int mode);
void warn_high_bit(char const *prog_c);
void warn_nul_chars(char const *prog_c);
void warn_trigraph(char const *prog_c);
void warn_wordbuf(char const *prog_c);
void warn_ungetc(char const *prog_c);
void warn_rule2b_size(struct info *infop, char const *prog_c);
void check_prog_c(struct info *infop, char const *entry_dir, char const *cp, char const *prog_c);
void sanity_chk(struct info *infop, char const *work_dir, char const *tar, char const *cp,
		       char const *ls, char const *txzchk, char const *fnamchk, char const *jinfochk, char const *jauthchk);
char *prompt(char const *str, size_t *lenp);
char *get_contest_id(struct info *infop, bool *testp, bool *read_answers_flag_used);
int get_entry_num(struct info *infop);
char *mk_entry_dir(char const *work_dir, char const *ioccc_id, int entry_num, char **tarball_path, time_t tstamp);
bool inspect_Makefile(char const *Makefile, struct info *infop);
void warn_Makefile(char const *Makefile, struct info *infop);
void check_Makefile(struct info *infop, char const *entry_dir, char const *cp, char const *Makefile);
void check_remarks_md(struct info *infop, char const *entry_dir, char const *cp, char const *remarks_md);
void check_extra_data_files(struct info *infop, char const *entry_dir, char const *cp, int count, char **args);
char const *lookup_location_name(char const *upper_code);
bool yes_or_no(char const *question);
char *get_title(struct info *infop);
char *get_abstract(struct info *infop);
int get_author_info(struct info *infop, char *ioccc_id, struct author **author_set);
void verify_entry_dir(char const *entry_dir, char const *ls);
bool json_fprintf_str(FILE *stream, char const *str);
bool json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
				      char const *tail);
bool json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
				    char const *tail);
bool json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
				    char const *tail);
void write_info(struct info *infop, char const *entry_dir, bool test_mode, char const *jinfochk);
void write_author(struct info *infop, int author_count, struct author *authorp, char const *entry_dir, char const *jauthchk);
void form_tarball(char const *work_dir, char const *entry_dir, char const *tarball_path, char const *tar,
			 char const *ls, char const *txzchk, char const *fnamchk);
void remind_user(char const *work_dir, char const *entry_dir, char const *tar, char const *tarball_path, bool test_mode);



#endif /* MKIOCCCENTRY_C */
#endif /* MKIOCCCENTRY_H */
