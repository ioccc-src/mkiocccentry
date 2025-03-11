/*
 * json_utf8 - JSON UTF-8 encoder/decoder
 *
 * "Because ASCII text is too limited, even for JSON." :-)
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
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


#if !defined(INCLUDE_JSON_UTF8_H)

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/dbg.h"
#elif defined(INTERNAL_INCLUDE_2)
#include "dbg/dbg.h"
#else
#include <dbg.h>
#endif

/*
 * util - common utility functions for the JSON parser
 */
#include "util.h"

/*
 * official jparse UTF-8 version
 */
#define JPARSE_UTF8_VERSION "2.1.0 2025-02-28"	/* format: major.minor YYYY-MM-DD */


extern size_t utf8len(const char *str, int32_t surrogate);

/*
 * The below function and macros are based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with a number of changes made by us.
 */

#define UTF8_2_BITS     0xC0
#define UTF8_3_BITS     0xE0
#define UTF8_4_BITS     0xF0
#define UTF8_N_BITS     0x80
#define UTF8_2_MASK     0xE0
#define UTF8_3_MASK     0xF0
#define UTF8_4_MASK     0xF8
#define UTF8_N_MASK     0xC0
#define UTF8_V_MASK     0x3F
#define UTF8_V_SHIFT    6

/*
 * NOTE: the original function name is utf8encode() but we have
 * renamed it because in JSON in BOTH encoding and decoding it should convert
 * \uxxxx to unicode. This is by all sources seen called encoding but since this
 * is a JSON library, to be less confusing, we call it utf8_to_unicode().
 */
extern int utf8_to_unicode(char *str, unsigned int val);

/*
 * The above function and macros are based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with a number of changes made by us.
 */

/*
 * -=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=
 */

/*
 * The below function (with some extra sanity checks added by us) and macros
 * come from https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode
 * library in the programming language C which deals with conversions to and
 * from the UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */

/* Surrogate pair zone. */
#define UNI_SUR_HIGH_START  0xD800
#define UNI_SUR_HIGH_END    0xDBFF
#define UNI_SUR_LOW_START   0xDC00
#define UNI_SUR_LOW_END     0xDFFF

/* For shifting by 10 bits. */
#define TEN_BITS 10
#define HALF_BASE 0x0010000UL

/*
 * This return value means the caller attempted to turn a code point for a
 * surrogate pair to or from UTF-8.
 */
#define UNICODE_SURROGATE_PAIR -2
/*
 * This return value means that code points which did not form a surrogate pair
 * were tried to be converted into a code point as if they were a surrogate
 * pair.
 */
#define UNICODE_NOT_SURROGATE_PAIR -3

/*
 * This return value indicates that there was an attempt to convert a code point
 * which was greater than UNICODE_MAXIMUM or UNICODE_UTF8_4 into UTF-8 bytes.
 */
#define UNICODE_TOO_BIG -7

extern int32_t surrogates_to_unicode (int32_t hi, int32_t lo);


/*
 * The above function (with some extra sanity checks added by us) and macros
 * come from https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode
 * library in the programming language C which deals with conversions to and
 * from the UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */

/*
 * -=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=
 */

#endif /* INCLUDE_JSON_UTF8_H */
