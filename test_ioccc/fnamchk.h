/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 *
 * "Because most people become disappointed when someone gets their name wrong." :-)
 *
 * Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.  All Rights Reserved.
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
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * This tool was co-developed in 2022 by Cody Boone Ferguson and Landon Curt Noll:
 *
 *  @xexyl
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
