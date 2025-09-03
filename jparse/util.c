/*
 * util - common utility functions for the JSON parser and tools
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
#include <poll.h>
#include <fcntl.h>		/* for open() */

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
# include "../dbg/dbg.h"
# include "../dyn_array/dyn_array.h"
#elif defined(INTERNAL_INCLUDE_2)
#include "../dbg/dbg.h"
#include "../dyn_array/dyn_array.h"
#else
#include <dbg.h>
#include <dyn_array.h>
#endif

/*
 * util - common utility functions for the JSON parser
 */
#include "util.h"


/*
 * static declarations
 */
static bool exists(char const *path);
static bool is_file(char const *path);
static bool is_dir(char const *path);
static bool is_exec(char const *path);
static bool is_read(char const *path);


/* NOTE: The following function is a static duplicate from from mkiocccentry toolkit */
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
 *
 * This function does not return on NULL pointer.
 */
static bool
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
    dbg(DBG_VHIGH, "path %s size: %jd", path, (intmax_t)buf.st_size);
    return true;
}


/* NOTE: The following function is a static duplicate from from mkiocccentry toolkit */
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
static bool
is_file(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(69, __func__, "called with NULL path");
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


/* NOTE: The following function is a static duplicate from from mkiocccentry toolkit */
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
static bool
is_dir(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(70, __func__, "called with NULL path");
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


/* NOTE: The following function is a static duplicate from from mkiocccentry toolkit */
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
static bool
is_exec(char const *path)
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


/* NOTE: The following function is a static duplicate from from mkiocccentry toolkit */
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
static bool
is_read(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(72, __func__, "called with NULL path");
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
 * fd_is_ready - test if a file descriptor is ready for I/O
 *
 * Perform a non-blocking test to determine of a given file descriptor is ready
 * for I/O (reading or writing).  If the file descriptor is NOT open, return false.
 *
 * The open_test_only flag effects if the test includes a test of the we can read or write
 * the file descriptor.  When open_test_only is true, we only return true if the file descriptor
 * is valid and open and not in an error state.  When open_test_only is is false, we also test
 * if the file descriptor has data ready to be read or can be written into.
 *
 * given:
 *	name		name of the calling function
 *	open_test_only	true ==>  do not test if we can read or write, only test if open & valid
 *			false ==> unless descriptor is ready to read or write.
 *	fd		file descriptor on which to perform an I/O test
 *
 * returns:
 *	true	if open_test_only == true:  file descriptor is open and not in an error state
 *		if open_test_only == false: file descriptor is open, not in an error state, and
 *					    has data ready to be read or can be written into
 *	false	if open_test_only == true:  file descriptor is closed or in an error state, or
 *					    file descriptor < 0
 *		if open_test_only == false: file descriptor is closed or in an error state, or
 *					    file descriptor is open but lacks data to read, or
 *					    file descriptor is open cannot be written info, or
 *					    file descriptor < 0
 */
bool
fd_is_ready(char const *name, bool open_test_only, int fd)
{
    struct pollfd fds;		/* poll selector */
    int ret;			/* return code holder */

    /*
     * firewall - fd < 0 returns false
     */
    if (fd < 0) {
	dbg(DBG_VVHIGH, "%s: called via %s: fd: %d < 0, returning false", __func__, name, fd);
	return false;
    } else if (isatty(fd)) {
	dbg(DBG_VVHIGH, "%s: called via %s: fd: isatty(%d) == 1, returning true", __func__, name, fd);
	return true;
    }

    /*
     * setup the poll selector for the file descriptor
     *
     * We look for all known poll events we are allowed to mark.  Some events are output only,
     * such as POLLERR, POLLHUP and POLLNVAL (are ignored in events) so we do not set them.
     */
    fds.fd = fd;
    fds.events = 0;
    if (open_test_only == false){
	fds.events |= POLLIN | POLLOUT | POLLPRI;
    }
#if defined(_GNU_SOURCE)
    fds.events |= POLLRDHUP;
#endif /* _GNU_SOURCE */
#if defined(_XOPEN_SOURCE)
    if (open_test_only == false) {
	fds.events |= POLLRDNORM | POLLRDBAND | POLLWRNORM | POLLWRBAND;
    }
#endif /* _XOPEN_SOURCE */
    dbg(DBG_VVHIGH, "%s: called via %s: poll for %d: events set to: %d", __func__, name, fd, fds.events);
    fds.revents = 0;

    /*
     * poll the file descriptor without blocking
     */
    errno = 0;			/* pre-clear errno for dbg() */
    ret = poll(&fds, 1, 0);
    if (ret < 0) {
	warn(__func__, "called via %s: poll on %d failed: %s, returning false", name, fd, strerror(errno));
	return false;

    /*
     * case: file descriptor has an event to check
     */
    } else if (ret == 1) {

	/*
	 * case: POLLNVAL
	 *
	 * The fd is not open.
	 */
	if (fds.revents & POLLNVAL) {
	    dbg(DBG_VVHIGH, "%s: called via %s: poll on %d found POLLNVAL revents: %x, returning false",
			    __func__, name, fd, fds.revents);
	    return false;

	/*
	 * case: POLLHUP
	 *
	 * Note that when reading from a channel such as a pipe or a
         * stream socket, this event merely indicates that the peer
         * closed its end of the channel.  Subsequent reads from the
         * channel will return 0 (end of file) only after all
         * outstanding data in the channel has been consumed.
	 */
	} else if (fds.revents & POLLHUP) {
	    dbg(DBG_VVHIGH, "%s: called via %s: poll on %d found POLLHUP revents: %x, returning false",
			    __func__, name, fd, fds.revents);
	    return false;

	/*
	 * case: POLLERR
	 *
	 * This bit is also set for a file descriptor referring to the write end of a pipe
	 * when the read end has been closed.
	 */
	} else if (fds.revents & POLLERR) {
	    dbg(DBG_VVHIGH, "%s: called via %s: poll on %d found POLLERR revents: %x, returning false",
			    __func__, name, fd, fds.revents);
	    return false;

#if defined(_GNU_SOURCE)
	/*
	 * case: POLLRDHUP
	 *
	 * Stream socket peer closed connection, or shut down writing half of connection.
	 */
	} else if (fds.revents & POLLRDHUP) {
	    dbg(DBG_VVHIGH, "%s: called via %s: poll on %d found POLLRDHUP revents: %x, returning false",
			    __func__, name, fd, fds.revents);
	    return false;
#endif /* _GNU_SOURCE */
	}

	/*
	 * cases: all other cases the file descriptor is considered ready
	 *
	 * These cases include:
	 *
	 * POLLIN - There is data to read.
	 * POLLPRI - some exceptional condition on the file descriptor such as:
	 *		out-of-band data on a TCP socket
	 *		pseudoterminal master in packet mode has seen a state change on the slave
	 *		cgroup.events file has been modified
	 * POLLOUT - Writing is now possible.
	 *
	 * And if defined(_XOPEN_SOURCE):
	 *
	 * POLLRDNORM - Equivalent to POLLIN.
	 * POLLRDBAND - Priority band data can be read.
	 * POLLWRNORM - Equivalent to POLLOUT.
	 * POLLWRBAND - Priority data may be written.
	 */
	dbg(DBG_VVHIGH, "%s: called via %s: poll on %d revents: %x such that we return true",
			__func__, name, fd, fds.revents);
	return true;

    /*
     * case: file descriptor has no events, return false
     *
     * When descriptors are have events, we assume that the file descriptor is valid (open non-error).
     *
     * If open_test_only is true: because we ignore events related to being able to read or write,
     *				  we can safely assume file descriptor is not suitable for I/O actions.
     * If open_test_only is false: we assume file descriptor cannot also be read from or written into.
     *
     * In either case we declare the file descriptors not ready.
     */
    } else if (ret == 0) {
	dbg(DBG_VVHIGH, "%s: called via %s: poll on %d found no events, returning false",
			__func__, name, fd);
	return false;
    }

    /*
     * unexpected poll return, return false
     */
    warn(__func__, "called via %s: poll on %d returned %d: expected 0 or 1, returning false", name, fd, ret);
    return false;
}


/*
 * chk_stdio_printf_err - check for a print function call errors
 *
 * Here "print function call" refers to functions such as:
 *
 *	printf(3), fprintf(3), dprintf(3),
 *	vprintf(3), vfprintf(3), vdprintf(3)
 *
 * On some systems, such as macOS, the first stdio print function call
 * that prints to a given stream first checks if the stream is associated
 * with a TTY (presumably so that it might perform certain TTY related
 * ioctl(2) actions on the stream).  The TTY check is performed by isatty(3).
 *
 * If the stream is NOT a TTY, then isatty(3) will fail.  Now on some systems,
 * such as macOS, the first stdio print function call that performs such an
 * isatty(3) will leave errno with the value ENOTTY, even though the actual
 * print function did not fail.
 *
 * The FILE structure, presumably, contains a boolean to indicate if
 * the stream is a TTY or not.  Thus on the second and later print function
 * calls to the stream, there is no need to call isatty(3).
 *
 * Consider what happens when a stream such as stdout or stderr becomes
 * associated with "/dev/null" which is NOT a TTY.  The first print function call
 * will call isatty(3), which will fail because "/dev/null" is not a TTY.
 * On systems such as macOS, the errno value will be left with ENOTTY
 * even though the stdio print function call did not fail.
 *
 * Because of this, we have to treat the case where the print function
 * returned a value > 0 and errno == ENOTTY and the stream is not a TTY.
 * We do NOT consider this situation to be a stdio print functions failure.
 *
 * given:
 *	stream		open file stream being used
 *	ret		return code from the stdio print function
 *
 * returns:
 *	true ==> the stdio print function failed
 *	false ==> the stdio print function did not fail
 */
bool
chk_stdio_printf_err(FILE *stream, int ret)
{
    int saved_errno = errno;	/* saved errno */

    /*
     * firewall
     */
    if (stream == NULL) {
	return true;	/* NULL stream is an error */
    }

    /*
     * case: no stdio error
     */
    if (ret > 0 && errno == 0) {
	return false;	/* not an stdio print function error */
    }

    /*
     * case: errno != 0 && errno != ENOTTY
     */
    if (errno != 0 && errno != ENOTTY) {
	return true;	/* errno != 0 and errno != ENOTTY is a print function error */
    }
    /* errno == ENOTTY or errno == 0 below here */

    /*
     * case: ret <= 0 && errno == 0
     */
    if (ret <= 0 && errno == 0) {
	return true;	/* ret <= 0 and errno = 0 is a print function error */
    }
    /* errno == ENOTTY below here */

    /*
     * case: stream is a tty
     */
    if (isatty(fileno(stream))) {

	/*
	 * restore errno in case isatty() changed it
	 */
	errno = saved_errno;
	return true;	/* errno == ENOTTY and stream is a TTY is a print function error */
    }
    /* errno == ENOTTY and not a TTY below here */

    /*
     * restore errno in case isatty() changed it
     */
    errno = saved_errno;

    /*
     * case: errno == ENOTTY and stream is not a TTY and ret <= 0
     */
    if (ret <= 0) {
	return true;	/* errno == ENOTTY and stream is a TTY and ret <= 0 is a print function error */
    }
    /* errno == ENOTTY and ret > 0 and not a TTY below here */

    /*
     * case: errno == ENOTTY and ret > 0 and stream is NOT a TTY
     */
    return false;	/* errno == ENOTTY and stream and ret > 0 is NOT a TTY is NOT a print function error */
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
	err(73, __func__, "stdout is NULL");
	not_reached();
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    /*
     * this may not always catch a bogus or unopened stdout, but try anyway
     */
    fd = fileno(stdout);
    if (fd < 0) {
	errp(74, __func__, "fileno on stdout returned: %d < 0", fd);
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
		errp(75, __func__, "error writing paragraph to a stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(76, __func__, "EOF while writing paragraph to a stdout");
		not_reached();
	    } else {
		errp(77, __func__, "unexpected fputs error writing paragraph to stdout");
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
		errp(78, __func__, "error writing newline to stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(79, __func__, "EOF while writing newline to stdout");
		not_reached();
	    } else {
		errp(80, __func__, "unexpected fputc error writing newline to stdout");
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
	    errp(81, __func__, "error flushing stdout");
	    not_reached();
	} else if (feof(stdout)) {
	    err(82, __func__, "EOF while flushing stdout");
	    not_reached();
	} else {
	    errp(83, __func__, "unexpected fflush error while flushing stdout");
	    not_reached();
	}
    }
    dbg(DBG_VVHIGH, "%s() printed %d line%s", __func__, line_count, line_count==1?"":"s");
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
	err(84, __func__, "stream is NULL");
	not_reached();
    }

    /*
     * this may not always catch a bogus or unopened stream, but try anyway
     */
    clearerr(stream);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    fd = fileno(stream);
    if (fd < 0) {
	errp(85, __func__, "fileno on stream returned: %d < 0", fd);
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
		errp(86, __func__, "error writing paragraph to stream");
		not_reached();
	    } else if (feof(stream)) {
		err(87, __func__, "EOF while writing paragraph to stream");
		not_reached();
	    } else {
		errp(88, __func__, "unexpected fputs error writing paragraph to stream");
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
		errp(89, __func__, "error writing newline to stream");
		not_reached();
	    } else if (feof(stream)) {
		err(90, __func__, "EOF while writing newline to stream");
		not_reached();
	    } else {
		errp(91, __func__, "unexpected fputc error writing newline to stream");
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
	    errp(92, __func__, "error flushing stream");
	    not_reached();
	} else if (feof(stream)) {
	    err(93, __func__, "EOF while flushing stream");
	    not_reached();
	} else {
	    errp(94, __func__, "unexpected fflush error while flushing stream");
	    not_reached();
	}
    }

    dbg(DBG_VVHIGH, "%s() printed %d line%s", __func__, line_count, line_count==1?"":"s");
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
    if (chk_stdio_printf_err(stream, fret)) {
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
 *                NULL ==> getline() will calloc() the linep buffer
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
	err(95, __func__, "called with NULL arg(s)");
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
	    dbg(DBG_VVHIGH, "EOF detected in getline");
	    return -1; /* EOF found */
	} else if (ferror(stream)) {
	    errp(96, __func__, "getline() error");
	    not_reached();
	} else {
	    errp(97, __func__, "unexpected getline() error");
	    not_reached();
	}
    }
    /*
     * paranoia
     */
    if (*linep == NULL) {
	err(98, __func__, "*linep is NULL after getline()");
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
 * readline_dup - read a line from a stream and duplicate to an allocated buffer.
 *
 * given:
 *      linep   - allocated line buffer (may be realloced) or ptr to NULL
 *                NULL ==> getline() will calloc() the linep buffer
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
	err(99, __func__, "called with NULL arg(s)");
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
    ret = calloc((size_t)len+1+1, sizeof(*ret));
    if (ret == NULL) {
	errp(100, __func__, "calloc of read line of %jd bytes failed", (intmax_t)len+1+1);
	not_reached();
    }
    memcpy(ret, *linep, (size_t)len);

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
	dbg(DBG_VVHIGH, "readline, after trailing whitespace stripped, is %jd bytes", (intmax_t)len);
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
 * read_all - read all data from an open file
 *
 * given:
 *	stream	    - an open file stream to read from
 *	psize	    - if psize != NULL, *psize is the amount of data read
 *
 * returns:
 *	calloc buffer containing the entire contents of stream,
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
 * If no data is read, the calloc buffer will still be NUL terminated.
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
 *	}
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
	err(101, __func__, "called with NULL stream");
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
		moved = dyn_array_seek(array, (off_t)last_read-READ_ALL_CHUNK, SEEK_CUR);
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
	*psize = (size_t)used;
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
 * string_to_intmax - convert base 10 str to intmax_t and check for errors
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
string_to_intmax(char const *str, intmax_t *ret)
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
	warn(__func__, "string \"%s\" has no digits", str);
	return false;
    } else if (*endptr != '\0') {
	warn(__func__, "number \"%s\" has invalid characters", str);
	return false;
    } else if (saved_errno != 0) {
	errno = saved_errno;
	warnp(__func__, "error converting string \"%s\" to intmax_t", str);
	return false;
    } else if (num <= INTMAX_MIN || num >= INTMAX_MAX) {
	warn(__func__, "number %s out of range for intmax_t (must be > %jd && < %jd)", str, INTMAX_MIN, INTMAX_MAX);
	return false;
    }

    /*
     * store conversion and report success
     */
    if (ret != NULL) {
	*ret = num;
    }

    return true;
}


