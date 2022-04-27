/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json - JSON functions supporting mkiocccentry code
 *
 * JSON related functions to support formation of .info.json files
 * and .author.json files, their related check tools, test code,
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


#if !defined(INCLUDE_JSON_H)
#    define  INCLUDE_JSON_H


#include <time.h>
#include <stdint.h>


/*
 * JSON parser related structures
 */


/*
 * JSON encoding of an octet in a JSON string
 *
 * XXX - this table assumes we process on a byte basis - XXX
 * XXX - consider an approach that allowed for smaller UTF-8 non-ASCII encoding - XXX
 */
struct encode {
    const u_int8_t byte;    /* 8 bit character to encode */
    const size_t len;	    /* length of encoding */
    const char * const enc; /* JSON encoding of val */
};


/*
 * parsed JSON integer
 *
 * NOTE: The as_str is normally the same as the string that was passed to, say, the
 *	 json_conv_number() function.  It can differ in a few ways.  The end of the
 *	 string passed to json_conv_number(ptr, len) does not need to be NUL terminated,
 *	 whereas as_str will be NUL terminated at the end of the string.
 *	 If the characters pointed at by str start with whitespace or have trailing
 *	 whitespace, then as_str will hove those characters trimmed off.
 *	 Normally the bison / flex code that would call json_conv_number(ptr, len)
 *	 will ONLY have the JSON integer string, we note this in case some other
 *	 future code that is not a careful calls json_conv_number(str, len).
 */
struct json_number
{
    char *as_str;		/* allocated JSON integer string, whitespace trimmed if needed */

    size_t orig_len;		/* length of original JSON floating point string */
    size_t as_str_len;		/* length of as_str */

    bool converted;		/* true ==> able to convert JSON integer string to some form of C integer */
    bool is_negative;		/* true ==> value < 0 */

    bool is_floating;		/* true ==> as_str had a . in it */
    bool is_e_notation;		/* true ==> e notation used */

    /* integer values */

    bool int8_sized;		/* true ==> converted JSON integer to C int8_t */
    int8_t as_int8;		/* JSON integer value in int8_t form, if int8_sized == true */

    bool uint8_sized;		/* true ==> converted JSON integer to C uint8_t */
    uint8_t as_uint8;		/* JSON integer value in uint8_t form, if uint8_sized == true */

    bool int16_sized;		/* true ==> converted JSON integer to C int16_t */
    int16_t as_int16;		/* JSON integer value in int16_t form, if int16_sized == true */

    bool uint16_sized;		/* true ==> converted JSON integer to C uint16_t */
    uint16_t as_uint16;		/* JSON integer value in uint16_t form, if uint16_sized == true */

    bool int32_sized;		/* true ==> converted JSON integer to C int32_t */
    int32_t as_int32;		/* JSON integer value in int32_t form, if int32_sized == true */

    bool uint32_sized;		/* true ==> converted JSON integer to C uint32_t */
    uint32_t as_uint32;		/* JSON integer value in uint32_t form, if uint32_sized == true */

    bool int64_sized;		/* true ==> converted JSON integer to C int64_t */
    int64_t as_int64;		/* JSON integer value in int64_t form, if int64_sized == true */

    bool uint64_sized;		/* true ==> converted JSON integer to C uint64_t */
    uint64_t as_uint64;		/* JSON integer value in uint64_t form, if uint64_sized == true */

    bool int_sized;		/* true ==> converted JSON integer to C int */
    int as_int;			/* JSON integer value in int form, if int_sized == true */

    bool uint_sized;		/* true ==> converted JSON integer to C unsigned int */
    unsigned int as_uint;	/* JSON integer value in unsigned int form, if uint_sized == true */

    bool long_sized;		/* true ==> converted JSON integer to C long */
    long as_long;		/* JSON integer value in long form, if long_sized == true */

    bool ulong_sized;		/* true ==> converted JSON integer to C unsigned long */
    unsigned long as_ulong;	/* JSON integer value in unsigned long form, if long_sized == true */

