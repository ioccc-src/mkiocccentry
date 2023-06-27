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

/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */

#include "jprint_util.h"

/* alloc_jprint	    - allocate and clear out a struct jprint *
 *
 * This function returns a newly allocated and cleared struct jprint *.
 *
 * This function will never return a NULL pointer.
 */
struct jprint *
alloc_jprint(void)
{
    /* allocate our struct jprint */
    struct jprint *jprint = calloc(1, sizeof *jprint);

    /* verify jprint != NULL */
    if (jprint == NULL) {
	err(22, "jprint", "failed to allocate jprint struct");
	not_reached();
    }

    /* clear everything out explicitly even after calloc() */

    jprint->is_stdin = false;			/* if it's stdin */
    jprint->match_found = false;		/* true if a pattern is specified and there is a match */
    jprint->ignore_case = false;		/* true if -i, case-insensitive */
    jprint->pattern_specified = false;		/* true if a pattern was specified */
    jprint->encode_strings = false;		/* -e used */
    jprint->quote_strings = false;		/* -Q used */
    jprint->type_specified = false;		/* -t used */
    jprint->type = JPRINT_TYPE_SIMPLE;		/* -t type specified, default simple */

    /* number range options, see struct jprint_number_range in jprint_util.h for details */

    /* max matches number range */
    jprint->jprint_max_matches.number = 0;
    jprint->jprint_max_matches.exact = false;
    jprint->jprint_max_matches.range.min = 0;
    jprint->jprint_max_matches.range.max = 0;
    jprint->jprint_max_matches.range.less_than_equal = false;
    jprint->jprint_max_matches.range.greater_than_equal = false;
    jprint->jprint_max_matches.range.inclusive = false;
    jprint->max_matches_requested = false;

    /* min matches number range */
    jprint->jprint_min_matches.number = 0;
    jprint->jprint_min_matches.exact = false;
    jprint->jprint_min_matches.range.min = 0;
    jprint->jprint_min_matches.range.max = 0;
    jprint->jprint_min_matches.range.less_than_equal = false;
    jprint->jprint_min_matches.range.greater_than_equal = false;
    jprint->jprint_min_matches.range.inclusive = false;
    jprint->min_matches_requested = false;

    /* levels number range */
    jprint->jprint_levels.number = 0;
    jprint->jprint_levels.exact = false;
    jprint->jprint_levels.range.min = 0;
    jprint->jprint_levels.range.max = 0;
    jprint->jprint_levels.range.less_than_equal = false;
    jprint->jprint_levels.range.greater_than_equal = false;
    jprint->jprint_levels.range.inclusive = false;
    jprint->levels_constrained = false;

    /* file contents */
    jprint->file_contents = NULL;			/* file.json contents */
    /* JSON FILE * */
    jprint->json_file = NULL;				/* JSON file */

    jprint->print_type = JPRINT_PRINT_VALUE;		/* -p type specified */
    jprint->print_type_option = false;			/* -p explicitly used */
    jprint->print_token_spaces = false;			/* -b specified */
    jprint->num_token_spaces = 1;			/* -b specified number of spaces or tabs */
    jprint->print_token_tab = false;			/* -b tab (or -b <num>[t]) specified */
    jprint->print_json_levels = false;			/* -L specified */
    jprint->num_level_spaces = 0;			/* number of spaces or tab for -L */
    jprint->print_level_tab = false;			/* -L tab option */
    jprint->print_colons = false;			/* -P specified */
    jprint->print_final_comma = false;			/* -C specified */
    jprint->print_braces = false;			/* -B specified */
    jprint->indent_levels = false;			/* -I used */
    jprint->indent_spaces = 0;				/* -I number of tabs or spaces */
    jprint->indent_tab = false;				/* -I <num>[{t|s}] specified */
    jprint->print_syntax = false;			/* -j used, will imply -p b -b 1 -c -e -Q -I 4 -t any */
    jprint->match_encoded = false;			/* -E used, match encoded name */
    jprint->use_substrings = false;			/* -s used, matching substrings okay */
    jprint->use_regexps = false;			/* -g used, allow grep-like regexps */
    jprint->count_only = false;				/* -c used, only show count */
    jprint->print_entire_file = false;			/* no name_arg specified */
    jprint->max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */

    jprint->search_value = false;			/* -Y search by value, not name. Uses print type */

    jprint->check_tool_stream = NULL;			/* FILE * stream for -S path */
    jprint->check_tool_path = NULL;			/* -S path */
    jprint->check_tool_args = NULL;			/* -A args */

    /* finally the linked list of patterns for matches */
    jprint->patterns = NULL;
    jprint->number_of_patterns = 0;
    jprint->total_matches = 0;

    return jprint;
}


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
 */
bool
jprint_match_int(uintmax_t types)
{
    return (types & JPRINT_TYPE_INT) != 0;
}
/*
 * jprint_match_float	- if floats should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_FLOAT set.
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
 */
bool
jprint_match_float(uintmax_t types)
{
    return (types & JPRINT_TYPE_FLOAT) != 0;
}
/*
 * jprint_match_exp	- if exponents should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_EXP set.
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
 */
bool
jprint_match_exp(uintmax_t types)
{
    return (types & JPRINT_TYPE_EXP) != 0;
}
/*
 * jprint_match_num	- if numbers of any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_NUM (or any of the number types) set.
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
 */
bool
jprint_match_num(uintmax_t types)
{
    return ((types & JPRINT_TYPE_NUM)||(types & JPRINT_TYPE_INT) || (types & JPRINT_TYPE_FLOAT) ||
	    (types & JPRINT_TYPE_EXP))!= 0;
}
/*
 * jprint_match_bool	- if booleans should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_BOOL set.
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
 */
bool
jprint_match_bool(uintmax_t types)
{
    return (types & JPRINT_TYPE_BOOL) != 0;
}
/*
 * jprint_match_string	    - if strings should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_STR set.
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
 */
bool
jprint_match_string(uintmax_t types)
{
    return (types & JPRINT_TYPE_STR) != 0;
}
/*
 * jprint_match_null	- if null should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_NULL set.
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
 */
bool
jprint_match_null(uintmax_t types)
{
    return (types & JPRINT_TYPE_NULL) != 0;
}
/*
 * jprint_match_object	    - if objects should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_OBJECT set.
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
 */
bool
jprint_match_object(uintmax_t types)
{
    return (types & JPRINT_TYPE_OBJECT) != 0;
}
/*
 * jprint_match_array	    - if arrays should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JPRINT_TYPE_ARRAY set.
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
 */
