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


/*
 * werrp_sem_val - form a struct json_sem_val_err with an error message string with errno details
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
werrp_sem_val(int val_err, struct json *node, unsigned int depth, struct json_sem *sem,
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
     * copy a werr() message (via vsnwerrp()) into the static buffer
     */
    msg[0] = '\0';	/* paranoia */
    msg[BUFSIZ] = '\0';	/* paranoia */
    vsnwerrp(val_err, msg, BUFSIZ, name, fmt, ap);

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


/*
 * sem_chk_null_args - check arguments for NULL
 *
 * Check if critical if args (node, sem, name) are NULL.  Report if a critical arg
 * is NULL via a val_err pointer to address where to place a JSON semantic validation error.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> one or more args are NULL, *val_err if non-NULL is JSON semantic error
 *	false ==> all args are non-NULL
 */
bool
sem_chk_null_args(struct json *node, unsigned int depth, struct json_sem *sem,
	          char const *name, struct json_sem_val_err **val_err)
{
    /*
     * firewall - args
     */
    if (name == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(10, node, depth, sem, "((NULL))", "name is NULL");
	}
	return true;
    }
    if (node == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(11, node, depth, sem, name, "node is NULL");
	}
	return true;
    }
    if (sem == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(12, node, depth, sem, name, "sem is NULL");
	}
	return true;
    }
    return false;
}


/*
 * sem_node_valid_converted - determine if a JSON node is a converted and valid type
 *
 * Given a JSON node, attempt to return the parent of a JSON node.
 * Beyond simply returning the parent node, we inspect the parent
 * node to determine if the parent node was successfully converted
 * into a known JTYPE.
 *
 * Because this call is often made when a function is first given a JSON node,
 * we make a few other JTYPE specific checks such as looking for invalid
 * pointers to NULL.  Where there is an integer length, we check for < 0.
 *
 * For JTYPE_MEMBER, verify that the name is a valid JTYPE_STRING.
 * This valid JTYPE_STRING check is a manual check to avoid potential problems
 * with recursion and loops.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON node is converted and a valid JTYPE
 *	    The val_err arg is ignored
 *	NULL ==> JSON node is not converted, invalid node type, or internal error
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_cnt_err)
 */
