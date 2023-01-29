/*
 * json_sem - JSON semantics support
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * The JSON parser was co-developed in 2022 by:
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


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <limits.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

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
    -1,		/* json_sem ptr is NULL, not in table */
    "NULL pointer given to werr_sem_val",	/* diagnostic message or NULL */
    false	/* true ==> struct json_sem_val_err was malloced */
		/* false ==> this is a static struct json_sem_val_err */
};
/* calloc failure */
static struct json_sem_val_err sem_calloc_err = {
    NULL,	/* JSON parse node in question or NULL */
    UINT_MAX,	/* JSON parse tree node depth or UINT_MAX */
    NULL,	/* semantic node in question or NULL */
    -1,		/* json_sem ptr is NULL, not in table */
    "calloc failure",	/* diagnostic message or NULL */
    false	/* true ==> struct json_sem_val_err was malloced */
		/* false ==> this is a static struct json_sem_val_err */
};
/* strdup failure */
static struct json_sem_val_err sem_strdup_err = {
    NULL,	/* JSON parse node in question or NULL */
    UINT_MAX,	/* JSON parse tree node depth or UINT_MAX */
    NULL,	/* semantic node in question or NULL */
    -1,		/* json_sem ptr is NULL, not in table */
    "strdup failure",	/* diagnostic message or NULL */
    false	/* true ==> struct json_sem_val_err was malloced */
		/* false ==> this is a static struct json_sem_val_err */
};
/* validation failure w/o json_sem_val_err */
static struct json_sem_val_err sem_val_err_NULL = {
    NULL,	/* JSON parse node in question or NULL */
    UINT_MAX,	/* JSON parse tree node depth or UINT_MAX */
    NULL,	/* semantic node in question or NULL */
    -1,		/* json_sem ptr is NULL, not in table */
    "validation failed yet json_sem_val_err is NULL",	/* diagnostic message or NULL */
    false	/* true ==> struct json_sem_val_err was malloced */
		/* false ==> this is a static struct json_sem_val_err */
};


/*
 * static functions
 */
