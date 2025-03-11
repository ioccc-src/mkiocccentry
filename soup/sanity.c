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

char *tar_paths[] =
{
    TAR_PATH_0,
    TAR_PATH_0,
    TAR_PATH_2,
    NULL /* MUST BE LAST!! */
};
char *ls_paths[] =
{
    LS_PATH_0,
    LS_PATH_1,
    LS_PATH_2,
    NULL /* MUST BE LAST!! */
};
char *fnamchk_paths[] =
{
    FNAMCHK_PATH_0,
    FNAMCHK_PATH_1,
    FNAMCHK_PATH_2,
    NULL /* MUST BE LAST!! */
};
char *txzchk_paths[] =
{
    TXZCHK_PATH_0,
    TXZCHK_PATH_1,
    TXZCHK_PATH_2,
    NULL /* MUST BE LAST!! */
};
char *chkentry_paths[] =
{
    CHKENTRY_PATH_0,
    CHKENTRY_PATH_1,
    CHKENTRY_PATH_2,
    NULL /* MUST BE LAST!! */
};
char *make_paths[] =
{
    MAKE_PATH_0,
    MAKE_PATH_1,
    MAKE_PATH_2,
    MAKE_PATH_3,
    NULL /* MUST BE LAST!! */
};
char *rm_paths[] =
{
    RM_PATH_0,
    RM_PATH_1,
    RM_PATH_1,
    NULL /* MUST BE LAST!! */
};

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
 * find_utils - find tools used by various tools
 *
 * given:
 *
 *	tar		    - if tar != NULL set *tar to to tar path
 *	ls		    - if ls != NULL set *ls to ls path
 *	txzchk		    - if txzchk != NULL set *txzchk to path
 *	fnamchk		    - if fnamchk ! NULL set *fnamchk to path
 *	chkentry	    - if chkentry != NULL set *chkentry to path
 *	make                - if make != NULL set *make to path
 *	rm                  - if rm != NULL set *rm to path
 */
