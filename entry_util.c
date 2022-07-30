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
	err(162, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free version values
     */
    if (infop->common.mkiocccentry_ver != NULL) {
	free(infop->common.mkiocccentry_ver);
	infop->common.mkiocccentry_ver = NULL;
    }

    /*
     * free entry values
     */
    if (infop->common.ioccc_id != NULL) {
	free(infop->common.ioccc_id);
	infop->common.ioccc_id = NULL;
    }
    if (infop->title != NULL) {
	free(infop->title);
	infop->title = NULL;
    }
    if (infop->abstract != NULL) {
	free(infop->abstract);
	infop->abstract = NULL;
    }

    /*
     * free filenames
     */
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

    if (infop->common.tarball != NULL) {
	free(infop->common.tarball);
	infop->common.tarball = NULL;
    }

    /*
     * free time values
     */
    if (infop->common.epoch != NULL) {
	free(infop->common.epoch);
	infop->common.epoch = NULL;
    }
    if (infop->common.utctime != NULL) {
	free(infop->common.utctime);
	infop->common.utctime = NULL;
    }
    memset(infop, 0, sizeof *infop);

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
	err(163, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(164, __func__, "author_count: %d < 0", author_count);
	not_reached();
    }

    /*
     * free elements of each array member
     */
    for (i = 0; i < author_count; ++i) {
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
    }

    memset(author_set, 0, sizeof *author_set);
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
		 "invalid: empty abstract");
	return false;
    }
    length = strlen(str);
    if (length > MAX_ABSTRACT_LEN) {
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
 * Determine if author_count matches AUTHOR_JSON_FILENAME.
 *
 * given:
 *	author_count	number of authors
 *
 * returns:
 *	true ==> string is valid,
 *	false ==> string is NOT valid, or NULL pointer, or some internal error
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
