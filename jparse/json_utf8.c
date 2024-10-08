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
#include <ctype.h>
#include "json_utf8.h"

/*
 * count_utf8_bytes	- count bytes needed to encode/decode in str
 *
 * given:
 *
 *	str	    the string to parse
 *	surrogate   if str == NULL then use this for the check
 *	bytes	    pointer to the number of bytes
 *
 * NOTE: if count is NULL we return false. Otherwise we attempt to
 * parse the string as %4x and then, assuming we extract a value, we count the
 * number of bytes required for the string and place it in *count, as long as
 * str != NULL. If, however str is NULL, we will simply check the value in
 * surrogate.  As long as this can be done we return true.
 *
 * NOTE: *str should point to the \u!
 */
bool
count_utf8_bytes(const char *str, int32_t surrogate, size_t *bytes)
{
    unsigned char xa = 0;   /* first hex digit */
    unsigned char xb = 0;   /* second hex digit */
    unsigned char xc = 0;   /* third hex digit */
    unsigned char xd = 0;   /* fourth hex digit */
    unsigned int x = 0;	    /* the hex value we attempt to extract */
    size_t len = 0;	    /* the number of bytes which *bytes will be set to */
    int scanned = 0;	    /* how many values read */


    /*
     * firewall
     */
    if (bytes == NULL) {
	warn(__func__, "bytes is NULL");
	return false;
    } else {
	*bytes = 0;
    }

    if (str == NULL) {
	x = surrogate;
	if (x < 0x80) {
	    len = 1;
	} else if (x < 0x800) {
	    len = 2;
	} else if (x < 0x10000) {
	    len = 3;
	} else if (x < 0x110000) {
	    len = 4;
	} else {
	    warn(__func__, "%x: illegal value\n", x);

	    if (bytes != NULL) {
		*bytes = 0;
	    }

	    return false;
	}

	if (bytes != NULL) {
	    *bytes = len;
	}

	return true;
    }

    /*
     * if we get here we know str != NULL
     */
    scanned = sscanf(str, "\\u%c%c%c%c", &xa, &xb, &xc, &xd);
    if (scanned != 4) {
	warn(__func__, "did not find \\u followed by four HEX digits: %ju values: <%s>: %x %x %x %x", scanned, str,
		xa, xb, xc, xd);
	if (bytes != NULL) {
	    *bytes = 0;
	}
	return false;
    } else {
	/*
	 * extra sanity check
	 */
	if (!isxdigit(xa) || !isxdigit(xb) || !isxdigit(xc) || !isxdigit(xd)) {
	    warn(__func__, "sscanf() found \\uxxxx but not all values are hex digits!");
	    if (bytes != NULL) {
		*bytes = 0;
	    }
	    return false;
	}
    }

    /*
     * now that we know that there is a \u followed by FOUR HEX digits we can
     * try and extract it as a SINGLE HEX number
     */
    scanned = sscanf(str, "\\u%4x", &x);
    if (scanned != 1) {
	err(10, __func__, "found \\uxxxx but NOT a hex value!");
	not_reached();
    } else {
	/*
	 * now that we have a SINGLE HEX number, we need to check the number of
	 * bytes required, setting it in *bytes.
	 */

	if (x < 0x80) {
	    len = 1;
	} else if (x < 0x800) {
	    len = 2;
	} else if (x < 0x10000) {
	    len = 3;
	} else if (x < 0x110000) {
	    len = 4;
	} else {
	    warn(__func__, "%x: illegal value\n", x);

	    if (bytes != NULL) {
		*bytes = 0;
	    }

	    return false;
	}

	if (bytes != NULL) {
	    *bytes = len;
	}
    }

    return true;
}

/*
 * NOTE: until the bug documented at https://github.com/xexyl/jparse/issues/13
 * is resolved fully, we have code here that comes from a number of locations.
 * Once the bug is resolved this file will be cleaned up. There are two
 * different locations at this time (29 Sep 2024).
 */

