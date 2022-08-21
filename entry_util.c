/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * entry_util - utility functions supporting mkiocccentry JSON files
 *
 * JSON related functions to support formation of .info.json files
 * and .author.json files, their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * As per IOCCC anonymous judging policy, the calls to json_dbg() in this file
 * that are JSON_DBG_MED or lower will NOT reveal any JSON content.
 * Only at JSON_DBG_HIGH or higher should json_dbg() calls in the file
 * will print JSON content.
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
 * Share and vomit on the JSON spec! :-)
 */


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <stdlib.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "entry_util.h"


/*
 * While this repo does not officially support pre-c11 systems that have
 * problems with the timegm() function nopt being declared in <time.h>.
 *
 * The work-a-round / gross hack below as a mild attempt to make such systems work.
 *
 * If your pre-c11 system fails to compile this code, we apologize and
 * request that you compile this repo on a more up to date system such as
 * a system that fully support c11 or later.
 */
#if defined(TIMEGM_PROBLEM)
extern time_t timegm(struct tm *timeptr);	/* work-a-round / gross hack */
#endif


/*
 * free_auth - free auto and related sub-elements
 *
 * given:
 *      authp   - pointer to auth structure to free
 *
 * This function does not return on error.
 */
void
free_auth(struct auth *authp)
{
    /*
     * firewall
     */
    if (authp == NULL) {
	err(50, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free file format strings
     */
    /* NOTE: no_comment is a compiled in constant */
    /* NOTE: author_version is a compiled in constant */
    /* NOTE: ioccc_contest is a compiled in constant */

    /*
     * free IOCCC tool versions
     */
    /* NOTE: mkiocccentry_ver is a compiled in constant */
    /* NOTE: chkentry_ver is a compiled in constant */
    /* NOTE: fnamchk_ver is a compiled in constant */
    /* NOTE: iocccsize_ver is a compiled in constant */
    /* NOTE: txzchk_ver is a compiled in constant */

    /*
     * free entry information
     */
    if (authp->ioccc_id != NULL) {
	free(authp->ioccc_id);
	authp->ioccc_id = NULL;
    }
    if (authp->tarball != NULL) {
	free(authp->tarball);
	authp->tarball = NULL;
    }

    /*
     * free author set
     */
    free_author_array(authp->author, authp->author_count);

    /*
     * free time values
     */
    /* NOTE: epoch is a compiled in constant */
    if (authp->utctime != NULL) {
	free(authp->utctime);
	authp->utctime = NULL;
    }

    /*
     * zeroize the auth structure
     */
    memset(authp, 0, sizeof(*authp));
    return;
}


/*
 * free_info - free info and related sub-elements
 *
 * given:
 *      infop   - pointer to info structure to free
 *
 * This function does not return on error.
 */
void
free_info(struct info *infop)
{
    int i;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(51, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free file format strings
     */
    /* NOTE: no_comment is a compiled in constant */
    /* NOTE: author_version is a compiled in constant */
    /* NOTE: ioccc_contest is a compiled in constant */

    /*
     * free IOCCC tool versions
     */
    /* NOTE: mkiocccentry_ver is a compiled in constant */
    /* NOTE: chkentry_ver is a compiled in constant */
    /* NOTE: fnamchk_ver is a compiled in constant */
    /* NOTE: iocccsize_ver is a compiled in constant */
    /* NOTE: txzchk_ver is a compiled in constant */

    /*
     * free entry information
     */
    if (infop->ioccc_id != NULL) {
	free(infop->ioccc_id);
	infop->ioccc_id = NULL;
    }
    if (infop->title != NULL) {
	free(infop->title);
	infop->title = NULL;
    }
    if (infop->abstract != NULL) {
	free(infop->abstract);
	infop->abstract = NULL;
    }
    if (infop->tarball != NULL) {
	free(infop->tarball);
	infop->tarball = NULL;
    }

    /*
     * free file name array
     */
    /* NOTE: info_file is a compiled in constant */
    /* NOTE: author_file is a compiled in constant */
    if (infop->prog_c != NULL) {
	free(infop->prog_c);
	infop->prog_c = NULL;
    }
    if (infop->Makefile != NULL) {
	free(infop->Makefile);
	infop->Makefile = NULL;
    }
    if (infop->remarks_md != NULL) {
	free(infop->remarks_md);
	infop->remarks_md = NULL;
    }
    if (infop->extra_file != NULL) {
	for (i = 0; i < infop->extra_count; ++i) {
	    if (infop->extra_file[i] != NULL) {
		free(infop->extra_file[i]);
		infop->extra_file[i] = NULL;
	    }
	}
	free(infop->extra_file);
	infop->extra_file = NULL;
    }

    /*
     * free time values
     */
    /* NOTE: epoch is a compiled in constant */
    if (infop->utctime != NULL) {
	free(infop->utctime);
	infop->utctime = NULL;
    }

    /*
     * zeroize the info structure
     */
    memset(infop, 0, sizeof(*infop));
    return;
}


/*
 * free_author_array - free storage related to a struct author *
 *
 * given:
 *      author_set      - pointer to a struct author array
 *      author_count    - length of author array
 */
void
free_author_array(struct author *author_set, int author_count)
{
    int i;

    /*
     * firewall
     */
    if (author_set == NULL) {
	err(52, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(53, __func__, "author_count: %d < 0", author_count);
	not_reached();
    }

    /*
     * free elements of each array member
     */
    for (i = 0; i < author_count; ++i) {

	/*
	 * free IOCCC author information
	 */
	if (author_set[i].name != NULL) {
	    free(author_set[i].name);
	    author_set[i].name = NULL;
	}
	if (author_set[i].location_code != NULL) {
	    free(author_set[i].location_code);
	    author_set[i].location_code = NULL;
	}
	if (author_set[i].email != NULL) {
	    free(author_set[i].email);
	    author_set[i].email = NULL;
	}
	if (author_set[i].url != NULL) {
	    free(author_set[i].url);
	    author_set[i].url = NULL;
	}
	if (author_set[i].twitter != NULL) {
	    free(author_set[i].twitter);
	    author_set[i].twitter = NULL;
	}
	if (author_set[i].github != NULL) {
	    free(author_set[i].github);
	    author_set[i].github = NULL;
	}
	if (author_set[i].affiliation != NULL) {
	    free(author_set[i].affiliation);
	    author_set[i].affiliation = NULL;
	}
	if (author_set[i].author_handle != NULL) {
	    free(author_set[i].author_handle);
	    author_set[i].author_handle = NULL;
	}

	/*
	 * zeroize the author structure
	 */
	memset(&(author_set[i]), 0, sizeof(author_set[i]));
    }
    return;
}


/*
 * object2author - load a struct author with validated elements
 *
 * In .author.json, we are required to find a JTYPE_MEMBER named "authors".
 * The value of this "authors" JTYPE_MEMBER is a JSON array (JTYPE_ARRAY).
 * That JSON array consists of an array of JSON objects (JTYPE_OBJECT),
 * each containing JSON members (JTYPE_MEMBER) with IOCCC author information.
 *
 * Given a node that is a JSON object (JTYPE_OBJECT) (from the JSON array),
 * we will convert a node that is a JSON object (JTYPE_OBJECT) containing
 * an array of JSON members (JTYPE_MEMBER) with author information,
 * validate the values of the JTYPE_MEMBER and then load that value
 * into a struct author.
 *
 * If we find a JSON member name under the node that is not part of the
 * IOCCC author information, we will return false.  If the JSON member value
 * is invalid, we will return false.
 *
 * given:
 *	node	JSON parse node being checked
 *	depth	depth of node in the JSON parse tree (0 ==> tree root)
 *	sem	JSON semantic node triggering the check
 *	name	name of caller function (NULL ==> "((NULL))")
 *	val_err	pointer to address where to place a JSON semantic validation error,
 *		NULL ==> do not report a JSON semantic validation error
 *	auth	pointer to a struct author to fill out
 *	auth_num	author number
 *
 * returns:
 *	true ==> struct author was loaded with validated elements
 *	false ==> invalid member value found, auth is NULL, or some internal error
 */
bool
object2author(struct json *node, unsigned int depth, struct json_sem *sem,
	      char const *name, struct json_sem_val_err **val_err,
	      struct author *auth, int auth_num)
{
    char *auth_name = NULL;		/* name of the author */
    bool found_name = false;		/* true ==> found name in node */
    char *location_code = NULL;		/* author location/country code */
    bool found_location_code = false;	/* true ==> found location_code in node */
    char const *location_name = NULL;	/* name of author location/country (compiled in from loc[]) */
    bool found_location_name = false;	/* true ==> found location_name in node */
    char *email = NULL;			/* Email address of author or NULL ==> not provided */
    bool found_email = false;		/* true ==> found email in node */
    char *url = NULL;			/* home URL of author or NULL ==> not provided */
    bool found_url = false;		/* true ==> found url in node */
    char *twitter = NULL;		/* author twitter handle or NULL ==> not provided */
    bool found_twitter = false;		/* true ==> found twitter in node */
    char *github = NULL;		/* author GitHub username or NULL ==> not provided */
    bool found_github = false;		/* true ==> found github in node */
    char *affiliation = NULL;		/* author affiliation or NULL ==> not provided */
    bool found_affiliation = false;	/* true ==> found affiliation in node */
    bool past_winner = false;		/* true ==> author claims to have won before */
					/* false ==> author claims not a prev winner */
    bool found_past_winner = false;	/* true ==> found past_winner in node */
    bool default_handle = false;	/* true ==> default author_handle accepted */
					/* false ==> author_handle entered */
    bool found_default_handle = false;	/* true ==> found default_handle in node */
    char *author_handle = NULL;		/* IOCCC author handle (for winning entries) */
    bool found_author_handle = false;	/* true ==> found author_handle in node */
    int author_number = -1;		/* author number */
    bool found_author_number = false;	/* true ==> found author_number in node */
    struct json_object *obj = NULL;	/* JSON node as JTYPE_OBJECT */
    int obj_len = 0;			/* length in JTYPE_MEMBER of the JSON node as JTYPE_OBJECT */
    bool test = false;			/* validation test result */
    struct str_or_null val_or_null;	/* JTYPE_MEMBER value that can be a JTYPE_STRING or JTYPE_NULL */
    bool *bool_val = NULL;		/* pointer to a converted JTYPE_BOOL */
    int *int_val = NULL;		/* pointer to a converted JTYPE_NUMNER as int */
    int i;

    /*
     * firewall - must not be NULL
     */
    if (auth == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(54, node, depth, sem, name,
				    "author array index[%d]: auth is NULL", auth_num);
	}
	return false;
    }

    /*
     * firewall - validate JTYPE_MEMBER item in the JTYPE_ARRAY
     */
    test = sem_node_valid_converted(node, depth, sem, __func__, val_err);
    if (test == false) {
	/* sem_node_valid_converted() will have set *val_err */
	return false;
    }
    if (node->type != JTYPE_OBJECT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(55, node, depth, sem, __func__,
				    "author array index[%d] type %s != JTYPE_OBJECT",
				    auth_num, json_type_name(node->type));
	}
	return false;
    }
    obj = &(node->item.object);
    obj_len = obj->len;
    if (obj_len <= 0) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(56, node, depth, sem, __func__,
				    "author array index[%d] length: %d <= 0",
				    auth_num, obj_len);
	}
	return false;
    }
    if (obj->set == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(57, node, depth, sem, __func__,
				    "author array index[%d] set is NULL", auth_num);
	}
	return false;
    }

    /*
     * examine each JTYPE_MEMBER of the JTYPE_OBJECT
     */
    for (i=0; i < obj_len; ++i) {
	struct json *e = obj->set[i];		/* next item in the JTYPE_OBJECT */
	char *name = NULL;			/* name string of name part of JTYPE_MEMBER */

	/*
	 * firewall - validate JTYPE_MEMBER item in the JTYPE_OBJECT
	 */
	test = sem_node_valid_converted(e, depth+1, sem, __func__, val_err);
	if (test == false) {
	    /* sem_node_valid_converted() will have set *val_err */
	    return false;
	}
	if (e->type != JTYPE_MEMBER) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(58, node, depth, sem, __func__,
					"author array index[%d] JTYPE_OBJECT[%d] type %s != JTYPE_MEMBER",
					auth_num, i, json_type_name(e->type));
	    }
	    return false;
	}
	name = sem_member_name_decoded_str(e, depth+2, sem, __func__, val_err);
	if (name == NULL) {
	    /* sem_member_name_decoded_str() will have set *val_err */
	    return false;
	}

	/*
	 * look for required JTYPE_MEMBER names in an IOCCC author's JTYPE_OBJECT
	 *
	 * NOTE: If the JTYPE_MEMBER value can be a JSON null, then we will leave
	 *	 the associated string value as NULL and convert it to an empty
	 *	 string later on in this function.
	 */
	/* case: IOCCC author nmae */
	if (strcmp(name, "name") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_name == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(59, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_name = true;

	    /* obtain value as JTYPE_STRING */
	    auth_name = sem_member_value_decoded_str(node, depth+2, sem, __func__, NULL);
	    /* we will deal with NULL auth_name later in this function */

	/* case: IOCCC author location_code */
	} else if (strcmp(name, "location_code") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_location_code == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(60, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_location_code = true;

	    /* obtain value as JTYPE_STRING */
	    location_code = sem_member_value_decoded_str(node, depth+2, sem, __func__, NULL);
	    /* we will deal with NULL location_code later in this function */

	/* case: IOCCC author location_name */
	} else if (strcmp(name, "location_name") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_location_name == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(61, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_location_name = true;

	    /* obtain value as JTYPE_STRING */
	    location_name = sem_member_value_decoded_str(node, depth+2, sem, __func__, NULL);
	    /* we will deal with NULL location_name later in this function */

	/* case: IOCCC author email */
	} else if (strcmp(name, "email") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_email == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(62, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_email = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(63, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    auth_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == false) {
		email = val_or_null.str;
	    }

	/* case: IOCCC author url */
	} else if (strcmp(name, "url") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_url == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(64, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_url = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(65, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    auth_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == false) {
		url = val_or_null.str;
	    }

	/* case: IOCCC author twitter */
	} else if (strcmp(name, "twitter") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_twitter == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(66, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		    }
		return false;
	    }
	    found_twitter = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(67, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    auth_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == false) {
		twitter = val_or_null.str;
	    }

	/* case: IOCCC author github */
	} else if (strcmp(name, "github") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_github == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(68, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_github = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(69, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    auth_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == false) {
		github = val_or_null.str;
	    }

	/* case: IOCCC author affiliation */
	} else if (strcmp(name, "affiliation") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_affiliation == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(70, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_affiliation = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(71, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    auth_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == false) {
		affiliation = val_or_null.str;
	    }

	/* case: IOCCC author past_winner */
	} else if (strcmp(name, "past_winner") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_past_winner == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(72, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_past_winner = true;

	    /* obtain value as JTYPE_BOOL */
	    bool_val = sem_member_value_bool(e, depth+2, sem, __func__, NULL);
	    if (bool_val == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(73, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid JSON boolean",
					    auth_num, i);
		}
		return false;
	    }
	    past_winner = *bool_val;

	/* case: IOCCC author default_handle */
	} else if (strcmp(name, "default_handle") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_default_handle == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(74, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_default_handle = true;

	    /* obtain value as JTYPE_BOOL */
	    bool_val = sem_member_value_bool(e, depth+2, sem, __func__, NULL);
	    if (bool_val == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(75, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid JSON boolean",
					    auth_num, i);
		}
		return false;
	    }
	    default_handle = *bool_val;

	/* case: IOCCC author author_handle */
	} else if (strcmp(name, "author_handle") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_author_handle == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(76, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_author_handle = true;

	    /* obtain value as JTYPE_STRING */
	    author_handle = sem_member_value_decoded_str(node, depth+2, sem, __func__, NULL);
	    /* we will deal with NULL author_handle later in this function */

	/* case: IOCCC author author_number */
	} else if (strcmp(name, "author_number") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_author_number == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(77, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found duplicate <%s>",
					    auth_num, i, name);
		}
		return false;
	    }
	    found_author_number = true;

	    /* obtain value as JTYPE_NUMBER as int */
	    int_val = sem_member_value_int(node, depth+2, sem, __func__, NULL);
	    if (int_val == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(78, node, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid author number",
					    auth_num, i);
		}
		return false;
	    }
	    author_number = *int_val;

	/* case: invalid JTYPE_MEMBER - not part of an IOCCC author's JTYPE_OBJECT */
	} else {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(79, node, depth+2, sem, __func__,
					"author array index[%d] JTYPE_OBJECT has invalid JTYPE_MEMBER name: <%s>",
					i, name);
	    }
	    return false;
	}
    }

    /*
     * looking for missing information in the JSON object
     */
    if (found_name == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(80, node, depth, sem, __func__,
				    "author array index[%d]: missing __func__", auth_num);
	}
	return false;
    }
    if (found_location_code == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(81, node, depth, sem, __func__,
				    "author array index[%d]: missing location_code", auth_num);
	}
	return false;
    }
    if (found_location_name == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(82, node, depth, sem, __func__,
				    "author array index[%d]: missing location_name", auth_num);
	}
	return false;
    }
    if (found_email == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(83, node, depth, sem, __func__,
				    "author array index[%d]: missing email", auth_num);
	}
	return false;
    }
    if (found_url == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(84, node, depth, sem, __func__,
				    "author array index[%d]: missing url", auth_num);
	}
	return false;
    }
    if (found_twitter == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(85, node, depth, sem, __func__,
				    "author array index[%d]: missing twiter", auth_num);
	}
	return false;
    }
    if (found_github == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(86, node, depth, sem, __func__,
				    "author array index[%d]: missing github", auth_num);
	}
	return false;
    }
    if (found_affiliation == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(87, node, depth, sem, __func__,
				    "author array index[%d]: missing affiliation", auth_num);
	}
	return false;
    }
    if (found_past_winner == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(88, node, depth, sem, __func__,
				    "author array index[%d]: missing past_winner", auth_num);
	}
	return false;
    }
    if (found_default_handle == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(89, node, depth, sem, __func__,
				    "author array index[%d]: missing default_handle", auth_num);
	}
	return false;
    }
    if (found_author_handle == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(90, node, depth, sem, __func__,
				    "author array index[%d]: missing author_handle", auth_num);
	}
	return false;
    }
    if (found_author_number == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(91, node, depth, sem, __func__,
				    "author array index[%d]: missing author_number", auth_num);
	}
	return false;
    }

    /*
     * warn if elements are NULL that must not be NULL
     * convert allowed NULL values into empty strings
     *
     * NOTE: If the JTYPE_MEMBER value can be a JSON null, then we
     *	     convert it here into an empty string.
     */
    if (auth_name == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(92, node, depth, sem, __func__,
				    "author array index[%d]: __func__ is NULL", auth_num);
	}
	return false;
    }
    if (location_code == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(93, node, depth, sem, __func__,
				    "author array index[%d]: location_code is NULL", auth_num);
	}
	return false;
    }
    if (location_name == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(94, node, depth, sem, __func__,
				    "author array index[%d]: location_name is NULL", auth_num);
	}
	return false;
    }
    if (email == NULL) {
	email = "";
    }
    if (url == NULL) {
	url = "";
    }
    if (twitter == NULL) {
	twitter = "";
    }
    if (github == NULL) {
	github = "";
    }
    if (affiliation == NULL) {
	affiliation = "";
    }
    if (author_handle == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(95, node, depth, sem, __func__,
				    "author array index[%d]: author_handle is NULL", auth_num);
	}
	return false;
    }

    /*
     * validate elements
     */
