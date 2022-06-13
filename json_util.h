/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_util - general JSON parser utility support functions
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * This is currently being worked on by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 * and
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This is very much a work in progress! See jparse.h, jparse.l and jparse.y,
 * chk_util.h, chk_util.c as well as json_parse.h and json_parse.c.
 */


#if !defined(INCLUDE_JSON_UTIL_H)
#    define  INCLUDE_JSON_UTIL_H


/*
 * dyn_array - dynamic array facility
 */
#include "dyn_array.h"


/*
 * JSON defines
 */

/*
 * JSON parser related definitions and structures
 */
#define JSON_CHUNK (16)			/* number of pointers to allocate at a time in dynamic array */
#define JSON_DEFAULT_MAX_DEPTH (256)	/* a sane parse tree depth to use */
#define JSON_INFINITE_DEPTH (-1)	/* no limit on parse tree depth to walk */


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

/*
 * JSON warn / error codes
 *
 * Codes 0 - 199 are reserved for special purposes so all normal codes should be
 * >= JSON_CODE_MIN && <= JSON_CODE_MAX via the JSON_CODE macro.
 *
 * NOTE: The reason 200 codes are reserved is because it's more than enough ever
 * and we don't want to have to ever change the codes after the parser and
 * checkers are complete as this would cause problems for the tools as well as
 * the test suites. Previously the range was 0 - 99 and although this is also
 * probably more than enough we want to be sure that there is never a problem
 * and we cannot imagine how 200 codes will ever not be a large enough range but
 * if the use of reserved codes change this might not be the case for just 100
 * (unlikely though that is).
 */
/* reserved code: all normal codes should be >= JSON_CODE_MIN && <= JSON_CODE_MAX via JSON_CODE macro */
#define JSON_CODE_RESERVED_MIN (0)
/* reserved code: all normal codes should be >= JSON_CODE_MIN && <= JSON_CODE_MAX via JSON_CODE macro */
#define JSON_CODE_RESERVED_MAX (199)
/* based on minimum reserved code, form an invalid json code number */
#define JSON_CODE_INVALID (JSON_CODE_RESERVED_MIN - 1)
/*
 * The minimum code for jwarn() is the JSON_CODE_RESERVED_MAX (currently 199) + 1.
 * However this does not mean that calls to jwarn() cannot use <= the
 * JSON_CODE_RESERVED_MAX: it's just for special purposes e.g. codes that are
 * used in more than one location.
 */
#define JSON_CODE_MIN (1+JSON_CODE_RESERVED_MAX)
/* The maximum json code for jwarn()/jerr(). This was arbitrarily selected. */
#define JSON_CODE_MAX (9999)
/* the number of unreserved JSON codes for jwarn()/jerr(): the max - the min + 1 */
#define NUM_UNRESERVED_JSON_CODES (JSON_CODE_MAX-JSON_CODE_MIN)
/*
 * To distinguish that a number is a JSON warn/error code rather than any other
 * type of number we use these macros.
 */
#define JSON_CODE(x) ((x)+JSON_CODE_RESERVED_MAX)
/* reserved JSON code */
#define JSON_CODE_RESERVED(x) (x)

/*
 * JSON warning codes to ignore
 *
 * When a tool is given command line arguments of the form:
 *
 *	.. -W 123 -W 1345 -W 56 ...
 *
 * this means the tool will ignore {JSON-0123}, {JSON-1345} and {JSON-0056}.
 * The ignore_json_code_set[] table holds the JSON codes to ignore.
 */
#define JSON_CODE_IGNORE_CHUNK (64)	/* number of codes to calloc or realloc at a time */

struct ignore_json_code
{
    int next_free;	/* the index of the next allowed but free JSON error code */
    int alloc;		/* number of JSON error codes allocated */
    int *code;		/* pointer to the allocated list of codes, or NULL (not allocated) */
};


/*
 * global variables
 */
extern int json_verbosity_level;	/* print json debug messages <= json_verbosity_level in json_dbg(), json_vdbg() */

/*
 * JSON warn/error code struct, variables, constants and macros for the jwarn()
 * and jerr() functions. Functions are prototyped later in the file.
 */
extern bool show_full_json_warnings;


/*
 * external function declarations
 */
extern bool json_dbg_allowed(int json_dbg_lvl);
extern bool json_warn_allowed(int code);
extern bool json_err_allowed(void);
extern bool is_json_code_ignored(int code);
extern void ignore_json_code(int code);
extern bool jwarn(int code, char const *name, char const *filename,
		  char const *line, int line_num, const char *fmt, ...) \
	__attribute__((format(printf, 6, 7)));		/* 6=format 7=params */
extern bool jwarnp(int code, char const *name, char const *filename,
		   char const *line, int line_num, const char *fmt, ...) \
	__attribute__((format(printf, 6, 7)));		/* 6=format 7=params */
extern void jerr(int exitcode, const char *name, char const *filename,
		 char const *line, int line_num, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 6, 7))); /* 6=format 7=params */
extern void jerrp(int exitcode, const char *name,
		  char const *filename, char const *line, int line_num, const char *fmt, ...) \
	__attribute__((noreturn)) __attribute__((format(printf, 6, 7))); /* 6=format 7=params */
extern bool json_dbg(int json_dbg_lvl, char const *name, const char *fmt, ...) \
	__attribute__((format(printf, 3, 4)));		/* 3=format 4=params */
extern bool json_vdbg(int json_dbg_lvl, char const *name, const char *fmt, va_list ap);
extern char const *json_element_type_name(struct json *node);
extern void json_free(struct json *node, int depth, ...);
extern void vjson_free(struct json *node, int depth, va_list ap);
extern void json_fprint(struct json *node, int depth, ...);
extern void vjson_fprint(struct json *node, int depth, va_list ap);
extern void json_tree_print(struct json *node, int max_depth, ...);
extern bool json_dbg_tree_print(int json_dbg_lvl, char const *name, struct json *tree, int max_depth);
extern void json_tree_free(struct json *node, int max_depth, ...);
extern void json_tree_walk(struct json *node, int max_depth,
			   void (*vcallback)(struct json *, int, va_list), ...);
extern void vjson_tree_walk(struct json *node, int max_depth, int depth,
			    void (*vcallback)(struct json *, int, va_list), va_list ap);
extern struct json *json_alloc(enum element_type type);
extern struct json *json_conv_number(char const *ptr, size_t len);
extern struct json *json_conv_number_str(char const *str, size_t *retlen);
extern struct json *json_conv_string(char const *ptr, size_t len, bool quote);
extern struct json *json_conv_string_str(char const *str, size_t *retlen, bool quote);
extern struct json *json_conv_bool(char const *ptr, size_t len);
extern struct json *json_conv_bool_str(char const *str, size_t *retlen);
extern struct json *json_conv_null(char const *ptr, size_t len);
extern struct json *json_conv_null_str(char const *str, size_t *retlen);
extern struct json *json_conv_member(struct json *name, struct json *value);
extern struct json *json_create_object(void);
extern bool json_object_add_member(struct json *node, struct json *member);
extern bool json_object_append_members(struct json *node, struct dyn_array *members);
extern struct json *json_create_array(void);
extern bool json_array_add_value(struct json *node, struct json *value);
extern bool json_array_append_values(struct json *node, struct dyn_array *values);


#endif /* INCLUDE_JSON_UTIL_H */
