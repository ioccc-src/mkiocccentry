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
 * the next five are for the json_filename() function
 */
#define INFO_JSON	(0)	    /* file is assumed to be a .info.json file */
#define AUTHOR_JSON	(1)	    /* file is assumed to be a .author.json file */
#define INFO_JSON_FILENAME ".info.json"
#define AUTHOR_JSON_FILENAME ".author.json"
#define INVALID_JSON_FILENAME "null"

#define FORMED_UTC_FMT "%a %b %d %H:%M:%S %Y UTC"   /* format of strptime() for formed_UTC check */

/*
 * JSON warn/error code struct, variables, constants and macros for the jwarn()
 * and jerr() functions. Functions are prototyped later in the file.
 */
extern bool show_full_json_warnings;

/*
 * JSON warn / error codes
 *
 * Codes 0 - 199 are reserved for special purposes so all normal codes should be
 * >= JSON_CODE_MIN && <= JSON_CODE_MAX via the JSON_CODE macro.
 *
 * NOTE: The reason 200 codes are reserved is because it's more than enough ever
 * and we don't want to have to ever change the codes after the parser and
 * checkers are complete as this would cause problems for the tools as well as
 * the test suites. Previously the range was 0 - 99 and although this is also
 * probably more than enough we want to be sure that there is never a problem
 * and we cannot imagine how 200 codes will ever not be a large enough range but
 * if the use of reserved codes change this might not be the case for just 100
 * (unlikely though that is).
 */
/* reserved code: all normal codes should be >= JSON_CODE_MIN && <= JSON_CODE_MAX via JSON_CODE macro */
#define JSON_CODE_RESERVED_MIN (0)
/* reserved code: all normal codes should be >= JSON_CODE_MIN && <= JSON_CODE_MAX via JSON_CODE macro */
#define JSON_CODE_RESERVED_MAX (199)
/* based on minimum reserved code, form an invalid json code number */
#define JSON_CODE_INVALID (JSON_CODE_RESERVED_MIN - 1)
/*
 * The minimum code for jwarn() is the JSON_CODE_RESERVED_MAX (currently 199) + 1.
 * However this does not mean that calls to jwarn() cannot use <= the
 * JSON_CODE_RESERVED_MAX: it's just for special purposes e.g. codes that are
 * used in more than one location.
 */
#define JSON_CODE_MIN (1+JSON_CODE_RESERVED_MAX)
/* The maximum json code for jwarn()/jerr(). This was arbitrarily selected. */
#define JSON_CODE_MAX (9999)
/* the number of unreserved JSON codes for jwarn()/jerr(): the max - the min + 1 */
#define NUM_UNRESERVED_JSON_CODES (JSON_CODE_MAX-JSON_CODE_MIN)
/*
 * To distinguish that a number is a JSON warn/error code rather than any other
 * type of number we use these macros.
 */
#define JSON_CODE(x) ((x)+JSON_CODE_RESERVED_MAX)
/* reserved JSON code */
#define JSON_CODE_RESERVED(x) (x)

/*
 * JSON error codes to ignore
 *
 * When a tool is given command line arguments of the form:
 *
 *	.. -W 123 -W 1345 -W 56 ...
 *
 * this means the tool will ignore {JSON-0123}, {JSON-1345} and {JSON-0056}.
 * The ignore_json_code_set[] table holds the JSON codes to ignore.
 */
#define JSON_CODE_IGNORE_CHUNK (64)	/* number of codes to calloc or realloc at a time */

struct ignore_json_code {
    int next_free;	/* the index of the next allowed but free JSON error code */
    int alloc;		/* number of JSON error codes allocated */
    int *code;		/* pointer to the allocated list of codes, or NULL (not allocated) */
};
extern struct ignore_json_code *ignore_json_code_set;

/*
 * JSON parser related structs
 */

/*
 * parsed JSON integer
 *
 * NOTE: The as_str is normally the same as the string that was passed to, say, the
 *	 malloc_json_conv_int() function.  It can differ in a few ways.  The end of the
 *	 string passed to malloc_json_conv_int(str, len) does not need to be NUL terminated,
 *	 whereas as_str will be NUL terminated at the end of the string.
 *	 If the characters pointed at by str start with whitespace or have trailing
 *	 whitespace, then as_str will hove those characters trimmed off.
 *	 Normally the bison / flex code that would call malloc_json_conv_int(str, len)
 *	 will ONLY have the JSON integer string, we note this in case some other
 *	 future code that is not a careful calls malloc_json_conv_int(str, len).
 */
