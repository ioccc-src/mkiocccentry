/*
 * entry_time - utility functions supporting mkiocccentry JSON files related to time.h
 *
 * JSON related functions to support formation of .info.json files
 * and .auth.json files, their related check tools, test code,
 * and string encoding/decoding tools.
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


#if !defined(INCLUDE_ENTRY_TIME_H)
#    define  INCLUDE_ENTRY_TIME_H


/*
 * defines
 */

/*
 * external function declarations
 */
extern bool test_formed_UTC(char const *str);
extern bool conv_timestr_test(char const *str);
extern time_t get_now(void);
extern bool timestr_eq_tstamp(char const *timestr, time_t timestamp);

#endif /* INCLUDE_ENTRY_TIME_H */