#if 0 /* XXX - unblock once the test function exists - XXX */
    if (test_name(auth_name) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(96, node, depth, sem, __func__,
				    "author array index[%d]: author __func__ is invalid", auth_num);
	}
	return false;
    }
#endif /* XXX - unblock once the test function exists - XXX */
    if (test_location_code(location_code) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(97, node, depth, sem, __func__,
				    "author array index[%d]: location_code is invalid", auth_num);
	}
	return false;
    }
#if 0 /* XXX - unblock once the test function exists - XXX */
    if (test_location_name(location_name) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(98, node, depth, sem, __func__,
				    "author array index[%d]: location_name is invalid", auth_num);
	}
	return false;
    }
#endif /* XXX - unblock once the test function exists - XXX */
    if (test_email(email) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(99, node, depth, sem, __func__,
				    "author array index[%d]: location_name is invalid", auth_num);
	}
	return false;
    }

    if (test_url(url) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(100, node, depth, sem, __func__,
				    "author array index[%d]: url is invalid", auth_num);
	}
	return false;
    }

    if (test_twitter(twitter) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(101, node, depth, sem, __func__,
				    "author array index[%d]: twitter is invalid", auth_num);
	}
	return false;
    }
    if (test_github(github) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(102, node, depth, sem, __func__,
				    "author array index[%d]: github is invalid", auth_num);
	}
	return false;
    }
    if (test_affiliation(affiliation) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(103, node, depth, sem, __func__,
				    "author array index[%d]: affiliation is invalid", auth_num);
	}
	return false;
    }