bool
sem_node_valid_converted(struct json *node, unsigned int depth, struct json_sem *sem,
		         char const *name, struct json_sem_val_err **val_err)
{
    /*
     * firewall - args
     */
    if (sem_chk_null_args(node, depth, sem, name, val_err) == true) {
	/* sem_chk_null_args() will have set *val_err */
	return false;
    }

    /*
     * validate JSON parse node type and check for not converted
     */
    switch (node->type) {
    case JTYPE_UNSET:   /* JSON item has not been set - must be the value 0 */
	if (val_err != NULL) {
	    *val_err = werr_sem_val(13, node, depth, sem, name, "node is JTYPE_UNSET this type is invalid here");
	}
	return false;
	break;

    case JTYPE_NUMBER:  /* JSON item is number - see struct json_number */
	{
	    struct json_number *item = &(node->item.number);

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(14, node, depth, sem, name, "JTYPE_NUMBER node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_NUMBER specific sanity checks */
	    if (item->as_str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(15, node, depth, sem, name, "JTYPE_NUMBER node: as_str is NULL");
		}
		return false;
	    }
	    if (item->first == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(16, node, depth, sem, name, "JTYPE_NUMBER node: first is NULL");
		}
		return false;
	    }
	}
	break;

    case JTYPE_STRING:  /* JSON item is a string - see struct json_string */
	{
	    struct json_string *item = &(node->item.string);

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(17, node, depth, sem, name, "JTYPE_STRING node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_STRING specific sanity checks */
	    if (item->as_str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(18, node, depth, sem, name, "JTYPE_STRING node: as_str is NULL");
		}
		return false;
	    }
	    if (item->str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(19, node, depth, sem, name, "JTYPE_STRING node: str is NULL");
		}
		return false;
	    }
	}
	break;

    case JTYPE_BOOL:    /* JSON item is a boolean - see struct json_boolean */
	{
	    struct json_boolean *item = &(node->item.boolean);

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(20, node, depth, sem, name, "JTYPE_BOOL node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_BOOL specific sanity checks */
	    if (item->as_str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(21, node, depth, sem, name, "JTYPE_BOOL node: as_str is NULL");
		}
		return false;
	    }
	}
	break;

    case JTYPE_NULL:    /* JSON item is a null - see struct json_null */
	{
	    struct json_null *item = &(node->item.null);

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(22, node, depth, sem, name, "JTYPE_NULL node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_NULL specific sanity checks */
	    if (item->as_str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(23, node, depth, sem, name, "JTYPE_NULL node: as_str is NULL");
		}
		return false;
	    }
	    if (item->value != NULL) {	/* yes, value must be NULL for a converted JTYPE_NULL */
		if (val_err != NULL) {
		    *val_err = werr_sem_val(24, node, depth, sem, name, "JTYPE_NULL node: ironically value is NOT NULL");
		}
		return false;
	    }
	}
	break;

    case JTYPE_MEMBER:  /* JSON item is a member */
	{
	    struct json_member *item = &(node->item.member);
	    struct json *member_name = NULL;		/* name part of JTYPE_MEMBER */
	    struct json_string *member_name_string;	/* name part of JTYPE_MEMBER as JTYPE_STRING */

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(25, node, depth, sem, name, "JTYPE_MEMBER node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_MEMBER specific sanity checks */
	    if (item->name_as_str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(26, node, depth, sem, name, "JTYPE_MEMBER node: name_as_str is NULL");
		}
		return false;
	    }
	    if (item->name_str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(27, node, depth, sem, name, "JTYPE_MEMBER node: name_str is NULL");
		}
		return false;
	    }
	    if (item->name == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(28, node, depth, sem, name, "JTYPE_MEMBER node: name is NULL");
		}
		return false;
	    }
	    if (item->value == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(29, node, depth, sem, name, "JTYPE_MEMBER node: value is NULL");
		}
		return false;
	    }

	    /* specific checks on name of JTYPE_MEMBER */
	    member_name = item->name;
	    if (member_name->type != JTYPE_STRING) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(30, node, depth, sem, name,
					    "JTYPE_MEMBER name node type: %d <%s> != JTYPE_STRING",
					    node->type, json_type_name(node->type));
		}
		return false;
	    }
	    member_name_string = &(member_name->item.string);
	    if (member_name_string->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(31, node, depth, sem, name, "JTYPE_MEMBER name node: converted is false");
		}
		return false;
	    }
	    if (member_name_string->as_str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(32, node, depth, sem, name, "JTYPE_MEMBER name node as_str is NULL");
		}
		return false;
	    }
	    if (member_name_string->str == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(33, node, depth, sem, name, "JTYPE_MEMBER name node str is NULL");
		}
		return false;
	    }
	}
	break;

    case JTYPE_OBJECT:  /* JSON item is a { members } */
	{
	    struct json_object *item = &(node->item.object);

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(34, node, depth, sem, name, "JTYPE_OBJECT node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_OBJECT specific sanity checks */
	    if (item->len < 0) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(35, node, depth, sem, name, "JTYPE_OBJECT node: len: %d < 0",
					    item->len);
		}
		return false;
	    }
	    if (item->set == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(36, node, depth, sem, name, "JTYPE_OBJECT node: set is NULL");
		}
		return false;
	    }
	    if (item->s == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(37, node, depth, sem, name, "JTYPE_OBJECT node: s is NULL");
		}
		return false;
	    }
	}
	break;

    case JTYPE_ARRAY:   /* JSON item is a [ elements ] */
	{
	    struct json_array *item = &(node->item.array);

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(38, node, depth, sem, name, "JTYPE_ARRAY node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_ARRAY specific sanity checks */
	    if (item->len < 0) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(39, node, depth, sem, name, "JTYPE_ARRAY node: len: %d < 0",
					    item->len);
		}
		return false;
	    }
	    if (item->set == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(40, node, depth, sem, name, "JTYPE_ARRAY node: set is NULL");
		}
		return false;
	    }
	    if (item->s == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(41, node, depth, sem, name, "JTYPE_ARRAY node: s is NULL");
		}
		return false;
	    }
	}
	break;

    case JTYPE_ELEMENTS:        /* JSON elements is zero or more JSON values */
	{
	    struct json_elements *item = &(node->item.elements);

	    /* converted check */
	    if (item->converted == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(42, node, depth, sem, name, "JTYPE_ELEMENTS node: converted is false");
		}
		return false;
	    }

	    /* JTYPE_ELEMENTS specific sanity checks */
	    if (item->len < 0) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(43, node, depth, sem, name, "JTYPE_ELEMENTS node: len: %d < 0",
					    item->len);
		}
		return false;
	    }
	    if (item->set == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(44, node, depth, sem, name, "JTYPE_ELEMENTS node: set is NULL");
		}
		return false;
	    }
	    if (item->s == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(45, node, depth, sem, name, "JTYPE_ARRAY node: s is NULL");
		}
		return false;
	    }
	}
	break;

    default:
	if (val_err != NULL) {
	    *val_err = werr_sem_val(46, node, depth, sem, name, "node type is unknown: %d <%s>",
				    node->type, json_type_name(node->type));
	}
	return false;
        break;
    }

    /*
     * node is converted and a valid JTYPE
     */
    return true;
}


