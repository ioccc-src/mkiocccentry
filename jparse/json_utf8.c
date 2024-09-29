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

#include <string.h>
#include <stdint.h>
#include "json_utf8.h"

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
int
utf8encode(char *str, unsigned int val)
{
    int len;

    /*
     * firewall
     */
    if (str == NULL) {
	err(10, __func__, "str is NULL");
	not_reached();
    }

    if (val < 0x80) {
	str[0] = val;
	len = 1;
    } else if (val < 0x800) {
	str[1] = val & UTF8_V_MASK;
	str[1] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[0] = val;
	str[0] |= UTF8_2_BITS;
	len = 2;
    } else if (val < 0x10000) {
	str[2] = val & UTF8_V_MASK;
	str[2] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[1] = val & UTF8_V_MASK;
	str[1] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[0] = val;
	str[0] |= UTF8_3_BITS;
	len = 3;
    } else if (val < 0x110000) {
	str[3] = val & UTF8_V_MASK;
	str[3] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[2] = val & UTF8_V_MASK;
	str[2] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[1] = val & UTF8_V_MASK;
	str[1] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[0] = val;
	str[0] |= UTF8_4_BITS;
	len = 4;
    } else {
	err(11, __func__, "%#x: illegal val\n", val);
	not_reached();
    }
    return len;
}

unsigned int
utf8decode(const char *str)
{
    const unsigned char *s = NULL;
    unsigned int unichar = 0;

    /*
     * firewall
     */
    if (str == NULL) {
	err(12, __func__, "str is NULL");
	not_reached();
    }

    s = (const unsigned char *)str;

    if (*s < 0x80) {
	unichar = *s;
    } else if (*s < UTF8_3_BITS) {
	unichar = *s++ & 0x1F;
	unichar <<= UTF8_V_SHIFT;
	unichar |= *s & 0x3F;
    } else if (*s < UTF8_4_BITS) {
	unichar = *s++ & 0x0F;
	unichar <<= UTF8_V_SHIFT;
	unichar |= *s++ & 0x3F;
	unichar <<= UTF8_V_SHIFT;
	unichar |= *s & 0x3F;
    } else {
	unichar = *s++ & 0x0F;
	unichar <<= UTF8_V_SHIFT;
	unichar |= *s++ & 0x3F;
	unichar <<= UTF8_V_SHIFT;
	unichar |= *s++ & 0x3F;
	unichar <<= UTF8_V_SHIFT;
	unichar |= *s & 0x3F;
    }
    return unichar;
}

/*
 * The above comes from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with pointer checks added to the functions.
 */

/*
 * The below table and code is from
 * https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode library
 * in the programming language C which deals with conversions to and from the
 * UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */


/* This is a Unicode library in the programming language C which deals
   with conversions to and from the UTF-8 format. */

/*
  Author:

  Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>

  Repository:

  https://github.com/benkasminbullock/unicode-c
*/


#ifdef HEADER

/*   _     _           _ _
    | |   (_)_ __ ___ (_) |_ ___
    | |   | | '_ ` _ \| | __/ __|
    | |___| | | | | | | | |_\__ \
    |_____|_|_| |_| |_|_|\__|___/ */



/* The maximum number of bytes we need to contain any Unicode code
   point as UTF-8 as a C string. This length includes one trailing nul
   byte. */

#define UTF8_MAX_LENGTH 5

/* The maximum possible value of a Unicode code point. See
   http://www.cl.cam.ac.uk/~mgk25/unicode.html#ucs. */

#define UNICODE_MAXIMUM 0x10ffff

/* The maximum possible value which will fit into four bytes of
   UTF-8. This is larger than UNICODE_MAXIMUM. */

#define UNICODE_UTF8_4 0x1fffff

/*   ____      _                                 _
    |  _ \ ___| |_ _   _ _ __ _ __   __   ____ _| |_   _  ___  ___
    | |_) / _ \ __| | | | '__| '_ \  \ \ / / _` | | | | |/ _ \/ __|
    |  _ <  __/ |_| |_| | |  | | | |  \ V / (_| | | |_| |  __/\__ \
    |_| \_\___|\__|\__,_|_|  |_| |_|   \_/ \__,_|_|\__,_|\___||___/ */


/* All of the functions in this library return an "int32_t". Negative
   values are used to indicate errors. */

/* This return value indicates the successful completion of a routine
   which doesn't use the return value to communicate data back to the
   caller. */

#define UNICODE_OK 0

/* This return value means that the leading byte of a UTF-8 sequence
   was not valid. */

#define UTF8_BAD_LEADING_BYTE -1

/* This return value means the caller attempted to turn a code point
   for a surrogate pair to or from UTF-8. */

#define UNICODE_SURROGATE_PAIR -2

