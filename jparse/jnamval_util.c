/*
 * jnamval_util - utility functions for JSON printer jnamval
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

#include "jnamval_util.h"

/*
 * alloc_jnamval	    - allocate a struct jnamval *, clear it out and set defaults
 *
 * This function returns a newly allocated and cleared struct jnamval *.
 *
 * This function will never return a NULL pointer.
 */
struct jnamval *
alloc_jnamval(void)
{
    /* allocate our struct jnamval */
    struct jnamval *jnamval = calloc(1, sizeof *jnamval);

    /* verify jnamval != NULL */
    if (jnamval == NULL) {
	err(22, __func__, "failed to allocate jnamval struct");
	not_reached();
    }

    /* explicitly clear everything out and set defaults */

    /* JSON file member variables */
    jnamval->is_stdin = false;			/* true if it's stdin */
    jnamval->file_contents = NULL;			/* file.json contents */
    jnamval->json_file = NULL;			/* JSON file * */

    jnamval->outfile_path = NULL;			/* assume no -o used */
    jnamval->outfile = stdout;			/* default stdout */
    jnamval->outfile_not_stdout = false;		/* by default we write to stdout */

    /* string related options */
    jnamval->encode_strings = false;		/* -e used */
    jnamval->quote_strings = false;		/* -Q used */


    /* number range options, see struct jnamval_number_range in jnamval_util.h for details */

    /* -l - levels number range */
    jnamval->jnamval_levels.number = 0;
    jnamval->jnamval_levels.exact = false;
    jnamval->jnamval_levels.range.min = 0;
    jnamval->jnamval_levels.range.max = 0;
    jnamval->jnamval_levels.range.less_than_equal = false;
    jnamval->jnamval_levels.range.greater_than_equal = false;
    jnamval->jnamval_levels.range.inclusive = false;
    jnamval->levels_constrained = false;

    /* print related options */
    jnamval->print_json_types_option = false;		/* -p explicitly used */
    jnamval->print_json_types = JNAMVAL_PRINT_VALUE;	/* -p type specified */
    jnamval->print_decoded = false;			/* -D not used if false */
    jnamval->print_json_levels = false;			/* -L specified */
    jnamval->num_level_spaces = 0;				/* number of spaces or tab for -L */
    jnamval->print_level_tab = false;			/* -L tab option */
    jnamval->invert_matches = false;			/* -i used */
    jnamval->count_only = false;				/* -c used, only show count */
    jnamval->count_and_show_values = false;		/* -C used, count and show values */

    /* search / matching related */
    /* json types to look for */
    jnamval->json_types_specified = false;			/* -t used */
    jnamval->json_types = JNAMVAL_TYPE_SIMPLE;		/* -t type specified, default simple */
    jnamval->ignore_case = false;				/* true if -f, case-insensitive */
    jnamval->match_decoded = false;			/* if -d used match decoded */
    jnamval->arg_specified = false;			/* true if an arg was specified */
    jnamval->match_substrings = false;			/* -s used, matching substrings okay */
    jnamval->use_regexps = false;				/* -g used, allow grep-like regexps */
    jnamval->match_json_member_names = false;		/* -N used, match based on member names */
    jnamval->match_hierarchies = false;			/* -H used, match any JSON member name */

    /* for -S */
    jnamval->string_cmp_used = false;
    jnamval->string_cmp = NULL;

    /* for -n */
    jnamval->num_cmp_used = false;
    jnamval->num_cmp = NULL;

    /* parsing related */
    jnamval->max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */
    jnamval->json_tree = NULL;


    /* matches for -c / -C - subject to change */
    jnamval->total_matches = 0;

    return jnamval;
}


/*
 * jnamval_match_none	- if no types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types == 0.
 */
bool
jnamval_match_none(uintmax_t types)
{
    return types == JNAMVAL_TYPE_NONE;
}