static void sem_walk(struct json *node, unsigned int depth, va_list ap);


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
werr_sem_val(int val_err, struct json const *node, unsigned int depth, struct json_sem *sem,
	     char const *name, char const *fmt, ...)
{
    va_list ap;					/* variable argument list */
    struct json_sem_val_err *ret = NULL;	/* malloced return value */
    char message[BUFSIZ+1];				/* vsnwerr() message buffer */
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
    message[0] = '\0';	/* paranoia */
    message[BUFSIZ] = '\0';	/* paranoia */
    vsnwerr(val_err, message, BUFSIZ, name, fmt, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * duplicate the error message
     */
    diagnostic = strdup(message);
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
werrp_sem_val(int val_err, struct json const *node, unsigned int depth, struct json_sem *sem,
	      char const *name, char const *fmt, ...)
{
    va_list ap;					/* variable argument list */
    struct json_sem_val_err *ret = NULL;	/* malloced return value */
    char mesg[BUFSIZ+1];				/* vsnwerr() message buffer */
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
    mesg[0] = '\0';	/* paranoia */
    mesg[BUFSIZ] = '\0';	/* paranoia */
    vsnwerrp(val_err, mesg, BUFSIZ, name, fmt, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * duplicate the error message
     */
    diagnostic = strdup(mesg);
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
sem_chk_null_args(struct json const *node, unsigned int depth, struct json_sem *sem,
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
 * Given a JSON node, perform sanity checks on the JSON node.
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
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_count_err)
 */
bool
sem_node_valid_converted(struct json const *node, unsigned int depth, struct json_sem *sem,
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
	    struct json_number const *item = &(node->item.number);

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
	    struct json_string const *item = &(node->item.string);

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
	    struct json_boolean const *item = &(node->item.boolean);

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
	    struct json_null const *item = &(node->item.null);

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
	    struct json_member const *item = &(node->item.member);
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
	    struct json_object const *item = &(node->item.object);

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
		    *val_err = werr_sem_val(35, node, depth, sem, name, "JTYPE_OBJECT node: len: %ju < 0",
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
	    struct json_array const *item = &(node->item.array);

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
		    *val_err = werr_sem_val(39, node, depth, sem, name, "JTYPE_ARRAY node: len: %ju < 0",
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
	    struct json_elements const *item = &(node->item.elements);

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
		    *val_err = werr_sem_val(43, node, depth, sem, name, "JTYPE_ELEMENTS node: len: %ju < 0",
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
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_count_err)
 */
struct json *
sem_member_name(struct json const *node, unsigned int depth, struct json_sem *sem,
		char const *name, struct json_sem_val_err **val_err)
{
    struct json_member const *item = NULL;	/* JSON member */
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
    item = &(node->item.member);
    if (item->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(48, node, depth, sem, name, "JTYPE_MEMBER node converted is false");
	}
	return NULL;
    }

    /*
     * firewall - name
     */
    n = item->name;
    if (n == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(49, node, depth, sem, name, "node name is NULL");
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
	    *val_err = werr_sem_val(50, node, depth, sem, name, "node name type %s != JTYPE_STRING",
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
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_count_err)
 */
struct json *
sem_member_value(struct json const *node, unsigned int depth, struct json_sem *sem,
		 char const *name, struct json_sem_val_err **val_err)
{
    struct json_member const *item = NULL;	/* JSON member */
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
	    *val_err = werr_sem_val(51, node, depth, sem, name, "node type %s != JTYPE_MEMBER",
				    json_type_name(node->type));
	}
	return NULL;
    }
    item = &(node->item.member);
    if (item->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(52, node, depth, sem, name, "JTYPE_MEMBER node converted is false");
	}
	return NULL;
    }

    /*
     * firewall - name
     */
    n = item->value;
    if (n == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(53, node, depth, sem, name, "node name is NULL");
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
 *	    If val_err != NULL, then *val_err is JSON semantic validation error (struct json_count_err)
 */
char *
sem_member_name_decoded_str(struct json const *node, unsigned int depth, struct json_sem *sem,
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
	    *val_err = werr_sem_val(54, node, depth, sem, name, "node name type %s != JTYPE_STRING",
				    json_type_name(n->type));
	}
	return NULL;
    }
    istr = &(n->item.string);
    if (istr->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(55, node, depth, sem, name, "node name JTYPE_STRING converted is false");
	}
	return NULL;
    }

    /*
     * firewall - decoded JSON string
     */
    str = istr->str;
    if (str == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(56, node, depth, sem, name, "node name decoded JSON string is NULL");
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
sem_member_value_decoded_str(struct json const *node, unsigned int depth, struct json_sem *sem,
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
	    *val_err = werr_sem_val(57, node, depth, sem, name, "node value type %s != JTYPE_STRING",
				    json_type_name(value->type));
	}
	return NULL;
    }
    istr = &(value->item.string);
    if (istr->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(58, node, depth, sem, name, "node value JTYPE_STRING converted is false");
	}
	return NULL;
    }

    /*
     * firewall - decoded JSON string
     */
    str = istr->str;
    if (str == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(59, node, depth, sem, name, "node value decoded JSON string is NULL");
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
sem_member_value_bool(struct json const *node, unsigned int depth, struct json_sem *sem,
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
	    *val_err = werr_sem_val(60, node, depth, sem, name, "node value type %s != JTYPE_BOOL",
				    json_type_name(value->type));
	}
	return NULL;
    }
    ibool = &(value->item.boolean);
    if (ibool->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(61, node, depth, sem, name, "node value JTYPE_BOOL converted is false");
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
sem_member_value_str_or_null(struct json const *node, unsigned int depth, struct json_sem *sem,
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
		*val_err = werr_sem_val(62, node, depth, sem, name, "node value JTYPE_STRING converted is false");
	    }
	    return ret;
	}

	/*
	 * firewall - decoded JSON string
	 */
	ret.str = istr->str;
	if (ret.str == NULL) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(63, node, depth, sem, name, "node value decoded JSON string is NULL");
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
		*val_err = werr_sem_val(64, node, depth, sem, name, "node value JTYPE_NULL converted is false");
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
	    *val_err = werr_sem_val(65, node, depth, sem, name, "node value type %s != JTYPE_STRING and != JTYPE_NULL",
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
sem_member_value_int(struct json const *node, unsigned int depth, struct json_sem *sem,
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
	    *val_err = werr_sem_val(66, node, depth, sem, name, "node value type %s != JTYPE_BOOL",
				    json_type_name(value->type));
	}
	return NULL;
    }
    inum = &(value->item.number);
    if (inum->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(67, node, depth, sem, name, "node value JTYPE_NUMBER converted is false");
	}
	return NULL;
    }

    /*
     * validate JTYPE_NUMBER was able to be converted into an int
     */
    if (inum->int_sized == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(68, node, depth, sem, name, "node value JTYPE_NUMBER was unable to convert to an int");
	}
	return NULL;
    }

    /*
     * case success: return JSON boolean
     */
    return &(inum->as_int);
}

/*
 * sem_member_value_size_t - return pointer to size_t from a JSON number value of JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the value of the JSON member
 * and if it is a JTYPE_NUMBER (JSON number), return a pointer to a converted
 * size_t or return NULL on error or invalid input.
 *
 * If the JTYPE_NUMBER (JSON number) cannot be returned as a size_t, NULL is returned.
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
 *	!= NULL ==> decoded JTYPE_NUMBER as a size_t from the value part of JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON conversion error
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_sem_val_err)
 */
size_t *
sem_member_value_size_t(struct json const *node, unsigned int depth, struct json_sem *sem,
			         char const *name, struct json_sem_val_err **val_err)
{
    struct json *value = NULL;			/* value of JTYPE_MEMBER */
    struct json_number *snum = NULL;		/* JTYPE_MEMBER value as JTYPE_NUMBER */

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
	    *val_err = werr_sem_val(69, node, depth, sem, name, "node value type %s != JTYPE_BOOL",
				    json_type_name(value->type));
	}
	return NULL;
    }
    snum = &(value->item.number);
    if (snum->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(70, node, depth, sem, name, "node value JTYPE_NUMBER converted is false");
	}
	return NULL;
    }

    /*
     * validate JTYPE_NUMBER was able to be converted into a size_t
     */
    if (snum->size_sized == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(71, node, depth, sem, name, "node value JTYPE_NUMBER was unable to convert to a size_t");
	}
	return NULL;
    }

    /*
     * case success: return JSON boolean
     */
    return &(snum->as_size);
}

