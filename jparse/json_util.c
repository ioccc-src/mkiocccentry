/*
 * json_util - general JSON parser utility support functions
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


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h> /* for chdir(2) */
#include <fcntl.h>  /* for open(2) */

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/dbg.h"
#else
#include <dbg.h>
#endif

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"


/*
 * global variables
 */
int json_verbosity_level = JSON_DBG_NONE;	/* json debug level set by -J in jparse */

/*
 * hexval - convert ASCII character to hex value
 *
 * NOTE: -1 means the ASCII character is not a valid hex character
 */
int const hexval[JSON_BYTE_VALUES] = {
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
 * static declarations
 */
static void fprnumber(FILE *stream, char *prestr, struct json_number *item, char *midstr, char *poststr);



/*
 * json_dbg_allowed - determine if verbosity level allows for JSON debug messages are allowed
 *
 * given:
 *	json_dbg_lvl	write message if >= verbosity level
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
json_dbg_allowed(int json_dbg_lvl)
{
    /*
     * determine if verbosity level allows for JSON debug messages
     */
    if (dbg_output_allowed == true &&
        (json_dbg_lvl == JSON_DBG_FORCED || json_dbg_lvl <= json_verbosity_level)) {
	return true;
    }
    return false;
}


/*
 * json_warn_allowed - determine if a JSON warning message is allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
json_warn_allowed(void)
{
    /*
     * determine if a JSON warning message is allowed
     */
    if (warn_output_allowed == false || (msg_warn_silent == true && json_verbosity_level <= 0)) {
	return false;
    }
    return true;
}


/*
 * json_err_allowed - determine if fatal JSON error messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
json_err_allowed(void)
{
    /*
     * determine if fatal error messages are allowed
     */
    if (err_output_allowed == false) {
	return false;
    }
    return true;
}


/*
 * json_dbg - print JSON debug message if we are verbose enough
 *
 * This function behaves like dbg() in determining if a JSON debug message
 * is printed.  To print a JSON debug message, dbg_output_allowed == true AND
 * json_dbg_lvl <= json_verbosity_level or json_dbg_lvl == JSON_DBG_FORCED.
 * When dbg_output_allowed == false, no debug output will be printed,
 * even when json_dbg_lvl == JSON_DBG_FORCED.
 *
 * See also: json_dbg_tree_print().
 *
 * given:
 *	json_dbg_lvl	print message if JSON_DBG_FORCED OR if <= json_verbosity_level
 *	name		function name
 *	fmt		printf format
 *	...
 *
 * Example:
 *
 *	json_dbg(1, __func__, "foobar information: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function returns true if debug output is allowed; else it returns false.
 */
void
json_dbg(int json_dbg_lvl, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno = 0;	/* errno at function start */
    bool allowed = false;	/* assume debug output is not allowed */

    /*
     * determine if JSON debug messages are allowed
     */
    allowed = json_dbg_allowed(json_dbg_lvl);
    if (allowed == false) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "NULL name, forcing use of: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, " NULL fmt, forcing use of: %s", fmt);
    }

    /*
     * print the debug message if allowed and allowed by the verbosity level
     */
    json_vdbg(json_dbg_lvl, name, fmt, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;

    return;
}


/*
 * json_vdbg - print debug message if we are verbose enough
 *
 * This function behaves like vdbg() in determining if a JSON debug message
 * is printed.  To print a JSON debug message, dbg_output_allowed == true AND
 * json_dbg_lvl <= json_verbosity_level or json_dbg_lvl == JSON_DBG_FORCED.
 * When dbg_output_allowed == false, no debug output will be printed,
 * even when json_dbg_lvl == JSON_DBG_FORCED.
 *
 * See also: json_dbg_tree_print().
 *
 * given:
 *	json_dbg_lvl	print message if JSON_DBG_FORCED OR if <= json_verbosity_level
 *	name		function name
 *	ap		variable argument list
 *
 * Example:
 *
 *	json_vdbg(1, __func__, "foobar information: %d", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function returns true if debug output is allowed; else it returns false.
 */