bool
jprint_match_array(uintmax_t types)
{
    return (types & JPRINT_TYPE_ARRAY) != 0;
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
 * != 0 (as it's a bitwise OR of all the types) which would suggest that any
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
 */
bool
jprint_match_simple(uintmax_t types)
{
    return (types & JPRINT_TYPE_SIMPLE) != 0;
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
 */
bool
jprint_match_compound(uintmax_t types)
{
    return (types & JPRINT_TYPE_COMPOUND) != 0;
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
	    errp(23, __func__, "strdup(%s) failed", optarg);
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
 */
bool
jprint_print_name(uintmax_t types)
{
    return ((types & JPRINT_PRINT_NAME) && !(types & JPRINT_PRINT_VALUE)) != 0;
}
/*
 * jprint_print_value	- if only values should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JPRINT_PRINT_VALUE set.
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
 */
bool
jprint_print_value(uintmax_t types)
{
    return ((types & JPRINT_PRINT_VALUE) && !(types & JPRINT_PRINT_NAME)) != 0;
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
	errp(24, __func__, "strdup(%s) failed", optarg);
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
	dbg(DBG_LOW, "will print both name and value");
    }
    else if (jprint_print_name(print_types)) {
	dbg(DBG_LOW, "will only print name");
    }
    else if (jprint_print_value(print_types)) {
	dbg(DBG_LOW, "will only print value");
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
 * See also the structs jprint_number_range and jprint_number in jprint_util.h
 * for more details.
 *
 * NOTE: this function does not return on syntax error or NULL number.
 */
bool
jprint_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jprint_number *number)
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

/* jprint_parse_st_tokens_option    - parse -b [num]{s,t}/-b tab option
 *
 * This function parses the -b option. It's necessary to have it this way
 * because some options like -j imply it and rather than duplicate code we just
 * have it here once.
 *
 * given:
 *
 *	optarg		    - option argument to -b option (can be faked)
 *	num_token_spaces    - pointer to number of token spaces or tabs
 *	print_token_tab	    - pointer to boolean indicating if tab or spaces are to be used
 *
 * Function returns void.
 *
 * NOTE: syntax errors are an error just like it was when it was in main().
 *
 * NOTE: this function does not return on NULL pointers.
 */
void
jprint_parse_st_tokens_option(char *optarg, uintmax_t *num_token_spaces, bool *print_token_tab)
{
    char ch = '\0';	/* whether spaces or tabs are to be used, 's' or 't' */

    /* firewall checks */
    if (optarg == NULL || *optarg == '\0') {
	err(3, __func__, "NULL or empty optarg"); /*ooo*/
	not_reached();
    } else if (num_token_spaces == NULL) {
	err(3, __func__, "NULL num_token_spaces"); /*ooo*/
	not_reached();
    } else if (print_token_tab == NULL) {
	err(3, __func__, "NULL print_token_tab"); /*ooo*/
	not_reached();
    } else {
	/* ensure that the variables are empty */

	/* make *num_token_spaces == 0 */
	*num_token_spaces = 0;
	/* make *print_token_tab == false */
	*print_token_tab = false;
    }

    if (sscanf(optarg, "%ju%c", num_token_spaces, &ch) == 2) {
	if (ch == 't') {
	    *print_token_tab = true;
	    dbg(DBG_LOW, "will print %ju tab%s between name and value", *num_token_spaces,
		*num_token_spaces==1?"":"s");
	} else if (ch == 's') {
	    *print_token_tab = false;
	    dbg(DBG_LOW, "will print %ju space%s between name and value", *num_token_spaces,
		*num_token_spaces==1?"":"s");
	} else {
	    err(3, __func__, "syntax error for -b <num>[ts]"); /*ooo*/
	    not_reached();
	}
    } else if (!strcmp(optarg, "tab")) {
	*print_token_tab = true;
	*num_token_spaces = 1;
	dbg(DBG_LOW, "will print %ju tab%s between name and value", *num_token_spaces,
	    *num_token_spaces==1?"":"s");
    } else if (!string_to_uintmax(optarg, num_token_spaces)) {
	err(3, "jprint", "couldn't parse -b <num>[ts]"); /*ooo*/
	not_reached();
    } else {
	*print_token_tab = false; /* ensure it's false in case specified previously */
	dbg(DBG_LOW, "will print %jd space%s between name and value", *num_token_spaces,
		*num_token_spaces==1?"":"s");
    }
}

/* jprint_parse_st_indent_option    - parse -I [num]{s,t}/-b indent option
 *
 * This function parses the -I option. It's necessary to have it this way
 * because some options like -j imply it and rather than duplicate code we just
 * have it here once.
 *
 * given:
 *
 *	optarg		    - option argument to -b option (can be faked)
 *	indent_level	    - pointer to number of indent spaces or tabs
 *	indent_tab	    - pointer to boolean indicating if tab or spaces are to be used
 *
 * Function returns void.
 *
 * NOTE: syntax errors are an error just like it was when it was in main().
 *
 * NOTE: this function does not return on NULL pointers.
 */
void
jprint_parse_st_indent_option(char *optarg, uintmax_t *indent_level, bool *indent_tab)
{
    char ch = '\0';	/* whether spaces or tabs are to be used, 's' or 't' */

    /* firewall checks */
    if (optarg == NULL || *optarg == '\0') {
	err(3, __func__, "NULL or empty optarg"); /*ooo*/
	not_reached();
    } else if (indent_level == NULL) {
	err(3, __func__, "NULL indent_level"); /*ooo*/
	not_reached();
    } else if (indent_tab == NULL) {
	err(3, __func__, "NULL print_token_tab"); /*ooo*/
	not_reached();
    } else {
	/* ensure that the variables are empty */

	/* make *indent_level == 0 */
	*indent_level = 0;
	/* make *ident_tab == false */
	*indent_tab = false;
    }


    if (sscanf(optarg, "%ju%c", indent_level, &ch) == 2) {
	if (ch == 't') {
	    *indent_tab = true;
	    dbg(DBG_LOW, "will indent with %ju tab%s after levels", *indent_level, *indent_level==1?"":"s");
	} else if (ch == 's') {
	    *indent_tab = false; /* ensure it's false in case specified previously */
	    dbg(DBG_LOW, "will indent with %jd space%s after levels", *indent_level, *indent_level==1?"":"s");
	} else {
	    err(3, __func__, "syntax error for -I"); /*ooo*/
	    not_reached();
	}
    } else if (!strcmp(optarg, "tab")) {
	    *indent_tab = true;
	    *indent_level = 1;
	    dbg(DBG_LOW, "will indent with %ju tab%s after levels", *indent_level, *indent_level==1?"":"s");
    } else if (!string_to_uintmax(optarg, indent_level)) {
	err(3, "jprint", "couldn't parse -I spaces"); /*ooo*/
	not_reached();
    } else {
	*indent_tab = false; /* ensure it's false in case specified previously */
	dbg(DBG_LOW, "will ident with %jd space%s after levels", *indent_level, *indent_level==1?"":"s");
    }
}

/* jprint_parse_st_level_option    - parse -L [num]{s,t}/-b level option
 *
 * This function parses the -L option. It's necessary to have it this way
 * because some options like -j imply it and rather than duplicate code we just
 * have it here once.
 *
 * given:
 *
 *	optarg		    - option argument to -b option (can be faked)
 *	num_level_spaces    - pointer to number of spaces or tabs to print after levels
 *	print_level_tab	    - pointer to boolean indicating if tab or spaces are to be used
 *
 * Function returns void.
 *
 * NOTE: syntax errors are an error just like it was when it was in main().
 *
 * NOTE: this function does not return on NULL pointers.
 */
void
jprint_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab)
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
	err(3, "jprint", "couldn't parse -L spaces"); /*ooo*/
	not_reached();
    } else {
	*print_level_tab = false; /* ensure it's false in case specified previously */
	dbg(DBG_LOW, "will print %jd space%s after levels", *num_level_spaces, *num_level_spaces==1?"":"s");
    }
}

/*
 * jprint_parse_value_option	- parse -t types list
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
jprint_parse_value_type_option(char *optarg)
{
    uintmax_t type = JPRINT_TYPE_SIMPLE; /* default is simple: num, bool, str and null */
    char *p = NULL;

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    }
    p = optarg;

    /* Determine if the arg is a valid type.  */
    if (!strcmp(p, "int")) {
	type = JPRINT_TYPE_INT;
    } else if (!strcmp(p, "float")) {
	type = JPRINT_TYPE_FLOAT;
    } else if (!strcmp(p, "exp")) {
	type = JPRINT_TYPE_EXP;
    } else if (!strcmp(p, "num")) {
	type = JPRINT_TYPE_NUM;
    } else if (!strcmp(p, "bool")) {
	type = JPRINT_TYPE_BOOL;
    } else if (!strcmp(p, "str")) {
	type = JPRINT_TYPE_STR;
    } else if (!strcmp(p, "null")) {
	type = JPRINT_TYPE_NULL;
    } else if (!strcmp(p, "simple")) {
	type = JPRINT_TYPE_SIMPLE;
    } else {
	/* unknown or unsupported type */
	err(3, __func__, "unknown or unsupported type '%s'", p); /*ooo*/
	not_reached();
    }

    return type;
}

