/*
 * pr_jparse_test - test print function calls
 *
 * Here "print function call" refers to functions such as:
 *
 *    printf(3), fprintf(3), dprintf(3),
 *    vprintf(3), vfprintf(3), vdprintf(3)
 *
 * We also test various print-like functions from util.c such as:
 *
 *    fprint(), fprstr(), print(), prstr(), para(), fpara(), fpr(), vfpr()
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_PR_JPARSE_TEST_H)
#    define  INCLUDE_PR_JPARSE_TEST_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../../dbg/dbg.h"
#elif defined(INTERNAL_INCLUDE_2)
#include "../dbg/dbg.h"
#else
#include <dbg.h>
#endif


/*
 * util - common utility functions for the JSON parser
 */
#include "../util.h"

/*
 * json_parse - JSON parser support code
 */
#include "../json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "../json_util.h"

/*
 * jparse - JSON parser
 */
#include "../jparse.h"

/*
 * version - JSON parser API and tool version
 */
#include "../version.h"

/*
 * json_utf8.h - JSON UTF-8 decoder
 */
#include "../json_utf8.h"

/*
 * official pr_jparse_test version
 */
#define PR_JPARSE_TEST_VERSION "2.0.1 2025-03-15"	/* format: major.minor YYYY-MM-DD */

/*
 * pr_jparse_test tool basename
 */
#define PR_JPARSE_TEST_BASENAME "pr_jparse_test"


/*
 * globals
 *
 */

/*
 * externals
 */

#endif /* INCLUDE_PR_JPARSE_TEST_H */