/*
 * string_to_uintmax - convert base 10 str to uintmax_t and check for errors
 *
 * given:
 *	str	- the string to convert to an uintmax_t
 *	*ret	- pointer to converted uintmax_t if return is true
 *
 * returns:
 *	true ==> conversion into *ret was successful,
 *	false ==> unable to convert / not a valid base 10 integer, NULL pointer or internal error
 */
bool
string_to_uintmax(char const *str, uintmax_t *ret)
{
    uintmax_t num = 0;
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
    num = strtoumax(str, &endptr, 10);
    saved_errno = errno;
    if (endptr == str) {
	warn(__func__, "string \"%s\" has no digits", str);
	return false;
    } else if (*endptr != '\0') {
	warn(__func__, "number \"%s\" has invalid characters", str);
	return false;
    } else if (saved_errno != 0) {
	errno = saved_errno;
	warnp(__func__, "error converting string \"%s\" to uintmax_t", str);
	return false;
    } else if (num <= 0 || num >= UINTMAX_MAX) {
	warn(__func__, "number %s out of range for uintmax_t (must be >= %jd && < %jd)", str, (uintmax_t)0, UINTMAX_MAX);
	return false;
    }

    /*
     * store conversion and report success
     */
    if (ret != NULL) {
	*ret = num;
    }

    return true;
}


