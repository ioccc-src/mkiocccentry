/*
 * entry_util - utility functions supporting mkiocccentry JSON files
 *
 * JSON related functions to support formation of .info.json files
 * and .auth.json files, their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * As per IOCCC anonymous judging policy, the calls to json_dbg() in this file
 * that are JSON_DBG_MED or lower will NOT reveal any JSON content.
 * Only at JSON_DBG_HIGH or higher should json_dbg() calls in the file
 * print JSON content.
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.
 * All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright, this permission notice and text
 * this comment, and the disclaimer below appear in all of the following:
 *
 *       supporting documentation
 *       source copies
 *       source works derived from this source
 *       binaries derived from this source or from derived source
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This tool and the JSON parser were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
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


#include <stdio.h>
#include <stdlib.h>
#include <strings.h> /* strcasecmp */
#include <ctype.h>
#include <errno.h>
#include <sys/types.h>
#include <fts.h>
#include <unistd.h>
#include <fcntl.h>

/*
 * file_util - common utility functions for file operations
 */
#include "file_util.h"


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * canon_path - canonicalize paths
 */
#include "../cpath/cpath.h"

/*
 * verge - the functionality to test versions
 */
#include "../jparse/verge.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * util - various util functions
 */
#include "util.h"

/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "entry_util.h"

/*
 * location - location/country codes and set the IOCCC locale
 */
#include "location.h"

/*
 * globals
 */
bool ignore_permissions = false;            /* true ==> ignore permissions checks on paths */

/*
 * mandatory files that must exist in the submission top level directory
 */
char *mandatory_filenames[] =
{
    AUTH_JSON_FILENAME,
    INFO_JSON_FILENAME,
    PROG_C_FILENAME,
    MAKEFILE_FILENAME,
    REMARKS_FILENAME,
    NULL /* MUST BE LAST!! */
};

/*
 * filenames (in top level submission directory only) that should be mode 0555
 */
char *executable_filenames[] =
{
    TRY_SH,
    TRY_ALT_SH,
    NULL /* MUST BE LAST!! */
};

static char const *poison_mkiocccentry_versions[] =
{
    NULL /* MUST BE LAST!! */
};

static char const *poison_txzchk_versions[] =
{
    NULL /* MUST BE LAST!! */
};

static char const *poison_iocccsize_versions[] =
{
    NULL /* MUST BE LAST!! */
};

static char const *poison_info_versions[] =
{
    NULL /* MUST BE LAST!! */
};

static char const *poison_auth_versions[] =
{
    NULL /* MUST BE LAST!! */
};

static char const *poison_fnamchk_versions[] =
{
    NULL /* MUST BE LAST!! */
};

static char const *poison_chksubmit_versions[] =
{
    NULL /* MUST BE LAST!! */
};


/*
 * static declarations
 */


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
	err(50, __func__, "called with NULL authp");
	not_reached();
    }

    /*
     * free file format strings
     */
    /* NOTE: no_comment is a compiled in constant */
    /* NOTE: auth_version is a compiled in constant */
    /* NOTE: IOCCC_contest is a compiled in constant */

    /*
     * free IOCCC tool versions
     */
    /* NOTE: mkiocccentry_ver is a compiled in constant */
    /* NOTE: chksubmit_ver is a compiled in constant */
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
    /*
     * firewall
     */
    if (infop == NULL) {
	err(51, __func__, "called with NULL infop");
	not_reached();
    }

    /*
     * free file format strings
     */
    /* NOTE: no_comment is a compiled in constant */
    /* NOTE: auth_version is a compiled in constant */
    /* NOTE: IOCCC_contest is a compiled in constant */

    /*
     * free IOCCC tool versions
     */
    /* NOTE: mkiocccentry_ver is a compiled in constant */
    /* NOTE: chksubmit_ver is a compiled in constant */
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

    /* NOTE: info_file is a compiled in constant */
    /* NOTE: auth_file is a compiled in constant */

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
	if (author_set[i].alt_url != NULL) {
	    free(author_set[i].alt_url);
	    author_set[i].alt_url = NULL;
	}
	if (author_set[i].mastodon != NULL) {
	    free(author_set[i].mastodon);
	    author_set[i].mastodon = NULL;
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
    free(author_set);
    author_set = NULL;
    return;
}


/*
 * free_manifest - free storage related to a struct manifest *
 *
 * given:
 *      manp   - pointer to manifest structure to free
 *
 * This function does not return on error.
 */
void
free_manifest(struct manifest *manp)
{
    /*
     * firewall
     */
    if (manp == NULL) {
	err(54, __func__, "called with NULL manp");
	not_reached();
    }

    /*
     * extra dynamic array
     */
    if (manp->extra != NULL) {
	dyn_array_free(manp->extra);
	manp->extra = NULL;
    }
    /*
     * shell script filenames dynamic array
     */
    if (manp->shell != NULL) {
        dyn_array_free(manp->shell);
        manp->shell = NULL;
    }

    /*
     * zeroize the manifest structure
     */
    memset(manp, 0, sizeof(*manp));
    return;
}


/*
 * valid_contest_id	    -	validate string as a contest ID
 *
 * given:
 *	str	    -	string to test
 *
 * Returns true if it's valid.
 *
 * Returns false if it's invalid, NULL or empty string.
 */
