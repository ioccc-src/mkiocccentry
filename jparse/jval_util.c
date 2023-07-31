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
    jval->common.is_stdin = false;			/* true if it's stdin */
    jval->common.file_contents = NULL;			/* file.json contents */
    jval->common.json_file = NULL;			/* JSON file * */
    jval->common.json_file_path = NULL;			/* JSON file path */

    jval->common.outfile_path = NULL;			/* assume no -o used */
    jval->common.outfile = stdout;			/* default stdout */
    jval->common.outfile_not_stdout = false;		/* by default we write to stdout */

    /* string related options */
    jval->json_name_val.encode_strings = false;		/* -e used */
    jval->json_name_val.quote_strings = false;		/* -Q used */


    /* number range options, see struct json_util_number_range in json_util.h for details */

    /* -l - levels number range */
    jval->common.json_util_levels.number = 0;
    jval->common.json_util_levels.exact = false;
    jval->common.json_util_levels.range.min = 0;
    jval->common.json_util_levels.range.max = 0;
    jval->common.json_util_levels.range.less_than_equal = false;
    jval->common.json_util_levels.range.greater_than_equal = false;
    jval->common.json_util_levels.range.inclusive = false;
    jval->common.levels_constrained = false;

    /* for -F format output option */
    jval->common.format_output_changed = false;			/* -F format used */
    jval->common.format = JSON_FMT_DEFAULT;		/* default format for -F */


    /* print related options */
    jval->json_name_val.print_decoded = false;			/* -D not used if false */
    jval->common.print_json_levels = false;			/* -L specified */
    jval->common.num_level_spaces = 0;				/* number of spaces or tab for -L */
    jval->common.print_level_tab = false;			/* -L tab option */
    jval->json_name_val.invert_matches = false;			/* -i used */
    jval->json_name_val.count_only = false;				/* -c used, only show count */
    jval->json_name_val.count_and_show_values = false;		/* -C used, count and show values */

    /* search / matching related */
    /* json types to look for */
    jval->json_name_val.json_types_specified = false;			/* -t used */
    jval->json_name_val.json_types = JVAL_TYPE_SIMPLE;		/* -t type specified, default simple */

    jval->json_name_val.ignore_case = false;				/* true if -f, case-insensitive */
    jval->json_name_val.match_decoded = false;			/* if -d used match decoded */
    jval->json_name_val.arg_specified = false;			/* true if an arg was specified */
    jval->json_name_val.match_substrings = false;			/* -s used, matching substrings okay */
    jval->json_name_val.use_regexps = false;				/* -g used, allow grep-like regexps */

    /* for -S */
    jval->json_name_val.string_cmp_used = false;
    jval->json_name_val.string_cmp = NULL;

    /* for -n */
    jval->json_name_val.num_cmp_used = false;
    jval->json_name_val.num_cmp = NULL;

    /* parsing related */
    jval->common.max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */
    jval->common.json_tree = NULL;


    /* matches for -c / -C - subject to change */
    jval->json_name_val.total_matches = 0;

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
    if ((*jval)->common.outfile != NULL && (*jval)->common.outfile != stdout) {
	fflush((*jval)->common.outfile);
	fclose((*jval)->common.outfile);
	(*jval)->common.outfile = NULL;
	(*jval)->common.outfile_path = NULL;
    }

    /* free the compare lists too */
    free_jval_cmp_op_lists(*jval);

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

    if (jval->json_name_val.count_only || jval->json_name_val.count_and_show_values) {
	/* XXX - the next line will be removed when -c and -C are done - XXX */
	fpr(jval->common.outfile?jval->common.outfile:stdout, "jval", "XXX - the count is currently incorrect - XXX\n");
	fpr(jval->common.outfile?jval->common.outfile:stdout, "jval", "%ju\n", jval->json_name_val.total_matches);
	return true;
    }

    return false;
}

/* parse_jval_name_args - add name_args to patterns list
 *
 * given:
 *
 *	jval	    - pointer to our struct jval
 *	argc	    - pointer to argc from main() (int *)
 *	argv	    - pointer to argv from main() (char ***)
 *
 * This function will not return on NULL pointers.
 *
 */
void
parse_jval_args(struct jval *jval, int *argc, char ***argv)
{
    int i;  /* to iterate through argv */

    /* firewall */
    if (jval == NULL) {
	err(25, __func__, "jval is NULL");
	not_reached();
    }
    if (argc == NULL) {
	err(26, __func__, "argc is NULL");
	not_reached();
    }
    if (argv == NULL) {
	err(27, __func__, "argv is NULL");
	not_reached();
    }

    /* skip past file name */
    (*argc)++;
    (*argv)++;

    for (i = 0; (*argv)[i] != NULL; ++i) {
	json_dbg(JSON_DBG_NONE, __func__, "arg[%d]: %s", i, (*argv)[i]);
	/* XXX - do we increment argc and argv? On the one hand this might seem
	 * logical but on the other if we keep argc and argv as it is (past the
	 * file name) then main() can check if any args are present. Now it
	 * might be that later on we will have a list or array of some kind but
	 * currently this is not the case so we don't update argc or argv past
	 * the file name.
	 */
    }
}

/* free_jval_cmp_op_lists - free the compare lists
 *
 * given:
 *
 *  jval	- pointer to our struct jval
 *
 * This function frees out both the number and string compare lists.
 *
 * This function will not return on NULL jval.
 */
void
free_jval_cmp_op_lists(struct jval *jval)
{
    struct json_util_cmp_op *op, *next_op;

    /* firewall */
    if (jval == NULL) {
	err(28, __func__, "jval is NULL");
	not_reached();
    }

    /* first the string compare list */
    for (op = jval->json_name_val.string_cmp; op != NULL; op = next_op) {
	next_op = op->next;

	/* XXX - free json node - XXX */

	free(op);
	op = NULL;
    }

    /* now the number compare list */
    for (op = jval->json_name_val.num_cmp; op != NULL; op = next_op) {
	next_op = op->next;

	/* XXX - free json node - XXX */

	free(op);
	op = NULL;
    }
}
