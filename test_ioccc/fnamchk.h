/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 *
 * "Because even fprintf has a return value worth paying attention to." :-)
 *
 * "Because everyone hates when someone gets their name wrong." :-)
 *
 * This tool was co-developed in 2022 by Cody Boone Ferguson and Landon Curt
 * Noll:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and enjoy! :-)
 */

#if !defined(INCLUDE_FNAMCHK_H)
#    define  INCLUDE_FNAMCHK_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * jparse/version - JSON parser API and tool version
 */
#include "../jparse/version.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "../soup/limit_ioccc.h"

/*
 * default_handle - translate UTF-8 into handle that is POSIX portable and + chars
 */
#include "../soup/default_handle.h"

/*
 * fnamchk tool basename
 */
#define FNAMCHK_BASENAME "fnamchk"

/*
 * globals
 */


/*
 * forward declarations
 */


#endif /* INCLUDE_FNAMCHK_H */