/*
 * sem_member_name - return JSON node that is the name of a JTYPE_MEMBER
 *
 * Given a JSON parse node of the JTYPE_MEMBER type, validate that node
 * and return the JSON node that is the name of said JTYPE_MEMBER.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> JSON node that is the name of a JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON error
 *	    If val_err != NULLm then *val_err is JSON semantic validation error (struct json_cnt_err)
 */
struct json *
sem_member_name(struct json *node, unsigned int depth, struct json_sem *sem,
		char const *name, struct json_sem_val_err **val_err)
{
    struct json_member *item = NULL;		/* JSON member */
    struct json *n = NULL;			/* name of JTYPE_MEMBER */
    bool valid = false;				/* true ==> JSON node is converted and valid JTYPE */

    /*
     * firewall - args
     */
    if (sem_chk_null_args(node, depth, sem, name, val_err) == true) {
	/* sem_chk_null_args() will have set *val_err */
	return NULL;
    }

    /*
     * validate JSON parse node type
     */
    valid = sem_node_valid_converted(node, depth, sem, name, val_err);
    if (valid == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return NULL;
    }
    if (node->type != JTYPE_MEMBER) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(47, node, depth, sem, name, "node type %s != JTYPE_MEMBER",
				    json_type_name(node->type));
	}
	return NULL;
    }

    /*
     * firewall - name
     */
    n = item->name;
    if (n == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(48, node, depth, sem, name, "node name is NULL");
	}
	return NULL;
    }

    /*
     * validate JSON parse node name type
     */
    valid = sem_node_valid_converted(n, depth+1, sem, name, val_err);
    if (valid == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return NULL;
    }
    if (n->type != JTYPE_STRING) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(49, node, depth, sem, name, "node name type %s != JTYPE_STRING",
				    json_type_name(n->type));
	}
	return NULL;
    }
    return n;
}


/*
 * sem_member_value - return JSON node that is the value of a JTYPE_MEMBER
 *
 * Given a JSON parse node of the JTYPE_MEMBER type, validate that node
 * and return the JSON node that is the value of said JTYPE_MEMBER.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> JSON node that is the value of a JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON error
 *	    If val_err != NULLm then *val_err is JSON semantic validation error (struct json_cnt_err)
 */
