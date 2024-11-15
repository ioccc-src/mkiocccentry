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
 * NOTE: only the repo release version, the jparse tool and the JSON parser
 * versions are here. For the version of the other tools, see their header file.
 */

/*
 * official jparse repo release
 *
 * NOTE: this should match the latest Release string in CHANGES.md
 */
#define JPARSE_REPO_VERSION "2.1.0 2024-11-15"		/* format: major.minor YYYY-MM-DD */

/*
 * official jparse version
 */
#define JPARSE_VERSION "1.2.3 2024-11-13"		/* format: major.minor YYYY-MM-DD */

/*
 * official JSON parser version
 */
#define JPARSE_LIBRARY_VERSION "2.1.0 2024-11-14"	/* library version format: major.minor YYYY-MM-DD */


#endif /* INCLUDE_JPARSE_VERSION_H */
