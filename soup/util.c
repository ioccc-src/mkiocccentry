/*
 * util - common utility functions
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
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
 * util - our functions
 */
#include "util.h"


/*
 * dbg - info, debug, warning, error, and usage message facility
 * dyn_array - dynamic array facility
 */
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"

/*
 * file_util - common utility functions for file operations
 */
#include "file_util.h"

/*
 * jparse/util - common utility functions for the JSON parser and tools
 */
#include "../jparse/util.h"


/*
 * vcmdprintf - calloc a safer shell command line for use with system() and popen() in va_list form
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
	warnp(__func__, "calloc from vcmdprintf of %zu bytes failed", size);
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
	warn(__func__, "stored characters: %zu != size: %zu", (size_t)(d + 1 - cmd), size);

	if (cmd != NULL) {
	    free(cmd);
	    cmd = NULL;
	}
	return NULL;
    }

    /*
     * return safer command line string
     */
    return cmd;
}


/*
 * cmdprintf - calloc a safer shell command line for use with system() and popen()
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
 * shell_cmd - pass a command, via vcmdprintf() interface, to the shell
 *
 * Attempt to call the shell with a command string.
 *
 * given:
 *	name		- name of the calling function
 *	flush_stdin	- true ==> stdin should be flushed as well as stdout and stderr,
 *			  false ==> only flush stdout and stderr
 *	abort_on_error	- false ==> return exit code if able to successfully call system(), or
 *			            return EXIT_CALLOC_FAILED calloc() failure,
 *			            return EXIT_FFLUSH_FAILED on fflush failure,
 *			            return EXIT_SYSTEM_FAILED if system() failed,
 *			            return EXIT_NULL_ARGS if NULL pointers were passed
 *			  true ==> return exit code if able to successfully call system(), or
 *			           call errp() (and thus exit) if unsuccessful
 *      format		- The format string, any % on this string inserts the
 *			  next string from the list, escaping special characters
 *			  that the shell might threaten as command characters.
 *			  In the worst case, the algorithm will make twice as
 *			  many characters.  Will not use escaping if it isn't needed.
 *      ...		- args to give after the format
 *
 * returns:
 *	>= ==> exit code, <0 ==> *_EXIT failure (if flag == false)
 *
 * NOTE: The values *_EXIT are < 0, and therefore do not match a valid exit code.
 *	 Moreover if abort_on_error == false, a simple check if the return was <
 *	 0 will allow the calling function to determine if this function failed.
 *
 * IMPORTANT: If flush_stdin == true, then pending stdin will be flushed.
 *	      If this process has not read all pending data on stdin, then
 *	      such pending data will be lost by the internal call to fflush(stdin).
 *	      It is the responsibility of the calling function to have read all stdin
 *	      OR accept that such pending stdin data will be lost.
 */
