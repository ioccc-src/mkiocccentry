/*
 * verge - determine if first version is greater or equal to the second version
 *
 * "Because too much minimalism can be subminimal." :-)
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * This JSON parser and tool were co-developed in 2022-205 by Cody Boone
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
#include <unistd.h>
#include <string.h>
#include <ctype.h>

/*
 * verge - determine if first version is greater or equal to the second version
 */
#include "verge.h"



/*
 * definitions
 */
#define REQUIRED_ARGS (2)	/* number of required arguments on the command line */

/*
 * vercmp - compare two version strings
 *
 * given:
 *
 *  ver1    -   first version to compare
 *  ver2    -   second version to compare
 *
 * Returns:
 *      0   ver1 version >= ver2
 *      1   ver1 < ver2
 *      4   first or second version string is an invalid version
 *
 * This function does not return on errors or NULL pointers.
 */
int
vercmp(char *ver1, char *ver2)
{
    int ver1_levels = 0;	/* number of version levels for first version string */
    int ver2_levels = 0;	/* number of version levels for second version string */
    intmax_t *vlevel1 = NULL;	/* allocated version levels from first version string */
    intmax_t *vlevel2 = NULL;	/* allocated version levels from second version string */
    int i=0;

    if (ver1 == NULL || *ver1 == '\0') {
        err(55, __func__, "first version string is NULL or empty");
        not_reached();
    }
    if (ver2 == NULL || *ver2 == '\0') {
        err(56, __func__, "second version string is NULL or empty");
        not_reached();
    }
    dbg(DBG_LOW, "first version: <%s>", ver1);
    dbg(DBG_LOW, "second version: <%s>", ver2);

    /*
     * convert first version string
     */
    ver1_levels = (int)allocate_vers(ver1, &vlevel1);
    if (ver1_levels <= 0) {
        /* free memory */
        if (vlevel1 != NULL) {
            free(vlevel1);
            vlevel1 = NULL;
        }
	dbg(DBG_MED, "first version string is invalid");
        return 4;
    }

    /*
     * convert second version string
     */
    ver2_levels = (int)allocate_vers(ver2, &vlevel2);
    if (ver2_levels <= 0) {
        /* free memory */
        if (vlevel1 != NULL) {
            free(vlevel1);
            vlevel1 = NULL;
        }
        if (vlevel2 != NULL) {
            free(vlevel2);
            vlevel2 = NULL;
        }

        dbg(DBG_MED, "second version string is invalid");
        return 4;
    }

    /*
     * compare versions
     */
    for (i=0; i < ver1_levels && i < ver2_levels; ++i) {

	/*
	 * compare both versions at a given level (i)
	 */
	if (vlevel1[i] > vlevel2[i]) {

	    /* ver1 > ver2 */
	    dbg(DBG_MED, "version 1 level %d: %jd > version 2 level %d: %jd",
			  i, vlevel1[i], i, vlevel2[i]);
	    dbg(DBG_LOW, "%s > %s", ver1, ver2);
	    /* free memory */
	    if (vlevel1 != NULL) {
		free(vlevel1);
		vlevel1 = NULL;
	    }
	    if (vlevel2 != NULL) {
		free(vlevel2);
		vlevel2 = NULL;
	    }
	    /* report ver1 > ver2 */
            return 0;
	} else if (vlevel1[i] < vlevel2[i]) {

	    /* ver1 < ver2 */
	    dbg(DBG_MED, "version 1 level %d: %jd < version 2 level %d: %jd",
			  i, vlevel1[i], i, vlevel2[i]);
	    dbg(DBG_LOW, "%s < %s", ver1, ver2);
	    /* free memory */
	    if (vlevel1 != NULL) {
		free(vlevel1);
		vlevel1 = NULL;
	    }
	    if (vlevel2 != NULL) {
		free(vlevel2);
		vlevel2 = NULL;
	    }
	    /* report ver1 < ver2 */
            return 1;
	} else {

	    /* versions match down to this level */
	    dbg(DBG_MED, "version 1 level %d: %jd == version 2 level %d: %jd",
			  i, vlevel1[i], i, vlevel2[i]);
	}
    }
    dbg(DBG_MED, "versions match down to level: %d",
		 (ver1_levels > ver2_levels) ? ver2_levels : ver1_levels);

    /*
     * free memory
     */
    if (vlevel1 != NULL) {
	free(vlevel1);
	vlevel1 = NULL;
    }
    if (vlevel2 != NULL) {
	free(vlevel2);
	vlevel2 = NULL;
    }

    /*
     * ver1 matches ver2 to the extent that they share the same level
     *
     * The presence of sub-levels will determine the final comparison
     */
    if (ver1_levels < ver2_levels) {

	dbg(DBG_MED, "version 1 level count: %d < version level count: %d",
		     ver1_levels, ver2_levels);
	dbg(DBG_LOW, "%s < %s", ver1, ver2);
	/* report ver1 < ver2 */
        return 1;
    } else if (ver1_levels > ver2_levels) {

	dbg(DBG_MED, "version 1 level count: %d > version level count: %d",
		     ver1_levels, ver2_levels);
	dbg(DBG_LOW, "%s > %s", ver1, ver2);
	/* report ver1 > ver2 */
        return 0;
    }

    /*
     * versions match
     */
    dbg(DBG_MED, "version 1 level count: %d == version level count: %d",
		 ver1_levels, ver2_levels);
    dbg(DBG_LOW, "%s == %s", ver1, ver2);
    /* report ver1 == ver2 */
    return 0;
}

