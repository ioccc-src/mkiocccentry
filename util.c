/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * util - IOCCC entry common utility functions
 *
 * Utility functions that are common to more than one utility
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
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */

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
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions
 */
#include "util.h"


/*
 * IOCCC limits
 */
#include "limit_ioccc.h"


/*
 * base_name - determine the final portion of a path
 *
 * given:
 *      path    - path to form the basename from
 *
 * returns:
 *      malloced basename
 *
 * This function does not return on error.
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
 *
 *      path    - the path to test
 *
 * returns:
 *      true ==> path exists,
 *      false ==> path does not exist
 *
 * This function does not return.
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
 *
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
 * is_exec - if a path is executable
 *
 * This function tests if a path exists and we have permissions to execute it.
 *
 * given:
 *
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);

    /*
     * test if we are allowed to execute it
     */
    ret = access(path, X_OK);
    if (ret < 0) {
	dbg(DBG_HIGH, "path %s is not executable", path);
	return false;
    }
    dbg(DBG_VHIGH, "path %s is executable", path);
    return true;
}


/*
 * is_dir - if a path is a directory
 *
 * This function tests if a path exists and is a directory.
 *
 * given:
 *
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
 *
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
 *
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
 * file_size - determine the file size
 *
 * Return the file size, or -1 if the file does not exist.
 *
 * given:
 *
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
 * given:
 *
 *      format - The format string, any % on this string inserts the next string from the list,
 *               escaping special characters that the shell might threaten as command characters.
 *               In the worst case, the algorithm will make twice as many characters.
 *               Will not use escaping if it isn't needed.
 *      ...    - args to give after the format
 *
 * returns:
 *	malloced shell command line, or
 *	NULL ==> error
 */
char *
cmdprintf(char const *format, ...)
{
    va_list ap;
    char *cmd = NULL;

    /*
     * firewall
     */
    if (format == NULL) {
	warn(__func__, "format is NULL");
	return NULL;
    }

    /*
     * stdarg setup
     */
    va_start(ap, format);

    /*
     * call vcmdprintf()
     */
    cmd = vcmdprintf(format, ap);

    /*
     * stdarg cleanup
     */
    va_end(ap);

    /*
     * return safer command line string
     */
    return cmd;
}


/*
 * vcmdprintf - malloc a safer shell command line for use with system() and popen()
 *
 * given:
 *
 *      format - The format string, any % on this string inserts the next string from the list,
 *               escaping special characters that the shell might threaten as command characters.
 *               In the worst case, the algorithm will make twice as many characters.
 *               Will not use escaping if it isn't needed.
 *      ap     - va_list of arguments for format
 *
 * returns:
 *	malloced shell command line, or
 *	NULL ==> error
 *
 * NOTE: This code is based on an enhancement request by GitHub user @ilyakurdyukov:
 *
 *		https://github.com/ioccc-src/mkiocccentry/issues/11
 *
 *	 and this function code was written by him.  Thank you Ilya Kurdyukov!
 */
