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
    jnamval->common.is_stdin = false;			/* true if it's stdin */
    jnamval->common.file_contents = NULL;			/* file.json contents */
    jnamval->common.json_file = NULL;			/* JSON file * */

    jnamval->common.outfile_path = NULL;			/* assume no -o used */
    jnamval->common.outfile = stdout;			/* default stdout */
    jnamval->common.outfile_not_stdout = false;		/* by default we write to stdout */

    /* string related options */
    jnamval->json_name_val.encode_strings = false;		/* -e used */
    jnamval->json_name_val.quote_strings = false;		/* -Q used */


    /* number range options, see struct json_util_number_range in jnamval_util.h for details */

    /* -l - levels number range */
    jnamval->common.json_util_levels.number = 0;
    jnamval->common.json_util_levels.exact = false;
    jnamval->common.json_util_levels.range.min = 0;
    jnamval->common.json_util_levels.range.max = 0;
    jnamval->common.json_util_levels.range.less_than_equal = false;
    jnamval->common.json_util_levels.range.greater_than_equal = false;
    jnamval->common.json_util_levels.range.inclusive = false;
    jnamval->common.levels_constrained = false;

    /* print related options */
    jnamval->print_json_types_option = false;		/* -p explicitly used */
    jnamval->print_json_types = JNAMVAL_PRINT_VALUE;	/* -p type specified */
    jnamval->json_name_val.print_decoded = false;			/* -D not used if false */
    jnamval->common.print_json_levels = false;			/* -L specified */
    jnamval->common.num_level_spaces = 0;				/* number of spaces or tab for -L */
    jnamval->common.print_level_tab = false;			/* -L tab option */
    jnamval->json_name_val.invert_matches = false;			/* -i used */
    jnamval->json_name_val.count_only = false;				/* -c used, only show count */
    jnamval->json_name_val.count_and_show_values = false;		/* -C used, count and show values */

    /* search / matching related */
    /* json types to look for */
    jnamval->json_name_val.json_types_specified = false;			/* -t used */
    jnamval->json_name_val.json_types = JNAMVAL_TYPE_SIMPLE;		/* -t type specified, default simple */
    jnamval->json_name_val.ignore_case = false;				/* true if -f, case-insensitive */
    jnamval->json_name_val.match_decoded = false;			/* if -d used match decoded */
    jnamval->json_name_val.arg_specified = false;			/* true if an arg was specified */
    jnamval->json_name_val.match_substrings = false;			/* -s used, matching substrings okay */
    jnamval->json_name_val.use_regexps = false;				/* -g used, allow grep-like regexps */
    jnamval->match_json_member_names = false;		/* -N used, match based on member names */
    jnamval->match_hierarchies = false;			/* -H used, match any JSON member name */


    /* comparison options -S and -n */

    /* -S op=string */
    jnamval->json_name_val.string_cmp_used = false;
    jnamval->json_name_val.string_cmp = NULL;
    /* -n op=number */
    jnamval->json_name_val.num_cmp_used = false;
    jnamval->json_name_val.num_cmp = NULL;

    /* parsing related */
    jnamval->common.max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */
    jnamval->common.json_tree = NULL;

    /* matches for -c / -C - subject to change */
    jnamval->json_name_val.total_matches = 0;

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
    if ((*jnamval)->common.outfile != NULL && (*jnamval)->common.outfile != stdout) {
	fflush((*jnamval)->common.outfile);
	fclose((*jnamval)->common.outfile);
	(*jnamval)->common.outfile = NULL;
	(*jnamval)->common.outfile_path = NULL;
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

    if (jnamval->json_name_val.count_only || jnamval->json_name_val.count_and_show_values) {
	fpr(jnamval->common.outfile?jnamval->common.outfile:stdout, "jnamval", "%ju\n", jnamval->json_name_val.total_matches);
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
    struct json_util_cmp_op *op, *next_op;

    /* firewall */
    if (jnamval == NULL) {
	err(42, __func__, "jnamval is NULL");
	not_reached();
    }

    /* first the string compare list */
    for (op = jnamval->json_name_val.string_cmp; op != NULL; op = next_op) {
	next_op = op->next;

	/* XXX - free json node - XXX */

	free(op);
	op = NULL;
    }

    /* now the number compare list */
    for (op = jnamval->json_name_val.num_cmp; op != NULL; op = next_op) {
	next_op = op->next;

	/* XXX - free json node - XXX */

	free(op);
	op = NULL;
    }
}
