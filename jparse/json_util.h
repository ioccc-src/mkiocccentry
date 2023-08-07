/*
 * json_util - general JSON parser utility support functions
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
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


#if !defined(INCLUDE_JSON_UTIL_H)
#    define  INCLUDE_JSON_UTIL_H


/*
 * dyn_array - dynamic array facility
 */
#include "../dyn_array/dyn_array.h"

/*
 * util - utilities and macros
 */
#include "util.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"


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
#define JSON_DBG_LEVEL	    (JSON_DBG_LOW)  /* default JSON debugging level json_verbosity_level */

/* for json tools jval and jnamval */
#define JSON_UTIL_CMP_OP_NONE	    (0)
#define JSON_UTIL_CMP_EQ	    (1)
#define JSON_UTIL_CMP_LT	    (2)
#define JSON_UTIL_CMP_LE	    (3)
#define JSON_UTIL_CMP_GT	    (4)
#define JSON_UTIL_CMP_GE	    (5)

/* for -F with jfmt, jval and jnamval */
enum output_format {
    JSON_FMT_TTY = 0,		    /* tty (default): when output is to a TTY, use colour, otherwise use simple */
    JSON_FMT_SIMPLE = 1,	    /* simple: each line has one JSON level determined by '[]'s and '{}'s */
    JSON_FMT_COLOUR = 2,	    /* coloured format: syntax highlighting */
    JSON_FMT_1LINE = 3,		    /* one line output: one line for all output */
    JSON_FMT_NOWS = 4,		    /* nows (no whitespace) output: one line for all output with no extra whitespace */
};

/* jval and jnamval -t types */
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

/* structures for jval and jnamval */

/* for comparison of numbers / strings - options -n and -S */
struct json_util_cmp_op
{
    struct json_number *number;	    /* for -n as signed number */
    struct json_string *string;	    /* for -S str */

    bool is_string;	    /* true if -S */
    bool is_number;	    /* true if -n */
    uintmax_t op;	    /* the operation - see JVAL_CMP macros above */

    struct json_util_cmp_op *next;	/* next in the list */
};


/* structures common to jval and jnamval  */
struct json_util_name_val
{
    /* string related options */
    bool encode_strings;			/* -e used */
    bool quote_strings;				/* -Q used */

    /* printing related options */
    bool print_decoded;				/* -D used */

    bool count_only;				/* -c used, only show count */
    bool count_and_show_values;			/* -C used, show count and values */

    bool string_cmp_used;			/* for -S */
    struct json_util_cmp_op *string_cmp;		/* for -S str */
    bool num_cmp_used;				/* for -n */
    struct json_util_cmp_op *num_cmp;		/* for -n num */

    /* search / matching related */
    bool invert_matches;			/* -i used */
    bool match_json_types_specified;		/* -t used */
    uintmax_t match_json_types;			/* -t types */
    bool print_json_types_specified;		/* -P used */
    uintmax_t print_json_types;			/* -P types */
    bool match_substrings;			/* -s used, match substrings */
    bool use_regexps;				/* -g used, allow grep-like regexps */

    bool ignore_case;				/* true if -f, case-insensitive */
    bool match_decoded;				/* -d used - match decoded */
    bool arg_specified;				/* true if an arg was specified */
    uintmax_t total_matches;			/* for -c */
};

/* structures common to jfmt, jval and jnamval */

/* number ranges for the options -l, -n and -n of jfmt, jval and jnamval */
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


/*
 * json_util - struct related to the json utils jfmt, jval and jnamval
 */
struct json_util
{
    /* JSON file related */
    bool is_stdin;				/* reading from stdin */
    FILE *json_file;				/* FILE * to json file */
    char *file_contents;			/* file contents */
    char *json_file_path;			/* path to JSON file to read */

    /* out file related to -o */
    char *outfile_path;				/* -o file path */
    FILE *outfile;				/* FILE * of -o ofile */
    bool outfile_not_stdout;			/* -o used without stdout */

    /* level constraints */
    bool levels_constrained;			/* -l specified */
    struct json_util_number json_util_levels;		/* -l level specified */

    bool print_json_levels;			/* -L specified */
    uintmax_t num_level_spaces;			/* number of spaces or tab for -L */
    bool print_level_tab;			/* -L tab option */

    bool indent_levels;				/* -I specified */
    uintmax_t indent_spaces;			/* -I specified */
    bool indent_tab;				/* -I <num>[{t|s}] specified */

    bool format_output_changed;			/* -F output_format used */
    enum output_format format;			/* for -F output_format */

    uintmax_t max_depth;			/* max depth to traverse set by -m depth */
    struct json *json_tree;			/* json tree if valid merely as a convenience */
};
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

/* for number range option -l */
bool json_util_parse_number_range(const char *option, char *optarg, bool allow_negative, struct json_util_number *number);
bool json_util_number_in_range(intmax_t number, intmax_t total_matches, struct json_util_number *range);
/* for -L option */
void json_util_parse_st_level_option(char *optarg, uintmax_t *num_level_spaces, bool *print_level_tab);
/* for -I option */
void json_util_parse_st_indent_option(char *optarg, uintmax_t *indent_level, bool *indent_tab);
/* for -S and -n */
struct json_util_cmp_op *json_util_parse_cmp_op(struct json_util_name_val *json_util_name_val, const char *option, char *optarg);
/* for -F option */
enum output_format parse_json_util_format(struct json_util *json_util, char const *name, char const *optarg);
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


#endif /* INCLUDE_JSON_UTIL_H */
