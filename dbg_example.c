/*
 * This is just a trivial demo, see the main
 * function in dbg.c for a better example.
 */

#include "dbg.h"

#define filename "foo.bar"
long length = 7;
int main(void)
{

    /*
     * We suggest you use getopt(3) and strtol(3) (cast to an int)
     * to convert -v verbosity_level on the command line.
     */
    verbosity_level = DBG_MED; /* DBG_MED == (3) */

    /*
     * This will print:
     *
     *	    Warning: main: elephant is sky-blue pink
     *
     * with newlines as described.
     */
    warn(__func__, "elephant is sky-blue pink");

    /* this will not print anything as verbosity_level 3 (DBG_MED) < 5 (DBG_HIGH): */
    dbg(DBG_HIGH, "starting critical section");

    /*
     * Because verbosity_level == 3 (DBG_MED) and filename is "foo.bar" and
     * length == 7 this will print (with newlines added as described):
     *
     *	    debug[3]: file: foo.bar has length: 7
     */
    dbg(DBG_MED, "file: %s has length: %ld", filename, length);

    /*
     * If EPERM == 1 then this will print:
     *
     *	    ERROR[2]: main: test: errno[1]: Operation not permitted
     *
     * with newlines as discussed and then exit 2.
     */
    errno = EPERM;
    errp(2, __func__, "test");
}
