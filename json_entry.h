/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_entry - supporting mkiocccentry JSON files
 *
 * JSON related functions to support formation of .info.json files
 * and .author.json files, their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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


#if !defined(INCLUDE_JSON_ENTRY_H)
#    define  INCLUDE_JSON_ENTRY_H


/*
 * json_parse - json related structs and definitions
 */
#include "json_parse.h"

/*
 * common json fields - for use in mkiocccentry.
 *
 * NOTE: We don't use the json_field or json_value fields (see json_chk.h) here
 * because this struct is for mkiocccentry which is in control of what's written
 * whereas for jinfochk and jauthchk we don't have control of what's in the file
 * and we can't prove that it wasn't made by hand or some other utility.
 */
struct json_common
{
    /*
     * version
     */
    char *mkiocccentry_ver;	/* mkiocccentry version (MKIOCCCENTRY_VERSION) */
    char const *iocccsize_ver;	/* iocccsize version (compiled in, same as iocccsize -V) */
    char const *jinfochk_ver;	/* jinfochk version (compiled in, same as jinfochk -V) */
    char const *jauthchk_ver;	/* jauthchk version (compiled in, same as jauthchk -V) */
    char const *fnamchk_ver;	/* fnamchk version (compiled in, same as fnamchk -V) */
    char const *txzchk_ver;	/* txzchk version (compiled in, same as txzchk -V) */
    /*
     * entry
     */
    char *ioccc_id;		/* IOCCC contest ID */
    int entry_num;		/* IOCCC entry number */
    char *tarball;		/* tarball filename */
    bool test_mode;		/* true ==> test mode entered */

    /*
     * time
     */
    time_t tstamp;		/* seconds since epoch when .info json was formed (see gettimeofday(2)) */
    int usec;			/* microseconds since the tstamp second */
    char *epoch;		/* epoch of tstamp, currently: Thu Jan 1 00:00:00 1970 UTC */
    char *utctime;		/* UTC converted string for tstamp (see strftime(3)) */
};

/*
 * author info
 */
struct author
{
    char *name;			/* name of the author */
    char *location_code;	/* author location/country code */
    char const *location_name;	/* name of author location/country */
    char *email;		/* Email address of author or or empty string ==> not provided */
    char *url;			/* home URL of author or or empty string ==> not provided */
    char *twitter;		/* author twitter handle or or empty string ==> not provided */
    char *github;		/* author GitHub username or or empty string ==> not provided */
    char *affiliation;		/* author affiliation or or empty string ==> not provided */
    bool past_winner;		/* true ==> author claims to have won before, false ==> author claims not a prev winner */
    bool default_handle;	/* true ==> default author_handle accepted, false ==> author_handle entered */
    char *author_handle;	/* IOCCC author handle (for winning entries) */
    int author_num;		/* author number */

    /*
     * common
     */
    struct json_common common;	/* fields that are common to this struct author and struct info (below) */
};

/*
 * info for JSON
 *
 * Information we will collect in order to form the .info json file.
 */
struct info
{
    /*
     * entry
     */
    char *title;		/* entry title */
    char *abstract;		/* entry abstract */
    off_t rule_2a_size;		/* Rule 2a size of prog.c */
    size_t rule_2b_size;	/* Rule 2b size of prog.c */
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
    bool test_mode;		/* true ==> contest ID is test */

    /*
     * filenames
     */
    char *prog_c;		/* prog.c filename */
    char *Makefile;		/* Makefile filename */
    char *remarks_md;		/* remarks.md filename */
    int extra_count;		/* number of extra files */
    char **extra_file;		/* list of extra filenames followed by NULL */

    /*
     * common
     */
    struct json_common common;	/* fields that are common to this struct info and struct author (above) */
};


/*
 * external function declarations
 */
extern bool json_putc(uint8_t const c, FILE *stream);
extern bool json_fprintf_str(FILE *stream, char const *str);
extern bool json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
				      char const *tail);
extern bool json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
				    char const *tail);
extern bool json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
				    char const *tail);
extern void free_info(struct info *infop);
extern void free_author_array(struct author *authorp, int author_count);


#endif /* INCLUDE_JSON_ENTRY_H */
