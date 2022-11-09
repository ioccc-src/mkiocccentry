/*
 * entry_time - utility functions supporting mkiocccentry JSON files related to time.h
 *
 * We isolate the code that needs _XOPEN_SOURCE defined for some systems.
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool and the JSON parser were co-developed by:
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
#include "dbg.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * entry_time - utility functions supporting mkiocccentry JSON files related to time
 */
#include "entry_time.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


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
timestr_eq_tstamp(char const *timestr, time_t timestamp)
{
    struct tm timeptr;			/* formed_UTC converted into broken-out time */
    char *ptr = NULL;			/* ptr to first char in buf not converted */
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
    if (ptr[0] != '\0') {
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
	    /* case: signed time_t */
	    json_dbg(JSON_DBG_HIGH, __func__,
		     "invalid: time string: <%s> %jd != timestamp: %jd",
		     timestr, (intmax_t)timestr_as_time_t, (intmax_t)timestamp);
	}
	return false;
    }
    json_dbg(JSON_DBG_MED, __func__, "time string same time as timestamp");
    return true;
}


/*
 * conv_timestr_test - compare time string with a reconverted time string
 *
 *
 * We convert a time string into a set of time elements, and then convert those time
 * elements back into a time string.  Then we compare the time string passed as an
 * argument with the reconverted time string.
 *
 * given:
 *	str		time string to test
 *
 * returns:
 *	true ==> time string matched reconverted time string
 *	false ==> time string did not match reconverted time string, or NULL pointer, or some internal error
 */
bool
conv_timestr_test(char const *str)
{
    char buf[MAX_TIMESTAMP_LEN+1+1];	/* conversion back to time string */
    struct tm timeptr;			/* formed_UTC converted into broken-out time */
    char *ptr = NULL;			/* ptr to first char in buf not converted */
    size_t strftime_ret;		/* length of strftime() string without the trailing newline */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }

    /*
     * convert into broken-out time
     */
    ptr = strptime(str, "%a %b %d %H:%M:%S %Y UTC", &timeptr);
    if (ptr == NULL) {
	json_dbg(JSON_DBG_MED, __func__,
		 "invalid: strptime cannot convert formed_UTC");
	json_dbg(JSON_DBG_HIGH, __func__,
		 "invalid: strptime failed convert formed_UTC: <%s>", str);
	return false;
    }
    if (ptr[0] != '\0') {
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
    return true;
}


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
     * now return :-)
     */
    return now;
}
