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


#if !defined(INCLUDE_JSON_PARSE_H)
#    define  INCLUDE_JSON_PARSE_H


/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"


/*
 * definitions
 */
#define JSON_BYTE_VALUES (BYTE_VALUES) /* to make the purpose clearer we have the JSON_ prefix */


/*
 * convenience macros
 */
#define PARSED_JSON_NODE(item) ((item) != NULL && ((item)->parsed == true))
#define CONVERTED_PARSED_JSON_NODE(item) ((item) != NULL && (((item)->parsed == true) && ((item)->converted == true)))
#define CONVERTED_JSON_NODE(item) ((item) != NULL && (item)->converted == true)
#define VALID_JSON_NODE(item) ((item) != NULL && (((item)->parsed == true) || ((item)->converted == true)))


/*
 * JSON encoding of an octet in a JSON string
 *
 * NOTE: this table assumes we process on a byte basis.
 */
struct encode
{
    const u_int8_t byte;    /* 8 bit character to encode */
    const size_t len;	    /* length of encoding */
    const char * const enc; /* JSON encoding of val */
};


/*
 * parsed JSON number
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * If converted == false, then the JSON number string was not able to be
 * converted into a C numeric value (neither integer, nor floating point), OR
 * the JSON number string was invalid (malformed), or the JSON number string
 * was too large or otherwise could not be converted into a C numeric value
 * by the standard libc conversion functions.
 *
 * If the allocation of as_str fails, then as_str == NULL and parsed == false.
 *
 * The non-NULL as_str allocated will be NUL byte terminated.
 *
 * While the parser is not designed to do so, it is possible that as_str
 * may begin with whitespace and end with whitespace and NUL bytes.
 *
 * The first will point to the first non-whitespace character, where the
 * actual JSON number string starts.  The as_str_len will be the original
 * length argument passed to json_conv_number().  The number_len will be
 * the number of bytes, starting with first, that contain the actual
 * JSON number string.
 *
 * If is_floating == false, then the JSON number was attempted to be converted
 * as an integer.  In this case the "integer values" fields will be used and
 * the "floating point values" fields will be unused (set to false, or 0.0);
 *
 * If is_floating == true then, then the JSON number was attempted to be converted
 * as a floating point value. In this case the "floating point values" fields
 * will be used, and the "integer values" fields will be unused (set to false,
 * or 0).
 *
 * A JSON number string is of the form:
 *
 *	({JSON_INTEGER}|{JSON_INTEGER}{JSON_FRACTION}|{JSON_INTEGER}{JSON_FRACTION}{JSON_EXPONENT})
 *
 * where {JSON_INTEGER} is of the form:
 *
 *	-?([1-9][0-9]*|0)
 *
 * and where {JSON_FRACTION} is of the form:
 *
 *	\.[0-9]+
 *
 * and where {JSON_EXPONENT} is of the form:
 *
 *	[Ee][-+]?[0-9]+
 *
 * For more information see jparse.y and jparse.l.
 */
struct json_number
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to convert JSON number string to some form of C value */

    char *as_str;		/* allocated copy of the original allocated JSON number, NUL terminated */
    char *first;		/* first whitespace character */

    size_t as_str_len;		/* length of as_str */
    size_t number_len;		/* length of JSON number, w/o leading or trailing whitespace and NUL bytes */

    bool is_negative;		/* true ==> value < 0 */

    bool is_floating;		/* true ==> as_str had a '.' in it such as 1.234, false ==> no '.' found */
    bool is_e_notation;		/* true ==> e notation used such as 1e10, false ==> no e notation found */
    bool is_integer;		/* true ==> converted to some integer type below */

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
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * If the allocation of as_str fails, then as_str == NULL and parsed == false.
 *
 * The non-NULL as_str allocated will be NUL byte terminated.
 *
 * A JSON string is of the form:
 *
 *	"([^\n"]|\\")*"
 *
 * NOTE: We let the conversion function decide whether the string is actually
 * invalid according to the JSON standard so the regex above is for the parser
 * even if it allows things in the string that JSON does not allow.
 */
