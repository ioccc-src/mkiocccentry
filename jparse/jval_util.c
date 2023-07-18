/*
 * jval_util - utility functions for JSON printer jval
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

#include "jval_util.h"

/*
 * alloc_jval	    - allocate a struct jval *, clear it out and set defaults
 *
 * This function returns a newly allocated and cleared struct jval *.
 *
 * This function will never return a NULL pointer.
 */
struct jval *
alloc_jval(void)
{
    /* allocate our struct jval */
    struct jval *jval = calloc(1, sizeof *jval);

    /* verify jval != NULL */
    if (jval == NULL) {
	err(22, __func__, "failed to allocate jval struct");
	not_reached();
    }

    /* explicitly clear everything out and set defaults */

    /* JSON file member variables */
    jval->is_stdin = false;			/* true if it's stdin */
    jval->file_contents = NULL;			/* file.json contents */
    jval->json_file = NULL;			/* JSON file * */

    jval->outfile_path = NULL;			/* assume no -o used */
    jval->outfile = stdout;			/* default stdout */
    jval->outfile_not_stdout = false;		/* by default we write to stdout */

    /* string related options */
    jval->encode_strings = false;		/* -e used */
    jval->quote_strings = false;		/* -Q used */


    /* number range options, see struct jval_number_range in jval_util.h for details */

    /* -l - levels number range */
    jval->jval_levels.number = 0;
    jval->jval_levels.exact = false;
    jval->jval_levels.range.min = 0;
    jval->jval_levels.range.max = 0;
    jval->jval_levels.range.less_than_equal = false;
    jval->jval_levels.range.greater_than_equal = false;
    jval->jval_levels.range.inclusive = false;
    jval->levels_constrained = false;

    /* print related options */
    jval->print_decoded = false;			/* -D not used if false */
    jval->print_json_levels = false;			/* -L specified */
    jval->num_level_spaces = 0;				/* number of spaces or tab for -L */
    jval->print_level_tab = false;			/* -L tab option */
    jval->invert_matches = false;			/* -i used */
    jval->count_only = false;				/* -c used, only show count */
    jval->count_and_show_values = false;		/* -C used, count and show values */

    /* search / matching related */
    /* json types to look for */
    jval->json_types_specified = false;			/* -t used */
    jval->json_types = JVAL_TYPE_SIMPLE;		/* -t type specified, default simple */

    jval->ignore_case = false;				/* true if -f, case-insensitive */
    jval->match_decoded = false;			/* if -d used match decoded */
    jval->arg_specified = false;			/* true if an arg was specified */
    jval->match_substrings = false;			/* -s used, matching substrings okay */
    jval->use_regexps = false;				/* -g used, allow grep-like regexps */

    /* for -S */
    jval->string_cmp_used = false;
    jval->string_cmp.string = NULL;
    jval->string_cmp.number = NULL;
    jval->string_cmp.op = 0;

    /* for -n */
    jval->num_cmp_used = false;
    jval->num_cmp.string = NULL;
    jval->num_cmp.number = 0;
    jval->num_cmp.op = 0;

    /* parsing related */
    jval->max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */
    jval->json_tree = NULL;


    /* matches for -c / -C - subject to change */
    jval->total_matches = 0;

    return jval;
}


/*
 * jval_match_none	- if no types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types == 0.
 */
bool
jval_match_none(uintmax_t types)
{
    return types == JVAL_TYPE_NONE;
}

/*
 * jval_match_int	- if ints should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JVAL_TYPE_INT set.
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
jval_match_int(uintmax_t types)
{
    return (types & JVAL_TYPE_INT) != 0;
}
/*
 * jval_match_float	- if floats should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JVAL_TYPE_FLOAT set.
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
jval_match_float(uintmax_t types)
{
    return (types & JVAL_TYPE_FLOAT) != 0;
}
/*
 * jval_match_exp	- if exponents should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JVAL_TYPE_EXP set.
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
jval_match_exp(uintmax_t types)
{
    return (types & JVAL_TYPE_EXP) != 0;
}
/*
 * jval_match_num	- if numbers of any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JVAL_TYPE_NUM (or any of the number types) set.
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
jval_match_num(uintmax_t types)
{
    return ((types & JVAL_TYPE_NUM)||(types & JVAL_TYPE_INT) || (types & JVAL_TYPE_FLOAT) ||
	    (types & JVAL_TYPE_EXP))!= 0;
}
/*
 * jval_match_bool	- if booleans should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JVAL_TYPE_BOOL set.
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
jval_match_bool(uintmax_t types)
{
    return (types & JVAL_TYPE_BOOL) != 0;
}
/*
 * jval_match_string	    - if strings should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JVAL_TYPE_STR set.
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
jval_match_string(uintmax_t types)
{
    return (types & JVAL_TYPE_STR) != 0;
}
/*
 * jval_match_null	- if null should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types has JVAL_TYPE_NULL set.
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
jval_match_null(uintmax_t types)
{
    return (types & JVAL_TYPE_NULL) != 0;
}
/*
 * jval_match_any	- if any type should match
 *
 * given:
 *
 *	types	- types set
 *
 * Returns true if types is equal to JVAL_TYPE_ANY.
 *
 * Why does it have to equal JVAL_TYPE_ANY if it checks for any type? Because
 * the point is that if JVAL_TYPE_ANY is set it can be any type but not
 * specific types. For the specific types those bits have to be set instead. If
 * JVAL_TYPE_ANY is set then any type can be set but if types is say
 * JVAL_TYPE_INT then checking for JVAL_TYPE_INT & JVAL_TYPE_ANY would be
 * != 0 (as it's a bitwise OR of all the types) which would suggest that any
 * type is okay even if JVAL_TYPE_INT was the only type.
 */
