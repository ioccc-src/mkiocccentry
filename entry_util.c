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
	err(190, __func__, "called with NULL arg(s)");
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
	err(191, __func__, "called with NULL arg(s)");
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
	err(192, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(193, __func__, "author_count: %d < 0", author_count);
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
 *	boolean	boolean to test
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
    if (author_number < 1) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: author_number: %d < 0", author_number);
	return false;
    } else if (author_number > MAX_AUTHORS) {
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
    json_dbg(JSON_DBG_MED, __func__, "mkiocccentry_version is valid");
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
 * test_empty_override - test if empty_override is valid
 *
 * Determine if empty_override boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean	boolean to test
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
 * test_rule_2a_override - test if rule_2a_override is valid
 *
 * Determine if rule_2a_override boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 * test_first_rule_is_all - test if first_rule_is_all is valid
 *
 * Determine if first_rule_is_all boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean	boolean to test
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
 * test_found_all_rule - test if found_all_rule is valid
 *
 * Determine if found_all_rule boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 *	boolean	boolean to test
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
 * test_test_mode - test if test_mode is valid
 *
 * Determine if test_mode boolean is valid.  :-)
 * Well this isn't much of a test, but we have to keep
 * up with the general form of tests!  :-)
 *
 * given:
 *	boolean	boolean to test
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