char *
vcmdprintf(char const *format, va_list ap)
{
    va_list ap2;		/* copy of original va_list for 2nd pass */
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
    if (format == NULL) {
	warn(__func__, "format is NULL");
	return NULL;
    }

    /*
     * copy va_list for 2nd pass
     */
    va_copy(ap2, ap);

    /*
     * pass 0: determine how much storage we will need for the command line
     */
    f = format;
    while ((c = *f++)) {
	if (c == '%') {
	    p = next = va_arg(ap, char const *);
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
	    /* counted % sign in the format string */
	    size += (size_t)(nquot >= 2 ? 2 : nquot) + (size_t)(p - next) - 2;
	}
    }
    va_end(ap);
    size += (size_t)(f - format);

    /*
     * malloc storage or return NULL
     */
    errno = 0;			/* pre-clear errno for warnp() */
    cmd = (char *)malloc(size);	/* trailing zero included in size */
    if (cmd == NULL) {
	warnp(__func__, "malloc from vcmdprintf of %ju bytes failed", (uintmax_t)size);
	return NULL;
    }

    /*
     * pass 1: form the safer command line
     */
    d = cmd;
    f = format;
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
    va_end(ap2);
    *d = '\0';	/* NUL terminate command line */

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
 *			    return MALLOC_FAILED_EXIT malloc() failure,
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
 *	 Moreover if flag == false, a simple check if the return was < 0 will allow the calling
 *	 function to determine if this function failed.
 */
int
shell_cmd(char const *name, bool abort_on_error, char const *format, ...)
{
    va_list ap;			/* stdarg block */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
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
     * stdarg setup
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
	    errp(112, name, "malloc failed in vcmdprintf()");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: malloc failed in vcmdprintf(): %s, returning: %d < 0",
			 name, strerror(errno), MALLOC_FAILED_EXIT);
	    va_end(ap);
	    return MALLOC_FAILED_EXIT;
	}
    }

    /*
     * pre-flush stdout to avoid system() buffered stdio issues
     */
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    ret = fflush(stdout);
    if (ret < 0) {
	/* free malloced command storage */
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
	    va_end(ap);
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
	/* free malloced command storage */
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
	    va_end(ap);
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
	/* free malloced command storage */
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
	    va_end(ap);
	    return SYSTEM_FAILED_EXIT;
	}

    /*
     * case: exit code 127 usually means the fork/exec was unable to invoke the shell
     */
    } else if (exit_code == 127) {
	/* free malloced command storage */
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
	    va_end(ap);
	    return SYSTEM_FAILED_EXIT;
	}
    }

    /*
     * free malloced command storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }

    /*
     * stdarg cleanup
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
    va_list ap;			/* stdarg block */
    char *cmd = NULL;		/* cp prog_c entry_dir/prog.c */
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
     * stdarg setup
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
	    errp(119, name, "malloc failed in vcmdprintf()");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: malloc failed in vcmdprintf(): %s returning: %d < 0",
			 name, strerror(errno), MALLOC_FAILED_EXIT);
	    va_end(ap);
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
	/* free malloced command storage */
	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    errp(120, name, "fflush(stdout): error code: %d", ret);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: fflush(stdout) failed: %s", name, strerror(errno));
	    va_end(ap);
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
	/* free malloced command storage */
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
	    va_end(ap);
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
	/* free malloced command storage */
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
	    va_end(ap);
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
     * free malloced command storage
     */
    if (cmd != NULL) {
	free(cmd);
	cmd = NULL;
    }

    /*
     * stdarg cleanup
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
 *      line    - 1st paragraph line to print
 *      ...     - strings as paragraph lines to print
 *      NULL    - end of string list
 *
 * This function does not return on error.
 */
void
para(char const *line, ...)
{
    va_list ap;			/* stdarg block */
    int ret;			/* libc function return value */
    int fd;			/* stdout as a file descriptor or -1 */
    int line_cnt;		/* number of lines in the paragraph */

    /*
     * stdarg setup
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
     * this may not always catch a bogus or un-opened stdout, but try anyway
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
    line_cnt = 0;
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
	++line_cnt;		/* count this line as printed */

	/*
	 * move to next line string
	 */
	line = va_arg(ap, char *);
    }

    /*
     * stdarg cleanup
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
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __func__, line_cnt);
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
 *      line    - 1st paragraph line to print
 *      ...     - strings as paragraph lines to print
 *      NULL    - end of string list
 *
 * This function does not return on error.
 */
void
fpara(FILE * stream, char const *line, ...)
{
    va_list ap;			/* stdarg block */
    int ret;			/* libc function return value */
    int fd;			/* stream as a file descriptor or -1 */
    int line_cnt;		/* number of lines in the paragraph */

    /*
     * stdarg setup
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
     * this may not always catch a bogus or un-opened stream, but try anyway
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
    line_cnt = 0;
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
	++line_cnt;		/* count this line as printed */

	/*
	 * move to next line string
	 */
	line = va_arg(ap, char *);
    }

    /*
     * stdarg cleanup
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
    dbg(DBG_VVHIGH, "%s() printed %d line paragraph", __func__, line_cnt);
    return;
}


/*
 * readline - read a line from a stream
 *
 * Read a line from an open stream.  Malloc or realloc the line
 * buffer as needed.  Remove the trailing newline that was read.
 *
 * given:
 *      linep   - malloced line buffer (may be realloced) or ptr to NULL
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
    char *p;			/* printer to NUL */

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
     * scan for embedded NUL bytes (before end of line)
     *
     */
    errno = 0;			/* pre-clear errno for errp() */
    p = (char *)memchr(*linep, 0, (size_t)ret);
    if (p != NULL) {
	errp(150, __func__, "found NUL before end of line");
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
    dbg(DBG_VVHIGH, "read %jd bytes + newline into %ju byte buffer", (intmax_t)ret, (uintmax_t)linecap);

    /*
     * return length of line without the trailing newline
     */
    return ret;
}


