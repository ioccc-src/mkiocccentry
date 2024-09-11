/*
 * jnum_chk - tool to check JSON number string conversions
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


#if !defined(INCLUDE_JNUM_CHK_H)
#    define  INCLUDE_JNUM_CHK_H


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