#if 0 /* XXX - unblock once the test function exists - XXX */
    if (test_past_winner(past_winner) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(104, node, depth, sem, __func__,
				    "author array index[%d]: past_winner is invalid", auth_num);
	}
	return false;
    }
#endif /* XXX - unblock once the test function exists - XXX */
    if (test_default_handle(default_handle) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(105, node, depth, sem, __func__,
				    "author array index[%d]: default_handle is invalid", auth_num);
	}
	return false;
    }
    if (test_author_handle(author_handle) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(106, node, depth, sem, __func__,
				    "author array index[%d]: author_handle is invalid", auth_num);
	}
	return false;
    }
    if (test_author_number(author_number) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(107, node, depth, sem, __func__,
				    "author array index[%d]: author_number is invalid", auth_num);
	}
	return false;
    }

    /*
     * load elements into struct author * (auth)
     *
     * For strings, we strdup() them.
     */
    errno = 0;		/* pre-clear errno for warnp() */
    auth->name = strdup(auth_name);
    if (auth->name == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(108, node, depth, sem, __func__,
				    "author array index[%d]: strdup of name failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for warnp() */
    auth->location_code = strdup(location_code);
    if (auth->location_code == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(109, node, depth, sem, __func__,
				    "author array index[%d]: strdup of location_code failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for warnp() */
    auth->location_name = strdup(location_name);
    if (auth->location_name == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(110, node, depth, sem, __func__,
				    "author array index[%d]: strdup of location_name failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for warnp() */
    auth->email = strdup(email);
    if (auth->email == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(111, node, depth, sem, __func__,
				    "author array index[%d]: strdup of email failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for warnp() */
    auth->url = strdup(url);
    if (auth->url == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(112, node, depth, sem, __func__,
				    "author array index[%d]: strdup of url failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for warnp() */
    auth->twitter = strdup(twitter);
    if (auth->url == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(113, node, depth, sem, __func__,
				    "author array index[%d]: strdup of twitter failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for warnp() */
    auth->github = strdup(github);
    if (auth->url == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(114, node, depth, sem, __func__,
				    "author array index[%d]: strdup of github failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for warnp() */
    auth->affiliation = strdup(affiliation);
    if (auth->affiliation == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(115, node, depth, sem, __func__,
				    "author array index[%d]: strdup of affiliation failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    auth->past_winner = past_winner;
    auth->default_handle = default_handle;
    errno = 0;		/* pre-clear errno for warnp() */
    auth->author_handle = strdup(author_handle);
    if (auth->location_code == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(116, node, depth, sem, __func__,
				    "author array index[%d]: strdup of author_handle failed", auth_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    auth->author_num = author_number;

    /*
     * report success in loading struct author
     */
    return true;
}


/*
 * timestr_eq_tstamp - determine if a time string matches a timestamp
 *
 * given:
 *	timestr		a time string formatted by "%a %b %d %H:%M:%S %Y UTC"
 *	timestamp	timestamp as a time_t to compare
 *
 * returns:
 *	true ==> time string is the same time as timestamp,
 *	false ==> time string differs in time from timestamp, or
 *		  time string is invalid format, or internal error
 */
bool
timestr_eq_tstamp(char *timestr, time_t timestamp)
{
    struct tm timeptr;			/* formed_UTC converted into broken-out time */
    char *ptr = NULL;			/* ptr to 1st char in buf not converted */
    time_t timestr_as_time_t;		/* timestr as a timestamp */

    /*
     * firewall
     */
    if (timestr == NULL) {
	warn(__func__, "timestr is NULL");
	return false;
    }

    /*
     * convert into broken-out time
     */
    ptr = strptime(timestr, "%a %b %d %H:%M:%S %Y UTC", &timeptr);
    if (ptr == NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: strptime cannot convert time string");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: strptime failed convert time string: <%s>", timestr);
	return false;
    }
    if (ptr[0] == '\0') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra data in time string");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: extra data in time string: <%s>", timestr);
	return false;
    }

    /*
     * convert broken-out time into timestamp
     */
    timestr_as_time_t = timegm(&timeptr);

    /*
     * compare timestamps
     */
    if (timestr_as_time_t != timestamp) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: time string not same time as timestamp");
	if ((time_t)-1 > 0) {
	    /* case: unsigned time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: time string: <%s> %ju != timestamp: %ju",
		     timestr, (uintmax_t)timestr_as_time_t, (uintmax_t)timestamp);
	} else {
	    /* case: unsigned time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: time string: <%s> %jd != timestamp: %jd",
		     timestr, (intmax_t)timestr_as_time_t, (intmax_t)timestamp);
	}
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "time string same time as timestamp");
    return true;
}


/* XXX - begin sorted order matching chk_validate.c here - XXX */


/*
 * test_IOCCC_author_version - test if IOCCC_author_version is valid
 *
 * Determine if IOCCC_author_version matches AUTHOR_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_IOCCC_author_version(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, AUTHOR_VERSION) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: IOCCC_author_version != AUTHOR_VERSION: %s", AUTHOR_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: IOCCC_author_version: %s is not AUTHOR_VERSION: %s", str, AUTHOR_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "IOCCC_author_version is valid");
    return true;
}


/*
 * test_IOCCC_contest_id - test if IOCCC_contest_id is valid
 *
 * Determine if IOCCC_contest_id is test or a UUID version 4 variant 1.
 *
 * The contest ID, if not "test" must be a UUID.  The UUID has the 36 character format:
 *
 *             xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx
 *
 * where 'x' is a hex character.  The 4 is the UUID version and the variant 1.
 *
 * Example:
 *
 *	12345678-1234-4321-abcd-1234567890ab
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_IOCCC_contest_id(char *str)
{
    size_t len;				/* string length */
    int ret;				/* libc function return */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;		/* UUID version hex character */
    unsigned int variant = 0;		/* UUID variant hex character */
    char guard;				/* scanf guard to catch excess amount of input */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * case: test IOCCC_contest_id
     */
    if (strcmp(str, "test") == 0) {
	json_dbg(JSON_DBG_MED, __func__, "test IOCCC_contest_id");
	return true;
    }

    /*
     * validate UUID version 4 variant 1
     */
    len = strlen(str);
    if (len != UUID_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: non-test IOCCC_contest_id len != UUID_LEN: %d", UUID_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: non-test IOCCC_contest_id len: %zd doesn't match UUID_LEN: %d", len, UUID_LEN);
	return false;
    }
    /* validate UUID string, version and variant */
    ret = sscanf(str, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x%c", &a, &b, &version, &c, &variant, &d, &e, &f, &guard);
    if (ret != 8) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: did not find 8 fields when parsing IOCCC_contest_id");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: field count: %d != 8", ret);
	return false;
    }
    if (version != UUID_VERSION) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: UUID version hex char != UUID_VERSION: %x", UUID_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: UUID version hex char: %1x is not UUID_VERSION: %x", version, UUID_VERSION);
	return false;
    }
    if (variant != UUID_VARIANT) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: UUID variant hex char != UUID_VARIANT: %x", UUID_VARIANT);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: UUID variant hex char: %1x is not UUID_VARIANT: %x", variant, UUID_VARIANT);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "IOCCC_contest_id is valid");
    return true;
}


/*
 * test_IOCCC_info_version - test if IOCCC_info_version is valid
 *
 * Determine if IOCCC_info_version matches INFO_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_IOCCC_info_version(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, INFO_VERSION) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: IOCCC_info_version != INFO_VERSION: %s", INFO_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: IOCCC_info_version: %s is not INFO_VERSION: %s", str, INFO_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "IOCCC_info_version is valid");
    return true;
}


/*
 * test_Makefile - test if Makefile filename is valid
 *
 * Determine if Makefile matches "Makefile".
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_Makefile(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, "Makefile") != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: Makefile != Makefile: %s", "Makefile");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: Makefile: %s is not Makefile: %s", str, "Makefile");
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "Makefile filename is valid");
    return true;
}


/*
 * test_Makefile_override - test if Makefile_override is valid
 *
 * Determine if Makefile_override boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_Makefile_override(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "Makefile_override is %s", booltostr(boolean));
    return true;
}


/*
 * test_abstract - test if abstract is valid
 *
 * Determine if abstract length is <= MAX_ABSTRACT_LEN.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_abstract(char *str)
{
    size_t length = 0;		/* length of string */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    length = strlen(str);
    /* check for a valid length */
    if (length <= 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty abstract");
	return false;
    } else if (length > MAX_ABSTRACT_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: abstract length %ju > max %d", (uintmax_t)length, MAX_ABSTRACT_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: abstract: <%s> is invalid", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "abstract is valid");
    return true;
}


/*
 * test_affiliation - test if affiliation is valid
 *
 * Determine if affiliation length is <= MAX_AFFILIATION_LEN.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_affiliation(char *str)
{
    size_t length = 0;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    /* check for a valid length */
    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty affiliation");
	return false;
    }
    length = strlen(str);
    if (length > MAX_AFFILIATION_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: affiliation length %ju > max %d", (uintmax_t)length, MAX_AFFILIATION_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: affiliation: <%s> is invalid", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "affiliation is valid");
    return true;
}


/*
 * test_author_JSON - test if author_JSON is valid
 *
 * Determine if author_JSON matches AUTHOR_JSON_FILENAME.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_author_JSON(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, AUTHOR_JSON_FILENAME) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_JSON != AUTHOR_JSON_FILENAME: %s", AUTHOR_JSON_FILENAME);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: author_JSON: %s is not AUTHOR_JSON_FILENAME: %s", str, AUTHOR_JSON_FILENAME);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "author_JSON is valid");
    return true;
}


/*
 * test_author_count - test if author_count is valid
 *
 * Determine if author_count is within the proper limits.
 *
 * given:
 *	author_count	number of authors
 *
 * returns:
 *	true ==> author_count is valid,
 *	false ==> author_count is NOT valid, or some internal error
 */
bool
test_author_count(int author_count)
{
    /*
     * validate count
     */
    if (author_count < 1) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_count: %d < 0", author_count);
	return false;
    } else if (author_count > MAX_AUTHORS) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_count: %d > MAX_AUTHORS: %d", author_count, MAX_AUTHORS);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "author_count is valid");
    return true;
}


/*
 * test_author_handle - test if author_handle is valid
 *
 * Determine if author_handle is a valid length and contains only allowed handle chars.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_author_handle(char *str)
{
    size_t length = 0;		/* length of string */
    bool test = false;		/* character test result */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    length = strlen(str);
    /* check for a valid length */
    if (length <= 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty abstract");
	return false;
    } else if (length > MAX_HANDLE) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_handle length %ju > max %d", (uintmax_t)length, MAX_HANDLE);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: author_handle: <%s> is invalid", str);
	return false;
    }
    /* IOCCC author handle must use only lower case POSIX portable filename and + chars */
    test = posix_plus_safe(str, false, false, true);
    if (test == false) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_handle does not match regexp: ^[0-9a-z][0-9a-z._+-]*$");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: author_handle: <%s> contains invalid characters", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "author_handle is valid");
    return true;
}


/*
 * test_author_number - test if author_number is valid
 *
 * Determine if author_number is within the proper limits.
 *
 * NOTE: The function does NOT determine if the author_number is unique among the authors.
 * NOTE: The function does NOT determine if the author_number is <= author_count.
 *
 * given:
 *	author_number	author number
 *
 * returns:
 *	true ==> author_number is valid,
 *	false ==> author_number is NOT valid, or some internal error
 */
bool
test_author_number(int author_number)
{
    /*
     * validate count
     */
    if (author_number < 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_number: %d < 0", author_number);
	return false;
    } else if (author_number >= MAX_AUTHORS) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_number: %d > MAX_AUTHORS: %d", author_number, MAX_AUTHORS);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "author_number is valid");
    return true;
}


/*
 * test_authors - test is the authors array contains unique authors
 *
 * We verify that the authors array contains author numbers that are unique
 * and bound in the range [0,author_count].
 *
 * We verify that the authors array contains author names that are unique.
 *
 * We verify that the authors array contains author handles that are unique.
 *
 * given:
 *	author_count    - length of the author structure array in elements
 *	authorp         - pointer to author structure array
 *
 * returns:
 *	true ==> authors array contains unique authors
 *	false ==> authors array contains non-unique authors, or NULL pointer, or some internal error
 */
bool
test_authors(int author_count, struct author *authorp)
{
    bool test = false;		/* character test result */
    int auth_num = -1;		/* author number */
    int *auth_nums = NULL;	/* array of author numbers */
    int i;
    int j;

    /*
     * firewall
     */
    if (authorp == NULL) {
	warn(__func__, "authorp is NULL");
	return false;
    }
    test = test_author_count(author_count);
    if (test == false) {
	warn(__func__, "author_count is invalid");
	return false;
    }

    /*
     * initialize a zeroized array of author numbers
     */
    errno = 0;		/* pre-clear errno for warnp() */
    auth_nums = calloc(author_count, sizeof(int));
    if (auth_nums == NULL) {
	warnp(__func__, "calloc of %d ints failed", author_count);
	return false;
    }

    /*
     * check each author number
     */
    for (i=0; i < author_count; ++i) {

	/*
	 * bound check i-th author's author number
	 */
	auth_num = authorp[i].author_num;
	if (auth_num < 0) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] number: %d < 0", i, auth_num);
	    /* free array of author numbers */
	    if (auth_nums != NULL) {
		free(auth_nums);
		auth_nums = NULL;
	    }
	    return false;
	} else if (auth_num > author_count) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: number[%d] number: %d > author_count: %d", i, auth_num, author_count);
	    /* free array of author numbers */
	    if (auth_nums != NULL) {
		free(auth_nums);
		auth_nums = NULL;
	    }
	    return false;
	}

	/*
	 * check the i-th author for a unique author number
	 */
	if (auth_nums[auth_num] != 0) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] number: %d is not unique, duplicate of author[%d]",
		     i, auth_num, auth_nums[auth_num]);
	    /* free array of author numbers */
	    if (auth_nums != NULL) {
		free(auth_nums);
		auth_nums = NULL;
	    }
	    return false;
	}
	auth_nums[auth_num] = i;

	/*
	 * pre-check for non-NULL author_handle (for the uniqueness check below)
	 */
	if (authorp[i].author_handle == NULL) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] handle is NUL:", i);
	    /* free array of author numbers */
	    if (auth_nums != NULL) {
		free(auth_nums);
		auth_nums = NULL;
	    }
	    return false;
	}

	/*
	 * pre-check for non-NULL name (for the uniqueness check below)
	 */
	if (authorp[i].name == NULL) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] name is NUL:", i);
	    /* free array of author numbers */
	    if (auth_nums != NULL) {
		free(auth_nums);
		auth_nums = NULL;
	    }
	    return false;
	}
    }

    /*
     * check for the uniqueness of each author handle
     *
     * The author_count will be a small number, so we can get away with a lazy O(n^2) match.
     */
    for (i=1; i < author_count; ++i) {
	for (j=0; j < i; ++j) {
	    if (strcmp(authorp[i].author_handle, authorp[j].author_handle) == 0) {
		json_dbg(JSON_DBG_MED, __func__,
			 "invalid: author[%d] handle matches author[%d] handle",
			 i, j);
		json_dbg(JSON_DBG_HIGH, __func__,
			 "invalid: author[%d] handle: <%s> == author[%d] handle: <%s>",
			 i, authorp[i].author_handle, j, authorp[j].author_handle);
		/* free array of author numbers */
		if (auth_nums != NULL) {
		    free(auth_nums);
		    auth_nums = NULL;
		}
		return false;
	    }
	}
    }

    /*
     * check for the uniqueness of each author handle
     *
     * The author_count will be a small number, so we can get away with a lazy O(n^2) match.
     */
    for (i=1; i < author_count; ++i) {
	for (j=0; j < i; ++j) {
	    if (strcmp(authorp[i].name, authorp[j].name) == 0) {
		json_dbg(JSON_DBG_MED, __func__,
			 "invalid: author[%d] name matches author[%d] name",
			 i, j);
		json_dbg(JSON_DBG_HIGH, __func__,
			 "invalid: author[%d] name: <%s> == author[%d] name: <%s>",
			 i, authorp[i].name, j, authorp[j].name);
		/* free array of author numbers */
		if (auth_nums != NULL) {
		    free(auth_nums);
		    auth_nums = NULL;
		}
		return false;
	    }
	}
    }

    /*
     * return success
     */
    /* free array of author numbers */
    if (auth_nums != NULL) {
	free(auth_nums);
	auth_nums = NULL;
    }
    json_dbg(JSON_DBG_MED, __func__, "author numbers and names are unique and in range");
    return true;
}


