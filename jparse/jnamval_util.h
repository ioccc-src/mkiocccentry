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
#define JNAMVAL_TYPE_NONE    (0)
#define JNAMVAL_TYPE_INT	    (1)
#define JNAMVAL_TYPE_FLOAT   (2)
#define JNAMVAL_TYPE_EXP	    (4)
#define JNAMVAL_TYPE_NUM	    (8)
#define JNAMVAL_TYPE_BOOL    (16)
#define JNAMVAL_TYPE_STR	    (32)
#define JNAMVAL_TYPE_NULL    (64)
#define JNAMVAL_TYPE_OBJECT  (128)
#define JNAMVAL_TYPE_ARRAY   (256)
#define JNAMVAL_TYPE_ANY	    (511) /* bitwise OR of the above values */
/* JNAMVAL_TYPE_SIMPLE is bitwise OR of num, bool, str and null */
#define JNAMVAL_TYPE_SIMPLE  (JNAMVAL_TYPE_NUM|JNAMVAL_TYPE_BOOL|JNAMVAL_TYPE_STR|JNAMVAL_TYPE_NULL)
/* JNAMVAL_TYPE_COMPOUND is bitwise OR of object and array */
#define JNAMVAL_TYPE_COMPOUND (JNAMVAL_TYPE_OBJECT|JNAMVAL_TYPE_ARRAY)

/* print types for -p option */
#define JNAMVAL_PRINT_NAME   (1)
#define JNAMVAL_PRINT_VALUE  (2)
#define JNAMVAL_PRINT_BOTH   (JNAMVAL_PRINT_NAME | JNAMVAL_PRINT_VALUE)


/* structs */

/* structs for various options */
/* number ranges for the options -l, -n and -n */
struct jnamval_number_range
{
    intmax_t min;   /* min in range */
    intmax_t max;   /* max in range */

    bool less_than_equal;	/* true if number type must be <= min */
    bool greater_than_equal;	/* true if number type must be >= max */
    bool inclusive;		/* true if number type must be >= min && <= max */
};
struct jnamval_number
{
    /* exact number if >= 0 */
    intmax_t number;		/* exact number exact number (must be >= 0) */
    bool exact;			/* true if an exact match (number) must be found */

    /* for number ranges */
    struct jnamval_number_range range;	/* for ranges */
};

/* jnamval_array - a struct for when an array matches, used in jnamval_match below
 *
 * XXX - this struct might or might not have to change - XXX
 */
struct jnamval_array
{
    struct json_array *array;

    struct jnamval_match *match;	    /* what matched this array */
};
/*
 * jnamval_match - a struct for a linked list of patterns matched in each pattern
 * requested.
 *
 * XXX - this struct is a work in progress - XXX
 */
struct jnamval_match
{
    char *match;		    /* string that matched */
    char *value;		    /* name or value string of that which matched */

    uintmax_t count;		    /* how many of this match are found */
    uintmax_t level;		    /* the level of the json member for -l */
    uintmax_t number;		    /* which match this is */
    enum item_type name_type;	    /* match type of name */
    enum item_type value_type;	    /* match type of value */

    struct json *node_name;	    /* struct json * node name. DO NOT FREE! */
    struct json *node_value;	    /* struct json * node value. DO NOT FREE! */

    struct jnamval_array *array;	    /* will not be NULL if match is an array. */

    struct jnamval_pattern *pattern; /* pointer to the pattern that matched. DO NOT FREE! */
    struct jnamval_match *next; /* next match found */
};

/*
 * jnamval_pattern - struct for a linked list of patterns requested, held in
 * struct jnamval
 *
 * XXX - the pattern concept is incorrect due to a misunderstanding - XXX
 */
struct jnamval_pattern
{
    char *pattern;		    /* the actual pattern or regexp string */
    bool use_regexp;		    /* whether -g was used */
    bool use_value;		    /* whether -Y was used, implying to search values */
    bool use_substrings;	    /* if -s was used */
    uintmax_t matches_found;	    /* number of matches found so far with this pattern */

