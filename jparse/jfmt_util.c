/*
 * jfmt_util - utility functions for JSON printer jfmt
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

#include "jfmt_util.h"

/* alloc_jfmt	    - allocate a struct jfmt *, clear it out and set defaults
 *
 * This function returns a newly allocated and cleared struct jfmt *.
 *
 * This function will never return a NULL pointer.
 */
struct jfmt *
alloc_jfmt(void)
{
    /* allocate our struct jfmt */
    struct jfmt *jfmt = calloc(1, sizeof *jfmt);

    /* verify jfmt != NULL */
    if (jfmt == NULL) {
	err(22, __func__, "failed to allocate jfmt struct");
	not_reached();
    }

    /* explicitly clear everything out and set defaults */

    /* JSON file member variables */
    jfmt->is_stdin = false;			/* true if it's stdin */
    jfmt->file_contents = NULL;		/* file.json contents */
    jfmt->json_file = NULL;			/* JSON file * */

    /* string related options */
    jfmt->encode_strings = false;		/* -e used */
    jfmt->quote_strings = false;		/* -Q used */


    /* number range options, see struct jfmt_number_range in jfmt_util.h for details */

    /* max matches number range */
    jfmt->jfmt_max_matches.number = 0;
    jfmt->jfmt_max_matches.exact = false;
    jfmt->jfmt_max_matches.range.min = 0;
    jfmt->jfmt_max_matches.range.max = 0;
    jfmt->jfmt_max_matches.range.less_than_equal = false;
    jfmt->jfmt_max_matches.range.greater_than_equal = false;
    jfmt->jfmt_max_matches.range.inclusive = false;
    jfmt->max_matches_requested = false;

    /* min matches number range */
    jfmt->jfmt_min_matches.number = 0;
    jfmt->jfmt_min_matches.exact = false;
    jfmt->jfmt_min_matches.range.min = 0;
    jfmt->jfmt_min_matches.range.max = 0;
    jfmt->jfmt_min_matches.range.less_than_equal = false;
    jfmt->jfmt_min_matches.range.greater_than_equal = false;
    jfmt->jfmt_min_matches.range.inclusive = false;
    jfmt->min_matches_requested = false;

    /* levels number range */
    jfmt->jfmt_levels.number = 0;
    jfmt->jfmt_levels.exact = false;
    jfmt->jfmt_levels.range.min = 0;
    jfmt->jfmt_levels.range.max = 0;
    jfmt->jfmt_levels.range.less_than_equal = false;
    jfmt->jfmt_levels.range.greater_than_equal = false;
    jfmt->jfmt_levels.range.inclusive = false;
    jfmt->levels_constrained = false;

    /* print related options */
    jfmt->print_json_types_option = false;		/* -p explicitly used */
    jfmt->print_json_types = JFMT_PRINT_VALUE;	/* -p type specified */
    jfmt->print_token_spaces = false;			/* -b specified */
    jfmt->num_token_spaces = 1;			/* -b specified number of spaces or tabs */
    jfmt->print_token_tab = false;			/* -b tab (or -b <num>[t]) specified */
    jfmt->print_json_levels = false;			/* -L specified */
    jfmt->num_level_spaces = 0;			/* number of spaces or tab for -L */
    jfmt->print_level_tab = false;			/* -L tab option */
    jfmt->print_colons = false;			/* -P specified */
    jfmt->print_final_comma = false;			/* -C specified */
    jfmt->print_braces = false;			/* -B specified */
    jfmt->indent_levels = false;			/* -I used */
    jfmt->indent_spaces = 0;				/* -I number of tabs or spaces */
    jfmt->indent_tab = false;				/* -I <num>[{t|s}] specified */
    jfmt->print_syntax = false;			/* -j used, will imply -p b -b 1 -c -e -Q -I 4 -t any */

    /* misc options */
    jfmt->count_only = false;				/* -c used, only show count */


    /* search related bools */
    /* json types to look for */
    jfmt->json_types_specified = false;		/* -t used */
    jfmt->json_types = JFMT_TYPE_SIMPLE;		/* -t type specified, default simple */
    jfmt->print_entire_file = false;			/* no name_arg specified */
    jfmt->match_found = false;			/* true if a pattern is specified and there is a match */
    jfmt->ignore_case = false;			/* true if -i, case-insensitive */
    jfmt->pattern_specified = false;			/* true if a pattern was specified */
    jfmt->search_value = false;			/* -Y search by value, not name. Uses print type */
    /*
     * Why is -o specified before -r? This is so that it spells out 'or' which
     * is what -o means. Obviously! :-)
     */
    jfmt->search_or_mode = false;			/* -o used: search with OR mode */
    jfmt->search_anywhere = false;			/* -r used: search under anywhere */

    jfmt->match_encoded = false;			/* -E used, match encoded name */
    jfmt->use_substrings = false;			/* -s used, matching substrings okay */
    jfmt->use_regexps = false;			/* -g used, allow grep-like regexps */
    jfmt->max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */


    /* check tool related */
    jfmt->check_tool_specified = false;		/* bool indicating -S was used */
    jfmt->check_tool_stream = NULL;			/* FILE * stream for -S tool */
    jfmt->check_tool_path = NULL;			/* -S tool_path */
    jfmt->check_tool_args = NULL;			/* -A tool_args */

    /* finally the linked list of patterns for matches */
    /* XXX - the pattern concept is incorrect */
    jfmt->patterns = NULL;
    jfmt->number_of_patterns = 0;
    /* matches - subject to change */
    jfmt->matches = NULL;
    jfmt->total_matches = 0;

    return jfmt;
}


/*
 * jfmt_match_none	- if no types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types == 0.
 */
bool
jfmt_match_none(uintmax_t types)
{
    return types == JFMT_TYPE_NONE;
}

/*
 * jfmt_match_int	- if ints should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_INT set.
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
jfmt_match_int(uintmax_t types)
{
    return (types & JFMT_TYPE_INT) != 0;
}
/*
 * jfmt_match_float	- if floats should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_FLOAT set.
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
jfmt_match_float(uintmax_t types)
{
    return (types & JFMT_TYPE_FLOAT) != 0;
}
/*
 * jfmt_match_exp	- if exponents should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_EXP set.
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
jfmt_match_exp(uintmax_t types)
{
    return (types & JFMT_TYPE_EXP) != 0;
}
/*
 * jfmt_match_num	- if numbers of any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_NUM (or any of the number types) set.
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
jfmt_match_num(uintmax_t types)
{
    return ((types & JFMT_TYPE_NUM)||(types & JFMT_TYPE_INT) || (types & JFMT_TYPE_FLOAT) ||
	    (types & JFMT_TYPE_EXP))!= 0;
}
/*
 * jfmt_match_bool	- if booleans should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_BOOL set.
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
jfmt_match_bool(uintmax_t types)
{
    return (types & JFMT_TYPE_BOOL) != 0;
}
/*
 * jfmt_match_string	    - if strings should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_STR set.
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
jfmt_match_string(uintmax_t types)
{
    return (types & JFMT_TYPE_STR) != 0;
}
/*
 * jfmt_match_null	- if null should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_NULL set.
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
jfmt_match_null(uintmax_t types)
{
    return (types & JFMT_TYPE_NULL) != 0;
}
/*
 * jfmt_match_object	    - if objects should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_OBJECT set.
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
jfmt_match_object(uintmax_t types)
{
    return (types & JFMT_TYPE_OBJECT) != 0;
}
/*
 * jfmt_match_array	    - if arrays should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JFMT_TYPE_ARRAY set.
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
jfmt_match_array(uintmax_t types)
{
    return (types & JFMT_TYPE_ARRAY) != 0;
}
/*
 * jfmt_match_any	- if any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types is equal to JFMT_TYPE_ANY.
 *
 * Why does it have to equal JFMT_TYPE_ANY if it checks for any type? Because
 * the point is that if JFMT_TYPE_ANY is set it can be any type but not
 * specific types. For the specific types those bits have to be set instead. If
 * JFMT_TYPE_ANY is set then any type can be set but if types is say
 * JFMT_TYPE_INT then checking for JFMT_TYPE_INT & JFMT_TYPE_ANY would be
 * != 0 (as it's a bitwise OR of all the types) which would suggest that any
 * type is okay even if JFMT_TYPE_INT was the only type.
 */