bool
valid_contest_id(char *str)
{
    size_t len;			/* input string length */
    int ret;			/* libc function return */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    char guard;			/* scanf guard to catch excess amount of input */
    size_t i;

    if (str == NULL || strlen(str) == 0) {
	return false;
    }

    if (!strcmp(str, "test")) {
	/*
	 * special case: test is valid
	 */
	return true;
    }
    len = strlen(str);

    /*
     * validate format of non-test contest ID.  The contest ID, if not "test"
     * must be a UUID.  The UUID has the 36 character format:
     *
     *     xxxxxxxx-xxxx-4xxx-Nxxx-xxxxxxxxxxxx
     *
     * where 'x' is a hex character, 4 is the UUID version, and N is one of 8, 9, a, or b.
     * 1.
     */
    if (len != UUID_LEN) {
	return false;
    }

    /* convert to UUID lower case before checking */
    for (i = 0; i < len; ++i) {
	str[i] = (char)tolower(str[i]);
    }
    dbg(DBG_VHIGH, "converted the IOCCC contest ID to: %s", str);
    /* validate UUID string, version and variant */
    ret = sscanf(str, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x%c", &a, &b, &version, &c, &variant, &d, &e, &f, &guard);
    dbg(DBG_HIGH, "UUID version hex char: %1x", version);
    dbg(DBG_HIGH, "UUID variant hex char: %1x", variant);
    if (ret != 8 || version != UUID_VERSION || (variant != UUID_VARIANT_0 && variant != UUID_VARIANT_1 &&
                variant != UUID_VARIANT_2 && variant != UUID_VARIANT_3)) {
	return false;
    }


    dbg(DBG_MED, "IOCCC contest ID is a UUID: %s", str);

    return true;
}


/*
 * object2author - load a struct author with validated elements
 *
 * In .auth.json, we are required to find a JTYPE_MEMBER named "authors".
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
 *	node		JSON parse node being checked
 *	depth		depth of node in the JSON parse tree (0 ==> tree root)
 *	sem		JSON semantic node triggering the check
 *	name		name of caller function (NULL ==> "((NULL))")
 *	val_err		pointer to address where to place a JSON semantic validation error,
 *			NULL ==> do not report a JSON semantic validation error
 *	auth		pointer to a struct author to fill out
 *	author_num	author number
 *
 * returns:
 *	true ==> struct author was loaded with validated elements
 *	false ==> invalid member value found, auth is NULL, or some internal error
 */
bool
object2author(struct json *node, unsigned int depth, struct json_sem *sem,
	      char const *name, struct json_sem_val_err **val_err,
	      struct author *auth, int author_num)
{
    char *auth_name = NULL;		/* name of the author */
    bool found_name = false;		/* true ==> found name in node */
    char *location_code = NULL;		/* author location/country code */
    bool found_location_code = false;	/* true ==> found location_code in node */
    char *email = NULL;			/* Email address of author or NULL ==> not provided */
    bool found_email = false;		/* true ==> found email in node */
    bool email_withheld = false;	/* true ==> Email address withheld */
    char *url = NULL;			/* home URL of author or NULL ==> not provided */
    char *alt_url = NULL;		/* alt URL of author or NULL ==> not provided */
    bool found_url = false;		/* true ==> found url in node */
    bool found_alt_url = false;		/* true ==> found alt_url in node */
    bool url_withheld = false;		/* true ==> home URL of author withheld */
    bool alt_url_withheld = false;	/* true ==> alt URL of author withheld */
    char *mastodon = NULL;		/* author mastodon handle or NULL ==> not provided */
    bool found_mastodon = false;		/* true ==> found mastodon in node */
    bool mastodon_withheld = false;	/* true ==> author mastodon handle withheld */
    char *github = NULL;		/* author GitHub username or NULL ==> not provided */
    bool found_github = false;		/* true ==> found github in node */
    bool github_withheld = false;	/* true ==> author mastodon handle withheld */
    char *affiliation = NULL;		/* author affiliation or NULL ==> not provided */
    bool found_affiliation = false;	/* true ==> found affiliation in node */
    bool affiliation_withheld = false;	/* true ==> author affiliation withheld */
    bool past_winning_author = false;		/* true ==> author claims to have won before */
					/* false ==> author claims they have not won before */
    bool found_past_winning_author = false;	/* true ==> found past_winning_author in node */
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
    int *int_val = NULL;		/* pointer to a converted JTYPE_NUMBER as int */
    int i;

    /*
     * firewall - must not be NULL
     */
    if (auth == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(55, node, depth, sem, name,
				    "author array index[%d]: auth is NULL", author_num);
	}
	return false;
    }

    /*
     * firewall - validate JTYPE_MEMBER item in the JTYPE_ARRAY
     */
    test = sem_node_valid(node, depth, sem, __func__, val_err);
    if (test == false) {
	/* sem_node_valid() will have set *val_err */
	return false;
    }
    if (node->type != JTYPE_OBJECT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(56, node, depth, sem, __func__,
				    "author array index[%d] type %s != JTYPE_OBJECT",
				    author_num, json_type_name(node->type));
	}
	return false;
    }
    obj = &(node->item.object);
    obj_len = obj->len;
    if (obj_len <= 0) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(57, node, depth, sem, __func__,
				    "author array index[%d] length: %d <= 0",
				    author_num, obj_len);
	}
	return false;
    }
    if (obj->set == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(58, node, depth, sem, __func__,
				    "author array index[%d] set is NULL", author_num);
	}
	return false;
    }

    /*
     * examine each JTYPE_MEMBER of the JTYPE_OBJECT
     */
    for (i=0; i < obj_len; ++i) {
	struct json *e = obj->set[i];		/* next item in the JTYPE_OBJECT */
	char *obj_name = NULL;			/* name string of name part of JTYPE_MEMBER */

	/*
	 * firewall - validate JTYPE_MEMBER item in the JTYPE_OBJECT
	 */
	test = sem_node_valid(e, depth+1, sem, __func__, val_err);
	if (test == false) {
	    /* sem_node_valid() will have set *val_err */
	    return false;
	}
	if (e->type != JTYPE_MEMBER) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(59, e, depth, sem, __func__,
					"author array index[%d] JTYPE_OBJECT[%d] type %s != JTYPE_MEMBER",
					author_num, i, json_type_name(e->type));
	    }
	    return false;
	}
	obj_name = sem_member_name_decoded_str(e, depth+2, sem, __func__, val_err);
	if (obj_name == NULL) {
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
	/* case: IOCCC author name */
	if (strcmp(obj_name, "name") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_name == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(60, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_name = true;

	    /* obtain value as JTYPE_STRING */
	    auth_name = sem_member_value_decoded_str(e, depth+2, sem, __func__, NULL);
	    /* we will deal with NULL auth_name later in this function */

	/* case: IOCCC author location_code */
	} else if (strcmp(obj_name, "location_code") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_location_code == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(61, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_location_code = true;

	    /* obtain value as JTYPE_STRING */
	    location_code = sem_member_value_decoded_str(e, depth+2, sem, __func__, NULL);
	    /* we will deal with NULL location_code later in this function */

	/* case: IOCCC author email */
	} else if (strcmp(obj_name, "email") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_email == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(62, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_email = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(63, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    author_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == true) {
		email = NULL;
		email_withheld = true;
	    } else {
		email = val_or_null.str;
		email_withheld = false;
	    }

	/* case: IOCCC author url */
	} else if (strcmp(obj_name, "url") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_url == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(64, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_url = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(65, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    author_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == true) {
		url = NULL;
		url_withheld = true;
	    } else {
		url = val_or_null.str;
		url_withheld = false;
	    }

	/* case: IOCCC author alt url */
	} else if (strcmp(obj_name, "alt_url") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_alt_url == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(66, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_alt_url = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(67, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    author_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == true) {
		alt_url = NULL;
		alt_url_withheld = true;
	    } else {
		alt_url = val_or_null.str;
		alt_url_withheld = false;
	    }

	/* case: IOCCC author mastodon */
	} else if (strcmp(obj_name, "mastodon") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_mastodon == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(68, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		    }
		return false;
	    }
	    found_mastodon = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(69, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    author_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == true) {
		mastodon = NULL;
		mastodon_withheld = true;
	    } else {
		mastodon = val_or_null.str;
		mastodon_withheld = false;
	    }

	/* case: IOCCC author github */
	} else if (strcmp(obj_name, "github") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_github == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(70, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_github = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(71, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    author_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == true) {
		github = NULL;
		github_withheld = true;
	    } else {
		github = val_or_null.str;
		github_withheld = false;
	    }

	/* case: IOCCC author affiliation */
	} else if (strcmp(obj_name, "affiliation") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_affiliation == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(72, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_affiliation = true;

	    /* obtain value as JTYPE_STRING or JTYPE_NULL */
	    val_or_null = sem_member_value_str_or_null(e, depth+2, sem, __func__, NULL);
	    if (val_or_null.valid == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(73, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid string or JSON null",
					    author_num, i);
		}
		return false;
	    }
	    if (val_or_null.is_null == true) {
		affiliation = NULL;
		affiliation_withheld = true;
	    } else {
		affiliation = val_or_null.str;
		affiliation_withheld = false;
	    }

	/* case: IOCCC author past_winning_author */
	} else if (strcmp(obj_name, "past_winning_author") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_past_winning_author == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(74, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_past_winning_author = true;

	    /* obtain value as JTYPE_BOOL */
	    bool_val = sem_member_value_bool(e, depth+2, sem, __func__, NULL);
	    if (bool_val == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(75, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid JSON boolean",
					    author_num, i);
		}
		return false;
	    }
	    past_winning_author = *bool_val;

	/* case: IOCCC author default_handle */
	} else if (strcmp(obj_name, "default_handle") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_default_handle == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(76, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_default_handle = true;

	    /* obtain value as JTYPE_BOOL */
	    bool_val = sem_member_value_bool(e, depth+2, sem, __func__, NULL);
	    if (bool_val == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(77, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid JSON boolean",
					    author_num, i);
		}
		return false;
	    }
	    default_handle = *bool_val;

	/* case: IOCCC author author_handle */
	} else if (strcmp(obj_name, "author_handle") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_author_handle == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(78, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_author_handle = true;

	    /* obtain value as JTYPE_STRING */
	    author_handle = sem_member_value_decoded_str(e, depth+2, sem, __func__, NULL);
	    /* we will deal with NULL author_handle later in this function */

	/* case: IOCCC author author_number */
	} else if (strcmp(obj_name, "author_number") == 0) {

	    /* firewall - check for duplicate JTYPE_MEMBER */
	    if (found_author_number == true) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(79, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] found more than 1 <%s>",
					    author_num, i, obj_name);
		}
		return false;
	    }
	    found_author_number = true;

	    /* obtain value as JTYPE_NUMBER as int */
	    int_val = sem_member_value_int(e, depth+2, sem, __func__, NULL);
	    if (int_val == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(80, e, depth+2, sem, __func__,
					    "author array index[%d] JTYPE_OBJECT[%d] invalid author number",
					    author_num, i);
		}
		return false;
	    }
	    author_number = *int_val;

	/* case: invalid JTYPE_MEMBER - not part of an IOCCC author's JTYPE_OBJECT */
	} else {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(81, e, depth+2, sem, __func__,
					"author array index[%d] JTYPE_OBJECT has invalid JTYPE_MEMBER name: <%s>",
					i, obj_name);
	    }
	    return false;
	}
    }

    /*
     * looking for missing information in the JSON object
     */
    if (found_name == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(82, node, depth, sem, __func__,
				    "author array index[%d]: missing name", author_num);
	}
	return false;
    }
    if (found_location_code == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(83, node, depth, sem, __func__,
				    "author array index[%d]: missing location_code", author_num);
	}
	return false;
    }
    if (found_email == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(84, node, depth, sem, __func__,
				    "author array index[%d]: missing email", author_num);
	}
	return false;
    }
    if (found_url == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(85, node, depth, sem, __func__,
				    "author array index[%d]: missing url", author_num);
	}
	return false;
    }
    if (found_alt_url == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(86, node, depth, sem, __func__,
				    "author array index[%d]: missing alt_url", author_num);
	}
	return false;
    }

    if (found_mastodon == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(87, node, depth, sem, __func__,
				    "author array index[%d]: missing mastodon", author_num);
	}
	return false;
    }
    if (found_github == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(88, node, depth, sem, __func__,
				    "author array index[%d]: missing github", author_num);
	}
	return false;
    }
    if (found_affiliation == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(89, node, depth, sem, __func__,
				    "author array index[%d]: missing affiliation", author_num);
	}
	return false;
    }
    if (found_past_winning_author == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(90, node, depth, sem, __func__,
				    "author array index[%d]: missing past_winning_author", author_num);
	}
	return false;
    }
    if (found_default_handle == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(91, node, depth, sem, __func__,
				    "author array index[%d]: missing default_handle", author_num);
	}
	return false;
    }
    if (found_author_handle == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(92, node, depth, sem, __func__,
				    "author array index[%d]: missing author_handle", author_num);
	}
	return false;
    }
    if (found_author_number == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(93, node, depth, sem, __func__,
				    "author array index[%d]: missing author_number", author_num);
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
	    *val_err = werr_sem_val(94, node, depth, sem, __func__,
				    "author array index[%d]: auth_name is NULL", author_num);
	}
	return false;
    }
    if (location_code == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(95, node, depth, sem, __func__,
				    "author array index[%d]: location_code is NULL", author_num);
	}
	return false;
    }
    if (email == NULL) {
	email = "";
    }
    if (url == NULL) {
	url = "";
    }
    if (alt_url == NULL) {
	alt_url = "";
    }
    if (mastodon == NULL) {
	mastodon = "";
    }
    if (github == NULL) {
	github = "";
    }
    if (affiliation == NULL) {
	affiliation = "";
    }
    if (author_handle == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(96, node, depth, sem, __func__,
				    "author array index[%d]: author_handle is NULL", author_num);
	}
	return false;
    }

    /*
     * validate elements
     */
    if (test_name(auth_name) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(97, node, depth, sem, __func__,
				    "author array index[%d]: auth_name is invalid", author_num);
	}
	return false;
    }
    if (test_location_code(location_code) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(98, node, depth, sem, __func__,
				    "author array index[%d]: location_code is invalid", author_num);
	}
	return false;
    }
    if (email_withheld == false && test_email(email) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(99, node, depth, sem, __func__,
				    "author array index[%d]: email is invalid", author_num);
	}
	return false;
    }

    if (url_withheld == false && test_url(url) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(100, node, depth, sem, __func__,
				    "author array index[%d]: url is invalid", author_num);
	}
	return false;
    }
    if (alt_url_withheld == false && test_alt_url(alt_url) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(101, node, depth, sem, __func__,
				    "author array index[%d]: alt_url is invalid", author_num);
	}
	return false;
    }


    if (mastodon_withheld == false && test_mastodon(mastodon) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(102, node, depth, sem, __func__,
				    "author array index[%d]: mastodon is invalid", author_num);
	}
	return false;
    }
    if (github_withheld == false && test_github(github) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(103, node, depth, sem, __func__,
				    "author array index[%d]: github is invalid", author_num);
	}
	return false;
    }
    if (affiliation_withheld == false && test_affiliation(affiliation) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(104, node, depth, sem, __func__,
				    "author array index[%d]: affiliation is invalid", author_num);
	}
	return false;
    }
    if (test_past_winning_author(past_winning_author) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(105, node, depth, sem, __func__,
				    "author array index[%d]: past_winning_author is invalid", author_num);
	}
	return false;
    }
    if (test_default_handle(default_handle) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(106, node, depth, sem, __func__,
				    "author array index[%d]: default_handle is invalid", author_num);
	}
	return false;
    }
    if (test_author_handle(author_handle) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(107, node, depth, sem, __func__,
				    "author array index[%d]: author_handle is invalid", author_num);
	}
	return false;
    }
    if (test_author_number(author_number) == false) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(108, node, depth, sem, __func__,
				    "author array index[%d]: author_number is invalid", author_num);
	}
	return false;
    }

    /*
     * load elements into struct author * (auth)
     *
     * For strings, we strdup() them.
     */
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->name = strdup(auth_name);
    if (auth->name == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(109, node, depth, sem, __func__,
				     "author array index[%d]: strdup of name failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->location_code = strdup(location_code);
    if (auth->location_code == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(110, node, depth, sem, __func__,
				     "author array index[%d]: strdup of location_code failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->email = strdup(email);
    if (auth->email == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(111, node, depth, sem, __func__,
				     "author array index[%d]: strdup of email failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->url = strdup(url);
    if (auth->url == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(112, node, depth, sem, __func__,
				     "author array index[%d]: strdup of url failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->alt_url = strdup(alt_url);
    if (auth->alt_url == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(113, node, depth, sem, __func__,
				     "author array index[%d]: strdup of alt_url failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->mastodon = strdup(mastodon);
    if (auth->mastodon == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(114, node, depth, sem, __func__,
				     "author array index[%d]: strdup of mastodon failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->github = strdup(github);
    if (auth->github == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(115, node, depth, sem, __func__,
				     "author array index[%d]: strdup of github failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->affiliation = strdup(affiliation);
    if (auth->affiliation == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(116, node, depth, sem, __func__,
				     "author array index[%d]: strdup of affiliation failed", author_num);
	}
	free_author_array(auth, 1);
	return false;
    }
    auth->past_winning_author = past_winning_author;
    auth->default_handle = default_handle;
    errno = 0;		/* pre-clear errno for werrp_sem_val() */
    auth->author_handle = strdup(author_handle);
    if (auth->location_code == NULL) {
	if (val_err != NULL) {
	    *val_err = werrp_sem_val(117, node, depth, sem, __func__,
				     "author array index[%d]: strdup of author_handle failed", author_num);
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
 * object2manifest - load a struct author with validated elements
 *
 * In .info.json, we are required to find a JTYPE_MEMBER named "manifest".
 * The value of this "manifest" JTYPE_MEMBER is a JSON array (JTYPE_ARRAY).
 * That JSON array consists of an array of JSON objects (JTYPE_OBJECT),
 * each containing JSON members (JTYPE_MEMBER) with filenames for the submission.
 *
 * Given a node that is a JSON object (JTYPE_OBJECT) (from the JSON array),
 * we will convert a node that is a JSON object (JTYPE_OBJECT) containing
 * an array of JSON members (JTYPE_MEMBER) with filename information,
 * validate the values of the JTYPE_MEMBER and then load that value
 * into a struct manifest.
 *
 * If we find a JSON member name under the node that is not part of the
 * IOCCC filenames for a valid manifest, we will return false.
 *
 * The IOCCC manifest MUST contain 1 and only 1 of these mandatory files:
 *
 *	info_JSON	".info.json"
 *	auth_JSON	".auth.json"
 *	c_src		"prog.c"
 *	Makefile	"Makefile"
 *	remarks		"remarks.md"
 *
 * In addition, the IOCCC manifest may contain 0 or more extra files:
 *
 *	extra_file
 *
 * and 0 or more shell files:
 *
 *      shell_script
 *
 * provided that those filenames do NOT match one of the above
 * mentioned mandatory files (case-insensitive) AND that the filename is
 * POSIX portable safe and + chars AND does not match another file (e.g. a
 * shell_script filename must NOT match an extra_file).
 *
 * It optionally may also have:
 *
 *      try_sh
 *      try_alt_sh
 *      c_alt_src
 *
 * provided that the values are correct ("try.sh", "try.alt.sh", "prog.alt.c").
 *
 * This function records the number of mandatory files found in the IOCCC
 * manifest. It will flag as an error, it a mandatory file is missing or if more
 * than one mandatory file is found.  It will flag as an error, if an extra_file
 * or shell_script has an invalid filename or duplicates a mandatory file or is
 * a duplicate of another extra_file or a shell_script.
 *
 * given:
 *	node		JSON parse node being checked
 *	depth		depth of node in the JSON parse tree (0 ==> tree root)
 *	sem		JSON semantic node triggering the check
 *	name		name of caller function (NULL ==> "((NULL))")
 *	val_err		pointer to address where to place a JSON semantic validation error,
 *			NULL ==> do not report a JSON semantic validation error
 *	manp		pointer to a struct manifest to fill out
 *
 * returns:
 *	true ==> struct manifest was loaded with validated elements
 *	false ==> invalid manifest member value found, manp is NULL, or some internal error
 */
bool
object2manifest(struct json *node, unsigned int depth, struct json_sem *sem,
	       char const *name, struct json_sem_val_err **val_err,
	       struct manifest *manp)
{
    struct json_array *array = NULL;	/* JSON node as JTYPE_ARRAY */
    int array_len = 0;			/* length in JTYPE_MEMBER of the JSON node as JTYPE_ARRAY */
    bool test = false;			/* validation test result */
    struct manifest man;		/* IOCCC manifest to fill out and test */
    char *extra_filename = NULL;	/* filename of an extra file */
    char *extra_filename2 = NULL;	/* second filename of an extra file */
    char *shell_filename = NULL;	/* filename of an shell_file */
    char *shell_filename2 = NULL;	/* second filename of a shell_file */
    intmax_t i;
    intmax_t j;

    /*
     * firewall - must not be NULL
     */
    if (manp == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(118, node, depth, sem, name,
				    "manp is NULL");
	}
	return false;
    }

    /*
     * firewall - validate JTYPE_MEMBER item in the JTYPE_ARRAY
     */
    test = sem_node_valid(node, depth, sem, __func__, val_err);
    if (test == false) {
	/* sem_node_valid() will have set *val_err */
	return false;
    }
    if (node->type != JTYPE_ARRAY) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(119, node, depth, sem, __func__,
				    "manifest type %s != JTYPE_ARRAY",
				    json_type_name(node->type));
	}
	return false;
    }
    array = &(node->item.array);
    array_len = array->len;
    if (array_len <= 0) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(120, node, depth, sem, __func__,
				    "manifest length: %d <= 0", array_len);
	}
	return false;
    }
    if (array->set == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(121, node, depth, sem, __func__,
				    "manifest set is NULL");
	}
	return false;
    }

    /*
     * initialize struct manifest
     */
    memset(&man, 0, sizeof(man));
    man.extra = dyn_array_create(sizeof(char *), JSON_CHUNK, JSON_CHUNK, false);
    if (man.extra == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(122, node, depth, sem, __func__,
				    "dyn_array_create failed to create man.extra");
	}
	return false;
    }

    /*
     * now shell script list
     */
    man.shell = dyn_array_create(sizeof(char *), JSON_CHUNK, JSON_CHUNK, false);
    if (man.shell == NULL) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(123, node, depth, sem, __func__,
				    "dyn_array_create failed to create man.shell");

	}
	dyn_array_free(man.extra);
	return false;
    }


    /*
     * examine each JTYPE_OBJECT for the manifest JTYPE_ARRAY
     */
    for (i=0; i < array_len; ++i) {
	struct json *e = array->set[i];		/* next item in the JTYPE_ARRAY */
	char *arr_name = NULL;			/* name string of name part of JTYPE_MEMBER */
	char *value = NULL;			/* value string of name part of JTYPE_MEMBER */
	struct json_object *o = NULL;		/* current JTYPE_OBJECT of the manifest JTYPE_ARRAY to examine */
	struct json *jo = NULL;			/* JSON element of JTYPE_OBJECT of the manifest JTYPE_ARRAY to examine */

	/*
	 * firewall - validate JTYPE_OBJECT item in the JTYPE_ARRAY
	 */
	test = sem_node_valid(e, depth+1, sem, __func__, val_err);
	if (test == false) {
	    /* sem_node_valid() will have set *val_err */
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}
	if (e->type != JTYPE_OBJECT) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(124, e, depth+1, sem, __func__,
					"manifest JTYPE_ARRAY[%jd] type %s != JTYPE_OBJECT",
					i, json_type_name(e->type));
	    }
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}
	o = &(e->item.object);

	/*
	 * examine the JTYPE_MEMBER of the JTYPE_OBJECT
	 *
	 * In our case, each JTYPE_OBJECT of the manifest JTYPE_ARRAY must contain
	 * ONLY 1 thing, a single JTYPE_MEMBER
	 */
	if (o->len != 1) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(125, e, depth+1, sem, __func__,
					"manifest JTYPE_ARRAY[%jd] JTYPE_OBJECT len: %ju != 1",
					i, o->len);
	    }
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}

	/*
	 * firewall - validate JTYPE_MEMBER item in the JTYPE_OBJECT of the manifest JTYPE_ARRAY
	 */
	jo = o->set[0];
	test = sem_node_valid(jo, depth+2, sem, __func__, val_err);
	if (test == false) {
	    /* sem_node_valid() will have set *val_err */
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}
	if (jo->type != JTYPE_MEMBER) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(126, jo, depth+1, sem, __func__,
					"manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT type %s != JTYPE_MEMBER",
					i, json_type_name(jo->type));
	    }
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}

	/*
	 * obtain JTYPE_MEMBER name and value as decoded JSON strings
	 */
	arr_name = sem_member_name_decoded_str(jo, depth+2, sem, __func__, val_err);
	if (arr_name == NULL) {
	    /* sem_member_name_decoded_str() will have set *val_err */
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}
	value = sem_member_value_decoded_str(jo, depth+2, sem, __func__, val_err);
	if (value == NULL) {
	    /* sem_member_value_decoded_str() will have set *val_err */
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}

	/*
	 * count mandatory manifest filenames
	 */
	/* case: info_JSON */
	if (strcmp(arr_name, "info_JSON") == 0) {

	    /* validate info_JSON filename */
	    test = test_info_JSON(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(128, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "info_JSON filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_info_JSON;

	    /* we are allowed only 1 of these mandatory manifest filenames */
	    if (man.count_info_JSON != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(129, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one info_JSON filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: auth_JSON */
	} else if (strcmp(arr_name, "auth_JSON") == 0) {

	    /* validate auth_JSON filename */
	    test = test_auth_JSON(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(130, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "auth_JSON filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_auth_JSON;

	    /* we are allowed only 1 of these mandatory manifest filenames */
	    if (man.count_auth_JSON != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(131, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one auth_JSON filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: c_src */
	} else if (strcmp(arr_name, "c_src") == 0) {

	    /* validate c_src filename */
	    test = test_c_src(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(132, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "c_src filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_c_src;

	    /* we are allowed only 1 of these mandatory manifest filenames */
	    if (man.count_c_src != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(133, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one c_src filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: Makefile */
	} else if (strcmp(arr_name, "Makefile") == 0) {

	    /* validate Makefile filename */
	    test = test_Makefile(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(134, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "Makefile filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_Makefile;

	    /* we are allowed only 1 of these mandatory manifest filenames */
	    if (man.count_Makefile != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(135, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one Makefile filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: remarks */
	} else if (strcmp(arr_name, "remarks") == 0) {

	    /* validate remarks filename */
	    test = test_remarks(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(136, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "remarks filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_remarks;

	    /* we are allowed only 1 of these mandatory manifest filenames */
	    if (man.count_remarks != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(137, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one remarks filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: c_alt_src */
	} else if (strcmp(arr_name, "c_alt_src") == 0) {

	    /* validate remarks filename */
	    test = test_c_alt_src(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(138, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "c_alt_src filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_c_alt_src;

	    /* we are allowed only 1 of these optional manifest filenames */
	    if (man.count_c_alt_src != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(139, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one c_alt_src filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: try_sh */
	} else if (strcmp(arr_name, "try_sh") == 0) {

	    /* validate remarks filename */
	    test = test_try_sh(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(140, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "try_sh filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_try_sh;

	    /* we are allowed only 1 of these optional manifest filenames */
	    if (man.count_try_sh != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(141, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one try_sh filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: try_alt_sh */
	} else if (strcmp(arr_name, "try_alt_sh") == 0) {

	    /* validate remarks filename */
	    test = test_try_alt_sh(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(142, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "try_alt_sh filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* count valid occurrence */
	    ++man.count_try_alt_sh;

	    /* we are allowed only 1 of these optional manifest filenames */
	    if (man.count_try_alt_sh != 1) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(143, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "found more than one try_alt_sh filename", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	/* case: shell_script */
	} else if (strcmp(arr_name, "shell_script") == 0) {

	    /* validate remarks filename */
	    test = test_shell_script(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(144, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "shell_script filename is invalid", i);
		}
		dyn_array_free(man.extra);
	        dyn_array_free(man.shell);
		return false;
	    }

	    /* append pointer to extra_file filename to dynamic array */
	    (void) dyn_array_append_value(man.shell, &value);
	    /* count valid occurrence */
	    ++man.count_shell_script;

	/*
	 * case: optional extra_file
	 */
	} else if (strcmp(arr_name, "extra_file") == 0) {

	    /* validate extra_file filename */
	    test = test_extra_filename(value);
	    if (test == false) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(145, jo, depth+2, sem, __func__,
					    "manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT JTYPE_MEMBER "
					    "extra_file #%jd filename is invalid", i, man.count_extra_file);
		}
		dyn_array_free(man.extra);
		dyn_array_free(man.shell);
		return false;
	    }

	    /* append pointer to shell_file filename to dynamic array */
	    (void) dyn_array_append_value(man.extra, &value);

	    /* count valid occurrence */
	    ++man.count_extra_file;

	/*
	 * case: invalid JTYPE_MEMBER - not part of an IOCCC manifest JTYPE_OBJECT
	 */
	} else {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(146, jo, depth+2, sem, __func__,
					"manifest JTYPE_ARRAY[%jd] 0th JTYPE_OBJECT "
					"has invalid JTYPE_MEMBER name: <%s>", i, arr_name);
	    }
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}
    }

    /*
     * verify that we have 1 each of the required mandatory files in manifest
     */
    if (man.count_info_JSON != 1) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(147, node, depth+2, sem, __func__,
				    "manifest: expected 1 valid info_JSON, found: %jd",
				    man.count_info_JSON);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }
    if (man.count_auth_JSON != 1) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(148, node, depth+2, sem, __func__,
				    "manifest: expected 1 valid auth_JSON, found: %jd",
				    man.count_auth_JSON);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }
    if (man.count_c_src != 1) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(149, node, depth+2, sem, __func__,
				    "manifest: expected 1 valid c_src, found: %jd",
				    man.count_c_src);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }
    if (man.count_Makefile != 1) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(150, node, depth+2, sem, __func__,
				    "manifest: expected 1 valid Makefile, found: %jd",
				    man.count_Makefile);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }
    if (man.count_remarks != 1) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(151, node, depth+2, sem, __func__,
				    "manifest: expected 1 valid remarks, found: %jd",
				    man.count_remarks);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }

    /*
     * verify that we do not have too many extra filenames or a bogus < 0 extra
     * filenames
     */
    if (man.count_extra_file < 0 || man.count_extra_file > MAX_EXTRA_FILE_COUNT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(152, node, depth+2, sem, __func__,
				    "manifest: man.count_extra_file: %jd must be >=0 and < %d",
				    man.count_extra_file, MAX_EXTRA_FILE_COUNT);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }

    /*
     * verify that we do not have too many shell filenames or a bogus < 0 shell
     * filenames
     */
    if (man.count_shell_script < 0 || man.count_shell_script > MAX_EXTRA_FILE_COUNT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(153, node, depth+2, sem, __func__,
				    "manifest: man.count_shell_script: %jd must be >=0 and < %d",
				    man.count_shell_script, MAX_EXTRA_FILE_COUNT);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }

    /*
     * verify that the total number of shell scripts and extra files is not >
     * the MAX_EXTRA_FILE_COUNT
     */
    if (man.count_shell_script + man.count_shell_script > MAX_EXTRA_FILE_COUNT) {
	if (val_err != NULL) {
	    *val_err = werr_sem_val(154, node, depth+2, sem, __func__,
                "manifest: man.count_extra_file %jd + man.count_shell_script %jd: %jd must be >=0 and < %d",
                man.count_extra_file, man.count_shell_script, man.count_extra_file+man.count_shell_script,
                MAX_EXTRA_FILE_COUNT);
	}
	dyn_array_free(man.extra);
	dyn_array_free(man.shell);
        return false;
    }

    /*
     * check for duplicates among valid extra filenames
     *
     * The count_extra_file will be a small number, so we can get away with a
     * lazy O(n^2) match.
     */
    for (i=1; i < man.count_extra_file; ++i) {

	/* obtain first extra filename to compare against */
	extra_filename = dyn_array_value(man.extra, char *, i);
	if (extra_filename == NULL) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(155, node, depth+2, sem, __func__,
					"manifest extra[i = %jd] is NULL", i);
	    }
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}

	/*
	 * compare first extra filename with the remaining extra filenames
	 */
	for (j=0; j < i; ++j) {

	    /* obtain second extra filename */
	    extra_filename2 = dyn_array_value(man.extra, char *, j);
	    if (extra_filename2 == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(156, node, depth+2, sem, __func__,
					    "manifest extra[j = %jd] is NULL", j);
		}
		dyn_array_free(man.extra);
		dyn_array_free(man.shell);
		return false;
	    }

	    /*
	     * compare first and second extra filenames
	     */
	    if (strcmp(extra_filename, extra_filename2) == 0) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(157, node, depth+2, sem, __func__,
					    "manifest extra[%jd] filename: matches manifest extra[%jd] filename",
					    i, j);
		}
		dyn_array_free(man.extra);
		dyn_array_free(man.shell);
		return false;
	    }
	}
    }

    /*
     * check for duplicates among valid shell filenames
     *
     * The count_shell_script will be a small number, so we can get away with a
     * lazy O(n^2) match.
     */
    for (i=1; i < man.count_shell_script; ++i) {

	/* obtain first shell_script filename to compare against */
	shell_filename = dyn_array_value(man.shell, char *, i);
	if (shell_filename == NULL) {
	    if (val_err != NULL) {
		*val_err = werr_sem_val(158, node, depth+2, sem, __func__,
					"manifest shell[i = %jd] is NULL", i);
	    }
	    dyn_array_free(man.extra);
	    dyn_array_free(man.shell);
	    return false;
	}

	/*
	 * compare first shell_script filename with the remaining shell_script filenames
	 */
	for (j=0; j < i; ++j) {

	    /* obtain second shell_script filename */
	    shell_filename2 = dyn_array_value(man.shell, char *, j);
	    if (shell_filename2 == NULL) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(159, node, depth+2, sem, __func__,
					    "manifest shell[j = %jd] is NULL", j);
		}
		dyn_array_free(man.extra);
		dyn_array_free(man.shell);
		return false;
	    }

	    /*
	     * compare first and second shell_script filenames
	     */
	    if (strcmp(shell_filename, shell_filename2) == 0) {
		if (val_err != NULL) {
		    *val_err = werr_sem_val(160, node, depth+2, sem, __func__,
					    "manifest shell[%jd] filename: matches manifest shell[%jd] filename",
					    i, j);
		}
		dyn_array_free(man.extra);
		dyn_array_free(man.shell);
		return false;
	    }
	}
    }

    /*
     * check for any shell_script that is the same path as an extra_file if
     * both are > 0 in size.
     */
    if (man.count_extra_file > 0 && man.count_shell_script > 0) {
        for (i=1; i < man.count_extra_file; ++i) {
            /* obtain extra filename to compare against */
            extra_filename = dyn_array_value(man.extra, char *, i);
            if (extra_filename == NULL) {
                if (val_err != NULL) {
                    *val_err = werr_sem_val(161, node, depth+2, sem, __func__,
                                            "manifest extra[i = %jd] is NULL", i);
                }
                dyn_array_free(man.extra);
                dyn_array_free(man.shell);
                return false;
            }

            /*
             * compare extra filename with the shell_script filenames
             */
            for (j=0; j < man.count_shell_script; ++j) {

                /* obtain second extra filename */
                shell_filename = dyn_array_value(man.shell, char *, j);
                if (shell_filename == NULL) {
                    if (val_err != NULL) {
                        *val_err = werr_sem_val(162, node, depth+2, sem, __func__,
                                                "manifest shell[j = %jd] is NULL", j);
                    }
                    dyn_array_free(man.extra);
                    dyn_array_free(man.shell);
                    return false;
                }

                /*
                 * compare first and second extra filenames
                 */
                if (strcmp(extra_filename, shell_filename) == 0) {
                    if (val_err != NULL) {
                        *val_err = werr_sem_val(163, node, depth+2, sem, __func__,
                                                "manifest extra[%jd] filename: matches manifest shell[%jd] filename",
                                                i, j);
                    }
                    dyn_array_free(man.extra);
                    dyn_array_free(man.shell);
                    return false;
                }
            }
        }
    }

    /*
     * load manifest array
     */
    *manp = man;

    /*
     * report success in loading struct manifest
     */
    return true;
}


/*
 * form_tar_filename - return a malloced tarball filename
 *
 * given:
 *	IOCCC_contest_id	IOCCC contest UUID or test
 *	submit_slot		IOCCC entry number
 *	test_mode		true ==> IOCCC entry is just a test
 *	formed_timestamp	timestamp of when entry was formed
 *
 * returns:
 *	malloced tarball filename or NULL ==> error
 */
char *
form_tar_filename(char const *IOCCC_contest_id, int submit_slot, bool test_mode,
		  time_t formed_timestamp)
{
    size_t tarball_len;			/* length of the compressed tarball path */
    char *tarball_filename = NULL;	/* malloced tarball filename to return */
    bool test = false;			/* test result */
    int ret;				/* libc function return */

    /*
     * firewall
     */
    if (IOCCC_contest_id == NULL) {
	warn(__func__, "IOCCC_contest_id is NULL");
	return NULL;
    } else if (*IOCCC_contest_id == '\0') { /* strlen(IOCCC_contest_id) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: IOCCC_contest_id is empty");
	return NULL;
    }
    test = test_submit_slot(submit_slot);
    if (test == false) {
	/* test_submit_slot() already issued json_dbg() messages */
	return NULL;
    }
    test = test_formed_timestamp(formed_timestamp);
    if (test == false) {
	/* test_formed_timestamp() already issued json_dbg() messages */
	return NULL;
    }

    /*
     * case: test mode specific test
     */
    if (test_mode == true) {

	/*
	 * test mode IOCCC_contest_id must be test
	 */
	if (strcmp(IOCCC_contest_id, "test") != 0) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: test mode and IOCCC_contest_id != test");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: test mode and IOCCC_contest_id: <%s> != test", IOCCC_contest_id);
	    return NULL;
	}

    /*
     * case: non-test (official entry) mode specific test
     */
    } else {

	/*
	 * non-test mode IOCCC_contest_id check
	 */
	test = test_IOCCC_contest_id(IOCCC_contest_id);
	if (test == false) {
	    /* test_IOCCC_contest_id() already issued json_dbg() messages */
	    return NULL;
	}

    }

    /*
     * allocate space for tarball filename
     */
    tarball_len = LITLEN("submit.") + strlen(IOCCC_contest_id) + 1 + MAX_SUBMIT_SLOT_CHARS + LITLEN(".123456789012.txz") + 1;
    errno = 0;			/* pre-clear errno for warnp() */
    tarball_filename = (char *)calloc(tarball_len + 1, sizeof(*tarball_filename));
    if (tarball_filename == NULL) {
	warnp(__func__, "malloc #1 of %zu bytes failed", (tarball_len + 1));
	return NULL;
    }
    tarball_filename[tarball_len] = '\0';	/* paranoia */

    /*
     * load tarball filename
     */
    errno = 0;			/* pre-clear errno for warnp() */
    if ((time_t)-1 > 0) {
	/* case: unsigned time_t */
	ret = snprintf(tarball_filename, tarball_len + 1, "submit.%s-%d.%ju.txz",
		       IOCCC_contest_id, submit_slot, (uintmax_t)formed_timestamp);
    } else {
	/* case: signed time_t */
	ret = snprintf(tarball_filename, tarball_len + 1, "submit.%s-%d.%jd.txz",
		       IOCCC_contest_id, submit_slot, (intmax_t)formed_timestamp);
    }
    if (ret <= 0) {
	warnp(__func__, "snprintf to form compressed tarball path failed");
	if (tarball_filename != NULL) {
	    free(tarball_filename);
	    tarball_filename = NULL;
	}
	return NULL;
    }

    /*
     * return allocated tarball filename
     */
    return tarball_filename;
}


/*
 * test_version - test if a version is >= a minimum version
 *
 * given:
 *
 *  str     - version to check
 *  min     - min version to check against
 *
 * Returns true if version >= min, false otherwise.
 */
bool
test_version(char const *str, char const *min)
{
    char *dup1 = NULL;
    char *dup2 = NULL;
    char *ver1 = NULL;
    char *ver2 = NULL;

    if (str == NULL || *str == '\0') {
        err(164, __func__, "str is NULL or empty string");
        not_reached();
    }
    if (min == NULL || *min == '\0') {
        err(165, __func__, "min is NULL or empty string");
        not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    dup1 = strdup(str);
    if (dup1 == NULL) {
        err(166, __func__, "strdup(str) returned NULL");
        not_reached();
    }
    errno = 0; /* pre-clear errno for errp() */
    dup2 = strdup(min);
    if (dup2 == NULL) {
        err(167, __func__, "strdup(min) returned NULL");
        not_reached();
    }

    ver1 = strchr(dup1, ' ');
    if (ver1 != NULL) {
        *ver1 = '\0';
    }
    ver2 = strchr(dup2, ' ');
    if (ver2 != NULL) {
        *ver2 = '\0';
    }

    if (vercmp(dup1, dup2) == 0) {
        return true;
    }

    return false;
}


/*
 * test_poisons - test if a version is a so-called poison version
 *
 * given:
 *
 *  str     - version to check
 *  min     - min version to check against
 *
 * Returns true if poisoned version, false otherwise
 */
bool
test_poison(char const *str, char const **poisons)
{
    size_t i;

    if (str == NULL || *str == '\0') {
        err(168, __func__, "str is NULL or empty string");
        not_reached();
    }
    if (poisons == NULL) {
        err(169, __func__, "poisons list is NULL");
        not_reached();
    }

    for (i = 0; poisons[i]; ++i) {
        if (!strcasecmp(str, poisons[i])) {
            return true;
        }
    }

    return false;
}


/*
 * test_IOCCC_auth_version - test if IOCCC_auth_version is valid
 *
 * Determine if IOCCC_auth_version matches AUTH_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_IOCCC_auth_version(char const *str)
{
    size_t i = 0;
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
    if (test_poison(str, poison_auth_versions)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: IOCCC_auth_version: is poisonous");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: IOCCC_auth_version: %s is poisonous: %s", str, poison_auth_versions[i]);
        return false;
    }
    if (!test_version(str, MIN_AUTH_VERSION)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: IOCCC_auth_version < MIN_AUTH_VERSION: %s", MIN_AUTH_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: IOCCC_auth_version: %s is not >= MIN_AUTH_VERSION: %s", str, MIN_AUTH_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "IOCCC_auth_version is valid");
    return true;
}


/*
 * test_IOCCC_contest_id - test if IOCCC_contest_id is valid
 *
 * Determine if IOCCC_contest_id is test or a UUID version 4 variant 1.
 *
 * The contest ID, if not "test" must be a UUID.  The UUID has the 36 character format:
 *
 *             xxxxxxxx-xxxx-4xxx-Nxxx-xxxxxxxxxxxx
 *
 * where 'x' is a hex character,  is the UUID version, and N is one of 8, 9, a, or b.
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
test_IOCCC_contest_id(char const *str)
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
		 "invalid: UUID version hex char != UUID_VERSION %x", UUID_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: UUID version hex char: %1x is not UUID_VERSION: %x", version, UUID_VERSION);
	return false;
    }
    if (variant != UUID_VARIANT_0 && variant != UUID_VARIANT_1 && variant != UUID_VARIANT_2 && variant != UUID_VARIANT_3) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: UUID variant hex char not one of: %x, %x, %x, %x", UUID_VARIANT_0, UUID_VARIANT_1,
                 UUID_VARIANT_2, UUID_VARIANT_3);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: UUID variant hex char: %1x is not one of: %x, %x, %x, %x", variant, UUID_VARIANT_0,
                 UUID_VARIANT_1, UUID_VARIANT_2, UUID_VARIANT_3);
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
test_IOCCC_info_version(char const *str)
{
    size_t i = 0;
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
    if (test_poison(str, poison_info_versions)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: IOCCC_info_version: is poisonous");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: IOCCC_info_version: %s is poisonous: %s", str, poison_info_versions[i]);
        return false;
    }
    if (!test_version(str, MIN_INFO_VERSION)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: IOCCC_info_version < MIN_INFO_VERSION: %s", MIN_INFO_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: IOCCC_info_version: %s is not >= MIN_INFO_VERSION: %s", str, MIN_INFO_VERSION);
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
test_Makefile(char const *str)
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
		 "invalid: Makefile != Makefile: %s", MAKEFILE_FILENAME);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: Makefile: %s is not Makefile: %s", str, MAKEFILE_FILENAME);
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
 * Determine if abstract length is > 0 && <= MAX_ABSTRACT_LEN.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_abstract(char const *str)
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
		 "invalid: abstract length %zu > max %d", length, MAX_ABSTRACT_LEN);
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
test_affiliation(char const *str)
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
		 "invalid: affiliation length %zu > max %d", length, MAX_AFFILIATION_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: affiliation: <%s> is invalid", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "affiliation is valid");
    return true;
}


/*
 * test_auth_JSON - test if auth_JSON is valid
 *
 * Determine if auth_JSON matches AUTH_JSON_FILENAME.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_auth_JSON(char const *str)
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
    if (strcmp(str, AUTH_JSON_FILENAME) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: auth_JSON != AUTH_JSON_FILENAME: %s", AUTH_JSON_FILENAME);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: auth_JSON: %s is not AUTH_JSON_FILENAME: %s", str, AUTH_JSON_FILENAME);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "auth_JSON is valid");
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
test_author_handle(char const *str)
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
		 "invalid: author_handle length %zu > max %d", length, MAX_HANDLE);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: author_handle: <%s> is invalid", str);
	return false;
    }
    /* IOCCC author_handle must use POSIX portable filename and + chars */
    test = safe_path_str(str, true, false); /* ^[0-9A-Za-z._][0-9A-Za-z._+-]*$ */
    if (!isascii(str[0]) || !isalnum(str[0]) || test == false) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_handle does not match regexp: ^[0-9A-Za-z][0-9A-Za-z._+-]*$");
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
 * test_authors - test if the authors array contains unique authors
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
test_authors(int author_count, struct author const *authorp)
{
    bool test = false;		/* character test result */
    int author_num = -1;		/* author number */
    int *author_nums = NULL;	/* array of author numbers */
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
    author_nums = calloc((size_t)author_count, sizeof(int));
    if (author_nums == NULL) {
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
	author_num = authorp[i].author_num;
	if (author_num < 0) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] number: %d < 0", i, author_num);
	    /* free array of author numbers */
	    if (author_nums != NULL) {
		free(author_nums);
		author_nums = NULL;
	    }
	    return false;
	} else if (author_num > author_count) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: number[%d] number: %d > author_count: %d", i, author_num, author_count);
	    /* free array of author numbers */
	    if (author_nums != NULL) {
		free(author_nums);
		author_nums = NULL;
	    }
	    return false;
	}

	/*
	 * check the i-th author for a unique author number
	 */
	if (author_nums[author_num] != 0) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] number: %d is not unique, duplicate of author[%d]",
		     i, author_num, author_nums[author_num]);
	    /* free array of author numbers */
	    if (author_nums != NULL) {
		free(author_nums);
		author_nums = NULL;
	    }
	    return false;
	}
	author_nums[author_num] = i;

	/*
	 * pre-check for non-NULL author_handle (for the uniqueness check below)
	 */
	if (authorp[i].author_handle == NULL) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] handle is NULL:", i);
	    /* free array of author numbers */
	    if (author_nums != NULL) {
		free(author_nums);
		author_nums = NULL;
	    }
	    return false;
	}

	/*
	 * pre-check for non-NULL name (for the uniqueness check below)
	 */
	if (authorp[i].name == NULL) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: author[%d] name is NULL:", i);
	    /* free array of author numbers */
	    if (author_nums != NULL) {
		free(author_nums);
		author_nums = NULL;
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
		if (author_nums != NULL) {
		    free(author_nums);
		    author_nums = NULL;
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
		if (author_nums != NULL) {
		    free(author_nums);
		    author_nums = NULL;
		}
		return false;
	    }
	}
    }

    /*
     * return success
     */
    /* free array of author numbers */
    if (author_nums != NULL) {
	free(author_nums);
	author_nums = NULL;
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
test_c_src(char const *str)
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
    if (strcmp(str, PROG_C_FILENAME) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: c_src != prog.c: %s", PROG_C_FILENAME);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: c_src: %s is not prog.c: %s", str, PROG_C_FILENAME);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "c_src filename is valid");
    return true;
}


/*
 * test_chksubmit_version - test if chksubmit_version is valid
 *
 * Determine if chksubmit_version matches CHKSUBMIT_VERSION.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_chksubmit_version(char const *str)
{
    size_t i = 0;
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
    if (test_poison(str, poison_chksubmit_versions)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: chksubmit_version: is poisonous");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: chksubmit_version: %s is poisonous: %s", str, poison_chksubmit_versions[i]);
        return false;
    }
    if (!test_version(str, MIN_CHKSUBMIT_VERSION)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: chksubmit_version < MIN_CHKSUBMIT_VERSION: %s", MIN_CHKSUBMIT_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: chksubmit_version: %s is not >= MIN_CHKSUBMIT_VERSION: %s", str, MIN_CHKSUBMIT_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "chksubmit_version is valid");
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
 *
 * NOTE: we don't check for domains in a way that would validate it as a
 * properly named domain name in the email address. It is outside the scope of
 * this code and the regex for email is ridiculously complicated. See
 * https://www.regular-expressions.info/email.html and scroll down to 'The
 * Official Standard: RFC 5322' for details.
 */
bool
test_email(char const *str)
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
     * reject if no @ or the first char is @ or the last char is @ or if more than one @
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
 * test_submit_slot - test if submit_slot is valid
 *
 * Determine if submit_slot is within the proper limits.
 *
 * given:
 *	submit_slot	author number
 *
 * returns:
 *	true ==> submit_slot is valid,
 *	false ==> submit_slot is NOT valid, or some internal error
 */
bool
test_submit_slot(int submit_slot)
{
    /*
     * validate count
     */
    if (submit_slot < 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: submit_slot: %d < 0", submit_slot);
	return false;
    } else if (submit_slot > MAX_SUBMIT_SLOT) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: submit_slot: %d > MAX_SUBMIT_SLOT: %d", submit_slot, MAX_SUBMIT_SLOT);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "submit_slot is valid");
    return true;
}


/*
 * test_extra_filename - test if extra_file is valid
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
test_extra_filename(char const *str)
{
    enum path_sanity sanity = PATH_OK;  /* canon_path path_sanity error, or PATH_OK */
    size_t len = 0;                     /* length of str */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * get length
     */
    len = strlen(str);
    /*
     * validate str
     */

    /*
     * validate that the filename is POSIX portable safe plus + chars
     */
    /* IMPORTANT: canon_path() MUST use a false "lower_case" arg!  See the path_in_item_array() function. */
    (void) canon_path(str, MAX_PATH_LEN, MAX_FILENAME_LEN, MAX_PATH_DEPTH,
		      &sanity, NULL, NULL, true, false, true, true, NULL);
    if (sanity != PATH_OK) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: canon_path safety check on extra_file failed");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: canon_path safety check on extra_file failed: <%s>", str);
	return false;
    }

    /* verify that extra_file does not match a mandatory filename */
    if (is_mandatory_filename(str)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: extra_file matches a mandatory file");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: extra_file matches a mandatory file: <%s>", str);
        return false;
    } else if (len > LITLEN(".sh") && strcmp(str + len - LITLEN(".sh"), ".sh") == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: extra_file filename ends in .sh");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: extra_file filename: <%s> ends in .sh", str);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "extra_file is valid");
    return true;
}

/*
 * test_filename_len - test filename length
 *
 * Determine if the filename length is > 0 && <= MAX_FILENAME_LEN.
 *
 * given:
 *	str	filename to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_filename_len(char const *str)
{
    size_t len = 0;      /* length of filename */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    len = strlen(str);

    /*
     * validate filename
     */
    if (len <= 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: filename length <= 0");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: filename: %s length is <= 0: %zu", str, len);
	return false;
    } else if (len > MAX_FILENAME_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: filename length > MAX_FILENAME_LEN");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: filename: %s length %zu > %d", str, len, MAX_FILENAME_LEN);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "filename length is valid");
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
test_fnamchk_version(char const *str)
{
    size_t i = 0;
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
    if (test_poison(str, poison_fnamchk_versions)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: fnamchk_version: is poisonous");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: fnamchk_version: %s is poisonous: %s", str, poison_fnamchk_versions[i]);
        return false;
    }
    if (!test_version(str, MIN_FNAMCHK_VERSION)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: fnamchk_version < MIN_FNAMCHK_VERSION: %s", MIN_FNAMCHK_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: fnamchk_version: %s is not >= MIN_FNAMCHK_VERSION: %s", str, MIN_FNAMCHK_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "fnamchk_version is valid");
    return true;
}