/*
 * free_jprint	    - free jprint struct
 *
 * given:
 *
 *	jprint	    - a struct jprint **
 *
 * This function will do nothing other than warn on NULL pointer (even though
 * it's safe to free a NULL pointer though if jprint itself was NULL it would be
 * an error to dereference it).
 *
 * We pass a struct jprint ** so that in the caller jprint can be set to NULL to
 * remove the need to repeatedly set it to NULL each time this function is
 * called. This way we remove the need to do more than just call this function.
 */
void
free_jprint(struct jprint **jprint)
{
    if (jprint == NULL || *jprint == NULL) {
	warn(__func__, "passed NULL struct jprint ** or *jprint is NULL");
	return;
    }

    free_jprint_patterns_list(*jprint); /* free patterns list first */

    free(*jprint);
    *jprint = NULL;
}


/* parse_jprint_name_args - add name_args to patterns list
 *
 * given:
 *
 *	jprint	    - pointer to our struct jprint
 *	argv	    - argv from main()
 *
 * This function will not return on NULL pointers.
 *
 * NOTE: by patterns we refer to name_args.
 *
 * XXX - the pattern concept is currently incorrect and needs to be fixed - XXX
 */
void
parse_jprint_name_args(struct jprint *jprint, char **argv)
{
    int i;  /* to iterate through argv */

    /* firewall */
    if (argv == NULL) {
	err(15, __func__, "argv is NULL"); /*ooo*/
	not_reached();
    }

    for (i = 1; argv[i] != NULL; ++i) {
	jprint->pattern_specified = true;

	if (add_jprint_pattern(jprint, jprint->use_regexps, jprint->use_substrings, argv[i]) == NULL) {
	    err(25, __func__, "failed to add pattern (substrings %s) '%s' to patterns list",
		    jprint->use_substrings?"OK":"ignored", argv[i]);
	    not_reached();
	}
    }
}


/*
 * add_jprint_pattern
 *
 * Add jprint pattern to the jprint struct pattern list.
 *
 * given:
 *
 *	jprint		- pointer to the jprint struct
 *	use_regexp	- whether to use regexp or not
 *	use_substrings	- if -s was specified, make this a substring match
 *	str		- the pattern to be added to the list
 *
 * NOTE: this function will not return if jprint is NULL. If str is NULL
 * this function will not return but if str is empty it will add an empty
 * string to the list. However the caller will usually check that it's not empty
 * prior to calling this function.
 *
 * Returns a pointer to the newly allocated struct jprint_pattern * that was
 * added to the jprint patterns list.
 *
 * Duplicate patterns will not be added (case sensitive).
 */
struct jprint_pattern *
add_jprint_pattern(struct jprint *jprint, bool use_regexp, bool use_substrings, char *str)
{
    struct jprint_pattern *pattern = NULL;
    struct jprint_pattern *tmp = NULL;

    /*
     * firewall
     */
    if (jprint == NULL) {
	err(26, __func__, "passed NULL jprint struct");
	not_reached();
    }
    if (str == NULL) {
	err(27, __func__, "passed NULL str");
	not_reached();
    }

    /*
     * first make sure the pattern is not already added to the list as the same
     * type
     */
    for (pattern = jprint->patterns; pattern != NULL; pattern = pattern->next) {
	if (pattern->pattern && pattern->use_regexp == use_regexp) {
	    /* XXX - add support for regexps - XXX */
	    if ((!jprint->ignore_case && !strcmp(pattern->pattern, str))||
		(jprint->ignore_case && strcasecmp(pattern->pattern, str))) {
		return pattern;
	    }
	}
    }
    /*
     * XXX either change the debug level or remove this message once
     * processing is complete
     */
    if (use_regexp) {
	dbg(DBG_LOW,"%s regex requested: '%s'", jprint->search_value?"value":"name", str);
    } else {
	dbg(DBG_LOW,"%s pattern requested: '%s'", jprint->search_value?"value":"name", str);
    }

    errno = 0; /* pre-clear errno for errp() */
    pattern = calloc(1, sizeof *pattern);
    if (pattern == NULL) {
	errp(28, __func__, "unable to allocate struct jprint_pattern *");
	not_reached();
    }

    errno = 0;
    pattern->pattern = strdup(str);
    if (pattern->pattern == NULL) {
	errp(29, __func__, "unable to strdup string '%s' for patterns list", str);
	not_reached();
    }

    pattern->use_regexp = use_regexp;
    pattern->use_value = jprint->search_value;
    pattern->use_substrings = use_substrings;
    /* increment how many patterns have been specified */
    ++jprint->number_of_patterns;
    /* let jprint know that a pattern was indeed specified */
    jprint->pattern_specified = true;
    pattern->matches_found = 0; /* 0 matches found at first */

    dbg(DBG_LOW, "adding %s pattern '%s' to patterns list", pattern->use_value?"value":"name", pattern->pattern);

    for (tmp = jprint->patterns; tmp && tmp->next; tmp = tmp->next)
	; /* on its own line to silence useless and bogus warning -Wempty-body */

    if (!tmp) {
	jprint->patterns = pattern;
    } else {
	tmp->next = pattern;
    }

    return pattern;
}


/* free_jprint_patterns_list	- free patterns list in a struct jprint *
 *
 * given:
 *
 *	jprint	    - the jprint struct
 *
 * If the jprint patterns list is empty this function will do nothing.
 *
 * NOTE: this function does not return on a NULL jprint.
 *
 * NOTE: this function calls free_jprint_matches_list() on all the patterns
 * prior to freeing the pattern itself.
 */
void
free_jprint_patterns_list(struct jprint *jprint)
{
    struct jprint_pattern *pattern = NULL; /* to iterate through patterns list */
    struct jprint_pattern *next_pattern = NULL; /* next in list */

    if (jprint == NULL) {
	err(30, __func__, "passed NULL jprint struct");
	not_reached();
    }

    for (pattern = jprint->patterns; pattern != NULL; pattern = next_pattern) {
	next_pattern = pattern->next;

	/* first free any matches */
	free_jprint_matches_list(pattern);

	/* now free the pattern string itself */
	if (pattern->pattern) {
	    free(pattern->pattern);
	    pattern->pattern = NULL;
	}

	/* finally free the pattern and set to NULL for the next pass */
	free(pattern);
	pattern = NULL;
    }

    jprint->patterns = NULL;
}


