/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_util - general JSON parser utility support functions
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * This JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and enjoy vomiting on the JSON spec! :-)
 */


#if !defined(INCLUDE_JSON_UTIL_H)
#    define  INCLUDE_JSON_UTIL_H


/*
 * dyn_array - dynamic array facility
 */
#include "dyn_array.h"

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


/*
 * global variables
 */
extern int json_verbosity_level;	/* print json debug messages <= json_verbosity_level in json_dbg(), json_vdbg() */


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
extern bool json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
				    char const *tail);
extern char const *json_type_name(enum item_type type);
extern char const *json_item_type_name(struct json *node);
extern void json_free(struct json *node, unsigned int depth, ...);
extern void vjson_free(struct json *node, unsigned int depth, va_list ap);
extern void json_fprint(struct json *node, unsigned int depth, ...);
extern void vjson_fprint(struct json *node, unsigned int depth, va_list ap);
extern void json_tree_print(struct json *node, unsigned int max_depth, ...);
extern void json_dbg_tree_print(int json_dbg_lvl, char const *name, struct json *tree, unsigned int max_depth);
extern void json_tree_free(struct json *node, unsigned int max_depth, ...);
extern void json_tree_walk(struct json *node, unsigned int max_depth,
			   void (*vcallback)(struct json *, unsigned int, va_list), ...);
extern void vjson_tree_walk(struct json *node, unsigned int max_depth, unsigned int depth,
			    void (*vcallback)(struct json *, unsigned int, va_list), va_list ap);


#endif /* INCLUDE_JSON_UTIL_H */