/*
 * test_c_src - test if c_src is valid
 *
 * Determine if c_src matches "prog.c".
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_c_src(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, "prog.c") != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: c_src != prog.c: %s", "prog.c");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: c_src: %s is not prog.c: %s", str, "prog.c");
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "c_src filename is valid");
    return true;
}


/*
 * test_chkentry_version - test if chkentry_version is valid
 *
 * Determine if chkentry_version matches CHKENTRY_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_chkentry_version(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, CHKENTRY_VERSION) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: chkentry_version != CHKENTRY_VERSION: %s", CHKENTRY_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: chkentry_version: %s is not CHKENTRY_VERSION: %s", str, CHKENTRY_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "chkentry_version is valid");
    return true;
}


/*
 * test_default_handle - test if default_handle is valid
 *
 * Determine if default_handle boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_default_handle(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "default_handle is %s", booltostr(boolean));
    return true;
}


/*
 * test_email - test if email is valid
 *
 * Determine if email matches email.
 *
 * If email is not an empty string, then
 * the format is of the form x@y where
 * neither x nor y contains @ AND
 * where neither x nor y is empty AND
 * where x@y is not too long.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_email(char *str)
{
    size_t length = 0;
    char *p = NULL;		/* location of @ */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */

    /*
     * reject if too long or empty
     */
    length = strlen(str);
    if (length == 0) {
	json_dbg(JSON_DBG_MED, __func__, "email is empty (address withheld)");
	return true;
    } else if (length > MAX_EMAIL_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: email length: %zu > MAX_EMAIL_LEN: %d", length, MAX_EMAIL_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: email <%s> length: %zu > MAX_EMAIL_LEN: %d", str, length, MAX_EMAIL_LEN);
	return false;
    }

    /*
     * reject if no leading @, or if more than one @
     */
    p = strchr(str, '@');
    if (p == NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: email is missing @");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: email <%s> email is missing @", str);
	return false;
    } else if (str[0] == '@') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: email starts with @");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: email <%s> starts with @", str);
	return false;
    } else if (str[length-1] == '@') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: email ends with @");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: email <%s> ends with @", str);
	return false;
    } else if (p != strrchr(str, '@')) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: email contains 2 or more @");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: email <%s> contains 2 or more @", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "email is valid");
    return true;
}


