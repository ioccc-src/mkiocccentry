/*
 * jnum_chk - tool to check JSON number string conversions
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
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


#if !defined(INCLUDE_JNUM_CHK_H)
#    define  INCLUDE_JNUM_CHK_H


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
 * official jnum_chk version
 */
#define JNUM_CHK_VERSION "2.0.1 2025-03-15"      /* format: major.minor YYYY-MM-DD */


/*
 * jnum_chk tool basename
 */
#define JNUM_CHK_BASENAME "jnum_chk"


/*
 * globals
 *
 * Note that the include guard must be here and the quiet must also be in this
 * file. As well the JNUM_TEST must be defined in jnum_test.c.
 */
#if !defined(JNUM_TEST)
static bool quiet = false;				/* true ==> quiet mode */
#endif

/*
 * externals
 */
extern int const test_count;			/* number of tests to perform */
extern char *test_set[];			/* test strings */
extern struct json_number test_result[];	/* struct integer conversions of test strings */

/*
 * function prototypes
 */
#if !defined(JNUM_TEST)
static bool chk_test(int testnum, struct json_number *item, struct json_number *test, size_t len, bool strict);
static void check_val(bool *testp, char const *type, int testnum, bool size_a, bool size_b, intmax_t val_a, intmax_t val_b);
static void check_uval(bool *testp, char const *type, int testnum, bool size_a, bool size_b, uintmax_t val_a, uintmax_t val_b);
static void check_fval(bool *testp, char const *type, int testnum, bool size_a, bool size_b,
		       long double val_a, long double val_b, bool int_a, bool int_b, bool strict);
#endif

#endif /* INCLUDE_JNUM_CHK_H */