void
json_vdbg(int json_dbg_lvl, char const *name, char const *fmt, va_list ap)
{
    int saved_errno = 0;	/* errno at function start */
    bool allowed = false;	/* assume debug message not allowed */
    int ret;			/* libc function return code */

    /*
     * determine if JSON debug messages are allowed
     */
    allowed = json_dbg_allowed(json_dbg_lvl);
    if (allowed == false) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "NULL name, forcing use of: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "NULL fmt, forcing use of: %s", fmt);
    }

    /*
     * print the debug message
     */
    errno = 0;
    ret = fprintf(stderr, "in %s(): JSON debug[%d]: ", name, json_dbg_lvl);
    if (chk_stdio_printf_err(stderr, ret)) {
	warn(__func__, "fprintf returned errno: %d: (%s)", errno, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (chk_stdio_printf_err(stderr, ret)) {
	warn(__func__, "vfprintf returned errno: %d: (%s)", errno, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	warn(__func__, "fputc returned errno: %d: (%s)", errno, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	warn(__func__, "fflush returned errno: %d: (%s)", errno, strerror(errno));
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
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
	warn(__func__, "called with NULL stream");
	return false;
    }

    /*
     * write JSON encoding to stream
     *
     * XXX - This is NOT the canonical way to encode Unicode characters! - XXX
     * XXX - Valid Unicode symbols when encoded as UTF-8 bytes should be - XXX
     * XXX - encoded as 1 or more consecutive \\u[0-9A-Fa-f]{4} strings! - XXX
     */
    errno = 0;	    /* pre-clear errno for warnp */
    ret = fprintf(stream, "%s", byte2asciistr[c].enc);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf #1 error");
	return false;
    }
    return true;
}


/*
 * json_fprintf_str - print a JSON string
 *
 * Print on stream:
 *
 * If str == NULL:
 *
 *	null
 *
 * else str != NULL:
 *
 *	str with JSON string encoding surrounded by '"'s
 *
 * See:
 *
 *	https://www.json.org/json-en.html
 *
 * given:
 *	stream	- open file stream to print on
 *	str	- the string to JSON encode or NULL
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
bool
json_fprintf_str(FILE *stream, char const *str)
{
    int ret;			/* libc function return */
    char const *p;

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL stream");
	return false;
    }

    /*
     * case: NULL
     */
    if (str == NULL) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(stream, "null");
	if (chk_stdio_printf_err(stream, ret)) {
	    warnp(__func__, "fprintf #0 error for null");
	    return false;
	}
	return true;
    }

    /*
     * print leading double-quote
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #0 error for leading double-quote");
	return false;
    }

    /*
     * print name, JSON encoded
     */
    for (p=str; *p != '\0'; ++p) {
	if (json_putc((uint8_t const)*p, stream) != true) {
	    warn(__func__, "json_putc #0 error");
	    return false;
	}
    }

    /*
     * print trailing double-quote
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #1 error for trailing double-quote");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_string - print name value (as a string) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded " middle "value_encoded" tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value string to JSON encode or NULL
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
bool
json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
			  char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON encoded string
     */
    if (json_fprintf_str(stream, value) != true) {
	warn(__func__, "json_fprintf_str for value as a string");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_long - print name value (as a long integer) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle long_value tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as long
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
bool
json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warnp(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON long
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%ld", value);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf for value as a long");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_time_t - print name value (as a time_t integer) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle time_t_value tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as time_t
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
bool
json_fprintf_value_time_t(FILE *stream, char const *lead, char const *name, char const *middle, time_t value,
			  char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warnp(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON time_t
     */
    errno = 0;			/* pre-clear errno for warnp() */
    if ((time_t)-1 > 0) {
	/* case: unsigned time_t */
	ret = fprintf(stream, "%ju", (uintmax_t)value);
    } else {
	/* case: signed time_t */
	ret = fprintf(stream, "%jd", (intmax_t)value);
    }
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf for value as a time_t");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_bool - print name value (as a boolean) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle true tail
 * or:
 *	lead "name_encoded" middle false tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as boolean
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
bool
json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON boolean
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", booltostr(value));
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf for value as a boolean");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (chk_stdio_printf_err(stream, ret)) {
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_type_name - return a struct json item union type name
 *
 * given:
 *	type		one of the values of the JTYPE_ enum
 *
 * returns:
 *	A constant (read-only) string that names the JTYPE_ enum.
 *
 * NOTE: This string returned is read only: It's not allocated on the stack.
 */
char const *
json_type_name(enum item_type type)
{
    char const *name = "JTYPE_UNSET";

    /*
     * determine type name based on type
     */
    switch (type) {
    case JTYPE_UNSET:
	name = "JTYPE_UNSET";
	break;
    case JTYPE_NUMBER:
	name = "JTYPE_NUMBER";
	break;
    case JTYPE_STRING:
	name = "JTYPE_STRING";
	break;
    case JTYPE_BOOL:
	name = "JTYPE_BOOL";
	break;
    case JTYPE_NULL:
	name = "JTYPE_NULL";
	break;
    case JTYPE_MEMBER:
	name = "JTYPE_MEMBER";
	break;
    case JTYPE_OBJECT:
	name = "JTYPE_OBJECT";
	break;
    case JTYPE_ARRAY:
	name = "JTYPE_ARRAY";
	break;
    case JTYPE_ELEMENTS:
	name = "JTYPE_ELEMENTS";
	break;
    default:
	name = "((JTYPE_UNKNOWN))";
	warn(__func__, "called with unknown JSON type: %d", type);
	break;
    }

    /* return read-only name constant string */
    return name;
}


/*
 * json_item_type_name - print a struct json item union type by name
 *
 * given:
 *	node	pointer to a JSON parser tree node to get the type name
 *
 * returns:
 *	A constant (read-only) string that names the JTYPE_ enum.
 *
 * NOTE: This string returned is read only: It's not allocated on the stack.
 */
char const *
json_item_type_name(const struct json *node)
{
    char const *name = "JTYPE_UNSET";

    /*
     * firewall
     */
    if (node == NULL) {
	name = "((node == NULL))";

    /*
     * determine type name based on type
     */
    } else {
	name = json_type_name(node->type);
    }

    /* return read-only name constant string */
    return name;
}

/*
 * json_get_type_str - print a struct json string (original match in scanner/parser)
 *
 * given:
 *	node	    pointer to a JSON parser tree node to get matching string from
 *	encoded	    true if we should return the encoded string
 *
 * returns:
 *	A constant (read-only) string that originally matched in the
 *	lexer/parser
 *
 * NOTE: This string returned is read only: It's not allocated on the stack.
 *
 * NOTE: this function can return a NULL pointer. It is the responsibility of
 * the caller to check for a NULL return value.
 */
char const *
json_get_type_str(struct json *node, bool encoded)
{
    char const *str = NULL;

    /*
     * firewall
     */
    if (node == NULL) {
	return NULL;
    }

    switch (node->type) {
	case JTYPE_NUMBER:
	    {
		struct json_number *item = &(node->item.number);
		if (item != NULL && VALID_JSON_NODE(item)) {
		    str = item->as_str;
		}
	    }
	    break;
	case JTYPE_STRING:
	    {
		struct json_string *item = &(node->item.string);

		if (item != NULL && CONVERTED_PARSED_JSON_NODE(item)) {
		    str = encoded ? item->as_str : item->str;
		}
	    }
	    break;
	case JTYPE_BOOL:
	    {
		struct json_boolean *item = &(node->item.boolean);

		if (item != NULL && CONVERTED_PARSED_JSON_NODE(item)) {
		    str = item->as_str;
		}
	    }
	    break;
	case JTYPE_NULL:
	    {
		struct json_null *item = &(node->item.null);

		if (item != NULL && CONVERTED_PARSED_JSON_NODE(item)) {
		    str = item->as_str;
		}
	    }
	    break;
	case JTYPE_MEMBER:
	    {
		struct json_member *item = &(node->item.member);

		if (item != NULL && CONVERTED_PARSED_JSON_NODE(item)) {
		    str = encoded ? item->name_as_str : item->name_str;
		}
	    }
	    break;
	case JTYPE_OBJECT:
	    break;
	case JTYPE_ARRAY:
	    break;
	case JTYPE_ELEMENTS:
	    break;
	default:
	    break;
    }

    /* return read-only name constant string */
    return str;
}


/*
 * open_json_dir_file - open a FILE * and parse as JSON
 *
 * Using open_dir_file(), attempt to open dir/filename (or if dir == NULL then
 * just filename), and if it can be opened for reading, parse it as JSON (or try
 * to) and return the tree (struct json *) if valid JSON, otherwise return NULL.
 * If open_dir_file() has an error that function will not return; if filename is
 * NULL here we will not return. If invalid JSON it is not an error but we do
 * warn about it.
 *
 * If dir == NULL, just try to open the file without a chdir.
 *
 * given:
 *	dir	directory into which we will temporarily chdir or
 *		    NULL ==> do not chdir
 *	file	path of readable file to open
 *
 * returns:
 *	a struct json *tree, if file is a valid JSON, otherwise NULL
 *
 * NOTE: if the file is invalid JSON then NULL will be returned but if the file
 *       cannot be opened it is an error.
 *
 * NOTE: it is the responsibility of the caller to free the JSON tree after they
 *       have used it.
 */
struct json *
open_json_dir_file(char const *dir, char const *filename)
{
    FILE *stream = NULL;	/* file stream to try and read as a JSON file */
    struct json *tree = NULL;   /* if we can open the file we will try and parse it as JSON */
    bool is_valid = false;      /* true ==> JSON is valid */

    /*
     * firewall
     */
    if (filename == NULL) {
	err(158, __func__, "called with NULL file");
	not_reached();
    }

    stream = open_dir_file(dir, filename);
    if (stream == NULL) {
        err(159, __func__, "open_dir_file(%s, %s) returned NULL", dir != NULL ? dir : "NULL", filename);
        not_reached();
    } else {
        /*
         * attempt to parse as JSON
         */
        tree = parse_json_stream(stream, filename, &is_valid);

        /*
         * the parse function should close or clearerr() the stream so we cannot
         * do that here
         */
        if (tree == NULL || !is_valid) {
            warn(__func__, "file: %s is not valid JSON", filename);
            return NULL;
        }
    }
    return tree;
}

/*
 * json_free - free storage of a single JSON parse tree node
 *
 * This function operates on a single JSON parse tree node.
 * See json_tree_free() for a function that frees an entire parse tree.
 *
 * given:
 *	node	pointer to a JSON parser tree node to free
 *	depth	current tree depth (0 ==> top of tree)
 *	...	extra args are ignored
 *
 * While the ... variable arg are ignored, we need to declare
 * then in order to be in vcallback form for use by json_tree_walk().
 *
 * This function will free the internals of a JSON parser tree node.
 * It is up to the caller to free the struct json if needed.
 *
 * This function does NOT walk the JSON parse tree, so it will
 * ignore links form this node to other JSON parse tree nodes.
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 */
void
json_free(struct json *node, unsigned int depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, depth);

    /*
     * call va_list free list function
     */
    vjson_free(node, depth, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * vjson_free - free storage of a single JSON parse tree node in va_list form
 *
 * This is a variable argument list interface to json_free(). See
 * json_tree_free() for a function that frees an entire parse tree.
 *
 * given:
 *	node	pointer to a JSON parser tree node to free
 *	depth	current tree depth (0 ==> top of tree)
 *	ap	variable argument list
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 */
void
vjson_free(struct json *node, unsigned int depth, va_list ap)
{
    UNUSED_ARG(depth);
    UNUSED_ARG(ap);

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * free internals based in node type
     */
    switch (node->type) {

    case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
	/* nothing to free */

	/* nothing internal to zeroize */
	break;

    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
	{
	    struct json_number *item = &(node->item.number);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_number));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
	{
	    struct json_string *item = &(node->item.string);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }
	    if (item->str != NULL) {
		free(item->str);
		item->str = NULL;
	    }

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_string));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
	{
	    struct json_boolean *item = &(node->item.boolean);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_boolean));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
	{
	    struct json_null *item = &(node->item.null);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_null));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    case JTYPE_MEMBER:	/* JSON item is a member - see struct json_member */
	{
	    struct json_member *item = &(node->item.member);

	    /* free internal storage */
	    item->name = NULL;
	    item->value = NULL;

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_member));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    case JTYPE_OBJECT:	/* JSON item is { members } - see struct json_object */
	{
	    struct json_object *item = &(node->item.object);

	    /* free internal storage */
	    if (item->s != NULL) {
		dyn_array_free(item->s);
		item->s = NULL;
		item->set = NULL;
		item->len = 0;
	    }

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_object));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] - see struct json_array */
	{
	    struct json_array *item = &(node->item.array);

	    /* free internal storage */
	    if (item->s != NULL) {
		dyn_array_free(item->s);
		item->s = NULL;
		item->set = NULL;
		item->len = 0;
	    }

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_array));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values - see struct json_elements */
	{
	    struct json_elements *item = &(node->item.elements);

	    /* free internal storage */
	    if (item->s != NULL) {
		dyn_array_free(item->s);
		item->s = NULL;
		item->set = NULL;
		item->len = 0;
	    }

	    /* zeroize internal item storage */
	    memset(item, 0, sizeof(struct json_elements));
	    item->converted = false;
	    item->parsed = false;
	}
	break;

    default:
	/* nothing we can free */
	warn(__func__, "node type is unknown: %d", node->type);
	break;
    }

    /*
     * reset the JSON node
     */
    node->type = JTYPE_UNSET;
    node->parent = NULL;
    return;
}