bool
jval_match_any(uintmax_t types)
{
    return types == JVAL_TYPE_ANY;
}
/*
 * jval_match_simple	- if simple types should match
 *
 * given:
 *
 *	types	- types set
 *
 * Simple is defined as a number, a bool, a string or a null.
 *
 * Returns true if types has JVAL_TYPE_SIMPLE set.
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
jval_match_simple(uintmax_t types)
{
    return (types & JVAL_TYPE_SIMPLE) != 0;
}

/*
 * jval_parse_types_option	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JVAL_TYPE_SIMPLE (as if '-t simple').
 */
uintmax_t
jval_parse_types_option(char *optarg)
{
    char *p = NULL;	    /* for strtok_r() */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;	    /* strdup()d copy of optarg */

    uintmax_t type = JVAL_TYPE_SIMPLE; /* default is simple: num, bool, str and null */

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
	    type |= JVAL_TYPE_INT;
	} else if (!strcmp(p, "float")) {
	    type |= JVAL_TYPE_FLOAT;
	} else if (!strcmp(p, "exp")) {
	    type |= JVAL_TYPE_EXP;
	} else if (!strcmp(p, "num")) {
	    type |= JVAL_TYPE_NUM;
	} else if (!strcmp(p, "bool")) {
	    type |= JVAL_TYPE_BOOL;
	} else if (!strcmp(p, "str")) {
	    type |= JVAL_TYPE_STR;
	} else if (!strcmp(p, "null")) {
	    type |= JVAL_TYPE_NULL;
	} else if (!strcmp(p, "simple")) {
	    type |= JVAL_TYPE_SIMPLE;
	} else if (!strcmp(p, "any")) {
	    type |= JVAL_TYPE_ANY;
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

/* jval_parse_number_range	- parse a number range for options -l, -N, -n
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
 * See also the structs jval_number_range and jval_number in jval_util.h
 * for more details.
 *
 * NOTE: this function does not return on syntax error or NULL number.
 */
bool
jval_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jval_number *number)
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
	memset(number, 0, sizeof(struct jval_number));

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

/* jval_number_in_range   - check if number is in required range
 *
 * given:
 *
 *	number		- number to check
 *	total_matches	- total number of matches found
 *	range		- pointer to struct jval_number with range
 *
 * Returns true if the number is in range.
 *
 * NOTE: if range is NULL it will return false.
 */
bool
jval_number_in_range(intmax_t number, intmax_t total_matches, struct jval_number *range)
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


/* jval_parse_st_level_option    - parse -L [num]{s,t}/-b level option
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
jval_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab)
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
 * jval_parse_value_option	- parse -t types list
 *
 * given:
 *
 *	optarg	    - option argument to -t option
 *
 * Returns: bitvector of types requested.
 *
 * NOTE: if optarg is NULL (which should never happen) or empty it returns the
 * default, JVAL_TYPE_SIMPLE (as if '-t simple').
 */
uintmax_t
jval_parse_value_type_option(char *optarg)
{
    uintmax_t type = JVAL_TYPE_SIMPLE; /* default is simple: num, bool, str and null */
    char *p = NULL;

    if (optarg == NULL || !*optarg) {
	/* NULL or empty optarg, assume simple */
	return type;
    }
    p = optarg;

    /* Determine if the arg is a valid type.  */
    if (!strcmp(p, "int")) {
	type = JVAL_TYPE_INT;
    } else if (!strcmp(p, "float")) {
	type = JVAL_TYPE_FLOAT;
    } else if (!strcmp(p, "exp")) {
	type = JVAL_TYPE_EXP;
    } else if (!strcmp(p, "num")) {
	type = JVAL_TYPE_NUM;
    } else if (!strcmp(p, "bool")) {
	type = JVAL_TYPE_BOOL;
    } else if (!strcmp(p, "str")) {
	type = JVAL_TYPE_STR;
    } else if (!strcmp(p, "null")) {
	type = JVAL_TYPE_NULL;
    } else if (!strcmp(p, "simple")) {
	type = JVAL_TYPE_SIMPLE;
    } else {
	/* unknown or unsupported type */
	err(3, __func__, "unknown or unsupported type '%s'", p); /*ooo*/
	not_reached();
    }

    return type;
}

