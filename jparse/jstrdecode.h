/*
 * jstrdecode - tool to JSON decode JSON encoded strings
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


#if !defined(INCLUDE_JSTRDECODE_H)
#    define  INCLUDE_JSTRDECODE_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * util - common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"


/*
 * globals
 */


/*
 * function prototypes
 */
static bool jstrdecode_stream(FILE *in_stream, FILE *out_stream, bool write_quote);


#endif /* INCLUDE_JSTRDECODE_H */