/* This return value means that code points which did not form a
   surrogate pair were tried to be converted into a code point as if
   they were a surrogate pair. */

#define UNICODE_NOT_SURROGATE_PAIR -3

/* This return value means that input which was supposed to be UTF-8
   encoded contained an invalid continuation byte. If the leading byte
   of a UTF-8 sequence is not valid, UTF8_BAD_LEADING_BYTE is returned
   instead of this. */

#define UTF8_BAD_CONTINUATION_BYTE -4

/* This return value indicates a zero byte was found in a string which
   was supposed to contain UTF-8 bytes. It is returned only by the
   functions which are documented as not allowing zero bytes. */

#define UNICODE_EMPTY_INPUT -5

/* This return value indicates that UTF-8 bytes were not in the
   shortest possible form. See
   http://www.cl.cam.ac.uk/~mgk25/unicode.html#utf-8.

   This return value is currently unused. If a character is not in the
   shortest form, the error UTF8_BAD_CONTINUATION_BYTE is returned. */

#define UTF8_NON_SHORTEST -6

/* This return value indicates that there was an attempt to convert a
   code point which was greater than UNICODE_MAXIMUM or UNICODE_UTF8_4
   into UTF-8 bytes. */

#define UNICODE_TOO_BIG -7

/* This return value indicates that the Unicode code-point ended with
   either 0xFFFF or 0xFFFE, meaning it cannot be used as a character
   code point, or it was in the disallowed range FDD0 to FDEF. */

#define UNICODE_NOT_CHARACTER -8

/* This return value indicates that the UTF-8 is valid. It is only
   used by "valid_utf8". */

#define UTF8_VALID 1

/* This return value indicates that the UTF-8 is not valid. It is only
   used by "valid_utf8". */

#define UTF8_INVALID 0

#endif /* def HEADER */

/* This table contains the length of a sequence which begins with the
   byte given. A value of zero indicates that the byte can not begin a
   UTF-8 sequence. */

/* https://metacpan.org/source/CHANSEN/Unicode-UTF8-0.60/UTF8.xs#L8 */

const uint8_t utf8_sequence_len[0x100] =
{
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x00-0x0F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x10-0x1F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x20-0x2F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x30-0x3F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x40-0x4F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x50-0x5F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x60-0x6F */
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, /* 0x70-0x7F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x80-0x8F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0x90-0x9F */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xA0-0xAF */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, /* 0xB0-0xBF */
    0,0,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xC0-0xCF */
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, /* 0xD0-0xDF */
    3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3, /* 0xE0-0xEF */
    4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0, /* 0xF0-0xFF */
};

/* This function returns the number of bytes of UTF-8 a sequence
   starting with byte "c" will become, either 1 (c = 0000xxxx), 2 (c =
   110xxxxx), 3 (c = 1110xxxx), or 4 (c = 111100xx or c =
   11110100). If "c" is not a valid UTF-8 first byte, the value
   UTF8_BAD_LEADING_BYTE is returned. */

int32_t utf8_bytes (uint8_t c)
{
    int32_t r;
    r = utf8_sequence_len[c];
    if (r == 0) {
	return UTF8_BAD_LEADING_BYTE;
    }
    return r;
}

/* This macro converts four bytes of UTF-8 into the corresponding code
   point. */

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

/* Reject surrogates. */

#define REJECT_SURROGATE(ucs2)						\
    if (ucs2 >= UNI_SUR_HIGH_START && ucs2 <= UNI_SUR_LOW_END) {	\
	/* Ill-formed. */						\
	return UNICODE_SURROGATE_PAIR;					\
    }

/* Try to convert "input" from UTF-8 to UCS-2, and return a value even
   if the input is partly broken.  This checks the first byte of the
   input, but it doesn't check the subsequent bytes. */

int32_t
utf8_no_checks (const uint8_t * input, const uint8_t ** end_ptr)
{
    uint8_t c;
    c = input[0];
    switch (utf8_sequence_len[c]) {
    case 1:
        * end_ptr = input + 1;
        return c;

    case 2:
        * end_ptr = input + 2;
        return
            (c & 0x1F) << 6  |
            (input[1] & 0x3F);

    case 3:
        * end_ptr = input + 3;
        return
            (c & 0x0F) << 12 |
            (input[1] & 0x3F) << 6  |
            (input[2] & 0x3F);

    case 4:
        * end_ptr = input + 4;
	return FOUR (input);

    case 0:
	/* fall through */
    default:
	return UTF8_BAD_LEADING_BYTE;
    }
}

/* Surrogate pair zone. */

#define UNI_SUR_HIGH_START  0xD800
#define UNI_SUR_HIGH_END    0xDBFF
#define UNI_SUR_LOW_START   0xDC00
#define UNI_SUR_LOW_END     0xDFFF