/*
 * is_decimal - if the buffer is a base 10 integer in ASCII
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
 * is_decimal_str - if the string str is a base 10 integer in ASCII
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
 *	 what is and is not considered an integer.
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
 * is_floating_notation - if the buffer is a base 10 floating point notation in ASCII
 *
 * given:
 *	ptr	    - pointer to buffer containing an integer in ASCII
 *	len	    - length, starting at ptr
 *
 * A floating point notation is like that of C but as the only use of this
 * function is the JSON parser it might be a bit different. The JSON parser
 * conversion routine, which only will be called on floating point or
 * e-notations, has more checks that are JSON specific.
 *
 * returns:
 *
 *	true	==> ptr points to a base 10 floating point notation in ASCII
 *	false	==> ptr does NOT point to a base 10 floating point notation in ASCII, or if ptr is NULL, or len <= 0
 */
bool
is_floating_notation(char const *str, size_t len)
{
    size_t str_len = 0;	/* length of string */
    char *dot_found = NULL; /* if dot found we have to check if more than one is found */
    char *dot = NULL;	    /* to check if second dot is the same as first */


    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "passed NULL str");
	return false;
    }
    if (len <= 0) {
	warn(__func__, "len <= 0: %ju", (intmax_t)len);
	return false;
    }
    if (str[0] == '\0') {
	warn(__func__, "called with empty string");
	return false;	/* processing failed */
    }

    if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	warn(__func__, "str[%ju-1] is not an ASCII digit: 0x%02x for str: %s", (uintmax_t)len, (int)str[len-1], str);
	return false;	/* processing failed */
    }

    str_len = strlen(str);
    if (str_len < len) {
	warn(__func__, "strlen(\"%s\"): %ju < len arg: %ju", str, (uintmax_t)str_len, (uintmax_t)len);
	return false;	/* processing failed */
    /*
     * JSON spec detail: floating point numbers cannot end with .
     */
    } else if (str[len-1] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with .: \"%s\"",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with -
     */
    } else if (str[len-1] == '-') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with -: \"%s\"",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with +
     */
    } else if (str[len-1] == '+') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with +: \"%s\"",
		      __func__, str);
	return false;	/* processing failed */

    }

    /* detect dot */
    dot_found = strchr(str, '.');
    /* if dot found see if there's another one */
    if (dot_found != NULL) {
	dot = strrchr(str, '.');
	if (dot != NULL && dot != dot_found) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have two '.'s: \"%s\"",
		      __func__, str);
	    return false;	/* processing failed */
	}

        return true;
    }


    return false;
}


