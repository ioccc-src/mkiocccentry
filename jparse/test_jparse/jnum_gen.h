/*
 * jnum_gen - generate JSON number string conversion test data
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


#if !defined(INCLUDE_JNUM_GEN_H)
#    define  INCLUDE_JNUM_GEN_H

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
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../../dyn_array/dyn_array.h"
#else
#include <dyn_array.h>
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
 * official jnum_gen version
 */
#define JNUM_GEN_VERSION "1.0.1 2024-03-02"	/* format: major.minor YYYY-MM-DD */

/*
 * jnum_gen tool basename
 */
#define JNUM_GEN_BASENAME "jnum_gen"



/*
 * globals
 */


/*
 * function prototypes
 */
static void fpr_number(FILE *stream, struct json_number *item);
static void fpr_info(FILE *stream, bool sized, intmax_t value, char const *scomm, char const *vcomm);
static void fpr_uinfo(FILE *stream, bool sized, uintmax_t value, char const *scomm, char const *vcomm);
static void fpr_finfo(FILE *stream, bool sized, long double value, bool intval, char const *scomm,
		      char const *vcomm, char const *sintval, char const *suffix);


#endif /* INCLUDE_JNUM_GEN_H */