/*
 * test_formed_timestamp - test if formed_timestamp is valid
 *
 * Determine if formed_timestamp is >= MIN_TIMESTAMP and if
 * formed_timestamp <= now + FUTURE_CLOCK_SKEW_LIMIT.
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
    time_t now = 0;			/* the time now */

    /*
     * record the time
     */
    now = time(NULL);
    if (now == (time_t)-1) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: time(3) returned an error");
	return false;
    }
    if ((time_t)-1 > 0) {
	/* case: unsigned time_t */
	json_dbg(JSON_DBG_HIGH, __func__, "now: %ju",
		 (uintmax_t)now);
	json_dbg(JSON_DBG_HIGH, __func__, "now+FUTURE_CLOCK_SKEW_LIMIT: %ju",
		 (uintmax_t)(now+FUTURE_CLOCK_SKEW_LIMIT));
    } else {
	/* case: signed time_t */
	json_dbg(JSON_DBG_HIGH, __func__, "now: %jd",
		 (intmax_t)now);
	json_dbg(JSON_DBG_HIGH, __func__, "now+FUTURE_CLOCK_SKEW_LIMIT: %jd",
		 (intmax_t)(now+FUTURE_CLOCK_SKEW_LIMIT));
    }

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
	    /* case: signed time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: formed_timestamp: %jd < MIN_TIMESTAMP: %jd",
		     (intmax_t)tstamp, (intmax_t)MIN_TIMESTAMP);
	}
	return false;

    /*
     * compare with now + clock error
     */
    } else if (tstamp > (now+FUTURE_CLOCK_SKEW_LIMIT)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: formed_timestamp > now+FUTURE_CLOCK_SKEW_LIMIT");
	if ((time_t)-1 > 0) {
	    /* case: unsigned time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: formed_timestamp: %ju > now+FUTURE_CLOCK_SKEW_LIMIT: %ju",
		     (uintmax_t)tstamp, (uintmax_t)(now+FUTURE_CLOCK_SKEW_LIMIT));
	} else {
	    /* case: signed time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: formed_timestamp: %jd > now+FUTURE_CLOCK_SKEW_LIMIT: %jd",
		     (intmax_t)tstamp, (intmax_t)(now+FUTURE_CLOCK_SKEW_LIMIT));
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
 * given:
 *	formed_timestamp_usec	- formed timestamp usec number
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
    if (formed_timestamp_usec < MIN_FORMED_TIMESTAMP_USEC) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: formed_timestamp_usec: formed_timestamp_usec < %d", MIN_FORMED_TIMESTAMP_USEC);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: formed_timestamp_usec: %d < %d", formed_timestamp_usec, MIN_FORMED_TIMESTAMP_USEC);
	return false;
    } else if (formed_timestamp_usec > MAX_FORMED_TIMESTAMP_USEC) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: formed_timestamp_usec > MAX_FORMED_TIMESTAMP_USEC %d", MAX_FORMED_TIMESTAMP_USEC);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: formed_timestamp_usec: %d > MAX_FORMED_TIMESTAMP_USEC %d", formed_timestamp_usec,
                 MAX_FORMED_TIMESTAMP_USEC);
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
test_github(char const *str)
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
		 "invalid: github length > max %d", MAX_GITHUB_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: github: <%s> is invalid", str);
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
    } else if (str[1] == '\0') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: github account has no chars after '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: github: <%s> is invalid", str);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "github is valid");
    return true;
}

/*
 * test_info_JSON - test if info_JSON is valid
 *
 * Determine if info_JSON matches INFO_JSON_FILENAME.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_info_JSON(char const *str)
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
 * test_IOCCC_contest - test if IOCCC_contest is valid
 *
 * Determine if IOCCC_contest matches AUTH_JSON_FILENAME.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_IOCCC_contest(char const *str)
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
		 "invalid: IOCCC_contest != IOCCC_CONTEST: %s", IOCCC_CONTEST);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: IOCCC_contest: %s is not IOCCC_CONTEST: %s", str, IOCCC_CONTEST);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "IOCCC_contest is valid");
    return true;
}


/*
 * test_IOCCC_year - test if IOCCC_year is valid
 *
 * Determine if IOCCC_year is within the proper limits.
 *
 * given:
 *	IOCCC_year	year
 *
 * returns:
 *	true ==> IOCCC_year is valid,
 *	false ==> IOCCC_year is NOT valid, or some internal error
 */
bool
test_IOCCC_year(int IOCCC_year)
{
    /*
     * validate count
     */
    if (IOCCC_year != IOCCC_YEAR) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: IOCCC_year != %d", IOCCC_YEAR);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: IOCCC_year %d != %d", IOCCC_year, IOCCC_YEAR);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "IOCCC_year is valid");
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
test_iocccsize_version(char const *str)
{
    size_t i = 0;
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
    if (test_poison(str, poison_iocccsize_versions)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: iocccsize_version: is poisonous");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: iocccsize_version: %s is poisonous: %s", str, poison_iocccsize_versions[i]);
        return false;
    }
    if (!test_version(str, MIN_IOCCCSIZE_VERSION)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: iocccsize_version < MIN_IOCCCSIZE_VERSION: %s", MIN_IOCCCSIZE_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: iocccsize_version: %s is not >= MIN_IOCCCSIZE_VERSION: %s", str, MIN_IOCCCSIZE_VERSION);
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
test_location_code(char const *str)
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
		 "invalid: location_code: length != 2");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: location_code: <%s> length: %zu != 2", str, length);
	return false;
    }

    /* validate 2 ASCII UPPER CASE characters */
    if (!isascii(str[0]) || !isupper(str[0]) || !isascii(str[1]) || !isupper(str[1])) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: location_code: is not 2 ASCII UPPER CASE characters");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: location_code: <%s> is not 2 ASCII UPPER CASE characters", str);
	return false;
    }

    /* validate ISO 3166-1 Alpha-2 in code */
    location_name = lookup_location_name(str, false);
    if (location_name == NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: location_code: not a known ISO 3166-1 location/country code");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: location_code: <%s> not a known ISO 3166-1 location/country code", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "location_code is valid");
    return true;
}


