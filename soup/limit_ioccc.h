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
#    define  INCLUDE_LIMIT_IOCCC_H


#include <time.h>

/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"


#undef DIGRAPHS		/* digraphs count a 2 for Rule 2b */
#undef TRIGRAPHS	/* trigraphs count a 3 for Rule 2b */

#define RULE_2A_SIZE ((off_t)(4993))		/* Rule 2a size of prog.c */
#define RULE_2B_SIZE ((size_t)(2503))		/* Rule 2b size of prog.c */
#define MAX_TARBALL_LEN ((off_t)(3999971))	/* compressed tarball size limit in bytes */
#define MAX_SUM_FILELEN ((off_t)(27651*1024))	/* maximum sum of the byte lengths of all files in the entry */
#define MANDATORY_FILE_COUNT (5)		/* number of required files in an entry */
/* NOTE: MAX_FILE_COUNT must be > MANDATORY_FILE_COUNT */
#define MAX_FILE_COUNT (42)		/* maximum number of files in an entry (MANDATORY_FILE_COUNT + extra files) */
/* NOTE: MAX_SUBMIT_SLOT must be < 10 to the MAX_SUBMIT_SLOT_CHARS power */
#define MAX_SUBMIT_SLOT (9)		/* entry numbers from 0 to MAX_SUBMIT_SLOT */
#define MAX_SUBMIT_SLOT_CHARS (1)		/* characters that represent the maximum entry number */
#define MAX_AUTHORS (5)			/* maximum number of authors of an entry */
#define MAX_AUTHORS_CHARS (1)		/* characters that represent the author number */
#define MAX_NAME_LEN (48)		/* max author name length */
#define MAX_EMAIL_LEN (48)		/* max Email address length */
#define MAX_URL_LEN (64)		/* max home URL length, including http:// or https:// */
#define MAX_MASTODON_LEN (20+64)	/* max mastodon handle length, including both '@'s */
#define MAX_GITHUB_LEN (16)		/* max GitHub account length, including the leading '@' */
#define MAX_AFFILIATION_LEN (48)	/* max affiliation name length */
#define MAX_TITLE_LEN (32)		/* maximum length of a title */
#define MAX_ABSTRACT_LEN (64)		/* maximum length of an abstract */
#define MAX_HANDLE (32)			/* maximum length of IOCCC winner handle */
#define MAX_BASENAME_LEN ((size_t)(99))	/* tar --format=v7 limits filenames to 99 characters */
#define UUID_LEN (36)			/* characters in a UUID string - as per RFC4122 */
#define UUID_VERSION (4)		/* version 4 - random UUID */
#define UUID_VARIANT (0xa)		/* variant 1 - encoded as 0xa */
#define TIMESTAMP_EPOCH "Thu Jan 01 00:00:00 1970 UTC"	/* gettimeofday epoch */
#define MAX_TIMESTAMP_LEN (48)		/* 28 + 20 more padding for locate */
#define MAX_CLOCK_ERROR ((42*60)-1)	/* maximum seconds allowed for a clock to be in error */

/*
 * Be careful not to change this value as it will invalidate all IOCCC timestamps < this value
 */
#define MIN_TIMESTAMP ((time_t)1716103155)


/*
 * IOCCC contest name
 *
 * use format: IOCCC[0-9][0-9] or IOCCCMOCK
 */
#define IOCCC_CONTEST "IOCCC28"


/*
 * The year that IOCCC_CONTEST closes
 *
 * NOTE: If the contest spans the new year, then the year
 *	 that IOCCC_CONTEST opens may be different.
 */
#define IOCCC_YEAR (2024)


/*
 * Did you know that the JSON author removed comments from the original
 * JSON spec because the creators claimed they saw people using comments
 * to hold parsing directives?  See the following URL for details:
 *
 *	https://stackoverflow.com/questions/244777/can-comments-be-used-in-json/10976934#10976934
 *
 * The IOCCC honors the above decision to remove comments from the JSON spec
 * by mandating the following directive in all IOCCC related JSON files:
 *
 *	JSON_PARSING_DIRECTIVE_NAME : JSON_PARSING_DIRECTIVE_VALUE
 */
#define JSON_PARSING_DIRECTIVE_NAME "no_comment"
#define JSON_PARSING_DIRECTIVE_VALUE "mandatory comment: because comments were removed from the original JSON spec"


#endif /* ! INCLUDE_LIMIT_IOCCC_H */
