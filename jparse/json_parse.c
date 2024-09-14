/*
 * json_parse - JSON parser support code
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
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>

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
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"


/*
 * JSON encoding of an octet in a JSON string
 *
 * NOTE: JSON_BYTE_VALUES is #defined as (BYTE_VALUES) (see util.h) and this table
 * MUST be 256 long.
 */
struct encode jenc[JSON_BYTE_VALUES] = {

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
    {'$', 1, "$"}, {'%', 1, "%"}, {'&', 1, "&"}, {'\'', 1, "'"},
    {'(', 1, "("}, {')', 1, ")"}, {'*', 1, "*"}, {'+', 1, "+"},
    {',', 1, ","}, {'-', 1, "-"}, {'.', 1, "."}, {'/', 1, "/"},

    /* \x30 - \x3f */
    {'0', 1, "0"}, {'1', 1, "1"}, {'2', 1, "2"}, {'3', 1, "3"},
    {'4', 1, "4"}, {'5', 1, "5"}, {'6', 1, "6"}, {'7', 1, "7"},
    {'8', 1, "8"}, {'9', 1, "9"}, {':', 1, ":"}, {';', 1, ";"},
    {'<', 1, "<"}, {'=', 1, "="}, {'>', 1, ">"}, {'?', 1, "?"},

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
    {0x80, 1, "\x80"}, {0x81, 1, "\x81"}, {0x82, 1, "\x82"}, {0x83, 1, "\x83"},
    {0x84, 1, "\x84"}, {0x85, 1, "\x85"}, {0x86, 1, "\x86"}, {0x87, 1, "\x87"},
    {0x88, 1, "\x88"}, {0x89, 1, "\x89"}, {0x8a, 1, "\x8a"}, {0x8b, 1, "\x8b"},
    {0x8c, 1, "\x8c"}, {0x8d, 1, "\x8d"}, {0x8e, 1, "\x8e"}, {0x8f, 1, "\x8f"},

    /* \x90 - \x9f */
    {0x90, 1, "\x90"}, {0x91, 1, "\x91"}, {0x92, 1, "\x92"}, {0x93, 1, "\x93"},
    {0x94, 1, "\x94"}, {0x95, 1, "\x95"}, {0x96, 1, "\x96"}, {0x97, 1, "\x97"},
    {0x98, 1, "\x98"}, {0x99, 1, "\x99"}, {0x9a, 1, "\x9a"}, {0x9b, 1, "\x9b"},
    {0x9c, 1, "\x9c"}, {0x9d, 1, "\x9d"}, {0x9e, 1, "\x9e"}, {0x9f, 1, "\x9f"},

    /* \xa0 - \xaf */
    {0xa0, 1, "\xa0"}, {0xa1, 1, "\xa1"}, {0xa2, 1, "\xa2"}, {0xa3, 1, "\xa3"},
    {0xa4, 1, "\xa4"}, {0xa5, 1, "\xa5"}, {0xa6, 1, "\xa6"}, {0xa7, 1, "\xa7"},
    {0xa8, 1, "\xa8"}, {0xa9, 1, "\xa9"}, {0xaa, 1, "\xaa"}, {0xab, 1, "\xab"},
    {0xac, 1, "\xac"}, {0xad, 1, "\xad"}, {0xae, 1, "\xae"}, {0xaf, 1, "\xaf"},

    /* \xb0 - \xbf */
    {0xb0, 1, "\xb0"}, {0xb1, 1, "\xb1"}, {0xb2, 1, "\xb2"}, {0xb3, 1, "\xb3"},
    {0xb4, 1, "\xb4"}, {0xb5, 1, "\xb5"}, {0xb6, 1, "\xb6"}, {0xb7, 1, "\xb7"},
    {0xb8, 1, "\xb8"}, {0xb9, 1, "\xb9"}, {0xba, 1, "\xba"}, {0xbb, 1, "\xbb"},
    {0xbc, 1, "\xbc"}, {0xbd, 1, "\xbd"}, {0xbe, 1, "\xbe"}, {0xbf, 1, "\xbf"},

    /* \xc0 - \xcf */
    {0xc0, 1, "\xc0"}, {0xc1, 1, "\xc1"}, {0xc2, 1, "\xc2"}, {0xc3, 1, "\xc3"},
    {0xc4, 1, "\xc4"}, {0xc5, 1, "\xc5"}, {0xc6, 1, "\xc6"}, {0xc7, 1, "\xc7"},
    {0xc8, 1, "\xc8"}, {0xc9, 1, "\xc9"}, {0xca, 1, "\xca"}, {0xcb, 1, "\xcb"},
    {0xcc, 1, "\xcc"}, {0xcd, 1, "\xcd"}, {0xce, 1, "\xce"}, {0xcf, 1, "\xcf"},

    /* \xd0 - \xdf */
    {0xd0, 1, "\xd0"}, {0xd1, 1, "\xd1"}, {0xd2, 1, "\xd2"}, {0xd3, 1, "\xd3"},
    {0xd4, 1, "\xd4"}, {0xd5, 1, "\xd5"}, {0xd6, 1, "\xd6"}, {0xd7, 1, "\xd7"},
    {0xd8, 1, "\xd8"}, {0xd9, 1, "\xd9"}, {0xda, 1, "\xda"}, {0xdb, 1, "\xdb"},
    {0xdc, 1, "\xdc"}, {0xdd, 1, "\xdd"}, {0xde, 1, "\xde"}, {0xdf, 1, "\xdf"},

    /* \xe0 - \xef */
    {0xe0, 1, "\xe0"}, {0xe1, 1, "\xe1"}, {0xe2, 1, "\xe2"}, {0xe3, 1, "\xe3"},
    {0xe4, 1, "\xe4"}, {0xe5, 1, "\xe5"}, {0xe6, 1, "\xe6"}, {0xe7, 1, "\xe7"},
    {0xe8, 1, "\xe8"}, {0xe9, 1, "\xe9"}, {0xea, 1, "\xea"}, {0xeb, 1, "\xeb"},
    {0xec, 1, "\xec"}, {0xed, 1, "\xed"}, {0xee, 1, "\xee"}, {0xef, 1, "\xef"},

    /* \xf0 - \xff */
    {0xf0, 1, "\xf0"}, {0xf1, 1, "\xf1"}, {0xf2, 1, "\xf2"}, {0xf3, 1, "\xf3"},
    {0xf4, 1, "\xf4"}, {0xf5, 1, "\xf5"}, {0xf6, 1, "\xf6"}, {0xf7, 1, "\xf7"},
    {0xf8, 1, "\xf8"}, {0xf9, 1, "\xf9"}, {0xfa, 1, "\xfa"}, {0xfb, 1, "\xfb"},
    {0xfc, 1, "\xfc"}, {0xfd, 1, "\xfd"}, {0xfe, 1, "\xfe"}, {0xff, 1, "\xff"}
};


/* for json number strings */
static bool json_process_decimal(struct json_number *item, char const *str, size_t len);
static bool json_process_floating(struct json_number *item, char const *str, size_t len);


