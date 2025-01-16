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
#define SPACE_ODDS (6)			/* average letters between a space */
#define RANDOM_TITLE_MIN_LEN (8)	/* minimum length of a title */
#define RANDOM_ABSTRACT_MIN_LEN (8)	/* minimum length of an abstract */
#define RANDOM_NAME_MIN_LEN (10)	/* minimum length of an author name */
#define RANDOM_EMAIL_MIN_LEN (6)	/* min email length is 6: x@x.xx */
#define RANDOM_URL_MIN_LEN (13)		/* min URL is 13: http://h.xx/u or 14: https://h.xx/u */
#define RANDOM_MASTODON_MIN_LEN (7)	/* min Mastodon handle is 7: @u@h.xx */
#define RANDOM_AFFILIATION_MIN_LEN (8)  /* minimum length of an affiliation */
#define MAX_RANDOM (0x7fffffff)		/* maximum value returned by random(3) */
#define STRLEN(s) (sizeof (s)-1)	/* length of a constant string */


/*
 * biased_random_range - generate a random number a range
 *
 * Returns a random value that is "nearly" uniformly distributed over a range,
 *
 * NOTE: When labs(above-min) is not a power of 2, then there will be a very tiny bias
 *	 to the lower part of the range.  But for what we need, this is good enough.
 *
 * given:
 *	min	    - minimum number to return
 *	above	    - 1 above the maximum value allowed to return
 *
 * return:
 *	"nearly" uniformly selected pseudo-random over the open range [min, above).
 *
 * NOTE: If min > above, the range will be reversed.
 *
 * NOTE: This function is uniform ONLY when the range labs(above-min) is a power of 2.
 *	 We could fix this very tiny bias, but the added complexity of turning a non-power of 2
 *	 into a power of 2 range is not worth it for our intended use.
 */
long
biased_random_range(long min, long above)
{
    long result;	/* return value >= min and <= max */
    long range;		/* range of values we select over */

    /*
     * setup the range
     *
     * If min is above above, reverse the range.
     */
    if (min > above) {
	/* open range values are out of order - swap them */
	range = min;
	min = above;
	above = range;
    }
    range = above - min;

    /*
     * firewall - unit range
     */
    if (range <= 1) {
	return min;
    }

    /*
     * return the result
     *
     * NOTE: If range is NOT a power of 2, then this random selection
     *	     will be biased very slightly in favor or smaller values.
     *
     *	     For what we need, this tiny bias does not concern us.
     *	     We could try to fix this tiny bias, but at the cost of
     *	     extra code and extra time.
     */
    result = (random() % range) + min;
    return result;
}


/*
 * top_skew_random_range - generate a random number a range skewed towards the top
 *
 * Unlike biased_random_range() that attempts to returns a random value that is
 * "uniformly distributed" over a range, this returns a random value in where
 * the range as a quadratic bias.  With a quadratic bias, selections are heavily shewed
 * towards the upper end of the range (along a quadratic curve).
 *
 * When we tallied 1000000 returns using top_skew_random_range(0,7), we found:
 *
 *     0 ==> 64490
 *     1 ==> 69447
 *     2 ==> 75521
 *     3 ==> 84030
 *     4 ==> 94609
 *     5 ==> 112253
 *     6 ==> 145898
 *     7 ==> 353752
 *
 * "Your mileage may vary" as the expression goes.
 *
 * given:
 *	min	    - minimum number to return
 *	above	    - 1 above the maximum value allowed to return
 *
 * return:
 *	pseudo-random selected value over the open range [min, above) that
 *	is quadratically biased towards the upper end of the range.
 *
 * NOTE: If min > above, the range will be reversed.
 *
 * NOTE: This function is uniform ONLY when the range labs(above-min) is a power of 2.
 *	 We could fix this very tiny bias, but the added complexity of turning a non-power of 2
 *	 into a power of 2 range is not worth it for our intended use.
 */
