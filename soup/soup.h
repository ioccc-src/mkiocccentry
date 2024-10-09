/*
 * soup - some delicious IOCCC soup recipes :-)
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file and the JSON parser were both co-developed in 2022
 * by:
 *
 *	@xexyl
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
#define CP_PATH_0 "/bin/cp"			    /* historic path for cp */
#define CP_PATH_1 "/usr/bin/cp"			    /* alternate cp path for some systems where /bin/cp != /usr/bin/cp */
#define LS_PATH_0 "/bin/ls"			    /* historic path for ls */
#define LS_PATH_1 "/usr/bin/ls"			    /* alternate ls path for some systems where /bin/ls != /usr/bin/ls */
#define FNAMCHK_PATH_0 "./test_ioccc/fnamchk"	    /* default path to fnamchk tool */
#define FNAMCHK_PATH_1 "/usr/local/bin/fnamchk"	    /* default path to fnamchk tool if installed */
#define TXZCHK_PATH_0 "./txzchk"		    /* default path to txzchk tool */
#define TXZCHK_PATH_1 "/usr/local/bin/txzchk"	    /* default path to txzchk tool if installed */
#define CHKENTRY_PATH_0 "./chkentry"		    /* default path to chkentry tool */
#define CHKENTRY_PATH_1 "/usr/local/bin/chkentry"   /* default path to chkentry tool if installed */
#define JPARSE_PATH_0 "./jparse/jparse"		    /* default path to jparse */
#define JPARSE_PATH_1 "/usr/local/bin/jparse"	    /* default path to jparse tool if installed */


/*
 * global variables
 *
 * We put the soup.a version string here because we need to put it somewhere. :-)
 */
extern const char *const soup_version;

#endif /* INCLUDE_SOUP_H */
