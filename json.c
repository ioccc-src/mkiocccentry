/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json - JSON functions supporting mkiocccentry code
 *
 * JSON related functions to support formation of .info.json files
 * and .author.json files their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
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


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions
 */
#include "util.h"

/*
 * IOCCC limits
 */
#include "limit_ioccc.h"

/*
 * JSON - JSON structures and functions
 */
#include "json.h"


/*
 * JSON encoding of an octet in a JSON string
 */
struct encode {
    u_int8_t byte;	    /* 8 bit character to encode */
    size_t len;		    /* length of encoding */
    const char * const enc; /* JSON encoding of val */
};
static struct encode jenc[BYTE_VALUES] = {
    /* \x00 - \x0f */
    {0x00, 6, "\\u0000"}, {0x01, 6, "\\u0001"}, {0x02, 6, "\\u0002"}, {0x03, 6, "\\u0003"},
    {0x04, 6, "\\u0004"}, {0x05, 6, "\\u0005"}, {0x06, 6, "\\u0006"}, {0x07, 6, "\\u0007"},
    {0x08, 2, "\\b"}, {0x09, 2, "\\t"}, {0x0a, 2, "\\n"}, {0x0b, 6, "\\u000b"},
    {0x0c, 2, "\\f"}, {0x0d, 2, "\\r"}, {0x0e, 6, "\\u000e"}, {0x0f, 6, "\\u000f"},

    /* \x10 - \x1f */
    {0x10, 6, "\\u0010"}, {0x11, 6, "\\u0011"}, {0x12, 6, "\\u0012"}, {0x13, 6, "\\u0013"},
    {0x14, 6, "\\u0014"}, {0x15, 6, "\\u0015"}, {0x16, 6, "\\u0016"}, {0x17, 6, "\\u0017"},
    {0x18, 6, "\\u0018"}, {0x19, 6, "\\u0019"}, {0x1a, 6, "\\u001a"}, {0x1b, 6, "\\u001b"},
    {0x1c, 6, "\\u001c"}, {0x1d, 6, "\\u001d"}, {0x1e, 6, "\\u001e"}, {0x1f, 6, "\\u001f"},

    /* \x20 - \x2f */
    {' ', 1, " "}, {'!', 1, "!"}, {'"', 2, "\\\""}, {'#', 1, "#"},
    {'$', 1, "$"}, {'%', 1, "%"}, {'&', 6, "\\u0026"}, {'\'', 1, "'"},
    {'(', 1, "("}, {')', 1, ")"}, {'*', 1, "*"}, {'+', 1, "+"},
    {',', 1, ","}, {'-', 1, "-"}, {'.', 1, "."}, {'/', 2, "\\/"},

    /* \x30 - \x3f */
    {'0', 1, "0"}, {'1', 1, "1"}, {'2', 1, "2"}, {'3', 1, "3"},
    {'4', 1, "4"}, {'5', 1, "5"}, {'6', 1, "6"}, {'7', 1, "7"},
    {'8', 1, "8"}, {'9', 1, "9"}, {':', 1, ":"}, {';', 1, ";"},
    {'<', 6, "\\u003c"}, {'=', 1, "="}, {'>', 6, "\\u003e"}, {'?', 1, "?"},

    /* \x40 - \x4f */
    {'@', 1, "@"}, {'A', 1, "A"}, {'B', 1, "B"}, {'C', 1, "C"},
    {'D', 1, "D"}, {'E', 1, "E"}, {'F', 1, "F"}, {'G', 1, "G"},
    {'H', 1, "H"}, {'I', 1, "I"}, {'J', 1, "J"}, {'K', 1, "K"},
    {'L', 1, "L"}, {'M', 1, "M"}, {'N', 1, "N"}, {'O', 1, "O"},

    /* \x50 - \x5f */
    {'P', 1, "P"}, {'Q', 1, "Q"}, {'R', 1, "R"}, {'S', 1, "S"},
    {'T', 1, "T"}, {'U', 1, "U"}, {'V', 1, "V"}, {'W', 1, "W"},
    {'X', 1, "X"}, {'Y', 1, "Y"}, {'Z', 1, "Z"}, {'[', 1, "["},
    {'\\', 2, "\\\\"}, {']', 1, "]"}, {'^', 1, "^"}, {'_', 1, "_"},

    /* \x60 - \x6f */
    {'`', 1, "`"}, {'a', 1, "a"}, {'b', 1, "b"}, {'c', 1, "c"},
    {'d', 1, "d"}, {'e', 1, "e"}, {'f', 1, "f"}, {'g', 1, "g"},
    {'h', 1, "h"}, {'i', 1, "i"}, {'j', 1, "j"}, {'k', 1, "k"},
    {'l', 1, "l"}, {'m', 1, "m"}, {'n', 1, "n"}, {'o', 1, "o"},

    /* \x70 - \x7f */
    {'p', 1, "p"}, {'q', 1, "q"}, {'r', 1, "r"}, {'s', 1, "s"},
    {'t', 1, "t"}, {'u', 1, "u"}, {'v', 1, "v"}, {'w', 1, "w"},
    {'x', 1, "x"}, {'y', 1, "y"}, {'z', 1, "z"}, {'{', 1, "{"},
    {'|', 1, "|"}, {'}', 1, "}"}, {'~', 1, "~"}, {0x7f, 6, "\\u007f"},

    /* \x80 - \x8f */
    {0x80, 6, "\\u0080"}, {0x81, 6, "\\u0081"}, {0x82, 6, "\\u0082"}, {0x83, 6, "\\u0083"},
    {0x84, 6, "\\u0084"}, {0x85, 6, "\\u0085"}, {0x86, 6, "\\u0086"}, {0x87, 6, "\\u0087"},
    {0x88, 6, "\\u0088"}, {0x89, 6, "\\u0089"}, {0x8a, 6, "\\u008a"}, {0x8b, 6, "\\u008b"},
    {0x8c, 6, "\\u008c"}, {0x8d, 6, "\\u008d"}, {0x8e, 6, "\\u008e"}, {0x8f, 6, "\\u008f"},

    /* \x90 - \x9f */
    {0x90, 6, "\\u0090"}, {0x91, 6, "\\u0091"}, {0x92, 6, "\\u0092"}, {0x93, 6, "\\u0093"},
    {0x94, 6, "\\u0094"}, {0x95, 6, "\\u0095"}, {0x96, 6, "\\u0096"}, {0x97, 6, "\\u0097"},
    {0x98, 6, "\\u0098"}, {0x99, 6, "\\u0099"}, {0x9a, 6, "\\u009a"}, {0x9b, 6, "\\u009b"},
    {0x9c, 6, "\\u009c"}, {0x9d, 6, "\\u009d"}, {0x9e, 6, "\\u009e"}, {0x9f, 6, "\\u009f"},

    /* \xa0 - \xaf */
    {0xa0, 6, "\\u00a0"}, {0xa1, 6, "\\u00a1"}, {0xa2, 6, "\\u00a2"}, {0xa3, 6, "\\u00a3"},
    {0xa4, 6, "\\u00a4"}, {0xa5, 6, "\\u00a5"}, {0xa6, 6, "\\u00a6"}, {0xa7, 6, "\\u00a7"},
    {0xa8, 6, "\\u00a8"}, {0xa9, 6, "\\u00a9"}, {0xaa, 6, "\\u00aa"}, {0xab, 6, "\\u00ab"},
    {0xac, 6, "\\u00ac"}, {0xad, 6, "\\u00ad"}, {0xae, 6, "\\u00ae"}, {0xaf, 6, "\\u00af"},

    /* \xb0 - \xbf */
    {0xb0, 6, "\\u00b0"}, {0xb1, 6, "\\u00b1"}, {0xb2, 6, "\\u00b2"}, {0xb3, 6, "\\u00b3"},
    {0xb4, 6, "\\u00b4"}, {0xb5, 6, "\\u00b5"}, {0xb6, 6, "\\u00b6"}, {0xb7, 6, "\\u00b7"},
    {0xb8, 6, "\\u00b8"}, {0xb9, 6, "\\u00b9"}, {0xba, 6, "\\u00ba"}, {0xbb, 6, "\\u00bb"},
    {0xbc, 6, "\\u00bc"}, {0xbd, 6, "\\u00bd"}, {0xbe, 6, "\\u00be"}, {0xbf, 6, "\\u00bf"},

