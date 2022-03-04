/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * utf8_posix_map - translate certain UTF-8 into POSIX+ safe filenames
 *
 * "Because even POSIX needs an extra plus." :-)
 *
 * An author_handle, for an IOCCC winner, will be used to form
 * a winner_handle.  These winner_handle's will become part of a
 * JSON filename on the www.ioccc.org web site.  For this reason,
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
 * map certain UTF-8 strings into safe lower case POSIX portable filenames plus +.
 */
struct utf8_posix_map {
    char *utf8_str;	/* UTF-8 string encode - use \x hex as needed */
    char *posix_str;	/* POSIX portable filenames plus + replacement for utf8_str */
};

/*
 * global variables
 */
extern struct utf8_ascii_map hmap[];	/* name to author handle map */


#endif				/* INCLUDE_UTF8_POSIX_MAP_H */
