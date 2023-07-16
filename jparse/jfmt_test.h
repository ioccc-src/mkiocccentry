/* jfmt_test - test functions for the jfmt tool
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool is being developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * The JSON parser was co-developed in 2022 by Cody and Landon.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_JFMT_TEST_H)
#    define  INCLUDE_JFMT_TEST_H

#include <stdlib.h>
#include <unistd.h>
#include <regex.h> /* for -g, regular expression matching */
#include <string.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * dyn_array - dynamic array facility
 */
#include "../dyn_array/dyn_array.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * jparse - JSON parser
 */
#include "jparse.h"

/*
 * jfmt_util - jfmt utility functions
 */
#include "jfmt_util.h"

bool jfmt_run_tests(void);
bool jfmt_test_number_range_opts(bool expected, intmax_t number, intmax_t total_matches,
	intmax_t line, struct jfmt_number *range);
bool jfmt_test_bits(bool expected, uintmax_t set_bits, intmax_t line, bool (*check_func)(uintmax_t), const char *name);

#endif /* !defined INCLUDE_JFMT_TEST_H */
