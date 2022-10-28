/*
 * util - entry common utility functions for the IOCCC toolkit
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Copyright (c) 2021,2022 by Landon Curt Noll.  All Rights Reserved.
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
 * LANDON CURT NOLL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL LANDON CURT NOLL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * Share and enjoy! :-)
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

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * hexval - convert ASCII character to hex value
 *
 * NOTE: -1 means the ASCII character is not a valid hex character
 */
int const hexval[BYTE_VALUES] = {
    /* \x00 - \x0f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x10 - \x1f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x20 - \x2f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x30 - \x3f */
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    /* \x40 - \x4f */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x50 - \x5f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x60 - \x6f */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x70 - \x7f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x80 - \x8f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x90 - \x9f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xa0 - \xaf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xb0 - \xbf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xc0 - \xcf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xd0 - \xdf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xe0 - \xef */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xf0 - \xff */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


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
 * NOTE: It is the caller's responsibility to free the returned string when it
 * is no longer needed.
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
    if (path == NULL) {
	err(100, __func__, "called with NULL arg(s)"); /*coo*/
	not_reached();
    }

    /*
     * duplicate the path for basename processing
     */
    errno = 0;			/* pre-clear errno for errp() */
    copy = strdup(path);
    if (copy == NULL) {
	errp(101, __func__, "strdup(%s) failed", path);
	not_reached();
    }

    /*
     * case: basename of empty string is an empty string
     */
    len = strlen(copy);
    if (len == 0) {
	dbg(DBG_VVHIGH, "#0: basename of path:%s is an empty string", path);
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
	dbg(DBG_VVHIGH, "#1: basename(%s) == %s", path, copy);
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
	dbg(DBG_VVHIGH, "#2: basename(%s) == %s", path, copy);
	return copy;
    }

    /*
     * duplicate the new string to return
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = strdup(p + 1);
    if (ret == NULL) {
	errp(102, __func__, "strdup(%s) failed", p + 1);
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
    dbg(DBG_VVHIGH, "#3: basename(%s) == %s", path, ret);
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
 *      true ==> path exists,
 *      false ==> path does not exist
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
	err(103, __func__, "called with NULL arg(s)");
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);
    return true;
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
	err(104, __func__, "called with NULL arg(s)");
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);

    /*
     * test if path is a regular file
     */
    if (!S_ISREG(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a regular file", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a regular file", path);
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
	err(105, __func__, "called with NULL arg(s)");
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
	dbg(DBG_VHIGH, "path is a directory: %s size: %jd", path, (intmax_t)buf.st_size);
    } else {
	dbg(DBG_VHIGH, "path is a file: %s size: %jd", path, (intmax_t)buf.st_size);
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
	dbg(DBG_VHIGH, "path %s is searchable", path);
    } else {
	dbg(DBG_VHIGH, "path %s is executable", path);
    }
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
	err(106, __func__, "called with NULL arg(s)");
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);

    /*
     * test if path is a regular directory
     */
    if (!S_ISDIR(buf.st_mode)) {
	dbg(DBG_HIGH, "path %s is not a directory", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is a directory", path);
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
	err(107, __func__, "called with NULL arg(s)");
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, R_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not readable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is readable", path);
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
	err(108, __func__, "called with NULL arg(s)");
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, W_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not writable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is writable", path);
    return true;
}


/*
 * is_open_stream - determine if a file stream is open
 *
 * given:
 *	stream	stream to read if open
 *
 * returns:
 *      true ==> stream is open
 *      false ==> stream is NULL or nor open
 */
bool
is_open_stream(FILE *stream)
{
    long pos = 0;	/* stream position */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return false;
    }

    /*
     * determine stream position
     *
     * The ftell() function will fail if stream is not open.
     */
    pos = ftell(stream);
    if (pos < 0) {
	return false;
    }

    /*
     * we know that stream is non-NULL and open
     */
    return true;
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
	err(109, __func__, "called with NULL arg(s)");
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);
    return buf.st_size;
}


/*
 * cmdprintf - malloc a safer shell command line for use with system() and popen()
 *
 * For each % in the format, the next argument from the list argument list (which
 * is assumed to be of type char *) is processed, escaping special characters that
 * the shell might threaten as command characters.  If no special characters are
 * found, no escaping is performed.
 *
 * NOTE: In the worst case, the length of the command line will double.
 *
 * given:
 *      fmt	shell command where % character are replaced with shell escaped args
 *      ...     args (assumed to be of type char *) to use with %'s in fmt
 *
 * returns:
 *	allocated shell command line, or
 *	NULL ==> error
 *
 * NOTE: It is the caller's responsibility to free the returned string when it
 * is no longer needed.
 */
char *
cmdprintf(char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    char *cmd = NULL;

    /*
     * firewall
     */
    if (fmt == NULL) {
	warn(__func__, "fmt is NULL");
	return NULL;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * call vcmdprintf()
     */
    cmd = vcmdprintf(fmt, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * return safer command line string
     */
    return cmd;
}


/*
 * vcmdprintf - malloc a safer shell command line for use with system() and popen() in va_list form
 *
 * This is a va_list form of cmdprintf().
 *
 * For each % in the format, the next argument from the list argument list (which
 * is assumed to be of type char *) is processed, escaping special characters that
 * the shell might threaten as command characters.  If no special characters are
 * found, no escaping is performed.
 *
 * NOTE: In the worst case, the length of the command line will double.
 *
 * given:
 *      fmt	shell command where % character are replaced with shell escaped args
 *      ap	variable argument list
 *
 * returns:
 *	allocated shell command line, or
 *	NULL ==> error
 *
 * NOTE: This code is based on an enhancement request by GitHub user @ilyakurdyukov:
 *
 *		https://github.com/ioccc-src/mkiocccentry/issues/11
 *
 *	 and this function code was written by him.  Thank you Ilya Kurdyukov!
 *
 *
 * NOTE: It is the caller's responsibility to free the returned string when it
 * is no longer needed.
 */
char *
vcmdprintf(char const *fmt, va_list ap)
{
    va_list ap2;		/* copy of original va_list for first and second pass */
    size_t size = 0;
    char const *next;
    char const *p;
    char const *f;
    char const *esc = "\t\n\r !\"#$&()*;<=>?[\\]^`{|}~";
    char *d;
    char *cmd;
    char c;
    int nquot;

    /*
     * firewall
     */
    if (fmt == NULL) {
	warn(__func__, "fmt is NULL");
	return NULL;
    }

    /*
     * copy va_list for first pass
     */
    va_copy(ap2, ap);

    /*
     * pass 0: determine how much storage we will need for the command line
     */
    f = fmt;
    while ((c = *f++)) {
	if (c == '%') {
	    p = next = va_arg(ap2, char const *);
	    nquot = 0;
	    while ((c = *p++)) {
		if (c == '\'') {
		    /* nquot >= 2: 'x##x' */
		    /* nquot == 1: x\#xx */
		    /* nquot == 0: xxxx */
		    /* +1 for escaping the single quote */
		    size += (size_t)(nquot >= 2 ? 2 : nquot) + 1;
		    nquot = 0;
		} else {
		    /* count the characters need to escape */
		    nquot += strchr(esc, c) != NULL;
		}
	    }
	    /* -2 for excluding counted NUL and */
	    /* counted % sign in the fmt string */
	    size += (size_t)(nquot >= 2 ? 2 : nquot) + (size_t)(p - next) - 2;
	}
    }
    size += (size_t)(f - fmt);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap2);

    /*
     * copy va_list for second pass
     */
    va_copy(ap2, ap);

    /*
     * calloc storage or return NULL
     */
    errno = 0;			    /* pre-clear errno for warnp() */
    cmd = (char *)calloc(1, size);  /* NOTE: the trailing NUL byte is included in size */
    if (cmd == NULL) {
	warnp(__func__, "calloc from vcmdprintf of %ju bytes failed", (uintmax_t)size);
	return NULL;
    }

    /*
     * pass 1: form the safer command line
     */
    d = cmd;
    f = fmt;
    while ((c = *f++)) {
	if (c != '%') {
	    *d++ = c;
	} else {
	    p = next = va_arg(ap2, char const *);
	    nquot = 0;
	    while ((c = *p++)) {
		if (c == '\'') {
		    if (nquot >= 2) {
			*d++ = '\'';
		    }
		    while (next < p - 1) {
			c = *next++;
			/* nquot == 1 means one character needs to be escaped */
			/* quotes around are not used in this mode */
			if (nquot == 1 && strchr(esc, c)) {
			    *d++ = '\\';
			    /* set nquot to zero since we processed it */
			    /* to not call strchr() again */
			    nquot = 0;
			}
			*d++ = c;
		    }
		    if (nquot >= 2) {
			*d++ = '\'';
		    }
		    nquot = 0;
		    next++;
		    *d++ = '\\';
		    *d++ = '\'';
		} else {
		    nquot += strchr(esc, c) != NULL;
		}
	    }

	    if (nquot >= 2) {
		*d++ = '\'';
	    }
	    while (next < p - 1) {
		c = *next++;
		if (nquot == 1 && strchr(esc, c)) {
		    *d++ = '\\';
		    nquot = 0;
		}
		*d++ = c;
	    }
	    if (nquot >= 2) {
		*d++ = '\'';
	    }

	}
    }
    *d = '\0';	/* NUL terminate command line */

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap2);

    /*
     * verify amount of data written
     */
    if ((size_t)(d + 1 - cmd) != size) {
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	warn(__func__, "stored characters: %jd != size: %ju",
	     (intmax_t)((size_t)(d + 1 - cmd)), (uintmax_t)size);
	return NULL;
    }

    /*
     * return safer command line string
     */
    return cmd;
}


/*
 * shell_cmd - pass a command, via vcmdprintf() interface, to the shell
 *
 * Attempt to call the shell with a command string.
 *
 * given:
 *	name		- name of the calling function
 *	abort_on_error	- false ==> return exit code if able to successfully call system(), or
 *			    return CALLOC_FAILED_EXIT malloc() failure,
 *			    return FLUSH_FAILED_EXIT on fflush failure,
 *			    return SYSTEM_FAILED_EXIT if system() failed,
 *			    return NULL_ARGS_EXIT if NULL pointers were passed
 *			  true ==> return exit code if able to successfully call system(), or
 *			   call errp() (and thus exit) if unsuccessful
 *      format		- The format string, any % on this string inserts the
 *			  next string from the list, escaping special characters
 *			  that the shell might threaten as command characters.
 *			  In the worst case, the algorithm will make twice as
 *			  many characters.  Will not use escaping if it isn't
 *			  needed.
 *      ...		- args to give after the format
 *
 * returns:
 *	>= ==> exit code, <0 ==> *_EXIT failure (if flag == false)
 *
 * NOTE: The values *_EXIT are < 0, and therefore do not match a valid exit code.
 *	 Moreover if abort_on_error == false, a simple check if the return was <
 *	 0 will allow the calling function to determine if this function failed.
 */