/*
 * jnamval_match_int	- if ints should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_INT set.
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
jnamval_match_int(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_INT) != 0;
}
/*
 * jnamval_match_float	- if floats should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_FLOAT set.
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
jnamval_match_float(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_FLOAT) != 0;
}
/*
 * jnamval_match_exp	- if exponents should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_EXP set.
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
jnamval_match_exp(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_EXP) != 0;
}
/*
 * jnamval_match_num	- if numbers of any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_NUM (or any of the number types) set.
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
jnamval_match_num(uintmax_t types)
{
    return ((types & JNAMVAL_TYPE_NUM)||(types & JNAMVAL_TYPE_INT) || (types & JNAMVAL_TYPE_FLOAT) ||
	    (types & JNAMVAL_TYPE_EXP))!= 0;
}
/*
 * jnamval_match_bool	- if booleans should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_BOOL set.
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
jnamval_match_bool(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_BOOL) != 0;
}
/*
 * jnamval_match_string	    - if strings should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_STR set.
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
jnamval_match_string(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_STR) != 0;
}
/*
 * jnamval_match_null	- if null should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_NULL set.
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
jnamval_match_null(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_NULL) != 0;
}

/*
 * jnamval_match_object	    - if objects should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_OBJECT set.
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
jnamval_match_object(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_OBJECT) != 0;
}

/*
 * jnamval_match_array	    - if arrays should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JNAMVAL_TYPE_ARRAY set.
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
jnamval_match_array(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_ARRAY) != 0;
}


/*
 * jnamval_match_simple	- if simple types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Simple is defined as a number, a bool, a string or a null.
 *
 * Returns true if types has JNAMVAL_TYPE_SIMPLE set.
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
jnamval_match_simple(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_SIMPLE) != 0;
}
/*
 * jnamval_match_member   - if members should match
 *
 * given:
 *
 *	types	- types set
 *
 * A member is defined as an object or array.
 *
 * Returns true if types has JNAMVAL_TYPE_MEMBER set.
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
jnamval_match_member(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_MEMBER) != 0;
}


/*
 * jnamval_match_compound   - if compounds should match
 *
 * given:
 *
 *	types	- types set
 *
 * A compound is defined as an object or array.
 *
 * Returns true if types has JNAMVAL_TYPE_COMPOUND set.
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
jnamval_match_compound(uintmax_t types)
{
    return (types & JNAMVAL_TYPE_COMPOUND) != 0;
}


/*
 * jnamval_match_any	- if any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types is equal to JNAMVAL_TYPE_ANY.
 *
 * Why does it have to equal JNAMVAL_TYPE_ANY if it checks for any type? Because
 * the point is that if JNAMVAL_TYPE_ANY is set it can be any type but not
 * specific types. For the specific types those bits have to be set instead. If
 * JNAMVAL_TYPE_ANY is set then any type can be set but if types is say
 * JNAMVAL_TYPE_INT then checking for JNAMVAL_TYPE_INT & JNAMVAL_TYPE_ANY would be
 * != 0 (as it's a bitwise OR of all the types) which would suggest that any
 * type is okay even if JNAMVAL_TYPE_INT was the only type.
 */
bool
jnamval_match_any(uintmax_t types)
{
    return types == JNAMVAL_TYPE_ANY;
}

/*
 * jnamval_parse_types_option	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JNAMVAL_TYPE_SIMPLE (as if '-t simple').
 */
