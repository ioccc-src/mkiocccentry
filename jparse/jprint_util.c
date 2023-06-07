/*
 * jprint_util - utility functions for JSON printer jprint
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool is being developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * The JSON parser was co-developed in 2022 by Cody and Landon.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#include "jprint_util.h"

/*
 * jprint_match_none	- if no types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types == 0.
 */
bool
jprint_match_none(uintmax_t types)
{
    return types == JPRINT_TYPE_NONE;
}

/*
 * jprint_match_int	- if ints should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_INT set.
 */
bool
jprint_match_int(uintmax_t types)
{
    return types & JPRINT_TYPE_INT;
}
/*
 * jprint_match_float	- if floats should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_FLOAT set.
 */
bool
jprint_match_float(uintmax_t types)
{
    return types & JPRINT_TYPE_FLOAT;
}
/*
 * jprint_match_exp	- if exponents should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_EXP set.
 */
bool
jprint_match_exp(uintmax_t types)
{
    return types & JPRINT_TYPE_EXP;
}
/*
 * jprint_match_bool	- if booleans should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_BOOL set.
 */
bool
jprint_match_bool(uintmax_t types)
{
    return types & JPRINT_TYPE_BOOL;
}
/*
 * jprint_match_string	    - if strings should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_STR set.
 */
bool
jprint_match_string(uintmax_t types)
{
    return types & JPRINT_TYPE_STR;
}
/*
 * jprint_match_null	- if null should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_NULL set.
 */
bool
jprint_match_null(uintmax_t types)
{
    return types & JPRINT_TYPE_NULL;
}
/*
 * jprint_match_object	    - if objects should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_OBJECT set.
 */
bool
jprint_match_object(uintmax_t types)
{
    return types & JPRINT_TYPE_OBJECT;
}
/*
 * jprint_match_array	    - if arrays should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_ARRAY set.
 */
bool
jprint_match_array(uintmax_t types)
{
    return types & JPRINT_TYPE_ARRAY;
}
/*
 * jprint_match_any	- if any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has any type set.
 */
bool
jprint_match_any(uintmax_t types)
{
    return types & JPRINT_TYPE_ANY;
}
/*
 * jprint_match_simple	- if simple types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Simple is defined as a number, a bool, a string or a null.
 *
 * Returns true if types has JPRINT_TYPE_SIMPLE set.
 */
bool
jprint_match_simple(uintmax_t types)
{
    return types & JPRINT_TYPE_SIMPLE;
}
/*
 * jprint_match_compound   - if compounds should match
 *
 * given:
 *
 *	types	- types set
 *
 * A compound is defined as an object or array.
 *
 * Returns true if types has JPRINT_TYPE_COMPOUND set.
 */
bool
jprint_match_compound(uintmax_t types)
{
    return types & JPRINT_TYPE_COMPOUND;
}

/*
 * jprint_parse_types_option	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JPRINT_TYPE_SIMPLE (as if '-t simple').
 */
uintmax_t
jprint_parse_types_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */

    uintmax_t type = JPRINT_TYPE_SIMPLE; /* default is simple: num, bool, str and null */

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    }

    /*
     * Go through comma-separated list of types, setting each as a bitvector
     *
     * NOTE: the way this is done might change if it proves there is a better
     * way (and there might be - I've thought of a number of ways already).
     */
    for (p = strtok_r(optarg, ",", &saveptr); p; p = strtok_r(NULL, ",", &saveptr)) {
	if (!strcmp(p, "int")) {
	    type |= JPRINT_TYPE_INT;
	} else if (!strcmp(p, "float")) {
	    type |= JPRINT_TYPE_FLOAT;
	} else if (!strcmp(p, "exp")) {
	    type |= JPRINT_TYPE_EXP;
	} else if (!strcmp(p, "num")) {
	    type |= JPRINT_TYPE_NUM;
	} else if (!strcmp(p, "bool")) {
	    type |= JPRINT_TYPE_BOOL;
	} else if (!strcmp(p, "str")) {
	    type |= JPRINT_TYPE_STR;
	} else if (!strcmp(p, "null")) {
	    type |= JPRINT_TYPE_NULL;
	} else if (!strcmp(p, "object")) {
	    type |= JPRINT_TYPE_OBJECT;
	} else if (!strcmp(p, "array")) {
	    type |= JPRINT_TYPE_ARRAY;
	} else if (!strcmp(p, "simple")) {
	    type |= JPRINT_TYPE_SIMPLE;
	} else if (!strcmp(p, "compound")) {
	    type |= JPRINT_TYPE_COMPOUND;
	} else if (!strcmp(p, "any")) {
	    type |= JPRINT_TYPE_ANY;
	} else {
	    /* unknown type */
	    err(6, __func__, "unknown type '%s'", p);
	    not_reached();
	}
    }

    return type;
}

/*
 * jprint_print_name	- if only names should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JPRINT_PRINT_NAME set.
 */
bool
jprint_print_name(uintmax_t types)
{
    return (types & JPRINT_PRINT_NAME) && !(types & JPRINT_PRINT_VALUE);
}
/*
 * jprint_print_value	- if only values should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JPRINT_PRINT_VALUE set.
 */