/*
 * test_manifest - test is the manifest is complete and has unique extra files
 *
 * We verify that the manifest has all of the required files, and that if
 * there are extra files, those files are valid and unique.
 *
 * The IOCCC manifest MUST contain 1 and only 1 of these mandatory files:
 *
 *	info_JSON	".info.json"
 *	auth_JSON	".auth.json"
 *	c_src		"prog.c"
 *	Makefile	"Makefile"
 *	remarks		"remarks.md"
 *
 * In addition, the IOCCC manifest may contain 0 or more extra files:
 *
 *	extra_file
 *
 * provided that those extra filenames do NOT match one of the above
 * mentioned mandatory files AND that the extra filename is POSIX portable
 * safe plus + chars AND they are not duplicated either AND any other check in
 * test_extra_filename().
 *
 * If any file in the manifest does not exist in the submission directory, it is
 * an error.
 *
 * given:
 *	manp		    pointer struct manifest
 *	submission_dir      submission directory path
 *
 * returns:
 *	true ==> manifest is complete with unique extra files
 *	false ==> manifest is incomplete, or extra files not unique or invalid,
 *		  or NULL pointer, or some internal error
 */
bool
test_manifest(struct manifest *manp, char *submission_dir)
{
    intmax_t count_extra_file = -1;	/* number of extra files */
    intmax_t count_shell_script = -1;	/* number of shell scripts */
    char *extra_filename = NULL;	/* filename of an extra file */
    char *extra_filename2 = NULL;	/* second filename of an extra file */
    char *shell_filename = NULL;	/* filename of a shell scrip */
    char *shell_filename2 = NULL;	/* second filename of a second shell script */
    char *pathname = NULL;              /* path name we're currently testing */
    struct walk_stat wstat;		/* walk_stat being processed */
    struct walk_set *wset_p = NULL;	/* pointer to a walk set */
    char const *context = NULL;		/* string describing context (tool & options) for debugging purposes */
    bool walk_ok = true;		/* true ==> no walk errors found, false ==> some walk errors found */
    struct item *i_p = NULL;		/* pointer to an element in the dynamic array */
    intmax_t i;
    intmax_t j;

    /*
     * firewall
     */
    if (manp == NULL) {
	warn(__func__, "manp is NULL");
	return false;
    }
    if (manp->extra == NULL) {
	warn(__func__, "manp->extra is NULL");
	return false;
    }
    if (manp->count_extra_file < 0) {
	warn(__func__, "manp->count_extra_file: %jd < 0", manp->count_extra_file);
	return false;
    }
    if (manp->count_extra_file != dyn_array_tell(manp->extra)) {
	warn(__func__, "manp->count_extra_file: %jd != dyn_array_tell(manp->extra): %jd",
		       manp->count_extra_file, dyn_array_tell(manp->extra));
	return false;
    }
    if (manp->count_shell_script < 0) {
	warn(__func__, "manp->count_shell_script: %jd < 0", manp->count_shell_script);
	return false;
    }
    if (manp->count_shell_script != dyn_array_tell(manp->shell)) {
	warn(__func__, "manp->count_shell_script: %jd != dyn_array_tell(manp->shell): %jd",
		       manp->count_shell_script, dyn_array_tell(manp->shell));
	return false;
    }
    if (submission_dir == NULL) {
        warn(__func__, "submission_dir is NULL");
        return false;
    }

    /*
     * initialize the walk_stat structure
     */
    memset(&wstat, 0, sizeof(wstat));
    wset_p = &walk_anything;
    context = __func__;
    init_walk_stat(&wstat,
                   ".", wset_p, context,
                   0, 0, 0,
                   false);
    /*
     * walk a file system tree, recording steps
     */
    walk_ok = fts_walk(&wstat);
    if (!walk_ok) {
        err(4, __func__, "failed to scan: %s", "."); /*ooo*/
        not_reached();
    }

    /*
     * all the other options are okay at their default value for the first tests
     *
     * get extra file count
     */
    count_extra_file = manp->count_extra_file;
    /*
     * also get shell script count
     */
    count_shell_script = manp->count_shell_script;

    /*
     * verify .info.json
     */
    pathname = INFO_JSON_FILENAME;
    i_p = path_in_walk_stat(&wstat, pathname);
    if (i_p == NULL) {

	/* report failure to find the pathname */
	warn(__func__, "path_in_walk_stat did not find: %s", pathname);
	free_walk_stat(&wstat);
	return false;

    } else {

	/*
	 * must be a file with mode 0444
	 */
	if (ITEM_IS_NOT_FILE(i_p->st_mode)) {
	    warn(__func__, "not a file: %s", pathname);
	    free_walk_stat(&wstat);
	    return false;
	} else if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0444) {
	    warn(__func__, "permission: %04o != %04o file: %s",
				   i_p->st_mode, ITEM_PERM_0444, pathname);
	    free_walk_stat(&wstat);
	    return false;
	}
    }

    /*
     * verify .auth.json
     */
    pathname = AUTH_JSON_FILENAME;
    i_p = path_in_walk_stat(&wstat, pathname);
    if (i_p == NULL) {

	/* report failure to find the pathname */
	warn(__func__, "path_in_walk_stat did not find: %s", pathname);
	free_walk_stat(&wstat);
	return false;

    } else {

	/*
	 * must be a file with mode 0444
	 */
	if (ITEM_IS_NOT_FILE(i_p->st_mode)) {
	    warn(__func__, "not a file: %s", pathname);
	    free_walk_stat(&wstat);
	    return false;
	} else if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0444) {
	    warn(__func__, "permission: %04o != %04o file: %s",
				   i_p->st_mode, ITEM_PERM_0444, pathname);
	    free_walk_stat(&wstat);
	    return false;
	}
    }

    /*
     * verify prog.c
     */
    pathname = PROG_C_FILENAME;
    i_p = path_in_walk_stat(&wstat, pathname);
    if (i_p == NULL) {

	/* report failure to find the pathname */
	warn(__func__, "path_in_walk_stat did not find: %s", pathname);
	free_walk_stat(&wstat);
	return false;

    } else {

	/*
	 * must be a file with mode 0444
	 */
	if (ITEM_IS_NOT_FILE(i_p->st_mode)) {
	    warn(__func__, "not a file: %s", pathname);
	    free_walk_stat(&wstat);
	    return false;
	} else if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0444) {
	    warn(__func__, "permission: %04o != %04o file: %s",
				   i_p->st_mode, ITEM_PERM_0444, pathname);
	    free_walk_stat(&wstat);
	    return false;
	}
    }

    /*
     * verify Makefile
     */
    pathname = MAKEFILE_FILENAME;
    i_p = path_in_walk_stat(&wstat, pathname);
    if (i_p == NULL) {

	/* report failure to find the pathname */
	warn(__func__, "path_in_walk_stat not found: %s", pathname);
	free_walk_stat(&wstat);
	return false;

    } else {

	/*
	 * must be a file with mode 0444
	 */
	if (ITEM_IS_NOT_FILE(i_p->st_mode)) {
	    warn(__func__, "not a file: %s", pathname);
	    free_walk_stat(&wstat);
	    return false;
	} else if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0444) {
	    warn(__func__, "permission: %04o != %04o file: %s",
				   i_p->st_mode, ITEM_PERM_0444, pathname);
	    free_walk_stat(&wstat);
	    return false;
	}
    }

    /*
     * verify remarks.md
     */
    pathname = REMARKS_FILENAME;
    i_p = path_in_walk_stat(&wstat, pathname);
    if (i_p == NULL) {

	/* report failure to find the pathname */
	warn(__func__, "path_in_walk_stat did not find: %s", pathname);
	free_walk_stat(&wstat);
	return false;

    } else {

	/*
	 * must be a file with mode 0444
	 */
	if (ITEM_IS_NOT_FILE(i_p->st_mode)) {
	    warn(__func__, "not a file: %s", pathname);
	    free_walk_stat(&wstat);
	    return false;
	} else if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0444) {
	    warn(__func__, "permission: %04o != %04o file: %s",
				   i_p->st_mode, ITEM_PERM_0444, pathname);
	    free_walk_stat(&wstat);
	    return false;
	}
    }

    /*
     * case: no extra files and no shell scripts
     */
    if (count_extra_file == 0 && count_shell_script == 0) {
	warn(__func__, "manifest is complete with no extra files and no shell scripts");
	free_walk_stat(&wstat);
	return true;
    }

    /*
     * verify that extra files are valid filenames and do not match a mandatory
     * file and verify they have the right permissions.
     */
    for (i=0; i < count_extra_file; ++i) {

	/* obtain this valid extra filename */
	extra_filename = dyn_array_value(manp->extra, char *, i);
	if (extra_filename == NULL) {
	    err(4, __func__, "invalid: manifest extra[%jd] is NULL", i);/*ooo*/
            not_reached();
	}

	/*
	 * validate filename for this extra file
	 */
	i_p = path_in_walk_stat(&wstat, extra_filename);
	if (i_p == NULL) {

	    /* report failure to find the pathname */
	    warn(__func__, "path_in_walk_stat did not find extra file: %s", extra_filename);
	    free_walk_stat(&wstat);
	    return false;

	} else {

	    /*
	     * must be a regular file that is NOT an executable filename AND
             * MUST HAVE mode 0444
	     */
	    if (ITEM_IS_NOT_FILE(i_p->st_mode)) {
		warn(__func__, "not a file: %s", pathname);
		free_walk_stat(&wstat);
		return false;
	    } else if (is_executable_filename(extra_filename)) {
                warn(__func__, "extra_file is an executable filename: %s", pathname);
                free_walk_stat(&wstat);
                return false;
	    } else if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0444) {
                warn(__func__, "permission: %04o != %04o file: %s",
                                       i_p->st_mode, ITEM_PERM_0444, pathname);
                free_walk_stat(&wstat);
                return false;
            }
        }
    }

    /*
     * verify that shell scripts are valid filenames and do not match a
     * mandatory file and verify they have the right permissions.
     */
    for (i=0; i < count_shell_script; ++i) {

	/* obtain this valid shell script filename */
	shell_filename = dyn_array_value(manp->shell, char *, i);
	if (shell_filename == NULL) {
	    err(4, __func__, "invalid: manifest shell[%jd] is NULL", i);/*ooo*/
            not_reached();
	}

	/*
	 * validate filename for this shell script file
	 */
	i_p = path_in_walk_stat(&wstat, shell_filename);
	if (i_p == NULL) {

	    /* report failure to find the pathname */
	    warn(__func__, "path_in_walk_stat did not find shell_script file: %s", shell_filename);
	    free_walk_stat(&wstat);
	    return false;

	} else {

	    /*
	     * must be a file with mode 0555
	     */
	    if (ITEM_IS_NOT_FILE(i_p->st_mode)) {
		warn(__func__, "not a file: %s", pathname);
		free_walk_stat(&wstat);
		return false;
	    } else if (!is_executable_filename(shell_filename)) {
		if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0555) {
		    warn(__func__, "permission: %04o != %04o file: %s",
					   i_p->st_mode, ITEM_PERM_0555, pathname);
		    free_walk_stat(&wstat);
		    return false;
		}
	    } else if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0555) {
                warn(__func__, "permission: %04o != %04o file: %s",
                                       i_p->st_mode, ITEM_PERM_0555, pathname);
                free_walk_stat(&wstat);
                return false;
            }
        }
    }

    /*
     * case: only 1 extra file
     */
    if ((count_extra_file == 1 && count_shell_script == 0) || (count_shell_script == 1 && count_extra_file == 0)) {
	json_dbg(JSON_DBG_MED, __func__, "manifest is complete with only 1 valid extra filename or 1 shell script filename");

	free_walk_stat(&wstat);
	return true;
    }

    /*
     * check for duplicates among valid extra filenames. Also check against all
     * shell script filenames (i.e. does it end in .sh?) and check if is a
     * mandatory filename. The executable filename check is redundant from above
     * but we do it as an extra sanity check.
     */
    for (i=1; i < count_extra_file; ++i) {

	/* obtain first extra filename to compare against */
	extra_filename = dyn_array_value(manp->extra, char *, i);
	if (extra_filename == NULL) {
	    err(4, __func__, "invalid: manifest extra[i = %jd] is NULL", i);/*ooo*/
	    free_walk_stat(&wstat);
	    return false;
	}

        /*
         * if this is a mandatory filename it is an error
         */
        if (is_mandatory_filename(extra_filename)) {
            warn(__func__, "invalid: manifest extra[i = %jd] is a mandatory filename", i);
            json_dbg(JSON_DBG_HIGH, __func__,
                     "invalid: manifest extra[%jd] filename: <%s> matches a mandatory filename",
                     i, extra_filename);
            free_walk_stat(&wstat);
            return false;
        }
        /*
         * if this is an executable filename it is an error
         */
        if (is_executable_filename(extra_filename)) {
            warn(__func__, "invalid: manifest extra[i = %jd] is an executable filename", i);
            json_dbg(JSON_DBG_HIGH, __func__,
                     "invalid: manifest extra[%jd] filename: <%s> matches an executable filename",
                     i, extra_filename);
            free_walk_stat(&wstat);
            return false;
        }

	/*
	 * compare first extra filename with the remaining extra filenames
	 */
	for (j=0; j < i; ++j) {

	    /* obtain second extra filename */
	    extra_filename2 = dyn_array_value(manp->extra, char *, j);
	    if (extra_filename2 == NULL) {
		err(4, __func__, "invalid: manifest extra[j = %jd] is NULL", j);/*ooo*/
                not_reached();
	    }
	    /*
	     * compare first and second extra filenames
	     */
	    if (strcasecmp(extra_filename, extra_filename2) == 0) {
		warn(__func__, "invalid: manifest extra[%jd] filename matches manifest extra[%jd] filename", i, j);
		json_dbg(JSON_DBG_HIGH, __func__,
			 "invalid: manifest extra[%jd] filename: <%s> matches manifest extra[%jd] filename: <%s>",
			 i, extra_filename, j, extra_filename2);
		free_walk_stat(&wstat);
		return false;
	    }
	}

	/*
	 * compare first extra filename with all the shell script filenames
	 */
	for (j=0; j < count_shell_script; ++j) {

	    /* obtain shell filename */
	    shell_filename = dyn_array_value(manp->shell, char *, j);
	    if (shell_filename == NULL) {
		err(4, __func__, "invalid: manifest shell[j = %jd] is NULL", j);/*ooo*/
                not_reached();
	    }
	    /*
	     * compare extra filename with shell_script filename
	     */
	    if (strcasecmp(extra_filename, shell_filename) == 0) {
		warn(__func__, "invalid: manifest extra[%jd] filename matches manifest shell[%jd] filename", i, j);
		json_dbg(JSON_DBG_HIGH, __func__,
			 "invalid: manifest extra[%jd] filename: <%s> matches manifest shell[%jd] filename: <%s>",
			 i, extra_filename, j, shell_filename);
		free_walk_stat(&wstat);
		return false;
	    }
	}
    }

    /*
     * check for duplicates among valid shell filenames. Also check against all
     * extra file filenames and check if is a mandatory filename.
     */
    for (i=1; i < count_shell_script; ++i) {

	/* obtain first extra filename to compare against */
	shell_filename = dyn_array_value(manp->shell, char *, i);
	if (shell_filename == NULL) {
	    err(4, __func__, "invalid: manifest shell[i = %jd] is NULL", i);/*ooo*/
            not_reached();
	}

        /*
         * if this is a mandatory filename it is an error
         */
        if (is_mandatory_filename(shell_filename)) {
            warn(__func__, "invalid: manifest shell[i = %jd] is a mandatory filename", i);
            json_dbg(JSON_DBG_HIGH, __func__,
                     "invalid: manifest shell[%jd] filename: <%s> matches a mandatory filename",
                     i, shell_filename);
            free_walk_stat(&wstat);
            return false;
        }
        /*
         * if this is NOT an executable filename it is an error. Yes this check
         * is redundant. We do this in case someone is doing any funny business.
         */
        if (!is_executable_filename(shell_filename)) {
            warn(__func__, "invalid: manifest shell[i = %jd] is NOT an executable filename", i);
            json_dbg(JSON_DBG_HIGH, __func__,
                     "invalid: manifest shell[%jd] filename: <%s> is NOT an executable filename",
                     i, shell_filename);
            free_walk_stat(&wstat);
            return false;
        }

	/*
	 * compare first shell_script filename with the remaining shell_script filenames
	 */
	for (j=0; j < i; ++j) {

	    /* obtain second shell_script filename */
	    shell_filename2 = dyn_array_value(manp->shell, char *, j);
	    if (shell_filename2 == NULL) {
		err(4, __func__, "invalid: manifest shell[j = %jd] is NULL", j);/*ooo*/
		not_reached();
		return false;
	    }
	    /*
	     * compare first and second shell_script filenames
	     */
	    if (strcasecmp(shell_filename, shell_filename2) == 0) {
		warn(__func__, "invalid: manifest shell[%jd] filename matches manifest shell[%jd] filename", i, j);
		json_dbg(JSON_DBG_HIGH, __func__,
			 "invalid: manifest shell[%jd] filename: <%s> matches manifest shell[%jd] filename: <%s>",
			 i, shell_filename, j, shell_filename2);
		free_walk_stat(&wstat);
		return false;
	    }
	}
	/*
	 * compare shell script filename with all the extra script filenames
	 */
	for (j=0; j < count_extra_file; ++j) {

	    /* obtain extra_file filename */
	    extra_filename = dyn_array_value(manp->extra, char *, j);
	    if (extra_filename == NULL) {
		err(4, __func__, "invalid: manifest extra[j = %jd] is NULL", j);/*ooo*/
                not_reached();
	    }
	    /*
	     * compare extra filename with shell_script filename
	     */
	    if (strcasecmp(extra_filename, shell_filename) == 0) {
		warn(__func__, "invalid: manifest shell[%jd] filename matches manifest extra[%jd] filename", i, j);
		json_dbg(JSON_DBG_HIGH, __func__,
			 "invalid: manifest extra[%jd] filename: <%s> matches manifest shell[%jd] filename: <%s>",
			 i, shell_filename, j, extra_filename);
		free_walk_stat(&wstat);
		return false;
	    }
	}

    }
    json_dbg(JSON_DBG_MED, __func__, "manifest is complete with valid unique extra filenames and shell script filenames");

    free_walk_stat(&wstat);
    return true;
}


