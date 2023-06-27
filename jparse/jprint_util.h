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


/* structs */

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

/*
 * jprint_match - a struct for a linked list of patterns matched in each pattern
 * requested.
 *
 * XXX - this struct is a work in progress - XXX
 */
struct jprint_match
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

    struct jprint_pattern *pattern; /* pointer to the pattern that matched. DO NOT FREE! */
    struct jprint_match *next; /* next match found */
};

/*
 * jprint_pattern - struct for a linked list of patterns requested, held in
 * struct jprint
 *
 * XXX - the pattern concept is incorrect due to a misunderstanding - XXX
 */
struct jprint_pattern
{
    char *pattern;		    /* the actual pattern or regexp string */
    bool use_regexp;		    /* whether -g was used */
    bool use_value;		    /* whether -Y was used, implying to search values */
    bool use_substrings;	    /* if -s was used */
    uintmax_t matches_found;	    /* number of matches found so far with this pattern */

    struct jprint_pattern *next;    /* the next in the list */

    struct jprint_match *matches;   /* matches found */
};

/*
 * jprint - struct that holds most of the options, other settings and other data
 */
struct jprint
{
    bool is_stdin;				/* reading from stdin */
    FILE *json_file;				/* FILE * to json file */
    char *file_contents;			/* file contents */
    bool match_found;				/* true if a pattern is specified and there is a match */
    bool ignore_case;				/* true if -i, case-insensitive */
    bool pattern_specified;			/* true if a pattern was specified */
    bool encode_strings;			/* -e used */
    bool quote_strings;				/* -Q used */
    bool type_specified;			/* -t used */
    uintmax_t type;				/* -t type */
    struct jprint_number jprint_max_matches;	/* -n count specified */
    bool max_matches_requested;			/* -n used */
    struct jprint_number jprint_min_matches;	/* -N count specified */
    bool min_matches_requested;			/* -N used */
    struct jprint_number jprint_levels;		/* -l level specified */
    bool levels_constrained;			/* -l specified */
    uintmax_t print_type;			/* -p type specified */
    bool print_type_option;			/* -p explicitly used */
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
    bool match_encoded;				/* -E used, match encoded name */
    bool use_substrings;			/* -s used, matching substrings okay */
    bool use_regexps;				/* -g used, allow grep-like regexps */
    bool count_only;				/* -c used, only show count */
    bool print_entire_file;			/* no name_arg specified */
    uintmax_t max_depth;			/* max depth to traverse set by -m depth */
    bool search_value;				/* -Y used, search for value, not name */
    FILE *check_tool_stream;			/* FILE * stream for -S path */
    char *check_tool_path;			/* -S used */
    char *check_tool_args;			/* -A used */

    /* any patterns specified */
    struct jprint_pattern *patterns;		/* linked list of patterns specified */
    uintmax_t number_of_patterns;		/* patterns specified */
    struct jprint_match *matches;		/* for entire file i.e. no name_arg */
    uintmax_t total_matches;			/* total matches of all patterns (name_args) */

    struct json *json_tree;			/* json tree if valid merely as a convenience */
};


/* function prototypes */
struct jprint *alloc_jprint(void);

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

/* -Y type option */
uintmax_t jprint_parse_value_type_option(char *optarg);

/* what to print - -p option */
uintmax_t jprint_parse_print_option(char *optarg);
bool jprint_print_name(uintmax_t types);
bool jprint_print_value(uintmax_t types);
bool jprint_print_name_value(uintmax_t types);

/* for number range options: -l, -n, -n */
bool jprint_parse_number_range(const char *option, char *optarg, bool allow_negative, struct jprint_number *number);
bool jprint_number_in_range(intmax_t number, intmax_t total_matches, struct jprint_number *range);

/* for -b option */
void jprint_parse_st_tokens_option(char *optarg, uintmax_t *num_token_spaces, bool *print_token_tab);

/* for -I option */
void jprint_parse_st_indent_option(char *optarg, uintmax_t *indent_level, bool *indent_tab);

/* for -L option */
void jprint_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab);

/*
 * patterns (name_args) specified
 *
 * XXX - the concept of the patterns is incorrect in that it's not individual
 * patterns but rather the second, third and Nth patterns are under the previous
 * patterns in the file.
 */
void parse_jprint_name_args(struct jprint *jprint, char **argv);
struct jprint_pattern *add_jprint_pattern(struct jprint *jprint, bool use_regexp, bool use_substrings, char *str);
void free_jprint_patterns_list(struct jprint *jprint);

/* matches found of each pattern */
struct jprint_match *add_jprint_match(struct jprint *jprint, struct jprint_pattern *pattern,
	struct json *node_name, struct json *node_value, char *name_str, char *value_str, uintmax_t level,
	enum item_type name_type, enum item_type value_type);
void free_jprint_matches_list(struct jprint_pattern *pattern);

/* functions to find matches in the JSON tree */
bool is_jprint_match(struct jprint *jprint, struct jprint_pattern *pattern, char *name, struct json *node, char *str);
void jprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, ...);
void vjprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, va_list ap);
void jprint_json_tree_search(struct jprint *jprint, struct json *node, unsigned int max_depth, ...);
void jprint_json_tree_walk(struct jprint *jprint, struct json *node, bool is_value, unsigned int max_depth, unsigned int depth,
		void (*vcallback)(struct jprint *, struct json *, bool, unsigned int, va_list), va_list ap);

/* functions to print matches */
bool jprint_print_count(struct jprint *jprint);
void jprint_print_final_comma(struct jprint *jprint);
void jprint_print_brace(struct jprint *jprint, bool open);
void jprint_print_match(struct jprint *jprint, struct jprint_pattern *pattern, struct jprint_match *match);
void jprint_print_matches(struct jprint *jprint);


/* for the -S check tool and -A check tool args */
void run_jprint_check_tool(struct jprint *jprint, char **argv);

/* to free the entire struct jprint */
void free_jprint(struct jprint **jprint);


#endif /* !defined INCLUDE_JPRINT_UTIL_H */
