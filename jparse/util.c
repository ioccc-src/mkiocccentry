/*
 * util - common utility functions for the JSON parser
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This JSON parser was co-developed in 2022 by:
 *
 *	@xexyl
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
# if defined(UTIL_TEST)
# include "../../dbg/dbg.h"
# include "../../dyn_array/dyn_array.h"
# else
# include "../dbg/dbg.h"
# include "../dyn_array/dyn_array.h"
# endif
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
#if !defined(UTIL_TEST)
#include "util.h"
#else
/*
 * util_test - part of the test suite
 */
#include <getopt.h>
#include "../util.h"
#include "../version.h"
/*
 * usage message
 *
 * test util usage message
 */
static char const * const usage =
"usage: %s [-h] [-v level] [-V] [-q]\n"
"\n"
"\t-h\t\tprint help message and exit\n"
"\t-v level\tset verbosity level: (def level: 0)\n"
"\t-J level\tset JSON verbosity level (def level: 0)\n"
"\t-V\t\tprint version string and exit\n"
"\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )\n"
"\n"
"jparse util test version: %s\n"
"jparse UTF-8 version: %s\n"
"jparse library version: %s";
#endif /* UTIL_TEST */

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
	err(100, __func__, "called with NULL path"); /*coo*/
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
	errp(102, __func__, "strdup(\"%s\") failed", p + 1);
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
bool
exists(char const *path)
{
    int ret;			/* return code holder */
    struct stat buf;		/* path status */

    /*
     * firewall
     */
    if (path == NULL) {
	err(103, __func__, "called with NULL path");
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
	err(104, __func__, "called with NULL path");
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
	err(105, __func__, "called with NULL path");
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
	err(106, __func__, "called with NULL path");
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
	err(107, __func__, "called with NULL path");
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
	err(108, __func__, "called with NULL path");
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
 * is_open_file_stream - determine if a file stream is open
 *
 * Determine if a file stream that is NOT stdin, NOR associated
 * with valid terminal type device (tty), is open.
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
 * chk_stdio_printf_err - check for a print function call errors
 *
 * Here "print function call" refers to functions such as:
 *
 *	print(3), fprintf(3), dprintf(3),
 *	vprintf(3), vfprintf(3), vdprintf(3)
 *
 * On some systems, such as macOS, the first stdio print function call
 * that prints to a given stream first checks if the stream is associated
 * with a TTY (presumably so that it might perform certain TTY related
 * ioctl(2) actions on the stream).  The TTY check is performed by isatty(3).
 *
 * If the stream is NOT a TTY, then isatty(3) will fail.  Now on some systems
 * the first stdio print function call that performs such a isatty(3) will
 * such as macOS, leave errno with the value ENOTTY, even though the actual
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
 * flush_tty - flush stdin, stdout and stderr
 *
 * We flush all pending stdio data if they are open.
 * We ignore the flush of they are not open.
 *
 * given:
 *	name		- name of the calling function
 *	flush_stdin	- true ==> stdin should be flushed as well as stdout and stderr,
 *			  false ==> only flush stdout and stderr
 *	abort_on_error	- false ==> return exit code if able to successfully call system(), or
 *			            return EXIT_CALLOC_FAILED malloc() failure,
 *			            return EXIT_FFLUSH_FAILED on fflush failure,
 *			            return EXIT_SYSTEM_FAILED if system() failed,
 *			            return EXIT_NULL_ARGS if NULL pointers were passed
 *			  true ==> return exit code if able to successfully call system(), or
 *			           call errp() (and thus exit) if unsuccessful
 *
 * IMPORTANT: If write_mode == true, then pending stdin will be flushed.
 *	      If this process has not read all pending data on stdin, then
 *	      such pending data will be lost by the internal call to fflush(stdin).
 *	      It is the responsibility of the calling function to have read all stdin
 *	      OR accept that such pending stdin data will be lost.
 *
 * NOTE: This function does not return on error (such as a stdio related error).
 */
void
flush_tty(char const *name, bool flush_stdin, bool abort_on_error)
{
    int ret;			/* return code holder */

    /*
     * case: flush_stdin is true
     */
    if (flush_stdin == true) {

	/*
	 * pre-flush stdin, if open, to avoid buffered stdio issues with the child process
	 *
	 * We do not want the child process to "inherit" buffered stdio stdin data
	 * waiting to be read as this could result in data being read twice or worse.
	 *
	 * NOTE: If this process has not read all pending data on stdin, this
	 *	     next fflush() will cause that data to be lost.
	 */
	if (fd_is_ready(name, true, fileno(stdin))) {
	    clearerr(stdin);		/* pre-clear ferror() status */
	    errno = 0;			/* pre-clear errno for errp() */
	    ret = fflush(stdin);
	    if (ret < 0) {
		/* exit or error return depending on abort_on_error */
		if (abort_on_error) {
		    errp(109, name, "fflush(stdin): error code: %d", ret);
		    not_reached();
		} else {
		    dbg(DBG_MED, "%s: called via %s: fflush(stdin) failed: %s", __func__, name, strerror(errno));
		    return;
		}
	    }
	} else {
	    dbg(DBG_VHIGH, "%s: called via %s: stdin is NOT open, flush request ignored", __func__, name);
	}
    }

    /*
     * pre-flush stdout, if open, to avoid buffered stdio issues with the child process
     *
     * We do not want the child process to "inherit" buffered stdio stdout data
     * waiting to be written as this could result in data being written twice or worse.
     */
    if (fd_is_ready(name, true, fileno(stdout))) {
	clearerr(stdout);		/* pre-clear ferror() status */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fflush(stdout);
	if (ret < 0) {
	    /* exit or error return depending on abort_on_error */
	    if (abort_on_error) {
		errp(110, name, "fflush(stdout): error code: %d", ret);
		not_reached();
	    } else {
		dbg(DBG_MED, "%s: called from %s: fflush(stdout) failed: %s", __func__, name, strerror(errno));
		return;
	    }
	}
    } else {
	dbg(DBG_VHIGH, "%s: called via %s: stdout is NOT open, flush request ignored", __func__, name);
    }

    /*
     * pre-flush stderr, if open, to avoid buffered stdio issues with the child process
     *
     * We do not want the child process to "inherit" buffered stdio stderr data
     * waiting to be written as this could result in data being written twice or worse.
     */
    if (fd_is_ready(name, true, fileno(stderr))) {
	clearerr(stderr);		/* pre-clear ferror() status */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fflush(stderr);
	if (ret < 0) {
	    /* exit or error return depending on abort_on_error */
	    if (abort_on_error) {
		errp(111, name, "fflush(stderr): error code: %d", ret);
		not_reached();
	    } else {
		dbg(DBG_MED, "%s: called from %s: fflush(stderr) failed: %s", __func__, name, strerror(errno));
		return;
	    }
	}
    } else {
	dbg(DBG_VHIGH, "%s: called via %s: stderr is NOT open, flush request ignored", __func__, name);
    }
    return;
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
	err(112, __func__, "called with NULL path");
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
	warn(__func__, "stored characters: %jd != size: %ju",
	     (intmax_t)((size_t)(d + 1 - cmd)), (uintmax_t)size);

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
 * shell_cmd - pass a command, via vcmdprintf() interface, to the shell
 *
 * Attempt to call the shell with a command string.
 *
 * given:
 *	name		- name of the calling function
 *	flush_stdin	- true ==> stdin should be flushed as well as stdout and stderr,
 *			  false ==> only flush stdout and stderr
 *	abort_on_error	- false ==> return exit code if able to successfully call system(), or
 *			            return EXIT_CALLOC_FAILED malloc() failure,
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
    va_list ap;			/* variable argument list */
    char *cmd = NULL;		/* e.g. cp prog.c submission_dir/prog.c */
    int exit_code;		/* exit code from system(cmd) */

    /*
     * firewall
     */
    if (name == NULL) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    err(113, __func__, "function name is not caller name because we were called with NULL name");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called with NULL name, returning: %d < 0", EXIT_NULL_ARGS);
	    return EXIT_NULL_ARGS;
	}
    }
    if (format == NULL) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    err(114, name, "called with NULL format");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called with NULL format, returning: %d < 0", EXIT_NULL_ARGS);
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
    errno = 0;			/* pre-clear errno for errp() */
    cmd = vcmdprintf(format, ap);
    if (cmd == NULL) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(115, name, "calloc failed in vcmdprintf()");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: calloc failed in vcmdprintf(): %s, returning: %d < 0",
			 name, strerror(errno), EXIT_CALLOC_FAILED);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return EXIT_CALLOC_FAILED;
	}
    }

    /*
     * flush stdio as needed
     */
    flush_tty(name, flush_stdin, abort_on_error);

    /*
     * execute the command
     */
    dbg(DBG_HIGH, "about to perform: system(%s)", cmd);
    errno = 0;			/* pre-clear errno for errp() */
    exit_code = system(cmd);
    if (exit_code < 0) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(116, __func__, "error calling system(%s)", cmd);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: error calling system(%s)", name, cmd);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    /* free allocated command storage */
	    if (cmd != NULL) {
		free(cmd);
		cmd = NULL;
	    }
	    return EXIT_SYSTEM_FAILED;
	}

    /*
     * case: exit code 127 usually means the fork/exec was unable to invoke the shell
     */
    } else if (exit_code == 127) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(117, __func__, "execution of the shell failed for system(%s)", cmd);
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: execution of the shell failed for system(%s)", name, cmd);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    /* free allocated command storage */
	    if (cmd != NULL) {
		free(cmd);
		cmd = NULL;
	    }
	    return EXIT_SYSTEM_FAILED;
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
 *      ...     - args to give after the format
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

    /*
     * firewall
     */
    if (name == NULL) {
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    err(118, __func__, "function name is not caller name because we were called with NULL name");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called with NULL name, returning NULL");
	    return NULL;
	}
    }
    if (format == NULL) {
	/* exit or error return depending on abort */
	if (abort_on_error) {
	    err(119, name, "called with NULL format");
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
	    errp(120, name, "calloc failed in vcmdprintf()");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: calloc failed in vcmdprintf(): %s returning: %d < 0",
			 name, strerror(errno), EXIT_CALLOC_FAILED);
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    return NULL;
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
    dbg(DBG_HIGH, "about to perform: popen(%s, \"%s\")", cmd, write_mode?"w":"r");
    errno = 0;			/* pre-clear errno for errp() */
    stream = popen(cmd, write_mode?"w":"r");
    if (stream == NULL) {
	/* exit or error return depending on abort_on_error */
	if (abort_on_error) {
	    errp(121, name, "error calling popen(%s, \"%s\")", cmd, write_mode?"w":"r");
	    not_reached();
	} else {
	    dbg(DBG_MED, "called from %s: error calling popen(%s, \"%s\"): %s", name, cmd, write_mode?"w":"r", strerror(errno));
	    va_end(ap);		/* stdarg variable argument list cleanup */
	    /* free allocated command storage */
	    if (cmd != NULL) {
		free(cmd);
		cmd = NULL;
	    }
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
	err(122, __func__, "stdout is NULL");
	not_reached();
    }
    clearerr(stdout);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    /*
     * this may not always catch a bogus or unopened stdout, but try anyway
     */
    fd = fileno(stdout);
    if (fd < 0) {
	errp(123, __func__, "fileno on stdout returned: %d < 0", fd);
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
		errp(124, __func__, "error writing paragraph to a stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(125, __func__, "EOF while writing paragraph to a stdout");
		not_reached();
	    } else {
		errp(126, __func__, "unexpected fputs error writing paragraph to stdout");
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
		errp(128, __func__, "error writing newline to stdout");
		not_reached();
	    } else if (feof(stdout)) {
		err(129, __func__, "EOF while writing newline to stdout");
		not_reached();
	    } else {
		errp(130, __func__, "unexpected fputc error writing newline to stdout");
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
	    errp(131, __func__, "error flushing stdout");
	    not_reached();
	} else if (feof(stdout)) {
	    err(132, __func__, "EOF while flushing stdout");
	    not_reached();
	} else {
	    errp(133, __func__, "unexpected fflush error while flushing stdout");
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
	err(134, __func__, "stream is NULL");
	not_reached();
    }

    /*
     * this may not always catch a bogus or unopened stream, but try anyway
     */
    clearerr(stream);		/* pre-clear ferror() status */
    errno = 0;			/* pre-clear errno for errp() */
    fd = fileno(stream);
    if (fd < 0) {
	errp(135, __func__, "fileno on stream returned: %d < 0", fd);
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
		errp(136, __func__, "error writing paragraph to stream");
		not_reached();
	    } else if (feof(stream)) {
		err(137, __func__, "EOF while writing paragraph to stream");
		not_reached();
	    } else {
		errp(138, __func__, "unexpected fputs error writing paragraph to stream");
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
		errp(139, __func__, "error writing newline to stream");
		not_reached();
	    } else if (feof(stream)) {
		err(140, __func__, "EOF while writing newline to stream");
		not_reached();
	    } else {
		errp(141, __func__, "unexpected fputc error writing newline to stream");
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
	    errp(142, __func__, "error flushing stream");
	    not_reached();
	} else if (feof(stream)) {
	    err(143, __func__, "EOF while flushing stream");
	    not_reached();
	} else {
	    errp(144, __func__, "unexpected fflush error while flushing stream");
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
	err(145, __func__, "called with NULL arg(s)");
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
	    errp(146, __func__, "getline() error");
	    not_reached();
	} else {
	    errp(147, __func__, "unexpected getline() error");
	    not_reached();
	}
    }
    /*
     * paranoia
     */
    if (*linep == NULL) {
	err(148, __func__, "*linep is NULL after getline()");
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
	err(149, __func__, "called with NULL arg(s)");
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
    ret = calloc((size_t)len+1+1, sizeof(char));
    if (ret == NULL) {
	errp(150, __func__, "calloc of read line of %jd bytes failed", (intmax_t)len+1+1);
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
	err(151, __func__, "called with NULL stream");
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
	warn(__func__, "string <%s> has no digits", str);
	return false;
    } else if (*endptr != '\0') {
	warn(__func__, "number <%s> has invalid characters", str);
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
	warn(__func__, "string <%s> has no digits", str);
	return false;
    } else if (*endptr != '\0') {
	warn(__func__, "number <%s> has invalid characters", str);
	return false;
    } else if (saved_errno != 0) {
	errno = saved_errno;
	warnp(__func__, "error converting string <%s> to uintmax_t", str);
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
	warn(__func__, "strlen(%s): %ju < len arg: %ju", str, (uintmax_t)str_len, (uintmax_t)len);
	return false;	/* processing failed */
    /*
     * JSON spec detail: floating point numbers cannot end with .
     */
    } else if (str[len-1] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with .: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with -
     */
    } else if (str[len-1] == '-') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with -: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with +
     */
    } else if (str[len-1] == '+') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with +: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    }

    /* detect dot */
    dot_found = strchr(str, '.');
    /* if dot found see if there's another one */
    if (dot_found != NULL) {
	dot = strrchr(str, '.');
	if (dot != NULL && dot != dot_found) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have two '.'s: <%s>",
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
	warn(__func__, "strlen(%s): %ju < len arg: %ju", str, (uintmax_t)str_len, (uintmax_t)len);
	return false;	/* processing failed */
    }

    /*
     * JSON spec detail: floating point numbers cannot start with .
     */
    if (str[0] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot start with .: <%s>",
		       __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with .
     */
    } else if (str[len-1] == '.') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with .: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with -
     */
    } else if (str[len-1] == '-') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with -: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with +
     */
    } else if (str[len-1] == '+') {
	dbg(DBG_HIGH, "in %s(): floating point numbers cannot end with +: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers must end in a digit
     */
    } else if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	dbg(DBG_HIGH, "in %s(): floating point numbers must end in a digit: <%s>",
		       __func__, str);
	return false;	/* processing failed */
    }

    /* detect dot */
    dot_found = strchr(str, '.');
    /* if dot found see if there's another one */
    if (dot_found != NULL) {
	dot = strrchr(str, '.');
	if (dot != NULL && dot != dot_found) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have two '.'s: <%s>",
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

	dbg(DBG_HIGH, "in %s(): floating point numbers cannot use both e and E: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /* case: neither 'e' nor 'E' found */
    } else if (e_found == NULL && cap_e_found == NULL) {
	/* NOTE: don't warn as it could be a floating point without e notation */
	dbg(DBG_HIGH, "in %s(): not e notation: neither 'e' nor 'E' found: <%s>",
		      __func__, str);
	return false;
    /* case: just e found, no E */
    } else if (e_found != NULL) {

	/* firewall - search for two 'e's */
	e = strrchr(str, 'e');
	if (e_found != e) {
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have more than one e: <%s>",
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
	    dbg(DBG_HIGH, "in %s(): floating point numbers cannot have more than one E: <%s>",
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
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot start with e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot end with e or E
	 */
	} else if (e == &(str[len-1])) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot end with e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot have e or E after .
	 */
	} else if (e > str && e[-1] == '.') {
	    dbg(DBG_HIGH, "in %s(): e notation numbers cannot have '.' before e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number must have digit before e or E
	 */
	} else if (e > str && (!isascii(e[-1]) || !isdigit(e[-1]))) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers must have digit before e or E: <%s>",
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
		dbg(DBG_HIGH, "in %s(): :e notation number with e+ or E+ must be followed by a digit <%s>",
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
		dbg(DBG_HIGH, "in %s(): :e notation number with e- or E- must be followed by a digit <%s>",
			      __func__, str);
		return false;	/* processing failed */
	    }

	/*
	 * JSON spec detail: e notation number must have + or - or digit after e or E
	 */
	} else if (!isascii(e[1]) || !isdigit(e[1])) {
	    dbg(DBG_HIGH, "in %s(): e notation numbers must follow e or E with + or - or digit: <%s>",
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
 *	true ==> str is a valid POSIX portable safe + filename, AND
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
 * sane_relative_path - test if each component of a path is posix plus safe
 *
 * Using posix_plus_safe() with lower_only == false, slash_ok == false and first
 * == true, for each component of the relative path, we determine if each
 * component of a relative path is sane.
 *
 * By relative we mean that the path cannot start with a '/'.
 *
 * By sane we mean that no path component is an unsafe name according to
 * posix_plus_safe(str, false, false, true).
 *
 * In other words, each directory and the final file in a path must match the
 * regexp:
 *
 *      ^[0-9A-Za-z]+[0-9A-Za-z_+.-]*$
 *
 * and in addition, all paths must be relative to the top directory (first
 * component; i.e. it must not start with a '/') and may not exceed max_depth
 * directories in the file path.
 *
 * given:
 *	str		    - string to test
 *      max_path_len        - max path length (length of str)
 *	max_filename_len    - max length of each component of path
 *      max_depth           - max depth of subdirectory tree
 *
 * returns:
 *      if a relative sane path: PATH_OK, otherwise another one of the enum
 *      path_sanity depending on what went wrong (see below for a table).
 *
 * NOTE: if str is NULL or empty we return PATH_ERR_PATH_IS_NULL rather than
 * make it an actual error that terminates the program.
 *
 * NOTE: depth is defined in the same way as the find(1) tool. For instance,
 * given the below directory tree:
 *
 *      foo/bar/baz/zab/rab/oof
 *
 *
 * the directory names and depths are as follows:
 *
 *      depth                   directory name
 *      0                       .
 *      1                       ./foo
 *      2                       ./foo/bar
 *      3                       ./foo/bar/baz
 *      4                       ./foo/bar/baz/zab
 *      5                       ./foo/bar/baz/zab/rab
 *      6                       ./foo/bar/baz/zab/rab/oof
 *
 * The following table shows the error conditions and the respective enum
 * path_sanity value:
 *
 *      condition                       path_sanity enum value
 *
 *      relative, sane                  PATH_OK
 *      str == NULL                     PATH_ERR_PATH_IS_NULL
 *      empty path (str)                PATH_ERR_PATH_EMPTY
 *      path starts with '/'            PATH_ERR_NOT_RELATIVE
 *      max_depth <= 0                  PATH_ERR_DEPTH_0
 *      max_path_len <= 0               PATH_ERR_PATH_LEN_0
 *      path (str) too long             PATH_ERR_PATH_TOO_LONG
 *      path component name too long    PATH_ERR_NAME_TOO_LONG
 *      max_filename_len <= 0           PATH_ERR_MAX_NAME_LEN_0
 *      depth > max_depth               PATH_ERR_PATH_TOO_DEEP
 *      path component not sane         PATH_ERR_NOT_POSIX_SAFE
 *
 *
 * NOTE: we MUST use strdup() on the string because we need to use strtok_r() to
 * extract the '/'s in the string. This strdup()d char * will be freed prior to
 * calling, unless an error occurs.
 */
enum path_sanity
sane_relative_path(char const *str, uintmax_t max_path_len, uintmax_t max_filename_len,
        uintmax_t max_depth)
{
    size_t len;		    /* length of str */
    size_t n;
    uintmax_t depth = 1;    /* to check max depth */
    char *p = NULL;         /* first '/' */
    char *saveptr = NULL;   /* for strtok_r() */
    char *dup = NULL;       /* we need to strdup() the string */
    bool sane = true;       /* assume path is sane */

    /*
     * firewall
     */
    /*
     * the path string (str) must not be NULL
     */
    if (str == NULL) {
	dbg(DBG_VVHIGH, "%s: str is NULL", __func__);

        return PATH_ERR_PATH_IS_NULL;
    }

    /*
     * if the max path length <= 0 we can't do anything else
     */
    if (max_path_len <= (uintmax_t)0) {
        dbg(DBG_VVHIGH, "%s: max_path_len %ju <= 0", __func__, max_path_len);

        return PATH_ERR_MAX_PATH_LEN_0;
    }

    /*
     * if the max filename length <= 0 we can't do anything else
     */
    if (max_filename_len <= (uintmax_t)0) {
        dbg(DBG_VVHIGH, "%s: max_filename_len %ju <= 0", __func__, max_filename_len);

        return PATH_ERR_MAX_NAME_LEN_0;
    }


    /*
     * now that we know the max path len > 0 we can determine if the path string
     * (str) is empty or too long
     */
    len = strlen(str);
    /*
     * case: empty path
     */
    if (len <= 0) {
	dbg(DBG_VVHIGH, "%s: str is an empty string", __func__);

        return PATH_ERR_PATH_EMPTY;
    /*
     * case: length of path too long
     */
    } else if (len > max_path_len) {
	dbg(DBG_VVVVHIGH, "%s: \"%s\" length %ju > max %ju", __func__, str, len, max_path_len);
        dbg(DBG_VVVHIGH, "\"%s\" is not a relative, sane path", str);

        return PATH_ERR_PATH_TOO_LONG;
    }

    /*
     * if the max depth is <= 0 there's nothing we can do
     */
    if (max_depth <= (uintmax_t)0) {
        dbg(DBG_VVHIGH, "%s: max depth %ju <= 0", __func__, (uintmax_t)max_depth);
        return PATH_ERR_MAX_DEPTH_0;
    }

    /*
     * debug output max values
     */
    dbg(DBG_VVVHIGH, "%s: max_filename_len: %ju", __func__, (uintmax_t)max_filename_len);
    dbg(DBG_VVVHIGH, "%s: max_depth: %ju", __func__, (uintmax_t)max_depth);
    dbg(DBG_VVVHIGH, "%s: max_path_len: %ju", __func__, (uintmax_t)max_path_len);

    /*
     * determine if the path is relative or not
     *
     * NOTE: a relative path is one that does not start with a '/'.
     */
    if (*str == '/') {
        dbg(DBG_VVVVVHIGH, "%s: \"%s\" first char is '/'", __func__, str);
        dbg(DBG_VVVHIGH, "\"%s\" is not a relative path", str);

        return PATH_ERR_NOT_RELATIVE;
    }

    /*
     * If we get here we KNOW it's a relative path so we must extract each
     * component and use posix_safe_plus(str, false, false, false) as well as
     * run checks on the depth (as we extract the components) and the length of
     * each component itself, with the max_depth and max_filename_len
     * parameters.
     */

    /*
     * Before we can do anything else, we have to duplicate the string. If
     * this fails, it is an error because we can't do anything more.
     *
     * NOTE: as this will terminate the program we do not need an error code in
     * the path_sanity enum.
     */
    errno = 0; /* pre-clear errno for errp() */
    dup = strdup(str);
    if (dup == NULL) {
        errp(152, __func__, "duplicating \"%s\" failed", str);
        not_reached();
    }

    /*
     * show the string we're checking, if debug level high enough
     */
    dbg(DBG_VVVHIGH, "%s: parsing string: \"%s\"", __func__, dup);

    /*
     * ensure depth is set to 1 first
     */
    depth = 1;

    /*
     * We already know it's a relative path so we can begin parsing the string.
     */
    p = strtok_r(dup, "/", &saveptr);
    if (p == NULL) {
        /*
         * In the case of no '/' found at all, the initial string is tested by itself.
         *
         * NOTE: we do not need to check the depth here because we KNOW the
         * max_depth is >= 1!
         */
        dbg(DBG_VVVHIGH, "%s: \"%s\" has no '/'", __func__, dup);

        /*
         * obtain length of the entire path as we have no '/'.
         */
        n = strlen(dup);
        dbg(DBG_VVVHIGH, "%s: \"%s\" length %ju", __func__, dup, (uintmax_t)n);

        /*
         * Here we have to verify that the path is not longer than the max filename
         * length, whereas if there is a '/' we check this against each
         * component. This check mean that even if the total length of the
         * path is not too long, if passed an invalid max filename length, the
         * check here could end up declaring the path is not a sane relative
         * path.
         */
        if (n > max_filename_len) {
            dbg(DBG_VVVVHIGH, "%s: \"%s\" length %ju > max %ju", __func__, dup, (uintmax_t)n, (uintmax_t)max_filename_len);
            return PATH_ERR_NAME_TOO_LONG;
        } else {
            /*
             * we know that the filename length (in this case the entire path)
             * is not too long but we still have to check for POSIX plus safe
             * chars on the entire string (instead of a component).
             */
            dbg(DBG_VVVVHIGH, "%s: \"%s\" length %ju <= max %ju", __func__, dup, (uintmax_t)n, (uintmax_t)max_filename_len);
            dbg(DBG_VVVHIGH, "%s: about to call: posix_plus_safe(\"%s\", false, false, true)", __func__, dup);
            sane = posix_plus_safe(dup, false, false, true);
            if (!sane) {
                dbg(DBG_VVVHIGH, "%s: \"%s\" is not POSIX plus + safe chars", __func__, dup);
                return PATH_ERR_NOT_POSIX_SAFE;
            } else {
                dbg(DBG_VVVHIGH, "%s: \"%s\" is POSIX plus + safe chars", __func__, dup);
            }
        }
    } else {
        /*
         * here we actually have at least one '/' so the checks above are done
         * for each component, rather than the entire path in str.
         */
        dbg(DBG_VVVVHIGH, "%s: testing first component \"%s\"", __func__, p);

        /*
         * like for the check when there is no '/' we need to check the filename
         * length, except that the filename is the component (which could be a
         * subdirectory name).
         */
        n = strlen(p);
        dbg(DBG_VVVHIGH, "%s: first component \"%s\" length %ju", __func__, p, (uintmax_t)n);
        if (n > max_filename_len) {
            /*
             * if the first component is too long we won't bother with the rest
             * of them
             */
            dbg(DBG_VVVVHIGH, "%s: first component \"%s\" length %ju > max %ju", __func__, p, (uintmax_t)n,
                    (uintmax_t)max_filename_len);

            return PATH_ERR_NAME_TOO_LONG;
        } else {
            /*
             * we first report, if debug level is high enough, that the first
             * component length is not too long according to max_filename_len.
             */
            dbg(DBG_VVVVHIGH, "%s: first component \"%s\" length %ju <= max %ju", __func__, p, n, max_filename_len);

            /*
             * also report, if debug level high enough, that we're about to test
             * the POSIX plus + safe chars on the first component.
             */
            dbg(DBG_VVVHIGH, "%s: about to call: posix_plus_safe(\"%s\", false, false, true)", __func__, p);

            /*
             * before we check for further components we have to verify that the
             * first component (before the first '/') is POSIX plus + safe
             * chars.
             */
            sane = posix_plus_safe(p, false, false, true);
            /*
             * if the first component is sane, we have to check any
             * additional components for sanity, doing the same steps as
             * above.
             */
            if (sane) {
                /*
                 * here we extract the remaining components of the path (after
                 * the first '/'), by looking for the next path separator
                 */
                for (p = strtok_r(NULL, "/", &saveptr), ++depth; p != NULL && sane; p = strtok_r(NULL, "/", &saveptr), ++depth) {
                    /*
                     * show additional debug information here as here we have to
                     * check depths too
                     */
                    dbg(DBG_VVVHIGH, "%s: testing component \"%s\" (depth %ju)", __func__, p, (uintmax_t)depth);

                    /*
                     * If the max depth is > 1 we do need to check the current
                     * depth. The reason we check for max depth > 1 first is
                     * because if we're here the depth will always be at least
                     * 1. This was arbitrarily chosen, however, and it is
                     * possible that it could be undone, if a reason occurs that
                     * necessitates it.
                     */
                    if (depth > max_depth) {
                        /*
                         * if we have gone beyond the max depth, we won't bother
                         * to continue so just report it (if verbosity level
                         * high enough), flag the path as not sane and break out
                         * of the loop.
                         */
                        dbg(DBG_VVVHIGH, "%s: depth %ju > max depth %ju", __func__, (uintmax_t)depth, (uintmax_t)max_depth);

                        return PATH_ERR_PATH_TOO_DEEP;
                    } else {
                        dbg(DBG_VVVVHIGH, "%s: depth %ju <= max depth %ju", __func__, (uintmax_t)depth, (uintmax_t)max_depth);
                    }

                    /*
                     * whereas earlier in this function we checked the full
                     * string (as there is no '/') here we check just this
                     * component, just like the first component before this loop
                     */
                    n = strlen(p);
                    if (n > max_filename_len) {
                        dbg(DBG_VVVVHIGH, "%s: component \"%s\" length %ju > max %ju", __func__, p, (uintmax_t)n,
                                (uintmax_t)max_filename_len);

                        return PATH_ERR_NAME_TOO_LONG;
                    } else {
                        dbg(DBG_VVVVHIGH, "%s: component \"%s\" length %ju <= max %ju", __func__, p, (uintmax_t)n,
                                (uintmax_t)max_filename_len);
                    }

                    /*
                     * as long as the depth is not > the max depth and the
                     * current component length is not > the max length, we can
                     * do the POSIX plus + safe chars checks
                     */
                    dbg(DBG_VVVHIGH, "%s: about to call: posix_plus_safe(\"%s\", false, false, true)", __func__, p);
                    sane = posix_plus_safe(p, false, false, true);
                    if (sane) {
                        dbg(DBG_VVVHIGH, "%s: component \"%s\" is POSIX plus + safe chars", __func__, p);
                    } else {
                        dbg(DBG_VVVHIGH, "%s: component \"%s\" is not POSIX plus + safe chars", __func__, p);

                        return PATH_ERR_NOT_POSIX_SAFE;
                    }
                }
            } else {
                /*
                 * when we get here, the first component is not safe so we don't
                 * do anything else but report, if verbosity level high enough,
                 * that the component is not POSIX plus safe.
                 */
                dbg(DBG_VVVHIGH, "%s: component \"%s\" is not POSIX plus + safe chars", __func__, p);

                return PATH_ERR_NOT_POSIX_SAFE;
            }
        }
    }

    /*
     * free dup, if not NULL (and it never should be)
     */
    if (dup != NULL) {
        free(dup);
        dup = NULL;
    }

    /*
     * return sane value
     */
    dbg(DBG_VVVHIGH, "%s is a relative, sane path", str);
    return PATH_OK;
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
            str ="path is a sane relative path";
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
        case PATH_ERR_MAX_PATH_LEN_0:
            str = "max path length <= 0";
            break;
        case PATH_ERR_MAX_DEPTH_0:
            str = "max path depth <= 0";
            break;
        case PATH_ERR_NOT_RELATIVE:
            str = "path not a relative path";
            break;
        case PATH_ERR_NAME_TOO_LONG:
            str = "a path component > max filename length";
            break;
        case PATH_ERR_MAX_NAME_LEN_0:
            str = "max filename length is <= 0";
            break;
        case PATH_ERR_PATH_TOO_DEEP:
            str = "depth > max depth";
            break;
        case PATH_ERR_NOT_POSIX_SAFE:
            str = "path component invalid";
            break;
        default:
            str = "invalid path_sanity value";
            break;
    }

    return str;
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
        case PATH_ERR_MAX_PATH_LEN_0:
            str = "PATH_ERR_MAX_PATH_LEN_0";
            break;
        case PATH_ERR_MAX_DEPTH_0:
            str = "PATH_ERR_MAX_DEPTH_0";
            break;
        case PATH_ERR_NOT_RELATIVE:
            str = "PATH_ERR_NOT_RELATIVE";
            break;
        case PATH_ERR_NAME_TOO_LONG:
            str = "PATH_ERR_NAME_TOO_LONG";
            break;
        case PATH_ERR_MAX_NAME_LEN_0:
            str = "PATH_ERR_MAX_NAME_LEN_0";
            break;
        case PATH_ERR_PATH_TOO_DEEP:
            str = "PATH_ERR_PATH_TOO_DEEP";
            break;
        case PATH_ERR_NOT_POSIX_SAFE:
            str = "PATH_ERR_NOT_POSIX_SAFE";
            break;
        default:
        case PATH_ERR_UNKNOWN:
            str = "PATH_ERR_UNKNOWN";
            break;
    }

    return str;
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
	err(153, __func__, "called with NULL arg(s)");
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
	dbg(DBG_VVHIGH, "posix_safe_chk(%s, false, false, false, false) returning: str is an empty string", str);
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
	dbg(DBG_VVHIGH, "posix_safe_chk(%s, %s, %s, %s, %s): string is NOT POSIX portable safe plus +/",
		str, booltostr(slash), booltostr(posix_safe), booltostr(first_alphanum), booltostr(upper));

    /*
     * report POSIX portable safe plus + safe with maybe /
     */
    } else {
	dbg(DBG_VVHIGH, "posix_safe_chk(%s, %s, %s, %s, %s): string is POSIX portable safe plus +/: <%s>",
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
    if (count <= *countp) {
	warn(__func__, "incremented count <= *countp: count %jd <= *countp %jd", count, *countp);
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
    char *buf = NULL;		/* malloced string to return */
    size_t len;			/* length of path */

    /*
     * firewall
     */
    if (filename == NULL) {
	err(154, __func__, "filename is NULL");
	not_reached();
    }

    /*
     * case: dirname is NULL
     *
     * NULL dirname means path is just filename
     */
    if (dirname == NULL) {

	/*
	 * just return a newly malloced filename
	 */
	errno = 0;		/* pre-clear errno for errp() */
	buf = strdup(filename);
	if (buf == NULL) {
	    errp(155, __func__, "strdup of filename failed: %s", filename);
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
	len = strlen(dirname) + 1 + strlen(filename) + 1; /* + 1 for NUL */
	buf = calloc(len+2, sizeof(char));	/* + 1 for paranoia padding */
	errno = 0;		/* pre-clear errno for errp() */
	if (buf == NULL) {
	    errp(156, __func__, "calloc of %ju bytes failed", (uintmax_t)len);
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
	    errp(157, __func__, "snprintf returned: %zu < 0", len);
	    not_reached();
	}
    }

    /*
     * return malloc path
     */
    if (buf == NULL) {
	errp(158, __func__, "function attempted to return NULL");
	not_reached();
    }
    return buf;
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
	err(159, __func__, "called with NULL file");
	not_reached();
    }

    /*
     * note the current directory so we can restore it later, after the chdir(work_dir) below
     */
    errno = 0;                  /* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
        errp(160, __func__, "cannot open .");
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
	    err(161, __func__, "directory does not exist: %s", dir);
	    not_reached();
	}
	if (!is_dir(dir)) {
	    err(162, __func__, "is not a directory: %s", dir);
	    not_reached();
	}
	if (!is_exec(dir)) {
	    err(163, __func__, "directory is not searchable: %s", dir);
	    not_reached();
	}

	/*
	 * chdir to to the directory
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = chdir(dir);
	if (ret < 0) {
	    errp(164, __func__, "cannot cd %s", dir);
	    not_reached();
	}
    }

    /*
     * must be a readable file
     */
    if (!exists(file)) {
	err(165, __func__, "file does not exist: %s", file);
	not_reached();
    }
    if (!is_file(file)) {
	err(166, __func__, "file is not a regular file: %s", file);
	not_reached();
    }
    if (!is_read(file)) {
	err(167, __func__, "file is not a readable file: %s", file);
	not_reached();
    }

    /*
     * open the readable file
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret_stream = fopen(file, "r");
    if (ret_stream == NULL) {
	errp(168, __func__, "cannot open file: %s", file);
	not_reached();
    }

    /*
     * if we did a chdir to dir, chdir back to cwd
     */
    if (dir != NULL && cwd >= 0) {

	/*
	 * switch back to the previous current directory
	 */
	errno = 0;                  /* pre-clear errno for errp() */
	ret = fchdir(cwd);
	if (ret < 0) {
	    errp(169, __func__, "cannot fchdir to the previous current directory");
	    not_reached();
	}
	errno = 0;                  /* pre-clear errno for errp() */
	ret = close(cwd);
	if (ret < 0) {
	    errp(170, __func__, "close of previous current directory failed");
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
	err(171, __func__, "given NULL str");
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
 *	    which is specific to each tool.
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

#if defined(UTIL_TEST)
/*
 * jparse - JSON library
 */
#include "../jparse.h"

/*
 * json_util - JSON util functions
 */
#include "../json_util.h"

/*
 * json_utf8 - jparse utf8 version
 */
#include "../json_utf8.h"

#define UTIL_TEST_VERSION "1.0.1 2025-01-08" /* version format: major.minor YYYY-MM-DD */

int
main(int argc, char **argv)
{
    char *program = NULL;		/* our name */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char *buf = NULL;
    char const *dirname = "foo";
    char const *filename = "bar";
    char const *relpath = "foo";
    struct json *tree = NULL;           /* check that the jparse.json file is valid JSON */
    int ret;
    int i;
    enum path_sanity sanity;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:Vqe:")) != -1) {
	switch (i) {
	case 'h':	/* -h - write help, to stderr and exit 0 */
	    fprintf_usage(0, stderr, usage, program, UTIL_TEST_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION); /*ooo*/
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    errno = 0;			/* pre-clear errno for errp() */
	    verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(1, __func__, "cannot parse -v arg: %s", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'J':	/* -J verbosity */
	    /* parse JSON verbosity */
	    errno = 0;			/* pre-clear errno for errp() */
	    json_verbosity_level = (int)strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(1, __func__, "cannot parse -J arg: %s", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - write version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("%s\n", JPARSE_LIBRARY_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error writing version string: %s", JPARSE_LIBRARY_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case ':':
	    (void) fprintf(stderr, "%s: requires an argument -- %c\n\n", program, optopt);
	    fprintf_usage(3, stderr, usage, program, UTIL_TEST_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION); /*ooo*/
	    not_reached();
	    break;
	case '?':
	    (void) fprintf(stderr, "%s: illegal option -- %c\n\n", program, optopt);
	    fprintf_usage(3, stderr, usage, program, UTIL_TEST_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION); /*ooo*/
	    not_reached();
	    break;
	default:
	    fprintf_usage(DO_NOT_EXIT, stderr, "invalid -flag");
	    fprintf_usage(3, stderr, usage, program, UTIL_TEST_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION); /*ooo*/
	    not_reached();
	}
    }
    /*
     * ignore any extra args
     */

    /*
     * report on dbg state, if debugging
     */
    fdbg(stderr, DBG_MED, "verbosity_level: %d", verbosity_level);
    fdbg(stderr, DBG_MED, "json_verbosity_level: %d", json_verbosity_level);
    fdbg(stderr, DBG_MED, "msg_output_allowed: %s", booltostr(msg_output_allowed));
    fdbg(stderr, DBG_MED, "dbg_output_allowed: %s", booltostr(dbg_output_allowed));
    fdbg(stderr, DBG_MED, "warn_output_allowed: %s", booltostr(warn_output_allowed));
    fdbg(stderr, DBG_MED, "err_output_allowed: %s", booltostr(err_output_allowed));
    fdbg(stderr, DBG_MED, "usage_output_allowed: %s", booltostr(usage_output_allowed));
    fdbg(stderr, DBG_MED, "msg_warn_silent: %s", booltostr(msg_warn_silent));
    fdbg(stderr, DBG_MED, "msg() output: %s", msg_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "dbg(DBG_MED) output: %s", dbg_allowed(DBG_MED) ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "warn() output: %s", warn_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "err() output: %s", err_allowed() ? "allowed" : "silenced");
    fdbg(stderr, DBG_MED, "usage() output: %s", usage_allowed() ? "allowed" : "silenced");


    errno = 0; /* pre-clear errno for errp() */
    buf = calloc_path(dirname, filename);
    if (buf == NULL) {
	errp(172, __func__, "calloc_path(%s, %s) returned NULL", dirname, filename);
	not_reached();
    } else if (strcmp(buf, "foo/bar") != 0) {
	err(173, __func__, "buf: %s != %s/%s", buf, dirname, filename);
	not_reached();
    } else {
	fdbg(stderr, DBG_MED, "calloc_path(%s, %s): returned %s", dirname, filename, buf);
    }

    /*
     * free buf
     */
    if (buf != NULL) {
	free(buf);
	buf = NULL;
    }

    /*
     * try calloc_path() again but without directory
     */
    dirname = NULL;
    errno = 0; /* pre-clear errno for errp() */
    buf = calloc_path(dirname, filename);
    if (buf == NULL) {
	errp(174, __func__, "calloc_path(NULL, %s) returned NULL", filename);
	not_reached();
    } else if (strcmp(buf, "bar") != 0) {
	err(175, __func__, "buf: %s != %s", buf, filename);
	not_reached();
    } else {
	fdbg(stderr, DBG_MED, "calloc_path(NULL, %s): returned %s", filename, buf);
    }

    if (buf != NULL) {
	free(buf);
	buf = NULL;
    }

    /*
     * now try parsing jparse.json as a JSON file
     */
    dirname = ".";
    filename = "jparse.json";
    tree = open_json_dir_file(dirname, filename);
    if (tree == NULL) {
        err(10, __func__, "jparse.json is invalid JSON"); /*ooo*/
        not_reached();
    } else {
        dbg(DBG_LOW, "jparse.json is valid JSON");
        json_tree_free(tree, JSON_DEFAULT_MAX_DEPTH);
        free(tree);
        tree = NULL;
    }

    /*
     * test the first relative path that we know is good
     */
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_OK) {
        err(176, __func__, "sane_relative_path(\"%s\", 99, 25, 4): expected PATH_OK, got: %s",
                relpath, path_sanity_name(sanity)); /*coo*/
        not_reached();
    }

    /*
     * test another sane path but with components
     */
    relpath = "foo/bar";
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_OK) {
        err(177, __func__, "sane_relative_path(\"%s\", 99, 25, 4): expected PATH_OK, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test empty path
     */
    relpath = "";
    sanity = sane_relative_path(relpath, 99, 25, 2);
    if (sanity != PATH_ERR_PATH_EMPTY) {
        err(178, __func__, "sane_relative_path(\"%s\", 99, 25, 2): expected PATH_ERR_PATH_EMPTY, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test path too long (path length > max_path_len)
     */
    relpath = "foo/bar/baz";
    sanity =sane_relative_path(relpath, 2, 99, 2);
    if (sanity != PATH_ERR_PATH_TOO_LONG) {
        err(179, __func__, "sane_relative_path(\"%s\", 2, 25, 2): expected PATH_ERR_PATH_TOO_LONG, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test max path len <= 0
     */
    relpath = "foo/bar/baz";
    sanity =sane_relative_path(relpath, 0, 25, 2);
    if (sanity != PATH_ERR_MAX_PATH_LEN_0) {
        err(180, __func__, "sane_relative_path(\"%s\", 0, 25, 2): expected PATH_ERR_MAX_PATH_LEN_0, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test max depth <= 0
     */
    relpath = "foo/bar/baz";
    sanity = sane_relative_path(relpath, 99, 25, 0);
    if (sanity != PATH_ERR_MAX_DEPTH_0) {
        err(181, __func__, "sane_relative_path(\"%s\", 99, 25, 0, &sanity): expected PATH_ERR_MAX_DEPTH_0, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test path starting with '/' (not relative path)
     */
    relpath = "/foo";
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_ERR_NOT_RELATIVE) {
        err(182, __func__, "sane_relative_path(\"%s\", 99, 25, 4): expected PATH_ERR_NOT_RELATIVE, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test path with filename too long
     */
    relpath = "aequeosalinocalcalinoceraceoaluminosocupreovitriolic"; /* 52 letter word recognised by some */
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_ERR_NAME_TOO_LONG) {
        err(183, __func__, "sane_relative_path(\"%s\", 99, 25, 4): expected PATH_ERR_NAME_TOO_LONG, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test max name length <= 0
     */
    relpath = "foo";
    sanity = sane_relative_path(relpath, 99, 0, 2);
    if (sanity != PATH_ERR_MAX_NAME_LEN_0) {
        err(184, __func__, "sane_relative_path(\"%s\", 99, 0, 2): expected PATH_ERR_MAX_NAME_LEN_0, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test a path that's too deep (depth)
     */
    relpath = "foo/bar";
    sanity = sane_relative_path(relpath, 99, 25, 1);
    if (sanity != PATH_ERR_PATH_TOO_DEEP) {
        err(185, __func__, "sane_relative_path(\"%s\", 99, 25, 1): expected PATH_ERR_PATH_TOO_DEEP, got: %s",
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test invalid path component
     */
    relpath = "foo/../";
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_ERR_NOT_POSIX_SAFE) {
        err(186, __func__, "%s(\"%s\", 99, 25, 4): expected PATH_ERR_NOT_POSIX_SAFE, got: %s", __func__,
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test another invalid path component
     */
    relpath = "foo/./";
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_ERR_NOT_POSIX_SAFE) {
        err(187, __func__, "%s(\"%s\", 99, 25, 4): expected PATH_ERR_NOT_POSIX_SAFE, got: %s", __func__,
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test another invalid path component
     */
    relpath = "./foo/";
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_ERR_NOT_POSIX_SAFE) {
        err(188, __func__, "%s(\"%s\", 99, 25, 4): expected PATH_ERR_NOT_POSIX_SAFE, got: %s", __func__,
                relpath, path_sanity_name(sanity));
        not_reached();
    }

    /*
     * test path with number in it
     */
    relpath = "foo1";
    sanity = sane_relative_path(relpath, 99, 25, 4);
    if (sanity != PATH_OK) {
        err(189, __func__, "%s(\"%s\", 99, 25, 4): expected PATH_OK, got: %s", __func__,
                relpath, path_sanity_name(sanity));
        not_reached();
    }

}
#endif
