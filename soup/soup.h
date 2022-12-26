/*
 * soup - some delicious IOCCC soup recipes :-)
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * The JSON parser was co-developed by:
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
#include "../entry_util.h"

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


#endif /* INCLUDE_SOUP_H */