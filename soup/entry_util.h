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
 * defines
 */
/*
 * mandatory filenames in the top directory
 */
#define AUTH_JSON_FILENAME ".auth.json"         /* filename of the .auth.json file */
#define INFO_JSON_FILENAME ".info.json"         /* filename of the .info.json file */
#define PROG_C_FILENAME "prog.c"                /* submission/winning entry source code filename */
#define MAKEFILE_FILENAME "Makefile"            /* submission/winning entry Makefile filename */
#define REMARKS_FILENAME "remarks.md"           /* remarks filename that form README.md for winning entries */
/*
 * forbidden filenames in the top directory
 */
#define GNUMAKEFILE_FILENAME "GNUmakefile"      /* GNUmakefile takes priority over Makefile which we don't want */
#define INDEX_HTML_FILENAME "index.html"        /* winning entry index.html filename */
#define PROG_FILENAME "prog"                    /* compiled submission/winning entry code filename */
#define PROG_ALT_FILENAME "prog.alt"            /* compiled submission/winning entry alt code filename */
#define PROG_ORIG_FILENAME "prog.orig"          /* compiled winning entry code filename */
#define PROG_ORIG_C_FILENAME "prog.orig.c"      /* entry source code filename */
#define README_MD_FILENAME "README.md"          /* README.md file that forms index.html for winning entries */

/*
 * submissions are not allowed to have any dot file other than the required
 * .auth.json and .info.json. However we do need this for chkentry(1) as it does
 * care about .entry.json too. Given that mkiocccentry(1) and txzchk(1) will
 * both reject a submission with the file .entry.json and given that chkentry(1)
 * will need to work with .entry.json we do not have it in the forbidden list.
 * Nonetheless we do need the macro.
 */
#define ENTRY_JSON_FILENAME ".entry.json"

/*
 * optional (and for some executable) filenames in the top level directory
 */
#define PROG_ALT_C "prog.alt.c"                 /* alt code source file */
#define TRY_ALT_SH "try.alt.sh"                 /* try.alt.sh for prog.alt.c */
#define TRY_SH "try.sh"                         /* try.sh for prog.c */

/*
 * directory names that should be ignored
 */
#define BAZAAR_DIRNAME ".bzr"                   /* for Bazaar */
#define CIRCLECI_DIRNAME ".circleci"            /* CircleCI */
#define FOSSIL_DIRNAME0 ".fslckout"             /* For Fossil */
#define GIT_DIRNAME ".git"                      /* for git */
#define GITHUB_DIRNAME ".github"                /* For GitHub */
#define GITLAB_DIRNAME ".gitlab"                /* For GitLab */
#define MERCURIAL_DIRNAME ".hg"                 /* for Mercurial */
#define JETBRAIN_DIRNAME ".idea"                /* JetBrains IDE */
#define SVN_DIRNAME ".svn"                      /* for svn */
#define BITKEEPER_DIRNAME "BitKeeper"           /* For BitKeeper */
#define CVS_DIRNAME "CVS"                       /* for CVS */
#define RCCS_DIRNAME "RCCS"                     /* for RCCS */
#define FOSSIL_DIRNAME1 "_FOSSIL_"              /* For Fossil */
#define MONOTONE_DIRNAME "_MTN"                 /* For Monotone */
#define DARCS_DIRNAME "_darcs"                  /* For Darcs */
#define SCCS_DIRNAME "SCCS"                     /* For SCCS */
#define RCS_DIRNAME "RCS"                       /* for RCS */

/*
 * filenames that should be ignored, mostly for chkentry -w but it can be used
 * in mkiocccentry too (although it is implicit since these are dot files)
 */
#define GITIGNORE_FILENAME      ".gitignore"            /* ignore list for git */
#define DS_STORE_FILENAME0      ".DS_Store"             /* Apple's annoying .DS_Store file */
#define DS_STORE_FILENAME1      "._.DS_Store"           /* Apple's annoying ._.DS_Store file */
#define DOT_PATH_FILENAME       ".path"                 /* IOCCC .path dot file */

extern char *mandatory_filenames[];             /* filenames that MUST exist in the top level directory */
extern char *forbidden_filenames[];             /* filenames that must NOT exist in the top level directory */
extern char *optional_filenames[];              /* filenames that are OPTIONAL in top level directory */
extern char *ignored_dirnames[];                /* directory names that should be ignored */
extern char *ignored_filenames[];               /* ignored filenames like .gitignore, .DS_Store etc. */
extern char *executable_filenames[];            /* filenames that should have mode 0555 */
extern struct dyn_array *ignored_paths;         /* ignored paths from chkentry -i path */
extern bool ignore_permissions;                 /* true ==> ignore permissions of files and directories */

/*
 * enums
 */

/*
 * manifest_path - used by test_manifest_path
 *
 * Technically the MAN_PATH_EPERM is not exactly the best analogue for EPERM but
 * since both are about permissions (EPERM being permission denied,
 * MAN_PATH_EPERM being the path is the wrong permission) this works okay.
 *
 * BTW: we explicitly set MAN_PATH_OK to 0 to be like other functions where the
 * return value of 0 indicates success. And although we could have the others >
 * 0 like we chose < 0 as a lot of functions also do that, though the errno is >
 * 0.
 */
