/*
 * location - location/country codes and set the IOCCC locale
 *
 * "Because there is an I in IOCCC." :-)
 *
 * Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.
 * All Rights Reserved.
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
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This tool was co-developed in 2022-2025 by Cody Boone Ferguson and Landon
 * Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 *
 * Share and enjoy! :-)
 */


#if !defined(INCLUDE_LOCATION_H)
#    define  INCLUDE_LOCATION_H

#include <ctype.h>
#include <locale.h>


/*
 * jparse - the JSON parser
 */
#include "../jparse/jparse.h"


/*
 * jparse/version - the JSON parser version
 */
#include "../jparse/version.h"

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"



/*
 * macros
 */

/*
 * location tool basename
 */
#define LOCATION_BASENAME "location"

/*
 * location/country codes
 */
struct location
{
    const char * const code;		/* ISO 3166-1 Alpha-2 Code - 2 ASCII UPPER CASE chars */
    const char * const name;		/* "Officially Known As" name from Wikipedia, if known, else "Common" name */
    const char * const common_name;	/* "Common" ISO 3166-1 name (using title case) */
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
extern char const *lookup_location_name(char const *code, bool use_common);
extern char const *lookup_location_code(char const *location_name, bool use_common);
extern char const *lookup_location_name_r(char const *code, size_t *idx, struct location **location,
					  bool substrings, bool use_common);
extern char const *lookup_location_code_r(char const *location_name, size_t *idx, struct location **location,
					  bool substrings, bool use_common);
extern bool location_code_name_match(char const *code, char const *location_name, bool use_common);
extern void set_ioccc_locale(void);

#endif				/* INCLUDE_LOCATION_H */