/*
 * is_floating_notation_str - if the string buffer str is a base 10 floating point notation in ASCII
 *
 * This is a simplified interface for is_floating_notation().
 *
 * given:
 *	str	    - pointer to buffer containing an integer in ASCII
 *	retlen	    - address of where to store length of str, if retlen != NULL
 *
 * returns:
 *
 *	true	==> str points to a base 10 floating point notation in ASCII
 *	false	==> str does NOT point to a base 10 floating point notation in
 *		    ASCII, str is NULL or str is empty
 *
 * NOTE: This function calls is_floating_notation().  See that function for
 * details on what is and is not considered floating point notation.
 */
bool
is_floating_notation_str(char const *str, size_t *retlen)
{
    size_t len = 0;		/* length of string to test */
    bool ret = false;		/* if str points to a base 10 floating point notation in ASCII */

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
     * convert to is_floating_notation() call
     */
    ret = is_floating_notation(str, len);

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return test of ASCII base floating point notation digits
     */
    return ret;
}


/*
 * is_e_notation - if the buffer is a base 10 exponent floating point notation in ASCII
 *
 * given:
 *	ptr	    - pointer to buffer containing an integer in ASCII
 *	len	    - length, starting at ptr
 *
 * e-notation is like that of C but as the only use of this function is the JSON
 * parser it might be a bit different. The JSON parser conversion routine, which
 * only will be called on floating point or e-notations, has more checks that
 * are JSON specific.
 *
 * returns:
 *
 *	true	==> ptr points to a base 10 exponent notation in ASCII
 *	false	==> ptr does NOT point to a base 10 exponent notation in ASCII, or if ptr is NULL, or len <= 0
 */