struct json_integer {
    char *as_str;		/* malloced JSON integer string, whitespace trimmed if needed */

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
 *	 malloc_json_conv_float() function.  It can differ in a few ways.  The end of the
 *	 string passed to malloc_json_conv_float(str, len) does not need to be NUL terminated,
 *	 whereas as_str will be NUL terminated at the end of the string.
 *	 If the characters pointed at by str start with whitespace or have trailing
 *	 whitespace, then as_str will have those characters trimmed off.
 *	 Normally the bison / flex code that would call malloc_json_conv_float(str, len)
 *	 will ONLY have the JSON integer string, we note this in case some other
 *	 future code that is not a careful calls malloc_json_conv_float(str, len).
 */
struct json_floating {
    char *as_str;		/* malloced JSON floating point string, whitespace trimmed if needed */

    size_t orig_len;		/* length of original JSON floating point string */
    size_t as_str_len;		/* length of as_str */

    bool converted;		/* true ==> able to convert JSON floating point string to some form of C floating point */
    bool is_negative;		/* true ==> value < 0 */

    bool float_sized;		/* true ==> converted JSON float to C float */
    float as_float;		/* JSON floating point value in float form, if float_sized  == true */

    bool double_sized;		/* true ==> converted JSON float to C double */
    double as_double;		/* JSON floating point value in double form, if double_sized  == true */

    bool longdouble_sized;	/* true ==> converted JSON float to C long double */
    long double as_longdouble;	/* JSON floating point value in long double form, if longdouble_sized  == true */
};


/*
 * parsed JSON string
 */
struct json_string {
    char *as_str;		/* malloced non-decoded JSON string, NUL terminated */
    char *str;			/* malloced decoded JSON string, NUL terminated */

    size_t str_len;		/* length of str, not including final NUL */
    size_t as_str_len;		/* length of as_str, not including final NUL */

    bool converted;		/* true ==> able to decode JSON string, false ==> str is invalid or not decoded */
    bool same;			/* true => original JSON string same as decoded string, no decoding required */

    bool has_nul;		/* true ==> decoded JSON string has a NUL byte inside it */
    bool posix_plus;		/* true => decoded JSON string has only POSIX portable safe plus + chars */
};


/*
 * parsed JSON boolean
 */
struct json_boolean {
    char *as_str;		/* malloced JSON floating point string, whitespace trimmed if needed */

    bool converted;		/* true ==> able to decode JSON boolean, false ==> as_str is invalid or not decoded */
    bool value;			/* converted JSON boolean value */
};


/*
 * parsed JSON null
 */
struct json_null {
    char *as_str;		/* malloced JSON floating point string, whitespace trimmed if needed */

    bool converted;		/* true ==> able to decode JSON null, false ==> as_str is invalid or not decoded */
};


/*
 * JSON object
 *
 * JSON object is one of:
 *
 *	{ }
 *	{ members }
 */
struct json_object {
    struct json *head;		/* first value in the members list, or NULL ==> empty list */
};


/*
 * JSON member
 *
 * A JSON member is of the form:
 *
 *	name : value
 */
struct json_member {
    struct json *name;		/* JSON string name */
    struct json *value;		/* JSON value */
};


/*
 * JSON ordered array of values
 *
 * A JSON arras is of the form:
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
 (	foo.value[i-1].parent == foo
 */
struct json_array {
    int len;			/* number of JSON values in the array, 0 ==> empty array */