uintmax_t
jnamval_parse_types_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;	    /* strdup()d copy of optarg */

    uintmax_t type = JNAMVAL_TYPE_SIMPLE; /* default is simple: num, bool, str and null */

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
	    type |= JNAMVAL_TYPE_INT;
	} else if (!strcmp(p, "float")) {
	    type |= JNAMVAL_TYPE_FLOAT;
	} else if (!strcmp(p, "exp")) {
	    type |= JNAMVAL_TYPE_EXP;
	} else if (!strcmp(p, "num")) {
	    type |= JNAMVAL_TYPE_NUM;
	} else if (!strcmp(p, "bool")) {
	    type |= JNAMVAL_TYPE_BOOL;
	} else if (!strcmp(p, "str")) {
	    type |= JNAMVAL_TYPE_STR;
	} else if (!strcmp(p, "null")) {
	    type |= JNAMVAL_TYPE_NULL;
	} else if (!strcmp(p, "object")) {
	    type |= JNAMVAL_TYPE_OBJECT;
	} else if (!strcmp(p, "array")) {
	    type |= JNAMVAL_TYPE_ARRAY;
	} else if (!strcmp(p, "simple")) {
	    type |= JNAMVAL_TYPE_SIMPLE;
	} else if (!strcmp(p, "compound")) {
	    type |= JNAMVAL_TYPE_COMPOUND;
	} else if (!strcmp(p, "member")) {
	    type |= JNAMVAL_TYPE_MEMBER;
	} else if (!strcmp(p, "any")) {
	    type |= JNAMVAL_TYPE_ANY;
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
 * jnamval_print_name	- if only names should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JNAMVAL_PRINT_NAME set.
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
jnamval_print_name(uintmax_t types)
{
    return ((types & JNAMVAL_PRINT_NAME) && !(types & JNAMVAL_PRINT_VALUE)) != 0;
}
/*
 * jnamval_print_value	- if only values should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JNAMVAL_PRINT_VALUE set.
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
jnamval_print_value(uintmax_t types)
{
    return ((types & JNAMVAL_PRINT_VALUE) && !(types & JNAMVAL_PRINT_NAME)) != 0;
}
/*
 * jnamval_print_both	- if names AND values should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types has JNAMVAL_PRINT_BOTH set.
 */
bool
jnamval_print_both(uintmax_t types)
{
    return types == JNAMVAL_PRINT_BOTH;
}

/*
 * jnamval_print_json	- if only member with json syntax should be printed
 *
 * given:
 *
 *	types	- print types set
 *
 * Returns true if types only has JNAMVAL_PRINT_JSON set.
 *
 * NOTE: why do we return that the bitwise AND is not != 0 rather than just the
 * bitwise AND? Because in some cases (like the test routines) we compare the
 * expected true json to the result of the function. But depending on the bits
 * set it might not end up being 1 so it ends up not comparing true to true but
 * another json to true which it might not be. This could be done a different
 * way where the test would be something like:
 *
 *	if ((test && !expected) || (expected && !test))
 *
 * but this seems like needless complications.
 */
bool
jnamval_print_json(uintmax_t types)
{
    return ((types & JNAMVAL_PRINT_JSON) && !(types & JNAMVAL_PRINT_NAME)) != 0;
}



/*
 * jnamval_parse_print_option	- parse -p option list
 *
 * given:
 *
 *	optarg	    - option argument to -p option
 *
 * Returns: bitvector of types to print.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JNAMVAL_PRINT_VALUE (as if '-p v').
 */
uintmax_t
jnamval_parse_print_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;	    /* strdup()d copy of optarg */

    uintmax_t print_json_types = 0; /* default is to print values */

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return JNAMVAL_PRINT_VALUE;
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
	    print_json_types |= JNAMVAL_PRINT_VALUE;
	} else if (!strcmp(p, "n") || !strcmp(p, "name")) {
	    print_json_types |= JNAMVAL_PRINT_NAME;
	} else if (!strcmp(p, "b") || !strcmp(p, "both")) {
	    print_json_types = JNAMVAL_PRINT_BOTH;
	    break;
	} else if (!strcmp(p, "j") || !strcmp(p, "json")) {
	    print_json_types |= JNAMVAL_PRINT_JSON;
	} else {
	    /* unknown keyword */
	    err(3, __func__, "unknown keyword '%s'", p); /*ooo*/
	    not_reached();
	}
    }

    if (jnamval_print_both(print_json_types)) {
	dbg(DBG_LOW, "will print both name and value");
    } else if (jnamval_print_name(print_json_types)) {
	dbg(DBG_LOW, "will only print name");
    } else if (jnamval_print_value(print_json_types)) {
	dbg(DBG_LOW, "will only print value");
    } else if (jnamval_print_json(print_json_types)) {
	dbg(DBG_LOW, "will print JSON member with JSON syntax");
    }

    if (dup != NULL) {
	free(dup);
	dup = NULL;
    }

    return print_json_types;
}