struct json *
sem_member_value(struct json *node, unsigned int depth, struct json_sem *sem,
		 char const *name, struct json_sem_val_err **val_err)
{
    struct json_member *item = NULL;		/* JSON member */
    struct json *n = NULL;			/* name of JTYPE_MEMBER */
    bool valid = false;				/* true ==> JSON node is converted and valid JTYPE */

    /*
     * firewall - args
     */
    if (sem_chk_null_args(node, depth, sem, name, val_err) == true) {
	/* sem_chk_null_args() will have set *val_err */
	return NULL;
    }

    /*
     * validate JSON parse node type
     */
    valid = sem_node_valid_converted(node, depth, sem, name, val_err);
    if (valid == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return NULL;
    }
    if (node->type != JTYPE_MEMBER) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(50, node, depth, sem, name, "node type %s != JTYPE_MEMBER",
				    json_type_name(node->type));
	}
	return NULL;
    }

    /*
     * firewall - name
     */
    n = item->value;
    if (n == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(51, node, depth, sem, name, "node name is NULL");
	}
	return NULL;
    }
    valid = sem_node_valid_converted(n, depth+1, sem, name, val_err);
    if (valid == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return NULL;
    }
    return n;
}


/*
 * sem_member_name_decoded_str - return the JSON decoded name string from a JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the name of the JSON member
 * and if it is a JTYPE_STRING (JSON string), return the name's JSON decoded string
 * or return NULL on error or invalid input.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> decoded JTYPE_STRING from the name part of JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON conversion error
 *	    If val_err != NULLm then *val_err is JSON semantic validation error (struct json_cnt_err)
 */
char *
sem_member_name_decoded_str(struct json *node, unsigned int depth, struct json_sem *sem,
			    char const *name, struct json_sem_val_err **val_err)
{
    struct json *n = NULL;			/* name of JTYPE_MEMBER */
    struct json_string *istr = NULL;		/* JTYPE_MEMBER name as JTYPE_STRING */
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */

    /*
     * obtain JTYPE_MEMBER name
     *
     * NOTE: The sem_member_name() call checks args via sem_chk_null_args().
     * NOTE: The sem_member_name() call verifies that node is of type JTYPE_MEMBER.
     */
    n = sem_member_name(node, depth, sem, name, val_err);
    if (n == NULL) {
	/* sem_member_name() will have set *val_err */
	return NULL;
    }

    /*
     * validate JSON parse node name type
     */
    if (n->type != JTYPE_STRING) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(52, node, depth, sem, name, "node name type %s != JTYPE_STRING",
				    json_type_name(n->type));
	}
	return NULL;
    }
    istr = &(n->item.string);
    if (istr->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(53, node, depth, sem, name, "node name JTYPE_STRING converted is false");
	}
	return NULL;
    }

    /*
     * firewall - decoded JSON string
     */
    str = istr->str;
    if (str == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(54, node, depth, sem, name, "node name decoded JSON string is NULL");
	}
	return NULL;
    }

    /*
     * case success: return decoded JSON string
     */
    return str;
}


/*
 * sem_member_value_decoded_str - return the JSON decoded value string from a JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the value of the JSON member
 * and if it is a JTYPE_STRING (JSON string), return the value's JSON decoded string
 * or return NULL on error or invalid input.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> decoded JTYPE_STRING from the value part of JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON conversion error
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_sem_val_err)
 */
