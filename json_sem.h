/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
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
 * Share and vomit on the JSON spec! :-)
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
 * definitions
 */
#define INF (0)			/* special max value for no limit */
#define INF_DEPTH (UINT_MAX)	/* no depth */


 /*
  * JSON semantic count error
  */
struct json_cnt_err
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
    struct json *node;		/* JSON parse node in question or NULL */
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
    enum item_type type;	/* type of JSON node */
    unsigned int min;		/* minimum allowed count */
    unsigned int max;		/* maximum allowed count, 0 ==> infinite */
    unsigned int count;		/* number of times this JSON semantic was matched */
    size_t name_len;		/* length of name_str, not including final NUL or 0 */
    bool (* validate)(struct json *node,
		      unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err);
				/* JSON parse tree node validator, or NULL */
    char *name;			/* if type == JTYPE_MEMBER, match decoded name or NULL */
};


/*
 * global variables
 */


/*
 * external function declarations
 */
extern struct json_sem_val_err *werr_sem_val(int val_err, struct json *node, unsigned int depth,
					     struct json_sem *sem, char const *name, char const *fmt, ...) \
	__attribute__((format(printf, 6, 7)));		/* 6=format 7=params */


#endif /* INCLUDE_JSON_SEM_H */