/*
 * json_tree_free - free storage of a JSON parse tree
 *
 * This function uses the json_tree_walk() interface to walk
 * the JSON parse tree and free all nodes under a given node.
 *
 * given:
 *	node	    pointer to a JSON parser tree node to free
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth.
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *
 * NOTE: This function will free the internals of a JSON parser tree node.
 *	 It is up to the caller to free the top level struct json if needed.
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 */
void
json_tree_free(struct json *node, unsigned int max_depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, max_depth);

    /*
     * free the JSON parse tree
     */
    vjson_tree_walk(node, max_depth, 0, true, vjson_free, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * json_fprint - print a line about a JSON parse tree node
 *
 * This function will print details about a JSON parse tree node,
 * as a single line, onto an open stream.
 *
 * If dbg_output_allowed == false, this function will not print.
 *
 * When json_dbg_lvl == JSON_DBG_FORCED, the printed prefix of:
 *
 *	JSON tree[%d]:
 *
 * is replaced with:
 *
 *	JSON tree node:
 *
 * given:
 *	node	pointer to a JSON parser tree node to print
 *	depth	current tree depth (0 ==> top of tree)
 *	...	extra args are ignored, required extra args:
 *				OR if <= json_verbosity_level
 *
 *		stream	    stream to print on
 *		json_dbg_lvl   print message if JSON_DBG_FORCED
 *			       OR if <= json_verbosity_level
 *
 * Example use - print a JSON parse tree
 *
 *	len = json_fprint(node, depth, NULL, JSON_DBG_MED);
 *	json_fprint(node, depth, stderr, JSON_DBG_FORCED;
 *	json_fprint(node, depth, stdout, JSON_DBG_MED);
 *
 * While the ... variable arg are ignored, we need to declare
 * then in order to be in vcallback form for use by json_tree_walk().
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if node == NULL.
 */
void
json_fprint(struct json *node, unsigned int depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, depth);

    /*
     * call va_list argument list function
     */
    vjson_fprint(node, depth, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * fprnumber - print information about a json_number on a stream
 *
 * given:
 *	stream	    open stream on which to print information about a json_number
 *	prestr	    first string to print
 *	info	    pointer to struct json_number for which to print in stream
 *	midstr	    third string to print
 *	poststr	    if non-NULL, fourth string to print, NULL ==> print "((NULL))"
 */
static void
fprnumber(FILE *stream, char *prestr, struct json_number *item, char *midstr, char *poststr)
{
    /*
     * firewall - must be -J 3 or more
     */
    if (json_verbosity_level < JSON_DBG_MED) {
	return;
    }
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return;
    }
    if (prestr == NULL) {
	warn(__func__, "prestr is NULL");
	return;
    }
    if (item == NULL) {
	warn(__func__, "item is NULL");
	return;
    }
    if (midstr == NULL) {
	warn(__func__, "midstr is NULL");
	return;
    }
    if (poststr == NULL) {
	poststr = "((NULL))";
    }

    /*
     * print the first prestr
     */
    fprint(stream, "%s", prestr);

    /*
     * print struct json_number information
     *
     * At -J 4 or higher, we print all struct json_number
     */
    if (json_verbosity_level > JSON_DBG_MED) {

	/* -J 4 and higher output */
	fprint(stream, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
			PARSED_JSON_NODE(item)?"p":"",
			CONVERTED_PARSED_JSON_NODE(item)?",":":",
			CONVERTED_PARSED_JSON_NODE(item)?"c:":"",
			item->is_negative?"-":"",
			item->is_floating?"F":"",
			item->is_e_notation?"E":"",
			item->is_integer?"I":"",
			item->int8_sized?"8":"",
			item->uint8_sized?"u8":"",
			item->int16_sized?"16":"",
			item->uint16_sized?"u16":"",
			item->int32_sized?"32":"",
			item->uint32_sized?"u32":"",
			item->int64_sized?"64":"",
			item->uint64_sized?"u64":"",
			item->int_sized?"i":"",
			item->uint_sized?"ui":"",
			item->long_sized?"l":"",
			item->ulong_sized?"ul":"",
			item->longlong_sized?"ll":"",
			item->ulonglong_sized?"ull":"",
			item->ssize_sized?"SS":"",
			item->size_sized?"s":"",
			item->off_sized?"o":"",
			item->maxint_sized?"m":"",
			item->umaxint_sized?"um":"",
			item->float_sized?"f":"",
			item->as_float_int?"fi":"",
			item->double_sized?"d":"",
			item->as_double_int?"di":"",
			item->longdouble_sized?"ld":"",
			item->as_longdouble_int?"ldi":"");
    } else {

	/* -J 3 */ fprint(stream, "%s%s%s%s%s%s%s",
			PARSED_JSON_NODE(item)?"p":"",
			CONVERTED_PARSED_JSON_NODE(item)?",":":",
			CONVERTED_PARSED_JSON_NODE(item)?"c:":"",
			item->is_negative?"-":"", item->is_floating?"F":"",
			item->is_e_notation?"E":"",
			item->is_integer?"I":"");
	if (item->int8_sized) {
	    fprint(stream, "%s", item->int8_sized?"8":"");
	} else if (item->uint8_sized) {
	    fprint(stream, "%s", item->uint8_sized?"u8":"");
	} else if (item->int16_sized) {
	    fprint(stream, "%s", item->int16_sized?"16":"");
	} else if (item->uint16_sized) {
	    fprint(stream, "%s", item->uint16_sized?"u16":"");
	} else if (item->int32_sized) {
	    fprint(stream, "%s", item->int32_sized?"32":"");
	} else if (item->uint32_sized) {
	    fprint(stream, "%s", item->uint32_sized?"u32":"");
	} else if (item->int64_sized) {
	    fprint(stream, "%s", item->int64_sized?"64":"");
	} else if (item->uint64_sized) {
	    fprint(stream, "%s", item->uint64_sized?"u64":"");
	} if (item->ssize_sized) {
	    fprint(stream, "%s", item->ssize_sized?"SS":"");
	} else if (item->size_sized) {
	    fprint(stream, "%s", item->size_sized?"s":"");
	} if (item->off_sized) {
	    fprint(stream, "%s", item->off_sized?"o":"");
	} if (item->maxint_sized) {
	    fprint(stream, "%s", item->maxint_sized?"m":"");
	} else if (item->umaxint_sized) {
	    fprint(stream, "%s", item->umaxint_sized?"um":"");
	} if (item->as_float_int) {
	    fprint(stream, "%s", item->as_float_int?"fi":"");
	} else if (item->float_sized) {
	    fprint(stream, "%s", item->float_sized?"f":"");
	} else if (item->as_double_int) {
	    fprint(stream, "%s", item->as_double_int?"di":"");
	} else if (item->double_sized) {
	    fprint(stream, "%s", item->double_sized?"d":"");
	} else if (item->as_longdouble_int) {
	    fprint(stream, "%s", item->as_longdouble_int?"ldi":"");
	} else if (item->longdouble_sized) {
	    fprint(stream, "%s", item->longdouble_sized?"ld":"");
	}
    }

    /*
     * print the third string (midstr)
     */
    fprint(stream, "%s", midstr);

    /*
     * print the fourth poststr
     */
    fprint(stream, "%s", poststr);
    return;
}


/*
 * vjson_fprint - print a line about a JSON parse tree node in va_list form
 *
 * This is a variable argument list interface to json_fprint().
 *
 * See json_tree_print() to print lines for an entire JSON parse tree.
 *
 * If dbg_output_allowed == false, this function will not print.
 *
 * When json_dbg_lvl == JSON_DBG_FORCED, the printed prefix of:
 *
 *	JSON tree[%d]:
 *
 * is replaced with:
 *
 *	JSON tree node:
 *
 * given:
 *	node	pointer to a JSON parser tree node to free
 *	depth	current tree depth (0 ==> top of tree)
 *	ap	variable argument list, required ap args:
 *
 *		stream	    stream to print on
 *		json_dbg_lvl   print message if JSON_DBG_FORCED
 *			       OR if <= json_verbosity_level
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 */
void
vjson_fprint(struct json *node, unsigned int depth, va_list ap)
{
    FILE *stream = NULL;	/* stream to print on */
    int json_dbg_lvl = JSON_DBG_DEFAULT;	/* JSON debug level if json_dbg_used == true */
    char const *tname = NULL;	/* name of the node type */
    va_list ap2;		/* copy of va_list ap */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * duplicate va_list ap
     */
    va_copy(ap2, ap);

    /*
     * obtain the stream, json_dbg_used, and json_dbg args
     */
    stream = va_arg(ap2, FILE *);
    if (stream == NULL) {
	va_end(ap2); /* stdarg variable argument list cleanup */
	return;
    }
    json_dbg_lvl = va_arg(ap2, int);

    /*
     * check JSON debug level if allowed
     */
    if (dbg_output_allowed == false ||
        (json_dbg_lvl != JSON_DBG_FORCED && json_dbg_lvl > json_verbosity_level)) {
	/* tree output disabled by json_verbosity_level */
	va_end(ap2); /* stdarg variable argument list cleanup */
	return;
    }

    /*
     * print debug header
     */
    if (json_dbg_lvl != JSON_DBG_FORCED) {
	fprint(stream, "JSON tree[%d]:\t", json_dbg_lvl);
    } else {
	fprstr(stream, "JSON tree node:\t");
    }

    /*
     * print node type
     */
    tname = json_item_type_name(node);
    if (tname == NULL) {
	warn(__func__, "json_item_type_name returned NULL");
	va_end(ap2); /* stdarg variable argument list cleanup */
	return;
    }
    fprint(stream, "lvl: %u\ttype: %s", depth, tname);

    /*
     * print node details
     */
    switch (node->type) {

    case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
	fprint(stream, "\tWarning: JTYPE_UNSET: %s", json_type_name(node->type));
	break;

    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
	{
	    struct json_number *item = &(node->item.number);

	    /*
	     * case: converted number
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {
	        fprnumber(stream, "\t{", item, "}: value:\t", item->as_str);

	    /*
	     * case: not converted but parsed
	     */
	    } else if (PARSED_JSON_NODE(item)) {
	        fprnumber(stream, "\t{", item, "}: value:\t", item->as_str);

	    /*
	     * case: not converted and not parsed number
	     */
	    } else {
	        fprnumber(stream, "\t{", item, "}:\t", "node not parsed");
	    }
	}
	break;

    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
	{
	    struct json_string *item = &(node->item.string);

	    /*
	     * case: converted and parsed string
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {

		/*
		 * print string preamble
		 */
		fprint(stream, "\tlen{%s%s%s%s%s%s%s%s%s}: %ju\tvalue:\t",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?",":":",
				CONVERTED_PARSED_JSON_NODE(item)?"c:":"",
				item->quote ? "q" : "",
				item->same ? "=" : "",
				item->slash ? "/" : "",
				item->posix_safe ? "P" : "",
				item->first_alphanum ? "a" : "",
				item->upper ? "U" : "",
				(uintmax_t)item->str_len);
		(void) fprint_line_buf(stream, item->str, item->str_len, '"', '"');

	    /*
	     * case: not converted but parsed string
	     */
	    } else if (PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}: no converted data to print",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: neither converted nor parsed
	     */
	    } else {
		fprint(stream, "\t{%s%s}: node not parsed",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");
	    }
	}
	break;

    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
	{
	    struct json_boolean *item = &(node->item.boolean);

	    /*
	     * case: converted and parsed boolean
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}value: %s",
			       PARSED_JSON_NODE(item)?"p":"",
			       CONVERTED_PARSED_JSON_NODE(item)?"c":"",
			       booltostr(item->value));

	    /*
	     * case: not converted but parsed string
	     */
	    } else if (PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}: no converted data to print",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: neither converted nor parsed
	     */
	    } else {
		fprint(stream, "\t{%s%s}: node not parsed",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");
	    }
	}
	break;

    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
	{
	    struct json_null *item = &(node->item.null);

	    /*
	     * case: converted and parsed null
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {

		fprint(stream, "\t{%s%s}: value: null",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: not converted but parsed string
	     */
	    } else if (PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}: no converted data to print",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: neither converted nor parsed
	     */
	    } else {
		fprint(stream, "\t{%s%s}: node not parsed",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");
	    }
	}
	break;

    case JTYPE_MEMBER:	/* JSON item is a member - see struct json_member */
	{
	    struct json_member *item = &(node->item.member);

	    /*
	     * case: converted and parsed member
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {

		/*
		 * print member name
		 */
		if (item->name != NULL) {
		    enum item_type type = item->name->type;

		    /*
		     * case: name is JTYPE_STRING
		     */
		    if (type == JTYPE_STRING) {
			struct json_string *item2 = &(item->name->item.string);

			if (CONVERTED_PARSED_JSON_NODE(item2)) {
			    fprint(stream, "\t{%s%s}name: ",
					   PARSED_JSON_NODE(item2)?"p":"",
					   CONVERTED_PARSED_JSON_NODE(item2)?"c":"");
			    (void) fprint_line_buf(stream, item2->str, item2->str_len, '"', '"');

			} else if (CONVERTED_PARSED_JSON_NODE(item2)) {
			    fprint(stream, "\t{%s%s}name: no converted name data to print",
					   PARSED_JSON_NODE(item2)?"p":"",
					   CONVERTED_PARSED_JSON_NODE(item2)?"c":"");

			} else {
			    fprint(stream, "\t{%s%s}: name node not parsed",
					    PARSED_JSON_NODE(item2)?"p":"",
					    CONVERTED_PARSED_JSON_NODE(item2)?"c":"");
			}

		    /*
		     * case: name is NOT JTYPE_STRING
		     */
		    } else {
			fprint(stream, "\t{%s%s}: Warning: wrong name type: %s",
				       PARSED_JSON_NODE(item)?"p":"",
				       CONVERTED_PARSED_JSON_NODE(item)?"c":"",
				       json_item_type_name(item->name));
		    }

		/*
		 * bogus name pointer
		 */
		} else {
		    fprint(stream, "\t{%s%s}: name == NULL",
				    PARSED_JSON_NODE(item)?"p":"",
				    CONVERTED_PARSED_JSON_NODE(item)?"c":"");
		}

	    /*
	     * case: not converted but parsed string
	     */
	    } else if (PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}: no converted data to print",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: neither converted nor parsed
	     */
	    } else {
		fprint(stream, "\t{%s%s}: node not parsed",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");
	    }
	}
	break;

    case JTYPE_OBJECT:	/* JSON item is a { members } - see struct json_object */
	{
	    struct json_object *item = &(node->item.object);

	    /*
	     * case: converted and parsed object
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {

		fprint(stream, "\t{%s%s}len: %ju",
			       PARSED_JSON_NODE(item)?"p":"",
			       CONVERTED_PARSED_JSON_NODE(item)?"c":"",
			       item->len);
		if (item->set == NULL) {
		    fprstr(stream, "\tWarning: set == NULL");
		}
		if (item->s == NULL) {
		    fprstr(stream, "\tWarning: s == NULL");
		}

	    /*
	     * case: not converted but parsed string
	     */
	    } else if (PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}: no converted data to print",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: neither converted nor parsed
	     */
	    } else {
		fprint(stream, "\t{%s%s}: node not parsed",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");
	    }
	}
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] - see struct json_array */
	{
	    struct json_array *item = &(node->item.array);

	    /*
	     * case: converted and parsed object
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {

		fprint(stream, "\t{%s%s}len: %ju",
			       PARSED_JSON_NODE(item)?"p":"",
			       CONVERTED_PARSED_JSON_NODE(item)?"c":"",
			       item->len);
		if (item->set == NULL) {
		    fprstr(stream, "\tWarning: set == NULL");
		}
		if (item->s == NULL) {
		    fprstr(stream, "\tWarning: s == NULL");
		}

	    /*
	     * case: not converted but parsed string
	     */
	    } else if (PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}: no converted data to print",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: neither converted nor parsed
	     */
	    } else {
		fprint(stream, "\t{%s%s}: node not parsed",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");
	    }
	}
	break;

    case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values - see struct json_elements */
	{
	    struct json_elements *item = &(node->item.elements);

	    /*
	     * case: converted and parsed object
	     */
	    if (CONVERTED_PARSED_JSON_NODE(item)) {

		fprint(stream, "\t{%s%s}len: %ju",
			       PARSED_JSON_NODE(item)?"p":"",
			       CONVERTED_PARSED_JSON_NODE(item)?"c":"",
			       item->len);
		if (item->set == NULL) {
		    fprstr(stream, "\tWarning: set == NULL");
		}
		if (item->s == NULL) {
		    fprstr(stream, "\tWarning: s == NULL");
		}

	    /*
	     * case: not converted but parsed string
	     */
	    } else if (PARSED_JSON_NODE(item)) {
		fprint(stream, "\t{%s%s}: no converted data to print",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");

	    /*
	     * case: neither converted nor parsed
	     */
	    } else {
		fprint(stream, "\t{%s%s}: node not parsed",
				PARSED_JSON_NODE(item)?"p":"",
				CONVERTED_PARSED_JSON_NODE(item)?"c":"");
	    }
	}
	break;

    default:
	fprint(stream, "\tunknown type: %d", node->type);
	break;
    }

    /*
     * print final newline
     */
    fprstr(stream, "\n");

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap2);
    return;
}