char *
sem_member_value_decoded_str(struct json *node, unsigned int depth, struct json_sem *sem,
			     char const *name, struct json_sem_val_err **val_err)
{
    struct json *value = NULL;			/* value of JTYPE_MEMBER */
    struct json_string *istr = NULL;		/* JTYPE_MEMBER value as JTYPE_STRING */
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */

    /*
     * obtain JTYPE_MEMBER value
     *
     * NOTE: The sem_member_value() call checks args via sem_chk_null_args().
     * NOTE: The sem_member_value() call verifies that node is of type JTYPE_MEMBER.
     */
    value = sem_member_value(node, depth, sem, name, val_err);
    if (value == NULL) {
	/* sem_member_value() will have set *val_err */
	return NULL;
    }

    /*
     * validate JSON parse node value type
     */
    if (value->type != JTYPE_STRING) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(55, node, depth, sem, name, "node value type %s != JTYPE_STRING",
				    json_type_name(value->type));
	}
	return NULL;
    }
    istr = &(value->item.string);
    if (istr->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(56, node, depth, sem, name, "node value JTYPE_STRING converted is false");
	}
	return NULL;
    }

    /*
     * firewall - decoded JSON string
     */
    str = istr->str;
    if (str == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(57, node, depth, sem, name, "node value decoded JSON string is NULL");
	}
	return NULL;
    }

    /*
     * case success: return decoded JSON string
     */
    return str;
}


/*
 * sem_member_value_bool - return the JSON boolean from a JTYPE_BOOL from a JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the value of the JSON member
 * and if it is a JTYPE_BOOL (JSON boolean), return a pointer to the JSON boolean
 * or return NULL on error or invalid input.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> decoded JTYPE_BOOL from the value part of JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON conversion error
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_sem_val_err)
 */
bool *
sem_member_value_bool(struct json *node, unsigned int depth, struct json_sem *sem,
		      char const *name, struct json_sem_val_err **val_err)
{
    struct json *value = NULL;			/* value of JTYPE_MEMBER */
    struct json_boolean *ibool = NULL;		/* JTYPE_MEMBER value as JTYPE_BOOL */

    /*
     * obtain JTYPE_MEMBER value
     *
     * NOTE: The sem_member_value() call checks args via sem_chk_null_args().
     * NOTE: The sem_member_value() call verifies that node is of type JTYPE_MEMBER.
     */
    value = sem_member_value(node, depth, sem, name, val_err);
    if (value == NULL) {
	/* sem_member_value() will have set *val_err */
	return NULL;
    }

    /*
     * validate JSON parse node value type
     */
    if (value->type != JTYPE_BOOL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(58, node, depth, sem, name, "node value type %s != JTYPE_BOOL",
				    json_type_name(value->type));
	}
	return NULL;
    }
    ibool = &(value->item.boolean);
    if (ibool->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(59, node, depth, sem, name, "node value JTYPE_BOOL converted is false");
	}
	return NULL;
    }

    /*
     * case success: return JSON boolean
     */
    return &(ibool->value);
}


/*
 * sem_member_value_str_or_null - return JSON decoded value string or JSON null from a JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the value of the JSON member
 * and if it is a JTYPE_STRING (JSON string), return the value's JSON decoded string,
 * and if it is a JTYPE_NULL (JSON null), indicate that value is JSON null.
 *
 * The return structure gives you a  way to distinguish between JTYPE_STRING and JTYPE_NULL:
 *
 *   When value is a JTYPE_STRING (JSON string):
 *
 *	ret.valid == true
 *	ret.is_null == false
 *	ret.str is the JSON decoded value string
 *
 *   When value is a JTYPE_NULL (JSON null):
 *
 *	ret.valid == true
 *	ret.is_null == true
 *	ret.str is NULL
 *
 *   When there is an error or invalid input:
 *
 *	ret.valid == false
 *	ret.is_null == false
 *	ret.str is NULL
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	struct str_or_null (see above)
 */
