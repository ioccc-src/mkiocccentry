/*
 * pr - stdio helper library
 *
 * "Small acts of kindness can fill the world with light."
 *
 *	-- J.R.R. Tolkien
 *
 * Copyright (c) 2008-2025 by Landon Curt Noll and Cody Boone Ferguson.  All rights reserved.
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
 * The code in this from repo was copied out of the mkiocccentry toolkit repo:
 *
 *	https://github.com/ioccc-src/mkiocccentry
 *
 * and out of the jparse repo:
 *
 *	https://github.com/xexyl/jparse
 *
 * The origin of libpr dates back to code written by Landon Curt Noll around 2008.
 *
 * That 2008 code was copied into the jparse repo, and the mkiocccentry toolkit repo
 * by Landon Curt Noll.  While in the jparse repo, both Landon Curt Noll and
 * Cody Boone Ferguson added to and improved this code base.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


/*
 * pr - stdio helper library
 */
#include "pr.h"


/*
 * global message control variables
 */
const char *const pr_version = PR_VERSION;    /* library version format: major.minor YYYY-MM-DD */


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
    dbg(DBG_VVVHIGH, "read %zd bytes + newline into %zu byte buffer", ret, linecap);

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
    dbg(DBG_VVHIGH, "readline returned %zd bytes", len);

    /*
     * duplicate the line
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = calloc((size_t)len+1+1, sizeof(*ret));
    if (ret == NULL) {
	errp(100, __func__, "calloc of read line of %zd bytes failed", len+1+1);
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
	dbg(DBG_VVHIGH, "readline, after trailing whitespace stripped, is %zd bytes", len);
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
	dbg(DBG_VVHIGH, "%s: fread(read_buf, %zu, %d, stream) read cycle: %ld returned: %zd",
			 __func__, sizeof(uint8_t), READ_ALL_CHUNK, read_cycle, last_read);
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
		dbg(DBG_HIGH, "fread returned: %zu normal EOF reading stream at: %jd bytes",
			      last_read, used);
	    } else if (ferror(stream)) {
		warnp(__func__, "fread returned: %zu I/O error detected while reading stream at: %jd bytes",
			        last_read, used);
	    } else {
		warnp(__func__, "fread returned %jd although neither the EOF nor ERROR flag were set: "
				"assuming EOF anyway", last_read);
	    }

	    /*
	     * stop reading stream
	     */
	    break;
	}
    } while (true);
    dbg(DBG_VVHIGH, "%s(stream, psize): last_read: %jd total bytes: %jd allocated: %jd "
		    "read_cycle: %ld move_cycle: %ld seek_cycle: %ld",
		    __func__, last_read, dyn_array_tell(array), dyn_array_alloced(array),
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
    struct stat dbuf;		/* dir status */
    struct stat fbuf;		/* file status */
    FILE *ret_stream = NULL;	/* open file stream to return */
    int fd;			/* ret_stream as a file descriptor */
    int ret = 0;		/* libc function return */
    int cwd = -1;		/* current working directory */

    /*
     * firewall
     */
    if (file == NULL) {
	err(102, __func__, "called with NULL file");
	not_reached();
    }

    /*
     * note the current directory so we can restore it later, after the
     * chdir(dir) below
     */
    errno = 0;                  /* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
        errp(103, __func__, "cannot open .");
        not_reached();
    }

    /*
     * Temporarily chdir if dir is non-NULL
     */
    if (dir != NULL && cwd >= 0) {

	/*
	 * check if we can search / work within the directory
	 */
	errno = 0;
	ret = stat(dir, &dbuf);
	if (ret < 0) {
	    err(104, __func__, "directory does not exist: %s", dir);
	    not_reached();
	}
	if (!S_ISDIR(dbuf.st_mode)) {
	    err(105, __func__, "is not a directory: %s", dir);
	    not_reached();
	}
	ret = access(dir, X_OK);
	if (ret < 0) {
	    err(106, __func__, "directory is not searchable: %s", dir);
	    not_reached();
	}

	/*
	 * chdir to to the directory
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = chdir(dir);
	if (ret < 0) {
	    errp(107, __func__, "cannot cd %s", dir);
	    not_reached();
	}
    }

    /*
     * open the file
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret_stream = fopen(file, "r");
    if (ret_stream == NULL) {
	errp(108, __func__, "cannot open file: %s", file);
	not_reached();
    }
    fd = fileno(ret_stream);
    if (fd < 0) {
	errp(109, __func__, "cannot determine fileno for open file: %s", file);
	not_reached();
    }

    /*
     * must be a readable file
     */
    errno = 0;
    ret = fstat(fd, &fbuf);
    if (ret < 0) {
	errp(110, __func__, "file does not exist: %s", file);
	not_reached();
    }
    if (!S_ISREG(fbuf.st_mode)) {
	err(111, __func__, "file is not a regular file: %s", file);
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
	    errp(112, __func__, "cannot fchdir to the previous current directory");
	    not_reached();
	}
	errno = 0;                  /* pre-clear errno for errp() */
	ret = close(cwd);
	if (ret < 0) {
	    errp(113, __func__, "close of previous current directory failed");
	    not_reached();
	}
    }

    /*
     * return open stream
     */
    return ret_stream;
}


