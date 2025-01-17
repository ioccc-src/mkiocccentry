/*
 * json_utf8 - JSON UTF-8 encoder/decoder
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
 * count_utf8_bytes	- count bytes needed to decode/encode in str
 *
 * given:
 *
 *	str	    the string to parse
 *	surrogate   if str == NULL then use this for the check
 *	bytes	    pointer to the number of bytes
 *
 * NOTE: If str is NULL we use the value in surrogate; otherwise we attempt to
 * extract the value by parsing the string as %4x and then, assuming we extract
 * a value, we count the number of bytes required for the string. It is this
 * value that is returned.
 *
 * NOTE: *str should point to the \u!
 */
size_t
utf8len(const char *str, int32_t surrogate)
{
    unsigned char xa = 0;   /* first hex digit */
    unsigned char xb = 0;   /* second hex digit */
    unsigned char xc = 0;   /* third hex digit */
    unsigned char xd = 0;   /* fourth hex digit */
    unsigned int x = 0;	    /* the hex value we attempt to extract */
    size_t len = 0;	    /* the number of bytes to return */
    int scanned = 0;	    /* how many values read */

    if (str == NULL) {
	x = surrogate;
	if (x < 0x80) {
	    len = 1;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else if (x < 0x800) {
	    len = 2;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else if (x < 0x10000) {
	    len = 3;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else if (x < 0x110000) {
	    len = 4;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else {
	    warn(__func__, "%X: illegal value, len: %jd\n", x, (intmax_t)len);
	    len = -1;
	}

	return len;
    }

    /*
     * if we get here we know str != NULL
     */
    scanned = sscanf(str, "\\u%c%c%c%c", &xa, &xb, &xc, &xd);
    if (scanned != 4) {
	warn(__func__, "did not find \\u followed by four HEX digits: %ju values: <%s>: %x %x %x %x", (uintmax_t)scanned, str,
		xa, xb, xc, xd);
	len = -1;

	return len;
    } else {
	/*
	 * extra sanity check
	 */
	if (!isxdigit(xa) || !isxdigit(xb) || !isxdigit(xc) || !isxdigit(xd)) {
	    warn(__func__, "sscanf() found \\uxxxx but not all values are hex digits!");
	    len = -1;

	    return len;
	}
    }

    /*
     * we have to perform additional checks
     */

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
	 * bytes required, setting it in len.
	 */
	if (x < 0x80) {
	    len = 1;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else if (x < 0x800) {
	    len = 2;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else if (x < 0x10000) {
	    len = 3;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else if (x < 0x110000) {
	    len = 4;
	    dbg(DBG_VVHIGH, "%X length %jd", x, (intmax_t)len);
	} else {
	    warn(__func__, "%X: illegal value, len %jd\n", x, (intmax_t)len);
	    len = -1;
	}
    }

    return len;
}

/*
 * The below functions are based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with a number of changes made by us.
 */

/*
 * NOTE: the following comment describes the function utf8encode() but we have
 * renamed it because in JSON in BOTH encoding and decoding it should convert
 * \uxxxx to unicode. This is by all sources seen called encoding but since this
 * is a JSON library, to be less confusing, we call it utf8_to_unicode().
 *
 * --
 *
 * UTF8 valid ranges.
 *
 * The UTF-8 decoding spreads the bits of a 32bit word over several
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
 * shortest representation of a 32bit value is to be used.  A encoder
 * must not encode sequences that do not satisfy this requirement.
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
utf8_to_unicode(char *str, unsigned int val)
{
    int len = -1;

    /*
     * firewall
     */
    if (str == NULL) {
	err(10, __func__, "str is NULL");
	not_reached();
    }

    if (val >= UNI_SUR_HIGH_START && val <= UNI_SUR_LOW_END) {
	warn(__func__, "codepoint: %X: illegal surrogate", val);
	len = UNICODE_SURROGATE_PAIR;
    } else if (val < 0x80) {
	dbg(DBG_VVHIGH, "%s: val: %X < 0x80", __func__, val);
	str[0] = val;
	len = 1;
    } else if (val < 0x800) {
	dbg(DBG_VVHIGH, "%s: val: %X < 0x800", __func__, val);
	str[1] = val & UTF8_V_MASK;
	str[1] |= UTF8_N_BITS;
	val >>= UTF8_V_SHIFT;
	str[0] = val;
	str[0] |= UTF8_2_BITS;
	len = 2;
    } else if (val < 0x10000) {
	dbg(DBG_VVHIGH, "%s: val: %X < 0x10000", __func__, val);
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
	dbg(DBG_VVHIGH, "%s: val: %X < 0x110000", __func__, val);
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
	len = -1;
	warn(__func__, "illegal value: %#X too big\n", val);
	len = UNICODE_TOO_BIG;
    }

    return len;
}


/*
 * The above function is based on code from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c,
 * with a number of changes made by us.
 */

/*
 * -=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=
 */

/*
 * The below function (with some extra sanity checks added by us) come from
 * https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode library
 * in the programming language C which deals with conversions to and from the
 * UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */


/*
 * Convert a surrogate pair in "hi" and "lo" to a single Unicode value. The
 * return value is the Unicode value. If the return value is negative, an error
 * has occurred. If "hi" and "lo" do not form a surrogate pair, the error value
 * UNICODE_NOT_SURROGATE_PAIR is returned.
 *
 * https://android.googlesource.com/platform/external/id3lib/+/master/unicode.org/ConvertUTF.c.
 */
int32_t
surrogates_to_unicode(int32_t hi, int32_t lo)
{
    int32_t u;

    /*
     * sanity checks
     *
     * These should theoretically never happen.
     */
    if (hi < 0 && lo < 0) {
        warn(__func__, "hi %jd < 0 && lo %jd < 0", (intmax_t)hi, (intmax_t)lo);
        return UNICODE_NOT_SURROGATE_PAIR;
    } else if (hi < 0) {
        warn(__func__, "hi %jd < 0", (intmax_t)hi);
        return UNICODE_NOT_SURROGATE_PAIR;
    } else if (lo < 0) {
        warn(__func__, "lo %jd < 0", (intmax_t)lo);
        return UNICODE_NOT_SURROGATE_PAIR;
    }

    if (hi < UNI_SUR_HIGH_START || hi > UNI_SUR_HIGH_END ||
	lo < UNI_SUR_LOW_START || lo > UNI_SUR_LOW_END) {
	return UNICODE_NOT_SURROGATE_PAIR;
    }

    u = ((hi - UNI_SUR_HIGH_START) << TEN_BITS)
      + (lo - UNI_SUR_LOW_START) + HALF_BASE;

    return u;
}

/*
 * The above function (with some extra sanity checks added by us) come from
 * https://github.com/benkasminbullock/unicode-c/, which is 'a Unicode which is
 * 'a Unicode library in the programming language C which deals with conversions
 * to and from the UTF-8 format', and was written by:
 *
 *	Ben Bullock <benkasminbullock@gmail.com>, <bkb@cpan.org>
 */

/*
 * -=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=-=-=-=---=-=-=-=-=-=-=-=-=
 */
