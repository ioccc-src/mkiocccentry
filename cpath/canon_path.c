/*
 * canon_path - canonicalize paths
 *
 * "... not all those who wander are lost."
 *
 *      -- J.R.R. Tolkien
 *
 * Copyright (c) 1991,2008,2014-2016,2022-2025 by Landon Curt Noll.  All Rights Reserved.
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
 * This code was developed between 1991-2025 by Landon Curt Noll:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
 *
 * We gratefully acknowledge the concept contributions, plus the documentation
 * corrections, and other suggestions made by Cody Boone Ferguson:
 *
 *	@xexyl
 *	https://xexyl.net
 *	https://ioccc.xexyl.net
 *
 * for whom the first part of the above quote applies:
 *
 * "All that is gold does not glitter, ..."
 *
 *      -- J.R.R. Tolkien
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


/*
 * cpath - canonicalize a path
 */
#include "cpath.h"


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
	    *(int_least32_t *)(depth_p) = deep; \
	} \
	if ((path) != NULL) { \
	    free(path); \
	    (path) = NULL; \
	} \
	if ((array) != NULL) { \
	    dyn_array_free(array); \
	    (array) = NULL; \
	} \
    }


#define PATH_INITIAL_SIZE (16)	/* initially allocate this many pointers */
#define PATH_CHUNK_SIZE (8)	/* grow dynamic array by this many pointers at a time */

/*
 * global static variables
 */
const char *const cpath_version = CPATH_VERSION;	/* format: major.minor YYYY-MM-DD */


/*
 * static functions
 */
static size_t private_strlcat(char *dst, const char *src, size_t dsize);


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
        case PATH_ERR_UNSET:
            str ="PATH_ERR_UNSET";
            break;
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
        case PATH_ERR_NOT_SAFE:
            str = "PATH_ERR_NOT_SAFE";
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
            str = "unknown_sanity_valud";
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
        case PATH_ERR_UNSET:
            str = "sanity value is unset";
            break;
        case PATH_OK:
            str = "path successfully canonicalized";
            break;
        case PATH_ERR_PATH_IS_NULL:
            str = "path string is NULL";
            break;
        case PATH_ERR_PATH_EMPTY:
            str = "path string length <= 0";
            break;
        case PATH_ERR_PATH_TOO_LONG:
            str = "canonicalized path is too long";
            break;
        case PATH_ERR_NOT_RELATIVE:
            str = "path not a relative path";
            break;
        case PATH_ERR_NAME_TOO_LONG:
            str = "a canonicalized path component is too long";
            break;
        case PATH_ERR_PATH_TOO_DEEP:
            str = "canonicalized path depth is too deep";
            break;
        case PATH_ERR_NOT_SAFE:
            str = "path component unsafe";
            break;
        case PATH_ERR_DOTDOT_OVER_TOPDIR:
            str = ".. (dot-dot) path component moved above topdir";
            break;
        case PATH_ERR_MALLOC:
            str = "malloc related failure during path processing (internal error)";
            break;
        case PATH_ERR_NULL_COMPONENT:
            str = "component on path stack is NULL (internal error)";
            break;
        case PATH_ERR_WRONG_LEN:
            str = "constructed canonical path has the wrong length (internal error)";
            break;
        default:
            str = "invalid path_sanity value";
            break;
    }

    return str;
}


/*
 * safe_str - test if a string is safe
 *
 * A safe string contains ('+-._'), and either ANYcase alphanumeric or lowercase alphanumeric,
 * depending on any_case, and perhaps '/' (slash) depending on slash_ok.
 *
 * given:
 *	str	    string to test
 *	any_case    true ==> UPPER CASE and lower case are allowed
 *		    false ==> only lower case allowed
 *	slash_ok    true ==> '/' (slash) allowed
 *		    false ==> '/' (slash) are NOT allowed
 *
 * If any_case is true,  slash_ok is true,  then str must match: ^[/0-9A-Za-z._+-]+$
 *
 * If any_case is true,  slash_ok is false, then str must match: ^[0-9A-Za-z._+-]+$
 *
 * If any_case is false, slash_ok is true,  then str must match: ^[/0-9a-z._+-]+$
 *
 * If any_case is false, slash_ok is false, then str must match: ^[0-9a-z._+-]+$
 *
 * return:
 *	true ==> str is safe
 *	false ==> str is NOT safe, or str is an empty string, or str is NULL
 */