/* Start of the "not character" range. */

#define UNI_NOT_CHAR_MIN    0xFDD0

/* End of the "not character" range. */

#define UNI_NOT_CHAR_MAX    0xFDEF

/* This function converts UTF-8 encoded bytes in "input" into the
   equivalent Unicode code point.  The return value is the Unicode
   code point corresponding to the UTF-8 character in "input" if
   successful, and a negative number if not successful.  Nul bytes are
   rejected.

   "*end_ptr" is set to the next character after the read character on
   success.  "*end_ptr" is set to the start of input on all failures.
   "end_ptr" may not be NULL.

   If the first byte of "input" is zero, in other words a NUL or '\0',
   UNICODE_EMPTY_INPUT is returned.

   If the first byte of "input" is not valid UTF-8,
   UTF8_BAD_LEADING_BYTE is returned.

   If the second or later bytes of "input" are not valid UTF-8,
   including NUL, UTF8_BAD_CONTINUATION_BYTE is returned.

   If the value extrapolated from "input" is greater than
   UNICODE_MAXIMUM, UNICODE_TOO_BIG is returned.

   If the value extrapolated from "input" ends in 0xFFFF or 0xFFFE,
   UNICODE_NOT_CHARACTER is returned.

   If the value extrapolated from "input" is between 0xFDD0 and 0xFDEF,
   UNICODE_NOT_CHARACTER is returned.

   If the value is within the range of surrogate pairs, the error
   UNICODE_SURROGATE_PAIR is returned.
*/

int32_t
utf8_to_ucs2 (const uint8_t * input, const uint8_t ** end_ptr)
{
    uint8_t c;
    uint8_t l;

    *end_ptr = input;
    c = input[0];
    if (c == 0) {
        return UNICODE_EMPTY_INPUT;
    }
    l = utf8_sequence_len[c];
    if (l == 1) {
        * end_ptr = input + 1;
        return (int32_t) c;
    }
    if (l == 2) {
	uint8_t d;
	d = input[1];
	/* Two byte case. */
        if (d < 0x80 || d > 0xBF) {
            return UTF8_BAD_CONTINUATION_BYTE;
	}
	if (c <= 0xC1) {
            return UTF8_BAD_CONTINUATION_BYTE;
	}
        * end_ptr = input + 2;
        return
            ((int32_t) (c & 0x1F) << 6)  |
            ((int32_t) (d & 0x3F));
    }
    if (l == 3) {
	uint8_t d;
	uint8_t e;
	int32_t r;

	d = input[1];
	e = input[2];
	/* Three byte case. */
        if (d < 0x80 || d > 0xBF ||
	    e < 0x80 || e > 0xBF) {
            return UTF8_BAD_CONTINUATION_BYTE;
	}
	if (c == 0xe0 && d < 0xa0) {
	    /* We don't need to check the value of input[2], because
	       the if statement above this one already guarantees that
	       it is 10xxxxxx. */
            return UTF8_BAD_CONTINUATION_BYTE;
	}
        r = ((int32_t) (c & 0x0F)) << 12 |
            ((int32_t) (d & 0x3F)) << 6  |
            ((int32_t) (e & 0x3F));
	REJECT_SURROGATE(r);
	REJECT_FFFF(r);
	REJECT_NOT_CHAR(r);
        * end_ptr = input + 3;
	return r;
    }
    else if (l == 4) {
	/* Four byte case. */
	uint8_t d;
	uint8_t e;
	uint8_t f;
	int32_t v;

	d = input[1];
	e = input[2];
	f = input[3];

	if (/* c must be 11110xxx. */
	    c >= 0xf8 ||
	    /* d, e, f must be 10xxxxxx. */
	    d < 0x80 || d >= 0xC0 ||
	    e < 0x80 || e >= 0xC0 ||
	    f < 0x80 || f >= 0xC0) {
	    return UTF8_BAD_CONTINUATION_BYTE;
	}

	if (c == 0xf0 && d < 0x90) {
	    /* We don't need to check the values of e and f, because
	       the if statement above this one already guarantees that
	       e and f are 10xxxxxx. */
            return UTF8_BAD_CONTINUATION_BYTE;
	}
	/* Calculate the code point. */
	v = FOUR (input);
	/* Greater than U+10FFFF */
	if (v > UNICODE_MAXIMUM) {
	    return UNICODE_TOO_BIG;
	}
	/* Non-characters U+nFFFE..U+nFFFF on plane 1-16 */
	REJECT_FFFF(v);
	/* We don't need to check for surrogate pairs here, since the
	   minimum value of UCS2 if there are four bytes of UTF-8 is
	   0x10000. */
        * end_ptr = input + 4;
	return v;
    }
    return UTF8_BAD_LEADING_BYTE;
}