/* jnamval_parse_number_range	- parse a number range for options -l, -N, -n
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
 * See also the structs jnamval_number_range and jnamval_number in jnamval_util.h
 * for more details.
 *
 * NOTE: this function does not return on syntax error or NULL number.
 */
bool
jnamval_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jnamval_number *number)
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
	memset(number, 0, sizeof(struct jnamval_number));

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

/* jnamval_number_in_range   - check if number is in required range
 *
 * given:
 *
 *	number		- number to check
 *	total_matches	- total number of matches found
 *	range		- pointer to struct jnamval_number with range
 *
 * Returns true if the number is in range.
 *
 * NOTE: if range is NULL it will return false.
 */
bool
jnamval_number_in_range(intmax_t number, intmax_t total_matches, struct jnamval_number *range)
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


/* jnamval_parse_st_level_option    - parse -L [num]{s,t}/-b level option
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
 */
void
jnamval_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab)
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
 * jnamval_parse_value_option	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JNAMVAL_TYPE_SIMPLE (as if '-t simple').
 */
uintmax_t
jnamval_parse_value_type_option(char *optarg)
{
    uintmax_t type = JNAMVAL_TYPE_SIMPLE; /* default is simple: num, bool, str and null */
    char *p = NULL;

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    }
    p = optarg;

    /* Determine if the arg is a valid type.  */
    if (!strcmp(p, "int")) {
	type = JNAMVAL_TYPE_INT;
    } else if (!strcmp(p, "float")) {
	type = JNAMVAL_TYPE_FLOAT;
    } else if (!strcmp(p, "exp")) {
	type = JNAMVAL_TYPE_EXP;
    } else if (!strcmp(p, "num")) {
	type = JNAMVAL_TYPE_NUM;
    } else if (!strcmp(p, "bool")) {
	type = JNAMVAL_TYPE_BOOL;
    } else if (!strcmp(p, "str")) {
	type = JNAMVAL_TYPE_STR;
    } else if (!strcmp(p, "null")) {
	type = JNAMVAL_TYPE_NULL;
    } else if (!strcmp(p, "simple")) {
	type = JNAMVAL_TYPE_SIMPLE;
    } else {
	/* unknown or unsupported type */
	err(3, __func__, "unknown or unsupported type '%s'", p); /*ooo*/
	not_reached();
    }

    return type;
}

/*
 * free_jnamval	    - free jnamval struct
 *
 * given:
 *
 *	jnamval	    - a struct jnamval **
 *
 * This function will do nothing other than warn on NULL pointer (even though
 * it's safe to free a NULL pointer though if jnamval itself was NULL it would be
 * an error to dereference it).
 *
 * We pass a struct jnamval ** so that in the caller jnamval can be set to NULL to
 * remove the need to repeatedly set it to NULL each time this function is
 * called. This way we remove the need to do more than just call this function.
 */
void
free_jnamval(struct jnamval **jnamval)
{
    /* firewall */
    if (jnamval == NULL || *jnamval == NULL) {
	warn(__func__, "passed NULL struct jnamval ** or *jnamval is NULL");
	return;
    }

    /* flush output file if open and then close it */
    if ((*jnamval)->outfile != NULL && (*jnamval)->outfile != stdout) {
	fflush((*jnamval)->outfile);
	fclose((*jnamval)->outfile);
	(*jnamval)->outfile = NULL;
	(*jnamval)->outfile_path = NULL;
    }

    /* free the compare lists too */
    free_jnamval_cmp_op_lists(*jnamval);

    free(*jnamval);
    *jnamval = NULL;
}