/*
 * add_jprint_match
 *
 * Add jprint pattern match to the jprint struct pattern match list.
 *
 * given:
 *
 *	jprint		- pointer to the jprint struct
 *	pattern		- the pattern that matched
 *	name		- struct json * name that matched
 *	value		- struct json * value that matched
 *	name_str	- the matching value, either a value or name
 *	value_str	- the matching value, the opposite of name_str
 *	level		- the depth or level for the -l / -L options (level 0 is top of tree)
 *	string		- boolean to indicate if the match is a string
 *	type		- enum item_type indicating the type of node (JTYPE_* in json_parse.h)
 *
 * NOTE: this function will not return if any of the pointers are NULL (except
 * the name and value - for now) including the pointers in the pattern struct.
 *
 * Returns a pointer to the newly allocated struct jprint_match * that was
 * added to the jprint matched patterns list.
 *
 * NOTE: depending on jprint->search_value the name and value nodes will be in a
 * different order. Specifically the name is what matched, whether a value in
 * the json tree or name, and the value is what will be printed. At least once
 * this feature is done :-)
 */
struct jprint_match *
add_jprint_match(struct jprint *jprint, struct jprint_pattern *pattern, struct json *name,
	struct json *value, char *name_str, char *value_str, uintmax_t level, bool string,
	enum item_type type)
{
    struct jprint_match *match = NULL;
    struct jprint_match *tmp = NULL;

    /*
     * firewall
     */
    if (jprint == NULL) {
	err(31, __func__, "passed NULL jprint struct");
	not_reached();
    }

    if (name_str == NULL) {
	err(32, __func__, "passed NULL name_str");
	not_reached();
    }
    if (value_str == NULL) {
	err(34, __func__, "value_str is NULL");
	not_reached();
    }

    /*
     * search for an exact match and only increment the count if found.
     *
     * NOTE: this means that when printing the output we have to go potentially
     * print the match more than once; if -c is specified we print only the
     * count.
     */
    for (tmp = pattern?pattern->matches:jprint->matches; tmp; tmp = tmp->next) {
	if (type == tmp->type) {
	    /* XXX - add support for regexps - XXX */
	    if (((!jprint->ignore_case && !strcmp(tmp->match, value_str) && !strcmp(tmp->match, value_str)))||
		(jprint->ignore_case && !strcasecmp(tmp->match, value_str) && !strcasecmp(tmp->match, value_str))) {
		    dbg(DBG_LOW, "incrementing count of match '%s' to %ju", tmp->match, tmp->count + 1);
		    jprint->total_matches++;
		    tmp->count++;
		    return tmp;
	    }
	}
    }

    /* if we get here we have to add the match to the matches list */
    errno = 0; /* pre-clear errno for errp() */
    match = calloc(1, sizeof *match);
    if (match == NULL) {
	errp(35, __func__, "unable to allocate struct jprint_match *");
	not_reached();
    }

    /* duplicate the match (name_str) */
    errno = 0; /* pre-clear errno for errp() */
    match->match = strdup(name_str);
    if (match->match == NULL) {
	errp(36, __func__, "unable to strdup string '%s' for match list", name_str);
	not_reached();
    }

    /* duplicate the value of the match, either name or value */
    errno = 0; /* pre-clear errno for errp() */
    match->value = strdup(value_str);
    if (match->match == NULL) {
	errp(37, __func__, "unable to strdup value string '%s' for match list", value_str);
	not_reached();
    }
    /* set level of the match for -l / -L options */
    match->level = level;

    /* set count to 1 */
    match->count = 1;

    /* record the pattern that was matched. It's okay if it is NULL. */
    match->pattern = pattern; /* DO NOT FREE THIS! */

    /* set struct json * nodes */
    match->node_name = name;
    match->node_value = value;

    /* set which match number this is, incrementing the pattern's total matches */
    if (pattern) {
	match->number = pattern->matches_found++;
    }

    /* increment total matches of ALL patterns (name_args) in jprint struct */
    jprint->total_matches++;

    /* set if a string for -j */
    match->string = string;

    /* record type */
    match->type = type;

    dbg(DBG_LOW, "adding match '%s' to pattern '%s' to match list",
	    jprint->search_value?match->value:match->match, name_str);

    for (tmp = pattern?pattern->matches:jprint->matches; tmp && tmp->next; tmp = tmp->next)
	; /* on its own line to silence useless and bogus warning -Wempty-body */

    if (!tmp) {
	if (pattern != NULL) {
	    pattern->matches = match;
	} else {
	    jprint->matches = match;
	}
    } else {
	tmp->next = match;
    }

    /* a match is found, set jprint->match_found to true */
    jprint->match_found = true;

    return match;
}

/* free_jprint_matches_list   - free matches list in a struct jprint_pattern *
 *
 * given:
 *
 *	pattern	    - the jprint pattern
 *
 * If the jprint patterns match list is empty this function will do nothing.
 *
 * NOTE: this function does not return on a NULL pattern.
 */
void
free_jprint_matches_list(struct jprint_pattern *pattern)
{
    struct jprint_match *match = NULL; /* to iterate through matches list */
    struct jprint_match *next_match = NULL; /* next in list */

    if (pattern == NULL) {
	err(38, __func__, "passed NULL pattern struct");
	not_reached();
    }

    for (match = pattern->matches; match != NULL; match = next_match) {
	next_match = match->next;
	if (match->match) {
	    free(match->match);
	    match->match = NULL;
	}

	if (match->value) {
	    free(match->value);
	    match->value = NULL;
	}

	/* DO NOT FREE match->pattern! */
	free(match);
	match = NULL;
    }

    pattern->matches = NULL;
}

/* is_jprint_match	- if the name or value is a match based on type
 *
 * given:
 *
 *	jprint	    - pointer to our struct jprint
 *	pattern	    - pointer to the pattern
 *	name	    - char * that is the matching name (if pattern == NULL)
 *	node	    - struct json node
 *	str	    - the string to compare
 *
 * Returns true if a match; else false.
 *
 * This function will not return if jprint or node or str is NULL. The pattern
 * and name pointers can be NULL unless both are NULL.
 */