bool
jprint_print_value(uintmax_t types)
{
    return (types & JPRINT_PRINT_VALUE) && !(types & JPRINT_PRINT_NAME);
}
/*
 * jprint_print_both	- if names AND values should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types has JPRINT_PRINT_BOTH set.
 */
bool
jprint_print_name_value(uintmax_t types)
{
    return types == JPRINT_PRINT_BOTH;
}


/*
 * jprint_parse_print_option	- parse -p option list
 *
 * given:
 *
 *	optarg	    - option argument to -p option
 *
 * Returns: bitvector of types to print.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JPRINT_PRINT_VALUE (as if '-p v').
 */
uintmax_t
jprint_parse_print_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */

    uintmax_t print_types = 0; /* default is to print values */

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return JPRINT_PRINT_VALUE;
    }

    /*
     * Go through comma-separated list of what to print, setting each as a bitvector
     *
     * NOTE: the way this is done might change if it proves there is a better
     * way (and there might very well be).
     */
    for (p = strtok_r(optarg, ",", &saveptr); p; p = strtok_r(NULL, ",", &saveptr)) {
	if (!strcmp(p, "v") || !strcmp(p, "value")) {
	    print_types |= JPRINT_PRINT_VALUE;
	} else if (!strcmp(p, "n") || !strcmp(p, "name")) {
	    print_types |= JPRINT_PRINT_NAME;
	} else if (!strcmp(p, "b") || !strcmp(p, "both")) {
	    print_types |= JPRINT_PRINT_BOTH;
	} else {
	    /* unknown keyword */
	    err(7, __func__, "unknown keyword '%s'", p);
	    not_reached();
	}
    }

    if (jprint_print_name_value(print_types)) {
	dbg(DBG_NONE, "will print both name and value");
    }
    else if (jprint_print_name(print_types)) {
	dbg(DBG_NONE, "will only print name");
    }
    else if (jprint_print_value(print_types)) {
	dbg(DBG_NONE, "will only print value");
    }
    return print_types;
}

/* jprint_parse_number_range	- parse a number range for options -l, -N, -n
 *
 * given:
 *
 *	option	    - option string (e.g. "-l"). Used for error and debug messages.
 *	optarg	    - the option argument
 *	number	    - pointer to struct number
 *
 * Returns true if successfully parsed.
 *
 * The following rules apply:
 *
 * (0) an exact number is a number optional arg by itself e.g. -l 5 or -l5.
 * (1) an inclusive range is <min>:<max> e.g. -l 5:10
 *     (1a) the minimum must be <= the max
 * (2) a minimum number, that is num >= minimum, is <num>:
 * (3) a maximum number, that is num <= maximum, is :<num>
 * (4) anything else is an error
 *
 * See also the structs jprint_number_range and jprint_number in jprint_util.h
 * for more details.
 *
 * NOTE: currently (as of 7 June 2023) the numbers are signed. This might or
 * might not change depending on what is needed.
 *
 * NOTE: this function does not return on syntax error or NULL number.
 */
bool
jprint_parse_number_range(const char *option, char *optarg, struct jprint_number *number)
{
    intmax_t max = 0;
    intmax_t min = 0;

    /* firewall */
    if (number == NULL) {
	err(8, __func__, "NULL number struct for option %s", option);
	not_reached();
    } else {
	memset(number, 0, sizeof(struct jprint_number));

	/* don't assume everything is 0 */
	number->exact = false;
	number->range.min = 0;
	number->range.max = 0;
	number->range.inclusive = false;
	number->range.less_than_equal = false;
	number->range.greater_than_equal = false;
    }

    if (optarg == NULL || *optarg == '\0') {
	warn(__func__, "NULL or empty optarg for %s, ignoring", option);
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
	    dbg(DBG_NONE, "exact number required for option %s: %jd", option, number->number);
	} else {
	    err(9, __func__, "invalid number for option %s: <%s>", option, optarg);
	    not_reached();
	}
    } else if (sscanf(optarg, "%jd:%jd", &min, &max) == 2) {
	if (min > max) {
	    err(10, __func__, "invalid inclusive range for option %s: min > max: %jd > %jd", option, min, max);
	    not_reached();
	}
	number->range.min = min;
	number->range.max = max;
	number->range.inclusive = true;
	number->range.less_than_equal = false;
	number->range.greater_than_equal = false;
	dbg(DBG_NONE, "number range inclusive required for option %s: >= %jd && <= %jd", option, number->range.min, number->range.max);
    } else if (sscanf(optarg, "%jd:", &min) == 1) {
	number->number = 0;
	number->exact = false;
	number->range.min = min;
	number->range.max = number->range.min;
	number->range.greater_than_equal = true;
	number->range.less_than_equal = false;
	number->range.inclusive = false;
	dbg(DBG_NONE, "minimum number required for option %s: must be >= %jd", option, number->range.min);
    } else if (sscanf(optarg, ":%jd", &max) == 1) {
	number->range.max = max;
	number->range.min = number->range.max;
	number->number = 0;
	number->exact = false;
	number->range.less_than_equal = true;
	number->range.greater_than_equal = false;
	number->range.inclusive = false;
	dbg(DBG_NONE, "maximum number required for option %s: must be <= %jd", option, number->range.max);
    } else {
	err(11, __func__, "number range syntax error for option %s: <%s>", option, optarg);
	not_reached();
    }

    return true;
}
