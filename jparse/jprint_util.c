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
 * Returns true if types is equal to JPRINT_TYPE_ANY.
 *
 * Why does it have to equal JPRINT_TYPE_ANY if it checks for any type? Because
 * the point is that if JPRINT_TYPE_ANY is set it can be any type but not
 * specific types. For the specific types those bits have to be set instead. If
 * JPRINT_TYPE_ANY is set then any type can be set but if types is say
 * JPRINT_TYPE_INT then checking for JPRINT_TYPE_INT & JPRINT_TYPE_ANY would be
 * != 0 (as it's a bitwise OR Of all the types) which would suggest that any
 * type is okay even if JPRINT_TYPE_INT was the only type.
 */
bool
jprint_match_any(uintmax_t types)
{
    return types == JPRINT_TYPE_ANY;
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
    char *dup = NULL;	    /* strdup()d copy of optarg */

    uintmax_t type = JPRINT_TYPE_SIMPLE; /* default is simple: num, bool, str and null */

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    } else {
	/* pre-clear errno for errp() */
	errno = 0;
	dup = strdup(optarg);
	if (dup == NULL) {
	    err(7, __func__, "strdup(%s) failed", optarg);
	    not_reached();
	}
    }

    /*
     * Go through comma-separated list of types, setting each as a bitvector
     *
     * NOTE: the way this is done might change if it proves there is a better
     * way (and there might be - I've thought of a number of ways already).
     */
    for (p = strtok_r(dup, ",", &saveptr); p; p = strtok_r(NULL, ",", &saveptr)) {
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
	    err(3, __func__, "unknown type '%s'", p); /*ooo*/
	    not_reached();
	}
    }

    if (dup != NULL) {
	free(dup);
	dup = NULL;
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
    char *dup = NULL;	    /* strdup()d copy of optarg */

    uintmax_t print_types = 0; /* default is to print values */

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return JPRINT_PRINT_VALUE;
    }

    errno = 0; /* pre-clear errno for errp() */
    dup = strdup(optarg);
    if (dup == NULL) {
	err(8, __func__, "strdup(%s) failed", optarg);
	not_reached();
    }

    /*
     * Go through comma-separated list of what to print, setting each as a bitvector
     *
     * NOTE: the way this is done might change if it proves there is a better
     * way (and there might very well be).
     */
    for (p = strtok_r(dup, ",", &saveptr); p; p = strtok_r(NULL, ",", &saveptr)) {
	if (!strcmp(p, "v") || !strcmp(p, "value")) {
	    print_types |= JPRINT_PRINT_VALUE;
	} else if (!strcmp(p, "n") || !strcmp(p, "name")) {
	    print_types |= JPRINT_PRINT_NAME;
	} else if (!strcmp(p, "b") || !strcmp(p, "both")) {
	    print_types |= JPRINT_PRINT_BOTH;
	} else {
	    /* unknown keyword */
	    err(3, __func__, "unknown keyword '%s'", p); /*ooo*/
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

    if (dup != NULL) {
	free(dup);
	dup = NULL;
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
 * (1) an inclusive range is <min>:<max> e.g. -l 5:10 where:
 *     (1a) the last number can be negative in which case it's up through the
 *	    count - max.
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
	err(3, __func__, "NULL number struct for option %s", option); /*ooo*/
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
	    err(3, __func__, "invalid number for option %s: <%s>", option, optarg); /*ooo*/
	    not_reached();
	}
    } else if (sscanf(optarg, "%jd:%jd", &min, &max) == 2) {
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
	dbg(DBG_NONE, "number range inclusive required for option %s: >= %jd && <= %jd", option, number->range.min,
		number->range.max);
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
	err(3, __func__, "number range syntax error for option %s: <%s>", option, optarg);/*ooo*/
	not_reached();
    }

    return true;
}

/* jprint_number_in_range   - check if number is in required range
 *
 * given:
 *
 *	number		- number to check
 *	total_matches	- total number of matches found
 *	range		- pointer to struct jprint_number with range
 *
 * Returns true if the number is in range.
 *
 * NOTE: if range is NULL it will return false.
 */
bool
jprint_number_in_range(intmax_t number, intmax_t total_matches, struct jprint_number *range)
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