bool
is_jprint_match(struct jprint *jprint, struct jprint_pattern *pattern, char *name, struct json *node, char *str)
{
    /* firewall */
    if (jprint == NULL) {
	err(39, __func__, "jprint is NULL");
	not_reached();
    } else if ((pattern == NULL || pattern->pattern == NULL) && name == NULL) {
	err(40, __func__, "pattern and name are both NULL");
	not_reached();
    } else if (node == NULL) {
	err(41, __func__, "node is NULL");
	not_reached();
    } else if (str == NULL) {
	err(42, __func__, "str is NULL");
	not_reached();
    }

    /* set up name if NULL to be pattern->pattern */
    if (name == NULL) {
	name = pattern->pattern;
    }

    /* check that name != NULL */
    if (name == NULL) {
	err(43, __func__, "name is NULL");
	not_reached();
    }

    /*
     * if there is a match found add it to the matches list
     *
     * XXX - an issue that must be worked out is that if one does something
     * like:
     *
     *	    jprint -Y int -j h2g2.json 42
     *
     * they will get not just the integer values but the string "42". The
     * problem is that in that file there is a string "42" and due to the bug in
     * string matching and possibly the traversing of the tree (how it's done)
     * this matches as all this function does is check the type of node and
     * compare that it's equal. The string node should not actually be examined
     * if the type does not match but this has to be fixed at a later time.
     */
    switch (node->type) {

	case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
	    break;

	case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
	    {
		struct json_number *item = &(node->item.number);

		if (item != NULL) {
		    if (!jprint->search_value || jprint_match_num(jprint->type) ||
			(item->is_integer&&jprint_match_int(jprint->type))||
			(item->is_floating && jprint_match_float(jprint->type)) ||
			(item->is_e_notation && jprint_match_exp(jprint->type))) {
			    if (jprint->use_substrings) {
				if (strstr(str, name) ||
				    (jprint->ignore_case&&strcasestr(str, name))) {
					return true;
				}
			    } else {
				if (!strcmp(name, str) ||
				    (jprint->ignore_case&&!strcasecmp(name, str))) {
					return true;
				}
			    }
			}
		}
	    }
	    break;

	case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
	    {
		struct json_string *item = &(node->item.string);

		if (item != NULL) {
		    /* XXX - as noted elsewhere, the -Y for strings is buggy - XXX */
		    if (!jprint->search_value || jprint_match_string(jprint->type)) {
			if (jprint->use_substrings) {
			    if (strstr(str, name) ||
				(jprint->ignore_case && strcasestr(str, name))) {
				    return true;
			    }
			} else {
			    if (!strcmp(name, str) ||
				(jprint->ignore_case && !strcasecmp(name, str))) {
				    return true;
			    }
			}
		    }
		}
	    }
	    break;

	case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
	    {
		struct json_boolean *item = &(node->item.boolean);

		if (item != NULL) {
		    if (!jprint->search_value || jprint_match_bool(jprint->type)) {
			if (jprint->use_substrings) {
			    if (strstr(str, name) ||
				(jprint->ignore_case && strcasestr(str, name))) {
				    return true;
			    }

			} else {
			    if (!strcmp(name, str) ||
				(jprint->ignore_case && !strcasecmp(name, str))) {
				    return true;
			    }
			}
		    }
		}
	    }
	    break;

	case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
	    {
		struct json_null *item = &(node->item.null);

		if (item != NULL) {
		    if (!jprint->search_value || jprint_match_null(jprint->type)) {
			if (jprint->use_substrings) {
			    if (strstr(str, name) ||
				(jprint->ignore_case && strcasestr(str, name))) {
				    return true;
			    }
			} else {
			    if (!strcmp(name, str) ||
				(jprint->ignore_case && !strcasecmp(name, str))) {
				    return true;
			    }
			}
		    }
	    }
	    break;

	case JTYPE_MEMBER:	/* JSON item is a member - see struct json_member */
	    {
		struct json_member *item = &(node->item.member);

		/* XXX - check if anything has to be done here */
		UNUSED_ARG(item);
	    }
	    break;

	case JTYPE_OBJECT:	/* JSON item is a { members } - see struct json_object */
	    {
		struct json_object *item = &(node->item.object);

		/* XXX - check if anything has to be done here */
		UNUSED_ARG(item);
	    }
	    break;

	case JTYPE_ARRAY:	/* JSON item is a [ elements ] - see struct json_array */
	    {
		struct json_array *item = &(node->item.array);

		/* XXX - check if anything has to be done here as a quick test
		 * suggests nothing has to be done - XXX */
		UNUSED_ARG(item);
	    }
	    break;

	case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values - see struct json_elements */
	    {
		struct json_elements *item = &(node->item.elements);

		/* XXX - check if anything has to be done here - XXX */
		UNUSED_ARG(item);
	    }
	    break;

	default:
	    break;
	}
    }
    /* nothing found, return false */
    return false;
}


/*
 * jprint_json_search
 *
 * Print information about a JSON node, depending on the booleans in struct
 * jprint if the tree node matches the name or value in any pattern in the
 * struct json.
 *
 * given:
 *	jprint	    pointer to our struct jprint
 *	node	    pointer to a JSON parser tree node to try and match
 *	is_value    whether node is a value or name
 *	depth	    current tree depth (0 ==> top of tree)
 *	...	    extra args are ignored, required extra args if <=
 *		    json_verbosity_level:
 *
 *			stream		stream to print on
 *			json_dbg_lvl	print message if JSON_DBG_FORCED
 *					OR if <= json_verbosity_level
 *
 * Example use - print a JSON parse tree
 *
 *	jprint_json_search(node, true, depth, JSON_DBG_MED);
 *	jprint_json_search(node, false, depth, JSON_DBG_FORCED;
 *	jprint_json_search(node, false, depth, JSON_DBG_MED);
 *
 * While the ... variable arg are ignored, we need to declare
 * then in order to be in vcallback form for use by json_tree_walk().
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if jprint == NULL or node == NULL.
 */
void
jprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, ...)
{
    va_list ap;		/* variable argument list */

    /* firewall */
    if (jprint == NULL || node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, depth);

    /*
     * call va_list argument list function
     */
    vjprint_json_search(jprint, node, is_value, depth, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);
    return;
}


/*
 * vjprint_json_search
 *
 * Search for matches in a JSON node, depending on the booleans in struct
 * jprint if the tree node matches the name or value in any pattern in the
 * struct json.
 *
 * This is a variable argument list interface to jprint_json_search().
 *
 * See jprint_json_tree_search() to go through the entire tree.
 *
 * given:
 *	jprint	    pointer to our struct json
 *	node	    pointer to a JSON parser tree node to free
 *	is_value    boolean to indicate if this is a value or name
 *	depth	    current tree depth (0 ==> top of tree)
 *	ap	    variable argument list, required ap args:
 *
 *			json_dbg_lvl	print message if JSON_DBG_FORCED
 *					OR if <= json_verbosity_level
 *
 * NOTE: This function does nothing if jprint == NULL or jprint->patterns ==
 * NULL or if none of the names/values match any of the patterns or node ==
 * NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 *
 * NOTE: this function is incomplete and does not do everything correctly. Any
 * problems will be fixed in a future commit.
 */
