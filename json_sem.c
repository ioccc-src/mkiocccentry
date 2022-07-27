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


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <limits.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * json_sem - JSON semantics support
 */
#include "json_sem.h"


/*
 * static variables
 */
/* NULL pointer error */
static struct json_sem_val_err sem_null_ptr = {
    NULL,	/* JSON parse node in question or NULL */
    UINT_MAX,	/* JSON parse tree node depth or UINT_MAX */
    NULL,	/* semantic node in question or NULL */
    INT_MAX,	/* validate function specific error code */
		/* INT_MAX ==> static error, 0 ==> not an error */
    "NULL pointer given to werr_sem_val",	/* diagnostic message or NULL */
    false	/* true ==> struct json_sem_val_err was malloced */
		/* false ==> this is a static struct json_sem_val_err */
};
/* calloc failure */
static struct json_sem_val_err sem_calloc_err = {
    NULL,	/* JSON parse node in question or NULL */
    UINT_MAX,	/* JSON parse tree node depth or UINT_MAX */
    NULL,	/* semantic node in question or NULL */
    INT_MAX,	/* validate function specific error code */
		/* INT_MAX ==> static error, 0 ==> not an error */
    "calloc failure",	/* diagnostic message or NULL */
    false	/* true ==> struct json_sem_val_err was malloced */
		/* false ==> this is a static struct json_sem_val_err */
};
/* strdup failure */
static struct json_sem_val_err sem_strdup_err = {
    NULL,	/* JSON parse node in question or NULL */
    UINT_MAX,	/* JSON parse tree node depth or UINT_MAX */
    NULL,	/* semantic node in question or NULL */
    INT_MAX,	/* validate function specific error code */
		/* INT_MAX ==> static error, 0 ==> not an error */
    "strdup failure",	/* diagnostic message or NULL */
    false	/* true ==> struct json_sem_val_err was malloced */
		/* false ==> this is a static struct json_sem_val_err */
};


/*
 * werr_sem_val - form a struct json_sem_val_err with an error message string
 *
 * This function fills out a struct json_sem_val_err containing JSON validation error
 * information about a failure to validate a JSON node's semantic context.
 *
 * Callers should examine the malloced element of the structure returned in determine
 * if the return value should be freed.
 *
 * given:
 *      val_err         validate function specific error code, 0 ==> not an error
 *	node		JSON parse node in question or NULL (OK to be NULL)
 *	depth		JSON parse tree node depth
 *	sem		semantic node in question or NULL (OK to be NULL)
 *      name            name of function issuing the warning (must NOT be NULL)
 *      fmt             format of the warning (must NOT be NULL)
 *      ...             optional format args
 *
 * returns:
 *	pointer to a struct json_sem_val_err containing JSON validation error information
 *	This may be a pointer to a malloced struct json_sem_val_err (ret->malloced == true),
 *	or a pointer to a static struct json_sem_val_err (ret->malloced == false).
 *
 * NOTE: This function will never return NULL.
 *
 * NOTE: The ret->diagnostic will never be set to NULL.
 *
 * NOTE: In case of calloc or strdup error, or if name is NULL, or if fmt is NULL,
 *	 then ret->malloced will be set to false and a pointer to a static
 *	 struct json_sem_val_err will be returned.
 */
struct json_sem_val_err *
werr_sem_val(int val_err, struct json *node, unsigned int depth, struct json_sem *sem,
	     char const *name, char const *fmt, ...)
{
    va_list ap;					/* variable argument list */
    struct json_sem_val_err *ret = NULL;	/* malloced return value */
    char msg[BUFSIZ+1];				/* vsnwerr() message buffer */
    char *diagnostic = NULL;			/* pointer to duplicated message buffer */

    /*
     * firewall
     */
    if (name == NULL || fmt == NULL) {
	/* report NULL pointer error */
	return &sem_null_ptr;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * copy a werr() message (via vsnwerr()) into the static buffer
     */
    msg[0] = '\0';	/* paranoia */
    msg[BUFSIZ] = '\0';	/* paranoia */
    vsnwerr(val_err, msg, BUFSIZ, name, fmt, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * duplicate the error message
     */
    diagnostic = strdup(msg);
    if (diagnostic == NULL) {
	/* report strdup error */
	return &sem_strdup_err;
    }

    /*
     * malloc the return value
     */
    ret = calloc(1, sizeof(struct json_sem_val_err));
    if (ret == NULL) {
	/* report calloc error */
	return &sem_calloc_err;
    }

    /*
     * fill the json_sem_val_err return value
     */
    ret->node = node;
    ret->depth = depth;
    ret->sem = sem;
    ret->val_err = val_err;
    ret->diagnostic = diagnostic;
    ret->malloced = true;

    /*
     * return pointer to new json_sem_val_err
     */
    return ret;
}