/* Input: a Unicode code point, "ucs2".

   Output: UTF-8 characters in buffer "utf8".

   Return value: the number of bytes written into "utf8", or a
   negative number if there was an error.

   If the value of "ucs2" is invalid because of being in the surrogate
   pair range from 0xD800 to 0xDFFF, the return value is
   UNICODE_SURROGATE_PAIR.

   If the value of "ucs2" is in the range 0xFDD0 to 0xFDEF inclusive,
   the return value is UNICODE_NOT_CHARACTER.

   If the lower two bytes of "ucs2" are either 0xFFFE or 0xFFFF, the
   return value is UNICODE_NOT_CHARACTER.

   If the value is too big to fit into four bytes of UTF-8,
   UNICODE_UTF8_4, the return value is UNICODE_TOO_BIG.

   However, it does not insist on ucs2 being less than
   UNICODE_MAXIMUM, so the user needs to check that "ucs2" is a valid
   code point.

   This adds a zero byte to the end of the string. It assumes that the
   buffer "utf8" has at least UNICODE_MAX_LENGTH (5) bytes of space to
   write to, without checking. */

int32_t
ucs2_to_utf8 (int32_t ucs2, uint8_t * utf8)
{
    REJECT_FFFF(ucs2);
    if (ucs2 < 0x80) {
        utf8[0] = ucs2;
        utf8[1] = '\0';
        return 1;
    }
    if (ucs2 < 0x800) {
        utf8[0] = (ucs2 >> 6)   | 0xC0;
        utf8[1] = (ucs2 & 0x3F) | 0x80;
        utf8[2] = '\0';
        return 2;
    }
    if (ucs2 < 0xFFFF) {
        utf8[0] = ((ucs2 >> 12)       ) | 0xE0;
        utf8[1] = ((ucs2 >> 6 ) & 0x3F) | 0x80;
        utf8[2] = ((ucs2      ) & 0x3F) | 0x80;
        utf8[3] = '\0';
	REJECT_SURROGATE(ucs2);
	REJECT_NOT_CHAR(ucs2);
        return 3;
    }
    if (ucs2 <= UNICODE_UTF8_4) {
	/* http://tidy.sourceforge.net/cgi-bin/lxr/source/src/utf8.c#L380 */
	utf8[0] = 0xF0 | (ucs2 >> 18);
	utf8[1] = 0x80 | ((ucs2 >> 12) & 0x3F);
	utf8[2] = 0x80 | ((ucs2 >> 6) & 0x3F);
	utf8[3] = 0x80 | ((ucs2 & 0x3F));
        utf8[4] = '\0';
        return 4;
    }
    return UNICODE_TOO_BIG;
}

/* For shifting by 10 bits. */
#define TEN_BITS 10
#define HALF_BASE 0x0010000UL
/* 0b1111111111 */
#define LOW_TEN_BITS 0x3FF

/* This converts the Unicode code point in "unicode" into a surrogate
   pair, and returns the two parts in "* hi_ptr" and "* lo_ptr".

   Return value:

   If "unicode" does not need to be a surrogate pair, the error
   UNICODE_NOT_SURROGATE_PAIR is returned, and the values of "*hi_ptr"
   and "*lo_ptr" are undefined. If the conversion is successful,
   UNICODE_OK is returned. */

int32_t
unicode_to_surrogates (int32_t unicode, int32_t * hi_ptr, int32_t * lo_ptr)
{
    int32_t hi = UNI_SUR_HIGH_START;
    int32_t lo = UNI_SUR_LOW_START;
    if (unicode < HALF_BASE) {
	/* Doesn't need to be a surrogate pair. */
	return UNICODE_NOT_SURROGATE_PAIR;
    }
    unicode -= HALF_BASE;
    hi |= ((unicode >> TEN_BITS) & LOW_TEN_BITS);
    lo |= ((unicode) & LOW_TEN_BITS);
    * hi_ptr = hi;
    * lo_ptr = lo;
    return UNICODE_OK;
}

/* Convert a surrogate pair in "hi" and "lo" to a single Unicode
   value. The return value is the Unicode value. If the return value
   is negative, an error has occurred. If "hi" and "lo" do not form a
   surrogate pair, the error value UNICODE_NOT_SURROGATE_PAIR is
   returned.

   https://android.googlesource.com/platform/external/id3lib/+/master/unicode.org/ConvertUTF.c */

int32_t
surrogates_to_unicode (int32_t hi, int32_t lo)
{
    int32_t u;
    if (hi < UNI_SUR_HIGH_START || hi > UNI_SUR_HIGH_END ||
	lo < UNI_SUR_LOW_START || lo > UNI_SUR_LOW_END) {
	return UNICODE_NOT_SURROGATE_PAIR;
    }
    u = ((hi - UNI_SUR_HIGH_START) << TEN_BITS)
      + (lo - UNI_SUR_LOW_START) + HALF_BASE;
    return u;
}

