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


/* function prototypes */
uintmax_t jprint_parse_types_option(char *optarg);
bool match_none(uintmax_t types);
bool match_int(uintmax_t types);
bool match_float(uintmax_t types);
bool match_exp(uintmax_t types);
bool match_bool(uintmax_t types);
bool match_string(uintmax_t types);
bool match_null(uintmax_t types);
bool match_object(uintmax_t types);
bool match_array(uintmax_t types);
bool match_any(uintmax_t types);
bool match_simple(uintmax_t types);
bool match_compound(uintmax_t types);



#endif /* !defined INCLUDE_JPRINT_UTIL_H */