/*
 * The below is based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with additional code added.
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
    int len = -1;

    /*
     * firewall
     */
    if (str == NULL) {
	err(10, __func__, "str is NULL");
	not_reached();
    }

    if (val >= UNI_NOT_CHAR_MIN && val <= UNI_NOT_CHAR_MAX) {
	warn(__func__, "invalid codepoint: %X", val);
	len = UNICODE_NOT_CHARACTER;
    } else if ((val & 0xFFFF) >= 0xFFFE) {
	warn(__func__, "codepoint %X: ends in either FFFE or FFFF", val);
	len = UNICODE_NOT_CHARACTER;
    } else if (val == 0xFF || val == 0xFE) {
	warn(__func__, "codepoint: %X: illegal value", val);
	len = UNICODE_NOT_CHARACTER;
    } else if (val >= UNI_SUR_HIGH_START && val <= UNI_SUR_LOW_END) {
	warn(__func__, "codepoint: %X: illegal surrogate");
	len = UNICODE_SURROGATE_PAIR;
    } else if (val < 0x80) {
	dbg(DBG_MED, "%s: val: %X < 0x80", __func__, (uintmax_t)val);
	str[0] = val;
	len = 1;
    } else if (val < 0x800) {
	dbg(DBG_MED, "%s: val: %X < 0x800", __func__, (uintmax_t)val);
	str[1] = val & UTF8_V_MASK;
	str[1] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[0] = val;
	str[0] |= UTF8_2_BITS;
	len = 2;
    } else if (val < 0x10000) {
	dbg(DBG_MED, "%s: val: %X < 0x10000", __func__, (uintmax_t)val);
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
	dbg(DBG_MED, "%s: val: %X < 0x110000", __func__, (uintmax_t)val);
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
	err(11, __func__, "%#X: illegal val\n", val);
	not_reached();
    }
    return len;
}


/*
 * The above is based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with additional code added.
 */

/*
 * The below table and code is from
 * https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode library
 * in the programming language C which deals with conversions to and from the
 * UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */


/*
 * This is a Unicode library in the programming language C which deals with
 * conversions to and from the UTF-8 format.
 */

/*
  Author:

  Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>

  Repository:

  https://github.com/benkasminbullock/unicode-c
*/


/*
 * This table contains the length of a sequence which begins with the byte
 * given. A value of zero indicates that the byte can not begin a UTF-8
 * sequence. This comes from:
 * https://metacpan.org/source/CHANSEN/Unicode-UTF8-0.60/UTF8.xs#L8.
 */
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

/*
 * This function converts UTF-8 encoded bytes in "input" into the equivalent
 * Unicode code point.  The return value is the Unicode code point corresponding
 * to the UTF-8 character in "input" if successful, and a negative number if not
 * successful.  Nul bytes are rejected.
 *
 * "*end_ptr" is set to the next character after the read character on success.
 * "*end_ptr" is set to the start of input on all failures.  "end_ptr" may not
 * be NULL.
 *
 * If the first byte of "input" is zero, in other words a NUL or '\0',
 * UNICODE_EMPTY_INPUT is returned.
 *
 * If the first byte of "input" is not valid UTF-8, UTF8_BAD_LEADING_BYTE is
 * returned.
 *
 * If the second or later bytes of "input" are not valid UTF-8, including NUL,
 * UTF8_BAD_CONTINUATION_BYTE is returned.
 *
 * If the value extrapolated from "input" is greater than UNICODE_MAXIMUM,
 * UNICODE_TOO_BIG is returned.
 *
 * If the value extrapolated from "input" ends in 0xFFFF or 0xFFFE,
 * UNICODE_NOT_CHARACTER is returned.
 *
 * If the value extrapolated from "input" is between 0xFDD0 and 0xFDEF,
 * UNICODE_NOT_CHARACTER is returned.
 *
 * If the value is within the range of surrogate pairs, the error
 * UNICODE_SURROGATE_PAIR is returned.
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


/*
 * Convert a surrogate pair in "hi" and "lo" to a single Unicode value. The
 * return value is the Unicode value. If the return value is negative, an error
 * has occurred. If "hi" and "lo" do not form a surrogate pair, the error value
 * UNICODE_NOT_SURROGATE_PAIR is returned.
 *
 * https://android.googlesource.com/platform/external/id3lib/+/master/unicode.org/ConvertUTF.c.
 */
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

/*
 * Given a nul-terminated string "utf8", return the total number of Unicode
 * characters it contains.
 *
 * If an error occurs, this may return UTF8_BAD_LEADING_BYTE or any of the
 * errors of "utf8_to_ucs2".
 */
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