#undef UNI_SUR_HIGH_START
#undef UNI_SUR_HIGH_END
#undef UNI_SUR_LOW_START
#undef UNI_SUR_LOW_END
#undef TEN_BITS
#undef HALF_BASE
#undef LOW_TEN_BITS

/* Convert the surrogate pair in "hi" and "lo" to UTF-8 in
   "utf8". This calls "surrogates_to_unicode" and "ucs2_to_utf8", thus
   it can return the same errors as them, and has the same restriction
   on "utf8" as "ucs2_to_utf8". */

int32_t
surrogate_to_utf8 (int32_t hi, int32_t lo, uint8_t * utf8)
{
    int32_t C;
    C = surrogates_to_unicode (hi, lo);
    if (C < 0) {
	return C;
    }
    return ucs2_to_utf8 (C, utf8);
}

/* Given a nul-terminated string "utf8" and a number of Unicode
   characters "n_chars", return the number of bytes into "utf8" at
   which the end of the characters occurs. A negative value indicates
   some kind of error. If "utf8" contains a zero byte, the return
   value is UNICODE_EMPTY_INPUT. This may also return any of the error
   values of "utf8_to_ucs2". */

int32_t
unicode_chars_to_bytes (const uint8_t * utf8, int32_t n_chars)
{
    int32_t i;
    const uint8_t * p = utf8;
    int32_t len = strlen ((const char *) utf8);
    if (len == 0 && n_chars != 0) {
	return UNICODE_EMPTY_INPUT;
    }
    for (i = 0; i < n_chars; i++) {
        int32_t ucs2 = utf8_to_ucs2 (p, & p);
        if (ucs2 < 0) {
	    return ucs2;
        }
    }
    return p - utf8;
}

/* Like unicode_count_chars, but without error checks or validation of
   the input. This only checks the first byte of each UTF-8 sequence,
   then jumps over the succeeding bytes. It may return
   UTF8_BAD_LEADING_BYTE if the first byte is invalid. */

int32_t
unicode_count_chars_fast (const uint8_t * utf8)
{
    int32_t chars;
    const uint8_t * p;
    chars = 0;
    p = utf8;
    while (*p) {
	int32_t len;
	len = utf8_sequence_len[*p];
	if (len == 0) {
	    /* The first byte of a UTF-8 sequence is bad, so return
	       this, not BAD_UTF8. */
	    return UTF8_BAD_LEADING_BYTE;
	}
	p += len;
	chars++;
    }
    return chars;
}

/* Given a nul-terminated string "utf8", return the total number of
   Unicode characters it contains.

   Return value

   If an error occurs, this may return UTF8_BAD_LEADING_BYTE or any of the
   errors of "utf8_to_ucs2". */

int32_t
unicode_count_chars (const uint8_t * utf8)
{
    int32_t chars = 0;
    const uint8_t * p = utf8;
    int32_t len = strlen ((const char *) utf8);
    if (len == 0) {
        return 0;
    }
    while (p - utf8 < len) {
        int32_t ucs2;
        ucs2 = utf8_to_ucs2 (p, & p);
        if (ucs2 < 0) {
	    /* Return the error from utf8_to_ucs2. */
            return ucs2;
        }
        chars++;
        if (*p == '\0') {
            return chars;
        }
    }
    /* Cannot be reached in practice, since strlen indicates the null
       byte. */
    return UTF8_BAD_LEADING_BYTE;
}

#ifdef HEADER

/* These are intended for use in switch statements, for example

   switch (c) {
   case BYTE_80_8F:
   do_something;

   They originally come from the Json3 project. */

#define BYTE_80_8F							\
    0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: \
 case 0x87: case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: \
 case 0x8E: case 0x8F
#define BYTE_80_9F							\
    0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: \
 case 0x87: case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: \
 case 0x8E: case 0x8F: case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: \
 case 0x95: case 0x96: case 0x97: case 0x98: case 0x99: case 0x9A: case 0x9B: \
 case 0x9C: case 0x9D: case 0x9E: case 0x9F
#define BYTE_80_BF							\
    0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: \
 case 0x87: case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: \
 case 0x8E: case 0x8F: case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: \
 case 0x95: case 0x96: case 0x97: case 0x98: case 0x99: case 0x9A: case 0x9B: \
 case 0x9C: case 0x9D: case 0x9E: case 0x9F: case 0xA0: case 0xA1: case 0xA2: \
 case 0xA3: case 0xA4: case 0xA5: case 0xA6: case 0xA7: case 0xA8: case 0xA9: \
 case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF: case 0xB0: \
 case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7: \
 case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: \
 case 0xBF
