/*
 * file_util - common utility functions for file operations
 *
 * "Courage is found in unlikely places."
 *
 *      -- J.R.R Tolkien
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <limits.h>
#include <errno.h>
#include <stdarg.h>
#include <fcntl.h>		/* for open(2) */
#include <libgen.h>		/* for basename(3) */

/*
 * dbg - info, debug, warning, error, and usage message facility
 * dyn_array - dynamic array facility
 * pr - stdio helper library
 */
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"
#include "../pr/pr.h"
#include "../cpath/cpath.h"

/*
 * file_util - common utility functions for file operations
 */
#include "file_util.h"

/*
 * util - common utility functions
 */
#include "util.h"

/*
 * jparse/util - common utility functions for the JSON parser and tools
 */
#include "../jparse/util.h"

/*
 * walk - walk directory trees and tar listings
 */
#include "walk.h"


#define PATH_INITIAL_SIZE (16)	/* initially allocate this many pointers */
#define PATH_CHUNK_SIZE (8)	/* grow dynamic array by this many pointers at a time */

/*
 * static functions
 */
static intmax_t array_path_count(struct dyn_array *array);


/*
 * base_name - determine the final portion of a path
 *
 * The basename(3) call is used carefully on a temporary copy of the original fts_path
 * as it may modify its arg.  The basename(3) is may also return a static "/" (slash)
 * or a static "." (dot).  Once the basename is obtained, we duplicate the
 * basename so that the item name (the basename) is an independently malloced string.
 *
 * given:
 *      path    - path to form the basename from
 *
 * returns:
 *      allocated basename
 *
 * NOTE: This functions returns a malloced copy of "." if path is NULL, empty, or basename(3) fails.
 *	 A debug message at DBG_HIGH is issued when this happens.
 *
 * NOTE: It is the caller's responsibility to free the returned string.
 *
 * NOTE: This function will NEVER return NULL.
 */
char *
base_name(char const *path)
{
    char *path_dup;             /* duplicated of path */
    char *bname;                /* basename(3) return */
    char *ret;			/* malloced basename to return */

    /*
     * firewall
     */
    if (path == NULL) {

	/* report NULL path */
	dbg(DBG_HIGH, "%s: path is NULL, returning \".\"", __func__);

	/* basename(NULL) returns . (dot), and so do we */
	ret = str_dup(".");
        return ret;
    }

    /*
     * Temporarily duplicate the path for basename processing in case basename(3) modifies the path.
     */
    errno = 0;			/* pre-clear errno for errp() */
    path_dup = strdup(path);	/* use strdup(3), not str_dup(3), so that we can report when strdup(3) fails */
    if (path_dup == NULL) {

	/* report strdup(3) failure of the path */
	dbg(DBG_HIGH, "%s: strdup(\"%s\") of path failed, returning \".\"", __func__, path);

	/* return malloced . (dot) on strdup(3) of path failure */
	ret = str_dup(".");
        return ret;
    }

    /*
     * obtain the basename
     */
    bname = basename(path_dup);
    if (bname == NULL) {

	/* report basename failure */
	dbg(DBG_HIGH, "%s: basename(\"%s\") failed, returning \".\"", __func__, path_dup);

	/* free storage */
	if (path_dup != NULL) {
	    free(path_dup);
	    path_dup = NULL;
	}

	/* return malloced . (dot) in basename(3) failure */
	ret = str_dup(".");
        return ret;
    }

    /*
     * The basename(3) library call ignores all trailing "/" (slash), however if the
     * path consists entirely of "/" (slash) characters, then a static "/" (slash) is returned.
     * For an empty string, then a static "." (dot) is returned.
     *
     * For these reasons, we duplicate the basename(3) return.
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = strdup(bname);	/* use strdup(3), not str_dup(3), so that we can report when strdup(3) fails */
    if (ret == NULL) {

	/* report strdup(3) failure of the basename */
	dbg(DBG_HIGH, "%s: strdup(\"%s\") failed, returning \".\"", __func__, bname);

	/* free storage */
	if (path_dup != NULL) {
	    free(path_dup);
	    path_dup = NULL;
	}

	/* return malloced . (dot) on strdup(3) of basename failure */
	ret = str_dup(".");
        return ret;
    }

    /*
     * free storage
     */
    if (path_dup != NULL) {
	free(path_dup);
	path_dup = NULL;
    }

    /*
     * return beyond the last /
     */
    dbg(DBG_VVHIGH, "%s: basename(\"%s\") == \"%s\"", __func__, path, ret);
    return ret;
}


/*
 * dir_name - determine the final portion of a canonicalized path
 *
 * The dirname(3) call is used carefully on a temporary copy of the original fts_path
 * as it may modify its arg.  The dirname(3) is may also return a static "/" (slash)
 * or a static "." (dot).  Once the dirname is obtained, we duplicate the
 * dirname so that the item name (the dirname) is an independently malloced string.
 *
 * The dirname(3) call will, for a path such as "//foo//./base//baz//" return "//foo//./base",
 * so we need to use canon_path() to "clean up" and canonicalize the path before
 * calling dirname(3) on it.
 *
 * given:
 *      path    - path to form the dirname from
 *
 * returns:
 *      allocated dirname
 *
 * NOTE: This functions returns a malloced copy of "." if path is NULL, empty, or dirname(3) fails.
 *	 A debug message at DBG_HIGH is issued when this happens.
 *
 * NOTE: If a ".." (dot dot) causes the path to go above the top level, then
 *	 malloced copy of "/" (slash) is returned if the path starts with "/" (slash),
 *	 otherwise malloced copy of "." is returned.
 *
 * NOTE: It is the caller's responsibility to free the returned string.
 *
 * NOTE: This function will NEVER return NULL.
 */
