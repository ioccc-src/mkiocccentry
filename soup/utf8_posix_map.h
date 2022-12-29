/*
 * utf8_posix_map - translate UTF-8 into POSIX portable filename and + chars
 *
 * "Because even POSIX needs an extra plus." :-)
 *
 * An author_handle, for an IOCCC winner, will be used to form
 * a winner_handle.  These winner handles will become part of a
 * JSON filename on the www.ioccc.org website.  For this reason,
 * (and other reasons), we must restrict an author_handle to
 * only lower case POSIX portable filenames, with the addition of the +,
 * and restrictions on leading characters.
 *
 * The author_handle (and winner_handle) must fit the following
 * regular expression:
 *
 *	^[0-9a-z][0-9a-z._+-]*$
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
 * Share and enjoy! :-)
 */


#if !defined(INCLUDE_UTF8_POSIX_MAP_H)
#    define  INCLUDE_UTF8_POSIX_MAP_H


/*
 * jparse - the parser
 */
#include "../jparse/jparse.h"

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * map certain UTF-8 strings into safe lower case POSIX portable filenames plus +.
 */
struct utf8_posix_map
{
    const char * const utf8_str;	/* UTF-8 string encode - use \x hex as needed */
    const char * const posix_str;	/* POSIX portable filenames plus + replacement for utf8_str */
    int utf8_str_len;			/* length of utf8_str or -1 ==> needs computing by check_utf8_posix_map() */
    int posix_str_len;			/* length of posix_str or -1 ==> needs computing by check_utf8_posix_map() */
};

/*
 * global variables
 */
extern struct utf8_posix_map hmap[];	/* name to author handle map */
extern size_t SIZEOF_UTF8_POSIX_MAP;

/*
 * function prototypes
 */
extern void check_utf8_posix_map(void);
extern char *default_handle(char const *name);


#endif				/* INCLUDE_UTF8_POSIX_MAP_H */