struct str_or_null
sem_member_value_str_or_null(struct json *node, unsigned int depth, struct json_sem *sem,
			     char const *name, struct json_sem_val_err **val_err)
{
    struct json *value = NULL;			/* value of JTYPE_MEMBER */
    struct json_string *istr = NULL;		/* JTYPE_MEMBER value as JTYPE_STRING */
    struct json_null *inull = NULL;		/* JTYPE_MEMBER value as JTYPE_NULL */
    struct str_or_null ret = {			/* return value - initialized to invalid */
	false, false, NULL
    };

    /*
     * obtain JTYPE_MEMBER value
     *
     * NOTE: The sem_member_value() call checks args via sem_chk_null_args().
     * NOTE: The sem_member_value() call verifies that node is of type JTYPE_MEMBER.
     */
    value = sem_member_value(node, depth, sem, name, val_err);
    if (value == NULL) {
	/* sem_member_value() will have set *val_err */
	return ret;
    }

    /*
     * process based on value type
     */
    switch (value->type) {

    /*
     * case: value is JTYPE_STRING
     */
    case JTYPE_STRING:

	/*
	 * firewall - value
	 */
	istr = &(value->item.string);
	if (istr->converted == false) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(60, node, depth, sem, name, "node value JTYPE_STRING converted is false");
	    }
	    return ret;
	}

	/*
	 * firewall - decoded JSON string
	 */
	ret.str = istr->str;
	if (ret.str == NULL) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(61, node, depth, sem, name, "node value decoded JSON string is NULL");
	    }
	    return ret;
	}
	ret.valid = true;
	break;

    /*
     * case: value is JTYPE_NULL
     */
    case JTYPE_NULL:

	/*
	 * firewall - value
	 */
	inull = &(value->item.null);
	if (inull->converted == false) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(62, node, depth, sem, name, "node value JTYPE_NULL converted is false");
	    }
	    return ret;
	}
	ret.is_null = true;
	ret.valid = true;
	break;

    /*
     * case: everything else
     */
    default:
	if (val_err != NULL) {
	    *val_err = werr_sem_val(63, node, depth, sem, name, "node value type %s != JTYPE_STRING and != JTYPE_NULL",
				    json_type_name(value->type));
	}
	break;
    }

    /*
     * return result
     */
    return ret;
}


/*
 * sem_member_value_int - return pointer to int from a JSON number value of JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the value of the JSON member
 * and if it is a JTYPE_NUMBER (JSON number), return a pointer to a converted int
 * or return NULL on error or invalid input.
 *
 * If the JTYPE_NUMBER (JSON number) cannot be returned as an int, NULL is returned.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> decoded JTYPE_NUMBER as an int from the value part of JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON conversion error
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_sem_val_err)
 */
int *
sem_member_value_int(struct json *node, unsigned int depth, struct json_sem *sem,
		     char const *name, struct json_sem_val_err **val_err)
{
    struct json *value = NULL;			/* value of JTYPE_MEMBER */
    struct json_number *inum = NULL;		/* JTYPE_MEMBER value as JTYPE_NUMBER */

    /*
     * obtain JTYPE_MEMBER value
     *
     * NOTE: The sem_member_value() call checks args via sem_chk_null_args().
     * NOTE: The sem_member_value() call verifies that node is of type JTYPE_MEMBER.
     */
    value = sem_member_value(node, depth, sem, name, val_err);
    if (value == NULL) {
	/* sem_member_value() will have set *val_err */
	return NULL;
    }

    /*
     * validate JSON parse node value type
     */
    if (value->type != JTYPE_NUMBER) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(64, node, depth, sem, name, "node value type %s != JTYPE_BOOL",
				    json_type_name(value->type));
	}
	return NULL;
    }
    inum = &(value->item.number);
    if (inum->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(65, node, depth, sem, name, "node value JTYPE_NUMBER converted is false");
	}
	return NULL;
    }

    /*
     * validate JTYPE_NUMBER was able to be converted into an int
     */
    if (inum->int_sized == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(66, node, depth, sem, name, "node value JTYPE_NUMBER was unable to convert to an int");
	}
	return NULL;
    }

    /*
     * case success: return JSON boolean
     */
    return &(inum->as_int);
}


/*
 * sem_member_value_time_t - return pointer to time_t from a JSON number value of JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the value of the JSON member
 * and if it is a JTYPE_NUMBER (JSON number), return a pointer to a time_t
 * or return NULL on error or invalid input.
 *
 * If the JTYPE_NUMBER (JSON number) cannot be returned as an time_t, NULL is returned.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> decoded JTYPE_NUMBER as an time_t from the value part of JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON conversion error
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_sem_val_err)
 */