/*
 * fchk_inval_opt - check for option error in getopt()
 *
 * given:
 *
 *	stream	    - open stream to write to, or NULL ==> just return length
 *      prog        - program name
 *      ch          - value returned by getopt()
 *      opt         - program's optopt (option triggering the error)
 *
 * return:
 *	true ==> opt is : or ?, or stream is NULL, or prog is NULL,
 *		 caller should call usage() as needed and exit as needed
 *	false ==> no issue detected, nothing printed
 *
 * NOTE:    If prog is NULL we warn and then set to ((NULL prog)).
 *
 * NOTE:    This function should only be called if getopt() returns a ':' or a
 *          '?' but if anything else is passed to this function we do nothing.
 *
 * NOTE:    This function does not call a call usage() because that is
 *	    specific to each tool.
 *
 * NOTE:    This function does NOT take an exit code because it is the caller's
 *          responsibility to do this. This is because they must call usage()
 *          which is specific to each tool.
 */
bool
fchk_inval_opt(FILE *stream, char const *prog, int ch, int opt)
{
    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "stream is NULL");
	return true;
    }
    if (prog == NULL) {
	warn(__func__, "prog is NULL");
	return true;
    }

    /*
     * unless value returned by getopt() is : (colon) or ? (question mark), nothing to do
     */
    if (ch != ':' && ch != '?') {
        return false;
    }

    /*
     * report to stderr, based on the value returned by getopt
     */
    switch (ch) {
        case ':':
            fprintf(stream, "%s: requires an argument -- %c\n\n", prog, (char)opt);
            break;
        case '?':
            fprintf(stream, "%s: illegal option -- %c\n\n", prog, (char)opt);
            break;
        default: /* should never be reached but we include it anyway */
            fprintf(stream, "%s: unexpected getopt() return value: 0x%02x == <%c>\n\n", prog, ch, (char)ch);
            break;
    }
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
 *			            return EXIT_CALLOC_FAILED calloc() failure,
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
		    errp(114, name, "fflush(stdin): error code: %d", ret);
		    not_reached();
		} else {
		    dbg(DBG_HIGH, "%s: called via %s: fflush(stdin) failed: %s", __func__, name, strerror(errno));
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
		errp(115, name, "fflush(stdout): error code: %d", ret);
		not_reached();
	    } else {
		dbg(DBG_HIGH, "%s: called from %s: fflush(stdout) failed: %s", __func__, name, strerror(errno));
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
		errp(116, name, "fflush(stderr): error code: %d", ret);
		not_reached();
	    } else {
		dbg(DBG_HIGH, "%s: called from %s: fflush(stderr) failed: %s", __func__, name, strerror(errno));
		return;
	    }
	}
    } else {
	dbg(DBG_VHIGH, "%s: called via %s: stderr is NOT open, flush request ignored", __func__, name);
    }
    return;
}