/*
 * free_jval	    - free jval struct
 *
 * given:
 *
 *	jval	    - a struct jval **
 *
 * This function will do nothing other than warn on NULL pointer (even though
 * it's safe to free a NULL pointer though if jval itself was NULL it would be
 * an error to dereference it).
 *
 * We pass a struct jval ** so that in the caller jval can be set to NULL to
 * remove the need to repeatedly set it to NULL each time this function is
 * called. This way we remove the need to do more than just call this function.
 */
void
free_jval(struct jval **jval)
{
    /* firewall */
    if (jval == NULL || *jval == NULL) {
	warn(__func__, "passed NULL struct jval ** or *jval is NULL");
	return;
    }

    /* flush output file if open and then close it */
    if ((*jval)->outfile != NULL && (*jval)->outfile != stdout) {
	fflush((*jval)->outfile);
	fclose((*jval)->outfile);
	(*jval)->outfile = NULL;
	(*jval)->outfile_path = NULL;
    }

    free(*jval);
    *jval = NULL;
}

/* jval_print_count	- print total matches if -c used
 *
 * given:
 *
 *	jval	    - pointer to our struct jval
 *
 * This function will not return on NULL jval.
 *
 * This function returns false if -c was not used and true if it was.
 */
bool
jval_print_count(struct jval *jval)
{
    /* firewall */
    if (jval == NULL) {
	err(24, __func__, "jval is NULL");
	not_reached();
    }

    if (jval->count_only || jval->count_and_show_values) {
	fpr(jval->outfile?jval->outfile:stdout, "jval", "%ju\n", jval->total_matches);
	return true;
    }

    return false;
}

/* parse_jval_name_args - add name_args to patterns list
 *
 * given:
 *
 *	jval	    - pointer to our struct jval
 *	argv	    - argv from main()
 *
 * This function will not return on NULL pointers.
 *
 */
void
parse_jval_args(struct jval *jval, char **argv)
{
    int i;  /* to iterate through argv */

    /* firewall */
    if (jval == NULL) {
	err(25, __func__, "jval is NULL");
	not_reached();
    }
    if (argv == NULL) {
	err(26, __func__, "argv is NULL");
	not_reached();
    }

    for (i = 1; argv[i] != NULL; ++i) {
	/* XXX - go through argv and add args to a list - XXX */
    }
}

/*
 * jval_parse_cmp_op	- parse -S / -n compare options
 *
 * given:
 *
 *	jval	    - pointer to our jval struct
 *	option	    - the option letter (without the '-') that triggered this
 *		      function
 *	optarg	    - option arg to the option
 *	cmp	    - pointer to our struct jval_cmp_op depending on the option used
 *
 *
 *  This function fills out either the jval->string_cmp or jval->num_cmp if the
 *  syntax is correct.
 *
 *  This function will not return on error in conversion or syntax error or NULL
 *  pointers.
 *
 *  This function returns void.
 */
void
jval_parse_cmp_op(struct jval *jval, const char *option, char *optarg, struct jval_cmp_op *cmp)
{
    char *p = NULL;		    /* to find the = separator */
    char *mode = NULL;		    /* if -S then "str" else "num" */
    struct json *item = NULL;	    /* to get the converted value */

    /* firewall */
    if (jval == NULL) {
	err(27, __func__, "NULL jval");
	not_reached();
    }
    if (option == NULL) {
	err(28, __func__, "NULL option");
	not_reached();
    }
    if (optarg == NULL) {
	err(29, __func__, "NULL optarg");
	not_reached();
    }
    if (cmp == NULL) {
	err(30, __func__, "NULL cmp pointer");
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
	cmp->op = JVAL_CMP_EQ;
    } else if (!strncmp(optarg, "lt=", 3)) {
	cmp->op = JVAL_CMP_LT;
    } else if (!strncmp(optarg, "le=", 3)) {
	cmp->op = JVAL_CMP_LE;
    } else if (!strncmp(optarg, "gt=", 3)){
	cmp->op = JVAL_CMP_GT;
    } else if (!strncmp(optarg, "ge=", 3)) {
	cmp->op = JVAL_CMP_GE;
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
	    cmp->string = &(item->item.string);
	    if (cmp->string == NULL) {
		err(37, __func__, "failed to convert string: <%s> for -%s: cmp->string is NULL", optarg + 3, option);
		not_reached();
	    } else if (!cmp->string->converted && !cmp->string->parsed) {
		err(38, __func__, "failed to convert or parse string: <%s> for option -%s but string pointer not NULL!",
			optarg + 3, option);
		not_reached();
	    }
	    json_dbg(JSON_DBG_NONE, __func__, "string to compare: <%s>", cmp->string->str);
	}
    } else if (!strcmp(option, "n")) { /* -n */
	item = json_conv_number(optarg + 3, strlen(optarg + 3));
	if (item == NULL) {
	    err(37, __func__, "syntax error in -%s: no number found: <%s>", option, optarg + 3);
	    not_reached();
	} else {
	    cmp->number = &(item->item.number);
	    if (!cmp->number->converted && !cmp->number->parsed) {
		err(38, __func__, "failed to convert or parse number: <%s> for option -%s but number pointer not NULL!",
			optarg + 3, option);
		not_reached();
	    }
	    /* TODO - add debug call if converted / parsed ? - TODO */
	}
    }
}
