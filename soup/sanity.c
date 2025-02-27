/*
 * sanity - perform common IOCCC sanity checks
 *
 * "Because sometimes we're all a little insane, some of us are a lot insane and
 * code is very often very insane." :-)
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
 * This support file was co-developed in 2022-2025 by Cody Boone Ferguson and
 * Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#include <stdio.h>

/*
 * sanity - perform common IOCCC sanity checks
 */
#include "sanity.h"


/*
 * global variables
 *
 * We put the soup.a version string here because we need to put it somewhere. :-)
 */
const char *const soup_version = SOUP_VERSION;	/* library version format: major.minor YYYY-MM-DD */


/*
 * ioccc_sanity_chks - perform IOCCC sanity checks
 *
 * Run sanity checks on specific data that's used in several of the IOCCC tools.
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
    check_default_handle_map();
    dbg(DBG_VVHIGH, "... all OK.");

    /*
     * Check that the location table is sane: that there are no embedded NULL
     * elements and that the final element is in fact NULL.
     */
    dbg(DBG_VVHIGH, "Running sanity checks on location table ...");
    check_location_table();
    dbg(DBG_VVHIGH, "... all OK.");
}


/*
 * find_utils - find tar, ls, txzchk and fnamchk, chkentry utilities
 *
 * given:
 *
 *	tar_flag_used	    - true ==> -t tar was used
 *	tar		    - if -t tar was not used and tar != NULL set *tar to to tar path
 *	ls_flag_used	    - true ==> -l ls was used
 *	ls		    - if -l ls was not used and ls != NULL set *ls to ls path
 *	txzchk_flag_used    - true ==> -T flag used
 *	txzchk		    - if -T txzchk was used and txzchk != NULL set *txzchk to path
 *	fnamchk_flag_used   - true ==> if fnamchk flag was used
 *	fnamchk		    - if fnamchk option used and fnamchk ! NULL set *fnamchk to path
 *	chkentry_flag_used  - true ==> -C chkentry was used	    -
 *	chkentry	    - if -C chkentry was used and chkentry != NULL set *chkentry to path
 *	make_flag_used      - true ==> -m make was used
 *	make                - if -m make was used and make != NULL set *make to path
 */
void
find_utils(bool tar_flag_used, char **tar, bool ls_flag_used,
	   char **ls, bool txzchk_flag_used, char **txzchk, bool fnamchk_flag_used, char **fnamchk,
	   bool chkentry_flag_used, char **chkentry, bool make_flag_used, char **make)
{
    /*
     * guess where tar and ls utilities are located
     *
     * If the user did not give a -t, -c and/or -l /path/to/x path, then look at
     * the historic location for the utility.  If the historic location of the utility
     * isn't executable, look for an executable in the alternate location.
     *
     * On some systems where /usr/bin != /bin, the distribution made the mistake of
     * moving historic critical applications, look to see if the alternate path works instead.
     */
    if (tar != NULL && !tar_flag_used && !is_exec(TAR_PATH_0) && is_exec(TAR_PATH_1)) {
	*tar = TAR_PATH_1;
	dbg(DBG_MED, "tar is not in historic location: %s : will use alternate location: %s", TAR_PATH_0, *tar);
    }
    if (ls != NULL && !ls_flag_used && !is_exec(LS_PATH_0) && is_exec(LS_PATH_1)) {
	*ls = LS_PATH_1;
	dbg(DBG_MED, "ls is not in historic location: %s : will use alternate location: %s", LS_PATH_0, *ls);
    }

    /* now do the same for our utilities: txzchk, fnamchk, and chkentry */
    if (txzchk != NULL && !txzchk_flag_used && !is_exec(TXZCHK_PATH_0) && is_exec(TXZCHK_PATH_1)) {
	*txzchk = TXZCHK_PATH_1;
	dbg(DBG_MED, "using default txzchk path: %s", TXZCHK_PATH_1);
    }
    if (fnamchk != NULL && !fnamchk_flag_used && !is_exec(FNAMCHK_PATH_0) && is_exec(FNAMCHK_PATH_1)) {
	*fnamchk = FNAMCHK_PATH_1;
	dbg(DBG_MED, "using default fnamchk path: %s", FNAMCHK_PATH_1);
    }
    if (chkentry != NULL && !chkentry_flag_used && !is_exec(CHKENTRY_PATH_0) && is_exec(CHKENTRY_PATH_1)) {
	*chkentry = CHKENTRY_PATH_1;
	dbg(DBG_MED, "using default chkentry path: %s", CHKENTRY_PATH_1);
    }
    if (make != NULL && !make_flag_used && !is_exec(MAKE_PATH_0) && is_exec(MAKE_PATH_1)) {
	*make = MAKE_PATH_1;
	dbg(DBG_MED, "using default make path: %s", MAKE_PATH_1);
    }


    return;
}