struct json_string
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to decode JSON string, false ==> str is invalid or not decoded */

    char *as_str;		/* allocated non-decoded JSON string, NUL terminated (perhaps sans JSON '"'s) */
    char *str;			/* allocated decoded JSON string, NUL terminated */

    size_t as_str_len;		/* length of as_str, not including final NUL */
    size_t str_len;		/* length of str, not including final NUL */

    bool quote;			/* The original JSON string included surrounding '"'s */

    bool same;			/* true => as_str same as str, JSON decoding not required */
    bool has_nul;		/* true ==> decoded JSON string has a NUL byte inside it */

    bool slash;			/* true ==> / was found after decoding */
    bool posix_safe;		/* true ==> all chars are POSIX portable safe plus + and maybe / after decoding */
    bool first_alphanum;	/* true ==> first char is alphanumeric after decoding */
    bool upper;			/* true ==> UPPER case chars found after decoding */
};


/*
 * parsed JSON boolean
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * A JSON boolean is of the form:
 *
 *	true
 *	false
 */
struct json_boolean
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to decode JSON boolean, false ==> as_str is invalid or not decoded */

    char *as_str;		/* allocated JSON boolean string, NUL terminated */
    size_t as_str_len;		/* length of as_str */

    bool value;			/* converted JSON boolean value */
};


/*
 * parsed JSON null
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * A JSON null is of the form:
 *
 *	null
 */
struct json_null
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to decode JSON null, false ==> as_str is invalid or not decoded */

    char *as_str;		/* allocated JSON null string, NUL terminated */
    size_t as_str_len;		/* length of as_str */

    void *value;		/* NULL */
};


/*
 * JSON member
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * A JSON member is of the form:
 *
 *	name : value
 *
 * where name is a JSON string of the form:
 *
 *	"([^\n"]|\\")*"
 *
 * and where value is any JSON value such as a:
 *
 *	JSON object
 *	JSON array
 *	JSON string
 *	JSON number
 *	JSON boolean
 *	JSON null
 *
 * These 4 items are copies of information from the JSON string name
 * and serve as a convenience for accessing JSON member name information.
 *
 * The name_as_str is a pointer copy of name->item.string.as_str pointer.
 * The name_str is a pointer copy of name->item.string.str pointer.
 * The name_as_str_len is a copy of name->item.string.as_str_len.
 * The name_str_len is a copy of name->item.string.str_len.
 */
struct json_member
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to decode JSON member */

    char *name_as_str;		/* name string as non-decoded JSON string - will not be NULL */
    char *name_str;		/* name string as decoded JSON string - will not be NULL */

    size_t name_as_str_len;	/* length of name_as_str, not including final NUL */
    size_t name_str_len;	/* length of name_str, not including final NUL */

    struct json *name;		/* JSON string name */
    struct json *value;		/* JSON value */
};


/*
 * JSON object
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * JSON object is one of:
 *
 *	{ }
 *	{ members }
 *
 * The pointer to the i-th JSON member in the JSON object, if i < len, is:
 *
 *	foo.set[i-1]
 */
struct json_object
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to decode JSON object */

    intmax_t len;		/* number of JSON members in the object, 0 ==> empty object */
    struct json **set;		/* set of JSON members belonging to the object */

    struct dyn_array *s;	/* dynamic array managed storage for the JSON object */
};


/*
 * JSON ordered array of values
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * A JSON array is of the form:
 *
 *	[ ]
 *	[ values ]
 *
 * The pointer to the i-th JSON value in the JSON array, if i < len, is:
 *
 *	foo.set[i-1]
 *
 * IMPORTANT: The struct json_array must be identical to struct json_elements because
 *	      parse_json_array() converts by just changing the JSON item type.
 */
struct json_array
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to decode JSON array */

    intmax_t len;		/* number of JSON values in the JSON array, 0 ==> empty array */
    struct json **set;		/* set of JSON values belonging to the JSON array */

    struct dyn_array *s;	/* dynamic array managed storage for the JSON array */
};


/*
 * JSON elements
 *
 * When parsed == false, then all other fields in this structure may be invalid.
 * So you must check the boolean of parsed and only use values if parsed == true.
 *
 * A JSON elements is zero or more JSON values.
 *
 * The pointer to the i-th JSON value in the JSON array, if i < len, is:
 *
 *	foo.set[i-1]
 *
 * IMPORTANT: The struct json_array must be identical to struct json_elements because
 *	      parse_json_array() converts by just changing the JSON item type.
 */