/*
 * test_empty_override - test if empty_override is valid
 *
 * Determine if empty_override boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_empty_override(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "empty_override is %s", booltostr(boolean));
    return true;
}


/*
 * test_entry_num - test if entry_num is valid
 *
 * Determine if entry_num is within the proper limits.
 *
 * given:
 *	entry_num	author number
 *
 * returns:
 *	true ==> entry_num is valid,
 *	false ==> entry_num is NOT valid, or some internal error
 */
bool
test_entry_num(int entry_num)
{
    /*
     * validate count
     */
    if (entry_num < 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: entry_num: %d < 0", entry_num);
	return false;
    } else if (entry_num > MAX_ENTRY_NUM) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: entry_num: %d > MAX_AUTHORS: %d", entry_num, MAX_ENTRY_NUM);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "entry_num is valid");
    return true;
}


/*
 * test_extra_file - test if extra_file is valid
 *
 * Determine if extra_file is a valid filename and does not
 * match one of the mandatory filenames.
 *
 * This function does NOT check if the extra_file is a duplicate
 * of another extra_file.  Moreover, this function does not
 * check if the file exists or is readable.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_extra_file(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */

    /* validate that the filename is POSIX portable safe plus + chars */
    if (posix_plus_safe(str, false, false, true) == false) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: posix_plus_safe check on extra_file failed");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: posix_plus_safe check on extra_file failed: <%s>", str);
	return false;
    }

    /* verify that extra_file does not match a mandatory filename */
    if (strcmp(str, ".info.json") == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra_file matches a mandatory file .info.json");
	return false;
    } else if (strcmp(str, ".author.json") == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra_file matches a mandatory file .author.json");
	return false;
    } else if (strcmp(str, "prog.c") == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra_file matches a mandatory file prog.c");
	return false;
    } else if (strcmp(str, "Makefile") == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra_file matches a mandatory file Makefile");
	return false;
    } else if (strcmp(str, "remarks.md") == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra_file matches a mandatory file remarks.md");
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "extra_file is valid");
    return true;
}


