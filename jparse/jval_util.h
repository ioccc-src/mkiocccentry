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
/* JVAL_TYPE_SIMPLE is bitwise OR of num, bool, str and null */
#define JVAL_TYPE_SIMPLE  (JVAL_TYPE_NUM|JVAL_TYPE_BOOL|JVAL_TYPE_STR|JVAL_TYPE_NULL)

#define JVAL_CMP_OP_NONE    (0)
#define JVAL_CMP_EQ	    (1)
#define JVAL_CMP_LT	    (2)
#define JVAL_CMP_LE	    (3)
#define JVAL_CMP_GT	    (4)
#define JVAL_CMP_GE	    (5)

/* structs */

/* structs for various options */

/* for comparison of numbers / strings - options -n and -S */
struct jval_cmp_op
{
    struct json_number *number;	    /* for -n as signed number */
    struct json_string *string;	    /* for -S str */

    bool is_string;	    /* true if -S */
    bool is_number;	    /* true if -n */
    uintmax_t op;	    /* the operation - see JVAL_CMP macros above */

    struct jval_cmp_op *next;	/* next in the list */
};

/*
 * jval - struct that holds most of the options, other settings and other data
 */
struct jval
{
    struct json_util common;			/* data common to all three tools: jfmt, jval and jnamval */

    /* string related options */
    bool encode_strings;			/* -e used */
    bool quote_strings;				/* -Q used */

    /* printing related options */
    bool print_decoded;				/* -D used */

    /* search / matching related */
    bool invert_matches;			/* -i used */
    bool json_types_specified;			/* -t used */
    uintmax_t json_types;			/* -t type */
    bool ignore_case;				/* true if -f, case-insensitive */
    bool match_decoded;				/* -d used - match decoded */
    bool arg_specified;				/* true if an arg was specified */
    bool match_substrings;			/* -s used, match substrings */
    bool use_regexps;				/* -g used, allow grep-like regexps */
    uintmax_t total_matches;			/* for -c */

    bool count_only;				/* -c used, only show count */
    bool count_and_show_values;			/* -C used, show count and values */

    bool string_cmp_used;			/* for -S */
    struct jval_cmp_op *string_cmp;		/* for -S str */
    bool num_cmp_used;				/* for -n */
    struct jval_cmp_op *num_cmp;		/* for -n num */
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
bool jval_match_simple(uintmax_t types);

/* for -S and -n */
struct jval_cmp_op *jval_parse_cmp_op(struct jval *jval, const char *option, char *optarg);

/* functions to print matches */
bool jval_print_count(struct jval *jval);

/*
 * XXX - currently does nothing functionally - might or might not be needed to
 * process argv - XXX
 */
void parse_jval_args(struct jval *jnamval, char **argv);

/* free compare lists */
void free_jval_cmp_op_lists(struct jval *jval);
/* to free the entire struct jval */
void free_jval(struct jval **jval);


#endif /* !defined INCLUDE_JVAL_UTIL_H */
