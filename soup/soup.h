/*
 * soup - some delicious IOCCC soup recipes :-)
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.
 * All Rights Reserved.
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
 * This tool and the JSON parser were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * The IOCCC soup is a joke between Landon and Cody after Cody misread, being
 * very tired at the time, one word as 'soup', when discussing decreasing the
 * number of files in the top level directory (thus the name of this directory).
 * We might have even left you all some delicious soup somewhere here. :-)
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_SOUP_H)
#    define  INCLUDE_SOUP_H


/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "entry_util.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "../jparse/json_util.h"

/*
 * json_sem - JSON semantics support
 */
#include "../jparse/json_sem.h"

/*
 * chk_sem_auth - check .auth.json semantics
 */
#include "chk_sem_auth.h"

/*
 * chk_sem_info - check .info.json semantics
 */
#include "chk_sem_info.h"

/*
 * chk_validate - validate functions for checking JSON semantics
 */
#include "chk_validate.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * sanity - perform common IOCCC sanity checks
 */
#include "sanity.h"

/*
 * default_handle - translate UTF-8 into POSIX portable filename and + chars
 */
#include "default_handle.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"

/*
 * paths to utilities the IOCCC tools use (including our own tools fnamchk,
 * txzchk, chkentry, etc.)
 */
#define TAR_PATH_0 "/usr/bin/tar"		    /* historic path for tar */
#define TAR_PATH_1 "/bin/tar"			    /* alternate tar path for some systems where /usr/bin/tar != /bin/tar */
#define LS_PATH_0 "/bin/ls"			    /* historic path for ls */
#define LS_PATH_1 "/usr/bin/ls"			    /* alternate ls path for some systems where /bin/ls != /usr/bin/ls */
#define FNAMCHK_PATH_0 "./test_ioccc/fnamchk"	    /* path to fnamchk tool if not installed */
#define FNAMCHK_PATH_1 "/usr/local/bin/fnamchk"	    /* default path to fnamchk tool if installed */
#define TXZCHK_PATH_0 "./txzchk"		    /* path to txzchk tool if not installed */
#define TXZCHK_PATH_1 "/usr/local/bin/txzchk"	    /* default path to txzchk tool if installed */
#define CHKENTRY_PATH_0 "./chkentry"		    /* path to chkentry tool if not installed */
#define CHKENTRY_PATH_1 "/usr/local/bin/chkentry"   /* default path to chkentry tool if installed */
#define JPARSE_PATH_0 "./jparse/jparse"		    /* path to jparse if not installed */
#define JPARSE_PATH_1 "/usr/local/bin/jparse"	    /* default path to jparse tool if installed */
#define MAKE_PATH_0 "/usr/bin/make"                 /* default path to make tool */
#define MAKE_PATH_1 "/bin/make"                     /* in case /usr/bin/make doesn't work */
#define RM_PATH_0 "/bin/rm"                         /* default path to rm tool */
#define RM_PATH_1 "/usr/bin/rm"                     /* in case /bin/rm doesn't work */


/*
 * global variables
 *
 * We put the soup.a version string here because we need to put it somewhere. :-)
 */
extern const char *const soup_version;

#endif /* INCLUDE_SOUP_H */