void
vjprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, va_list ap)
{
    FILE *stream = NULL;	/* stream to print on */
    int json_dbg_lvl = JSON_DBG_DEFAULT;	/* JSON debug level if json_dbg_used */
    struct jprint_pattern *pattern = NULL; /* to iterate through patterns list */
    va_list ap2;		/* copy of va_list ap */

    /*
     * firewall - nothing to do for NULL jprint or NULL patterns list or NULL node
     */
    if (jprint == NULL || jprint->patterns == NULL || node == NULL) {
	return;
    }

    /*
     * duplicate va_list ap
     */
    va_copy(ap2, ap);

    /*
     * obtain the stream, json_dbg_used, and json_dbg args
     */
    stream = stdout;
    if (stream == NULL) {
	va_end(ap2); /* stdarg variable argument list clean up */
	return;
    }
    json_dbg_lvl = va_arg(ap2, int);

    /*
     * XXX: This is buggy in a number of ways and the is_value will possibly
     * have to change. Strings in particular are problematic here and it does
     * not work right. Also -t can end up searching by value without -Y even
     * though it's not supposed to. What does work is that specifying a type in
     * general can prevent one or the other from showing up. Nevertheless to
     * develop the type checks this has to be done until it can be fixed. This
     * is very much a work in progress.
     */
    if (((!jprint->search_value && is_value) || (!is_value && jprint->search_value))) {
	va_end(ap2); /* stdarg variable argument list clean up */
	return;
    }

    /* only search for matches if level constraints allow it */
    if (!jprint->levels_constrained || jprint_number_in_range(depth, jprint->number_of_patterns, &jprint->jprint_levels))
    {
	for (pattern = jprint->patterns; pattern != NULL; pattern = pattern->next) {
	    /* XXX: for each pattern we have to find a match and then add it to
	     * the matches list of that pattern. After that we can go through
	     * the matches found and print out the matches as desired by the
	     * user. We will not add matches if the constraints do not allow it.
	     *
	     * However note that we currently do not have a working way to check
	     * if the node is a value or name so what ends up happening is that
	     * a value can match as a name and vice versa. See above comment.
	     */

	    /*
	     * if there is a match found add it to the matches list
	     */
	    switch (node->type) {

		case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
		    break;

		case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
		    {
			struct json_number *item = &(node->item.number);
			if (item != NULL && item->converted) {
			    char *str = item->as_str;

			    if (str != NULL) {
				if (is_jprint_match(jprint, pattern, pattern->pattern, node, str)) {
				    if (add_jprint_match(jprint, pattern, jprint->search_value?
					NULL:node, jprint->search_value?node:NULL, pattern->pattern, str, depth, false,
					JTYPE_NUMBER) == NULL) {
					    err(44, __func__, "adding match '%s' to pattern failed", str);
					    not_reached();
				    }
				}
			    }
			}
		    }
		    break;

		case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
		    {
			struct json_string *item = &(node->item.string);
			if (item != NULL && item->converted) {
			    char *str = jprint->match_encoded ? item->as_str : item->str;

			    if (str != NULL) {
				/* XXX - as noted above, the -Y for strings is buggy - XXX */
				if (is_jprint_match(jprint, pattern, pattern->pattern, node, str)) {
				    if (add_jprint_match(jprint, pattern, jprint->search_value?NULL:node,
					jprint->search_value?node:NULL, jprint->search_value?pattern->pattern:str,
					jprint->search_value?str:pattern->pattern, depth, true, JTYPE_STRING) == NULL) {
					    err(45, __func__, "adding match '%s' to pattern failed", str);
					    not_reached();
				    }
				}
			    }
			}
		    }
		    break;

		case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
		    {
			struct json_boolean *item = &(node->item.boolean);

			if (item != NULL && item->converted) {
			    char *str = item->as_str;

			    if (str != NULL) {
				if (is_jprint_match(jprint, pattern, pattern->pattern, node, str)) {
				    if (add_jprint_match(jprint, pattern, jprint->search_value?NULL:node,
					jprint->search_value?node:NULL, pattern->pattern, str, depth, false, JTYPE_BOOL) == NULL) {
					    err(46, __func__, "adding match '%s' to pattern failed", str);
					    not_reached();
				    }
				}
			    }
			}
		    }
		    break;

		case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
		    {
			struct json_null *item = &(node->item.null);

			if (item != NULL && item->converted) {
			    char *str = item->as_str;

			    if (str != NULL) {
				if (is_jprint_match(jprint, pattern, pattern->pattern, node, str)) {
				    if (add_jprint_match(jprint, pattern, jprint->search_value?NULL:node,
					jprint->search_value?node:NULL, pattern->pattern, str, depth, false, JTYPE_NULL) == NULL) {
					    err(47, __func__, "adding match '%s' to pattern failed", str);
					    not_reached();
				    }
				}
			    }
			}
		    }
		    break;

		case JTYPE_MEMBER:	/* JSON item is a member - see struct json_member */
		    {
			struct json_member *item = &(node->item.member);

			/* XXX - fix to check for match of the member and add to
			 * the matches list if it fits within constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		case JTYPE_OBJECT:	/* JSON item is a { members } - see struct json_object */
		    {
			struct json_object *item = &(node->item.object);

			/* XXX - fix to check for match of the object and add to
			 * the matches list if it fits within constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		case JTYPE_ARRAY:	/* JSON item is a [ elements ] - see struct json_array */
		    {
			struct json_array *item = &(node->item.array);

			/* XXX - fix to check for match of the array and add it
			 * to the matches list if it fits within the constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values - see struct json_elements */
		    {
			struct json_elements *item = &(node->item.elements);

			/* XXX - fix to check for match of the element and add it
			 * to the matches list if it fits within the constraints - XXX */
			UNUSED_ARG(item);
		    }
		    break;

		default:
		    break;
	    }
	}
    }
    /*
     * stdarg variable argument list clean up
     */
    va_end(ap2);
    return;
}


/*
 * jprint_json_tree_search - search nodes of an entire JSON parse tree
 *
 * This function uses the jprint_json_tree_walk() interface to walk
 * the JSON parse tree and print requested information about matching nodes.
 *
 * given:
 *	jprint	    pointer to our struct jprint
 *	node	    pointer to a JSON parser tree node to free
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	...	extra args are ignored, required extra args:
 *
 *		json_dbg_lvl   print message if JSON_DBG_FORCED
 *			       OR if <= json_verbosity_level
 *
 * Example uses - print an entire JSON parse tree
 *
 *	jprint_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_FORCED);
 *	jprint_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_FORCED);
 *	jprint_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_MED);
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if jprint == NULL, jprint->patterns == NULL
 * or node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 *
 * NOTE: this function is a wrapper to jprint_json_tree_walk() with the callback
 * vjprint_json_search().
 */
void
jprint_json_tree_search(struct jprint *jprint, struct json *node, unsigned int max_depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (jprint == NULL || jprint->patterns == NULL || node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, max_depth);

    /*
     * walk the JSON parse tree
     */
    jprint_json_tree_walk(jprint, node, max_depth, false, 0, vjprint_json_search, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);
    return;
}

/*
 * jprint_json_tree_walk - walk a JSON parse tree calling a function on each node in va_list form
 *
 * This is the va_list form of json_tree_walk().
 *
 * Walk a JSON parse tree, Depth-first Post-order (LRN) order.  See:
 *
 *	https://en.wikipedia.org/wiki/Tree_traversal#Post-order,_LRN
 *
 * Example use - walk an entire JSON parse tree, looking for matches and
 * printing requested information on those matches.
 *
 *	jprint_json_tree_walk(jprint, tree, JSON_DEFAULT_MAX_DEPTH, 0, json_free);
 *
 * given:
 *	node	    pointer to a JSON parse tree
 *	is_value    if it's a value or name
 *	max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *			NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
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
 * why we call it vcallback and not something else.
 *
 * If max_depth is >= 0 and the tree depth > max_depth, then this function return.
 * In this case it will NOT operate on the node, or will be descend and further
 * into the tree.
 *
 * NOTE: This function warns but does not do anything if an arg is NULL.
 *
 * NOTE: this function might be incomplete or does something that is incorrect.
 * If this is the case it will be fixed in a future update.
 */
void
jprint_json_tree_walk(struct jprint *jprint, struct json *node, bool is_value, unsigned int max_depth, unsigned int depth,
		void (*vcallback)(struct jprint *, struct json *, bool, unsigned int, va_list), va_list ap)
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

    /* if depth is 0 it can't be a value */
    if (depth == 0) {
	is_value = false;
    }

    /*
     * walk based on type of node
     */
    switch (node->type) {

    case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
	/* perform function operation on this terminal parse tree node, all of
	 * which have to be a value */
	(*vcallback)(jprint, node, true, depth, ap);
	break;

    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */

	/* perform function operation on this terminal parse tree node */
	(*vcallback)(jprint, node, is_value, depth, ap);
	break;

    case JTYPE_MEMBER:	/* JSON item is a member */
	{
	    struct json_member *item = &(node->item.member);

	    /* perform function operation on JSON member name (left branch) node */
	    jprint_json_tree_walk(jprint, item->name, false, max_depth, depth+1, vcallback, ap);

	    /* perform function operation on JSON member value (right branch) node */
	    jprint_json_tree_walk(jprint, item->value, true, max_depth, depth+1, vcallback, ap);
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth+1, ap);
	break;

    case JTYPE_OBJECT:	/* JSON item is a { members } */
	{
	    struct json_object *item = &(node->item.object);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jprint_json_tree_walk(jprint, item->set[i], is_value, max_depth, depth, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth+1, ap);
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] */
	{
	    struct json_array *item = &(node->item.array);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jprint_json_tree_walk(jprint, item->set[i], true, max_depth, depth, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth+1, ap);
	break;

    case JTYPE_ELEMENTS:	/* JSON items is zero or more JSON values */
	{
	    struct json_elements *item = &(node->item.elements);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jprint_json_tree_walk(jprint, item->set[i], true, max_depth, depth, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jprint, node, is_value, depth+1, ap);
	break;

    default:
	warn(__func__, "node type is unknown: %d", node->type);
	/* nothing we can traverse */
	break;
    }
    return;
}


/* jprint_print_count	- print total matches if -c used
 *
 * given:
 *
 *	jprint	    - pointer to our struct jprint
 *
 * This function will not return on NULL jprint.
 *
 * This function returns false if -c was not used and true if it was.
 */
bool
jprint_print_count(struct jprint *jprint)
{
    /* firewall */
    if (jprint == NULL) {
	err(48, __func__, "jprint is NULL");
	not_reached();
    }

    if (jprint->count_only) {
	print("%ju\n", jprint->total_matches);
	return true;
    }

    return false;
}


/* jprint_print_final_comma	- print final comma if -C used
 *
 * given:
 *
 *	jprint	    - pointer to our struct jprint
 *
 * This function will not return on NULL jprint.
 *
 * This function does nothing if -C was not used or if -c was used.
 */
void
jprint_print_final_comma(struct jprint *jprint)
{
    /* firewall */
    if (jprint == NULL) {
	err(49, __func__, "jprint is NULL");
	not_reached();
    }

    if (jprint->print_final_comma && !jprint->count_only) {
	print("%c", ',');
    }
}


/* jprint_print_brace - print a brace if -B used
 *
 * given:
 *
 *	jprint	    - pointer to our jprint struct
 *	open	    - boolean indicating if we need an open or close brace
 *
 * This function returns void.
 *
 * This function will not return on NULL jprint.
 */
void
jprint_print_brace(struct jprint *jprint, bool open)
{
    /* firewall */
    if (jprint == NULL) {
	err(50, __func__, "jprint is NULL");
	not_reached();
    }

    if (jprint->print_braces && !jprint->count_only) {
	print("%c\n", open?'{':'}');
    }
}


/* jprint_print_match	    - print a single match
 *
 * given:
 *
 *	jprint	    - our struct jprint with patterns list
 *	pattern	    - the pattern with the match
 *	match	    - the match to print
 *
 * NOTE: this function will not return if NULL pointers.
 *
 * NOTE: if any pointer in a match is NULL this function will not return as it
 * indicates incorrect behaviour in the program as it should never have got this
 * far in the first place.
 *
 * XXX: the concept of more than one pattern is not correct. This has to be
 * fixed.
 */
void
jprint_print_match(struct jprint *jprint, struct jprint_pattern *pattern, struct jprint_match *match)
{
    uintmax_t i = 0;			    /* to iterate through count of each match */
    uintmax_t j = 0;			    /* temporary iterator */

    /* firewall */
    if (jprint == NULL) {
	err(51, __func__, "jprint is NULL");
	not_reached();
    } else if (match == NULL) {
	err(52, __func__, "match is NULL");
	not_reached();
    } else if (pattern == NULL) {
	err(53, __func__, "pattern is NULL");
	not_reached();
    }

    /* if the name of the match is NULL it is a fatal error */
    if (match->match == NULL) {
	err(54, __func__, "match->match is NULL");
	not_reached();
    } else if (*match->match == '\0') {
	/* warn on empty name for now and then go to next match */
	warn(__func__, "empty match name");
	return;
    }

    if (match->value == NULL) {
	err(55, __func__, "match '%s' has NULL value", match->match);
	not_reached();
    } else if (*match->value == '\0') {
	/* for now we only warn on empty value */
	warn(__func__, "empty value for match '%s'", match->match);
	return;
    }

    /*
     * if we get here we have to print the name and/or match
     */
    for (i = 0; i < match->count; ++i) {
	/* print the match if constraints allow it
	 *
	 * XXX - add final constraint checks
	 *
	 * XXX - This is buggy in some cases. This must be fixed.
	 */
	if (jprint->print_json_levels) {
	    print("%ju", match->level);

	    for (j = 0; j < jprint->num_level_spaces; ++j) {
		printf("%s", jprint->print_level_tab?"\t":" ");
	    }
	    if (jprint->indent_levels) {
		if (jprint->indent_spaces) {
		    for (j = 0; j < match->level * jprint->indent_spaces; ++j) {
			print("%s", jprint->indent_tab?"\t":" ");
		    }
		}
	    }

	}
	if (jprint_print_name_value(jprint->print_type) || jprint->print_syntax) {
	    if (jprint->print_syntax) {

		print("\"%s\"", match->match);

		for (j = 0; j < jprint->num_token_spaces; ++j) {
		    print("%s", jprint->print_token_tab?"\t":" ");
		}

		print("%s", ":");

		for (j = 0; j < jprint->num_token_spaces; ++j) {
		    print("%s", jprint->print_token_tab?"\t":" ");
		}

		print("%s%s%s%s\n",
			match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"",
			match->value,
			match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"",
			match->next || (pattern->next&&pattern->next->matches) || i+1<match->count?",":"");
	    } else {
		print("%s%s%s",
			match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"",
			match->match,
			match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"");
		for (j = 0; j < jprint->num_token_spaces; ++j) {
		    print("%s", jprint->print_token_tab?"\t":" ");
		}
		if (jprint->print_colons) {
		    print("%s", ":");
		}
		for (j = 0; j < jprint->num_token_spaces; ++j) {
		    print("%s", jprint->print_token_tab?"\t":" ");
		}
		print("%s%s%s\n",
			match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"",
			match->value,
			match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"");
	    }
	} else if (jprint_print_name(jprint->print_type)) {
	    print("%s%s%s\n",
		  match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"",
		  match->match,
		  match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"");
	} else if (jprint_print_value(jprint->print_type)) {
	    print("%s%s%s\n",
		    match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"",
		    match->value,
		    match->string && (jprint->quote_strings||jprint->print_syntax||jprint->print_entire_file)?"\"":"");
	}
	/*
	 * XXX: more functions will have to be added to print the matches
	 * and currently the struct jprint_match struct is a work in
	 * progress. More will have to be added like the JSON type that
	 * matched (this includes not only the jprint type but the JSON
	 * type).
	 */
    }
}

/* jprint_print_matches	    - print all matches found
 *
 * given:
 *
 *	jprint	    - our struct jprint with patterns list
 *
 * NOTE: this function will not return if jprint is NULL.
 *
 * NOTE: this function will only warn if patterns and matches are both NULL.
 *
 * NOTE: if any pointer in a match is NULL this function will not return as it
 * indicates incorrect behaviour in the program as it should never have got this
 * far in the first place.
 *
 * NOTE: this function uses jprint_print_match() to print each match.
 *
 * XXX: the concept of more than one pattern is not correct. This has to be
 * fixed.
 */
void
jprint_print_matches(struct jprint *jprint)
{
    struct jprint_pattern *pattern = NULL;  /* to iterate through patterns list */
    struct jprint_match *match = NULL;	    /* to iterate through matches of each pattern in the patterns list */

    /* firewall */
    if (jprint == NULL) {
	err(56, __func__, "jprint is NULL");
	not_reached();
    } else if (jprint->patterns == NULL && jprint->matches == NULL) {
	warn(__func__, "NULL patterns and matches list");
	return;
    }

    /* if -c used just print total number of matches */
    if (jprint_print_count(jprint)) {
	return;
    }

    /* if -c was not used we have more to do */

    /*
     * although printing syntax is not yet fully implemented (though a
     * reasonable amount of it is), we will check for -B and print the braces so
     * that after the syntax printing is implemented nothing has to be done with
     * -B. The function jprint_print_braces() will not do anything if -B was not
     * used.
     */
    jprint_print_brace(jprint, true);

    for (pattern = jprint->patterns; pattern != NULL; pattern = pattern->next) {
	for (match = pattern->matches; match != NULL; match = match->next) {
	    jprint_print_match(jprint, pattern, match);
	}
    }

    /*
     * although printing syntax is not yet fully implemented, we will check for
     * -B and print the braces so that after the syntax printing is implemented
     * nothing has to be done with -B. The function jprint_print_braces() will
     * not do anything if -B was not used.
     */
    if (jprint->print_braces && !jprint->count_only) {
	jprint_print_brace(jprint, false);
    }
    /*
     * as well, even though -j is not yet fully implemented, we will check for
     * -C and print the final comma if requested so that once -j fully
     * implemented we shouldn't have to do anything else with this option. Don't
     * print final comma if -c used.
     */
    jprint_print_final_comma(jprint);

    /* if we need a newline print it */
    if ((jprint->print_braces || jprint->print_final_comma) && !jprint->count_only) {
	puts("");
    }
}

/* run_jprint_check_tool    - run the JSON check tool from -S path
 *
 * given:
 *
 *	jprint	    - pointer to our struct jprint (has everything needed)
 *	argv	    - main()'s argv
 *
 * This function does not return on NULL jprint. It does check for NULL
 * jprint->check_tool_path and jprint->check_tool_args as it's not required to
 * be set: the jprint_sanity_chks() function only verifies that IF it is set it
 * exists and is executable and if the args are specified that the -S is also
 * specified (and the path is an executable file).
 *
 * This function does not return on NULL jprint->file_contents.
 *
 * It does NOT check for the path existing as an executable file as the
 * jprint_sanity_chks() does that and if it somehow failed it's an error anyway.
 */
void
run_jprint_check_tool(struct jprint *jprint, char **argv)
{
    int exit_code = 0;			/* exit code for -S path execution */

    /* firewall */
    if (jprint == NULL) {
	err(57, __func__, "NULL jprint");
	not_reached();
    } else if (jprint->file_contents == NULL) {
	err(58, __func__, "NULL jprint->file_contents");
	not_reached();
    }

    /* run tool if -S used */
    if (jprint->check_tool_path != NULL) {
	/* try running via shell_cmd() first */
	if (jprint->check_tool_args != NULL) {
	    dbg(DBG_MED, "about to execute: %s %s %s >/dev/null 2>&1", jprint->check_tool_path, jprint->check_tool_args, argv[0]);
	    exit_code = shell_cmd(__func__, true, true, "% % -- %", jprint->check_tool_path, jprint->check_tool_args, argv[0]);
	} else {
	    dbg(DBG_MED, "about to execute: %s %s >/dev/null 2>&1", jprint->check_tool_path, argv[0]);
	    exit_code = shell_cmd(__func__, true, true, "% %", jprint->check_tool_path, argv[0]);
	}
	if(exit_code != 0) {
	    free_jprint(&jprint);
	    if (jprint->check_tool_args != NULL) {
		err(7, __func__, "JSON check tool '%s' with args '%s' failed with exit code: %d",/*ooo*/
			jprint->check_tool_path, jprint->check_tool_args, exit_code);
	    } else {
		err(7, __func__, "JSON check tool '%s' without args failed with exit code: %d",/*ooo*/
			jprint->check_tool_path, exit_code);
	    }
	    not_reached();
	}
	/* now open a write-only pipe */
	if (jprint->check_tool_args != NULL) {
	    jprint->check_tool_stream = pipe_open(__func__, true, true, "% % %", jprint->check_tool_path,
		    jprint->check_tool_args, argv[0]);
	} else {
	    jprint->check_tool_stream = pipe_open(__func__, true, true, "% %", jprint->check_tool_path, argv[0]);
	}
	if (jprint->check_tool_stream == NULL) {
	    free_jprint(&jprint);
	    if (jprint->check_tool_args != NULL) {
		err(7, __func__, "opening pipe to JSON check tool '%s' with args '%s' failed", /*ooo*/
			jprint->check_tool_path, jprint->check_tool_args);
	    } else {
		err(7, __func__, "opening pipe to JSON check tool '%s' without args failed", jprint->check_tool_path); /*ooo*/
	    }
	    not_reached();
	} else {
	    /* process the pipe */
	    int exit_status = 0;

	    /*
	     * write the file contents, which we know to be a valid JSON
	     * document that is NUL terminated, to the pipe.
	     */
	    fpr(jprint->check_tool_stream, __func__, "%s", jprint->file_contents);

	    /*
	     * close down the pipe to the child process and obtain the status of the pipe child process
	     */
	    exit_status = pclose(jprint->check_tool_stream);

	    /*
	     * examine the exit status of the child process and error if the child had a non-zero exit
	     */
	    if (WEXITSTATUS(exit_status) != 0) {
		free_jprint(&jprint);
		err(7, __func__, "pipe child process exited non-zero"); /*ooo*/
		not_reached();
	    } else {
		dbg(DBG_MED, "pipe child process exited OK");
	    }
	}
	jprint->check_tool_stream = NULL;
    }
}


