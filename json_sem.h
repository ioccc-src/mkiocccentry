/*
 * json_sem - JSON semantics support
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
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
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_JSON_SEM_H)
#    define  INCLUDE_JSON_SEM_H


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
 * definitions
 */
#define INF (0)			/* special max value for no limit */
#define INF_DEPTH (UINT_MAX)	/* no depth */


 /*
  * JSON semantic count error
  */
struct json_sem_cnt_err
{
    struct json_sem *sem;	/* semantic node in question or NULL (unknown_node == true) */
    unsigned int count;		/* number of times this JSON semantic was matched */
    bool bad_min;		/* true ==> JSON semantic node count under minimum */
    bool bad_max;		/* true ==> JSON semantic node count over maximum */
    bool unknown_node;		/* true ==> JSON node is not known to JSON semantics */
    char *diagnostic;		/* diagnostic message or NULL */
};

/*
 * JSON semantic validation error
 *
 * When validate() returns false, that validating function
 * records information about the validation error in this form.
 */
struct json_sem_val_err
{
    struct json const *node;	/* JSON parse node in question or NULL */
    unsigned int depth;		/* JSON parse tree node depth or UINT_MAX */
    struct json_sem *sem;	/* semantic node in question or NULL */
    int val_err;		/* validate function specific error code */
				/* INT_MAX ==> static error, 0 ==> not an error */
    char *diagnostic;		/* diagnostic message or NULL */
    bool malloced;		/* true ==> struct json_sem_val_err was malloced */
				/* false ==> this is a static struct json_sem_val_err */
};

/*
 * JSON semantic node
 *
 * An array of JSON semantics may describe the what is required
 * or allowed in a given valid JSON document.
 */
struct json_sem
{
    unsigned int depth;		/* JSON parse tree node depth */
    enum item_type type;	/* type of JSON node, JTYPE_UNSET ==> end of table */
    unsigned int min;		/* minimum allowed count */
    unsigned int max;		/* maximum allowed count, 0 ==> infinite */
    unsigned int count;		/* number of times this JSON semantic was matched */
    size_t name_len;		/* length of name_str, not including final NUL or 0 */
    bool (* validate)(struct json const *node,
		      unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
				/* JSON parse tree node validator, or NULL */
    char *name;			/* if type == JTYPE_MEMBER, match decoded name or NULL */
};


/*
 * str_or_null - report if a JSON JTYPE_MEMBER value is a valid JSON_STRING or a valid JSON_NULL
 *
 * Assuming valid == true, then if is_null == true, then str will be NULL else if is_null == false,
 * then str will be non-NULL.
 *
 * When valid == false, then is_null and str have no meaning.
 */
struct str_or_null {
    bool valid;		/* true == JSON JTYPE_MEMBER value was is valid JSON_STRING or a valid JSON_NULL */
    bool is_null;	/* true ==> JTYPE_MEMBER value is valid JSON_NULL, false ==> is valid JSON_STRING */
    char *str;		/* decoded value string from JSON member or NULL */
};


/*
 * external function declarations
 */
extern struct json_sem_val_err *werr_sem_val(int val_err, struct json const *node, unsigned int depth,
					     struct json_sem *sem, char const *name, char const *fmt, ...) \
	__attribute__((format(printf, 6, 7)));		/* 6=format 7=params */
extern struct json_sem_val_err *werrp_sem_val(int val_err, struct json const *node, unsigned int depth,
					      struct json_sem *sem, char const *name, char const *fmt, ...) \
	__attribute__((format(printf, 6, 7)));		/* 6=format 7=params */
extern bool sem_chk_null_args(struct json const *node, unsigned int depth, struct json_sem *sem,
			      char const *name, struct json_sem_val_err **val_err);
extern bool sem_node_valid_converted(struct json const *node, unsigned int depth, struct json_sem *sem,
				     char const *name, struct json_sem_val_err **val_err);
extern struct json *sem_member_name(struct json const *node, unsigned int depth, struct json_sem *sem,
				    char const *name, struct json_sem_val_err **val_err);
extern struct json *sem_member_value(struct json const *node, unsigned int depth, struct json_sem *sem,
				     char const *name, struct json_sem_val_err **val_err);
extern char *sem_member_name_decoded_str(struct json const *node, unsigned int depth, struct json_sem *sem,
				         char const *name, struct json_sem_val_err **val_err);
extern char *sem_member_value_decoded_str(struct json const *node, unsigned int depth, struct json_sem *sem,
				          char const *name, struct json_sem_val_err **val_err);
extern bool *sem_member_value_bool(struct json const *node, unsigned int depth, struct json_sem *sem,
			           char const *name, struct json_sem_val_err **val_err);
extern struct str_or_null sem_member_value_str_or_null(struct json const *node, unsigned int depth,
						       struct json_sem *sem,
						       char const *name, struct json_sem_val_err **val_err);
extern int *sem_member_value_int(struct json const *node, unsigned int depth, struct json_sem *sem,
			         char const *name, struct json_sem_val_err **val_err);
extern time_t *sem_member_value_time_t(struct json const *node, unsigned int depth, struct json_sem *sem,
				       char const *name, struct json_sem_val_err **val_err);
extern struct json *sem_node_parent(struct json const *node, unsigned int depth, struct json_sem *sem,
				    char const *name, struct json_sem_val_err **val_err);
extern struct json *sem_object_find_name(struct json const *node, unsigned int depth, struct json_sem *sem,
				         char const *name, struct json_sem_val_err **val_err,
				         char const *memname);
extern void json_sem_zero_count(struct json_sem *sem, int count);
extern unsigned int json_sem_check(struct json *node, unsigned int max_depth, struct json_sem *sem, int count,
				   struct dyn_array **pcnt_err, struct dyn_array **pval_err);


#endif /* INCLUDE_JSON_SEM_H */
