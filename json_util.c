/*
 * json_util - general JSON parser utility support functions
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * This is currently being worked on by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 * and
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This is very much a work in progress!
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
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * json - the json core
 */
#include "json_parse.h"

/*
 * json_util - #includes what we need
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

/* for json number strings */
static bool json_process_decimal(struct json_number *item, char const *str, size_t len);
static bool json_process_floating(struct json_number *item, char const *str, size_t len);


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
 *	 ASCII digit.  It is assumed that str[len-1] is the final JSON number
 *	 character.
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
	    dbg(DBG_VVVHIGH, "strtoimax failed to convert");
	    item->converted = false;
	    return false;	/* processing failed */
	}
	item->converted = true;
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
	    dbg(DBG_VVVHIGH, "strtoumax failed to convert");
	    item->converted = false;
	    return false;	/* processing failed */
	}
	item->converted = true;
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
    size_t str_len = 0;			/* length as a C string, of str */

    /*
     * firewall
     */
    if (item == NULL) {
	warn(__func__, "called with NULL item");
	return false;	/* processing failed */
    }
    if (str == NULL) {
	warn(__func__, "called with NULL ptr");
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
	dbg(DBG_VVVHIGH, "strtold failed to convert");
	item->converted = false;
	return false;	/* processing failed */
    }
    item->converted = true;
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
	item->double_sized = false;
	dbg(DBG_VVVHIGH, "strtod for <%s> failed", str);
    } else {
	item->double_sized = true;
	item->as_double_int = (item->as_double == floorl(item->as_double));
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
	item->float_sized = false;
	dbg(DBG_VVVHIGH, "strtof for <%s> failed", str);
    } else {
	item->float_sized = true;
	item->as_float_int = (item->as_longdouble == floorl(item->as_longdouble));
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%g: %.22g", str, item->as_float);
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%e: %.22e", str, item->as_float);
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%f: %.22f", str, item->as_float);
	dbg(DBG_VVVHIGH, "strtof returned an integer value: %s", booltostr(item->as_float_int));
    }

    /*
     * processing was successful
     */
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
 * returns:
 *	true ==> debug output was successful.
 *	false ==> all debug output was disallowed or some (or all) output failed
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
bool
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
	return false;
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
    allowed = json_vdbg(json_dbg_lvl, name, fmt, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;

    return allowed;
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
 * returns:
 *	true ==> debug output was successful.
 *	false ==> all debug output was disallowed or some (or all) output failed
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
bool
json_vdbg(int json_dbg_lvl, char const *name, char const *fmt, va_list ap)
{
    int saved_errno = 0;	/* errno at function start */
    bool allowed = false;	/* assume debug message not allowed */
    bool output_ok = true;	/* false ==> some debug output failed */
    int ret;			/* libc function return code */

    /*
     * determine if JSON debug messages are allowed
     */
    allowed = json_dbg_allowed(json_dbg_lvl);
    if (allowed == false) {
	return false;
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
	output_ok = false;	/* indicate output failure */
	warn(__func__, "fprintf returned errno: %d: (%s)", errno, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	output_ok = false;	/* indicate output failure */
	warn(__func__, "vfprintf returned errno: %d: (%s)", errno, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	output_ok = false;	/* indicate output failure */
	warn(__func__, "fputc returned errno: %d: (%s)", errno, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	output_ok = false;	/* indicate output failure */
	warn(__func__, "fflush returned errno: %d: (%s)", errno, strerror(errno));
    }

    /*
     * if some output was NOT OK, indicate by setting allowed to false
     */
    if (output_ok == false) {
	allowed = false;
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return allowed;
}


/*
 * jwarn - issue a JSON warning message
 *
 * given:
 *	name	    name of function issuing the warning
 *	filename    file with the problem (can be stdin)
 *	line	    JSON line
 *	line_num    the offending line number in the json file
 *	fmt	    format of the warning
 *	...	    optional format args
 *
 * Example:
 *
 *	jwarn(__func__, file, line, __LINE__, "unexpected foobar: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function returns true if the warning message is not ignored; else it
 * will return false.
 */
bool
jwarn(char const *name, char const *filename,
      char const *line, int line_num, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int ret = 0;		/* libc function return code */
    int saved_errno = 0;	/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * do nothing if the JSON warning is not allowed
     */
    allowed = json_warn_allowed();
    if (allowed == false) {
	return false;
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
	warn(__func__, "called with NULL name, forcing name: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "called with NULL fmt, forcing fmt: %s", fmt);
    }
    if (line == NULL) {
	/* currently line will be NULL so we make it empty */
	line = "";
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "%s(): called with NULL filename, forcing filename: %s\n",
		       __func__, filename);
    }


    ret = fprintf(stderr, "# %s\n", name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "%s: %d: ", filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return true;
}


/*
 * jwarnp - issue a JSON warning message with errno information
 *
 * given:
 *	name	    name of function issuing the warning
 *	filename    file with the problem (can be stdin)
 *	line	    JSON line
 *	line_num    the offending line number in the json file
 *	fmt	    format of the warning
 *	...	    optional format args
 *
 * Example:
 *
 *	jwarnp(__func__, file, line, __LINE__, "unexpected foobar: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function returns true if the warning message is not ignored; else it
 * will return false.
 */
bool
jwarnp(char const *name, char const *filename,
       char const *line, int line_num, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int ret = 0;		/* libc function return code */
    int saved_errno = 0;	/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * do nothing if the JSON warning is not allowed
     */
    allowed = json_warn_allowed();
    if (allowed == false) {
	return false;
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
	warn(__func__, "called with NULL name, forcing name: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "called with NULL fmt, forcing fmt: %s", fmt);
    }
    if (line == NULL) {
	/* currently line will be NULL so we make it empty */
	line = "";
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "in %s(): called with NULL filename, forcing filename: %s\n",
		       __func__, filename);
    }


    errno = 0;
    ret = fprintf(stderr, "# %s\n", name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "%s: %d: ", filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fprintf with errno returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n",
			       __func__, name, filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return true;
}


/*
 * jerr - issue a fatal JSON error message and exit
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the error
 *	filename	file with the problem (can be stdin)
 *	line		line with the problem (or NULL)
 *	line_num	line number with the problem or -1
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	jerr(JSON_CODE(1), __func__, file, line, line_num, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function does not return.
 */
void
jerr(int exitcode, char const *name, char const *filename,
     char const *line, int line_num, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int ret;			/* libc function return code */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * determine if conditions allow JSON error message, exit if not
     */
    allowed = json_err_allowed();
    if (allowed == false) {
	exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	not_reached();
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "called with exitcode <0: %d", exitcode);
	exitcode = 255;
	warn(__func__, "forcing exit code: %d", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "called with NULL name, forcing name: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "called with NULL fmt, forcing fmt: %s", fmt);
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "in %s(): called with NULL filename, forcing filename: %s\n",
		       __func__, filename);
    }
    if (line == NULL) {
	line = "";
	dbg(DBG_VHIGH, "in %s(): called with NULL line, making \"\"",
		       __func__);
    }

    errno = 0;
    ret = fprintf(stderr, "# %s\n", name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "JSON[%04d]: %s: %d: ", exitcode, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "vfprintf returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * jerrp - issue a fatal error message with errno information and exit
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	filename	file with the problem (can be stdin)
 *	line		line with the problem or NULL
 *	line_num	line number of the problem or -1
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	jerrp(JSON_CODE(1), __func__, file, line, line_num, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function does not return.
 */
void
jerrp(int exitcode, char const *name, char const *filename,
      char const *line, int line_num, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int ret = 0;		/* libc function return code */
    int saved_errno = 0;	/* errno value when called */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * determine if conditions allow JSON error message, exit if not
     */
    allowed = json_err_allowed();
    if (allowed == false) {
	exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	not_reached();
    }

    /*
     * save errno in case we need it for strerror()
     */
    saved_errno = errno;

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__func__, "called with exitcode <0: %d", exitcode);
	exitcode = 255;
	warn(__func__, "forcing exit code: %d", exitcode);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "called with NULL name, forcing name: %s", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "called with NULL fmt, forcing fmt: %s", fmt);
    }
    if (filename == NULL) {
	filename = "((NULL))";
	warn(__func__, "called with NULL filename, forcing filename: %s", filename);
    }
    if (line == NULL) {
	line = "";
	warn(__func__, "called with NULL line, making \"\"");
    }


    errno = 0;
    ret = fprintf(stderr, "# %s\n", name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "JSON[%04d]: %s: %d: ", exitcode, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "vfprintf returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in %s(%d, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n",
			       __func__, exitcode, name, filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * json_item_type_name - print a struct json item union type by name
 *
 * given:
 *	json_type   one of the values of the JTYPE_ enum
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
	switch (node->type) {
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
	    warn(__func__, "called with unknown JSON type: %d", node->type);
	    break;
	}
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
json_free(struct json *node, int depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }
    if (depth < 0) {
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
vjson_free(struct json *node, int depth, va_list ap)
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

    case JTYPE_MEMBER:	/* JSON item is a member */
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

    case JTYPE_OBJECT:	/* JSON item is a { members } */
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

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] */
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

    case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values */
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
 *	max_depth   maximum tree depth to descend, or <0 ==> infinite depth
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
json_tree_free(struct json *node, int max_depth, ...)
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
json_fprint(struct json *node, int depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }
    if (depth < 0) {
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
vjson_fprint(struct json *node, int depth, va_list ap)
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
    if (depth < 0) {
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
	return;
    }
    json_dbg_lvl = va_arg(ap2, int);

    /*
     * check JSON debug level if allowed
     */
    if (dbg_output_allowed == false ||
        (json_dbg_lvl != JSON_DBG_FORCED && json_dbg_lvl > json_verbosity_level)) {
	/* tree output disabled by json_verbosity_level */
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
	return;
    }
    fprint(stream, "lvl: %d\ttype: %s", depth, tname);

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

    case JTYPE_MEMBER:	/* JSON item is a member */
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

    case JTYPE_OBJECT:	/* JSON item is a { members } */
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

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] */
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

    case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values */
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
 *	max_depth   maximum tree depth to descend, or <0 ==> infinite depth
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
json_tree_print(struct json *node, int max_depth, ...)
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
 *	max_depth   maximum tree depth to descend, or <0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *	...	extra args are ignored, required extra args:
 *
 *		stream	    stream to print on
 *		json_dbg_lvl   print message if JSON_DBG_FORCED
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
 *	max_depth	maximum tree depth to descend, or <0 ==> infinite depth
 *			    NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *	stream		stream on which to print
 *
 * returns:
 *	true ==> debug output was successful.
 *	false ==> all debug output was disallowed or some (or all) output failed
 */
bool
json_dbg_tree_print(int json_dbg_lvl, char const *name, struct json *tree, int max_depth)
{
    int saved_errno = 0;	/* errno at function start */
    bool allowed = false;	/* assume debug message not allowed */
    bool output_ok = true;	/* false ==> some debug output failed */

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
	return false;
    }

    /*
     * print the debug message if allowed and allowed by the verbosity level
     */
    allowed = json_dbg_allowed(json_dbg_lvl);
    if (allowed == true) {
	json_tree_print(tree, max_depth, stderr, json_dbg_lvl);
    }

    /*
     * if some output was NOT OK, indicate by setting allowed to false
     */
    if (output_ok == false) {
	allowed = false;
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return allowed;
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
 *	max_depth   maximum tree depth to descend, or <0 ==> infinite depth
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
 * If max_depth is >= 0 and the tree depth > max_depth, then this function return.
 * In this case it will NOT operate on the node, or will be descend and further
 * into the tree.
 *
 * NOTE: This function warns but does not do anything if an arg is NULL.
 */
void
json_tree_walk(struct json *node, int max_depth, void (*vcallback)(struct json *, int, va_list), ...)
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
 *	max_depth   maximum tree depth to descend, or <0 ==> infinite depth
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
vjson_tree_walk(struct json *node, int max_depth, int depth, void (*vcallback)(struct json *, int, va_list), va_list ap)
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
    if (max_depth >= 0 && depth > max_depth) {
	warn(__func__, "tree walk descent stopped, tree depth: %d > max_depth: %d", depth, max_depth);
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
	errp(211, __func__, "calloc #0 error allocating %ju bytes", (uintmax_t)sizeof(*ret));
	not_reached();
    }

    /*
     * initialize the JSON parse tree element
     */
    ret->type = type;
    ret->parent = NULL;
    json_dbg(JSON_DBG_VHIGH, __func__, "allocated json with type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_number - convert JSON number string to C numeric value
 *
 * A JSON number string is of the form:
 *
 *      ({JSON_INTEGER}|{JSON_INTEGER}{JSON_FRACTION}|{JSON_INTEGER}{JSON_FRACTION}{JSON_EXPONENT}_
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
 *	allocated JSON parser tree node of the converted JSON number
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
    bool success = false;		    /* true ==> processing was successful */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_NUMBER);
    if (ret == NULL) {
	errp(212, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.number);
    item->as_str = NULL;
    item->first = NULL;
    item->converted = false;
    item->is_negative = false;
    item->is_floating = false;
    item->is_e_notation = false;
    /* integer values */
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
    item->as_float = 0.0;
    item->as_float_int = false;
    item->double_sized = false;
    item->as_double = 0.0;
    item->as_double_int = false;
    item->longdouble_sized = false;
    item->as_longdouble = 0.0;
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
	errp(213, __func__, "calloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
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
     * case: JSON number is a base 10 integer in ASCII
     */
    decimal = is_decimal(item->first, item->number_len);
    if (decimal == true) {

	/* process JSON number as a base 10 integer in ASCII */
	success = json_process_decimal(item, item->first, item->number_len);
	if (success == false) {
	    warn(__func__, "JSON number as base 10 integer in ASCII processing failed: <%*.*s>",
			   (int)item->number_len, (int)item->number_len, item->first);
	}

    /*
     * case: JSON number is not a base 10 integer in ASCII
     *
     * The JSON number might be a floating point or e-notation number.
     */
    } else {

	/* process JSON number as floating point or e-notation number */
	success = json_process_floating(item, item->first, item->number_len);
	if (success == false) {
	    warn(__func__, "JSON number as floating point or e-notation number failed: <%*.*s>",
			   (int)item->number_len, (int)item->number_len, item->first);
	}
    }
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}


/*
 * json_conv_number_str - convert JSON number string to C numeric value
 *
 * This is a simplified interface for json_conv_int().  See that function for details.
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
	err(214, __func__, "json_conv_number() returned NULL");
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
	errp(215, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.string);
    item->as_str = NULL;
    item->str = NULL;
    item->converted = false;
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
	warn(__func__, "called with NULL ptr");
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
	errp(216, __func__, "calloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    strncpy(item->as_str, ptr, len);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */

    /*
     * decode the JSON encoded string
     */
    /* decode the entire string */
    item->str = json_decode_str(item->as_str, &(item->str_len));
    if (item->str == NULL) {
	warn(__func__, "quote === %s: JSON string decode failed for: <%s>",
		       booltostr(quote), item->as_str);
	return ret;
    }
    item->converted = true;	/* JSON decoding successful */

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
     * determine if decoded JSON string is a C string
     */
    item->has_nul = is_string(item->str, item->str_len);

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
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_string() call
     */
    ret = json_conv_string(str, len, quote);
    if (ret == NULL) {
	err(217, __func__, "json_conv_string() returned NULL");
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
	errp(218, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.boolean);
    item->as_str = NULL;
    item->converted = false;
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
	errp(219, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
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
	item->value = true;
    } else if (strcmp(item->as_str, "false") == 0) {
	item->converted = true;
	item->value = false;
    } else {
	warn(__func__, "JSON boolean string neither true nor false: <%s>", item->as_str);
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
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_bool() call
     */
    ret = json_conv_bool(str, len);
    if (ret == NULL) {
	err(220, __func__, "json_conv_bool() returned NULL");
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
	errp(221, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.null);
    item->as_str = NULL;
    item->value = NULL;

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
	errp(222, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
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
	item->value = NULL;
    } else {
	warn(__func__, "JSON null string is not null: <%s>", item->as_str);
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
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_null() call
     */
    ret = json_conv_null(str, len);
    if (ret == NULL) {
	err(223, __func__, "json_conv_null() returned NULL");
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
    struct json_member *item = NULL;	    /* allocated JSON member */

    /*
     * allocate an initialized JSON parse tree item
     */
    ret = json_alloc(JTYPE_MEMBER);
    if (ret == NULL) {
	errp(224, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON item
     */
    item = &(ret->item.member);
    item->converted = false;
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

    /*
     * return the JSON parse tree element
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
	errp(225, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON object
     */
    item = &(ret->item.object);
    item->converted = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON objects
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(226, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;

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
	err(227, __func__, "node is NULL");
	not_reached();
    }
    if (member == NULL) {
	err(228, __func__, "member is NULL");
	not_reached();
    }
    if (node->type != JTYPE_OBJECT) {
	err(229, __func__, "node type expected to be JTYPE_OBJECT: %d found type: %d",
		           JTYPE_OBJECT, node->type);
	not_reached();
    }
    if (member->type != JTYPE_MEMBER) {
	err(230, __func__, "node type expected to be JTYPE_MEMBER: %d found type: %d",
		           JTYPE_MEMBER, node->type);
	not_reached();
    }

    /*
     * point to object
     */
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON object type: %s", json_item_type_name(node));
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON member type: %s", json_item_type_name(member));
    item = &(node->item.object);
    if (item->s == NULL) {
	err(231, __func__, "item->s is NULL");
	not_reached();
    }

    /*
     * link JSON parse tree child to this parent node
     */
    member->parent = node;

    /*
     * append member
     */
    moved = dyn_array_append_value(item->s, member);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s(): dyn_array_append_value moved data",
		      __func__);
    }

    /*
     * update accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return node;
}


/*
 * json_object_append_members - add a dynamic array of JSON members to a JSON object
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
 *	members	dynamic array of JSON members
 *
 * returns:
 *	true ==> JSON members added to JSON object
 *	false ==> wrong type of node, or
 *		  wrong type of members in dynamic members, or
 *		  NULL pointer
 *
 * NOTE: This function will not return on malloc error.
 */
bool
json_object_append_members(struct json *node, struct dyn_array *members)
{
    struct json_object *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */
    int i;

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return false;
    }
    if (members == NULL) {
	warn(__func__, "members is NULL");
	return false;
    }
    if (node->type != JTYPE_OBJECT) {
	warn(__func__, "node type expected to be JTYPE_OBJECT: %d found type: %d",
		       JTYPE_OBJECT, node->type);
	return false;
    }
    if (members->data == NULL) {
	warn(__func__, "dynamic array data is NULL");
	return false;
    }
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON object type: %s", json_item_type_name(node));
    for (i=0; i < dyn_array_tell(members); ++i) {
	json_dbg(JSON_DBG_VHIGH, __func__, "JSON members[%d] type: %s",
				         i, json_item_type_name(dyn_array_value(members, struct json *, i)));
	if (dyn_array_value(members, struct json *, i)->type != JTYPE_MEMBER) {
	    warn(__func__, "members[%d] node type expected to be JTYPE_MEMBER: %d found type: %d",
			   i, JTYPE_MEMBER, dyn_array_value(members, struct json *, i)->type);
	    return false;
	}
    }

    /*
     * point to object
     */
    item = &(node->item.object);
    if (item->s == NULL) {
	warn(__func__, "item->s is NULL");
	return false;
    }

    /*
     * link JSON parse tree children to this parent node
     */
    for (i=0; i < dyn_array_tell(members); ++i) {
	dyn_array_value(members, struct json *, i)->parent = node;
    }

    /*
     * append member
     */
    moved = dyn_array_concat_array(item->s, members);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s(): dyn_array_concat_array moved data",
		      __func__);
    }

    /*
     * update accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return true;
}


/*
 * json_create_elements - allocate an emepty JSON elements
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
	errp(232, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON elements
     */
    item = &(ret->item.array);
    item->converted = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON values
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(233, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;

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
 *	value	JSON node of the JSON value
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
	err(234, __func__, "node is NULL");
	not_reached();
    }
    if (value == NULL) {
	err(235, __func__, "value is NULL");
	not_reached();
    }
    if (node->type != JTYPE_ELEMENTS) {
	err(236, __func__, "node type expected to be JTYPE_ELEMENTS: %d found type: %d",
			   JTYPE_ELEMENTS, node->type);
	not_reached();
    }
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON item type: %s", json_item_type_name(node));
    switch (value->type) {
    case JTYPE_NUMBER:
    case JTYPE_STRING:
    case JTYPE_BOOL:
    case JTYPE_MEMBER:
    case JTYPE_OBJECT:
    case JTYPE_ARRAY:
	json_dbg(JSON_DBG_VHIGH, __func__, "JSON item type: %s", json_item_type_name(value));
	break;
    case JTYPE_ELEMENTS:
	err(237, __func__, "JSON type %s (type: %d) is invalid here",
		json_item_type_name(node), JTYPE_ELEMENTS);
	not_reached();
    default:
	err(238, __func__, "expected JSON item, array, string, number, boolean or null, found type: %d",
			   value->type);
	not_reached();
    }

    /*
     * point to array
     */
    item = &(node->item.elements);
    if (item->s == NULL) {
	err(239, __func__, "item->s is NULL");
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


#if 0 /* XXX - code likely not needed - remove this section if that is the case - XXX */
/*
 * json_array_append_values - add a dynamic array of JSON values to a JSON array
 *
 * JSON array is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON array, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * given:
 *	node	JSON node of the JSON array being added to
 *	values	dynamic array of JSON values
 *
 * returns:
 *	true ==> JSON values added to JSON array
 *	false ==> wrong type of node, or
 *		  wrong type of values, or
 *		  NULL pointer
 *
 * NOTE: This function will not return on malloc error.
 */
bool
json_array_append_values(struct json *node, struct dyn_array *values)
{
    struct json_array *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */
    int i;

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return false;
    }
    if (values == NULL) {
	warn(__func__, "values is NULL");
	return false;
    }
    if (values->data == NULL) {
	warn(__func__, "dynamic array data is NULL");
	return false;
    }
    if (node->type != JTYPE_ARRAY) {
	warn(__func__, "node type expected to be JTYPE_ARRAY: %d found type: %d",
		       JTYPE_ARRAY, node->type);
	return false;
    }
    json_dbg(JSON_DBG_VHIGH, __func__, "JSON object type: %s", json_element_type_name(node));
    for (i=0; i < dyn_array_tell(values); ++i) {
	json_dbg(JSON_DBG_VHIGH, __func__, "JSON values[%d] type: %s",
			       i, json_element_type_name(dyn_array_value(values, struct json *, i)));
	switch (dyn_array_value(values, struct json *, i)->type) {
	case JTYPE_NUMBER:
	case JTYPE_STRING:
	case JTYPE_BOOL:
	case JTYPE_MEMBER:
	case JTYPE_OBJECT:
	case JTYPE_ARRAY:
	    break;
	default:
	    warn(__func__, "expected JSON values[%d], array, string, number, boolean or null, found type: %d",
			   i, dyn_array_value(values, struct json *, i)->type);
	    return false;
	}
    }

    /*
     * point to array
     */
    item = &(node->element.array);
    if (item->s == NULL) {
	warn(__func__, "item->s is NULL");
	return false;
    }

    /*
     * link JSON parse tree child to this parent node
     */
    for (i=0; i < dyn_array_tell(values); ++i) {
	dyn_array_value(values, struct json *, i)->parent = node;
    }

    /*
     * append value
     */
    moved = dyn_array_concat_array(item->s, values);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s(): dyn_array_concat_array moved data",
		      __func__);
    }

    /*
     * update accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return true;
}
#endif /* XXX - code likely not needed - remove this section if that is the case - XXX */


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
	errp(240, __func__, "json_alloc() returned NULL");
	not_reached();
    }

    /*
     * initialize the JSON array
     */
    item = &(ret->item.array);
    item->converted = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON arrays
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(241, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;

    json_dbg(JSON_DBG_VHIGH, __func__, "JSON return type: %s", json_item_type_name(ret));

    /*
     * return the JSON parse tree item
     */
    return ret;
}
