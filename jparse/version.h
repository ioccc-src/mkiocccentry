/*
 * version - official jparse API and tool versions
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This JSON parser was co-developed in 2022 by:
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

#if !defined(INCLUDE_JPARSE_VERSION_H)
#    define  INCLUDE_JPARSE_VERSION_H

/*
 * NOTE: only the repo release version, the jparse tool version, the jparse
 * utils (util.c, not json_util.c, see below) version and the JSON parser
 * version are here.  For the version of the other tools, see their header file.
 *
 * NOTE: the json_util.c is part of the jparse library version so when that code
 * is updated the JPARSE_LIBRARY_VERSION should be updated.
 */

/*
 * official jparse repo release
 *
 * NOTE: this should match the latest Release string in CHANGES.md
 */
#define JPARSE_REPO_VERSION "2.2.20 2025-02-17"		/* format: major.minor YYYY-MM-DD */

/*
 * official jparse version
 */
#define JPARSE_VERSION "1.2.8 2024-12-31"		/* format: major.minor YYYY-MM-DD */

/*
 * official JSON parser version
 */
#define JPARSE_LIBRARY_VERSION "2.2.7 2025-01-18"	/* library version format: major.minor YYYY-MM-DD */

/*
 * official utility functions (util.c) version
 */
#define JPARSE_UTILS_VERSION "1.0.12 2025-02-17"         /* format: major.minor YYYY-MM-DD */


#endif /* INCLUDE_JPARSE_VERSION_H */
