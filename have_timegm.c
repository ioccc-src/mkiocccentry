/*
 * have_timegm - test strptime, timegm and strftime
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/errno.h>
#include <string.h>

#define MAX_TIMESTAMP_LEN (48)		/* 28 + 20 more padding for locale */
#define FMT "%a %b %d %H:%M:%S %Y UTC"	/* timestamp string conversion format */

static char const * const usage =
    "usage: %s 'time_string'\n"
    "\n"
    "    time_string   example: Thu Mar 17 01:44:38 2022 UTC\n";
char const *program = NULL;	/* our name */


int
main(int argc, char *argv[])
{
    char *time_string = NULL;		/* time string */
    char buf[MAX_TIMESTAMP_LEN+1+1];	/* conversion back to time string */
    struct tm timeptr;			/* time string conversion */
    size_t strftime_ret;	        /* length of strftime() string without the trailing newline */
    time_t time_stamp;			/* broken-out time converted into a timestamp */
    char *ptr = NULL;			/* ptr to first char in buf not converted */

    /*
     * parse args
     */
    program = argv[0];
    if (argc != 2) {
	fprintf(stderr, usage, program);
	exit(1);
    }
    time_string = argv[1];

    /*
     * convert to timeptr
     */
    errno = 0;
    ptr = strptime(time_string, FMT, &timeptr);
    if (ptr == NULL) {
	fprintf(stderr, "strptime(%s, &timeptr) failed: errno: %d (%s)\n",
		time_string, errno, strerror(errno));
	exit(2);
    }
    if (ptr[0] != '\0') {
	fprintf(stderr, "strptime return: 0x%02x != 0x00\n", ptr[0]);
	exit(3);
    }

    /*
     * print time and time string
     */
    printf("seconds (0 - 60)\ttm_sec: %d\n", timeptr.tm_sec);
    printf("minutes (0 - 59)\ttm_min: %d\n", timeptr.tm_min);
    printf("hours (0 - 23)\t\ttm_hour: %d\n", timeptr.tm_hour);
    printf("day of month (1 - 31)\ttm_mday: %d\n", timeptr.tm_mday);
    printf("month of year (0 - 11)\ttm_mon: %d\n", timeptr.tm_mon);
    printf("year - 1900\t\ttm_year: %d\n", timeptr.tm_year);
    printf("day of week (Sunday=0)\ttm_wday: %d\n", timeptr.tm_wday);
    printf("day of year (0 - 365)\ttm_yday: %d\n", timeptr.tm_yday);
    printf("summer time\t\ttm_isdst: %d\n", timeptr.tm_isdst);
#if 0
    printf("TZ name\ttm_zone: %s\n", ((timeptr.tm_zone == NULL) ? "NULL": timeptr.tm_zone));
    printf("sec offset from UTC\ttm_gmtoff: %ld\n", timeptr.tm_gmtoff);
#endif
    time_stamp = timegm(&timeptr);
    printf("timestamp:\t\t%jd\n", time_stamp);

    /*
     * convert timeptr back to time string
     */
    buf[MAX_TIMESTAMP_LEN+1] = '\0';	/* paranoia */
    errno = 0;
    strftime_ret = strftime(buf, MAX_TIMESTAMP_LEN+1, FMT, &timeptr);
    if (strftime_ret == 0) {
	fprintf(stderr, "strftime(%s, %d, %s) failed: errno: %d (%s)\n",
		buf, MAX_TIMESTAMP_LEN+1, FMT, errno, strerror(errno));
	exit(4);
    }

    /*
     * compare original time_string with reconverted time string buffer
     */
    if (strcmp(time_string, buf) != 0) {
	fprintf(stderr, "time_string: <%s> != strftime buf: <%s>\n", time_string, buf);
	exit(5);
    }
    printf("success:\t\t<%s>\n", buf);
    exit(0); /*ooo*/
}
