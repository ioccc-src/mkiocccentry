/*
 * entry_util - utilities supporting mkiocccentry JSON files
 *
 * JSON related functions to support formation of .info.json files
 * and .auth.json files, their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * As per IOCCC anonymous judging policy, the calls to json_dbg() in this file
 * that are JSON_DBG_MED or lower will NOT reveal any JSON content.
 * Only at JSON_DBG_HIGH or higher should json_dbg() calls in the file
 * print JSON content.
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.
 * All Rights Reserved.
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
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This tool and the JSON parser were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_ENTRY_UTIL_H)
#    define  INCLUDE_ENTRY_UTIL_H


/*
 * jparse - the parser
 */
#include "../jparse/jparse.h"

/*
 * walk - walk directory trees and tar listings
 */
#include "walk.h"


/*
 * defines
 */

/*
 * filenames in the top directory where write_json_files() sets the manifest member name
 *
 * NOTE: The write_json_files() matches fts_name (basename of canonicalized path) exactly.
 */
#define AUTH_JSON_FILENAME ".auth.json"         /* filename of the .auth.json file */
#define INFO_JSON_FILENAME ".info.json"         /* filename of the .info.json file */
#define PROG_C_FILENAME "prog.c"                /* submission/winning entry source code filename */
#define MAKEFILE_FILENAME "Makefile"            /* submission/winning entry Makefile filename */
#define REMARKS_FILENAME "remarks.md"           /* remarks filename that form README.md for winning entries */
#define PROG_ALT_FILENAME "prog.alt"            /* compiled submission/winning entry alt code filename */
#define PROG_ORIG_FILENAME "prog.orig"          /* compiled winning entry code filename */
#define PROG_ORIG_C_FILENAME "prog.orig.c"      /* entry source code filename */
#define README_MD_FILENAME "README.md"          /* README.md file that forms index.html for winning entries */
#define PROG_ALT_C "prog.alt.c"                 /* alt code source file */
#define TRY_SH "try.sh"                         /* try.sh for prog.c */
#define TRY_ALT_SH "try.alt.sh"                 /* try.alt.sh for prog.alt.c */

/*
 * FUTURE_CLOCK_SKEW_LIMIT - how far in the future is "too far" for a current timestamp
 *
 * Fun world timezone fact (as of 2025):
 *
 * For 50 hours, there exists a timezone where a given calendar date is the current day.
 * It starts when the UTC+14 (Line Islands (LINT)) timezone starts the given calendar day.
 * Then you have a full 24 hours for the given calendar day.
 * It ends when the UTC-12 (Baker Island (BIT)) timezone ends the given calendar day.
 *
 * So how does the this relate to FUTURE_CLOCK_SKEW_LIMIT?  We need an excuse for clock error,
 * and this 50 hour concept is good enough.
 *
 * The FUTURE_CLOCK_SKEW_LIMIT value is used by test_formed_timestamp() to test if a formed
 * timestamp is up to FUTURE_CLOCK_SKEW_LIMIT seconds in the future.
 */
#define FUTURE_CLOCK_SKEW_LIMIT (50*60*60)	/* maximum seconds allowed for a clock to be in error */

/*
 * submissions are not allowed to have any dot file other than the required
 * .auth.json and .info.json. However we do need this for chkentry(1) as it does
 * care about .entry.json too. Given that mkiocccentry(1) and txzchk(1) will
 * both reject a submission with the file .entry.json and given that chkentry(1)
 * will need to work with .entry.json we do not have it in the forbidden list.
 * Nonetheless we do need the macro.
 */
#define ENTRY_JSON_FILENAME ".entry.json"

extern char *mandatory_filenames[];             /* filenames that MUST exist in the top level directory */
extern char *executable_filenames[];            /* filenames that should have mode 0555 */
extern bool ignore_permissions;                 /* true ==> ignore permissions of files and directories */