/*
 * json_tree_print - print lines for an entire JSON parse tree.
 *
 * This function uses the json_tree_walk() interface to walk
 * the JSON parse tree and print lines about all tree nodes.
 *
 * If dbg_output_allowed == false, this function will not print.
 *
 * When json_dbg_lvl == JSON_DBG_FORCED, the printed prefix of:
 *
 *	JSON tree[%d]:
 *
 * is replaced with:
 *
 *	JSON tree node:
 *
 * given:
 *	node	    pointer to a JSON parser tree node to free
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	...	extra args are ignored, required extra args:
 *
 *		stream	    stream to print on
 *		json_dbg_lvl   print message if JSON_DBG_FORCED
 *			       OR if <= json_verbosity_level
 *
 * Example uses - print an entire JSON parse tree:
 *
 *	json_tree_print(tree, JSON_DEFAULT_MAX_DEPTH, NULL, JSON_DBG_FORCED);
 *	json_tree_print(tree, JSON_DEFAULT_MAX_DEPTH, stderr, JSON_DBG_FORCED);
 *	json_tree_print(tree, JSON_DEFAULT_MAX_DEPTH, stdout, JSON_DBG_MED);
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 *
 * NOTE: this function is a wrapper to vjson_tree_walk() with the callback
 * vjson_fprint().
 */
