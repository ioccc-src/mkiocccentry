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
#include <fcntl.h>		/* for open() */

/*
 * dbg - info, debug, warning, error, and usage message facility
 * dyn_array - dynamic array facility
 * pr - stdio helper library
 */
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"
#include "../pr/pr.h"

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
 * report a path canonicalization error - used by canon_path()
 *
 * Set *sanity_p to path_err if sanity_p is non-NULL
 * Set *len_p to path_len if path_len is non-NULL
 * Set *depth_p to deep if path_len is non-NULL
 * Free path if path is non-NULL
 * Free array if array is non-NULL
 */
#define report_canon_err(path_err, sanity_p, len_p, depth_p, path, array) \
    { \
	if ((sanity_p) != NULL) { \
	    *(enum path_sanity *)(sanity_p) = (enum path_sanity)(path_err); \
	} \
	if ((len_p) != NULL) { \
	    *(size_t *)(len_p) = path_len; \
	} \
	if ((depth_p) != NULL) { \
	    *(int32_t *)(depth_p) = deep; \
	} \
	if ((array) != NULL) { \
	    dyn_array_free(array); \
	    (array) = NULL; \
	} \
	if ((path) != NULL) { \
	    free(path); \
	    (path) = NULL; \
	} \
    }


#define PATH_INITIAL_SIZE (16)	/* initially allocate this many pointers */
#define PATH_CHUNK_SIZE (8)	/* grow dynamic array by this many pointers at a time */

/*
 * static functions
 */
static size_t private_strlcat(char *dst, const char *src, size_t dsize);
static enum file_type file_type(char const *path);
static int fts_cmp(const FTSENT **a, const FTSENT **b);
static bool check_fts_info(FTS *fts, FTSENT *ent);


/*
 * private_strlcat - private copy of bounded string concatenation
 *
 * given:
 *	dst	    string destination of concatenation
 *	src	    string to to concatenate onto dst
 *	dsize	    limit dst to dsize-1 characters followed by a NUL byte
 *
 * returns:
 *	total length of the concatenated string
 *
 *	If the return value is >= dstsize, the output string has been truncated.
 *
 * Appends src to string dst of size dsize (unlike strncat, dsize is the
 * full size of dst, not space left).  At most dsize-1 characters
 * will be copied.  Always NUL terminates (unless dsize <= strlen(dst)).
 * Returns strlen(src) + MIN(dsize, strlen(initial dst)).
 * If retval >= dsize, truncation occurred.
 *
 * This function is from:
 *
 *	https://github.com/libressl/openbsd/blob/master/src/lib/libc/string/strlcat.c
 *
 * This function is:
 *
 * Copyright (c) 1998, 2015 Todd C. Miller <millert@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
static size_t
private_strlcat(char *dst, const char *src, size_t dsize)
{
    const char *odst = dst;
    const char *osrc = src;
    size_t n = dsize;
    size_t dlen;

    /* Find the end of dst and adjust bytes left but don't go past end. */
    while (n-- != 0 && *dst != '\0') {
	dst++;
    }
    dlen = dst - odst;
    n = dsize - dlen;

    if (n-- == 0) {
	return(dlen + strlen(src));
    }
    while (*src != '\0') {
	if (n != 0) {
	    *dst++ = *src;
	    n--;
	}
	src++;
    }
    *dst = '\0';

    return(dlen + (src - osrc));	/* count does not include NUL */
}


/*
 * base_name - determine the final portion of a path
 *
 * given:
 *      path    - path to form the basename from
 *
 * returns:
 *      allocated basename
 *
 * This function does not return on error.
 *
 * NOTE: it is the caller's responsibility to free the returned string when it
 * is no longer needed.
 *
 * NOTE: on a NULL or empty string this function returns a copy of the string
 * "." like basename(3).
 */
char *
base_name(char const *path)
{
    size_t len;			/* length of path */
    char *copy;			/* copy of path to work from and maybe return */
    char *ret;			/* allocated string to return */
    char *p;
    size_t i;

    /*
     * firewall
     */
    if (path == NULL || *path == '\0') {
        errno = 0; /* pre-clear errno for errp() */
        copy = strdup(".");
        if (copy == NULL) {
            errp(55, __func__, "strdup(\".\") returned NULL");
            not_reached();
        }
        return copy;
    }

    /*
     * duplicate the path for basename processing
     */
    errno = 0;			/* pre-clear errno for errp() */
    copy = strdup(path);
    if (copy == NULL) {
	errp(56, __func__, "strdup(\"%s\") failed", path);
	not_reached();
    }

    /*
     * case: basename of empty string is an empty string
     */
    len = strlen(copy);
    if (len <= 0) {
	dbg(DBG_VVHIGH, "#0: basename of path: \"%s\" is an empty string", path);
	return copy;
    }

    /*
     * remove any multiple trailing /'s
     */
    for (i = len - 1; i > 0; --i) {
	if (copy[i] == '/') {
	    /* trim the trailing / */
	    copy[i] = '\0';
	} else {
	    /* last character (now) is not / */
	    break;
	}
    }
    /*
     * now copy has no trailing /'s, unless it is just /
     */

    /*
     * case: basename of / is /
     */
    if (strcmp(copy, "/") == 0) {
	/*
	 * path is just /, so return /
	 */
	dbg(DBG_VVHIGH, "#1: basename(\"%s\") == \"/\"", path);
	return copy;
    }

    /*
     * look for the last /
     */
    p = strrchr(copy, '/');
    if (p == NULL) {
	/*
	 * path is just a filename, return that filename
	 */
	dbg(DBG_VVHIGH, "#2: basename(\"%s\") == \"%s\"", path, copy);
	return copy;
    }

    /*
     * duplicate the new string to return
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = strdup(p + 1);
    if (ret == NULL) {
	errp(57, __func__, "strdup(\"%s\") failed", p + 1);
	not_reached();
    }

    /*
     * free storage
     */
    if (copy != NULL) {
	free(copy);
	copy = NULL;
    }

    /*
     * return beyond the last /
     */
    dbg(DBG_VVHIGH, "#3: basename(\"%s\") == \"%s\"", path, ret);
    return ret;
}


/*
 * dir_name - determine the dirname at a specific level
 *
 * given:
 *      path    - path to form the dirname from
 *      level   - level to remove from the name
 *
 * returns:
 *      allocated dirname
 *
 * This function does not return on error.
 *
 * Given the path:
 *
 *      foo/bar/baz/zab
 *
 * the following table applies:
 *
 *      -1      foo
 *      0       foo/bar/baz/zab
 *      1       foo/bar/baz
 *      2       foo/bar
 *      3       foo
 *      4       foo
 *
 * Given the path:
 *
 *      foo
 *
 * the following table applies:
 *
 *      -1      foo
 *      0       foo
 *      1       foo
 *
 * NOTE: It is the caller's responsibility to free the returned string when it
 * is no longer needed.
 *
 * NOTE: if the level > the depth in the path then only the first component is
 * returned. This is done this way as a convenience as a user might not know
 * what the actual path is.
 *
 * If the level < 0 then the first component is returned i.e. if the path is:
 *
 *      "/foo/bar/baz"
 * or
 *      "foo/bar/baz"
 * or
 *      "foo"
 * or
 *      "/foo"
 *
 * and level == -1 we will return: "foo".
 *
 * NOTE: a NULL path, an empty string path or a path with no '/' will return '.'
 * indicating the current working directory, except if level < 0 in which case
 * if it's not an empty string it'll return the string itself.
 *
 * The returned value is always a strdup()d copy so you must free() it when
 * done.
 */
char *
dir_name(char const *path, int level)
{
    size_t len = 0;		/* length of path (which might change at different stages) */
    char *copy = NULL;		/* copy of path to work from and maybe return */
    char *ret = NULL;		/* allocated string to return */
    char *p = NULL;             /* for parsing the string with strchr() and strrchr() */
    char *saveptr = NULL;       /* for strtok_r() when level < 0 */
    int lvl = 0;                /* iterator through levels */
    size_t i = 0;               /* iterator through string */

    /*
     * firewall
     */
    if (path == NULL || *path == '\0' || (strchr(path, '/') == NULL && level >= 0)) {
        /*
         * emulate dirname(3): return string "."
         */
        errno = 0; /* pre-clear errno for errp() */
        copy = strdup(".");
        if (copy == NULL) {
            errp(58, __func__, "failed to strdup(\".\")");
            not_reached();
        }
        return copy;
    }

    /*
     * duplicate the path for dirname processing
     *
     * NOTE: we do this even if it's an empty string because the function
     * returns a dynamically allocated copy of the string in every case (except
     * error) so the user can safely free() the string when they're finished.
     */
    errno = 0;			/* pre-clear errno for errp() */
    copy = strdup(path);
    if (copy == NULL) {
	errp(59, __func__, "strdup(\"%s\") failed", path);
	not_reached();
    }

    /*
     * we need the length of the string now
     */
    len = strlen(copy);
    if (len <= 0) {
        /*
         * case: path string is empty
         */
        err(60, __func__, "#0: dir_name(\"%s\", %d): copied string is empty", path, level);
        not_reached();
    }

    /*
     * remove any trailing /'s
     */
    for (i = len - 1; i > 0; --i) {
	if (copy[i] == '/') {
	    /* trim this trailing / (located at copy[i]) */
	    copy[i] = '\0';
	} else {
	    /* last character (now) is not / */
	    break;
	}
    }

    /*
     * now copy has no trailing /'s, unless it is just /
     */

    /*
     * get length again
     */
    len = strlen(copy);
    if (len <= 0) {
        /*
         * if length <= 0 we will return a copy of the (now) modified string
         */
	dbg(DBG_VVHIGH, "#1: dir_name(\"%s\", %d) is an empty string", path, level);
        /*
         * free copy and strdup(".") like dirname(3)
         */
        if (copy != NULL) {
            free(copy);
            copy = NULL;
        }
        errno = 0; /* pre-clear errno for errp() */
        copy = strdup(".");
        if (copy == NULL) {
            errp(61, __func__, "failed to strdup(\".\")");
            not_reached();
        }
        return copy;
    }

    if (!strcmp(copy, "/") || level == 0) {
	/*
	 * path is just / or level is 0, so return copy of the path (modified or
         * not)
	 */
	dbg(DBG_VVHIGH, "#2: dir_name(\"%s\", %d): %s", path, level, copy);
	return copy;
    }

    /*
     * if we do not have any more '/' then the path is just a filename so we
     * will check this prior to anything else. Thus we will first check for a
     * '/' (we need the last one in our case so we use strrchr(3) and not
     * strchr(3).
     */
    p = strrchr(copy, '/');
    if (p == NULL) {
	/*
	 * path is just a filename, return that filename
	 */
	dbg(DBG_VVHIGH, "#3: dir_name(\"%s\", %d): %s", path, level, copy);
	return copy;
    } else if (level < 0) {
        /*
         * We KNOW here that there is AT LEAST one '/' but if level < 0 we need
         * to return what is BEFORE the first '/'. This is true whether or not
         * the path is an absolute path (i.e.  starts with '/').
         *
         * What this means is that if the path is "/foo/bar/baz" or
         * "foo/bar/baz" we will return "foo".
         *
         * NOTE: although there is no way that there is no '/' left at this
         * point there might be more than one. Since strtok(3)/strtok_r(3) will
         * do exactly this we will use it. However since it modifies the string
         * we will first duplicate it (the copy).
         */
        errno = 0; /* pre-clear errno for errp() */
        ret = strdup(copy);
        if (ret == NULL) {
            errp(62, __func__, "failed to strdup(\"%s\")", copy);
            not_reached();
        }
        p = strtok_r(ret, "/", &saveptr);
        if (p != NULL) {
            /*
             * theoretically we should always get here since we already know at
             * least one '/' still exists
             */

            /*
             * we now need to make sure to free(copy) if not NULL, prior to
             * returning.
             */
            if (copy != NULL) {
                free(copy);
                copy = NULL;
            }
            /*
             * now strdup(p) so we can return it
             */
            errno = 0; /* pre-clear errno for errp() */
            ret = strdup(p);
            if (ret == NULL) {
                errp(63, __func__, "strdup(\"%s\") failed", p);
                not_reached();
            }

            dbg(DBG_V1_HIGH, "#4: dir_name(\"%s\", %d): %s", path, level, ret);
            return ret;
        } else {
            /*
             * free ret if not NULL (and it shouldn't be)
             */
            if (ret != NULL) {
                free(ret);
                ret = NULL;
            }
            dbg(DBG_V1_HIGH, "#5: dir_name(\"%s\", %d): %s", path, level, copy);
            return copy;
        }
    }

    /*
     * if we get here, we have even more work to do: we have to remove 'level'
     * levels from the remaining path string.
     *
     * NOTE: p should NOT be NULL here (in fact *p should be '/' in the copy)
     * but if it is the copy of the path (in copy) is still sane and the while()
     * loop below will not be entered so this is safe.
     */
    lvl = 0;
    while (lvl < level && p != NULL) {
        /*
         * remove any (multiple) trailing /'s
         */
        while (*p == '/') {
            *p-- = '\0';
        }
        /*
         * get (next) last '/'
         */
        p = strrchr(copy, '/');
        /*
         * increase the level so that we only remove up to level directory
         * names
         */
        ++lvl;
    }

    /*
     * duplicate the new string to return
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = strdup(copy);
    if (ret == NULL) {
	errp(64, __func__, "strdup(\"%s\") failed", copy);
	not_reached();
    }

    /*
     * free storage
     */
    if (copy != NULL) {
	free(copy);
	copy = NULL;
    }

    /*
     * return the dirname
     */
    dbg(DBG_VVHIGH, "#6: dir_name(\"%s\", %d): %s", path, level, ret);
    return ret;
}


/*
 * count_comps       - count comp delimited components in a string
 *
 * given:
 *      str             - string to test
 *      comp            - component delimiting character
 *      remove_all      - true ==> remove all trailing delimiter chars,
 *                        false ==> remove all but last trailing delimiter char
 *
 * returns:
 *      0 ==> empty string,
 *      1 ==> comp not found or str is the delimiter itself
 *      > 1 ==> number of components found delimited by comp
 *
 * NOTE: successive components are counted as one (i.e. if comp is '/' then
 * foo///bar will be counted as two and /// will be counted as 1.
 *
 * NOTE: if the delimiting character is '/' and the string is "foo///" then
 * after removing the trailing '/'s the string will be "foo/".
 *
 * NOTE: if the delimiting character is '/' and the string is "foo/" then the
 * count is 1 just as if it was "foo//"; on the other hand, if the string is
 * "foo" without the delimiting character, the count is 0 as there actually is
 * no component!
 */