    struct json *value;		/* array of JSON values, NULL ==> empty array */
};


/*
 * element_type - JSON element type - an enum for each union element member in struct json
 */
enum element_type {
    JTYPE_EOT = -1,		/* special end of the table value */
    JTYPE_UNSET = 0,		/* JSON element has not been set */
    JTYPE_INT,			/* JSON element is an integer - see struct json_integer */
    JTYPE_FLOAT,		/* JSON element is a float - see struct json_floating */
    JTYPE_STRING,		/* JSON element is a string - see struct json_string */
    JTYPE_BOOL,			/* JSON element is a boolean - see struct json_boolean */
    JTYPE_NULL,			/* JSON element is a null - see struct json_null */
    JTYPE_OBJECT,		/* JSON element is a { members } */
    JTYPE_MEMBER,		/* JSON element is a member */
    JTYPE_ARRAY,		/* JSON element is a [ elements ] */
};

/*
 * struct json - struct for the JSON parser tree
 *
 * For the parse tree we have this struct and its associated union. At the risk
 * of stating the obvious this is incomplete but it's a good start.
 */
struct json {
    enum element_type type;		/* union element specifier */
    union json_union {
	struct json_string string;	/* JTYPE_INT - value is a string */
	struct json_integer integer;	/* JTYPE_FLOAT - value is either a signed or unsigned integer */
	struct json_floating floating;	/* JTYPE_STRING - value is a floating point */
	struct json_boolean boolean;	/* JTYPE_BOOL - value is a JSON boolean */
	struct json_null null;		/* JTYPE_NULL - value is a JSON null value */
	struct json_object object;	/* JTYPE_OBJECT - value is a JSON { members } */
	struct json_member member;	/* JTYPE_MEMBER - value is a JSON member: name : value */
	struct json_array array;	/* JTYPE_ARRAY - value is a JSON [ elements ] */
    } element;

    struct json *parent;	/* parent JSON parse tree member, or NULL if tree root */

    /*
     * If this is part of a element list, then these point forward/backward.
     * Element lists are NULL terminated.  The 1st member of the list will have
     * the prev pointing back to a struct json pf element_type JTYPE_OBJECT.
     *
     * If this is NOT of a element list, then both prev and next will be NULL.
     */
    struct json *prev;		/* previous JSON list member if part of elements list */
    struct json *next;		/* next JSON list member if part of elements list */
};

/*
 * XXX - this probably should be replaced with enum element_type - XXX
 *
 * defines of the JSON types for the json fields tables.
 *
 * NOTE: As the parser is built these might be removed entirely: the parser
 * already has very similarly named tokens and the enum above does as well so
 * this quite possibly will happen. However because the current version of
 * jinfochk and jauthchk rely on the tables (based these constants and the
 * structs json_field and json_value) and because we won't want to cause make
 * test to fail we have them in here. Another possibility is to comment out the
 * execution of jinfochk and jauthchk so that we can get rid of these but I'd
 * rather keep these and the structs json_value and json_field and the
 * associated tables as well as the checks in json.c, jinfochk.c and jauthchk.c
 * because I hope to make use of them once the parser is complete; see above for
 * how I'm currently thinking about it (it's 9 April 2022).
 */
#define JSON_NUM	    (0)	    /* json field is supposed to be a number */
#define JSON_BOOL	    (1)	    /* json field is supposed to be a boolean */
#define JSON_CHARS	    (2)	    /* json field is supposed to be a string */
#define JSON_ARRAY	    (3)	    /* json field is supposed to be an array */
#define JSON_ARRAY_NUMBER   (5)	    /* json field is supposed to be a number in an array */
#define JSON_ARRAY_BOOL	    (6)	    /* json field is supposed to be a bool in an array (NB: not used) */
#define JSON_ARRAY_CHARS    (7)	    /* json field is supposed to be a string in an array */
#define JSON_EOT	    (-1)    /* json field is NULL (not null): used internally to mark end of the tables */

/*
 * JSON value: a linked list of all values of the same json_field (below).
 *
 * NOTE: As the parser is built this struct (and struct json_field below) might
 * be removed. This will depend upon how the parser builds the tree and this has
 * not been decided yet. Notice the TODO comment in the struct json above for
 * more information on this.
 */
struct json_value
{
    char *value;

    int line_num;	    /* the 'line number': actually the field number in the list */

    struct json_value *next;
};

/*
 * JSON field: a JSON field consists of the name and all the values (if more
 * than one field of the same name is found in the file).
 *
 * NOTE: As the parser is built this struct (and struct json_value above) might
 * be removed. This will depend upon how the parser builds the tree and this has
 * not been decided yet. Notice the TODO comment in the struct json above for
 * more information on this.
 */
struct json_field
{
    char *name;			/* field name */
    struct json_value *values;	/* linked list of values */

    /*
     * the below are for the tables: common_json_fields, info_json_fields and
     * author_json_fields:
     *
     * Number of times this field has been seen in the list, how many are
     * actually allowed and whether the field has been found: This is for the
     * tables that say many times a field has been seen, how many times it
     * is allowed and whether or not it's been seen (it is true that this could
     * simply be count > 0 but this is to be more clear, however slight).
     *
     * In other words this is done as part of the checks after the field:value
     * pairs have been extracted.
     *
     * NOTE: A max_count == 0 means that there's no limit and that it's not
     * required.
     */
    size_t count;		/* how many of this field in the list (or how many values) */
    size_t max_count;		/* how many of this field is allowed */
    bool found;			/* if this field was found */

