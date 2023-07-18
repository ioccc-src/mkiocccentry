/* jval_util - utility functions for JSON printer jval
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


#if !defined(INCLUDE_JVAL_UTIL_H)
#    define  INCLUDE_JVAL_UTIL_H

#define _GNU_SOURCE /* feature test macro for strcasestr() */
#include <stdlib.h>
#include <unistd.h>
#include <regex.h> /* for -g and -G, regular expression matching */
#include <string.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * dyn_array - dynamic array facility
 */
#include "../dyn_array/dyn_array.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * jparse - JSON parser
 */
#include "jparse.h"

/* defines */

/* -t types */
#define JVAL_TYPE_NONE	    (0)
#define JVAL_TYPE_INT	    (1)
#define JVAL_TYPE_FLOAT	    (2)
#define JVAL_TYPE_EXP	    (4)
#define JVAL_TYPE_NUM	    (8)
#define JVAL_TYPE_BOOL	    (16)
#define JVAL_TYPE_STR	    (32)
#define JVAL_TYPE_NULL	    (64)
#define JVAL_TYPE_ANY	    (127) /* bitwise OR of the above values */
/* JVAL_TYPE_SIMPLE is bitwise OR of num, bool, str and null */
#define JVAL_TYPE_SIMPLE  (JVAL_TYPE_NUM|JVAL_TYPE_BOOL|JVAL_TYPE_STR|JVAL_TYPE_NULL)

#define JVAL_CMP_EQ	(1)
#define JVAL_CMP_LT	(2)
#define JVAL_CMP_LE	(3)
#define JVAL_CMP_GT	(4)
#define JVAL_CMP_GE	(5)

/* structs */

/* structs for various options */

/* for comparison of numbers / strings - options -n and -S */
struct jval_cmp_op
{
    struct json_number *number;	    /* for -n as signed number */
    struct json_string *string;	    /* for -S str */

    uintmax_t op;	    /* the operation - see JVAL_CMP macros above */
};

/* number ranges for the options -l, -n and -n */
struct jval_number_range
{
    intmax_t min;   /* min in range */
    intmax_t max;   /* max in range */

    bool less_than_equal;	/* true if number type must be <= min */
    bool greater_than_equal;	/* true if number type must be >= max */
    bool inclusive;		/* true if number type must be >= min && <= max */
};
struct jval_number
{
    /* exact number if >= 0 */
    intmax_t number;		/* exact number exact number (must be >= 0) */
    bool exact;			/* true if an exact match (number) must be found */

    /* for number ranges */
    struct jval_number_range range;	/* for ranges */
};

/*
 * jval - struct that holds most of the options, other settings and other data
 */
struct jval
{
    /* JSON file related */
    bool is_stdin;				/* reading from stdin */
    FILE *json_file;				/* FILE * to json file */
    char *file_contents;			/* file contents */

    /* out file related to -o */
    char *outfile_path;				/* -o file path */
    FILE *outfile;				/* FILE * of -o ofile */
    bool outfile_not_stdout;			/* -o used without stdout */

    /* string related options */
    bool encode_strings;			/* -e used */
    bool quote_strings;				/* -Q used */

    /* level constraints */
    bool levels_constrained;			/* -l specified */
    struct jval_number jval_levels;		/* -l level specified */

    /* printing related options */
    bool print_decoded;				/* -D used */
    bool print_json_levels;			/* -L specified */
    uintmax_t num_level_spaces;			/* number of spaces or tab for -L */
    bool print_level_tab;			/* -L tab option */
    bool invert_matches;			/* -i used */
    bool count_only;				/* -c used, only show count */
    bool count_and_show_values;			/* -C used, show count and values */

    /* search / matching related */
    bool json_types_specified;			/* -t used */
    uintmax_t json_types;			/* -t type */
    bool ignore_case;				/* true if -f, case-insensitive */
    bool match_decoded;				/* -d used - match decoded */
    bool arg_specified;				/* true if an arg was specified */
    bool match_substrings;			/* -s used, match substrings */
    bool use_regexps;				/* -g used, allow grep-like regexps */
    uintmax_t total_matches;			/* for -c */

    bool string_cmp_used;			/* for -S */
    struct jval_cmp_op string_cmp;		/* for -S str */
    bool num_cmp_used;				/* for -n */
    struct jval_cmp_op num_cmp;			/* for -n num */
    uintmax_t max_depth;			/* max depth to traverse set by -m depth */
    struct json *json_tree;			/* json tree if valid merely as a convenience */
};


/* function prototypes */
struct jval *alloc_jval(void);

/* JSON types - -t option*/
uintmax_t jval_parse_types_option(char *optarg);
bool jval_match_none(uintmax_t types);
bool jval_match_int(uintmax_t types);
bool jval_match_float(uintmax_t types);
bool jval_match_exp(uintmax_t types);
bool jval_match_exp(uintmax_t types);
bool jval_match_bool(uintmax_t types);
bool jval_match_num(uintmax_t types);
bool jval_match_string(uintmax_t types);
bool jval_match_null(uintmax_t types);
bool jval_match_any(uintmax_t types);
bool jval_match_simple(uintmax_t types);

/* for number range option -l */
bool jval_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jval_number *number);
bool jval_number_in_range(intmax_t number, intmax_t total_matches, struct jval_number *range);

/* for -S and -n */
void jval_parse_cmp_op(struct jval *jval, const char *option, char *optarg, struct jval_cmp_op *cmp);

/* for -L option */
void jval_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab);

/* functions to print matches */
bool jval_print_count(struct jval *jval);

/*
 * XXX - currently does nothing functionally - might or might not be needed to
 * process argv - XXX
 */
void parse_jval_args(struct jval *jnamval, char **argv);

/* to free the entire struct jval */
void free_jval(struct jval **jval);


#endif /* !defined INCLUDE_JVAL_UTIL_H */
