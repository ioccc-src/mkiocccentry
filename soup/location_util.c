/*
 * location_util - location/country codes utility functions
 *
 * "Because there is an I in IOCCC." :-)
 *
 * Copyright (c) 2022,2023 by Landon Curt Noll.  All Rights Reserved.
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
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <strings.h> /* strcasecmp */

/*
 * location - location/country codes
 */
#include "location.h"


/*
 * check_location_table	    - make sure that there are no embedded NULL elements
 *			      in the location table and that the last element is
 *			      NULL
 *
 * This function verifies that the only NULL element in the table is the very
 * last element: if it's not there or there's another NULL element it's a
 * problem that has to be fixed.
 *
 * This function does not return on error.
 */
void
check_location_table(void)
{
    size_t max = SIZEOF_LOCATION_TABLE;

    size_t i;

    for (i = 0; i < max - 1 && loc[i].code != NULL; )
	++i;

    if (max - 1 != i) {
	err(333, __func__, "found embedded NULL element in location table; fix table in %s and recompile", __FILE__);
	not_reached();
    }
    if (loc[i].code != NULL || loc[i].name != NULL || loc[i].common_name) {
	err(10, __func__, "no final NULL element found in location table; fix table in %s and recompile", __FILE__);
	not_reached();
    }
}


/*
 * lookup_location_name - convert a ISO 3166-1 Alpha-2 into a location name
 *
 * Even though the ISO 3166-1 Alpha-2 code is only UPPER CASE, the
 * code is compared in a case independent way.  That is, a non-canonical
 * lower case code will match the canonical UPPER CASE ISO 3166-1 Alpha-2 code.
 *
 * given:
 *      code		ISO 3166-1 Alpha-2 code
 *	use_common	true ==> use common name, false ==> use official name
 *
 * return:
 *      location name or NULL ==> unlisted code
 *
 * This function does not return on error.
 */
char const *
lookup_location_name(char const *code, bool use_common)
{
    struct location *p;		/* entry in the location table */
    size_t length = 0;		/* length of code */

    /*
     * firewall
     */
    if (code == NULL) {
	err(11, __func__, "code is NULL");
	not_reached();
    }

    /*
     * sanity check - code must be 2 characters long
     */
    length = strlen(code);
    if (length != 2) {
	dbg(DBG_HIGH, "code: <%s> length: %zu != 2", code, length);
	return NULL;
    }

    /*
     * sanity check - code must be 2 ASCII alpha characters
     */
    if (!isascii(code[0]) || !isalpha(code[0]) ||
	!isascii(code[1]) || !isalpha(code[1])) {
	dbg(DBG_HIGH, "code: <%s> is not 2 ASCII alpha characters", code);
	return NULL;
    }

    /*
     * search location table for the code
     */
    for (p = &loc[0]; p->code != NULL && p->name != NULL && p->common_name != NULL; ++p) {
	if (strcasecmp(code, p->code) == 0) {
	    dbg(DBG_VHIGH, "code: %s found name: <%s> common_name: <%s>", p->code, p->name, p->common_name);
	    break;
	}
    }

    /*
     * return name or NULL
     */
    if (p->name == NULL || p->common_name) {
	dbg(DBG_HIGH, "code: <%s> is unknown", code);
    }
    if (use_common) {
	return p->common_name;
    } else {
	return p->name;
    }
}


/*
 * lookup_location_code - convert a ISO 3166-1 Alpha-2 into a location name
 *
 * Even though the location table contains a canonical name in a canonical case,
 * we compare names in a case independent way.  Nevertheless, the ISO 3166-1 Alpha-2
 * code returned will be a 2 character ASCII UPPER CASE code.
 *
 * given:
 *      location_name      location name of a ISO 3166-1 Alpha-2 entry
 *	use_common	   true ==> use common name, false ==> use official name
 *
 * return:
 *	ISO 3166-1 Alpha-2 in UPPER CASE code or NULL ==> unknown location name
 *
 * This function does not return on error.
 */
char const *
lookup_location_code(char const *location_name, bool use_common)
{
    struct location *p;		/* entry in the location table */

    /*
     * firewall
     */
    if (location_name == NULL) {
	err(12, __func__, "location_name is NULL");
	not_reached();
    }

    /*
     * search location table for the code
     */
    if (use_common) {
	for (p = &loc[0]; p->common_name != NULL && p->code != NULL; ++p) {
	    if (strcasecmp(location_name, p->common_name) == 0) {
		dbg(DBG_VHIGH, "common_name: <%s> found code: %s", p->common_name, p->code);
		break;
	    }
	}
    } else {
	for (p = &loc[0]; p->name != NULL && p->code != NULL; ++p) {
	    if (strcasecmp(location_name, p->name) == 0) {
		dbg(DBG_VHIGH, "name: <%s> found code: %s", p->name, p->code);
		break;
	    }
	}
    }

    /*
     * return code or NULL
     */
    if (p->code == NULL) {
	dbg(DBG_HIGH, "name: <%s> is unknown", location_name);
    }
    return p->code;
}