long
top_skew_random_range(long min, long above)
{
    long result;	/* return value >= min and <= max */
    long range;		/* range of values we select over */
    double bias;	/* biased value from 0 to 1.0 */

    /*
     * setup the range
     *
     * If min is above above, reverse the range.
     */
    if (min > above) {
	/* open range values are out of order - swap them */
	range = min;
	min = above;
	above = range;
    }
    range = above - min;

    /*
     * firewall - unit range
     */
    if (range <= 1) {
	return min;
    }

    /*
     * generate a pseudo-random value between 0 and 1, shewed aquatically to the high end
     *
     * The largest value returned by random(3) is MAX_RANDOM, so we use one more
     * than that value for our conversion to a [0.0, 1.0} half open range.
     */
    bias = (double)random() / ((double)MAX_RANDOM + 1.0);
    /* convert from [0.0, 1.0) into inverted quadratic [0.0, 1.0) */
    bias = 1.0 - (bias*bias);

    /*
     * return the result
     *
     * NOTE: If range is NOT a power of 2, then this random selection
     *	     will be biased very slightly in favor or smaller values.
     *
     *	     For what we need, this tiny bias does not concern us.
     *	     We could try to fix this tiny bias, but at the cost of
     *	     extra code and extra time.
     */
    result = min + (long)(bias * (double)range);
    return result;
}


/*
 * random_alpha_char - return a random alpha character
 *
 * return:
 *	A constant character pseudo-randomly selected from [A-Za-z]
 */
char
random_alpha_char(void)
{
    unsigned int pick;	    /* letter to select */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 26+26);

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
 * random_upper_alpha_char - return a random UPPER case alpha character
 *
 * return:
 *	A constant character pseudo-randomly selected from [A-Z]
 */
char
random_upper_alpha_char(void)
{
    unsigned int pick;	    /* character offset */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 26);

    /*
     * case: return an UPPER case letter
     */
    pick = (pick >= 26) ? 25 : pick; /* paranoia */
    return 'A' + pick;
}


/*
 * random_lower_alpha_char - return a random loser case alpha character
 *
 * return:
 *	A constant character pseudo-randomly selected from [a-z]
 */
char
random_lower_alpha_char(void)
{
    unsigned int pick;	    /* character offset */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 26);

    /*
     * case: return a lower case letter
     */
    pick = (pick >= 26) ? 25 : pick; /* paranoia */
    return 'a' + pick;
}


/*
 * random_alphanum_char - return a random alpha numeric character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9A-Za-z]
 */
char
random_alphanum_char(void)
{
    unsigned int pick;	    /* letter to select */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26+26);

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
 * random_upper_alphanum_char - return a random UPPER case alpha numeric character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9A-Z]
 */
char
random_upper_alphanum_char(void)
{
    unsigned int pick;	    /* character offset */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26);

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
    pick = (pick >= 26) ? 25 : pick; /* paranoia */
    return 'A' + pick;
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
    unsigned int pick;	    /* character offset */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26);

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
 * random_posixsafe_char - return a random POSIX safe character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9A-Za-z._+-]
 */
char
random_posixsafe_char(void)
{
    unsigned int pick;	    /* character offset */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26+26+4);

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
    if (pick < 26) {
	return 'a' + pick;
    }
    pick -= 26;

    /*
     * case: . _ + or -
     */
    switch (pick) {
    case 0:
	return '.';
    case 1:
	return '_';
    case 2:
	return '+';
    }
    return '-';
}


/*
 * random_upper_posixsafe_char - return a random UPPER case POSIX safe character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9A-Z._+-]
 */
char
random_upper_posixsafe_char(void)
{
    unsigned int pick;	    /* character offset */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26+4);

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
     * case: . _ + or -
     */
    switch (pick) {
    case 0:
	return '.';
    case 1:
	return '_';
    case 2:
	return '+';
    }
    return '-';
}


/*
 * random_lower_posixsafe_char - return a random lower case POSIX safe character
 *
 * return:
 *	A constant character pseudo-randomly selected from [0-9a-z._+-]
 */
char
random_lower_posixsafe_char(void)
{
    unsigned int pick;	    /* character offset */

    /*
     * pick the letter
     */
    pick = biased_random_range(0, 10+26+4);

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
    if (pick < 26) {
	return 'a' + pick;
    }
    pick -= 26;

    /*
     * case: . _ + or -
     */
    switch (pick) {
    case 0:
	return '.';
    case 1:
	return '_';
    case 2:
	return '+';
    }
    return '-';
}