enum manifest_path
{
    MAN_PATH_ERR = -5,      /* some unexpected condition occurred (should never happen) */
    MAN_PATH_NULL = -4,     /* path is NULL */
    MAN_PATH_EMPTY = -3,    /* path is empty string */
    MAN_PATH_ENOENT = -2,    /* path does not exist */
    MAN_PATH_EPERM = -1,     /* path wrong permissions */
    MAN_PATH_OK = 0,        /* path exists and right mode */
};
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
    char const *chkentry_ver;	/* chkentry version (compiled in, same as chkentry -V) */
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
    char const *chkentry_ver;	/* chkentry version (compiled in, same as chkentry -V) */
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
    off_t rule_2a_size;		/* Rule 2a size of prog.c */
    size_t rule_2b_size;	/* Rule 2b size of prog.c */
    /* entry boolean values */
    bool empty_override;	/* true ==> empty prog.c override requested */
    bool rule_2a_override;	/* true ==> Rule 2a override requested */
    bool rule_2a_mismatch;	/* true ==> file size != rule_count function size */
    bool rule_2b_override;	/* true ==> Rule 2b override requested */
    bool highbit_warning;	/* true ==> high bit character(s) detected */
    bool nul_warning;		/* true ==> NUL character(s) detected */
    bool trigraph_warning;	/* true ==> unknown or invalid trigraph(s) detected */
    bool wordbuf_warning;	/* true ==> word buffer overflow detected */
    bool ungetc_warning;	/* true ==> ungetc warning detected */
    bool Makefile_override;	/* true ==> Makefile rule override requested */
    bool first_rule_is_all;	/* true ==> Makefile first rule is all */
    bool found_all_rule;	/* true ==> Makefile has an all rule */
    bool found_clean_rule;	/* true ==> Makefile has clean rule */
    bool found_clobber_rule;	/* true ==> Makefile has a clobber rule */
    bool found_try_rule;	/* true ==> Makefile has a try rule */
    /* test or non-test mode */
    bool test_mode;		/* true ==> test mode entered */

    /*
     * dynamic arrays for files and directories
     */
    struct dyn_array *required_files;   /* required three files */
    struct dyn_array *extra_files;      /* extra files to be added to tarball */
    struct dyn_array *directories;      /* directories seen */
    struct dyn_array *ignored_dirs;     /* ignored directories */
    struct dyn_array *forbidden_files;  /* forbidden files */
    struct dyn_array *unsafe_files;     /* unsafe files */
    struct dyn_array *unsafe_dirs;      /* unsafe directories */
    struct dyn_array *ignored_symlinks; /* ignored symlinks files */
    struct dyn_array *ignore_paths;     /* list of paths user requested we ignore (-I foo -I bar) */
    struct dyn_array *manifest_paths;   /* list of paths to include from -M file */

    /*
     * JSON stuff
     */
    char const *info_file;	/* .info.json filename */
    char const *auth_file;	/* .auth.json filename */
    size_t extra_count;		/* number of extra files */

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
extern bool test_chkentry_version(char const *str);
extern bool test_default_handle(bool boolean);
extern bool test_email(char const *str);
extern bool test_empty_override(bool boolean);
extern bool test_submit_slot(int submit_slot);
extern bool test_extra_filename(char const *str);
extern bool test_filename_len(char const *str);
extern bool test_first_rule_is_all(bool boolean);
extern bool test_fnamchk_version(char const *str);
extern bool test_formed_timestamp(time_t tstamp);
extern bool test_formed_timestamp_usec(int formed_timestamp_usec);
extern bool test_found_all_rule(bool boolean);
extern bool test_found_clean_rule(bool boolean);
extern bool test_found_clobber_rule(bool boolean);
extern bool test_found_try_rule(bool boolean);
extern bool test_github(char const *str);
extern bool test_highbit_warning(bool boolean);
extern bool test_info_JSON(char const *str);
extern bool test_IOCCC_contest(char const *str);
extern bool test_IOCCC_year(int IOCCC_year);
extern bool test_iocccsize_version(char const *str);
extern bool test_location_code(char const *str);
extern bool test_manifest(struct manifest *manp, char *submission_dir);
enum manifest_path check_manifest_path(char *path, char const *name, mode_t mode);
void test_manifest_path(char *path, char const *name, enum manifest_path error, mode_t mode);
extern bool test_min_timestamp(time_t tstamp);
extern bool test_mkiocccentry_version(char const *str);
extern bool test_name(char const *str);
extern bool test_no_comment(char const *str);
extern bool test_nul_warning(bool boolean);
extern bool test_past_winning_author(bool boolean);
extern bool test_remarks(char const *str);
extern bool test_rule_2a_mismatch(bool boolean);
extern bool test_rule_2a_override(bool boolean);
extern bool test_rule_2a_size(off_t rule_2a_size);
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
extern bool test_wordbuf_warning(bool boolean);
extern bool is_mandatory_filename(char const *str);
extern bool is_forbidden_filename(char const *str);
extern bool is_optional_filename(char const *str);
extern bool is_ignored_dirname(char const *str);
extern bool is_executable_filename(char const *str);
extern bool has_ignored_dirname(char const *path);


#endif /* INCLUDE_ENTRY_UTIL_H */