#define BYTE_80_8F_B0_BF						\
    0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: \
 case 0x87: case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: \
 case 0x8E: case 0x8F: case 0xB0:					\
 case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7: \
 case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: \
 case 0xBF
#define BYTE_80_B6_B8_BF						\
    0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: \
 case 0x87: case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: \
 case 0x8E: case 0x8F: case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: \
 case 0x95: case 0x96: case 0x97: case 0x98: case 0x99: case 0x9A: case 0x9B: \
 case 0x9C: case 0x9D: case 0x9E: case 0x9F: case 0xA0: case 0xA1: case 0xA2: \
 case 0xA3: case 0xA4: case 0xA5: case 0xA6: case 0xA7: case 0xA8: case 0xA9: \
 case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF: case 0xB0: \
 case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: \
 case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: \
 case 0xBF
#define BYTE_80_BD							\
    0x80: case 0x81: case 0x82: case 0x83: case 0x84: case 0x85: case 0x86: \
 case 0x87: case 0x88: case 0x89: case 0x8A: case 0x8B: case 0x8C: case 0x8D: \
 case 0x8E: case 0x8F: case 0x90: case 0x91: case 0x92: case 0x93: case 0x94: \
 case 0x95: case 0x96: case 0x97: case 0x98: case 0x99: case 0x9A: case 0x9B: \
 case 0x9C: case 0x9D: case 0x9E: case 0x9F: case 0xA0: case 0xA1: case 0xA2: \
 case 0xA3: case 0xA4: case 0xA5: case 0xA6: case 0xA7: case 0xA8: case 0xA9: \
 case 0xAA: case 0xAB: case 0xAC: case 0xAD: case 0xAE: case 0xAF: case 0xB0: \
 case 0xB1: case 0xB2: case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7: \
 case 0xB8: case 0xB9: case 0xBA: case 0xBB: case 0xBC: case 0xBD
#define BYTE_90_BF							\
    0x90: case 0x91: case 0x92: case 0x93: case 0x94: case 0x95: case 0x96: \
 case 0x97: case 0x98: case 0x99: case 0x9A: case 0x9B: case 0x9C: case 0x9D: \
 case 0x9E: case 0x9F: case 0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: \
 case 0xA5: case 0xA6: case 0xA7: case 0xA8: case 0xA9: case 0xAA: case 0xAB: \
 case 0xAC: case 0xAD: case 0xAE: case 0xAF: case 0xB0: case 0xB1: case 0xB2: \
 case 0xB3: case 0xB4: case 0xB5: case 0xB6: case 0xB7: case 0xB8: case 0xB9: \
 case 0xBA: case 0xBB: case 0xBC: case 0xBD: case 0xBE: case 0xBF
#define BYTE_A0_BF							\
    0xA0: case 0xA1: case 0xA2: case 0xA3: case 0xA4: case 0xA5: case 0xA6: \
 case 0xA7: case 0xA8: case 0xA9: case 0xAA: case 0xAB: case 0xAC: case 0xAD: \
 case 0xAE: case 0xAF: case 0xB0: case 0xB1: case 0xB2: case 0xB3: case 0xB4: \
 case 0xB5: case 0xB6: case 0xB7: case 0xB8: case 0xB9: case 0xBA: case 0xBB: \
 case 0xBC: case 0xBD: case 0xBE: case 0xBF
#define BYTE_C2_DF							\
    0xC2: case 0xC3: case 0xC4: case 0xC5: case 0xC6: case 0xC7: case 0xC8: \
 case 0xC9: case 0xCA: case 0xCB: case 0xCC: case 0xCD: case 0xCE: case 0xCF: \
 case 0xD0: case 0xD1: case 0xD2: case 0xD3: case 0xD4: case 0xD5: case 0xD6: \
 case 0xD7: case 0xD8: case 0xD9: case 0xDA: case 0xDB: case 0xDC: case 0xDD: \
 case 0xDE: case 0xDF
#define BYTE_E1_EC							\
    0xE1: case 0xE2: case 0xE3: case 0xE4: case 0xE5: case 0xE6: case 0xE7: \
 case 0xE8: case 0xE9: case 0xEA: case 0xEB: case 0xEC
#define BYTE_F1_F3				\
    0xF1: case 0xF2: case 0xF3
#endif /* def HEADER */

#define UNICODEADDBYTE i++

#define UNICODEFAILUTF8(want) return UTF8_INVALID

#define UNICODENEXTBYTE c = input[i]

/* Given "input" and "input_length", validate "input" byte by byte up
   to "input_length". The return value may be UTF8_VALID or
   UTF8_INVALID. */