/*
 * sem_member_value_time_t - return pointer to time_t from a JSON number value of JTYPE_MEMBER
 *
 * Given a JSON node of type JTYPE_MEMBER, look at the value of the JSON member
 * and if it is a JTYPE_NUMBER (JSON number), return a pointer to a time_t
 * or return NULL on error or invalid input.
 *
 * If the JTYPE_NUMBER (JSON number) cannot be returned as a time_t, NULL is returned.
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
 *	!= NULL ==> decoded JTYPE_NUMBER as a time_t from the value part of JTYPE_MEMBER
 *	    The val_err arg is ignored
 *	NULL ==> invalid arguments or JSON conversion error
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_sem_val_err)
 */
time_t *
sem_member_value_time_t(struct json const *node, unsigned int depth, struct json_sem *sem,
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
	    *val_err = werr_sem_val(72, node, depth, sem, name, "node value type %s != JTYPE_BOOL",
				    json_type_name(value->type));
	}
	return NULL;
    }
    inum = &(value->item.number);
    if (inum->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(73, node, depth, sem, name, "node value JTYPE_NUMBER converted is false");
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
		*val_err = werr_sem_val(74, node, depth, sem, name, "node negative JTYPE_NUMBER with unsigned time_t");
	    }
	    return NULL;
	}
	if (inum->umaxint_sized == false) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(75, node, depth, sem, name, "JTYPE_NUMBER umaxint_sized false with unsigned time_t");
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
		*val_err = werr_sem_val(76, node, depth, sem, name, "JTYPE_NUMBER maxint_sized false with signed time_t");
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
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_count_err)
 */
struct json *
sem_node_parent(struct json const *node, unsigned int depth, struct json_sem *sem,
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
		*val_err = werr_sem_val(77, node, depth, sem, name,
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
	    *val_err = werr_sem_val(78, node, depth, sem, name, "node parent node NULL");
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
 *	    If val_err != NULL then *val_err is JSON semantic validation error (struct json_count_err)
 */
struct json *
sem_object_find_name(struct json const *node, unsigned int depth, struct json_sem *sem,
		     char const *name, struct json_sem_val_err **val_err,
		     char const *memname)
{
    struct json_object const *item = NULL;	/* JSON member */
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
	    *val_err = werr_sem_val(79, node, depth, sem, name, "memname is NULL");
	}
	return NULL;
    }

    /*
     * validate JSON parse node type
     */
    if (node->type != JTYPE_OBJECT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(80, node, depth, sem, name, "node type %s != JTYPE_OBJECT",
				    json_type_name(node->type));
	}
	return NULL;
    }
    item = &(node->item.object);
    if (item->converted == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(81, node, depth, sem, name, "JTYPE_OBJECT node converted is false");
	}
	return NULL;
    }

    /*
     * validate JSON object member array
     */
    if (item->len < 0) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(82, node, depth, sem, name, "JTYPE_OBJECT set length: %ju < 0", item->len);
	}
	return NULL;
    } else if (item->len == 0) {
	/* case: empty object member array will not have the named JTYPE_MEMBER */
	return NULL;
    }
    if (item->set == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(83, node, depth, sem, name, "JTYPE_OBJECT len: %ju > 0 and set is NULL", item->len);
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
		*val_err = werr_sem_val(84, node, depth, sem, name, "JTYPE_OBJECT set[%d] is NULL", i);
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
		*val_err = werr_sem_val(85, node, depth+1, sem, name, "JTYPE_OBJECT set[%d] type: %s != JTYPE_MEMBER",
					i, json_type_name(s->type));
	    }
	    return NULL;
	}

	/*
	 * try to match the smemname with this set member
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
	*val_err = werr_sem_val(86, node, depth, sem, name, "JTYPE_OBJECT has no member named: <%s>", memname);
    }
    return NULL;
}


/*
 * json_sem_zero_count - zero JSON semantic nodes and set their index
 *
 * given:
 *	sem		pointer to a JSON semantic table (ends with a JTYPE_UNSET JSON type)
 *
 * NOTE: This function does not return for NULL pointers or count <= 0
 */
void
json_sem_zero_count(struct json_sem *sem)
{
    int i;

    /*
     * firewall - args
     */
    if (sem == NULL) {
	err(87, __func__, "sem is NULL");
	not_reached();
    }

    /*
     * clear counts and set index
     */
    for (i=0; sem[i].type != JTYPE_UNSET; ++i) {
	sem[i].count = 0;
	sem[i].sem_index = i;
    }

    /*
     * set the special JTYPE_UNSET node
     */
    if (sem[i].type == JTYPE_UNSET) {
	sem[i].count = 0;
	sem[i].sem_index = -1;
    }
    return;
}