int
shell_cmd(char const *name, bool abort_on_error, char const *format, ...)
{
    va_list ap;			/* variable argument list */
    char *cmd = NULL;		/* e.g. cp prog.c entry_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (name == NULL) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    err(110, __func__, "function name is not caller name because we were called with NULL name");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called with NULL name, returning: %d < 0", NULL_ARGS_EXIT);
	    return NULL_ARGS_EXIT;
	}
    }
    if (format == NULL) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    err(111, name, "called NULL format");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called with NULL format, returning: %d < 0", NULL_ARGS_EXIT);
	    return NULL_ARGS_EXIT;
	}
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, format);

    /*
     * build a safe shell command
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = vcmdprintf(format, ap);
    if (cmd == NULL) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(112, name, "calloc failed in vcmdprintf()");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: calloc failed in vcmdprintf(): %s, returning: %d < 0",
			 name, strerror(errno), CALLOC_FAILED_EXIT);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return CALLOC_FAILED_EXIT;
	}
    }

    /*
     * pre-flush stdout to avoid system() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	/* free allocated command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(113, name, "fflush(stdout): error code: %d", ret);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: fflush(stdout) failed: %s", name, strerror(errno));
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return FLUSH_FAILED_EXIT;
	}
    }

    /*
     * pre-flush stderr to avoid system() buffered stdio issues
     */
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	/* free allocated command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(114, name, "fflush(stderr): error code: %d", ret);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: fflush(stderr) failed", name);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return FLUSH_FAILED_EXIT;
	}
    }

    /*
     * execute the command
     */
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	/* free allocated command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(115, __func__, "error calling system(%s)", cmd);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: error calling system(%s)", name, cmd);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return SYSTEM_FAILED_EXIT;
	}

    /*
     * case: exit code 127 usually means the fork/exec was unable to invoke the shell
     */
    } else if (exit_code == 127) {
	/* free allocated command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(116, __func__, "execution of the shell failed for system(%s)", cmd);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: execution of the shell failed for system(%s)", name, cmd);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return SYSTEM_FAILED_EXIT;
	}
    }

    /*
     * free allocated command storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * return exit code from system()
     */
    return exit_code;
}


/*
 * pipe_open - pass a command, via vcmdprintf() interface, to the shell
 *
 * Attempt to call the shell with a command string.
 *
 * given:
 *	name		- name of the calling function
 *	abort_on_error	- false ==> return FILE * stream for open pipe to shell, or
 *			    return NULL on failure
 *			  true ==> return FILE * stream for open pipe to shell, or
 *			   call errp() (and thus exit) if unsuccessful
 *      format		- The format string, any % on this string inserts the
 *			  next string from the list, escaping special characters
 *			  that the shell might threaten as command characters.
 *			  In the worst case, the algorithm will make twice as
 *			  many characters.  Will not use escaping if it isn't
 *			  needed.
 *      ...     - args to give after the format
 *
 * returns:
 *	FILE * stream for open pipe to shell, or NULL ==> error
 */
FILE *
pipe_open(char const *name, bool abort_on_error, char const *format, ...)
{
    va_list ap;			/* variable argument list */
    char *cmd = NULL;		/* e.g. cp prog.c entry_dir/prog.c */
    FILE *stream = NULL;	/* open pipe to shell command or NULL */
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (name == NULL) {
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    err(117, __func__, "function name is not caller name because we were called with NULL name");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called with NULL name, returning NULL");
	    return NULL;
	}
    }
    if (format == NULL) {
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    err(118, name, "called NULL format");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called with NULL format, returning NULL");
	    return NULL;
	}
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, format);

    /*
     * build a safe shell command
     */
    errno = 0;			/* pre-clear errno for errp() */
    cmd = vcmdprintf(format, ap);
    if (cmd == NULL) {
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    errp(119, name, "calloc failed in vcmdprintf()");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: calloc failed in vcmdprintf(): %s returning: %d < 0",
			 name, strerror(errno), CALLOC_FAILED_EXIT);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return NULL;
	}
    }

    /*
     * pre-flush stdout to avoid popen() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	/* free allocated command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(120, name, "fflush(stdout): error code: %d", ret);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: fflush(stdout) failed: %s", name, strerror(errno));
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return NULL;
	}
    }

    /*
     * pre-flush stderr to avoid popen() buffered stdio issues
     */
    clearerr(stderr);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stderr);
    if (ret < 0) {
	/* free allocated command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(121, name, "fflush(stderr): error code: %d", ret);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: fflush(stderr) failed: %s", name, strerror(errno));
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return NULL;
	}
    }

    /*
     * establish the open pipe to the shell command
     */
    dbg(DBG_HIGH, "about to perform: popen(%s, \"r\")", cmd);
    errno = 0;			/* pre-clear errno for errp() */
    stream = popen(cmd, "r");
    if (stream == NULL) {
	/* free allocated command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(122, name, "error calling popen(%s, \"r\")", cmd);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: error calling popen(%s, \"r\"): %s", name, cmd, strerror(errno));
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return NULL;
	}
    }

    /*
     * set stream to line buffered
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = setvbuf(stream, (char *)NULL, _IOLBF, 0);
    if (ret != 0) {
	warnp(name, "setvbuf failed for %s", cmd);
    }

    /*
     * free allocated command storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * return open pipe stream
     */
    return stream;
}


/*
 * para - print a paragraph of lines to stdout
 *
 * Print a collection of strings with newlines added after each string.
 * The final string pointer must be a NULL.
 *
 * Example:
 *      para("line 1", "line 2", "", "prev line 3 was an empty line", NULL);
 *
 * given:
 *      line    - first paragraph line to print
 *      ...     - strings as paragraph lines to print
 *      NULL    - end of string list
 *
 * This function does not return on error.
 */
void
para(char const *line, ...)
{
    va_list ap;			/* variable argument list */
    int ret;			/* libc function return value */
    int fd;			/* stdout as a file descriptor or -1 */
    int line_count;		/* number of lines in the paragraph */

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, line);

    /*
     * firewall
     */
    if (stdout == NULL) {
	err(123, __func__, "stdout is NULL");
	not_reached();
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    /*
     * this may not always catch a bogus or unopened stdout, but try anyway
     */
    fd = fileno(stdout);
    if (fd < 0) {
	errp(124, __func__, "fileno on stdout returned: %d < 0", fd);
	not_reached();
    }
    clearerr(stdout);		/* paranoia */

    /*
     * print paragraph strings followed by newlines
     */
    line_count = 0;
    while (line != NULL) {

	/*
	 * print the string
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputs(line, stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(125, __func__, "error writing paragraph to a stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(126, __func__, "EOF while writing paragraph to a stdout");
		not_reached();
	    } else {
		errp(128, __func__, "unexpected fputs error writing paragraph to a stdout");
		not_reached();
	    }
	}

	/*
	 * print the newline
	 */
	clearerr(stdout);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputc('\n', stdout);
	if (ret == EOF) {
	    if (ferror(stdout)) {
		errp(129, __func__, "error writing newline to a stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(130, __func__, "EOF while writing newline to a stdout");
		not_reached();
	    } else {
		errp(131, __func__, "unexpected fputc error newline a stdout");
		not_reached();
	    }
	}
	++line_count;		/* count this line as printed */

	/*
	 * move to next line string
	 */
	line = va_arg(ap, char *);
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * flush the paragraph onto the stdout
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret == EOF) {
	if (ferror(stdout)) {
	    errp(132, __func__, "error flushing stdout");
	    not_reached();
	} else if (feof(stdout)) {
	    err(133, __func__, "EOF while flushing stdout");
	    not_reached();
	} else {
	    errp(134, __func__, "unexpected fflush error while flushing stdout");
	    not_reached();
	}
    }
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __func__, line_count);
    return;
}


/*
 * fpara - print a paragraph of lines to an open stream
 *
 * Print a collection of strings with newlines added after each string.
 * The final string pointer must be a NULL.
 *
 * Example:
 *      fpara(stderr, "line 1", "line 2", "", "prev line 3 was an empty line", NULL);
 *
 * given:
 *      stream  - open file stream to print a paragraph onto
 *      line    - first paragraph line to print
 *      ...     - strings as paragraph lines to print
 *      NULL    - end of string list
 *
 * This function does not return on error.
 */
void
fpara(FILE * stream, char const *line, ...)
{
    va_list ap;			/* variable argument list */
    int ret;			/* libc function return value */
    int fd;			/* stream as a file descriptor or -1 */
    int line_count;		/* number of lines in the paragraph */

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, line);

    /*
     * firewall
     */
    if (stream == NULL) {
	err(135, __func__, "stream is NULL");
	not_reached();
    }

    /*
     * this may not always catch a bogus or unopened stream, but try anyway
     */
    clearerr(stream);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    fd = fileno(stream);
    if (fd < 0) {
	errp(136, __func__, "fileno on stream returned: %d < 0", fd);
	not_reached();
    }
    clearerr(stream);		/* paranoia */

    /*
     * print paragraph strings followed by newlines
     */
    line_count = 0;
    while (line != NULL) {

	/*
	 * print the string
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputs(line, stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(137, __func__, "error writing paragraph to a stream");
		not_reached();
	    } else if (feof(stream)) {
		err(138, __func__, "EOF while writing paragraph to a stream");
		not_reached();
	    } else {
		errp(139, __func__, "unexpected fputs error writing paragraph to a stream");
		not_reached();
	    }
	}

	/*
	 * print the newline
	 */
	clearerr(stream);	/* pre-clear ferror() status */
	errno = 0;		/* pre-clear errno for errp() */
	ret = fputc('\n', stream);
	if (ret == EOF) {
	    if (ferror(stream)) {
		errp(140, __func__, "error writing newline to a stream");
		not_reached();
	    } else if (feof(stream)) {
		err(141, __func__, "EOF while writing newline to a stream");
		not_reached();
	    } else {
		errp(142, __func__, "unexpected fputc error newline a stream");
		not_reached();
	    }
	}
	++line_count;		/* count this line as printed */

	/*
	 * move to next line string
	 */
	line = va_arg(ap, char *);
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * flush the paragraph onto the stream
     */
    clearerr(stream);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stream);
    if (ret == EOF) {
	if (ferror(stream)) {
	    errp(143, __func__, "error flushing stream");
	    not_reached();
	} else if (feof(stream)) {
	    err(144, __func__, "EOF while flushing stream");
	    not_reached();
	} else {
	    errp(145, __func__, "unexpected fflush error while flushing stream");
	    not_reached();
	}
    }
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __func__, line_count);
    return;
}


/*
 * vfpr - call fprintf, flush and check the result, in va_list form
 *
 * given:
 *      stream  - open file stream to print on
 *	name	- name of calling function
 *	fmt	- fprintf format
 *	ap	- variable argument list
 *
 * NOTE: This function calls warnp() and warn() on errors.
 */
