/*
 * verge - determine if first version is greater or equal to the second version
 *
 * "Because too much minimalism can be sub-minimal." :-)
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


#if !defined(INCLUDE_VERGE_H)
#    define  INCLUDE_VERGE_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/dbg.h"
#else
#include <dbg.h>
#endif

/*
 * util - common utility functions for the JSON parser
 */
#include "util.h"

/*
 * json_utf8.h - JSON UTF-8 decoder
 */
#include "json_utf8.h"

/*
 * version - JSON parser API and tool version
 */
#include "version.h"

/*
 * official verge tool version
 */
#define VERGE_VERSION "1.2.0 2024-10-09"		/* format: major.minor YYYY-MM-DD */


/*
 * verge tool basename
 */
#define VERGE_BASENAME "verge"



/*
 * globals
 */

/*
 * externals
 */

/*
 * function prototypes
 */
static size_t allocate_vers(char *str, intmax_t **pvers);


#endif /* INCLUDE_VERGE_H */