/*
 * json_sem_count_chk - validate semantic table counts
 *
 * given:
 *	sem		pointer to a JSON semantic table (ends with a JTYPE_UNSET JSON type)
 */
void
json_sem_count_chk(struct json_sem *sem, struct dyn_array *count_err)
{
    struct json_sem_count_err count;	/* semantic count error */
    int i;

    /*
     * firewall - args
     */
    if (sem == NULL) {
	warn(__func__, "sem is NULL");
	return;
    }
    if (count_err == NULL) {
	warn(__func__, "count_err is NULL");
	return;
    }

    /*
     * examine counts
     */
    for (i=0; sem[i].type != JTYPE_UNSET; ++i) {

	/*
	 * case: count is too small
	 */
	if (sem[i].count < sem[i].min) {

	    /*
	     * form count is too small error
	     */
	    count.node = NULL;
	    count.sem = &(sem[i]);
	    count.count = sem[i].count;
	    count.bad_min = true;
	    count.bad_max = false;
	    count.unknown_node = false;
	    if (count.sem != NULL) {
		count.sem_index = count.sem->sem_index;
	    } else {
		count.sem_index = -1;
	    }
	    count.diagnostic = calloc(BUFSIZ+1, sizeof(char));
	    if (count.diagnostic == NULL) {
		count.diagnostic = "calloc BUFSIZ calloc failed for count is too small";
		count.malloced = false;
	    } else {
		snmsg(count.diagnostic, BUFSIZ, "node type %s parse tree depth %u%s%s: found %u < minimum: %d",
		      json_type_name(sem[i].type), sem[i].depth,
		      (sem[i].name != NULL) ? " member name: " : "",
		      (sem[i].name != NULL) ? sem[i].name : "",
		      sem[i].count, sem[i].min);
		count.malloced = true;
	    }

	    /* save semantic count error */
	    dyn_array_append_value(count_err, &count);

	/*
	 * case: count is too large
	 */
	} else if (sem[i].max > 0 && sem[i].count > sem[i].max) {

	    /*
	     * form count is too large error
	     */
	    count.node = NULL;
	    count.sem = &(sem[i]);
	    count.count = sem[i].count;
	    count.bad_min = false;
	    count.bad_max = true;
	    count.unknown_node = false;
	    if (count.sem != NULL) {
		count.sem_index = count.sem->sem_index;
	    } else {
		count.sem_index = -1;
	    }
	    count.diagnostic = calloc(BUFSIZ+1, sizeof(char));
	    if (count.diagnostic == NULL) {
		count.diagnostic = "calloc BUFSIZ calloc failed for count is too small";
		count.malloced = false;
	    } else {
		snmsg(count.diagnostic, BUFSIZ, "node type %s parse tree depth %u%s%s: found %u > maximum: %d",
		      json_type_name(sem[i].type), sem[i].depth,
		      (sem[i].name != NULL) ? " member name: " : "",
		      (sem[i].name != NULL) ? sem[i].name : "",
		      sem[i].count, sem[i].max);
		count.malloced = true;
	    }

	    /* save semantic count error */
	    dyn_array_append_value(count_err, &count);
	}
    }
    return;
}


/*
 * json_sem_find - given JSON node, find first match in JSON semantic table
 *
 * given:
 *	node		pointer to a JSON parse tree
 *	depth		depth of node in the JSON parse tree (0 ==> tree root)
 *	sem		pointer to a JSON semantic table (ends with a JTYPE_UNSET JSON type)
 *
 * returns:
 *	>=0 ==> index into JSON semantic table for first match
 *	-1  ==> no JSON semantic table match found
 *	< -1 ==> invalid JSON node, or NULL ptr, or internal error
 */
int
json_sem_find(struct json *node, unsigned int depth, struct json_sem *sem)
{
    bool test = false;		/* JSON node test result */
    char *name = NULL;		/* name of JTYPE_MEMBER node or NULL */
    enum item_type type;	/* type of JSON node */
    int i;

    /*
     * firewall - args
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return -2;
    }
    if (sem == NULL) {
	warn(__func__, "sem is NULL");
	return -3;
    }

    /*
     * obtain information about the JSON node
     */
    type = node->type;
    if (type == JTYPE_MEMBER) {
        /* sem_member_name_decoded_str() call checks args via sem_chk_null_args() */
	/* sem_member_name_decoded_str() also calls sem_node_valid_converted() */
	/* determine name of JTYPE_MEMBER or return NULL */
	name = sem_member_name_decoded_str(node, depth, sem, __func__, NULL);
    } else {
	test = sem_node_valid_converted(node, depth, sem, __func__, NULL);
	if (test == false) {
	    warn(__func__, "JSON node is invalid");
	    return -4;
	}
    }

    /*
     * search the JSON semantic table
     */
    for (i=0; sem[i].type != JTYPE_UNSET; ++i) {

	/*
	 * compare levels
	 */
	if (depth != sem[i].depth) {
	    continue; /* no match */
	}

	/*
	 * compare levels
	 */
	if (type != sem[i].type) {
	    continue; /* no match */
	}

	/*
	 * case: JTYPE_MEMBER - compare names
	 */
	if (type == JTYPE_MEMBER && name != NULL && sem[i].name_len > 0 && strcmp(name, sem[i].name) == 0) {
	    return i;	/* match found */
	}

	/*
	 * case: non-JTYPE_MEMBER or JTYPE_MEMBER w/o name match
	 */
	if (type != JTYPE_MEMBER || name == NULL || sem[i].name_len == 0) {
	    return i;	/* match found */
	}
    }

    /*
     * no match for the JSON node in the JSON semantic table
     */
    return -1;
}


