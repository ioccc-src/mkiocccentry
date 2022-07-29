/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * chk_validate - validate functions for checking JSON semantics
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


#if !defined(INCLUDE_CHK_VALIDATE_H)
#    define  INCLUDE_CHK_VALIDATE_H


/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "entry_util.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * json_sem - JSON semantics support
 */
#include "json_sem.h"

/*
 * chk_sem_auth - check .author.json semantics
 */
#include "chk_sem_auth.h"

/*
 * chk_sem_info - check .info.json semantics
 */
#include "chk_sem_info.h"


/*
 * str_or_null - report if a JSON JTYPE_MEMBER value is a valid JSON_STRING or a valid JSON_NULL
 *
 * Assuming valid == true, then if is_null == true, then str will be NULL else if is_null == false,
 * then str will be non-NULL.
 *
 * When valid == false, then is_null and str have no meaning.
 */
struct str_or_null {
    bool valid;		/* true == JSON JTYPE_MEMBER value was is valid JSON_STRING or a valid JSON_NULL */
    bool is_null;	/* true ==> JTYPE_MEMBER value is valid JSON_NULL, false ==> is valid JSON_STRING */
    char *str;		/* decoded value string from JSON member or NULL */
};


/*
 * external function declarations
 *
 * Functions not defined in chk_sem_auth.h nor in the chk_sem_info.h include files.
 */


#endif /* INCLUDE_CHK_VALIDATE_H */
