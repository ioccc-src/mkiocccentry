/*
 * version - official IOCCC toolkit versions
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla (with a healthy
 * overdose of chocolate :-) ) C.  Why? Because this is an obfuscated C contest.
 * But then why isn't this code obfuscated?  Because the IOCCC judges prefer to
 * write in robust unobfuscated code.  Besides, the IOCCC was started as an
 * ironic commentary on the Bourne shell source and finger daemon source.
 * Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, please let the judges know.
 * To contact the judges please see:
 *
 *      https://www.ioccc.org/contact.html
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
 * Copyright (c) 2021-2025 by Landon Curt Noll.  All Rights Reserved.
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


#if !defined(INCLUDE_VERSION_H)
#    define  INCLUDE_VERSION_H


/*
 * official mkiocccentry repo release
 *
 * NOTE: This should match the latest Release string in CHANGES.md
 */
#define MKIOCCCENTRY_REPO_VERSION "2.3.37 2025-02-19"	/* special release format: major.minor[.patch] YYYY-MM-DD */


/*
 * official soup version (aka recipe :-) )
 */
#define SOUP_VERSION "1.1.20 2025-02-12"	/* format: major.minor YYYY-MM-DD */

/*
 * official iocccsize version
 */
#define IOCCCSIZE_VERSION "28.15 2024-06-27"	/* format: major.minor YYYY-MM-DD */

/*
 * official mkiocccentry versions (mkiocccentry itself and answers)
 */
#define MKIOCCCENTRY_VERSION "1.2.28 2025-02-19"	/* format: major.minor YYYY-MM-DD */
#define MKIOCCCENTRY_ANSWERS_VERSION "MKIOCCCENTRY_ANSWERS_IOCCC28-1.0" /* answers file version */
#define MKIOCCCENTRY_ANSWERS_EOF "ANSWERS_EOF" /* answers file EOF marker */

/*
 * Version of info for JSON the .info.json file.
 */
#define INFO_VERSION "1.16 2024-05-18"		/* format: major.minor YYYY-MM-DD */

/*
 * Version of info for JSON the .auth.json file.
 */
#define AUTH_VERSION "1.22 2024-05-18"		/* format: major.minor YYYY-MM-DD */

/*
 * official fnamchk version
 */
#define FNAMCHK_VERSION "1.0.4 2025-02-19"	/* format: major.minor YYYY-MM-DD */

/*
 * official txzchk version
 */
#define TXZCHK_VERSION "1.1.14 2025-02-19"	/* format: major.minor YYYY-MM-DD */

/*
 * official chkentry version
 */
#define CHKENTRY_VERSION "1.1.0 2025-01-18"	/* format: major.minor YYYY-MM-DD */

/*
 * Version of info for JSON the .entry.json files.
 */
#define ENTRY_VERSION "1.2 2024-09-25"		/* format: major.minor YYYY-MM-DD */

/*
 * Version of info for JSON the author_handle.json files.
 */
#define AUTHOR_VERSION "1.1 2024-02-11"		/* format: major.minor YYYY-MM-DD */


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
