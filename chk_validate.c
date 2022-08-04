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
	    *val_err = werr_sem_val(63, node, depth, sem, __func__, "invalid IOCCC_author_version");
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
	    *val_err = werr_sem_val(64, node, depth, sem, __func__, "invalid IOCCC_contest_id");
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
	    *val_err = werr_sem_val(65, node, depth, sem, __func__, "invalid IOCCC_info_version");
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
	    *val_err = werr_sem_val(66, node, depth, sem, __func__, "invalid Makefile filename");
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
	    *val_err = werr_sem_val(67, node, depth, sem, __func__, "invalid Makefile_override");
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
	    *val_err = werr_sem_val(68, node, depth, sem, __func__, "invalid abstract");
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
	    *val_err = werr_sem_val(69, node, depth, sem, __func__,
				    "val.valid true, val.is_null false, but val.str is NULL");
	}
	return false;
    }
    test = test_affiliation(val.str);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(70, node, depth, sem, __func__, "invalid affiliation");
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
	    *val_err = werr_sem_val(71, node, depth, sem, __func__, "invalid author_JSON");
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
	    *val_err = werr_sem_val(72, node, depth, sem, __func__, "invalid author_count");
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
	    *val_err = werr_sem_val(73, node, depth, sem, __func__, "invalid author_handle");
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
	    *val_err = werr_sem_val(74, node, depth, sem, __func__, "invalid author_number");
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
    struct json *parent = NULL;		/* JSON parse tree node parent */
    struct json *acount_node = NULL;	/* JSON parse node containing author_count */
    struct json_object *object = NULL;	/* JSON parse node as JTYPE_OBJECT */
    int *author_count = NULL;		/* pointer tp author count as int from JSON parse node for author_count */
    bool test = false;			/* validation test result */

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
    if (node->type != JTYPE_OBJECT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(75, node, depth, sem, __func__, "node type %s != JTYPE_OBJECT",
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

    /*
     * firewall - author count
     */
    test = test_author_count(*author_count);
    if (test == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(76, node, depth, sem, __func__, "invalid author_count");
	}
	return false;
    }
    object = &(node->item.object);
    if (*author_count != object->len) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(77, node, depth, sem, __func__, "author_count: %d != object len: %d",
				    *author_count, object->len);
	}
	return false;
    }

    /* XXX - more code here */

    /*
     * return validation success
     */
    if (val_err != NULL) {
	*val_err = NULL;
    }
    return true;
}


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
	    *val_err = werr_sem_val(49, node, depth, sem, __func__, "invalid rule_2a_override");
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
	    *val_err = werr_sem_val(50, node, depth, sem, __func__, "invalid rule_2a_mismatch");
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
	    *val_err = werr_sem_val(51, node, depth, sem, __func__, "invalid rule_2b_override");
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
	    *val_err = werr_sem_val(52, node, depth, sem, __func__, "invalid highbit_warning");
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
	    *val_err = werr_sem_val(53, node, depth, sem, __func__, "invalid nul_warning");
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
	    *val_err = werr_sem_val(54, node, depth, sem, __func__, "invalid trigraph_warning");
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
	    *val_err = werr_sem_val(55, node, depth, sem, __func__, "invalid wordbuf_warning");
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
	    *val_err = werr_sem_val(56, node, depth, sem, __func__, "invalid ungetc_warning");
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
	    *val_err = werr_sem_val(57, node, depth, sem, __func__, "invalid first_rule_is_all");
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
	    *val_err = werr_sem_val(58, node, depth, sem, __func__, "invalid found_all_rule");
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
	    *val_err = werr_sem_val(59, node, depth, sem, __func__, "invalid found_clean_rule");
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
	    *val_err = werr_sem_val(60, node, depth, sem, __func__, "invalid found_clobber_rule");
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
	    *val_err = werr_sem_val(61, node, depth, sem, __func__, "invalid found_try_rule");
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
	    *val_err = werr_sem_val(62, node, depth, sem, __func__, "invalid test_mode");
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