/*
 * readline_dup - read a line from a stream and duplicate to a malloced buffer.
 *
 * given:
 *      linep   - malloced line buffer (may be realloced) or ptr to NULL
 *                NULL ==> getline() will malloc() the linep buffer
 *                else ==> getline() might realloc() the linep buffer
 *      strip   - true ==> remove trailing whitespace,
 *                false ==> only remove the trailing newline
 *      lenp    - != NULL ==> pointer to length of final length of line malloced,
 *                NULL ==> do not return length of line
 *      stream - file stream to read from
 *
 * returns:
 *      malloced buffer containing the input without a trailing newline,
 *      and if strip == true, without trailing whitespace,
 *      or NULL ==> EOF
 *
 * This function does not return on error.
 */
char *
readline_dup(char **linep, bool strip, size_t *lenp, FILE *stream)
{
    ssize_t len;		/* getline return and our modified size return */
    char *ret;			/* malloced input */
    ssize_t i;

    /*
     * firewall
     */
    if (linep == NULL || stream == NULL) {
	err(151, __func__, "called with NULL arg(s)");
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
    ret = strdup(*linep);
    if (ret == NULL) {
	errp(152, __func__, "strdup of read line of %jd bytes failed", (intmax_t)len);
	not_reached();
    }

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
     * return the malloced string
     */
    return ret;
}


/*
 * find_utils - find tar, cp, ls, txzchk and fnamchk utilities
 *
 * given:
 *
 *	tar_flag_used	    - true ==> -t tar was used
 *	tar		    - if -t tar was not used and tar != NULL set *tar to to tar path
 *	cp_flag_used	    - true ==> -c cp was used
 *	cp		    - if -c cp was not used and cp != NULL set *cp to cp path
 *	ls_flag_used	    - true ==> -l ls was used
 *	ls		    - if -l ls was not used and ls != NULL set *ls to ls path
 *	txzchk_flag_used    - true ==> -C flag used
 *	txzchk		    - if -C txzchk was used and txzchk != NULL set *txzchk to path
 *	fnamchk_flag_used   - true ==> if fnamchk flag was used
 *	fnamchk		    - if fnamchk option used and fnamchk ! NULL set *fnamchk to path
 *	jinfochk_flag_used  - true ==> -j jinfochk was used
 *	jinfochk	    - if -j jinfochk was used and jinfochk != NULL set *jinfochk to path
 *	jauthchk_flag_used  - true ==> -J jauthchk was used	    -
 *	jauthchk	    - if -J jauthchk was used and jauthchk != NULL set *jauthchk to path
 */
void
find_utils(bool tar_flag_used, char **tar, bool cp_flag_used, char **cp, bool ls_flag_used, char **ls,
	bool txzchk_flag_used, char **txzchk, bool fnamchk_flag_used, char **fnamchk,
	bool jinfochk_flag_used, char **jinfochk, bool jauthchk_flag_used, char **jauthchk)
{
    /*
     * guess where tar, cp and ls utilities are located
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
	dbg(DBG_MED, "tar is not in historic location: %s : will try alternate location: %s", TAR_PATH_0, *tar);
    }
    if (cp != NULL && !cp_flag_used && !is_exec(CP_PATH_0) && is_exec(CP_PATH_1)) {
	*cp = CP_PATH_1;
	dbg(DBG_MED, "cp is not in historic location: %s : will try alternate location: %s", CP_PATH_0, *cp);
    }
    if (ls != NULL && !ls_flag_used && !is_exec(LS_PATH_0) && is_exec(LS_PATH_1)) {
	*ls = LS_PATH_1;
	dbg(DBG_MED, "ls is not in historic location: %s : will try alternate location: %s", LS_PATH_0, *ls);
    }

    /* now do the same for our utilities: txzchk, fnamchk, jinfochk and jauthchk */
    if (txzchk != NULL && !txzchk_flag_used && !is_exec(TXZCHK_PATH_0) && is_exec(TXZCHK_PATH_1)) {
	*txzchk = TXZCHK_PATH_1;
	dbg(DBG_MED, "using default txzchk path: %s", TXZCHK_PATH_1);
    }
    if (fnamchk != NULL && !fnamchk_flag_used && !is_exec(FNAMCHK_PATH_0) && is_exec(FNAMCHK_PATH_1)) {
	*fnamchk = FNAMCHK_PATH_1;
	dbg(DBG_MED, "using default fnamchk path: %s", FNAMCHK_PATH_1);
    }
    if (jinfochk != NULL && !jinfochk_flag_used && !is_exec(JINFOCHK_PATH_0) && is_exec(JINFOCHK_PATH_1)) {
	*jinfochk = JINFOCHK_PATH_1;
	dbg(DBG_MED, "using default jinfochk path: %s", JINFOCHK_PATH_1);
    }
    if (jauthchk != NULL && !jauthchk_flag_used && !is_exec(JAUTHCHK_PATH_0) && is_exec(JAUTHCHK_PATH_1)) {
	*jauthchk = JAUTHCHK_PATH_1;
	dbg(DBG_MED, "using default jauthchk path: %s", JAUTHCHK_PATH_1);
    }


    return;
}


/*
 * round_to_multiple - round up to a multiple
 *
 * given:
 *
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

    if (!multiple || !num || num < 0) {
	return num;
    }

    mod = num % multiple;
    if (!mod) {
        return num;
    }

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
 * The malloced buffer may be larger than the amount of data read.
 * In this case *psize (if psize != NULL) will contain the exact
 * amount of data read, ignoring any extra allocated data.
 * Any extra unused space in the malloced buffer will be zeroized
 * before returning.
 *
 * This function will always add at least one extra byte of allocated
 * data to the end of the malloced buffer (zeroized as mentioned above).
 * These extra bytes(s) WILL be set to NUL.  Thus, a file or stread
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
 *	    .. data has at least 1 internal NUL byte ..
 }	}
 *
 * Because files can contain NUL bytes, the strlen() function on
 * the malloced buffer may return a different length than the
 * amount of data read from stream.  This is also why the function
 * returns a pointer to void.
 *
 */
void *
read_all(FILE *stream, size_t *psize)
{
    uint8_t *buf = NULL;    /* allocated buffer to return */
    uint8_t *tmp = NULL;    /* temporary reallocation buffer */
    size_t size = 0;	    /* size of the buffer allocated */
    size_t used = 0;	    /* amount of data read into the buffer */
    size_t last_read = 0;   /* amount last fread read from open stream */
    long read_cycle = 0;    /* number of read cycles */
    long realloc_cycle = 0; /* number of realloc cycles */

    /*
     * firewall
     */
    if (stream == NULL) {
	err(153, __func__, "called with NULL stream");
	not_reached();
    }

    /*
     * allocate the initial zeroized buffer
     */
    errno = 0;			/* pre-clear errno for warnp() */
    dbg(DBG_VVHIGH, "%s: about to start calloc cycle: %ld", __func__, realloc_cycle);
    buf = calloc(INITIAL_BUF_SIZE, 1);
    if (buf == NULL) {
	warnp(__func__, "calloc of %ju bytes failed", (uintmax_t)INITIAL_BUF_SIZE);
	return NULL;
    }
    size = INITIAL_BUF_SIZE;
    dbg(DBG_VVHIGH, "%s: calloc cycle: %ld new size: %ju", __func__, realloc_cycle, (uintmax_t)size);
    ++realloc_cycle;

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
     * read until stream EOF or ERROR
     */
    do {

	/*
	 * expand buffer as needed for the next potential read
	 */
	if (used + READ_ALL_CHUNK + 1 > size) {

	    /*
	     * realloc buffer
	     */
	    dbg(DBG_VVHIGH, "%s: about to start realloc cycle: %ld", __func__, realloc_cycle);
	    errno = 0;			/* pre-clear errno for warnp() */
	    tmp = realloc(buf, size + READ_ALL_CHUNK);
	    if (tmp == NULL) {
		/* report realloc failure */
		warnp(__func__, "realloc from %ju bytes to %ju bytes failed",
				(uintmax_t)size, (uintmax_t)(size + READ_ALL_CHUNK));
		/* free up the previous buffer */
		if (buf != NULL) {
		    free(buf);
		    buf = NULL;
		}
		/* toss data we might have collected as we cannot read all of the stream */
		size = 0;
		used = 0;
		/* record empty size, if requested */
		if (psize != NULL) {
		    *psize = 0;
		}
		/* return failure to read all of the stream */
		return NULL;
	    }
	    buf = tmp;

	    /*
	     * zeroize expanded chunk of data
	     */
	    memset(buf+size, 0, READ_ALL_CHUNK);

	    /*
	     * note expanded buffer size
	     */
	    size += READ_ALL_CHUNK;
	    dbg(DBG_VVHIGH, "%s: realloc cycle: %ld new size: %ju", __func__, realloc_cycle, (uintmax_t)size);
	    ++realloc_cycle;
	}

	/*
	 * try to read more data from the stream
	 */
	dbg(DBG_VVHIGH, "%s: about to start read cycle: %ld", __func__, read_cycle);
	errno = 0;			/* pre-clear errno for warnp() */
	last_read = fread(buf+used, 1, READ_ALL_CHUNK, stream);
	used += last_read;	/* record newly read data, if any */
	if (last_read == 0 || ferror(stream) || feof(stream)) {

	    /* report the I/O condition */
	    if (feof(stream)) {
		dbg(DBG_HIGH, "normal EOF reading stream at: %ju bytes", (uintmax_t)used);
	    } else if (ferror(stream)) {
		warnp(__func__, "I/O error detected while reading stream at: %ju bytes", (uintmax_t)used);
	    } else {
		warnp(__func__, "fread returned 0 although neither the EOF nor ERROR flag were set: assuming EOF anyway");
	    }

	    /*
	     * stop reading stream
	     */
	    break;
	}
	dbg(DBG_VVHIGH, "%s: read cycle: %ld read count: %ju", __func__, read_cycle, (uintmax_t)read_cycle);
	++read_cycle;
    } while (true);

    /*
     * report the amount of data actually read, if requested
     */
    if (psize != NULL) {
	*psize = used;
    }

    /*
     * return the malloced buffer
     */
    return buf;
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
	dbg(DBG_VVHIGH, "is_string: no NUL found from ptr thru ptr[%jd]",
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
 *
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
	err(154, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoll(str, NULL, 10);

    if (errno != 0) {
	errp(155, __func__, "error converting string \"%s\" to long int: %s", str, strerror(errno));
	not_reached();
    }
    else if (num < LONG_MIN || num > LONG_MAX) {
	err(156, __func__, "number %s out of range for long int (must be >= %ld && <= %ld)", str, LONG_MIN, LONG_MAX);
	not_reached();
    }
    return (long)num;
}


/*
 * string_to_long_long   -	convert str to long long and check for errors
 *
 * given:
 *
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
	err(157, __func__, "passed NULL arg");
	not_reached();
    }

    if (strlen(str) > LLONG_MAX_BASE10_DIGITS) {
	err(158, __func__, "string '%s' too long", str);
	not_reached();
    }

    errno = 0;
    num = strtoll(str, NULL, 10);

    if (errno != 0) {
	errp(159, __func__, "error converting string \"%s\" to long long int: %s", str, strerror(errno));
	not_reached();
    }
    else if (num <= LLONG_MIN || num >= LLONG_MAX) {
	err(160, __func__, "number %s out of range for long long int (must be > %lld && < %lld)", str, LLONG_MIN, LLONG_MAX);
	not_reached();
    }
    return num;
}


/*
 * string_to_int   -	convert str to int and check for errors
 *
 * given:
 *
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
	err(161, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = (int)strtoll(str, NULL, 10);

    if (errno != 0) {
	errp(162, __func__, "error converting string \"%s\" to int: %s", str, strerror(errno));
	not_reached();
    }
    else if (num < INT_MIN || num > INT_MAX) {
	err(163, __func__, "number %s out of range for int (must be >= %d && <= %d)", str, INT_MIN, INT_MAX);
	not_reached();
    }
    return (int)num;
}


/*
 * string_to_unsigned_long - string to unsigned long with error checks
 *
 * given:
 *
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
	err(164, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoul(str, NULL, 10);
    if (errno != 0) {
	errp(165, __func__, "strtoul(%s): %s", str, strerror(errno));
	not_reached();
    } else if (num >= ULONG_MAX) {
	err(166, __func__, "strtoul(%s): too big", str);
	not_reached();
    }

    return num;
}


/*
 * string_to_unsigned_long_long - string to unsigned long long with error checks
 *
 * given:
 *
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
	err(167, __func__, "passed NULL arg");
	not_reached();
    }

    errno = 0;
    num = strtoul(str, NULL, 10);
    if (errno != 0) {
	errp(168, __func__, "strtoul(%s): %s", str, strerror(errno));
	not_reached();
    } else if (num >= ULLONG_MAX) {
	err(169, __func__, "strtoul(%s): too big", str);
	not_reached();
    }

    return num;

}


/*
 * valid_contest_id	    -	validate string as a contest ID
 *
 * given:
 *
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
 *
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
 * is_number	    - if the string str is a number
 *
 * given:
 *
 *	str	    - string to check
 *
 * For our purposes a number is defined as: a string that starts with either a
 * '-' or '+' and beyond that no other characters but [0-9] (any count).
 *
 * returns:
 *
 *	true	==> str is a number
 *	false	==> str is not a number
 *
 * NOTE: This function does not return on error (str == NULL). An empty string
 * is considered not a number.
 */
bool
is_number(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	err(170, __func__, "passed NULL string");
	not_reached();
    }

    switch (*str) {
	case '-':
	case '+':
	    str++;
	    break;
	default:
	    break;
    }

    if (!strlen(str)) {
	return false;
    }

    return strspn(str, "0123456789") == strlen(str);
}


/*
 * string_to_bool	- convert string to a bool
 *
 * given:
 *
 *	str		- string to convert to bool
 *
 * Returns true if !strcmp(str, "true").
 *
 * This function does not return on NULL str. If strlen(str) == 0 return false.
 */
bool
string_to_bool(char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	err(171, __func__, "passed NULL string");
	not_reached();
    }

    return !strcmp(str, "true");
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
 *	first		- first ==> true, str is at beginning, perform 1st char check
 *			  false ==> false, str may be in the middle, skip 1st char check
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
	dbg(DBG_VVHIGH, "str is an empty string");
	return false;
    }

    /*
     * special case: first character is /
     */
    if (first == true) {
	if (str[0] == '/') {
		if (slash_ok == false) {
		    dbg(DBG_VVHIGH, "str[0]: slash_ok is false and 1st character is /");
		    return false;
		}

	/*
	 * special case: first character is not /, so it must be alphanumeric
	 */
	} else {
	    /* ASCII non-/ check */
	    if (!isascii(str[0])) {
		dbg(DBG_VVHIGH, "str[0]: 1st character is non-ASCII: 0x%02x", (unsigned int)str[0]);
		return false;
	    }
	    /* alphanumeric non-/ check */
	    if (!isalnum(str[0])) {
		dbg(DBG_VVHIGH, "str[0]: 1st character not alphanumeric: 0x%02x", (unsigned int)str[0]);
		return false;
	    }
	    /* special case: lower_only is true, alphanumeric lower case only */
	    if (lower_only == true && isupper(str[0])) {
		dbg(DBG_VVHIGH, "str[0]: lower_only is true and 1st character is upper case: 0x%02x",
				(unsigned int)str[0]);
		return false;
	    }
	}
	/* 1st character already checked, scan beyond 1st character next */
	start = 1;
    }

    /*
     * Beyond the 1st character, they must be POSIX portable filename or +
     */
    for (i=start; i < len; ++i) {

	/*
	 * special case: / check
	 */
	if (str[i] == '/') {
		if (slash_ok == false) {
		    dbg(DBG_VVHIGH, "slash_ok is false and / found at str[%jd]",
				    (uintmax_t)i);
		    return false;
		}

	/*
	 * case: non-/ check
	 */
	} else {
	    /* ASCII non-/ check */
	    if (!isascii(str[i])) {
		dbg(DBG_VVHIGH, "str[%jd]: character is non-ASCII: 0x%02x",
				(uintmax_t)i, (unsigned int)str[i]);
		return false;
	    }
	    /* alphanumeric nor [._+-] nor non-/ check */
	    if (!isalnum(str[i]) && str[i] != '.' && str[i] != '_' && str[i] != '+' && str[i] != '-') {
		dbg(DBG_VVHIGH, "str[%jd]: 1st character not alphanumeric nor ._+-: 0x%02x",
				(uintmax_t)i, (unsigned int)str[i]);
		return false;
	    }
	    /* special case: lower_only is true, alphanumeric lower case only */
	    if (lower_only == true && isupper(str[i])) {
		dbg(DBG_VVHIGH, "str[%jd]: lower_only is true and 1st character is upper case: 0x%02x",
				(uintmax_t)i, (unsigned int)str[i]);
		return false;
	    }
	}
    }

    /*
     * all is well
     */
    dbg(DBG_VVVHIGH, "lower_only: %s slash_ok: %s first: %s str is valid: <%s>",
		     (lower_only ? "true" : "false"),
		     (slash_ok ? "true" : "false"),
		     (first ? "true" : "false"),
		     str);
    return true;
}

/* find_matching_quote	find the next unescaped '"'
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
	err(172, __func__, "passed NULL pointer");
	not_reached();
    }

    for (++q; *q && *q != '"'; ++q)
	if (*q == '\\')
	    ++q;

    if (!*q)
	return NULL;

    return q;
}