void
vfpr(FILE *stream, char const *name, char const *fmt, va_list ap)
{
    int fret;			/* vfprintf function return value */
    int ret;			/* libc function return value */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return;
    }
    if (name == NULL) {
	warn(__func__, "name is NULL, using ((NULL))");
	name = "((NULL))";
    }
    if (fmt == NULL) {
	warn(__func__, "called from %s: fmt is NULL", name);
	return;
    }

    /*
     * formatted print to the stream
     */
    errno = 0;			/* pre-clear errno for warnp() */
    fret = vfprintf(stream, fmt, ap);
    if (fret <= 0 || errno != 0) {
	warnp(__func__, "called from %s: vfprintf returned: %d <= 0", name, fret);
    }

    /*
     * flush the paragraph onto the stream
     */
    clearerr(stream);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret == EOF) {
	if (ferror(stream)) {
	    warnp(__func__, "called from %s: error flushing stream", name);
	} else if (feof(stream)) {
	    warnp(__func__, "called from %s: EOF while flushing stream", name);
	} else {
	    warnp(__func__, "called from %s: unexpected fflush error while flushing stream", name);
	}
    }
    return;
}


/*
 * fpr - call fprintf, flush and check the result
 *
 * given:
 *      stream  - open file stream to print on
 *	name	- name of calling function
 *	fmt	- fprintf format
 *	...	- optional fprintf args
 *
 * NOTE: This function calls warnp() and warn() on errors.
 */
