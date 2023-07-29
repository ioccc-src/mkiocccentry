/* jnamval_util - utility functions for JSON printer jnamval
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


#if !defined(INCLUDE_JNAMVAL_UTIL_H)
#    define  INCLUDE_JNAMVAL_UTIL_H

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
#define JNAMVAL_TYPE_NONE	    (0)
#define JNAMVAL_TYPE_INT	    (1)
#define JNAMVAL_TYPE_FLOAT	    (2)
#define JNAMVAL_TYPE_EXP	    (4)
#define JNAMVAL_TYPE_NUM	    (8)
#define JNAMVAL_TYPE_BOOL	    (16)
#define JNAMVAL_TYPE_STR	    (32)
#define JNAMVAL_TYPE_NULL	    (64)
#define JNAMVAL_TYPE_MEMBER	    (128)
#define JNAMVAL_TYPE_OBJECT	    (256)
#define JNAMVAL_TYPE_ARRAY	    (512)
#define JNAMVAL_TYPE_ANY	    (1023) /* bitwise OR of the above values */
/* JNAMVAL_TYPE_SIMPLE is bitwise OR of num, bool, str and null */
#define JNAMVAL_TYPE_SIMPLE  (JNAMVAL_TYPE_NUM|JNAMVAL_TYPE_BOOL|JNAMVAL_TYPE_STR|JNAMVAL_TYPE_NULL)
/* JNAMVAL_TYPE_COMPOUND is bitwise OR of object and array */
#define JNAMVAL_TYPE_COMPOUND (JNAMVAL_TYPE_MEMBER|JNAMVAL_TYPE_OBJECT|JNAMVAL_TYPE_ARRAY)

/* print types for -p option */
#define JNAMVAL_PRINT_NAME   (1)
#define JNAMVAL_PRINT_VALUE  (2)
#define JNAMVAL_PRINT_JSON   (4)
#define JNAMVAL_PRINT_BOTH   (JNAMVAL_PRINT_NAME | JNAMVAL_PRINT_VALUE)

/* -S and -n */
#define JNAMVAL_CMP_OP_NONE JSON_UTIL_CMP_OP_NONE
#define JNAMVAL_CMP_EQ	    JSON_UTIL_CMP_OP_EQ
#define JNAMVAL_CMP_LT	    JSON_UTIL_CMP_OP_LT
#define JNAMVAL_CMP_LE	    JSON_UTIL_CMP_OP_LE
#define JNAMVAL_CMP_GT	    JSON_UTIL_CMP_OP_GT
#define JNAMVAL_CMP_GE	    JSON_UTIL_CMP_OP_GE

/* structs */

/*
 * jnamval - struct that holds most of the options, other settings and other data
 */
struct jnamval
{
    struct json_util common;			/* common data related to tools: jfmt, jval, jnamval */

    struct json_util_name_val json_name_val; /* common to jval and jnamval */

    /* below are those not common to any other tools */

    /* printing related options */
    bool print_json_types_option;		/* -p explicitly used */
    uintmax_t print_json_types;			/* -p type specified */

    /* search / matching related */
    bool match_json_member_names;		/* -N used, match based on member names */
    bool match_hierarchies;			/* -H used, name hierarchies */
};


/* function prototypes */
struct jnamval *alloc_jnamval(void);

/* JSON types - -t option*/
uintmax_t jnamval_parse_types_option(char *optarg);
bool jnamval_match_none(uintmax_t types);
bool jnamval_match_any(uintmax_t types);
bool jnamval_match_int(uintmax_t types);
bool jnamval_match_float(uintmax_t types);
bool jnamval_match_exp(uintmax_t types);
bool jnamval_match_bool(uintmax_t types);
bool jnamval_match_num(uintmax_t types);
bool jnamval_match_string(uintmax_t types);
bool jnamval_match_null(uintmax_t types);
bool jnamval_match_simple(uintmax_t types);
bool jnamval_match_object(uintmax_t types);
bool jnamval_match_array(uintmax_t types);
bool jnamval_match_compound(uintmax_t types);
bool jnamval_match_member(uintmax_t types);

/* what to print - -p option */
uintmax_t jnamval_parse_print_option(char *optarg);
bool jnamval_print_name(uintmax_t types);
bool jnamval_print_value(uintmax_t types);
bool jnamval_print_both(uintmax_t types);
bool jnamval_print_json(uintmax_t types);


/* functions to print matches */
bool jnamval_print_count(struct jnamval *jnamval);

/* process argv */
void parse_jnamval_args(struct jnamval *jnamval, int *argc, char ***argv);

/* free compare lists */
void free_jnamval_cmp_op_lists(struct jnamval *jnamval);
/* to free the entire struct jnamval */
void free_jnamval(struct jnamval **jnamval);


#endif /* !defined INCLUDE_JNAMVAL_UTIL_H */