void
json_tree_print(struct json *node, unsigned int max_depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, max_depth);

    /*
     * print the JSON parse tree
     */
    vjson_tree_walk(node, max_depth, 0, true, vjson_fprint, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * json_dbg_tree_print - print JSON tree if we are verbose enough
 *
 * This function behaves like json_dbg() in determining if a JSON debug message
 * is printed.  To print a JSON debug message, dbg_output_allowed == true AND
 * json_dbg_lvl <= json_verbosity_level or json_dbg_lvl == JSON_DBG_FORCED.
 * When dbg_output_allowed == false, no debug output will be printed,
 * even when json_dbg_lvl == JSON_DBG_FORCED.
 *
 * If dbg_output_allowed == false, this function will not print.
 *
 * When json_dbg_lvl == JSON_DBG_FORCED, the printed prefix of:
 *
 *	JSON tree[%d]:
 *
 * is replaced with:
 *
 *	JSON tree node:
 *
 * given:
 *	node	    pointer to a JSON parser tree node to free
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			       OR if <= json_verbosity_level
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *
 * Example use - free an entire JSON parse tree
 *
 * When json_dbg_lvl == JSON_DBG_FORCED, the printed prefix of:
 *
 *	JSON tree[%d]:
 *
 * is replaced with:
 *
 *	JSON tree node:
 *
 * given:
 *	json_dbg_lvl	print message if JSON_DBG_FORCED OR if >= verbosity level
 *	name		function name
 *	node		pointer to a JSON parser tree node to free
 *	max_depth	maximum tree depth to descend, or 0 ==> infinite depth
 *			    NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			    NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	stream		stream on which to print
 */
void
json_dbg_tree_print(int json_dbg_lvl, char const *name, struct json *tree, unsigned int max_depth)
{
    int saved_errno = 0;	/* errno at function start */
    bool allowed = false;	/* assume debug output is not allowed */

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "NULL name, forcing use of: %s", name);
    }
    if (tree == NULL) {
	warn(__func__, "NULL tree, returning false");
	return;
    }

    /*
     * print the debug message if allowed and allowed by the verbosity level
     */
    allowed = json_dbg_allowed(json_dbg_lvl);
    if (allowed == true) {
	json_tree_print(tree, max_depth, stderr, json_dbg_lvl);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * json_tree_walk - walk a JSON parse tree calling a function on each node
 *
 * Walk a JSON parse tree, Depth-first Post-order (LRN).  See:
 *
 *	https://en.wikipedia.org/wiki/Tree_traversal#Post-order,_LRN
 *
 * Example use - free an entire JSON parse tree
 *
 *	json_tree_walk(tree, JSON_DEFAULT_MAX_DEPTH, 0, true, json_free);
 *
 * given:
 *	node	    pointer to a JSON parse tree
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	depth	    current tree depth (0 ==> top of tree)
 *	post_order  true ==> walk tree in post-order (LRN), false ==> walk in pre-order (NLR)
 *	vcallback   function to operate JSON parse tree node in va_list form
 *	...	    extra args for vcallback
 *
 * The vcallback() function must NOT call va_arg() nor call va_end() on the
 * va_list argument directly.  Instead they must call va_copy() and then use
 * va_arg() and va_end() on the va_list copy.
 *
 * Although in C ALL functions are pointers which means one can call foo()
 * as foo() or (*foo)() we use the latter format for the callback function
 * to make it clearer that it is in fact a function that's passed in so
 * that we can use this function to do more than one thing. This is also
 * why we call it vcallback (v for variadic support) and not something else.
 *
 * If max_depth is >= 0 and the tree depth > max_depth, then this function
 * returns. In this case it will NOT operate on the node and it will NOT descend
 * any further into the tree.
 *
 * NOTE: This function warns but does not do anything if an arg is NULL.
 */
void
json_tree_walk(struct json *node, unsigned int max_depth, unsigned int depth, bool post_order,
	       void (*vcallback)(struct json *, unsigned int, va_list), ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return;
    }
    if (vcallback == NULL) {
	warn(__func__, "vcallback is NULL");
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, vcallback);

    /*
     * walk the tree according to max_depth
     */
    vjson_tree_walk(node, max_depth, depth, post_order, vcallback, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * vjson_tree_walk - walk a JSON parse tree calling a function on each node in va_list form
 *
 * This is the va_list form of json_tree_walk().
 *
 * Walk a JSON parse tree, Depth-first Post-order (LRN).  See:
 *
 *	https://en.wikipedia.org/wiki/Tree_traversal#Post-order,_LRN
 *
 * Example use - free an entire JSON parse tree
 *
 *	json_tree_walk(tree, JSON_DEFAULT_MAX_DEPTH, 0, true, json_free);
 *
 * given:
 *	node	    pointer to a JSON parse tree
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	depth	    current tree depth (0 ==> top of tree)
 *	post_order  true ==> walk tree in post-order (LRN), false ==> walk in pre-order (NLR)
 *	vcallback   function to operate JSON parse tree node in va_list form
 *	ap	    variable argument list for vcallback
 *
 * The vcallback() function must NOT call va_arg() nor call va_end() on the
 * va_list argument directly.  Instead they must call va_copy() and then use
 * va_arg() and va_end() on the va_list copy.
 *
 * Although in C ALL functions are pointers which means one can call foo()
 * as foo() or (*foo)() we use the latter format for the callback function
 * to make it clearer that it is in fact a function that's passed in so
 * that we can use this function to do more than one thing. This is also
 * why we call it vcallback and not something else.
 *
 * If max_depth is >= 0 and the tree depth > max_depth, then this function return.
 * In this case it will NOT operate on the node, or will be descend and further
 * into the tree.
 *
 * NOTE: This function warns but does not do anything if an arg is NULL.
 */
void
vjson_tree_walk(struct json *node, unsigned int max_depth, unsigned int depth, bool post_order,
		void (*vcallback)(struct json *, unsigned int, va_list), va_list ap)
{
    int i;

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return ;
    }
    if (vcallback == NULL) {
	warn(__func__, "vcallback is NULL");
	return ;
    }

    /*
     * do nothing if we are too deep
     */
    if (max_depth != JSON_INFINITE_DEPTH && depth > max_depth) {
	warn(__func__, "tree walk descent stopped, tree depth: %u > max_depth: %u", depth, max_depth);
	return;
    }

    /*
     * walk based on type of node
     */
    switch (node->type) {

    case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */

	/* case: terminal JSON tree leaf node */
	/* perform function operation on this terminal parse tree node */
	(*vcallback)(node, depth, ap);
	break;

    case JTYPE_MEMBER:	/* JSON item is a member */
	/* case: pre-order (NLR) */
	if (post_order == false) {
	    (*vcallback)(node, depth, ap);
	}

	/* descend the tree */
	{
	    struct json_member *item = &(node->item.member);

	    /* perform function operation on JSON member name (left branch) node */
	    vjson_tree_walk(item->name, max_depth, depth+1, post_order, vcallback, ap);

	    /* perform function operation on JSON member value (right branch) node */
	    vjson_tree_walk(item->value, max_depth, depth+1, post_order, vcallback, ap);
	}

	/* finally perform function operation on the parent node */
	/* case: post-order (LRN) */
	if (post_order == true) {
	    (*vcallback)(node, depth, ap);
	}
	break;

    case JTYPE_OBJECT:	/* JSON item is a { members } */
	/* case: pre-order (NLR) */
	if (post_order == false) {
	    (*vcallback)(node, depth, ap);
	}

	/* descend the tree */
	{
	    struct json_object *item = &(node->item.object);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    vjson_tree_walk(item->set[i], max_depth, depth+1, post_order, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	/* case: post-order (LRN) */
	if (post_order == true) {
	    (*vcallback)(node, depth, ap);
	}
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] */
	/* case: pre-order (NLR) */
	if (post_order == false) {
	    (*vcallback)(node, depth, ap);
	}

	/* descend the tree */
	{
	    struct json_array *item = &(node->item.array);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    vjson_tree_walk(item->set[i], max_depth, depth+1, post_order, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	/* case: post-order (LRN) */
	if (post_order == true) {
	    (*vcallback)(node, depth, ap);
	}
	break;

    case JTYPE_ELEMENTS:	/* JSON items is zero or more JSON values */
	/* case: pre-order (NLR) */
	if (post_order == false) {
	    (*vcallback)(node, depth, ap);
	}

	/* descend the tree */
	{
	    struct json_elements *item = &(node->item.elements);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    vjson_tree_walk(item->set[i], max_depth, depth+1, post_order, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	/* case: post-order (LRN) */
	if (post_order == true) {
	    (*vcallback)(node, depth, ap);
	}
	break;

    default:
	warn(__func__, "node type is unknown: %d", node->type);
	/* nothing we can traverse */
	break;
    }
    return;
}

/*
 * json_util_parse_number_range	- parse a number ranges
 *
 * This may be useful for json tools that need to compare JSON related values.
 *
 * given:
 *
 *	option		- option string (e.g. "-l"). Used for error and debug messages.
 *	optarg		- the option argument
 *	allow_negative	- true if max can be < 0
 *	number		- pointer to struct number
 *
 * Returns true if successfully parsed.
 *
 * The following rules apply:
 *
 * (0) an exact number is a number optional arg by itself e.g. -l 5 or -l5.
 * (1) an inclusive range is <min>:<max> e.g. -l 5:10 where:
 *     (1a) the last number can be negative in which case it's up through the
 *	    count - max.
 * (2) a minimum number, that is num >= minimum, is <num>:
 * (3) a maximum number, that is num <= maximum, is :<num>
 * (4) if allow_negative is true then max can be < 0 otherwise it's an error.
 * (5) anything else is an error
 *
 * See also the structs json_util_number_range and json_util_number in json_util_util.h
 * for more details.
 *
 * NOTE: this function does not return on syntax error or NULL number.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_parse_number_range(const char *option, char *optarg, bool allow_negative, struct json_util_number *number)
{
    intmax_t max = 0;
    intmax_t min = 0;

    /* firewall */
    if (option == NULL || *option == '\0') {
	err(3, __func__, "NULL or empty option given"); /*ooo*/
	not_reached();
    }
    if (number == NULL) {
	err(3, __func__, "NULL number struct for option %s", option); /*ooo*/
	not_reached();
    } else {
	memset(number, 0, sizeof(struct json_util_number));

	/* don't assume everything is 0 */
	number->exact = false;
	number->range.min = 0;
	number->range.max = 0;
	number->range.inclusive = false;
	number->range.less_than_equal = false;
	number->range.greater_than_equal = false;
    }

    if (optarg == NULL || *optarg == '\0') {
	err(3, __func__, "NULL or empty optarg for %s", option); /*ooo*/
	return false;
    }

    if (!strchr(optarg, ':')) {
	if (string_to_intmax(optarg, &number->number)) {
	    number->exact = true;
	    number->range.min = 0;
	    number->range.max = 0;
	    number->range.inclusive = false;
	    number->range.less_than_equal = false;
	    number->range.greater_than_equal = false;
	    dbg(DBG_LOW, "exact number required for option %s: %jd", option, number->number);
	} else {
	    err(3, __func__, "invalid number for option %s: <%s>", option, optarg); /*ooo*/
	    not_reached();
	}
    } else if (sscanf(optarg, "%jd:%jd", &min, &max) == 2) {
	/* if allow_negative is false we won't allow negative max in range. */
	if (!allow_negative && max < 0) {
	    err(3, __func__, "invalid number for option %s: <%s>: max cannot be < 0", option, optarg); /*ooo*/
	    not_reached();
	} else {
	    /*
	     * NOTE: we can't check that min >= max because a negative number in the
	     * maximum means that the range is up through the count - max matches
	     */
	    number->range.min = min;
	    number->range.max = max;
	    number->range.inclusive = true;
	    number->range.less_than_equal = false;
	    number->range.greater_than_equal = false;
	    /* XXX - this debug message is problematic wrt the negative number
	     * option
	     */
	    dbg(DBG_LOW, "number range inclusive required for option %s: >= %jd && <= %jd", option, number->range.min,
		    number->range.max);
	}
    } else if (sscanf(optarg, "%jd:", &min) == 1) {
	number->number = 0;
	number->exact = false;
	number->range.min = min;
	number->range.max = number->range.min;
	number->range.greater_than_equal = true;
	number->range.less_than_equal = false;
	number->range.inclusive = false;
	dbg(DBG_LOW, "minimum number required for option %s: must be >= %jd", option, number->range.min);
    } else if (sscanf(optarg, ":%jd", &max) == 1) {
	/* if allow_negative is false we won't allow negative max in range. */
	if (!allow_negative && max < 0) {
	    err(3, __func__, "invalid number for option %s: <%s>: max cannot be < 0", option, optarg); /*ooo*/
	    not_reached();
	} else {
	    number->range.max = max;
	    number->range.min = number->range.max;
	    number->number = 0;
	    number->exact = false;
	    number->range.less_than_equal = true;
	    number->range.greater_than_equal = false;
	    number->range.inclusive = false;
	    dbg(DBG_LOW, "maximum number required for option %s: must be <= %jd", option, number->range.max);
	}
    } else {
	err(3, __func__, "number range syntax error for option %s: <%s>", option, optarg);/*ooo*/
	not_reached();
    }

    return true;
}

/*
 * json_util_number_in_range   - check if number is in required range
 *
 * This may be useful for json tools that need to compare JSON related values.
 *
 * given:
 *
 *	number		- number to check
 *	total_matches	- total number of matches found
 *	range		- pointer to struct json_util_number with range
 *
 * Returns true if the number is in range.
 *
 * NOTE: if range is NULL it will return false.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_number_in_range(intmax_t number, intmax_t total_matches, struct json_util_number *range)
{
    /* firewall check */
    if (range == NULL) {
	return false;
    }

    /* if exact is set and range->number == number then return true. */
    if (range->exact && range->number == number) {
	return true;
    } else if (range->range.inclusive) {
	/* if the number must be inclusive in range then we have to make sure
	 * that number >= min and <= max.
	 *
	 * NOTE: we have to make a special check for negative numbers because a
	 * negative number is up through the count of matches - the negative max
	 * number (rather if there are 10 matches and the string -l 5:-3 is
	 * specified then the items 5, 6, 7, 8 are to be printed).
	 */
	if (number >= range->range.min) {
	    if (range->range.max < 0 && number <= total_matches + range->range.max) {
		return true;
	    } else if (number <= range->range.max) {
		return true;
	    } else {
		return false;
	    }
	} else {
	    return false;
	}
    } else if (range->range.less_than_equal) {
	/* if number has to be less than equal we check number <= the maximum
	 * number (range->range.max).
	 */
	if (number <= range->range.max) {
	    return true;
	} else {
	    return false;
	}
    } else if (range->range.greater_than_equal) {
	/* if number has to be greater than or equal to the number then we check
	 * that number >= minimum number (range->range.min).
	 */
	if (number >= range->range.min) {
	    return true;
	} else {
	    return false;
	}
    }

    return false; /* no match */
}

/*
 * json_util_parse_st_level_option    - parse -L [num]{s,t}/-b level option
 *
 * This may be useful for json tools need to focus on various levels in
 * a JSON parse tree.
 *
 * This function parses the -L option.
 *
 * given:
 *
 *	optarg		    - option argument to -L option (can be faked)
 *	num_level_spaces    - pointer to number of spaces or tabs to print after levels
 *	print_level_tab	    - pointer to boolean indicating if tab or spaces are to be used
 *
 * Function returns void.
 *
 * NOTE: syntax errors are an error just like it was when it was in main().
 *
 * NOTE: this function does not return on NULL pointers.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
void
json_util_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab)
{
    char ch = '\0';	/* whether spaces or tabs are to be used, 's' or 't' */

    /* firewall checks */
    if (optarg == NULL || *optarg == '\0') {
	err(3, __func__, "NULL or empty optarg"); /*ooo*/
	not_reached();
    } else if (num_level_spaces == NULL) {
	err(3, __func__, "NULL num_level_spaces"); /*ooo*/
	not_reached();
    } else if (print_level_tab == NULL) {
	err(3, __func__, "NULL print_token_tab"); /*ooo*/
	not_reached();
    } else {
	/* ensure that the variables are empty */

	/* make *num_level_spaces == 0 */
	*num_level_spaces = 0;
	/* make *print_level_tab == false */
	*print_level_tab = false;
    }

    if (sscanf(optarg, "%ju%c", num_level_spaces, &ch) == 2) {
	if (ch == 't') {
	    *print_level_tab = true;
	    dbg(DBG_LOW, "will print %ju tab%s after levels", *num_level_spaces, *num_level_spaces==1?"":"s");
	} else if (ch == 's') {
	    *print_level_tab = false; /* ensure it's false in case specified previously */
	    dbg(DBG_LOW, "will print %jd space%s after levels", *num_level_spaces, *num_level_spaces==1?"":"s");
	} else {
	    err(3, __func__, "syntax error for -L"); /*ooo*/
	    not_reached();
	}
    } else if (!strcmp(optarg, "tab")) {
	    *print_level_tab = true;
	    *num_level_spaces = 1;
	    dbg(DBG_LOW, "will print %ju tab%s after levels", *num_level_spaces, *num_level_spaces==1?"":"s");
    } else if (!string_to_uintmax(optarg, num_level_spaces)) {
	err(3, __func__, "couldn't parse -L spaces"); /*ooo*/
	not_reached();
    } else {
	*print_level_tab = false; /* ensure it's false in case specified previously */
	dbg(DBG_LOW, "will print %jd space%s after levels", *num_level_spaces, *num_level_spaces==1?"":"s");
    }
}

/*
 * json_util_parse_match_types	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JSON_UTIL_MATCH_TYPE_SIMPLE (as if '-t simple').
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
uintmax_t
json_util_parse_match_types(char *optarg)
{
    uintmax_t type = JSON_UTIL_MATCH_TYPE_SIMPLE; /* default is simple: num, bool, str and null */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;	    /* strdup()d copy of optarg */
    char *p = NULL;

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    }
    p = optarg;

    errno = 0; /* pre-clear errno for errp() */
    dup = strdup(optarg);
    if (dup == NULL) {
	errp(160, __func__, "strdup(%s) failed", optarg);
	not_reached();
    }

    /*
     * Go through comma-separated list of what to match, setting each as a bitvector
     */
    for (p = strtok_r(dup, ",", &saveptr); p; p = strtok_r(NULL, ",", &saveptr)) {
	if (!strcmp(p, "int")) {
	    type |= JSON_UTIL_MATCH_TYPE_INT;
	} else if (!strcmp(p, "float")) {
	    type |= JSON_UTIL_MATCH_TYPE_FLOAT;
	} else if (!strcmp(p, "exp")) {
	    type |= JSON_UTIL_MATCH_TYPE_EXP;
	} else if (!strcmp(p, "num")) {
	    type |= JSON_UTIL_MATCH_TYPE_NUM;
	} else if (!strcmp(p, "bool")) {
	    type |= JSON_UTIL_MATCH_TYPE_BOOL;
	} else if (!strcmp(p, "str")) {
	    type |= JSON_UTIL_MATCH_TYPE_STR;
	} else if (!strcmp(p, "null")) {
	    type |= JSON_UTIL_MATCH_TYPE_NULL;
	} else if (!strcmp(p, "simple")) {
	    type |= JSON_UTIL_MATCH_TYPE_SIMPLE;
	} else {
	    /* unknown or unsupported type */
	    err(3, __func__, "unknown or unsupported type '%s'", p); /*ooo*/
	    not_reached();
	}
    }

    return type;
}

/*
 * json_util_match_none	- if no types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types == 0.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_none(uintmax_t types)
{
    return types == JSON_UTIL_MATCH_TYPE_NONE;
}

/*
 * json_util_match_int	- if ints should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_INT set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_int(uintmax_t types)
{
    return (types & JSON_UTIL_MATCH_TYPE_INT) != 0;
}
/*
 * json_util_match_float	- if floats should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_FLOAT set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_float(uintmax_t types)
{
    return (types & JSON_UTIL_MATCH_TYPE_FLOAT) != 0;
}
/*
 * json_util_match_exp	- if exponents should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_EXP set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_exp(uintmax_t types)
{
    return (types & JSON_UTIL_MATCH_TYPE_EXP) != 0;
}
/*
 * json_util_match_num	- if numbers of any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_NUM (or any of the number types) set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_num(uintmax_t types)
{
    return ((types & JSON_UTIL_MATCH_TYPE_NUM)||(types & JSON_UTIL_MATCH_TYPE_INT) || (types & JSON_UTIL_MATCH_TYPE_FLOAT) ||
	    (types & JSON_UTIL_MATCH_TYPE_EXP))!= 0;
}
/*
 * json_util_match_bool	- if booleans should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_BOOL set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_bool(uintmax_t types)
{
    return (types & JSON_UTIL_MATCH_TYPE_BOOL) != 0;
}
/*
 * json_util_match_string	    - if strings should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_STR set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_string(uintmax_t types)
{
    return (types & JSON_UTIL_MATCH_TYPE_STR) != 0;
}
/*
 * json_util_match_null	- if null should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_NULL set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_null(uintmax_t types)
{
    return (types & JSON_UTIL_MATCH_TYPE_NULL) != 0;
}
/*
 * json_util_match_simple	- if simple types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Simple is defined as a number, a bool, a string or a null.
 *
 * Returns true if types has JSON_UTIL_MATCH_TYPE_SIMPLE set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true value to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another value to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 *
 * WARNING: this function is VERY subject to change, if it is not deleted!
 */
bool
json_util_match_simple(uintmax_t types)
{
    return (types & JSON_UTIL_MATCH_TYPE_SIMPLE) != 0;
}