char *
dir_name(char const *path)
{
    char *cpath;		/* malloced canonicalized path */
    enum path_sanity sanity = PATH_ERR_UNSET;	    /* canon_path path_sanity error */
    char *dname;                /* dirname(3) return */
    char *ret;			/* malloced dirname to return */

    /*
     * firewall
     */
    if (path == NULL) {
	dbg(DBG_HIGH, "%s: will return \".\" because path is NULL", __func__);
	ret = str_dup(".");
        return ret;
    }

    /*
     * canonicalize path
     *
     * A side effect of canonicalization process is to obtain a safe duplicate of the path.
     * We do not put any limits on the canonicalization process, nor are we interested in
     * any of the usual stats that canon_path() might otherwise return.
     */
    /* IMPORTANT: canon_path() MUST use a false "lower_case" arg!  See the path_in_item_array() function. */
    cpath = canon_path(path, 0, 0, 0,
		       &sanity, NULL, NULL, false, false, false, false, NULL);
    switch (sanity) {
    case PATH_ERR_UNSET:
    case PATH_ERR_PATH_IS_NULL:
    case PATH_ERR_NOT_RELATIVE:
    case PATH_ERR_NAME_TOO_LONG:
    case PATH_ERR_NOT_SAFE:
    case PATH_ERR_NULL_COMPONENT:
    case PATH_ERR_WRONG_LEN:
	/* this should never happen */
	dbg(DBG_HIGH, "%s: will return \".\" due to unexpected canon_path error: %s (%s) for: %s",
	     __func__, path_sanity_name(sanity), path_sanity_error(sanity), path);
	ret = str_dup(".");
	return ret;

    case PATH_ERR_MALLOC:
	dbg(DBG_HIGH, "%s: will return \".\" due to malloc canon_path error: %s (%s) for: %s",
	     __func__, path_sanity_name(sanity), path_sanity_error(sanity), path);
	ret = str_dup(".");
        return ret;

    case PATH_ERR_PATH_EMPTY:
	dbg(DBG_HIGH, "%s: will return \".\" due empty path",
		      __func__);
	ret = str_dup(".");
	/* returned . for special case */
	return ret;

    case PATH_ERR_DOTDOT_OVER_TOPDIR:
	if (path[0] == '/') {
	    dbg(DBG_HIGH, "%s: will return \"/\" excessive .. in absolute path",
			  __func__);
	    ret = str_dup("/");
	/* returned . for special case */
	} else {
	    dbg(DBG_HIGH, "%s: will return \".\" excessive .. in relative path",
			  __func__);
	    ret = str_dup(".");
	}
	return ret;

    case PATH_OK:
	/* paranoia */
	if (cpath == NULL) {
	    /* this should never happen */
	    dbg(DBG_HIGH, "%s: will return \".\" because cpath is NULL yet canon_path sanity is PATH_OK for: %s",
			  __func__, path);
	    ret = str_dup(".");
	    return ret;
	}
	/* all is well */
	break;

    default:
	/* this should never happen */
	dbg(DBG_HIGH, "%s: will return \".\" due to unknown canon_path error: %s (%s) for: %s",
	     __func__, path_sanity_name(sanity), path_sanity_error(sanity), path);
	ret = str_dup(".");
        return ret;
    }
    /* assertion: cpath is NOT NULL */

    /*
     * obtain the dirname of the canonicalized path
     */
    dname = dirname(cpath);
    if (dname == NULL) {
	dbg(DBG_HIGH, "%s: dirname(\"%s\") failed, returning \".\" for: %s", __func__, cpath, path);
	ret = str_dup(".");
        return ret;
    }

    /*
     * The dirname(3) library call ignores all trailing "/" (slash), however if the
     * path consists entirely of "/" (slash) characters, then a static "/" (slash) is returned.
     * If the cpath contains no "/" (slash) characters, a static "." (dot) is returned.
     * For an empty string, then a static "." (dot) is returned.
     *
     * For these reasons, we duplicate the dirname(3) return.
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = strdup(dname);	/* use strdup(3), not str_dup(3), so that we can report when strdup(3) fails */
    if (ret == NULL) {
	dbg(DBG_HIGH, "%s: strdup(\"%s\") failed, returning \".\" for: %s", __func__, dname, path);
	ret = str_dup(".");
        return ret;
    }

    /*
     * free storage
     */
    if (cpath != NULL) {
	free(cpath);
	cpath = NULL;
    }

    /*
     * return the determine the final portion of the canonicalized path
     */
    dbg(DBG_VVHIGH, "%s: dirname(\"%s\") == \"%s\"", __func__, path, ret);
    return ret;
}


/*
 * exists - if a path exists
 *
 * This function tests if a path exists.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *
 *      true ==> path exists,
 *      false ==> path does not exist
 *
 * This function does not return on NULL pointer.
 */
bool
exists(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(12, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);
    return true;
}


/*
 * is_mode - if a path is a certain mode
 *
 * This function tests if a path exists and if its stat.st_mode is mode.
 *
 * given:
 *      path    - the path to test
 *      mode    - the mode to check
 *
 * returns:
 *      true ==> path exists and its mode is mode
 *      false ==> path does not exist OR is has a different mode
 *
 * NOTE: this checks exact mode except that depending on the file type (e.g.
 * regular file, directory, symlink, socket etc.) the mode will be ORed with a
 * specific value as a workaround in some systems. In other words if you want to
 * check if a file is readable/writable/executable you should use
 * is_read()/is_write()/is_exec(). This function does NOT filter out by
 * user/group/other and it does not check any other bits either. If you need
 * that just use has_mode() instead.
 */