/*
 * json_encode - return a JSON encoding of a block of memory
 *
 * JSON string encoding:
 *
 * These escape characters are required by JSON:
 *
 *         old			      new
 *      ---------------------------------------
 *	\x00-\x07		\u0000 - \u0007
 *	<backspace>		\b	(\x08)
 *	<horizontal_tab>	\t	(\x09)
 *	<newline>		\n	(\x0a)
 *	\x0b			\u000b <vertical_tab>
 *	<form_feed>		\f	(\x0c)
 *	<enter>			\r	(\x0d)
 *	\x0e-\x1f		\u000e - \x001f
 *	<double_quote>		\"	(\x22)
 *	\			\\	(\x5c)
 *
 * given:
 *	ptr		start of memory block to encode
 *	len		length of block to encode in bytes
 *	retlen		address of where to store allocated length,
 *			    if retlen != NULL
 *	skip_quote	true ==> ignore any double quotes if they are both
 *				 at the start and end of the memory block
 *			false ==> process all bytes in the block
 *
 * returns:
 *	allocated JSON encoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
json_encode(char const *ptr, size_t len, size_t *retlen, bool skip_quote)
{
    char *ret = NULL;	    /* allocated encoding string or NULL */
    char *beyond = NULL;    /* beyond the end of the allocated encoding string */
    ssize_t mlen = 0;	    /* length of allocated encoded string */
    char *p;		    /* next place to encode */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	/* error - clear allocated length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }

    /*
     * count the bytes that will be in the encoded allocated string
     */
    for (i=0; i < len; ++i) {
	mlen += jenc[(uint8_t)(ptr[i])].len;
    }
    if (mlen < 0) { /* paranoia */
	/* error - clear allocated length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "mlen #0: %ju < 0", (uintmax_t)mlen);
	return NULL;
    }

    /*
     * malloc the encoded string
     */
    ret = malloc((size_t)mlen + 1 + 1);
    if (ret == NULL) {
	/* error - clear allocated length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "malloc of %ju bytes failed", (uintmax_t)(mlen + 1 + 1));
	return NULL;
    }
    ret[mlen] = '\0';   /* terminate string */
    ret[mlen + 1] = '\0';   /* paranoia */
    beyond = &(ret[mlen]);

    /*
     * skip any enclosing quotes if requested
     *
     * We only skip enclosing quotes if skip_quote is true, the memory block is
     * long enough to contain two '"'s and the memory block starts and ends with
     * a '"'.
     */
    if (skip_quote == true && len > 1 && ptr[0] == '"' && ptr[len-1] == '"') {
	i = 1;	/* start encoding on the next byte beyond the " */
	--len;	/* do not encode the last byte */
    } else {
	i = 0;	/* start encoding at the first byte */
    }

    /*
     * JSON encode each byte
     */
    for (p=ret; i < len; ++i) {
	if (p+jenc[(uint8_t)(ptr[i])].len > beyond) {
	    /* error - clear allocated length */
	    if (retlen != NULL) {
		*retlen = 0;
	    }
	    if (ret != NULL) {
		free(ret);
		ret = NULL;
	    }
	    warn(__func__, "encoding ran beyond end of allocated encoded string");
	    return NULL;
	}
	strcpy(p, jenc[(uint8_t)(ptr[i])].enc);
	p += jenc[(uint8_t)(ptr[i])].len;
    }
    *p = '\0';	/* paranoia */
    mlen = p - ret; /* paranoia */
    if (mlen < 0) { /* paranoia */
	warn(__func__, "mlen #1: %ju < 0", (uintmax_t)mlen);
	if (ret != NULL) {
	    free(ret);
	    ret = NULL;
	}
	return NULL;
    }

    /*
     * return result
     */
    dbg(DBG_VVVHIGH, "returning from json_encode(ptr, %ju, *%ju, %s)",
		     (uintmax_t)len, (uintmax_t)mlen, booltostr(skip_quote));
    if (retlen != NULL) {
	*retlen = (size_t)mlen;
    }
    return ret;
}


/*
 * json_encode_str - return a JSON encoding of a string
 *
 * This is a simplified interface for json_encode().
 *
 * given:
 *	str		NUL terminated C-style string to encode
 *	retlen		address of where to store allocated length, if retlen != NULL
 *	skip_quotes	true ==> ignore any double quotes if they are both
 *				 at the start and end of the memory block
 *			false ==> process all bytes in the block
 *
 * returns:
 *	allocated JSON encoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
json_encode_str(char const *str, size_t *retlen, bool skip_quote)
{
    void *ret = NULL;	    /* allocated encoding string or NULL */
    size_t len = 0;	    /* length of string to encode */

    /*
     * firewall
     */
    if (str == NULL) {
	/* error - clear allocated length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    len = strlen(str);

    /*
     * convert to json_encode() call
     */
    ret = json_encode(str, len, retlen, skip_quote);
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
    unsigned int int_hexval;/* hex value from xxxx part of \uxxxx */
    char guard;		/* scanf guard to catch excess amount of input */
    int indx;		/* test index */
    char const *encstr;	/* encoding string */
    int ret;		/* libc function return value */
    char str[2];	/* character string to encode */
    char *mstr = NULL;	/* allocated encoding string */
    size_t mlen = 0;	/* length of allocated encoding string */
    char *mstr2 = NULL;	/* allocated decoding string */
    size_t mlen2 = 0;	/* length of allocated decoding string */
    unsigned int i;

    /*
     * assert: bits in byte must be 8
     */
    if (BITS_IN_BYTE != 8) {
	err(100, __func__, "BITS_IN_BYTE: %d != 8", BITS_IN_BYTE);
	not_reached();
    }

    /*
     * assert: JSON_BYTE_VALUES must be 256
     */
    if (JSON_BYTE_VALUES != 256) {
	err(101, __func__, "JSON_BYTE_VALUES: %d != 256", JSON_BYTE_VALUES);
	not_reached();
    }

    /*
     * assert: table must be of size 256
     */
    if (TBLLEN(jenc) != JSON_BYTE_VALUES) {
	err(102, __func__, "jenc table length is %ju instead of %d",
			   (uintmax_t)TBLLEN(jenc), JSON_BYTE_VALUES);
	not_reached();
    }

    /*
     * assert: byte must be an index from 0 to 256
     */
    for (i=0; i < JSON_BYTE_VALUES; ++i) {
	if (jenc[i].byte != i) {
	    err(103, __func__, "jenc[0x%02x].byte: %d != %d", i, jenc[i].byte, i);
	    not_reached();
	}
    }

    /*
     * assert: enc string must be non-NULL
     */
    for (i=0; i < JSON_BYTE_VALUES; ++i) {
	if (jenc[i].enc == NULL) {
	    err(104, __func__, "jenc[0x%02x].enc == NULL", i);
	    not_reached();
	}
    }

    /*
     * assert: length of enc string must match len
     */
    for (i=0; i < JSON_BYTE_VALUES; ++i) {
	if (strlen(jenc[i].enc) != jenc[i].len) {
	    err(105, __func__, "jenc[0x%02x].enc length: %ju != jenc[0x%02x].len: %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       i, (uintmax_t)jenc[i].len);
	    not_reached();
	}
    }

    /*
     * assert: \x00-\x07 encodes to \uxxxx
     */
    for (i=0x00; i <= 0x07; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(106, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       (uintmax_t)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &int_hexval, &guard);
	if (ret != 1) {
	    err(107, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != int_hexval) {
	    err(108, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * assert: \x08 encodes to \b
     */
    indx = 0x08;
    encstr = "\\b";
    if (jenc[indx].len != LITLEN("\\b")) {
	err(109, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(110, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x09 encodes to \t
     */
    indx = 0x09;
    encstr = "\\t";
    if (jenc[indx].len != LITLEN("\\b")) {
	err(111, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(112, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0a encodes to \n
     */
    indx = 0x0a;
    encstr = "\\n";
    if (jenc[indx].len != strlen(encstr)) {
	err(113, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(114, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0b encodes to \u000b
     */
    indx = 0x0b;
    encstr = "\\u000b";
    if (jenc[indx].len != strlen(encstr)) {
	err(115, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(116, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0c encodes to \f
     */
    indx = 0x0c;
    encstr = "\\f";
    if (jenc[indx].len != strlen(encstr)) {
	err(117, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(118, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0d encodes to \r
     */
    indx = 0x0d;
    encstr = "\\r";
    if (jenc[indx].len != strlen(encstr)) {
	err(119, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(120, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0e-\x1f encodes to \uxxxx
     */
    for (i=0x0e; i <= 0x1f; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(121, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       (uintmax_t)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &int_hexval, &guard);
	if (ret != 1) {
	    err(122, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != int_hexval) {
	    err(123, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * assert: \x20-\x21 encodes to the character
     */
    for (i=0x20; i <= 0x21; ++i) {
	if (jenc[i].len != 1) {
	    err(124, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(125, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x22 encodes to \"
     */
    indx = 0x22;
    encstr = "\\\"";
    if (jenc[indx].len != strlen(encstr)) {
	err(126, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(128, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x23-\x5b encodes to the character
     */
    for (i=0x23; i <= 0x5b; ++i) {
	if (jenc[i].len != 1) {
	    err(129, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(130, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x5c encodes to \\
     */
    indx = 0x5c;
    encstr = "\\\\";
    if (jenc[indx].len != strlen(encstr)) {
	err(131, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(132, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x5d-\x7e encodes to the character
     */
    for (i=0x5d; i <= 0x7e; ++i) {
	if (jenc[i].len != 1) {
	    err(133, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(134, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x7f encodes to \u007f
     */
    for (i=0x7f; i <= 0x7f; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(135, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       (uintmax_t)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &int_hexval, &guard);
	if (ret != 1) {
	    err(136, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != int_hexval) {
	    err(137, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * assert: \x80-\xff encodes to the character
     */
    for (i=0x80; i <= 0xff; ++i) {
	if (jenc[i].len != 1) {
	    err(138, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((uint8_t)(jenc[i].enc[0]) != i) {
	    err(139, __func__, "jenc[0x%02x].enc[0]: 0x%02x is not 0x%02jx",
			       i, (uint8_t)(jenc[i].enc[0]) & 0xff, (uintmax_t)i);
	    not_reached();
	}
	if ((uint8_t)(jenc[i].enc[1]) != 0) {
	    err(140, __func__, "jenc[0x%02x].enc[1]: 0x%02x is not 0",
			       i, (uint8_t)(jenc[i].enc[1]) & 0xff);
	    not_reached();
	}
    }

    /*
     * special test for encoding a NUL byte
     */
    dbg(DBG_VVVHIGH, "testing json_encode(0x00, 1, *mlen)");
    memset(str, 0, sizeof(str));    /* clear all bytes in str, including the final '\0' */
    mstr = json_encode(str, 1,  &mlen, false);
    if (mstr == NULL) {
	err(141, __func__, "json_encode(0x00, 1, *mlen: %ju) == NULL", (uintmax_t)mlen);
	not_reached();
    }
    if (mlen != jenc[0].len) {
	err(142, __func__, "json_encode(0x00, 1, *mlen: %ju != %ju)",
			   (uintmax_t)mlen, (uintmax_t)(jenc[0].len));
	not_reached();
    }
    if (strcmp(jenc[0].enc, mstr) != 0) {
	err(143, __func__, "json_encode(0x00, 1, *mlen: %ju) != <%s>",
			   (uintmax_t)mlen, jenc[0].enc);
	not_reached();
    }
    /* free the allocated encoded string */
    if (mstr != NULL) {
	free(mstr);
	mstr = NULL;
    }

    /*
     * finally try to encode every possible string with a single non-NUL character
     */
    for (i=1; i < JSON_BYTE_VALUES; ++i) {

	/*
	 * test JSON encoding
	 */
	dbg(DBG_VVVHIGH, "testing json_encode_str(0x%02x, *mlen)", i);
	/* load input string */
	str[0] = (char)i;
	/* test json_encode_str() */
	mstr = json_encode_str(str, &mlen, false);
	/* check encoding result */
	if (mstr == NULL) {
	    err(144, __func__, "json_encode_str(0x%02x, *mlen: %ju) == NULL",
			       i, (uintmax_t)mlen);
	    not_reached();
	}
	if (mlen != jenc[i].len) {
	    err(145, __func__, "json_encode_str(0x%02x, *mlen %ju != %ju)",
			       i, (uintmax_t)mlen, (uintmax_t)jenc[i].len);
	    not_reached();
	}
	if (strcmp(jenc[i].enc, mstr) != 0) {
	    err(146, __func__, "json_encode_str(0x%02x, *mlen: %ju) != <%s>", i,
			       (uintmax_t)mlen, jenc[i].enc);
	    not_reached();
	}
	dbg(DBG_VVHIGH, "testing json_encode_str(0x%02x, *mlen) encoded to <%s>", i, mstr);

	/*
	 * test decoding the JSON encoded string
	 */
	dbg(DBG_VVVHIGH, "testing json_decode_str(<%s>, *mlen, true)", mstr);
	/* test json_decode_str() */
	mstr2 = json_decode_str(mstr, &mlen2);
	if (mstr2 == NULL) {
	    err(147, __func__, "json_decode_str(<%s>, *mlen2: %ju, true) == NULL",
			       mstr, (uintmax_t)mlen2);
	    not_reached();
	}
	if (mlen2 != 1) {
	    err(148, __func__, "json_decode_str(<%s>, *mlen2 %ju != 1, true)",
			       mstr, (uintmax_t)mlen2);
	    not_reached();
	}
	if ((uint8_t)(mstr2[0]) != i) {
	    err(149, __func__, "json_decode_str(<%s>, *mlen2: %ju, true): 0x%02x != 0x%02x",
			       mstr, (uintmax_t)mlen2, (uint8_t)(mstr2[0]) & 0xff, i);
	    not_reached();
	}

	/* free the allocated encoded string */
	if (mstr != NULL) {
	    free(mstr);
	    mstr = NULL;
	}
	if (mstr2 != NULL) {
	    free(mstr2);
	    mstr2 = NULL;
	}
    }

    /*
     * all seems well win the jenc[] table
     */
    dbg(DBG_VVHIGH, "jenc[] table passes");
    return;
}


/*
 * json_decode - return the decoding of a JSON encoded block of memory
 *
 * given:
 *	ptr	start of memory block to decode
 *	len	length of block to decode in bytes
 *	retlen	address of where to store allocated length, if retlen != NULL
 *	has_nul	if != NULL and we find an encoded NUL byte we will do *has_nul = true
 *
 * returns:
 *	allocated JSON decoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
json_decode(char const *ptr, size_t len, size_t *retlen, bool *has_nul)
{
    char *ret = NULL;	    /* allocated encoding string or NULL */
    char *beyond = NULL;    /* beyond the end of the allocated encoding string */
    size_t mlen = 0;	    /* length of allocated encoded string */
    char *p = NULL;	    /* next place to encode */
    char n = 0;		    /* next character beyond a \\ */
    char a = 0;		    /* first hex character after \u */
    int xa = 0;		    /* first hex character numeric value */
    char b = 0;		    /* second hex character after \u */
    int xb = 0;		    /* second hex character numeric value */
    char c = 0;		    /* character to decode or third hex character after \u */
    int xc = 0;		    /* 3nd hex character numeric value */
    char d = 0;		    /* fourth hex character after \u */
    int xd = 0;		    /* fourth hex character numeric value */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	/* error - clear allocated length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }

    /*
     * count the bytes that will be in the decoded allocated string
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
	     * disallow characters that should have been escaped
	     */
	    switch (c) {
	    case '\b':  /*fallthrough*/
	    case '\t':  /*fallthrough*/
	    case '\n':  /*fallthrough*/
	    case '\f':  /*fallthrough*/
	    case '\r':
		/* error - clear allocated length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found non-\\-escaped char: 0x%02x", (uint8_t)c);
		return NULL;
		break;
	    case '"':   /*fallthrough*/
	    case '\\':
		/* error - clear allocated length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found \\-escaped char: %c", c);
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
	 * valid \-escaped characters count as 2 or 6
	 */
	} else {

	    /*
	     * there must be at least one more character beyond \
	     */
	    if (i+1 >= len) {
		/* error - clear allocated length */
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
	    case 'b':	/*fallthrough*/
	    case 't':	/*fallthrough*/
	    case 'n':	/*fallthrough*/
	    case 'f':	/*fallthrough*/
	    case 'r':	/*fallthrough*/
	    case '"':	/*fallthrough*/
	    case '/':	/*fallthrough*/
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
		 * there must be at least five more characters beyond \
		 */
		if (i+5 >= len) {
		    /* error - clear allocated length */
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
		     * case: \u00xx is 1 character
		     */
		    if (a == '0' && b == '0') {
			++mlen;
			i += 5;

			if (c == '0' && d == '0' && has_nul != NULL) {
			    *has_nul = true; /* set has_nul to true */
			}
		    /*
		     * case: count \uxxxx as 2 characters
		     */
		    } else {
			mlen += 2;
			i += 5;
		    }
		    break;

		/*
		 * case: \uxxxx is invalid because xxxx is not HEX
		 */
		} else {
			warn(__func__, "\\u, not followed by 4 hex chars");
			return NULL;
		}
		break;

	    /*
	     * found invalid JSON \-escape character
	     */
	    default:
		/* error - clear allocated length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found invalid JSON \\-escape: followed by 0x%02x", (uint8_t)c);
		return NULL;
	    }
	}
    }

    /*
     * allocated decoded string
     */
    ret = malloc(mlen + 1 + 1);
    if (ret == NULL) {
	/* error - clear allocated length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "malloc of %ju bytes failed", (uintmax_t)(mlen + 1 + 1));
	return NULL;
    }
    ret[mlen] = '\0';   /* terminate string */
    ret[mlen + 1] = '\0';   /* paranoia */
    beyond = &(ret[mlen]);

    /*
     * JSON string decode
     *
     * In the above counting code, prior to the malloc for the decoded string,
     * we already determined that the JSON encoded block of memory is valid.
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
	    /* error - clear allocated length */
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
		*p++ = 0x0b;  /* not all C compilers understand \e */
		break;
	    case '"':	/*fallthrough*/
	    case '/':	/*fallthrough*/
	    case '\\':
		++i;
		*p++ = n;	/* escape decodes to itself */
		break;

	    /*
	     * decode \uxxxx
	     */
	    case 'u':

		/*
		 * there must be at least five more characters beyond \
		 */
		if (i+5 >= len) {
		    /* error - clear allocated length */
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
		    /* single byte \u00xx */
		    i += 5;
		    *p++ = (char)((xc << 4) | xd);

		    if (xc == 0 && xd == 0 && has_nul != NULL) {
			*has_nul = true; /* record NUL byte */
		    }
		/*
		 * case: \uxxxx
		 */
		} else {

		    /*
		     * paranoia
		     */
		    if (p+1 >= beyond) {
			/* error - clear allocated length */
			if (retlen != NULL) {
			    *retlen = 0;
			}
			warn(__func__, "ran beyond end of decoded string for non-UTF-8 \\u encoding");
			return NULL;
		    }
		    /* double byte \uxxxx */
		    i += 5;
		    *p++ = (char)((xa << 4) | xb);
		    *p++ = (char)((xc << 4) | xd);
		}
		break;

	    /*
	     * unknown \c escaped pairs
	     */
	    default:
		/* error - clear allocated length */
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
    dbg(DBG_VVVHIGH, "returning from json_decode(ptr, %ju, *%ju, %s)",
		     (uintmax_t)len, (uintmax_t)mlen, has_nul != NULL ? booltostr(*has_nul) : "false");
    if (retlen != NULL) {
	*retlen = mlen;
    }
    return ret;
}


/*
 * json_decode_str - return a JSON decoding of a string
 *
 * This is a simplified interface for json_decode().
 *
 * given:
 *	str	NUL terminated C-style string to decode
 *	retlen	address of where to store allocated length, if retlen != NULL
 *
 * returns:
 *	allocated JSON decoding of a block, or NULL ==> error
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function assumes that str is a valid C-style string with a terminating
 *	 NUL byte.  This code assumes that the first NUL byte found is the end of
 *	 the string.  When working with raw binary data, or data that could have
 *	 a NUL byte inside the block of memory (instead at the very end), this
 *	 function should NOT be used.  In such cases, json_decode() should
 *	 be used instead!
 */
char *
json_decode_str(char const *str, size_t *retlen)
{
    void *ret = NULL;	    /* allocated decoding string or NULL */
    size_t len = 0;	    /* length of string to decode */

    /*
     * firewall
     */
    if (str == NULL) {
	/* error - clear allocated length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    len = strlen(str);

    /*
     * convert to json_decode() call
     */
    ret = json_decode(str, len, retlen, NULL);
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
 * parse_json_string - parse a json string
 *
 * given:
 *
 *	string	    - the text that triggered the action
 *	len	    - length of the string to convert (important for NUL bytes)
 *
 * Returns a pointer to a struct json with the converted string.
 *
 * NOTE: The len is important for strings that have NUL bytes: without it we
 * would rely on strlen() which would mean that the first NUL byte would be the
 * end of the string. If len <= 0 this function uses strlen() on the string.
 *
 * NOTE: This function does not return if passed a NULL string or if conversion fails.
 */
struct json *
parse_json_string(char const *string, size_t len)
{
    struct json *str = NULL;
    struct json_string *item = NULL;

    /*
     * firewall
     */
    if (string == NULL) {
	err(150, __func__, "passed NULL string");
	not_reached();
    }

    /* obtain length if necessary */
    if (len <= 0)
	len = strlen(string);

    /*
     * we say that quote == true because the pattern in the lexer will include
     * the '"'s.
     */
    str = json_conv_string(string, len, true);
    /* paranoia - these tests should never result in an error */
    if (str == NULL) {
        err(151, __func__, "converting JSON string returned NULL: <%s>", string);
        not_reached();
    } else if (str->type != JTYPE_STRING) {
        err(152, __func__, "expected JTYPE_STRING, found type: %s", json_item_type_name(str));
        not_reached();
    }
    item = &(str->item.string);
    if (!VALID_JSON_NODE(item)) {
	err(153, __func__, "couldn't decode string: <%s>", string);
	not_reached();
    }
    return str;
}


/*
 * parse_json_bool - parse a json bool
 *
 * given:
 *
 *	string	    - the text that triggered the action
 *
 * returns:
 *	pointer to a struct json with the converted JSON boolean
 *
 * NOTE: This function does not return if passed a NULL string or conversion fails.
 */
struct json *
parse_json_bool(char const *string)
{
    struct json *boolean = NULL;
    struct json_boolean *item = NULL;

    /*
     * firewall
     */
    if (string == NULL) {
	err(154, __func__, "passed NULL string");
	not_reached();
    }

    boolean = json_conv_bool_str(string, NULL);
    /* paranoia - these tests should never result in an error */
    if (boolean == NULL) {
	err(155, __func__, "converting JSON bool returned NULL: <%s>", string);
	not_reached();
    } else if (boolean->type != JTYPE_BOOL) {
        err(156, __func__, "expected JTYPE_BOOL, found type: %s", json_item_type_name(boolean));
        not_reached();
    }
    item = &(boolean->item.boolean);
    if (!VALID_JSON_NODE(item)) {
	/*
	 * json_conv_bool_str() calls json_conv_bool() which will warn if the
	 * boolean is neither true nor false. We know that this function should never
	 * be called on anything but the strings "true" or "false" and since the
	 * function will abort if NULL is returned we should check if
	 * boolean->converted == true.
	 *
	 * If it's not we abort as there's a serious mismatch between the
	 * scanner and the parser.
	 */
	err(157, __func__, "called on non-boolean string: <%s>", string);
	not_reached();
    } else if (item->as_str == NULL) {
	/* extra sanity check - make sure the allocated string != NULL */
	err(158, __func__, "boolean->as_str == NULL");
	not_reached();
    } else if (strcmp(item->as_str, "true") && strcmp(item->as_str, "false")) {
	/*
	 * extra sanity check - make sure the allocated string is "true"
	 * or "false"
	 */
	err(159, __func__, "boolean->as_str neither \"true\" nor \"false\"");
	not_reached();
    } else {
	/*
	 * extra sanity checks - convert back and forth as string to bool and
	 * bool to string and make sure everything matches.
	 */
	char const *str = booltostr(item->value);
	if (str == NULL) {
	    err(160, __func__, "could not convert boolean->value back to a string");
	    not_reached();
	} else if (strcmp(str, item->as_str)) {
	    err(161, __func__, "boolean->as_str != item->value as a string");
	    not_reached();
	} else if (strtobool(item->as_str) != item->value) {
	    err(162, __func__, "mismatch between boolean string and converted value");
	    not_reached();
	} else if (strtobool(str) != item->value) {
	    err(163, __func__, "mismatch between converted string value and converted value");
	    not_reached();
	}
    }

    return boolean;
}


/*
 * parse_json_null - parse a json null
 *
 * given:
 *
 *	string	    - the text that triggered the action
 *
 * returns:
 *	pointer to a struct json with the converted JSON null
 *
 * NOTE: This function does not return if passed a NULL string or if null becomes NULL :-)
 */
struct json *
parse_json_null(char const *string)
{
    struct json *null = NULL;
    struct json_null *item = NULL;

    /*
     * firewall
     */
    if (string == NULL) {
	err(164, __func__, "passed NULL string");
	not_reached();
    }

    /*
     * null should not be NULL :-)
     */
    null = json_conv_null_str(string, NULL);
    if (null == NULL) {
	/* ironically null is NULL and it actually should not be :-) */
	err(165, __func__, "null is NULL");
	not_reached();
    } else if (null->type != JTYPE_NULL) {
        err(166, __func__, "expected JTYPE_NULL, found type: %s", json_item_type_name(null));
        not_reached();
    }
    item = &(null->item.null);
    if (!VALID_JSON_NODE(item)) {
	/* why is it an error if we can't convert nothing ? :-) */
	err(167,__func__, "couldn't convert null: <%s>", string);
	not_reached();
    }

    return null;
}


/*
 * parse_json_number - parse a JSON number
 *
 * given:
 *
 *	string	    - the text that triggered the action
 *
 * returns:
 *	pointer to a struct json with the converted JSON number
 *
 * NOTE: This function does not return if passed a NULL string or if conversion fails.
 */
struct json *
parse_json_number(char const *string)
{
    struct json *number = NULL;
    struct json_number *item = NULL;

    /*
     * firewall
     */
    if (string == NULL) {
	err(168, __func__, "passed NULL string");
	not_reached();
    }

    /*
     * form the JSON number parse tree node
     */
    number = json_conv_number_str(string, NULL);
    /* paranoia - these tests should never result in an error */
    if (number == NULL) {
	err(169, __func__, "converting JSON number returned NULL: <%s>", string);
        not_reached();
    } else if (number->type != JTYPE_NUMBER) {
        err(170, __func__, "expected JTYPE_NUMBER, found type: %s", json_item_type_name(number));
        not_reached();
    }
    item = &(number->item.number);
    if (!VALID_JSON_NODE(item)) {
	err(171, __func__, "couldn't convert number string: <%s>", string);
	not_reached();
    }
    return number;
}


/*
 * parse_json_array - parse a JSON array given a JSON elements
 *
 * Given a JSON elements, we turn it into a JSON array.
 *
 * IMPORTANT: The struct json_array must be identical to struct json_elements because
 *	      parse_json_array() converts by just changing the JSON item type.
 *
 * given:
 *
 *	elements    - the struct json * name of the JSON elements
 *
 * returns:
 *	pointer to a struct json with the converted JSON array
 *
 * NOTE: This function does not return if passed a NULL elements;
 */
struct json *
parse_json_array(struct json *elements)
{
    struct json_array *item = NULL;

    /*
     * firewall
     */
    if (elements == NULL) {
	err(172, __func__, "passed NULL elements value");
	not_reached();
    }
    if (elements->type != JTYPE_ELEMENTS) {
	err(173, __func__, "expected type JTYPE_ELEMENTS: found: %s (%d)",
			   json_item_type_name(elements), elements->type);
	not_reached();
    }

    /*
     * change the item type
     *
     * This hack works because struct json_array is identical to struct json_elements.
     */
    elements->type = JTYPE_ARRAY;
    /* paranoia - these tests should never result in an error */
    item = &(elements->item.array);
    if (!VALID_JSON_NODE(item)) {
	err(174, __func__, "couldn't convert array");
	not_reached();
    }
    return elements;
}


/*
 * parse_json_member - parse a JSON member
 *
 * given:
 *
 *	name	    - the struct json * name of the member
 *	value	    - the struct json * value of the member
 *
 * returns:
 *	pointer to a struct json with the converted JSON member
 *
 * NOTE: This function does not return if passed a NULL name or value or if conversion fails.
 */
struct json *
parse_json_member(struct json *name, struct json *value)
{
    struct json *member = NULL;
    struct json_member *item = NULL;

    /*
     * firewall
     */
    if (name == NULL) {
	err(175, __func__, "passed NULL name value");
	not_reached();
    } else if (value == NULL) {
	err(176, __func__, "passed NULL value");
	not_reached();
    }
    if (name->type != JTYPE_STRING) {
	err(177, __func__, "expected name->type == JTYPE_STRING: is %s (%d)", json_item_type_name(name), name->type);
	not_reached();
    }

    /*
     * form the JSON member parse tree node
     */
    member = json_conv_member(name, value);
    /* paranoia - these tests should never result in an error */
    if (member == NULL) {
	err(178, __func__, "converting JSON member returned NULL");
	not_reached();
    } else if (member->type != JTYPE_MEMBER) {
        err(179, __func__, "expected JTYPE_MEMBER, found type: %s", json_item_type_name(member));
        not_reached();
    }
    item = &(member->item.member);
    if (!VALID_JSON_NODE(item)) {
	err(180, __func__, "couldn't convert member");
	not_reached();
    }
    return member;
}


/*
 * json_alloc - allocate and initialize the JSON parse tree item
 *
 * given:
 *	type	a valid struct json type
 *
 * returns:
 *	pointer to an initialized the JSON parse tree item
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_alloc(enum item_type type)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */

    /*
     * inspect the struct json type for unknown types
     */
    switch (type) {
    case JTYPE_UNSET:
    case JTYPE_NUMBER:
    case JTYPE_STRING:
    case JTYPE_BOOL:
    case JTYPE_NULL:
    case JTYPE_MEMBER:
    case JTYPE_OBJECT:
    case JTYPE_ARRAY:
    case JTYPE_ELEMENTS:
	break;
    default:
	warn(__func__, "called with unknown JSON type: %d", type);
	break;
    }

    /*
     * allocate the JSON parse tree item
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = calloc(1, sizeof(*ret));
    if (ret == NULL) {
	errp(181, __func__, "calloc #0 error allocating %ju bytes", (uintmax_t)sizeof(*ret));
	not_reached();
    }

    /*
     * initialize the JSON parse tree item
     */
    ret->type = type;
    ret->parent = NULL;

    /*
     * paranoia - explicitly elements in the "struct json_foo" structure
     *
     * We will set the common elements in the "struct json_foo" as follows:
     *
     *		parsed = false;
     *		converted = false;
     *		as_str = NULL;
     *
     * We also set other pointers to NULL on a type by type basis.
     *
     * It could be argued that the above use of calloc(3) will zeroize the structure
     * and so set pointers to NULL and booleans to false.  As one might read in this FAQ:
     *
     *		https://c-faq.com/null/nullor0.html
     *		https://c-faq.com/null/runtime0.html
     *
     * The answer is not so simple: especially when you have to take into account
     * the way a compiler writer (or compiler feature or compiler mis-feature)
     * might interpret such FAQs.
     *
     * Instead of diving into a pedantic details for NULL vs. 0, we explicitly
     * set NULL pointers.  And while 0 == false, we also explicitly set both
     * parsed and converted to false as well.
     *
     * One advantage of such explicit setting is that certain code analyzers will
     * understand the NULL or false intent of such structure elements.
     */
    switch (ret->type) {
    case JTYPE_UNSET:
	break;
    case JTYPE_NUMBER:
	{
	    struct json_number *item = &(ret->item.number);

	    item->parsed = false;
	    item->converted = false;
	    item->as_str = NULL;
	};
	break;
    case JTYPE_STRING:
	{
	    struct json_string *item = &(ret->item.string);

	    item->parsed = false;
	    item->converted = false;
	    item->as_str = NULL;
	    item->str = NULL;
	};
	break;
    case JTYPE_BOOL:
	{
	    struct json_boolean *item = &(ret->item.boolean);

	    item->parsed = false;
	    item->converted = false;
	    item->as_str = NULL;
	};
	break;
    case JTYPE_NULL:
	{
	    struct json_null *item = &(ret->item.null);

	    item->parsed = false;
	    item->converted = false;
	    item->as_str = NULL;
	};
	break;
    case JTYPE_MEMBER:
	{
	    struct json_member *item = &(ret->item.member);

	    item->parsed = false;
	    item->converted = false;
	    item->name_as_str = NULL;
	    item->name_str = NULL;
	    item->name = NULL;
	    item->value = NULL;
	};
	break;
    case JTYPE_OBJECT:
	{
	    struct json_object *item = &(ret->item.object);

	    item->parsed = false;
	    item->converted = false;
	    item->set = NULL;
	    item->s = NULL;
	};
	break;
    case JTYPE_ARRAY:
	{
	    struct json_array *item = &(ret->item.array);

	    item->parsed = false;
	    item->converted = false;
	    item->set = NULL;
	    item->s = NULL;
	};
	break;
    case JTYPE_ELEMENTS:
	{
	    struct json_elements *item = &(ret->item.elements);

	    item->parsed = false;
	    item->converted = false;
	    item->set = NULL;
	    item->s = NULL;
	};
	break;
    default:
	warn(__func__, "attempt to initialize an unknown JSON type: %d", ret->type);
	break;
    }
    json_dbg(JSON_DBG_VHIGH, __func__, "allocated json with type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_process_decimal - process a JSON integer string
 *
 * We will fill out the struct json_number item assuming that ptr, with length len,
 * points to a JSON integer string of base 10 ASCII as allowed by the so-called JSON spec.
 *
 * given:
 *	item	pointer to a JSON number structure (struct json_number*)
 *	str	JSON integer as a NUL terminated C-style string
 *	len	length of the JSON number that is not whitespace
 *
 * NOTE: This function assumes that str points to the start of a JSON number, NOT whitespace.
 *
 * NOTE: This function assumes that str is a NUL terminated string,
 *	 even if the NUL is well beyond the end of the JSON number.
 *
 * NOTE: While it is OK if str has trailing whitespace, str[len-1] must be an
 *	 ASCII digit.  It is assumed that str[len-1] is the final JSON number character.
 */
static bool
json_process_decimal(struct json_number *item, char const *str, size_t len)
{
    char *endptr;			/* first invalid character or str */
    size_t str_len = 0;			/* length as a C string, of str */

    /*
     * firewall
     */
    if (item == NULL) {
	warn(__func__, "called with NULL item");
	return false;	/* processing failed */
    }
    if (str == NULL) {
	warn(__func__, "called with NULL str");
	return false;	/* processing failed */
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return false;	/* processing failed */
    }
    if (str[0] == '\0') {
	warn(__func__, "called with empty string");
	return false;	/* processing failed */
    }
    if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	warn(__func__, "str[%ju-1] is not an ASCII digit: 0x%02x for str: %s", (uintmax_t)len, (int)str[len-1], str);
	return false;	/* processing failed */
    }
    str_len = strlen(str);
    if (str_len < len) {
	warn(__func__, "strlen(%s): %ju < len arg: %ju", str, (uintmax_t)str_len, (uintmax_t)len);
	return false;	/* processing failed */
    }

    /*
     * determine if JSON integer negative
     */
    if (str[0] == '-') {

	/* parse JSON integer that is < 0 */
	item->is_negative = true;

        /*
	 * JSON spec detail: lone - invalid JSON
	 */
	if (len <= 1 || str[1] == '\0') {
	    dbg(DBG_HIGH, "in %s(): called with just -: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

        /*
	 * JSON spec detail: leading -0 followed by digits - invalid JSON
	 */
	} else if (len > 2 && str[1] == '0' && isascii(str[2]) && isdigit(str[2])) {
	    dbg(DBG_HIGH, "in %s(): called with -0 followed by more digits: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}

    /* case: JSON integer is >= 0 */
    } else {

	/* parse JSON integer that is >= 0 */
	item->is_negative = false;

        /*
	 * JSON spec detail: leading 0 followed by digits - invalid JSON
	 */
	if (len > 1 && str[0] == '0' && isascii(str[1]) && isdigit(str[1])) {
	    dbg(DBG_HIGH, "in %s(): called with 0 followed by more digits: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}
    }

    /*
     * attempt to convert to the largest possible integer
     */
    if (item->is_negative) {

	/* case: negative, try for largest signed integer */
	errno = 0;			/* pre-clear errno for errp() */
	item->as_maxint = strtoimax(str, &endptr, 10);
	if (errno == ERANGE || errno == EINVAL || endptr == str || endptr == NULL) {
	    if (errno == ERANGE) {
		dbg(DBG_VVVHIGH, "negative integer out of range, strtoimax failed to convert");
	    } else {
		dbg(DBG_VVVHIGH, "invalid negative integer, strtoimax failed to convert");
	    }
	    return false;	/* processing failed */
	}
	item->maxint_sized = true;
	dbg(DBG_VVVHIGH, "strtoimax for <%s> returned: %jd", str, item->as_maxint);

	/* case int8_t: range check */
	if (item->as_maxint >= (intmax_t)INT8_MIN && item->as_maxint <= (intmax_t)INT8_MAX) {
	    item->int8_sized = true;
	    item->as_int8 = (int8_t)item->as_maxint;
	}

	/* case uint8_t: cannot be because JSON string is < 0 */
	item->uint8_sized = false;

	/* case int16_t: range check */
	if (item->as_maxint >= (intmax_t)INT16_MIN && item->as_maxint <= (intmax_t)INT16_MAX) {
	    item->int16_sized = true;
	    item->as_int16 = (int16_t)item->as_maxint;
	}

	/* case uint16_t: cannot be because JSON string is < 0 */
	item->uint16_sized = false;

	/* case int32_t: range check */
	if (item->as_maxint >= (intmax_t)INT32_MIN && item->as_maxint <= (intmax_t)INT32_MAX) {
	    item->int32_sized = true;
	    item->as_int32 = (int32_t)item->as_maxint;
	}

	/* case uint32_t: cannot be because JSON string is < 0 */
	item->uint32_sized = false;

	/* case int64_t: range check */
	if (item->as_maxint >= (intmax_t)INT64_MIN && item->as_maxint <= (intmax_t)INT64_MAX) {
	    item->int64_sized = true;
	    item->as_int64 = (int64_t)item->as_maxint;
	}

	/* case uint64_t: cannot be because JSON string is < 0 */
	item->uint64_sized = false;

	/* case int: range check */
	if (item->as_maxint >= (intmax_t)INT_MIN && item->as_maxint <= (intmax_t)INT_MAX) {
	    item->int_sized = true;
	    item->as_int = (int)item->as_maxint;
	}

	/* case unsigned int: cannot be because JSON string is < 0 */
	item->uint_sized = false;

	/* case long: range check */
	if (item->as_maxint >= (intmax_t)LONG_MIN && item->as_maxint <= (intmax_t)LONG_MAX) {
	    item->long_sized = true;
	    item->as_long = (long)item->as_maxint;
	}

	/* case unsigned long: cannot be because JSON string is < 0 */
	item->ulong_sized = false;

	/* case long long: range check */
	if (item->as_maxint >= (intmax_t)LLONG_MIN && item->as_maxint <= (intmax_t)LLONG_MAX) {
	    item->longlong_sized = true;
	    item->as_longlong = (long long)item->as_maxint;
	}

	/* case unsigned long long: cannot be because JSON string is < 0 */
	item->ulonglong_sized = false;

	/* case size_t: cannot be because JSON string is < 0 */
	item->size_sized = false;

	/* case ssize_t: range check */
	if (item->as_maxint >= (intmax_t)SSIZE_MIN && item->as_maxint <= (intmax_t)SSIZE_MAX) {
	    item->ssize_sized = true;
	    item->as_ssize = (ssize_t)item->as_maxint;
	}

	/* case off_t: range check */
	if (item->as_maxint >= (intmax_t)OFF_MIN && item->as_maxint <= (intmax_t)OFF_MAX) {
	    item->off_sized = true;
	    item->as_off = (off_t)item->as_maxint;
	}

	/* case intmax_t: was handled by the above call to strtoimax() */

	/* case uintmax_t: cannot be because JSON string is < 0 */
	item->umaxint_sized = false;

    } else {

	/* case: positive, try for largest unsigned integer */
	errno = 0;			/* pre-clear errno for errp() */
	item->as_umaxint = strtoumax(str, &endptr, 10);
	if (errno == ERANGE || errno == EINVAL || endptr == str || endptr == NULL) {
	    if (errno == ERANGE) {
		dbg(DBG_VVVHIGH, "positive integer out of range, strtoumax failed to convert");
	    } else {
		dbg(DBG_VVVHIGH, "invalid positive integer, strtoumax failed to convert");
	    }
	    return false;	/* processing failed */
	}
	item->umaxint_sized = true;
	dbg(DBG_VVVHIGH, "strtoumax for <%s> returned: %ju", str, item->as_umaxint);

	/* case int8_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT8_MAX) {
	    item->int8_sized = true;
	    item->as_int8 = (int8_t)item->as_umaxint;
	}

	/* case uint8_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT8_MAX) {
	    item->uint8_sized = true;
	    item->as_uint8 = (uint8_t)item->as_umaxint;
	}

	/* case int16_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT16_MAX) {
	    item->int16_sized = true;
	    item->as_int16 = (int16_t)item->as_umaxint;
	}

	/* case uint16_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT16_MAX) {
	    item->uint16_sized = true;
	    item->as_uint16 = (uint16_t)item->as_umaxint;
	}

	/* case int32_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT32_MAX) {
	    item->int32_sized = true;
	    item->as_int32 = (int32_t)item->as_umaxint;
	}

	/* case uint32_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT32_MAX) {
	    item->uint32_sized = true;
	    item->as_uint32 = (uint32_t)item->as_umaxint;
	}

	/* case int64_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT64_MAX) {
	    item->int64_sized = true;
	    item->as_int64 = (int64_t)item->as_umaxint;
	}

	/* case uint64_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT64_MAX) {
	    item->uint64_sized = true;
	    item->as_uint64 = (uint64_t)item->as_umaxint;
	}

	/* case int: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT_MAX) {
	    item->int_sized = true;
	    item->as_int = (int)item->as_umaxint;
	}

	/* case unsigned int: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT_MAX) {
	    item->uint_sized = true;
	    item->as_uint = (unsigned int)item->as_umaxint;
	}

	/* case long: bounds check */
	if (item->as_umaxint <= (uintmax_t)LONG_MAX) {
	    item->long_sized = true;
	    item->as_long = (long)item->as_umaxint;
	}

	/* case unsigned long: bounds check */
	if (item->as_umaxint <= (uintmax_t)ULONG_MAX) {
	    item->ulong_sized = true;
	    item->as_ulong = (unsigned long)item->as_umaxint;
	}

	/* case long long: bounds check */
	if (item->as_umaxint <= (uintmax_t)LLONG_MAX) {
	    item->longlong_sized = true;
	    item->as_longlong = (long long)item->as_umaxint;
	}

	/* case unsigned long long: bounds check */
	if (item->as_umaxint <= (uintmax_t)ULLONG_MAX) {
	    item->ulonglong_sized = true;
	    item->as_ulonglong = (unsigned long long)item->as_umaxint;
	}

	/* case ssize_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)SSIZE_MAX) {
	    item->ssize_sized = true;
	    item->as_ssize = (ssize_t)item->as_umaxint;
	}

	/* case size_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)SIZE_MAX) {
	    item->size_sized = true;
	    item->as_size = (size_t)item->as_umaxint;
	}

	/* case off_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)OFF_MAX) {
	    item->off_sized = true;
	    item->as_off = (off_t)item->as_umaxint;
	}

	/* case intmax_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INTMAX_MAX) {
	    item->maxint_sized = true;
	    item->as_maxint = (intmax_t)item->as_umaxint;
	}

	/* case uintmax_t: was handled by the above call to strtoumax() */
    }

    /*
     * processing was successful
     */
    return true;
}


/*
 * json_process_floating - process JSON floating point or e-notation string
 *
 * We will fill out the struct json_number item assuming that ptr, with length len,
 * points to a JSON floating point string as allowed by the so-called JSON spec.
 * The JSON floating point string can be with a JSON float (what the spec calls
 * integer + faction + exponent).
 *
 * given:
 *	item	pointer to a JSON number structure (struct json_number*)
 *	str	JSON floating point or JSON e-notation value as a NUL terminated C-style string
 *	len	length of the JSON number that is not whitespace
 *
 * NOTE: This function assumes that str points to the start of a JSON number, NOT whitespace.
 *
 * NOTE: This function assumes that str is a NUL terminated string,
 *	 even if the NUL is well beyond the end of the JSON number.
 *
 * NOTE: While it is OK if str has trailing whitespace, str[len-1] must be an
 *	 ASCII digit.  It is assumed that str[len-1] is the final JSON number
 *	 character.
 */
static bool
json_process_floating(struct json_number *item, char const *str, size_t len)
{
    char *endptr;			/* first invalid character or str */
    char *e_found = NULL;		/* strchr() search for e */
    char *cap_e_found = NULL;		/* strchr() search for E */
    char *e = NULL;			/* strrchr() search for second e or E */
    char *dot_found = NULL;		/* strchr() search for '.' */
    char *dot = NULL;			/* strrchr() search for '.' */
    size_t str_len = 0;			/* length as a C string, of str */

    /*
     * firewall
     */
    if (item == NULL) {
	warn(__func__, "called with NULL item");
	return false;	/* processing failed */
    }
    if (str == NULL) {
	warn(__func__, "called with NULL str");
	return false;	/* processing failed */
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return false;	/* processing failed */
    }
    if (str[0] == '\0') {
	warn(__func__, "called with empty string");
	return false;	/* processing failed */
    }
    if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	warn(__func__, "str[%ju-1] is not an ASCII digit: 0x%02x for str: %s", (uintmax_t)len, (int)str[len-1], str);
	return false;	/* processing failed */
    }
    str_len = strlen(str);
    if (str_len < len) {
	warn(__func__, "strlen(%s): %ju < len arg: %ju", str, (uintmax_t)str_len, (uintmax_t)len);
	return false;	/* processing failed */
    }

    /*
     * JSON spec detail: floating point numbers cannot start with .
     */
    if (str[0] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot start with .: <%s>",
		       __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with .
     */
    } else if (str[len-1] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with .: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with -
     */
    } else if (str[len-1] == '-') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with -: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with +
     */
    } else if (str[len-1] == '+') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with +: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers must end in a digit
     */
    } else if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	dbg(DBG_HIGH, "in %s(): floating point numbers must end in a digit: <%s>",
		       __func__, str);
	return false;	/* processing failed */
    }

    /* detect dot */
    dot_found = strchr(str, '.');
    /* if dot found see if there's another one */
    if (dot_found != NULL) {
	dot = strrchr(str, '.');
	if (dot != NULL && dot != dot_found) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have two '.'s: <%s>",
		      __func__, str);
	    return false;	/* processing failed */
	}
	item->is_floating = true;
    }

    /*
     * detect use of e notation
     */
    e_found = strchr(str, 'e');
    cap_e_found = strchr(str, 'E');
    /* case: both e and E found */
    if (e_found != NULL && cap_e_found != NULL) {

	dbg(DBG_HIGH, "in %s(): floating point numbers cannot use both e and E: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /* case: just e found, no E */
    } else if (e_found != NULL) {

	/* firewall - search for two 'e's */
	e = strrchr(str, 'e');
	if (e_found != e) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have more than one e: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}

	/* note e notation */
	item->is_e_notation = true;

    /* case: just E found, no e */
    } else if (cap_e_found != NULL) {

	/* firewall - search for two 'E's */
	e = strrchr(str, 'E');
	if (cap_e_found != e) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have more than one E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}

	/* note e notation */
	item->is_e_notation = true;
    } else {
	/* case: neither 'e' nor 'E' found */
	item->is_e_notation = false;
    }


    /*
     * perform additional JSON number checks on e notation
     *
     * NOTE: If item->is_e_notation == true, then e points to the e or E
     */
    if (item->is_e_notation == true) {

	/*
	 * JSON spec detail: e notation number cannot start with e or E
	 */
	if (e == str) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot start with e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot end with e or E
	 */
	} else if (e == &(str[len-1])) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot end with e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot have e or E after .
	 */
	} else if (e > str && e[-1] == '.') {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot have '.' before e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number must have digit before e or E
	 */
	} else if (e > str && (!isascii(e[-1]) || !isdigit(e[-1]))) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers must have digit before e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * case: e notation number with a leading + in the exponent
	 *
	 * NOTE: From "floating point numbers cannot end with +" we know + is not at the end.
	 */
	} else if (e[1] == '+') {

	    /*
	     * JSON spec detail: e notation number with e+ or E+ must be followed by a digit
	     */
	    if (e+1 < &(str[len-1]) && (!isascii(e[2]) || !isdigit(e[2]))) {
		dbg(DBG_HIGH, "in %s(): :e notation number with e+ or E+ must be followed by a digit <%s>",
			      __func__, str);
		return false;	/* processing failed */
	    }

	/*
	 * case: e notation number with a leading - in the exponent
	 *
	 * NOTE: From "floating point numbers cannot end with -" we know - is not at the end.
	 */
	} else if (e[1] == '-') {

	    /*
	     * JSON spec detail: e notation number with e- or E- must be followed by a digit
	     */
	    if (e+1 < &(str[len-1]) && (!isascii(e[2]) || !isdigit(e[2]))) {
		dbg(DBG_HIGH, "in %s(): :e notation number with e- or E- must be followed by a digit <%s>",
			      __func__, str);
		return false;	/* processing failed */
	    }

	/*
	 * JSON spec detail: e notation number must have + or - or digit after e or E
	 */
	} else if (!isascii(e[1]) || !isdigit(e[1])) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers must follow e or E with + or - or digit: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}
    }

    /*
     * convert to largest floating point value
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_longdouble = strtold(str, &endptr);
    if (errno == ERANGE || endptr == str || endptr == NULL) {
	if (errno == ERANGE) {
	    /* if range problem we know it's parsable */
	    item->parsed = true;
	}
	dbg(DBG_VVVHIGH, "strtold failed to convert");
	return false;	/* processing failed */
    }
    item->longdouble_sized = true;
    item->as_longdouble_int = (item->as_longdouble == floorl(item->as_longdouble));
    dbg(DBG_VVVHIGH, "strtold for <%s> returned as %%Lg: %.22Lg", str, item->as_longdouble);
    dbg(DBG_VVVHIGH, "strtold for <%s> returned as %%Le: %.22Le", str, item->as_longdouble);
    dbg(DBG_VVVHIGH, "strtold for <%s> returned as %%Lf: %.22Lf", str, item->as_longdouble);
    dbg(DBG_VVVHIGH, "strtold returned an integer value: %s", booltostr(item->as_longdouble_int));

    /*
     * note if value < 0
     */
    if (item->as_longdouble < 0) {
	item->is_negative = true;
    }

    /*
     * convert to double
     */
    errno = 0;			/* pre-clear conversion test */
    item->as_double = strtod(str, &endptr);
    if (errno == ERANGE || endptr == str || endptr == NULL) {
	if (errno == ERANGE) {
	    /* if range problem we know it's parsable */
	    item->parsed = true;
	}
	item->double_sized = false;
	dbg(DBG_VVVHIGH, "strtod for <%s> failed", str);
    } else {
	item->double_sized = true;
	item->parsed = true;
	item->as_double_int = (item->as_double == floor(item->as_double));
	dbg(DBG_VVVHIGH, "strtod for <%s> returned as %%lg: %.22lg", str, item->as_double);
	dbg(DBG_VVVHIGH, "strtod for <%s> returned as %%le: %.22le", str, item->as_double);
	dbg(DBG_VVVHIGH, "strtod for <%s> returned as %%lf: %.22lf", str, item->as_double);
	dbg(DBG_VVVHIGH, "strtod returned an integer value: %s", booltostr(item->as_double_int));
    }

    /*
     * convert to float
     */
    errno = 0;			/* pre-clear conversion test */
    item->as_float = strtof(str, &endptr);
    if (errno == ERANGE || endptr == str || endptr == NULL) {
	if (errno == ERANGE) {
	    /* if range issue we know it's parsable */
	    item->parsed = true;
	}
	item->float_sized = false;
	dbg(DBG_VVVHIGH, "strtof for <%s> failed", str);
    } else {
	item->parsed = true;
	item->float_sized = true;
	item->as_float_int = (item->as_longdouble == floorl(item->as_longdouble));
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%g: %.22g", str, (double)item->as_float);
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%e: %.22e", str, (double)item->as_float);
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%f: %.22f", str, (double)item->as_float);
	dbg(DBG_VVVHIGH, "strtof returned an integer value: %s", booltostr(item->as_float_int));
    }

    /*
     * processing was successful
     */
    return true;
}


/*
 * json_conv_number - convert JSON number string to C numeric value
 *
 * A JSON number string is of the form:
 *
 *	({JSON_INTEGER}|{JSON_INTEGER}{JSON_FRACTION}|{JSON_INTEGER}{JSON_FRACTION}{JSON_EXPONENT}|{JSON_INTEGER}{JSON_EXPONENT})
 *
 * where {JSON_INTEGER} is of the form:
 *
 *      -?([1-9][0-9]*|0)
 *
 * and where {JSON_FRACTION} is of the form:
 *
 *      \.[0-9]+
 *
 * and where {JSON_EXPONENT} is of the form:
 *
 *      [Ee][-+]?[0-9]+
 *
 * given:
 *	ptr	pointer to buffer containing a JSON number
 *	len	length, starting at ptr, of the JSON number string
 *
 * returns:
 *	allocated JSON parser tree node of the parsed JSON number
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_number(char const *ptr, size_t len)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_number *item = NULL;	    /* JSON number item inside JSON parser tree node */
    bool decimal = false;		    /* true ==> ptr points to a base 10 integer in ASCII */
    bool e_notation = false;		    /* true ==> ptr points to e notation in ASCII */
    bool floating_notation = false;	    /* true ==> ptr points to floating point notation in ASCII */
    bool success = false;		    /* true ==> processing was successful */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_NUMBER);
    if (ret == NULL) {
	errp(182, __func__, "json_alloc(JTYPE_NUMBER) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.number);
    item->parsed = false;
    item->converted = false;
    item->as_str = NULL;
    item->first = NULL;
    item->is_negative = false;
    item->is_floating = false;
    item->is_e_notation = false;
    /* integer values */
    item->is_integer = false;
    item->int8_sized = false;
    item->uint8_sized = false;
    item->int16_sized = false;
    item->uint16_sized = false;
    item->int32_sized = false;
    item->uint32_sized = false;
    item->int64_sized = false;
    item->uint64_sized = false;
    item->int_sized = false;
    item->uint_sized = false;
    item->long_sized = false;
    item->ulong_sized = false;
    item->longlong_sized = false;
    item->ulonglong_sized = false;
    item->ssize_sized = false;
    item->size_sized = false;
    item->off_sized = false;
    item->maxint_sized = false;
    item->umaxint_sized = false;
    /* floating point values */
    item->float_sized = false;
    item->as_float = 0.0f;
    item->as_float_int = false;
    item->double_sized = false;
    item->as_double = 0.0;
    item->as_double_int = false;
    item->longdouble_sized = false;
    item->as_longdouble = 0.0L;
    item->as_longdouble_int = false;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL ptr");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    item->as_str_len = len;	/* save original length */
    if (ptr[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON integer string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = calloc(len+1+1, sizeof(char));
    if (item->as_str == NULL) {
	errp(183, __func__, "calloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    strncpy(item->as_str, ptr, len);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */

    /*
     * ignore whitespace
     *
     * Find the first ASCII text, ignoring any leading whitespace if found.
     * The length, within the len limit, of ASCII text that is neither
     * whitespace nor NUL byte, is also found.
     *
     * While the common use of this function is via bison/flex produced C code,
     * we want to keep the general case working where this function might
     * someday be called from some other code.  For such a future case we
     * want to trim off leading and trailing whitespace so that the code below
     * checking for < 0 and the code checking the conversion is not confused.
     */
    item->number_len = find_text(item->as_str, item->as_str_len, &(item->first));
    if (item->number_len <= 0) {
	dbg(DBG_HIGH, "in %s(): called with string containing all whitespace: <%s>",
		      __func__, item->as_str);
	return ret;
    }

    /*
     * attempt to determine the type of JSON number we have been given
     */
    floating_notation = is_floating_notation(item->first, item->number_len);
    item->is_floating = floating_notation;
    /**/
    e_notation = is_e_notation(item->first, item->number_len);
    item->is_e_notation = e_notation;
    /**/
    decimal = is_decimal(item->first, item->number_len);
    item->is_integer = decimal;

    /*
     * case: JSON number is a floating point number
     */
    if (floating_notation) {

	/* process JSON number as floating point number */
	success = json_process_floating(item, item->first, item->number_len);
	if (success == false) {

	    /*
	     * this JSON number is not a true floating point value
	     */
	    json_dbg(JSON_DBG_HIGH, __func__, "JSON number as floating point number failed: <%s>",
				   item->as_str);

	} else {

	    /*
	     * this JSON number is a converted floating point value
	     */
	    item->converted = true;
	    json_dbg(JSON_DBG_VHIGH, __func__, "converted JSON floating point number of type %s: <%s>",
				     json_item_type_name(ret), item->as_str);
	}
	item->parsed = true;	/* floating point number has been parsed, regardless of conversion status */
    }

    /*
     * case: JSON number is an e-notation number
     */
    if (e_notation) {

	/* process JSON number as floating point or e-notation number */
	success = json_process_floating(item, item->first, item->number_len);
	if (success == false) {

	    /*
	     * this JSON number is not a true e_notation value
	     */
	    json_dbg(JSON_DBG_HIGH, __func__, "JSON number as e-notation number failed: <%s>", item->as_str);

	} else {

	    /*
	     * this JSON number is a converted e_notation value
	     */
	    item->converted = true;
	    json_dbg(JSON_DBG_VHIGH, __func__, "converted JSON e-notation number of type %s: <%s>",
				     json_item_type_name(ret), item->as_str);
	}
	item->parsed = true;	/* e-notation number has been parsed, regardless of conversion status */
    }

    /*
     * case: JSON number is a base 10 integer in ASCII
     */
    if (decimal) {

	/*
	 * process JSON number as a base 10 integer in ASCII
	 */
	success = json_process_decimal(item, item->first, item->number_len);
	if (success == false) {

	    /*
	     * this JSON number is not a true integer value
	     */
	    json_dbg(JSON_DBG_HIGH, __func__, "JSON number as base 10 integer in ASCII processing failed: <%s>", item->as_str);

	} else {

	    /*
	     * this JSON number is a converted integer value
	     */
	    item->converted = true;
	    json_dbg(JSON_DBG_VHIGH, __func__, "converted JSON integer of type %s: <%s>",
				     json_item_type_name(ret), item->as_str);
	}
	item->parsed = true;	/* 10 integer has been parsed, regardless of conversion status */
    }

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_number_str - convert JSON number string to C numeric value
 *
 * This is a simplified interface for json_conv_number().  See that function for
 * details.
 *
 * given:
 *	str	JSON number in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	allocated JSON parser tree node converted JSON integer
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_number_str(char const *str, size_t *retlen)
{
    struct json *ret = NULL;	    /* JSON parser tree node to return */
    size_t len = 0;		    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_number() handle the arg firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_number() call
     */
    ret = json_conv_number(str, len);
    if (ret == NULL) {
	err(184, __func__, "json_conv_number() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_string - convert JSON encoded string to C string
 *
 * A JSON string is of the form:
 *
 *      "([^\n"]|\\")*"
 *
 * given:
 *	ptr	pointer to buffer containing a JSON encoded string
 *	len	length, starting at ptr, of the JSON encoded string
 *	quote	true ==>  ignore JSON double quotes: both ptr[0] & ptr[len-1]
 *			  must be '"'
 *		false ==> the entire ptr is to be converted
 *
 * returns:
 *	allocated JSON parser tree node converted JSON string
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 *
 * NOTE: We let this function decide whether the string is actually valid
 * according to the JSON standard so the regex above is for the parser even if
 * it would theoretically allow invalid JSON strings.
 */
struct json *
json_conv_string(char const *ptr, size_t len, bool quote)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_string *item = NULL;	    /* JSON string item inside JSON parser tree node */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_STRING);
    if (ret == NULL) {
	errp(185, __func__, "json_alloc(JTYPE_STRING) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.string);
    item->as_str = NULL;
    item->str = NULL;
    item->converted = false;
    item->parsed = false;
    item->quote = false;
    item->same = false;
    item->has_nul = false;
    item->slash = false;
    item->posix_safe = false;
    item->first_alphanum = false;
    item->upper = false;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL string");
	return ret;
    }

    /*
     * case: JSON surrounding '"'s are to be ignored
     */
    item->as_str_len = len;	/* save length of as_str */
    if (quote == true) {

	/*
	 * firewall
	 */
	if (len < 2) {
	    warn(__func__, "quote === true: called with len: %ju < 2", (uintmax_t)len);
	    return ret;
	}
	if (ptr[0] != '"') {
	    warn(__func__, "quote === true: string does NOT start with a \"");
	    return ret;
	}
	if (ptr[len-1] != '"') {
	    warn(__func__, "quote === true: string does NOT end with a \"");
	    return ret;
	}

	/*
	 * ignore JSON surrounding '"'s
	 */
	item->quote = true;
	++ptr;
	len -= 2;
    }

    /*
     * duplicate the JSON string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = calloc(len+1+1, sizeof(char));
    if (item->as_str == NULL) {
	errp(186, __func__, "calloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    strncpy(item->as_str, ptr, len);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */

    /*
     * decode the JSON encoded string
     */
    /* decode the entire string */
    item->str = json_decode(item->as_str, len, &(item->str_len), &(item->has_nul));
    if (item->str == NULL) {
	warn(__func__, "quote === %s: JSON string decode failed for: <%s>",
		       booltostr(quote), item->as_str);
	return ret;
    }
    item->converted = true;	/* JSON decoding successful */
    item->parsed = true;	/* JSON parsed successful */

    /*
     * determine if decoded string is identical to the original JSON encoded string
     *
     * NOTE: We use memcmp() because there might be NUL bytes in the 'char *'
     * and strcmp() would stop at the first '\0'.
     */
    if (item->as_str_len == item->str_len && memcmp(item->as_str, item->str, item->as_str_len) == 0) {
	item->same = true;	/* decoded string same an original JSON encoded string (perhaps sans '"'s) */
    }

    /*
     * determine POSIX state of the decoded string
     */
    posix_safe_chk(item->str, item->str_len, &item->slash, &item->posix_safe, &item->first_alphanum, &item->upper);
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_string_str - convert JSON encoded string to C string
 *
 * This is a simplified interface for json_conv_string(). See that function for details.
 *
 * given:
 *	str	encoded JSON string in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *	quote	true ==> ignore JSON double quotes, both str[0] & str[len-1]
 *		must be '"', false ==> the entire str is to be converted
 *
 * returns:
 *	allocated JSON parser tree node converted JSON string
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_string_str(char const *str, size_t *retlen, bool quote)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    size_t len = 0;			    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_string() handle the arg firewall
     */
    if (str == NULL) {
	/* the firewall in json_conv_string() called below will handle the NULL str case */
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_string() call
     */
    ret = json_conv_string(str, len, quote);
    if (ret == NULL) {
	err(187, __func__, "json_conv_string() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_bool - convert JSON boolean to C bool
 *
 * A JSON boolean is of the form:
 *
 *      true
 *      false
 *
 * given:
 *	ptr	pointer to buffer containing a JSON boolean
 *	len	length, starting at ptr, of the JSON boolean
 *
 * returns:
 *	allocated JSON parser tree node converted JSON boolean
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_bool(char const *ptr, size_t len)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_boolean *item = NULL;	    /* allocated decoding string or NULL */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_BOOL);
    if (ret == NULL) {
	errp(188, __func__, "json_alloc(JTYPE_BOOL) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.boolean);
    item->as_str = NULL;
    item->converted = false;
    item->parsed = false;
    item->value = false;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL ptr");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    if (ptr[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON encoded string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = malloc(len+1+1);
    if (item->as_str == NULL) {
	errp(189, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    memcpy(item->as_str, ptr, len+1);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */
    item->as_str_len = len;

    /*
     * decode the JSON boolean
     */
    if (strcmp(item->as_str, "true") == 0) {
	item->converted = true;
	item->parsed = true;
	item->value = true;
    } else if (strcmp(item->as_str, "false") == 0) {
	item->converted = true;
	item->parsed = true;
	item->value = false;
    } else {
	warn(__func__, "JSON boolean string neither true nor false: <%s>", item->as_str);
	item->converted = false; /* extra sanity - force converted to false */
	item->parsed = false;	/* extra sanity - force parsed to false */
    }

    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_bool_str - convert JSON boolean to C bool
 *
 * This is a simplified interface for json_conv_bool(). See that function for details.
 *
 * given:
 *	str	JSON boolean in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	allocated JSON parser tree node converted JSON boolean
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_bool_str(char const *str, size_t *retlen)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    size_t len = 0;			    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_bool() handle the arg firewall
     */
    if (str == NULL) {
	/* the firewall in json_conv_bool() called below will handle the NULL str case */
	warn(__func__, "called with NULL string");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_bool() call
     */
    ret = json_conv_bool(str, len);
    if (ret == NULL) {
	err(190, __func__, "json_conv_bool(%s, %jd) returned NULL", str, (uintmax_t)len);
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_null - convert JSON null to C NULL
 *
 * A JSON null is of the form:
 *
 *      null
 *
 * given:
 *	ptr	pointer to buffer containing a JSON null
 *	len	length, starting at ptr, of the JSON null
 *
 * returns:
 *	allocated JSON parser tree node converted JSON null
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_null(char const *ptr, size_t len)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_null *item = NULL;	    /* allocated decoding string or NULL */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_NULL);
    if (ret == NULL) {
	errp(191, __func__, "json_alloc(JTYPE_NULL) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.null);
    item->as_str = NULL;
    item->value = NULL;
    item->converted = false;
    item->parsed = false;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL ptr");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    if (ptr[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = malloc(len+1+1);
    if (item->as_str == NULL) {
	errp(192, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    memcpy(item->as_str, ptr, len+1);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */
    item->as_str_len = len;

    /*
     * decode the JSON null
     */
    if (strcmp(item->as_str, "null") == 0) {
	item->converted = true;
	item->parsed = true;
	item->value = NULL;
    } else {
	warn(__func__, "JSON null string is not null: <%s>", item->as_str);
	item->converted = false;    /* extra sanity - force converted to false */
	item->parsed = false;	    /* extra sanity - force parsed to false */
    }

    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_null_str - convert JSON null to C NULL
 *
 * This is a simplified interface for json_conv_null(). See that function for details.
 *
 * given:
 *	str	JSON null in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	allocated JSON parser tree node converted JSON null
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_null_str(char const *str, size_t *retlen)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    size_t len = 0;			    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_null() handle the arg firewall
     */
    if (str == NULL) {
	/* the firewall in json_conv_null() called below will handle the NULL str case */
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_null() call
     */
    ret = json_conv_null(str, len);
    if (ret == NULL) {
	err(193, __func__, "json_conv_null() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_member - convert JSON member into JSON parser tree node
 *
 * A JSON member is of the form:
 *
 *      name : value
 *
 * where name is a JSON string of the form:
 *
 *      "([^\n"]|\\")*"
 *
 * and where value is any JSON value such as a:
 *
 *      JSON object
 *      JSON array
 *      JSON string
 *      JSON number
 *      JSON boolean
 *      JSON null
 *
 * given:
 *	name	JSON member string
 *	value	JSON member value (object, array, string, number, boolean, null)
 *
 * returns:
 *	allocated JSON parser tree node converted JSON member
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_member(struct json *name, struct json *value)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_member *item = NULL;	    /* JSON member */
    struct json_string *item2 = NULL;	    /* JSON string */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_MEMBER);
    if (ret == NULL) {
	errp(194, __func__, "json_alloc(JTYPE_MEMBER) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.member);
    item->converted = false;
    item->parsed = false;
    item->name_as_str = NULL;
    item->name_str = NULL;
    item->name = NULL;
    item->value = NULL;

    /*
     * firewall
     */
    if (name == NULL) {
	warn(__func__, "called with NULL name");
	return ret;
    }
    if (value == NULL) {
	warn(__func__, "called with NULL value");
	return ret;
    }
    if (name->type != JTYPE_STRING) {
	warn(__func__, "expected JSON string JTYPE_STRING (type: %d) found type: %s (type: %d)",
		JTYPE_STRING, json_item_type_name(name),  name->type);
	return ret;
    }
    switch (value->type) {
    case JTYPE_NUMBER:
    case JTYPE_STRING:
    case JTYPE_BOOL:
    case JTYPE_NULL:
    case JTYPE_MEMBER:
    case JTYPE_OBJECT:
    case JTYPE_ARRAY:
	json_dbg(JSON_DBG_VHIGH, __func__, "JSON name type: %s", json_item_type_name(name));
	json_dbg(JSON_DBG_VHIGH, __func__, "JSON value type: %s", json_item_type_name(value));
	break;
    case JTYPE_ELEMENTS:
	warn(__func__, "JSON type %s (type: %d) is invalid here",
		json_item_type_name(name), JTYPE_ELEMENTS);
	return ret;
	break;
    default:
	warn(__func__, "expected JSON object, array, string, number, boolean or null, found type: %d", value->type);
	return ret;
    }

    /*
     * link JSON parse tree children to this parent node
     */
    name->parent = ret;
    value->parent = ret;

    /*
     * link name and value
     */
    item->name = name;
    item->value = value;
    item->converted = true;
    item->parsed = true;

    /*
     * copy convenience values related to name
     */
    item2 = &(name->item.string);
    item->name_as_str = item2->as_str;
    /* paranoia */
    if (item->name_as_str == NULL) {
	err(195, __func__, "item->name_as_str is NULL");
	not_reached();
    }
    item->name_str = item2->str;
    /* paranoia */
    if (item->name_str == NULL) {
	err(196, __func__, "item->name_str is NULL");
	not_reached();
    }
    item->name_as_str_len = item2->as_str_len;
    item->name_str_len = item2->str_len;

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_create_object - allocate a JSON object
 *
 * JSON object is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON object, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * returns:
 *	allocated JSON parser tree node converted JSON object
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_create_object(void)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_object *item = NULL;	    /* allocated JSON member */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_OBJECT);
    if (ret == NULL) {
	errp(197, __func__, "json_alloc(JTYPE_OBJECT) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON object
     */
    item = &(ret->item.object);
    item->converted = false;
    item->parsed = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON objects
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(198, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;
    item->parsed = true;

    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_object_add_member - add a JSON member to a JSON object
 *
 * JSON object is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON object, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * given:
 *	node	JSON node of the JSON object being added to
 *	member	JSON node of the JSON member to add
 *
 * returns:
 *	JSON parser tree as a JSON member
 *
 * NOTE: This function will not return given NULL pointers, or
 *	 if node is not type JTYPE_MEMBER, or
 *	 if member is type bot type JTYPE_MEMBER.
 */
struct json *
json_object_add_member(struct json *node, struct json *member)
{
    struct json_object *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */

    /*
     * firewall
     */
    if (node == NULL) {
	err(199, __func__, "node is NULL");
	not_reached();
    }
    if (member == NULL) {
	err(200, __func__, "member is NULL");
	not_reached();
    }
    if (node->type != JTYPE_OBJECT) {
	err(201, __func__, "object node type expected to be JTYPE_OBJECT: %d found type: %d",
		           JTYPE_OBJECT, node->type);
	not_reached();
    }
    if (member->type != JTYPE_MEMBER) {
	err(202, __func__, "object member type expected to be JTYPE_MEMBER: %d found type: %d",
		           JTYPE_MEMBER, node->type);
	not_reached();
    }

    /*
     * point to object
     */
    item = &(node->item.object);
    if (item->s == NULL) {
	err(203, __func__, "item->s is NULL");
	not_reached();
    }

    /*
     * link JSON parse tree child to this parent node
     */
    member->parent = node;

    /*
     * append member
     */
    moved = dyn_array_append_value(item->s, &member);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s(): dyn_array_append_value moved data",
		      __func__);
    }

    /*
     * update accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);

    json_dbg(JSON_DBG_VHIGH, __func__, "JSON object node type: %s", json_item_type_name(node));
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON object member type: %s", json_item_type_name(member));

    return node;
}


/*
 * json_create_elements - allocate an empty JSON elements
 *
 * A JSON elements is a set of JSON values/
 *
 * The pointer to the i-th JSON value in the JSON array, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * returns:
 *	allocated JSON parser tree as a JSON elements
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_create_elements(void)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_array *item = NULL;	    /* allocated JSON member */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_ELEMENTS);
    if (ret == NULL) {
	errp(204, __func__, "json_alloc(JTYPE_ELEMENTS) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON elements
     */
    item = &(ret->item.array);
    item->converted = false;
    item->parsed = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON values
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(205, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;
    item->parsed = true;

    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_elements_add_value - add a JSON value to a JSON elements item
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON array, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * given:
 *	node	JSON node of the JSON elements being added to
 *	value	JSON node of the JSON value to add
 *
 * returns:
 *	JSON parser tree as a JSON elements
 *
 * NOTE: This function will not return given NULL pointers, or
 *	 if node is not type JTYPE_ELEMENTS, or
 *	 if value is type is invalid.
 */
struct json *
json_elements_add_value(struct json *node, struct json *value)
{
    struct json_elements *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */

    /*
     * firewall
     */
    if (node == NULL) {
	err(206, __func__, "node is NULL");
	not_reached();
    }
    if (value == NULL) {
	err(207, __func__, "value is NULL");
	not_reached();
    }
    if (node->type != JTYPE_ELEMENTS) {
	err(208, __func__, "node type expected to be JTYPE_ELEMENTS: %d found type: %d",
			   JTYPE_ELEMENTS, node->type);
	not_reached();
    }
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON item type: %s", json_item_type_name(node));
    switch (value->type) {
    case JTYPE_NUMBER:
    case JTYPE_STRING:
    case JTYPE_BOOL:
    case JTYPE_NULL:
    case JTYPE_MEMBER:
    case JTYPE_OBJECT:
    case JTYPE_ARRAY:
	json_dbg(JSON_DBG_VHIGH, __func__, "JSON item type: %s", json_item_type_name(value));
	break;
    case JTYPE_ELEMENTS:
	err(209, __func__, "JSON type %s (type: %d) is invalid here",
		json_item_type_name(node), JTYPE_ELEMENTS);
	not_reached();
    default:
	err(210, __func__, "expected JSON item, array, string, number, boolean or null, found type: %d",
			   value->type);
	not_reached();
    }

    /*
     * point to array
     */
    item = &(node->item.elements);
    if (item->s == NULL) {
	err(211, __func__, "item->s is NULL");
	not_reached();
    }

    /*
     * link JSON parse tree child to this parent node
     */
    value->parent = node;

    /*
     * append value
     */
    moved = dyn_array_append_value(item->s, &value);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s(): dyn_array_append_value moved data",
		      __func__);
    }

    /*
     * update accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return node;
}


/*
 * json_create_array - allocate a JSON array
 *
 * A JSON array is of the form:
 *
 *      [ ]
 *      [ values ]
 *
 * The pointer to the i-th JSON value in the JSON array, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * returns:
 *	allocated JSON parser tree node converted JSON array
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_create_array(void)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_array *item = NULL;	    /* allocated JSON member */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_ARRAY);
    if (ret == NULL) {
	errp(212, __func__, "json_alloc(JTYPE_ARRAY) returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON array
     */
    item = &(ret->item.array);
    item->converted = false;
    item->parsed = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON arrays
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(213, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;
    item->parsed = true;

    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}
