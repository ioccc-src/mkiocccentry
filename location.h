/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * location - location/country codes
 *
 * "Because there is an I in IOCCC." :-)
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


#if !defined(INCLUDE_LOCATION_H)
#    define  INCLUDE_LOCATION_H

/*
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * location/country codes
 */
struct location
{
    const char * const code;		/* ISO 3166-1 Alpha-2 Code */
    const char * const name;		/* name (short name lower case) */
};

/*
 * global variables
 */
extern struct location loc[];		/* location/country codes */
extern size_t SIZEOF_LOCATION_TABLE;

/*
 * function prototypes
 */
extern void check_location_table(void);
extern char const *lookup_location_name(char const *upper_code);

#endif				/* INCLUDE_LOCATION_H */