time_t *
sem_member_value_time_t(struct json *node, unsigned int depth, struct json_sem *sem,
		        char const *name, struct json_sem_val_err **val_err)
{
    struct json *value = NULL;			/* value of JTYPE_MEMBER */
    struct json_number *inum = NULL;		/* JTYPE_MEMBER value as JTYPE_NUMBER */
    time_t *ret;				/* pointer converted time_t to return */

    /*
     * obtain JTYPE_MEMBER value
     *
     * NOTE: The sem_member_value() call checks args via sem_chk_null_args().
     * NOTE: The sem_member_value() call verifies that node is of type JTYPE_MEMBER.
     */
    value = sem_member_value(node, depth, sem, name, val_err);
    if (value == NULL) {
	/* sem_member_value() will have set *val_err */
	return NULL;
    }

    /*
     * validate JSON parse node value type
     */
    if (value->type != JTYPE_NUMBER) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(67, node, depth, sem, name, "node value type %s != JTYPE_BOOL",
				    json_type_name(value->type));
	}
	return NULL;
    }
    inum = &(value->item.number);
    if (inum->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(68, node, depth, sem, name, "node value JTYPE_NUMBER converted is false");
	}
	return NULL;
    }

    /*
     * determine if time_t is signed or unsigned
     */
    if ((time_t)-1 > 0) {

	/*
	 * case: time_t is unsigned
	 */
	if (inum->is_negative == true) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(69, node, depth, sem, name, "node negative JTYPE_NUMBER with unsigned time_t");
	    }
	    return NULL;
	}
	if (inum->umaxint_sized == false) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(70, node, depth, sem, name, "JTYPE_NUMBER umaxint_sized false with unsigned time_t");
	    }
	    return NULL;
	}
	ret = (time_t *)&(inum->as_umaxint);

    } else {

	/*
	 * case: time_t is signed
	 */
	if (inum->maxint_sized == false) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(71, node, depth, sem, name, "JTYPE_NUMBER maxint_sized false with signed time_t");
	    }
	    return NULL;
	}
	ret = (time_t *)&(inum->as_maxint);
    }

    /*
     * case success: return JSON boolean
     */
    return ret;
}


/*
 * sem_node_parent - return pointer to the parent of a JSON parse node
 *
 * Given a JSON node, attempt to return the parent of a JSON node.
 * Beyond simply returning the parent node, we inspect the parent
 * node to determine if the parent node was successfully converted
 * into a known JTYPE.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	!= NULL ==> valid parent JSON node
 *	    The val_err arg is ignored
 *	NULL ==> no parent node, invalid parent node, or internal error
 *	    If val_err != NULLm then *val_err is JSON semantic validation error (struct json_cnt_err)
 */
struct json *
sem_node_parent(struct json *node, unsigned int depth, struct json_sem *sem,
	        char const *name, struct json_sem_val_err **val_err)
{
    bool valid = false;			/* true ==> JSON node is converted and valid JTYPE */
    struct json *parent = NULL;		/* JSON parse parent node */

    /*
     * firewall - args
     */
    if (sem_chk_null_args(node, depth, sem, name, val_err) == true) {
	/* sem_chk_null_args() will have set *val_err */
	return NULL;
    }

    /*
     * validate this JSON parse node type
     */
    valid = sem_node_valid_converted(node, depth, sem, name, val_err);
    if (valid == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return NULL;
    }

    /*
     * case: if depth is 0 we are the tree root or unlinked node
     */
    parent = node->parent;
    if (depth <= 0) {
	if (parent != NULL) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(72, node, depth, sem, name,
					"depth: %d <= 0 with non-NULL parent node pointer", depth);
	    }
	}
	return NULL;
    }

    /*
     * firewall - parent pointer
     */
    if (parent == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(73, node, depth, sem, name, "node parent node NULL");
	}
	return NULL;
    }

    /*
     * validate parent JSON parse node type
     */
    valid = sem_node_valid_converted(parent, depth-1, sem, name, val_err);
    if (valid == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return NULL;
    }

    /*
     * return parent node
     */
    return parent;
}