int32_t
valid_utf8 (const uint8_t * input, int32_t input_length)
{
    int32_t error;
    utf8_info_t info;
    error = validate_utf8 (input, input_length, & info);
    if (error < 0) {
	return UTF8_INVALID;
    }
    return UTF8_VALID;
}

#define FAIL(x)				\
    info->len_read = i;			\
    return x

#ifdef HEADER

typedef struct utf8_info
{
    int32_t len_read;
    int32_t runes_read;
}
utf8_info_t;

#endif /* def HEADER */

/* Given "input" and "len", validate "input" byte by byte up to
   "len". The return value is "UNICODE_OK" (zero) on success or the
   error found (a negative number) on failure.

   utf8_info_t is defined in "unicode.h".

   The value of "info.len_read" is the number of bytes processed. the
   value of "info.runes_read" is the number of Unicode code points in
   the input. */

int32_t
validate_utf8 (const uint8_t * input, int32_t len, utf8_info_t * info)
{
    int32_t i;
    uint8_t c;

    info->len_read = 0;
    /* We want to increment the runes after "string_start", but that
       would give us one too many. */
    info->runes_read = -1;
    i = 0;

 string_start:

    /* We get here after successfully reading a "rune". */

    info->runes_read++;
    if (i >= len) {
	info->len_read = len;
	return UNICODE_OK; /* 0 */
    }

    /* Set c separately here since we use a range comparison before
       the switch statement. */

    c = input[i];

    if (c == 0) {
	FAIL (UNICODE_EMPTY_INPUT);
    }
    /* Admit all bytes < 0x80. */
    if (c < 0x80) {
	i++;
	goto string_start;
    }
    switch (c) {
    case BYTE_C2_DF:
	UNICODEADDBYTE;
	goto byte_last_80_bf;

    case 0xE0:
	UNICODEADDBYTE;
	goto byte23_a0_bf;

    case BYTE_E1_EC:
	UNICODEADDBYTE;
	goto byte_penultimate_80_bf;

    case 0xED:
	UNICODEADDBYTE;
	goto byte23_80_9f;

    case 0xEE:
	UNICODEADDBYTE;
	goto byte_penultimate_80_bf;

    case 0xEF:
	UNICODEADDBYTE;
	goto byte_ef_80_bf;

    case 0xF0:
	UNICODEADDBYTE;
	goto byte24_90_bf;

    case BYTE_F1_F3:
	UNICODEADDBYTE;
	goto byte24_80_bf;

    case 0xF4:
	UNICODEADDBYTE;
	goto byte24_80_8f;

    default:
	FAIL (UTF8_BAD_LEADING_BYTE);
    }

 byte_last_80_bf:

    switch (UNICODENEXTBYTE) {
    case BYTE_80_BF:
	UNICODEADDBYTE;
	goto string_start;
    default:
	FAIL (UTF8_BAD_CONTINUATION_BYTE);
    }

 byte_ef_b7:
    switch (UNICODENEXTBYTE) {
    case BYTE_80_8F_B0_BF:
	UNICODEADDBYTE;
	goto string_start;
    default:
	if (c >= 0x90 && c <= 0xAF) {
	    FAIL (UNICODE_NOT_CHARACTER);
	}
	else {
	    FAIL (UTF8_BAD_CONTINUATION_BYTE);
	}
    }

 byte_last_80_bd:

    switch (UNICODENEXTBYTE) {
    case BYTE_80_BD:
	UNICODEADDBYTE;
	goto string_start;
    case 0xBE:
    case 0xBF:
	FAIL (UNICODE_NOT_CHARACTER);
    default:
	FAIL (UTF8_BAD_CONTINUATION_BYTE);
    }

 byte_penultimate_80_bf:

    switch (UNICODENEXTBYTE) {
    case BYTE_80_BF:
	UNICODEADDBYTE;
	goto byte_last_80_bf;
    default:
	FAIL (UTF8_BAD_CONTINUATION_BYTE);
    }

 byte_ef_80_bf:
    switch (UNICODENEXTBYTE) {
    case BYTE_80_B6_B8_BF:
	UNICODEADDBYTE;
	goto byte_last_80_bd;
    case 0xB7:
	UNICODEADDBYTE;
	/* FDD0 - FDE7 */
	goto byte_ef_b7;
    default:
	FAIL (UTF8_BAD_CONTINUATION_BYTE);
    }

 byte24_90_bf:

    switch (UNICODENEXTBYTE) {
    case BYTE_90_BF:
	UNICODEADDBYTE;
	goto byte_penultimate_80_bf;
    default:
	FAIL (UTF8_BAD_CONTINUATION_BYTE);
    }

 byte23_80_9f:

    switch (UNICODENEXTBYTE) {
    case BYTE_80_9F:
	UNICODEADDBYTE;
	goto byte_last_80_bf;
    default:
	if (c >= 0xA0 && c <= 0xBF) {
	    FAIL (UNICODE_SURROGATE_PAIR);
	}
	else {
	    FAIL (UTF8_BAD_CONTINUATION_BYTE);
	}
    }

 byte23_a0_bf:

    switch (UNICODENEXTBYTE) {
    case BYTE_A0_BF:
	UNICODEADDBYTE;
	goto byte_last_80_bf;
    default:
	FAIL (UTF8_BAD_CONTINUATION_BYTE);
    }

 byte24_80_bf:

    switch (UNICODENEXTBYTE) {
    case BYTE_80_BF:
	UNICODEADDBYTE;
	goto byte_ef_80_bf;
    default:
	FAIL (UTF8_BAD_CONTINUATION_BYTE);
    }

 byte24_80_8f:

    switch (UNICODENEXTBYTE) {
    case BYTE_80_8F:
	UNICODEADDBYTE;
	goto byte_ef_80_bf;
    default:
	if (c >= 0x90) {
	    FAIL (UNICODE_TOO_BIG);
	}
	else {
	    FAIL (UTF8_BAD_CONTINUATION_BYTE);
	}
    }
}

