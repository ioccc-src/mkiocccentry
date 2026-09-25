/*
 * pr_test - test the stdio helper library
 *
 * "Small acts of kindness can fill the world with light."
 *
 *      -- J.R.R. Tolkien
 *
 * Copyright (c) 2008-2026 by Landon Curt Noll and Cody Boone Ferguson.
 * All rights reserved.
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
 * Cody Boone Ferguson added to and improved this code base:
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
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * dyn_array_test - test the dynamic array facility
 */
#include "pr.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define PR_TEST_BASENAME "pr_test"
#define PR_TEST_VERSION "1.2.0 2026-09-25"


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\ttest suite failed\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    " >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "pr library version: %s\n";


/*
 * forward declarations
 */
static void usage(int exitcode, char const *prog, char const *str);
static long fd_limit(void);
static bool fd_is_open(int fd);
static bool snapshot_open_fds(bool *open_fds, size_t fd_count);
static bool test_readline_dup_strip(void);
static bool test_read_all_chunk_terminated(void);
static bool test_open_dir_file_no_fd_leak(void);


/*
 * fd_limit - return the process file descriptor limit used by tests
 */
static long
fd_limit(void)
{
    long max_fd = 0;

    max_fd = sysconf(_SC_OPEN_MAX);
    if (max_fd <= 0) {
	max_fd = 1024;
    } else if (max_fd > 256) {
	max_fd = 256;
    }
    return max_fd;
}


/*
 * fd_is_open - determine if a file descriptor is open
 */
static bool
fd_is_open(int fd)
{
    errno = 0;
    return fcntl(fd, F_GETFD) >= 0 || errno != EBADF;
}


/*
 * snapshot_open_fds - record which file descriptors are currently open
 */
static bool
snapshot_open_fds(bool *open_fds, size_t fd_count)
{
    size_t i;

    if (open_fds == NULL) {
	return false;
    }
    for (i = 0; i < fd_count; ++i) {
	open_fds[i] = fd_is_open((int)i);
    }
    return true;
}


/*
 * test_readline_dup_strip - verify readline_dup strips trailing whitespace
 */
static bool
test_readline_dup_strip(void)
{
    FILE *stream = NULL;
    char *linep = NULL;
    char *dup = NULL;
    size_t len = 0;
    bool success = false;

    stream = tmpfile();
    if (stream == NULL) {
	warnp(__func__, "tmpfile failed");
	return false;
    }
    if (fputs("abc \t\n", stream) == EOF) {
	warnp(__func__, "fputs failed");
    } else if (fflush(stream) == EOF) {
	warnp(__func__, "fflush failed");
    } else {
	rewind(stream);
	dup = readline_dup(&linep, true, &len, stream);
	if (dup == NULL) {
	    warn(__func__, "readline_dup returned NULL");
	} else if (len != 3) {
	    warn(__func__, "readline_dup returned length: %zu != 3", len);
	} else if (strcmp(dup, "abc") != 0) {
	    warn(__func__, "readline_dup returned <%s> != <abc>", dup);
	} else {
	    success = true;
	}
    }

    free(dup);
    free(linep);
    clearerr_or_fclose(stream);
    return success;
}


/*
 * test_read_all_chunk_terminated - verify read_all keeps an extra NUL byte
 */
static bool
test_read_all_chunk_terminated(void)
{
    FILE *stream = NULL;
    unsigned char *data = NULL;
    size_t len = 0;
    size_t written = 0;
    char chunk[4096];
    bool success = false;

    memset(chunk, 'A', sizeof(chunk));
    stream = tmpfile();
    if (stream == NULL) {
	warnp(__func__, "tmpfile failed");
	return false;
    }

    while (written < READ_ALL_CHUNK) {
	size_t to_write = READ_ALL_CHUNK - written;

	if (to_write > sizeof(chunk)) {
	    to_write = sizeof(chunk);
	}
	if (fwrite(chunk, 1, to_write, stream) != to_write) {
	    warnp(__func__, "fwrite failed after %zu bytes", written);
	    clearerr_or_fclose(stream);
	    return false;
	}
	written += to_write;
    }
    if (fflush(stream) == EOF) {
	warnp(__func__, "fflush failed");
    } else if (fseek(stream, 0L, SEEK_SET) != 0) {
	warnp(__func__, "fseek failed");
    } else {
	data = read_all(stream, &len);
	if (data == NULL) {
	    warn(__func__, "read_all returned NULL");
	} else if (len != READ_ALL_CHUNK) {
	    warn(__func__, "read_all length: %zu != %d", len, READ_ALL_CHUNK);
	} else if (data[0] != 'A' || data[len - 1] != 'A') {
	    warn(__func__, "read_all data did not preserve written content");
	} else if (data[len] != '\0') {
	    warn(__func__, "read_all buffer is not NUL terminated at offset %zu", len);
	} else {
	    success = true;
	}
    }

    free(data);
    clearerr_or_fclose(stream);
    return success;
}


