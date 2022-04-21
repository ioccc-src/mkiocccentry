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
 * definitions
 */

/*
 * byte as octet constants
 */
#define BITS_IN_BYTE (8)	    /* assume 8 bit bytes */
#define MAX_BYTE (0xff)		    /* maximum byte value */
#define BYTE_VALUES (MAX_BYTE+1)    /* number of different combinations of bytes */

/*
 * JSON parser related structs
 */

/*
 * parsed JSON integer
 *
 * NOTE: The as_str is normally the same as the string that was passed to, say, the
 *	 json_conv_int() function.  It can differ in a few ways.  The end of the
 *	 string passed to json_conv_int(str, len) does not need to be NUL terminated,
 *	 whereas as_str will be NUL terminated at the end of the string.
 *	 If the characters pointed at by str start with whitespace or have trailing
 *	 whitespace, then as_str will hove those characters trimmed off.
 *	 Normally the bison / flex code that would call json_conv_int(str, len)
 *	 will ONLY have the JSON integer string, we note this in case some other
 *	 future code that is not a careful calls json_conv_int(str, len).
 */
struct json_integer
{
    char *as_str;		/* allocated JSON integer string, whitespace trimmed if needed */

    size_t orig_len;		/* length of original JSON integer string */
    size_t as_str_len;		/* length of as_str */

    bool converted;		/* true ==> able to convert JSON integer string to some form of C integer */
    bool is_negative;		/* true ==> value < 0 */

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
};


/*
 * parsed JSON floating point value
 *
 * NOTE: The as_str is normally the same as the string that was passed to, say, the
 *	 json_conv_float() function.  It can differ in a few ways.  The end of the
 *	 string passed to json_conv_float(str, len) does not need to be NUL terminated,
 *	 whereas as_str will be NUL terminated at the end of the string.
 *	 If the characters pointed at by str start with whitespace or have trailing
 *	 whitespace, then as_str will have those characters trimmed off.
 *	 Normally the bison / flex code that would call json_conv_float(str, len)
 *	 will ONLY have the JSON integer string, we note this in case some other
 *	 future code that is not a careful calls json_conv_float(str, len).
 */
struct json_floating
{
    char *as_str;		/* allocated JSON floating point string, whitespace trimmed if needed */

    size_t orig_len;		/* length of original JSON floating point string */
    size_t as_str_len;		/* length of as_str */

    bool converted;		/* true ==> able to convert JSON floating point string to some form of C floating point */
    bool is_negative;		/* true ==> value < 0 */
    bool is_e_notation;		/* true ==> e notation used */

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
    JSON_EOT	    = -1,   /* special end of the table value */
    JSON_UNSET	    = 0,    /* JSON element has not been set - must be the value 0 */
    JSON_INT,		    /* JSON element is an integer - see struct json_integer */
    JSON_FLOAT,		    /* JSON element is a float - see struct json_floating */
    JSON_STRING,	    /* JSON element is a string - see struct json_string */
    JSON_BOOL,		    /* JSON element is a boolean - see struct json_boolean */
    JSON_NULL,		    /* JSON element is a null - see struct json_null */
    JSON_OBJECT,	    /* JSON element is a { members } */
    JSON_MEMBER,	    /* JSON element is a member */
    JSON_ARRAY,		    /* JSON element is a [ elements ] */
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
	struct json_integer integer;	/* JSON_INT - value is either a signed or unsigned integer */
	struct json_floating floating;	/* JSON_FLOAT - value is a floating point */
	struct json_string string;	/* JSON_STRING - value is a string */
	struct json_boolean boolean;	/* JSON_BOOL - value is a JSON boolean */
	struct json_null null;		/* JSON_NULL - value is a JSON null value */
	struct json_object object;	/* JSON_OBJECT - value is a JSON { members } */
	struct json_member member;	/* JSON_MEMBER - value is a JSON member: name : value */
	struct json_array array;	/* JSON_ARRAY - value is a JSON [ elements ] */
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
 * common json fields - for use in mkiocccentry.
 *
 * NOTE: We don't use the json_field or json_value fields here because this
 * struct is for mkiocccentry which is in control of what's written whereas for
 * jinfochk and jauthchk we don't have control of what's in the file and we
 * can't prove that it wasn't made by hand or some other utility.
 */
struct json_common
{
    /*
     * version
     */
    char *mkiocccentry_ver;	/* mkiocccentry version (MKIOCCCENTRY_VERSION) */
    char const *iocccsize_ver;	/* iocccsize version (compiled in, same as iocccsize -V) */
    char const *jinfochk_ver;	/* jinfochk version (compiled in, same as jinfochk -V) */
    char const *jauthchk_ver;	/* jauthchk version (compiled in, same as jauthchk -V) */
    char const *fnamchk_ver;	/* fnamchk version (compiled in, same as fnamchk -V) */
    char const *txzchk_ver;	/* txzchk version (compiled in, same as txzchk -V) */
    /*
     * entry
     */
    char *ioccc_id;		/* IOCCC contest ID */
    int entry_num;		/* IOCCC entry number */
    char *tarball;		/* tarball filename */
    bool test_mode;		/* true ==> test mode entered */

