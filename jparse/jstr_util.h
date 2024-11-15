/*
 * jstr_util - common utility functions for the JSON string encoder/decoder
 *
 * "JSON: when a minimal design falls below a critical minimum." :-)
 *
 * This JSON parser was co-developed in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_JSTR_UTIL_H)
#    define  INCLUDE_JSTR_UTIL_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/dbg.h"
#else
#include <dbg.h>
#endif

/*
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dyn_array/dyn_array.h"
#else
#include <dyn_array.h>
#endif

/*
 * util - common utility functions for the JSON parser
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * jparse - JSON parser
 */
#include "jparse.h"

/*
 * struct for jstr utilities
 */
struct jstring
{
    char *jstr;
    size_t bufsiz;
    struct jstring *next;   /* next in list */
};

/*
 * globals
 */

/*
 * function prototypes
 */
extern struct jstring *alloc_jstr(char *string, size_t bufsiz);
extern int parse_entertainment(char const *optarg);
extern void free_jstring(struct jstring **jstr);
extern void free_jstring_list(struct jstring **jstring_list);

#endif /* INCLUDE_JSTR_UTIL_H */
