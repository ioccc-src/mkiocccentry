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
char *chksubmit_paths[] =
{
    CHKSUBMIT_PATH_0,
    CHKSUBMIT_PATH_1,
    CHKSUBMIT_PATH_2,
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
    dbg(DBG_VVHIGH, "Running sanity checks on handle map ...");
    (void) default_handle(NULL);
    dbg(DBG_VVHIGH, "... all OK.");

    /*
     * Check that the location table is sane: that there are no embedded NULL
     * elements and that the final element is in fact NULL.
     */
    dbg(DBG_VVHIGH, "Running sanity checks on location table ...");
    check_location_table();
    dbg(DBG_VVHIGH, "... all OK.");
    return;
}


/*
 * find_utils - find tools used by various tools
 *
 * given:
 *
 *      found_tar           - pointer to bool to set if tar is found
 *	tar		    - if tar != NULL set *tar to to tar path
 *      found_ls            - pointer to bool to set if ls is found
 *	ls		    - if ls != NULL set *ls to ls path
 *      found_txzchk        - pointer to bool to set if txzchk is found
 *	txzchk		    - if txzchk != NULL set *txzchk to path
 *      found_fnamchk       - pointer to bool to set if fnamchk is found
 *	fnamchk		    - if fnamchk ! NULL set *fnamchk to path
 *      found_chkentry      - pointer to bool to set if chkentry is found
 *	chkentry	    - if chkentry != NULL set *chkentry to path
 *	found_chksubmit     - pointer to bool to set if chksubmit found
 *	chksubmit           - if chksubmit != NULL set *chksubmit to path
 *      found_make          - pointer to bool to set if make is found
 *	make                - if make != NULL set *make to path
 *      found_rm            - pointer to bool to set if rm is found
 *	rm                  - if rm != NULL set *rm to path
 *
 * NOTE: if a found_ boolean is NULL we will not check for the tool.
 */
