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
 * util - common utility functions for the JSON parser
 */
#include "util.h"


/*
 * static declarations
 */


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
	err(103, __func__, "given NULL str");
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