struct json_elements
{
    bool parsed;		/* true ==> able to parse correctly */
    bool converted;		/* true ==> able to decode JSON array */

    intmax_t len;		/* number of JSON values in the JSON elements, 0 ==> empty array */
    struct json **set;		/* set of JSON values belonging to the JSON elements */

    struct dyn_array *s;	/* dynamic array managed storage for the JSON array */
};


/*
 * item_type - JSON item type - an enum for each union item member in struct json
 */
enum item_type {
    JTYPE_UNSET	    = 0,    /* JSON item has not been set - must be the value 0 */
    JTYPE_NUMBER,	    /* JSON item is a number - see struct json_number */
    JTYPE_STRING,	    /* JSON item is a string - see struct json_string */
    JTYPE_BOOL,		    /* JSON item is a boolean - see struct json_boolean */
    JTYPE_NULL,		    /* JSON item is a null - see struct json_null */
    JTYPE_MEMBER,	    /* JSON item is a member */
    JTYPE_OBJECT,	    /* JSON item is a { members } */
    JTYPE_ARRAY,	    /* JSON item is a [ elements ] */
    JTYPE_ELEMENTS,	    /* JSON item for building a JSON array */
};

/*
 * struct json - item for the JSON parse tree
 *
 * For the parse tree we have this struct and its associated union.
 */
struct json
{
    enum item_type type;		/* union item specifier */
    union json_union {
	struct json_number number;	/* JTYPE_NUMBER - value is number (integer or floating point) */
	struct json_string string;	/* JTYPE_STRING - value is a string */
	struct json_boolean boolean;	/* JTYPE_BOOL - value is a JSON boolean */
	struct json_null null;		/* JTYPE_NULL - value is a JSON null value */
	struct json_member member;	/* JTYPE_MEMBER - value is a JSON member: name : value */
	struct json_object object;	/* JTYPE_OBJECT - value is a JSON { members } */
	struct json_array array;	/* JTYPE_ARRAY - value is a JSON [ elements ] */
	struct json_elements elements;	/* JTYPE_ELEMENTS - zero or more JSON values */
    } item;

    /*
     * JSON parse tree links
     */
    struct json *parent;	/* parent node in the JSON parse tree, or NULL if tree root or unlinked */
};


/*
 * global variables
 */

/*
 * external data structures
 *
 * NOTE: this table assumes we process on an 8-bit byte basis.
 */
extern struct encode jenc[];


/*
 * external function declarations
 */
extern char *json_encode(char const *ptr, size_t len, size_t *retlen, bool skip_quote);
extern char *json_encode_str(char const *str, size_t *retlen, bool skip_quote);
extern void jencchk(void);
extern char *json_decode(char const *ptr, size_t len, size_t *retlen, bool *has_nul);
extern char *json_decode_str(char const *str, size_t *retlen);
extern struct json *parse_json_string(char const *string, size_t len);
extern struct json *parse_json_bool(char const *string);
extern struct json *parse_json_null(char const *string);
extern struct json *parse_json_number(char const *string);
extern struct json *parse_json_array(struct json *elements);
extern struct json *parse_json_member(struct json *name, struct json *value);
extern struct json *json_alloc(enum item_type type);
extern struct json *json_conv_number(char const *ptr, size_t len);
extern struct json *json_conv_number_str(char const *str, size_t *retlen);
extern struct json *json_conv_string(char const *ptr, size_t len, bool quote);
extern struct json *json_conv_string_str(char const *str, size_t *retlen, bool quote);
extern struct json *json_conv_bool(char const *ptr, size_t len);
extern struct json *json_conv_bool_str(char const *str, size_t *retlen);
extern struct json *json_conv_null(char const *ptr, size_t len);
extern struct json *json_conv_null_str(char const *str, size_t *retlen);
extern struct json *json_conv_member(struct json *name, struct json *value);
extern struct json *json_create_object(void);
extern struct json *json_object_add_member(struct json *node, struct json *member);
extern struct json *json_create_elements(void);
extern struct json *json_elements_add_value(struct json *node, struct json *value);
extern struct json *json_create_array(void);


#endif /* INCLUDE_JSON_PARSE_H */