/*
 * sem_object_find_name - find a given named JTYPE_MEMBER in a JTYPE_OBJECT
 *
 * Given a JSON node of type JTYPE_OBJECT, look for a JTYPE_MEMBER with a given name
 * that is directly under the JTYPE_OBJECT and return a pointer to that JTYPE_MEMBER,
 * or return NULL on error or invalid input.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *	memname	name of JTYPE_MEMBER to find directly under JTYPE_OBJECT
 *
 * returns:
 *	!= NULL ==> JTYPE_MEMBER with a given name
 *	    The val_err arg is ignored
 *	NULL ==> no such JTYPE_MEMBER found, or invalid arguments or JSON conversion error
 *	    If val_err != NULLm then *val_err is JSON semantic validation error (struct json_cnt_err)
 */
struct json *
sem_object_find_name(struct json *node, unsigned int depth, struct json_sem *sem,
		     char const *name, struct json_sem_val_err **val_err,
		     char const *memname)
{
    struct json_object *item = NULL;		/* JSON member */
    bool valid = false;				/* true ==> JSON node is converted and valid JTYPE */
    int i;

    /*
     * firewall - args
     */
    if (sem_chk_null_args(node, depth, sem, name, val_err) == true) {
	/* sem_chk_null_args() will have set *val_err */
	return NULL;
    }
    if (memname == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(74, node, depth, sem, name, "memname is NULL");
	}
	return NULL;
    }

    /*
     * validate JSON parse node type
     */
    if (node->type != JTYPE_OBJECT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(75, node, depth, sem, name, "node type %s != JTYPE_OBJECT",
				    json_type_name(node->type));
	}
	return NULL;
    }
    item = &(node->item.object);
    if (item->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(76, node, depth, sem, name, "JTYPE_OBJECT node converted is false");
	}
	return NULL;
    }

    /*
     * validate JSON object member array
     */
    if (item->len < 0) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(77, node, depth, sem, name, "JTYPE_OBJECT set length: %d < 0", item->len);
	}
	return NULL;
    } else if (item->len == 0) {
	/* case: empty object member array will not have the named JTYPE_MEMBER */
	return NULL;
    }
    if (item->set == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(78, node, depth, sem, name, "JTYPE_OBJECT len: %d > 0 and set is NULL", item->len);
	}
	return NULL;
    }

    /*
     * search the JSON member array for the named member
     */
    for (i=0; i < item->len; ++i) {
	struct json *s = item->set[i];		/* set member under the JTYPE_OBJECT */
	char *smemname = NULL;			/* name of set member */

	/*
	 * firewall - validate set member (must be a valid JTYPE_MEMBER with non-NULL
	 */
	if (s == NULL) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(79, node, depth, sem, name, "JTYPE_OBJECT set[%d] is NULL", i);
	    }
	    return NULL;
	}
	valid = sem_node_valid_converted(s, depth+1, sem, name, val_err);
	if (valid == false) {
	    /* sem_node_valid_converted() will have set *val_err */
	    return NULL;
	}
	if (s->type != JTYPE_MEMBER) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(80, node, depth+1, sem, name, "JTYPE_OBJECT set[%d] type: %s != JTYPE_MEMBER",
					i, json_type_name(s->type));
	    }
	    return NULL;
	}

	/*
	 * try to match the memname with this set member
	 */
	smemname = sem_member_name_decoded_str(s, depth+1, sem, name, val_err);
	if (smemname == NULL) {
	    /* sem_member_name_decoded_str() will have set *val_err */
	    return NULL;
	}
	if (strcmp(memname, smemname) == 0) {
	    /* found match */
	    return s;
	}
    }

    /*
     * no such member
     */
    if (val_err != NULL) {
	*val_err = werr_sem_val(81, node, depth, sem, name, "JTYPE_OBJECT has no member named: <%s>", memname);
    }
    return NULL;
}