/*
 * sem_walk - JSON parse tree semantic tree check walk
 *
 * given:
 *	node	pointer to a JSON parser tree node to free
 *	depth	current tree depth (0 ==> top of tree)
 *	ap	variable argument list, required ap args:
 *
 *		sem		JSON semantic table (ends with a JTYPE_UNSET JSON type)
 *		count_err	dynamic array of JSON semantic count errors
 *		val_err		dynamic array of JSON semantic validation errors
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 */
static void
sem_walk(struct json *node, unsigned int depth, va_list ap)
{
    va_list ap2;			/* copy of va_list ap */
    struct json_sem *sem = NULL;	/* JSON semantic table (ends with a JTYPE_UNSET JSON type) */
    struct dyn_array *count_err = NULL;	/* dynamic array of JSON semantic count errors */
    struct dyn_array *val_err = NULL;	/* dynamic array of JSON semantic validation errors */
    bool test = false;			/* validation test result */
    struct json_sem_val_err *error = NULL;/* pointer to semantic validation error */
    struct json_sem_count_err count;	/* semantic count error */
    int index = -1;			/* semantic array index match or -1 ==> no march or < -1 ==> error */

    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * duplicate va_list ap
     */
    va_copy(ap2, ap);

    /*
     * obtain the stream, json_dbg_used, and json_dbg args
     */
    sem = va_arg(ap2, struct json_sem *);
    if (sem == NULL) {
	va_end(ap2); /* stdarg variable argument list cleanup */
	return;
    }
    count_err = va_arg(ap2, struct dyn_array *);
    if (count_err == NULL) {
	va_end(ap2); /* stdarg variable argument list cleanup */
	return;
    }
    val_err = va_arg(ap2, struct dyn_array *);
    if (val_err == NULL) {
	va_end(ap2); /* stdarg variable argument list cleanup */
	return;
    }

    /*
     * search for node match in the semantic table
     */
    index = json_sem_find(node, depth, sem);

    /*
     * process search result
     */
    if (index >= 0) {

	/*
	 * semantic table match, count node use
	 *
	 * NOTE: We always count a match regardless of validation status
	 */
	++(sem[index].count);

	/*
	 * execute validation function is available
	 */
	if (sem[index].validate != NULL) {

	    /*
	     * try to validate
	     */
	    test = sem[index].validate(node, depth, sem, &error);

	    /*
	     * case: validation failed
	     */
	    if (test == false) {

		/* be sure we have a validation error message */
		if (error == NULL) {
		    /* error is NULL, assume sem_val_err_NULL */
		    error = &sem_val_err_NULL;

		/* record semantic table index */
		} else {
		    error->sem_index = sem[index].sem_index;
		}

		/* save validation error message */
		dyn_array_append_value(val_err, error);
	    }
	}

    } else if (index == -1) {

	/*
	 * semantic table non-match
	 */
	count.node = node;
	count.sem = NULL;
	count.count = 1;
	count.bad_min = false;
	count.bad_max = false;
	count.unknown_node = true;
	count.sem_index = -1;
	count.diagnostic = calloc(BUFSIZ+1, sizeof(char));
	if (count.diagnostic == NULL) {
	    count.diagnostic = "calloc BUFSIZ calloc failed for unexpected node";
	    count.malloced = false;
	} else {
	    if (node->type == JTYPE_MEMBER) {
		char *name = NULL;	/* name of JTYPE_MEMBER */

		name = sem_member_name_decoded_str(node, depth, sem, __func__, &error);
		if (name == NULL) {
		    snmsg(count.diagnostic, BUFSIZ, "depth: %u type: %s name: ((NULL)); unnamed member",
			  depth, json_item_type_name(node));

		    /* be sure we have a validation error message */
		    if (error == NULL) {
			/* error is NULL, assume sem_val_err_NULL */
			error = &sem_val_err_NULL;
		    }

		    /* also save validation error message */
		    dyn_array_append_value(val_err, error);

		} else {
		    snmsg(count.diagnostic, BUFSIZ, "depth: %u type: %s name: \"%s\"; unexpected node",
			  depth, json_item_type_name(node), name);
		}
	    } else {
		snmsg(count.diagnostic, BUFSIZ, "depth: %u type: %s; unexpected node",
		      depth, json_item_type_name(node));
	    }
	    count.malloced = true;
	}

	/* save semantic count error */
	dyn_array_append_value(count_err, &count);

    } else {

	/*
	 * error searching semantic table
	 */
	count.node = node;
	count.sem = NULL;
	count.count = 1;
	count.bad_min = false;
	count.bad_max = false;
	count.unknown_node = true;
	count.sem_index = -1;
	count.diagnostic = calloc(BUFSIZ+1, sizeof(char));
	if (count.diagnostic == NULL) {
	    count.diagnostic = "calloc BUFSIZ calloc failed for json_sem_find result < -1";
	    count.malloced = false;
	} else {
	    snwerr(index, count.diagnostic, BUFSIZ, "json_sem_find",
			  "json_sem_find failed, returned %d < -1", index);
	    count.malloced = true;
	}

	/* save semantic count error */
	dyn_array_append_value(count_err, &count);
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap2);
    return;
}


