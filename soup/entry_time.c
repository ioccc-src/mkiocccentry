/*
 * entry_time - utility functions supporting mkiocccentry JSON files related to time.h
 *
 * We isolate the code that needs _XOPEN_SOURCE defined for some systems.
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool and the JSON parser were co-developed in 2022 by:
 *
 *	@xexyl
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


#define _GNU_SOURCE
#include <time.h>
#include <sys/time.h>


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * jparse - the parser
 */
#include "../jparse/jparse.h"

/*
 * entry_time - utility functions supporting mkiocccentry JSON files related to time
 */
#include "entry_time.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * get_now - get the time, now, as a timestamp
 */
time_t
get_now(void)
{
    struct timeval tp;			/* gettimeofday time value */
    time_t now = 0;			/* the time now */
    int ret;				/* libc return code */

    /*
     * record the time
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = gettimeofday(&tp, NULL);
    if (ret < 0) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: gettimeofday error");
	return false;
    }
    now = tp.tv_sec;
    if ((time_t)-1 > 0) {
	/* case: unsigned time_t */
	json_dbg(JSON_DBG_HIGH, __func__, "now: %ju",
		 (uintmax_t)now);
	json_dbg(JSON_DBG_HIGH, __func__, "now+MAX_CLOCK_ERROR: %ju",
		 (uintmax_t)(now+MAX_CLOCK_ERROR));
    } else {
	/* case: signed time_t */
	json_dbg(JSON_DBG_HIGH, __func__, "now: %jd",
		 (intmax_t)now);
	json_dbg(JSON_DBG_HIGH, __func__, "now+MAX_CLOCK_ERROR: %jd",
		 (intmax_t)(now+MAX_CLOCK_ERROR));
    }

    /*
     * now return now :-)
     */
    return now;
}