    struct jnamval_pattern *next;    /* the next in the list */

    struct jnamval_match *matches;   /* matches found */
};

/*
 * jnamval - struct that holds most of the options, other settings and other data
 */
struct jnamval
{
    /* JSON file related */
    bool is_stdin;				/* reading from stdin */
    FILE *json_file;				/* FILE * to json file */
    char *file_contents;			/* file contents */

    /* string related options */
    bool encode_strings;			/* -e used */
    bool quote_strings;				/* -Q used */


    /* number ranges */
    /* max number of matches */
    bool max_matches_requested;			/* -n used */
    struct jnamval_number jnamval_max_matches;	/* -n count specified */
    /* min number of matches */
    bool min_matches_requested;			/* -N used */
    struct jnamval_number jnamval_min_matches;	/* -N count specified */
    /* level constraints */
    bool levels_constrained;			/* -l specified */
    struct jnamval_number jnamval_levels;		/* -l level specified */

    /* printing related options */
    bool print_json_types_option;		/* -p explicitly used */
    uintmax_t print_json_types;			/* -p type specified */
    bool print_token_spaces;			/* -b specified */
    uintmax_t num_token_spaces;			/* -b specified number of spaces or tabs */
    bool print_token_tab;			/* -b tab (or -b <num>[t]) specified */
    bool print_json_levels;			/* -L specified */
    uintmax_t num_level_spaces;			/* number of spaces or tab for -L */
    bool print_level_tab;			/* -L tab option */
    bool print_colons;				/* -P specified */
    bool print_final_comma;			/* -C specified */
    bool print_braces;				/* -B specified */
    bool indent_levels;				/* -I specified */
    uintmax_t indent_spaces;			/* -I specified */
    bool indent_tab;				/* -I <num>[{t|s}] specified */
    bool print_syntax;				/* -j used, will imply -p b -b 1 -c -e -Q -I 4 -t any */

    /* search related bools */
    bool json_types_specified;			/* -t used */
    uintmax_t json_types;			/* -t type */
    bool print_entire_file;			/* no name_arg specified */
    bool match_found;				/* true if a pattern is specified and there is a match */
    bool ignore_case;				/* true if -i, case-insensitive */
    bool pattern_specified;			/* true if a pattern was specified */
    bool search_value;				/* -Y used, search for value, not name */
    bool search_or_mode;			/* -o used: search with OR mode */
    bool search_anywhere;			/* -r used: search under anywhere */
    bool match_encoded;				/* -E used, match encoded name */
    bool use_substrings;			/* -s used, matching substrings okay */
    bool use_regexps;				/* -g used, allow grep-like regexps */
    bool count_only;				/* -c used, only show count */
    uintmax_t max_depth;			/* max depth to traverse set by -m depth */

    /* check tool related things */
    bool check_tool_specified;			/* bool indicating -S was used */
    FILE *check_tool_stream;			/* FILE * stream for -S path */
    char *check_tool_path;			/* -S tool path used */
    char *check_tool_args;			/* -A tool args used */

    /* any patterns specified */
    /* XXX - the pattern concept is incorrect - XXX */
    struct jnamval_pattern *patterns;		/* linked list of patterns specified */
    uintmax_t number_of_patterns;		/* patterns specified */
    /* matches found - subject to change */
    struct jnamval_match *matches;		/* for entire file i.e. no name_arg */
    uintmax_t total_matches;			/* total matches of all patterns (name_args) */

    struct json *json_tree;			/* json tree if valid merely as a convenience */
};


/* function prototypes */
struct jnamval *alloc_jnamval(void);

