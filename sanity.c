/*
 * sanity - perform common IOCCC sanity checks
 *
 * "Because sometimes we're all a little insane, some of us are a lot insane and
 * code is very often very insane." :-)
 */

#include <stdio.h>


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
     * elements and that the final element is in fact NULL. It also sets up the
     * length of the source and target strings.
     */
    dbg(DBG_VVHIGH, "Running sanity checks on UTF-8 POSIX map ...");
    check_utf8_posix_map();
    dbg(DBG_VVHIGH, "... all OK.");

    /*
     * Check that the location table is sane: that there are no embedded NULL
     * elements and that the final element is in fact NULL.
     */
    dbg(DBG_VVHIGH, "Running sanity checks on location table ...");
    check_location_table();
    dbg(DBG_VVHIGH, "... all OK.");
}
