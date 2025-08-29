/*
 * dbg_example.c - trivial demo
 *
 * This is just a trivial demo for the dbg API.  See the main() function in
 * dbg.c for a more complete example.
 *
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * Expanded in 2025 by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * The dbg library was written by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 */


/*
 * dbg - info, debug, warning, error, and usage message facility
 */

#include "dbg.h"

#define filename "foo.bar"

static long length = 1290;	/* largest integer whose cube fits into 32 bit signed value */

int
main(void)
{
    long i;		/* counter */

    /*
     * We suggest you use getopt(3) and the parse_verbosity(3) function to
     * convert -v verbosity_level on the command line like:
     *
     *	    verbosity_level = parse_verbosity(optarg);
     *	    if (verbosity_level < 0) {
     *		... report a -v optarg parsing error ...
     *	    }
     */
    msg("NOTE: Setting verbosity_level to DBG_MED: %d", DBG_MED);
    verbosity_level = DBG_MED; /* set an initial level of debugging */
    if (is_dbg_enabled) {
	msg("NOTE: debugging has been enabled");
    }

    /*
     * This will print:
     *
     *	    Warning: main: elephant is sky-blue pink
     *
     * with newlines as described.
     */
    msg("NOTE: The next line should say: \"Warning: %s: %s", __func__, "elephant is sky-blue pink\"");
    warn(__func__, "elephant is sky-blue pink\n");

    /* this will not print anything as verbosity_level 3 (DBG_MED) < 5 (DBG_HIGH): */
    dbg(DBG_HIGH, "starting critical section");
    if (! dbg_allowed(DBG_HIGH)) {
	msg("NOTE: verbose debugging level has not been set");
    }

    /*
     * Because verbosity_level == 3 (DBG_MED) and filename is "foo.bar" and
     * length == 7 this will print (with newlines added as described):
     *
     *	    debug[3]: file: foo.bar has length: 7
     */
    msg("NOTE: The next line should read: \"debug[3]: file: %s has length: %ld\"", filename, length);
    dbg(DBG_MED, "file: %s has length: %ld", filename, length);

    /*
     * We can avoid even making debug calls when the debug level is not high enough.
     */
    msg("\nNOTE: about to loop %ld times", length);
    for (i=0; i < length; ++i) {
	if (dbg_allowed(DBG_VVVHIGH)) {
	    /* this call is skipped unless debugging level is at least DBG_VVVHIGH */
	    dbg(DBG_VVVHIGH, "i = %ld, i*i = %ld, i*i*i = %ld", i, i*i, i*i*i);
	}
    }
    verbosity_level = DBG_VVVVVHIGH;	/* raise debugging level to an extreme level */
    msg("\nNOTE: raised debug level to: %d", verbosity_level);
    dbg(DBG_VVVVVHIGH, "i = %ld, i*i = %ld, i*i*i = %ld", i, i*i, i*i*i);

    /*
     * turn off debugging
     */
    dbg_output_allowed = false;
    msg("\nNOTE: all debugging has been disabled");
    if (is_dbg_enabled) {
	warn(__func__, "debugging should have been been disabled!");
    }
    dbg(DBG_LOW, "\nNOTE: this minimal debug message will not print because debugging has been disabled");

    /*
     * If EPERM == 1 then this will print:
     *
     *	    ERROR[2]: main: test: errno[1]: Operation not permitted
     *
     * with newlines as discussed and then exit 2.
     */
    errno = EPERM;
    msg("\nNOTE: The next line should read: \"ERROR[2]: main: test: errno[%d]: %s\"", errno, strerror(errno));
    errp(2, __func__, "test");
    not_reached();

    return 2; /* this return is never reached */
}