/* jnamval_print_count	- print total matches if -c used
 *
 * given:
 *
 *	jnamval	    - pointer to our struct jnamval
 *
 * This function will not return on NULL jnamval.
 *
 * This function returns false if -c was not used and true if it was.
 */
bool
jnamval_print_count(struct jnamval *jnamval)
{
    /* firewall */
    if (jnamval == NULL) {
	err(25, __func__, "jnamval is NULL");
	not_reached();
    }

    if (jnamval->count_only || jnamval->count_and_show_values) {
	fpr(jnamval->outfile?jnamval->outfile:stdout, "jnamval", "%ju\n", jnamval->total_matches);
	return true;
    }

    return false;
}

/* parse_jnamval_name_args - add name_args to patterns list
 *
 * given:
 *
 *	jnamval	    - pointer to our struct jnamval
 *	argv	    - argv from main()
 *
 * This function will not return on NULL pointers.
 *
 */
void
parse_jnamval_args(struct jnamval *jnamval, char **argv)
{
    int i;  /* to iterate through argv */

    /* firewall */
    if (jnamval == NULL) {
	err(26, __func__, "jnamval is NULL");
	not_reached();
    }
    if (argv == NULL) {
	err(27, __func__, "argv is NULL");
	not_reached();
    }

    for (i = 1; argv[i] != NULL; ++i) {
	/* XXX - go through argv and add args to a list - XXX */
    }
}

/*
 * jnamval_parse_cmp_op	- parse -S / -n compare options
 *
 * given:
 *
 *	jnamval	    - pointer to our jnamval struct
 *	option	    - the option letter (without the '-') that triggered this
 *		      function
 *	optarg	    - option arg to the option
 *
 *
 *  This function fills out either the jnamval->string_cmp or jnamval->num_cmp if the
 *  syntax is correct. Or more correctly it adds to the list as more than one
 *  can be specified.
 *
 *  This function will not return on error in conversion or syntax error or NULL
 *  pointers.
 *
 *  This function returns void.
 */
