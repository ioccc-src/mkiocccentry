/*
 * sanity - perform common IOCCC sanity checks
 *
 * "Because sometimes we're all a little insane, some of us are a lot insane and
 * code is very often very insane." :-)
 */


#if !defined(INCLUDE_SANITY_H)
#    define  INCLUDE_SANITY_H


/*
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * location table
 */
#include "location.h"

/*
 * UTF-8 POSIX map
 */
#include "utf8_posix_map.h"

/*
 * json_ckk - support chkinfo and chkauth services
 */
#include "chk_util.h"


/*
 * function prototypes
 */
extern void ioccc_sanity_chks(void); /* all *_sanity_chks() functions should call this */



#endif /* INCLUDE_SANITY_H */
