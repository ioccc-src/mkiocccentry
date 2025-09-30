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
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody
 * Boone Ferguson and Landon Curt Noll:
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
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else if (x < 0x800) {
	    len = 2;
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else if (x < 0x10000) {
	    len = 3;
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else if (x < 0x110000) {
	    len = 4;
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else {
	    warn(__func__, "%X: illegal value, len: %zd\n", x, len);
	    len = -1;
	}

	return len;
    }

    /*
     * if we get here we know str != NULL
     */
    scanned = sscanf(str, "\\u%c%c%c%c", &xa, &xb, &xc, &xd);
    if (scanned != 4) {
	warn(__func__, "expected \\u followed by FOUR HEX digits, only got %d", scanned);
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
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else if (x < 0x800) {
	    len = 2;
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else if (x < 0x10000) {
	    len = 3;
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else if (x < 0x110000) {
	    len = 4;
	    dbg(DBG_VVHIGH, "%X length %zd", x, len);
	} else {
	    warn(__func__, "%X: illegal value, len %zd\n", x, len);
	    len = -1;
	}
    }

    return len;
}


/*
 * surrogate_pair_to_codepoint - convert surrogate pair to a single codepoint
 *
 * given:
 *
 *      hi  - high byte
 *      lo  - low byte
 *
 * NOTE: if a value is out of range the function will return a negative number.
 * NOTE: although we could use is_surrogate_pair() this would needlessly
 * complicate the decode functions in json_parse.c. In that code if this
 * function returns < 0 it the string is processed as if it was a single \uxxxx.
 * That happens if one or both of the bytes is out of range. Otherwise the
 * codepoint is returned and it is processed in the other function (the decode
 * on in json_parse.c).
 */
int32_t
surrogate_pair_to_codepoint(int32_t hi, int32_t lo)
{
    int32_t codepoint;

    /*
     * sanity checks
     *
     * These should theoretically never happen.
     */
    if (hi < 0) {
        warn(__func__, "high byte < 0: %4x", hi);
        return -1;
    } else if (lo < 0) {
        warn(__func__, "low byte < 0: %4x", lo);
        return -1;
    }

    if (hi < 0xD800 || hi > 0xDBFF || lo < 0xDC00 || lo > 0xDFFF) {
        /*
         * don't report this as it might just be that it's not a surrogate pair.
         */
	return -1;
    }
    codepoint = ((hi - 0xD800) << 10) + (lo - 0xDC00) + 0x10000;
    dbg(DBG_VVHIGH, "codepoint: %d", codepoint);
    return codepoint;
}


/*
 * codepoint_to_unicode - convert a codepoint to Unicode
 *
 * given:
 *      output      - buffer to write output Unicode to
 *      codepoint   - codepoint to convert to Unicode
 *
 * NOTE: it is the caller's responsibility to ensure that the output buffer is
 * big enough for the result + the terminating NUL byte.
 *
 * NOTE: if the output buffer is NULL this function will not return.
 *
 * NOTE: if an error occurs this function returns a negative number.
 *
 * The below comments come from
 * https://lxr.missinglinkelectronics.com/linux+v5.19/fs/unicode/mkutf8data.c
 * which we leave for posterity, even though we no longer use their code (the
 * constants notwithstanding as we have no choice in the matter):
 *
 * UTF8 valid ranges
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
codepoint_to_unicode(char *output, unsigned int codepoint)
{
    int len = -1;

    /*
     * firewall
     */
    if (output == NULL) {
	err(10, __func__, "output is NULL");
	not_reached();
    }

    if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
	warn(__func__, "codepoint: %X: illegal surrogate", codepoint);
	len = -2;
    } else if (codepoint <= 0x7F) {
        output[0] = (char)codepoint;
        output[1] = '\0';
        len = 1;
        dbg(DBG_VVHIGH, "%s: codepoint: %X <= 0x7F", __func__, codepoint);
    } else if (codepoint <= 0x7FF) {
        output[0] = (char)(0xC0 | (codepoint >> 6));
        output[1] = (char)(0x80 | (codepoint & 0x3F));
        output[2] = '\0';
        len = 2;
        dbg(DBG_VVHIGH, "%s: codepoint: %X <= 0x7FF", __func__, codepoint);
    } else if (codepoint <= 0xFFFF) {
        output[0] = (char)(0xE0 | (codepoint >> 12));
        output[1] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        output[2] = (char)(0x80 | (codepoint & 0x3F));
        output[3] = '\0';
        len = 3;
        dbg(DBG_VVHIGH, "%s: codepoint: %X <= 0xFFFF", __func__, codepoint);
    } else if (codepoint <= 0x10FFFF) {
        output[0] = (char)(0xF0 | (codepoint >> 18));
        output[1] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
        output[2] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
        output[3] = (char)(0x80 | (codepoint & 0x3F));
        output[4] = '\0';
        len = 4;
        dbg(DBG_VVHIGH, "%s: codepoint: %X <= 0x10FFFF", __func__, codepoint);
    } else {
	warn(__func__, "illegal value: %#X too big\n", codepoint);
	len = -1;
    }

    return len;
}


/*
 * utf8_to_codepoint - convert UTF-8 byte(s) to a codepoint
 *
 * given:
 *      str - UTF-8 byte(s)
 *
 * Returns the codepoint assuming no error occurs.
 *
 * NOTE: this function will not return on a NULL pointer.
 */
uint32_t utf8_to_codepoint(const char *str)
{
    unsigned char c = '\0';

    /*
     * firewall
     */
    if (str == NULL || *str == '\0') {
        err(55, __func__, "str is NULL or empty");
        not_reached();
    }

    c = str[0];
    if ((str[0] & 0x80) == 0) {
        return *str;
    } else if ((c & 0xE0) == 0xC0 && str[1] != '\0') {
        return ((*str & 0x1F) << 6) | (str[1] & 0x3F);
    } else if ((*str & 0xF0) == 0xE0 && str[1] != '\0' && str[2] != '\0') {
        return ((*str & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
    } else if ((c & 0xF8) == 0xF0 && str[1] != '\0' && str[2] != '\0' && str[3] != '\0') {
        return ((*str & 0x07) << 18) | ((str[1] & 0x3F) << 12) | ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
    }
    warn(__func__, "invalid UTF-8 sequence");
    return 0;
}


/*
 * is_surrogate_pair    - detect if valid surrogate pair
 *
 * given:
 *
 *      xa      first \uxxxx
 *      xb      second \uxxxx
 *
 * Returns true if a valid surrogate pair, else false.
 *
 * NOTE: this function checks that the xa (high surrogate or presumed high
 * surrogate) is not a low surrogate if it's not a valid surrogate pair as an
 * extra sanity check.
 *
 * NOTE: this function is not strictly needed due to the fact that
 * surrogate_pair_to_codepoint() does the proper checks too, but we offer this
 * function for those who need or desire it (we do not use it as it would
 * complicate the code even more).
 */
bool
is_surrogate_pair(const int32_t xa, const int32_t xb)
{
    if (xa < 0) {
        warn(__func__, "high surrogate < 0");
        return false;
    } else if (xb < 0) {
        warn(__func__, "low surrogate < 0");
        return false;
    }
    if (xa >= 0xD800 && xa <= 0xDBFF && xb >= 0xDC00 && xb <= 0xDFFF) {
        dbg(DBG_HIGH, "high surrogate %4x followed by low surrogate %4x", xa, xb);
        return true;
    } else if (xa >= 0xDC00 && xa <= 0xDFFF) {
        warn(__func__, "low surrogate %4x not preceded by high surrogate", xa);
        return false;
    }
    return false;
}