/*
 * json_sem_check - check a JSON parse tree against a JSON semantic table
 *
 * First, if *pcount_err == NULL, then dynamic array *pcount_err is
 * created as an empty dynamic array, else the existing dynamic array *pcount_err
 * is used.  If *pval_err == NULL, then dynamic array *pval_err is
 * created as an empty dynamic array, else the existing dynamic array *pval_err
 * is used.
 *
 * We then walk the JSON parse tree and check each node against the JSON semantic table,
 * counting as nodes on the first match found in the JSON semantic table,
 * or appending a JSON semantic count error to the *pcount_err dynamic array
 * when an unknown JSON node is found.
 *
 * When a JSON node matches a JSON semantic table entry that has a non-NULL
 * validate function, the validate function will be called.  When the validate
 * function returns false (indicating a JSON semantic validation error is found),
 * the *pval_err dynamic array is appended with the given JSON semantic validation error.
 *
 * Once the JSON parse tree is traversed, the counts in the JSON semantic table
 * are checked against the minimum and maximum allowed counts.  When a count
 * is found to be out of range, JSON semantic count error is appended to
 * the *pcount_err dynamic array.
 *
 * given:
 *	node		pointer to a JSON parse tree
 *	max_depth	maximum tree depth to descend, or 0 ==> infinite depth
 *			    NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *			    NOTE: Consider use of JSON_DEFAULT_MAX_DEPTH for good default.
 *	sem		pointer to a JSON semantic table (ends with a JTYPE_UNSET JSON type)
 *	pcount_err	pointer to dynamic array of JSON semantic count errors,
 *			    NOTE: If *pcount_err == NULL, the dynamic array will be created,
 *				  If *pcount_err != NULL, the existing dynamic array will be used.
 *	pval_err	pointer to dynamic array of JSON semantic validation errors
 *			    NOTE: If *pcount_err == NULL, the dynamic array will be created,
 *				  If *pcount_err != NULL, the existing dynamic array will be used.
 *
 * return:
 *	0 ==> JSON parse tree is semantically consistent with the JSON semantic table,
 *	> 0  ==> number of errors (count+validation+internal) found
 *
 * NOTE: The number of errors does not reflect the sum of JSON semantic count and
 *	 JSON semantic validation errors because internal errors are also counted.
 *	 When evaluating a non-zero return and both the *pval_err dynamic array
 *	 and the *pcount_err dynamic array are empty, them report an internal json_sem_check()
 *	 error was encountered.
 */
uintmax_t
json_sem_check(struct json *node, unsigned int max_depth, struct json_sem *sem,
	       struct dyn_array **pcount_err, struct dyn_array **pval_err)
{
    struct dyn_array *count_err = NULL;		/* JSON semantic count errors */
    struct dyn_array *val_err = NULL;		/* JSON semantic validation errors */
    uintmax_t errors = 0;			/* number of errors (count+validation+internal) */

    /*
     * firewall - check args
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	++errors;
    }
    if (sem == NULL) {
	warn(__func__, "sem is NULL");
	++errors;
    }
    if (pcount_err == NULL) {
	warn(__func__, "pcount_err is NULL");
	++errors;
    }
    if (pval_err == NULL) {
	warn(__func__, "pval_err is NULL");
	++errors;
    }
    /* abort early on internal errors */
    if (errors > 0) {
	return errors;
    }

    /*
     * allocate empty dynamic arrays if dynamic array pointers are NULL
     */
    if (*pcount_err == NULL) {
	count_err = dyn_array_create(sizeof(struct json_sem_count_err), JSON_CHUNK, JSON_CHUNK, true);
	if (count_err == NULL) {
	    warn(__func__, "dyn_array_create() failed to create count_err");
	    ++errors;
	}
	*pcount_err = count_err;
    } else {
	count_err = *pcount_err;
    }
    if (*pval_err == NULL) {
	val_err = dyn_array_create(sizeof(struct json_sem_val_err), JSON_CHUNK, JSON_CHUNK, true);
	if (val_err == NULL) {
	    warn(__func__, "dyn_array_create() failed to create val_err");
	    ++errors;
	}
	*pval_err = val_err;
    } else {
	val_err = *pval_err;
    }
    if (count_err == NULL) {
	warn(__func__, "count_err remains NULL in dyn_array_create()");
	++errors;
    }
    if (val_err == NULL) {
	warn(__func__, "val_err remains NULL in dyn_array_create()");
	++errors;
    }
    /* abort early on internal errors */
    if (errors > 0) {
	return errors;
    }