/*
 * test_min_timestamp - test if min_timestamp is valid
 *
 * Determine if min_timestamp == MIN_TIMESTAMP.
 *
 * given:
 *	tstamp		timestamp as time_t to test
 *
 * returns:
 *	true ==> min_timestamp is valid,
 *	false ==> min_timestamp is NOT valid, or some internal error
 */
bool
test_min_timestamp(time_t tstamp)
{
    /*
     * compare with the minimum timestamp
     */
    if (tstamp != MIN_TIMESTAMP) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: min_timestamp != MIN_TIMESTAMP");
	if ((time_t)-1 > 0) {
	    /* case: unsigned time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: min_timestamp %ju != MIN_TIMESTAMP: %ju",
		     (uintmax_t)tstamp, (uintmax_t)MIN_TIMESTAMP);
	} else {
	    /* case: signed time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: min_timestamp: %jd != MIN_TIMESTAMP: %jd",
		     (intmax_t)tstamp, (intmax_t)MIN_TIMESTAMP);
	}
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__,
	     "valid: min_timestamp == MIN_TIMESTAMP");
    return true;
}


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
test_mkiocccentry_version(char const *str)
{
    size_t i = 0;
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
    if (test_poison(str, poison_mkiocccentry_versions)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: mkiocccentry_version: is poisonous");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: mkiocccentry_version: %s is poisonous: %s", str, poison_mkiocccentry_versions[i]);
        return false;
    }
    if (!test_version(str, MIN_MKIOCCCENTRY_VERSION)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mkiocccentry_version < MIN_MKIOCCCENTRY_VERSION: %s", MIN_MKIOCCCENTRY_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mkiocccentry_version: %s is not >= MIN_MKIOCCCENTRY_VERSION: %s", str, MIN_MKIOCCCENTRY_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "mkiocccentry_version is valid");
    return true;
}


/*
 * test_name - test that name is valid
 *
 * Test that length is > 0 && <= MAX_NAME_LEN.
 *
 * NOTE: This function does NOT test if the author name is unique to the authors
 * of the submission.
 *
 * given:
 *	str	name to test
 *
 * returns:
 *	true ==> string is valid
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 *
 */
bool
test_name(char const *str)
{
    size_t length;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    length = strlen(str);
    if (length <= 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty name");
	return false;
    } else if (length > MAX_NAME_LEN) {
	json_dbg(JSON_DBG_MED, __func__, "invalid: name length > max length: %d", MAX_NAME_LEN);
	json_dbg(JSON_DBG_HIGH, __func__, "invalid: name <%s> length %zu > max length: %d", str, length, MAX_NAME_LEN);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "name is valid");
    return true;
}


/*
 * test_no_comment - test if no_comment is valid
 *
 * Determine if no_comment string is equal to JSON_PARSING_DIRECTIVE_VALUE.
 *
 * given:
 *	str		no_comment to test
 *
 * returns:
 *	true ==> min_timestamp is valid,
 *	false ==> min_timestamp is NOT valid, or some internal error
 */
bool
test_no_comment(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }
    else if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: no_comment is empty");
	return false;
    }
    /*
     * compare with the timestamp epoch
     */
    if (strcmp(str, JSON_PARSING_DIRECTIVE_VALUE)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: no_comment != JSON_PARSING_DIRECTIVE_VALUE");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: no_comment: <%s> != JSON_PARSING_DIRECTIVE_VALUE: <%s>",
		 str, JSON_PARSING_DIRECTIVE_VALUE);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__,
	     "valid: no_comment == JSON_PARSING_DIRECTIVE_VALUE");
    return true;

}