    bool longlong_sized;	/* true ==> converted JSON integer to C long long */
    long long as_longlong;	/* JSON integer value in long long form, if longlong_sized longlong_sized == true */

    bool ulonglong_sized;	/* true ==> converted JSON integer to C unsigned long long */
    unsigned long long as_ulonglong;	/* JSON integer value in unsigned long long form, if ulonglong_sized a== true */

    bool ssize_sized;		/* true ==> converted JSON integer to C ssize_t */
    ssize_t as_ssize;		/* JSON integer value in ssize_t form, if ssize_sized == true */

    bool size_sized;		/* true ==> converted JSON integer to C size_t */
    size_t as_size;		/* JSON integer value in size_t form, if size_sized == true */

    bool off_sized;		/* true ==> converted JSON integer to C off_t */
    off_t as_off;		/* JSON integer value in off_t form, if off_sized == true */

    bool maxint_sized;		/* true ==> converted JSON integer to C maxint_t */
    intmax_t as_maxint;		/* JSON integer value in as_maxint form, if maxint_sized == true */

    bool umaxint_sized;		/* true ==> converted JSON integer to C umaxint_t */
    uintmax_t as_umaxint;	/* JSON integer value in as_umaxint form, if umaxint_sized == true */

    /* floating point values */

    bool float_sized;		/* true ==> converted JSON float to C float */
    float as_float;		/* JSON floating point value in float form, if float_sized  == true */
    bool as_float_int;		/* if float_sized == true, true ==> as_float is an integer */

    bool double_sized;		/* true ==> converted JSON float to C double */
    double as_double;		/* JSON floating point value in double form, if double_sized  == true */
    bool as_double_int;		/* if double_sized == true, true ==> as_double is an integer */

    bool longdouble_sized;	/* true ==> converted JSON float to C long double */
    long double as_longdouble;	/* JSON floating point value in long double form, if longdouble_sized  == true */
    bool as_longdouble_int;	/* if longdouble_sized == true, true ==> as_longdouble is an integer */
};


/*
 * parsed JSON string
 */
struct json_string
{
    char *as_str;		/* allocated non-decoded JSON string, NUL terminated (perhaps sans JSON "s) */
    char *str;			/* allocated decoded JSON string, NUL terminated */

    size_t as_str_len;		/* length of as_str, not including final NUL */
    size_t str_len;		/* length of str, not including final NUL */

    bool converted;		/* true ==> able to decode JSON string, false ==> str is invalid or not decoded */
    bool quote;			/* The original JSON string included surrounding "'s */

    bool same;			/* true => as_str same as str, JSON decoding not required */
    bool has_nul;		/* true ==> decoded JSON string has a NUL byte inside it */

    bool slash;			/* true ==> / was found after decoding */
    bool posix_safe;		/* true ==> all chars are POSIX portable safe plus + and maybe / after decoding */
    bool first_alphanum;	/* true ==> first char is alphanumeric after decoding */
    bool upper;			/* true ==> UPPER case chars found after decoding */
};


/*
 * parsed JSON boolean
 */
struct json_boolean
{
    char *as_str;		/* allocated JSON floating point string, whitespace trimmed if needed */
    size_t as_str_len;		/* length of as_str */

    bool converted;		/* true ==> able to decode JSON boolean, false ==> as_str is invalid or not decoded */
    bool value;			/* converted JSON boolean value */
};


/*
 * parsed JSON null
 */
struct json_null
{
    char *as_str;		/* allocated JSON floating point string, whitespace trimmed if needed */
    size_t as_str_len;		/* length of as_str */

    bool converted;		/* true ==> able to decode JSON null, false ==> as_str is invalid or not decoded */
    void *value;		/* NULL */
};


/*
 * JSON member
 *
 * A JSON member is of the form:
 *
 *	name : value
 */
struct json_member
{
    struct json *name;		/* JSON string name */
    struct json *value;		/* JSON value */
};


/*
 * JSON object
 *
 * JSON object is one of:
 *
 *	{ }
 *	{ members }
 */