    /*
     * zero semantic counts
     *
     * NOTE: This is very important because the semantic tables have counts
     *	     that are initialized with the count of the reference JSON file
     *	     that jsemcgen.sh used to generate them.
     */
    json_sem_zero_count(sem);

    /*
     * perform a semantic scan of the JSON parse tree
     */
    json_tree_walk(node, max_depth, sem_walk, sem, count_err, val_err);

    /*
     * check semantic table counts
     */
    json_sem_count_chk(sem, count_err);

    /*
     * count errors, if any
     */
    errors = (uintmax_t)dyn_array_tell(count_err) + (uintmax_t)dyn_array_tell(val_err);

    /*
     * report on the number of errors found
     */
    return errors;
}


/*
 * free_count_err - free semantic count errors
 *
 * given:
 *	count_err		semantic count error dynamic array to free
 */
void
free_count_err(struct dyn_array *count_err)
{
    struct json_sem_count_err *p = NULL;	/* pointer to JSON semantic count error */
    intmax_t count = 0;		/* length of semantic count array */
    intmax_t i = 0;

    /*
     * firewall
     */
    if (count_err == NULL) {
	warn(__func__, "count_err is NULL");
	return;
    }

    /*
     * free each semantic count error if malloced
     */
    count = dyn_array_tell(count_err);
    for (i=0; i < count; ++i) {

	/*
	 * free diagnostic is malloced
	 */
	p = dyn_array_addr(count_err, struct json_sem_count_err, i);
	if (p->malloced == true) {
	    free(p->diagnostic);
	    p->diagnostic = NULL;
	    p->malloced = false;
	}
    }

    /*
     * free the dynamic array
     */
    dyn_array_free(count_err);
    return;
}


/*
 * free_val_err - free semantic count errors
 *
 * given:
 *	val_err		semantic count error dynamic array to free
 */
void
free_val_err(struct dyn_array *val_err)
{
    struct json_sem_val_err *p = NULL;	/* pointer to JSON semantic count error */
    intmax_t count = 0;		/* length of semantic count array */
    intmax_t i = 0;

    /*
     * firewall
     */
    if (val_err == NULL) {
	warn(__func__, "val_err is NULL");
	return;
    }

    /*
     * free each semantic count error if malloced
     */
    count = dyn_array_tell(val_err);
    for (i=0; i < count; ++i) {

	/*
	 * free diagnostic is malloced
	 */
	p = dyn_array_addr(val_err, struct json_sem_val_err, i);
	if (p->malloced == true) {
	    free(p->diagnostic);
	    p->diagnostic = NULL;
	    p->malloced = false;
	}
    }

    /*
     * free the dynamic array
     */
    dyn_array_free(val_err);
    return;
}


/*
 * fprint_count_err - print information about a count error on stream
 *
 * given:
 *	stream		open stream to write on
 *	prefix		print prefix, if non-NULL, before printing info
 *	sem_count_err	pointer to semantic count error to print information about
 *	postfix		print postfix, if non-NULL, after printing info
 */
void
fprint_count_err(FILE *stream, char const *prefix, struct json_sem_count_err *sem_count_err, char const *postfix)
{
    char *p = NULL;		/* JSON node related string */
    struct json_sem sem_node;	/* JSON semantic node */
    int ret = 0;		/* libc return value */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return;
    }
    if (is_open_stream(stream) == false) {
	warn(__func__, "stream is is not an open FILE *stream");
	return;
    }
    if (sem_count_err == NULL) {
	warn(__func__, "sem_count_err is NULL");
	return;
    }

    /*
     * fill in sem_node for use if needed
     */
    if (sem_count_err->sem == NULL) {
	/* setup a dummy struct json_sem if the error has a NULL sem */
	memset(&sem_node, 0, sizeof(sem_node));
	sem_node.depth = INF_DEPTH;
	sem_node.type = JTYPE_UNSET;
	sem_node.sem_index = -1;
	sem_node.validate = NULL;
	sem_node.name = NULL;
    } else {
	memmove(&sem_node, sem_count_err->sem, sizeof(sem_node));
    }

    /*
     * print prefix if non-NULL
     */
    fpr(stream, __func__, "%s", prefix);

    /*
     * be more verbose if we are JSON debugging
     */
    if (json_dbg_allowed(JSON_DBG_LOW) == true) {

	/*
	 * case: we have a JSON semantic table index
	 */
	if (sem_count_err->sem_index >= 0) {
	    fpr(stream, __func__, "sem_tbl[%d]: ", sem_count_err->sem_index);
	}

	/*
	 * case: sem_count_err->node != NULL
	 */
	if (sem_count_err->node != NULL) {

	    /*
	     * print JSON node type
	     */
	    fpr(stream, __func__, "node type: %s ", json_item_type_name(sem_count_err->node));

	    /*
	     * case: JSON node is a member, print name
	     */
	    if (sem_count_err->node->type == JTYPE_MEMBER) {
		p = sem_member_name_decoded_str(sem_count_err->node, INF_DEPTH, &sem_node, __func__, NULL);
		fpr(stream, __func__, "name: \"%s\" ", p);
	    }
	}

	/*
	 * case: bad_min
	 */
	if (sem_count_err->bad_min == true) {
	    fpr(stream, __func__, "count: %u < min: %u ", sem_count_err->count, sem_node.min);
	}

	/*
	 * case: bad_max
	 */
	if (sem_count_err->bad_max == true) {
	    fpr(stream, __func__, "count: %u > max: %u ", sem_count_err->count, sem_node.max);
	}

	/*
	 * case: unknown_node
	 */
	if (sem_count_err->unknown_node == true) {
	    fpr(stream, __func__, "unknown node found: %u times ", sem_count_err->count);
	}

	/*
	 * case: we have a diagnostic string
	 */
	if (sem_count_err->diagnostic != NULL) {
	    fpr(stream, __func__, "/ error: %s ", sem_count_err->diagnostic);
	} else {
	    fpr(stream, __func__, "/ error: ((NULL))");
	}

    /*
     * otherwise just print the diagnostic
     */
    } else {

	/*
	 * case: we have a diagnostic string
	 */
	if (sem_count_err->diagnostic != NULL) {
	    fpr(stream, __func__, "%s ", sem_count_err->diagnostic);
	} else {
	    fpr(stream, __func__, "((NULL)) ");
	}
    }

    /*
     * print postfix if non-NULL
     */
    fpr(stream, __func__, "%s", postfix);

    /*
     * flush stream
     */
    errno = 0;		/* pre-clear fflush() status */
    ret = fflush(stream);
    if (ret == EOF) {
	warn(__func__, "fflush returned errno: %d: (%s)", errno, strerror(errno));
    }
    return;
}