bool
is_mode(char const *path, mode_t mode)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */
    /*
     * firewall
     */
    if (path == NULL) {
	err(13, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * verify the mode
     */
    if (ITEM_PERM(buf.st_mode) != ITEM_PERM(mode)) {
        dbg(DBG_HIGH, "path %s mode %o != %o", path, ITEM_PERM(buf.st_mode), ITEM_PERM(mode));
        return false;
    }
    dbg(DBG_V1_HIGH, "path %s mode is %o", path, ITEM_PERM(mode));

    return true;
}


/*
 * has_mode - if a path has certain modes set (stat.st_mode)
 *
 * This function tests if a path exists and if its stat.st_mode has the mode
 * bits set.
 *
 * given:
 *      path    - the path to test
 *      mode    - the mode to check
 *
 * returns:
 *      true ==> path exists and it has the bits in mode in stat.st_mode
 *      false ==> path does not exist OR does not have the bits in mode in
 *                stat.st_mode
 *
 * NOTE: if you need exact check then use is_mode() instead.
 */
bool
has_mode(char const *path, mode_t mode)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */
    /*
     * firewall
     */
    if (path == NULL) {
	err(14, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    if (buf.st_mode & mode) {
        dbg(DBG_HIGH, "path %s mode %o has %o set: %o & %o == %o", path, buf.st_mode, mode,
            buf.st_mode, mode, buf.st_mode & mode);
        return true;
    }

    dbg(DBG_V1_HIGH, "path %s mode %o does not have %o set: %o & %o == %o", path, buf.st_mode,
            mode, buf.st_mode, mode, buf.st_mode & mode);

    return false;
}


/*
 * is_file - if a path is a file
 *
 * This function tests if a path exists and is a regular file.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a regular file,
 *      false ==> path does not exist OR is not a regular file
 */
bool
is_file(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(15, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if path is a regular file
     */
    if (!S_ISREG(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a regular file", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is a regular file", path);
    return true;
}


/*
 * is_dir - if a path is a directory
 *
 * This function tests if a path exists and is a directory.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a directory,
 *      false ==> path does not exist OR is not a directory
 */
bool
is_dir(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(16, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if path is a regular directory
     */
    if (!S_ISDIR(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a directory", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is a directory", path);
    return true;
}


/*
 * is_symlink - if a path is a symlink
 *
 * This function tests if a path exists and is a symlink.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a symlink,
 *      false ==> path does not exist OR is not a symlink
 */
bool
is_symlink(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(17, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = lstat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if path is a symlink
     */
    if (!S_ISLNK(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a symlink", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is a symlink", path);
    return true;
}


/*
 * is_socket - if a path is a socket
 *
 * This function tests if a path exists and is a socket.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a socket,
 *      false ==> path does not exist OR is not a socket
 */
bool
is_socket(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(18, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if path is a socket
     */
    if (!S_ISSOCK(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a socket", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is a socket", path);
    return true;
}


/*
 * is_chardev - if a path is a character device
 *
 * This function tests if a path exists and is a character device.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a character device,
 *      false ==> path does not exist OR is not a character device
 */
bool
is_chardev(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(19, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if path is a character device
     */
    if (!S_ISCHR(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a character device", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is a character device", path);
    return true;
}


/*
 * is_blockdev - if a path is a block device
 *
 * This function tests if a path exists and is a block device.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a block device,
 *      false ==> path does not exist OR is not a block device
 */
bool
is_blockdev(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(20, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if path is a block device
     */
    if (!S_ISBLK(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a block device", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is a block device", path);
    return true;
}


/*
 * is_fifo - if a path is a FIFO
 *
 * This function tests if a path exists and is a FIFO.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and is a FIFO
 *      false ==> path does not exist OR is not a FIFO
 */
bool
is_fifo(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(21, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if path is a FIFO
     */
    if (!S_ISFIFO(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a FIFO", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is a FIFO", path);
    return true;
}


/*
 * is_exec - if a path is executable or directory is searchable
 *
 * This function tests if a path exists and we have permissions to execute it.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and we have executable access,
 *      false ==> path does not exist OR is not executable OR
 *                we don't have permission to execute it
 */
bool
is_exec(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(22, __func__, "called with NULL path");
	not_reached();
     }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    if (S_ISDIR(buf.st_mode)) {
	dbg(DBG_V1_HIGH, "path is a directory: %s size: %lld", path, (long long)buf.st_size);
    } else {
	dbg(DBG_V1_HIGH, "path is a file: %s size: %lld", path, (long long)buf.st_size);
    }

    /*
     * test if we are allowed to execute or search it
     */
    ret = access(path, X_OK);
    if (ret < 0) {
	if (S_ISDIR(buf.st_mode)) {
	    dbg(DBG_HIGH, "cannot search directory: %s ", path);
	} else {
	    dbg(DBG_HIGH, "is is not executable: %s", path);
	}
	return false;
    }
    if (S_ISDIR(buf.st_mode)) {
	dbg(DBG_V1_HIGH, "path %s is searchable", path);
    } else {
	dbg(DBG_V1_HIGH, "path %s is executable", path);
    }
    return true;
}


/*
 * is_read - if a path is readable
 *
 * This function tests if a path exists and we have permissions to read it.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and we have read access,
 *      false ==> path does not exist OR is not read OR
 *                we don't have permission to read it
 */
bool
is_read(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(23, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, R_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not readable", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is readable", path);
    return true;
}


/*
 * is_write - if a path is writable
 *
 * This function tests if a path exists and we have permissions to write it.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists and we have write access,
 *      false ==> path does not exist OR is not writable OR
 *                we don't have permission to write it
 */
bool
is_write(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(24, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, W_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not writable", path);
	return false;
    }
    dbg(DBG_V1_HIGH, "path %s is writable", path);
    return true;
}


/*
 * filemode - return stat.st_mode of a path
 *
 * This function will, if it is a path that exists, return the permission
 * bits from the stat(2) stat.st_mode.
 *
 * given:
 *      path        - path to check
 *
 * NOTE: this function does not return on a NULL path.
 */
mode_t
filemode(char const *path)
{
    struct stat buf;		/* path status */
    mode_t st_mode = 0;		/* from from stat(2) */
    int ret;			/* return from stat(2) */

    /*
     * firewall
     */
    if (path == NULL) {
	err(25, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return 0;
    }

    /*
     * extract the permission bits
     */
    st_mode = ITEM_PERM(buf.st_mode);
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);
    dbg(DBG_HIGH, "path %s is mode %04o", path, st_mode);
    return st_mode;
}


/*
 * is_open_file_stream - determine if a file stream is open
 *
 * Determine if a file stream that is NOT stdin and NOT associated
 * with a valid terminal type device (tty), is open.
 *
 * If you wish to determine of a stdio stream associated with stdin,
 * or a stdio stream associated with tty (such as stdin, stdout or stderr),
 * then perhaps you want to call is_open_stdio instead:
 *
 *	fd_is_ready(__func__, true, fileno(stream))
 *
 * (MIS)FEATURE: Calling is_open_file_stream(stdin) will always return true.
 *	         Calling is_open_file_stream(tty_stream) on a tty based stream
 *	         will always return true.
 *
 * given:
 *	stream	stream to read if open
 *
 * returns:
 *      true ==> stream is open, or
 *		 stream is stdin, or
 *		 stream is associated with a valid terminal type device (tty)
 *      false ==> stream is NULL or not open
 */
bool
is_open_file_stream(FILE *stream)
{
    long pos = 0;	/* stream position */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return false;
    } else if (stream == stdin || isatty(fileno(stream))) {
	return true;
    }

    /*
     * determine stream position
     *
     * The ftell() function will fail if stream is not open.
     */
    errno = 0;
    pos = ftell(stream);
    if (pos < 0) {
	warnp(__func__, "ftell() returned < 0");
	return false;
    }

    /*
     * we know that stream is non-NULL and open
     */
    return true;
}


/*
 * array_has_path - determine if array has path in it
 *
 * given:
 *
 *      array       - array to search
 *      path        - path to find in array
 *      fts         - struct fts * with parameters
 *      match_case  - true ==> use strcmp(), not strcasecmp()
 *      fn          - true ==> use fnmatch(3)
 *      idx         - if not NULL, set to location in array
 *
 * NOTE: this function will return false on a NULL array, an empty array or a
 * NULL path (it will not return false on an empty path as due to the way the
 * find_path() and find_paths() functions work it is possible to have an empty
 * string in it).
 */
bool
array_has_path(struct dyn_array *array, char *path, bool match_case, bool fn, intmax_t *idx)
{
    intmax_t len = 0;	/* number of strings in the array */
    char *u = NULL;     /* name pointer */
    intmax_t i;

    /*
     * firewall
     */
    if (array == NULL || path == NULL) {
        return false;
    }

    /*
     * make sure if idx != NULL that *idx is < 0
     */
    if (idx != NULL) {
        *idx = -1;
    }

    len = array_path_count(array);
    for (i=0; i < len; ++i) {

	/* get next string pointer */
	u = dyn_array_value(array, char *, i);
	if (u == NULL) {	/* paranoia */
	    err(26, __func__, "found NULL pointer in path name dynamic array element: %ju", i);
	    not_reached();
	}

	/* look for match */
	if ((match_case && !strcmp(path, u)) || (!match_case && !strcasecmp(path, u)) || (fn && !fnmatch(u, path, 0))) {
	    /* str found in array */
            if (idx != NULL) {
                *idx = (intmax_t)i;
            }
            return true;
	}
    }

    /*
     * not found
     */
    return false;
}


/*
 * array_path_count   - return number of paths in an array
 *
 * given:
 *
 *  array   - array to count elements (paths)
 *
 * NOTE: if array is NULL we return 0.
 */
static intmax_t
array_path_count(struct dyn_array *array)
{
    /*
     * firewall
     */
    if (array == NULL) {
        return 0;
    }

    return dyn_array_tell(array);
}


/*
 * file_size - determine the file size
 *
 * Return the file size, or -1 if the file does not exist.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      >= 0 ==> file size,
 *      <0 ==> file does not exist
 */
off_t
file_size(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(27, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s does not exist, stat returned: %s", path, strerror(errno));
	return -1;
    }

    /*
     * return file size
     */
    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);
    return buf.st_size;
}


/*
 * size_if_file - return the size of a regular file
 *
 * This function returns the file size if and only if path is a regular file.
 * In all other cases such as not a file, or path does not exist, or path
 * is not accessible, 0 is returned.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      >= 0 ==> file size,
 *      <0 ==> file does not exist
 */
off_t
size_if_file(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(28, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path
     */
    errno = 0;
    ret = stat(path, &buf);
    if (ret < 0) {
	/* non-existent paths are treated as 0 length */
	dbg(DBG_V1_HIGH, "non-existent path, return path %s size: 0", path);
	return 0;
    }

    /*
     * test for regular file
     */
    if (!S_ISREG(buf.st_mode)) {
	/* non-regular file are treated as 0 length */
	dbg(DBG_V1_HIGH, "not a regular file, return path %s size: 0", path);
	return 0;
    }

    /*
     * return file size
     */
    dbg(DBG_VVHIGH, "regular file path %s size: %lld", path, (long long)buf.st_size);
    return buf.st_size;
}


/*
 * is_empty - determine if the file is empty
 *
 * Return if a file is empty (size 0).
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      true ==> file size <= 0
 *      false ==> file size > 0
 *
 * NOTE: if the file does not exist (ENOENT) we return true. In other words if
 * file_size() returns <= 0 it is true; otherwise it is false.
 */
bool
is_empty(char const *path)
{
    off_t size = -1;

    /*
     * firewall
     */
    if (path == NULL) {
	err(29, __func__, "called with NULL path");
	not_reached();
    }

    size = file_size(path);

    if (size < 0) {
        dbg(DBG_V1_HIGH, "path %s does not exist", path);
        return true;
    } else if (size == 0) {
        dbg(DBG_V1_HIGH, "path %s is empty", path);
        return true;
    }

    /*
     * return not empty
     */
    dbg(DBG_V1_HIGH, "path %s is not empty, size: %lld", path, (long long)size);
    return false;
}


/*
 * resolve_path - resolve an executable under $PATH
 *
 * given:
 *
 *  cmd     -   string to find under $PATH
 *
 * This function does not return on a NULL cmd.
 *
 * If getenv("PATH") returns NULL or an empty string or the command is not found
 * under the path, if the path (cmd) is not a regular executable file, NULL is
 * returned; otherwise the original path is returned, strdup()d.
 *
 * If the command is not a regular executable file and $PATH is NULL or empty
 * then NULL is returned.
 *
 * If it is a relative or absolute path and it is both a regular file and
 * executable then a strdup()d copy of that is returned.
 *
 * If nothing is found in $PATH that results in a regular executable file then
 * NULL is returned.
 *
 * It is the caller's responsibility to free() the returned value.
 *
 * This function will not return on error.
 */
char *
resolve_path(char const *cmd)
{
    char *path = NULL;     /* getenv("PATH") if path == NULL or *path == '\0' */
    char *dup = NULL;   /* to duplicate path or path as strtok_r() modifies the string */
    char *saveptr = NULL; /* for strtok_r() */
    char *p = NULL;   /* to tokenise path */
    char *str = NULL;   /* string to return */

    /*
     * firewall
     */
    if (cmd == NULL) {
        err(30, __func__, "passed NULL cmd");
        not_reached();
    }
    /*
     * check if absolute path or if it starts with ./
     */
    if (*cmd == '/' || (*cmd == '.' && cmd[1] == '/')) {
        if (is_file(cmd) && is_exec(cmd)) {
            /*
             * return a strdup() copy of the path
             */
            errno = 0; /* pre-clear errno for errp() */
            str = strdup(cmd);
            if (str == NULL) {
                errp(31, __func__, "strstr(cmd) returned NULL");
                not_reached();
            }
            return str;
        } else {
            /*
             * no regular executable file here, return NULL
             */
            return NULL;
        }
    }

    /*
     * get PATH variable
     */
    path = getenv("PATH");
    if (path == NULL || *path == '\0') {
        /*
         * if NULL or empty we will just strdup() the original command, unless
         * the command is not a regular executable file
         */
        if (is_file(cmd) && is_exec(cmd)) {
            errno = 0; /* pre-clear errno for errp() */
            str = strdup(cmd);
            if (str == NULL) {
                errp(32, __func__, "strdup(cmd) returned NULL");
                not_reached();
            }
            return str;
        } else {
            return NULL;
        }
    }
    /*
     * now duplicate path to tokenise
     */
    errno = 0; /* pre-clear errno for errp() */
    dup = strdup(path);
    if (dup == NULL) {
        errp(33, __func__, "strdup(path) returned NULL");
        not_reached();
    }

    /*
     * now we can tokenise the path
     */
    for (p = strtok_r(dup, ":", &saveptr); p != NULL; p = strtok_r(NULL, ":", &saveptr)) {
        str = calloc_path(p, cmd);
        if (str == NULL) {
            err(34, __func__, "failed to allocate path: %s/%s", p, cmd);
            not_reached();
        }
        if (is_file(str) && is_exec(str)) {
            /*
             * str should be the command's path
             */
            dbg(DBG_HIGH, "found executable file at: %s", str);
            /*
             * free dup
             */
            if (dup != NULL) {
                free(dup);
                dup = NULL;
            }
            return str;
        }

        if (str != NULL) {
            free(str);
            str = NULL;
        }
    }

    /*
     * free memory
     */
    if (dup != NULL) {
        free(dup);
        dup = NULL;
    }

    /*
     * nothing found
     */
    return NULL;
}


/*
 * copyfile - copy src file (path) to dest file (path)
 *
 * given:
 *      src         - src file path
 *      dest        - dest file path
 *      copy_mode   - true ==> copy st_mode from source file, false ==> set mode in mode
 *      mode        - mode for chmod(2) if copy_mode == false
 *
 * This function does not return on NULL pointers.
 *
 * This function will not return on an allocation error.
 *
 * This function will NOT overwrite an existing file.
 *
 * If the number of bytes written is not the same as the number of bytes read it
 * is an error. If the copied contents is not the same (in the written file) as
 * the source file it is an error.
 *
 * returns:
 *      the number of bytes written to dest file
 */
size_t
copyfile(char const *src, char const *dest, bool copy_mode, mode_t mode)
{
    size_t inbytes = 0;         /* bytes read in from src file */
    size_t outbytes = 0;        /* bytes written to dest file */
    FILE *in_file = NULL;       /* source file stream */
    FILE *out_file = NULL;      /* output file stream */
    void *buf = NULL;           /* src file contents */
    void *copy = NULL;          /* to verify copied buffer is the same */
    int infd = -1;              /* input file file descriptor */
    int outfd = -1;             /* output file file descriptor */
    int ret = -1;               /* libc return value */
    struct stat in_st;          /* to get the permissions of source file */
    struct stat out_st;

    /*
     * firewall
     */
    if (src == NULL) {
        err(35, __func__, "src path is NULL");
        not_reached();
    } else if (*src == '\0') {
        err(36, __func__, "src path is empty string");
        not_reached();
    }

    if (dest == NULL) {
        err(37, __func__, "dest path is NULL");
        not_reached();
    } else if (*dest == '\0') {
        err(38, __func__, "dest path is empty string");
        not_reached();
    }

    /*
     * verify that src file exists
     */
    if (!exists(src)) {
        err(39, __func__, "src file does not exist: %s", src);
        not_reached();
    } else if (!is_file(src)) {
        err(40, __func__, "src file is not a regular file: %s", src);
        not_reached();
    } else if (!is_read(src)) {
        err(41, __func__, "src file is not readable: %s", src);
        not_reached();
    }

    /*
     * verify dest path does NOT exist
     */
    if (exists(dest)) {
        err(42, __func__, "dest file already exists: %s", dest);
        not_reached();
    }

    /*
     * open src file for reading
     */
    errno = 0;      /* pre-clear errno for errp() */
    in_file = fopen(src, "rb");
    if (in_file == NULL) {
        errp(43, __func__, "couldn't open src file %s for reading: %s", src, strerror(errno));
        not_reached();
    }

    /*
     * obtain file descriptor of src file
     */
    errno = 0; /* pre-clear errno for errp() */
    infd = open(src, O_RDONLY|O_CLOEXEC, S_IRWXU);
    if (infd < 0) {
        errp(44, __func__, "failed to obtain file descriptor for %s: %s", src, strerror(errno));
        not_reached();
    }

    /*
     * get stat(2) info of src file
     */
    errno = 0;      /* pre-clear errno for errp() */
    ret = fstat(infd, &in_st);
    if (ret < 0) {
	errp(45, __func__, "failed to get stat info for %s, stat returned: %s", src, strerror(errno));
        not_reached();
    }


    /*
     * ensure inbytes is 0 before we read anything in
     */
    inbytes = 0;
    /*
     * read in entire source file
     */
    buf = read_all(in_file, &inbytes);
    if (buf == NULL) {
        err(46, __func__, "couldn't read in src file: %s", src);
        not_reached();
    }

    dbg(DBG_HIGH, "read %zu bytes from src file %s", inbytes, src);

    /*
     * close the src file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(in_file);
    if (ret < 0) {
	errp(47, __func__, "fclose error for %s: %s", src, strerror(errno));
	not_reached();
    }

    /*
     * open dest file for writing
     */
    errno = 0;          /* pre-clear errno for errp() */
    out_file = fopen(dest, "wb");
    if (out_file == NULL) {
        if (buf != NULL) {
            free(buf);
            buf = NULL;
        }
        errp(48, __func__, "couldn't open dest file %s for writing: %s", dest, strerror(errno));
        not_reached();
    }

    /*
     * obtain file descriptor of dest file
     */
    errno = 0; /* pre-clear errno for errp() */
    outfd = open(dest, O_WRONLY|O_CLOEXEC, S_IRWXU);
    if (outfd < 0) {
        errp(49, __func__, "failed to obtain file descriptor for %s: %s", dest, strerror(errno));
        not_reached();
    }


    /*
     * write buffer into the dest file
     */
    errno = 0;		/* pre-clear errno for warnp() */
    outbytes = fwrite(buf, 1, inbytes, out_file);
    if (outbytes != inbytes) {
        errp(50, __func__, "error: wrote %zu bytes out of expected %zu bytes", outbytes, inbytes);
        not_reached();
    } else {
        dbg(DBG_HIGH, "wrote %zu bytes to dest file %s == %zu read bytes", outbytes, src, inbytes);
    }

    /*
     * close the dest file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(out_file);
    if (ret < 0) {
	errp(51, __func__, "fclose error for %s: %s", dest, strerror(errno));
	not_reached();
    }

    /*
     * we need to open the file for reading, to verify that it's the same as the
     * src file buffer
     */
    errno = 0;          /* pre-clear errno for errp() */
    out_file = fopen(dest, "rb");
    if (out_file == NULL) {
        if (buf != NULL) {
            free(buf);
            buf = NULL;
        }
        err(52, __func__, "couldn't open dest file for reading: %s: %s", dest, strerror(errno));
        not_reached();
    }

    /*
     * now that we have the copied file open for reading, we need to read in the
     * contents, to compare it to the source file's contents
     */

    /*
     * ensure inbytes is 0 before we read anything in
     */
    inbytes = 0;
    /*
     * read in entire source file
     */
    copy = read_all(out_file, &inbytes);
    if (copy == NULL) {
        err(53, __func__, "couldn't read in dest file: %s", dest);
        not_reached();
    }

    dbg(DBG_HIGH, "read %zu bytes from dest file %s", inbytes, src);

    /*
     * close the dest file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(out_file);
    if (ret < 0) {
	errp(54, __func__, "fclose error for %s: %s", dest, strerror(errno));
	not_reached();
    }

    /*
     * first check that the bytes read in is the same as the bytes written
     */
    if (outbytes != inbytes) {
        err(55, __func__, "error: read %zu bytes out of expected %zu bytes", inbytes, outbytes);
        not_reached();
    } else {
        dbg(DBG_HIGH, "read in %zu bytes from dest file %s out of expected %zu bytes from src file %s",
                inbytes, dest, outbytes, src);
    }

    /*
     * we need to check that the buffer read from src file is the same as the
     * buffer from the dest file (copy of src file)
     */
    if (memcmp(copy, buf, inbytes) != 0) {
        err(56, __func__, "copy of src file %s is not the same as the contents of the dest file %s", src, dest);
        not_reached();
    } else {
        dbg(DBG_HIGH, "copy of src file %s is identical to dest file %s", src, dest);
    }

    /*
     * now we need to free the read in data (buf and copy)
     */
    if (buf != NULL) {
        free(buf);
        buf = NULL;
    }
    if (copy != NULL) {
        free(copy);
        copy = NULL;
    }

    /*
     * we now need to deal with the st_mode of the dest file
     */

    /*
     * if copy_mode is true we want to set the mode to the same as original file
     */
    if (copy_mode) {
        errno = 0;      /* pre-clear errno for errp() */
        ret = fchmod(outfd, in_st.st_mode);
        if (ret != 0) {
            errp(57, __func__, "fchmod(2) failed to set source file %s mode %o on %s: %s", src, in_st.st_mode,
                    dest, strerror(errno));
            not_reached();
        }

        /*
         * get stat(2) info of dest file
         */
        errno = 0;      /* pre-clear errno for errp() */
        ret = fstat(outfd, &out_st);
        if (ret != 0) {
            errp(58, __func__, "failed to get stat info for %s, stat returned: %s", dest, strerror(errno));
            not_reached();
        }

        /*
         * we now need to verify that the modes are the same
         */
        if (in_st.st_mode != out_st.st_mode) {
            err(59, __func__, "failed to copy st_mode %o from %s to %s: %o != %o", in_st.st_mode, src, dest, in_st.st_mode,
                    out_st.st_mode);
            not_reached();
        }
    } else {
        /*
         * case: set dest file mode to mode in mode
         */
        errno = 0;      /* pre-clear errno for errp() */
        ret = fchmod(outfd, mode);
        if (ret != 0) {
            errp(60, __func__, "fchmod(2) failed to set requested mode on %s: %s", dest, strerror(errno));
            not_reached();
        }

        /*
         * NOTE: we cannot verify that the mode was set correctly because if one
         * does something like:
         *
         *          S_IRUSR|S_IWUSR
         *
         * then the only way we could verify that the mode is correct is by
         * masking it with
         *
         *          mode & ((S_IRWXU | S_IRWXG | S_IRWXO))
         *
         * but if one were to set other bits it would make this test wrong.
         *
         * Thus we cannot do an extra sanity test.
         */
    }

    /*
     * close descriptor
     */
    errno = 0; /* pre-clear for errp() */
    ret = close(outfd);
    if (ret < 0) {
        errp(61, __func__, "close(outfd) failed: %s", strerror(errno));
        not_reached();
    }

    return outbytes;
}


/*
 * touch - make a file with mode mode
 *
 * This is similar the touch command but allowing one to set the mode and not
 * updating the modified time (do not do anything if it already exists).
 *
 * given:
 *
 *  path    - path to create
 *  mode    - mode to set on file
 *
 * This function does nothing on a NULL path or empty path.
 *
 * This function does not return on error.
 *
 * NOTE: this function is essentially:
 *
 *      open(path, O_CREAT | O_WRONLY, mode);
 *
 * It then closes the FD as it's only meant to be like touch(1) (and we cannot
 * possibly know the modes necessary). And although it's meant to be like
 * touch(1) it will not modify the timestamp as it simply ignores files that
 * exist (whether or not they are a directory or something else entirely).
 */
void
touch(char const *path, mode_t mode)
{
    int fd = -1;

    /*
     * firewall
     */
    if (path == NULL || *path == '\0') {
        return;
    }

    if (exists(path)) {
        /* do nothing if it already exists */
        return;
    }

    errno = 0; /* pre-clear errno for errp() */
    fd = open(path, O_CREAT | O_WRONLY, mode);
    if (fd < 0) {
        errp(62, __func__, "failed to create file: %s", path);
        not_reached();
    }

    dbg(DBG_HIGH, "created file %s with mode %04o", path, mode);

    /*
     * now close the file we created
     */
    errno = 0; /* pre-clear errno for errp() */
    if (close(fd) != 0) {
        errp(63, __func__, "failed to close newly created file: %s", path);
        not_reached();
    }
}


/*
 * touchat - make a file with mode mode at a specific location
 *
 * This is similar the touch command but allowing one to set the mode and not
 * updating the modified time (do not do anything if it already exists). Another
 * difference is one can specify the directory to make it in (by path or FD) (if
 * you do not need this you can use touch() directly).
 *
 * given:
 *
 *  path    - path to create
 *  mode    - mode to set on file
 *  dir     - path to directory to chdir(2) first (if not NULL)
 *  dirfd   - FD to directory to fchdir(2) if dir != NULL
 *
 * NOTE: if dir is NULL and dirfd is < 0 then it will not change directories
 * first.
 *
 * NOTE: this function returns -1 on a NULL path or an empty path.
 *
 * NOTE: this function does not return on error.
 *
 * NOTE: this function uses touch().
 */
void
touchat(char const *path, mode_t mode, char const *dir, int dirfd)
{
    int cwd = -1; /* current working directory to restore after changing */

    /*
     * firewall
     */
    if (path == NULL || *path == '\0') {
        return;
    }

    /*
     * note the current directory so we can restore it later, after we're done
     * with everything
     */
    errno = 0;                  /* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
        errp(64, __func__, "cannot open .");
        not_reached();
    }

    if (dir != NULL) {
        errno = 0; /* pre-clear errno for errp() */
        if (chdir(dir) != 0) {
            if (dirfd >= 0) {
                errno = 0; /* pre-clear errno for errp() */
                if (fchdir(dirfd) != 0) {
                    errp(65, __func__, "both chdir(2) and fchdir(2) failed");
                    not_reached();
                }
            } else {
                errp(66, __func__, "chdir(2) failed");
                not_reached();
            }
        }
    } else if (dirfd >= 0) {
        errno = 0; /* pre-clear errno for errp() */
        if (fchdir(dirfd) != 0) {
            errp(67, __func__, "fchdir(2) failed");
            not_reached();
        }
    }

    /*
     * now that we have changed directories we will use touch()
     */
    touch(path, mode);

    /*
     * NOTE: if touch() failed we won't get here
     */
    /*
     * restore previous directory
     */
    errno = 0; /* pre-clear errno for errp() */
    if (fchdir(cwd) != 0) {
        errp(68, __func__, "failed to fchdir(2) back to original directory");
        not_reached();
    }

    /*
     * now close the directory FD we opened
     */
    errno = 0; /* pre-clear errno for errp() */
    if (close(cwd) != 0) {
        errp(69, __func__, "failed to close(cwd)");
        not_reached();
    }
}


/*
 * mkdirs - create (sub)directories (like mkdir -p) of str with mode mode
 *
 * given:
 *
 *      dirfd       - file descriptor to start at (-1 == current working directory)
 *      str         - path of (sub)directories (like mkdir -p) to make
 *      mode        - mode to set on the directory
 *
 * NOTE: if a path already exists (i.e. EEXIST) then we ignore it, continuing if
 * more needs to be done.
 *
 * NOTE: the mode is only set on directories that are created. This is a safety
 * mechanism in case someone sets (for example) 0777 and a directory already
 * exists (it would make that directory 0777 too). It is the responsibility of
 * the user to pass the right mode just like when using mkdir(2).
 *
 * NOTE: the mkdir(2) call uses mode 0 and then if the directory was created it
 * uses chmod(2) as it's not affected by the umask.
 *
 * NOTE: this function does not return on NULL pointers or errors.
 */
int
mkdirs(int dirfd, const char *str, mode_t mode)
{
    char *p = NULL;         /* first '/' */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;       /* we need to strdup() the string */
    size_t len = 0;         /* length of str */
    int dir = -1;           /* for fchdir() after each directory is created */
    int cwd = -1;           /* to restore directory we started at */

    /*
     * firewall
     */
    if (str == NULL) {
        err(70, __func__, "str (path) is NULL");
        not_reached();
    }

    len = strlen(str);
    if (len <= 0) {
        err(71, __func__, "str (path) is empty");
        not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    dup = strdup(str);
    if (dup == NULL) {
        errp(72, __func__, "duplicating \"%s\" failed", str);
        not_reached();
    }

    dbg(DBG_V3_HIGH, "%s: making directories: \"%s\"", __func__, dup);

    /*
     * note the current directory so we can restore it later, after we're done
     * with everything
     */
    errno = 0;                  /* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
        errp(73, __func__, "cannot open .");
        not_reached();
    }

    /*
     * if dirfd < 0 we will work at the current working directory
     */
    if (dirfd < 0) {
	dirfd = cwd;
    }

    /*
     * switch to the directory to start out with
     */
    errno = 0; /* pre-clear errno for errp() */
    if (fchdir(dirfd) != 0) {
        errp(74, __func__, "failed to change to parent directory");
        not_reached();
    }

    /*
     * start extracting the '/' components
     */
    p = strtok_r(dup, "/", &saveptr);
    if (p == NULL) {
        /*
         * if no '/' then we only have to create one directory
         */
        errno = 0; /* pre-clear errno for errp() */
        if (mkdir(dup, 0) != 0) {
            if (errno != EEXIST) {
                errp(75, __func__, "mkdir() of %s failed with: %s", dup, strerror(errno));
                not_reached();
            } else {
                /*
                 * now set modes
                 */
                errno = 0; /* pre-clear errno for errp */
                if (chmod(dup, mode) != 0) {
                    errp(76, __func__, "chmod(\"%s\", %o) failed", dup, mode);
                    not_reached();
                } else {
                    dbg(DBG_HIGH, "set modes %o on %s", mode, dup);
                }
            }
        } else {
            dbg(DBG_HIGH, "made directory: %s", dup);
            /*
             * now set modes
             */
            errno = 0; /* pre-clear errno for errp */
            if (chmod(dup, mode) != 0) {
                errp(77, __func__, "chmod(\"%s\", %o) failed", dup, mode);
                not_reached();
            } else {
                dbg(DBG_HIGH, "set modes %o on %s", mode, dup);
            }
        }
    } else {
        /*
         * we have at least one '/' so we have more work to do
         */

        /*
         * first create the first component
         */
        errno = 0; /* pre-clear errno for errp() */
        if (mkdir(p, 0) != 0) {
            if (errno != EEXIST) {
                errp(78, __func__, "mkdir() of %s failed with: %s", p, strerror(errno));
                not_reached();
            } else {
                /*
                 * now set modes
                 */
                errno = 0; /* pre-clear errno for errp */
                if (chmod(dup, mode) != 0) {
                    errp(79, __func__, "chmod(\"%s\", %o) failed", dup, mode);
                    not_reached();
                } else {
                    dbg(DBG_HIGH, "set mode %o on %s", mode, dup);
                }
            }
        } else {
            dbg(DBG_HIGH, "made directory: %s", p);
            /*
             * now set modes
             */
            errno = 0; /* pre-clear errno for errp */
            if (chmod(dup, mode) != 0) {
                errp(80, __func__, "chmod(\"%s\", %o) failed", dup, mode);
                not_reached();
            } else {
                dbg(DBG_HIGH, "set mode %o on %s", mode, dup);
            }
        }

        /*
         * change to newly made directory
         */
        errno = 0; /* pre-clear errno for errp() */
        if (chdir(p) != 0) {
            errp(81, __func__, "failed to change to %s", p);
            not_reached();
        }

        /*
         * now we have to extract the next components
         */
        for (p = strtok_r(NULL, "/", &saveptr); p != NULL; p = strtok_r(NULL, "/", &saveptr)) {
            errno = 0; /* pre-clear errno for errp() */
            if (mkdir(p, 0) != 0) {
                if (errno != EEXIST) {
                    errp(82, __func__, "mkdir() of %s failed with: %s", p, strerror(errno));
                    not_reached();
                } else {
                    /*
                     * now set modes
                     */
                    errno = 0; /* pre-clear errno for errp */
                    if (chmod(p, mode) != 0) {
                        errp(83, __func__, "chmod(\"%s\", %o) failed", p, mode);
                        not_reached();
                    } else {
                        dbg(DBG_HIGH, "set mode %o on %s", mode, p);
                    }
                }
            } else {
                /*
                 * now set modes
                 */
                errno = 0; /* pre-clear errno for errp */
                if (chmod(p, mode) != 0) {
                    errp(84, __func__, "chmod(\"%s\", %o) failed", p, mode);
                    not_reached();
                } else {
                    dbg(DBG_HIGH, "set mode %o on %s", mode, p);
                }
            }
            errno = 0; /* pre-clear errno for errp() */
            dir = chdir(p);
            if (dir < 0) {
                errp(85, __func__, "failed to open directory %s", p);
                not_reached();
            }
        }
    }

    /*
     * go back to original directory
     */
    errno = 0; /* pre-clear errno for errp() */
    if (fchdir(cwd) != 0) {
        errp(86, __func__, "failed to change back to previous directory");
        not_reached();
    }

    /*
     * close our file descriptor of the current working directory
     */
    errno = 0; /* pre-clear errno for errp() */
    if (close(cwd) != 0) {
        errp(87, __func__, "failed to close(cwd): %s", strerror(errno));
        not_reached();
    }

    return 0;
}


/*
 * calloc_path - calloc a file path
 *
 * given:
 *	dirname		directory containing file, or NULL ==> file is by itself
 *	filename	path of a file (if dirname == NULL), or path under dirname (if dirname != NULL)
 *
 * returns:
 *	allocated string with the path copied into it
 *
 * NOTE: This function does not return on error.
 * NOTE: This function will not return NULL.
 */
char *
calloc_path(char const *dirname, char const *filename)
{
    int ret = -1;		/* libc return status */
    char *buf = NULL;		/* calloced string to return */
    size_t len;			/* length of path */

    /*
     * firewall
     */
    if (filename == NULL) {
	err(88, __func__, "filename is NULL");
	not_reached();
    }

    /*
     * case: dirname is NULL
     *
     * NULL dirname means path is just filename
     */
    if (dirname == NULL) {
	/*
	 * just return a newly calloced filename
	 */
	errno = 0;		/* pre-clear errno for errp() */
	buf = strdup(filename);
	if (buf == NULL) {
	    errp(89, __func__, "strdup of filename failed: %s", filename);
	    not_reached();
	}

    /*
     * case: dirname is not NULL
     *
     * We need to form: dirname/filename
     */
    } else {
	/*
	 * calloc string
	 */
	len = strlen(dirname) + 1 + strlen(filename) + 1; /* + 1 for NUL */
	buf = calloc(len+2, sizeof(*buf));	/* + 1 for paranoia padding */
	errno = 0;		/* pre-clear errno for errp() */
	if (buf == NULL) {
	    errp(90, __func__, "calloc of %zu bytes failed", len);
	    not_reached();
	}

	/*
	 * paranoia - make sure string is zeroed out
	 */
	buf[len] = '\0';
	buf[len+1] = '\0';
	/*
	 * extra paranoia
	 */
	memset(buf, '\0', len+1);

	/*
	 * form string
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = snprintf(buf, len, "%s/%s", dirname, filename);
	if (ret < 0) {
	    errp(91, __func__, "snprintf returned: %zu < 0", len);
	    not_reached();
	}
    }

    /*
     * return calloc path
     */
    if (buf == NULL) {
	errp(92, __func__, "function attempted to return NULL");
	not_reached();
    }
    return buf;
}


/*
 * file_type_name - determine the name of the mode of the file
 *
 * given:
 *      st_mode  - mode of the file (see stat(2))
 *
 * returns:
 *	read-only string representation of mode of the file
 */
char const *
file_type_name(mode_t st_mode)
{
    char const *str = NULL;

    /*
     * try to determine the mode of the file
     */
    if (ITEM_IS_FILE(st_mode)) {
	str = "file";
    } else if (ITEM_IS_DIR(st_mode)) {
	str = "directory";
    } else if (ITEM_IS_SYMLINK(st_mode)) {
	str = "symlink";
    } else if (ITEM_TYPE(st_mode) == S_IFIFO) {
	str = "fifo";
    } else if (ITEM_TYPE(st_mode) == S_IFCHR) {
	str = "cdev";
    } else if (ITEM_TYPE(st_mode) == S_IFBLK) {
	str = "bdev";
    } else if (ITEM_TYPE(st_mode) == S_IFSOCK) {
	str = "socket";
    } else {
	str = "other";
    }

    return str;
}


/*
 * chdir_save_cwd - change directory and return descriptor of previous current directory
 *
 * Note the current directory (so we can chdir back via restore_cwd()),
 * change to the new directory, and return the descriptor of previous current directory
 *
 * given:
 *	newdir	    - directory to chdir to
 *
 * return:
 *	open descriptor of previous current directory
 *
 * NOTE: This function does not return on error.
 */
int
chdir_save_cwd(char const *newdir)
{
    int cwd;		/* current working directory */

    /*
     * firewall
     */
    if (newdir == NULL) {
	err(93, __func__, "newdir is NULL");
	not_reached();
    }

    /*
     * note the current directory so we can restore it later
     */
    errno = 0;	    /* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
        errp(94, __func__, "cannot open .");
        not_reached();
    }

    /*
     * cd to topdir
     */
    errno = 0; /* pre-clear errno for errp() */
    if (chdir(newdir) != 0) {
        errp(95, __func__, "cannot chdir to: %s", newdir);
        not_reached();
    }

    /*
     * return open descriptor of previous current directory
     */
    return cwd;
}


/*
 * restore_cwd
 *
 * Change to the directory of the descriptor and close that descriptor.
 *
 * given:
 *	prev_cwd	- descriptor of previous current directory
 *
 * NOTE: This function does not return on error.
 */
void
restore_cwd(int prev_cwd)
{
    int ret;	    /* fchdir(2) return */

    /*
     * switch back to the original cwd
     */
    errno = 0;          /* pre-clear errno for errp() */
    ret = fchdir(prev_cwd);
    if (ret < 0) {
        errp(96, __func__, "unable to fchdir(prev_cwd)");
        not_reached();
    }

    /*
     * close the previous descriptor
     */
    errno = 0;          /* pre-clear errno for errp() */
    if (close(prev_cwd) < 0) {
        errp(97, __func__, "unable close descriptor");
        not_reached();
    }
    return;
}
