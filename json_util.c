/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_util - general JSON parser utility support functions
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * This JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and vomit on the JSON spec! :-)
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

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
 * static declarations
 */

/*
 * JSON warn (NOT error) codes to ignore
 *
 * When a tool is given command line arguments of the form:
 *
 *	.. -W 123 -W 1345 -W 56 ...
 *
 * this means the tool will ignore {JSON-0123}, {JSON-1345}, and {JSON-0056}.
 * The code_ignore_settable holds the JSON codes to ignore.
 *
 * NOTE: A NULL ignore_json_code_set means that the set has not been setup.
 */
static void alloc_json_code_ignore_set(void);
static int cmp_codes(const void *a, const void *b);
static void expand_json_code_ignore_set(void);


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
json_warn_allowed()
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
    if (ret < 0) {
	warn(__func__, "fprintf returned errno: %d: (%s)", errno, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
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
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * write JSON encoding to stream
     */
    errno = 0;	    /* pre-clear errno for warnp */
    ret = fprintf(stream, "%s", jenc[c].enc);
    if (ret <= 0) {
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
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * case: NULL
     */
    if (str == NULL) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(stream, "null");
	if (ret <= 0) {
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
	if (json_putc(*p, stream) != true) {
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
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON long
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%ld", value);
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a long");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
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
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a time_t");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
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
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON boolean
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", booltostr(value));
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a boolean");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_type_name - print a struct json item union type by name
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
 *	node	pointer to a JSON parser tree node to free
 *
 * returns:
 *	A constant (read-only) string that names the JTYPE_ enum.
 *
 * NOTE: This string returned is read only: It's not allocated on the stack.
 */
char const *
json_item_type_name(struct json *node)
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
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
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
    vjson_tree_walk(node, max_depth, 0, vjson_free, ap);

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
 *	node	pointer to a JSON parser tree node to free
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
     * print debug leader
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
	break;

    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
	{
	    struct json_number *item = &(node->item.number);

	    /*
	     * case: converted number
	     */
	    if (item->converted == true) {

		/*
		 * case: converted negative number
		 */
		if (item->is_negative == true) {

		    /*
		     * case: converted e-notation negative number
		     */
		    if (item->is_e_notation == true) {

			/*
			 * try to print the smallest converted floating point
			 */
			if (item->float_sized == true) {
			    fprint(stream, "\tf-val: %e\tinteger: %s",
					   item->as_float, booltostr(item->as_float_int));
			} else if (item->double_sized == true) {
			    fprint(stream, "\td-val: %le\tinteger: %s",
					   item->as_double, booltostr(item->as_double_int));
			} else if (item->longdouble_sized == true) {
			    fprint(stream, "\tL-val: %Le\tinteger: %s",
					   item->as_longdouble, booltostr(item->as_longdouble_int));
			} else {
			    fprstr(stream, "\tWarning: -e-notation: no common size:\t");
			    (void) fprint_line_buf(stream, item->first, item->number_len, '<', '>');
			}

		    /*
		     * case: converted floating negative number
		     */
		    } else if (item->is_floating == true) {

			/*
			 * try to print the smallest converted floating point
			 */
			if (item->float_sized == true) {
			    fprint(stream, "\tf-val: %f\tinteger: %s",
					   item->as_float, booltostr(item->as_float_int));
			} else if (item->double_sized == true) {
			    fprint(stream, "\td-val: %lf\tinteger: %s",
					   item->as_double, booltostr(item->as_double_int));
			} else if (item->longdouble_sized == true) {
			    fprint(stream, "\tL-val: %Lf\tinteger: %s",
					   item->as_longdouble, booltostr(item->as_longdouble_int));
			} else {
			    fprstr(stream, "\tWarning: -floating: no common size:\t");
			    (void) fprint_line_buf(stream, item->first, item->number_len, '<', '>');
			}

		    /*
		     * case: converted integer negative number
		     */
		    } else {

			/*
			 * try to print the smallest converted integer
			 */
			if (item->int8_sized == true) {
			    fprint(stream, "\tval-8: %jd", (intmax_t)item->as_int8);
			} else if (item->int16_sized == true) {
			    fprint(stream, "\tval-16: %jd", (intmax_t)item->as_int16);
			} else if (item->int32_sized == true) {
			    fprint(stream, "\tval-32: %jd", (intmax_t)item->int32_sized);
			} else if (item->int64_sized == true) {
			    fprint(stream, "\tval-64: %jd", (intmax_t)item->int64_sized);
			} else if (item->maxint_sized == true) {
			    fprint(stream, "\tval-max: %jd", (intmax_t)item->maxint_sized);
			} else {
			    fprstr(stream, "\tWarning: -integer: no common size:\t");
			    (void) fprint_line_buf(stream, item->first, item->number_len, '<', '>');
			}
		    }

		/*
		 * case: converted positive number
		 */
		} else {

		    /*
		     * case: converted e-notation positive number
		     */
		    if (item->is_e_notation == true) {

			/*
			 * try to print the smallest converted floating point
			 */
			if (item->float_sized == true) {
			    fprint(stream, "\tf+val: %e\tinteger: %s",
					   item->as_float, booltostr(item->as_float_int));
			} else if (item->double_sized == true) {
			    fprint(stream, "\td+val: %le\tinteger: %s",
					   item->as_double, booltostr(item->as_double_int));
			} else if (item->longdouble_sized == true) {
			    fprint(stream, "\tL+val: %Le\tinteger: %s",
					   item->as_longdouble, booltostr(item->as_longdouble_int));
			} else {
			    fprstr(stream, "\tWarning: +e-notation: no common size:\t");
			    (void) fprint_line_buf(stream, item->first, item->number_len, '<', '>');
			}

		    /*
		     * case: converted floating positive number
		     */
		    } else if (item->is_floating == true) {

			/*
			 * try to print the smallest converted floating point
			 */
			if (item->float_sized == true) {
			    fprint(stream, "\tf+val: %f\tinteger: %s",
					   item->as_float, booltostr(item->as_float_int));
			} else if (item->double_sized == true) {
			    fprint(stream, "\td+val: %lf\tinteger: %s",
					   item->as_double, booltostr(item->as_double_int));
			} else if (item->longdouble_sized == true) {
			    fprint(stream, "\tL+val: %Lf\tinteger: %s",
					   item->as_longdouble, booltostr(item->as_longdouble_int));
			} else {
			    fprstr(stream, "\tWarning: +floating: no common size:\t");
			    (void) fprint_line_buf(stream, item->first, item->number_len, '<', '>');
			}

		    /*
		     * case: converted integer positive number
		     */
		    } else {

			/*
			 * try to print the smallest converted integer
			 */
			if (item->int8_sized == true) {
			    fprint(stream, "\tval+8: %jd", (intmax_t)item->as_int8);
			} else if (item->int16_sized == true) {
			    fprint(stream, "\tval+16: %jd", (intmax_t)item->as_int16);
			} else if (item->int32_sized == true) {
			    fprint(stream, "\tval+32: %jd", (intmax_t)item->int32_sized);
			} else if (item->int64_sized == true) {
			    fprint(stream, "\tval+64: %jd", (intmax_t)item->int64_sized);
			} else if (item->maxint_sized == true) {
			    fprint(stream, "\tval+max: %jd", (intmax_t)item->maxint_sized);
			} else {
			    fprstr(stream, "\tWarning: +integer: no common size:\t");
			    (void) fprint_line_buf(stream, item->first, item->number_len, '<', '>');
			}
		    }
		}

	    /*
	     * case: not converted number
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
	    }
	}
	break;

    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
	{
	    struct json_string *item = &(node->item.string);

	    /*
	     * case: converted string
	     */
	    if (item->converted == true) {

		/*
		 * print string preamble
		 */
		fprint(stream, "\tlen{%s%s%s%s%s%s%s}: %ju\tvalue:\t",
			       item->quote ? "q" : "",
			       item->same ? "=" : "",
			       item->has_nul ? "0" : "",
			       item->slash ? "/" : "",
			       item->posix_safe ? "P" : "",
			       item->first_alphanum ? "a" : "",
			       item->upper ? "U" : "",
			       (uintmax_t)item->str_len);
		(void) fprint_line_buf(stream, item->str, item->str_len, '"', '"');

	    /*
	     * case: not converted string
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
	    }
	}
	break;

    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
	{
	    struct json_boolean *item = &(node->item.boolean);

	    /*
	     * case: converted boolean
	     */
	    if (item->converted == true) {

		fprint(stream, "\tvalue: %s", booltostr(item->value));

	    /*
	     * case: not converted boolean
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
	    }
	}
	break;

    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
	{
	    struct json_null *item = &(node->item.null);

	    /*
	     * case: converted null
	     */
	    if (item->converted == true) {

		fprstr(stream, "\tvalue: NULL");

	    /*
	     * case: not converted null
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
	    }
	}
	break;

    case JTYPE_MEMBER:	/* JSON item is a member - see struct json_member */
	{
	    struct json_member *item = &(node->item.member);

	    /*
	     * case: converted member
	     */
	    if (item->converted == true) {

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

			if (item2->converted == true) {
			    fprstr(stream, "\tname: ");
			    (void) fprint_line_buf(stream, item2->str, item2->str_len, '"', '"');
			} else {
			    fprstr(stream, "\tname converted: false");
			}

		    /*
		     * case: name is NOT JTYPE_STRING
		     */
		    } else {
			fprint(stream, "\tWarning: wrong name type: %s", json_item_type_name(item->name));
		    }

		/*
		 * bogus name pointer
		 */
		} else {
		    fprstr(stream, "\tWarning: name == NULL");
		}

	    /*
	     * case: not converted member
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
	    }
	}
	break;

    case JTYPE_OBJECT:	/* JSON item is a { members } - see struct json_object */
	{
	    struct json_object *item = &(node->item.object);

	    /*
	     * case: converted object
	     */
	    if (item->converted == true) {

		fprint(stream, "\tlen: %d", item->len);
		if (item->set == NULL) {
		    fprstr(stream, "\tWarning: set == NULL");
		}
		if (item->s == NULL) {
		    fprstr(stream, "\tWarning: s == NULL");
		}

	    /*
	     * case: not converted object
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
	    }
	}
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] - see struct json_array */
	{
	    struct json_array *item = &(node->item.array);

	    /*
	     * case: converted object
	     */
	    if (item->converted == true) {

		fprint(stream, "\tlen: %d", item->len);
		if (item->set == NULL) {
		    fprstr(stream, "\tWarning: set == NULL");
		}
		if (item->s == NULL) {
		    fprstr(stream, "\tWarning: s == NULL");
		}

	    /*
	     * case: not converted object
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
	    }
	}
	break;

    case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values - see struct json_elements */
	{
	    struct json_elements *item = &(node->item.elements);

	    /*
	     * case: converted object
	     */
	    if (item->converted == true) {

		fprint(stream, "\tlen: %d", item->len);
		if (item->set == NULL) {
		    fprstr(stream, "\tWarning: set == NULL");
		}
		if (item->s == NULL) {
		    fprstr(stream, "\tWarning: s == NULL");
		}

	    /*
	     * case: not converted object
	     */
	    } else {
		fprstr(stream, "\tconverted: false");
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
 *	...	extra args are ignored, required extra args:
 *
 *		stream	    stream to print on
 *		json_dbg_lvl   print message if JSON_DBG_FORCED
 *			       OR if <= json_verbosity_level
 *
 * Example use - free an entire JSON parse tree
 *
 *	len = json_tree_print(tree, JSON_DEFAULT_MAX_DEPTH, NULL, JSON_DBG_FORCED);
 *	json_tree_print(tree, JSON_DEFAULT_MAX_DEPTH, stderr, JSON_DBG_FORCED);
 *	json_tree_print(tree, JSON_DEFAULT_MAX_DEPTH, stdout, JSON_DBG_MED);
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
     * free the JSON parse tree
     */
    vjson_tree_walk(node, max_depth, 0, vjson_fprint, ap);

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
 * Walk a JSON parse tree, Depth-first Post-order (LRN) order.  See:
 *
 *	https://en.wikipedia.org/wiki/Tree_traversal#Post-order,_LRN
 *
 * Example use - free an entire JSON parse tree
 *
 *	json_tree_walk(tree, JSON_DEFAULT_MAX_DEPTH, json_free);
 *
 * given:
 *	node	    pointer to a JSON parse tree
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
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
json_tree_walk(struct json *node, unsigned int max_depth,
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
    vjson_tree_walk(node, max_depth, 0, vcallback, ap);

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
 * Walk a JSON parse tree, Depth-first Post-order (LRN) order.  See:
 *
 *	https://en.wikipedia.org/wiki/Tree_traversal#Post-order,_LRN
 *
 * Example use - free an entire JSON parse tree
 *
 *	json_tree_walk(tree, JSON_DEFAULT_MAX_DEPTH, 0, json_free);
 *
 * given:
 *	node	    pointer to a JSON parse tree
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *	depth	    current tree depth (0 ==> top of tree)
 *	vcallback   function to operate JSON parse tree node in va_list form
 *	ap	    variable argument list
 *
 * The vcallback() function must NOT call va_arg() nor call va_end() on the
 * va_list argument directly.  Instead they must call va_copy() and then use
 * va_arg() and va_end() on the va_list copy.
 *
 * Although in C ALL functions are pointers which means one can call foo()
 * as foo() or (*foo)() we use the latter format for the callback function
 * to make it clearer that it is in fact a function that's passed in so
 * that we can use this function to do more than one thing. This is also
 * why we call it callback and not something else.
 *
 * If max_depth is >= 0 and the tree depth > max_depth, then this function return.
 * In this case it will NOT operate on the node, or will be descend and further
 * into the tree.
 *
 * NOTE: This function warns but does not do anything if an arg is NULL.
 */
void
vjson_tree_walk(struct json *node, unsigned int max_depth, unsigned int depth,
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

	/* perform function operation on this terminal parse tree node */
	(*vcallback)(node, depth, ap);
	break;

    case JTYPE_MEMBER:	/* JSON item is a member */
	{
	    struct json_member *item = &(node->item.member);

	    /* perform function operation on JSON member name (left branch) node */
	    vjson_tree_walk(item->name, max_depth, depth+1, vcallback, ap);

	    /* perform function operation on JSON member value (right branch) node */
	    vjson_tree_walk(item->value, max_depth, depth+1, vcallback, ap);
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(node, depth, ap);
	break;

    case JTYPE_OBJECT:	/* JSON item is a { members } */
	{
	    struct json_object *item = &(node->item.object);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    vjson_tree_walk(item->set[i], max_depth, depth+1, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(node, depth, ap);
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] */
	{
	    struct json_array *item = &(node->item.array);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    vjson_tree_walk(item->set[i], max_depth, depth+1, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(node, depth, ap);
	break;

    case JTYPE_ELEMENTS:	/* JSON items is zero or more JSON values */
	{
	    struct json_elements *item = &(node->item.elements);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    vjson_tree_walk(item->set[i], max_depth, depth+1, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(node, depth, ap);
	break;

    default:
	warn(__func__, "node type is unknown: %d", node->type);
	/* nothing we can traverse */
	break;
    }
    return;
}
