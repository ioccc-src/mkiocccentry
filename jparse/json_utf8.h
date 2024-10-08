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

extern bool count_utf8_bytes(const char *str, int32_t surrogate, size_t *bytes);

/*
 * NOTE: until the bug documented at https://github.com/xexyl/jparse/issues/13
 * is resolved fully, we have code here that comes from a number of locations.
 * Once the bug is resolved this file will be cleaned up. There are two
 * different locations at this time (29 Sep 2024).
 */

/*
 * The below comes from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with pointer checks added to the functions.
 */

/*
 * UTF8 valid ranges.
 *
 * The UTF-8 encoding spreads the bits of a 32bit word over several
 * bytes. This table gives the ranges that can be held and how they'd
 * be represented.
 *
 * 0x00000000 0x0000007F: 0xxxxxxx
 * 0x00000000 0x000007FF: 110xxxxx 10xxxxxx
 * 0x00000000 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
 * 0x00000000 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x00000000 0x03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x00000000 0x7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * There is an additional requirement on UTF-8, in that only the
 * shortest representation of a 32bit value is to be used.  A decoder
 * must not decode sequences that do not satisfy this requirement.
 * Thus the allowed ranges have a lower bound.
 *
 * 0x00000000 0x0000007F: 0xxxxxxx
 * 0x00000080 0x000007FF: 110xxxxx 10xxxxxx
 * 0x00000800 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
 * 0x00010000 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x00200000 0x03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 * 0x04000000 0x7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
 *
 * Actual unicode characters are limited to the range 0x0 - 0x10FFFF,
 * 17 planes of 65536 values.  This limits the sequences actually seen
 * even more, to just the following.
 *
 *          0 -     0x7f: 0                     0x7f
 *       0x80 -    0x7ff: 0xc2 0x80             0xdf 0xbf
 *      0x800 -   0xffff: 0xe0 0xa0 0x80        0xef 0xbf 0xbf
 *    0x10000 - 0x10ffff: 0xf0 0x90 0x80 0x80   0xf4 0x8f 0xbf 0xbf
 *
 * Even within those ranges not all values are allowed: the surrogates
 * 0xd800 - 0xdfff should never be seen.
 *
 * Note that the longest sequence seen with valid usage is 4 bytes,
 * the same a single UTF-32 character.  This makes the UTF-8
 * representation of Unicode strictly smaller than UTF-32.
 *
 * The shortest sequence requirement was introduced by:
 *    Corrigendum #1: UTF-8 Shortest Form
 * It can be found here:
 *    http://www.unicode.org/versions/corrigendum1.html
 *
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

/*
 * The above comes from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with pointer checks added to the functions.
 */


/*
 * The below is from https://github.com/benkasminbullock/unicode-c/, which is 'a
 * Unicode library in the programming language C which deals with conversions to
 * and from the UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */

/*
 * This macro converts four bytes of UTF-8 into the corresponding code point.
 */
#define FOUR(x)							\
      (((int32_t) (x[0] & 0x07)) << 18)				\
    | (((int32_t) (x[1] & 0x3F)) << 12)				\
    | (((int32_t) (x[2] & 0x3F)) <<  6)				\
    | (((int32_t) (x[3] & 0x3F)))

/* Reject code points which end in either FFFE or FFFF. */
#define REJECT_FFFF(x)				\
    if ((x & 0xFFFF) >= 0xFFFE) {		\
	return UNICODE_NOT_CHARACTER;		\
    }
/* Reject code points in a certain range. */
#define REJECT_NOT_CHAR(r)					\
    if (r >= UNI_NOT_CHAR_MIN && r <= UNI_NOT_CHAR_MAX) {	\
	return UNICODE_NOT_CHARACTER;				\
    }

#define REJECT_FE_FF(c)				\
    if (c == 0xFF || c == 0xFE) {		\
	return UNICODE_NOT_CHARACTER;		\
    }

/* Surrogate pair zone. */
#define UNI_SUR_HIGH_START  0xD800
#define UNI_SUR_HIGH_END    0xDBFF
#define UNI_SUR_LOW_START   0xDC00
#define UNI_SUR_LOW_END     0xDFFF

/* Reject surrogates. */
#define REJECT_SURROGATE(ucs2)						\
    if (ucs2 >= UNI_SUR_HIGH_START && ucs2 <= UNI_SUR_LOW_END) {	\
	/* Ill-formed. */						\
	return UNICODE_SURROGATE_PAIR;					\
    }

/* Start of the "not character" range. */
#define UNI_NOT_CHAR_MIN    0xFDD0
/* End of the "not character" range. */
#define UNI_NOT_CHAR_MAX    0xFDEF

/* For shifting by 10 bits. */
#define TEN_BITS 10
#define HALF_BASE 0x0010000UL

/*
 * The maximum number of bytes we need to contain any Unicode code point as
 * UTF-8 as a C string. This length includes one trailing nul byte.
 */
#define UTF8_MAX_LENGTH 5
/*
 * The maximum possible value of a Unicode code point. See
 * http://www.cl.cam.ac.uk/~mgk25/unicode.html#ucs.
 */
#define UNICODE_MAXIMUM 0x10ffff
/* The maximum possible value which will fit into four bytes of UTF-8. This is
 * larger than UNICODE_MAXIMUM.
 */
#define UNICODE_UTF8_4 0x1fffff
/*
 * This return value indicates the successful completion of a routine which
 * doesn't use the return value to communicate data back to the caller.
 */
#define UNICODE_OK 0
/*
 * This return value means that the leading byte of a UTF-8 sequence was not
 * valid.
 */
#define UTF8_BAD_LEADING_BYTE -1
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
 * This return value means that input which was supposed to be UTF-8 encoded
 * contained an invalid continuation byte. If the leading byte of a UTF-8
 * sequence is not valid, UTF8_BAD_LEADING_BYTE is returned instead of this.
 */
#define UTF8_BAD_CONTINUATION_BYTE -4
/*
 * This return value indicates a zero byte was found in a string which was
 * supposed to contain UTF-8 bytes. It is returned only by the functions which
 * are documented as not allowing zero bytes.
 */
#define UNICODE_EMPTY_INPUT -5
/*
 * This return value indicates that UTF-8 bytes were not in the shortest
 * possible form. See http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8.  This
 * return value is currently unused. If a character is not in the shortest form,
 * the error UTF8_BAD_CONTINUATION_BYTE is returned.
 */
#define UTF8_NON_SHORTEST -6
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

extern const uint8_t utf8_sequence_len[];

/*
 * All of the functions in this library return an "int32_t". Negative values are
 * used to indicate errors.
 */
extern int32_t utf8_to_ucs2 (const uint8_t* input, const uint8_t** end_ptr);
extern int32_t surrogates_to_unicode (int32_t hi, int32_t lo);
extern int32_t unicode_count_chars (const uint8_t* utf8);

/*
 * The above is from https://github.com/benkasminbullock/unicode-c/, which is 'a
 * Unicode library in the programming language C which deals with conversions to
 * and from the UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */

#endif /* INCLUDE_JSON_UTF8_H */