/*
 * test_first_rule_is_all - test if first_rule_is_all is valid
 *
 * Determine if first_rule_is_all boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_first_rule_is_all(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "first_rule_is_all is %s", booltostr(boolean));
    return true;
}


/*
 * test_fnamchk_version - test if fnamchk_version is valid
 *
 * Determine if fnamchk_version matches FNAMCHK_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_fnamchk_version(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, FNAMCHK_VERSION) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: fnamchk_version != FNAMCHK_VERSION: %s", FNAMCHK_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: fnamchk_version: %s is not FNAMCHK_VERSION: %s", str, FNAMCHK_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "fnamchk_version is valid");
    return true;
}


/*
 * test_formed_UTC - test if formed_UTC is valid
 *
 * Determine if formed_UTC is convertable into a broken-out time and
 * then back to the same time string.
 *
 * given:
 *	str			string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_formed_UTC(char *str)
{
    struct tm timeptr;			/* formed_UTC converted into broken-out time */
    char *ptr = NULL;			/* ptr to 1st char in buf not converted */
    char buf[MAX_TIMESTAMP_LEN+1+1];	/* conversion back to time string */
    size_t strftime_ret;		/* length of strftime() string without the trailing newline */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */

    /* convert into broken-out time */
    ptr = strptime(str, "%a %b %d %H:%M:%S %Y UTC", &timeptr);
    if (ptr == NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: strptime cannot convert formed_UTC");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: strptime failed convert formed_UTC: <%s>", str);
	return false;
    }
    if (ptr[0] == '\0') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra data in formed_UTC");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: extra data in formed_UTC: <%s>", str);
	return false;
    }

    /*
     * convert back to time string
     */
    memset(buf, 0, sizeof(buf));
    strftime_ret = strftime(buf, MAX_TIMESTAMP_LEN+1, "%a %b %d %H:%M:%S %Y UTC", &timeptr);
    if (strftime_ret == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: strftime failed to convert back to time string");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: strftime conversion back to time string failed: <%s>", str);
	return false;
    }

    /*
     * compare original time_string with reconverted time string buffer
     */
    if (strcmp(str, buf) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: formed_UTC != reconverted time string");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: formed_UTC: <%s> != reconverted: <%s>", str, buf);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "formed_UTC is valid");
    return true;
}