/*
 * random_alpha_str - load random alpha characters into a string
 *
 * Return as a string:
 *
 *   ^[A-Za-z]+$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_alpha_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
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
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_alpha_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_upper_alpha_str - load random UPPER case alpha characters into a string
 *
 * Return as a string:
 *
 *   ^[A-Z]+$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_upper_alpha_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(202, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_upper_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_upper_alpha_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_lower_alpha_str - load random lower case alpha characters into a string
 *
 * Return as a string:
 *
 *   ^[a-z]+$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_lower_alpha_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(203, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_lower_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_lower_alpha_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_alphanum_str - load random alphanumeric characters into a string
 *
 * Return as a string:
 *
 *   ^[0-9A-Za-z]+$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_alphanum_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(204, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_alphanum_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_alphanum_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_upper_alphanum_str - load random UPPER case alphanumeric characters into a string
 *
 * Return as a string:
 *
 *   ^[0-9A-Z]+$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_upper_alphanum_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(205, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_upper_alphanum_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_upper_alphanum_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_lower_alphanum_str - load random lower case alphanumeric characters into a string
 *
 * Return as a string:
 *
 *   ^[0-9a-z]+$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_lower_alphanum_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(206, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_lower_alphanum_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_lower_alphanum_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_posixsafe_str - load a POSIX safe chars into a string
 *
 * Return as a string:
 *
 *   ^[0-9A-Za-z][0-9A-Za-z._+-]*$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_posixsafe_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(207, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_alphanum_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_posixsafe_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_upper_posixsafe_str - load UPPER case POSIX safe chars into a string
 *
 * Return as a string:
 *
 *   ^[0-9A-Z][0-9A-Z._+-]*$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_upper_posixsafe_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(208, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_upper_alphanum_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_upper_posixsafe_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_lower_posixsafe_str - load lower case POSIX safe chars into a string
 *
 * Return as a string:
 *
 *   ^[0-9a-z][0-9a-z._+-]*$
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_lower_posixsafe_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(209, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_lower_alphanum_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {
	str[i] = random_lower_posixsafe_char();
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_words_str - load random words into a string
 *
 * Return as a string:
 *
 *   ^[A-Z][a-z]*( [A-Z][a-z]*)*$
 *
 * A word starts with a UPPER case letter.
 * The rest of the characters in the word are lower case.
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_words_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(210, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_upper_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {

	/*
	 * case: If the previous character is not space, and
	 *	 we are not on the last character, and
	 *	 the of SPACE_ODDS is 0,
	 *	 we set this character as space.
	 */
	if (i < len-1 && str[i-1] != ' ' && biased_random_range(0, SPACE_ODDS) == 0) {
	    str[i] = ' ';

	/*
	 * case: select first character of a new word
	 */
	} else if (str[i-1] == ' ') {
	    str[i] = random_upper_alpha_char();

	/*
	 * otherwise: select a subsequent word character
	 */
	} else {
	    str[i] = random_lower_alpha_char();
	}
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_upper_words_str - load random UPPER case words into a string
 *
 * Return as a string:
 *
 *   ^[A-Z]+( [A-Z]+)*$
 *
 * A word starts with a UPPER case letter.
 * The rest of the characters in the word are UPPER case.
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_upper_words_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(211, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_upper_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {

	/*
	 * case: If the previous character is not space, and
	 *	 we are not on the last character, and
	 *	 the quadratically skewed top SPACE_ODDS is 0,
	 *	 we set this character as space.
	 */
	if (i < maxlen-1 && str[i-1] != ' ' && top_skew_random_range(0, SPACE_ODDS) == 0) {
	    str[i] = ' ';

	/*
	 * otherwise: select a subsequent word character
	 */
	} else {
	    str[i] = random_upper_alpha_char();
	}
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_lower_words_str - load random upper case words into a string
 *
 * Return as a string:
 *
 *   ^[a-z]+( [a-z]+)*$
 *
 * A word starts with a lower case letter.
 * The rest of the characters in the word are lower case.
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_lower_words_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(212, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_lower_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {

	/*
	 * case: If the previous character is not space, and
	 *	 we are not on the last character, and
	 *	 the quadratically skewed top SPACE_ODDS is 0,
	 *	 we set this character as space.
	 */
	if (i < maxlen-1 && str[i-1] != ' ' && top_skew_random_range(0, SPACE_ODDS) == 0) {
	    str[i] = ' ';

	/*
	 * otherwise: select a subsequent word character
	 */
	} else {
	    str[i] = random_lower_alpha_char();
	}
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_posixsafe_words_str - load a random POSIX safe char words into a string
 *
 * Return as a string:
 *
 *   ^[0-9A-Za-z][0-9A-Za-z._+-]*( [0-9A-Za-z][0-9A-Za-z._+-]*)*$
 *
 * A word starts with an alphanum case letter.
 * The rest of the characters in the word are POSIX safe.
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_posixsafe_words_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(213, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_upper_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {

	/*
	 * case: If the previous character is not space, and
	 *	 we are not on the last character, and
	 *	 the quadratically skewed top SPACE_ODDS is 0,
	 *	 we set this character as space.
	 */
	if (i < maxlen-1 && str[i-1] != ' ' && top_skew_random_range(0, SPACE_ODDS) == 0) {
	    str[i] = ' ';

	/*
	 * case: select first character of a new word
	 */
	} else if (str[i-1] == ' ') {
	    str[i] = random_upper_alpha_char();

	/*
	 * otherwise: select a subsequent word character
	 */
	} else {
	    str[i] = random_posixsafe_char();
	}
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_upper_posixsafe_words_str - load a random UPPER case POSIX safe char words into a string
 *
 * Return as a string:
 *
 *   ^[0-9A-Z][0-9A-Z._+-]*( [0-9A-Z][0-9A-Z._+-]*)*$
 *
 * A word starts with an alphanum case letter.
 * The rest of the characters in the word are POSIX safe UPPER case.
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * A word starts with an alphanum case letter.  The rest of the characters in the word are POSIX lower case safe chars.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_upper_posixsafe_words_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(214, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_upper_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {

	/*
	 * case: If the previous character is not space, and
	 *	 we are not on the last character, and
	 *	 the quadratically skewed top SPACE_ODDS is 0,
	 *	 we set this character as space.
	 */
	if (i < maxlen-1 && str[i-1] != ' ' && top_skew_random_range(0, SPACE_ODDS) == 0) {
	    str[i] = ' ';

	/*
	 * case: select first character of a word
	 */
	} else if (str[i-1] == ' ') {
	    str[i] = random_upper_alpha_char();

	/*
	 * otherwise: select a rest of the word character
	 */
	} else {
	    str[i] = random_upper_posixsafe_char();
	}
    }
    str[len] = '\0'; /* terminate the string */
    return;
}


/*
 * random_lower_posixsafe_words_str - load a random lower case POSIX safe char words into a string
 *
 * Return as a string:
 *
 *   ^[0-9a-z][0-9a-z._+-]*( [0-9a-z][0-9a-z._+-]*)*$
 *
 * A word starts with an alphanum case letter.
 * The rest of the characters in the word are POSIX safe lower case.
 *
 * We use a quadratically top skewed SPACE_ODDS to determine the odds that a character is space
 * with a bias towards making longer words.
 *
 * given:
 *	buf	    - where to load string (must be at least BUFSIZ bytes long)
 *	minlen	    - minimum number of characters (not including trailing NUL) to load
 *	maxlen	    - maximum number of characters (not including trailing NUL) to load
 *
 * NOTE: If minlen < 1, we will force minlen to be 1
 * NOTE: If maxlen >= BUFSIZ, we will force maxlen to be BUFSIZ-1
 * NOTE: If minlen > maxlen, we will force minlen to be maxlen
 */
void
random_lower_posixsafe_words_str(char *str, unsigned int minlen, unsigned int maxlen)
{
    unsigned int len;	    /* form a string this line */
    unsigned int i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(215, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * force bogus args into some form of sanity
     */
    if (minlen < 1) {
	minlen = 1;
    }
    if (maxlen >= BUFSIZ) {
	maxlen = BUFSIZ-1;
    }
    if (minlen > maxlen) {
	minlen = maxlen;
    }

    /*
     * determine the length of the string
     */
    len = top_skew_random_range(minlen, maxlen+1);

    /*
     * generate the first character
     */
    str[0] = random_lower_alpha_char();

    /*
     * load the rest of the string
     */
    for (i=1; i < len; ++i) {

	/*
	 * case: If the previous character is not space, and
	 *	 we are not on the last character, and
	 *	 the quadratically skewed top SPACE_ODDS is 0,
	 *	 we set this character as space.
	 */
	if (i < maxlen-1 && str[i-1] != ' ' && top_skew_random_range(0, SPACE_ODDS) == 0) {
	    str[i] = ' ';

	/*
	 * case: select first character of a word
	 */
	} else if (str[i-1] == ' ') {
	    str[i] = random_lower_alpha_char();

	/*
	 * otherwise: select a rest of the word character
	 */
	} else {
	    str[i] = random_lower_posixsafe_char();
	}
    }
    str[len] = '\0'; /* terminate the string */
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
    int rlen;				/* random length of something to form */
    int ret;				/* libc return code */
    int i;

    /*
     * firewall
     */
    if (answers == NULL) {
	err(216, __func__, "called with NULL arg");
	not_reached();
    }

    /*
     * open the answers file for writing
     */
    errno = 0;			/* pre-clear errno for errp() */
    answerp = fopen(answers, "w");
    if (answerp == NULL) {
	errp(217, __func__, "cannot create answers file: %s", answers);
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
    fprint(answerp, "%ld\n", biased_random_range(0, MAX_SUBMIT_SLOT+1));

    /*
     * form a random title
     */
    memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
    (void) random();		/* just for j-random fun */
    random_lower_posixsafe_str(buf, RANDOM_TITLE_MIN_LEN, MAX_TITLE_LEN);
    fprint(answerp, "%s\n", buf);

    /*
     * form a random abstract
     */
    memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
    (void) random();		/* just for j-random fun */
    random_words_str(buf, RANDOM_ABSTRACT_MIN_LEN, MAX_TITLE_LEN);
    fprint(answerp, "%s\n", buf);

    /*
     * select a random number of authors
     */
    author_count = biased_random_range(1, MAX_AUTHORS+1);
    fprint(answerp, "%d\n", author_count);

    /*
     * form information for each author
     */
    for (i=0; i < author_count; ++i) {

	unsigned int pos;	/* position to assign a specific character within a string */
	unsigned int head_len;	/* length of the leading part of what is being formed */
	char const *url_lead;	/* URL lead for http:// or https:// */
	char *handle = NULL;	/* default author handle */
	bool anonymous;		/* true ==> is anonymous */

	/*
	 * form random author name
	 */
	memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
	(void) random();		/* just for j-random fun */
	random_words_str(buf, RANDOM_NAME_MIN_LEN, MAX_NAME_LEN);
	fprint(answerp, "%s\n", buf);

	/*
	 * form default handle
	 */
	handle = default_handle(buf);

	/*
	 * form random author location
	 */
	(void) random();		/* just for j-random fun */
	fprint(answerp, "%s\n", loc[biased_random_range(0, SIZEOF_LOCATION_TABLE-1)].code);

	/*
	 * form random author email address
	 *
	 * 25% of the time, the author will be anonymous.
	 */
	memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
	(void) random();		/* just for j-random fun */
	if (biased_random_range(0, 4) > 0) {				    /* 75% chance of not anonymous */
	    rlen = biased_random_range(RANDOM_EMAIL_MIN_LEN, MAX_EMAIL_LEN+1);
	    random_alphanum_str(buf, rlen, rlen);			    /* form overall string */
	    pos = biased_random_range(2, rlen-4);			    /* put the @ randomly in the middle */
	    buf[pos] = '@';						    /* load @ */
	    buf[rlen-3] = '.';						    /* load . */
	}
	fprint(answerp, "%s\n", buf);

	/*
	 * form random author 1st URL
	 *
	 * 25% of the time, the author will be anonymous.
	 */
	memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
	(void) random();		/* just for j-random fun */
	anonymous = true;
	if (biased_random_range(0, 4) > 0) {				    /* 75% chance of not anonymous */
	    anonymous = false;
	    if (biased_random_range(0, 4) > 0) {				/* 75% chance of https:// */
		rlen = biased_random_range(RANDOM_URL_MIN_LEN+1, MAX_URL_LEN+1);
		url_lead =	  "https://";
		head_len = STRLEN("https://");
	    } else {								/* 25% chance of http:// */
		rlen = biased_random_range(RANDOM_URL_MIN_LEN, MAX_URL_LEN+1);
		url_lead =	  "http://";
		head_len = STRLEN("http://");
	    }
	    strncpy(buf, url_lead, head_len);
	    random_lower_alphanum_str(buf+head_len, rlen-head_len, rlen-head_len);   /* form overall string */
	    pos = biased_random_range(head_len+2,  rlen-4);
	    buf[pos] = '.';						    /* load . */
	    buf[pos+3] = '/';						    /* load / */
	}
	fprint(answerp, "%s\n", buf);

	/*
	 * form random author 2nd URL
	 *
	 * If author URL was NOT anonymous, 50% of the time, the author will have a 2nd URL
	 */
	memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
	(void) random();		/* just for j-random fun */
	if (anonymous == false && biased_random_range(0, 2) > 0) {	    /* 50% change of 2nd URL if 1st URL */
	    if (biased_random_range(0, 4) > 0) {				/* 75% chance of https:// */
		rlen = biased_random_range(RANDOM_URL_MIN_LEN+1, MAX_URL_LEN+1);
		url_lead =	  "https://";
		head_len = STRLEN("https://");
	    } else {								/* 25% chance of http:// */
		rlen = biased_random_range(RANDOM_URL_MIN_LEN, MAX_URL_LEN+1);
		url_lead =	  "http://";
		head_len = STRLEN("http://");
	    }
	    strncpy(buf, url_lead, head_len);
	    random_lower_alphanum_str(buf+head_len, rlen-head_len, rlen-head_len);   /* form overall string */
	    pos = biased_random_range(head_len+2,  rlen-4);
	    buf[pos] = '.';						    /* load . */
	    buf[pos+3] = '/';						    /* load / */
	}
	fprint(answerp, "%s\n", buf);

	/*
	 * form Mastodon handle
	 *
	 * 25% of the time, the author will be anonymous.
	 */
	memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
	(void) random();		/* just for j-random fun */
	if (biased_random_range(0, 4) > 0) {				    /* 75% chance of not anonymous */
	    rlen = biased_random_range(RANDOM_MASTODON_MIN_LEN, MAX_MASTODON_LEN+1);
	    random_lower_alphanum_str(buf+1, rlen-1, rlen-1);		    /* form overall string */
	    buf[0] = '@';						    /* 1st @ */
	    pos = biased_random_range(2, rlen-3);
	    buf[pos] = '@';						    /* 2nd @ */
	    buf[rlen-3] = '.';						    /* load . */
	}
	fprint(answerp, "%s\n", buf);

	/*
	 * form random author GitHub account
	 *
	 * 25% of the time, the author will be anonymous.
	 */
	memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
	(void) random();		/* just for j-random fun */
	if (biased_random_range(0, 4) > 0) {				    /* 75% chance of not anonymous */
	    rlen = biased_random_range(2, MAX_GITHUB_LEN+1);		    /* min length is 2: @x */
	    buf[0] = '@';						    /* @ */
	    random_lower_alphanum_str(buf+1, rlen-1, rlen-1);		    /* form user */
	}
	fprint(answerp, "%s\n", buf);

	/*
	 * form random author affiliation
	 *
	 * 25% of the time, the author will be anonymous.
	 */
	memset(buf, 0, BUFSIZ+1);	/* clear buffer and paranoia */
	(void) random();		/* just for j-random fun */
	if (biased_random_range(0, 4) > 0) {				    /* 75% chance of not anonymous */
	    rlen = biased_random_range(RANDOM_AFFILIATION_MIN_LEN, MAX_AFFILIATION_LEN+1);
	    random_posixsafe_words_str(buf, rlen, rlen);		    /* form user */
	}
	fprint(answerp, "%s\n", buf);

	/*
	 * determine if previous winner
	 *
	 * 50% of the time, the author will be a previous IOCCC winner
	 */
	if (biased_random_range(0, 2) > 0) {				    /* 50% chance previous winner */
	    fprstr(answerp, "y\n");
	} else {
	    fprstr(answerp, "n\n");
	}

	/*
	 * form author handle
	 */
	fprint(answerp, "%s\n", handle);
	/* free allocation by default_handle() */
	free(handle);
	handle = NULL; /* paranoia */
    }

    /*
     * end answers file
     */
    fprint(answerp, "%s\n", MKIOCCCENTRY_ANSWERS_EOF);

    /*
     * close up the answers file
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fclose(answerp);
    if (ret != 0) {
	warnp(__func__, "error in fclose to the random answers file");
    }
    return;
}