/*
 * allocate_vers - convert version string into an allocated array or version numbers
 *
 * given:
 *	ver	version string
 *	pvers	pointer to allocated array of versions
 *
 * returns:
 *	> 0  ==> number of version integers in allocated array of versions
 *	0 <= ==> string was not a valid version string,
 *		 array of versions not allocated
 *
 * NOTE: This function does not return on allocation failures or NULL args.
 */
size_t
allocate_vers(char *str, intmax_t **pvers)
{
    char *wstr = NULL;		/* working allocated copy of orig_str */
    char *wstr_start = NULL;	/* pointer to starting point of wstr */
    size_t len;			/* length of version string */
    bool dot = false;		/* true ==> previous character was dot */
    size_t dot_count = 0;	/* number of .'s in version string */
    char *word = NULL;		/* token */
    char *brkt;			/* last arg for strtok_r() */
    size_t i;

    /*
     * firewall
     */
    if (str == NULL || pvers == NULL) {
	err(57, __func__, "NULL arg(s)");
	not_reached();
    }
    len = strlen(str);
    if (len <= 0) {
	dbg(DBG_MED, "version string is empty");
	return 0;
    }

    /*
     * duplicate str
     */
    errno = 0;		/* pre-clear errno for errp() */
    wstr = strdup(str);
    if (wstr == NULL) {
	errp(58, __func__, "cannot strdup: <%s>", str);
	not_reached();
    }
    wstr_start = wstr;

    /*
     * trim leading non-digits
     */
    for (i=0; i < len; ++i) {
	if (isascii(wstr[i]) && isdigit(wstr[i])) {
	    /* stop on first digit */
	    break;
	}
    }
    if (i == len) {
	/* report invalid version string */
	dbg(DBG_MED, "version string contained no digits: <%s>", wstr);
	/* free strdup()d string if != NULL */
	if (wstr_start != NULL) {
	    free(wstr_start);
	    wstr_start = NULL;
	}
	return 0;
    }
    wstr += i;
    len -= i;

    /*
     * trim at and beyond any whitespace
     */
    for (i=0; i < len; ++i) {
	if (isascii(wstr[i]) && isspace(wstr[i])) {
	    wstr[i] = '\0';
	    len = i;
	    break;
	}
    }

    /*
     * trim trailing non-digits
     */
    for (i=len-1; i > 0; --i) {
	if (isascii(wstr[i]) && isdigit(wstr[i])) {
	    /* stop on first digit */
	    break;
	}
    }
    wstr[i+1] = '\0';
    len = i;

    /*
     * we now have a string that starts and ends with digits
     *
     * Inspect the remaining string for digits and '.'s only.
     * Also reject string if we find more than 2 '.'s in a row.
     */
    dbg(DBG_HIGH, "trimmed version string: <%s>", wstr);
    for (i=0; i < len; ++i) {
	if (isascii(wstr[i]) && isdigit(wstr[i])) {
	    dot = false;
	} else if (wstr[i] == '.') {
	    if (dot == true) {
		/* report invalid version string */
		dbg(DBG_MED, "trimmed version string contains 2 dots in a row: <%s>", wstr);
		/* free strdup()d string if != NULL */
		if (wstr_start != NULL) {
		    free(wstr_start);
		    wstr_start = NULL;
		}
		return 0;
	    }
	    dot = true;
	    ++dot_count;
	} else {
	    /* report invalid version string */
	    dbg(DBG_MED, "trimmed version string contains non-version character: wstr[%ju] = <%c>: <%s>",
			 (uintmax_t)i, wstr[i], wstr);
	    /* free strdup()d string if != NULL */
	    if (wstr_start != NULL) {
		free(wstr_start);
		wstr_start = NULL;
	    }
	    return 0;
	}
    }
    dbg(DBG_MED, "trimmed version string is valid format: <%s>", wstr);
    dbg(DBG_HIGH, "trimmed version string has %ju '.'s in it: <%s>", (uintmax_t)dot_count, wstr);

    /*
     * we now know the version string is valid format: ([0-9]+\.)*[0-9]+
     *
     * Allocate the array of dot_count+1 versions.
     */
    errno = 0;		/* pre-clear errno for errp() */
    *pvers = (intmax_t *)calloc(dot_count+1+1, sizeof(intmax_t));
    if (*pvers == NULL) {
	errp(59, __func__, "cannot calloc %ju intmax_ts", (uintmax_t)dot_count+1+1);
	not_reached();
    }

    /*
     * tokenize version string using '.' delimiters
     */
    for (i=0, word=strtok_r(wstr, ".", &brkt);
         i <= dot_count && word != NULL;
	 ++i, word=strtok_r(NULL, ".", &brkt)) {
	/* word is the next version string - convert to integer */
	dbg(DBG_VVHIGH, "version level %ju word: <%s>", (uintmax_t)i, word);
	(void) string_to_intmax(word, &(*pvers)[i]);
	dbg(DBG_VHIGH, "version level %ju: %jd", (uintmax_t)i, (*pvers)[i]);
    }

    /* we no longer need the duplicated string */
    if (wstr_start != NULL) {
	free(wstr_start);
	wstr_start = NULL;
    }

    /*
     * return number of version levels
     */
    return dot_count+1;
}
