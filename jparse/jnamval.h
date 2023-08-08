/* jnamval - JSON value printer
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


#if !defined(INCLUDE_JNAMVAL_H)
#    define  INCLUDE_JNAMVAL_H

#define _GNU_SOURCE /* feature test macro for strcasestr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h> /* for -g, regular expression matching */
#include <strings.h> /* for -i, strcasecmp */

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
 * jnamval_test - test functions
 */
#include "jnamval_test.h"

/*
 * jparse - JSON parser
 */
#include "jparse.h"

/* jnamval version string */
#define JNAMVAL_VERSION "0.0.14 2023-08-08"		/* format: major.minor YYYY-MM-DD */

/* jnamval functions - see jnamval_util.h for most */

/* sanity checks on environment for specific options and the tool arguments */
FILE *jnamval_sanity_chks(struct jnamval *jnamval, char const *program, int *argc, char ***argv);

#endif /* !defined INCLUDE_JNAMVAL_H */