void
find_utils(char **tar, char **ls, char **txzchk, char **fnamchk, char **chkentry, char **make, char **rm)
{
    size_t i = 0; /* for iterating through paths arrays */
    bool tar_found = false;
    bool ls_found = false;
    bool txzchk_found = false;
    bool fnamchk_found = false;
    bool chkentry_found = false;
    bool make_found = false;
    bool rm_found = false;

    /*
     * guess where tools are
     *
     * First we will try resolving paths by just name. Then if that fails we'll
     * try traditional locations and then if that fails, try alternate locations
     * until we find a match.
     *
     * On some systems where /usr/bin != /bin, the distribution made the mistake
     * of moving (pre-)historic critical applications, look to see if the
     * alternate path works instead.
     */

    if (tar != NULL && *tar != NULL && is_file(*tar) && is_exec(*tar)) {
        /*
         * we have to strdup() it
         */
        errno = 0; /* pre-clear errno for errp */
        *tar = strdup(*tar);
        if (*tar == NULL) {
            errp(55, __func__, "strdup(tar) failed");
            not_reached();
        }
        tar_found = true;
    }
    if (!tar_found && tar != NULL) {
        for (i = 0; tar_paths[i] != NULL; ++i) {
            if (is_file(tar_paths[i]) && is_exec(tar_paths[i])) {
                /*
                 * we have to strdup() it
                 */
                errno = 0; /* pre-clear errno for errp() */
                *tar = strdup(tar_paths[i]);
                if (*tar == NULL) {
                    errp(56, __func__, "strdup(\"%s\") failed", tar_paths[i]);
                    not_reached();
                }
                tar_found = true;
                break;
            } else {
                /*
                 * try resolving the path
                 */
                *tar = resolve_path(tar_paths[i]);
                if (*tar != NULL) {
                    tar_found = true;
                    break;
                }
            }
        }
    }

    if (tar_found) {
        dbg(DBG_MED, "found tar at: %s", *tar);
    }
    if (ls != NULL && *ls != NULL && is_file(*ls) && is_exec(*ls)) {
        /*
         * we have to strdup() it
         */
        errno = 0; /* pre-clear errno for errp */
        *ls = strdup(*ls);
        if (*ls == NULL) {
            errp(57, __func__, "strdup(ls) failed");
            not_reached();
        }
        ls_found = true;
    }
    if (!ls_found && ls != NULL) {
        for (i = 0; ls_paths[i] != NULL; ++i) {
            if (is_file(ls_paths[i]) && is_exec(ls_paths[i])) {
                /*
                 * we have to strdup() it
                 */
                errno = 0; /* pre-clear errno for errp() */
                *ls = strdup(ls_paths[i]);
                if (*ls == NULL) {
                    errp(58, __func__, "strdup(\"%s\") failed", ls_paths[i]);
                    not_reached();
                }
                ls_found = true;
                break;
            } else {
                /*
                 * try resolving the path
                 */
                *ls = resolve_path(ls_paths[i]);
                if (*ls != NULL) {
                    ls_found = true;
                    break;
                }
            }
        }
    }

    if (ls_found) {
        dbg(DBG_MED, "found ls at: %s", *ls);
    }
    if (txzchk != NULL && *txzchk != NULL && is_file(*txzchk) && is_exec(*txzchk)) {
        /*
         * we have to strdup() it
         */
        errno = 0; /* pre-clear errno for errp */
        *txzchk = strdup(*txzchk);
        if (*txzchk == NULL) {
            errp(59, __func__, "strdup(txzchk) failed");
            not_reached();
        }
        txzchk_found = true;
    }
    if (!txzchk_found && txzchk != NULL) {
        for (i = 0; txzchk_paths[i] != NULL; ++i) {
            if (is_file(txzchk_paths[i]) && is_exec(txzchk_paths[i])) {
                /*
                 * we have to strdup() it
                 */
                errno = 0; /* pre-clear errno for errp() */
                *txzchk = strdup(txzchk_paths[i]);
                if (*txzchk == NULL) {
                    errp(60, __func__, "strdup(\"%s\") failed", txzchk_paths[i]);
                    not_reached();
                }
                txzchk_found = true;
                break;
            } else {
                /*
                 * try resolving the path
                 */
                *txzchk = resolve_path(txzchk_paths[i]);
                if (*txzchk != NULL) {
                    txzchk_found = true;
                    break;
                }
            }
        }
    }

    if (txzchk_found) {
        dbg(DBG_MED, "found txzchk at: %s", *txzchk);
    }
    if (fnamchk != NULL && *fnamchk != NULL && is_file(*fnamchk) && is_exec(*fnamchk)) {
        /*
         * we have to strdup() it
         */
        errno = 0; /* pre-clear errno for errp */
        *fnamchk = strdup(*fnamchk);
        if (*fnamchk == NULL) {
            errp(61, __func__, "strdup(fnamchk)");
            not_reached();
        }
        fnamchk_found = true;
    }
    if (!fnamchk_found && fnamchk != NULL) {
        for (i = 0; fnamchk_paths[i] != NULL; ++i) {
            if (is_file(fnamchk_paths[i]) && is_exec(fnamchk_paths[i])) {
                /*
                 * we have to strdup() it
                 */
                errno = 0; /* pre-clear errno for errp() */
                *fnamchk = strdup(fnamchk_paths[i]);
                if (*fnamchk == NULL) {
                    errp(62, __func__, "strdup(\"%s\") failed", fnamchk_paths[i]);
                    not_reached();
                }
                fnamchk_found = true;
                break;
            } else {
                /*
                 * try resolving the path
                 */
                *fnamchk = resolve_path(fnamchk_paths[i]);
                if (*fnamchk != NULL) {
                    fnamchk_found = true;
                    break;
                }
            }
        }
    }

    if (fnamchk_found) {
        dbg(DBG_MED, "found fnamchk at: %s", *fnamchk);
    }

    if (chkentry != NULL && *chkentry != NULL && is_file(*chkentry) && is_exec(*chkentry)) {
        /*
         * we have to strdup() it
         */
        errno = 0; /* pre-clear errno for errp */
        *chkentry = strdup(*chkentry);
        if (*chkentry == NULL) {
            errp(63, __func__, "strdup(chkentry) failed");
            not_reached();
        }
        chkentry_found = true;
    }
    if (!chkentry_found && chkentry != NULL) {
        for (i = 0; chkentry_paths[i] != NULL; ++i) {
            if (is_file(chkentry_paths[i]) && is_exec(chkentry_paths[i])) {
                /*
                 * we have to strdup() it
                 */
                errno = 0; /* pre-clear errno for errp() */
                *chkentry = strdup(chkentry_paths[i]);
                if (*chkentry == NULL) {
                    errp(64, __func__, "strdup(\"%s\") failed", chkentry_paths[i]);
                    not_reached();
                }
                chkentry_found = true;
                break;
            } else {
                /*
                 * try resolving the path
                 */
                *chkentry = resolve_path(chkentry_paths[i]);
                if (*chkentry != NULL) {
                    chkentry_found = true;
                    break;
                }
            }
        }
    }

    if (chkentry_found) {
        dbg(DBG_MED, "found chkentry at: %s", *chkentry);
    }
    if (make != NULL && *make != NULL && is_file(*make) && is_exec(*make)) {
        /*
         * we have to strdup() it
         */
        errno = 0; /* pre-clear errno for errp */
        *make = strdup(*make);
        if (*make == NULL) {
            errp(65, __func__, "strdup(make) failed");
            not_reached();
        }
        make_found = true;
    }
    if (!make_found && make != NULL) {
        for (i = 0; make_paths[i] != NULL; ++i) {
            if (is_file(make_paths[i]) && is_exec(make_paths[i])) {
                /*
                 * we have to strdup() it
                 */
                errno = 0; /* pre-clear errno for errp() */
                *make = strdup(make_paths[i]);
                if (*make == NULL) {
                    errp(66, __func__, "strdup(\"%s\") failed", make_paths[i]);
                    not_reached();
                }
                make_found = true;
                break;
            } else {
                /*
                 * try resolving the path
                 */
                *make = resolve_path(make_paths[i]);
                if (*make != NULL) {
                    make_found = true;
                    break;
                }
            }
        }
    }

    if (make_found) {
        dbg(DBG_MED, "found make at: %s", *make);
    }

    if (rm != NULL && *rm != NULL && is_file(*rm) && is_exec(*rm)) {
        /*
         * we have to strdup() it
         */
        errno = 0; /* pre-clear errno for errp */
        *rm = strdup(*rm);
        if (*rm == NULL) {
            errp(67, __func__, "strdup(rm) failed");
            not_reached();
        }
        rm_found = true;
    }
    if (!rm_found && rm != NULL) {
        for (i = 0; rm_paths[i] != NULL; ++i) {
            if (is_file(rm_paths[i]) && is_exec(rm_paths[i])) {
                /*
                 * we have to strdup() it
                 */
                errno = 0; /* pre-clear errno for errp() */
                *rm = strdup(rm_paths[i]);
                if (*rm == NULL) {
                    errp(68, __func__, "strdup(\"%s\") failed", rm_paths[i]);
                    not_reached();
                }
                rm_found = true;
                break;
            } else {
                /*
                 * try resolving the path
                 */
                *rm = resolve_path(rm_paths[i]);
                if (*rm != NULL) {
                    rm_found = true;
                    break;
                }
            }
        }
    }

    if (rm_found) {
        dbg(DBG_MED, "found rm at: %s", *rm);
    }
    return;
}