int
shell_cmd(char const *name, bool flush_stdin, bool abort_on_error, char const *format, ...)
{
    va_list ap;                 /* variable argument list */
    char *cmd = NULL;           /* e.g. cp prog.c submission_dir/prog.c */
    int exit_code;              /* exit code from system(cmd) */
    int saved_errno = 0;        /* before we return from the function we need to let the caller have the errno */
    char *command = NULL;
    char *path = NULL;          /* for resolve_path() */

    /*
     * firewall
     */
    if (name == NULL) {
        /* exit or error return depending on abort_on_error */
        if (abort_on_error) {
            err(150, __func__, "function name is not caller name because we were called with NULL name");
            not_reached();
        } else {
            warn(__func__, "called with NULL name, returning: %d < 0", EXIT_NULL_ARGS);
            return EXIT_NULL_ARGS;
        }
    }
    if (format == NULL) {
        /* exit or error return depending on abort_on_error */
        if (abort_on_error) {
            err(151, name, "called with NULL format");
            not_reached();
        } else {
            warn(__func__, "called with NULL format, returning: %d < 0", EXIT_NULL_ARGS);
            return EXIT_NULL_ARGS;
        }
    }

   /*
     * stdarg variable argument list setup
     */
    va_start(ap, format);

    /*
     * build a safe shell command
     */
    errno = 0;                  /* pre-clear errno for errp() */
    cmd = vcmdprintf(format, ap);
    if (cmd == NULL) {
        saved_errno = 0;
        /* exit or error return depending on abort_on_error */
        if (abort_on_error) {
            errp(152, name, "calloc failed in vcmdprintf()");
            not_reached();
        } else {
            warn(__func__, "called from %s: calloc failed in vcmdprintf(): %s, returning: %d < 0",
                         name, strerror(errno), EXIT_CALLOC_FAILED);
            va_end(ap);         /* stdarg variable argument list cleanup */
            errno = saved_errno;
            return EXIT_CALLOC_FAILED;
        }
    }

    /*
     * flush stdio as needed
     */
    flush_tty(name, flush_stdin, abort_on_error);

    /*
     * if we don't have a path we will try resolving the command
     */
    if (strchr(cmd, '/') == NULL) {
        /*
         * try resolving path
         */
        path = resolve_path(cmd);
        if (path != NULL) {
            free(cmd);
            cmd = path;
        }
    }

    /*
     * try executing the command directly
     */
    dbg(DBG_HIGH, "about to perform: system(\"%s\")", cmd);
    exit_code = system(cmd);
    /*
     * if it failed try running directly with the shell
     */
    if (exit_code < 0) {
        dbg(DBG_HIGH, "system(\"%s\") failed, will attempt to run through shell", cmd);

        errno = 0; /* pre-clear errno for errp() */
        command = calloc(1, LITLEN("/bin/sh -c ") + strlen(cmd) + 1);
        if (command == NULL) {
            errp(153, __func__, "calloc failed");
            free(cmd);
            cmd = NULL;
            return EXIT_CALLOC_FAILED;
        } else {
            sprintf(command, "/bin/sh -c %s", cmd);
            /*
             * now try executing the command via the shell
             */
            exit_code = system(command);
            free(command);
            command = NULL;
        }
    /*
     * case: exit code 127 usually means the fork/exec was unable to invoke the shell
     */
    } else if (exit_code == 127) {
        /* exit or error return depending on abort_on_error */
        if (abort_on_error) {
            errp(154, __func__, "execution of the shell failed for system(\"%s\")", cmd);
            not_reached();
        } else {
            saved_errno = errno;
            warn(__func__, "called from %s: execution of the shell failed for system(\"%s\")", name, cmd);
            va_end(ap);         /* stdarg variable argument list cleanup */
            /* free allocated command storage */
            if (cmd != NULL) {
                free(cmd);
                cmd = NULL;
            }
            errno = saved_errno;
            return EXIT_SYSTEM_FAILED;
        }
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    /*
     * free storage
     */
    free(cmd);
    cmd = NULL;

    return exit_code;
}


/*
 * pipe_open - pass a command, via vcmdprintf() interface, to the shell
 *
 * Attempt to call the shell with a command string.
 *
 * given:
 *	name		- name of the calling function
 *	write_mode	- true ==> open a pipe for writing and flush stdin
 *			  false ==> open a pipe for reading
 *	abort_on_error	- false ==> return FILE * stream for open pipe to shell, or
 *			            return NULL on failure
 *			  true ==> return FILE * stream for open pipe to shell, or
 *			           call errp() (and thus exit) if unsuccessful
 *      format		- The format string, any % on this string inserts the
 *			  next string from the list, escaping special characters
 *			  that the shell might threaten as command characters.
 *			  In the worst case, the algorithm will make twice as
 *			  many characters.  Will not use escaping if it isn't needed.
 *      ...             - args to give after the format
 *
 * returns:
 *	FILE * stream for open pipe to shell, or NULL ==> error
 *
 * IMPORTANT: If write_mode == true, then pending stdin will be flushed.
 *	      If this process has not read all pending data on stdin, then
 *	      such pending data will be lost by the internal call to fflush(stdin).
 *	      It is the responsibility of the calling function to have read all stdin
 *	      OR accept that such pending stdin data will be lost.
 */
FILE *
pipe_open(char const *name, bool write_mode, bool abort_on_error, char const *format, ...)
{
    va_list ap;			/* variable argument list */
    char *cmd = NULL;		/* e.g., cp prog.c submission_dir/prog.c */
    FILE *stream = NULL;	/* open pipe to shell command or NULL */
    int ret;			/* libc function return */
    int saved_errno = 0;        /* in case of error, save errno for before we return */
    char *path = NULL;          /* for resolve_path() */

    /*
     * firewall
     */
    if (name == NULL) {
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    err(155, __func__, "function name is not caller name because we were called with NULL name");
	    not_reached();
	} else {
	    warn(__func__, "called with NULL name, returning NULL");
	    return NULL;
	}
    }
    if (format == NULL) {
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    err(156, name, "called with NULL format");
	    not_reached();
	} else {
	    warn(__func__, "called with NULL format, returning NULL");
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
	    errp(157, name, "calloc failed in vcmdprintf()");
	    not_reached();
	} else {
	    warn(__func__, "called from %s: calloc failed in vcmdprintf(): %s returning: %d < 0",
			 name, strerror(errno), EXIT_CALLOC_FAILED);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return NULL;
	}
    }

    /*
     * if it has no path resolve it
     */
    if (strchr(cmd, '/') == NULL) {
        path = resolve_path(cmd);
        if (path != NULL) {
            free(cmd);
            cmd = path;
        }
    }

    /*
     * flush stdio as needed
     *
     * If we are in write_mode to a pipe, we also flush stdin in order to
     * avoid duplicate reads (or worse) of buffered stdin data.
     */
    flush_tty(name, write_mode, abort_on_error);

    /*
     * establish the open pipe to the shell command
     */
    dbg(DBG_HIGH, "about to perform: popen(\"%s\", \"%s\")", cmd, write_mode ? "w" : "r");
    errno = 0;            /* pre-clear errno for errp() */
    stream = popen(cmd, write_mode ? "w" : "r");
    if (stream == NULL) {
        if (abort_on_error) {
            errp(158, name, "error calling popen(\"%s\", \"%s\")", cmd, write_mode ? "w" : "r");
            not_reached();
        } else {
            saved_errno = errno;
            warn(__func__, "called from %s: error calling popen(\"%s\", \"%s\"): %s", name, cmd, write_mode ? "w" : "r",
                strerror(errno));
            va_end(ap);        /* stdarg variable argument list cleanup */
            if (cmd != NULL) {
                free(cmd);
                cmd = NULL;
            }
            errno = saved_errno;
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
 * str_dup - return a duplicate of a string
 *
 * given:
 *	str	the string to duplicate
 *
 * return:
 *	a malloced copy of str
 *
 * NOTE: If str == NULL, then a calloc this function will attempt to return a calloc a zero length string.
 *	 That is, calloc will attempt to allocated a single NUL byte.
 *	 A debug message at DBG_HIGH is issued when this happens.
 *
 * NOTE: This function attempts to return a calloc a zero length string if strdup(3) fails.
 *	 That is, calloc will attempt to allocated a single NUL byte.
 *	 A debug message at DBG_HIGH is issued when this happens.
 *
 * NOTE: In the unlikely event of the calloc of a zero length string failed, this function will NOT return.
 *       We exit because if the calloc of a single NUL byte fails, we can only conclude that some
 *	 extremely serious error has happened: exiting is perhaps the best option in such a case.
 *	 Therefore, it should be EXTREMELY unlikely that this function will NOT return.
 *
 * NOTE: This function will NEVER return NULL.
 */
char *
str_dup(char const *str)
{
    char *ret;			/* malloced string to return */

    /*
     * firewall
     */
    if (str == NULL) {
	dbg(DBG_HIGH, "%s: str is NULL, will will attempt return a calloc a single NUL byte", __func__);
	ret = calloc(1, 1);
	if (ret == NULL) {
	    /* likely a serious memory correction as happened: we exit */
	    errp(159, __func__, "calloc #0 of a zero length string failed!");
	    not_reached();
	}
	return ret;
    }

    /*
     * duplicate string
     */
    errno = 0; /* pre-clear errno for errp() */
    ret = strdup(str);
    if (ret == NULL) {
	dbg(DBG_HIGH, "%s: strdup(\"%s\") failed, will attempt return a calloc a single NUL byte", __func__, str);
	ret = calloc(1, 1);
	if (ret == NULL) {
	    /* likely a serious memory correction as happened: we exit */
	    errp(160, __func__, "calloc #1 of a zero length string failed!");
	    not_reached();
	}
	return ret;
    }
    return ret;
}
