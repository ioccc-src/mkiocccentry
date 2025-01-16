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


#if !defined(INCLUDE_RANDOM_ANSWERS_H)
#    define  INCLUDE_RANDOM_ANSWERS_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * location - location/country codes
 */
#include "location.h"

/*
 * default_handle - translate UTF-8 into POSIX portable filename and + chars
 */
#include "default_handle.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "../jparse/json_util.h"


/*
 * definitions
 */
#define NO_SEED (-1)		    /* neither -d nor -s seed used, don't generate / use pseudo-random answers */
#define RANDOM_OFFSET (10320)	    /* 5-digit random number to be subtracted from all seeds before calling srandom(3) */
#define DEFAULT_SEED (21701)	    /* -d is an alias for -s DEFAULT_SEED - a nice Mersenne prime exponent */
#define SEED_MASK (0x7fffffff)	    /* bit mask answer_seed with SEED_MASK before calling srandom(3) */
#define SEED_DECIMAL_DIGITS (10)    /* decimal digits in the maximum seed value */


/*
 * forward declarations
 */
long biased_random_range(long min, long above);
long top_skew_random_range(long min, long above);

char random_alpha(void);
char random_upper_alpha_char(void);
char random_lower_alpha_char(void);

char random_alphanum_char(void);
char random_upper_alphanum_char(void);
char random_lower_alphanum_char(void);

char random_posixsafe_char(void);
char random_upper_posixsafe_char(void);
char random_lower_posixsafe_char(void);

void random_alpha_str(char *str,
		      unsigned int minlen,
		      unsigned int maxlen);
void random_upper_alpha_str(char *str,
			    unsigned int minlen,
			    unsigned int maxlen);
void random_lower_alpha_str(char *str,
			    unsigned int minlen,
			    unsigned int maxlen);

void random_alphanum_str(char *str,
			 unsigned int minlen,
			 unsigned int maxlen);
void random_upper_alphanum_str(char *str,
			       unsigned int minlen,
			       unsigned int maxlen);
void random_lower_alphanum_str(char *str,
			       unsigned int minlen,
			       unsigned int maxlen);

void random_posixsafe_str(char *str,
			  unsigned int minlen,
			  unsigned int maxlen);
void random_upper_posixsafe_str(char *str,
				unsigned int minlen,
				unsigned int maxlen);
void random_lower_posixsafe_str(char *str,
				unsigned int minlen,
				unsigned int maxlen);

void random_words_str(char *str,
		      unsigned int minlen,
		      unsigned int maxlen);
void random_upper_words_str(char *str,
			    unsigned int minlen,
			    unsigned int maxlen);
void random_lower_words_str(char *str,
			    unsigned int minlen,
			    unsigned int maxlen);

void random_posixsafe_words_str(char *str,
				unsigned int minlen,
				unsigned int maxlen);
void random_upper_posixsafe_words_str(char *str,
				      unsigned int minlen,
				      unsigned int maxlen);
void random_lower_posixsafe_words_str(char *str,
				      unsigned int minlen,
				      unsigned int maxlen);

void generate_answers(char const *answers);


#endif /* INCLUDE_RANDOM_ANSWERS_H */
