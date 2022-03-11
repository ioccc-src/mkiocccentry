/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * mkiocccentry -  official IOCCC toolkit versions
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla C.
 * Why?  Because this is a obfuscated C contest.  But then why isn't
 * this code obfuscated?  Because the IOCCC judges prefer to write
 * in robust unobfuscated code.  Besides, the IOCCC was started
 * as an ironic commentary on the Bourne shell source and finger daemon
 * source.  Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, let the judges know.
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
 * overall IOCCC entry toolkit release version
 */
#define IOCCC_TOOLSET_RELEASE "0.2"		/* format: major.minor */


/*
 * official iocccsize version
 */
#define IOCCCSIZE_VERSION "28.9 2022-02-23"	/* format: major.minor YYYY-MM-DD */


/*
 * official mkiocccentry version
 */
#define MKIOCCCENTRY_VERSION "0.39 2022-03-04"	/* format: major.minor YYYY-MM-DD */


/*
 * Version of info for JSON the .info.json file.
 *
 * The following is NOT the version of this mkiocccentry tool!
 */
#define INFO_VERSION "1.8 2022-02-13"		/* format: major.minor YYYY-MM-DD */


/*
 * Version of info for JSON the .author.json file.
 *
 * The following is NOT the version of this mkiocccentry tool!
 */
#define AUTHOR_VERSION "1.11 2022-02-23"	/* format: major.minor YYYY-MM-DD */


/*
 * official fnamchk version
 */
#define FNAMCHK_VERSION "0.5 2022-03-04"	/* format: major.minor YYYY-MM-DD */

/*
 * official txzchk version
 */
#define TXZCHK_VERSION "0.9 2022-03-04"		/* format: major.minor YYYY-MM-DD */

/*
 * official jinfochk version
 */
#define JINFOCHK_VERSION "0.13 2022-03-07"	/* format: major.minor YYYY-MM-DD */

/*
 * official jauthchk version
 */
#define JAUTHCHK_VERSION "0.12 2022-03-04"	/* format: major.minor YYYY-MM-DD */


/*
 * official jstrencode version
 */
#define JSTRENCODE_VERSION "0.3 2022-03-04"	/* format: major.minor YYYY-MM-DD */


/*
 * official jstrdecode version
 */
#define JSTRDECODE_VERSION "0.3 2022-03-04"	/* format: major.minor YYYY-MM-DD */


#endif /* INCLUDE_VERSION_H */