/*
 * IOCCC author information
 *
 * Information we will collect for each author of an IOCCC entry to fill out the
 * authors JSON array within the .auth.json file.
 *
 * NOTE: While location_name is collected for user prompting and informational purposes,
 *	 it is NO longer written to the .auth.json file.
 */
struct author
{
    char *name;			/* name of the author */
    char *location_code;	/* author location/country code */
    char const *location_name;	/* name of author location/country (compiled in from loc[]) */
    char const *common_name;	/* common name of author location/country (compiled in from loc[]) */
    char *email;		/* Email address of author or empty string ==> not provided */
    char *url;			/* home URL of author or empty string ==> not provided */
    char *alt_url;		/* alt URL of author or empty string ==> not provided */
    char *mastodon;		/* author mastodon handle or empty string ==> not provided */
    char *github;		/* author GitHub username or empty string ==> not provided */
    char *affiliation;		/* author affiliation or empty string ==> not provided */
    bool past_winning_author;	/* true ==> author claims to have won before, false ==> author claims they haven't won before */
    bool default_handle;	/* true ==> default author_handle accepted, false ==> author_handle entered */
    char *author_handle;	/* IOCCC author handle (for winning entries) */
    int author_num;		/* author number */
};

/*
 * IOCCC .auth.json information
 *
 * Information we will collect in order to form the .auth.json file.
 */
struct auth
{
    /*
     * .auth.json information before the authors array
     */
    /* file format strings */
    char const *no_comment;	/* mandatory comment: because comments were removed from the original JSON spec :-) */
    char const *auth_version;	/* IOCCC .auth.json format version (compiled in AUTH_VERSION) */
    char const *IOCCC_contest;	/* IOCCC contest string (compiled in IOCCC[0-9][0-9] or IOCCCMOCK) */
    /* contest year */
    int year;			/* IOCCC year */
    /* IOCCC tool versions */
    char const *mkiocccentry_ver; /* mkiocccentry version (compiled in MKIOCCCENTRY_VERSION) */
    char const *chksubmit_ver;	/* chksubmit version (compiled in, same as chksubmit -V) */
    char const *fnamchk_ver;	/* fnamchk version (compiled in, same as fnamchk -V) */
    char const *iocccsize_ver;	/* iocccsize version (compiled in, same as iocccsize -V) */
    char const *txzchk_ver;	/* txzchk version (compiled in, same as txzchk -V) */
    /* entry information */
    char *ioccc_id;		/* IOCCC contest ID */
    int submit_slot;		/* IOCCC entry number */
    char *tarball;		/* tarball filename */
    /* test or non-test mode */
    bool test_mode;		/* true ==> test mode entered */

    /*
     * author set
     */
    int author_count;		/* number of authors - length of author array */
    struct author *author;	/* set of authors for this entry */

    /*
     * .auth.json information after the authors array
     */
    time_t tstamp;		/* seconds since epoch when .info json was formed (see gettimeofday(2)) */
    int usec;			/* microseconds since the tstamp second */
    char const *epoch;		/* epoch of tstamp, currently: Thu Jan 1 00:00:00 1970 UTC */
    time_t min_stamp;		/* minimum seconds since epoch value of tstamp */
    char *utctime;		/* UTC converted string for tstamp (see strftime(3)) */
};

/*
 * IOCCC .info.json information
 *
 * Information we will collect in order to form the .info.json file.
 */