bool
is_e_notation(char const *str, size_t len)
{
    size_t str_len = 0;	/* length of string */
    char *e_found = NULL;   /* for e notation: lower case e */
    char *cap_e_found = NULL;	/* for e notation: upper case E */
    char *e = NULL;	/* check if more than one e or E */
    bool is_e_notation = false;	/* if e or E notation valid */
    char *dot_found = NULL; /* if dot found we have to check if more than one is found */
    char *dot = NULL;	    /* to check if second dot is the same as first */

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "passed NULL str");
	return false;
    }
    if (len <= 0) {
	warn(__func__, "len <= 0: %ju", (intmax_t)len);
	return false;
    }
    if (str[0] == '\0') {
	warn(__func__, "called with empty string");
	return false;	/* processing failed */
    }

    if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	warn(__func__, "str[%ju-1] is not an ASCII digit: 0x%02x for str: %s", (uintmax_t)len, (int)str[len-1], str);
	return false;	/* processing failed */
    }

    str_len = strlen(str);
    if (str_len < len) {
	warn(__func__, "strlen(\"%s\"): %ju < len arg: %ju", str, (uintmax_t)str_len, (uintmax_t)len);
	return false;	/* processing failed */
    }

    /*
     * JSON spec detail: floating point numbers cannot start with .
     */
    if (str[0] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot start with .: \"%s\"",
		       __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with .
     */
    } else if (str[len-1] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with .: \"%s\"",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with -
     */
    } else if (str[len-1] == '-') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with -: \"%s\"",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with +
     */
    } else if (str[len-1] == '+') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with +: \"%s\"",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers must end in a digit
     */
    } else if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	dbg(DBG_HIGH, "in %s(): floating point numbers must end in a digit: \"%s\"",
		       __func__, str);
	return false;	/* processing failed */
    }

    /* detect dot */
    dot_found = strchr(str, '.');
    /* if dot found see if there's another one */
    if (dot_found != NULL) {
	dot = strrchr(str, '.');
	if (dot != NULL && dot != dot_found) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have two '.'s: \"%s\"",
		      __func__, str);
	    return false;	/* processing failed */
	}
    }

    /*
     * detect use of e notation
     */
    e_found = strchr(str, 'e');
    cap_e_found = strchr(str, 'E');
    /* case: both e and E found */
    if (e_found != NULL && cap_e_found != NULL) {

	dbg(DBG_HIGH, "in %s(): floating point numbers cannot use both e and E: \"%s\"",
		      __func__, str);
	return false;	/* processing failed */

    /* case: neither 'e' nor 'E' found */
    } else if (e_found == NULL && cap_e_found == NULL) {
	/* NOTE: don't warn as it could be a floating point without e notation */
	dbg(DBG_HIGH, "in %s(): not e notation: neither 'e' nor 'E' found: \"%s\"",
		      __func__, str);
	return false;
    /* case: just e found, no E */
    } else if (e_found != NULL) {

	/* firewall - search for two 'e's */
	e = strrchr(str, 'e');
	if (e_found != e) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have more than one e: \"%s\"",
			  __func__, str);
	    return false;	/* processing failed */
	}

	/* note e notation */
	is_e_notation = true;

    /* case: just E found, no e */
    } else if (cap_e_found != NULL) {

	/* firewall - search for two 'E's */
	e = strrchr(str, 'E');
	if (cap_e_found != e) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have more than one E: \"%s\"",
			  __func__, str);
	    return false;	/* processing failed */
	}

	/* note e notation */
	is_e_notation = true;
    }


    /*
     * perform additional JSON number checks on e notation
     *
     * NOTE: If item->is_e_notation == true, then e points to the e or E
     */
    if (is_e_notation) {

	/*
	 * JSON spec detail: e notation number cannot start with e or E
	 */
	if (e == str) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot start with e or E: \"%s\"",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot end with e or E
	 */
	} else if (e == &(str[len-1])) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot end with e or E: \"%s\"",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot have e or E after .
	 */
	} else if (e > str && e[-1] == '.') {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot have '.' before e or E: \"%s\"",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number must have digit before e or E
	 */
	} else if (e > str && (!isascii(e[-1]) || !isdigit(e[-1]))) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers must have digit before e or E: \"%s\"",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * case: e notation number with a leading + in the exponent
	 *
	 * NOTE: From "floating point numbers cannot end with +" we know + is not at the end.
	 */
	} else if (e[1] == '+') {

	    /*
	     * JSON spec detail: e notation number with e+ or E+ must be followed by a digit
	     */
	    if (e+1 < &(str[len-1]) && (!isascii(e[2]) || !isdigit(e[2]))) {
		dbg(DBG_HIGH, "in %s(): :e notation number with e+ or E+ must be followed by a digit \"%s\"",
			      __func__, str);
		return false;	/* processing failed */
	    }

	/*
	 * case: e notation number with a leading - in the exponent
	 *
	 * NOTE: From "floating point numbers cannot end with -" we know - is not at the end.
	 */
	} else if (e[1] == '-') {

	    /*
	     * JSON spec detail: e notation number with e- or E- must be followed by a digit
	     */
	    if (e+1 < &(str[len-1]) && (!isascii(e[2]) || !isdigit(e[2]))) {
		dbg(DBG_HIGH, "in %s(): :e notation number with e- or E- must be followed by a digit \"%s\"",
			      __func__, str);
		return false;	/* processing failed */
	    }

	/*
	 * JSON spec detail: e notation number must have + or - or digit after e or E
	 */
	} else if (!isascii(e[1]) || !isdigit(e[1])) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers must follow e or E with + or - or digit: \"%s\"",
			  __func__, str);
	    return false;	/* processing failed */
	}
    }


    return true;
}


