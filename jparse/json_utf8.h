/*
 * json_utf8 - JSON UTF-8 decoder
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
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

#if !defined(INCLUDE_JSON_UTF8_H)

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
 * official jparse UTF-8 version
 */
#define JPARSE_UTF8_VERSION "2.0.0 2024-10-3`"	/* format: major.minor YYYY-MM-DD */


extern size_t utf8len(const char *str, int32_t surrogate);
extern bool is_unicode_noncharacter(int32_t x);

/*
 * The below functions and macros are based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with a number of changes.
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

extern int utf8encode(char *str, unsigned int val);
extern unsigned int utf8decode(const char *str);

/*
 * The above functions and macros are based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with a number of changes.
 */

/*
 * -=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=
 */

/*
 * The below function and macros come from
 * https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode library
 * in the programming language C which deals with conversions to and from the
 * UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */

/* Surrogate pair zone. */
#define UNI_SUR_HIGH_START  0xD800
#define UNI_SUR_HIGH_END    0xDBFF
#define UNI_SUR_LOW_START   0xDC00
#define UNI_SUR_LOW_END     0xDFFF

/* Start of the "not character" range. */
#define UNI_NOT_CHAR_MIN    0xFDD0
/* End of the "not character" range. */
#define UNI_NOT_CHAR_MAX    0xFDEF

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
/*
 * This return value indicates that the Unicode code-point ended with either
 * 0xFFFF or 0xFFFE, meaning it cannot be used as a character code point, or it
 * was in the disallowed range FDD0 to FDEF.
 */
#define UNICODE_NOT_CHARACTER -8

extern int32_t surrogates_to_unicode (int32_t hi, int32_t lo);

/*
 * The above macros and function from
 * https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode library
 * in the programming language C which deals with conversions to and from the
 * UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */

/*
 * -=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=
 */

#endif /* INCLUDE_JSON_UTF8_H */
