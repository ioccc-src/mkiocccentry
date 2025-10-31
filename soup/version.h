/*
 * version - official IOCCC toolkit versions
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Make an IOCCC compressed tarball for an IOCCC submission.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla (with a healthy
 * overdose of chocolate :-) ) C.  Why?  Because this is an obfuscated C
 * contest.  But then why isn't this code obfuscated?  Because the IOCCC judges
 * prefer to write in robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell source and finger daemon source.
 * Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * OK, we do make use of shell scripts to help build and test
 * this repo: but who doesn't use a bit of shell scripting now and then?  :-)
 * Nevertheless, the core of building your IOCCC entry compressed tarball,
 * the code that you, the IOCCC contestant will use, is all written in
 * C because this is the IOCCC.
 *
 * If you do find a problem with this code, please let the us know by opening an
 * issue at the GitHub issues page:
 *
 *      https://github.com/ioccc-src/mkiocccentry/issues
 *
 * Many thanks are due to a number of people who provided important
 * and valuable testing, suggestions, issue reports and GitHub pull
 * requests to this code.  Without their time and effort, this tool
 * would not work very well!
 *
 * Among the GitHub users we wish to thank include these fine developers
 * in alphabetical GitHub username order:
 *
 *	@ilyakurdyukov		Ilya Kurdyukov
 *	@SirWumpus		Anthony Howe
 *	@vog			Volker Diels-Grabsch
 *
 * Copyright (c) 2021-2025 by Landon Curt Noll and Cody Boone Ferguson.
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
 * Most of these tools were co-developed in 2021-2025 by Cody Boone Ferguson and
 * Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * and a few tools were developed by Cody and the rest by Landon, with the
 * exception of iocccsize, which was developed by Anthony C Howe.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_VERSION_H)
#    define  INCLUDE_VERSION_H


/*
 * official mkiocccentry repo release
 *
 * NOTE: This should match the latest Release string in CHANGES.md
 */
#define MKIOCCCENTRY_REPO_VERSION "2.8.5 2025-10-31"	/* special release format: major.minor[.patch] YYYY-MM-DD */

/*
 * official soup version (aka recipe :-) )
 */
#define SOUP_VERSION "2.2.5 2025-10-31"	/* format: major.minor[.patch] YYYY-MM-DD */

/*
 * official iocccsize version
 */
#define IOCCCSIZE_VERSION "29.0.1 2025-09-29"	/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_IOCCCSIZE_VERSION IOCCCSIZE_VERSION

/*
 * official mkiocccentry versions (mkiocccentry itself and answers)
 */
#define MKIOCCCENTRY_VERSION "2.1.5 2025-10-17"	/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_MKIOCCCENTRY_VERSION MKIOCCCENTRY_VERSION
#define MKIOCCCENTRY_ANSWERS_VERSION "MKIOCCCENTRY_ANSWERS_IOCCC29-1.0" /* answers file version */
#define MKIOCCCENTRY_ANSWERS_EOF "ANSWERS_EOF" /* answers file EOF marker */

/*
 * official location version
 */
#define LOCATION_VERSION "1.0.6 2025-09-23"	/* format: major.minor[.patch] YYYY-MM-DD */

/*
 * Version of info for JSON the .info.json file.
 */
#define INFO_VERSION "1.18 2025-05-28"		/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_INFO_VERSION INFO_VERSION

/*
 * Version of info for JSON the .auth.json file.
 */
#define AUTH_VERSION "1.24 2025-08-28"		/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_AUTH_VERSION AUTH_VERSION

/*
 * official fnamchk version
 */
#define FNAMCHK_VERSION "2.1.5 2025-10-17"	/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_FNAMCHK_VERSION FNAMCHK_VERSION

/*
 * official txzchk version
 */
#define TXZCHK_VERSION "2.0.10 2025-10-31"	/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_TXZCHK_VERSION TXZCHK_VERSION

/*
 * official chkentry version
 */
#define CHKENTRY_VERSION "2.1.6 2025-10-29"	/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_CHKENTRY_VERSION CHKENTRY_VERSION

/*
 * official chksubmit version
 */
#define CHKSUBMIT_VERSION "1.0.1 2025-10-17"	/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_CHKSUBMIT_VERSION CHKSUBMIT_VERSION

/*
 * Version of info for JSON the .entry.json files.
 */
#define ENTRY_VERSION "1.2 2024-09-25"		/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_ENTRY_VERSION ENTRY_VERSION

/*
 * Version of info for JSON the author_handle.json files.
 */
#define AUTHOR_VERSION "1.1 2024-02-11"		/* format: major.minor[.patch] YYYY-MM-DD */
#define MIN_AUTHOR_VERSION AUTHOR_VERSION

/*
 * test_file_util - test common utility functions for file operations
 */
#define FILE_UTIL_TEST_VERSION "2.1.2 2025-10-20" /* version format: major.minor[.patch] YYYY-MM-DD */

 /*
  * try_fts_walk - walk a directory tree using one of the static walk_rule sets
  */
#define TRY_FTS_WALK_VERSION "1.0.3 2025-10-25"	/* version format: major.minor[.patch] YYYY-MM-DD */

 /*
  * try_walk_set - try using one of the static walk_rule sets
  */
#define TRY_WALK_SET_VERSION "1.0.2 2025-10-25"	/* version format: major.minor[.patch] YYYY-MM-DD */


/*
 * debug version
 *
 * Because the dbg is a self contained system, we cannot define DBG_VERSION here.
 * See dbg/dbg.h for the DBG_VERSION value.
 */


/*
 * dynamic array versions
 *
 * Because the dynamic array is a self contained system, we cannot define
 * DYN_ALLOC_VERSION here. See dyn_array/dyn_array.h for the DYN_ALLOC_VERSION
 * value.
 *
 * Because the dynamic array is a self contained system, we cannot define
 * DYN_TEST_VERSION here. See dyn_test.h for the DYN_TEST_VERSION value.
 */


/*
 * pr version
 *
 * Because the pr is a self contained system, we cannot define PR_VERSION here.
 * See pr/pr.h for the PR_VERSION value.
 */


/*
 * cpath version
 *
 * Because the pr is a self contained system, we cannot define CPATH_VERSION here.
 * See cpath/cpath.h for the CPATH_VERSION value.
 */


/*
 * JSON parser versions
 *
 * Because the JSON parser is a self contained system, we cannot define
 * JSON_PARSER_VERSION here. See jparse/jparse.h for the JSON_PARSER_VERSION
 * value.
 *
 * Because the JSON parser is a self contained system, we cannot define
 * JPARSE_VERSION here. See jparse/jparse_main.h for the JPARSE_VERSION value.
 */


/*
 * Generate JSON semantics table version
 *
 * Because the JSON parser is a self contained system, we cannot define
 * JSEMTBLGEN_VERSION here. See jparse/jsemtblgen.h for the JSEMTBLGEN_VERSION
 * value.
 */


#endif /* INCLUDE_VERSION_H */
