/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * mkiocccentry -  official IOCCC toolkit versions
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla C.
 * Why?  Because this is an obfuscated C contest.  But then why isn't
 * this code obfuscated?  Because the IOCCC judges prefer to write
 * in robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell source and finger daemon
 * source.  Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, please let the judges know.
 * To contact the judges please see:
 *
 *      https://www.ioccc.org/judges.html
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


#if !defined(INCLUDE_VERSION_H)
#    define  INCLUDE_VERSION_H


/*
 * official iocccsize version
 */
#define IOCCCSIZE_VERSION "28.10 2022-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * official mkiocccentry version
 */
#define MKIOCCCENTRY_VERSION "0.40 2022-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * Version of info for JSON the .info.json file.
 *
 * The following is NOT the version of this mkiocccentry tool and NOT the
 * version of the jinfochk tool!
 */
#define INFO_VERSION "1.9 2022-03-15"		/* format: major.minor YYYY-MM-DD */

/*
 * Version of info for JSON the .author.json file.
 *
 * The following is NOT the version of this mkiocccentry tool and NOT the
 * version of the jauthchk tool!
 */
#define AUTHOR_VERSION "1.12 2022-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * official fnamchk version
 */
#define FNAMCHK_VERSION "0.6 2022-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * official txzchk version
 */
#define TXZCHK_VERSION "0.10 2022-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * official jinfochk version
 */
#define JINFOCHK_VERSION "0.14 2022-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * official jauthchk version
 */
#define JAUTHCHK_VERSION "0.13 2022-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * official jstrencode version
 */
#define JSTRENCODE_VERSION "0.5 2022-04-17"	/* format: major.minor YYYY-MM-DD */

/*
 * official jstrdecode version
 */
#define JSTRDECODE_VERSION "0.5 2022-04-17"	/* format: major.minor YYYY-MM-DD */

/*
 * official jnum_chk version
 */
#define JNUM_CHK_VERSION "0.8 2022-05-01"	/* format: major.minor YYYY-MM-DD */

/*
 * official jnum_gen version
 */
#define JNUM_GEN_VERSION "0.7 2022-05-01"	/* format: major.minor YYYY-MM-DD */

/*
 * minimum bison version needed to build the JSON parser
 */
#define MIN_BISON_VERSION "3.8.2"			/* from: bison -V 2>&1 | head -n 1 | awk '{print $NF}' */

/*
 * minimum flex version needed to build the JSON parser
 */
#define MIN_FLEX_VERSION "2.6.4"			/* from: flex -V 2>&1 | head -n 1 | awk '{print $NF}' */

/*
 * official verge tool version
 */
#define VERGE_VERSION "0.1 2022-04-02"		/* format: major.minor YYYY-MM-DD */


/*
 * debug version
 *
 * Because the dbg is a self contained system, we cannot define DBG_VERSION here.
 * See dbg.h for the DBG_VERSION value.
 */


/*
 * dynamic array version
 *
 * Because the dynamic array is a self contained system, we cannot define DYN_ALLOC_VERSION here.
 * Because the dynamic array is a self contained system, we cannot define DYN_TEST_VERSION here.
 * See dyn_array.h for the DYN_ALLOC_VERSION value.
 * See dyn_test.h for the DYN_TEST_VERSION value.
 */


 /*
  * JSON parser version
  *
  * Because the JSON parser is a self contained system, we cannot define JSON_PARSER_VERSION here.
  * Because the JSON parser is a self contained system, we cannot define JPARSE_VERSION here.
  * See jparse.h for the JSON_PARSER_VERSION value.
  * See jparse_main.h for the JPARSE_VERSION value.
  */


#endif /* INCLUDE_VERSION_H */
