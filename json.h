/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json - JSON functions supporting mkiocccentry code
 *
 * JSON related fuctions to support formation of .info.json files
 * and .author.json filesm their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * "Because JSON enbodies a commitment to original design flaws." :-)
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


#ifndef JSON_H
#define JSON_H


#include <time.h>
#include <stdint.h>


/*
 * byte as octet constants
 */
#define BITS_IN_BYTE (8)	    /* assume 8 bit bytes */
#define MAX_BYTE (0xff)		    /* maximum byte value */
#define BYTE_VALUES (MAX_BYTE+1)    /* number of different combinations of bytes */


/*
 * author info
 */
struct author {
    char *name;			/* name of the author */
    char *location_code;	/* author location/country code */
    char const *location_name;	/* name of author location/country */
    char *email;		/* Email address of author or or empty string ==> not provided */
    char *url;			/* home URL of author or or empty string ==> not provided */
    char *twitter;		/* author twitter handle or or empty string ==> not provided */
    char *github;		/* author GitHub username or or empty string ==> not provided */
    char *affiliation;		/* author affiliation or or empty string ==> not provided */
    int author_num;		/* author number */

    /* jauthchk specific */
    unsigned issues;		/* number of issues found in file (for jauthchk) */
};


/*
 * info for JSON
 *
 * Information we will collect in order to form the .info json file.
 */
struct info {
    /*
     * version
     */
    char *mkiocccentry_ver;	/* mkiocccentry version (MKIOCCCENTRY_VERSION) */
    char const *iocccsize_ver;	/* iocccsize version (compiled in, same as iocccsize -V) */
    /*
     * entry
     */
    char *ioccc_id;		/* IOCCC contest ID */
    int entry_num;		/* IOCCC entry number */
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
    bool trigraph_warning;	/* true ==> unknown or invalid tri-graph(s) detected */
    bool wordbuf_warning;	/* true ==> word buffer overflow detected */
    bool ungetc_warning;	/* true ==> ungetc warning detected */
    bool Makefile_override;	/* true ==> Makefile rule override requested */
    bool first_rule_is_all;	/* true ==> Makefile first rule is all */
    bool found_all_rule;	/* true ==> Makefile has an all rule */
    bool found_clean_rule;	/* true ==> Makefile has clean rule */
    bool found_clobber_rule;	/* true ==> Makefile has a clobber rule */
    bool found_try_rule;	/* true ==> Makefile has a try rule */
    unsigned answers_errors;	/* > 0 ==> flushing or closing answers file failed */
    /*
     * filenames
     */
    char *prog_c;		/* prog.c filename */
    char *Makefile;		/* Makefile filename */
    char *remarks_md;		/* remarks.md filename */
    int extra_count;		/* number of extra files */
    char **extra_file;		/* list of extra filenames followed by NULL */
    char *tarball_path;		/* tarball filename */
    /*
     * time
     */
    time_t tstamp;		/* seconds since epoch when .info json was formed (see gettimeofday(2)) */
    int usec;			/* microseconds since the tstamp second */
    char *epoch;		/* epoch of tstamp, currently: Thr Jan 1 00:00:00 1970 UTC */
    char *gmtime;		/* UTC converted string for tstamp (see asctime(3)) */

    /* jinfochk specific */
    unsigned issues;		/* number of issues found in file (for jinfochk) */
};


/*
 * external function declarations
 */
extern char *malloc_json_encode(char const *ptr, size_t len, size_t *retlen);
extern char *malloc_json_encode_str(char const *str, size_t *retlen);
extern void jencchk(void);
extern bool json_putc(uint8_t const c, FILE *stream);
extern char *malloc_json_decode(char const *ptr, size_t len, size_t *retlen, bool strict);
extern char *malloc_json_decode_str(char const *str, size_t *retlen, bool strict);


#endif /* JSON_H */
