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
 * function prototypes
 */
extern void ioccc_sanity_chks(void); /* all *_sanity_chks() functions should call this */
extern void find_utils(bool tar_flag_used, char **tar, bool cp_flag_used, char **cp, bool ls_flag_used, 
	   char **ls, bool txzchk_flag_used, char **txzchk, bool fnamchk_flag_used, char **fnamchk,
	   bool chkentry_flag_used, char **chkentry);



#endif /* INCLUDE_SANITY_H */