    /*
     * These are used in both checking and parsing: checking that the data is
     * valid and parsing in that certain data types have to be parsed
     * differently.
     *
     * Data type: one of JSON_NUM, JSON_BOOL, JSON_CHARS or
     * JSON_ARRAY_ equivalents.
     */
    int field_type;
    /*
     * Some strings can be empty but others cannot; there are no other values
     * that can currently be empty but this is for all lists so we have to
     * include this bool
     */
    bool can_be_empty;	    /* if the value can be empty */


    /* NOTE: don't add to more than one list */
    struct json_field *next;	/* the next in the whatever list */
};

extern struct json_field common_json_fields[];
extern size_t SIZEOF_COMMON_JSON_FIELDS_TABLE;

/*
 * linked list of the common json fields found in the .info.json and
 * .author.json files.
 *
 * A common json field is a field that is supposed to be in both .info.json and
 * .author.json.
 */
struct json_field *found_common_json_fields;

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
struct author {
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
struct info {
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
extern char *malloc_json_encode(char const *ptr, size_t len, size_t *retlen);
extern char *malloc_json_encode_str(char const *str, size_t *retlen);
extern void jencchk(void);
extern bool json_putc(uint8_t const c, FILE *stream);
extern char *malloc_json_decode(char const *ptr, size_t len, size_t *retlen, bool strict);
extern char *malloc_json_decode_str(char const *str, size_t *retlen, bool strict);

/* jinfochk and jauthchk related */
extern struct json_field *find_json_field_in_table(struct json_field *table, char const *name, size_t *loc);
extern char const *json_filename(int type);
/* checks on the JSON fields tables */
extern void check_json_fields_tables(void);
extern void check_common_json_fields_table(void);
extern void check_author_json_fields_table(void);
extern void check_info_json_fields_table(void);
extern int check_first_json_char(char const *file, char *data, bool strict, char **first, char ch);
extern int check_last_json_char(char const *file, char *data, bool strict, char **last, char ch);
extern struct json_field *add_found_common_json_field(char const *json_filename, char const *name, char const *val, int line_num);
extern bool add_common_json_field(char const *program, char const *json_filename, char *name, char *val, int line_num);
extern int check_found_common_json_fields(char const *program, char const *json_filename, char const *fnamchk, bool test);
extern struct json_field *new_json_field(char const *json_filename, char const *name, char const *val, int line_num);
extern struct json_value *add_json_value(char const *json_filename, struct json_field *field, char const *val, int line_num);
extern void jwarn(int code, const char *program, char const *name, char const *filename, char const *line,
		  int line_num, const char *fmt, ...) \
	__attribute__((format(printf, 7, 8)));		/* 7=format 8=params */
extern void jwarnp(int code, const char *program, char const *name, char const *filename, char const *line,
		   int line_num, const char *fmt, ...) \
	__attribute__((format(printf, 7, 8)));		/* 7=format 8=params */
extern void jerr(int exitcode, char const *program, const char *name, char const *filename, char const *line,
		 int line_num, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 7, 8))); /* 7=format 8=params */
extern void jerrp(int exitcode, char const *program, const char *name, char const *filename, char const *line,
		  int line_num, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 7, 8))); /* 7=format 8=params */


/* free() functions */
extern void free_json_field_values(struct json_field *field);
extern void free_found_common_json_fields(void);
extern void free_json_field(struct json_field *field);
/* these free() functions are also used in mkiocccentry.c */
extern void free_info(struct info *infop);
extern void free_author_array(struct author *authorp, int author_count);
/* ignore code functions */
extern bool is_json_code_ignored(int code);
extern void ignore_json_code(int code);
/* JSON conversion functions */
extern struct json_integer *malloc_json_conv_int(char const *str, size_t len);
extern struct json_integer *malloc_json_conv_int_str(char const *str, size_t *retlen);
extern struct json_floating *malloc_json_conv_float(char const *str, size_t len);
extern struct json_floating *malloc_json_conv_float_str(char const *str, size_t *retlen);


#endif /* INCLUDE_JSON_H */