    /* \xc0 - \xcf */
    {0xc0, 6, "\\u00c0"}, {0xc1, 6, "\\u00c1"}, {0xc2, 6, "\\u00c2"}, {0xc3, 6, "\\u00c3"},
    {0xc4, 6, "\\u00c4"}, {0xc5, 6, "\\u00c5"}, {0xc6, 6, "\\u00c6"}, {0xc7, 6, "\\u00c7"},
    {0xc8, 6, "\\u00c8"}, {0xc9, 6, "\\u00c9"}, {0xca, 6, "\\u00ca"}, {0xcb, 6, "\\u00cb"},
    {0xcc, 6, "\\u00cc"}, {0xcd, 6, "\\u00cd"}, {0xce, 6, "\\u00ce"}, {0xcf, 6, "\\u00cf"},

    /* \xd0 - \xdf */
    {0xd0, 6, "\\u00d0"}, {0xd1, 6, "\\u00d1"}, {0xd2, 6, "\\u00d2"}, {0xd3, 6, "\\u00d3"},
    {0xd4, 6, "\\u00d4"}, {0xd5, 6, "\\u00d5"}, {0xd6, 6, "\\u00d6"}, {0xd7, 6, "\\u00d7"},
    {0xd8, 6, "\\u00d8"}, {0xd9, 6, "\\u00d9"}, {0xda, 6, "\\u00da"}, {0xdb, 6, "\\u00db"},
    {0xdc, 6, "\\u00dc"}, {0xdd, 6, "\\u00dd"}, {0xde, 6, "\\u00de"}, {0xdf, 6, "\\u00df"},

    /* \xe0 - \xef */
    {0xe0, 6, "\\u00e0"}, {0xe1, 6, "\\u00e1"}, {0xe2, 6, "\\u00e2"}, {0xe3, 6, "\\u00e3"},
    {0xe4, 6, "\\u00e4"}, {0xe5, 6, "\\u00e5"}, {0xe6, 6, "\\u00e6"}, {0xe7, 6, "\\u00e7"},
    {0xe8, 6, "\\u00e8"}, {0xe9, 6, "\\u00e9"}, {0xea, 6, "\\u00ea"}, {0xeb, 6, "\\u00eb"},
    {0xec, 6, "\\u00ec"}, {0xed, 6, "\\u00ed"}, {0xee, 6, "\\u00ee"}, {0xef, 6, "\\u00ef"},

    /* \xf0 - \xff */
    {0xf0, 6, "\\u00f0"}, {0xf1, 6, "\\u00f1"}, {0xf2, 6, "\\u00f2"}, {0xf3, 6, "\\u00f3"},
    {0xf4, 6, "\\u00f4"}, {0xf5, 6, "\\u00f5"}, {0xf6, 6, "\\u00f6"}, {0xf7, 6, "\\u00f7"},
    {0xf8, 6, "\\u00f8"}, {0xf9, 6, "\\u00f9"}, {0xfa, 6, "\\u00fa"}, {0xfb, 6, "\\u00fb"},
    {0xfc, 6, "\\u00fc"}, {0xfd, 6, "\\u00fd"}, {0xfe, 6, "\\u00fe"}, {0xff, 6, "\\u00ff"}
};


/*
 * hexval - concert ASCII character to hex value
 *
 * NOTE: -1 means the ASCII character is not a value hex character
 */
static int hexval[BYTE_VALUES] = {
    /* \x00 - \x0f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x10 - \x1f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x20 - \x2f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x30 - \x3f */
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    /* \x40 - \x4f */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x50 - \x5f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x60 - \x6f */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x70 - \x7f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x80 - \x8f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x90 - \x9f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xa0 - \xaf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xb0 - \xbf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xc0 - \xcf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xd0 - \xdf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xe0 - \xef */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xf0 - \xff */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


/*
 * malloc_json_encode - return a JSON encoding of a block of memory
 *
 * JSON string encoding:
 *
 * These escape characters are required by JSON:
 *
 *      old			new
 *      ----------------------------
 *	<backspace>		\b	(\x08)
 *	<horizontal_tab>	\t	(\x09)
 *	<newline>		\n	(\x0a)
 *	<form_feed>		\f	(\x0c)
 *	<enter>			\r	(\x0d)
 *	"			\"	(\x22)
 *	/			\/	(\x2f)
 *	\			\\	(\x5c)
 *
 * These escape characters are implied by JSON due to
 * HTML and XML encoding, although not strictly required:
 *
 *      old		new
 *      --------------------
 *	&		\u0026		(\x26)
 *	<		\u003c		(\x3c)
 *	>		\u003e		(\x3e)
 *
 * These escape characters are implied by JSON to let humans
 * view JSON without worrying about characters that might
 * not display / might not be printable:
 *
 *      old			new
 *      ----------------------------
 *	\x00-\x07		\u0000 - \u0007
 *	\x0b			\u000b <vertical_tab>
 *	\x0e-\x1f		\u000e - \x001f
 *	\x7f-\xff		\u007f - \u00ff
 *
 * See:
 *
 *	https://developpaper.com/escape-and-unicode-encoding-in-json-serialization/
 *
 * NOTE: We chose to not escape '%' as was suggested by the above URL
 *	 because it is neither required by JSON nor implied by JSON.
 *
 * NOTE: While there exist C escapes for characters such as '\v',
 *	 due to flaws in the JSON spec, we must encode such characters
 *	 using the \uffff notation.
 *
 * given:
 *	ptr	start of memory block to encode
 *	len	length of block to encode in bytes
 *	retlen	address of where to store malloced length, if retlen != NULL
 *
 * returns:
 *	malloced JSON encoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
malloc_json_encode(char const *ptr, size_t len, size_t *retlen)
{
    char *ret = NULL;	    /* malloced encoding string or NULL */
    char *beyond = NULL;    /* beyond the end of the malloced encoding string */
    size_t mlen = 0;	    /* length of malloced encoded string */
    char *p;		    /* next place to encode */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %lu must be > 0", (unsigned long) len);
	return NULL;
    }

    /*
     * count the bytes that will be in the encoded malloced string
     */
    for (i=0; i < len; ++i) {
	mlen += jenc[(uint8_t)(ptr[i])].len;
    }

    /*
     * malloc the encoded string
     */
    ret = malloc(mlen + 1 + 1);
    if (ret == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "malloc of %ld bytes failed", (unsigned long)(mlen + 1 + 1));
	return NULL;
    }
    ret[mlen] = '\0';   /* terminate string */
    ret[mlen + 1] = '\0';   /* paranoia */
    beyond = &(ret[mlen]);

    /*
     * JSON encode each byte
     */
    for (i=0, p=ret; i < len; ++i) {
	if (p+jenc[(uint8_t)(ptr[i])].len > beyond) {
	    /* error - clear malloced length */
	    if (retlen != NULL) {
		*retlen = 0;
	    }
	    warn(__func__, "encoding ran beyond end of malloced encoded string");
	    return NULL;
	}
	strcpy(p, jenc[(uint8_t)(ptr[i])].enc);
	p += jenc[(uint8_t)(ptr[i])].len;
    }

    /*
     * return result
     */
    dbg(DBG_VVVHIGH, "returning from malloc_json_encode(ptr, %lu, *%lu)",
		     (unsigned long)len, (unsigned long)mlen);
    if (retlen != NULL) {
	*retlen = mlen;
    }
    return ret;
}


/*
 * malloc_json_encode_str - return a JSON encoding of a string
 *
 * This is an simplified interface for malloc_json_encode().
 *
 * given:
 *	str	a string to encode
 *	retlen	address of where to store malloced length, if retlen != NULL
 *
 * returns:
 *	malloced JSON encoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
malloc_json_encode_str(char const *str, size_t *retlen)
{
    void *ret = NULL;	    /* malloced encoding string or NULL */
    size_t len = 0;	    /* length of string to encode */

    /*
     * firewall
     */
    if (str == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    len = strlen(str);
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %lu must be > 0", (unsigned long) len);
	return NULL;
    }

    /*
     * convert to malloc_json_encode() call
     */
    ret = malloc_json_encode(str, len, retlen);
    if (ret == NULL) {
	dbg(DBG_VVHIGH, "returning NULL for encoding of: <%s>", str);
    } else {
	dbg(DBG_VVHIGH, "string: JSON encoded as: <%s>", (char *)ret);
    }

    /*
     * return encoded result or NULL
     */
    return ret;
}


/*
 * jencchk - validate the contents of the jenc[] table
 *
 * This function performs various sanity checks on the jenc[] table.
 *
 * This function does not return on error.
 */