bool
jfmt_match_any(uintmax_t types)
{
    return types == JFMT_TYPE_ANY;
}
/*
 * jfmt_match_simple	- if simple types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Simple is defined as a number, a bool, a string or a null.
 *
 * Returns true if types has JFMT_TYPE_SIMPLE set.
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
jfmt_match_simple(uintmax_t types)
{
    return (types & JFMT_TYPE_SIMPLE) != 0;
}
/*
 * jfmt_match_compound   - if compounds should match
 *
 * given:
 *
 *	types	- types set
 *
 * A compound is defined as an object or array.
 *
 * Returns true if types has JFMT_TYPE_COMPOUND set.
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
jfmt_match_compound(uintmax_t types)
{
    return (types & JFMT_TYPE_COMPOUND) != 0;
}

/*
 * jfmt_parse_types_option	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JFMT_TYPE_SIMPLE (as if '-t simple').
 */
uintmax_t
jfmt_parse_types_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;	    /* strdup()d copy of optarg */

    uintmax_t type = JFMT_TYPE_SIMPLE; /* default is simple: num, bool, str and null */

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
	    type |= JFMT_TYPE_INT;
	} else if (!strcmp(p, "float")) {
	    type |= JFMT_TYPE_FLOAT;
	} else if (!strcmp(p, "exp")) {
	    type |= JFMT_TYPE_EXP;
	} else if (!strcmp(p, "num")) {
	    type |= JFMT_TYPE_NUM;
	} else if (!strcmp(p, "bool")) {
	    type |= JFMT_TYPE_BOOL;
	} else if (!strcmp(p, "str")) {
	    type |= JFMT_TYPE_STR;
	} else if (!strcmp(p, "null")) {
	    type |= JFMT_TYPE_NULL;
	} else if (!strcmp(p, "object")) {
	    type |= JFMT_TYPE_OBJECT;
	} else if (!strcmp(p, "array")) {
	    type |= JFMT_TYPE_ARRAY;
	} else if (!strcmp(p, "simple")) {
	    type |= JFMT_TYPE_SIMPLE;
	} else if (!strcmp(p, "compound")) {
	    type |= JFMT_TYPE_COMPOUND;
	} else if (!strcmp(p, "any")) {
	    type |= JFMT_TYPE_ANY;
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
 * jfmt_print_name	- if only names should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JFMT_PRINT_NAME set.
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
jfmt_print_name(uintmax_t types)
{
    return ((types & JFMT_PRINT_NAME) && !(types & JFMT_PRINT_VALUE)) != 0;
}
/*
 * jfmt_print_value	- if only values should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JFMT_PRINT_VALUE set.
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
jfmt_print_value(uintmax_t types)
{
    return ((types & JFMT_PRINT_VALUE) && !(types & JFMT_PRINT_NAME)) != 0;
}
/*
 * jfmt_print_both	- if names AND values should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types has JFMT_PRINT_BOTH set.
 */
bool
jfmt_print_name_value(uintmax_t types)
{
    return types == JFMT_PRINT_BOTH;
}


/*
 * jfmt_parse_print_option	- parse -p option list
 *
 * given:
 *
 *	optarg	    - option argument to -p option
 *
 * Returns: bitvector of types to print.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JFMT_PRINT_VALUE (as if '-p v').
 */
uintmax_t
jfmt_parse_print_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;	    /* strdup()d copy of optarg */

    uintmax_t print_json_types = 0; /* default is to print values */

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return JFMT_PRINT_VALUE;
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
	    print_json_types |= JFMT_PRINT_VALUE;
	} else if (!strcmp(p, "n") || !strcmp(p, "name")) {
	    print_json_types |= JFMT_PRINT_NAME;
	} else if (!strcmp(p, "b") || !strcmp(p, "both")) {
	    print_json_types |= JFMT_PRINT_BOTH;
	} else {
	    /* unknown keyword */
	    err(3, __func__, "unknown keyword '%s'", p); /*ooo*/
	    not_reached();
	}
    }

    if (jfmt_print_name_value(print_json_types)) {
	dbg(DBG_LOW, "will print both name and value");
    }
    else if (jfmt_print_name(print_json_types)) {
	dbg(DBG_LOW, "will only print name");
    }
    else if (jfmt_print_value(print_json_types)) {
	dbg(DBG_LOW, "will only print value");
    }

    if (dup != NULL) {
	free(dup);
	dup = NULL;
    }

    return print_json_types;
}

/* jfmt_parse_number_range	- parse a number range for options -l, -N, -n
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
 * See also the structs jfmt_number_range and jfmt_number in jfmt_util.h
 * for more details.
 *
 * NOTE: this function does not return on syntax error or NULL number.
 */
bool
jfmt_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jfmt_number *number)
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
	memset(number, 0, sizeof(struct jfmt_number));

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

/* jfmt_number_in_range   - check if number is in required range
 *
 * given:
 *
 *	number		- number to check
 *	total_matches	- total number of matches found
 *	range		- pointer to struct jfmt_number with range
 *
 * Returns true if the number is in range.
 *
 * NOTE: if range is NULL it will return false.
 */
bool
jfmt_number_in_range(intmax_t number, intmax_t total_matches, struct jfmt_number *range)
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

/* jfmt_parse_st_tokens_option    - parse -b [num]{s,t}/-b tab option
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
jfmt_parse_st_tokens_option(char *optarg, uintmax_t *num_token_spaces, bool *print_token_tab)
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
	err(3, __func__, "couldn't parse -b <num>[ts]"); /*ooo*/
	not_reached();
    } else {
	*print_token_tab = false; /* ensure it's false in case specified previously */
	dbg(DBG_LOW, "will print %jd space%s between name and value", *num_token_spaces,
		*num_token_spaces==1?"":"s");
    }
}

/* jfmt_parse_st_indent_option    - parse -I [num]{s,t}/-b indent option
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
jfmt_parse_st_indent_option(char *optarg, uintmax_t *indent_level, bool *indent_tab)
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
	err(3, __func__, "couldn't parse -I spaces"); /*ooo*/
	not_reached();
    } else {
	*indent_tab = false; /* ensure it's false in case specified previously */
	dbg(DBG_LOW, "will ident with %jd space%s after levels", *indent_level, *indent_level==1?"":"s");
    }
}

/* jfmt_parse_st_level_option    - parse -L [num]{s,t}/-b level option
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
jfmt_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab)
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
 * jfmt_parse_value_option	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JFMT_TYPE_SIMPLE (as if '-t simple').
 */