/*
 * is_e_notation_str - if the string buffer str is a base 10 exponent floating point notation in ASCII
 *
 * This is a simplified interface for is_e_notation().
 *
 * given:
 *	str	    - pointer to buffer containing an integer in ASCII
 *	retlen	    - address of where to store length of str, if retlen != NULL
 *
 * returns:
 *
 *	true	==> str points to a base 10 exponent floating point notation in ASCII
 *	false	==> str does NOT point to a base 10 exponent floating point
 *		    notation in ASCII, str is NULL or str is empty
 *
 * NOTE: This function calls is_e_notation().  See that function for details on
 *	 what is and is not considered exponent notation.
 */
bool
is_e_notation_str(char const *str, size_t *retlen)
{
    size_t len = 0;		/* length of string to test */
    bool ret = false;		/* if str points to a base 10 exponent notation in ASCII */

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
     * convert to is_e_notation() call
     */
    ret = is_e_notation(str, len);

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return test of ASCII base exponent notation digits
     */
    return ret;
}


/*
 * posix_plus_safe - if string is a valid POSIX portable safe and + chars
 *
 * If slash_ok is true:
 *
 *	If first is true:
 *
 *	    If lower_only is true, then the string must match:
 *
 *		^[/0-9a-z][0-9a-z._+-]*$
 *
 *	    If lower_only is false, then the string must match:
 *
 *		^[/0-9A-Za-z][0-9A-Za-z._+-]*$
 *
 *	If first is false:
 *
 *	    If lower_only is true, then the string must match:
 *
 *		^[/0-9a-z._+-]*$
 *
 *	    If lower_only is false, then the string must match:
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
 *	true ==> str is a valid POSIX portable safe + path name, AND
 *		 the case of str matches lower_only and slash_ok conditions
 *	false ==> an unsafe issue was found, or str is empty, or str is NULL
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
		    dbg(DBG_VVHIGH, "str[0]: slash_ok is false and first character is '/'");
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
     * Beyond the first character, they must be POSIX portable path name or +
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
    dbg(DBG_VVVHIGH, "lower_only: %s slash_ok: %s first: %s str is valid: \"%s\"",
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
	err(102, __func__, "called with NULL arg(s)");
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
	dbg(DBG_VVHIGH, "posix_safe_chk(\"%s\", false, false, false, false) returning: str is an empty string", str);
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
		dbg(DBG_VVVHIGH, "posix_safe_chk(): str[0] is UPPER CASE: 0x%02x", (unsigned int)str[0]);
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
     * examine second character through the last character
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
		    dbg(DBG_VVVHIGH, "posix_safe_chk(): found first UPPER CASE at str[%ju]: 0x%02x",
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
	dbg(DBG_VVHIGH, "posix_safe_chk(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\"): string is NOT POSIX portable safe plus +/",
		str, booltostr(slash), booltostr(posix_safe), booltostr(first_alphanum), booltostr(upper));

    /*
     * report POSIX portable safe plus + safe with maybe /
     */
    } else {
	dbg(DBG_VVHIGH, "posix_safe_chk(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\"): string is POSIX portable safe plus +/: \"%s\"",
		str, booltostr(slash), booltostr(posix_safe), booltostr(first_alphanum), booltostr(upper), str);
    }
    return;
}


