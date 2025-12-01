/*
 * mkiocccentry - form IOCCC entry compressed tarball
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
 * This tool was co-developed in 2021-2025 by Cody Boone Ferguson and Landon
 * Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_MKIOCCCENTRY_H)
#    define  INCLUDE_MKIOCCCENTRY_H


/*
 * jparse/version - JSON parser API and tool version
 */
#include "jparse/version.h"

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg/dbg.h"

/*
 * canon_path - canonicalize paths
 */
#include "cpath/cpath.h"

/*
 * location - location/country codes and set the IOCCC locale
 */
#include "soup/location.h"

/*
 * default_handle - translate UTF-8 into handle that is POSIX portable and + chars
 */
#include "soup/default_handle.h"

/*
 * sanity - perform common IOCCC sanity checks
 */
#include "soup/sanity.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "soup/limit_ioccc.h"

/*
 * iocccsize use
 */
#include "iocccsize.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "soup/version.h"

/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "soup/entry_util.h"

/*
 * file_util - utilities for files
 */
#include "soup/file_util.h"

/*
 * util - various utils
 */
#include "soup/util.h"

/*
 * random_answers - generate a random answer file for mkiocccentry
 */
#include "soup/random_answers.h"

/*
 * walk - walk directory trees and tar listings
 */
#include "soup/walk.h"


/*
 * definitions
 *
 */
#define MKIOCCCENTRY_BASENAME "mkiocccentry"	/* mkiocccentry tool basename for -V/-h */
#define ISO_3166_1_CODE_URL0 "\thttps://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements"
#define ISO_3166_1_CODE_URL1 "\thttps://en.wikipedia.org/wiki/ISO_3166-1_alpha-2"
#define ISO_3166_1_CODE_URL2 "\thttps://www.iso.org/obp/ui/#iso:pub:PUB500001:en"
#define ISO_3166_1_CODE_URL3 "\thttps://www.iso.org/obp/ui/#search"
#define ISO_3166_1_CODE_URL4 "\thttps://www.iso.org/glossary-for-iso-3166.html"

#define IOCCC_REGISTER_URL "https://www.freelists.org/list/ioccc29-reg" /* for mailing list to register for IOCCC29 */
#define IOCCC_SUBMIT_URL "https://submit.ioccc.org"	/* URL to submit to contest */
#define IOCCC_NEWS_URL "https://www.ioccc.org/news.html"
#define IOCCC_REGISTER_FAQ_URL "https://www.ioccc.org/faq.html#enter"
#define IOCCC_REGISTER_INFO_URL "https://www.ioccc.org/next/register.html"
#define IOCCC_PW_CHANGE_INFO_URL "https://www.ioccc.org/next/pw-change.html"
#define IOCCC_SUBMIT_INFO_URL "https://www.ioccc.org/next/submit.html"
#define IOCCC_REGISTRATION_READY
#define IOCCC_SUBMIT_SERVER_READY

/* NOTE: The next two are for the warn_rule_2a_size() function. Do **NOT** change these values! */
#define RULE_2A_BIG_FILE_WARNING (0)	/* warn that prog.c appears to be too big under Rule 2a */
#define RULE_2A_IOCCCSIZE_MISMATCH (1)	/* warn that prog.c iocccsize size differs from the file size */


#endif /* INCLUDE_MKIOCCCENTRY_H */
