/*
 * jstrencode - tool to encode a string for JSON
 *
 * "JSON: when a minimal design falls below a critical minimum." :-)
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


#if !defined(INCLUDE_JSTRENCODE_H)
#    define  INCLUDE_JSTRENCODE_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/dbg.h"
#else
#include <dbg.h>
#endif

/*
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dyn_array/dyn_array.h"
#else
#include <dyn_array.h>
#endif


/*
 * util - common utility functions for the JSON parser
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * jstr_util - jstrencode/jstrdecode utilities
 */
#include "jstr_util.h"

/*
 * jparse - JSON parser
 */
#include "jparse.h"

/*
 * version - JSON parser API and tool version
 */
#include "version.h"

/*
 * official jstrencode version
 */
#define JSTRENCODE_VERSION "1.2.0 2024-10-09"	/* format: major.minor YYYY-MM-DD */


/*
 * jstrencode tool basename
 */
#define JSTRENCODE_BASENAME "jstrencode"


/*
 * globals
 */


#endif /* INCLUDE_JSTRENCODE_H */
