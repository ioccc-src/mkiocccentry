/* jprint_util - utility functions for JSON printer jprint
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


#if !defined(INCLUDE_JPRINT_UTIL_H)
#    define  INCLUDE_JPRINT_UTIL_H

#include <stdlib.h>
#include <unistd.h>
#include <regex.h> /* for -g, regular expression matching */
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
#define JPRINT_TYPE_NONE    (0)
#define JPRINT_TYPE_INT	    (1)
#define JPRINT_TYPE_FLOAT   (2)
#define JPRINT_TYPE_EXP	    (4)
#define JPRINT_TYPE_NUM	    (8)
#define JPRINT_TYPE_BOOL    (16)
#define JPRINT_TYPE_STR	    (32)
#define JPRINT_TYPE_NULL    (64)
#define JPRINT_TYPE_OBJECT  (128)
#define JPRINT_TYPE_ARRAY   (256)
#define JPRINT_TYPE_ANY	    (511) /* bitwise OR of the above values */
/* JPRINT_TYPE_SIMPLE is bitwise OR of num, bool, str and null */
#define JPRINT_TYPE_SIMPLE  (JPRINT_TYPE_NUM|JPRINT_TYPE_BOOL|JPRINT_TYPE_STR|JPRINT_TYPE_NULL)
/* JPRINT_TYPE_COMPOUND is bitwise OR of object and array */
#define JPRINT_TYPE_COMPOUND (JPRINT_TYPE_OBJECT|JPRINT_TYPE_ARRAY)

/* print types for -p option */
#define JPRINT_PRINT_NAME   (1)
#define JPRINT_PRINT_VALUE  (2)
#define JPRINT_PRINT_BOTH   (JPRINT_PRINT_NAME | JPRINT_PRINT_VALUE)

/* structs for various options */

/* number ranges for the options -l, -n and -n */
struct jprint_number_range
{
    intmax_t min;   /* min in range */
    intmax_t max;   /* max in range */

    bool less_than_equal;	/* true if number type must be <= min */
    bool greater_than_equal;	/* true if number type must be >= max */
    bool inclusive;		/* true if number type must be >= min && <= max */
};
struct jprint_number
{
    /* exact number if >= 0 */
    intmax_t number;		/* exact number exact number (must be >= 0) */
    bool exact;			/* true if an exact match (number) must be found */

    /* for number ranges */
    struct jprint_number_range range;	/* for ranges */
};

/* function prototypes */

/* JSON types - -t option*/
uintmax_t jprint_parse_types_option(char *optarg);
bool jprint_match_none(uintmax_t types);
bool jprint_match_int(uintmax_t types);
bool jprint_match_float(uintmax_t types);
bool jprint_match_exp(uintmax_t types);
bool jprint_match_exp(uintmax_t types);
bool jprint_match_bool(uintmax_t types);
bool jprint_match_num(uintmax_t types);
bool jprint_match_string(uintmax_t types);
bool jprint_match_null(uintmax_t types);
bool jprint_match_object(uintmax_t types);
bool jprint_match_array(uintmax_t types);
bool jprint_match_any(uintmax_t types);
bool jprint_match_simple(uintmax_t types);
bool jprint_match_compound(uintmax_t types);

/* what to print - -p option */
uintmax_t jprint_parse_print_option(char *optarg);
bool jprint_print_name(uintmax_t types);
bool jprint_print_value(uintmax_t types);
bool jprint_print_name_value(uintmax_t types);

/* for number range options: -l, -n, -n */
bool jprint_parse_number_range(const char *option, char *optarg, struct jprint_number *number);
bool jprint_number_in_range(intmax_t number, intmax_t total_matches, struct jprint_number *range);

#endif /* !defined INCLUDE_JPRINT_UTIL_H */
