/*
 * soup - some delicious IOCCC soup recipes :-)
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * The JSON parser was co-developed in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


#if !defined(INCLUDE_SOUP_H)
#    define  INCLUDE_SOUP_H


/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "entry_util.h"

/*
 * entry_time - utility functions supporting mkiocccentry JSON files related to time.h
 */
#include "entry_time.h"

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
 * utf8_posix_map - translate UTF-8 into POSIX portable filename and + chars
 */
#include "utf8_posix_map.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "version.h"

/*
 * global variables
 *
 * We put the soup.a version string here because we need to put it somewhere. :-)
 */
extern const char *const soup_version;

#endif /* INCLUDE_SOUP_H */