/*
 * test_past_winning_author - test if past_winning_author is valid
 *
 * Determine if past_winning_author boolean is valid.  :-)
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
test_past_winning_author(bool boolean)
{
    json_dbg(JSON_DBG_MED, __func__, "past_winning_author is %s", booltostr(boolean));
    return true;
}


/*
 * test_remarks - test that remarks filename is valid
 *
 * Test that length is > 0 and that the string is equal to REMARKS_FILENAME.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_remarks(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty remarks filename is invalid");
	return false;
    } else if (strcmp(str, REMARKS_FILENAME) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: remarks filename is not %s", REMARKS_FILENAME);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: remarks filename: <%s> is not %s", str, REMARKS_FILENAME);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "remarks filename is valid");
    return true;
}


/*
 * test_c_alt_src - test that prog.alt.c filename is valid
 *
 * Test that length is > 0 and that the string is equal to PROG_ALT_C.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 *
 */
bool
test_c_alt_src(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty c_alt_src filename is invalid");
	return false;
    } else if (strcmp(str, PROG_ALT_C)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: c_alt_src filename is not %s", PROG_ALT_C);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: c_alt_src filename: <%s> is not %s", str, PROG_ALT_C);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "c_alt_src filename is valid");
    return true;
}


/*
 * test_try_sh - test that try.sh filename is valid
 *
 * Test that length is > 0 and that the string is equal to TRY_SH.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_try_sh(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty try_sh filename is invalid");
	return false;
    } else if (strcmp(str, TRY_SH) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: try_sh filename is not %s", TRY_SH);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: try_sh filename: <%s> is not %s", str, TRY_SH);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "try_sh filename is valid");
    return true;
}


/*
 * test_try_alt_sh - test that try.sh filename is valid
 *
 * Test that length is > 0 and that the string is equal to try_alt_sh.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_try_alt_sh(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty try_alt_sh filename is invalid");
	return false;
    } else if (strcmp(str, TRY_ALT_SH) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: try_alt_sh filename is not %s", TRY_ALT_SH);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: try_alt_sh filename: <%s> is not %s", str, TRY_ALT_SH);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "try_alt_sh filename is valid");
    return true;
}


/*
 * test_shell_script - test that shell script filename is valid
 *
 * Test that length is > 0 and that the string suffix is ".sh".
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_shell_script(char const *str)
{
    size_t len = 0;	    /* length of str */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }
    len = strlen(str);

    if (len == 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty shell_script filename is invalid");
	return false;
    } else if (len < LITLEN(".sh") || strcmp(str + len - LITLEN(".sh"), ".sh") != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: shell_script filename does not end in .sh");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: shell_script filename: <%s> does not end in .sh", str);
	return false;
    } else if (!strcmp(str, TRY_SH)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: shell_script filename is try.sh");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: shell_script filename: <%s> is try.sh", str);
	return false;
    } else if (!strcmp(str, TRY_ALT_SH)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: shell_script filename is try.alt.sh");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: shell_script filename: <%s> is try.alt.sh", str);
	return false;
    } else if (is_mandatory_filename(str)) {
        /*
         * this check is redundant but we do it anyway in case someone is trying
         * some kind of funny business
         */
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: shell_script filename matches a mandatory file");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: shell_script filename matches a mandatory file: <%s>", str);
        return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "shell_script filename is valid");
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
 * test_rule_2a_size - test if rule_2a_size is valid
 *
 * Determine if rule_2a_size is within the proper limits.
 *
 * NOTE: Although this function does not return false if the size <= 0 or
 *	 if size > RULE_2A_SIZE we don't check that in this case the rule
 *	 2a override option is enabled.
 *
 * because:
 *
 *	Failing the test_rule_2a_size() test does NOT disqualify the submission. It
 *	just triggers, in the case of mkiocccentry, to ask the user if they
 *	really want to try and break the rule.
 *
 * given:
 *	rule_2a_size	rule 2a size
 *
 * returns:
 *	true ==> rule_2a_size is valid,
 *	false ==> rule_2a_size is NOT valid, or some internal error
 */
