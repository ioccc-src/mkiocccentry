/*
 * sanity: the IOCCC source code sanity checker
 *
 * Each IOCCC tool in the mkiocccentry repo should run the ioccc_sanity_chks()
 * function to verify that certain things are in a sane state. This should be
 * done via each tool's sanity checker function e.g. txzchk_sanity_chks() for
 * txzchk, mkiocccentry_sanity_chks() for mkiocccentry etc.
 *
 * "Because sometimes we're all a little insane, some of us are a lot insane and
 * code is very often very insane." :-)
 *
 */

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>


/*
 * sanity - our header file
 *
 * #includes everything we need
 *
 */
#include "sanity.h"

/*
 * ioccc_sanity_chks	-   run sanity checks on specific data that's
 *			    used in several of the IOCCC tools
 *
 * This function does not return if things are not sane.
 */
void
ioccc_sanity_chks(void)
{
    /*
     * Check that the UTF-8 POSIX map is sane: that there are no embedded NULL
     * elements and that the final element is in fact NULL.
     */
    dbg(DBG_MED, "Running sanity checks on UTF-8 POSIX map ...");
    check_utf8_posix_map();
    dbg(DBG_MED, "... all OK.");

    /*
     * Check that the location table is sane: that there are no embedded NULL
     * elements and that the final element is in fact NULL.
     */
    dbg(DBG_MED, "Running sanity checks on location table ...");
    check_location_table();
    dbg(DBG_MED, "... all OK.");

    /*
     * Check that the JSON fields tables are sane: that there are no
     * embedded NULL elements, that the only JSON_NULL type is the final
     * element, that the other elements have valid field types and that the
     * final element is in fact NULL.
     *
     * NOTE: The below function calls dbg() for each table it checks so we don't
     * duplicate those messages here.
     */
    check_json_fields_tables();
}