#define REJECT_FE_FF(c)				\
    if (c == 0xFF || c == 0xFE) {		\
	return UNICODE_NOT_CHARACTER;		\
    }

/* Make "* ptr" point to the start of the first UTF-8 character after
   its initial value. This assumes that there are at least four bytes
   which can be read, and that "* ptr" points to valid UTF-8.

   If "** ptr" does not have its top bit set, 00xx_xxxx, this does not
   change the value of "* ptr", and it returns UNICODE_OK. If "** ptr"
   has its top two bits set, 11xx_xxxx, this does not change the value
   of "* ptr" and it returns UNICODE_OK. If "**ptr" has its top bit
   set but its second-to-top bit unset, 10xx_xxxx, so it is the
   second, third, or fourth byte of a multibyte sequence, "* ptr" is
   incremented until either "** ptr" is a valid first byte of a UTF-8
   sequence, or too many bytes have passed for it to be valid
   UTF-8. If too many bytes have passed, UTF8_BAD_CONTINUATION_BYTE is
   returned and "*ptr" is left unchanged.

   If a valid UTF-8 first byte was found, either 11xx_xxxx or
   00xx_xxxx, UNICODE_OK is returned, and "*ptr" is set to the address
   of the valid byte. Nul bytes (bytes containing zero) are considered
   valid.

   If any of the bytes read contains invalid UTF-8 bytes 0xFE and
   0xFF, the error code UNICODE_NOT_CHARACTER is returned and "*ptr"
   is left unchanged. */

int32_t
trim_to_utf8_start (const uint8_t ** ptr)
{
    const uint8_t * p = *ptr;
    uint8_t c;
    int32_t i;

    c = * p;
    REJECT_FE_FF (c);
    /* 0xC0 = 1100_0000. */
    c &= 0xC0;
    if (c == 0xC0 || c == 0x00) {
	return UNICODE_OK;
    }
    for (i = 0; i < UTF8_MAX_LENGTH - 1; i++) {
	c = p[i];
	REJECT_FE_FF (c);
	if ((c & 0x80) != 0x80 || (c & 0x40) != 0) {
	    * ptr = p + i;
	    return UNICODE_OK;
	}
    }
    return UTF8_BAD_CONTINUATION_BYTE;
}

/* Given a return value "code" which is negative or zero, return a
   string which describes what the return value means. Positive
   non-zero return values never indicate errors or statuses in this
   library. Unknown error codes result in a default string being
   returned. */

const char *
unicode_code_to_error (int32_t code)
{
    switch (code) {
    case UTF8_BAD_LEADING_BYTE:
	return "The leading byte of a UTF-8 sequence was invalid";
    case UTF8_BAD_CONTINUATION_BYTE:
	return "A continuation byte of a UTF-8 sequence was invalid";
    case UNICODE_SURROGATE_PAIR:
	return "A surrogate pair code point could not be converted to UTF-8";
    case UNICODE_NOT_SURROGATE_PAIR:
	return "Input code points did not form a surrogate pair";
    case UNICODE_OK:
	return "Successful completion";
    case UNICODE_TOO_BIG:
	return "A code point was beyond limits";
    case UNICODE_NOT_CHARACTER:
	return "A number ending in hex FFFF or FFFE is not valid Unicode";
    case UTF8_NON_SHORTEST:
	return "A UTF-8 input was not in the shortest form";
    case UNICODE_EMPTY_INPUT:
	return "A byte with value zero was found in UTF-8 input";
    default:
	return "Unknown/invalid error code";
    }
}