struct json_object
{
    struct json *head;		/* first value in the members list, or NULL ==> empty list */
};


/*
 * JSON ordered array of values
 *
 * A JSON array is of the form:
 *
 *	[ ]
 *	[ values ]
 *
 * The i-th value in the array, if i < len, is:
 *
 *	foo.value[i-1]
 *
 * That value is a link back to this array object:
 *
 *	foo.value[i-1].parent == foo
 */
struct json_array
{
    int len;			/* number of JSON values in the array, 0 ==> empty array */

    struct json *value;		/* array of JSON values, NULL ==> empty array */
};


/*
 * element_type - JSON element type - an enum for each union element member in struct json
 */
enum element_type {
    JTYPE_EOT	    = -1,   /* special end of the table value */
    JTYPE_UNSET	    = 0,    /* JSON element has not been set - must be the value 0 */
    JTYPE_NUMBER,	    /* JSON element is an number - see struct json_integer */
    JTYPE_STRING,	    /* JSON element is a string - see struct json_string */
    JTYPE_BOOL,		    /* JSON element is a boolean - see struct json_boolean */
    JTYPE_NULL,		    /* JSON element is a null - see struct json_null */
    JTYPE_MEMBER,	    /* JSON element is a member */
    JTYPE_OBJECT,	    /* JSON element is a { members } */
    JTYPE_ARRAY,	    /* JSON element is a [ elements ] */
};

/*
 * struct json - element for the JSON parse tree
 *
 * For the parse tree we have this struct and its associated union.
 */
struct json
{
    enum element_type type;		/* union element specifier */
    union json_union {
	struct json_number number;	/* JTYPE_NUMBER - value is number (integer or floating point) */
	struct json_string string;	/* JTYPE_STRING - value is a string */
	struct json_boolean boolean;	/* JTYPE_BOOL - value is a JSON boolean */
	struct json_null null;		/* JTYPE_NULL - value is a JSON null value */
	struct json_member member;	/* JTYPE_MEMBER - value is a JSON member: name : value */
	struct json_object object;	/* JTYPE_OBJECT - value is a JSON { members } */
	struct json_array array;	/* JTYPE_ARRAY - value is a JSON [ elements ] */
    } element;

    /*
     * JSON parse tree double links
     *
     * NOTE: If a pointer is NULL then it means you have reached the end of the
     *	     linked list and/or at the end/top/bottom of the tree.
     */
    struct json *parent;	/* parent node in the JSON parse tree, or NULL if tree root or unlinked */
    struct json *prev;		/* previous in a JSON parse tree linked list, or NULL is link head or unlinked */
    struct json *next;		/* next in a JSON parse tree linked list, or NULL is link tail or unlinked */
};


/*
 * external data structures
 *
 * XXX - this table assumes we process on a byte basis - XXX
 * XXX - consider an approach that allowed for smaller UTF-8 non-ASCII encoding - XXX
 */
extern struct encode jenc[];

/*
 * external function declarations
 */
extern char *json_encode(char const *ptr, size_t len, size_t *retlen);
extern char *json_encode_str(char const *str, size_t *retlen);
extern void jencchk(void);
extern char *json_decode(char const *ptr, size_t len, size_t *retlen);
extern char *json_decode_str(char const *str, size_t *retlen);
/* JSON conversion functions */
extern void json_conv_free(struct json *node);
extern struct json *json_conv_number(char const *ptr, size_t len);
extern struct json *json_conv_number_str(char const *str, size_t *retlen);
extern struct json *json_conv_string(char const *ptr, size_t len, bool quote);
extern struct json *json_conv_string_str(char const *str, size_t *retlen, bool quote);
extern struct json *json_conv_bool(char const *ptr, size_t len);
extern struct json *json_conv_bool_str(char const *str, size_t *retlen);
extern struct json *json_conv_null(char const *ptr, size_t len);
extern struct json *json_conv_null_str(char const *str, size_t *retlen);


#endif /* INCLUDE_JSON_H */
