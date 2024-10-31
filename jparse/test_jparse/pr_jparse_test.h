/*
 * pr_jparse_test - test print function call
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This JSON parser was co-developed in 2022 by:
 *
 *	@xexyl
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
 * json_utf8.h - JSON UTF-8 encoder
 */
#include "../json_utf8.h"

/*
 * official pr_jparse_test version
 */
#define PR_JPARSE_TEST_VERSION "1.2.0 2024-10-09"	/* format: major.minor YYYY-MM-DD */

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