/*
 * test_open_dir_file_no_fd_leak - verify open_dir_file(NULL, ...) closes temp cwd state
 */
static bool
test_open_dir_file_no_fd_leak(void)
{
    char path[] = "/tmp/pr_test_open_dir_file.XXXXXX";
    int fd = -1;
    long max_fd = 0;
    bool *before = NULL;
    bool *after = NULL;
    FILE *stream = NULL;
    bool success = false;
    long i;

    fd = mkstemp(path);
    if (fd < 0) {
	warnp(__func__, "mkstemp failed");
	return false;
    }
    if (write(fd, "data\n", 5) != 5) {
	warnp(__func__, "write failed");
	(void)close(fd);
	(void)unlink(path);
	return false;
    }
    if (close(fd) != 0) {
	warnp(__func__, "close failed");
	(void)unlink(path);
	return false;
    }

    max_fd = fd_limit();
    before = calloc((size_t)max_fd, sizeof(*before));
    after = calloc((size_t)max_fd, sizeof(*after));
    if (before == NULL || after == NULL) {
	warnp(__func__, "calloc failed");
	free(before);
	free(after);
	(void)unlink(path);
	return false;
    }
    if (snapshot_open_fds(before, (size_t)max_fd) == false) {
	warn(__func__, "snapshot_open_fds failed before open_dir_file");
	free(before);
	free(after);
	(void)unlink(path);
	return false;
    }

    stream = open_dir_file(NULL, path);
    if (stream == NULL) {
	warn(__func__, "open_dir_file returned NULL");
    }
    if (stream != NULL) {
	clearerr_or_fclose(stream);
    }
    if (snapshot_open_fds(after, (size_t)max_fd) == false) {
	warn(__func__, "snapshot_open_fds failed after close");
    } else {
	for (i = 0; i < max_fd; ++i) {
	    if (after[i] && before[i] == false) {
		warn(__func__, "open_dir_file(NULL, ...) left leaked fd open after close: %ld", i);
		break;
	    }
	}
	if (i >= max_fd) {
	    success = true;
	}
    }

    free(before);
    free(after);
    (void)unlink(path);
    return success;
}


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    bool error = false;		/* true ==> test error found */
    bool opt_error = false;	/* fchk_inval_opt() return */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:V")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, ""); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    (void) printf("%s version: %s\n", PR_TEST_BASENAME, PR_TEST_VERSION);
	    (void) printf("libpr version: %s\n", pr_version);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, program, ""); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, __func__, "getopt() return: %c optopt: %c", (char)i, (char)optopt);
	    }
	    break;
	}
    }
    if (argc - optind != REQUIRED_ARGS) {
	usage(3, program, "wrong number of arguments"); /*ooo*/
	not_reached();
    }

    if (test_readline_dup_strip() == false) {
	error = true;
    }
    if (test_read_all_chunk_terminated() == false) {
	error = true;
    }
    if (test_open_dir_file_no_fd_leak() == false) {
	error = true;
    }

    /*
     * exit based on the test result
     */
    if (error == true) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "missing required argument(s), program: %s");
 *
 * given:
 *	exitcode        value to exit with
 *	prog		our program name
 *	str		top level usage message
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT,
						     PR_TEST_BASENAME, PR_TEST_VERSION,
						     dyn_array_version);
    exit(exitcode); /*ooo*/
    not_reached();
}