struct info
{
    /*
     * .info.json information before the file name array
     */
    /* file format strings */
    char const *no_comment;	/* mandatory comment: because comments were removed from the original JSON spec :-) */
    char const *info_version;	/* IOCCC .info.json format version (compiled in INFO_VERSION) */
    char const *IOCCC_contest;	/* IOCCC contest string (compiled in IOCCC[0-9][0-9] or IOCCCMOCK) */
    /* contest year */
    int year;			/* IOCCC year */
    /* IOCCC tool versions */
    char const *mkiocccentry_ver; /* mkiocccentry version (compiled in MKIOCCCENTRY_VERSION) */
    char const *chksubmit_ver;	/* chksubmit version (compiled in, same as chksubmit -V) */
    char const *fnamchk_ver;	/* fnamchk version (compiled in, same as fnamchk -V) */
    char const *iocccsize_ver;	/* iocccsize version (compiled in, same as iocccsize -V) */
    char const *txzchk_ver;	/* txzchk version (compiled in, same as txzchk -V) */
    /* entry information */
    char *ioccc_id;		/* IOCCC contest ID */
    int submit_slot;		/* IOCCC entry number */
    char *title;		/* entry title */
    char *abstract;		/* entry abstract */
    char *tarball;		/* tarball filename */
    /* entry rule 2 sizes */
    size_t rule_2a_size;	/* Rule 2a size of prog.c */
    size_t rule_2b_size;	/* Rule 2b size of prog.c */
    /* entry boolean values */
    bool empty_override;	/* true ==> empty prog.c override requested */
    bool rule_2a_override;	/* true ==> Rule 2a override requested */
    bool rule_2a_mismatch;	/* true ==> file size != rule_count function size */
    bool rule_2b_override;	/* true ==> Rule 2b override requested */
    bool trigraph_warning;	/* true ==> unknown or invalid trigraph(s) detected */
    bool wordbuf_warning;	/* true ==> word buffer overflow detected */
    bool ungetc_warning;	/* true ==> ungetc warning detected */
    bool Makefile_override;	/* true ==> Makefile rule override requested */
    bool found_all_rule;	/* true ==> Makefile has an all rule */
    bool found_clean_rule;	/* true ==> Makefile has clean rule */
    bool found_clobber_rule;	/* true ==> Makefile has a clobber rule */
    /* test or non-test mode */
    bool test_mode;		/* true ==> test mode entered */

    /*
     * JSON stuff
     */
    char const *info_file;	/* .info.json filename */
    char const *auth_file;	/* .auth.json filename */

    /*
     * .info.json information after the file name array
     */
    time_t tstamp;		/* seconds since epoch when .info json was formed (see gettimeofday(2)) */
    int usec;			/* microseconds since the tstamp second */
    char const *epoch;		/* epoch of tstamp, currently: Thu Jan 1 00:00:00 1970 UTC */
    time_t min_stamp;		/* minimum seconds since epoch value of tstamp */
    char *utctime;		/* UTC converted string for tstamp (see strftime(3)) */
};

/*
 * IOCCC manifest - information on the file IOCCC manifest for an entry
 */
struct manifest
{
    intmax_t count_info_JSON;	/* count of info_JSON JSON member found (will be ".info.json") */
    intmax_t count_auth_JSON;	/* count of auth_JSON JSON member found (will be ".auth.json") */
    intmax_t count_c_src;	/* count of c_src JSON member found (will be "prog.c") */
    intmax_t count_Makefile;	/* count of Makefile JSON member found (will be "Makefile") */
    intmax_t count_remarks;	/* count of remarks JSON member found (will be "remarks") */
    intmax_t count_c_alt_src;	/* count of c_alt_src JSON member found (will be "c_alt_src") */
    intmax_t count_try_sh;	/* count of try_sh JSON member found (will be "try_sh") */
    intmax_t count_try_alt_sh;	/* count of try_alt_sh JSON member found (will be "remarks") */
    intmax_t count_shell_script;/* count of shell scripts (not try.sh nor try.alt.sh) JSON member found (will be "remarks") */
    intmax_t count_extra_file;	/* count of extra JSON members found */
    struct dyn_array *extra;	/* dynamic array of extra JSON member filenames (char *) */
};


/*
 * external function declarations
 */