bool
safe_str(char const *str, bool any_case, bool slash_ok)
{
    size_t len;		/* length of str */
    size_t ret;		/* strspn() return - number of allowed characters in str */
    char const *accept;	/* set of characters allowed in str */

    /*
     * firewall
     */
    if (str == NULL) {
	warn("%s: str is NULL", __func__);
	return false;
    }

    /*
     * empty strings are NOT safe
     */
    len = strlen(str);
    if (len <= 0) {
	return false;
    }

    /*
     * determine the allowed character set
     */
    if (any_case) {
	/* case: safe ('+-._'), '/' (slash) allowed, and ANYcase alphanumeric allowed */
	if (slash_ok) {
	    accept = "+-."
		     "/"
		     "0123456789"
		     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		     "_"
		     "abcdefghijklmnopqrstuvwxyz";
	/* case: safe ('+-._'), but NOT '/' (slash) allowed, and ANYcase alphanumeric allowed */
	} else {
	    accept = "+-."
		     "0123456789"
		     "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		     "_"
		     "abcdefghijklmnopqrstuvwxyz";
	}
    } else {
	/* case: safe ('+-._'), '/' (slash) allowed, and lowercase alphanumeric allowed */
	if (slash_ok) {
	    accept = "+-."
		     "/"
		     "0123456789"
		     "_"
		     "abcdefghijklmnopqrstuvwxyz";
	/* case: safe ('+-._'), but NOT '/' (slash) allowed, and lowercase alphanumeric allowed */
	} else {
	    accept = "+-."
		     "0123456789"
		     "_"
		     "abcdefghijklmnopqrstuvwxyz";
	}
    }

    /*
     * determine if the string only contains allowed characters
     */
    ret = strspn(str, accept);

    /*
     * string is case if all characters in string are allowed
     */
    if (ret == len) {
	return true;
    }
    return false;
}


/*
 * safe_path_str - test if a path string is safe
 *
 * A path string starts with an ANYcase alphanumeric or lowercase alphanumeric character,
 * depending on any_case, or '_' (underscore), or '-' (dash),
 * or with '/' (slash) depending on slash_ok.
 *
 * After the 1st path character, a safe path string contains ('+-._'), and either
 * ANYcase alphanumeric or lowercase alphanumeric characters, depending on any_case,
 * and perhaps '/' (slash) depending on slash_ok.
 *
 * given:
 *	path_str	    string to test
 *	any_case    true ==> UPPER CASE and lower case are allowed
 *		    false ==> only lower case allowed
 *	slash_ok    true ==> '/' (slash) allowed
 *		    false ==> '/' (slash) are NOT allowed
 *
 * If any_case is true,  slash_ok is true,  then str must match: ^[/0-9A-Za-z._][/0-9A-Za-z._+-]*$
 *
 * If any_case is true,  slash_ok is false, then str must match: ^[0-9A-Za-z._][0-9A-Za-z._+-]*$
 *
 * If any_case is false, slash_ok is true,  then str must match: ^[/0-9a-z._][/0-9a-z._+-]*$
 *
 * If any_case is false, slash_ok is false, then str must match: ^[0-9a-z._][0-9a-z._+-]*$
 *
 * return:
 *	true ==> path_str is safe
 *	false ==> path_str is NOT safe, or str is an empty string, or str is NULL
 */