/*
 * test_formed_timestamp - test if formed_timestamp is valid
 *
 * Determine if formed_timestamp is >= MIN_TIMESTAMP.
 *
 * given:
 *	tstamp		timestamp as time_t to test
 *
 * returns:
 *	true ==> formed_timestamp is valid,
 *	false ==> formed_timestamp is NOT valid, or some internal error
 */
bool
test_formed_timestamp(time_t tstamp)
{
    /*
     * compare with the minimum timestamp
     */
    if (tstamp < MIN_TIMESTAMP) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: formed_timestamp < MIN_TIMESTAMP");
	if ((time_t)-1 > 0) {
	    /* case: unsigned time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: formed_timestamp: %ju < MIN_TIMESTAMP: %ju",
		     (uintmax_t)tstamp, (uintmax_t)MIN_TIMESTAMP);
	} else {
	    /* case: unsigned time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: formed_timestamp: %jd < MIN_TIMESTAMP: %jd",
		     (intmax_t)tstamp, (intmax_t)MIN_TIMESTAMP);
	}
	return false;
    }
    return true;
}


/*
 * test_formed_timestamp_usec - test if formed_timestamp_usec is valid
 *
 * Determine if formed_timestamp_usec is within the proper limits.
 *
 * NOTE: The function does NOT determine if the formed_timestamp_usec is unique among the authors.
 * NOTE: The function does NOT determine if the formed_timestamp_usec is <= author_count.
 *
 * given:
 *	formed_timestamp_usec	author number
 *
 * returns:
 *	true ==> formed_timestamp_usec is valid,
 *	false ==> formed_timestamp_usec is NOT valid, or some internal error
 */
bool
test_formed_timestamp_usec(int formed_timestamp_usec)
{
    /*
     * validate count
     */
    if (formed_timestamp_usec < 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: formed_timestamp_usec: %d < 0", formed_timestamp_usec);
	return false;
    } else if (formed_timestamp_usec > 999999) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: formed_timestamp_usec: %d > 999999", formed_timestamp_usec);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "formed_timestamp_usec is valid");
    return true;
}


/*
 * test_found_all_rule - test if found_all_rule is valid
 *
 * Determine if found_all_rule boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_found_all_rule(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "found_all_rule is %s", booltostr(boolean));
    return true;
}


/*
 * test_found_clean_rule - test if found_clean_rule is valid
 *
 * Determine if found_clean_rule boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_found_clean_rule(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "found_clean_rule is %s", booltostr(boolean));
    return true;
}


/*
 * test_found_clobber_rule - test if found_clobber_rule is valid
 *
 * Determine if found_clobber_rule boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_found_clobber_rule(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "found_clobber_rule is %s", booltostr(boolean));
    return true;
}


/*
 * test_found_try_rule - test if found_try_rule is valid
 *
 * Determine if found_try_rule boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_found_try_rule(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "found_try_rule is %s", booltostr(boolean));
    return true;
}


/*
 * test_info_JSON - test if info_JSON is valid
 *
 * Determine if info_JSON matches AUTHOR_JSON_FILENAME.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_info_JSON(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, INFO_JSON_FILENAME) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: info_JSON != INFO_JSON_FILENAME: %s", INFO_JSON_FILENAME);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: info_JSON: %s is not INFO_JSON_FILENAME: %s", str, INFO_JSON_FILENAME);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "info_JSON is valid");
    return true;
}


/*
 * test_ioccc_contest - test if ioccc_contest is valid
 *
 * Determine if ioccc_contest matches AUTHOR_JSON_FILENAME.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_ioccc_contest(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, IOCCC_CONTEST) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: ioccc_contest != IOCCC_CONTEST: %s", IOCCC_CONTEST);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: ioccc_contest: %s is not IOCCC_CONTEST: %s", str, IOCCC_CONTEST);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "ioccc_contest is valid");
    return true;
}


/*
 * test_ioccc_year - test if ioccc_year is valid
 *
 * Determine if ioccc_year is within the proper limits.
 *
 * NOTE: The function does NOT determine if the ioccc_year is unique among the authors.
 * NOTE: The function does NOT determine if the ioccc_year is <= author_count.
 *
 * given:
 *	ioccc_year	author number
 *
 * returns:
 *	true ==> ioccc_year is valid,
 *	false ==> ioccc_year is NOT valid, or some internal error
 */
bool
test_ioccc_year(int ioccc_year)
{
    /*
     * validate count
     */
    if (ioccc_year != IOCCC_YEAR) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: ioccc_year: %d != %d", ioccc_year, IOCCC_YEAR);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "ioccc_year is valid");
    return true;
}


/*
 * test_iocccsize_version - test if iocccsize_version is valid
 *
 * Determine if iocccsize_version matches IOCCCSIZE_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_iocccsize_version(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, IOCCCSIZE_VERSION) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: iocccsize_version != IOCCCSIZE_VERSION: %s", IOCCCSIZE_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: iocccsize_version: %s is not IOCCCSIZE_VERSION: %s", str, IOCCCSIZE_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "iocccsize_version is valid");
    return true;
}


/*
 * test_location_code - test if location_code is valid
 *
 * Determine if location_code matches location_code.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_location_code(char *str)
{
    size_t length = 0;
    char const *location_name = NULL;		/* location name or NULL ==> unlisted code */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */

    /* validate length */
    length = strlen(str);
    if (length != 2) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: location_code: length: %zu != 2", length);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: location_code: <%s> length: %zu != 2", str, length);
	return false;
    }

    /* validate 2 ASCII UPPER CASE characters */
    if (!isascii(str[0]) || !isupper(str[0]) || !isascii(str[1]) || !isupper(str[1])) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: location_code: is not 2 ASCII UPPER CASE characters");
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: location_code: <%s> is not 2 ASCII UPPER CASE characters", str);
	return false;
    }

    /* validate ISO 3166-1 Alpha-2 in UPPER CASE code */
    location_name = lookup_location_name(str);
    if (location_name == NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: location_code: not a known ISO 3166-1 location/country code");
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: location_code: <%s> not a known ISO 3166-1 location/country code", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "location_code is valid");
    return true;
}