void
jencchk(void)
{
    unsigned int hexval;/* hex value from xxxx part of \uxxxx */
    char guard;		/* scanf guard to catch excess amount of input */
    int indx;		/* test index */
    char const *encstr;	/* encoding string */
    int ret;		/* libc function return value */
    char str[2];	/* character string to encode */
    char *mstr = NULL;	/* malloced encoding string */
    size_t mlen = 0;	/* length of malloced encoding string */
    char *mstr2 = NULL;	/* malloced strict decoding string */
    size_t mlen2 = 0;	/* length of malloced strict decoding string */
    char *mstr3 = NULL;	/* malloced non-strict decoding string */
    size_t mlen3 = 0;	/* length of malloced non-strict decoding string */
    unsigned int i;

    /*
     * assert: bits in byte must be 8
     */
    if (BITS_IN_BYTE != 8) {
	err(150, __func__, "BITS_IN_BYTE: %d != 8", BITS_IN_BYTE);
	not_reached();
    }

    /*
     * assert: table must be 256 elements long
     */
    if (sizeof(jenc)/sizeof(jenc[0]) != BYTE_VALUES) {
	err(151, __func__, "jenc table as %lu elements instead of %d",
			   (unsigned long)sizeof(jenc)/sizeof(jenc[0]), BYTE_VALUES);
	not_reached();
    }

    /*
     * assert: byte must be an index from 0 to 256
     */
    for (i=0; i < BYTE_VALUES; ++i) {
	if (jenc[i].byte != i) {
	    err(152, __func__, "jenc[0x%02x].byte: %d != %d", i, jenc[i].byte, i);
	    not_reached();
	}
    }

    /*
     * assert: enc string must be non-NULL
     */
    for (i=0; i < BYTE_VALUES; ++i) {
	if (jenc[i].enc == NULL) {
	    err(153, __func__, "jenc[0x%02x].enc == NULL", i);
	    not_reached();
	}
    }

    /*
     * assert: length of enc string must match len
     */
    for (i=0; i < BYTE_VALUES; ++i) {
	if (strlen(jenc[i].enc) != jenc[i].len) {
	    err(154, __func__, "jenc[0x%02x].enc length: %lu != jenc[0x%02x].len: %lu",
			       i, (unsigned long)strlen(jenc[i].enc),
			       i, (unsigned long)jenc[i].len);
	    not_reached();
	}
    }

    /*
     * assert: \x00-\x07 encodes to \uxxxx
     */
    for (i=0x00; i <= 0x07; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(155, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			       i, (unsigned long)strlen(jenc[i].enc),
			       (unsigned long)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &hexval, &guard);
	if (ret != 1) {
	    err(156, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != hexval) {
	    err(157, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * assert: \x08 encodes to \b
     */
    indx = 0x08;
    encstr = "\\b";
    if (jenc[indx].len != LITLEN("\\b")) {
	err(158, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(159, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x09 encodes to \t
     */
    indx = 0x09;
    encstr = "\\t";
    if (jenc[indx].len != LITLEN("\\b")) {
	err(160, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(161, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0a encodes to \n
     */
    indx = 0x0a;
    encstr = "\\n";
    if (jenc[indx].len != strlen(encstr)) {
	err(162, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(163, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0b encodes to \u000b
     */
    indx = 0x0b;
    encstr = "\\u000b";
    if (jenc[indx].len != strlen(encstr)) {
	err(164, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(165, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0c encodes to \f
     */
    indx = 0x0c;
    encstr = "\\f";
    if (jenc[indx].len != strlen(encstr)) {
	err(166, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(167, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0d encodes to \r
     */
    indx = 0x0d;
    encstr = "\\r";
    if (jenc[indx].len != strlen(encstr)) {
	err(168, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(169, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0e-\x1f encodes to \uxxxx
     */
    for (i=0x0e; i <= 0x1f; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(170, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			       i, (unsigned long)strlen(jenc[i].enc),
			       (unsigned long)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &hexval, &guard);
	if (ret != 1) {
	    err(171, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != hexval) {
	    err(172, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * assert: \x20-\x21 encodes to the character
     */
    for (i=0x20; i <= 0x21; ++i) {
	if (jenc[i].len != 1) {
	    err(173, __func__, "jenc[0x%02x].enc length: %lu != %d",
			       i, (unsigned long)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(174, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x22 encodes to \"
     */
    indx = 0x22;
    encstr = "\\\"";
    if (jenc[indx].len != strlen(encstr)) {
	err(175, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(176, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x23-\x25 encodes to the character
     */
    for (i=0x23; i <= 0x25; ++i) {
	if (jenc[i].len != 1) {
	    err(177, __func__, "jenc[0x%02x].enc length: %lu != %d",
			       i, (unsigned long)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(178, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x26 encodes to \u0026
     */
    indx = 0x26;
    encstr = "\\u0026";
    if (jenc[indx].len != strlen(encstr)) {
	err(179, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(180, __func__, "jenc[0x%02x].enc: <%s> is not <\\b> form", indx, encstr);
	not_reached();
    }

    /*
     * assert: \x27-\x2e encodes to the character
     */
    for (i=0x27; i <= 0x2e; ++i) {
	if (jenc[i].len != 1) {
	    err(181, __func__, "jenc[0x%02x].enc length: %lu != %d",
			       i, (unsigned long)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(182, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x2f encodes to \/
     */
    indx = 0x2f;
    encstr = "\\/";
    if (jenc[indx].len != strlen(encstr)) {
	err(183, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(184, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x30-\x3b encodes to the character
     */
    for (i=0x30; i <= 0x3b; ++i) {
	if (jenc[i].len != 1) {
	    err(185, __func__, "jenc[0x%02x].enc length: %lu != %d",
			       i, (unsigned long)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(186, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x3c encodes to \u003c
     */
    indx = 0x3c;
    encstr = "\\u003c";
    if (jenc[indx].len != strlen(encstr)) {
	err(187, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(188, __func__, "jenc[0x%02x].enc: <%s> is not <\\b> form", indx, encstr);
	not_reached();
    }

    /*
     * assert: \x3d-\x3d encodes to the character
     */
    for (i=0x3d; i <= 0x3d; ++i) {
	if (jenc[i].len != 1) {
	    err(189, __func__, "jenc[0x%02x].enc length: %lu != %d",
			       i, (unsigned long)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(190, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x3e encodes to \u003e
     */
    indx = 0x3e;
    encstr = "\\u003e";
    if (jenc[indx].len != strlen(encstr)) {
	err(191, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(192, __func__, "jenc[0x%02x].enc: <%s> is not in <\\b> form", indx, encstr);
	not_reached();
    }

    /*
     * assert: \x3f-\x5b encodes to the character
     */
    for (i=0x3f; i <= 0x5b; ++i) {
	if (jenc[i].len != 1) {
	    err(193, __func__, "jenc[0x%02x].enc length: %lu != %d",
			       i, (unsigned long)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(194, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x5c encodes to \\
     */
    indx = 0x5c;
    encstr = "\\\\";
    if (jenc[indx].len != strlen(encstr)) {
	err(195, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			   indx, (unsigned long)strlen(jenc[indx].enc),
			   (unsigned long)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(196, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x5d-\x7e encodes to the character
     */
    for (i=0x5d; i <= 0x7e; ++i) {
	if (jenc[i].len != 1) {
	    err(197, __func__, "jenc[0x%02x].enc length: %lu != %d",
			       i, (unsigned long)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(198, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x7f-\xff encodes to \uxxxx
     */
    for (i=0x7f; i <= 0xff; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(199, __func__, "jenc[0x%02x].enc length: %lu != %lu",
			       i, (unsigned long)strlen(jenc[i].enc),
			       (unsigned long)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &hexval, &guard);
	if (ret != 1) {
	    err(200, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != hexval) {
	    err(201, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * special test for encoding a NUL byte
     */
    dbg(DBG_VVVHIGH, "testing malloc_json_encode(0x00, 1, *mlen)");
    memset(str, 0, sizeof(str));    /* clear all bytes in str, including the final \0 */
    mstr = malloc_json_encode(str, 1,  &mlen);
    if (mstr == NULL) {
	err(202, __func__, "malloc_json_encode(0x00, 1, *mlen: %lu) == NULL", (unsigned long)mlen);
	not_reached();
    }
    if (mlen != jenc[0].len) {
	err(203, __func__, "malloc_json_encode(0x00, 1, *mlen: %lu != %lu)",
			   (unsigned long)mlen, (unsigned long)(jenc[0].len));
	not_reached();
    }
    if (strcmp(jenc[0].enc, mstr) != 0) {
	err(204, __func__, "malloc_json_encode(0x00, 1, *mlen: %lu) != <%s>",
			   (unsigned long)mlen, jenc[0].enc);
	not_reached();
    }
    /* free the malloced encoded string */
    if (mstr != NULL) {
	free(mstr);
	mstr = NULL;
    }

    /* XXX = test decoding \u0000 */

    /*
     * finally try to encode every possible string with a single non-NUL character
     */
    for (i=1; i < BYTE_VALUES; ++i) {

	/*
	 * test JSON encoding
	 */
	dbg(DBG_VVVHIGH, "testing malloc_json_encode_str(0x%02x, *mlen)", i);
	/* load input string */
	str[0] = (char)i;
	/* test malloc_json_encode_str() */
	mstr = malloc_json_encode_str(str, &mlen);
	/* check encoding result */
	if (mstr == NULL) {
	    err(205, __func__, "malloc_json_encode_str(0x%02x, *mlen: %lu) == NULL",
			       i, (unsigned long)mlen);
	    not_reached();
	}
	if (mlen != jenc[i].len) {
	    err(206, __func__, "malloc_json_encode_str(0x%02x, *mlen %lu != %lu)",
			       i, (unsigned long)mlen, (unsigned long)jenc[i].len);
	    not_reached();
	}
	if (strcmp(jenc[i].enc, mstr) != 0) {
	    err(207, __func__, "malloc_json_encode_str(0x%02x, *mlen: %lu) != <%s>", i,
			       (unsigned long)mlen, jenc[i].enc);
	    not_reached();
	}
	dbg(DBG_VVHIGH, "testing malloc_json_encode_str(0x%02x, *mlen) encoded to <%s>", i, mstr);

	/*
	 * test strict decoding the JSON encoded string
	 */
	dbg(DBG_VVVHIGH, "testing malloc_json_decode_str(<%s>, *mlen, true)", mstr);
	/* test malloc_json_decode_str() */
	mstr2 = malloc_json_decode_str(mstr, &mlen2, true);
	if (mstr2 == NULL) {
	    err(208, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, true) == NULL",
			       mstr, (unsigned long)mlen2);
	    not_reached();
	}
	if (mlen2 != 1) {
	    err(209, __func__, "malloc_json_decode_str(<%s>, *mlen2 %lu != 1, true)",
			       mstr, (unsigned long)mlen2);
	    not_reached();
	}
	if ((uint8_t)(mstr2[0]) != i) {
	    err(210, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, true): 0x%02x != 0x%02x",
			       mstr, (unsigned long)mlen2, (uint8_t)(mstr2[0]), i);
	    not_reached();
	}

	/*
	 * test non-strict decoding the JSON encoded string
	 */
	dbg(DBG_VVVHIGH, "testing malloc_json_decode_str(<%s>, *mlen, false)", mstr);
	/* test malloc_json_decode_str() */
	mstr3 = malloc_json_decode_str(mstr, &mlen3, false);
	if (mstr3 == NULL) {
	    err(211, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, false) == NULL",
			       mstr, (unsigned long)mlen3);
	    not_reached();
	}
	if (mlen3 != 1) {
	    err(212, __func__, "malloc_json_decode_str(<%s>, *mlen3 %lu != 1, false)",
			       mstr, (unsigned long)mlen3);
	    not_reached();
	}
	if ((uint8_t)(mstr3[0]) != i) {
	    err(213, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, false): 0x%02x != 0x%02x",
			       mstr, (unsigned long)mlen3, (uint8_t)(mstr3[0]), i);
	    not_reached();
	}

	/* free the malloced encoded string */
	if (mstr != NULL) {
	    free(mstr);
	    mstr = NULL;
	}
	if (mstr2 != NULL) {
	    free(mstr2);
	    mstr2 = NULL;
	}
	if (mstr3 != NULL) {
	    free(mstr3);
	    mstr3 = NULL;
	}
    }

    /*
     * all seems well win the jenc[] table
     */
    dbg(DBG_VVHIGH, "jenc[] table passes");
    return;
}


/*
 * json_putc - print a UTF-8 character with JSON encoding
 *
 * given:
 *	stream	- string to print on
 *	c	- UTF-8 character to encode
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
bool
json_putc(uint8_t const c, FILE *stream)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * write JSON encoding to stream
     */
    ret = fprintf(stream, "%s", jenc[c].enc);
    if (ret <= 0) {
	warnp(__func__, "fprintf #1 error");
	return false;
    }
    return true;
}


/*
 * malloc_json_decode - return a decoding of a block of JSON encoded memory
 *
 * given:
 *	ptr	start of memory block to decode
 *	len	length of block to decode in bytes
 *	retlen	address of where to store malloced length, if retlen != NULL
 *	strict	true ==> strict decoding based on how malloc_json_encode() encodes
 *	        false ==> permit a wider use of \-escaping and un-encoded char
 *
 *		    NOTE: strict == false implies a strict reading of the JSON spec
 *
 * returns:
 *	malloced JSON decoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
malloc_json_decode(char const *ptr, size_t len, size_t *retlen, bool strict)
{
    char *ret = NULL;	    /* malloced encoding string or NULL */
    char *beyond = NULL;    /* beyond the end of the malloced encoding string */
    size_t mlen = 0;	    /* length of malloced encoded string */
    char *p = NULL;	    /* next place to encode */
    char n = 0;		    /* next character beyond a \\ */
    char a = 0;		    /* 1st hex character after \u */
    int xa = 0;		    /* 1st hex character numeric value */
    char b = 0;		    /* 2nd hex character after \u */
    int xb = 0;		    /* 2nd hex character numeric value */
    char c = 0;		    /* character to decode or 3rd hex character after \u */
    int xc = 0;		    /* 3nd hex character numeric value */
    char d = 0;		    /* 4th hex character after \u */
    int xd = 0;		    /* 4th hex character numeric value */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %lu must be > 0", (unsigned long) len);
	return NULL;
    }

    /*
     * count the bytes that will be in the decoded malloced string
     */
    for (i=0; i < len; ++i) {

	/*
	 * examine the current character
	 */
	c = (char)((uint8_t)(ptr[i]));

	/*
	 * valid non-\-escaped characters count as 1
	 */
        if (c != '\\') {

	    /*
	     * case: strict encoding based on malloc_json_encode() \-escapes
	     */
	    if (strict == true) {

		/*
		 * disallow characters that should have been escaped
		 */
		if (c <= 0x1f || c >= 0x7f) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "strict encoding at %lu found un-escaped char: 0x%02x", (unsigned long)i, (uint8_t)c);
		    return NULL;
		}
		switch (c) {
		case '"':   /*fallthru*/
		case '/':   /*fallthru*/
		case '\\':  /*fallthru*/
		case '&':   /*fallthru*/
		case '<':   /*fallthru*/
		case '>':
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "strict encoding at %lu found un-escaped char: %c", (unsigned long)i, c);
		    return NULL;
		    break;

		/*
		 * count a valid character
		 */
		default:
		    ++mlen;
		    break;
		}

	    /*
	     * case: non-strict encoding - only what JSON mandates
	     */
	    } else {

		/*
		 * disallow characters that should have been escaped
		 */
		switch (c) {
		case '\b':  /*fallthru*/
		case '\t':  /*fallthru*/
		case '\n':  /*fallthru*/
		case '\f':  /*fallthru*/
		case '\r':
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "non-strict encoding found \\-escaped char: 0x%02x", (uint8_t)c);
		    return NULL;
		    break;
		case '"':   /*fallthru*/
		case '/':   /*fallthru*/
		case '\\':
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "non-strict encoding found \\-escaped char: %c", c);
		    return NULL;
		    break;

		/*
		 * count a valid character
		 */
		default:
		    ++mlen;
		    break;
		}
	    }

	/*
	 * valid \-escaped characters count as 2 or 6
	 */
	} else {

	    /*
	     * there must be at least 1 more character beyond \
	     */
	    if (i+1 >= len) {
		/* error - clear malloced length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found \\ at end of buffer, missing next character");
		return NULL;
	    }

	    /*
	     * look at the next character beyond \
	     */
	    n = (char)((uint8_t)(ptr[i+1]));

	    /*
	     * process single \c escaped pairs
	     */
	    switch (n) {
	    case 'b':	/*fallthru*/
	    case 't':	/*fallthru*/
	    case 'n':	/*fallthru*/
	    case 'f':	/*fallthru*/
	    case 'r':	/*fallthru*/
	    case '"':	/*fallthru*/
	    case '/':	/*fallthru*/
	    case '\\':
		/*
		 * count \c escaped pair as 1 character
		 */
		++mlen;
		++i;
		break;

	    /*
	     * process \uxxxx where x is a hex character
	     */
	    case 'u':

		/*
		 * there must be at least 5 more characters beyond \
		 */
		if (i+5 >= len) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "found \\u, but not enough for 4 hex chars at end of buffer");
		    return NULL;
		}
		a = (char)((uint8_t)(ptr[i+2]));
		b = (char)((uint8_t)(ptr[i+3]));
		c = (char)((uint8_t)(ptr[i+4]));
		d = (char)((uint8_t)(ptr[i+5]));

		/*
		 * the next 4 characters beyond \u must be hex characters
		 */
		if (isxdigit(a) && isxdigit(b) && isxdigit(c) && isxdigit(d)) {

		    /*
		     * case: strict encoding - the 4 hex characters must be lower case
		     */
		    if (strict == true &&
		        (isupper(a) || isupper(b) || isupper(c) || isupper(d))) {
			/* error - clear malloced length */
			if (retlen != NULL) {
			    *retlen = 0;
			}
			warn(__func__, "strict mode found \\u, some of the  4 hex chars are UPPERCASE: 0x%02X%02X%02X%02X",
				       (uint8_t)a, (uint8_t)b, (uint8_t)c, (uint8_t)d);
			return NULL;
		    }

		    /*
		     * count \uxxxx set as 1 character
		     */
		    ++mlen;
		    i += 5;
		    break;
		}
		break;

	    /*
	     * valid C escape sequence but unusual JSON \-escape character
	     */
	    case 'a':	/* ASCII bell */ /*fallthru*/
	    case 'v':	/* ASCII vertical tab */ /*fallthru*/
	    case 'e':	/* ASCII escape */

		/*
		 * case: strict mode - invalid unusual \-escape character
		 */
		if (strict == true) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "strict mode found an unusual JSON \\-escape: \\%c", (uint8_t)c);
		    return NULL;
		}

		/*
		 * case: non-strict - count \c escaped pair as 1 character
		 */
		++mlen;
		++i;
		break;

	    /*
	     * found invalid JSON \-escape character
	     */
	    default:
		/* error - clear malloced length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found invalid JSON \\-escape: followed by 0x%02x", (uint8_t)c);
		return NULL;
	    }
	}
    }

    /*
     * malloced decoded string
     */
    ret = malloc(mlen + 1 + 1);
    if (ret == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "malloc of %ld bytes failed", (unsigned long)(mlen + 1 + 1));
	return NULL;
    }
    ret[mlen] = '\0';   /* terminate string */
    ret[mlen + 1] = '\0';   /* paranoia */
    beyond = &(ret[mlen]);

    /*
     * JSON string decode
     *
     * In the above counting code, prior to the malloc for the decoded string,
     * we determined that the JSON encoded string is valid.  We can now safely
     * decode regardless of strict mode.
     */
    for (i=0, p=ret; i < len; ++i) {

	/*
	 * examine the current character
	 */
	c = (char)((uint8_t)(ptr[i]));

	/*
	 * paranoia
	 */
	if (p >= beyond) {
	    /* error - clear malloced length */
	    if (retlen != NULL) {
		*retlen = 0;
	    }
	    warn(__func__, "ran beyond end of decoded string");
	    return NULL;
	}

	/*
	 * case: JSON decode non \-escape character
	 */
	if (c != '\\') {
	    /* no translation encoding */
	    *p++ = c;

	/*
	 * case: JSON decode \-escape character
	 */
	} else {

	    /*
	     * look at the next character beyond \
	     */
	    n = (char)((uint8_t)(ptr[i+1]));

	    /*
	     * decode single \c escaped pairs
	     */
	    switch (n) {
	    case 'b':	/* ASCII backspace */
		++i;
		*p++ = '\b';
		break;
	    case 't':	/* ASCII horizontal tab */
		++i;
		*p++ = '\t';
		break;
	    case 'n':	/* ASCII line feed */
		++i;
		*p++ = '\n';
		break;
	    case 'f':	/* ASCII form feed */
		++i;
		*p++ = '\f';
		break;
	    case 'r':	/* ASCII carriage return */
		++i;
		*p++ = '\r';
		break;
	    case 'a':	/* ASCII bell */
		++i;
		*p++ = '\a';
		break;
	    case 'v':	/* ASCII vertical tab */
		++i;
		*p++ = '\v';
		break;
	    case 'e':	/* ASCII escape */
		++i;
		*p++ = 0x0b;  /* no all C compilers understand /e */
		break;
	    case '"':	/*fallthru*/
	    case '/':	/*fallthru*/
	    case '\\':
		++i;
		*p++ = n;	/* escape decodes to itself */
		break;

	    /*
	     * decode \uxxxx
	     */
	    case 'u':

		/*
		 * there must be at least 5 more characters beyond \
		 */
		if (i+5 >= len) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "found \\u while decoding, but not enough for 4 hex chars at end of buffer");
		    return NULL;
		}
		xa = hexval[(uint8_t)(ptr[i+2])];
		xb = hexval[(uint8_t)(ptr[i+3])];
		xc = hexval[(uint8_t)(ptr[i+4])];
		xd = hexval[(uint8_t)(ptr[i+5])];

		/*
		 * case: \u00xx
		 */
		if (xa == 0 && xb == 0) {
		    i += 5;
		    *p++ = (char)((xc << 4) | xd);
		    break;

		/*
		 * case: \uxxxx
		 */
		} else {

		    /*
		     * case: strict mode: found non UTF-8 encoded character
		     */
		    if (strict == true) {
			/* error - clear malloced length */
			if (retlen != NULL) {
			    *retlen = 0;
			}
			warn(__func__, "strict mode: found non-UTF-8 \\u encoding: \\u%c%c%c%c", a,b,c,d);
			return NULL;

		    /*
		     * case; non-strict mode: found non UTF-8 encoded character
		     */
		    } else {

			/*
			 * paranoia
			 */
			if (p+1 >= beyond) {
			    /* error - clear malloced length */
			    if (retlen != NULL) {
				*retlen = 0;
			    }
			    warn(__func__, "ran beyond end of decoded string for non-UTF-8 \\u encoding");
			    return NULL;
			}
			i += 5;
			*p++ = (char)((xa << 4) | xb);
			*p++ = (char)((xc << 4) | xd);
			break;
		    }
		}
		break;

	    /*
	     * unknown \c escaped pairs
	     */
	    default:
		/* error - clear malloced length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found invalid JSON \\-escape while decoding: followed by 0x%02x", (uint8_t)c);
		return NULL;
	    }
	}
     }

    /*
     * return result
     */
    dbg(DBG_VVVHIGH, "returning from malloc_json_decode(ptr, %lu, *%lu)",
		     (unsigned long)len, (unsigned long)mlen);
    if (retlen != NULL) {
	*retlen = mlen;
    }
    return ret;
}


/*
 * malloc_json_decode_str - return a JSON decoding of a string
 *
 * This is a simplified interface for malloc_json_decode().
 *
 * given:
 *	str	a string to decode
 *	retlen	address of where to store malloced length, if retlen != NULL
 *	strict	true ==> strict decoding based on how malloc_json_decode() decodes
 *	        false ==> permit a wider use of \-escaping and un-decoded char
 *
 *		    NOTE: strict == false implies a strict reading of the JSON spec
 *
 *
 * returns:
 *	malloced JSON decoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
malloc_json_decode_str(char const *str, size_t *retlen, bool strict)
{
    void *ret = NULL;	    /* malloced decoding string or NULL */
    size_t len = 0;	    /* length of string to decode */

    /*
     * firewall
     */
    if (str == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    len = strlen(str);
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %lu must be > 0", (unsigned long) len);
	return NULL;
    }

    /*
     * convert to malloc_json_decode() call
     */
    ret = malloc_json_decode(str, len, retlen, strict);
    if (ret == NULL) {
	dbg(DBG_VVHIGH, "returning NULL for decoding of: <%s>", str);
    } else {
	dbg(DBG_VVHIGH, "string: <%s> JSON decoded", str);
    }

    /*
     * return decoded result or NULL
     */
    return ret;
}

/*
 * Common JSON fields table used to determine if a name is a common field,
 * whether it's been added to the found_common_json_fields list, how many times
 * it has been seen and how many are allowed.
 */
struct json_field common_json_fields[] =
{
    { "ioccc_contest",		    NULL, 0, 1, false, JSON_STRING, NULL },
    { "ioccc_year",		    NULL, 0, 1, false, JSON_NUMBER, NULL },
    { "mkiocccentry_version",	    NULL, 0, 1, false, JSON_STRING, NULL },
    { "iocccsize_version",	    NULL, 0, 1, false, JSON_STRING, NULL },
    { "IOCCC_contest_id",	    NULL, 0, 1, false, JSON_STRING, NULL },
    { "entry_num",		    NULL, 0, 1, false, JSON_NUMBER, NULL },
    { "author_count",		    NULL, 0, 1, false, JSON_NUMBER, NULL },
    { "tarball",		    NULL, 0, 1, false, JSON_STRING, NULL },
    { "formed_timestamp",	    NULL, 0, 1, false, JSON_NUMBER, NULL },
    { "formed_timestamp_usec",	    NULL, 0, 1, false, JSON_NUMBER, NULL },
    { "timestamp_epoch",	    NULL, 0, 1, false, JSON_STRING, NULL },
    { "min_timestamp",		    NULL, 0, 1, false, JSON_NUMBER, NULL },
    { "formed_UTC",		    NULL, 0, 1, false, JSON_STRING, NULL },
    { NULL,			    NULL, 0, 0, false, JSON_NULL,   NULL } /* XXX this **MUST** be last! */
};


/* find_json_field_in_table	    - find field 'name' in json table
 *
 * given:
 *
 *	table			    - the table to check
 *	name			    - the name to check
 *	loc			    - if != NULL set to index
 *
 * XXX The table MUST end with NULL!
 *
 * NOTE: If loc != NULL and the name is not found in the table then in the
 * calling function loc will be the number of entries in the table. To get the
 * number of entries in the table pass in a NULL pointer or zero length name.
 *
 * This function will return NULL if the field is not in the table. It will not
 * return if table is NULL.
 */
struct json_field *
find_json_field_in_table(struct json_field *table, char const *name, size_t *loc)
{
    struct json_field *field = NULL;	/* the found field or NULL if not found */
    size_t i = 0;			/* current index */
    /*
     * firewall
     */
    if (table == NULL) {
	err(214, __func__, "passed NULL arg table");
	not_reached();
    }

    /* zero length name is equivalent to NULL */
    if (!strlen(name)) {
	name = NULL;
    }

    /*
     * While the current index's name is not what we're looking for, continue to
     * the next field in the table.
     */
    for (i = 0; table[i].name != NULL; ++i) {
	if (name && !strcmp(table[i].name, name)) {
	    field = &table[i];
	    break;
	}
    }

    if (loc != NULL) {
	*loc = i;
    }

    return field;
}

/* check_common_json_fields_table	    - perform some sanity checks on the
 *					      common_json_fields table
 *
 * This function checks if JSON_NULL is used on any field other than the NULL
 * field. It also makes sure that each field_type is valid. More tests might be
 * devised later on but this is a good start (27 Feb 2022).
 *
 * This function does not return on error.
 */
void
check_common_json_fields_table(void)
{
    size_t loc;

    for (loc = 0; common_json_fields[loc].name != NULL; ++loc) {
	switch (common_json_fields[loc].field_type) {
	    case JSON_NULL:
		if (common_json_fields[loc].name != NULL) {
		    err(215, __func__, "found JSON_NULL element with non NULL name '%s' location %lu in common_json_fields table",
                            common_json_fields[loc].name, (unsigned long)loc);
		    not_reached();
		}
		break;
	    case JSON_NUMBER:
	    case JSON_BOOL:
	    case JSON_STRING:
	    case JSON_ARRAY:
	    case JSON_ARRAY_NUMBER:
	    case JSON_ARRAY_BOOL:
	    case JSON_ARRAY_STRING:
		/* these are all the valid types */
		break;
	    default:
		err(216, __func__, "found invalid data_type in common_json_fields table location %lu", (unsigned long)loc);
		not_reached();
		break;
	}
    }
}


/*
 * json_filename    - return ".info.json", ".author.json" or "null" depending on type
 *
 * given:
 *
 *	type	    - INFO_JSON or AUTHOR_JSON
 *
 * Returns a char * depending on type or "null" (NOT a NULL pointer!) if invalid
 * type.
 *
 * This function never returns NULL.
 *
 */
char const *
json_filename(int type)
{
    char const *name = INVALID_JSON_FILENAME; /* "null" */

    switch (type) {
	case INFO_JSON:
	    return INFO_JSON_FILENAME; /* ".info.json" */
	    break; /* in case the return is ever removed */
	case AUTHOR_JSON:
	    return AUTHOR_JSON_FILENAME; /* ".author.json" */
	    break; /* in case the return is ever removed */
	default:
	    break; /* in case the return is ever removed ... which it ironically was. */
    }
    return name;
}



/*
 * check_first_json_char - check if first char is '{'
 *
 * given:
 *
 *	file		- path to file
 *	data		- the data read in from the file
 *	strict		- true ==> disallow anything (including whitespace) before the first '{'.
 *	first		- if != NULL set *first to the first character
 *
 *  Returns 0 if first character is '{' and 1 if it is not.
 *
 *  Sets *first to the first character (for debugging purposes).
 *
 *  Does not return on NULL.
 */
int
check_first_json_char(char const *file, char *data, bool strict, char **first)
{
    /*
     * firewall
     */
    if (data == NULL || strlen(data) == 0) {
	err(217, __func__, "passed NULL or zero length data");
	not_reached();
    } else if (file == NULL || first == NULL) {
	err(218, __func__, "passed NULL arg(s)");
	not_reached();
    }

    if (!strict) {
	while (*data && isspace(*data))
	    ++data;
    }
    if (first != NULL) {
	*first = data;
    }

    if (*data != '{')
	return 1;
    return 0;
}

/*
 * check_last_json_char - check if last char is '}'
 *
 * given:
 *
 *	file		- path to file
 *	data		- the data read in from the file
 *	strict		- true ==> permit only a single trailing newline ("\n") after the last '}'.
 *	last		- if != NULL set *last to last char
 *
 *  Returns 0 if last character is '}' and 1 if it is not.
 *
 *  Does not return on error.
 */
int
check_last_json_char(char const *file, char *data, bool strict, char **last)
{
    char *p;

    /*
     * firewall
     */
    if (data == NULL || strlen(data) == 0) {
	err(219, __func__, "passed NULL or zero length data");
	not_reached();
    } else if (file == NULL || last == NULL) {
	err(220, __func__, "passed NULL arg(s)");
	not_reached();
    }

    p = data + strlen(data) - 1;

    if (!strict) {
	if (*p && isspace(*p))
	    --p;
    }
    else if (*p && *p == '\n') {
	--p;
    }
    if (last != NULL) {
	*last = p;
    }
    if (*p != '}')
	return 1;

    return 0;
}

/* add_found_common_json_field   - add a common json field to found_common_json_fields list
 *
 *
 * given:
 *
 *	name	    - name of the field
 *	val	    - value of the field
 *
 * Allocates a struct json_field * and add it to the found_json_common_fields
 * list. This will be used for reporting errors after parsing the file as well
 * as (later on) verifying that these fields are present in both .info.json and
 * .author.json (this detail is not yet completely thought out).
 *
 * This function returns the newly allocated struct json_field OR if the field
 * was already in the list, the previous one with the count incremented and the
 * value added to the values list.
 */
struct json_field *
add_found_common_json_field(char const *name, char const *val)
{
    struct json_field *field; /* newly allocated field */
    struct json_field *field_in_table = NULL;
    size_t loc = 0; /* location in table */

    /*
     * firewall
     */
    if (name == NULL || val == NULL) {
	err(221, __func__, "passed NULL arg(s)");
	not_reached();
    }

    field_in_table = find_json_field_in_table(common_json_fields, name, &loc);
    if (field_in_table == NULL) {
	err(222, __func__, "called add_found_common_json_field() on uncommon field '%s'", name);
	not_reached();
    }
    /*
     * Set in table that it's found and increment the number of times it's been
     * seen.
     */
    common_json_fields[loc].count++;
    common_json_fields[loc].found = true;
    for (field = found_common_json_fields; field != NULL; field = field->next) {
	if (field->name && !strcmp(field->name, name)) {
	    field->count++;
	    if (add_json_value(field, val) == NULL) {
		err(223, __func__, "couldn't add value '%s' to field '%s'", val, field->name);
		not_reached();
	    }
	    return field;
	}
    }

    field = new_json_field(name, val);
    if (field == NULL) {
	/* this should NEVER be reached but we check just to be sure */
	err(224, __func__, "new_json_field() returned NULL pointer");
	not_reached();
    }

    /* add field to found_common_json_fields list */
    field->next = found_common_json_fields;
    found_common_json_fields = field;

    dbg(DBG_HIGH, "added field '%s' value '%s' to found_common_json_fields", field->name, val);
    return field;
}

/*
 * get_common_json_field	-   check if name is common to both .info.json
 *				    and author.json and if it is add to common
 *				    list
 *
 * given:
 *
 *	program	- which util called this (jinfochk or jauthchk)
 *	file	- the file being parsed (path to)
 *	name	- the field name
 *	val	- the value of the field
 *
 * returns:
 *	1 ==> if the name is common to both files
 *	0 ==> if it's not one of the common field names
 *
 * NOTE: Does not return on error (NULL pointers).
 */
int
get_common_json_field(char const *program, char const *file, char *name, char *val)
{
    int ret = 1;	/* return value: 1 ==> known field, 0 ==> not a common field */
    struct json_field *field = NULL; /* the field in the common_json_fields table if found */
    size_t loc = 0; /* location in the common_json_fields table */

    /*
     * firewall
     */
    if (program == NULL || file == NULL || name == NULL || val == NULL) {
	err(225, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * check if the name is an expected common field
     */
    field = find_json_field_in_table(common_json_fields, name, &loc);
    if (field != NULL) {
	dbg(DBG_HIGH, "found common field '%s' value '%s'", field->name, val);
	add_found_common_json_field(field->name, val);
    } else {
	ret = 0;
    }
    return ret;
}

/*
 * check_found_common_json_fields - check that all the fields in the
 *				    found_common_json_fields table have valid
 *				    values
 *
 * given:
 *
 *	program	- which util called this (jinfochk or jauthchk)
 *	file	- the file being parsed (path to)
 *	fnamchk	- path to fnamchk util
 *
 * returns:
 *	>0 ==> the number of issues found
 *	0 ==> if no issues were found
 *
 * NOTE: Does not return on error (NULL pointers).
 */
int
check_found_common_json_fields(char const *program, char const *file, char const *fnamchk, bool test)
{
    int year = 0;	/* ioccc_year: IOCCC year as an integer */
    int entry_num = -1;	/* entry_num: entry number as an integer */
    int author_count = 0; /* author count */
    long ts = 0;	/* formed_timestamp_usec: microseconds as an integer */
    struct tm tm;	/* formed_timestamp: formatted as a time structure */
    int exit_code = 0;	/* tarball: exit code from fnamchk command */
    int issues = 0;	/* number of issues found */
    char *p;
    struct json_field *field; /* current field in found_common_json_fields list */
    struct json_value *value; /* current value in current field's values list */
    struct json_field *common_field = NULL; /* element in the common_json_fields table */
    size_t loc = 0;	/* location in the common_json_fields table */
    size_t val_length = 0; /* current value length */

    /*
     * firewall
     */
    if (program == NULL || file == NULL || fnamchk == NULL) {
	err(226, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * pre-clear time before strptime() use
     */
    memset(&tm, 0, sizeof tm);

    for (field = found_common_json_fields; field != NULL; field = field->next) {
	/*
	 * process a given common field
	 */

	/*
	 * first make sure the name != NULL and strlen() > 0
	 */
	if (field->name == NULL || !strlen(field->name)) {
	    err(227, __func__, "found NULL or empty field in found_common_json_fields list");
	    not_reached();
	}

	/* now make sure it's allowed to be in this table. */
	loc = 0;
	common_field = find_json_field_in_table(common_json_fields, field->name, &loc);

	/*
	 * If the field is not allowed in the list then it suggests there is a
	 * problem in the code because only common fields should be added to the
	 * list in the first place. Thus it's an error if a field that's in the
	 * common list is not a common field name.
	 */
	if (common_field == NULL) {
	    err(228, __func__, "illegal field name '%s' in found_common_json_fields list", field->name);
	    not_reached();
	}

	/*
	 * Next we test if the field has been seen more times than allowed. In
	 * the case of the common fields each is only allowed once but in
	 * uncommon fields some are allowed more than once.
	 */
	if (common_field->count > common_field->max_count) {
	    warn(__func__, "field '%s' found %lu times but is only allowed once", common_field->name, (unsigned long)common_field->count);
	    ++issues;
	}

	for (value = field->values; value != NULL; value = value->next) {
	    char *val = value->value;
	    val_length = strlen(val);

	    if (!val_length) {
		warn(__func__, "empty value found for field '%s' in file %s", field->name, file);
		/* don't increase issues because the below checks will do that
		 * too: this warning only notes the reason the test will fail.
		 */
	    }
	    /* first we do checks on the field type */
	    switch (common_field->field_type) {
		case JSON_BOOL:
		    if (strcmp(val, "false") && strcmp(val, "true")) {
			warn(__func__, "bool field '%s' has no boolean value '%s' in file %s", common_field->name, val, file);
			++issues;
			continue;
		    }
		    break;
		case JSON_ARRAY_BOOL:
		    break; /* arrays are not handled yet */
		case JSON_NUMBER:
		    if (!is_number(val)) {
			warn(__func__, "number field '%s' has non-number value '%s' in file %s", common_field->name, val, file);
			++issues;
			continue;
		    }
		    break;
		case JSON_ARRAY_NUMBER:
		    break; /* arrays are not handled yet */
		default:
		    break;
	    }

	    if (!strcmp(field->name, "ioccc_contest")) {
		if (strcmp(val, IOCCC_CONTEST)) {
		    warn(__func__, "ioccc_contest \"%s\" != \"%s\" in file %s", val, IOCCC_CONTEST, file);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "ioccc_year")) {
		year = string_to_int(val);
		if (year != IOCCC_YEAR) {
		    warn(__func__, "ioccc_year %d != IOCCC_YEAR %d", year, IOCCC_YEAR);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "mkiocccentry_version")) {
		if (!test && strcmp(val, MKIOCCCENTRY_VERSION)) {
		    warn(__func__, "mkiocccentry_version \"%s\" != MKIOCCCENTRY_VERSION \"%s\"", val, MKIOCCCENTRY_VERSION);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "iocccsize_version")) {
		if (!test && strcmp(val, IOCCCSIZE_VERSION)) {
		    warn(__func__, "iocccsize_version \"%s\" != IOCCCSIZE_VERSION \"%s\"", val, IOCCCSIZE_VERSION);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "IOCCC_contest_id")) {
		if (!valid_contest_id(val)) {
		    warn(__func__, "IOCCC_contest_id \"%s\" is invalid", val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "min_timestamp")) {
		ts = string_to_long(val);
		if (ts != MIN_TIMESTAMP) {
		    warn(__func__, "min_timestamp '%ld' != MIN_TIMESTAMP '%ld'", ts, MIN_TIMESTAMP);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "timestamp_epoch")) {
		if (strcmp(val, TIMESTAMP_EPOCH)) {
		    warn(__func__, "timestamp_epoch \"%s\" != TIMESTAMP_EPOCH \"%s\"", val, TIMESTAMP_EPOCH);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "formed_timestamp_usec")) {
		errno = 0;
		ts = string_to_long(val);
		if (ts < 0 || ts > 999999) {
		    warnp(__func__, "formed_timestamp_usec '%ld' out of range of >= 0 && <= 999999", ts);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "entry_num")) {
		entry_num = string_to_int(val);
		if (!(entry_num >= 0 && entry_num <= MAX_ENTRY_NUM)) {
		    warn(__func__, "entry number %d out of range", entry_num);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "author_count")) {
		author_count = string_to_int(val);
		if (!(author_count > 0 && author_count <= MAX_AUTHORS)) {
		    warn(__func__, "author count %d out of range of > 1 && <= %d", author_count, MAX_AUTHORS);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "formed_UTC")) {
		p = strptime(val, FORMED_UTC_FMT, &tm);
		if (p == NULL) {
		    warn(__func__, "formed_UTC \"%s\" does not match FORMED_UTC_FMT \"%s\"", val, FORMED_UTC_FMT);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "formed_timestamp")) {
		ts = string_to_long(val);
		if (ts < MIN_TIMESTAMP) {
		    warn(__func__, "formed_timestamp '%ld' < MIN_TIMESTAMP '%ld'", ts, MIN_TIMESTAMP);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "tarball")) {

		/*
		 * execute the fnamchk command
		 */
		exit_code = shell_cmd(__func__, true, "% % >/dev/null", fnamchk, val);
		if (exit_code != 0) {
		    warn(__func__, "%s: %s %s > /dev/null: failed with exit code: %d",
					program, fnamchk, val, WEXITSTATUS(exit_code));
		    ++issues;
		}

	    }
	}
    }

    /*
     * Now that we've checked each field by name, we still have to make sure
     * that each field expected is actually there. Note that in the above loop
     * we already tested if each field has been seen more times than allowed so
     * we don't do that here. This is because the fields that are in the list
     * are those that will potentially have more than allowed whereas here we're
     * making sure every field that is required is actually in the list.
     */
    for (loc = 0; common_json_fields[loc].name != NULL; ++loc) {
	if (!common_json_fields[loc].found) {
	    warn(__func__, "field '%s' not found in common_json_fields list", common_json_fields[loc].name);
	    ++issues;
	}
    }
    return issues;
}


/* new_json_field	- allocate a new json_field with the value passed in
 *
 * given:
 *
 *	name	    - the field name
 *	val	    - the field's value
 *
 * Returns the newly allocated struct json_field *.
 *
 * NOTE: This function does not return on NULL and it does not check any list:
 * as long as no NULL pointers are encountered it will return a newly allocated
 * struct json_field *. This means that it is the caller's responsibility to
 * check if the field is already in the list.
 */
struct json_field *
new_json_field(char const *name, char const *val)
{
    struct json_field *field; /* the new field */

    /*
     * firewall
     */
    if (name == NULL || val == NULL) {
	err(229, __func__, "passed NULL arg(s)");
	not_reached();
    }

    errno = 0;
    field = calloc(1, sizeof *field);
    if (field == NULL) {
	errp(230, __func__, "error allocating new struct json_field * for field '%s' and value '%s': %s", name, val, strerror(errno));
	not_reached();
    }

    errno = 0;
    field->name = strdup(name);
    if (field->name == NULL) {
	errp(231, __func__, "unable to strdup() field name '%s': %s", name, strerror(errno));
	not_reached();
    }

    if (add_json_value(field, val) == NULL) {
	err(232, __func__, "error adding value '%s' to field '%s'", val, name);
	not_reached();
    }

    field->count = 1;

    return field;
}

/* add_json_value	- add a value to a struct json_field *
 *
 * given:
 *
 *	field		- the field to add to
 *	val		- the value to add
 *
 * This function returns the newly allocated struct json_value * with the value
 * strdup()d and added to the struct json_field * values list.
 *
 * NOTE: This function does not return on error.
 *
 */
struct json_value *
add_json_value(struct json_field *field, char const *val)
{
    struct json_value *new_value = NULL;    /* the newly allocated value */
    struct json_value *value = NULL;	    /* the current list of values in field */

    /*
     * firewall
     */
    if (field == NULL || val == NULL) {
	err(233, __func__, "passed NULL arg(s)");
	not_reached();
    }

    errno = 0;
    new_value = calloc(1, sizeof *new_value);
    if (new_value == NULL) {
	errp(234, __func__, "error allocating new value '%s' for field '%s': %s", val, field->name, strerror(errno));
	not_reached();
    }
    errno = 0;
    new_value->value = strdup(val);
    if (new_value->value == NULL) {
	errp(235, __func__, "error strdup()ing value '%s' for field '%s': %s", val, field->name, strerror(errno));
	not_reached();
    }
    /* find end of list */
    for (value = field->values; value != NULL && value->next != NULL; value = value->next)
	; /* satisfy warnings */

    /* append new value to values list (field->values) */
    if (!value) {
	field->values = new_value;
    } else {
	value->next = new_value;
    }
    return new_value;
}


/* free_json_field_values	- free a field's values list
 *
 * given:
 *
 *	field			- field to free the values list
 *
 * Returns void.
 *
 * NOTE: This function does not return on error (NULL field passed in).
 */
void
free_json_field_values(struct json_field *field)
{
    struct json_value *value, *next_value; /* to free the values of the field */

    /*
     * firewall
     */
    if (field == NULL) {
	err(236, __func__, "passed NULL field");
	not_reached();
    }

    /* free each value */
    for (value = field->values; value != NULL; value = next_value) {
	next_value = value->next;

	if (value->value != NULL) {
	    free(value->value);
	    value->value = NULL;
	}
	value = NULL;
    }
}

/* free_found_common_json_fields  - free the common json fields list
 *
 * This function returns void.
 */
void
free_found_common_json_fields(void)
{
    struct json_field *field, *next_field;


    for (field = found_common_json_fields; field != NULL; field = next_field) {
	next_field = field->next;

	if (field->name) {
	    free(field->name);
	    field->name = NULL;
	}
	if (field->values) {
	    free_json_field_values(field);
	    field->values = NULL; /* redundant but for safety anyway */
	}
	free(field);
	field = NULL;
    }

    found_common_json_fields = NULL;
}

/* free_json_field	- release memory in a struct json_field *
 *
 * given:
 *
 *	field	    - the field to free
 *
 * NOTE: This function does not return on NULL.
 *
 * XXX It is the caller's responsibility to remove it from the list(s) it is in!
 *
 */
void
free_json_field(struct json_field *field)
{
    /*
     * firewall
     */
    if (field == NULL) {
	err(237, __func__, "passed NULL field");
	not_reached();
    }

    /* free the field's values list */
    free_json_field_values(field);
    field->values = NULL; /* redundant but for safety */

    if (field->name) {
	free(field->name);
	field->name = NULL;
    }

    free(field);
    field = NULL;
}

/*
 * free_info - free info and related sub-elements
 *
 * given:
 *      infop   - pointer to info structure to free
 *
 * This function does not return on error.
 */
void
free_info(struct info *infop)
{
    int i;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(238, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free version values
     */
    if (infop->common.mkiocccentry_ver != NULL) {
	free(infop->common.mkiocccentry_ver);
	infop->common.mkiocccentry_ver = NULL;
    }

    /*
     * free entry values
     */
    if (infop->common.ioccc_id != NULL) {
	free(infop->common.ioccc_id);
	infop->common.ioccc_id = NULL;
    }
    if (infop->title != NULL) {
	free(infop->title);
	infop->title = NULL;
    }
    if (infop->abstract != NULL) {
	free(infop->abstract);
	infop->abstract = NULL;
    }

    /*
     * free filenames
     */
    if (infop->prog_c != NULL) {
	free(infop->prog_c);
	infop->prog_c = NULL;
    }
    if (infop->Makefile != NULL) {
	free(infop->Makefile);
	infop->Makefile = NULL;
    }
    if (infop->remarks_md != NULL) {
	free(infop->remarks_md);
	infop->remarks_md = NULL;
    }
    if (infop->extra_file != NULL) {
	for (i = 0; i < infop->extra_count; ++i) {
	    if (infop->extra_file[i] != NULL) {
		free(infop->extra_file[i]);
		infop->extra_file[i] = NULL;
	    }
	}
	free(infop->extra_file);
	infop->extra_file = NULL;
    }

    if (infop->common.tarball != NULL) {
	free(infop->common.tarball);
	infop->common.tarball = NULL;
    }

    /*
     * free time values
     */
    if (infop->common.epoch != NULL) {
	free(infop->common.epoch);
	infop->common.epoch = NULL;
    }
    if (infop->common.utctime != NULL) {
	free(infop->common.utctime);
	infop->common.utctime = NULL;
    }
    memset(infop, 0, sizeof *infop);

    return;
}

/*
 * free_author_array - free storage related to a struct author
 *
 * given:
 *      author_set      - pointer to a struct author array
 *      author_count    - length of author array
 */
void
free_author_array(struct author *author_set, int author_count)
{
    int i;

    /*
     * firewall
     */
    if (author_set == NULL) {
	err(239, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(240, __func__, "author_count: %d < 0", author_count);
	not_reached();
    }

    /*
     * free elements of each array member
     */
    for (i = 0; i < author_count; ++i) {
	if (author_set[i].name != NULL) {
	    free(author_set[i].name);
	    author_set[i].name = NULL;
	}
	if (author_set[i].location_code != NULL) {
	    free(author_set[i].location_code);
	    author_set[i].location_code = NULL;
	}
	if (author_set[i].email != NULL) {
	    free(author_set[i].email);
	    author_set[i].email = NULL;
	}
	if (author_set[i].url != NULL) {
	    free(author_set[i].url);
	    author_set[i].url = NULL;
	}
	if (author_set[i].twitter != NULL) {
	    free(author_set[i].twitter);
	    author_set[i].twitter = NULL;
	}
	if (author_set[i].github != NULL) {
	    free(author_set[i].github);
	    author_set[i].github = NULL;
	}
	if (author_set[i].affiliation != NULL) {
	    free(author_set[i].affiliation);
	    author_set[i].affiliation = NULL;
	}
    }

    memset(author_set, 0, sizeof *author_set);
    return;
}
