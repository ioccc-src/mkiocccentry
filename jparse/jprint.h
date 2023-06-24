/* jprint - JSON printer
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


#if !defined(INCLUDE_JPRINT_H)
#    define  INCLUDE_JPRINT_H

#define _GNU_SOURCE /* feature test macro for strcasestr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h> /* for -g, regular expression matching */
#include <strings.h> /* for -i, strcasecmp */

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
 * jprint_util - our utility functions, macros and variables
 */
#include "jprint_util.h"

/*
 * jprint_test - test functions
 */
#include "jprint_test.h"

/*
 * jparse - JSON parser
 */
#include "jparse.h"

/* jprint version string */
#define JPRINT_VERSION "0.0.27 2023-06-24"		/* format: major.minor YYYY-MM-DD */

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
    enum item_type type;	    /* match type */
    bool string;		    /* match is a string */

    struct json *node_name;	    /* struct json * node name. DO NOT FREE! */
    struct json *node_value;	    /* struct json * node value. DO NOT FREE! */

    struct jprint_pattern *pattern; /* pointer to the pattern that matched. DO NOT FREE! */
    struct jprint_match *next; /* next match found */
};

/*
 * jprint_pattern - struct for a linked list of patterns requested, held in
 * struct jprint
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
    bool substrings_okay;			/* -s used, matching substrings okay */
    bool use_regexps;				/* -g used, allow grep-like regexps */
    bool explicit_regexp;			/* -G used, will not allow -Y */
    bool count_only;				/* -c used, only show count */
    bool print_entire_file;			/* no name_arg specified */
    uintmax_t max_depth;			/* max depth to traverse set by -m depth */
    bool search_value;				/* -Y used, search for value, not name */

    /* any patterns specified */
    struct jprint_pattern *patterns;		/* linked list of patterns specified */
    uintmax_t number_of_patterns;		/* patterns specified */
    uintmax_t total_matches;			/* total matches of all patterns (name_args) */
};

/* functions */

/* to free the entire struct jprint */
void free_jprint(struct jprint **jprint);

/* patterns list in struct jprint */
struct jprint_pattern *add_jprint_pattern(struct jprint *jprint, bool use_regexp, bool use_substrings, char *str);
void free_jprint_patterns_list(struct jprint *jprint);

/* matches found of each pattern */
struct jprint_match *add_jprint_match(struct jprint *jprint, struct jprint_pattern *pattern,
	struct json *node_name, struct json *node_value, char *value, uintmax_t level, bool string, enum item_type type);
void free_jprint_matches_list(struct jprint_pattern *pattern);

/* for finding matches and printing them */
void jprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, ...);
void vjprint_json_search(struct jprint *jprint, struct json *node, bool is_value, unsigned int depth, va_list ap);
void jprint_json_tree_search(struct jprint *jprint, struct json *node, unsigned int max_depth, ...);
void jprint_json_tree_walk(struct jprint *jprint, struct json *node, bool is_value, unsigned int max_depth, unsigned int depth,
		void (*vcallback)(struct jprint *, struct json *, bool, unsigned int, va_list), va_list ap);
void jprint_print_matches(struct jprint *jprint);
bool jprint_print_count(struct jprint *jprint);
void jprint_print_final_comma(struct jprint *jprint);
void jprint_print_brace(struct jprint *jprint, bool open);
void jprint_print_match(struct jprint *jprint, struct jprint_pattern *pattern, struct jprint_match *match);

/* sanity checks on environment for specific options */
FILE *jprint_sanity_chks(struct jprint *jprint, char const *program, int *argc, char ***argv,
	char const *tool_path, char const *tool_args);

#endif /* !defined INCLUDE_JPRINT_H */