/*
 * clearerr_or_fclose - clear FILE stream if stdin, stdout, or stderr OR close the stream
 *
 * If stream is NULL, this function does nothing.
 *
 * This function calls clearerr() if stream is stdin, or stdout, or stderr.
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
 *
 * NOTE: this function will NOT print unicode symbols. To do this a new flag
 * to not print the \x99 but just the character. The purpose of this function is
 * not to print decoded/encoded data but rather the raw data in the buffer.
 */
ssize_t
fprint_line_buf(FILE *stream, const void *buf, size_t len, int start, int end)
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
	c = (int)(((const uint8_t *)buf)[i]);

	/*
	 * case: character is non-NUL start or non-NUL end or non-ASCII character
	 */
	if ((start != 0 && c == start) || (end != 0 && c == end) || !isascii(c)) {

	    /* print character as \x99 if non-NULL stream to avoid start/end confusion */
	    if (stream != NULL) {
		errno = 0;  /* clear errno */
		ret = fprintf(stream, "\\x%02x", c);
		if (chk_stdio_printf_err(stream, ret) || ret != 4) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		    if (chk_stdio_printf_err(stream, ret) || ret != 2) {
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
		if (isascii(c) && isprint(c)) {

		    /* print character if non-NULL stream */
		    if (stream != NULL) {
			errno = 0;	/* clear errno */
			ret = fputc(c, stream);
			if (chk_stdio_printf_err(stream, ret) || ret == EOF) {
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
			if (chk_stdio_printf_err(stream, ret) || ret != 4) {
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
     * if we had a print error, report the last errno that was observed
     */
    if (delayed_errno != 0) {
	warn(__func__, "last write errno: %d (\"%s\")", delayed_errno, strerror(delayed_errno));
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
    return (ssize_t)count;
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
    ssize_t count = 0;		/* number of characters in line */
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
 * open_dir_file - open a readable file in a given directory
 *
 * Temporarily chdir to the directory, if non-NULL, try to open the file,
 * and then chdir back to the current directory.
 *
 * If dir == NULL, just try to open the file without a chdir.
 *
 * given:
 *	dir	directory into which we will temporarily chdir or
 *		    NULL ==> do not chdir
 *	file	path of readable file to open
 *
 * returns:
 *	open readable file stream
 *
 * NOTE: This function does not return if path is NULL,
 *	 if we cannot chdir to a non-NULL dir, if not a readable file,
 *	 or if unable to open file.
 *
 * NOTE: This function will NOT return NULL.
 */
FILE *
open_dir_file(char const *dir, char const *file)
{
    FILE *ret_stream = NULL;		/* open file stream to return */
    int ret = 0;		/* libc function return */
    int cwd = -1;		/* current working directory */

    /*
     * firewall
     */
    if (file == NULL) {
	err(103, __func__, "called with NULL file");
	not_reached();
    }

    /*
     * note the current directory so we can restore it later, after the
     * chdir(dir) below
     */
    errno = 0;                  /* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
        errp(104, __func__, "cannot open .");
        not_reached();
    }

    /*
     * Temporarily chdir if dir is non-NULL
     */
    if (dir != NULL && cwd >= 0) {

	/*
	 * check if we can search / work within the directory
	 */
	if (!exists(dir)) {
	    err(105, __func__, "directory does not exist: %s", dir);
	    not_reached();
	}
	if (!is_dir(dir)) {
	    err(106, __func__, "is not a directory: %s", dir);
	    not_reached();
	}
	if (!is_exec(dir)) {
	    err(107, __func__, "directory is not searchable: %s", dir);
	    not_reached();
	}

	/*
	 * chdir to to the directory
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = chdir(dir);
	if (ret < 0) {
	    errp(108, __func__, "cannot cd %s", dir);
	    not_reached();
	}
    }

    /*
     * must be a readable file
     */
    if (!exists(file)) {
	err(109, __func__, "file does not exist: %s", file);
	not_reached();
    }
    if (!is_file(file)) {
	err(110, __func__, "file is not a regular file: %s", file);
	not_reached();
    }
    if (!is_read(file)) {
	err(111, __func__, "file is not a readable file: %s", file);
	not_reached();
    }

    /*
     * open the readable file
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret_stream = fopen(file, "r");
    if (ret_stream == NULL) {
	errp(112, __func__, "cannot open file: %s", file);
	not_reached();
    }

    /*
     * if we did a chdir to dir, chdir back to previous cwd
     */
    if (dir != NULL && cwd >= 0) {

	/*
	 * switch back to the previous current directory
	 */
	errno = 0;                  /* pre-clear errno for errp() */
	ret = fchdir(cwd);
	if (ret < 0) {
	    errp(113, __func__, "cannot fchdir to the previous current directory");
	    not_reached();
	}
	errno = 0;                  /* pre-clear errno for errp() */
	ret = close(cwd);
	if (ret < 0) {
	    errp(114, __func__, "close of previous current directory failed");
	    not_reached();
	}
    }

    /*
     * return open stream
     */
    return ret_stream;
}


/*
 * count_char - count the number of instances of a char in the string
 *
 * given:
 *
 *	str	string to search
 *	ch	character to find
 *
 * NOTE: this function does not return on NULL pointer.
 */
size_t
count_char(char const *str, int ch)
{
    size_t count = 0;	    /* number of ch in the string */
    size_t i = 0;	    /* what character in str we're at */

    /*
     * firewall
     */
    if (str == NULL) {
	err(115, __func__, "given NULL str");
	not_reached();
    }

    for (count = 0, i = 0; str[i] != '\0'; ++i) {
	if (str[i] == ch) {
	    ++count;
	}
    }

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
	warn(__func__, "len <= 0");
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
 * check_invalid_option - check option error in getopt()
 *
 * given:
 *
 *	prog	    - program name
 *	ch	    - value returned by getopt()
 *	opt	    - program's optopt (option triggering the error)
 *
 * NOTE:    if prog is NULL we warn and then set to ((NULL prog)).
 * NOTE:    this function should only be called if getopt() returns a ':' or a
 *	    '?' but if anything else is passed to this function we do nothing.
 * NOTE:    this function does NOT take an exit code because it is the caller's
 *	    responsibility to do this. This is because they must call usage()
 *	    (or do whatever they need to do) which is specific to tools etc.
 */
void
check_invalid_option(char const *prog, int ch, int opt)
{
    /*
     * firewall
     */
    if (ch != ':' && ch != '?') {
	return; /* do nothing */
    }
    if (prog == NULL) {
	warn(__func__, "prog is NULL, forcing it to be: ((NULL prog))");
	prog = "((NULL prog))";
    }

    /*
     * report to stderr, based on the value returned by getopt
     */
    switch (ch) {
	case ':':
	    fprint(stderr, "%s: requires an argument -- %c\n\n", prog, opt);
	    break;
	case '?':
	    fprint(stderr, "%s: illegal option -- %c\n\n", prog, opt);
	    break;
	default: /* should never be reached but we include it anyway */
	    fprint(stderr, "%s: unexpected getopt() return value: 0x%02x == <%c>\n\n", prog, ch, ch);
	    break;
    }
    return;
}