void
fpr(FILE *stream, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return;
    }
    if (name == NULL) {
	warn(__func__, "name is NULL, using ((NULL))");
	name = "((NULL))";
    }
    if (fmt == NULL) {
	warn(__func__, "called from %s: fmt is NULL", name);
	return;
    }

    /*
     * formatted print to the stream
     */
    vfpr(stream, name, fmt, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * pr - call printf, flush and check the result
 *
 * given:
 *	name	- name of calling function
 *	fmt	- printf format
 *	...	- optional printf args
 *
 * NOTE: This function calls warnp() and warn() on errors.
 */
void
pr(char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (name == NULL) {
	warn(__func__, "name is NULL, using ((NULL))");
	name = "((NULL))";
    }
    if (fmt == NULL) {
	warn(__func__, "called from %s: fmt is NULL", name);
	return;
    }

    /*
     * formatted print to the stdout
     */
    vfpr(stdout, name, fmt, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * readline - read a line from a stream
 *
 * Read a line from an open stream.  Malloc or realloc the line
 * buffer as needed.  Remove the trailing newline that was read.
 *
 * given:
 *      linep   - allocated line buffer (may be realloced) or ptr to NULL
 *                NULL ==> getline() will malloc() the linep buffer
 *                else ==> getline() might realloc() the linep buffer
 *      stream - file stream to read from
 *
 * returns:
 *      number of characters in the line with newline removed,
 *      or -1 for EOF
 *
 * This function does not return on error.
 */
ssize_t
readline(char **linep, FILE * stream)
{
    size_t linecap = 0;		/* allocated capacity of linep buffer */
    ssize_t ret;		/* getline return and our modified size return */

    /*
     * firewall
     */
    if (linep == NULL || stream == NULL) {
	err(146, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * read the line
     */
    clearerr(stream);
    errno = 0;			/* pre-clear errno for errp() */
    ret = getline(linep, &linecap, stream);
    if (ret < 0) {
	if (feof(stream)) {
	    dbg(DBG_VVHIGH, "EOF detected on readline");
	    return -1; /* EOF found */
	} else if (ferror(stream)) {
	    errp(147, __func__, "getline() error");
	    not_reached();
	} else {
	    errp(148, __func__, "unexpected getline() error");
	    not_reached();
	}
    }
    /*
     * paranoia
     */
    if (*linep == NULL) {
	err(149, __func__, "*linep is NULL after getline()");
	not_reached();
    }

    /*
     * process trailing newline or lack there of
     */
    if ((*linep)[ret - 1] != '\n') {
	warn(__func__, "line does not end in newline: %s", *linep);
    } else {
	(*linep)[ret - 1] = '\0';	/* clear newline */
	--ret;
    }
    dbg(DBG_VVVHIGH, "read %jd bytes + newline into %ju byte buffer", (intmax_t)ret, (uintmax_t)linecap);

    /*
     * return length of line without the trailing newline
     */
    return ret;
}


/*
 * readline_dup - read a line from a stream and duplicate to a allocated buffer.
 *
 * given:
 *      linep   - allocated line buffer (may be realloced) or ptr to NULL
 *                NULL ==> getline() will malloc() the linep buffer
 *                else ==> getline() might realloc() the linep buffer
 *      strip   - true ==> remove trailing whitespace,
 *                false ==> only remove the trailing newline
 *      lenp    - != NULL ==> pointer to length of final length of line allocated,
 *                NULL ==> do not return length of line
 *      stream - file stream to read from
 *
 * returns:
 *      allocated buffer containing the input without a trailing newline,
 *      and if strip == true, without trailing whitespace,
 *      or NULL ==> EOF
 *
 * This function does not return on error.
 *
 * NOTE: It is the caller's responsibility to free the returned string when it
 * is no longer needed.
 */
char *
readline_dup(char **linep, bool strip, size_t *lenp, FILE *stream)
{
    ssize_t len;		/* getline return and our modified size return */
    char *ret;			/* allocated input */
    ssize_t i;

    /*
     * firewall
     */
    if (linep == NULL || stream == NULL) {
	err(150, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * read the line
     */
    len = readline(linep, stream);
    if (len < 0) {
	/*
	 * EOF found
	 */
	return NULL;
    }
    dbg(DBG_VVHIGH, "readline returned %jd bytes", (intmax_t)len);

    /*
     * duplicate the line
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = calloc(len+1+1, sizeof(char));
    if (ret == NULL) {
	errp(151, __func__, "calloc of read line of %jd bytes failed", (intmax_t)len+1+1);
	not_reached();
    }
    memcpy(ret, *linep, len);

    /*
     * strip trailing whitespace if requested
     */
    if (strip) {
	if (len > 0) {
	    for (i = len - 1; i >= 0; --i) {
		if (isascii(ret[i]) && isspace(ret[i])) {
		    /*
		     * strip trailing ASCII whitespace
		     */
		    --len;
		    ret[i] = '\0';
		} else {
		    break;
		}
	    }
	}
	dbg(DBG_VVHIGH, "readline, after trailing whitespace strip is %jd bytes", (intmax_t)len);
    }
    if (lenp != NULL) {
	*lenp = (size_t)len;
    }

    /*
     * return the allocated string
     */
    return ret;
}


/*
 * round_to_multiple - round up to a multiple
 *
 * given:
 *	num		- the number to round up
 *	multiple	- the multiple to round up to
 *
 *  Returns num rounded up to the nearest multiple.
 *
 *  Slightly modified code from https://stackoverflow.com/a/3407254/9205647
 *  because sometimes we all get to be lazy. :-)
 *
 *  Returns num rounded up to the next multiple of 1024.
 *
 *  Examples:
 *
 *	0 rounds to 0
 *	1 rounds to 1024
 *	1023 rounds to 1024
 *	1024 rounds to 1024
 *	1025 rounds to 2048
 *	2047 rounds to 2048
 *	2048 rounds to 2048
 *	etc.
 */
off_t
round_to_multiple(off_t num, off_t multiple)
{
    off_t mod;

    if (!multiple || num <= 0) {
	return num;
    }

    mod = num % multiple;
    if (!mod)
        return num;

    return num + multiple - mod;
}


/*
 * read_all - read all data from an open file
 *
 * given:
 *	stream	    - an open file stream to read from
 *	psize	    - if psize != NULL, *psize is the amount of data read
 *
 * returns:
 *	malloc buffer containing the entire contents of stream,
 *	or NULL is an error occurred.
 *
 * This function will update *psize, if it was non-NULL, to indicate
 * the amount of data that was read from stream before EOF.
 *
 * The allocated buffer may be larger than the amount of data read.
 * In this case *psize (if psize != NULL) will contain the exact
 * amount of data read, ignoring any extra allocated data.
 * Any extra unused space in the allocated buffer will be zeroized
 * before returning.
 *
 * This function will always add at least one extra byte of allocated
 * data to the end of the allocated buffer (zeroized as mentioned above).
 * These extra bytes(s) WILL be set to NUL.  Thus, a file or stream
 * without a NUL byte will return a NUL terminated C-style string.
 *
 * If no data is read, the malloc buffer will still be NUL terminated.
 *
 * If one is using is_string() to check if the data read is a string,
 * one should check for ONE EXTRA BYTE!  That is:
 *
 *	data = read_all(stream, len);
 *	if (data == NULL) {
 *	    .. handle read_all errors ..
 *	}
 *
 *	...
 *
 *	if (is_string(data, len+1) == true) {
 *	    .. data has no internal NUL byte ..
 *	} else {
 *	    .. data has at least one internal NUL byte ..
 }	}
 *
 * Because files can contain NUL bytes, the strlen() function on
 * the allocated buffer may return a different length than the
 * amount of data read from stream.  This is also why the function
 * returns a pointer to void.
 *
 * NOTE: It is the caller's responsibility to free the returned string when it
 * is no longer needed.
 */
void *
read_all(FILE *stream, size_t *psize)
{
    struct dyn_array *array = NULL;	/* dynamic array for file content */
    long dyn_array_seek_cycle = 0;	/* number of dyn_array_seek() calls */
    long move_cycle = 0;		/* number of realloc cycles that moved data */
    bool moved = false;			/* true ==> location of the elements array moved during realloc() */
    uint8_t *read_buf = NULL;		/* where next to read data into */
    long read_cycle = 0;		/* number of read cycles */
    size_t last_read = 0;		/* amount last fread read from open stream */
    intmax_t used = 0;		        /* amount of data read into the buffer */
    uint8_t *ret = NULL;		/* buffer containing the while file to return */
    int fread_errno = 0;		/* errno after fread() call */

    /*
     * firewall
     */
    if (stream == NULL) {
	err(152, __func__, "called with NULL stream");
	not_reached();
    }

    /*
     * quick return with no data if stream is already in ERROR or EOF state
     */
    if (feof(stream) || ferror(stream)) {
	/* report the I/O condition */
	if (feof(stream)) {
	    warn(__func__, "EOF found at start of reading stream");
	} else if (ferror(stream)) {
	    warn(__func__, "I/O error flag found at start of reading stream");
	}
	/* record empty size, if requested */
	if (psize != NULL) {
	    *psize = 0;
	}
	return NULL;
    }

    /*
     * create the dynamic array
     */
    array = dyn_array_create(sizeof(uint8_t), READ_ALL_CHUNK, INITIAL_BUF_SIZE, true);
    ++dyn_array_seek_cycle;

    /*
     * read until stream EOF or ERROR
     */
    do {

	/*
	 * expand buffer by a READ_ALL_CHUNK
	 */
	used = dyn_array_tell(array);
	moved = dyn_array_seek(array, READ_ALL_CHUNK, SEEK_CUR);
	if (moved == true) {
	    ++move_cycle;
	    dbg(DBG_VVVHIGH, "dyn_array_seek() caused a realloc data move, count: %ld", move_cycle);
	}
	dbg(DBG_VVHIGH, "%s: dyn_array_seek cycle: %ld new size: %jd", __func__,
			dyn_array_seek_cycle, dyn_array_tell(array));
	++dyn_array_seek_cycle;

	/*
	 * try to read more data from the stream
	 */
	dbg(DBG_VVHIGH, "%s: about to start read cycle: %ld", __func__, read_cycle);
	read_buf = dyn_array_addr(array, uint8_t, used);
	errno = 0;			/* pre-clear errno for warnp() */
	last_read = fread(read_buf, sizeof(uint8_t), READ_ALL_CHUNK, stream);
	fread_errno = errno;	/* save errno from fread() call for later reporting if needed */
	dbg(DBG_VVHIGH, "%s: fread(read_buf, %ju, %d, stream) read cycle: %ld returned: %jd",
			 __func__, (uintmax_t)sizeof(uint8_t), READ_ALL_CHUNK, read_cycle, (intmax_t)last_read);
	++read_cycle;

	/*
	 * account for the amount of data read
	 */
	if (last_read > 0) {
	    if (last_read != READ_ALL_CHUNK) {
		/* update the dynamic array size based on amount of read in last read */
		moved = dyn_array_seek(array, last_read-READ_ALL_CHUNK, SEEK_CUR);
		if (moved == true) {
		    ++move_cycle;
		    dbg(DBG_VVVHIGH, "dyn_array_seek() caused a realloc data move, count: %ld", move_cycle);
		}
	    }
	/* case: no data read */
	} else {
		/* restore old dynamic array size */
		moved = dyn_array_seek(array, used, SEEK_SET);
		if (moved == true) {
		    ++move_cycle;
		    dbg(DBG_VVVHIGH, "dyn_array_seek() caused a realloc data move, count: %ld", move_cycle);
		}
	}
	used = dyn_array_tell(array);

	/*
	 * look for I/O errors and EOF
	 */
	errno = fread_errno;	/* restore errno from fread() call */
	if (last_read == 0 || ferror(stream) || feof(stream)) {

	    /* report the I/O condition */
	    if (feof(stream)) {
		dbg(DBG_HIGH, "fread returned: %ju normal EOF reading stream at: %jd bytes",
			      (uintmax_t)last_read, used);
	    } else if (ferror(stream)) {
		warnp(__func__, "fread returned: %ju I/O error detected while reading stream at: %jd bytes",
			        (uintmax_t)last_read, used);
	    } else {
		warnp(__func__, "fread returned %ju although neither the EOF nor ERROR flag were set: "
				"assuming EOF anyway", (uintmax_t)last_read);
	    }

	    /*
	     * stop reading stream
	     */
	    break;
	}
    } while (true);
    dbg(DBG_VVHIGH, "%s(stream, psize): last_read: %ju total bytes: %jd allocated: %jd "
		    "read_cycle: %ld move_cycle: %ld seek_cycle: %ld",
		    __func__, (uintmax_t)last_read, dyn_array_tell(array), dyn_array_alloced(array),
		    read_cycle, move_cycle, dyn_array_seek_cycle);

    /*
     * report the amount of data actually read, if requested
     */
    if (psize != NULL) {
	*psize = used;
    }

    /*
     * return the allocated buffer
     */
    ret = dyn_array_addr(array, uint8_t, 0);
    return ret;
}


/*
 * is_string - determine if a block of memory is a C string
 *
 * given:
 *	ptr	- pointer to a character
 *	len	- number of bytes to check, including the final NUL
 *
 * returns:
 *	true ==> ptr is a C-style string of length len, that is NUL terminated,
 *	false ==> ptr is NULL, or
 *		  NUL character was found before the final byte, or
 *		  the string is not NUL terminated
 *
 * NOTE: If you are using is_string() to detect if read_all() read an internal
 *	 NUL byte, be sure to check for ONE EXTRA BYTE.  See the read_all()
 *	 comment above for an example.
 */
bool
is_string(char const * const ptr, size_t len)
{
    char *nul_found = NULL;	/* where a NUL character was found, if any */

    /*
     * firewall
     */
    if (ptr == NULL) {
	dbg(DBG_VVHIGH, "is_string: ptr is NULL");
	return false;
    }

    /*
     * look for a NUL byte
     */
    nul_found = memchr(ptr, '\0', len);

    /*
     * process the result of the NUL byte search
     */
    if (nul_found == NULL) {
	dbg(DBG_VVHIGH, "is_string: no NUL found from ptr through ptr[%jd]",
			(intmax_t)(len-1));
	return false;
    }
    if ((intmax_t)(nul_found-ptr) != (intmax_t)(len-1)) {
	dbg(DBG_VVHIGH, "is_string: found NUL at ptr[%jd] != ptr[%jd]",
			(intmax_t)(nul_found-ptr),
			(intmax_t)(len-1));
	return false;
    }

    /*
     * report that ptr is a C-style string of length len
     */
    dbg(DBG_VVVHIGH, "is_string: is a C-style string of length: %jd",
		     (intmax_t)len);
    return true;
}


/*
 * strnull - detect if string is empty
 *
 * given:
 *	str	- C-style string or NULL
 *
 * returns:
 *	str if str a non-NULL non-empty string,
 *	else NULL
 */
char const *
strnull(char const * const str)
{
    /*
     * if str is non-NULL and non-zero length, return str
     */
    if (str != NULL && str[0] != '\0') {
	return str;
    }
    /* NULL pointer or empty C-style string */
    return NULL;
}


/*
 * string_to_long   -	convert str to long and check for errors
 *
 * given:
 *	str	-   the string to convert to a long int.
 *
 * Returns string 'str' as a long int.
 *
 * Does not return on error or NULL pointer.
 */
long
string_to_long(char const *str)
{
    long long num; /* use a long long for overflow/underflow checks */

    /*
     * firewall
     */
    if (str == NULL) {
	err(153, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoll(str, NULL, 10);

    if (errno != 0) {
	errp(154, __func__, "error converting string \"%s\" to long int: %s", str, strerror(errno));
	not_reached();
    }
    else if (num < LONG_MIN || num > LONG_MAX) {
	err(155, __func__, "number %s out of range for long int (must be >= %ld && <= %ld)", str, LONG_MIN, LONG_MAX);
	not_reached();
    }
    return (long)num;
}


/*
 * string_to_long_long   -	convert str to long long and check for errors
 *
 * given:
 *	str	-   the string to convert to a long long int.
 *
 * Returns string 'str' as a long long int.
 *
 * Does not return on error or NULL pointer.
 */
long long
string_to_long_long(char const *str)
{
    long long num;

    /*
     * firewall
     */
    if (str == NULL) {
	err(156, __func__, "passed NULL arg");
	not_reached();
    }

    if (strlen(str) > LLONG_MAX_BASE10_DIGITS) {
	err(157, __func__, "string '%s' too long", str);
	not_reached();
    }

    errno = 0;
    num = strtoll(str, NULL, 10);

    if (errno != 0) {
	errp(158, __func__, "error converting string \"%s\" to long long int: %s", str, strerror(errno));
	not_reached();
    }
    else if (num <= LLONG_MIN || num >= LLONG_MAX) {
	err(159, __func__, "number %s out of range for long long int (must be > %lld && < %lld)", str, LLONG_MIN, LLONG_MAX);
	not_reached();
    }
    return num;
}


/*
 * string_to_int   -	convert str to int and check for errors
 *
 * given:
 *	str	-   the string to convert to an int.
 *
 * Returns string 'str' as an int.
 *
 * Does not return on error or NULL pointer.
 */
int
string_to_int(char const *str)
{
    long long num; /* use a long long for overflow/underflow checks */

    /*
     * firewall
     */
    if (str == NULL) {
	err(160, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = (int)strtoll(str, NULL, 10);

    if (errno != 0) {
	errp(161, __func__, "error converting string \"%s\" to int: %s", str, strerror(errno));
	not_reached();
    }
    else if (num < INT_MIN || num > INT_MAX) {
	err(162, __func__, "number %s out of range for int (must be >= %d && <= %d)", str, INT_MIN, INT_MAX);
	not_reached();
    }
    return (int)num;
}


/*
 * string_to_unsigned_long - string to unsigned long with error checks
 *
 * given:
 *	str	- the string to convert to unsigned long
 *
 * Returns str as an unsigned long.
 *
 * Does not return on error.
 */
unsigned long
string_to_unsigned_long(char const *str)
{
    unsigned long num = 0;

    /*
     * firewall
     */
    if (str == NULL) {
	err(163, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoul(str, NULL, 10);
    if (errno != 0) {
	errp(164, __func__, "strtoul(%s): %s", str, strerror(errno));
	not_reached();
    } else if (num >= ULONG_MAX) {
	err(165, __func__, "strtoul(%s): too big", str);
	not_reached();
    }

    return num;
}


/*
 * string_to_unsigned_long_long - string to unsigned long long with error checks
 *
 * given:
 *	str	- the string to convert to unsigned long long
 *
 * Returns str as an unsigned long long.
 *
 * Does not return on error.
 */
unsigned long long
string_to_unsigned_long_long(char const *str)
{
    unsigned long long num;

    /*
     * firewall
     */
    if (str == NULL) {
	err(166, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoul(str, NULL, 10);
    if (errno != 0) {
	errp(167, __func__, "strtoul(%s): %s", str, strerror(errno));
	not_reached();
    } else if (num >= ULLONG_MAX) {
	err(168, __func__, "strtoul(%s): too big", str);
	not_reached();
    }

    return num;
}


/*
 * string_to_intmax   -	convert base 10 str to intmax_t and check for errors
 *
 * given:
 *	str	-   the string to convert to an intmax_t.
 *
 * Returns string 'str' as a long long int.
 *
 * Does not return on error or NULL pointer.
 */
intmax_t
string_to_intmax(char const *str)
{
    intmax_t num;

    /*
     * firewall
     */
    if (str == NULL) {
	err(169, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoimax(str, NULL, 10);

    if (errno != 0) {
	errp(170, __func__, "error converting string \"%s\" to intmax_t: %s", str, strerror(errno));
	not_reached();
    }
    else if (num <= INTMAX_MIN || num >= INTMAX_MAX) {
	err(171, __func__, "number %s out of range for intmax_t (must be > %jd && < %jd)", str, INTMAX_MIN, INTMAX_MAX);
	not_reached();
    }
    return num;
}


/*
 * string_to_intmax2   - second interface to convert base 10 str to intmax_t and check for errors
 *
 * given:
 *	str	- the string to convert to an intmax_t
 *	*ret	- pointer to converted intmax_t if return is true
 *
 * returns:
 *	true ==> conversion into *ret was successful,
 *	false ==> unable to convert / not a valid base 10 integer, NULL pointer or internal error
 */
bool
string_to_intmax2(char const *str, intmax_t *ret)
{
    intmax_t num = 0;
    int saved_errno = 0;
    char *endptr = NULL;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return false;
    }
    if (ret == NULL) {
	warn(__func__, "ret is NULL");
	return false;
    }

    /*
     * perform the conversion
     */
    errno = 0;		/* pre-clear errno for warnp() */
    num = strtoimax(str, &endptr, 10);
    saved_errno = errno;
    if (endptr == str) {
	warn(__func__, "string %s has no digits", str);
	return false;
    } else if (*endptr != '\0') {
	warn(__func__, "number %s has invalid characters", str);
	return false;
    } else if (saved_errno != 0) {
	errno = saved_errno;
	warnp(__func__, "error converting string <%s> to intmax_t", str);
	return false;
    } else if (num <= INTMAX_MIN || num >= INTMAX_MAX) {
	warn(__func__, "number %s out of range for intmax_t (must be > %jd && < %jd)", str, INTMAX_MIN, INTMAX_MAX);
	return false;
    }

    /*
     * store conversion and report success
     */
    *ret = num;
    return true;
}


/*
 * string_to_uintmax - string to uintmax_t with error checks
 *
 * given:
 *	str	- the string to convert to uintmax_t
 *
 * Returns str as an unsigned long long.
 *
 * Does not return on error.
 */
uintmax_t
string_to_uintmax(char const *str)
{
    uintmax_t num;

    /*
     * firewall
     */
    if (str == NULL) {
	err(172, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoumax(str, NULL, 10);
    if (errno != 0) {
	errp(173, __func__, "strtoumax(%s): %s", str, strerror(errno));
	not_reached();
    } else if (num >= UINTMAX_MAX) {
	err(174, __func__, "strtoumax(%s): too big", str);
	not_reached();
    }

    return num;
}


/*
 * string_to_float - string to long double with error checks
 *
 * given:
 *	str	- the string to convert to a quad_t
 *
 * Returns str as an unsigned long long.
 *
 * Does not return on error.
 */
long double
string_to_float(char const *str)
{
    long double num;

    /*
     * firewall
     */
    if (str == NULL) {
	err(175, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtold(str, NULL);

    if (errno != 0) {
	errp(176, __func__, "error converting string \"%s\" to long double : %s", str, strerror(errno));
	not_reached();
    }
    else if (num <= LDBL_MIN || num >= LDBL_MAX) {
	err(177, __func__, "number %s out of range for long double (must be > %Lf && < %Lf)", str, LDBL_MIN, LDBL_MAX);
	not_reached();
    }
    return num;
}


/*
 * valid_contest_id	    -	validate string as a contest ID
 *
 * given:
 *	str	    -	string to test
 *
 * Returns true if it's valid.
 *
 * Returns false if it's invalid, NULL or empty string.
 */
bool
valid_contest_id(char *str)
{
    size_t len;			/* input string length */
    int ret;			/* libc function return */
    unsigned int a, b, c, d, e, f;	/* parts of the UUID string */
    unsigned int version = 0;	/* UUID version hex character */
    unsigned int variant = 0;	/* UUID variant hex character */
    char guard;			/* scanf guard to catch excess amount of input */
    size_t i;

    if (str == NULL || strlen(str) == 0) {
	return false;
    }

    if (!strcmp(str, "test")) {
	/*
	 * special case: test is valid
	 */
	return true;
    }
    len = strlen(str);

    /*
     * validate format of non-test contest ID.  The contest ID, if not "test"
     * must be a UUID.  The UUID has the 36 character format:
     *
     *	    xxxxxxxx-xxxx-4xxx-axxx-xxxxxxxxxxxx
     *
     * where 'x' is a hex character.  The 4 is the UUID version and the variant
     * 1.
     */
    if (len != UUID_LEN) {
	return false;
    }

    /* convert to UUID lower case before checking */
    for (i = 0; i < len; ++i) {
	str[i] = (char)tolower(str[i]);
    }
    dbg(DBG_VHIGH, "converted the IOCCC contest ID to: %s", str);
    /* validate UUID string, version and variant */
    ret = sscanf(str, "%8x-%4x-%1x%3x-%1x%3x-%8x%4x%c", &a, &b, &version, &c, &variant, &d, &e, &f, &guard);
    dbg(DBG_HIGH, "UUID version hex char: %1x", version);
    dbg(DBG_HIGH, "UUID variant hex char: %1x", variant);
    if (ret != 8 || version != UUID_VERSION || variant != UUID_VARIANT) {
	return false;
    }


    dbg(DBG_MED, "IOCCC contest ID is a UUID: %s", str);

    return true;
}


/*
 * parse_verbosity	- parse -v option for our tools
 *
 * given:
 *	program		- the calling program e.g. txzchk, fnamchk, mkiocccentry etc.
 *	arg		- the optarg in the calling tool
 *
 * Returns the parsed verbosity.
 *
 * Returns DBG_NONE if passed NULL args or empty string.
 */
int
parse_verbosity(char const *program, char const *arg)
{
    int verbosity;

    if (program == NULL || arg == NULL || !strlen(arg)) {
	return DBG_NONE;
    }

    /*
     * parse verbosity
     */
    errno = 0;		/* pre-clear errno for errp() */
    verbosity = (int)strtol(arg, NULL, 0);
    if (errno != 0) {
	errp(1, __func__, "%s: cannot parse -v arg: %s error: %s", program, arg, strerror(errno)); /*ooo*/
	not_reached();
    }

    return verbosity;
}


/*
 * is_decimal	    - if the buffer is a base 10 integer in ASCII
 *
 * given:
 *	ptr	    - pointer to buffer containing an integer in ASCII
 *	len	    - length, starting at ptr
 *
 * For our purposes a number is defined as: a string that starts with either a
 * '-' or '+' and beyond that no other characters but [0-9] (any count).
 *
 * This function tests only for the following regex:
 *
 *	[+-]?[0-9]+
 *
 * In particular these are NOT considered ASCII integers by this function:
 *
 *	[0-9a-fA-F]+		<== may return false if we find a [a-fA-F]
 *	0x[0-9a-fA-F]+		<== will return false due to 'x'
 *	0b[01]*			<== will return false due to 'b'
 *	[0-9]*.0*		<== will return false due to '.'
 *	~[0-9]+			<== will return false due to '~'
 *
 * We know that some so-called specifications for data exchange do not
 * allow for a leading +.  Others might not allow for 0 followed by
 * digits, 'x' or even 'b'.  This function is more general on one hand,
 * and more specific on the other hand, from any "amateur open mic
 * specification" that you might encounter at your local developer pub. :-)
 *
 * returns:
 *
 *	true	==> ptr points to a base 10 integer in ASCII
 *	false	==> ptr does NOT point to a base 10 integer in ASCII, or if ptr is NULL, or len <= 0
 */
bool
is_decimal(char const *ptr, size_t len)
{
    size_t start = 0;	/* starting character number for ASCII digits */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "passed NULL ptr");
	return false;
    }
    if (len <= 0) {
	warn(__func__, "len <= 0: %ju", (intmax_t)len);
	return false;
    }

    /*
     * case: leading - or + is OK
     */
    switch (*ptr) {
	case '-':
	case '+':
	    start = 1; /* skip sign */
	    break;
	default:
	    break;
    }
    if (start >= len) {
	warn(__func__, "only sign found, no digits");
	return false;
    }

    /*
     * Test for ASCII base 10 digits
     *
     * NOTE: We cannot use strspn() nor strcspn() because we cannot assume the
     *	     ASCII integer is immediately followed by a NUL byte.
     *
     * NOTE: Alas, there is no strnspn() nor strncspn() in the standard due to
     *	     "reasons other than technical reasons" *sigh*.
     */
    for (i=start; i < len; ++i) {
	if (!isascii(ptr[i]) || !isdigit(ptr[i])) {
	    /* found a non-ASCII digit */
	    return false;
	}
    }

    /*
     * ptr points to a base 10 integer in ASCII
     */
    return true;
}


/*
 * is_decimal_str	    - if the string str is a base 10 integer in ASCII
 *
 * This is a simplified interface for is_decimal().
 *
 * given:
 *	str	    - pointer to buffer containing an integer in ASCII
 *	retlen	    - address of where to store length of str, if retlen != NULL
 *
 * returns:
 *
 *	true	==> str points to a base 10 integer in ASCII
 *	false	==> str does NOT point to a base 10 integer in ASCII, str is
 *		    NULL or str is empty
 *
 * NOTE: This function calls is_decimal().  See that function for details on
 *	 what is or is not considered an integer.
 */
bool
is_decimal_str(char const *str, size_t *retlen)
{
    size_t len = 0;		/* length of string to test */
    bool ret = false;		/* if str points to a base 10 integer in ASCII */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "passed NULL str");
	return false;
    } else {
	len = strlen(str);
    }

    /*
     * convert to is_decimal() call
     */
    ret = is_decimal(str, len);

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return test of ASCII base 10 digits
     */
    return ret;
}

/*
 * posix_plus_safe - if string is a valid POSIX portable safe plus + chars
 *
 * If slash_ok is true:
 *
 *	If first is true:
 *
 *	    If lower_only is true, then the string must match:
 *
 *		^[/0-9a-z][0-9a-z._+-]*$
 *
 *	   If lower_only is false, then the string must match:
 *
 *		^[/0-9A-Za-z][0-9A-Za-z._+-]*$
 *
 *	If first is false:
 *
 *	    If lower_only is true, then the string must match:
 *
 *		^[/0-9a-z._+-]*$
 *
 *	   If lower_only is false, then the string must match:
 *
 *		^[/0-9A-Za-z._+-]*$
 *
 * If slash_ok is false:
 *
 *	If first is true:
 *
 *	    If lower_only is true, then the string must match:
 *
 *		^[0-9a-z][0-9a-z._+-]*$
 *
 *	   If lower_only is false, then the string must match:
 *
 *		^[0-9A-Za-z][0-9A-Za-z._+-]*$
 *
 *	If first is false:
 *
 *	    If lower_only is true, then the string must match:
 *
 *		^[0-9a-z._+-]*$
 *
 *	   If lower_only is false, then the string must match:
 *
 *		^[0-9A-Za-z._+-]*$
 *
 * given:
 *	str		- string to test
 *	lower_only	- true ==> only lower case characters are allowed
 *			- false ==> both UPPER and lower case characters are allowed
 *	slash_ok	- true ==> / is allowed as str can be a path
 *			  false ==> / is NOT allowed, str is a basename only
 *	first		- true ==> str is at beginning, perform first char check
 *			  false ==> str may be in the middle, skip first char check
 *
 * returns:
 *	true ==> str is a valid POSIX portable safe + filename, AND
 *		 the case of str matches lower_only and slash_ok conditions
 *	false ==> an unsafe issue was found, or
 *		  str is an empty string, or
 *		  str is NULL
 */
bool
posix_plus_safe(char const *str, bool lower_only, bool slash_ok, bool first)
{
    size_t len;		/* length of str */
    size_t start = 0;	/* starting position of full string scan */
    size_t i;

    /*
     * firewall
     */
    if (str == NULL) {
	dbg(DBG_VVHIGH, "str is NULL");
	return false;
    }
    len = strlen(str);
    if (len <= 0) {
	dbg(DBG_VVHIGH, "posix_plus_safe(): str is an empty string");
	return false;
    }

    /*
     * special case: first character is /
     */
    if (first == true) {
	if (str[0] == '/') {
		if (slash_ok == false) {
		    dbg(DBG_VVHIGH, "str[0]: slash_ok is false and first character is /");
		    return false;
		}

	/*
	 * special case: first character is not /, so it must be alphanumeric
	 */
	} else {
	    /* ASCII non-/ check */
	    if (!isascii(str[0])) {
		dbg(DBG_VVHIGH, "str[0]: first character is non-ASCII: 0x%02x", (unsigned int)str[0]);
		return false;
	    }
	    /* alphanumeric non-/ check */
	    if (!isalnum(str[0])) {
		dbg(DBG_VVHIGH, "str[0]: first character not alphanumeric: 0x%02x", (unsigned int)str[0]);
		return false;
	    }
	    /* special case: lower_only is true, alphanumeric lower case only */
	    if (lower_only == true && isupper(str[0])) {
		dbg(DBG_VVHIGH, "str[0]: lower_only is true and first character is upper case: 0x%02x",
				(unsigned int)str[0]);
		return false;
	    }
	}
	/* first character already checked, scan beyond first character next */
	start = 1;
    }

    /*
     * Beyond the first character, they must be POSIX portable filename or +
     */
    for (i=start; i < len; ++i) {

	/*
	 * special case: / check
	 */
	if (str[i] == '/') {
		if (slash_ok == false) {
		    dbg(DBG_VVHIGH, "slash_ok is false and / found at str[%ju]",
				    (uintmax_t)i);
		    return false;
		}

	/*
	 * case: non-/ check
	 */
	} else {
	    /* ASCII non-/ check */
	    if (!isascii(str[i])) {
		dbg(DBG_VVHIGH, "str[%ju]: character is non-ASCII: 0x%02x",
				(uintmax_t)i, (unsigned int)str[i]);
		return false;
	    }
	    /* alphanumeric nor [._+-] nor non-/ check */
	    if (!isalnum(str[i]) && str[i] != '.' && str[i] != '_' && str[i] != '+' && str[i] != '-') {
		dbg(DBG_VVHIGH, "str[%ju]: character not alphanumeric nor ._+-: 0x%02x",
				(uintmax_t)i, (unsigned int)str[i]);
		return false;
	    }
	    /* special case: lower_only is true, alphanumeric lower case only */
	    if (lower_only == true && isupper(str[i])) {
		dbg(DBG_VVHIGH, "str[%ju]: lower_only is true and character is upper case: 0x%02x",
				(uintmax_t)i, (unsigned int)str[i]);
		return false;
	    }
	}
    }

    /*
     * all is well
     */
    dbg(DBG_VVVHIGH, "lower_only: %s slash_ok: %s first: %s str is valid: <%s>",
		     booltostr(lower_only), booltostr(slash_ok), booltostr(first), str);
    return true;
}


/*
 * posix_safe_chk - test a string for various POSIX related tests
 *
 * given:
 *	str		- string to test
 *	len		- length of str to test
 *	*slash		- set to true ==> a / was found
 *			  set to false ==> no / was found
 *	*posix_safe	- set to true ==> all chars are POSIX portable safe plus +/
 *			- set to false ==> one or more chars are not portable safe plus +/
 *	*first_alphanum	- set to true ==> first char is alphanumeric
 *			  set to false ==> first char is not alphanumeric
 *	*upper		- set to true ==> UPPER case chars found
 *			- set to false ==> no UPPER case chars found
 *
 * NOTE: This function does not return when given NULL ptr
 */
void
posix_safe_chk(char const *str, size_t len, bool *slash, bool *posix_safe, bool *first_alphanum, bool *upper)
{
    bool found_unsafe = false;		/* true ==> found non-ASCII or non-POSIX portable safe plus +/ */
    size_t i;

    /*
     * firewall
     */
    if (str == NULL || slash == NULL || posix_safe == NULL || first_alphanum == NULL || upper == NULL) {
	err(178, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * assume all tests will fail
     */
    *slash = false;
    *posix_safe = false;
    *first_alphanum = false;
    *upper = false;

    /*
     * empty string fails all tests
     */
    if (len <= 0) {
	dbg(DBG_VVHIGH, "posix_safe_chk(.., false, false, false, false) returning: str is an empty string");
	return;
    }

    /*
     * test first character
     */
    if (isascii(str[0])) {

	/*
	 * case: first character is /
	 */
	if (str[0] == '/') {
	    *slash = true;
	    dbg(DBG_VVVHIGH, "posix_safe_chk(): str[0] is /: 0x%02x", (unsigned int)str[0]);

	/*
	 * case: first character is alphanumeric
	 */
	} else if (isalnum(str[0])) {
	    *first_alphanum = true;
	    if (isupper(str[0])) {
		*upper = true;
		dbg(DBG_VVVHIGH, "posix_safe_chk(): str[0] is UPPER CASE alphanumeric: 0x%02x", (unsigned int)str[0]);
	    } else {
		dbg(DBG_VVVHIGH, "posix_safe_chk(): str[0] is alphanumeric: 0x%02x", (unsigned int)str[0]);
	    }

	/*
	 * case: first character is non-alphanumeric portable POSIX safe plus +
	 */
	} else if (str[0] == '.' || str[0] == '_' || str[0] == '+') {
	    dbg(DBG_VVVHIGH, "posix_safe_chk(): str[0] is ASCII non-alphanumeric POSIX portable safe plus +: 0x%02x",
			     (unsigned int)str[0]);

	/*
	 * case: first character is not POSIX portable safe plus +
	 */
	} else {
	    found_unsafe = true;
	    dbg(DBG_VVVHIGH, "posix_safe_chk(): str[0] is not POSIX portable safe plus +/: 0x%02x",
			     (unsigned int)str[0]);
	}

    /*
     * case: first character is not ASCII
     */
    } else {
	found_unsafe = true;
	dbg(DBG_VVVHIGH, "posix_safe_chk(): str[0] is non-ASCII: 0x%02x",
			 (unsigned int)str[0]);
    }

    /*
     * example second to last characters
     */
    for (i=1; i < len; ++i) {

	/*
	 * test character
	 */
	if (isascii(str[i])) {

	    /*
	     * case: / check
	     */
	    if (str[i] == '/') {
		if (*slash == false) {
		    dbg(DBG_VVVHIGH, "posix_safe_chk(): found first / at str[%ju]: 0x%02x",
				     (uintmax_t)i, (unsigned int)str[i]);
		}
		*slash = true;

	    /*
	     * case: character is alphanumeric
	     */
	    } else if (isalnum(str[i])) {
		if (*upper == false && isupper(str[i])) {
		    dbg(DBG_VVVHIGH, "posix_safe_chk(): found first UPPER case at str[%ju]: 0x%02x",
				     (uintmax_t)i, (unsigned int)str[i]);
		    *upper = true;
		}

	    /*
	     * case: is not POSIX portable safe plus +
	     */
	    } else if (str[i] != '.' && str[i] != '_' && str[i] != '+' && str[i] != '-') {
		if (found_unsafe == false) {
		    dbg(DBG_VVVHIGH, "posix_safe_chk(): str[%ju] found first non-POSIX portable safe plus +/: 0x%02x",
				      (uintmax_t)i, (unsigned int)str[i]);
		}
		found_unsafe = true;
	    }

	/*
	 * case: character is non-ASCII
	 */
	} else {
	    if (found_unsafe == false) {
		dbg(DBG_VVVHIGH, "posix_safe_chk(): str[%ju] found first non-ASCII: 0x%02x",
				  (uintmax_t)i, (unsigned int)str[i]);
	    }
	    found_unsafe = true;
	}
    }

    /*
     * report non-POSIX portable safe plus + maybe /
     */
    if (found_unsafe == false) {
	*posix_safe = true;
	dbg(DBG_VVHIGH, "posix_safe_chk(..., %s, %s, %s, %s): string is NOT POSIX portable safe plus +/",
		         booltostr(slash), booltostr(posix_safe), booltostr(first_alphanum), booltostr(upper));

    /*
     * report POSIX portable safe plus + safe with maybe /
     */
    } else {
	dbg(DBG_VVHIGH, "posix_safe_chk(..., %s, %s, %s, %s): string is POSIX portable safe plus +/: <%s>",
		        booltostr(slash), booltostr(posix_safe), booltostr(first_alphanum), booltostr(upper), str);
    }
    return;
}


/*
 * find_matching_quote	find the next unescaped '"'
 *
 * Assuming *q == '"' find the matching (closing) '"' (i.e. the next unescaped
 * '"') and return a pointer to it (or NULL if not found). If *q != '"' && *q !=
 * '\0' it will return 'q'; else it'll return q updated to either the end of the
 * string (which means NULL return value) or the matching unescaped '"'.
 *
 * NOTE: This function does not return on NULL pointer passed in but it can
 * return a NULL pointer: this happens if no matching '"' is found (or the
 * string was empty in the first place).
 */
char *
find_matching_quote(char *q)
{
    /*
     * firewall
     */
    if (q == NULL) {
	err(179, __func__, "passed NULL pointer");
	not_reached();
    }

    for (++q; *q && *q != '"'; ++q)
	if (*q == '\\')
	    ++q;

    if (!*q)
	return NULL;

    return q;
}


/*
 * clearerr_or_fclose
 *
 * If stream is NULL, this function does nothing.
 *
 * This function calls clearerr() is stream is stdin, or stdout, or stderr.
 * Otherwise fclose() will be called on the stream.
 *
 * given:
 *	stream		open stream to clear or close, or NULL ==> do nothing
 */
void
clearerr_or_fclose(FILE *stream)
{
    int ret;

    /*
     * firewall
     */
    if (stream == NULL) {
	return;
    }

    /*
     * if stdin, stdout or stderr, then clear the error flag
     */
    if (stream == stdin || stream == stdout || stream == stderr) {
	clearerr(stream);

    /*
     * close the stream is neither stdin, nor stdout, or stderr
     */
    } else {
	errno = 0;		/* pre-clear errno for warnp() */
	ret = fclose(stream);
	if (ret != 0) {
	    warnp(__func__, "failed to fclose stream");
	}
    }
    return;
}


/*
 * fprint_line_buf - print a buffer as a single line string on a stream
 *
 * Unlike the older fprint_str(), which printed normal chars normally and C
 * escape chars as literal C escape chars (i.e. if a \n was encountered it would
 * print "\\n" to show that it was a newline), this function will print the
 * buffer as a single line, possibly enclosed in starting and ending characters.
 * Non-ASCII characters, non-printable ASCII characters, \-characters, start and
 * end characters (if non-NUL) will all be printed as \x99 where 99 is the hex
 * value, or \C where C is a C-style \-character.
 *
 * The line printed will be printed as a single line, without a final
 * newline.
 *
 * When stream == NULL, no output is performed.  This is useful to
 * determine the length of the single line that would be printed.
 * This length includes any non-0 start and end characters.
 *
 * The stream is flushed before returning.
 *
 * The errno value is restore to its original state before returning.
 *
 * Examples:
 *	line_len = fprint_line_buf(stderr, buf, len, '<', '>');
 *	line_len = fprint_line_buf(stderr, buf, len, '"', '"');
 *	line_len = fprint_line_buf(stderr, buf, len, 0, '\n');
 *	line_len = fprint_line_buf(stderr, buf, len, 0, 0);
 *	line_len = fprint_line_buf(NULL, buf, len, 0, 0);
 *
 * given:
 *	stream	open stream to write to, or NULL ==> just return length
 *	buf	buffer to print
 *	len	the length of the buffer
 *	start	print start character before line, if != 0,
 *		   any start character found in buf will be \-escaped
 *	end	print end character after line, if != 0,
 *		   any start character found in buf will be \-escaped
 *
 * returns:
 *	length of line (even if MULL stream), or
 *	EOF ==> write error or NULL buf
 */
ssize_t
fprint_line_buf(FILE *stream, void *buf, size_t len, int start, int end)
{
    size_t count = 0;		/* number of characters in line */
    int delayed_errno = 0;	/* for printing warning at end of function if necessary */
    bool success = true;	/* if no errors (assume no errors at start) */
    int saved_errno = 0;	/* saved errno to restore before returning */
    int ret;			/* libc function return */
    int c;			/* a byte in buf to print */
    size_t i;

    /*
     * save errno
     */
    saved_errno = errno;

    /*
     * firewall
     */
    if (buf == NULL) {
	warn(__func__, "buf is NULL");
	errno = saved_errno;
	return EOF;
    }

    /*
     * print start if non-NUL on non-NULL stream
     */
    if (start != 0) {

	/* print start if non-NULL stream */
	if (stream != NULL) {
	    errno = 0;	/* clear errno */
	    ret = fputc(start, stream);
	    if (ret == EOF) {
		delayed_errno = errno;
		success = false;
	    }
	}

	/* count the character printed */
	++count;
    }

    /*
     * print each byte of buf
     */
    for (i=0; i < len; ++i) {

	/*
	 * print based on the byte value
	 */
	c = (int)(((uint8_t *)buf)[i]);

	/*
	 * case: character is non-NUL start or non-NUL end or non-ASCII character
	 */
	if ((start != 0 && c == start) || (end != 0 && c == end) || ! isascii(c)) {

	    /* print character as \x99 if non-NULL stream to avoid start/end confusion */
	    if (stream != NULL) {
		errno = 0;  /* clear errno */
		ret = fprintf(stream, "\\x%02x", c);
		if (ret != 2) {
		    delayed_errno = errno;
		    success = false;
		}
	    }

	    /* count the 4 characters */
	    count += 4;

	/*
	 * case: ASCII character
	 */
	} else {

	    /*
	     * print ASCII character based on character value
	     */
	    switch(c) {

	    case 0:	/* NUL */

		/* print \0 if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\0");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\a':	/* alert (beep, bell) */

		/* print \a if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\a");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\b':	/* backspace */

		/* print \b if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\b");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case 0x1b:	/* escape */

		/* print \e if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\e");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\f':	/* form feed page break */

		/* print \f if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\f");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\n':	/* newline */

		/* print \n if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\n");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\r':	/* carriage return */

		/* print \r if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\r");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\t':	/* horizontal tab */

		/* print \t if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\t");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\v':	/* vertical tab */

		/* print \v if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\v");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    case '\\':	/* backslash */

		/* print \\ if non-NULL stream */
		if (stream != NULL) {
		    errno = 0;	/* clear errno */
		    ret = fprintf(stream, "\\\\");
		    if (ret != 2) {
			delayed_errno = errno;
			success = false;
		    }
		}

		/* count the 2 characters */
		count += 2;
		break;

	    default:	/* other characters */

		/*
		 * case: ASCII printable character
		 */
		if (isprint(c)) {

		    /* print character if non-NULL stream */
		    if (stream != NULL) {
			errno = 0;	/* clear errno */
			ret = fputc(c, stream);
			if (ret == EOF) {
			    delayed_errno = errno;
			    success = false;
			}
		    }

		    /* count the character printed */
		    ++count;

		/*
		 * case: ASCII non-printable character
		 */
		} else {

		    /* print character as \x99 if non-NULL stream */
		    if (stream != NULL) {
			errno = 0;  /* clear errno */
			ret = fprintf(stream, "\\x%02x", c);
			if (ret != 4) {
			    delayed_errno = errno;
			    success = false;
			}
		    }

		    /* count the 4 characters */
		    count += 4;
		}
		break;
	    }
	}
    }

    /*
     * print end if non-NUL on non-NULL stream
     */
    if (end != 0) {

	/* print end if non-NULL stream */
	if (stream != NULL) {
	    errno = 0;	/* clear errno */
	    ret = fputc(end, stream);
	    if (ret == EOF) {
		delayed_errno = errno;
		success = false;
	    }
	}

	/* count the character printed */
	++count;
    }

    /*
     * flush stream if non-NULL stream
     */
    if (stream != NULL) {
	errno = 0;	/* clear errno */
	ret = fflush(stream);
	if (ret == EOF) {
	    delayed_errno = errno;
	    success = false;
	}
    }

    /*
     * if we had an print error, report the last errno that was observed
     */
    if (delayed_errno != 0) {
	warn(__func__, "last write errno: %d (%s)", delayed_errno, strerror(delayed_errno));
    }

    /*
     * restore errno
     */
    errno = saved_errno;

    /*
     * return length or EOF if write error
     */
    if (success == false) {
	return EOF;
    }
    return count;
}


/*
 * fprint_line_str - print a string as a single line string on a stream
 *
 * This is a simplified interface for fprint_line_buf().  See that function for
 * details.
 *
 * given:
 *	stream	open stream to write to, or NULL ==> just return length
 *	str	string to print
 *	retlen	address of where to store length of str, if retlen != NULL
 *	start	print start character before line, if != 0,
 *		   any start character found in buf will be \-escaped
 *	end	print end character after line, if != 0,
 *		   any start character found in buf will be \-escaped
 *
 * returns:
 *	length of line (even if MULL stream), or
 *	EOF ==> write error or NULL buf
 */
ssize_t
fprint_line_str(FILE *stream, char *str, size_t *retlen, int start, int end)
{
    size_t count = 0;		/* number of characters in line */
    int saved_errno = 0;	/* saved errno to restore before returning */
    size_t len = 0;		/* string length */

    /*
     * save errno
     */
    saved_errno = errno;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	errno = saved_errno;
	return EOF;
    }
    len = strlen(str);

    /*
     * convert to fprint_line_buf() call
     */
    count = fprint_line_buf(stream, str, len, start, end);

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * restore errno
     */
    errno = saved_errno;

    /*
     * return the length
     */
    return count;
}


/*
 * find_text - find ASCII text within a field of whitespace and trailing NUL bytes
 *
 * Find the first ASCII text, ignoring any leading whitespace is found.
 * Determine the length, within the len limit, of ASCII text that is
 * neither whitespace nor NUL byte.
 *
 * given:
 *	ptr	    address to start looking for text
 *	len	    amount of data to search through
 *	**first	    if non-NULL and return > 0, location of first
 *		    non-whitespace/non-NUL text
 *
 * returns:
 *	number of non-whitespace/non-NUL bytes found, or
 *	0 if no non-whitespace/non-NUL bytes found, or if buf == NULL
 */
size_t
find_text(char const *ptr, size_t len, char **first)
{
    size_t ret = 0;	/* number of non-whitespace/non-NUL bytes found */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "ptr is NULL");
	return 0;
    }
    if (len <= 0) {
	return 0;
    }

    /*
     * scan the buffer for non-whitespace that is not NUL
     */
    for (i=0; i < len; ++i) {
	if (!isascii(ptr[i]) || !isspace(ptr[i]) || ptr[i] == '\0') {
	    break;
	}
    }

    /*
     * case: only whitespace found
     */
    if (i >= len) {
	return 0;
    }

    /*
     * note the first non-whitespace character if required
     */
    if (first != NULL) {
	*first = (char *)ptr+i;
    }

    /*
     * determine the length of non-whitespace that is not NUL
     */
    for (ret=1, ++i; i < len; ++i, ++ret) {
	if ((isascii(ptr[i]) && isspace(ptr[i])) || ptr[i] == '\0') {
	    break;
	}
    }

    /*
     * return length
     */
    return ret;
}


/*
 * find_text_str - find ASCII text within a field of whitespace and trailing NUL bytes
 *
 * This is a simplified interface for find_text().
 *
 * given:
 *	str	    address of a NUL terminated string to start looking for text
 *	**first	    if non-NULL and return > 0, location of first non-whitespace/non-NUL text
 *
 * returns:
 *	number of non-whitespace/non-NUL bytes found, or
 *	0 if no non-whitespace/non-NUL bytes found, or if buf == NULL
 */
size_t
find_text_str(char const *str, char **first)
{
    size_t ret = 0;	/* number of non-whitespace/non-NUL bytes found */
    size_t len = 0;	/* length of str */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "str is NULL");
	return 0;
    }

    /*
     * convert to find_text() call
     */
    len = strlen(str);
    ret = find_text(str, len, first);

    /*
     * return the number of non-NUL non-whitespace bytes
     */
    return ret;
}


/*
 * sum_and_count - add to a sum, count the number of additions
 *
 * Given an integer, we will add to the converted value to a sum and count it.
 *
 * Programmer's apology:
 *
 *	We go through a number of extraordinary steps to try and make sure that we
 *	correctly sum and count, even in the face of certain hardware errors and
 *	various stack memory correction problems.  Thus, we do much more
 *	than ++count; sum += value; in this function.
 *
 * Example usage:
 *
 *	... static values private to some .c file (outside of any function) ...
 *
 *	static intmax_t sum_check;
 *	static intmax_t count_check;
 *
 *	... at start of function that is checking the total file sum and count ...
 *
 *	intmax_t sum = 0;
 *	intmax_t count = 0;
 *	intmax_t length = 0;
 *	bool test = false;
 *
 *	... loop the following over ALL files where length_str is the length of the current file ...
 *
 *	test = string_to_intmax2(length_str, &length);
 *	if (test == false) {
 *	    ... object to file length string not being a non-negative base 10 integer ...
 *	}
 *
 *	test = sum_and_count(length, &sum, &count, &sum_check, &count_check);
 *	if (test == false) {
 *	    ... object to internal/computational error ...
 *	}
 *	if (sum < 0) {
 *	    ... object to negative total file length  ...
 *	}
 *	if (sum > MAX_SUM_FILELEN) {
 *	    ... object to sum of all file lengths being too large ...
 *	}
 *	if (count <= 0) {
 *	    ... object no or negative file count ...
 *	}
 *	if (count > MAX_FILE_COUNT) {
 *	    ... object to too many files ...
 *	}
 *
 * given:
 *	value		    non-negative value to sum
 *	sump		    pointer to the sum
 *	countp		    pointer to the current count
 *	sum_checkp	    pointer to negative of previous sum (should be a pointer to a static global value)
 *	count_checkp	    pointer to negative of previous count (should be a pointer to a static global value)
 *
 * return:
 *	true ==> sum successful, count successful,
 *		 value added to *sump, *countp incremented
 *	false ==> sum error occurred, value is not a non-negative integer, NULL pointer, internal error
 *
 * NOTE: Errors in computation result in a call to warn(), whereas a negative value will only call dbg().
 *	 A false value is returned in either case.
 *
 * NOTE: The values *sum_checkp and *count_checkp are pointers to intmax_t values that are for
 *	 internal function use only.  It is recommended, but not required, that these point to global static values.
 */
bool
sum_and_count(intmax_t value, intmax_t *sump, intmax_t *countp, intmax_t *sum_checkp, intmax_t *count_checkp)
{
    intmax_t sum = -1;		/* imported sum of valid byte lengths found so far */
    intmax_t count = 0;		/* imported count of the number of sums */
    intmax_t prev_sum = -1;	/* previous value of sum / -1 is an invalid sum */
    intmax_t prev_count = 0;	/* previous number of additions */
    intmax_t inv_prev_sum = ~ -1;	/* inverted previous sum */
    intmax_t inv_prev_count = ~ 0;	/* inverted previous count */
    intmax_t inv_sum = ~ -1;	/* inverted sum */
    intmax_t inv_count = ~ 0;	/* inverted count */
    intmax_t inv_value = ~ -1;	/* inverted value */

    /*
     * firewall
     */
    if (sump == NULL) {
	warn(__func__, "sump is NULL");
	return false;
    }
    if (countp == NULL) {
	warn(__func__, "countp is NULL");
	return false;
    }
    if (sum_checkp == NULL) {
	warn(__func__, "sum_checkp is NULL");
	return false;
    }
    if (count_checkp == NULL) {
	warn(__func__, "count_checkp is NULL");
	return false;
    }

    /*
     * check for invalid negative values
     */
    inv_value = ~value;
    if (value < 0) {
	dbg(DBG_HIGH, "sum_and_count value argument < 0: value %jd < 0", value);
	return false;
    }

    /*
     * import count and sum
     */
    sum = *sump;
    count = *countp;
    inv_sum = ~sum;
    inv_count = ~count;

    /*
     * save previous values of counts and sum in various ways
     */
    prev_sum = sum;
    prev_count = count;
    inv_prev_sum = ~prev_sum;
    *sum_checkp = -(*sump);
    inv_prev_count = ~prev_count;
    *count_checkp = -(*countp);
    if (~inv_sum != *sump) {
	dbg(DBG_HIGH, "inv_sum: %jd", inv_sum);
	dbg(DBG_HIGH, "*sump: %jd", *sump);
	warn(__func__, "imported inverted sum changed: ~inv_sum %jd != *sump %jd", ~inv_sum, *sump);
	return false;
    }
    if (*sump != sum) {
	dbg(DBG_HIGH, "*sump: %jd", *sump);
	dbg(DBG_HIGH, "sum: %jd", sum);
	warn(__func__, "imported sum changed: *sump %jd != sum %jd", *sump, sum);
	return false;
    }
    if (~inv_count != *countp) {
	dbg(DBG_HIGH, "inv_count: %jd", inv_count);
	dbg(DBG_HIGH, "*countp: %jd", *countp);
	warn(__func__, "imported inverted count changed: ~inv_count %jd != *countp %jd", ~inv_count, *countp);
	return false;
    }
    if (*countp != count) {
	dbg(DBG_HIGH, "*countp: %jd", *countp);
	dbg(DBG_HIGH, "count: %jd", count);
	warn(__func__, "imported count changed: *countp %jd != count %jd", *countp, count);
	return false;
    }
    if (*sum_checkp != -sum) {
	dbg(DBG_HIGH, "*sum_checkp: %jd", *sum_checkp);
	dbg(DBG_HIGH, "sum: %jd", sum);
	warn(__func__, "sum negation changed: *sum_checkp %jd != -sum %jd", *sum_checkp, -sum);
	return false;
    }
    if (*count_checkp != -count) {
	dbg(DBG_HIGH, "*count_checkp: %jd", *count_checkp);
	dbg(DBG_HIGH, "count: %jd", count);
	warn(__func__, "count negation changed: *count_checkp %jd != -count %jd", *count_checkp, -count);
	return false;
    }

    /*
     * paranoid count increment
     */
    ++count; /* now count > *countp */
    if (count <= 0) {
	warn(__func__, "incremented count went negative: count %jd <= 0", count);
	return false;
    }
    if (count <= prev_count) {
	warn(__func__, "incremented count is lower than previous count: count %jd <= prev_count %jd", count, prev_count);
	return false;
    }

    /*
     * attempt the sum
     */
    dbg(DBG_HIGH, "adding value %jd to sum %jd", value, sum);
    sum += value;
    dbg(DBG_HIGH, "new sum: %jd", sum);

    /*
     * more paranoia: sum cannot be negative
     */
    if (sum < 0) {
	warn(__func__, "sum went negative: sum %jd < 0", sum);
	return false;
    }

    /*
     * more paranoia: sum cannot be < previous sum
     */
    if (sum < prev_sum) {
	warn(__func__, "sum < previous sum: sum %jd < prev_sum %jd", sum, prev_sum);
	return false;
    }

    /*
     * try to verify a consistent previous sum
     */
    if (prev_sum != ~inv_prev_sum) {
	dbg(DBG_HIGH, "prev_sum: %jd", prev_sum);
	dbg(DBG_HIGH, "inv_prev_sum: %jd", inv_prev_sum);
	warn(__func__, "unexpected change to the previous sum: prev_sum %jd != ~inv_prev_sum %jd", prev_sum, ~inv_prev_sum);
	return false;
    } else if (-prev_sum != *sum_checkp) {
	dbg(DBG_HIGH, "prev_sum: %jd", prev_sum);
	dbg(DBG_HIGH, "*sum_checkp: %jd", *sum_checkp);
	warn(__func__, "unexpected change to the previous sum: -prev_sum %jd != *sum_checkp %jd", -prev_sum, *sum_checkp);
	return false;
    }

    /*
     * second and third sanity check for count increment
     */
    if ((*countp) != count-1) {
	dbg(DBG_HIGH, "*countp: %jd", *countp);
	dbg(DBG_HIGH, "count: %jd", count);
	warn(__func__, "second check on count increment failed: (*countp) %jd != (count-1) %jd", (*countp), count-1);
	return false;
    }
    if (count != prev_count+1) {
	dbg(DBG_HIGH, "count: %jd", count);
	dbg(DBG_HIGH, "prev_count: %jd", prev_count);
	warn(__func__, "third check on count increment failed: count %jd != (prev_count+1) %jd", count, prev_count + 1);
	return false;
    }

    /*
     * try to verify a consistent previous count
     */
    if (-prev_count != *count_checkp) {
	dbg(DBG_HIGH, "prev_count: %jd", prev_count);
	dbg(DBG_HIGH, "*count_checkp: %jd", *count_checkp);
	warn(__func__, "unexpected change to the previous count: -prev_count %jd != *count_checkp %jd", -prev_count, *count_checkp);
	return false;
    } else if (prev_count != ~inv_prev_count) {
	dbg(DBG_HIGH, "prev_count: %jd", prev_count);
	dbg(DBG_HIGH, "inv_prev_count: %jd", inv_prev_count);
	warn(__func__, "unexpected change to the previous count: prev_count %jd != ~inv_prev_count %jd",
		prev_count, ~inv_prev_count);
	return false;
    }

    /*
     * second and third sanity check for sum
     */
    if ((*sum_checkp)-value != -sum) {
	dbg(DBG_HIGH, "*sum_checkp: %jd", *sum_checkp);
	dbg(DBG_HIGH, "value: %jd", value);
	dbg(DBG_HIGH, "(*sum_checkp)-value: %jd", (*sum_checkp)-value);
	dbg(DBG_HIGH, "sum: %jd", sum);
	warn(__func__, "second check on sum failed: (*sum_checkp)-value %jd != -sum %jd", (*sum_checkp)-value, -sum);
	return false;
    }

    /*
     * second sanity check for value
     */
    if (~inv_value != value) {
	dbg(DBG_HIGH, "inv_value: %jd", inv_value);
	dbg(DBG_HIGH, "value: %jd", value);
	warn(__func__, "value unexpectedly changed: ~inv_val %jd != value %jd", ~inv_value, value);
	return false;
    }

    /*
     * final checks in counts and values
     */
    if (*countp != ~inv_count) {
	dbg(DBG_HIGH, "*countp: %jd", *countp);
	dbg(DBG_HIGH, "inv_count: %jd", inv_count);
	warn(__func__, "final check on imported inverted count changed: *countp %jd != ~inv_count %jd", *countp, ~inv_count);
	return false;
    }
    if (*sump != ~inv_sum) {
	dbg(DBG_HIGH, "*sump: %jd", *sump);
	dbg(DBG_HIGH, "inv_sum: %jd", inv_sum);
	warn(__func__, "final check on imported inverted sum changed: *sump %jd != ~inv_sum %jd", *sump, ~inv_sum);
	return false;
    }
    if (count < 0) {
	warn(__func__, "final check: count is negative: count %jd < 0", count);
	return false;
    }
    if (count == 0) {
	warn(__func__, "final check: count is 0: count == %jd", count);
	return false;
    }
    if (sum < 0) {
	warn(__func__, "final check: sum is negative: sum %jd < 0", sum);
	return false;
    }

    /*
     * update sum and count
     */
    *sump = sum;
    *countp = count;

    dbg(DBG_HIGH, "new sum is %jd", *sump);
    dbg(DBG_HIGH, "new count is %jd", *countp);

    /*
     * report sum and count success
     */
    return true;
}


/*
 * malloc_path - malloc a JSON file path
 *
 * given:
 *	dirname		directory containing file, or NULL ==> file is by itself
 *	filename	path of a file (if dirname == NULL), or path under dirname (if dirname != NULL)
 *
 * returns:
 *	malloced path
 *
 * NOTE: This function does not return on error.
 * NOTE: This function will not return NULL.
 */
char *
malloc_path(char const *dirname, char const *filename)
{
    int ret = -1;		/* libc return status */
    char *buf = NULL;		/* malloced string to return */
    size_t len;			/* length of path */

    /*
     * firewall
     */
    if (filename == NULL) {
	err(180, __func__, "filename is NULL");
	not_reached();
    }

    /*
     * case: dirname is NULL
     *
     * NULL dirname means path is just filename
     */
    if (dirname == NULL) {

	/*
	 * just return a newlt malloced filename
	 */
	errno = 0;		/* pre-clear errno for errp() */
	buf = strdup(filename);
	if (buf == NULL) {
	    errp(181, __func__, "strdup of filename failed: %s", filename);
	    not_reached();
	}

    /*
     * case: dirname is not NULL
     *
     * We need to form: dirname/filename
     */
    } else {

	/*
	 * malloc string
	 */
	len = strlen(dirname) + 1 + strlen(filename) + 1;	/* +1 for NUL */
	buf = calloc(len+1, sizeof(char));	/* + 1 for paranoia padding */
	errno = 0;		/* pre-clear errno for errp() */
	if (buf == NULL) {
	    errp(182, __func__, "malloc of %ju bytes failed", (uintmax_t)len);
	    not_reached();
	}

	/*
	 * form string
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = snprintf(buf, len, "%s/%s", dirname, filename);
	if (ret < 0) {
	    errp(183, __func__, "snprintf returned: %zu < 0", len);
	    not_reached();
	}
    }

    /*
     * return malloc path
     */
    if (buf == NULL) {
	errp(184, __func__, "function attempted to return NULL");
	not_reached();
    }
    return buf;
}