    /*
     * time
     */
    time_t tstamp;		/* seconds since epoch when .info json was formed (see gettimeofday(2)) */
    int usec;			/* microseconds since the tstamp second */
    char *epoch;		/* epoch of tstamp, currently: Thu Jan 1 00:00:00 1970 UTC */
    char *utctime;		/* UTC converted string for tstamp (see strftime(3)) */
};

/*
 * author info
 */
struct author
{
    char *name;			/* name of the author */
    char *location_code;	/* author location/country code */
    char const *location_name;	/* name of author location/country */
    char *email;		/* Email address of author or or empty string ==> not provided */
    char *url;			/* home URL of author or or empty string ==> not provided */
    char *twitter;		/* author twitter handle or or empty string ==> not provided */
    char *github;		/* author GitHub username or or empty string ==> not provided */
    char *affiliation;		/* author affiliation or or empty string ==> not provided */
    bool past_winner;		/* true ==> author claims to have won before, false ==> author claims not a prev winner */
    bool default_handle;	/* true ==> default author_handle accepted, false ==> author_handle entered */
    char *author_handle;	/* IOCCC author handle (for winning entries) */
    int author_num;		/* author number */

    struct json_common common;	/* fields that are common to this struct author and struct info (below) */
};

/*
 * info for JSON
 *
 * Information we will collect in order to form the .info json file.
 */
struct info
{
    /*
     * entry
     */
    char *title;		/* entry title */
    char *abstract;		/* entry abstract */
    off_t rule_2a_size;		/* Rule 2a size of prog.c */
    size_t rule_2b_size;	/* Rule 2b size of prog.c */
    bool empty_override;	/* true ==> empty prog.c override requested */
    bool rule_2a_override;	/* true ==> Rule 2a override requested */
    bool rule_2a_mismatch;	/* true ==> file size != rule_count function size */
    bool rule_2b_override;	/* true ==> Rule 2b override requested */
    bool highbit_warning;	/* true ==> high bit character(s) detected */
    bool nul_warning;		/* true ==> NUL character(s) detected */
    bool trigraph_warning;	/* true ==> unknown or invalid trigraph(s) detected */
    bool wordbuf_warning;	/* true ==> word buffer overflow detected */
    bool ungetc_warning;	/* true ==> ungetc warning detected */
    bool Makefile_override;	/* true ==> Makefile rule override requested */
    bool first_rule_is_all;	/* true ==> Makefile first rule is all */
    bool found_all_rule;	/* true ==> Makefile has an all rule */
    bool found_clean_rule;	/* true ==> Makefile has clean rule */
    bool found_clobber_rule;	/* true ==> Makefile has a clobber rule */
    bool found_try_rule;	/* true ==> Makefile has a try rule */
    bool test_mode;		/* true ==> contest ID is test */
    /*
     * filenames
     */
    char *prog_c;		/* prog.c filename */
    char *Makefile;		/* Makefile filename */
    char *remarks_md;		/* remarks.md filename */
    int extra_count;		/* number of extra files */
    char **extra_file;		/* list of extra filenames followed by NULL */
    /*
     * time
     */

    struct json_common common;	/* fields that are common to this struct info and struct author (above) */
};

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
extern struct json *json_conv_int(char const *str, size_t len);
extern struct json *json_conv_int_str(char const *str, size_t *retlen);
extern struct json *json_conv_float(char const *str, size_t len);
extern struct json *json_conv_float_str(char const *str, size_t *retlen);
extern struct json *json_conv_string(char const *str, size_t len, bool quote);
extern struct json *json_conv_string_str(char const *str, size_t *retlen, bool quote);
extern struct json *json_conv_bool(char const *str, size_t len);
extern struct json *json_conv_bool_str(char const *str, size_t *retlen);
extern struct json *json_conv_null(char const *str, size_t len);
extern struct json *json_conv_null_str(char const *str, size_t *retlen);
/* functions to create json files */
extern bool json_putc(uint8_t const c, FILE *stream);
extern bool json_fprintf_str(FILE *stream, char const *str);
extern bool json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
				      char const *tail);
extern bool json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
				    char const *tail);
extern bool json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
				    char const *tail);

/* these general JSON free() functions are used in mkiocccentry */
extern void free_info(struct info *infop);
extern void free_author_array(struct author *authorp, int author_count);

#endif /* INCLUDE_JSON_H */
