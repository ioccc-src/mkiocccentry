/*
 * random_answers - generate a random answer file for mkiocccentry
 *
 * Copyright (c) 2025 by Landon Curt Noll.  All Rights Reserved.
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
#include <limits.h>

/*
 * mkiocccentry - form IOCCC entry compressed tarball
 */
#include "random_answers.h"


/*
 * definitions
 */
#define DISABLED_UUID "00000000-0000-4000-8000-000000000000"	/* username for which "disable_login" is true */
#define SPACE_ODDS (8)		    /* average letters between a space */
#define RANDOM_TITLE_MIN_LEN (8)    /* minimum length of a title */


/*
 * biased_random_range - generate a random number a range
 *
 * NOTE: When max-min is not a power of 2, then there will be a very tiny bias
 *	 to the lower part of the range.  But for what we need, this is good enough.
 *
 * given:
 *	min	    - minimum number in the range
 *	max	    - maximum number in the range
 *
 * return:
 *	random number >= min and <= max
 *
 * NOTE: This biased_random_range() function is completely fair when
 *	 max-min is a power of 2.
 *
 * NOTE: If min > max, the range will be inverted.
 *
 * NOTE: If max >= LONG_MAX, it will be treated as max it LONG_MAX-1 to avoid long overflow.
 *
 * NOTE: If min >= max, and min >= min is LONG_MAX-1 to avoid long overflow.
 */
long
biased_random_range(long min, long max)
{
    long result;	/* return value >= min and <= max */
    long low;		/* low value */
    long high;		/* high value */

    /*
     * firewall - min == max
     *
     * If someone sets min and max to the same value we will just return min.
     */
    if (min == max) {
	return min;
    }

    /*
     * setup the range
     *
     * We invert the range if min >= max.
     *
     * We also guard against long overflow by forcing
     * the top of the range to be LONG_MAX-1 if the
     * top of the range is set to LONG_MAX.
     */
    if (min < max) {
	low = min;
	if (max < LONG_MAX) {
	    high = max+1;
	} else {
	    high = max;
	}
    } else {
	if (max < LONG_MAX) {
	    low = max+1;
	} else {
	    low = max;
	}
	low = max+1;
	high = min;
    }

    /*
     * firewall - zero length range
     *
     * If in setting up the range, due to >= LONG_MAX, we avoid division by zero.
     */
    if (low >= high) {
	return low;
    }

    /*
     * return the result
     *
     * NOTE: If hi - low is not a power of 2, then this random selection
     *	     will be biased very slightly in favor or smaller values.
     *	     For what we need, this tiny bias does not concern us.
     *	     We could try to fix this tiny bias, but at the cost of
     *	     extra code and extra time.
     */
    result = (random() % (high - low)) + low;
    return result;
}


/*
 * random_alphanum - return a random alpha numeric character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9A-Za-z]
 */
char
random_alphanum(void)
{
    int pick;	    /* letter to select */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26+26-1);	/* -1 because we count from 0 */

    /*
     * case: return a digit
     */
    if (pick < 10) {
	return '0' + pick;
    }
    pick -= 10;

    /*
     * case: return an UPPER case letter
     */
    if (pick < 26) {
	return 'A' + pick;
    }
    pick -= 26;

    /*
     * case: return a lower case letter
     */
    pick = (pick >= 26) ? 25 : pick; /* paranoia */
    return 'a' + pick;
}


/*
 * random_lower_alphanum_char - return a random lower case alpha numeric character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9a-z]
 */
char
random_lower_alphanum_char(void)
{
    int pick;	    /* letter to select */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26-1); /* -1 because we count from 0 */

    /*
     * case: return a digit
     */
    if (pick < 10) {
	return '0' + pick;
    }
    pick -= 10;

    /*
     * case: return a lower case letter
     */
    pick = (pick >= 26) ? 25 : pick; /* paranoia */
    return 'a' + pick;
}


/*
 * random_lower_posixsafe_char - return a random lower case posix safe character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9a-z_+-]
 */
char
random_lower_posixsafe_char(void)
{
    int pick;	    /* letter to select */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26-3+1); /* -1 because we count from 0 */

    /*
     * case: return a digit
     */
    if (pick < 10) {
	return '0' + pick;
    }
    pick -= 10;

    /*
     * case: _ + or -
     */
    switch (pick) {
    case 0:
	return '_';
    case 1:
	return '+';
    case 2:
	return '-';
    default:
	pick -= 3;
	/* FALL THROUGH */
    }

    /*
     * case: return a lower case letter
     */
    pick = (pick >= 26) ? 25 : pick; /* paranoia */
    return 'a' + pick;
}


