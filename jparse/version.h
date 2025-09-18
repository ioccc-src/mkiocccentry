/*
 * version - official jparse API, parser and utils versions
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
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
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody
 * Boone Ferguson and Landon Curt Noll:
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
#define JPARSE_REPO_VERSION "2.3.3 2025-09-16"		/* format: major.minor YYYY-MM-DD */

/*
 * official jparse version
 */
#define JPARSE_TOOL_VERSION "2.0.2 2025-09-09"		/* format: major.minor YYYY-MM-DD */

/*
 * official JSON parser version
 */
#define JPARSE_LIBRARY_VERSION "2.3.1 2025-06-26"	/* library version format: major.minor YYYY-MM-DD */

/*
 * official utility functions (util.c) version
 */
#define JPARSE_UTILS_VERSION "2.1.0 2025-09-01"         /* format: major.minor YYYY-MM-DD */


#endif /* INCLUDE_JPARSE_VERSION_H */
