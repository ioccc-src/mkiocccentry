/*
 * cpath - canonicalize paths
 *
 * "... not all those who wander are lost."
 *
 *      -- J.R.R. Tolkien
 *
 * Copyright (c) 1991,2008,2014-2016,2022-2025 by Landon Curt Noll.  All Rights Reserved.
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
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * This code was developed between 1991-2025 by Landon Curt Noll:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 *
 * We gratefully acknowledge the concept contributions, plus the documentation
 * corrections, and other suggestions made by Cody Boone Ferguson:
 *
 *	@xexyl
 *	https://xexyl.net
 *	https://ioccc.xexyl.net
 *
 * for whom the first part of the above quote applies:
 *
 * "All that is gold does not glitter, ..."
 *
 *      -- J.R.R. Tolkien
 */


#if !defined(INCLUDE_CPATH_H)
#    define  INCLUDE_CPATH_H


#include <regex.h>	/* for regexec(3) */


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
  #include "../dbg/c_bool.h"
  #include "../dbg/c_compat.h"
  #include "../dbg/dbg.h"
#else
  #include <c_bool.h>
  #include <c_compat.h>
  #include <dbg.h>
#endif

/*
 * dyn_array - dynamic array facility
 */
#if defined(INTERNAL_INCLUDE)
  #include "../dyn_array/dyn_array.h"
#else
  #include <dyn_array.h>
#endif

/*
 * pr - stdio helper library
 */
#if defined(INTERNAL_INCLUDE)
  #include "../pr/pr.h"
#else
  #include <pr.h>
#endif


/*
 * official cpath version
 */
#define CPATH_VERSION "2.0.0 2025-10-16" /* format: major.minor YYYY-MM-DD */


/*
 * enums
 */

/*
 * for the path sanity functions
 */
enum path_sanity
{
    PATH_ERR_UNSET = -1,                /* error code not set */
    PATH_OK = 0,                        /* path successfully canonicalized */
    PATH_ERR_PATH_IS_NULL,              /* path string is NULL */
    PATH_ERR_PATH_EMPTY,		/* path string is 0 length (empty) */
    PATH_ERR_PATH_TOO_LONG,             /* canonicalized path is too long */
    PATH_ERR_NOT_RELATIVE,              /* canonicalized path starts with / (slash) and rel_only is true */
    PATH_ERR_NAME_TOO_LONG,             /* a canonicalized path component is too long */
    PATH_ERR_PATH_TOO_DEEP,             /* canonicalized path depth is too deep */
    PATH_ERR_NOT_SAFE,			/* a canonicalized path component does not match the safety regex */
    PATH_ERR_DOTDOT_OVER_TOPDIR,	/* '..' (dot-dot) path component moved above topdir */
    PATH_ERR_MALLOC,			/* malloc related failure during path processing (internal error) */
    PATH_ERR_NULL_COMPONENT,		/* component on path stack is NULL (internal error) */
    PATH_ERR_WRONG_LEN,			/* constructed canonical path has the wrong length (internal error) */
};


/*
 * external global variables
 */
extern const char *const pr_version;	    /* library version format: major.minor YYYY-MM-DD */


/*
 * external function declarations
 */
extern char const *path_sanity_name(enum path_sanity sanity);
extern char const *path_sanity_error(enum path_sanity sanity);
extern bool safe_str(char const *str, bool any_case, bool slash_ok);
extern bool safe_path_str(char const *str, bool any_case, bool slash_ok);
extern char *canon_path(char const *orig_path,
			size_t max_path_len, size_t max_filename_len, int_least32_t max_depth,
			enum path_sanity *sanity_p, size_t *len_p, int_least32_t *depth_p,
			bool rel_only, bool lower_case, bool safe_chk, bool dotdot_err,
			const regex_t *restrict preg);

#endif				/* INCLUDE_CPATH_H */