/*
 * random_lower_posixsafe_str - load a random posix safe string
 *
 * Return a POSIX safe string of the form:
 *
 *   [a-z0-9][a-z0-9_+-]*
 *
 * given:
 *	buf	    - where to write string (must BUFSIZ bytes or longer)
 *	min	    - minimum number of characters to load
 *	max	    - maximum number of characters to load
 *
 * NOTE: If min < 1, we will force min to be 1
 * NOTE: If max >= BUFSIZ, we will force max to be BUFSIZ-1
 * NOTE: If min > max, we will force min to be max
 */
void
random_lower_posixsafe_str(char *str, unsigned int min, unsigned int max)
{
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(200, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (min < 1) {
	min = 1;
    }
    if (max >= BUFSIZ) {
	max = BUFSIZ-1;
    }
    if (min > max) {
	min = max;
    }

    /*
     * generate the lower case alphanum character
     */
    str[0] = random_lower_alphanum_char();

    /*
     * load the rest of the lower case POSIX safe characters
     */
    for (i=1; i < max; ++i) {
	str[i] = random_lower_posixsafe_char();
    }
    str[max] = '\0';
    return;
}


/*
 * random_words_str - load a random words into a string
 *
 * Return words in a string:
 *
 *   [a-z0-9][a-z0-9 ]*
 *
 * We use SPACE_ODDS to determine the odds that a character is space.
 *
 * given:
 *	buf	    - where to write string (must BUFSIZ bytes or longer)
 *	min	    - minimum number of characters to load
 *	max	    - maximum number of characters to load
 *
 * NOTE: If min < 1, we will force min to be 1
 * NOTE: If max >= BUFSIZ, we will force max to be BUFSIZ-1
 * NOTE: If min > max, we will force min to be max
 */
void
random_words_str(char *str, unsigned int min, unsigned int max)
{
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(201, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (min < 1) {
	min = 1;
    }
    if (max >= BUFSIZ) {
	max = BUFSIZ-1;
    }
    if (min > max) {
	min = max;
    }

    /*
     * generate the first alphanum character
     */
    str[0] = random_alphanum();

    /*
     * load the rest of the title characters
     */
    for (i=1; i < max; ++i) {

	/*
	 * case: odds pick space, expect when at the last character
	 */
	if (i < max-1 && biased_random_range(1, SPACE_ODDS) == SPACE_ODDS) {
	    str[i] = ' ';

	/*
	 * otherwise: POSIX safe char
	 */
	} else {
	    str[i] = random_alphanum();
	}
    }
    str[max] = '\0';
    return;
}


/*
 * generate_answers - generate pseudo-randomly generated answers
 *
 * given:
 *	answers	    - filename of the pseudo-randomly generated answers to form
 *
 * NOTE: This function does NOT return on error.
 */
void
generate_answers(char const *answers)
{
    FILE *answerp = NULL;		/* file pointer to the answers file */
    char buf[BUFSIZ+1];			/* scratch buffer for random strings of sane length */
    int author_count;			/* number of authors */
    int ret;				/* libc return code */
    int i;

    /*
     * firewall
     */
    if (answers == NULL) {
	err(202, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * open the answers file for writing
     */
    errno = 0;			/* pre-clear errno for errp() */
    answerp = fopen(answers, "w");
    if (answerp == NULL) {
	errp(203, __func__, "cannot create answers file: %s", answers);
	not_reached();
    }

    /*
     * initialize answer file with the proper answer version
     */
    fprint(answerp, "%s\n", MKIOCCCENTRY_ANSWERS_VERSION);

    /*
     * we will use username for which "disable_login" is true
     *
     * The submit tool's default etc/init.iocccpasswd.json file
     * for the DISABLED_UUID has "disable_login" is true
     * and thus we use it here because the resulting cannot
     * be uploaded to the submit server.
     */
    fprint(answerp, "%s\n", DISABLED_UUID);

    /*
     * pick a random slot number
     */
    fprint(answerp, "%ld\n", biased_random_range(1, MAX_SUBMIT_SLOT));

    /*
     * form a random title
     */
    buf[BUFSIZ] = buf[0] = '\0';    /* paranoia */
    random_lower_posixsafe_str(buf, 1, MAX_TITLE_LEN);
    fprint(answerp, "%s\n", buf);

    /*
     * form a random abstract
     */
    buf[BUFSIZ] = buf[0] = '\0';    /* paranoia */
    random_words_str(buf, RANDOM_TITLE_MIN_LEN, MAX_TITLE_LEN);
    fprint(answerp, "%s\n", buf);

    /*
     * select a random number of authors
     */
    author_count = biased_random_range(1, MAX_AUTHORS);

    /*
     * generate information for each author
     */
    for (i=0; i < author_count; ++i) {
	/* XXX - add more code here XXX */
    }

    /* XXX - add more code here XXX */

    /*
     * close up the answers file
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fclose(answerp);
    if (ret != 0) {
	warnp(__func__, "error in fclose to the random answers file");
    }
    err(0, __func__, "XXX - -d and -s seed code not fully implemented - XXX"); /*ooo*/
    return;
}
