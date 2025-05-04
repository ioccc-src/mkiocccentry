/*
 * limit_ioccc - IOCCC size and rule related limitations
 *
 * Copyright (c) 2021,2022,2023,2024 by Landon Curt Noll. All Rights Reserved.
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
/*
 * IMPORTANT:
 *
 * The MAX_TARBALL_LEN value MUST match the MAX_TARBALL_LEN variable as defined
 * in the IOCCC submit server code.
 *
 * Possible mandatory files for a submission:
 *
 *	prog.c
 *	Makefile
 *	remarks.md	    (NOTE: turns into README.md for a winning entry)
 *	.info.json	    (NOTE: ignored by mkiocccentry, error for txzchk, becomes .entry.json for a winning entry)
 *	.auth.json	    (NOTE: ignored by mkiocccentry, error for txzchk, becomes .entry.json for a winning entry)
 *
 * FYI: Optional files (and sometimes encouraged) for a submission that are not considered extra:
 *
 *	try.sh		    (NOTE: encouraged optional file that does NOT count as an extra file if it exists)
 *	prog.alt.c	    (NOTE: a file, if it exists, that does NOT count as an extra file if it exists)
 *
 * FYI: Possible mandatory files for a winning entry, not a submission, include:
 *
 *	prog.c
 *	Makefile
 *	README.md	    (NOTE: built in part from the submission remarks.md file)
 *	.entry.json	    (NOTE: built in part from the submission .info.json and .auth.json files)
 *	index.html	    (NOTE: built in part from the README.md file)
 *	YYYY_name.tar.bz2   (NOTE: built from the set of winning entry directories and files)
 *	.path		    (NOTE: error for both mkiocccentry and txzchk, while winning entries will have this file)
 *	prog.orig.c	    (NOTE: error for both mkiocccentry and txzchk)
 *
 * FYI: Possible files that may be added to a winning entry:
 *
 *	.gitignore	    (NOTE: error for both mkiocccentry and txzchk, while winning entries may have this file)
 *	prog.alt.c	    (NOTE: winning entries may have this file)
 *	try.sh		    (NOTE: winning entries likely have this file)
 *	*		    (NOTE: The IOCCC judges may add more files to a winning entry as needed)
 *
 * NOTE: The MANDATORY_SUBMISSION_FILES file is a limit on extra files for a submission, NOT a winning entry
 *	 and does NOT include any mandatory files.
 */
#define MAX_TARBALL_LEN ((off_t)(3999971))	/* compressed tarball size limit in bytes */
#define MAX_SUM_FILELEN ((off_t)(27651*1024))	/* maximum sum of the byte lengths of all files in the entry */
#define MANDATORY_SUBMISSION_FILES (5)		/* number of required files in submission */
#define OPTIONAL_SUBMISSION_FILES (3)		/* submission files, if they exist, that do NOT count towards the extra total */
#define MAX_EXTRA_FILE_COUNT (31)		/* maximum number of files not including mandatory submission files */
#define MAX_EXTRA_DIR_COUNT (13)                /* maximum number of EXTRA directories */
/* maximum total file count, including mandatory files, for a submission */
#define MAX_FILE_COUNT (MANDATORY_SUBMISSION_FILES+OPTIONAL_SUBMISSION_FILES+MAX_EXTRA_FILE_COUNT)
/*
 * IMPORTANT:
 *
 * The MAX_SUBMIT_SLOT value MUST match the MAX_SUBMIT_SLOT variable as defined
 * in the IOCCC submit server code.
 *
 * The MAX_SUBMIT_SLOT must be < 10 to the MAX_SUBMIT_SLOT_CHARS power!
 */
#define MAX_SUBMIT_SLOT (9)		/* entry numbers from 0 to MAX_SUBMIT_SLOT */
#define MAX_SUBMIT_SLOT_CHARS (1)	/* characters that represent the maximum entry number */
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
#define UUID_VARIANT_0 (0x8)		/* variant 0 - encoded as 0x8 */
#define UUID_VARIANT_1 (0x9)		/* variant 1 - encoded as 0x9 */
#define UUID_VARIANT_2 (0xa)		/* variant 2 - encoded as 0xa */
#define UUID_VARIANT_3 (0xb)		/* variant 3 - encoded as 0xb */
#define TIMESTAMP_EPOCH "Thu Jan 01 00:00:00 1970 UTC"	/* gettimeofday epoch */
#define MAX_TIMESTAMP_LEN (48)		/* 28 + 20 more padding for locate */
#define MAX_CLOCK_ERROR (48*60*60)	/* maximum seconds allowed for a clock to be in error */

/*
 * submission tarballs may have subdirectories as long as they fit certain
 * constraints
 */
#define MAX_FILENAME_LEN (38)    /* max path component length */
#define MAX_PATH_DEPTH (4)       /* max depth of a subdirectory tree */
#define MAX_PATH_LEN (99)        /* max length of a path */

#define MIN_FORMED_TIMESTAMP_USEC (1)       /* minimum formed_timestamp_usec value */
#define MAX_FORMED_TIMESTAMP_USEC (999999)  /* maximum formed_timestamp_usec value */

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

#define IOCCC_STATUS_URL "https://www.ioccc.org/status.html"        /* status of IOCCC URL */
#define IOCCC_ENTER_FAQ_URL "https://www.ioccc.org/faq.html#enter"  /* how to enter FAQ */
#define IOCCC_FIND_AUTHOR_HANDLE "https://www.ioccc.org/faq.html#find_author_handle"	/* how to find an author handle */


#endif /* ! INCLUDE_LIMIT_IOCCC_H */
