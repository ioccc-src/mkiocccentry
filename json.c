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

#define JSON_C

/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>


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
	strncpy(p, jenc[(uint8_t)(ptr[i])].enc, jenc[(uint8_t)(ptr[i])].len);
	p += jenc[(uint8_t)(ptr[i])].len;
    }

    /*
     * return result
     */
    dbg(DBG_VVVHIGH, "returning from malloc_json_encode(ptr, %lu, *%lu)", len, mlen);
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
	err(202, __func__, "malloc_json_encode(0x00, 1, *mlen: %lu) == NULL", mlen);
	not_reached();
    }
    if (mlen != jenc[0].len) {
	err(203, __func__, "malloc_json_encode(0x00, 1, *mlen: %lu != %lu)",
			   mlen, (unsigned long)(jenc[0].len));
	not_reached();
    }
    if (strcmp(jenc[0].enc, mstr) != 0) {
	err(204, __func__, "malloc_json_encode(0x00, 1, *mlen: %lu) != <%s>",
			   mlen, jenc[0].enc);
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
	    err(205, __func__, "malloc_json_encode_str(0x%02x, *mlen: %lu) == NULL", i, mlen);
	    not_reached();
	}
	if (mlen != jenc[i].len) {
	    err(206, __func__, "malloc_json_encode_str(0x%02x, *mlen %lu != %lu)",
			       i, mlen, (unsigned long)jenc[i].len);
	    not_reached();
	}
	if (strcmp(jenc[i].enc, mstr) != 0) {
	    err(207, __func__, "malloc_json_encode_str(0x%02x, *mlen: %lu) != <%s>", i, mlen, jenc[i].enc);
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
	    err(208, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, true) == NULL", mstr, mlen2);
	    not_reached();
	}
	if (mlen2 != 1) {
	    err(209, __func__, "malloc_json_decode_str(<%s>, *mlen2 %lu != 1, true)", mstr, mlen2);
	    not_reached();
	}
	if ((uint8_t)(mstr2[0]) != i) {
	    err(210, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, true): 0x%02x != 0x%02x",
			       mstr, mlen2, (uint8_t)(mstr2[0]), i);
	    not_reached();
	}

	/*
	 * test non-strict decoding the JSON encoded string
	 */
	dbg(DBG_VVVHIGH, "testing malloc_json_decode_str(<%s>, *mlen, false)", mstr);
	/* test malloc_json_decode_str() */
	mstr3 = malloc_json_decode_str(mstr, &mlen3, false);
	if (mstr3 == NULL) {
	    err(211, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, false) == NULL", mstr, mlen3);
	    not_reached();
	}
	if (mlen3 != 1) {
	    err(212, __func__, "malloc_json_decode_str(<%s>, *mlen3 %lu != 1, false)", mstr, mlen3);
	    not_reached();
	}
	if ((uint8_t)(mstr3[0]) != i) {
	    err(213, __func__, "malloc_json_decode_str(<%s>, *mlen: %lu, false): 0x%02x != 0x%02x",
			       mstr, mlen3, (uint8_t)(mstr3[0]), i);
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
		if ((c >= 0x00 && c <= 0x1f) || c >= 0x7f) {
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
    dbg(DBG_VVVHIGH, "returning from malloc_json_decode(ptr, %lu, *%lu)", len, mlen);
    if (retlen != NULL) {
	*retlen = mlen;
    }
    return ret;
}


/*
 * malloc_json_decode_str - return a JSON decoding of a string
 *
 * This is an simplified interface for malloc_json_decode().
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

/* check_first_json_char - check if first char is '{'
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
	err(214, __func__, "passed NULL or zero length data");
	not_reached();
    } else if (file == NULL || first == NULL) {
	err(215, __func__, "passed NULL arg(s)");
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

/* check_last_json_char - check if last char is '}'
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
	err(216, __func__, "passed NULL or zero length data");
	not_reached();
    } else if (file == NULL || last == NULL) {
	err(217, __func__, "passed NULL arg(s)");
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
/* json_filename    - return ".info.json", ".author.json" or "null" depending on type
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
    switch (type) {
	case INFO_JSON:
	    return ".info.json";
	    break; /* in case the return is ever removed */
	case AUTHOR_JSON:
	    return ".author.json";
	    break; /* in case the return is ever removed */
	default:
	    return "null";
	    break; /* in case the return is ever removed */
    }
}