uintmax_t
jfmt_parse_value_type_option(char *optarg)
{
    uintmax_t type = JFMT_TYPE_SIMPLE; /* default is simple: num, bool, str and null */
    char *p = NULL;

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    }
    p = optarg;

    /* Determine if the arg is a valid type.  */
    if (!strcmp(p, "int")) {
	type = JFMT_TYPE_INT;
    } else if (!strcmp(p, "float")) {
	type = JFMT_TYPE_FLOAT;
    } else if (!strcmp(p, "exp")) {
	type = JFMT_TYPE_EXP;
    } else if (!strcmp(p, "num")) {
	type = JFMT_TYPE_NUM;
    } else if (!strcmp(p, "bool")) {
	type = JFMT_TYPE_BOOL;
    } else if (!strcmp(p, "str")) {
	type = JFMT_TYPE_STR;
    } else if (!strcmp(p, "null")) {
	type = JFMT_TYPE_NULL;
    } else if (!strcmp(p, "simple")) {
	type = JFMT_TYPE_SIMPLE;
    } else {
	/* unknown or unsupported type */
	err(3, __func__, "unknown or unsupported type '%s'", p); /*ooo*/
	not_reached();
    }

    return type;
}

/*
 * free_jfmt	    - free jfmt struct
 *
 * given:
 *
 *	jfmt	    - a struct jfmt **
 *
 * This function will do nothing other than warn on NULL pointer (even though
 * it's safe to free a NULL pointer though if jfmt itself was NULL it would be
 * an error to dereference it).
 *
 * We pass a struct jfmt ** so that in the caller jfmt can be set to NULL to
 * remove the need to repeatedly set it to NULL each time this function is
 * called. This way we remove the need to do more than just call this function.
 */