/*
 * fprint_val_err - print information about a count error on stream
 *
 * given:
 *	stream		open stream to write on
 *	prefix		print prefix, if non-NULL, before printing info
 *	sem_val_err	pointer to semantic count error to print information about
 *	postfix		print postfix, if non-NULL, after printing info
 */
void
fprint_val_err(FILE *stream, char const *prefix, struct json_sem_val_err *sem_val_err, char const *postfix)
{
    char *p = NULL;		/* JSON node related string */
    struct json_sem sem_node;	/* JSON semantic node */
    int ret = 0;		/* libc return value */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return;
    }
    if (is_open_stream(stream) == false) {
	warn(__func__, "stream is is not an open FILE *stream");
	return;
    }
    if (sem_val_err == NULL) {
	warn(__func__, "sem_val_err is NULL");
	return;
    }

    /*
     * fill in sem_node for use if needed
     */
    if (sem_val_err->sem == NULL) {
	/* setup a dummy struct json_sem if the error has a NULL sem */
	memset(&sem_node, 0, sizeof(sem_node));
	sem_node.depth = INF_DEPTH;
	sem_node.type = JTYPE_UNSET;
	sem_node.sem_index = -1;
	sem_node.validate = NULL;
	sem_node.name = NULL;
    } else {
	memmove(&sem_node, sem_val_err->sem, sizeof(sem_node));
    }

    /*
     * print prefix if non-NULL
     */
    fpr(stream, __func__, "%s", prefix);

    /*
     * be more verbose if we are JSON debugging
     */
    if (json_dbg_allowed(JSON_DBG_LOW) == true) {

	/*
	 * case: we have a JSON semantic table index
	 */
	if (sem_val_err->sem_index >= 0) {
	    fpr(stream, __func__, "sem_tbl[%d]: ", sem_val_err->sem_index);
	}

	/*
	 * case: sem_val_err->node != NULL
	 */
	if (sem_val_err->node != NULL) {

	    /*
	     * print JSON node type
	     */
	    fpr(stream, __func__, "node type: %s ", json_item_type_name(sem_val_err->node));

	    /*
	     * case: JSON node is a member, print name
	     */
	    if (sem_val_err->node->type == JTYPE_MEMBER) {
		p = sem_member_name_decoded_str(sem_val_err->node, INF_DEPTH, &sem_node, __func__, NULL);
		fpr(stream, __func__, "name: \"%s\" ", p);
	    }
	}

	/*
	 * case: we have a diagnostic string
	 */
	if (sem_val_err->diagnostic != NULL) {
	    fpr(stream, __func__, "/ error: %s ", sem_val_err->diagnostic);
	} else {
	    fpr(stream, __func__, "/ error: ((NULL)) ");
	}

    /*
     * otherwise just print the diagnostic
     */
    } else {

	/*
	 * case: we have a diagnostic string
	 */
	if (sem_val_err->diagnostic != NULL) {
	    fpr(stream, __func__, "%s ", sem_val_err->diagnostic);
	} else {
	    fpr(stream, __func__, "((NULL)) ");
	}
    }

    /*
     * print postfix if non-NULL
     */
    fpr(stream, __func__, "%s", postfix);

    /*
     * flush stream
     */
    errno = 0;		/* pre-clear fflush() status */
    ret = fflush(stream);
    if (ret == EOF) {
	warn(__func__, "fflush returned errno: %d: (%s)", errno, strerror(errno));
    }
    return;
}
