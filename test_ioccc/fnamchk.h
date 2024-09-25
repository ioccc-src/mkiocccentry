/*
 * fnamchk - IOCCC compressed tarball filename sanity check tool
 *
 * "Because even fprintf has a return value worth paying attention to." :-)
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because everyone hates when someone gets their name wrong." :-)
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
 * utf8_posix_map - translate UTF-8 into POSIX portable filename and + chars
 */
#include "../soup/utf8_posix_map.h"

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
