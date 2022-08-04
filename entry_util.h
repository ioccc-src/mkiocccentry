/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * entry_util - utilities supporting mkiocccentry JSON files
 *
 * JSON related functions to support formation of .info.json files
 * and .author.json files, their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * This JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and vomit on the JSON spec! :-)
 */


#if !defined(INCLUDE_ENTRY_UTIL_H)
#    define  INCLUDE_ENTRY_UTIL_H


/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * defines
 */
#define INFO_JSON_FILENAME ".info.json"
#define AUTHOR_JSON_FILENAME ".author.json"


/*
 * IOCCC author information
 *
 * Information we will collect for each author of an IOCCC entry to fill out the authors JSON array
 * within the .author.json file.
 */
struct author
{
    char *name;			/* name of the author */
    char *location_code;	/* author location/country code */
    char const *location_name;	/* name of author location/country (compiled in from loc[]) */
    char *email;		/* Email address of author or empty string ==> not provided */
    char *url;			/* home URL of author or empty string ==> not provided */
    char *twitter;		/* author twitter handle or empty string ==> not provided */
    char *github;		/* author GitHub username or empty string ==> not provided */
    char *affiliation;		/* author affiliation or empty string ==> not provided */
    bool past_winner;		/* true ==> author claims to have won before, false ==> author claims not a prev winner */
    bool default_handle;	/* true ==> default author_handle accepted, false ==> author_handle entered */
    char *author_handle;	/* IOCCC author handle (for winning entries) */
    int author_num;		/* author number */
};

/*
 * IOCCC .author.json information
 *
 * Information we will collect in order to form the .author.json file.
 */
struct auth
{
    /*
     * .author.json information before the authors array
     */
    /* file format strings */
    char const *no_comment;	/* mandatory JSON parsing directive :-) */
    char const *author_version;	/* IOCCC .author.json format version (compiled in AUTHOR_VERSION) */
    char const *ioccc_contest;	/* IOCCC contest string (compiled in IOCCC99 or IOCCCMOCK) */
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
    int entry_num;		/* IOCCC entry number */
    char *tarball;		/* tarball filename */
    /* test or non-test mode */
    bool test_mode;		/* true ==> test mode entered */

    /*
     * author set
     */
    int author_count;		/* number of authors - length of author array */
    struct author *author;	/* set of authors for this entry */

    /*
     * .author.json information after the authors array
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
    char const *no_comment;	/* mandatory JSON parsing directive :-) */
    char const *info_version;	/* IOCCC .info.json format version (compiled in INFO_VERSION) */
    char const *ioccc_contest;	/* IOCCC contest string (compiled in IOCCC99 or IOCCCMOCK) */
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
    int entry_num;		/* IOCCC entry number */
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
     * file name array
     */
    char const *info_file;	/* .info.json filename */
    char const *author_file;	/* .author.json filename */
    char *prog_c;		/* prog.c filename */
    char *Makefile;		/* Makefile filename */
    char *remarks_md;		/* remarks.md filename */
    int extra_count;		/* number of extra files */
    char **extra_file;		/* list of extra filenames followed by NULL */

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
 * external function declarations
 */
extern void free_auth(struct auth *authp);
extern void free_info(struct info *infop);
extern void free_author_array(struct author *authorp, int author_count);
extern bool load_author(struct author *authorp, char *name, char *location_code, char const *location_name,
			char *email, char *url, char *twitter, char *github, char *affiliation,
			bool past_winner, bool default_handle, char *author_handle, int author_number);
/* XXX - begin sorted order matching chk_validate.c here - XXX */
extern bool test_IOCCC_author_version(char *str);
extern bool test_IOCCC_contest_id(char *str);
extern bool test_IOCCC_info_version(char *str);
extern bool test_Makefile(char *str);
extern bool test_Makefile_override(bool boolean);
extern bool test_abstract(char *str);
extern bool test_affiliation(char *str);
extern bool test_author_JSON(char *str);
extern bool test_author_count(int author_count);
extern bool test_author_handle(char *str);
extern bool test_author_number(int author_number);
extern bool test_rule_2a_override(bool boolean);
extern bool test_rule_2a_mismatch(bool boolean);
extern bool test_rule_2b_override(bool boolean);
extern bool test_highbit_warning(bool boolean);
extern bool test_nul_warning(bool boolean);
extern bool test_trigraph_warning(bool boolean);
extern bool test_wordbuf_warning(bool boolean);
extern bool test_ungetc_warning(bool boolean);
extern bool test_first_rule_is_all(bool boolean);
extern bool test_found_all_rule(bool boolean);
extern bool test_found_clean_rule(bool boolean);
extern bool test_found_clobber_rule(bool boolean);
extern bool test_found_try_rule(bool boolean);
extern bool test_test_mode(bool boolean);
/* XXX - end sorted order matching chk_validate.c here - XXX */
extern bool test_authors(int author_count, struct author *authorp);
extern bool test_mkiocccentry_version(char *str);
extern bool test_iocccsize_version(char *str);
extern bool test_chkentry_version(char *str);
extern bool test_fnamchk_version(char *str);
extern bool test_txzchk_version(char *str);
extern bool test_c_src(char *str);
extern bool test_title(char *str);

#endif /* INCLUDE_ENTRY_UTIL_H */