void
find_utils(bool *found_tar, char **tar,
        bool *found_ls, char **ls,
        bool *found_txzchk, char **txzchk,
        bool *found_fnamchk, char **fnamchk,
        bool *found_chkentry, char **chkentry,
        bool *found_chksubmit, char **chksubmit,
        bool *found_make, char **make,
        bool *found_rm, char **rm)
{
    size_t i = 0; /* for iterating through paths arrays */

    /*
     * set up booleans if not NULL
     */
    if (found_tar != NULL) {
        *found_tar = false;
    }
    if (found_ls != NULL) {
        *found_ls = false;
    }
    if (found_txzchk != NULL) {
        *found_txzchk = false;
    }
    if (found_fnamchk != NULL) {
        *found_fnamchk = false;
    }
    if (found_chkentry != NULL) {
        *found_chkentry = false;
    }
    if (found_chksubmit != NULL) {
        *found_chksubmit = false;
    }
    if (found_make != NULL) {
        *found_make = false;
    }
    if (found_rm != NULL) {
        *found_rm = false;
    }

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

    if (found_tar != NULL) {
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
            *found_tar = true;
        }
        if (!*found_tar && tar != NULL) {
            for (i = 0; tar_paths[i] != NULL; ++i) {
                *tar = resolve_path(tar_paths[i]);
                if (*tar != NULL) {
                    *found_tar = true;
                    break;
                }
            }
        }
        if (*found_tar) {
            dbg(DBG_MED, "found tar at: %s", *tar);
        }
    } else if (tar != NULL) {
        *tar = NULL;
    }

    if (found_ls != NULL) {
        if (ls != NULL && *ls != NULL && is_file(*ls) && is_exec(*ls)) {
            /*
             * we have to strdup() it
             */
            errno = 0; /* pre-clear errno for errp */
            *ls = strdup(*ls);
            if (*ls == NULL) {
                errp(56, __func__, "strdup(ls) failed");
                not_reached();
            }
            *found_ls = true;
        }
        if (!*found_ls && ls != NULL) {
            for (i = 0; ls_paths[i] != NULL; ++i) {
                *ls = resolve_path(ls_paths[i]);
                if (*ls != NULL) {
                    *found_ls = true;
                    break;
                }
            }
        }
        if (*found_ls) {
            dbg(DBG_MED, "found ls at: %s", *ls);
        }
    } else if (ls != NULL) {
        *ls = NULL;
    }

    if (found_txzchk != NULL) {
        if (txzchk != NULL && *txzchk != NULL && is_file(*txzchk) && is_exec(*txzchk)) {
            /*
             * we have to strdup() it
             */
            errno = 0; /* pre-clear errno for errp */
            *txzchk = strdup(*txzchk);
            if (*txzchk == NULL) {
                errp(57, __func__, "strdup(txzchk) failed");
                not_reached();
            }
            *found_txzchk = true;
        }
        if (!*found_txzchk && txzchk != NULL) {
            for (i = 0; txzchk_paths[i] != NULL; ++i) {
                /*
                 * try resolving the path
                 */
                *txzchk = resolve_path(txzchk_paths[i]);
                if (*txzchk != NULL) {
                    *found_txzchk = true;
                    break;
                }
            }
        }

        if (*found_txzchk) {
            dbg(DBG_MED, "found txzchk at: %s", *txzchk);
        }
    } else if (txzchk != NULL) {
        *txzchk = NULL;
    }

    if (found_fnamchk != NULL) {
        if (fnamchk != NULL && *fnamchk != NULL && is_file(*fnamchk) && is_exec(*fnamchk)) {
            /*
             * we have to strdup() it
             */
            errno = 0; /* pre-clear errno for errp */
            *fnamchk = strdup(*fnamchk);
            if (*fnamchk == NULL) {
                errp(58, __func__, "strdup(fnamchk)");
                not_reached();
            }
            *found_fnamchk = true;
        }
        if (!*found_fnamchk && fnamchk != NULL) {
            for (i = 0; fnamchk_paths[i] != NULL; ++i) {
                *fnamchk = resolve_path(fnamchk_paths[i]);
                if (*fnamchk != NULL) {
                    *found_fnamchk = true;
                    break;
                }
            }
        }

        if (*found_fnamchk) {
            dbg(DBG_MED, "found fnamchk at: %s", *fnamchk);
        }
    } else if (fnamchk != NULL) {
        *fnamchk = NULL;
    }

    if (found_chkentry != NULL) {
        if (chkentry != NULL && *chkentry != NULL && is_file(*chkentry) && is_exec(*chkentry)) {
            /*
             * we have to strdup() it
             */
            errno = 0; /* pre-clear errno for errp */
            *chkentry = strdup(*chkentry);
            if (*chkentry == NULL) {
                errp(59, __func__, "strdup(chkentry) failed");
                not_reached();
            }
            *found_chkentry = true;
        }
        if (!*found_chkentry && chkentry != NULL) {
            for (i = 0; chkentry_paths[i] != NULL; ++i) {
                /*
                 * try resolving the path
                 */
                *chkentry = resolve_path(chkentry_paths[i]);
                if (*chkentry != NULL) {
                    *found_chkentry = true;
                    break;
                }
            }
        }

        if (*found_chkentry) {
            dbg(DBG_MED, "found chkentry at: %s", *chkentry);
        }
    } else if (chkentry != NULL) {
        *chkentry = NULL;
    }

    if (found_chksubmit != NULL) {
        if (chksubmit != NULL && *chksubmit != NULL && is_file(*chksubmit) && is_exec(*chksubmit)) {
            /*
             * we have to strdup() it
             */
            errno = 0; /* pre-clear errno for errp */
            *chksubmit = strdup(*chksubmit);
            if (*chksubmit == NULL) {
                errp(60, __func__, "strdup(chksubmit) failed");
                not_reached();
            }
            *found_chksubmit = true;
        }
        if (!*found_chksubmit && chksubmit != NULL) {
            for (i = 0; chksubmit_paths[i] != NULL; ++i) {
                /*
                 * try resolving the path
                 */
                *chksubmit = resolve_path(chksubmit_paths[i]);
                if (*chksubmit != NULL) {
                    *found_chksubmit = true;
                    break;
                }
            }
        }

        if (*found_chksubmit) {
            dbg(DBG_MED, "found chksubmit at: %s", *chksubmit);
        }
    } else if (chksubmit != NULL) {
        *chksubmit = NULL;
    }


    if (found_make != NULL) {
        if (make != NULL && *make != NULL && is_file(*make) && is_exec(*make)) {
            /*
             * we have to strdup() it
             */
            errno = 0; /* pre-clear errno for errp */
            *make = strdup(*make);
            if (*make == NULL) {
                errp(61, __func__, "strdup(make) failed");
                not_reached();
            }
            *found_make = true;
        }
        if (!*found_make && make != NULL) {
            for (i = 0; make_paths[i] != NULL; ++i) {
                *make = resolve_path(make_paths[i]);
                if (*make != NULL) {
                    *found_make = true;
                    break;
                }
            }
        }

        if (*found_make) {
            dbg(DBG_MED, "found make at: %s", *make);
        }
    } else if (make != NULL) {
        *make = NULL;
    }

    if (found_rm != NULL) {
        if (rm != NULL && *rm != NULL && is_file(*rm) && is_exec(*rm)) {
            /*
             * we have to strdup() it
             */
            errno = 0; /* pre-clear errno for errp */
            *rm = strdup(*rm);
            if (*rm == NULL) {
                errp(62, __func__, "strdup(rm) failed");
                not_reached();
            }
            *found_rm = true;
        }
        if (!*found_rm && rm != NULL) {
            for (i = 0; rm_paths[i] != NULL; ++i) {
                *rm = resolve_path(rm_paths[i]);
                if (*rm != NULL) {
                    *found_rm = true;
                    break;
                }
            }
        }

        if (*found_rm) {
            dbg(DBG_MED, "found rm at: %s", *rm);
        }
    } else if (rm != NULL) {
        *rm = NULL;
    }
    return;
}