/* JSON types - -t option*/
uintmax_t jnamval_parse_types_option(char *optarg);
bool jnamval_match_none(uintmax_t types);
bool jnamval_match_int(uintmax_t types);
bool jnamval_match_float(uintmax_t types);
bool jnamval_match_exp(uintmax_t types);
bool jnamval_match_exp(uintmax_t types);
bool jnamval_match_bool(uintmax_t types);
bool jnamval_match_num(uintmax_t types);
bool jnamval_match_string(uintmax_t types);
bool jnamval_match_null(uintmax_t types);
bool jnamval_match_object(uintmax_t types);
bool jnamval_match_array(uintmax_t types);
bool jnamval_match_any(uintmax_t types);
bool jnamval_match_simple(uintmax_t types);
bool jnamval_match_compound(uintmax_t types);

/* -Y type option */
uintmax_t jnamval_parse_value_type_option(char *optarg);

/* what to print - -p option */
uintmax_t jnamval_parse_print_option(char *optarg);
bool jnamval_print_name(uintmax_t types);
bool jnamval_print_value(uintmax_t types);
bool jnamval_print_name_value(uintmax_t types);

/* for number range options: -l, -n, -n */
bool jnamval_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jnamval_number *number);
bool jnamval_number_in_range(intmax_t number, intmax_t total_matches, struct jnamval_number *range);

/* for -b option */
void jnamval_parse_st_tokens_option(char *optarg, uintmax_t *num_token_spaces, bool *print_token_tab);

/* for -I option */
void jnamval_parse_st_indent_option(char *optarg, uintmax_t *indent_level, bool *indent_tab);

/* for -L option */
void jnamval_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab);

/*
 * patterns (name_args) specified
 *
 * XXX - the concept of the patterns is incorrect in that it's not individual
 * patterns but rather the second, third and Nth patterns are under the previous
 * patterns in the file.
 */
void parse_jnamval_name_args(struct jnamval *jnamval, char **argv);
struct jnamval_pattern *add_jnamval_pattern(struct jnamval *jnamval, bool use_regexp, bool use_substrings, char *str);
void free_jnamval_patterns_list(struct jnamval *jnamval);

/* matches found of each pattern */
struct jnamval_match *add_jnamval_match(struct jnamval *jnamval, struct jnamval_pattern *pattern,
	struct json *node_name, struct json *node_value, char const *name_str, char const *value_str, uintmax_t level,
	enum item_type name_type, enum item_type value_type);
void free_jnamval_matches_list(struct jnamval_pattern *pattern);

/* functions to find matches in the JSON tree */
bool is_jnamval_match(struct jnamval *jnamval, struct jnamval_pattern *pattern, char const *name, struct json *node, char const *str);
void jnamval_json_search(struct jnamval *jnamval, struct json *name_node, struct json *value_node, bool is_value,
	unsigned int depth, ...);
void vjnamval_json_search(struct jnamval *jnamval, struct json *name_node, struct json *value_node, bool is_value,
	unsigned int depth, va_list ap);
void jnamval_json_tree_search(struct jnamval *jnamval, struct json *node, unsigned int max_depth, ...);
void jnamval_json_tree_walk(struct jnamval *jnamval, struct json *lnode, struct json *rnode, bool is_value,
		unsigned int max_depth, unsigned int depth, void (*vcallback)(struct jnamval *, struct json *, struct json *, bool,
		unsigned int, va_list), va_list ap);

/* functions to print matches */
bool jnamval_print_count(struct jnamval *jnamval);
void jnamval_print_final_comma(struct jnamval *jnamval);
void jnamval_print_brace(struct jnamval *jnamval, bool open);
void jnamval_print_match(struct jnamval *jnamval, struct jnamval_pattern *pattern, struct jnamval_match *match);
void jnamval_print_matches(struct jnamval *jnamval);


/* for the -S check tool and -A check tool args */
void run_jnamval_check_tool(struct jnamval *jnamval, char **argv);

/* to free the entire struct jnamval */
void free_jnamval(struct jnamval **jnamval);


#endif /* !defined INCLUDE_JNAMVAL_UTIL_H */