bool
safe_path_str(char const *path_str, bool any_case, bool slash_ok)
{
    bool safe;		/* if beyond 1st character is safe */

    /*
     * firewall
     */
    if (path_str == NULL) {
	warn("%s: path_str is NULL", __func__);
	return false;
    }

    /*
     * test the first character
     */
    if (path_str[0] == '\0') {
	/* empty string is not considered safe */
	return false;
    } else if (path_str[0] == '/' && slash_ok == false) {
	/* string starts with '/' (slash), however slash_ok is false, thus is NOT safe */
	return false;
    } else if (!isascii(path_str[0])) {
	/* string starts with a non-ASCII character, thus is NOT safe */
	return false;
    } else if (path_str[0] != '.' && path_str[0] != '_' &&
	      any_case == true && !isalnum(path_str[0])) {
	/* string does NOT start with an UPPERcase character NOR a lowercase character, NOR a digit, thus is NOT safe */
	return false;
    } else if (path_str[0] != '.' && path_str[0] != '_' &&
	       any_case == false && !(islower(path_str[0]) || isdigit(path_str[0]))) {
	/* string does NOT start with a lowercase character, NOR a digit, thus is NOT safe */
	return false;
    }
    /* assertion: 1st character of path_str is safe */

    /*
     * if path_str is a single character, then path_str is safe
     */
    if (path_str[1] == '\0') {
	return true;
    }

    /*
     * determine if the rest of path_str is safe
     */
    safe = safe_str(path_str+1, any_case, slash_ok);

    /*
     * if rest of path_str is safe, then path_str is safe, otherwise it is NOT
     */
    return safe;
}


/*
 * depth - Examples of path depths are as follows:
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
 */


/*
 * canon_path - canonicalize a path
 *
 * given:
 *	orig_path	    - path to canonicalize
 *      max_path_len        - max canonicalized path length, 0 ==> no limit
 *	max_filename_len    - max length of each component of path, 0 ==> no limit
 *      max_depth           - max depth of subdirectory path, 0 ==> no limit, <0 ==> reserved for future use
 *      sanity_p	    - NULL ==> don't save canon_path path_sanity error, or PATH_OK
 *			      != NULL ==> save enum path_sanity in *sanity_p
 *      len_p		    - NULL ==> don't save canonical path length,
 *		              != NULL ==> save canonical path length in *len_p
 *      depth_p		    - NULL ==> don't save canonical depth,
 *			      != NULL ==> record canonical depth in *depth_p
 *      rel_only	    - true ==> path from "/" (slash) NOT allowed, path depth counted from implied "." (dot)
 *			      false ==> path from "/" (slash) allowed, path depth counted from /
 *	lower_case	    - true ==> convert UPPER CASE to lower case during canonicalization
 *			      false ==> don't change path case
 *	safe_chk	    - true ==> test each canonical path component for safety
 *			      false ==> do not perform path safety tests on the path
 *	dotdot_err	    - true ==> return PATH_ERR_DOTDOT_OVER_TOPDIR if .. (dot-dot) moves before start of path
 *			      false ==> keep .. (dot_dot) in canonicalized path when it moves before start of path
 *	preg		    - NULL ==> safe component check via safe_path_str() for ^[0-9A-Za-z._][0-9A-Za-z._+-]*$
 *			      != NULL ==> safe component check via regcomp(3) compiled regular expression
 *
 * returns:
 *	NULL ==> invalid path, internal error, or NULL pointer used
 *	!= NULL ==> malloced canonicalized path
 *
 * NOTE: When NULL is returned:                     if sanity_p is non-NULL, then *sanity_p will be set != PATH_OK.
 *	 When a non-NULL malloced path is returned: if sanity_p is non-NULL, then *sanity_p == PATH_OK.
 */