/*
 * lookup_location_name_r - convert a ISO 3166-1 Alpha-2 into a location name, re-entrant version
 *
 * Even though the ISO 3166-1 Alpha-2 code is only UPPER CASE, the
 * code is compared in a case independent way.  That is, a non-canonical
 * lower case code will match the canonical UPPER CASE ISO 3166-1 Alpha-2 code.
 *
 * given:
 *      code		    ISO 3166-1 Alpha-2 code
 *      idx		    index to resume searching for -a option
 *      location	    pointer to struct location * for caller
 *      substrings	    whether to search by substring (strcasestr(), not strcasecmp())
 *	use_common	    true ==> use common name, false ==> use official name
 *
 * return:
 *      location name or NULL ==> unlisted code
 *
 * This function does not return on error.
 *
 * If idx != NULL and we *idx < SIZEOF_LOCATION_TABLE then we start searching at
 * element *idx of the location table. If found and idx != NULL we save the
 * position + 1 in *idx for the next call.
 */
char const *
lookup_location_name_r(char const *code, size_t *idx, struct location **location, bool substrings, bool use_common)
{
    struct location *p;		/* entry in the location table */
    size_t length = 0;		/* length of code */
    size_t i = 0;		/* to iterate through table */

    /*
     * firewall
     */
    if (code == NULL) {
	err(13, __func__, "code is NULL");
	not_reached();
    }

    /*
     * sanity check - code must be 2 characters long
     */
    length = strlen(code);
    if (length != 2) {
	dbg(DBG_HIGH, "code: <%s> length: %zu != 2", code, length);
	return NULL;
    }

    if (idx != NULL && *idx >= SIZEOF_LOCATION_TABLE) {
	return NULL;
    }

    /*
     * sanity check - code must be 2 ASCII alpha characters
     */
    if (!isascii(code[0]) || !isalpha(code[0]) ||
	!isascii(code[1]) || !isalpha(code[1])) {
	dbg(DBG_HIGH, "code: <%s> is not 2 ASCII alpha characters", code);
	return NULL;
    }

    /*
     * search location table for the code
     */
    if (use_common) {
	for (i = idx != NULL ? *idx : 0, p = &loc[i]; p->code != NULL && p->common_name != NULL; ++p, ++i) {
	    if ((strcasecmp(code, p->code) == 0) || (substrings && strcasestr(code, p->code))) {
		dbg(DBG_VHIGH, "code: %s found common_name: <%s>", p->code, p->common_name);
		if (location != NULL) {
		    *location = &loc[i];
		}
		break;
	    }
	}
    } else {
	for (i = idx != NULL ? *idx : 0, p = &loc[i]; p->code != NULL && p->name != NULL; ++p, ++i) {
	    if ((strcasecmp(code, p->code) == 0) || (substrings && strcasestr(code, p->code))) {
		dbg(DBG_VHIGH, "code: %s found name: <%s>", p->code, p->name);
		if (location != NULL) {
		    *location = &loc[i];
		}
		break;
	    }
	}
    }

    /*
     * return name or NULL
     */
    if (use_common) {
	if (p->common_name == NULL) {
	    dbg(DBG_HIGH, "code: <%s> is unknown", code);
	}
    } else {
	if (p->name == NULL) {
	    dbg(DBG_HIGH, "code: <%s> is unknown", code);
	}
    }

    /* save index after everything even if we return NULL */
    if (idx != NULL) {
	*idx = i + 1;
    }

    if (use_common) {
	return p->common_name;
    } else {
	return p->name;
    }
}
/*
 * lookup_location_code_r - convert a ISO 3166-1 Alpha-2 into a location name, re-entrant version
 *
 * Even though the location table contains a canonical name in a canonical case,
 * we compare names in a case independent way.  Nevertheless, the ISO 3166-1 Alpha-2
 * code returned will be a 2 character ASCII UPPER CASE code.
 *
 * given:
 *      location_name	    location name of a ISO 3166-1 Alpha-2 entry
 *      idx		    index to resume searching for -a option
 *      location	    pointer to struct location * for caller
 *      substrings	    whether to search by substring (strcasestr(), not strcasecmp())
 *	use_common	    true ==> use common name, false ==> use official name
 *
 * return:
 *	ISO 3166-1 Alpha-2 in UPPER CASE code or NULL ==> unknown location name
 *
 * This function does not return on error. It can return a NULL pointer.
 *
 * If idx != NULL and we *idx < SIZEOF_LOCATION_TABLE then we start searching at
 * element *idx of the location table. If found and idx != NULL we save the
 * position + 1 in *idx for the next call.
 */