extern void free_auth(struct auth *authp);
extern void free_info(struct info *infop);
extern void free_author_array(struct author *authorp, int author_count);
extern void free_manifest(struct manifest *manp);
extern bool valid_contest_id(char *str);
extern bool object2author(struct json *node, unsigned int depth, struct json_sem *sem,
			  char const *name, struct json_sem_val_err **val_err,
			  struct author *auth, int author_num);
extern bool object2manifest(struct json *node, unsigned int depth, struct json_sem *sem,
			    char const *name, struct json_sem_val_err **val_err,
			    struct manifest *manp);
extern char *form_tar_filename(char const *IOCCC_contest_id, int submit_slot, bool test_mode,
			       time_t formed_timestamp);

extern bool test_version(char const *str, char const *min);
extern bool test_poison(char const *str,  char const **poisons);
extern bool test_IOCCC_auth_version(char const *str);
extern bool test_IOCCC_contest_id(char const *str);
extern bool test_IOCCC_info_version(char const *str);
extern bool test_Makefile(char const *str);
extern bool test_Makefile_override(bool boolean);
extern bool test_abstract(char const *str);
extern bool test_affiliation(char const *str);
extern bool test_auth_JSON(char const *str);
extern bool test_author_count(int author_count);
extern bool test_author_handle(char const *str);
extern bool test_author_number(int author_number);
extern bool test_authors(int author_count, struct author const *authorp);
extern bool test_c_src(char const *str);
extern bool test_chksubmit_version(char const *str);
extern bool test_default_handle(bool boolean);
extern bool test_email(char const *str);
extern bool test_empty_override(bool boolean);
extern bool test_submit_slot(int submit_slot);
extern bool test_extra_filename(char const *str);
extern bool test_filename_len(char const *str);
extern bool test_fnamchk_version(char const *str);
extern bool test_formed_timestamp(time_t tstamp);
extern bool test_formed_timestamp_usec(int formed_timestamp_usec);
extern bool test_found_all_rule(bool boolean);
extern bool test_found_clean_rule(bool boolean);
extern bool test_found_clobber_rule(bool boolean);
extern bool test_github(char const *str);
extern bool test_info_JSON(char const *str);
extern bool test_IOCCC_contest(char const *str);
extern bool test_IOCCC_year(int IOCCC_year);
extern bool test_iocccsize_version(char const *str);
extern bool test_location_code(char const *str);
extern bool test_manifest(struct manifest *manp, char *submission_dir);
extern bool test_min_timestamp(time_t tstamp);
extern bool test_mkiocccentry_version(char const *str);
extern bool test_name(char const *str);
extern bool test_no_comment(char const *str);
extern bool test_past_winning_author(bool boolean);
extern bool test_remarks(char const *str);
extern bool test_c_alt_src(char const *str);
extern bool test_try_sh(char const *str);
extern bool test_shell_script(char const *str);
extern bool test_try_alt_sh(char const *str);
extern bool test_rule_2a_mismatch(bool boolean);
extern bool test_rule_2a_override(bool boolean);
extern bool test_rule_2a_size(size_t rule_2a_size);
extern bool test_rule_2b_override(bool boolean);
extern bool test_rule_2b_size(size_t rule_2b_size);
extern bool test_tarball(char const *str, char const *IOCCC_contest_id, int submit_slot, bool test_mode,
			 time_t formed_timestamp);
extern bool test_test_mode(bool boolean);
extern bool test_timestamp_epoch(char const *str);
extern bool test_title(char const *str);
extern bool test_trigraph_warning(bool boolean);
extern bool test_mastodon(char const *str);
extern bool test_txzchk_version(char const *str);
extern bool test_ungetc_warning(bool boolean);
extern bool test_url(char const *str);
extern bool test_alt_url(char const *str);
extern bool is_mandatory_filename(char const *str);
extern bool is_executable_filename(char const *str);
extern bool has_ignored_dirname(char const *path);
extern size_t count_char(char const *str, int ch);


#endif /* INCLUDE_ENTRY_UTIL_H */