size_t
count_comps(char const *str, char comp, bool remove_all)
{
    size_t count = 0;       /* number of components */
    char *copy;             /* to simplify counting */
    size_t len;		    /* length of str */
    char *p;
    size_t i;

    /*
     * firewall
     */
    if (str == NULL) {
	err(65, __func__, "called with NULL str");
	not_reached();
    }

    /*
     * duplicate the string for counting, as to simplify this we truncate the
     * string
     */
    errno = 0;			/* pre-clear errno for errp() */
    copy = strdup(str);
    if (copy == NULL) {
	errp(66, __func__, "strdup(\"%s\") failed", str);
	not_reached();
    }

    dbg(DBG_V3_HIGH, "#0: count_comps(\"%s\", %c, \"%s\")", str, comp, booltostr(remove_all));

    /*
     * case: empty string is 0
     */
    len = strlen(copy);
    if (len <= 0) {
	dbg(DBG_VVHIGH, "#1: count_comps(\"%s\", %c, \"%s\"): \"%s\" is an empty string", str, comp, str,
                booltostr(remove_all));
	return 0;
    }

    /*
     * remove any multiple trailing delimiter chars except the last one
     */
    if (remove_all) {
	dbg(DBG_V1_HIGH, "#2: string before removing any multiple trailing delimiter chars '%c's: %s", comp, copy);
        for (i = len - 1; i > 0; --i) {
            if (copy[i] == comp) {
                if (i > 0 && copy[i-1] != comp) {
                    /*
                     * if we get here it means that there are no more successive
                     * delimiting characters so we do not want to remove this one
                     */
                    break;
                } else {
                    /* trim the trailing / */
                    copy[i] = '\0';
                }
            } else {
                /*
                 * no more than one trailing delimiter char exist (now)
                 */
                break;
            }
        }
        dbg(DBG_HIGH, "#4: string after removing trailing '%c's: %s", comp, copy);
    }

    /*
     * now copy has no successive trailing delimiting characters
     */
    len = strlen(copy);

    /*
     * case: length is 0
     */
    if (len <= 0) {
        /*
         * string is empty
         */
	dbg(DBG_V1_HIGH, "#5: count_comps(\"%s\", '%c', \"%s\") == 0", str, comp, booltostr(remove_all));
        if (copy != NULL) {
            free(copy);
            copy = NULL;
        }
        return 0;

    /*
     * case: length is 1
     */
    } else if (len == 1) {
        char tmp = '\0';
        if (copy != NULL) {
            tmp = *copy;
            free(copy);
            copy = NULL;
        }
        if (tmp == comp) {
            /*
             * if len is 1 and the character is the component then there is exactly 1 component.
             *
             * We know this because we have removed all successive component chars
             * at the end of the string.
             */
            dbg(DBG_V1_HIGH, "#6: count_comps(\"%s\", '%c', \"%s\") == 1", str, comp, booltostr(remove_all));
            return 1;
        } else {
            /*
             * if there is only one character and it is not the delimiting
             * character then we have 0 components
             */
            return 0;
        }
    }

    /*
     * if we get here, we have more work to do
     */
    count = 0;

    /*
     * look for the last component char
     */
    p = strrchr(copy, comp);
    if (p == NULL) {

	/*
	 * str does not have the component, return 1
	 */
	dbg(DBG_VVHIGH, "#3: count_comps(\"%s\", %c, \"%s\") == 1", str, comp, booltostr(remove_all));
        return 0;

    } else {
        while (p != NULL) {
            ++count;
            /*
             * we need to remove successive component chars
             */
            while (p >= copy && *p && *p == comp) {
                *p-- = '\0';
            }

            /*
             * get next last component char
             */
            p = strrchr(copy, comp);
        }
    }

    /*
     * free storage
     */
    if (copy != NULL) {
	free(copy);
	copy = NULL;
    }

    /*
     * return the total components
     */
    dbg(DBG_VVHIGH, "#4: count_comps(\"%s\", %c, \"%s\") == %zu", str, comp, booltostr(remove_all), count);
    return count;
}


/*
 * count_dirs
 *
 * Return number of directory components in a path, using count_comps()
 *
 * given:
 *      path    - path to count directory components
 *
 * returns:
 *      number (size_t) of directory components in path
 *
 * NOTE: this function does not return on a NULL path but an empty path will
 * return 0.
 */
