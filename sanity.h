/*
 * sanity: the IOCCC source code sanity checker
 *
 * Each IOCCC tool in the mkiocccentry repo should run the sanity_chk()
 * function to verify that certain things are in a sane state. This should be
 * done via each tool's sanity checker function e.g. txzchk_sanity_chk() for
 * txzchk, mkiocccentry_sanity_chk() for mkiocccentry etc.
 *
 * "Because sometimes we're all a little insane, some of us are a lot insane and
 * code is often very insane." :-)
 *
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
 * UTF-8 -> POSIX map
 */
#include "utf8_posix_map.h"

/*
 * JSON specific tables
 */
#include "json.h"

/*
 * function prototypes
 */
extern void ioccc_sanity_chk(); /* all *_sanity_chk() functions should call this */

extern void check_utf8_posix_map(void);
extern void check_location_table(void);
extern void check_common_json_fields_table(void);


#endif /* INCLUDE_SANITY_H */
