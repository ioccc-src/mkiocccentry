/*
 * json_util - general JSON parser utility support functions
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
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


#if !defined(INCLUDE_JSON_UTIL_H)
#    define  INCLUDE_JSON_UTIL_H

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/dbg.h"
#elif defined(INTERNAL_INCLUDE_2)
#include "dbg/dbg.h"
#else
#include <dbg.h>
#endif



/*
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dyn_array/dyn_array.h"
#elif defined(INTERNAL_INCLUDE_2)
#include "dyn_array/dyn_array.h"
#else
#include <dyn_array.h>
#endif

/*
 * util - utilities and macros
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
 * JSON defines
 */

/*
 * JSON parser related definitions and structures
 */
#define JSON_CHUNK (16)			/* number of pointers to allocate at a time in dynamic array */
#define JSON_DEFAULT_MAX_DEPTH (256)	/* a sane parse tree depth to use */
#define JSON_INFINITE_DEPTH (0)		/* no limit on parse tree depth to walk */


/*
 * JSON debug levels
 */
#define JSON_DBG_DEFAULT    (JSON_DBG_NONE) /* default JSON debug information related to the parser */
#define JSON_DBG_NONE	    (DBG_NONE)	    /* no JSON debugging information related to the parser */
#define JSON_DBG_LOW	    (DBG_LOW)	    /* minimal JSON debugging information related to parser */
#define JSON_DBG_MED	    (DBG_MED)	    /* somewhat more JSON debugging information related to parser */
#define JSON_DBG_HIGH	    (DBG_HIGH)	    /* verbose JSON debugging information related to parser */
#define JSON_DBG_VHIGH	    (DBG_VHIGH)	    /* very verbose debugging information related to parser */
#define JSON_DBG_VVHIGH	    (DBG_VVHIGH)    /* very very verbose debugging information related to parser */
#define JSON_DBG_VVVHIGH    (DBG_VVVHIGH)   /* very very very verbose debugging information related to parser */
#define JSON_DBG_VVVVHIGH   (DBG_VVVVHIGH)  /* very very very very verbose debugging information related to parser */
#define JSON_DBG_FORCED	    (-1)	    /* always print information, even if dbg_output_allowed == false */

/* WARNING: the below enums, macros and structs are VERY subject to change, if they are not deleted */

/* comparison enum - for json tools that might wish to compare JSON related values */
enum JSON_UTIL_CMP_OP
{
    JSON_CMP_OP_NONE = 0,	/* must be first */
    JSON_CMP_OP_EQ = 1,		/* equality check */
    JSON_CMP_OP_NE = 2,		/* not equal */
    JSON_CMP_OP_LT = 3,		/* less than check */
    JSON_CMP_OP_LE = 4,		/* less than or equal check */
    JSON_CMP_OP_GT = 5,		/* greater than check */
    JSON_CMP_OP_GE = 6,		/* greater than or equal check */
};


/* JSON element match types - for json tools that might wish to match based on JSON types */
#define JSON_UTIL_MATCH_TYPE_NONE	    (0)
#define JSON_UTIL_MATCH_TYPE_INT	    (1)
#define JSON_UTIL_MATCH_TYPE_FLOAT	    (2)
#define JSON_UTIL_MATCH_TYPE_EXP	    (4)
#define JSON_UTIL_MATCH_TYPE_NUM	    (8)
#define JSON_UTIL_MATCH_TYPE_BOOL	    (16)
#define JSON_UTIL_MATCH_TYPE_STR	    (32)
#define JSON_UTIL_MATCH_TYPE_NULL	    (64)
/* JSON_UTIL_MATCH_TYPE_SIMPLE is bitwise OR of num, bool, str and null */
#define JSON_UTIL_MATCH_TYPE_SIMPLE  \
    (JSON_UTIL_MATCH_TYPE_NUM|JSON_UTIL_MATCH_TYPE_BOOL|JSON_UTIL_MATCH_TYPE_STR|JSON_UTIL_MATCH_TYPE_NULL)


/* structures */

/* structures: this may be useful for json tools that need to compare JSON related values */

struct json_util_number_range
{
    intmax_t min;   /* min in range */
    intmax_t max;   /* max in range */

    bool less_than_equal;	/* true if number type must be <= min */
    bool greater_than_equal;	/* true if number type must be >= max */
    bool inclusive;		/* true if number type must be >= min && <= max */
};

struct json_util_number
{
    /* exact number if >= 0 */
    intmax_t number;		/* exact number exact number (must be >= 0) */
    bool exact;			/* true if an exact match (number) must be found */

    /* for number ranges */
    struct json_util_number_range range;	/* for ranges */
};

/* End WARNING from above */


/*
 * global variables
 */
extern int json_verbosity_level;	/* print json debug messages <= json_verbosity_level in json_dbg(), json_vdbg() */
extern int const hexval[];

/*
 * external function declarations
 */
extern bool json_dbg_allowed(int json_dbg_lvl);
extern bool json_warn_allowed(void);
extern bool json_err_allowed(void);
extern void json_dbg(int json_dbg_lvl, char const *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern void json_vdbg(int json_dbg_lvl, char const *name, const char *fmt, va_list ap);
extern bool json_putc(uint8_t const c, FILE *stream);
extern bool json_fprintf_str(FILE *stream, char const *str);
extern bool json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
				      char const *tail);
extern bool json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
				    char const *tail);
extern bool json_fprintf_value_time_t(FILE *stream, char const *lead, char const *name, char const *middle, time_t value,
				      char const *tail);
extern bool json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
				    char const *tail);
extern char const *json_type_name(enum item_type type);
extern char const *json_item_type_name(const struct json *node);
extern char const *json_get_type_str(struct json *node, bool encoded);
extern struct json * open_json_dir_file(char const *dir, char const *filename);
extern void json_free(struct json *node, unsigned int depth, ...);
extern void vjson_free(struct json *node, unsigned int depth, va_list ap);
extern void json_fprint(struct json *node, unsigned int depth, ...);
extern void vjson_fprint(struct json *node, unsigned int depth, va_list ap);
extern void json_tree_print(struct json *node, unsigned int max_depth, ...);
extern void json_dbg_tree_print(int json_dbg_lvl, char const *name, struct json *tree, unsigned int max_depth);
extern void json_tree_free(struct json *node, unsigned int max_depth, ...);
extern void json_tree_walk(struct json *node, unsigned int max_depth, unsigned int depth, bool post_order,
			   void (*vcallback)(struct json *, unsigned int, va_list), ...);
extern void vjson_tree_walk(struct json *node, unsigned int max_depth, unsigned int depth, bool post_order,
			    void (*vcallback)(struct json *, unsigned int, va_list), va_list ap);

/* WARNING: the below functions are VERY subject to change, if they are not deleted */
bool json_util_parse_number_range(const char *option, char *optarg, bool allow_negative, struct json_util_number *number);
bool json_util_number_in_range(intmax_t number, intmax_t total_matches, struct json_util_number *range);
void json_util_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab);
/* JSON types - -t option*/
uintmax_t json_util_parse_match_types(char *optarg);
bool json_util_match_none(uintmax_t types);
bool json_util_match_int(uintmax_t types);
bool json_util_match_float(uintmax_t types);
bool json_util_match_exp(uintmax_t types);
bool json_util_match_exp(uintmax_t types);
bool json_util_match_bool(uintmax_t types);
bool json_util_match_num(uintmax_t types);
bool json_util_match_string(uintmax_t types);
bool json_util_match_null(uintmax_t types);
bool json_util_match_simple(uintmax_t types);
/* End WARNING */


#endif /* INCLUDE_JSON_UTIL_H */
