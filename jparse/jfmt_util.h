/* jfmt_util - utility functions for JSON printer jfmt
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


#if !defined(INCLUDE_JFMT_UTIL_H)
#    define  INCLUDE_JFMT_UTIL_H

#define _GNU_SOURCE /* feature test macro for strcasestr() */
#include <stdlib.h>
#include <unistd.h>
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
#define JFMT_TYPE_NONE    (0)
#define JFMT_TYPE_INT	    (1)
#define JFMT_TYPE_FLOAT   (2)
#define JFMT_TYPE_EXP	    (4)
#define JFMT_TYPE_NUM	    (8)
#define JFMT_TYPE_BOOL    (16)
#define JFMT_TYPE_STR	    (32)
#define JFMT_TYPE_NULL    (64)
#define JFMT_TYPE_OBJECT  (128)
#define JFMT_TYPE_ARRAY   (256)
#define JFMT_TYPE_ANY	    (511) /* bitwise OR of the above values */
/* JFMT_TYPE_SIMPLE is bitwise OR of num, bool, str and null */
#define JFMT_TYPE_SIMPLE  (JFMT_TYPE_NUM|JFMT_TYPE_BOOL|JFMT_TYPE_STR|JFMT_TYPE_NULL)
/* JFMT_TYPE_COMPOUND is bitwise OR of object and array */
#define JFMT_TYPE_COMPOUND (JFMT_TYPE_OBJECT|JFMT_TYPE_ARRAY)

/* print types for -p option */
#define JFMT_PRINT_NAME   (1)
#define JFMT_PRINT_VALUE  (2)
#define JFMT_PRINT_BOTH   (JFMT_PRINT_NAME | JFMT_PRINT_VALUE)


/* structs */

/* structs for various options */
/* number ranges for the options -l, -n and -n */
struct jfmt_number_range
{
    intmax_t min;   /* min in range */
    intmax_t max;   /* max in range */

    bool less_than_equal;	/* true if number type must be <= min */
    bool greater_than_equal;	/* true if number type must be >= max */
    bool inclusive;		/* true if number type must be >= min && <= max */
};
struct jfmt_number
{
    /* exact number if >= 0 */
    intmax_t number;		/* exact number exact number (must be >= 0) */
    bool exact;			/* true if an exact match (number) must be found */

    /* for number ranges */
    struct jfmt_number_range range;	/* for ranges */
};

/* jfmt_array - a struct for when an array matches, used in jfmt_match below
 *
 * XXX - this struct might or might not have to change - XXX
 */
struct jfmt_array
{
    struct json_array *array;

    struct jfmt_match *match;	    /* what matched this array */
};


/*
 * jfmt - struct that holds most of the options, other settings and other data
 */
struct jfmt
{
    /* JSON file related */
    bool is_stdin;				/* reading from stdin */
    FILE *json_file;				/* FILE * to json file */
    char *file_contents;			/* file contents */

    /* out file related to -o */
    char *outfile_path;				/* -o file path */
    FILE *outfile;				/* FILE * of -o outfile */
    bool outfile_not_stdout;			/* -o used without stdout */

    /* number ranges */
    /* level constraints */
    bool levels_constrained;			/* -l specified */
    struct jfmt_number jfmt_levels;		/* -l level specified */

    /* printing related options */
    bool print_json_levels;			/* -L specified */
    uintmax_t num_level_spaces;			/* number of spaces or tab for -L */
    bool print_level_tab;			/* -L tab option */
    bool indent_levels;				/* -I specified */
    uintmax_t indent_spaces;			/* -I specified */
    bool indent_tab;				/* -I <num>[{t|s}] specified */

    /* search related bools */
    uintmax_t max_depth;			/* max depth to traverse set by -m depth */

    struct json *json_tree;			/* json tree if valid merely as a convenience */
};


/* function prototypes */
struct jfmt *alloc_jfmt(void);

/* for number range option: -l */
bool jfmt_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jfmt_number *number);
bool jfmt_number_in_range(intmax_t number, intmax_t total_matches, struct jfmt_number *range);

/* for -I option */
void jfmt_parse_st_indent_option(char *optarg, uintmax_t *indent_level, bool *indent_tab);

/* for -L option */
void jfmt_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab);

/* to free the entire struct jfmt */
void free_jfmt(struct jfmt **jfmt);


#endif /* !defined INCLUDE_JFMT_UTIL_H */