/* check_common_json_fields	-   check if field is common to both .info.json
 *				    and author.json and check the value if it is
 *
 * given:
 *
 *	file	- the file being parsed (path to)
 *	field	- the field name
 *	value	- the value of the field
 *
 * Returns 1 if the field was parsed (that is the field is one of the common
 * fields to both files and it has a valid value); if it's not one of the common
 * fields return 0.
 *
 * Does not return on error (NULL pointers).
 */
int check_common_json_fields(char const *file, char const *field, char const *value)
{
    int ret = 1;
    int year = 0;
    int entry_num = -1;
    long ts = 0;

    /*
     * firewall
     */
    if (file == NULL || field == NULL || value == NULL) {
	err(218, __func__, "passed NULL arg(s)");
	not_reached();
    }

    if (!strcmp(field, "IOCCC_info_version")) {
	if (strcmp(value, INFO_VERSION)) {
	    err(219, __func__, "IOCCC_info_version \"%s\" != \"%s\" in file %s", value, INFO_VERSION, file);
	    not_reached();
	}
    } else if (!strcmp(field, "ioccc_contest")) {
	if (strcmp(value, IOCCC_CONTEST)) {
	    err(220, __func__, "ioccc_contest \"%s\" != \"%s\" in file %s", value, IOCCC_CONTEST, file);
	    not_reached();
	}
    } else if (!strcmp(field, "ioccc_year")) {
	errno = 0;
	year = (int)strtol(value, NULL, 10);
	if (errno != 0) {
	    err(221, __func__, "parsing ioccc_year \"%s\" in file %s", value, file);
	    not_reached();
	} else if (year != IOCCC_YEAR) {
	    err(222, __func__, "ioccc_year %d != IOCCC_YEAR %d", year, IOCCC_YEAR);
	    not_reached();
	}
    } else if (!strcmp(field, "mkiocccentry_version")) {
	if (strcmp(value, MKIOCCCENTRY_VERSION)) {
	    err(223, __func__, "mkiocccentry_version \"%s\" != MKIOCCCENTRY_VERSION \"%s\"", value, MKIOCCCENTRY_VERSION);
	    not_reached();
	}
    } else if (!strcmp(field, "iocccsize_version")) {
	if (strcmp(value, IOCCCSIZE_VERSION)) {
	    err(224, __func__, "iocccsize_version \"%s\" != IOCCCSIZE_VERSION \"%s\"", value, IOCCCSIZE_VERSION);
	    not_reached();
	}
    } else if (!strcmp(field, "IOCCC_contest_id")) {
	/* TODO add handling of IOCCC_contest_id field */
    } else if (!strcmp(field, "min_timestamp")) {
	errno = 0;
	ts = strtol(value, NULL, 10);
	if (errno != 0) {
	    err(225, __func__, "unable to parse min_timestamp \"%s\"", value);
	    not_reached();
	} else if (ts != MIN_TIMESTAMP) {
	    err(226, __func__, "min_timestamp '%ld' != MIN_TIMESTAMP '%ld'", ts, MIN_TIMESTAMP);
	    not_reached();
	}
    } else if (!strcmp(field, "timestamp_epoch")) {
	if (strcmp(value, TIMESTAMP_EPOCH)) {
	    err(227, __func__, "timestamp_epoch \"%s\" != TIMESTAMP_EPOCH \"%s\"", value, TIMESTAMP_EPOCH);
	    not_reached();
	}
    } else if (!strcmp(field, "formed_timestamp_usec")) {
	errno = 0;
	ts = strtol(value, NULL, 10);
	if (errno != 0) {
	    err(228, __func__, "unable to parse min_timestamp \"%s\"", value);
	    not_reached();
	} else if (ts < 0 || ts > 999999) {
	    err(229, __func__, "formed_timestamp_usec '%ld' out of range of >= 0 && <= 999999", ts);
	    not_reached();
	}
    } else if (!strcmp(field, "entry_num")) {
	errno = 0;
	entry_num = (int)strtol(value, NULL, 10);
	if (errno != 0) {
	    err(230, __func__, "parsing entry_num \"%s\" in file %s", value, file);
	    not_reached();
	} else if (!(entry_num >= 0 && entry_num <= MAX_ENTRY_NUM)) {
	    err(231, __func__, "entry number %d out of range", entry_num);
	    not_reached();
	}
    } else {
	ret = 0;
    }

    return ret;
}
