/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * limit_ioccc - IOCCC size and rule related limitations
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

#if !defined(INCLUDE_LIMIT_IOCCC_H)
#define INCLUDE_LIMIT_IOCCC_H

#include <time.h>

#undef DIGRAPHS		/* digraphs count a 2 for Rule 2b */
#undef TRIGRAPHS	/* trigraphs count a 3 for Rule 2b */

#define RULE_2A_SIZE ((off_t)(4096))		/* Rule 2a size of prog.c */
#define RULE_2B_SIZE ((size_t)(2503))		/* Rule 2b size of prog.c */
#define MAX_TARBALL_LEN ((off_t)(3999971))	/* the compressed tarball formed cannot be longer than this many bytes */
#define MAX_DIR_KSIZE (27651)			/* entry directory cannot exceed this size in kibibyte (1024 byte) blocks */
#define MAX_ENTRY_NUM (9)		/* entry numbers from 0 to MAX_ENTRY_NUM - must be < 10 to the MAX_ENTRY_CHARS power */
#define MAX_ENTRY_CHARS (1)		/* characters that represent the maximum entry number */
#define MAX_AUTHORS (5)			/* maximum number of authors of an entry */
#define MAX_NAME_LEN (48)		/* max author name length */
#define MAX_EMAIL_LEN (48)		/* max Email address length */
#define MAX_URL_LEN (64)		/* max home URL, including http:// or https:// */
#define MAX_TWITTER_LEN (19)		/* max twitter handle, including the leading @, length */
#define MAX_GITHUB_LEN (16)		/* max GitHub account, including the leading @, length */
#define MAX_AFFILIATION_LEN (48)	/* max affiliation name length */
#define MAX_TITLE_LEN (20)		/* maximum length of a title */
#define MAX_ABSTRACT_LEN (64)		/* maximum length of an abstract */
#define MAX_HANDLE (24)		/* maximum IOCCC winner handle */
#define MAX_BASENAME_LEN ((size_t)(99))	/* tar --format=v7 limits filenames to 99 characters */
#define UUID_LEN (36)		/* characters in a UUID string - as per RFC4122 */
#define UUID_VERSION (4)	/* version 4 - random UUID */
#define UUID_VARIANT (0xa)	/* variant 1 - encoded as 0xa */
#define TIMESTAMP_EPOCH "Thu Jan  1 00:00:00 1970 UTC"	/* gettimeofday epoch */

/*
 * Be careful to change this value as it will invalidate all IOCCC timestamps < this value
 */
#define MIN_TIMESTAMP ((time_t)1643987926)


/*
 * official iocccsize version
 */
#define IOCCCSIZE_VERSION "28.7 2022-02-01"	/* use format: major.minor YYYY-MM-DD */


/*
 * official mkiocccentry version
 */
#define MKIOCCCENTRY_VERSION "0.36 2022-02-18"	/* use format: major.minor YYYY-MM-DD */


/*
 * Version of info for JSON the .info.json file.
 *
 * The following is NOT the version of this mkiocccentry tool!
 */
#define INFO_VERSION "1.8 2022-02-13"		/* version of the .info.json file to produce */


/*
 * Version of info for JSON the .author.json file.
 *
 * The following is NOT the version of this mkiocccentry tool!
 */
#define AUTHOR_VERSION "1.10 2022-02-18"	/* version of the .author.json file to produce */


/*
 * fnamchk version
 */
#define FNAMCHK_VERSION "0.2 2022-02-07"	/* use format: major.minor YYYY-MM-DD */


/*
 * jstrencode version
 */
#define JSTRENCODE_VERSION "0.1 2022-02-14"	/* use format: major.minor YYYY-MM-DD */


/*
 * jstrdecode version
 */
#define JSTRDECODE_VERSION "0.1 2022-02-14"	/* use format: major.minor YYYY-MM-DD */


/*
 * IOCCC contest name
 *
 * use format: IOCCC[0-9][0-9] or IOCCCMOCK
 */
#define IOCCC_CONTEST "IOCCCMOCK"


/*
 * The year that IOCCC_CONTEST closes
 *
 * NOTE: If the contest spans the new year, then the year
 *	 that IOCCC_CONTEST opens may be different.
 */
#define IOCCC_YEAR (2022)


/*
 * rule_count() processing results
 */
struct iocccsize {
	off_t rule_2a_size;	/* official IOCCC Rule 2a calculated size */
	size_t rule_2b_size;	/* official IOCCC Rule 2b calculated size */
	size_t keywords;	/* keyword count - for -v mode */
	int char_warning;	/* != 0 ==> found high-bit character */
	int nul_warning;	/* != 0 ==> found NUL */
	int trigraph_warning;	/* != 0 ==> found an unknown Tri-Graph */
	int wordbuf_warning;	/* != 0 ==> word buffer overflow detected */
	int ungetc_warning;	/* != 0 ==> ungetc called too many times */
};


/*
 * external functions related to size
 */
extern char const * const iocccsize_version;
extern struct iocccsize rule_count(FILE *fp, int debug);


#endif /* ! INCLUDE_LIMIT_IOCCC_H */
