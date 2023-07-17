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
    jfmt->file_contents = NULL;			/* file.json contents */
    jfmt->json_file = NULL;			/* JSON file * */

    jfmt->outfile_path = NULL;			/* assume no -o used */
    jfmt->outfile = stdout;			/* default stdout */
    jfmt->outfile_not_stdout = false;		/* by default we write to stdout */

    /* number range options, see struct jfmt_number_range in jfmt_util.h for details */

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
    jfmt->print_json_levels = false;			/* -L specified */
    jfmt->num_level_spaces = 0;			/* number of spaces or tab for -L */
    jfmt->print_level_tab = false;			/* -L tab option */
    jfmt->indent_levels = false;			/* -I used */
    jfmt->indent_spaces = 0;				/* -I number of tabs or spaces */
    jfmt->indent_tab = false;				/* -I <num>[{t|s}] specified */

    /* parsing related */
    jfmt->max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */

    return jfmt;
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

/* jfmt_parse_st_indent_option    - parse -I [num]{s,t}
 *
 * This function parses the -I option. It's necessary to have it this way
 * because some options like -j imply it and rather than duplicate code we just
 * have it here once.
 *
 * given:
 *
 *	optarg		    - option argument to -I option (can be faked)
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

/* jfmt_parse_st_level_option    - parse -L [num]{s,t}
 *
 * This function parses the -L option. It's necessary to have it this way
 * because some options like -j imply it and rather than duplicate code we just
 * have it here once.
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
    /* firewall */
    if (jfmt == NULL || *jfmt == NULL) {
	warn(__func__, "passed NULL struct jfmt ** or *jfmt is NULL");
	return;
    }

    /* flush output file if open and then close it */
    if ((*jfmt)->outfile != NULL && (*jfmt)->outfile != stdout) {
	fflush((*jfmt)->outfile);
	fclose((*jfmt)->outfile);
	(*jfmt)->outfile = NULL;
	(*jfmt)->outfile_path = NULL;
    }

    free(*jfmt);
    *jfmt = NULL;
}