/* XXX - end sorted order matching chk_validate.c here - XXX */


/*
 * test_mkiocccentry_version - test if mkiocccentry_version is valid
 *
 * Determine if mkiocccentry_version matches MKIOCCCENTRY_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_mkiocccentry_version(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, MKIOCCCENTRY_VERSION) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mkiocccentry_version != MKIOCCCENTRY_VERSION: %s", MKIOCCCENTRY_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mkiocccentry_version: %s is not MKIOCCCENTRY_VERSION: %s", str, MKIOCCCENTRY_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "formed_UTC is valid");
    return true;
}


/*
 * test_txzchk_version - test if txzchk_version is valid
 *
 * Determine if txzchk_version matches TXZCHK_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 *
 *  "Because sometimes people might try to hide the fact that they're planning
 *  on throwing feathers on the tar." :-)
 */
bool
test_txzchk_version(char *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    if (strcmp(str, TXZCHK_VERSION) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: txzchk_version != TXZCHK_VERSION: %s", TXZCHK_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: txzchk_version: %s is not TXZCHK_VERSION: %s", str, TXZCHK_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "txzchk_version is valid");
    return true;
}


/*
 * test_title - test if title is valid
 *
 * Determine if title length is <= MAX_TITLE_LEN and that it matches the regexp:
 *
 *	^[0-9a-z][0-9a-z._+-]*$
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_title(char *str)
{
    size_t length = 0;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    /* check for a valid length */
    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty title");
	return false;
    }
    length = strlen(str);
    if (length > MAX_TITLE_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: title length %ju > max %d: <%s>", (uintmax_t)length, MAX_TITLE_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title: <%s> is invalid", str);
	return false;
    }
    /* check for valid title chars */
    if (!posix_plus_safe(str, true, false, true)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: title does not match regexp ^[0-9a-z][0-9a-z._+-]*$: '%s'", str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title: <%s> is invalid", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "title is valid");
    return true;
}


/*
 * test_rule_2a_override - test if rule_2a_override is valid
 *
 * Determine if rule_2a_override boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_rule_2a_override(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "rule_2a_override is %s", booltostr(boolean));
    return true;
}


/*
 * test_rule_2a_mismatch - test if rule_2a_mismatch is valid
 *
 * Determine if rule_2a_mismatch boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_rule_2a_mismatch(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "rule_2a_mismatch is %s", booltostr(boolean));
    return true;
}


/*
 * test_rule_2b_override - test if rule_2b_override is valid
 *
 * Determine if rule_2b_override boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_rule_2b_override(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "rule_2b_override is %s", booltostr(boolean));
    return true;
}


/*
 * test_highbit_warning - test if highbit_warning is valid
 *
 * Determine if highbit_warning boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_highbit_warning(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "highbit_warning is %s", booltostr(boolean));
    return true;
}


/*
 * test_nul_warning - test if nul_warning is valid
 *
 * Determine if nul_warning boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_nul_warning(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "nul_warning is %s", booltostr(boolean));
    return true;
}


/*
 * test_trigraph_warning - test if trigraph_warning is valid
 *
 * Determine if trigraph_warning boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_trigraph_warning(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "trigraph_warning is %s", booltostr(boolean));
    return true;
}


/*
 * test_wordbuf_warning - test if wordbuf_warning is valid
 *
 * Determine if wordbuf_warning boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_wordbuf_warning(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "wordbuf_warning is %s", booltostr(boolean));
    return true;
}


/*
 * test_ungetc_warning - test if ungetc_warning is valid
 *
 * Determine if ungetc_warning boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_ungetc_warning(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "ungetc_warning is %s", booltostr(boolean));
    return true;
}


/*
 * test_test_mode - test if test_mode is valid
 *
 * Determine if test_mode boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean		boolean to test
 *
 * returns:
 *	true ==> bool is valid,
 *	false ==> bool is NOT valid, or some internal error
 */
bool
test_test_mode(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "test_mode is %s", booltostr(boolean));
    return true;
}

/*
 * test_github - test if GitHub account is valid
 *
 * Determine if github length is <= MAX_GITHUB_LEN and that it has a leading '@'
 * and _only_ one '@'.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_github(char *str)
{
    size_t length = 0;

    char *p = NULL;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    /* check for a valid length */
    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "empty github is invalid");
	return false;
    }
    length = strlen(str);
    if (length > MAX_GITHUB_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: github length %ju > max %d: <%s>", (uintmax_t)length, MAX_GITHUB_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title: <%s> is invalid", str);
	return false;
    }
    /* check for valid github account chars */
    p = strchr(str, '@');
    if (p == NULL || str[0] != '@') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: github account does not start with '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: github: <%s> is invalid", str);
	return false;
    } else if (p != strrchr(str, '@')) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: github account has more than one '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: github: <%s> is invalid", str);
	return false;

    }

    json_dbg(JSON_DBG_MED, __func__, "github is valid");
    return true;
}

/*
 * test_twitter - test if twitter account is valid
 *
 * Determine if twitter length is <= MAX_TWITTER_LEN and that it has a leading '@'
 * and _only_ one '@'.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_twitter(char *str)
{
    size_t length = 0;
    char *p = NULL;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    /* check for a valid length */
    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "empty twitter is invalid");
	return false;
    }
    length = strlen(str);
    if (length > MAX_TWITTER_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: twitter length %ju > max %d: <%s>", (uintmax_t)length, MAX_TWITTER_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title: <%s> is invalid", str);
	return false;
    }
    /* check for valid twitter account chars */
    p = strchr(str, '@');
    if (p == NULL || str[0] != '@') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: twitter account does not start with '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: twitter: <%s> is invalid", str);
	return false;
    } else if (p != strrchr(str, '@')) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: twitter account has more than one '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: twitter: <%s> is invalid", str);
	return false;

    }

    json_dbg(JSON_DBG_MED, __func__, "twitter is valid");
    return true;
}

/*
 * test_url - test if URL is valid
 *
 * Determine if url length is <= MAX_URL_LEN and that it starts with either
 * http:// or https:// and more characters.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_url(char *str)
{
    size_t length = 0;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * validate str
     */
    /* check for a valid length */
    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "empty url is invalid");
	return false;
    }
    length = strlen(str);
    if (length > MAX_URL_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: url length %ju > max %d: <%s>", (uintmax_t)length, MAX_URL_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title: <%s> is invalid", str);
	return false;
    }
    if (((strncmp(str, "http://", LITLEN("http://")) != 0) &&
	 (str[LITLEN("http://")] != '\0')) ||
	((strncmp(str, "https://", LITLEN("https://")) != 0))) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: url does not start with either https:// or http://");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: url: <%s> is invalid", str);
	    return false;

    } else if (str[LITLEN("https://")] == '\0') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: url does not have more characters after either the http:// or https://");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: url: <%s> is invalid", str);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "url is valid");
    return true;
}