char *
canon_path(char const *orig_path,
	   size_t max_path_len, size_t max_filename_len, int_least32_t max_depth,
	   enum path_sanity *sanity_p, size_t *len_p, int_least32_t *depth_p,
	   bool rel_only, bool lower_case, bool safe_chk, bool dotdot_err,
	   const regex_t *restrict preg)
{
    char *path = NULL;		/* duplicated orig_path to be/that has been canonicalized */
    size_t path_len = 0;	/* full path length */
    size_t tmp_len = 0;		/* temporary path length */
    size_t comp_len = 0;	/* path component length */
    enum path_sanity sanity = PATH_ERR_UNSET;	/* canon_path path_sanity error, or PATH_OK */
    bool relative = true;		/* true ==> path is relative to "." (dot), false ==> path is absolute */
    struct dyn_array *array = NULL;	/* dynamic array of pointers to strings - path component stack */
    char *p = NULL;		/* path component */
    char **q = NULL;		/* address of a dynamic array string element */
    int_least32_t deep = 0;	/* path depth (see note above this function) */
    bool test = true;		/* true ==> passed test, false == failed test */
    int regexec_ret = 0;	/* regexec(3) return code */
    char *ret_path = NULL;	/* malloced canonicalized path to return */
    size_t strlcpy_ret = 0;	/* private_strlcpy() return value */
    size_t i = 0;		/* path component number */

    /*
     * firewall
     */
    if (orig_path == NULL) {

	/* orig_path is NULL error */
	dbg(DBG_V2_HIGH, "%s: error #0: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	report_canon_err(PATH_ERR_PATH_IS_NULL, sanity_p, len_p, depth_p, path, array);
	return NULL;
    }

    /*
     * case: empty path
     */
    if (orig_path[0] == '\0') {

	/* orig_path is an empty string error */
	dbg(DBG_V2_HIGH, "%s: error #1: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	report_canon_err(PATH_ERR_PATH_EMPTY, sanity_p, len_p, depth_p, path, array);
	return NULL;

    /*
     * case: orig_path is absolute
     */
    } else if (orig_path[0] == '/') {
	if (rel_only) {

	    /* orig_path is absolute, but rel_only is true */
	    dbg(DBG_V2_HIGH, "%s: error #2: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    report_canon_err(PATH_ERR_NOT_RELATIVE, sanity_p, len_p, depth_p, path, array);
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
	dbg(DBG_V2_HIGH, "%s: error #3 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
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
	 * if .. (dot-dot), attempt to pop the path component stack
	 */
	} else if (strcmp(p, "..") == 0) {

	    /*
	     * case: component stack is empty
	     */
	    deep = dyn_array_tell(array);
	    if (deep <= 0) {

		/*
		 * case: return PATH_ERR_DOTDOT_OVER_TOPDIR if .. (dot-dot) moves above topdir
		 */
		if (dotdot_err) {

		    /* path component stack underflow */
		    dbg(DBG_V2_HIGH, "%s: error #4: path component stack underflow error %s: %s",
			__func__, path_sanity_name(sanity), path_sanity_error(sanity));
		    report_canon_err(PATH_ERR_DOTDOT_OVER_TOPDIR, sanity_p, len_p, depth_p, path, array);
		    return NULL;

		/*
		 * case: for a relative path that is now at the beginning of the path, we push .. (dot-dot) if possible
		 */
		} else if (relative) {

		    /*
		     * check path component if max_filename_len > 0
		     */
		    if (max_filename_len > 0) {

			/*
			 * be sure that the path component is not too long
			 */
			if (comp_len > max_filename_len) {

			    /* path component too long */
			    dbg(DBG_V3_HIGH, "%s: error #5: path component length: %zu > max_filename_len: %zu",
				__func__, comp_len, max_filename_len);
			    dbg(DBG_V2_HIGH, "%s: error #6: path component too long %s: %s",
				__func__, path_sanity_name(sanity), path_sanity_error(sanity));
			    report_canon_err(PATH_ERR_NAME_TOO_LONG, sanity_p, len_p, depth_p, path, array);
			    return NULL;
			}
		    }

		    /*
		     * push component onto the path stack
		     *
		     * NOTE: If the path stack were to grow to larger than INT32_MAX,
		     *	     and if we have a maximum depth allowed, we will declare
		     *	     an immediate PATH_ERR_PATH_TOO_DEEP, even if some later
		     *	     .. (dot-dot) might be able to later reduce the depth.
		     */
		    test = dyn_array_push(array, p);
		    deep = dyn_array_tell(array);
		    if (max_depth > 0 && deep > INT32_MAX) {

			/* path component too deep */
			dbg(DBG_V3_HIGH, "%s: error #7: path depth: %d max_depth: %d", __func__, deep, max_depth);
			dbg(DBG_V2_HIGH, "%s: error #8: while canonicalizing, path became too deep %s: %s",
			    __func__, path_sanity_name(sanity), path_sanity_error(sanity));
			report_canon_err(PATH_ERR_PATH_TOO_DEEP, sanity_p, len_p, depth_p, path, array);
			return NULL;
		    }
		    dbg(DBG_V3_HIGH, "%s: #0: pushed path component on stack, depth: %d", __func__, deep);
		    dbg(DBG_V4_HIGH, "%s: #0: data moved: %s", __func__, booltostr(test));

		/*
		 * case: For an absolute path that is now at /, we simply toss this .. (dot-dot).
		 *	 This is because at the / (slash) root, .. (dot-dot) is linked to the / (slash) root.
		 */
		} else {
		    dbg(DBG_V3_HIGH, "%s: at the / (slash) root, so we toss this .. (dot-dot)", __func__);
		}

	    /*
	     * consider popping the non-empty path component stack due to .. (dot-dot)
	     *
	     * NOTE: If the top (i.e., the previous path) the path component stack is .. (dot-dot),
	     *	     then we do NOT pop the top but rather push a new . (dot-dot) onto the stack.
	     *
	     * case: deep > 0
	     */
	    } else {

		char *top;	    /* top of component stack */

		/*
		 * obtain top the component stack
		 */
		top = dyn_array_value(array, char *, deep-1);
		if (top == NULL) {
		    /* NULL component pointer */
		    dbg(DBG_V2_HIGH, "%s: error #9: %s  %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
		    report_canon_err(PATH_ERR_NULL_COMPONENT, sanity_p, len_p, depth_p, path, array);
		    return NULL;
		}

		/*
		 * case: top (i.e., the previous path) the component stack is .. (dot-dot)
		 *
		 * We let the .. (dot-dot) component on the component stack stay and push,
		 * if possible, another .. (dot-dot) component onto the path stack
		 */
		if (strcmp(top, "..") == 0) {

		    /*
		     * check path component if max_filename_len > 0
		     */
		    if (max_filename_len > 0) {

			/*
			 * be sure that the path component is not too long
			 */
			if (comp_len > max_filename_len) {

			    /* path component too long */
			    dbg(DBG_V3_HIGH, "%s: error #10: path component length: %zu > max_filename_len: %zu",
				__func__, comp_len, max_filename_len);
			    dbg(DBG_V2_HIGH, "%s: error #11: path component too long %s: %s",
				__func__, path_sanity_name(sanity), path_sanity_error(sanity));
			    report_canon_err(PATH_ERR_NAME_TOO_LONG, sanity_p, len_p, depth_p, path, array);
			    return NULL;
			}
		    }

		    /*
		     * push component onto the path stack
		     *
		     * NOTE: If the path stack were to grow to larger than INT32_MAX,
		     *	     and if we have a maximum depth allowed, we will declare
		     *	     an immediate PATH_ERR_PATH_TOO_DEEP, even if some later
		     *	     .. (dot-dot) might be able to later reduce the depth.
		     */
		    test = dyn_array_push(array, p);
		    deep = dyn_array_tell(array);
		    if (max_depth > 0 && deep > INT32_MAX) {

			/* path component too deep */
			dbg(DBG_V3_HIGH, "%s: error #12: path depth: %d max_depth: %d", __func__, deep, max_depth);
			dbg(DBG_V2_HIGH, "%s: error #13: while canonicalizing, path became too deep %s: %s",
			    __func__, path_sanity_name(sanity), path_sanity_error(sanity));
			report_canon_err(PATH_ERR_PATH_TOO_DEEP, sanity_p, len_p, depth_p, path, array);
			return NULL;
		    }
		    dbg(DBG_V3_HIGH, "%s: #1: pushed path component on stack, depth: %d", __func__, deep);
		    dbg(DBG_V4_HIGH, "%s: #1: data moved: %s", __func__, booltostr(test));

		/*
		 * case: top (i.e., the previous path) the component stack is NOT .. (dot-dot)
		 *
		 * We let the component .. (dot-dot) to pop the previous previous path component from the stack
		 */
		} else {
		    deep = (int_least32_t)dyn_array_pop(array, NULL);
		    dbg(DBG_V3_HIGH, "%s: .. component stack pop, stack depth: %d", __func__, deep);
		}
	    }

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
		    dbg(DBG_V3_HIGH, "%s: error #14: path component length: %zu > max_filename_len: %zu",
			__func__, comp_len, max_filename_len);
		    dbg(DBG_V2_HIGH, "%s: error #15: path component too long %s: %s",
			__func__, path_sanity_name(sanity), path_sanity_error(sanity));
		    report_canon_err(PATH_ERR_NAME_TOO_LONG, sanity_p, len_p, depth_p, path, array);
		    return NULL;
		}
	    }

	    /*
	     * convert to lower case if lower_case
	     */
	    if (lower_case) {

		/*
		 * convert UPPER case to lower case
		 */
		for (i=0; i < comp_len; ++i) {
		    p[i] = tolower(p[i]);
		}
		dbg(DBG_V3_HIGH, "%s: lower case path component[%zu]: %s", __func__, i, p);
	    }

	    /*
	     * push component onto the path stack
	     *
	     * NOTE: If the path stack were to grow to larger than INT32_MAX,
	     *	     and if we have a maximum depth allowed, we will declare
	     *	     an immediate PATH_ERR_PATH_TOO_DEEP, even if some later
	     *	     .. (dot-dot) might be able to later reduce the depth.
	     */
	    test = dyn_array_push(array, p);
	    deep = dyn_array_tell(array);
	    if (max_depth > 0 && deep > INT32_MAX) {

		/* path component too deep */
		dbg(DBG_V3_HIGH, "%s: error #16: path depth: %d max_depth: %d", __func__, deep, max_depth);
		dbg(DBG_V2_HIGH, "%s: error #17: while canonicalizing, path became too deep %s: %s",
		    __func__, path_sanity_name(sanity), path_sanity_error(sanity));
		report_canon_err(PATH_ERR_PATH_TOO_DEEP, sanity_p, len_p, depth_p, path, array);
		return NULL;
	    }
	    dbg(DBG_V3_HIGH, "%s: #2: pushed path component on stack, depth: %d", __func__, deep);
	    dbg(DBG_V4_HIGH, "%s: #2: data moved: %s", __func__, booltostr(test));
	}
    }

    /* assertion: the path stack contains the component of the canonicalized stack */

    /*
     * check depth if max_depth > 0
     */
    if (max_depth > 0 && deep > max_depth) {

	/* path component too deep */
	dbg(DBG_V3_HIGH, "%s: error #18: path depth: %d max_depth: %d", __func__, deep, max_depth);
	dbg(DBG_V2_HIGH, "%s: error #19: path component too is deep %s: %s",
	    __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	report_canon_err(PATH_ERR_PATH_TOO_DEEP, sanity_p, len_p, depth_p, path, array);
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
	    dbg(DBG_V2_HIGH, "%s: error #20: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    report_canon_err(PATH_ERR_NULL_COMPONENT, sanity_p, len_p, depth_p, path, array);
	    return NULL;
	}

	/*
	 * case: test each canonical path component is safe
	 */
	if (safe_chk) {

	    /*
	     * case: preg is NULL
	     *
	     * Use safe_path_str(*q, true, false) to check the canonical path component for:
	     *
	     *	    ^[0-9A-Za-z._][0-9A-Za-z._+-]*$
	     */
	    if (preg == NULL) {

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
		test = safe_path_str(*q, true, false);
		if (! test) {

		    /* path component is unsafe */
		    dbg(DBG_V2_HIGH, "%s: error #21: path component is not safe: %s", __func__, *q);
		    report_canon_err(PATH_ERR_NOT_SAFE, sanity_p, len_p, depth_p, path, array);
		    return NULL;
		}

	    /*
	     * case: use the compiled regular expression preg
	     *
	     * We use the compiled regular expression check the canonical path component.
	     */
	    } else {

		/*
		 * test the canonical path component with the compiled regular expression
		 */
		regexec_ret = regexec(preg, *q, 0, NULL, 0);
		if (regexec_ret != 0) {

		    /* path component is unsafe */
		    dbg(DBG_V2_HIGH, "%s: error #22: path component is not safe: %s", __func__, *q);

		    /*
		     * If the compiled regular expression failed, instead of simply failed
		     * to match, print the regexec() error message if debugging is high enough.
		     */
		    if (regexec_ret != REG_NOMATCH && dbg_allowed(DBG_V2_HIGH)) {

			char errbuf[BUFSIZ+1];	    /* regerror() message buffer */

			memset(errbuf, 0, sizeof(errbuf));
			(void) regerror(regexec_ret, preg, errbuf, BUFSIZ);
			dbg(DBG_V2_HIGH, "%s: regexec error: %s", __func__, errbuf);
		    }
		    report_canon_err(PATH_ERR_NOT_SAFE, sanity_p, len_p, depth_p, path, array);
		    return NULL;
		}
	    }
	}

	/* sum component length */
	path_len += strlen(*q);
    }

    /*
     * check canonicalized path length if max_path_len > 0
     */
    if (max_path_len > 0 && path_len > max_path_len) {

	/* path too long */
	dbg(DBG_V2_HIGH, "%s: error #23: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	report_canon_err(PATH_ERR_PATH_TOO_LONG, sanity_p, len_p, depth_p, path, array);
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
		dbg(DBG_V2_HIGH, "%s: error #24: strdup(\".\") error: %s: %s", __func__,
				 path_sanity_name(sanity), path_sanity_error(sanity));
		report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
		return NULL;
	    }

	/*
	 * case: path is absolute, return "/" (slash)
	 */
	} else {
	    /* return "/" (slash) */
	    ret_path = strdup("/");
	    if (ret_path == NULL) {
		dbg(DBG_V2_HIGH, "%s: error #25: strdup(\"/\") error: %s: %s", __func__,
				 path_sanity_name(sanity), path_sanity_error(sanity));
		report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
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
	dbg(DBG_V2_HIGH, "%s: error #26: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	report_canon_err(PATH_ERR_MALLOC, sanity_p, len_p, depth_p, path, array);
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
	    dbg(DBG_V2_HIGH, "%s: error #27 %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    report_canon_err(PATH_ERR_NULL_COMPONENT, sanity_p, len_p, depth_p, path, array);
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
	    dbg(DBG_V2_HIGH, "%s: error #28: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	    report_canon_err(PATH_ERR_WRONG_LEN, sanity_p, len_p, depth_p, path, array);
	    return NULL;
	}
    }
    /* canonicalized path length sanity check */
    if (path_len != strlen(ret_path)) {
	/* canonicalized path length mis-calculation */
	dbg(DBG_V2_HIGH, "%s: error #29: %s: %s", __func__, path_sanity_name(sanity), path_sanity_error(sanity));
	report_canon_err(PATH_ERR_WRONG_LEN, sanity_p, len_p, depth_p, path, array);
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
	*len_p = path_len;
    }
    if (depth_p != NULL) {
	*depth_p = deep;
    }

    /*
     * free duplicated orig_path
     */
    if (path != NULL) {
	free(path);
	path = NULL;
    }

    /*
     * free stack storage
     */
    if (array != NULL) {
	dyn_array_free(array);
	array = NULL;
    }

    /*
     * return a sand and canonical path
     */
    return ret_path;
}