char const *
lookup_location_code_r(char const *location_name, size_t *idx, struct location **location, bool substrings, bool use_common)
{
    struct location *p;		/* entry in the location table */
    size_t i = 0;		/* counter for table */

    /*
     * firewall
     */
    if (location_name == NULL) {
	err(14, __func__, "location_name is NULL");
	not_reached();
    }

    if (idx != NULL && *idx >= SIZEOF_LOCATION_TABLE) {
	return NULL;
    }

    /*
     * search location table for the code
     */
    if (use_common) {
	for (i = idx != NULL ? *idx : 0, p = &loc[i]; p->common_name != NULL && p->code != NULL; ++p, ++i) {
		if ((strcasecmp(location_name, p->common_name) == 0) || (substrings && strcasestr(p->common_name, location_name))) {
		    dbg(DBG_VHIGH, "common_name: <%s> found code: %s", p->common_name, p->code);
		    if (location != NULL) {
			*location = &loc[i];
		    }
		    break;
		}
	}
    } else {
	for (i = idx != NULL ? *idx : 0, p = &loc[i]; p->name != NULL && p->code != NULL; ++p, ++i) {
		if ((strcasecmp(location_name, p->name) == 0) || (substrings && strcasestr(p->name, location_name))) {
		    dbg(DBG_VHIGH, "name: <%s> found code: %s", p->name, p->code);
		    if (location != NULL) {
			*location = &loc[i];
		    }
		    break;
		}
	}
    }

    /*
     * return code or NULL
     */
    if (p->code == NULL) {
	dbg(DBG_HIGH, "name: <%s> is unknown", location_name);
    }

    /* save index after everything even if we return NULL */
    if (idx != NULL) {
	*idx = i + 1;
    }
    return p->code;
}



/*
 * location_code_name_match - if a location code & location name refer to the same place
 *
 * We look up a ISO 3166-1 Alpha-2 in UPPER CASE code and determine if the associated
 * location name matches the given location name.
 *
 * given:
 *	code	ISO 3166-1 Alpha-2 in UPPER CASE
 *	location_name	location name of a ISO 3166-1 Alpha-2 entry
 *	use_common	    true ==> use common name, false ==> use official name
 *
 * return:
 *	true ==> location code and location name match,
 *	false ==> location code and location name do not match or,
 *		  unknown location code, or NULL ptr
 */
bool
location_code_name_match(char const *code, char const *location_name, bool use_common)
{
    struct location *p;		/* entry in the location table */

    /*
     * firewall
     */
    if (code == NULL) {
	err(15, __func__, "code is NULL");
	not_reached();
    }
    if (location_name == NULL) {
	err(16, __func__, "location_name is NULL");
	not_reached();
    }

    /*
     * search location table for the code
     */
    if (use_common) {
	for (p = &loc[0]; p->code != NULL && p->common_name != NULL; ++p) {
	    if (strcasecmp(code, p->code) == 0) {
		dbg(DBG_VHIGH, "code: %s found common_name: <%s>", p->code, p->common_name);
		break;
	    }
	}
    } else {
	for (p = &loc[0]; p->code != NULL && p->name != NULL; ++p) {
	    if (strcasecmp(code, p->code) == 0) {
		dbg(DBG_VHIGH, "code: %s found name: <%s>", p->code, p->name);
		break;
	    }
	}
    }
    if (p->code == NULL) {
	dbg(DBG_HIGH, "code: <%s> is unknown", code);
	return false;
    }

    if (p->name == NULL || p->common_name) {
	dbg(DBG_HIGH, "name: <%s> is unknown", location_name);
	return false;
    }

    /*
     * compare location name with name from the code found in the table
     */
    if (use_common) {
	if (strcasecmp(location_name, p->common_name) != 0) {
	    dbg(DBG_HIGH, "code: %s does not match common_name: <%s>", code, location_name);
	    return false;
	}
    } else {
	if (strcasecmp(location_name, p->name) != 0) {
	    dbg(DBG_HIGH, "code: %s does not match name: <%s>", code, location_name);
	    return false;
	}
    }

    /*
     * return match
     */
    return true;
}