size_t
count_dirs(char const *path)
{
    /*
     * firewall
     */
    if (path == NULL) {
        err(67, __func__, "path is NULL");
        not_reached();
    }

    /*
     * return the number of components in the path
     */
    return count_comps(path, '/', false);
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
	err(68, __func__, "called with NULL path");
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
 * file_type     -  determine type of file of path
 *
 * This function determines the type of file if path exists, else it returns an
 * appropriate error.
 *
 * given:
 *      path    - the path to test
 *
 * returns:
 *      an enum file_type:
 *          FILE_TYPE_ERR       some error occurred other than ENOENT (no such file or directory)
 *          FILE_TYPE_ENOENT    file does not exist (errno == ENOENT)
 *          FILE_TYPE_FILE      regular file
 *          FILE_TYPE_DIR       directory
 *          FILE_TYPE_SYMLINK   symlink
 *          FILE_TYPE_SOCK      socket
 *          FILE_TYPE_CHAR      character device
 *          FILE_TYPE_BLOCK     block device
 *          FILE_TYPE_FIFO      FIFO
 *
 * NOTE: this function does not return on a NULL path or if we cannot determine
 * the type of file.
 *
 * NOTE: this function returns FILE_TYPE_ERR for any error on stat(2) (return
 * value != 0) except for errno == ENOENT (no such file or directory) in which
 * case we return FILE_TYPE_ENOENT. We only distinguish this type because this
 * is usually(?) the most common reason for stat(2) to fail (okay this might not
 * be strictly true but it is nonetheless an often enough error message when
 * using stat(2)).
 */
/* XXX - make static after table driven walk code in in place - XXX */
static enum file_type
file_type(char const *path)
{
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(69, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * test for existence of path, first with lstat(2)
     */
    errno = 0;  /* pre-clear errno so we can determine why it failed, if it does */
    if (lstat(path, &buf) != 0) {
        if (errno == ENOENT) {
            return FILE_TYPE_ENOENT; /* no such file or directory */
        } else {
            warnp(__func__, "stat() failed for %s: %s", path, strerror(errno));
            return FILE_TYPE_ERR;   /* some other error */
        }
    }

    /*
     * now we KNOW that the file exists. We still have to check if it's a link,
     * however, before anything else.
     */
    if (S_ISLNK(buf.st_mode)) {
        /*
         * is a symlink
         */
        dbg(DBG_HIGH, "path is a symlink: %s", path);
        return FILE_TYPE_SYMLINK;
    }

    /*
     * now we need to get 'regular' stat information
     */
    errno = 0; /* pre-clear errno to check for non-existence/other conditions */
    if (stat(path, &buf) != 0) {
        if (errno == ENOENT) {
            return FILE_TYPE_ENOENT; /* no such file or directory */
        } else {
            return FILE_TYPE_ERR;
        }
    }
    dbg(DBG_VVHIGH, "path size: %lld: %s", (long long)buf.st_size, path);

    /*
     * determine type of file
     */
    if (S_ISREG(buf.st_mode)) {
	dbg(DBG_HIGH, "path is a regular file: %s", path);
	return FILE_TYPE_FILE;
    } else if (S_ISDIR(buf.st_mode)) {
        dbg(DBG_HIGH, "path is a directory: %s", path);
        return FILE_TYPE_DIR;
    } else if (S_ISSOCK(buf.st_mode)) {
        dbg(DBG_HIGH, "path is a socket: %s", path);
        return FILE_TYPE_SOCK;
    } else if (S_ISCHR(buf.st_mode)) {
        dbg(DBG_HIGH, "path is a character device: %s", path);
        return FILE_TYPE_CHAR;
    } else if (S_ISBLK(buf.st_mode)) {
        dbg(DBG_HIGH, "path is a block device: %s", path);
        return FILE_TYPE_BLOCK;
    } else if (S_ISFIFO(buf.st_mode)) {
        dbg(DBG_HIGH, "path is a FIFO: %s", path);
        return FILE_TYPE_FIFO;
    }
    /*
     * unknown type - shouldn't happen
     */
    err(70, __func__, "path is unknown file type: %s", path);
    not_reached();
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
	err(71, __func__, "called with NULL path");
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

    switch (file_type(path)) {
        case FILE_TYPE_ERR:
            err(72, __func__, "error in obtaining file type of path: %s", path);
            not_reached();
        case FILE_TYPE_ENOENT:
            warn(__func__, "path does not exist: %s", path);
            break;
        case FILE_TYPE_FILE:
            mode |= S_IFREG;
            break;
        case FILE_TYPE_DIR:
            mode |= S_IFDIR;
            break;
        case FILE_TYPE_SYMLINK:
            mode |= S_IFLNK;
            break;
        case FILE_TYPE_SOCK:
            mode |= S_IFSOCK;
            break;
        case FILE_TYPE_CHAR:
            mode |= S_IFCHR;
            break;
        case FILE_TYPE_BLOCK:
            mode |= S_IFBLK;
            break;
        case FILE_TYPE_FIFO:
            mode |= S_IFIFO;
            break;
    }

    if (buf.st_mode != mode) {
        dbg(DBG_HIGH, "path %s mode %o != %o", path, buf.st_mode, mode);
        return false;
    }

    dbg(DBG_V1_HIGH, "path %s mode is %o", path, mode);

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
	err(73, __func__, "called with NULL path");
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
	err(74, __func__, "called with NULL path");
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
	err(75, __func__, "called with NULL path");
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
	err(76, __func__, "called with NULL path");
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
	err(77, __func__, "called with NULL path");
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
	err(78, __func__, "called with NULL path");
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
	err(79, __func__, "called with NULL path");
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
	err(80, __func__, "called with NULL path");
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
	err(81, __func__, "called with NULL path");
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
	err(82, __func__, "called with NULL path");
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
	err(83, __func__, "called with NULL path");
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
 * This function will, if it is a path that exists, return the stat.st_mode,
 * unless printing == true in which case we will mask it with the appropriate
 * macro to make it easier to recognise.
 *
 * given:
 *      path        - path to check
 *      printing    - the user wants the value to be printed
 *
 * NOTE: this function does not return on a NULL path.
 *
 * NOTE: if printing == true the format specifier should be %04o.
 */
mode_t
filemode(char const *path, bool printing)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */
    mode_t st_mode = 0;
    enum file_type type = FILE_TYPE_ERR; /* assume some error */
    /*
     * firewall
     */
    if (path == NULL) {
	err(84, __func__, "called with NULL path");
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
     * we have to do special checks to mask out certain bits depending on if we
     * want to print the mode OR if we want to get the raw bits. If we are not
     * printing we need to mask out certain bits; otherwise we can mask out
     * simply S_IFMT (i.e. st_mode &= ~S_IFMT).
     */
    st_mode = buf.st_mode;
    /*
     * if printing we can just  mask out the single macro.
     */
    if (printing) {
        st_mode &= ~S_IFMT;
    } else {
        /*
         * her we need to get the file type first
         */
        type = file_type(path);
        switch (type) {
            case FILE_TYPE_FILE:
                st_mode &= ~S_IFREG;
                break;
            case  FILE_TYPE_DIR:
                st_mode &= ~S_IFDIR;
                break;
            case FILE_TYPE_SYMLINK:
                st_mode &= ~S_IFLNK;
                break;
            case FILE_TYPE_SOCK:
                st_mode &= ~S_IFSOCK;
                break;
            case FILE_TYPE_CHAR:
                st_mode &= ~S_IFCHR;
                break;
            case FILE_TYPE_BLOCK:
                st_mode &= ~S_IFBLK;
                break;
            case FILE_TYPE_FIFO:
                st_mode &= ~S_IFIFO;
                break;
            default:
                err(85, __func__, "unexpected error in determining file type");
                not_reached();
                break;
        }
    }

    dbg(DBG_V1_HIGH, "path %s size: %lld", path, (long long)buf.st_size);
    dbg(DBG_HIGH, "path %s is mode %o (printing: %s)", path, st_mode, booltostr(printing));
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
 * reset_fts    - clear out a struct fts *
 *
 * Given a struct fts * we will set everything to NULL, false or the defaults.
 * In the case of tree (an FTS *), one must be careful that it's not already a
 * valid stream. Since we cannot rely on checking for it being a valid pointer
 * we will simply set it to NULL.
 *
 * After this function the struct fts will have sane default values. In
 * particular, after this function is called you should assume that in the
 * struct fts * the following variables (with the corresponding type) are set to
 * the value:
 *
 *  variable            type                                        value
 *
 *      tree            (FTS *)                                     NULL
 *      options         int                                         FTS_NOCHDIR
 *      logical         bool                                        false
 *      count           int                                         0
 *      depth           int                                         0
 *      min_depth       int                                         0
 *      max_depth       int                                         0
 *      base            bool                                        false
 *      seedot          bool                                        false
 *      match_case      bool                                        false
 *      ignore          struct dyn_array *                          NULL if free_ignored == true
 *      match           struct dyn_array *                          NULL if free_match == true
 *      cmp             int (const FTSENT **, const FTSENT **)      fts_cmp
 *      fn_ignore_flags int                                         -1
 *      fn_match_flags  int                                         -1
 *      check           bool (FTS *, FTSENT *)                      check_fts_info
 *
 * given:
 *
 *      fts             - pointer to pointer struct fts to clear out (set to default values)
 *      free_ignored    - true ==> free ignored list
 *      free_match      - true ==> free match list
 *
 * NOTE: this function will do nothing on NULL fts.
 *
 * IMPORTANT NOTE: you MUST call this function prior to the first use of the FTS
 * functions. If you do not then not only are the values unpredictable but we
 * cannot safely check for pointers later on. Additionally, you MUST NOT set
 * fts->initialised to true! This is only for this function.This allows us to
 * safely check the pointers.
 *
 * NOTE: if the array ignore != NULL and free_ignored == true we will clear the
 * array out with free_paths_array().
 *
 * NOTE: although this function does help with clearing out the struct, one
 * still has to explicitly set the values they wish after resetting it. This is
 * to prevent the problem that this set up resolved, namely functions with way
 * too many args.
 */
void
reset_fts(struct fts *fts, bool free_ignored, bool free_match)
{
    /*
     * firewall
     */
    if (fts == NULL) {
        err(86, __func__, "fts is NULL");
        not_reached();
    }

    /*
     * reset everything
     */

    /*
     * if we've already called this once and the tree is not NULL, we will close
     * it
     */
    if (fts->initialised && fts->tree != NULL) {
        errno = 0; /* pre-clear errno for errp() */
        if (fts_close(fts->tree) != 0) {
            errp(87, __func__, "fts_close(fts->tree) failed");
            not_reached();
        }
    }
    fts->tree = NULL; /* make sure tree is NULL */
    fts->options = FTS_NOCHDIR; /* important - although we always set this if not set */
    fts->logical = false;
    fts->type = FTS_TYPE_ANY; /* default to any type to not make any assumption */
    fts->count = 0;
    fts->depth = 0;
    fts->min_depth = 0;
    fts->max_depth = 0;
    fts->base = false;
    fts->seedot = false;
    fts->match_case = false;

    /*
     * if this struct has been initialised and it is requested to clear the
     * ignore array we will do so
     */
    if (fts->initialised && free_ignored && fts->ignore != NULL) {
        free_paths_array(&(fts->ignore), false);
        fts->ignore = NULL; /* paranoia */
    } else if (!fts->initialised) {
        fts->ignore = NULL; /* paranoia */
    }
    fts->cmp = fts_cmp;
    fts->fn_ignore_flags = -1;
    /*
     * if this struct has been initialised and it is requested to clear the
     * match array we will do so
     */
    if (fts->initialised && free_match && fts->match != NULL) {
        free_paths_array(&(fts->match), false);
        fts->match = NULL; /* paranoia */
    } else if (!fts->initialised) {
        fts->match = NULL; /* paranoia */
    }
    fts->fn_match_flags = -1;
    fts->check = check_fts_info;

    /*
     * this MUST be last!
     */
    fts->initialised = true;
}


/*
 * fts_path     - determine offset and return ent->fts_path in a sane way
 *
 *  given:
 *
 *      ent        - FTSENT to return the correct fts_path
 *
 * Assuming that (as is done in read_fts() below) the path_argv to fts_open() is
 * ".", determine the offset to get past the './' (or if just that or '.' or
 * '..' don't go beyond those). In other words, if the file is '.' or '..' or
 * just './' - which probably shouldn't happen - we do not want to ent->fts_path
 * + 2 as it would go beyond the element; instead we would just return
 * ent->fts_path.
 *
 * This function does not return if given a dead Ent :-) (a NULL ent)
 *
 * This function does NOT strdup() it! Thus you should NOT use this after
 * closing down the stream (in the case of read_fts() you should not do it after
 * it returns NULL, assuming of course you did the do..while loop).
 *
 * NOTE: if the path is NULL or empty then something funny in the tree is
 * probably happening so we will abort with an error. In other words this
 * function will never return a NULL or empty string.
 */
/* XXX - make static after table driven walk code in in place - XXX */
char *
fts_path(FTSENT *ent)
{
    char *path = NULL;

    /*
     * firewall
     */
    if (ent == NULL) {
        /*
         * Treebeard died :(
         */
        err(88, __func__, "ent is NULL");
        not_reached();
    }
    if (ent->fts_path == NULL || *(ent->fts_path) == '\0') {
        err(89, __func__, "ent->fts_path == NULL or empty string");
        not_reached();
    } else if (ent->fts_path[0] == '\0' || ent->fts_path[1] == '\0' || ent->fts_path[2] == '\0') {
        /*
         * don't do + 2 as it will either go beyond the string's end or it will
         * be an empty string
         */
        path = ent->fts_path;
    } else {
        /*
         * we can do fts_path + 2
         */
        path = ent->fts_path + 2;
    }

    if (path == NULL) { /* should never happen */
        /*
         * Fangorn Forest burnt down :-(
         */
        err(90, __func__, "error: path ended up NULL for FTS entry");
        not_reached();
    } else if (*path == '\0') {
        /*
         * Fangorn Forest burnt down :-(
         */
        err(91, __func__, "error: path ended up as empty string for FTS entry");
        not_reached();
    }

    return path; /* all okay */
}


/*
 * fts_cmp
 *
 * Compare fts_name members in two FTSENT structs in normal order
 * (i.e. like strcmp() does).
 *
 * given:
 *	a	pointer to pointer to first FTSENT to compare
 *	b	pointer to pointer to second FTSENT to compare
 *
 * returns:
 *      -1      a < b
 *      0       a == b
 *      1       a > b
 */
static int
fts_cmp(const FTSENT **a, const FTSENT **b)
{
    int cmp = 0;				/* byte string comparison */

    /*
     * firewall
     */
    if (a == NULL || *a == NULL) {
	err(92, __func__, "a is NULL");
	not_reached();
    }
    if (b == NULL || *b == NULL) {
	err(93, __func__, "b is NULL");
	not_reached();
    }

    /*
     * compare name if not NULL
     */
    if (*(*a)->fts_name != '\0') {
	if (*(*b)->fts_name != '\0') {
	    cmp = strcmp((*a)->fts_name, (*b)->fts_name);
	    if (cmp > 0) {
		return 1;	/* a > b */
	    } else if (cmp < 0) {
		return -1;	/* a < b */
	    }
	} else {
	    return -1;	/* a < NULL b */
	}
    } else if (*(*b)->fts_name != '\0') {
	return 1;	/* NULL a > b */
    }
    /* case: name matches */

    /*
     * entries match
     */
    return 0;
}


/*
 * check_fts_info
 *
 * Default function to check fts_info for errors and problematic fts_info
 * values.
 *
 * given:
 *      ent     - an FTSENT * to check (fts->fts_info) for errors
 *
 *
 * We check for the error conditions here (even though we also check it
 * in read_fts()) as an extra sanity check.
 *
 * NOTE: the default for read_fts() (directly or via from find_path() or
 * find_paths()) if the function pointer check (in the function call) is this
 * function but one may need to override this. We have chosen a good default set
 * but there are cases (like mkiocccentry) where we need to have additional or
 * different checks. The read_fts() does check for error/problematic conditions
 * (FTS_ERR, FTS_DC and FTS_DNR) because these are issues (and if someone
 * overrides it here they would not be handled right) but as an example
 * mkiocccentry needs to check for FTS_DEFAULT. However that is perfectly valid
 * here. So to help out we do check for some conditions in read_fts() but one
 * may add additional checks here.
 *
 * NOTE: a good way to handle FTS_DC (directory causes a cycle in the tree) and
 * FTS_DNR (directory not readable) is to use fts_set(ent, FTS_SKIP) to not
 * descend into the directory. We do this here and also in read_fts() because
 * they are problematic; on the other hand in both places FTS_ERR is checked as
 * it is an error and we should not continue. If it ever occurs that one (for
 * some strange) reason needs FTS_ERR we might consider changing the functions
 * to not check for it (or have an option more likely) but at this time we
 * explicitly check it in both places to be sure it's not an issue. Another
 * error condition is FTS_NSOK because FTS_NOSTAT should NEVER be set (we also
 * check FTS_NS since we need stat(2) info).
 *
 * This function does not return on a NULL (dead :-) ) ent (Ent :-) ).
 *
 * Although not always useful this function will return true if all is good.
 * This allows a user to do something additional that we might not anticipate.
 * For instance if they want to have a function that makes the read_fts() stop
 * traversing if a certain condition (fts_info) is found they can do so since
 * they can return false in that case.
 */
static bool
check_fts_info(FTS *fts, FTSENT *ent)
{
    char *path = NULL;  /* fts path */

    /*
     * firewall
     */
    if (fts == NULL) {
        err(94, __func__, "fts is NULL");
        not_reached();
    }
    if (ent == NULL) {
        /*
         * Treebeard died :(
         */
        err(95, __func__, "ent is NULL");
        not_reached();
    }
    path = fts_path(ent);
    if (path == NULL || *path == '\0') {
        /*
         * Fangorn Forest burnt down :-(
         *
         * This should never happen but if it does it is an error
         */
        err(96, __func__, "path is NULL or empty string");
        not_reached();
    }

    switch (ent->fts_info) {
        case FTS_DC: /* cycle in directory tree */
            errno = 0; /* pre-clear errno for errp() */
            if (fts_set(fts, ent, FTS_SKIP) != 0) {
                errp(97, __func__, "failed to set FTS_SKIP on a directory that causes a cycle in the tree: %s", path);
                not_reached();
            } else {
                warn(__func__, "skipping directory %s because it causes a cycle in the tree", path);
            }
            return false;
            break;
        case FTS_DNR: /* directory not readable */
            errno = 0; /* pre-clear errno for errp() */
            if (fts_set(fts, ent, FTS_SKIP) != 0) {
                errp(98, __func__, "failed to set FTS_SKIP on an unreadable directory in the tree: %s", path);
                not_reached();
            } else {
                warn(__func__, "skipping unreadable directory %s in the tree", path);
            }
            return false;
            break;
        case FTS_NS: /* no stat(2) info available but we requested it */
            /*
             * fake errno
             */
            errno = ent->fts_errno;
            errp(99, __func__, "no stat(2) info available for %s in tree", path);
            not_reached();
            break;
        case FTS_NSOK: /* stat(2) not requested */
            err(100, __func__, "stat(2) info not requested for %s in tree: FTS_NOSTAT set!", path);
            not_reached();
            break;
        case FTS_ERR: /* some error condition */
            /*
             * fake errno
             */
            errno = ent->fts_errno; /* pre-clear errno for errp() */
            errp(101, __func__, "encountered error from path %s in tree", path);
            not_reached();
            break;
        default:
            /* if the caller needs to check other conditions they can do
             * that themselves in their check function.
             */
            break;
    }

    return true;
}


/*
 * read_fts - read first (or next) entry in a directory tree (via fts(3))
 *
 * Open an FTS * and read the first FTSENT * or, if fts->tree != NULL, read the
 * next FTSENT in the tree, and return it (unless specific errors occur). If the
 * next entry is NULL (i.e. we have finished traversing the tree), NULL is
 * returned (after closing fts->tree and setting it to NULL).
 *
 * This function will not return on error. If dir == NULL then we will scan from
 * the current working directory OR if dirfd >= 0 we will try and scan from there
 * (fchdir(dirfd)); otherwise the dir is where to chdir(2) prior to opening the
 * tree with fts_open(). If chdir(dir) fails it is an error, unless dirfd >= 0
 * and fchdir(dirfd) does not fail.
 *
 * The function will read from the '.' AFTER the chdir(2)/fchdir(2) succeeds (we
 * will not chdir(dir) if *fts != NULL, however, so that this function can be
 * called again until everything is processed. If you need to find a specific
 * file it is better to use find_path(); and if you need to find a number of
 * paths you should use find_paths() instead. These functions (via this one,
 * read_fts()) allows ignoring certain paths, by basename or not, by
 * case-sensitive or not and various other things. For details see those
 * functions and the details below for the fts struct.
 *
 * If fts is NULL it is an error but if fts->tree is NULL we will open the
 * stream (fts_open()) and set fts->tree to the FTS * returned by fts_open() (if
 * it fails it is an error). Otherwise, if fts->tree != NULL we will read from
 * THAT FTS *. If fts->tree IS NULL we will first open the stream from
 * fts_open() (assigning it to fts->tree) and THEN if not a NULL pointer (i.e.
 * no error encountered) we will read the first entry of the tree. The next call
 * will read the next, or if it's skipped for some reason (due to certain
 * conditions, the struct fts asks for it to be ignored or skipped etc.), it
 * will read the one after that, going until no more remain (at which point it
 * would close the stream, set it to NULL and then return NULL).
 *
 * The intended purpose of this function is to use it in a do..while loop but as
 * noted above if you need to find a specific path or paths there are better
 * functions (they also use the fts struct). This function allows you to check
 * by depth (exact or min and/or max)  as well and also file type, ignoring
 * those that are not needed or wanted.
 *
 * The cmp() function pointer in the struct fts is a callback which is used in
 * fts_open() (user-defined) to order the way the tree is traversed. If it is
 * NULL we will use fts_cmp() but one may pass in their own function, OR, if
 * they do not wish to not write their own they can use fts_cmp() or fts_rcmp()
 * which is the reverse order of fts_cmp().
 *
 * If cwd != NULL we will set *cwd to the file descriptor of the directory that
 * one started out as (unless fts->tree != NULL as that indicates we already set
 * it, assuming the first call did not pass in a NULL cwd).
 *
 * The options in struct fts will be passed to fts_open(). There is one that
 * will ALWAYS be SET (FTS_NOCHDIR), one that will ALWAYS be UNSET (FTS_NOSTAT).
 * If the fts->logical is true the flag FTS_LOGICAL is set; otherwise
 * FTS_PHYSICAL is set. If the fts->seedot is true FTS_SEEDOT Is set, otherwise
 * it is unset.
 *
 * The reason we need FTS_NOCHDIR set is so that the path can remain valid. The
 * reason we need FTS_NOSTAT unset is because if it is set fts_info will always
 * be FTS_NSOK thus making it impossible to know the file type!
 *
 * The dir is where we will chdir(2) to before proceeding, if dir != NULL &&
 * fts->tree == NULL.
 *
 * given:
 *
 *  dir             -   char * which is the path to chdir(2) to before opening path but
 *                      only if != NULL && *fts == NULL
 *  dirfd           -   if dir == NULL and dirfd >= 0, fchdir(dirfd) to it; if dir !=
 *                      NULL but chdir(dir) fails and dirfd >= 0 try fchdir(2)
 *  cwd             -   if != NULL set *cwd PRIOR to any change in directory
 *  fts             -   struct fts * with parameters for function
 *
 * Returns:
 *  the next entry (an FTSENT *) in the tree or NULL if no other entry remains.
 *
 * NOTE: if fts_read() returns NULL then we will call fts_close(fts->tree) and
 * set fts->tree to NULL. If the user does not call it for every entry (say
 * they're looking for a specific file, although in that case they really should
 * be using find_path()) then they need to do this. In that case they should
 * make sure that fts->tree != NULL!
 *
 * NOTE: paths are searched from the directory in dir (or dirfd) or if dir ==
 * NULL and dirfd < 0 the directory the process is in.
 *
 * NOTE: the struct fts has:
 *
 *      tree            - FTS * returned by fts_open()
 *      options         - options to pass to fts_open() (via read_fts()), see above
 *                        on options
 *      logical         - true ==> use FTS_LOGICAL (follow symlinks), false ==> use
 *                        FTS_PHYSICAL (do not follow symlinks, refer to link itself)
 *      type            - bitwise of types (enum fts_type) to allow different types
 *                        only
 *      count           - if > 0 and base == false search until count file have
 *                        been found
 *      depth           - if > 0 required depth
 *      min_depth       - if > 0 the path depth must be >= this value (if depth <= 0)
 *      max_depth       - if > 0 the path depth must be <= this value (if depth <= 0)
 *      base            - true ==> basename only (i.e. fts->fts_name)
 *      seedot          - true ==> don't skip '.' and '..',
 *                        false ==> skip '.' and '..'
 *      match_case      - true ==> use strcmp(), not strcasecmp()
 *      ignore          - struct dyn_array * of paths to ignore (if desired, else NULL)
 *      match           - struct dyn_array * of paths to find (only) if desired, else NULL
 *      cmp             - callback for fts_open() (used by read_fts())
 *      fn_ignore_flags - flags for fnmatch(3) for ignore list or < 0 (default) if undesired
 *      fn_match_flags  - flags for fnmatch(3) to find files (no other files will be found)
 *      check           - pointer to function to check FTSENT * for certain conditions
 *                        (if NULL we will use check_fts_info())
 *      initialised     - used internally by reset_fts()
 *
 * To use this function you might do something like this where:
 *
 *      dir             ==> "test_jparse" (w/o quotes)
 *      dirfd           ==> -1
 *      cwd             ==> pointer to int to store cwd before changing directories
 *      fts             ==> struct fts * with parameters
 *
 * As for the struct fts, we'll assume that the values are:
 *
 *      tree            ==> FTS * (must be NULL on first pass, see below)
 *      options         ==> FTS_NOCHDIR (done by reset_fts())
 *      logical         ==> false (will set fts->fts_options to FTS_PHYSICAL)
 *      type            ==> FTS_TYPE_ANY (any type of file)
 *      depth           ==> 0 (ignore depth)
 *      min_depth       ==> 0 (no min depth)
 *      max_depth       ==> 0 (no max depth)
 *      seedot          ==> false (remove FTS_SEEDOT; true could be done in options as well)
 *      ignore          ==> NULL (don't ignore anything)
 *      match           ==> NULL (don't search for only specific files)
 *      cmp             ==> fts_rcmp
 *      fn_ignore_flags ==> -1 for ignore list (not strictly relevant as ignore is NULL)
 *      fn_match_flags  ==> -1 for match list (not strictly relevant as match is ignore)
 *      check           ==> NULL (don't do any checks)
 *
 * We do not use the variables count or match_case as this traverses a tree and
 * does not have a name or names to look for (except for ignored list).
 *
 * IMPORTANT: you MUST memset() struct fts to 0 first BEFORE calling reset_fts()
 * and you MUST use reset_fts() before the first use of this function or
 * find_path() or find_paths() (which actually use this function)! This is
 * because we can then be sure everything is a sane value. An example use:
 *
 *      FTSENT *ent = NULL;
 *      struct fts fts;
 *      int cwd = -1;
 *
 *      // zero out fts first!
 *      memset(&fts, 0, sizeof(struct fts)); // important!
 *      // reset fts to sane values, including type to FTS_TYPE_ANY
 *      reset_fts(&fts, true); // reset struct fts to a sane state
 *      fts.cmp = rcmp_fts; // reverse ordering in traversal
 *
 *      ent = read_fts(dir, dirfd, &cwd, &fts);
 *      if (ent == NULL){
 *           .. handle error or return NULL depending on what you need
 *      } else {
 *          do {
 *              ... stuff ...
 *          } while ((ent = read_fts(dir, dirfd, &cwd, &fts)) != NULL);
 *      }
 *
 * or so.
 *
 * That will open fts_open() with the path_argv (of fts_open()) as "." searching
 * from the directory "test_jparse" (w/o quotes) with the options specified in
 * fts->options passed to fts_open() (fts_open() is only called the first time,
 * as long as the args are passed correctly). Since dir != NULL it will first
 * change to the directory ("test_jparse/") (again only the first call) and if
 * that fails it is an error (because dirfd < 0). Otherwise it will use
 * fts_open() with the parameters given in fts. Since we have given a pointer to
 * the cwd (&cwd) before changing directories we store the current working
 * directory in *cwd.
 *
 * If you need to ignore paths you might do this PRIOR to calling the function
 * but AFTER doing the memset() and reset_fts() (!):
 *
 *      append_path(&(fts.ignore), "foo", true, false, false, false);
 *
 * NOTE: second to last false means don't match case. The last false is to not
 * use fnmatch(3) when searching if the path is already in the array. This does
 * not depend on the fts struct because it's a more general function (and one
 * might have the flags set but not want it in some case). If you wish for this
 * to be a glob you can set the fts.fn_ignore_flags to >= 0 (either 0 or one of
 * the flags in the fnmatch(3) man page), otherwise keep -1.
 *
 * If you want to match only specific files (or globs) try something like:
 *
 *      append_path(&(fts.match), "foo*", true, false, false, true);
 *
 * The same notes above apply here except that here we allow globs with
 * fnmatch(3) prior to adding the path (the same could be done for ignore paths
 * of course). In other words, the finding routine will use fnmatch(3) to check
 * if the path already exists in the array.
 *
 * NOTE: if one specifies a bogus range for min/max depth the function will find
 * nothing. For example if one gives a min_depth > the max_depth the function
 * will skip past every file. Of course this only matters if they are > 0. This
 * is tested in the util test code.
 *
 * As a special feature we offer a way to restore and close the previous working
 * directory. If all the pointers (except cwd) are NULL and *cwd > 0 we will try
 * fchdir(*cwd) and then close(*cwd) (meaning that in the calling function cwd
 * is INVALID!).  This is useful to restore the previous current working
 * directory, assuming that &cwd was passed into the function the first time.
 *
 * NOTE: because fts_check_info() has important checks we will call it
 * explicitly too.
 */
FTSENT *
read_fts(char *dir, int dirfd, int *cwd, struct fts *fts)
{
    char *path[] = { ".", NULL };   /* "." for fts_open() */
    char *name = NULL; /* fts path (from fts_path()) */
    FTSENT *ent = NULL; /* next entry from fts_open() */
    intmax_t ignored = 0;   /* > 0 ==> ignored list is not empty */
    intmax_t matches = 0;   /* > 0 ==> match list is not empty */
    intmax_t i = 0;         /* iterate through ignore and match lists */

    /*
     * special check to see if we need to simply change back to the original
     * directory
     */
    if (dir == NULL && dirfd < 0 && cwd != NULL && fts == NULL) {
        if (*cwd > 0) {
            errno = 0;  /* pre-clear errno for errp() */
            if (fchdir(*cwd) != 0) {
                errp(102, __func__, "failed to fchdir(%d)", *cwd);
                not_reached();
            }
            errno = 0; /* pre-clear errno for errp() */
            if (close(*cwd) != 0) {
                errp(103, __func__, "close(%d) failed", *cwd);
                not_reached();
            }
            *cwd = -1;
            return NULL;
        } else {
            warn(__func__, "tried to close invalid FD");
            return NULL;
        }
    }

    /*
     * firewall
     */
    if (fts == NULL) {
        err(104, __func__, "fts is NULL");
        not_reached();
    }

    /*
     * if we have not yet used fts_open() and cwd != NULL we will record the
     * current directory file descriptor
     */
    if (cwd != NULL && fts->tree == NULL) {
        /*
         * note the current directory so user can restore it after they're done
         * with the tree
         */
        errno = 0;			/* pre-clear errno for errp() */
        *cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        if (*cwd < 0) {
            errp(105, __func__, "cannot open .");
            not_reached();
        }
    }

    /*
     * if dir != NULL we need to chdir(dir) first
     */
    if (dir != NULL && *dir != '\0') {
        errno = 0; /* pre-clear errno for warnp() */
        if (chdir(dir) != 0) {
            warnp(__func__, "failed to chdir(\"%s\")", dir);
            if (dirfd > 0) {
                errno = 0;  /* pre-clear errno for errp() */
                if (fchdir(dirfd) != 0) {
                    errp(106, __func__, "both chdir(\"%s\") and fchdir(%d) failed",
                            dir, dirfd);
                    not_reached();
                }
            }
        }
    } else if (dirfd > 0) {
        errno = 0; /* pre-clear errno for errp() */
        if (fchdir(dirfd) != 0) {
            errp(107, __func__, "fchdir(%d) failed", dirfd);
            not_reached();
        }
    }

    /*
     * Correct options, making sure they are > 0 and FTS_NOCHDIR is ALWAYS set
     * so that the paths can be valid after this function ends and that
     * FTS_NOSTAT is NEVER set so that we can determine the file types.
     *
     * We also make sure that neither FTS_PHYSICAL nor FTS_LOGICAL is set
     * (unsetting if necessary) and then if logical is true we set FTS_LOGICAL
     * and if false we set FTS_PHYSICAL.
     *
     * If fts->seedot == true we'll set FTS_SEEDOT.
     */
    if (fts->options <= 0) {
        fts->options = FTS_NOCHDIR;
        if (fts->logical) {
            SET_BIT(fts->options, FTS_NOCHDIR | FTS_LOGICAL);
        } else {
            SET_BIT(fts->options, FTS_NOCHDIR | FTS_PHYSICAL);
        }
        if (fts->seedot) {
            SET_BIT(fts->options, FTS_SEEDOT);
        }
    } else {
        /*
         * first remove FTS_NOSTAT, FTS_LOGICAL, FTS_PHYSICAL and FTS_SEEDOT
         */
        REMOVE_BIT(fts->options, FTS_NOSTAT | FTS_LOGICAL | FTS_PHYSICAL | FTS_SEEDOT);

        /*
         * now ensure that both FTS_NOCHDIR and EITHER FTS_LOGICAL or
         * FTS_PHYSICAL is set, depending on the logical bool but NOT both
         */
        if (fts->logical) {
            SET_BIT(fts->options, FTS_NOCHDIR | FTS_LOGICAL); /* dereference symlinks */
        } else {
            SET_BIT(fts->options, FTS_NOCHDIR | FTS_PHYSICAL);
        }
        /*
         * then if fts->seedot is true, set FTS_SEEDOT
         */
        if (fts->seedot) {
            SET_BIT(fts->options, FTS_SEEDOT);
        }
    }

    /*
     * make sure fts->cmp is set
     */
    if (fts->cmp == NULL) {
        fts->cmp = fts_cmp;
    }

     /* if fts->tree == NULL we need to open an FTS stream "." first
     */
    if (fts->tree == NULL) {
        fts->tree = fts_open(path, fts->options, fts->cmp);
        if (fts->tree == NULL) {
            errp(108, __func__, "fts_open() returned NULL for: %s", dir != NULL ? dir : ".");
            not_reached();
        }
    }

    ignored = paths_in_array(fts->ignore);
    matches = paths_in_array(fts->match);
    /*
     * extra sanity check
     */
    if (fts->tree != NULL) {
        /*
         * Now that we have an FTS * we can read the next entry in the tree.
         *
         * NOTE: we do this in a do..while loop so that we always enter it but
         * in the case of an entry we need to skip (e.g. we encounter an
         * unreadable directory) we can get the next one (or NULL if there are
         * no more). In the case everything is valid we will return the entry
         * anyway so the loop will be exited.
         */
        do {
            ent = fts_read(fts->tree);
            if (ent == NULL) {
                /*
                 * Treebeard died :(
                 */

                /*
                 * close down stream
                 */
                fts_close(fts->tree);
                /*
                 * we should set fts to NULL for safety purposes
                 */
                fts->tree = NULL;
                /*
                 * NULL will be returned below
                 */
            } else {
                name = fts_path(ent);
                if (name == NULL || *name == '\0') {
                    /*
                     * this should not really happen unless something odd is
                     * going on but in case there is something odd going on we
                     * will skip the entry.
                     *
                     * Even so check_fts_info() (called next) will abort on such
                     * a condition but as an additional sanity check we will
                     * check here too.
                     */
                    err(109, __func__, "ent has NULL path, skipping");
                    not_reached();
                }

                /*
                 * check for specific conditions that are either errors or are
                 * problematic conditions. If check was NULL we will only call
                 * check_fts_info() but if check != NULL we will call that after
                 * check_fts_info(). The reason we always call check_fts_info()
                 * is because it checks for error conditions that should be
                 * handled.
                 */
                if (!check_fts_info(fts->tree, ent)) {
                    continue;
                }
                /*
                 * now if fts->check != NULL and it is not check_fts_info we
                 * will use this too
                 */
                if (fts->check != NULL && fts->check != check_fts_info) {
                    /*
                     * NOTE: although fts->check(ent) works we use
                     * (*(fts->check))(ent) to make it clearer that it is a
                     * pointer to a function (okay, so every function is a
                     * pointer but the point is that check is a pointer to a
                     * function passed in if one needs to have specific checks).
                     */
                    if (!(*(fts->check))(fts->tree, ent)) {
                        /*
                         * if a test failed and we get here (i.e. not a fatal error)
                         * then we will continue.
                         */
                        continue;
                    }
                }
                /*
                 * first check depth
                 */
                /*
                 * if depth > 0 then we will NOT check for min or max depth
                 */
                if (fts->depth > 0) {
                    if (ent->fts_level != fts->depth) {
                        continue;
                    }
                } else if (fts->min_depth > 0 || fts->max_depth > 0) {
                    /*
                     * if min_depth > 0 and max_depth > 0 the depth has to be in
                     * the range
                     */
                    if (fts->min_depth > 0 && fts->max_depth > 0 &&
                            (ent->fts_level < fts->min_depth || ent->fts_level > fts->max_depth)) {
                        continue;
                    } else if (fts->min_depth > 0) {
                        /*
                         * if only min depth is set then the depth of this file
                         * must be >= that value
                         */
                        if (ent->fts_level < fts->min_depth) {
                            continue;
                        }
                    } else if (fts->max_depth > 0) {
                        /*
                         * if max_depth > 0 then the current depth must be <=
                         * the max_depth
                         */
                        if (ent->fts_level > fts->max_depth) {
                            continue;
                        }
                    }
                /*
                 * case: we do not want to see '.' or '..' or './' and that is
                 * what the path is.
                 */
                } else if (!fts->seedot && (!strcmp(ent->fts_name, ".") || !strcmp(ent->fts_name, "..") ||
                           !strcmp(ent->fts_name, "./"))) {
                    continue;
                }
                /*
                 * now if the ignored list is > 0 in size we have to check for
                 * matches and if it is a match we will skip it
                 */
                if (ignored > 0) {
                    bool skip = false;
                    for (i = 0; i < ignored; ++i) {
                        char *u = NULL;
                        /* get next string pointer */
                        u = dyn_array_value(fts->ignore, char *, i);
                        if (u == NULL) {	/* paranoia */
                            err(110, __func__, "found NULL pointer in fts->ignore[%jd]", i);
                            not_reached();
                        }
                        if ((fts->base || count_dirs(name) == 1) && (((fts->match_case && !strcmp(ent->fts_name, u)) ||
                           (!fts->match_case && !strcasecmp(ent->fts_name, u)))||(fts->fn_ignore_flags >= 0 &&
                            fnmatch(u, ent->fts_name, fts->fn_ignore_flags) == 0))) {
                            /*
                             * if this is a directory we will not descend into
                             * it
                             */
                            if (ent->fts_info == FTS_D || ent->fts_info == FTS_DP) {
                                errno = 0; /* pre-clear errno for warnp() */
                                if (fts_set(fts->tree, ent, FTS_SKIP) != 0) {
                                    warnp(__func__, "failed to set FTS_SKIP on an ignored directory in the tree: %s", u);
                                }
                            }
                            dbg(DBG_HIGH, "ignoring name: %s", ent->fts_name);
                            skip = true;
                            break;
                        } else if (!fts->base && (((fts->match_case && !strcmp(u, name)) ||
                                  (!fts->match_case && !strcasecmp(u, name)))||(fts->fn_ignore_flags >= 0 &&
                                      fnmatch(u, name, fts->fn_ignore_flags) == 0))) {
                            /*
                             * if this is a directory we will not descend into
                             * it
                             */
                            if (ent->fts_info == FTS_D || ent->fts_info == FTS_DP) {
                                errno = 0; /* pre-clear errno for warnp() */
                                if (fts_set(fts->tree, ent, FTS_SKIP) != 0) {
                                    warnp(__func__, "failed to set FTS_SKIP on an ignored directory in the tree: %s", u);
                                }
                            }
                            dbg(DBG_HIGH, "ignoring path: %s", ent->fts_path);
                            skip = true;
                            break;
                        }
                    }
                    if (skip) {
                        continue;
                    }
                }

                /*
                 * now if the match list is > 0 in size we have to check for
                 * matches and if it is NOT a match we will skip it
                 */
                if (matches > 0) {
                    bool match = false;
                    for (i = 0; i < matches; ++i) {
                        char *u = NULL;
                        /* get next string pointer */
                        u = dyn_array_value(fts->match, char *, i);
                        if (u == NULL) {	/* paranoia */
                            err(111, __func__, "found NULL pointer in fts->match[%jd]", i);
                            not_reached();
                        }
                        if ((fts->base || count_dirs(name) == 1) && (((fts->match_case && !strcmp(ent->fts_name, u)) ||
                           (!fts->match_case && !strcasecmp(ent->fts_name, u)))||(fts->fn_match_flags >= 0 &&
                            !fnmatch(u, ent->fts_name, fts->fn_match_flags)))) {
                            match = true;
                            dbg(DBG_HIGH, "found match: %s", ent->fts_name);
                            match = true;
                        } else if (!fts->base && (((fts->match_case && !strcmp(u, name)) ||
                                  (!fts->match_case && !strcasecmp(u, name)))||(fts->fn_match_flags >= 0 &&
                                      !fnmatch(u, name, fts->fn_match_flags)))) {
                            match = true;
                            dbg(DBG_HIGH, "found match: %s", ent->fts_name);
                        }
                    }
                    if (!match) {
                        /*
                         * if this is a directory we will not descend into
                         * it
                         */
                        if (ent->fts_info == FTS_D || ent->fts_info == FTS_DP) {
                            errno = 0; /* pre-clear errno for warnp() */
                            if (fts_set(fts->tree, ent, FTS_SKIP) != 0) {
                                warnp(__func__, "failed to set FTS_SKIP on a non-match directory in the tree: %s", name);
                            }
                        }
                        continue;
                    }
                }

                /*
                 * if we get here then we can check the file type against the
                 * fts_type enum specified
                 */
                if (fts->type != FTS_TYPE_ANY) {
                    switch (ent->fts_info) {
                        case FTS_D: /* check if directory type is desired */
                        case FTS_DP: /* check if directory type is desired */
                            if (!(fts->type & FTS_TYPE_DIR)) {
                                /*
                                 * NOTE: we don't set it so that we skip the
                                 * directory because there could be files under
                                 * it
                                 */
                                continue;
                            }
                            break;
                        case FTS_F: /* check if regular file is desired */
                            if (!(fts->type & FTS_TYPE_FILE)) {
                                continue;
                            }
                            break;
                        case FTS_SL: /* check if symlink is desired */
                            if (!(fts->type & FTS_TYPE_SYMLINK)) {
                                continue;
                            }
                            break;
                        case FTS_DEFAULT: /* other kinds of files */
                            {
                                /*
                                 * here we have to do more checks than the other cases
                                 * above
                                 */

                                /*
                                 * get file first. This is useful so we don't have to repeatedly
                                 * call lstat(2)/stat(2) in the case of FTS_DEFAULT.
                                 */
                                enum file_type type = file_type(ent->fts_path);
                                if ((!(fts->type & FTS_TYPE_SOCK) && type == FILE_TYPE_SOCK) ||
                                    (!(fts->type & FTS_TYPE_CHAR) && type == FILE_TYPE_CHAR) ||
                                    (!(fts->type & FTS_TYPE_BLOCK) && type == FILE_TYPE_BLOCK) ||
                                    (!(fts->type & FTS_TYPE_FIFO) && type == FILE_TYPE_FIFO)) {
                                        continue;
                                }
                            }
                            break;
                        default:
                            /*
                             * errors were checked in check_fts_info() so this probably
                             * shouldn't even happen but we'll ignore it if it does
                             */
                            break;
                    }
                }

                /*
                 * Return the next Ent, whether it's Treebeard, Quickbeam or any
                 * other one. :-)
                 *
                 * Fun fact: Quickbeam was given the nickname because he answered
                 * 'Yes' to an elder Ent before the question was completely asked.
                 * But did you know that Quickbeam is another word for 'mountain
                 * ash'? So what is 'mountain ash' you might ask?
                 *
                 *  * a small deciduous tree of the rose family, with compound leaves,
                 *    white flowers, and red berries. Compare with rowan.
                 *
                 * Okay but what is 'rowan'? It is:
                 *
                 * * a small deciduous tree of the rose family, with compound
                 * leaves, white flowers, and red berries. Compare with mountain
                 * ash.
                 *
                 * Okay but what is 'mountain ash' ? :-)
                 *
                 * Recursive humour aside, note the 'rose' part. There were many
                 * rowans in Quickbeam's home! They were savagely treated by the
                 * Orcs of Isengard and so that is why he was more 'hasty'. It might
                 * be even a better pun on Tolkien's part because he was also quick
                 * to want to attack Isengard!
                 *
                 * As Tolkien was a lover of trees (dendrophile) (even saying he
                 * would love to get to know a tree and want to ask it what it
                 * thought about things) and as a philologist and lexicographer, he
                 * certainly knew the above fact and now you do too! :-)
                 */
                return ent;
            }
        } while (ent != NULL);
    } else {
        err(112, __func__, "fts->tree is NULL when it shouldn't be");
        not_reached();
    }
    return NULL;
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
    uintmax_t len = 0;	/* number of strings in the array */
    char *u = NULL;     /* name pointer */
    uintmax_t i;

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

    len = paths_in_array(array);
    for (i=0; i < len; ++i) {

	/* get next string pointer */
	u = dyn_array_value(array, char *, i);
	if (u == NULL) {	/* paranoia */
	    err(113, __func__, "found NULL pointer in path name dynamic array element: %ju", i);
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
 * paths_in_array   - return number of paths in an array
 *
 * given:
 *
 *  array   - array to count elements (paths)
 *
 * NOTE: if array is NULL we return 0.
 */
uintmax_t
paths_in_array(struct dyn_array *array)
{
    /*
     * firewall
     */
    if (array == NULL) {
        return 0;
    }

    return (uintmax_t)dyn_array_tell(array);
}


/*
 * find_path_in_array
 *
 * Given a pointer to string (a path), we search a dynamic array of pointers
 * to strings (presumably path(s)).  If an exact match is found (i.e. the
 * string is already in the dynamic array), we return the path. Otherwise we
 * will return NULL, indicating to the caller it could not be found.
 *
 * given:
 *	str		string to find in array
 *	paths		pointer to a struct dyn_array of char * (paths)
 *	match_case      true ==> match case (use strcmp(), not strcasecmp())
 *	fn              true ==> use fnmatch(3)
 *	idx             if not NULL set *idx to element if found
 *
 * returns:
 *	the string if it was found, otherwise NULL.
 *
 * NOTE: if given a NULL pointer (other than idx) this function returns NULL.
 *
 * NOTE: if idx != NULL we will first set it to -1. If the string is then found
 * in the array it will be set to the index (element). If it is NULL or nothing
 * is found it will be either untouched or *idx will be -1. In any case if the
 * string is not found NULL will be returned.
 */
/* XXX - make static after table driven walk code in in place - XXX */
char *
find_path_in_array(char *path, struct dyn_array *paths, bool match_case, bool fn, intmax_t *idx)
{
    intmax_t i;

    /*
     * firewall
     */
    if (paths == NULL || path == NULL) {
        return NULL;
    }

    /*
     * reset *idx if idx != NULL
     */
    if (idx != NULL) {
        *idx = -1;
    }

    i = -1;
    if (array_has_path(paths, path, match_case, fn, &i) && i >= 0) {
        if (idx != NULL) {
            /*
             * record index if requested
             */
            *idx = i;
        }
        return dyn_array_value(paths, char *, i);
    }

    /*
     * nothing found
     */
    return NULL;
}


/*
 * append_path
 *
 * Given a pointer to string (a path), we search a dynamic array of pointers
 * to strings (presumably path(s)).  If an exact match is found (i.e. the
 * string is already in the dynamic array), nothing is done other than to return
 * false (unless unique is false) If no match is found, the pointer to the
 * string is appended to the dynamic array and we return true. If duped is false
 * the string will be dynamically allocated prior to adding it to the array (if
 * it's not already in the list).
 *
 * given:
 *	paths		pointer to pointer to a struct dyn_array * of paths
 *	path		string to search array and append if not already found
 *	unique          true ==> don't add to array if it already exists
 *	duped           true ==> str was dynamically allocated
 *	fn              true ==> use fnmatch(3)
 *	match_case      true ==> use strcmp() not strcasecmp()
 *
 * returns:
 *	true		path was not already in dynamic array and has now been appended
 *	false		path was already in the dynamic array, table is unchanged
 *
 * NOTE: this function does not return if given a NULL path or NULL array.
 *
 * NOTE: if paths != NULL && *paths == NULL we will create an array with a chunk
 * size of 64.
 */
bool
append_path(struct dyn_array **paths, char *path, bool unique, bool duped, bool match_case, bool fn)
{
    char *u = NULL;		/* unique name pointer */

    /*
     * firewall
     */
    if (paths == NULL) {
	err(114, __func__, "paths is NULL");
	not_reached();
    }
    if (path == NULL) {
	err(115, __func__, "path is NULL");
	not_reached();
    }

    if (*paths == NULL) {
        /*
         * create array for user with chunk size of 64
         */
        *paths  = dyn_array_create(sizeof(char *), 64, 64, true);
        if (*paths == NULL) {
            err(116, __func__, "failed to create paths paths");
            not_reached();
        }
    }

    /*
     * case: only add paths if it does not already exist.
     *
     * NOTE: we don't care about idx.
     */
    if (unique && find_path_in_array(path, *paths, match_case, fn, NULL) != NULL) {
        return false;
    }

    /*
     * if not allocated dynamically we have to do dupe it (and yes, yes the joke
     * is intentional)
     */
    if (!duped) {
        errno = 0; /* pre-clear errno for errp() */
        u = strdup(path);
        if (u == NULL) {
            errp(117, __func__, "failed to strdup(\"%s\")", path);
            not_reached();
        }
    } else {
        u = path;
    }
    /*
     * append to paths array
     *
     * NOTE: do NOT call append_path() (this function) as it would cause an
     * infinite recursive loop!
     */
    (void) dyn_array_append_value(*paths, &u);
    return true;
}


/*
 * free_paths_array     - free a struct dyn_array of paths
 *
 * Given a struct dyn_array **paths we will, if paths != NULL and *paths !=
 * NULL, free the array and set *paths to NULL.
 *
 * given:
 *
 *      paths           - array of paths (char *)
 *      only_empty      - true ==> only free array if it's empty
 *
 * This function will do nothing if paths or *paths is NULL so it is safe to
 * call this even even if the pointer is NULL.
 *
 * If only_empty is true we will only free the array if the array is empty (0
 * length). If only_empty is false and there is at least one element in the
 * array we will leave the array untouched.
 *
 * NOTE: the reason for the only_empty bool is it allows one to not have to work
 * out whether or not anything was found: but if something was found the array
 * should be returned (by the caller) and otherwise if it is empty it will
 * return NULL (by way of this function).
 *
 * NOTE: this function can be used for any dyn_array of char * but as we use it
 * specifically for paths and since free_array() has a high chance of being
 * found elsewhere, we use free_paths_array() as it has a lower chance of being
 * used elsewhere (though the chance is certainly not 0).
 */
void
free_paths_array(struct dyn_array **paths, bool only_empty)
{
    uintmax_t len = 0; /* length of array */
    uintmax_t i = 0; /* iterator through array to free */
    char *p = NULL; /* to free each element in the array */

    /*
     * firewall
     */
    if (paths == NULL || *paths == NULL) {
        return;
    } else if (*paths != NULL) {
        len = paths_in_array(*paths);
        if (len > 0 && !only_empty) {
            /*
             * we only free the array and set to NULL if the length is > 0 or we
             * are not ignoring empty arrays
             */
            for (i = 0; i < len; ++i) {
                p = dyn_array_value(*paths, char *, i);
                if (p != NULL) {
                    free(p);
                    p = NULL;
                }
            }
            dyn_array_free(*paths);
            *paths = NULL;
        }
    }

    /* here we do NOT want to touch the array even if it is empty */
}


/*
 * find_path
 *
 * Find a path based in the directory/directory FD or if NULL and < 0 the
 * current working directory of the processes.
 *
 * We use the read_fts() function to find a match based on the name and various
 * parameters in the struct fts along with the path name.
 *
 * If the abspath is true and we did obtain the absolute path then the absolute
 * path will be added to the array; otherwise the path relative to where the
 * process is at the time (prior to changing directory!).
 *
 * If fts->depth > 0 and the depth is not the same the path is skipped (again
 * via read_fts()).
 *
 * If you want specific types of files you can use the fts->type bits:
 * FTS_TYPE_ANY is any kind of file. You can OR and if you wish you can AND the
 * values with a mask to get whatever you need. If the type is not requested
 * read_fts() will skip said file.
 *
 * For more information see the comments above the read_fts() function.
 *
 * given:
 *
 *      path        - path to locate
 *      dir         - dir name to start from if not NULL (used in read_fts())
 *      dirfd       - directory file descriptor to look from if >= 0 (only if
 *                    dir is NULL or chdir(dir) fails) (used in read_fts())
 *      cwd         - if not != NULL set to current working directory FD (in
 *                    read_fts())
 *      abspath     - true ==> get absolute path of matches
 *      fts         - struct fts * with parameters for read_fts() and this
 *                    function.
 *
 * For details on the struct fts see the comments above read_fts().
 *
 * NOTE: this function returns a COPY (strdup()d) of the path so you need to
 * free it when you are done.
 *
 * This function does not return on a NULL path or NULL fts.
 *
 * IMPORTANT: make SURE that before you pass fts you have zeroed it out with
 * memset() (it does not need to be allocated; instead pass a pointer to it) and
 * then use reset_fts()!  That is:
 *
 *      struct fts fts;
 *      memset(&fts, 0, sizeof(struct fts));
 *      reset_fts(&fts, true, true);
 */
char *
find_path(char const *path, char *dir, int dirfd, int *cwd, bool abspath, struct fts *fts)
{
    FTSENT *ent = NULL; /* for read_fts() */
    int i = 0;      /* for count check */
    char *path_found = NULL; /* returned path if a match is found (strdup()ed first) */
    char *dirname = NULL;

    /*
     * firewall
     */
    if (path == NULL) {
        err(118, __func__, "passed NULL path");
        not_reached();
    } else if (fts == NULL) {
        err(119, __func__, "passed NULL fts struct");
        not_reached();
    }

    dbg(DBG_V1_HIGH, "FTS_LOGICAL: %s", booltostr(fts->logical));
    dbg(DBG_V1_HIGH, "basename search: %s", booltostr(fts->base));
    dbg(DBG_V1_HIGH, "FTS_SEEDOT: %s", booltostr(fts->seedot));
    dbg(DBG_V1_HIGH, "case-sensitive: %s", booltostr(fts->match_case));
    dbg(DBG_V1_HIGH, "depth: %d", fts->depth);
    dbg(DBG_V1_HIGH, "count: %d", fts->count);
    dbg(DBG_V1_HIGH, "absolute path: %s", booltostr(abspath));


    /*
     * first open the stream and get the first entry
     */
    ent = read_fts(dir, dirfd, cwd, fts);
    if (ent != NULL) {
        i = 0;
        /*
         * if abspath is true get absolute path of where we are
         */
        if (abspath) {
            errno = 0; /* pre-clear errno for errp() */
            dirname = getcwd(NULL, 0);
            if (dirname == NULL) {
                errp(120, __func__, "failed to get absolute path");
                not_reached();
            }
        } else {
            dirname = NULL; /* paranoia */
        }

        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(121, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }
            if (*path == '\0') {
                if (fts->count <= 0 || (fts->count > 0 && ++i == fts->count)) {
                    if (fts->count > 0) {
                        dbg(DBG_HIGH, "found path with count %d at depth %d: %s", fts->count, fts->depth, p);
                    } else {
                        dbg(DBG_HIGH, "found path %s at depth %d", p, fts->depth);
                    }
                    /*
                     * found a match: save found path
                     */

                    /*
                     * first make sure path_found is NULL
                     */
                    path_found = NULL;
                    /*
                     * depending on if abspath is true, construct either an
                     * absolute path or relative path
                     */
                    if (abspath && dirname != NULL) {
                        path_found = calloc_path(dirname, p);
                        if (path_found == NULL) {
                            err(122, __func__, "failed to allocate absolute path of %s", p);
                            not_reached();
                        }
                        if (dirname != NULL) {
                            /* free d as we no longer need it */
                            free(dirname);
                            dirname = NULL;
                        }
                    } else {
                        errno = 0; /* pre-clear errno for errp() */
                        path_found = strdup(p);
                        if (path_found == NULL) {
                            errp(123, __func__, "failed to strdup(\"%s\")", p);
                            not_reached();
                        }
                    }
                    /*
                     * close down stream
                     */
                    fts_close(fts->tree);
                    fts->tree = NULL;

                    return path_found;
                }
            } else if (fts->base && ((fts->match_case && !strcmp(ent->fts_name, path)) ||
                (!fts->match_case && !strcasecmp(ent->fts_name, path)))) {
                    if (fts->count <= 0 || (fts->count > 0 && ++i == fts->count)) {
                        if (fts->count > 0) {
                            dbg(DBG_HIGH, "found path with count %d at depth %d: %s", fts->count, fts->depth, p);
                        } else {
                            dbg(DBG_HIGH, "found path %s at depth %d", p, fts->depth);
                        }
                        /*
                         * found a match, save path
                         */

                        /*
                         * depending on if abspath is true, construct either an
                         * absolute path or relative path
                         */
                        if (abspath && dirname != NULL) {
                            path_found = calloc_path(dirname, p);
                            if (path_found == NULL) {
                                err(124, __func__, "failed to allocate absolute path of %s", p);
                                not_reached();
                            }
                            if (dirname != NULL) {
                                /* free d as we no longer need it */
                                free(dirname);
                                dirname = NULL;
                            }
                        } else {
                            errno = 0; /* pre-clear errno for errp() */
                            path_found = strdup(p);
                            if (path_found == NULL) {
                                errp(125, __func__, "failed to strdup(\"%s\")", p);
                                not_reached();
                            }
                        }
                        /*
                         * close down stream
                         */
                        fts_close(fts->tree);
                        fts->tree = NULL;

                        return path_found;
                }
            } else if (!fts->base && ((fts->match_case && !strcmp(p, path)) ||
                (!fts->match_case && !strcasecmp(p, path)))) {
                    if (fts->count <= 0 || (fts->count > 0 && ++i == fts->count)) {
                        /*
                         * found a match
                         */

                        if (fts->count > 0) {
                            dbg(DBG_HIGH, "found path with count %d at depth %d: %s", fts->count, fts->depth, p);
                        } else {
                            dbg(DBG_HIGH, "found path %s at depth %d", p, fts->depth);
                        }

                        /*
                         * save found path
                         */
                        /*
                         * depending on if abspath is true, construct either an
                         * absolute path or relative path
                         */
                        if (abspath && dirname != NULL) {
                            path_found = calloc_path(dirname, p);
                            if (path_found == NULL) {
                                err(126, __func__, "failed to allocate absolute path of %s", p);
                                not_reached();
                            }
                            if (dirname != NULL) {
                                /* free d as we no longer need it */
                                free(dirname);
                                dirname = NULL;
                            }
                        } else {
                            errno = 0; /* pre-clear errno for errp() */
                            path_found = strdup(p);
                            if (path_found == NULL) {
                                errp(128, __func__, "failed to strdup(\"%s\")", p);
                                not_reached();
                            }
                        }

                        /*
                         * close down stream
                         */
                        fts_close(fts->tree);
                        fts->tree = NULL;

                        return path_found;
                }
            } else {
                dbg(DBG_VVHIGH, "path %s does not match %s", p, path);
            }
        } while ((ent = read_fts(NULL, -1, NULL, fts)) != NULL);
    }

    /*
     * nothing found with correct type/parameters
     *
     * NOTE: do NOT call fts_close(3) at this point!
     */
    return NULL;
}


/*
 * find_paths
 *
 * Like find_path() but finds multiple paths, returning the paths as a dyn_array
 * (of char *). The paths to find are in a dyn_array which has a list of path(s)
 * to find (also char *). The rest is the same as find_path() and this also uses
 * read_fts().
 *
 * If paths array is NULL or fts is NULL it is an error.
 *
 * Assuming that everything is in order we will use read_fts(), iterating
 * through the paths array to find matches, just like find_path() except that
 * that we scan until there is nothing else left to scan (or an error occurs).
 *
 * If the abspath is true and we did obtain the absolute path then the absolute
 * path will be added to the array; otherwise the path relative to where the
 * process is at the time (prior to changing directory!).
 *
 * For more information see the comments above the read_fts() function and the
 * find_path() function.
 *
 * given:
 *
 *      paths       - dynamic array (struct dyn_array *) of paths (as char *s) to locate
 *      dir         - dir name to start from if not NULL (used in read_fts())
 *      dirfd       - directory file descriptor to look from if >= 0 (only if
 *                    dir is NULL or chdir(dir) fails) (used in read_fts())
 *      cwd         - if not != NULL set to current working directory FD (in
 *                    read_fts())
 *      abspath     - true ==> get absolute path of matches
 *      fts         - struct fts * with parameters for read_fts() and this
 *                    function.
 *
 * NOTE: before first use you MUST memset() the struct to 0 and then call
 * reset_fts(&fts, true). In other words you must do something like:
 *
 *      struct fts fts;
 *      memset(&fts, 0, sizeof(struct fts));
 *      reset_fts(&fts, true, true);
 *
 * NOTE: paths are searched from the directory in dir (or dirfd) or if dir ==
 * NULL and dirfd < 0 the directory the process is in.
 *
 * NOTE: the strings added to the returned dyn_array are COPIES of the char * so
 * one must free the strings before freeing the array itself. To do this you can
 * do:
 *
 *      free_paths_array(&array, true); // free everything only if empty
 *      free_paths_array(&array, false); // free array even if empty
 *
 * IMPORTANT: make SURE that before you pass fts you have zeroed it out with
 * memset() (it does not need to be allocated; instead pass a pointer to it) and
 * then use reset_fts()!  That is:
 *
 *      struct fts fts;
 *      memset(&fts, 0, sizeof(struct fts));
 *      reset_fts(&fts, true, true);
 */
struct dyn_array *
find_paths(struct dyn_array *paths, char *dir, int dirfd, int *cwd, bool abspath, struct fts *fts)
{
    FTSENT *ent = NULL; /* for read_fts() */
    uintmax_t len = 0; /* length of arrays */
    struct dyn_array *paths_found = NULL; /* returned array */
    char *path = NULL;
    char *name = NULL;
    char *dirname = NULL;
    int i = 0;      /* for count check */
    uintmax_t j = 0;   /* for array iteration */

    /*
     * firewall
     */
    if (paths == NULL) {
        err(129, __func__, "paths list is NULL");
        not_reached();
    }
    if (fts == NULL) {
        err(130, __func__, "fts is NULL");
        not_reached();
    }


    dbg(DBG_V1_HIGH, "FTS_LOGICAL: %s", booltostr(fts->logical));
    dbg(DBG_V1_HIGH, "basename search: %s", booltostr(fts->base));
    dbg(DBG_V1_HIGH, "FTS_SEEDOT: %s", booltostr(fts->seedot));
    dbg(DBG_V1_HIGH, "case-sensitive: %s", booltostr(fts->match_case));
    dbg(DBG_V1_HIGH, "depth: %d", fts->depth);
    dbg(DBG_V1_HIGH, "count: %d", fts->count);
    dbg(DBG_V1_HIGH, "absolute path: %s", booltostr(abspath));

    /*
     * first open the stream and get the first entry
     */
    ent = read_fts(dir, dirfd, cwd, fts);
    if (ent != NULL) {
        i = 0;
        /*
         * if abspath is true get absolute path of where we are after read_fts()
         * changed directories (if it did)
         */
        if (abspath) {
            errno = 0; /* pre-clear errno for errp() */
            dirname = getcwd(NULL, 0);
            if (dirname == NULL) {
                errp(131, __func__, "failed to get absolute path");
                not_reached();
            }
        } else {
            dirname = NULL; /* paranoia */
        }

        do {
            char *p = fts_path(ent);
            if (p == NULL) {
                err(132, __func__, "fts_path(ent) returned NULL");
                not_reached();
            }

            len = paths_in_array(paths);
            for (j = 0; j < len; ++j) {
                path = dyn_array_value(paths, char *, j);
                if (path == NULL) {
                    err(133, __func__, "paths[%ju] == NULL", j);
                    not_reached();
                }
                if (*path == '\0') {
                    if (fts->count <= 0 || (fts->count > 0 && ++i == fts->count)) {
                        if (fts->count > 0) {
                            dbg(DBG_HIGH, "found path with count %d at depth %d: %s", fts->count, fts->depth, p);
                        } else {
                            dbg(DBG_HIGH, "found path %s at depth %d", p, fts->depth);
                        }
                        /*
                         * found a match
                         */

                        name = NULL;
                        /*
                         * if abspath == true we will add the absolute path,
                         * otherwise will will only add relative path
                         */
                        if (abspath && dirname != NULL) {
                            name = calloc_path(dirname, p);
                            if (name == NULL) {
                                err(134, __func__, "failed to allocate path: %s/%s", dirname, p);
                                not_reached();
                            } else {
                                dbg(DBG_VVHIGH, "allocated absolute path: %s", name);
                            }
                            /*
                             * false, false because it's already allocated!
                             *
                             * NOTE: as we have an exact path name here we do
                             * not want to use fnmatch(3), thus false as the
                             * last parameter to append_path().
                             */
                            append_path(&paths_found, name, false, false, fts->match_case, false);

                            /*
                             * do NOT free the name as it's been added to the
                             * paths array!
                             */
                        } else {
                            /*
                             * regular append in this case
                             *
                             * NOTE: as we have an exact path name here we do
                             * not want to use fnmatch(3), thus false as the
                             * last parameter to append_path().
                             */
                            append_path(&paths_found, p, true, false, fts->match_case, false);
                        }

                        /*
                         * continue to next entry
                         */
                        continue;
                    }
                } else if (fts->base && ((fts->match_case && !strcmp(ent->fts_name, path)) ||
                            (!fts->match_case&&!strcasecmp(ent->fts_name, path)))) {
                    if (fts->count <= 0 || (fts->count > 0 && ++i == fts->count)) {
                        /*
                         * found a match
                         */
                        if (fts->count > 0) {
                            dbg(DBG_HIGH, "found path with count %d at depth %d: %s", fts->count, fts->depth, p);
                        } else {
                            dbg(DBG_HIGH, "found path %s at depth %d", p, fts->depth);
                        }

                        name = NULL;
                        /*
                         * if abspath == true we will add the absolute path,
                         * otherwise will will only add relative path
                         */
                        if (abspath && dirname != NULL) {
                            name = calloc_path(dirname, p);
                            if (name == NULL) {
                                err(135, __func__, "failed to allocate path: %s/%s", dirname, p);
                                not_reached();
                            } else {
                                dbg(DBG_VVHIGH, "allocated absolute path: %s", name);
                            }
                            /*
                             * false, false because it's already allocated!
                             *
                             * NOTE: as we have an exact path name here we do
                             * not want to use fnmatch(3), thus false as the
                             * last parameter to append_path().
                             */
                            append_path(&paths_found, name, false, false, fts->match_case, false);

                            /*
                             * do NOT free the name as it's been added to the
                             * paths array!
                             */
                        } else {
                            /*
                             * regular append in this case
                             *
                             * NOTE: as we have an exact path name here we do
                             * not want to use fnmatch(3), thus false as the
                             * last parameter to append_path().
                             */
                            append_path(&paths_found, p, true, false, fts->match_case, false);
                        }

                        /*
                         * continue to next entry
                         */
                        continue;
                    }
                } else if (!fts->base && ((fts->match_case && !strcmp(p, path)) ||
                            (!fts->match_case && !strcasecmp(p, path)))) {
                    if (fts->count <= 0 || (fts->count > 0 && ++i == fts->count)) {
                        /*
                         * found a match
                         */

                        if (fts->count > 0) {
                            dbg(DBG_HIGH, "found path with count %d at depth %d: %s", fts->count, fts->depth, p);
                        } else {
                            dbg(DBG_HIGH, "found path %s at depth %d", p, fts->depth);
                        }

                        name = NULL;
                        /*
                         * if abspath == true we will add the absolute path,
                         * otherwise will will only add relative path
                         */
                        if (abspath && dirname != NULL) {
                            name = calloc_path(dirname, p);
                            if (name == NULL) {
                                err(136, __func__, "failed to allocate path: %s/%s", dirname, p);
                                not_reached();
                            } else {
                                dbg(DBG_VVHIGH, "allocated absolute path: %s", name);
                            }
                            /*
                             * false, false because it's already allocated!
                             *
                             * NOTE: as we have an exact path name here we do
                             * not want to use fnmatch(3), thus false as the
                             * last parameter to append_path().
                             */
                            append_path(&paths_found, name, false, false, fts->match_case, false);

                            /*
                             * do NOT free the name as it's been added to the
                             * paths array!
                             */
                        } else {
                            /*
                             * regular append in this case
                             *
                             * NOTE: as we have an exact path name here we do
                             * not want to use fnmatch(3), thus false as the
                             * last parameter to append_path().
                             */
                            append_path(&paths_found, p, true, false, fts->match_case, false);
                        }

                        /*
                         * continue to next entry
                         */
                        continue;
                    }
                } else {
                    dbg(DBG_VVHIGH, "path %s does not match %s", p, path);
                }
            }
        } while ((ent = read_fts(NULL, -1, NULL, fts)) != NULL);
    }

    /*
     * NOTE: do NOT call fts_close(3) at this point!
     */

    /*
     * NOTE: if paths_array is empty it will be freed. Otherwise the array will
     * be left untouched. That should not happen but we do this check just in
     * case.
     */
    free_paths_array(&paths_found, true);
    if (dirname != NULL){
        free(dirname);
        dirname = NULL;
    }
    return paths_found;
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
	err(137, __func__, "called with NULL path");
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
	err(138, __func__, "called with NULL path");
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
	err(139, __func__, "called with NULL path");
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
        err(140, __func__, "passed NULL cmd");
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
                errp(141, __func__, "strstr(cmd) returned NULL");
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
                errp(142, __func__, "strdup(cmd) returned NULL");
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
        errp(143, __func__, "strdup(path) returned NULL");
        not_reached();
    }

    /*
     * now we can tokenise the path
     */
    for (p = strtok_r(dup, ":", &saveptr); p != NULL; p = strtok_r(NULL, ":", &saveptr)) {
        str = calloc_path(p, cmd);
        if (str == NULL) {
            err(144, __func__, "failed to allocate path: %s/%s", p, cmd);
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
        err(145, __func__, "src path is NULL");
        not_reached();
    } else if (*src == '\0') {
        err(146, __func__, "src path is empty string");
        not_reached();
    }

    if (dest == NULL) {
        err(147, __func__, "dest path is NULL");
        not_reached();
    } else if (*dest == '\0') {
        err(148, __func__, "dest path is empty string");
        not_reached();
    }

    /*
     * verify that src file exists
     */
    if (!exists(src)) {
        err(149, __func__, "src file does not exist: %s", src);
        not_reached();
    } else if (!is_file(src)) {
        err(150, __func__, "src file is not a regular file: %s", src);
        not_reached();
    } else if (!is_read(src)) {
        err(151, __func__, "src file is not readable: %s", src);
        not_reached();
    }

    /*
     * verify dest path does NOT exist
     */
    if (exists(dest)) {
        err(152, __func__, "dest file already exists: %s", dest);
        not_reached();
    }

    /*
     * open src file for reading
     */
    errno = 0;      /* pre-clear errno for errp() */
    in_file = fopen(src, "rb");
    if (in_file == NULL) {
        errp(153, __func__, "couldn't open src file %s for reading: %s", src, strerror(errno));
        not_reached();
    }

    /*
     * obtain file descriptor of src file
     */
    errno = 0; /* pre-clear errno for errp() */
    infd = open(src, O_RDONLY|O_CLOEXEC, S_IRWXU);
    if (infd < 0) {
        errp(154, __func__, "failed to obtain file descriptor for %s: %s", src, strerror(errno));
        not_reached();
    }

    /*
     * get stat(2) info of src file
     */
    errno = 0;      /* pre-clear errno for errp() */
    ret = fstat(infd, &in_st);
    if (ret < 0) {
	errp(155, __func__, "failed to get stat info for %s, stat returned: %s", src, strerror(errno));
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
        err(156, __func__, "couldn't read in src file: %s", src);
        not_reached();
    }

    dbg(DBG_HIGH, "read %zu bytes from src file %s", inbytes, src);

    /*
     * close the src file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(in_file);
    if (ret < 0) {
	errp(157, __func__, "fclose error for %s: %s", src, strerror(errno));
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
        errp(158, __func__, "couldn't open dest file %s for writing: %s", dest, strerror(errno));
        not_reached();
    }

    /*
     * obtain file descriptor of dest file
     */
    errno = 0; /* pre-clear errno for errp() */
    outfd = open(dest, O_WRONLY|O_CLOEXEC, S_IRWXU);
    if (outfd < 0) {
        errp(159, __func__, "failed to obtain file descriptor for %s: %s", dest, strerror(errno));
        not_reached();
    }


    /*
     * write buffer into the dest file
     */
    errno = 0;		/* pre-clear errno for warnp() */
    outbytes = fwrite(buf, 1, inbytes, out_file);
    if (outbytes != inbytes) {
        errp(160, __func__, "error: wrote %zu bytes out of expected %zu bytes", outbytes, inbytes);
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
	errp(161, __func__, "fclose error for %s: %s", dest, strerror(errno));
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
        err(162, __func__, "couldn't open dest file for reading: %s: %s", dest, strerror(errno));
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
        err(163, __func__, "couldn't read in dest file: %s", dest);
        not_reached();
    }

    dbg(DBG_HIGH, "read %zu bytes from dest file %s", inbytes, src);

    /*
     * close the dest file
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fclose(out_file);
    if (ret < 0) {
	errp(164, __func__, "fclose error for %s: %s", dest, strerror(errno));
	not_reached();
    }

    /*
     * first check that the bytes read in is the same as the bytes written
     */
    if (outbytes != inbytes) {
        err(165, __func__, "error: read %zu bytes out of expected %zu bytes", inbytes, outbytes);
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
        err(166, __func__, "copy of src file %s is not the same as the contents of the dest file %s", src, dest);
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
            errp(167, __func__, "fchmod(2) failed to set source file %s mode %o on %s: %s", src, in_st.st_mode,
                    dest, strerror(errno));
            not_reached();
        }

        /*
         * get stat(2) info of dest file
         */
        errno = 0;      /* pre-clear errno for errp() */
        ret = fstat(outfd, &out_st);
        if (ret != 0) {
            errp(168, __func__, "failed to get stat info for %s, stat returned: %s", dest, strerror(errno));
            not_reached();
        }

        /*
         * we now need to verify that the modes are the same
         */
        if (in_st.st_mode != out_st.st_mode) {
            err(169, __func__, "failed to copy st_mode %o from %s to %s: %o != %o", in_st.st_mode, src, dest, in_st.st_mode,
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
            errp(170, __func__, "fchmod(2) failed to set requested mode on %s: %s", dest, strerror(errno));
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
        errp(171, __func__, "close(outfd) failed: %s", strerror(errno));
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
        errp(172, __func__, "failed to create file: %s", path);
        not_reached();
    }

    dbg(DBG_HIGH, "created file %s with mode %04o", path, mode);

    /*
     * now close the file we created
     */
    errno = 0; /* pre-clear errno for errp() */
    if (close(fd) != 0) {
        errp(173, __func__, "failed to close newly created file: %s", path);
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
        errp(174, __func__, "cannot open .");
        not_reached();
    }

    if (dir != NULL) {
        errno = 0; /* pre-clear errno for errp() */
        if (chdir(dir) != 0) {
            if (dirfd >= 0) {
                errno = 0; /* pre-clear errno for errp() */
                if (fchdir(dirfd) != 0) {
                    errp(175, __func__, "both chdir(2) and fchdir(2) failed");
                    not_reached();
                }
            } else {
                errp(176, __func__, "chdir(2) failed");
                not_reached();
            }
        }
    } else if (dirfd >= 0) {
        errno = 0; /* pre-clear errno for errp() */
        if (fchdir(dirfd) != 0) {
            errp(177, __func__, "fchdir(2) failed");
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
        errp(178, __func__, "failed to fchdir(2) back to original directory");
        not_reached();
    }

    /*
     * now close the directory FD we opened
     */
    errno = 0; /* pre-clear errno for errp() */
    if (close(cwd) != 0) {
        errp(179, __func__, "failed to close(cwd)");
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
        err(180, __func__, "str (path) is NULL");
        not_reached();
    }

    len = strlen(str);
    if (len <= 0) {
        err(181, __func__, "str (path) is empty");
        not_reached();
    }

    errno = 0; /* pre-clear errno for errp() */
    dup = strdup(str);
    if (dup == NULL) {
        errp(182, __func__, "duplicating \"%s\" failed", str);
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
        errp(183, __func__, "cannot open .");
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
        errp(184, __func__, "failed to change to parent directory");
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
                errp(185, __func__, "mkdir() of %s failed with: %s", dup, strerror(errno));
                not_reached();
            } else {
                /*
                 * now set modes
                 */
                errno = 0; /* pre-clear errno for errp */
                if (chmod(dup, mode) != 0) {
                    errp(186, __func__, "chmod(\"%s\", %o) failed", dup, mode);
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
                errp(187, __func__, "chmod(\"%s\", %o) failed", dup, mode);
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
                errp(188, __func__, "mkdir() of %s failed with: %s", p, strerror(errno));
                not_reached();
            } else {
                /*
                 * now set modes
                 */
                errno = 0; /* pre-clear errno for errp */
                if (chmod(dup, mode) != 0) {
                    errp(189, __func__, "chmod(\"%s\", %o) failed", dup, mode);
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
                errp(190, __func__, "chmod(\"%s\", %o) failed", dup, mode);
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
            errp(191, __func__, "failed to change to %s", p);
            not_reached();
        }

        /*
         * now we have to extract the next components
         */
        for (p = strtok_r(NULL, "/", &saveptr); p != NULL; p = strtok_r(NULL, "/", &saveptr)) {
            errno = 0; /* pre-clear errno for errp() */
            if (mkdir(p, 0) != 0) {
                if (errno != EEXIST) {
                    errp(192, __func__, "mkdir() of %s failed with: %s", p, strerror(errno));
                    not_reached();
                } else {
                    /*
                     * now set modes
                     */
                    errno = 0; /* pre-clear errno for errp */
                    if (chmod(p, mode) != 0) {
                        errp(193, __func__, "chmod(\"%s\", %o) failed", p, mode);
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
                    errp(194, __func__, "chmod(\"%s\", %o) failed", p, mode);
                    not_reached();
                } else {
                    dbg(DBG_HIGH, "set mode %o on %s", mode, p);
                }
            }
            errno = 0; /* pre-clear errno for errp() */
            dir = chdir(p);
            if (dir < 0) {
                errp(195, __func__, "failed to open directory %s", p);
                not_reached();
            }
        }
    }

    /*
     * go back to original directory
     */
    errno = 0; /* pre-clear errno for errp() */
    if (fchdir(cwd) != 0) {
        errp(196, __func__, "failed to change back to previous directory");
        not_reached();
    }

    /*
     * close our file descriptor of the current working directory
     */
    errno = 0; /* pre-clear errno for errp() */
    if (close(cwd) != 0) {
        errp(197, __func__, "failed to close(cwd): %s", strerror(errno));
        not_reached();
    }

    return 0;
}


/*
 * path_sanity_name
 *
 * Returns read-only string representation of path sanity enum value.
 *
 * given:
 *      sanity  - an enum path_sanity
 */
char const *
path_sanity_name(enum path_sanity sanity)
{
    char const *str = NULL;

    switch (sanity) {
        case PATH_OK:
            str ="PATH_OK";
            break;
        case PATH_ERR_PATH_IS_NULL:
            str = "PATH_ERR_PATH_IS_NULL";
            break;
        case PATH_ERR_PATH_EMPTY:
            str = "PATH_ERR_PATH_EMPTY";
            break;
        case PATH_ERR_PATH_TOO_LONG:
            str = "PATH_ERR_PATH_TOO_LONG";
            break;
        case PATH_ERR_NOT_RELATIVE:
            str = "PATH_ERR_NOT_RELATIVE";
            break;
        case PATH_ERR_NAME_TOO_LONG:
            str = "PATH_ERR_NAME_TOO_LONG";
            break;
        case PATH_ERR_PATH_TOO_DEEP:
            str = "PATH_ERR_PATH_TOO_DEEP";
            break;
        case PATH_ERR_NOT_POSIX_SAFE:
            str = "PATH_ERR_NOT_POSIX_SAFE";
            break;
        case PATH_ERR_DOTDOT_OVER_TOPDIR:
            str = "PATH_ERR_DOTDOT_OVER_TOPDIR";
            break;
        case PATH_ERR_MALLOC:
            str = "PATH_ERR_MALLOC";
            break;
        case PATH_ERR_NULL_COMPONENT:
            str = "PATH_ERR_NULL_COMPONENT";
            break;
        case PATH_ERR_WRONG_LEN:
            str = "PATH_ERR_WRONG_LEN";
            break;
        default:
        case PATH_ERR_UNKNOWN:
            str = "PATH_ERR_UNKNOWN";
            break;
    }

    return str;
}


/*
 * path_sanity_error
 *
 * Returns read-only string describing path sanity enum value.
 *
 * given:
 *      sanity  - an enum path_sanity
 */
char const *
path_sanity_error(enum path_sanity sanity)
{
    char const *str = NULL;

    switch (sanity) {
        case PATH_OK:
            str = "path is a sane path";
            break;
        case PATH_ERR_PATH_IS_NULL:
            str = "path string is NULL";
            break;
        case PATH_ERR_PATH_EMPTY:
            str = "path string length <= 0";
            break;
        case PATH_ERR_PATH_TOO_LONG:
            str = "path string length > max path length";
            break;
        case PATH_ERR_NOT_RELATIVE:
            str = "path not a relative path";
            break;
        case PATH_ERR_NAME_TOO_LONG:
            str = "a path component > max filename length";
            break;
        case PATH_ERR_PATH_TOO_DEEP:
            str = "depth > max depth";
            break;
        case PATH_ERR_NOT_POSIX_SAFE:
            str = "path component unsafe";
            break;
        case PATH_ERR_DOTDOT_OVER_TOPDIR:
            str = "'..' (dotdot) path component moved above topdir";
            break;
        case PATH_ERR_MALLOC:
            str = "malloc related failure during path processing";
            break;
        case PATH_ERR_NULL_COMPONENT:
            str = "component on path stack is NULL";
            break;
        case PATH_ERR_WRONG_LEN:
            str = "constructed canonical path has the wrong length";
            break;
        default:
            str = "invalid path_sanity value";
            break;
    }

    return str;
}


/*
 * canon_path - canonicalize a path
 *
 * given:
 *	orig_path	    - path to canonicalize
 *      max_path_len        - max canonicalized path length, 0 ==> no limit
 *	max_filename_len    - max length of each component of path, 0 ==> no limit
 *      max_depth           - max depth of subdirectory tree, 0 ==> no limit, <0 ==> reserved for future use
 *      sanity_p	    - NULL ==> don't save canon_path path_sanity error, or PATH_OK
 *			      != NULL ==> save enum path_sanity in *sanity_p
 *      len_p		    - NULL ==> don't save canonical path length,
 *		              != NULL ==> save canonical path length in *len_p
 *      depth_p		    - NULL ==> don't save canonical depth,
 *			      != NULL ==> record canonical depth in *depth_p
 *      rel_only	    - true ==> path from "/" (slash) NOT allowed, path depth counted from implied "." (dot)
 *			      false ==> path from "/" (slash) allowed, path depth counted from /
 *	any_case	    - true ==> don't change path case
 *			      false ==> convert UPPER CASE to lower case during canonicalization
 *	safe_chk	    - true ==> test each canonical path component using safe_path_str(str, any_case, false)
 *			      false ==> do not perform path safety tests on the path
 *
 * Examples of path depths are as follows:
 *
 *      depth                   path
 *      -----                   ----
 *      0                       .
 *      0                       /
 *      0                       foo/..
 *
 *      1                       /foo
 *      1                       ./foo
 *      1                       foo
 *
 *      2                       /foo/bar
 *      2                       ./foo/bar
 *      2                       foo/bar
 *
 *      2                       foo/bar/../baz
 *      2                       ./foo/bar/curds/../../baz
 *      2                       /foo/bar/baz/..
 *
 *      3                       /foo/bar/baz
 *      3                       ./foo/bar/baz
 *      3                       foo/bar/baz
 *
 *      4                       /foo/bar/baz/zab
 *      4                       ./foo/bar/baz/zab
 *      4                       foo/bar/baz/zab
 *
 *      5                       /foo/bar/baz/zab/rab
 *      5                       ./foo/bar/baz/zab/rab
 *      5                       foo/bar/baz/zab/rab
 *
 * returns:
 *	NULL ==> invalid path, internal error, or NULL pointer used
 *	!= NULL ==> malloced path that has been canonicalized
 */
char *
canon_path(char const *orig_path, size_t max_path_len, size_t max_filename_len, int32_t max_depth,
	   enum path_sanity *sanity_p, size_t *len_p, int32_t *depth_p, bool rel_only, bool any_case, bool safe_chk)
{
    char *path = NULL;		/* duplicated orig_path to be/that has been canonicalized */
    size_t path_len = 0;	/* full path length */
    size_t tmp_len = 0;		/* temporary path length */
    size_t comp_len = 0;	/* path component length */
    enum path_sanity sanity = PATH_OK;	/* canon_path path_sanity error, or PATH_OK */
    bool relative = true;	/* true ==> path is relative to "." (dot), false ==> path is absolute */
    struct dyn_array *array = NULL;    /* dynamic array of pointers to strings - path component stack */
    char *p = NULL;		/* path component */
    char **q = NULL;		/* address of a dynamic array string element */
    int32_t deep = 0;		/* dynamic array stack depth */
    bool test = true;		/* true ==> passed test, false == failed test */
    char *ret_path = NULL;	/* malloced canonicalized path to return */
    size_t strlcpy_ret = 0;	/* private_strlcpy() return value */
    size_t i = 0;		/* path component number */

    /*
     * firewall
     */
    if (orig_path == NULL) {

	/* orig_path is NULL error */
	report_canon_err(PATH_ERR_PATH_IS_NULL, sanity_p, len_p, depth_p, path, array);
	dbg(DBG_HIGH, "%s: error %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	return NULL;
    }

    /*
     * case: empty path
     */
    if (orig_path[0] == '\0') {

	/* orig_path is an empty string error */
	report_canon_err(PATH_ERR_PATH_EMPTY, sanity_p, len_p, depth_p, path, array);
	dbg(DBG_HIGH, "%s: error %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	return NULL;

    /*
     * case: orig_path is absolute
     */
    } else if (orig_path[0] == '/') {
	if (rel_only) {

	    /* orig_path is absolute, but rel_only is true */
	    report_canon_err(PATH_ERR_NOT_RELATIVE, sanity_p, len_p, depth_p, path, array);
	    dbg(DBG_HIGH, "%s: error %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    return NULL;

	} else {

	    /* orig_path is absolute and allowed to be absolute */
	    relative = false;
	}
    }

    /*
     * duplicate orig_path so that we can modify it as needed as we parse it
     */
    path = strdup(orig_path);
    if (path == NULL) {

	/* strdup failure */
	report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
	dbg(DBG_HIGH, "%s: error #0 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	return NULL;
    }

    /*
     * Remove trailing "/", unless the path is just a single character
     */
    tmp_len = strlen(path);
    dbg(DBG_VVHIGH, "%s: initial path: %s", __func__, path);
    while (tmp_len > 1 && path[tmp_len-1] == '/') {
	path[--tmp_len] = '\0';
    }
    dbg(DBG_V3_HIGH, "%s: path with trailing /'s removed: %s", __func__, path);

    /*
     * create path component stack
     */
    array = dyn_array_create(sizeof(char *), PATH_CHUNK_SIZE, PATH_INITIAL_SIZE, true);

    /*
     * process each path component
     *
     * The strtok(3) function will skip over 1 or more "/" (slash)'s and turn the
     * next path component into a NUL byte terminated string.
     */
    for (i=0, p=strtok(path, "/"); p != NULL; p=strtok(NULL, "/"), ++i) {

	/*
	 * check for "." (dot)
	 */
	dbg(DBG_V3_HIGH, "%s: path component[%zu]: %s", __func__, i, p);
	comp_len = strlen(p);
	if (strcmp(p, ".") == 0) {

	    /*
	     * ignore "." (dot)
	     */
	    dbg(DBG_V3_HIGH, "%s: skipping . path component", __func__);

	/*
	 * if ".." (dotdot), attempt to pop the path component stack
	 */
	} else if (strcmp(p, "..") == 0) {

	    /*
	     * pop the component stack due to ..
	     */
	    if (dyn_array_tell(array) <= 0) {

		/* path component stack underflow */
		report_canon_err(PATH_ERR_DOTDOT_OVER_TOPDIR, sanity_p, len_p, depth_p, path, array);
		dbg(DBG_HIGH, "%s: path component stack underflow error %s: %s",
		    __func__, path_sanity_name(sanity), path_sanity_error(sanity));
		return NULL;
	    }
	    deep = (int32_t)dyn_array_pop(array, NULL);
	    dbg(DBG_V3_HIGH, "%s: .. component stack pop, stack depth: %d", __func__, deep);

	/*
	 * process this path component
	 */
	} else {

	    /*
	     * check path component if max_filename_len > 0
	     */
	    if (max_filename_len > 0) {

		/*
		 * be sure that the path component is not too long
		 */
		if (comp_len > max_filename_len) {

		    /* path component too long */
		    dbg(DBG_V3_HIGH, "%s: path component length: %zu > max_filename_len: %zu",
			__func__, comp_len, max_filename_len);
		    report_canon_err(PATH_ERR_NAME_TOO_LONG, sanity_p, len_p, depth_p, path, array);
		    dbg(DBG_HIGH, "%s: path component too long %s: %s",
			__func__, path_sanity_name(sanity), path_sanity_error(sanity));
		    return NULL;
		}
	    }

	    /*
	     * check if path string is safe
	     */
	    if (safe_chk) {

		/*
		 * We will perform a test if a path string is safe
		 *
		 * We will set any_case as true because either any_case == true,
		 *	in which case ANY case is OK,
		 * or any_case == false, in which case we need to convert to lower case,
		 * and converting to lower case means that beforehand, ANY case is OK.
		 *
		 * We will set slash_ok to false because this is a path component
		 * and strtok(3) should never let "/" (slash) be present.
		 */
		test = safe_path_str(p, true, false);
		if (! test) {

		    /* path component is unsafe */
		    report_canon_err(PATH_ERR_NOT_POSIX_SAFE, sanity_p, len_p, depth_p, path, array);
		    dbg(DBG_HIGH, "%s: path component is not safe", __func__);
		    return NULL;
		}
	    }

	    /*
	     * convert to lower case if not any_case
	     */
	    if (! any_case) {

		/*
		 * convert UPPER case to lower case
		 */
		for (i=0; i < comp_len; ++i) {
		    p[i] = tolower(p[i]);
		}
		dbg(DBG_V3_HIGH, "%s: lower case path component[%zu: %s", __func__, i, p);
	    }

	    /*
	     * push component into the path stack
	     *
	     * NOTE: If the path stack were to grow to larger than INT32_MAX,
	     *	     and if we have a maximum depth allowed, we will declare
	     *	     an immediate PATH_ERR_PATH_TOO_DEEP, even if some later
	     *	     ".." (dotdot) might be able to later reduce the depth.
	     */
	    test = dyn_array_push(array, p);
	    if (max_depth > 0 && dyn_array_tell(array) > INT32_MAX) {

		/* path component too deep */
		dbg(DBG_V3_HIGH, "%s: path depth: %d max_depth: %d", __func__, deep, max_depth);
		report_canon_err(PATH_ERR_PATH_TOO_DEEP, sanity_p, len_p, depth_p, path, array);
		dbg(DBG_HIGH, "%s: while canonicalizing, path became too deep %s: %s",
		    __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    }
	    deep = (dyn_array_tell(array) > INT32_MAX) ? INT32_MAX : (int32_t)dyn_array_tell(array);
	    dbg(DBG_V3_HIGH, "%s: pushed path component on stack, depth: %d", __func__, deep);
	    dbg(DBG_V4_HIGH, "%s: data moved: %s", __func__, booltostr(test));
	}
    }

    /* assertion: the path stack contains the component of the canonicalized stack */

    /*
     * check depth if max_depth > 0
     */
    if (max_depth > 0 && deep > max_depth) {

	/* path component too deep */
	dbg(DBG_V3_HIGH, "%s: path depth: %d max_depth: %d", __func__, deep, max_depth);
	report_canon_err(PATH_ERR_PATH_TOO_DEEP, sanity_p, len_p, depth_p, path, array);
	dbg(DBG_HIGH, "%s: path component too is deep %s: %s",
	    __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	return NULL;
    }

    /*
     * determine canonicalized path length
     *
     * If orig_path is an allowed absolute path, then the canonicalized path length
     * is the sum of the lengths of the canonicalized stack plus a 1 for each path level.
     *
     * If orig_path is a relative path, then the canonicalized path length is one less than if
     * it were a absolute path because the canonicalized path length doesn't start with "/" (slash).
     */
    path_len = (relative) ? (deep>0 ? deep-1 : 0) : deep;
    for (q = dyn_array_first(array, char *); q < dyn_array_beyond(array, char *); ++q) {

	/* paranoia */
	if (q == NULL || *q == NULL) {

	    /* NULL component pointer */
	    report_canon_err(PATH_ERR_NULL_COMPONENT, sanity_p, len_p, depth_p, path, array);
	    dbg(DBG_HIGH, "%s: error #0 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    return NULL;
	}

	/* sum component length */
	path_len += strlen(*q);
    }

    /*
     * check canonicalized path length if max_path_len > 0
     */
    if (max_path_len > 0 && path_len > max_path_len) {

	/* path too long */
	report_canon_err(PATH_ERR_PATH_TOO_LONG, sanity_p, len_p, depth_p, path, array);
	dbg(DBG_HIGH, "%s: error %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	return NULL;
    }

    /*
     * special case: path is "." (dot) or "/" (slash) or empty path
     *
     * If we have no components on the path stack, then we have just the topdir,
     * which will be "/" (slash) is path is absolute, or "." (dot) otherwise.
     */
    if (deep <= 0 || path_len <= 0) {

	/*
	 * case: path is relative, return "." (dot)
	 */
	if (relative) {

	    /* return "." (dot) */
	    ret_path = strdup(".");
	    if (ret_path == NULL) {

		/* strdup failure */
		report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
		dbg(DBG_HIGH, "%s: error #1 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
		return NULL;
	    }

	/*
	 * case: path is absolute, return "/" (slash)
	 */
	} else {
	    /* return "/" (slash) */
	    ret_path = strdup("/");
	    if (ret_path == NULL) {

		/* strdup failure */
		report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
		dbg(DBG_HIGH, "%s: error #2 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
		return NULL;
	    }
	}

	/*
	 * fill in return stats as required
	 */
	if (sanity_p != NULL) {
	    *sanity_p = PATH_OK;
	}
	if (len_p != NULL) {
	    *len_p = 1;
	}
	if (depth_p != NULL) {
	    *depth_p = 0;
	}

	/*
	 * free stack storage
	 */
	if (path != NULL) {
	    free(path);
	    path = NULL;
	}
	dyn_array_free(array);
	array = NULL;

	/*
	 * return special canonicalized path
	 */
	return ret_path;
    }

    /*
     * allocate canonicalized path to return
     *
     * NOTE: The special case of a "/" (slash) or a "." (dot) canonicalized path
     *	     has been already handled above.
     */
    ret_path = calloc(1, path_len+1+1);	/* +1 for trailing NUL, +1 for paranoia */
    if (ret_path == NULL) {
	/* malloc failure */
	report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
	dbg(DBG_HIGH, "%s: error #3 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	return NULL;
    }
    ret_path[path_len+1] = '\0';    /* paranoia */

    /*
     * load canonicalized path from path stack components with "/"s (slash) as needed
     *
     * NOTE: The special case of a "/" (slash) or a "." (dot) canonicalized path
     *	     has been already handled above.
     */
    for (q = dyn_array_first(array, char *); q < dyn_array_beyond(array, char *); ++q) {
	/* paranoia */
	if (q == NULL || *q == NULL) {

	    /* NULL component pointer */
	    report_canon_err(PATH_ERR_NULL_COMPONENT, sanity_p, len_p, depth_p, path, array);
	    dbg(DBG_HIGH, "%s: error #1 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    return NULL;
	}

	/*
	 * append "/"s (slash) as needed
	 */
	if (q == dyn_array_first(array, char *)) {
	    /*
	     * case: on first component of the canonicalized path
	     *
	     * We prep the start of canonicalized path.
	     * An absolute canonicalized path starts with "/" (slash)
	     */
	    ret_path[0] = (relative) ? '\0' : '/';
	} else {
	    /*
	     * case: a subsequent component of the canonicalized path
	     */
	    strcat(ret_path, "/");
	}

	/* append component from stack */
	strlcpy_ret = private_strlcat(ret_path, *q, path_len+1);
	if (strlcpy_ret >= path_len+1) {
	    /* canonicalized path length mis-calculation */
	    report_canon_err(PATH_ERR_WRONG_LEN, sanity_p, len_p, depth_p, path, array);
	    dbg(DBG_HIGH, "%s: error #0 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    return NULL;
	}
    }
    /* canonicalized path length sanity check */
    if (path_len != strlen(ret_path)) {
	/* canonicalized path length mis-calculation */
	report_canon_err(PATH_ERR_WRONG_LEN, sanity_p, len_p, depth_p, path, array);
	dbg(DBG_HIGH, "%s: error #1 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	return NULL;
    }
    ret_path[path_len] = '\0'; /* end of string paranoia */

    /*
     * fill in return stats as required
     */
    if (sanity_p != NULL) {
	*sanity_p = PATH_OK;
    }
    if (len_p != NULL) {
	*len_p = 1;
    }
    if (depth_p != NULL) {
	*depth_p = 0;
    }

    /*
     * free stack storage
     */
    if (path != NULL) {
	free(path);
	path = NULL;
    }
    dyn_array_free(array);
    array = NULL;

    /*
     * return a sand and canonical path
     */
    return ret_path;
}


/*
 * path_has_component - test if path has a directory component by the name name
 *
 * given:
 *      path        - complete path
 *      name        - name to check each component for
 *
 * NOTE: this function will not return on a NULL pointer.
 *
 * NOTE: this function skips consecutive '/'s.
 */
bool
path_has_component(char const *path, char const *name)
{
    char const *next = NULL;      /* next component */
    char *path_dup = NULL;  /* we have to duplicate the path for strtok_r() */
    char *endptr = NULL;    /* for strtok_r() */

    /*
     * firewall
     */
    if (path == NULL) {
        err(198, __func__, "path is NULL");
        not_reached();
    }
    if (name == NULL) {
        err(199, __func__, "name is NULL");
        not_reached();
    }

    errno = 0;      /* pre-clear errno for errp() */
    path_dup = strdup(path);
    if (path_dup == NULL) {
        errp(200, __func__, "duplicating %s failed", path);
        not_reached();
    }

    next = strtok_r(path_dup, "/", &endptr);
    if (next != NULL) {
        if (!strcmp(next, name)) {
            dbg(DBG_HIGH, "path component %s == name %s", next, name);
            free(path_dup);
            path_dup = NULL;
            return true;
        }
        while ((next = strtok_r(NULL, "/", &endptr)) != NULL) {
            if (!strcmp(next, name)) {
                dbg(DBG_HIGH, "path component %s == name %s", next, name);
                free(path_dup);
                path_dup = NULL;
                return true;
            }
        }
    }

    if (path_dup != NULL) {
        free(path_dup);
        path_dup = NULL;
    }

    return false;
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
	err(201, __func__, "filename is NULL");
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
	    errp(202, __func__, "strdup of filename failed: %s", filename);
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
	    errp(203, __func__, "calloc of %zu bytes failed", len);
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
	    errp(204, __func__, "snprintf returned: %zu < 0", len);
	    not_reached();
	}
    }

    /*
     * return calloc path
     */
    if (buf == NULL) {
	errp(205, __func__, "function attempted to return NULL");
	not_reached();
    }
    return buf;
}