bool
test_rule_2a_size(size_t rule_2a_size)
{
    /* test lower bound */
    if (rule_2a_size <= 0) {
	json_dbg(JSON_DBG_MED, __func__, "warning: rule_2a_size <= 0");
	json_dbg(JSON_DBG_HIGH, __func__, "warning: rule_2a_size: %zu <= 0", rule_2a_size);
    /* test upper bound */
    } else if (rule_2a_size > RULE_2A_SIZE) {
	json_dbg(JSON_DBG_MED, __func__, "warning: rule_2a_size > %d", RULE_2A_SIZE);
	json_dbg(JSON_DBG_HIGH, __func__, "warning: rule_2a_size: %zu > %d", rule_2a_size, RULE_2A_SIZE);
    } else {
	/* rule_2a_size is valid */
	json_dbg(JSON_DBG_MED, __func__,
		"valid: rule_2a_size");
    }
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
 * test_rule_2b_size - test if rule_2b_size is valid
 *
 * Determine if rule_2b_size is within the proper limits.
 *
 * NOTE: Although this function does not return false if the size <= 0 or
 *	 if size > RULE_2B_SIZE we don't check that in this case the rule
 *	 2b override option is enabled.
 *
 * because:
 *
 *	Failing the test_rule_2b_size() test does NOT disqualify the submission. It
 *	just triggers, in the case of mkiocccentry, to ask the user if they
 *	really want to try and break the rule.
 *
 * given:
 *	rule_2b_size	rule 2b size
 *
 * returns:
 *	true ==> rule_2b_size is valid,
 *	false ==> rule_2b_size is NOT valid, or some internal error
 */
bool
test_rule_2b_size(size_t rule_2b_size)
{
    /* test lower bound */
    if (rule_2b_size <= 0) {
	json_dbg(JSON_DBG_MED, __func__, "warning: rule_2b_size <= 0");
	json_dbg(JSON_DBG_HIGH, __func__, "warning: rule_2b_size: %zu <= 0", rule_2b_size);
    /* test upper bound */
    } else if (rule_2b_size > RULE_2B_SIZE) {
	json_dbg(JSON_DBG_MED, __func__, "warning: rule_2b_size > %d", RULE_2B_SIZE);
	json_dbg(JSON_DBG_HIGH, __func__, "warning: rule_2b_size: %zu > %d", rule_2b_size, RULE_2B_SIZE);
    } else {
	/* rule_2b_size is valid */
	json_dbg(JSON_DBG_MED, __func__, "valid: rule_2b_size");
    }
    return true;
}


/*
 * test_tarball - test if tarball is valid
 *
 * Determine if tarball string is equal to tarball.
 *
 * given:
 *	str			tarball to test
 *	IOCCC_contest_id	IOCCC contest UUID or test
 *	submit_slot		IOCCC entry number
 *	test_mode		true ==> IOCCC entry is just a test
 *	formed_timestamp	timestamp of when entry was formed
 *
 * returns:
 *	true ==> tarball is valid,
 *	false ==> tarball is NOT valid, or some internal error
 */
bool
test_tarball(char const *str, char const *IOCCC_contest_id, int submit_slot, bool test_mode,
	     time_t formed_timestamp)
{
    char *tar_filename = NULL;	/* formed tarball filename */
    bool test = false;		/* test result */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    } else if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: tarball is empty");
	return false;
    }
    if (IOCCC_contest_id == NULL) {
	warn(__func__, "IOCCC_contest_id is NULL");
	return false;
    } else if (*IOCCC_contest_id == '\0') { /* strlen(IOCCC_contest_id) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: IOCCC_contest_id is empty");
	return false;
    }
    test = test_submit_slot(submit_slot);
    if (test == false) {
	/* test_submit_slot() already issued json_dbg() messages */
	return false;
    }
    test = test_formed_timestamp(formed_timestamp);
    if (test == false) {
	/* test_formed_timestamp() already issued json_dbg() messages */
	return false;
    }

    /*
     * case: test mode specific test
     */
    if (test_mode == true) {

	/*
	 * test mode IOCCC_contest_id must be test
	 */
	if (strcmp(IOCCC_contest_id, "test") != 0) {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: test mode and IOCCC_contest_id != test");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: test mode and IOCCC_contest_id: <%s> != test", IOCCC_contest_id);
	    return false;
	}

    /*
     * case: non-test (official entry) mode specific test
     */
    } else {

	/*
	 * non-test mode IOCCC_contest_id check
	 */
	test = test_IOCCC_contest_id(IOCCC_contest_id);
	if (test == false) {
	    /* test_IOCCC_contest_id() already issued json_dbg() messages */
	    return false;
	}

    }

    /*
     * form a malloced valid tarball filename
     */
    tar_filename = form_tar_filename(IOCCC_contest_id, submit_slot, test_mode, formed_timestamp);
    if (tar_filename == NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: form_tar_filename failed and returned NULL");
	return false;
    }

    /*
     * compare str tarball filename with formed tarball filename
     */
    if (strcmp(str, tar_filename) != 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: invalid tar_filename");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: tarball: <%s> != expected tarball filename: <%s>",
		 str, tar_filename);
	if (tar_filename != NULL) {
	    free(tar_filename);
	    tar_filename = NULL;
	}
	return false;
    }

    /* tarball filename is valid */
    if (tar_filename != NULL) {
	free(tar_filename);
	tar_filename = NULL;
    }
    json_dbg(JSON_DBG_MED, __func__,
	     "valid: tarball filename");
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
 * test_timestamp_epoch - test if timestamp_epoch is valid
 *
 * Determine if timestamp_epoch string is equal to TIMESTAMP_EPOCH.
 *
 * given:
 *	str		timestamp_epoch to test
 *
 * returns:
 *	true ==> min_timestamp is valid,
 *	false ==> min_timestamp is NOT valid, or some internal error
 */
