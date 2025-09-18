/*
 * jstr_util - common utility functions for the JSON string encoder/decoder
 *
 * "JSON: when a minimal design falls below a critical minimum." :-)
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
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
  #include "../dbg/c_bool.h"
  #include "../dbg/c_compat.h"
  #include "../dbg/dbg.h"
#elif defined(INTERNAL_INCLUDE_2)
  #include "../dbg/c_bool.h"
  #include "../dbg/c_compat.h"
  #include "../dbg/dbg.h"
#else
  #include <c_bool.h>
  #include <c_compat.h>
  #include <dbg.h>
#endif

/*
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
  #include "../dyn_array/dyn_array.h"
#elif defined(INTERNAL_INCLUDE_2)
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