void
free_jfmt(struct jfmt **jfmt)
{
    struct jfmt_match *match = NULL; /* to iterate through matches list */
    struct jfmt_match *next_match = NULL; /* next in list */

    /* firewall */
    if (jfmt == NULL || *jfmt == NULL) {
	warn(__func__, "passed NULL struct jfmt ** or *jfmt is NULL");
	return;
    }

    free_jfmt_patterns_list(*jfmt); /* free patterns list first */

    /* we have to free matches attached to jfmt itself too */
    for (match = (*jfmt)->matches; match != NULL; match = next_match) {
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


    free(*jfmt);
    *jfmt = NULL;
}


/* parse_jfmt_name_args - add name_args to patterns list
 *
 * given:
 *
 *	jfmt	    - pointer to our struct jfmt
 *	argv	    - argv from main()
 *
 * This function will not return on NULL pointers.
 *
 * NOTE: by patterns we refer to name_args.
 *
 * XXX - the pattern concept is currently incorrect and needs to be fixed - XXX
 */
void
parse_jfmt_name_args(struct jfmt *jfmt, char **argv)
{
    int i;  /* to iterate through argv */

    /* firewall */
    if (argv == NULL) {
	err(15, __func__, "argv is NULL"); /*ooo*/
	not_reached();
    }

    for (i = 1; argv[i] != NULL; ++i) {
	jfmt->pattern_specified = true;

	if (add_jfmt_pattern(jfmt, jfmt->use_regexps, jfmt->use_substrings, argv[i]) == NULL) {
	    err(25, __func__, "failed to add pattern (substrings %s) '%s' to patterns list",
		    jfmt->use_substrings?"OK":"ignored", argv[i]);
	    not_reached();
	}
    }
}


/*
 * add_jfmt_pattern
 *
 * Add jfmt pattern to the jfmt struct pattern list.
 *
 * given:
 *
 *	jfmt		- pointer to the jfmt struct
 *	use_regexp	- whether to use regexp or not
 *	use_substrings	- if -s was specified, make this a substring match
 *	str		- the pattern to be added to the list
 *
 * NOTE: this function will not return if jfmt is NULL. If str is NULL
 * this function will not return but if str is empty it will add an empty
 * string to the list. However the caller will usually check that it's not empty
 * prior to calling this function.
 *
 * Returns a pointer to the newly allocated struct jfmt_pattern * that was
 * added to the jfmt patterns list.
 *
 * Duplicate patterns will not be added (case sensitive).
 */
struct jfmt_pattern *
add_jfmt_pattern(struct jfmt *jfmt, bool use_regexp, bool use_substrings, char *str)
{
    struct jfmt_pattern *pattern = NULL;
    struct jfmt_pattern *tmp = NULL;

    /*
     * firewall
     */
    if (jfmt == NULL) {
	err(26, __func__, "passed NULL jfmt struct");
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
    for (pattern = jfmt->patterns; pattern != NULL; pattern = pattern->next) {
	if (pattern->pattern && pattern->use_regexp == use_regexp) {
	    /* XXX - add support for regexps - XXX */
	    if ((!jfmt->ignore_case && !strcmp(pattern->pattern, str))||
		(jfmt->ignore_case && strcasecmp(pattern->pattern, str))) {
		return pattern;
	    }
	}
    }
    /*
     * XXX either change the debug level or remove this message once
     * processing is complete
     */
    if (use_regexp) {
	dbg(DBG_LOW,"%s regex requested: '%s'", jfmt->search_value?"value":"name", str);
    } else {
	dbg(DBG_LOW,"%s pattern requested: '%s'", jfmt->search_value?"value":"name", str);
    }

    errno = 0; /* pre-clear errno for errp() */
    pattern = calloc(1, sizeof *pattern);
    if (pattern == NULL) {
	errp(28, __func__, "unable to allocate struct jfmt_pattern *");
	not_reached();
    }

    errno = 0;
    pattern->pattern = strdup(str);
    if (pattern->pattern == NULL) {
	errp(29, __func__, "unable to strdup string '%s' for patterns list", str);
	not_reached();
    }

    pattern->use_regexp = use_regexp;
    pattern->use_value = jfmt->search_value;
    pattern->use_substrings = use_substrings;
    /* increment how many patterns have been specified */
    ++jfmt->number_of_patterns;
    /* let jfmt know that a pattern was indeed specified */
    jfmt->pattern_specified = true;
    pattern->matches_found = 0; /* 0 matches found at first */

    dbg(DBG_LOW, "adding %s pattern '%s' to patterns list", pattern->use_value?"value":"name", pattern->pattern);

    for (tmp = jfmt->patterns; tmp && tmp->next; tmp = tmp->next)
	; /* on its own line to silence useless and bogus warning -Wempty-body */

    if (!tmp) {
	jfmt->patterns = pattern;
    } else {
	tmp->next = pattern;
    }

    return pattern;
}


/* free_jfmt_patterns_list	- free patterns list in a struct jfmt *
 *
 * given:
 *
 *	jfmt	    - the jfmt struct
 *
 * If the jfmt patterns list is empty this function will do nothing.
 *
 * NOTE: this function does not return on a NULL jfmt.
 *
 * NOTE: this function calls free_jfmt_matches_list() on all the patterns
 * prior to freeing the pattern itself.
 */
void
free_jfmt_patterns_list(struct jfmt *jfmt)
{
    struct jfmt_pattern *pattern = NULL; /* to iterate through patterns list */
    struct jfmt_pattern *next_pattern = NULL; /* next in list */

    if (jfmt == NULL) {
	err(30, __func__, "passed NULL jfmt struct");
	not_reached();
    }

    for (pattern = jfmt->patterns; pattern != NULL; pattern = next_pattern) {
	next_pattern = pattern->next;

	/* first free any matches */
	free_jfmt_matches_list(pattern);

	/* now free the pattern string itself */
	if (pattern->pattern) {
	    free(pattern->pattern);
	    pattern->pattern = NULL;
	}

	/* finally free the pattern and set to NULL for the next pass */
	free(pattern);
	pattern = NULL;
    }

    jfmt->patterns = NULL;
}


/*
 * add_jfmt_match
 *
 * Add jfmt pattern match to the jfmt struct pattern match list.
 *
 * given:
 *
 *	jfmt		- pointer to the jfmt struct
 *	pattern		- the pattern that matched
 *	name		- struct json * name that matched
 *	value		- struct json * value that matched
 *	name_str	- the matching value, either a value or name
 *	value_str	- the matching value, the opposite of name_str
 *	level		- the depth or level for the -l / -L options (level 0 is top of tree)
 *	string		- boolean to indicate if the match is a string
 *	name_type	- enum item_type indicating the type of name node (JTYPE_* in json_parse.h)
 *	value_type	- enum item_type indicating the type of value node (JTYPE_* in json_parse.h)
 *
 * NOTE: this function will not return if any of the pointers are NULL (except
 * the name and value - for now) including the pointers in the pattern struct.
 *
 * Returns a pointer to the newly allocated struct jfmt_match * that was
 * added to the jfmt matched patterns list.
 *
 * NOTE: depending on jfmt->search_value the name and value nodes will be in a
 * different order. Specifically the name is what matched, whether a value in
 * the json tree or name, and the value is what will be printed. At least once
 * this feature is done :-)
 */
struct jfmt_match *
add_jfmt_match(struct jfmt *jfmt, struct jfmt_pattern *pattern, struct json *name,
	struct json *value, char const *name_str, char const *value_str, uintmax_t level,
	enum item_type name_type, enum item_type value_type)
{
    struct jfmt_match *match = NULL;
    struct jfmt_match *tmp = NULL;

    /*
     * firewall
     */
    if (jfmt == NULL) {
	err(31, __func__, "passed NULL jfmt struct");
	not_reached();
    }

    if (name_str == NULL) {
	err(32, __func__, "passed NULL name_str");
	not_reached();
    }
    if (value_str == NULL) {
	err(33, __func__, "value_str is NULL");
	not_reached();
    }

    /*
     * search for an exact match and only increment the count if found.
     *
     * NOTE: this means that when printing the output we have to go potentially
     * print the match more than once; if -c is specified we print only the
     * count.
     */
    for (tmp = pattern?pattern->matches:jfmt->matches; tmp; tmp = tmp->next) {
	if (name_type == tmp->name_type) {
	    /* XXX - add support for regexps - XXX */
	    if (((!jfmt->ignore_case && !strcmp(tmp->match, value_str) && !strcmp(tmp->match, value_str)))||
		(jfmt->ignore_case && !strcasecmp(tmp->match, value_str) && !strcasecmp(tmp->match, value_str))) {
		    dbg(DBG_LOW, "incrementing count of match '%s' to %ju", tmp->match, tmp->count + 1);
		    jfmt->total_matches++;
		    tmp->count++;
		    return tmp;
	    }
	}
    }

    /* if we get here we have to add the match to the matches list */
    errno = 0; /* pre-clear errno for errp() */
    match = calloc(1, sizeof *match);
    if (match == NULL) {
	errp(34, __func__, "unable to allocate struct jfmt_match *");
	not_reached();
    }

    /* duplicate the match (name_str) */
    errno = 0; /* pre-clear errno for errp() */
    match->match = strdup(name_str);
    if (match->match == NULL) {
	errp(35, __func__, "unable to strdup string '%s' for match list", name_str);
	not_reached();
    }

    /* duplicate the value of the match, either name or value */
    errno = 0; /* pre-clear errno for errp() */
    match->value = strdup(value_str);
    if (match->match == NULL) {
	errp(36, __func__, "unable to strdup value string '%s' for match list", value_str);
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

    /* increment total matches of ALL patterns (name_args) in jfmt struct */
    jfmt->total_matches++;

    /* record types */
    match->name_type = name_type;
    match->value_type = value_type;

    dbg(DBG_LOW, "adding match '%s' to pattern '%s' to match list",
	    jfmt->search_value?match->value:match->match, name_str);

    for (tmp = pattern?pattern->matches:jfmt->matches; tmp && tmp->next; tmp = tmp->next)
	; /* on its own line to silence useless and bogus warning -Wempty-body */

    if (!tmp) {
	if (pattern != NULL) {
	    pattern->matches = match;
	} else {
	    jfmt->matches = match;
	}
    } else {
	tmp->next = match;
    }

    /* a match is found, set jfmt->match_found to true */
    jfmt->match_found = true;

    return match;
}

/* free_jfmt_matches_list   - free matches list in a struct jfmt_pattern *
 *
 * given:
 *
 *	pattern	    - the jfmt pattern
 *
 * If the jfmt patterns match list is empty this function will do nothing.
 *
 * NOTE: this function does not return on a NULL pattern.
 */
void
free_jfmt_matches_list(struct jfmt_pattern *pattern)
{
    struct jfmt_match *match = NULL; /* to iterate through matches list */
    struct jfmt_match *next_match = NULL; /* next in list */

    if (pattern == NULL) {
	err(37, __func__, "passed NULL pattern struct");
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

/* is_jfmt_match	- if the name or value is a match based on type
 *
 * given:
 *
 *	jfmt	    - pointer to our struct jfmt
 *	pattern	    - pointer to the pattern
 *	name	    - char const * that is the matching name (if pattern == NULL)
 *	node	    - struct json node
 *	str	    - the string to compare
 *
 * Returns true if a match; else false.
 *
 * This function will not return if jfmt or node or str is NULL. The pattern
 * and name pointers can be NULL unless both are NULL.
 */
bool
is_jfmt_match(struct jfmt *jfmt, struct jfmt_pattern *pattern, char const *name, struct json *node, char const *str)
{
    /* firewall */
    if (jfmt == NULL) {
	err(38, __func__, "jfmt is NULL");
	not_reached();
    } else if ((pattern == NULL || pattern->pattern == NULL) && name == NULL) {
	err(39, __func__, "pattern and name are both NULL");
	not_reached();
    } else if (node == NULL) {
	err(40, __func__, "node is NULL");
	not_reached();
    } else if (str == NULL) {
	err(41, __func__, "str is NULL");
	not_reached();
    }

    /* set up name if NULL to be pattern->pattern */
    if (name == NULL) {
	name = pattern->pattern;
    }

    /* check that name != NULL */
    if (name == NULL) {
	err(42, __func__, "name is NULL");
	not_reached();
    }

    /*
     * if there is a match found add it to the matches list
     *
     * XXX - an issue that must be worked out is that if one does something
     * like:
     *
     *	    jfmt -Y int -j h2g2.json 42
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

		if (item != NULL && item->converted) {
		    if (!jfmt->search_value || jfmt_match_num(jfmt->json_types) ||
			(item->is_integer&&jfmt_match_int(jfmt->json_types))||
			(item->is_floating && jfmt_match_float(jfmt->json_types)) ||
			(item->is_e_notation && jfmt_match_exp(jfmt->json_types))) {
			    if (jfmt->use_substrings) {
				if (strstr(str, name) ||
				    (jfmt->ignore_case&&strcasestr(str, name))) {
					return true;
				}
			    } else {
				if (!strcmp(name, str) ||
				    (jfmt->ignore_case&&!strcasecmp(name, str))) {
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

		if (item != NULL && item->converted) {
		    if (!jfmt->search_value || jfmt_match_string(jfmt->json_types)) {
			if (jfmt->use_substrings) {
			    if (strstr(str, name) ||
				(jfmt->ignore_case && strcasestr(str, name))) {
				    return true;
			    }
			} else {
			    if (!strcmp(name, str) ||
				(jfmt->ignore_case && !strcasecmp(name, str))) {
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

		if (item != NULL && item->converted) {
		    if (!jfmt->search_value || jfmt_match_bool(jfmt->json_types)) {
			if (jfmt->use_substrings) {
			    if (strstr(str, name) ||
				(jfmt->ignore_case && strcasestr(str, name))) {
				    return true;
			    }

			} else {
			    if (!strcmp(name, str) ||
				(jfmt->ignore_case && !strcasecmp(name, str))) {
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

		if (item != NULL && item->converted) {
		    if (!jfmt->search_value || jfmt_match_null(jfmt->json_types)) {
			if (jfmt->use_substrings) {
			    if (strstr(str, name) ||
				(jfmt->ignore_case && strcasestr(str, name))) {
				    return true;
			    }
			} else {
			    if (!strcmp(name, str) ||
				(jfmt->ignore_case && !strcasecmp(name, str))) {
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
 * jfmt_json_search
 *
 * Print information about a JSON node, depending on the booleans in struct
 * jfmt if the tree node matches the name or value in any pattern in the
 * struct json.
 *
 * given:
 *	jfmt	    pointer to our struct jfmt
 *	name_node   pointer to a JSON parser tree name node to try and match or add
 *	value_node  pointer to a JSON parser tree value node to try and match or add
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
 *	jfmt_json_search(node, true, depth, JSON_DBG_MED);
 *	jfmt_json_search(node, false, depth, JSON_DBG_FORCED;
 *	jfmt_json_search(node, false, depth, JSON_DBG_MED);
 *
 * While the ... variable arg are ignored, we need to declare
 * then in order to be in vcallback form for use by json_tree_walk().
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if jfmt == NULL or node == NULL.
 */
void
jfmt_json_search(struct jfmt *jfmt, struct json *name_node, struct json *value_node, bool is_value, unsigned int depth, ...)
{
    va_list ap;		/* variable argument list */

    /* firewall */
    if (jfmt == NULL || (name_node == NULL&&value_node == NULL)) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, depth);

    /*
     * call va_list argument list function
     */
    vjfmt_json_search(jfmt, name_node, value_node, is_value, depth, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);
    return;
}


/*
 * vjfmt_json_search
 *
 * Search for matches in a JSON node, depending on the booleans in struct
 * jfmt if the tree node matches the name or value in any pattern in the
 * struct json.
 *
 * This is a variable argument list interface to jfmt_json_search().
 *
 * See jfmt_json_tree_search() to go through the entire tree.
 *
 * given:
 *	jfmt	    pointer to our struct json
 *	name_node   pointer to a JSON parser tree name node to search
 *	value_node  pointer to a JSON parser tree value node to search
 *	is_value    boolean to indicate if this is a value or name
 *	depth	    current tree depth (0 ==> top of tree)
 *	ap	    variable argument list, required ap args:
 *
 *			json_dbg_lvl	print message if JSON_DBG_FORCED
 *					OR if <= json_verbosity_level
 *
 * NOTE: This function does nothing if jfmt == NULL or jfmt->patterns ==
 * NULL or if none of the names/values match any of the patterns or node ==
 * NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 *
 * NOTE: this function is incomplete and does not do everything correctly. Any
 * problems will be fixed in a future commit.
 */
void
vjfmt_json_search(struct jfmt *jfmt, struct json *name_node, struct json *value_node, bool is_value,
	unsigned int depth, va_list ap)
{
    FILE *stream = NULL;	/* stream to print on */
    int json_dbg_lvl = JSON_DBG_DEFAULT;	/* JSON debug level if json_dbg_used */
    struct jfmt_pattern *pattern = NULL; /* to iterate through patterns list */
    va_list ap2;		/* copy of va_list ap */

    /*
     * firewall - nothing to do for NULL jfmt or NULL patterns list or NULL
     * nodes
     */
    if (jfmt == NULL || jfmt->patterns == NULL || name_node == NULL || value_node == NULL) {
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
    if (((!jfmt->search_value && is_value) || (!is_value && jfmt->search_value))) {
	va_end(ap2); /* stdarg variable argument list clean up */
	return;
    }

    /* only search for matches if level constraints allow it */
    if (!jfmt->levels_constrained || jfmt_number_in_range(depth, jfmt->number_of_patterns, &jfmt->jfmt_levels))
    {
	for (pattern = jfmt->patterns; pattern != NULL; pattern = pattern->next) {
	    /* XXX: for each pattern we have to find a match and then add it to
	     * the matches list of that pattern. After that we can go through
	     * the matches found and print out the matches as desired by the
	     * user. We will not add matches if the constraints do not allow it.
	     *
	     * However note that we currently do not have a working way to check
	     * if the node is a value or name so what ends up happening is that
	     * a value can match as a name and vice versa. See above comment.
	     */

	    /* get the name and value */
	    struct json *name = jfmt->search_value?value_node:name_node;
	    struct json *value = jfmt->search_value?name_node:value_node;

	    /*
	     * Get strings of name and value. NULL is checked prior to use,
	     * below
	     */
	    char const *str = name  != NULL ? json_get_type_str(name, jfmt->match_encoded) : NULL;
	    char const *val = value != NULL ? json_get_type_str(value, jfmt->match_encoded) : NULL;

	    if (name != NULL) {
		switch (name->type) {

		    case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
			break;

		    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
			{
			    if (str != NULL) {
				switch (value->type) {
				    case JTYPE_STRING:
					{
					    if (val != NULL) {
						if (is_jfmt_match(jfmt, pattern, pattern->pattern, name, str)) {
						    if (add_jfmt_match(jfmt, pattern, name, value,
							str, val, depth, jfmt->search_value?JTYPE_STRING:JTYPE_NUMBER,
							jfmt->search_value?JTYPE_NUMBER:JTYPE_STRING) == NULL) {
							    err(43, __func__, "adding match '%s' to pattern failed", str);
							    not_reached();
						    }
						}
					}
				    }
				    break;
				    default:
					/* XXX - determine if other types need to be handled */
				    break;
				    }
			    }
			}
			break;

		    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */
			{
			    if (str != NULL) {
				switch (value->type) {
				    case JTYPE_NUMBER:
				    {
					if (val != NULL) {
					    if (is_jfmt_match(jfmt, pattern, pattern->pattern, name,
						jfmt->search_value?val:str)) {
						    if (add_jfmt_match(jfmt, pattern, name, value,
							str, val, depth,
							name->type, value->type) == NULL) {
							    err(44, __func__, "adding match '%s' to pattern failed", str);
							    not_reached();
						    }
					    }
					}
				    }
				    break;
				    case JTYPE_STRING:
				    {
					if (val != NULL) {
					    if (is_jfmt_match(jfmt, pattern, pattern->pattern, name, str)) {
						if (add_jfmt_match(jfmt, pattern, name, value, str, val,
						    depth, JTYPE_STRING, JTYPE_STRING) == NULL) {
							err(45, __func__, "adding match '%s' to pattern failed", str);
							not_reached();
						}
					    }
					}
				    }
				    break;
				    case JTYPE_BOOL:
				    {
					if (val != NULL) {
					    if (is_jfmt_match(jfmt, pattern, pattern->pattern, name, str)) {
						if (add_jfmt_match(jfmt, pattern, name, value, str, val,
						    depth, jfmt->search_value?JTYPE_BOOL:JTYPE_STRING,
						    jfmt->search_value?JTYPE_STRING:JTYPE_BOOL) == NULL) {
							err(46, __func__, "adding match '%s' to pattern failed", str);
							not_reached();
						}
					    }
					}
				    }
				    break;
				    case JTYPE_NULL:
				    {
					if (val != NULL) {
					    if (is_jfmt_match(jfmt, pattern, pattern->pattern, name, str)) {
						if (add_jfmt_match(jfmt, pattern, name, value, str, val,
						    depth, jfmt->search_value?JTYPE_NULL:JTYPE_STRING,
						    jfmt->search_value?JTYPE_STRING:JTYPE_NULL) == NULL) {
							err(47, __func__, "adding match '%s' to pattern failed", str);
							not_reached();
						}
					    }
					}

				    }
				    break;
				    default:
					break;
				}
			    }
			}
			break;

		    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
			{
			    if (str != NULL) {
				switch (value->type) {
				    case JTYPE_STRING:
				    {
					if (val != NULL) {
					    if (is_jfmt_match(jfmt, pattern, pattern->pattern, name, str)) {
						if (add_jfmt_match(jfmt, pattern, name, value, str, val,
						    depth, jfmt->search_value?JTYPE_STRING:JTYPE_BOOL,
						    jfmt->search_value?JTYPE_BOOL:JTYPE_STRING) == NULL) {
							err(48, __func__, "adding match '%s' to pattern failed", str);
							not_reached();
						}
					    }
					}
				    }
				    break;
				    default: /* only string is valid */
					break;
				}
			    }
			}
			break;

		    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
			{
			    if (str != NULL) {
				switch (value->type) {
				    case JTYPE_STRING:
				    {
					if (val != NULL) {
					    if (is_jfmt_match(jfmt, pattern, pattern->pattern, name, str)) {
						if (add_jfmt_match(jfmt, pattern, name, value, str, val,
						    depth, jfmt->search_value?JTYPE_STRING:JTYPE_NULL,
						    jfmt->search_value?JTYPE_NULL:JTYPE_STRING) == NULL) {
							err(49, __func__, "adding match '%s' to pattern failed", str);
							not_reached();
						}
					    }
					}
				    }
				    break;
				    default: /* only string is valid */
					break;
				}
			    }
			}
			break;

		    case JTYPE_MEMBER:	/* JSON item is a member - see struct json_member */
			{
			    struct json_member *item = &(name->item.member);

			    /* XXX - fix to check for match of the member and add to
			     * the matches list if it fits within constraints - XXX */
			    UNUSED_ARG(item);
			}
			break;

		    case JTYPE_OBJECT:	/* JSON item is a { members } - see struct json_object */
			{
			    struct json_object *item = &(name->item.object);

			    /* XXX - fix to check for match of the object and add to
			     * the matches list if it fits within constraints - XXX */
			    UNUSED_ARG(item);
			}
			break;

		    case JTYPE_ARRAY:	/* JSON item is a [ elements ] - see struct json_array */
			{
			    struct json_array *item = &(name->item.array);

			    /* XXX - fix to check for match of the array and add it
			     * to the matches list if it fits within the constraints - XXX */
			    UNUSED_ARG(item);
			}
			break;

		    case JTYPE_ELEMENTS:	/* JSON elements is zero or more JSON values - see struct json_elements */
			{
			    struct json_elements *item = &(name->item.elements);

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
    }
    /*
     * stdarg variable argument list clean up
     */
    va_end(ap2);
    return;
}


/*
 * jfmt_json_tree_search - search nodes of an entire JSON parse tree
 *
 * This function uses the jfmt_json_tree_walk() interface to walk
 * the JSON parse tree and print requested information about matching nodes.
 *
 * given:
 *	jfmt	    pointer to our struct jfmt
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
 *	jfmt_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_FORCED);
 *	jfmt_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_FORCED);
 *	jfmt_json_tree_search(tree, JSON_DEFAULT_MAX_DEPTH, JSON_DBG_MED);
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if jfmt == NULL, jfmt->patterns == NULL
 * or node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 *
 * NOTE: this function is a wrapper to jfmt_json_tree_walk() with the callback
 * vjfmt_json_search().
 */
void
jfmt_json_tree_search(struct jfmt *jfmt, struct json *node, unsigned int max_depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (jfmt == NULL || jfmt->patterns == NULL || node == NULL) {
	return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, max_depth);

    /*
     * walk the JSON parse tree
     */
    jfmt_json_tree_walk(jfmt, node, node, max_depth, false, 0, vjfmt_json_search, ap);

    /*
     * stdarg variable argument list clean up
     */
    va_end(ap);
    return;
}

/*
 * jfmt_json_tree_walk - walk a JSON parse tree calling a function on each node in va_list form
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
 *	jfmt_json_tree_walk(jfmt, tree, JSON_DEFAULT_MAX_DEPTH, 0, json_free);
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
jfmt_json_tree_walk(struct jfmt *jfmt, struct json *lnode, struct json *rnode, bool is_value,
	unsigned int max_depth, unsigned int depth, void (*vcallback)(struct jfmt *, struct json *, struct json *, bool,
	unsigned int, va_list), va_list ap)
{
    int i;

    /*
     * firewall
     */
    if (lnode == NULL || rnode == NULL) {
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
    switch (lnode->type) {

    case JTYPE_UNSET:	/* JSON item has not been set - must be the value 0 */
    case JTYPE_NULL:	/* JSON item is a null - see struct json_null */
    case JTYPE_BOOL:	/* JSON item is a boolean - see struct json_boolean */
    case JTYPE_NUMBER:	/* JSON item is number - see struct json_number */
	/* perform function operation on this terminal parse tree node, all of
	 * which have to be a value */
	(*vcallback)(jfmt, lnode, rnode, true, depth, ap);
	break;

    case JTYPE_STRING:	/* JSON item is a string - see struct json_string */

	/* perform function operation on this terminal parse tree node */
	(*vcallback)(jfmt, lnode, rnode, is_value, depth, ap);
	break;

    case JTYPE_MEMBER:	/* JSON item is a member */
	{
	    struct json_member *item = &(lnode->item.member);

	    /* perform function operation on JSON member name (left branch) node */
	    jfmt_json_tree_walk(jfmt, item->name, item->value, false, max_depth, depth+1, vcallback, ap);

	    /* perform function operation on JSON member value (right branch) node */
	    jfmt_json_tree_walk(jfmt, item->name, item->value, true, max_depth, depth+1, vcallback, ap);
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jfmt, lnode, rnode, is_value, depth+1, ap);
	break;

    case JTYPE_OBJECT:	/* JSON item is a { members } */
	{
	    struct json_object *item = &(lnode->item.object);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jfmt_json_tree_walk(jfmt, item->set[i], item->set[i], is_value, max_depth, depth, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jfmt, lnode, rnode, is_value, depth+1, ap);
	break;

    case JTYPE_ARRAY:	/* JSON item is a [ elements ] */
	{
	    struct json_array *item = &(lnode->item.array);

	    /* perform function operation on each object member in order */

	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jfmt_json_tree_walk(jfmt, item->set[i], item->set[i], true, max_depth, depth, vcallback, ap);
		}
	    }
	}

	/* just call callback on the array node */
	(*vcallback)(jfmt, lnode, rnode, is_value, depth+1, ap);
	break;

    case JTYPE_ELEMENTS:	/* JSON items is zero or more JSON values */
	{
	    struct json_elements *item = &(lnode->item.elements);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    jfmt_json_tree_walk(jfmt, item->set[i], item->set[i], true, max_depth, depth, vcallback, ap);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*vcallback)(jfmt, lnode, rnode, is_value, depth+1, ap);
	break;

    default:
	warn(__func__, "node type is unknown: %d", lnode->type);
	/* nothing we can traverse */
	break;
    }
    return;
}


/* jfmt_print_count	- print total matches if -c used
 *
 * given:
 *
 *	jfmt	    - pointer to our struct jfmt
 *
 * This function will not return on NULL jfmt.
 *
 * This function returns false if -c was not used and true if it was.
 */
bool
jfmt_print_count(struct jfmt *jfmt)
{
    /* firewall */
    if (jfmt == NULL) {
	err(50, __func__, "jfmt is NULL");
	not_reached();
    }

    if (jfmt->count_only) {
	print("%ju\n", jfmt->total_matches);
	return true;
    }

    return false;
}


/* jfmt_print_final_comma	- print final comma if -C used
 *
 * given:
 *
 *	jfmt	    - pointer to our struct jfmt
 *
 * This function will not return on NULL jfmt.
 *
 * This function does nothing if -C was not used or if -c was used.
 */
void
jfmt_print_final_comma(struct jfmt *jfmt)
{
    /* firewall */
    if (jfmt == NULL) {
	err(51, __func__, "jfmt is NULL");
	not_reached();
    }

    if (jfmt->print_final_comma && !jfmt->count_only) {
	print("%c", ',');
    }
}


/* jfmt_print_brace - print a brace if -B used
 *
 * given:
 *
 *	jfmt	    - pointer to our jfmt struct
 *	open	    - boolean indicating if we need an open or close brace
 *
 * This function returns void.
 *
 * This function will not return on NULL jfmt.
 */
void
jfmt_print_brace(struct jfmt *jfmt, bool open)
{
    /* firewall */
    if (jfmt == NULL) {
	err(52, __func__, "jfmt is NULL");
	not_reached();
    }

    if (jfmt->print_braces && !jfmt->count_only) {
	print("%c\n", open?'{':'}');
    }
}


/* jfmt_print_match	    - print a single match
 *
 * given:
 *
 *	jfmt	    - our struct jfmt with patterns list
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
jfmt_print_match(struct jfmt *jfmt, struct jfmt_pattern *pattern, struct jfmt_match *match)
{
    uintmax_t i = 0;			    /* to iterate through count of each match */
    uintmax_t j = 0;			    /* temporary iterator */

    /* firewall */
    if (jfmt == NULL) {
	err(53, __func__, "jfmt is NULL");
	not_reached();
    } else if (match == NULL) {
	err(54, __func__, "match is NULL");
	not_reached();
    } else if (pattern == NULL) {
	err(55, __func__, "pattern is NULL");
	not_reached();
    }

    /* if the name of the match is NULL it is a fatal error */
    if (match->match == NULL) {
	err(56, __func__, "match->match is NULL");
	not_reached();
    } else if (*match->match == '\0') {
	/* warn on empty name for now and then go to next match */
	warn(__func__, "empty match name");
	return;
    }

    if (match->value == NULL) {
	err(57, __func__, "match '%s' has NULL value", match->match);
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
	/* print the match in the way requested
	 *
	 * XXX - the count is probably incorrect as it means the printing could
	 * be out of order
	 *
	 * XXX - add final constraint checks
	 *
	 * XXX - part of the below is buggy in some cases. This must be fixed.
	 *
	 * XXX - more functions will have to be added to print the matches.
	 * Currently the jfmt_match struct is a work in progress. More will
	 * have to be done depending on the type that matched (this includes not
	 * only the jfmt type but the JSON type too).
	 */

	/* first print JSON levels if requested */
	if (jfmt->print_json_levels) {
	    print("%ju", match->level);

	    for (j = 0; j < jfmt->num_level_spaces; ++j) {
		printf("%s", jfmt->print_level_tab?"\t":" ");
	    }
	    if (jfmt->indent_levels) {
		if (jfmt->indent_spaces) {
		    for (j = 0; j < match->level * jfmt->indent_spaces; ++j) {
			print("%s", jfmt->indent_tab?"\t":" ");
		    }
		}
	    }

	}

	/*
	 * if we're printing name and value or the syntax we have extra things
	 * to do
	 */
	if (jfmt_print_name_value(jfmt->print_json_types) || jfmt->print_syntax) {
	    /* if we print syntax there are some extra things we have to do */
	    if (jfmt->print_syntax) {

		/* XXX - this doesn't print arrays and other more complicated types - XXX */
		print("\"%s\"", match->match);

		for (j = 0; j < jfmt->num_token_spaces; ++j) {
		    print("%s", jfmt->print_token_tab?"\t":" ");
		}

		print("%s", ":");

		for (j = 0; j < jfmt->num_token_spaces; ++j) {
		    print("%s", jfmt->print_token_tab?"\t":" ");
		}

		print("%s%s%s%s\n",
			(jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
			match->value_type == JTYPE_STRING?"\"":"",
			match->value,
			(jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
			match->value_type == JTYPE_STRING?"\"":"",
			match->next || (pattern->next&&pattern->next->matches) || i+1<match->count?",":"");
	    } else { /* if we're not printing syntax */
		/* print the name, quoting it if necessary */
		print("%s%s%s",
			(jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
			match->name_type == JTYPE_STRING?"\"":"",
			match->match,
			(jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
			match->name_type == JTYPE_STRING?"\"":"");

		/* print spaces or tabs according to command line */
		for (j = 0; j < jfmt->num_token_spaces; ++j) {
		    print("%s", jfmt->print_token_tab?"\t":" ");
		}

		/*
		 * we're not printing the syntax but if colons are requested we
		 * have to print them
		 */
		if (jfmt->print_colons) {
		    print("%s", ":");
		}

		/* print spaces or tabs according to command line */
		for (j = 0; j < jfmt->num_token_spaces; ++j) {
		    print("%s", jfmt->print_token_tab?"\t":" ");
		}

		/* finally print the value.
		 *
		 * NOTE the check for printing syntax or entire file is not
		 * necessary as such.
		 */
		print("%s%s%s\n",
			(jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
			match->value_type == JTYPE_STRING?"\"":"",
			match->value,
			(jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
			match->value_type == JTYPE_STRING?"\"":"");
	    }
	} else if (jfmt_print_name(jfmt->print_json_types) || jfmt_print_value(jfmt->print_json_types)) {
	    /*
	     * here we will print just the name or value, quoting and doing
	     * other things as necessary.
	     *
	     * NOTE the check for printing syntax or entire file is not
	     * necessary as such.
	     */
	    print("%s%s%s\n",
		  (jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
		  ((match->name_type == JTYPE_STRING&&jfmt_print_name(jfmt->print_json_types))||
		   (match->value_type == JTYPE_STRING&&jfmt_print_value(jfmt->print_json_types)))?"\"":"",
		  jfmt_print_name(jfmt->print_json_types)?match->match:match->value,
		  (jfmt->quote_strings||jfmt->print_syntax||jfmt->print_entire_file)&&
		  ((match->name_type == JTYPE_STRING&&jfmt_print_name(jfmt->print_json_types))||
		   (match->value_type == JTYPE_STRING&&jfmt_print_value(jfmt->print_json_types)))?"\"":"");
	}
    }
}

/* jfmt_print_matches	    - print all matches found
 *
 * given:
 *
 *	jfmt	    - our struct jfmt with patterns list
 *
 * NOTE: this function will not return if jfmt is NULL.
 *
 * NOTE: this function will only warn if patterns and matches are both NULL.
 *
 * NOTE: if any pointer in a match is NULL this function will not return as it
 * indicates incorrect behaviour in the program as it should never have got this
 * far in the first place.
 *
 * NOTE: this function uses jfmt_print_match() to print each match.
 *
 * XXX: the concept of more than one pattern is not correct. This has to be
 * fixed.
 */
void
jfmt_print_matches(struct jfmt *jfmt)
{
    struct jfmt_pattern *pattern = NULL;  /* to iterate through patterns list */
    struct jfmt_match *match = NULL;	    /* to iterate through matches of each pattern in the patterns list */

    /* firewall */
    if (jfmt == NULL) {
	err(58, __func__, "jfmt is NULL");
	not_reached();
    } else if (jfmt->patterns == NULL && jfmt->matches == NULL) {
	warn(__func__, "NULL patterns and matches list");
	return;
    }

    /* if -c used just print total number of matches */
    if (jfmt_print_count(jfmt)) {
	return;
    }

    /* if -c was not used we have more to do */

    /*
     * although printing syntax is not yet fully implemented (though a
     * reasonable amount of it is), we will check for -B and print the braces so
     * that after the syntax printing is implemented nothing has to be done with
     * -B. The function jfmt_print_braces() will not do anything if -B was not
     * used.
     */
    jfmt_print_brace(jfmt, true);

    for (pattern = jfmt->patterns; pattern != NULL; pattern = pattern->next) {
	for (match = pattern->matches; match != NULL; match = match->next) {
	    jfmt_print_match(jfmt, pattern, match);
	}
    }

    /*
     * although printing syntax is not yet fully implemented, we will check for
     * -B and print the braces so that after the syntax printing is implemented
     * nothing has to be done with -B. The function jfmt_print_braces() will
     * not do anything if -B was not used.
     */
    if (jfmt->print_braces && !jfmt->count_only) {
	jfmt_print_brace(jfmt, false);
    }
    /*
     * as well, even though -j is not yet fully implemented, we will check for
     * -C and print the final comma if requested so that once -j fully
     * implemented we shouldn't have to do anything else with this option. Don't
     * print final comma if -c used.
     */
    jfmt_print_final_comma(jfmt);

    /* if we need a newline print it */
    if ((jfmt->print_braces || jfmt->print_final_comma) && !jfmt->count_only) {
	puts("");
    }
}

/* run_jfmt_check_tool    - run the JSON check tool from -S path
 *
 * given:
 *
 *	jfmt	    - pointer to our struct jfmt (has everything needed)
 *	argv	    - main()'s argv
 *
 * This function does not return on NULL jfmt. It does check for NULL
 * jfmt->check_tool_path and jfmt->check_tool_args as it's not required to
 * be set: the jfmt_sanity_chks() function only verifies that IF it is set it
 * exists and is executable and if the args are specified that the -S is also
 * specified (and the path is an executable file).
 *
 * This function does not return on NULL jfmt->file_contents.
 *
 * It does NOT check for the path existing as an executable file as the
 * jfmt_sanity_chks() does that and if it somehow failed it's an error anyway.
 */
void
run_jfmt_check_tool(struct jfmt *jfmt, char **argv)
{
    int exit_code = 0;			/* exit code for -S path execution */

    /* firewall */
    if (jfmt == NULL) {
	err(59, __func__, "NULL jfmt");
	not_reached();
    } else if (jfmt->file_contents == NULL) {
	err(60, __func__, "NULL jfmt->file_contents");
	not_reached();
    }

    /* run tool if -S used */
    if (jfmt->check_tool_path != NULL) {
	/* try running via shell_cmd() first */
	if (jfmt->check_tool_args != NULL) {
	    dbg(DBG_MED, "about to execute: %s %s -- %s >/dev/null 2>&1",
		    jfmt->check_tool_path, jfmt->check_tool_args, argv[0]);
	    exit_code = shell_cmd(__func__, true, true, "% % -- %", jfmt->check_tool_path, jfmt->check_tool_args, argv[0]);
	} else {
	    dbg(DBG_MED, "about to execute: %s %s >/dev/null 2>&1", jfmt->check_tool_path, argv[0]);
	    exit_code = shell_cmd(__func__, true, true, "% %", jfmt->check_tool_path, argv[0]);
	}
	if(exit_code != 0) {
	    if (jfmt->check_tool_args != NULL) {
		err(7, __func__, "JSON check tool '%s' with args '%s' failed with exit code: %d",/*ooo*/
			jfmt->check_tool_path, jfmt->check_tool_args, exit_code);
	    } else {
		err(7, __func__, "JSON check tool '%s' without args failed with exit code: %d",/*ooo*/
			jfmt->check_tool_path, exit_code);
	    }
	    not_reached();
	}
	/* now open a write-only pipe */
	if (jfmt->check_tool_args != NULL) {
	    jfmt->check_tool_stream = pipe_open(__func__, true, true, "% % %", jfmt->check_tool_path,
		    jfmt->check_tool_args, argv[0]);
	} else {
	    jfmt->check_tool_stream = pipe_open(__func__, true, true, "% %", jfmt->check_tool_path, argv[0]);
	}
	if (jfmt->check_tool_stream == NULL) {
	    if (jfmt->check_tool_args != NULL) {
		err(7, __func__, "opening pipe to JSON check tool '%s' with args '%s' failed", /*ooo*/
			jfmt->check_tool_path, jfmt->check_tool_args);
	    } else {
		err(7, __func__, "opening pipe to JSON check tool '%s' without args failed", jfmt->check_tool_path); /*ooo*/
	    }
	    not_reached();
	} else {
	    /* process the pipe */
	    int exit_status = 0;

	    /*
	     * write the file contents, which we know to be a valid JSON
	     * document that is NUL terminated, to the pipe.
	     */
	    fpr(jfmt->check_tool_stream, __func__, "%s", jfmt->file_contents);

	    /*
	     * close down the pipe to the child process and obtain the status of the pipe child process
	     */
	    exit_status = pclose(jfmt->check_tool_stream);

	    /*
	     * examine the exit status of the child process and error if the child had a non-zero exit
	     */
	    if (WEXITSTATUS(exit_status) != 0) {
		free_jfmt(&jfmt);
		err(7, __func__, "pipe child process exited non-zero"); /*ooo*/
		not_reached();
	    } else {
		dbg(DBG_MED, "pipe child process exited OK");
	    }
	}
	jfmt->check_tool_stream = NULL;
    }
}