struct jnamval_cmp_op *
jnamval_parse_cmp_op(struct jnamval *jnamval, const char *option, char *optarg)
{
    char *p = NULL;		    /* to find the = separator */
    char *mode = NULL;		    /* if -S then "str" else "num" */
    struct json *item = NULL;	    /* to get the converted value */
    struct jnamval_cmp_op *cmp = NULL;	/* compare operation struct */
    int op = JNAMVAL_CMP_OP_NONE;	/* assume invalid op */

    /* firewall */
    if (jnamval == NULL) {
	err(28, __func__, "NULL jnamval");
	not_reached();
    }
    if (option == NULL) {
	err(29, __func__, "NULL option");
	not_reached();
    }
    if (optarg == NULL) {
	err(30, __func__, "NULL optarg");
	not_reached();
    }

    if (!strcmp(option, "S")) {
	mode = "str";
    } else if (!strcmp(option, "n")) {
	mode = "num";
    } else {
	err(31, __func__, "invalid option used for function: -%s", option);
	not_reached();
    }

    p = strchr(optarg, '=');
    if (p == NULL) {
	err(32, __func__, "syntax error in -%s: use -%s {eq,lt,le,gt,ge}=%s", option, option, mode);
	not_reached();
    } else if (p == optarg) {
	err(33, __func__, "syntax error in -%s: use -%s {eq,lt,le,gt,ge}=%s", option, option, mode);
	not_reached();
    } else if (p[1] == '\0') {
	err(34, __func__, "nothing found after =: use -%s {eq,lt,le,gt,ge}=%s", option, mode);
	not_reached();
    }

    if (!strncmp(optarg, "eq=", 3)) {
	op = JNAMVAL_CMP_EQ;
    } else if (!strncmp(optarg, "lt=", 3)) {
	op = JNAMVAL_CMP_LT;
    } else if (!strncmp(optarg, "le=", 3)) {
	op = JNAMVAL_CMP_LE;
    } else if (!strncmp(optarg, "gt=", 3)){
	op = JNAMVAL_CMP_GT;
    } else if (!strncmp(optarg, "ge=", 3)) {
	op = JNAMVAL_CMP_GE;
    } else {
	err(35, __func__, "invalid op found for -%s: use -%s {eq,lt,le,gt,ge}=%s", option, option, mode);
	not_reached();
    }

    if (!strcmp(option, "S")) { /* -S */
	errno = 0;
	item = json_conv_string(optarg + 3, strlen(optarg + 3), *(optarg +3) == '"' ? true : false);
	if (item == NULL) {
	    err(36, __func__, "failed to convert string <%s> for -%s", optarg + 3, option);
	    not_reached();
	} else {
	    cmp = calloc(1, sizeof *cmp);
	    if (cmp == NULL) {
		err(37, __func__, "failed to allocate struct jval_cmp_op *");
		not_reached();
	    }
	    cmp->string = &(item->item.string);
	    if (cmp->string == NULL) {
		err(38, __func__, "failed to convert string: <%s> for -%s: cmp->string is NULL", optarg + 3, option);
		not_reached();
	    } else if (!CONVERTED_PARSED_JSON_NODE(cmp->string)) {
		err(39, __func__, "failed to convert or parse string: <%s> for option -%s but string pointer not NULL!",
			optarg + 3, option);
		not_reached();
	    }

	    cmp->op = op;

	    cmp->next = jnamval->string_cmp;
	    jnamval->string_cmp = cmp;

	    /* XXX - add function that prints out what compare operation - XXX */
	    json_dbg(JSON_DBG_NONE, __func__, "string to compare: <%s>", cmp->string->str);

	}
    } else if (!strcmp(option, "n")) { /* -n */
	item = json_conv_number(optarg + 3, strlen(optarg + 3));
	if (item == NULL) {
	    err(40, __func__, "syntax error in -%s: no number found: <%s>", option, optarg + 3);
	    not_reached();
	} else {
	    cmp = calloc(1, sizeof *cmp);
	    if (cmp == NULL) {
		err(41, __func__, "failed to allocate struct jval_cmp_op *");
		not_reached();
	    }
	    cmp->number = &(item->item.number);
	    if (!CONVERTED_PARSED_JSON_NODE(cmp->number)) {
		err(7, __func__, "failed to convert or parse number: <%s> for option -%s but number pointer not NULL!",/*ooo*/
			optarg + 3, option);
		not_reached();
	    } else if (PARSED_JSON_NODE(cmp->number) && !CONVERTED_JSON_NODE(cmp->number)) {
		err(7, __func__, "failed to convert number: <%s> for option -%s", optarg +3 , option); /*ooo*/
		not_reached();
	    }

	    cmp->op = op;

	    cmp->next = jnamval->num_cmp;
	    jnamval->num_cmp = cmp;

	    /* XXX - add function that prints out what compare operation - XXX */
	    json_dbg(JSON_DBG_NONE, __func__, "number to compare: <%s>", cmp->number->as_str);
	}
    }

    return cmp;
}

/* free_jnamval_cmp_op_lists - free the compare lists
 *
 * given:
 *
 *  jnamval	- pointer to our struct jnamval
 *
 * This function frees out both the number and string compare lists.
 *
 * This function will not return on NULL jnamval.
 */
void
free_jnamval_cmp_op_lists(struct jnamval *jnamval)
{
    struct jnamval_cmp_op *op, *next_op;

    /* firewall */
    if (jnamval == NULL) {
	err(42, __func__, "jnamval is NULL");
	not_reached();
    }

    /* first the string compare list */
    for (op = jnamval->string_cmp; op != NULL; op = next_op) {
	next_op = op->next;

	/* XXX - free json node - XXX */

	free(op);
	op = NULL;
    }

    /* now the number compare list */
    for (op = jnamval->num_cmp; op != NULL; op = next_op) {
	next_op = op->next;

	/* XXX - free json node - XXX */

	free(op);
	op = NULL;
    }
}
