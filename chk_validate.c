/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chk_validate - validate functions for checking JSON semantics
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <unistd.h>

/*
 * chk_validate - validate functions for checking JSON semantics
 */
#include "chk_validate.h"


/*
 * static function declarations
 */


/* XXX - begin sorted order matching entry_util.c here - XXX */


/*
 * chk_IOCCC_author_version - JSON semantic check for IOCCC_author_version
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_IOCCC_author_version(struct json *node,
			 unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_IOCCC_author_version(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(100, node, depth, sem, __func__, "invalid IOCCC_author_version");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_IOCCC_contest_id - JSON semantic check for IOCCC_contest_id
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_IOCCC_contest_id(struct json *node,
		     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_IOCCC_contest_id(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(101, node, depth, sem, __func__, "invalid IOCCC_contest_id");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_IOCCC_info_version - JSON semantic check for IOCCC_info_version
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_IOCCC_info_version(struct json *node,
		       unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_IOCCC_info_version(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(102, node, depth, sem, __func__, "invalid IOCCC_info_version");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_Makefile - JSON semantic check for Makefile filename
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_Makefile(struct json *node,
	     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_Makefile(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(103, node, depth, sem, __func__, "invalid Makefile filename");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_Makefile_override - JSON semantic check for Makefile_override
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_Makefile_override(struct json *node,
		      unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and boolean check
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_Makefile_override(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(104, node, depth, sem, __func__, "invalid Makefile_override");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_abstract - JSON semantic check for abstract
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_abstract(struct json *node,
	     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_abstract(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(105, node, depth, sem, __func__, "invalid abstract");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_affiliation - JSON semantic check for affiliation
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_affiliation(struct json *node,
	        unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    struct str_or_null val;			/* report JSON JTYPE_MEMBER value */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string or JSON null check
     */
    val = sem_member_value_str_or_null(node, depth, sem, __func__, val_err);
    if (val.valid == false) {
	/* sem_member_value_str_or_null() will have set *val_err */
	return false;
    }

    /*
     * case: affiliation is null
     */
    if (val.is_null == true) {
	return true;
    }

    /*
     * validate decoded JSON string
     */
     /* paranoia */
    if (val.str == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(106, node, depth, sem, __func__,
				    "val.valid true, val.is_null false, but val.str is NULL");
	}
	return false;
    }
    test = test_affiliation(val.str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(107, node, depth, sem, __func__, "invalid affiliation");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_author_JSON - JSON semantic check for author_JSON
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_author_JSON(struct json *node,
	        unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_author_JSON(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(108, node, depth, sem, __func__, "invalid author_JSON");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_author_count - JSON semantic check for author_count
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_author_count(struct json *node,
	     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    int *value = NULL;				/* JSON_NUMBER as decoded int */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and JSON number as int check
     */
    value = sem_member_value_int(node, depth, sem, __func__, val_err);
    if (value == NULL) {
	/* sem_member_value_int() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_author_count(*value);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(109, node, depth, sem, __func__, "invalid author_count");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_author_handle - JSON semantic check for author_handle
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_author_handle(struct json *node,
	          unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_author_handle(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(110, node, depth, sem, __func__, "invalid author_handle");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_author_number - JSON semantic check for author_number
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_author_number(struct json *node,
		  unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    int *value = NULL;				/* JSON_NUMBER as decoded int */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and JSON number as int check
     */
    value = sem_member_value_int(node, depth, sem, __func__, val_err);
    if (value == NULL) {
	/* sem_member_value_int() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_author_number(*value);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(111, node, depth, sem, __func__, "invalid author_number");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_author - JSON semantic check for author array
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_author(struct json *node,
	   unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    struct json *value = NULL;          /* value of JTYPE_MEMBER */
    struct json *parent = NULL;		/* JSON parse tree node parent */
    struct json *acount_node = NULL;	/* JSON parse node containing author_count */
    struct json_array *array = NULL;	/* JSON parse node value as JTYPE_ARRAY */
    struct author *aset = NULL;		/* array of authors converted from array of JSON parse tree JSON_OBJECT */
    int *author_count = NULL;		/* pointer to author count as int from JSON parse node for author_count */
    bool test = false;			/* validation test result */
    int i;

    /*
     * firewall - args
     */
    if (sem_chk_null_args(node, depth, sem, __func__, val_err) == true) {
	/* sem_chk_null_args() will have set *val_err */
	return false;
    }

    /*
     * firewall - node type
     */
    test = sem_node_valid_converted(node, depth, sem, __func__, val_err);
    if (test == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return false;
    }
    if (node->type != JTYPE_MEMBER) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(112, node, depth, sem, __func__, "node type %s != JTYPE_MEMBER",
				    json_type_name(node->type));
	}
	return false;
    }

    /*
     * look at parent of the author array
     */
    parent = sem_node_parent(node, depth, sem, __func__, val_err);
    if (parent == NULL) {
	/* sem_node_parent() will have set *val_err */
	return false;
    }

    /*
     * find author_count in parent node
     *
     * NOTE: sem_object_find_name() will verify that parent is a JSON parse node of JTYPE_OBJECT type.
     */
    acount_node = sem_object_find_name(parent, depth-1, sem, __func__, val_err, "author_count");
    if (acount_node == NULL) {
	/* sem_object_find_name() will have set *val_err */
	return false;
    }

    /*
     * obtain the "valid_author" count
     */
    author_count = sem_member_value_int(acount_node, depth, sem, __func__, val_err);
    if (author_count == NULL) {
	/* sem_member_value_int() will have set *val_err */
	return false;
    }
    test = test_author_count(*author_count);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(113, node, depth, sem, __func__, "invalid author_count");
	}
	return false;
    }

    /*
     * obtain the JTYPE_ARRAY that is the value of this node
     */
    value = sem_member_value(node, depth, sem, __func__, val_err);
    if (value == NULL) {
        /* sem_member_value() will have set *val_err */
        return false;
    }
    if (value->type != JTYPE_ARRAY) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(114, node, depth, sem, __func__, "node type %s != JTYPE_ARRAY",
				    json_type_name(value->type));
	}
	return false;
    }
    array = &(value->item.array);
    if (array->set == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(115, node, depth+1, sem, __func__,
				    "node value JTYPE_ARRAY set is NULL");
	}
	return false;
    }

    /*
     * firewall - author count must match array length
     */
    if (*author_count != array->len) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(116, node, depth, sem, __func__,
				    "author_count: %d != array len: %d",
				    *author_count, array->len);
	}
	return false;
    }

    /*
     * allocate array of authors
     */
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    aset = calloc(*author_count, sizeof(aset[0]));
    if (aset == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(117, node, depth, sem, __func__,
				     "calloc of %d authors failed", *author_count);
	}
	return false;
    }

    /*
     * look at each JSON node (author) of node value (JTYPE_ARRAY)
     */
    for (i=0; i < *author_count; ++i) {
	struct json *e = array->set[i];		/* next item in the JTYPE_ARRAY */
	bool load = false;			/* true ==> able to load author array item */

	/*
	 * load author array item from JTYPE_OBJECT if possible
	 */
	load = object2author(e, depth+2, sem, __func__, val_err, &aset[i], i);
	if (load == false) {
	    /* object2athor() will have set *val_err */
	    return false;
	}
    }

    /*
     * look at the authors array for unique authors
     */
    test = test_authors(*author_count, aset);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(118, node, depth, sem, __func__,
				    "author set contains invalid author numbers "
				    "and/or duplicate names");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_c_src - JSON semantic check for c_src
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_c_src(struct json *node,
	  unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_c_src(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(119, node, depth, sem, __func__, "invalid c_src");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_chkentry_version - JSON semantic check for chkentry_version
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_chkentry_version(struct json *node,
		     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_chkentry_version(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(120, node, depth, sem, __func__, "invalid chkentry_version");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_default_handle - JSON semantic check for default_handle
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_default_handle(struct json *node,
		   unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_default_handle(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(121, node, depth, sem, __func__, "invalid default_handle");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_email - JSON semantic check for email
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_email(struct json *node,
	  unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_email(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(122, node, depth, sem, __func__, "invalid email");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_empty_override - JSON semantic check for empty_override
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_empty_override(struct json *node,
	           unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_empty_override(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(123, node, depth, sem, __func__, "invalid empty_override");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_entry_num - JSON semantic check for entry_num
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_entry_num(struct json *node,
	      unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    int *value = NULL;				/* JSON_NUMBER as decoded int */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and JSON number as int check
     */
    value = sem_member_value_int(node, depth, sem, __func__, val_err);
    if (value == NULL) {
	/* sem_member_value_int() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_entry_num(*value);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(124, node, depth, sem, __func__, "invalid entry_num");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_extra_file - JSON semantic check for extra_file
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_extra_file(struct json *node,
	       unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_extra_file(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(125, node, depth, sem, __func__, "invalid extra_file");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_first_rule_is_all - JSON semantic check for first_rule_is_all
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_first_rule_is_all(struct json *node,
		      unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_first_rule_is_all(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(126, node, depth, sem, __func__, "invalid first_rule_is_all");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_fnamchk_version - JSON semantic check for fnamchk_version
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_fnamchk_version(struct json *node,
	       unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    char *str = NULL;				/* JTYPE_STRING as decoded JSON string */
    bool test = false;				/* validation test result */

    /*
     * firewall - args and decoded string check
     */
    str = sem_member_value_decoded_str(node, depth, sem, __func__, val_err);
    if (str == NULL) {
	/* sem_member_value_decoded_str() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_fnamchk_version(str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(128, node, depth, sem, __func__, "invalid fnamchk_version");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/* XXX - end sorted order matching entry_util.c here -XXX */


/*
 * chk_rule_2a_override - JSON semantic check for rule_2a_override
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_rule_2a_override(struct json *node,
		     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_rule_2a_override(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(129, node, depth, sem, __func__, "invalid rule_2a_override");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_rule_2a_mismatch - JSON semantic check for rule_2a_mismatch
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_rule_2a_mismatch(struct json *node,
		     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_rule_2a_mismatch(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(130, node, depth, sem, __func__, "invalid rule_2a_mismatch");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_rule_2b_override - JSON semantic check for rule_2b_override
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_rule_2b_override(struct json *node,
		      unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_rule_2b_override(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(131, node, depth, sem, __func__, "invalid rule_2b_override");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_highbit_warning - JSON semantic check for highbit_warning
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_highbit_warning(struct json *node,
		    unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    if (sem_chk_null_args(node, depth, sem, __func__, val_err) == true) {
	/* chk_null_args() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_highbit_warning(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(132, node, depth, sem, __func__, "invalid highbit_warning");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_nul_warning - JSON semantic check for nul_warning
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_nul_warning(struct json *node,
		unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_nul_warning(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(133, node, depth, sem, __func__, "invalid nul_warning");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_trigraph_warning - JSON semantic check for trigraph_warning
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_trigraph_warning(struct json *node,
		     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_trigraph_warning(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(134, node, depth, sem, __func__, "invalid trigraph_warning");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_wordbuf_warning - JSON semantic check for wordbuf_warning
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_wordbuf_warning(struct json *node,
		    unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_wordbuf_warning(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(135, node, depth, sem, __func__, "invalid wordbuf_warning");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_ungetc_warning - JSON semantic check for ungetc_warning
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_ungetc_warning(struct json *node,
		   unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_ungetc_warning(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(136, node, depth, sem, __func__, "invalid ungetc_warning");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_found_all_rule - JSON semantic check for found_all_rule
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_found_all_rule(struct json *node,
		   unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_found_all_rule(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(137, node, depth, sem, __func__, "invalid found_all_rule");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_found_clean_rule - JSON semantic check for found_clean_rule
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_found_clean_rule(struct json *node,
		     unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_found_clean_rule(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(138, node, depth, sem, __func__, "invalid found_clean_rule");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_found_clobber_rule - JSON semantic check for found_clobber_rule
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_found_clobber_rule(struct json *node,
		       unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_found_clobber_rule(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(139, node, depth, sem, __func__, "invalid found_clobber_rule");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_found_try_rule - JSON semantic check for found_try_rule
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_found_try_rule(struct json *node,
		   unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_found_try_rule(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(140, node, depth, sem, __func__, "invalid found_try_rule");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


/*
 * chk_test_mode - JSON semantic check for test_mode
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *
 * returns:
 *	true ==> JSON element is valid
 *	false ==> JSON element is NOT valid, or NULL pointer, or some internal error
 */
bool
chk_test_mode(struct json *node,
	      unsigned int depth, struct json_sem *sem, struct json_sem_val_err **val_err)
{
    bool *boolean = NULL;			/* pointer to JTYPE_BOOL as decoded JSON boolean */
    bool test = false;				/* validation test result */

    /*
     * firewall - args
     */
    boolean = sem_member_value_bool(node, depth, sem, __func__, val_err);
    if (boolean == NULL) {
	/* sem_member_value_bool() will have set *val_err */
	return false;
    }

    /*
     * validate decoded JSON string
     */
    test = test_test_mode(*boolean);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(141, node, depth, sem, __func__, "invalid test_mode");
	}
	return false;
    }

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}