bool
test_timestamp_epoch(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    } else if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: timestamp_epoch is empty");
	return false;
    }
    /*
     * compare with the timestamp epoch
     */
    if (strcmp(str, TIMESTAMP_EPOCH)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: timestamp_epoch != TIMESTAMP_EPOCH: <%s>",
		 TIMESTAMP_EPOCH);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: timestamp_epoch: <%s> != TIMESTAMP_EPOCH: <%s>",
		 str, TIMESTAMP_EPOCH);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__,
	     "valid: timestamp_epoch == TIMESTAMP_EPOCH");
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
test_title(char const *str)
{
    size_t length = 0;
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
    /* check for a valid length */
    if (*str == '\0') { /* strlen(str) == 0 */
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: empty title");
	return false;
    }
    length = strlen(str);
    if (length > MAX_TITLE_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: title length > max %d", MAX_TITLE_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title length %zu > max %d: <%s>", length, MAX_TITLE_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title: <%s> is invalid", str);
	return false;
    }
    /* check for valid title chars */
    test = safe_path_str(str, false, false); /* ^[0-9a-z_.][0-9a-z._+-]*$ */
    if (test == false) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: title does not match regexp ^[0-9a-z][0-9a-z._+-]*$");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title does not match regexp ^[0-9a-z][0-9a-z._+-]*$: '%s'", str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: title: <%s> is invalid", str);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "title is valid");
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
 * test_mastodon - test if mastodon account is valid
 *
 * Determine if mastodon length is <= MAX_MASTODON_LEN and that it has a leading '@'
 * and one other '@' which is not right next to the first one.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 *
 * NOTE: we don't check for domains in a way that would validate it as a
 * properly named domain name. Just like email this would get complicated. The
 * proper regex for email is ridiculously complicated. See
 * https://www.regular-expressions.info/email.html and scroll down to 'The
 * Official Standard: RFC 5322' for details.
 */
bool
test_mastodon(char const *str)
{
    size_t length = 0;
    char *p = NULL;
    char *last_at = NULL;

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
		 "empty mastodon is invalid");
	return false;
    }
    length = strlen(str);
    if (length > MAX_MASTODON_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mastodon length %zu > max %d: <%s>", length, MAX_MASTODON_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mastodon: <%s> is invalid", str);
	return false;
    }
    /* check for valid mastodon account chars */
    p = strchr(str, '@');
    last_at = strrchr(str, '@');
    if (p == NULL || str[0] != '@') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mastodon account does not start with '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mastodon: <%s> is invalid", str);
	return false;
    } else if (str[1] == '\0') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mastodon account is empty beyond the first '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mastodon: <%s> is invalid", str);
	return false;
    } else if (last_at == NULL || last_at == p) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mastodon account has only one '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mastodon: <%s> is invalid", str);
	return false;
    } else if (strstr(str, "@@") != NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mastodon account has adjacent '@'s");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mastodon: <%s> is invalid", str);
	return false;
    } else if (count_char(str, '@') != 2) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mastodon account does not have exactly two '@'s");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mastodon: <%s> is invalid", str);
	return false;
    } else if (str[length - 1] == '@') {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: mastodon account ends with a '@'");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: mastodon: <%s> is invalid", str);
	return false;

    }

    json_dbg(JSON_DBG_MED, __func__, "mastodon is valid");
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
test_txzchk_version(char const *str)
{
    size_t i = 0;
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
    if (test_poison(str, poison_txzchk_versions)) {
        json_dbg(JSON_DBG_MED, __func__,
                 "invalid: txzchk_version: is poisonous");
        json_dbg(JSON_DBG_HIGH, __func__,
                 "invalid: txzchk_version: %s is poisonous: %s", str, poison_txzchk_versions[i]);
        return false;
    }
    if (!test_version(str, MIN_TXZCHK_VERSION)) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: txzchk_version < MIN_TXZCHK_VERSION: %s", MIN_TXZCHK_VERSION);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: txzchk_version: %s is not >= MIN_TXZCHK_VERSION: %s", str, MIN_TXZCHK_VERSION);
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "txzchk_version is valid");
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
 * test_alt_url - test if URL is valid
 *
 * Determine if alt_url length is <= MAX_URL_LEN, and that it starts with either
 * https:// or http:// followed by more characters and starts with neither
 * https:/// nor http:///.
 *
 * While this URL test is naive, it does a good enough job for our purposes.
 * We do not want to write a generalized URI/URL checker, nor do we need to
 * do so.  All we want is for the URL to begin with something that looks sane.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_alt_url(char const *str)
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
		 "empty alt_url is invalid");
	return false;
    }
    length = strlen(str);
    if (length > MAX_URL_LEN) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: alt_url length > max %d: <%s>", MAX_URL_LEN, str);
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: alt_url length %zu > max %d: <%s>", length, MAX_URL_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: alt_url: <%s> is invalid", str);
	return false;
    }

    /*
     * case: https://
     */
    if (strncmp(str, "https://", LITLEN("https://")) == 0) {

	/* detect lack of a hostname in the URL */
	if (str[LITLEN("https://")] == '\0') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: alt_url does not have more characters after https://");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: alt_url: <%s> is invalid", str);
	    return false;

	/* do not allow https:/// */
	} else if (str[LITLEN("https://")] == '/') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: alt_url cannot start with https:///");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: alt_url: <%s> is invalid", str);
	    return false;
	}

    /*
     * case: http://
     */
    } else if (strncmp(str, "http://", LITLEN("http://")) == 0) {

	/* detect lack of a hostname in the URL */
	if (str[LITLEN("http://")] == '\0') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: alt_url does not have more characters after http://");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: alt_url: <%s> is invalid", str);
	    return false;

	/* do not allow http:/// */
	} else if (str[LITLEN("http://")] == '/') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: alt_url cannot start with http:///");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: alt_url: <%s> is invalid", str);
	    return false;
	}

    /*
     * case: invalid URL start
     */
    } else {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: alt_url starts with neither https:// nor http://");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: alt_url: <%s> is invalid", str);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "alt_url is valid");
    return true;
}


/*
 * test_url - test if URL is valid
 *
 * Determine if url length is <= MAX_URL_LEN, and that it starts with either
 * https:// or http:// followed by more characters and starts with neither
 * https:/// nor http:///.
 *
 * While this URL test is naive, it does a good enough job for our purposes.
 * We do not want to write a generalized URI/URL checker, nor do we need to
 * do so.  All we want is for the URL to begin with something that looks sane.
 *
 * given:
 *	str	string to test
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
 */
bool
test_url(char const *str)
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
		 "invalid: url length > max %d", MAX_URL_LEN);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: url length %zu > max %d: <%s>", length, MAX_URL_LEN, str);
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: url: <%s> is invalid", str);
	return false;
    }

    /*
     * case: https://
     */
    if (strncmp(str, "https://", LITLEN("https://")) == 0) {

	/* detect lack of a hostname in the URL */
	if (str[LITLEN("https://")] == '\0') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: url does not have more characters after https://");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: url: <%s> is invalid", str);
	    return false;

	/* do not allow https:/// */
	} else if (str[LITLEN("https://")] == '/') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: url cannot start with https:///");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: url: <%s> is invalid", str);
	    return false;
	}

    /*
     * case: http://
     */
    } else if (strncmp(str, "http://", LITLEN("http://")) == 0) {

	/* detect lack of a hostname in the URL */
	if (str[LITLEN("http://")] == '\0') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: url does not have more characters after http://");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: url: <%s> is invalid", str);
	    return false;

	/* do not allow http:/// */
	} else if (str[LITLEN("http://")] == '/') {
	    json_dbg(JSON_DBG_MED, __func__,
		     "invalid: url cannot start with http:///");
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: url: <%s> is invalid", str);
	    return false;
	}

    /*
     * case: invalid URL start
     */
    } else {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: url starts with neither https:// nor http://");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: url: <%s> is invalid", str);
	return false;
    }

    json_dbg(JSON_DBG_MED, __func__, "url is valid");
    return true;
}


/* XXX - fix this function or remove it - XXX */
/*
 * is_mandatory_filename  - check if str is a mandatory filename
 *
 *  given:
 *          str      - name to check
 *
 * NOTE: if str is NULL we return false.
 */
bool
is_mandatory_filename(char const *str)
{
    size_t i = 0;

    /*
     * firewall
     */
    if (str == NULL) {
        warn(__func__, "str is NULL");
        return false;
    }

    for (i = 0; mandatory_filenames[i] != NULL; ++i) {
        if (!strcasecmp(mandatory_filenames[i], str)) {
            return true;
        }
    }

    return false;
}


/*
 * is_executable_filename  - check if str is a filename that should be 0555
 *
 *  given:
 *          str      - name to check
 *
 * NOTE: if str is NULL we return false.
 *
 * If there is a match the file MUST be mode 0555.
 *
 * NOTE: an executable filename is one that matches a name in the
 * executable_filenames array OR one that matches the glob:
 *
 *      *.sh
 *
 * If it is a directory this does not apply. Yes the try.sh and try.alt.sh
 * scripts in the executable_filenames array are redundant. The array, however,
 * exists so that if other files have to be executable in the future, it can be
 * updated to account for this. Also, for those in the array, it is more
 * efficient to check the array first (and those files MUST be executable) than
 * to use fnmatch(3). Note that txzchk(1) also uses this function.
 *
 * NOTE: we use FNM_PERIOD to match the shell: the glob *sh would not match
 * .foo.sh but it would match foo.sh.
 */
bool
is_executable_filename(char const *str)
{
    size_t i = 0;

    /*
     * firewall
     */
    if (str == NULL) {
        warn(__func__, "str is NULL");
        return false;
    }

    /*
     * first check the executable_filenames array
     */
    for (i = 0; executable_filenames[i] != NULL; ++i) {
        if (!strcasecmp(executable_filenames[i], str)) {
            return true;
        }
    }

    /*
     * if this ends in .sh then it is an executable filename.
     *
     * Yes this will ignore files like .foo.sh but that is a forbidden
     * filename.
     */
    if (strlen(str) >= LITLEN(".sh") &&
        !strcmp(str + strlen(str) - LITLEN(".sh"), ".sh")) {
        return true;
    }
    return false;
}


/*
 * count_char - count the number of instances of a char in the string
 *
 * given:
 *
 *	str	string to search
 *	ch	character to find
 *
 * NOTE: this function does not return on NULL pointer.
 */
size_t
count_char(char const *str, int ch)
{
    size_t count = 0;	    /* number of ch in the string */
    size_t i = 0;	    /* what character in str we're at */

    /*
     * firewall
     */
    if (str == NULL) {
	err(170, __func__, "given NULL str");
	not_reached();
    }

    for (count = 0, i = 0; str[i] != '\0'; ++i) {
	if (str[i] == ch) {
	    ++count;
	}
    }
    return count;
}
