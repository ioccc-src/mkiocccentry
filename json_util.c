/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_util - general JSON parser utility support functions
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * and
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * This is very much a work in progress!
 */


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

/*
 * json_util - #includes what we need
 */
#include "json_util.h"


/*
 * static declarations
 */
static void alloc_json_code_ignore_set(void);
static int cmp_codes(const void *a, const void *b);
static void expand_json_code_ignore_set(void);
static struct ignore_json_code *ignore_json_code_set;
static bool json_process_decimal(struct json_number *item, char const *str, size_t len);
static bool json_process_floating(struct json_number *item, char const *str, size_t len);


/*
 * globals
 */

int json_verbosity_level = JSON_DBG_NONE;	/* json debug level set by -J in jparse */

/*
 * JSON warn (NOT error) codes to ignore
 *
 * When a tool is given command line arguments of the form:
 *
 *	.. -W 123 -W 1345 -W 56 ...
 *
 * this means the tool will ignore {JSON-0123}, {JSON-1345}, and {JSON-0056}.
 * The code_ignore_settable holds the JSON codes to ignore.
 *
 * NOTE: A NULL ignore_json_code_set means that the set has not been setup.
 */
static struct ignore_json_code *ignore_json_code_set = NULL;


/*
 * jwarn - issue a JSON warning message
 *
 * given:
 *	code	    warning code
 *	program	    name of program e.g. jinfochk, jauthchk etc.
 *	name	    name of function issuing the warning
 *	filename    file with the problem (can be stdin)
 *	line	    JSON line
 *	line_num    the offending line number in the json file
 *	fmt	    format of the warning
 *	...	    optional format args
 *
 * Example:
 *
 *	jwarn(JSON_CODE(1), program, __func__, file, line, __LINE__, "unexpected foobar: %d", value);
 *
 * XXX As of 13 March 2022 the line will be empty but in time this should be
 * changed to be the offending JSON text and the offending JSON line number
 * (which currently is done but the way it's done might change). Instead the
 * field and values are usually shown as part of the optional format args.
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function does not return on code < JSON_CODE_RESERVED_MIN (0).
 */
void
jwarn(int code, char const *program, char const *name, char const *filename, char const *line, int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    if (is_json_code_ignored(code))
	return;

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (program == NULL) {
	program = "((NULL program))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL program, forcing name: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (code < JSON_CODE_RESERVED_MIN) {
	err(198, __func__, "invalid JSON code passed to jwarn(): %d", code);
	not_reached();
    }
    if (line == NULL) {
	/* currently line will be NULL so we make it empty */
	line = "";
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jwarn(): called with NULL filename, forcing filename: %s\n", filename);
    }


    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "{JSON-%04d}: %s: %d: ", code, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * jwarnp - issue a JSON warning message with errno information
 *
 * given:
 *	code	    warning code
 *	program	    name of program e.g. jinfochk, jauthchk etc.
 *	name	    name of function issuing the warning
 *	filename    file with the problem (can be stdin)
 *	line	    JSON line
 *	line_num    the offending line number in the json file
 *	fmt	    format of the warning
 *	...	    optional format args
 *
 * Example:
 *
 *	jwarn(JSON_CODE(1), program, __func__, file, line, __LINE__, "unexpected foobar: %d", value);
 *
 * XXX As of 13 March 2022 the line will be empty but in time this should be
 * changed to be the offending JSON text and the offending JSON line number
 * (which currently is done but the way it's done might change). Instead the
 * field and values are usually shown as part of the optional format args.
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function does not return on code < JSON_CODE_RESERVED_MIN (0).
 */
void
jwarnp(int code, char const *program, char const *name, char const *filename, char const *line,
       int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    if (is_json_code_ignored(code))
	return;

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (program == NULL) {
	program = "((NULL program))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL program, forcing name: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (code < JSON_CODE_RESERVED_MIN) {
	err(199, __func__, "invalid JSON code passed to jwarn(): %d", code);
	not_reached();
    }
    if (line == NULL) {
	/* currently line will be NULL so we make it empty */
	line = "";
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jwarn(): called with NULL filename, forcing filename: %s\n", filename);
    }


    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "{JSON-%04d}: %s: %d: ", code, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf with errno returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * jerr - issue a fatal JSON error message and exit
 *
 * given:
 *	exitcode	value to exit with
 *	program		program or NULL (will be set to "jchk")
 *	name		name of function issuing the error
 *	filename	file with the problem (can be stdin)
 *	line		line with the problem (or NULL)
 *	line_num	line number with the problem or -1
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	jerr(JSON_CODE(1), __func__, program, file, line, line_num, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function does not return.
 */
void
jerr(int exitcode, char const *program, char const *name, char const *filename, char const *line,
     int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin jerr(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin jerr(): forcing exit code: %d\n", exitcode);
    }
    if (program == NULL) {
	program = "jchk";
	dbg(DBG_VHIGH, "\nin jerr(): called with NULL program, forcing program: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin jerr(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin jerr(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jerr(): called with NULL filename, forcing filename: %s\n", filename);
    }
    if (line == NULL) {
	line = "";
	dbg(DBG_VHIGH, "jerr(): called with NULL line, making \"\"");
    }

    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "JSON[%04d]: %s: %d: ", exitcode, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "vfprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * jerrp - issue a fatal error message with errno information and exit
 *
 * given:
 *	exitcode	value to exit with
 *	program		program or NULL (will be set to "jchk")
 *	name		name of function issuing the warning
 *	filename	file with the problem (can be stdin)
 *	line		line with the problem or NULL
 *	line_num	line number of the problem or -1
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	jerrp(JSON_CODE(1), program, __func__, file, line, line_num, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * NOTE: In some cases the file noted is the source file that raised the issue.
 *
 * This function does not return.
 */
void
jerrp(int exitcode, char const *program, char const *name, char const *filename, char const *line,
      int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
    int saved_errno;	/* errno value when called */

    /*
     * save errno in case we need it for strerror()
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__func__, "\nin jerrp(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin jerrp(): forcing exit code: %d\n", exitcode);
    }
    if (program == NULL) {
	program = "jchk";
	dbg(DBG_VHIGH, "\nin jerrp(): called with NULL program, forcing program: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin jerrp(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin jerrp(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jerrp(): called with NULL filename, forcing filename: %s\n", filename);
    }
    if (line == NULL) {
	line = "";
	dbg(DBG_VHIGH, "jerrp(): called with NULL line, making \"\"");
    }


    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "JSON[%04d]: %s: %d: ", exitcode, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "vfprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * alloc_json_code_ignore_set - allocate the initial JSON ignore code set
 *
 * This function will setup the ignore_json_code_set table.  If the ignore_json_code_set
 * table is already setup, this function will do nothing.
 *
 * NOTE: This function does not return on error.
 */
static void
alloc_json_code_ignore_set(void)
{
    struct ignore_json_code *tbl = NULL;	/* allocated struct ignore_json_code */
    int i;

    /*
     * firewall - do nothing if already setup
     */
    if (ignore_json_code_set != NULL) {
	dbg(DBG_VVHIGH, "ignore_json_code_set already set up");
	return;
    }

    /*
     * allocate the initial ignore_json_code_set[]
     */
    errno = 0;			/* pre-clear errno for errp() */
    tbl = malloc(sizeof(struct ignore_json_code));
    if (tbl == NULL) {
	errp(200, __func__, "failed to malloc struct ignore_json_code");
	not_reached();
    }

    /*
     * allocate an initial block of ignore codes
     */
    errno = 0;			/* pre-clear errno for errp() */
    tbl->code = malloc((JSON_CODE_IGNORE_CHUNK+1+1) * sizeof(int));
    if (tbl->code == NULL) {
	errp(201, __func__, "cannot allocate %d ignore codes", JSON_CODE_IGNORE_CHUNK+1+1);
	not_reached();
    }

    /*
     * initialize
     */
    tbl->next_free = 0;
    tbl->alloc = JSON_CODE_IGNORE_CHUNK + 1;		/* report one less for the guard code */
    for (i=0; i < tbl->alloc; ++i) {
	tbl->code[i] = -1;	/* -1 is not a valid ignore code */
    }
    ignore_json_code_set = tbl;
}


/*
 * cmp_codes - compare two codes for reverse sorting ignore_json_code_set[]
 *
 * This function will allow qsort() to reverse sort the codes in ignore_json_code_set[].
 *
 * given:
 *	a	- pointer to first code to compare
 *	b	- pointer to second code to compare
 *
 * returns:
 *	-1	a > b
 *	0	a == b
 *	1	a < b
 */
static int
cmp_codes(const void *a, const void *b)
{
    /*
     * firewall
     */
    if (a == NULL || b == NULL) {
	err(202, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * compare for reverse sort
     */
    if (*(int *)a < *(int *)b) {
	return 1;	/* a < b */
    } else if (*(int *)a > *(int *)b) {
	return -1;	/* a > b */
    }
    return 0;	/* a == b */
}


/*
 * expand_json_code_ignore_set - expand the size of alloc json_code_ignore_set[]
 *
 * This function will expand the allocated  json_code_ignore_set[] by
 * JSON_CODE_IGNORE_CHUNK JSON error codes, and set those new codes (in the end
 * of the table) to -1 to indicate that they are not valid codes.
 *
 * NOTE: This function does not return on error.
 */
static void
expand_json_code_ignore_set(void)
{
    int *p;	/* reallocated code set */
    int i;

    /*
     * setup ignore_json_code_set if needed
     */
    alloc_json_code_ignore_set();
    if (ignore_json_code_set == NULL) {
	err(203, __func__, "ignore_json_code_set is NULL after allocation");
	not_reached();
    }

    /*
     * if no room, expand the table
     */
    if (ignore_json_code_set->next_free >= ignore_json_code_set->alloc) {
	p = realloc(ignore_json_code_set->code, (ignore_json_code_set->alloc+JSON_CODE_IGNORE_CHUNK+1) * sizeof(int));
	errno = 0;			/* pre-clear errno for errp() */
	if (p == NULL) {
	    errp(204, __func__, "cannot expand ignore_json_code_set from %d to %d codes",
				ignore_json_code_set->alloc+1, ignore_json_code_set->alloc+JSON_CODE_IGNORE_CHUNK+1);
	    not_reached();
	}
	for (i=ignore_json_code_set->alloc; i < ignore_json_code_set->alloc+JSON_CODE_IGNORE_CHUNK; ++i) {
	    p[i] = JSON_CODE_INVALID; /* JSON_CODE_INVALID (JSON_CODE_RESERVED_MIN - 1) is not a valid ignore code */
	}
	ignore_json_code_set->code = p;
	/* report one less for the guard code */
	ignore_json_code_set->alloc = ignore_json_code_set->alloc + JSON_CODE_IGNORE_CHUNK;
    }
    return;
}


/*
 * is_json_code_ignored - determine of a JSON warn/error code is to be ignored
 *
 * given:
 *	code	- code to test if code should be ignored
 *
 * returns:
 *	true ==> code is in ignore_json_code_set[] and should be ignored
 *	false ==> code is not in ignore_json_code_set[] and should NOT be ignored
 */
bool
is_json_code_ignored(int code)
{
    int i;

    /*
     * firewall
     */
    if (code < 0) {
	err(205, __func__, "code %d < 0", code);
	not_reached();
    }

    /*
     * setup ignore_json_code_set if needed
     */
    alloc_json_code_ignore_set();
    if (ignore_json_code_set == NULL) {
	err(206, __func__, "ignore_json_code_set is NULL after allocation");
	not_reached();
    }

    /*
     * search ignore_json_code_set[] for the code
     */
    for (i=0; i < ignore_json_code_set->next_free; ++i) {

	/* look for match */
	if (ignore_json_code_set->code[i] == code) {
	    dbg(DBG_VVVHIGH, "code %d is in ignore_json_code_set[]", code);
	    return true;	/* report code should be ignored */
	}

	/* look for going beyond sorted values */
	if (ignore_json_code_set->code[i] <= code) {
	     break;
	}
    }
    return false;	/* report code should NOT be ignored */
}


/*
 * add_ignore_json_code - add a JSON error code to be ignored
 *
 * If code >= 0 and is not in json_code_ignore_set[], then this function will
 * add it and then reverse sort the json_code_ignore_set[] table.
 *
 * NOTE: The json_code_ignore_set[] will be initialized or expanded as needed.
 *
 * given:
 *	code	- code to ignore
 *
 * NOTE: This function does not return on error.
 */
void
ignore_json_code(int code)
{
    /*
     * firewall
     */
    if (code < 0) {
	err(207, __func__, "code %d < 0", code);
	not_reached();
    }

    /*
     * allocate or expand the json_code_ignore_set[] if needed
     */
    if (ignore_json_code_set == NULL || ignore_json_code_set->next_free >= ignore_json_code_set->alloc) {
	expand_json_code_ignore_set();
    }
    if (ignore_json_code_set == NULL) {
	err(208, __func__, "ignore_json_code_set is NULL after allocation or expansion");
	not_reached();
    }

    /*
     * verify that code is not already in json_code_ignore_set[]
     */
    if (is_json_code_ignored(code)) {
	/* nothing to do it code is already added */
	dbg(DBG_VVVHIGH, "code %d is already in ignore_json_code_set[]", code);
	return;
    }

    /*
     * add the code to the json_code_ignore_set[]
     */
    ignore_json_code_set->code[ignore_json_code_set->next_free] = code;
    ++ignore_json_code_set->next_free;

    /*
     * reverse sort the json_code_ignore_set[]
     */
    qsort(ignore_json_code_set->code, ignore_json_code_set->next_free, sizeof(int), cmp_codes);
    dbg(DBG_VHIGH, "code %d added to ignore_json_code_set[]", code);
    return;
}

/*
 * json_dbg - print JSON debug message if we are verbose enough
 *
 * given:
 *	level	    print message if >= verbosity level
 *	program	    program name
 *	name	    function name
 *	fmt	    printf format
 *	...
 *
 * Example:
 *
 *	json_dbg(1, __func__, "foobar information: %d", value);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
json_dbg(int level, char const *name, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int saved_errno;	/* errno at function start */

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin json_dbg(%d, ...): NULL name, forcing use of: %s\n", level, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin json_dbg(%d, ...): NULL fmt, forcing use of: %s\n", level, fmt);
    }

    /*
     * print the debug message if allowed and allowed by the verbosity level
     */
    json_vdbg(level, name, fmt, ap);

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * json_vdbg - print debug message if we are verbose enough
 *
 * given:
 *	level	    print message if >= verbosity level
 *	name	    function name
 *	ap	    va_list
 *
 * Example:
 *
 *	json_vdbg(1, __func__, "foobar information: %d", ap);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
void
json_vdbg(int level, char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * firewall
     */
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin json_vdbg(%d, ...): NULL name, forcing use of: %s\n", level, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin json_vdbg(%d, ...): NULL fmt, forcing use of: %s\n", level, fmt);
    }

    /*
     * print the debug message if allowed and allowed by the verbosity level
     */
    if (dbg_output_allowed) {
	if (level <= json_verbosity_level) {
	    errno = 0;
	    ret = fprintf(stderr, "JSON DEBUG[%d]: ", level);
	    if (ret < 0) {
		warn(__func__, "\nin json_vdbg(%d, %s, %s ...): fprintf returned error: %s\n",
			       level, name, fmt, strerror(errno));
	    }

	    errno = 0;
	    ret = vfprintf(stderr, fmt, ap);
	    if (ret < 0) {
		warn(__func__, "\nin json_vdbg(%d, %s, %s ...): vfprintf returned error: %s\n",
			       level, name, fmt, strerror(errno));
	    }

	    errno = 0;
	    ret = fputc('\n', stderr);
	    if (ret != '\n') {
		warn(__func__, "\nin json_vdbg(%d, %s ...): fputc returned error: %s\n",
			       level, fmt, strerror(errno));
	    }

	    errno = 0;
	    ret = fflush(stderr);
	    if (ret < 0) {
		warn(__func__, "\nin json_vdbg(%d, %s, %s ...): fflush returned error: %s\n",
			       level, name, fmt, strerror(errno));
	    }
	}
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}

/*
 * json_element_type_name - print a struct json element union type by name
 *
 * given:
 *	json_type   one of the values of the JTYPE_ enum
 *
 * returns:
 *	A constant (read-only) string that names the JTYPE_ enum.
 *
 * NOTE: This string returned is read only: It's not allocated on the stack.
 */
char const *
json_element_type_name(struct json *node)
{
    char const *name = "JTYPE_UNSET";

    /*
     * firewall
     */
    if (node == NULL) {
	name = "((node == NULL))";

    /*
     * determine type name based on type
     */
    } else {
	switch (node->type) {
	case JTYPE_UNSET:
	    name = "JTYPE_UNSET";
	    break;
	case JTYPE_NUMBER:
	    name = "JTYPE_NUMBER";
	    break;
	case JTYPE_STRING:
	    name = "JTYPE_STRING";
	    break;
	case JTYPE_BOOL:
	    name = "JTYPE_BOOL";
	    break;
	case JTYPE_NULL:
	    name = "JTYPE_NULL";
	    break;
	case JTYPE_MEMBER:
	    name = "JTYPE_MEMBER";
	    break;
	case JTYPE_OBJECT:
	    name = "JTYPE_OBJECT";
	    break;
	case JTYPE_ARRAY:
	    name = "JTYPE_ARRAY";
	    break;
	default:
	    name = "((JTYPE_UNKNOWN))";
	    warn(__func__, "called with unknown JSON type: %d", node->type);
	    break;
	}
    }

    /* return read-only name constant string */
    return name;
}


/*
 * json_free - free storage of a single JSON parse tree node
 *
 * This function operates on a single JSON parse tree node.
 * See json_tree_free() for a function that frees an entire parse tree.
 *
 * given:
 *	node	pointer to a JSON parser tree node to free
 *
 * NOTE: This function will free the internals of a JSON parser tree node.
 *	 It is up to the caller to free the struct json if needed.
 *
 * NOTE: This function does NOT walk the JSON parse tree, so it will
 *	 ignore links form this node to other JSON parse tree nodes.
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 */
void
json_free(struct json *node)
{
    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * free internals based in node type
     */
    switch (node->type) {

    case JTYPE_UNSET:	/* JSON element has not been set - must be the value 0 */
	/* nothing to free */

	/* nothing internal to zeroize */
	break;

    case JTYPE_NUMBER:	/* JSON element is number - see struct json_number */
	{
	    struct json_number *item = &(node->element.number);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }

	    /* zeroize internal element storage */
	    memset(item, 0, sizeof(struct json_number));
	    item->converted = false;
	}
	break;

    case JTYPE_STRING:	/* JSON element is a string - see struct json_string */
	{
	    struct json_string *item = &(node->element.string);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }
	    if (item->str != NULL) {
		free(item->str);
		item->str = NULL;
	    }

	    /* zeroize internal element storage */
	    memset(item, 0, sizeof(struct json_string));
	    item->converted = false;
	}
	break;

    case JTYPE_BOOL:	/* JSON element is a boolean - see struct json_boolean */
	{
	    struct json_boolean *item = &(node->element.boolean);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }

	    /* zeroize internal element storage */
	    memset(item, 0, sizeof(struct json_boolean));
	    item->converted = false;
	}
	break;

    case JTYPE_NULL:	/* JSON element is a null - see struct json_null */
	{
	    struct json_null *item = &(node->element.null);

	    /* free internal storage */
	    if (item->as_str != NULL) {
		free(item->as_str);
		item->as_str = NULL;
	    }

	    /* zeroize internal element storage */
	    memset(item, 0, sizeof(struct json_null));
	    item->converted = false;
	}
	break;

    case JTYPE_MEMBER:	/* JSON element is a member */
	{
	    struct json_member *item = &(node->element.member);

	    /* free internal storage */
	    item->name = NULL;
	    item->value = NULL;

	    /* zeroize internal element storage */
	    memset(item, 0, sizeof(struct json_member));
	    item->converted = false;
	}
	break;

    case JTYPE_OBJECT:	/* JSON element is a { members } */
	{
	    struct json_object *item = &(node->element.object);

	    /* free internal storage */
	    if (item->s != NULL) {
		dyn_array_free(item->s);
		item->s = NULL;
		item->set = NULL;
		item->len = 0;
	    }

	    /* zeroize internal element storage */
	    memset(item, 0, sizeof(struct json_object));
	    item->converted = false;
	}
	break;

    case JTYPE_ARRAY:	/* JSON element is a [ elements ] */
	{
	    struct json_array *item = &(node->element.array);

	    /* free internal storage */
	    if (item->s != NULL) {
		dyn_array_free(item->s);
		item->s = NULL;
		item->set = NULL;
		item->len = 0;
	    }

	    /* zeroize internal element storage */
	    memset(item, 0, sizeof(struct json_array));
	    item->converted = false;
	}
	break;

    default:
	/* nothing we can free */
	warn(__func__, "node type is unknown: %d", node->type);
	break;
    }

    /*
     * reset the JSON node
     */
    node->type = JTYPE_UNSET;
    node->parent = NULL;
    return;
}


/*
 * json_tree_free - free storage of a JSON parse tree
 *
 * This function uses the json_tree_walk() interface to walk
 * the JSON parse tree and free all nodes under a given node.
 *
 * given:
 *	node	pointer to a JSON parser tree node to free
 *
 * NOTE: This function will free the internals of a JSON parser tree node.
 *	 It is up to the caller to free the top level struct json if needed.
 *
 * NOTE: If the pointer to allocated storage == NULL,
 *	 this function does nothing.
 *
 * NOTE: This function does nothing if node == NULL.
 *
 * NOTE: This function does nothing if the node type is invalid.
 */
void
json_tree_free(struct json *node)
{
    /*
     * firewall - nothing to do for a NULL node
     */
    if (node == NULL) {
	return;
    }

    /*
     * free the JSON parse tree
     */
    json_tree_walk(node, json_free);
    return;
}


/*
 * json_tree_walk - walk a JSON parse tree calling a function on each node
 *
 * Walk a JSON parse tree, Depth-first Post-order (LRN) order.  See:
 *
 *	https://en.wikipedia.org/wiki/Tree_traversal#Post-order,_LRN
 *
 * Example use - free an entire JSON parse tree
 *
 *	json_tree_walk(tree, json_free);
 *
 * given:
 *	node	    pointer to a JSON parse tree
 *	callback    function to operate on every node under the JSON parse tree
 *
 * NOTE: This function warns but does not do anything if an arg is NULL.
 *
 * NOTE: Although in C ALL functions are pointers which means one can call foo()
 *	 as foo() or (*foo)() we use the latter format for the callback function
 *	 to make it clearer that it is in fact a function that's passed in so
 *	 that we can use this function to do more than one thing. This is also
 *	 why we call it callback and not something else.
 *
 * XXX - make use of JSON_MAX_DEPTH and JSON_MAX_DEPTH - XXX
 */
void
json_tree_walk(struct json *node, void (*callback)(struct json *))
{
    int i;

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return ;
    }
    if (callback == NULL) {
	warn(__func__, "callback is NULL");
	return ;
    }

    /*
     * walk based on type of node
     */
    switch (node->type) {

    case JTYPE_UNSET:	/* JSON element has not been set - must be the value 0 */
    case JTYPE_NUMBER:	/* JSON element is number - see struct json_number */
    case JTYPE_STRING:	/* JSON element is a string - see struct json_string */
    case JTYPE_BOOL:	/* JSON element is a boolean - see struct json_boolean */
    case JTYPE_NULL:	/* JSON element is a null - see struct json_null */

	/* perform function operation on this terminal parse tree node */
	(*callback)(node);
	break;

    case JTYPE_MEMBER:	/* JSON element is a member */
	{
	    struct json_member *item = &(node->element.member);

	    /* perform function operation on JSON member name (left branch) node */
	    json_tree_walk(item->name, callback);

	    /* perform function operation on JSON member value (right branch) node */
	    json_tree_walk(item->value, callback);
	}

	/* finally perform function operation on the parent node */
	(*callback)(node);
	break;

    case JTYPE_OBJECT:	/* JSON element is a { members } */
	{
	    struct json_object *item = &(node->element.object);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    json_tree_walk(item->set[i], callback);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*callback)(node);
	break;

    case JTYPE_ARRAY:	/* JSON element is a [ elements ] */
	{
	    struct json_array *item = &(node->element.array);

	    /* perform function operation on each object member in order */
	    if (item->set != NULL) {
		for (i=0; i < item->len; ++i) {
		    json_tree_walk(item->set[i], callback);
		}
	    }
	}

	/* finally perform function operation on the parent node */
	(*callback)(node);
	break;

    default:
	warn(__func__, "node type is unknown: %d", node->type);
	/* nothing we can traverse */
	break;
    }
    return;
}

/*
 * json_process_decimal - process a JSON integer string
 *
 * We will fill out the struct json_number item assuming that ptr, with length len,
 * points to a JSON integer string of base 10 ASCII as allowed by the so-called JSON spec.
 *
 * given:
 *	item	pointer to a JSON number structure (struct json_number*)
 *	str	JSON integer as a NUL terminated C-style string
 *	len	length of the JSON number that is not whitespace
 *
 * NOTE: This function assumes that str points to the start of a JSON number, NOT whitespace.
 *
 * NOTE: This function assumes that str is a NUL terminated string,
 *	 even if the NUL is well beyond the end of the JSON number.
 *
 * NOTE: While it is OK if str has trailing whitespace, str[len-1] must be an
 *	 ASCII digit.  It is assumed that str[len-1] is the final JSON number
 *	 character.
 */
static bool
json_process_decimal(struct json_number *item, char const *str, size_t len)
{
    char *endptr;			/* first invalid character or str */
    size_t str_len = 0;			/* length as a C string, of str */

    /*
     * firewall
     */
    if (item == NULL) {
	warn(__func__, "called with NULL item");
	return false;	/* processing failed */
    }
    if (str == NULL) {
	warn(__func__, "called with NULL str");
	return false;	/* processing failed */
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return false;	/* processing failed */
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
     * determine if JSON integer negative
     */
    if (str[0] == '-') {

	/* parse JSON integer that is < 0 */
	item->is_negative = true;

        /*
	 * JSON spec detail: lone - invalid JSON
	 */
	if (len <= 1 || str[1] == '\0') {
	    dbg(DBG_HIGH, "%s called with just -: <%s>", __func__, str);
	    return false;	/* processing failed */

        /*
	 * JSON spec detail: leading -0 followed by digits - invalid JSON
	 */
	} else if (len > 2 && str[1] == '0' && isascii(str[2]) && isdigit(str[2])) {
	    dbg(DBG_HIGH, "%s called with -0 followed by more digits: <%s>", __func__, str);
	    return false;	/* processing failed */
	}

    /* case: JSON integer is >= 0 */
    } else {

	/* parse JSON integer that is >= 0 */
	item->is_negative = false;

        /*
	 * JSON spec detail: leading 0 followed by digits - invalid JSON
	 */
	if (len > 1 && str[0] == '0' && isascii(str[1]) && isdigit(str[1])) {
	    dbg(DBG_HIGH, "%s called with 0 followed by more digits: <%s>", __func__, str);
	    return false;	/* processing failed */
	}
    }

    /*
     * attempt to convert to the largest possible integer
     */
    if (item->is_negative) {

	/* case: negative, try for largest signed integer */
	errno = 0;			/* pre-clear errno for errp() */
	item->as_maxint = strtoimax(str, &endptr, 10);
	if (errno == ERANGE || errno == EINVAL || endptr == str || endptr == NULL) {
	    dbg(DBG_VVVHIGH, "strtoimax failed to convert");
	    item->converted = false;
	    return false;	/* processing failed */
	}
	item->converted = true;
	item->maxint_sized = true;
	dbg(DBG_VVVHIGH, "strtoimax for <%s> returned: %jd", str, item->as_maxint);

	/* case int8_t: range check */
	if (item->as_maxint >= (intmax_t)INT8_MIN && item->as_maxint <= (intmax_t)INT8_MAX) {
	    item->int8_sized = true;
	    item->as_int8 = (int8_t)item->as_maxint;
	}

	/* case uint8_t: cannot be because JSON string is < 0 */
	item->uint8_sized = false;

	/* case int16_t: range check */
	if (item->as_maxint >= (intmax_t)INT16_MIN && item->as_maxint <= (intmax_t)INT16_MAX) {
	    item->int16_sized = true;
	    item->as_int16 = (int16_t)item->as_maxint;
	}

	/* case uint16_t: cannot be because JSON string is < 0 */
	item->uint16_sized = false;

	/* case int32_t: range check */
	if (item->as_maxint >= (intmax_t)INT32_MIN && item->as_maxint <= (intmax_t)INT32_MAX) {
	    item->int32_sized = true;
	    item->as_int32 = (int32_t)item->as_maxint;
	}

	/* case uint32_t: cannot be because JSON string is < 0 */
	item->uint32_sized = false;

	/* case int64_t: range check */
	if (item->as_maxint >= (intmax_t)INT64_MIN && item->as_maxint <= (intmax_t)INT64_MAX) {
	    item->int64_sized = true;
	    item->as_int64 = (int64_t)item->as_maxint;
	}

	/* case uint64_t: cannot be because JSON string is < 0 */
	item->uint64_sized = false;

	/* case int: range check */
	if (item->as_maxint >= (intmax_t)INT_MIN && item->as_maxint <= (intmax_t)INT_MAX) {
	    item->int_sized = true;
	    item->as_int = (int)item->as_maxint;
	}

	/* case unsigned int: cannot be because JSON string is < 0 */
	item->uint_sized = false;

	/* case long: range check */
	if (item->as_maxint >= (intmax_t)LONG_MIN && item->as_maxint <= (intmax_t)LONG_MAX) {
	    item->long_sized = true;
	    item->as_long = (long)item->as_maxint;
	}

	/* case unsigned long: cannot be because JSON string is < 0 */
	item->ulong_sized = false;

	/* case long long: range check */
	if (item->as_maxint >= (intmax_t)LLONG_MIN && item->as_maxint <= (intmax_t)LLONG_MAX) {
	    item->longlong_sized = true;
	    item->as_longlong = (long long)item->as_maxint;
	}

	/* case unsigned long long: cannot be because JSON string is < 0 */
	item->ulonglong_sized = false;

	/* case size_t: cannot be because JSON string is < 0 */
	item->size_sized = false;

	/* case ssize_t: range check */
	if (item->as_maxint >= (intmax_t)SSIZE_MIN && item->as_maxint <= (intmax_t)SSIZE_MAX) {
	    item->ssize_sized = true;
	    item->as_ssize = (ssize_t)item->as_maxint;
	}

	/* case off_t: range check */
	if (item->as_maxint >= (intmax_t)OFF_MIN && item->as_maxint <= (intmax_t)OFF_MAX) {
	    item->off_sized = true;
	    item->as_off = (off_t)item->as_maxint;
	}

	/* case intmax_t: was handled by the above call to strtoimax() */

	/* case uintmax_t: cannot be because JSON string is < 0 */
	item->umaxint_sized = false;

    } else {

	/* case: non-negative, try for largest unsigned integer */
	errno = 0;			/* pre-clear errno for errp() */
	item->as_umaxint = strtoumax(str, &endptr, 10);
	if (errno == ERANGE || errno == EINVAL || endptr == str || endptr == NULL) {
	    dbg(DBG_VVVHIGH, "strtoumax failed to convert");
	    item->converted = false;
	    return false;	/* processing failed */
	}
	item->converted = true;
	item->umaxint_sized = true;
	dbg(DBG_VVVHIGH, "strtoumax for <%s> returned: %ju", str, item->as_umaxint);

	/* case int8_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT8_MAX) {
	    item->int8_sized = true;
	    item->as_int8 = (int8_t)item->as_umaxint;
	}

	/* case uint8_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT8_MAX) {
	    item->uint8_sized = true;
	    item->as_uint8 = (uint8_t)item->as_umaxint;
	}

	/* case int16_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT16_MAX) {
	    item->int16_sized = true;
	    item->as_int16 = (int16_t)item->as_umaxint;
	}

	/* case uint16_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT16_MAX) {
	    item->uint16_sized = true;
	    item->as_uint16 = (uint16_t)item->as_umaxint;
	}

	/* case int32_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT32_MAX) {
	    item->int32_sized = true;
	    item->as_int32 = (int32_t)item->as_umaxint;
	}

	/* case uint32_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT32_MAX) {
	    item->uint32_sized = true;
	    item->as_uint32 = (uint32_t)item->as_umaxint;
	}

	/* case int64_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT64_MAX) {
	    item->int64_sized = true;
	    item->as_int64 = (int64_t)item->as_umaxint;
	}

	/* case uint64_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT64_MAX) {
	    item->uint64_sized = true;
	    item->as_uint64 = (uint64_t)item->as_umaxint;
	}

	/* case int: bounds check */
	if (item->as_umaxint <= (uintmax_t)INT_MAX) {
	    item->int_sized = true;
	    item->as_int = (int)item->as_umaxint;
	}

	/* case unsigned int: bounds check */
	if (item->as_umaxint <= (uintmax_t)UINT_MAX) {
	    item->uint_sized = true;
	    item->as_uint = (unsigned int)item->as_umaxint;
	}

	/* case long: bounds check */
	if (item->as_umaxint <= (uintmax_t)LONG_MAX) {
	    item->long_sized = true;
	    item->as_long = (long)item->as_umaxint;
	}

	/* case unsigned long: bounds check */
	if (item->as_umaxint <= (uintmax_t)ULONG_MAX) {
	    item->ulong_sized = true;
	    item->as_ulong = (unsigned long)item->as_umaxint;
	}

	/* case long long: bounds check */
	if (item->as_umaxint <= (uintmax_t)LLONG_MAX) {
	    item->longlong_sized = true;
	    item->as_longlong = (long long)item->as_umaxint;
	}

	/* case unsigned long long: bounds check */
	if (item->as_umaxint <= (uintmax_t)ULLONG_MAX) {
	    item->ulonglong_sized = true;
	    item->as_ulonglong = (unsigned long long)item->as_umaxint;
	}

	/* case ssize_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)SSIZE_MAX) {
	    item->ssize_sized = true;
	    item->as_ssize = (ssize_t)item->as_umaxint;
	}

	/* case size_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)SIZE_MAX) {
	    item->size_sized = true;
	    item->as_size = (size_t)item->as_umaxint;
	}

	/* case off_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)OFF_MAX) {
	    item->off_sized = true;
	    item->as_off = (off_t)item->as_umaxint;
	}

	/* case intmax_t: bounds check */
	if (item->as_umaxint <= (uintmax_t)INTMAX_MAX) {
	    item->maxint_sized = true;
	    item->as_maxint = (intmax_t)item->as_umaxint;
	}

	/* case uintmax_t: was handled by the above call to strtoumax() */
    }

    /*
     * processing was successful
     */
    return true;
}


/*
 * json_process_floating - process JSON floating point or e-notation string
 *
 * We will fill out the struct json_number item assuming that ptr, with length len,
 * points to a JSON floating point string as allowed by the so-called JSON spec.
 * The JSON floating point string can be with a JSON float (what the spec calls
 * integer + faction + exponent).
 *
 * given:
 *	item	pointer to a JSON number structure (struct json_number*)
 *	str	JSON floating point or JSON e-notation value as a NUL terminated C-style string
 *	len	length of the JSON number that is not whitespace
 *
 * NOTE: This function assumes that str points to the start of a JSON number, NOT whitespace.
 *
 * NOTE: This function assumes that str is a NUL terminated string,
 *	 even if the NUL is well beyond the end of the JSON number.
 *
 * NOTE: While it is OK if str has trailing whitespace, str[len-1] must be an
 *	 ASCII digit.  It is assumed that str[len-1] is the final JSON number
 *	 character.
 */
static bool
json_process_floating(struct json_number *item, char const *str, size_t len)
{
    char *endptr;			/* first invalid character or str */
    char *e_found = NULL;		/* strchr() search for e */
    char *cap_e_found = NULL;		/* strchr() search for E */
    char *e = NULL;			/* strrchr() search for 2nd e or E */
    size_t str_len = 0;			/* length as a C string, of str */

    /*
     * firewall
     */
    if (item == NULL) {
	warn(__func__, "called with NULL item");
	return false;	/* processing failed */
    }
    if (str == NULL) {
	warn(__func__, "called with NULL ptr");
	return false;	/* processing failed */
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return false;	/* processing failed */
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
	dbg(DBG_HIGH, "%s: floating point numbers cannot start with .: <%s>",
		       __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with .
     */
    } else if (str[len-1] == '.') {
	dbg(DBG_HIGH, "%s: floating point numbers cannot end with .: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with -
     */
    } else if (str[len-1] == '-') {
	dbg(DBG_HIGH, "%s: floating point numbers cannot end with -: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers cannot end with +
     */
    } else if (str[len-1] == '+') {
	dbg(DBG_HIGH, "%s: floating point numbers cannot end with +: <%s>",
		      __func__, str);
	return false;	/* processing failed */

    /*
     * JSON spec detail: floating point numbers must end in a digit
     */
    } else if (!isascii(str[len-1]) || !isdigit(str[len-1])) {
	dbg(DBG_HIGH, "%s: floating point numbers must end in a digit: <%s>",
		       __func__, str);
	return false;	/* processing failed */
    }

    /*
     * detect use of e notation
     */
    e_found = strchr(str, 'e');
    cap_e_found = strchr(str, 'E');
    /* case: both e and E found */
    if (e_found != NULL && cap_e_found != NULL) {

	dbg(DBG_HIGH, "%s: floating point numbers cannot use both e and E: <%s>",
			  __func__, str);
	return false;	/* processing failed */

    /* case: just e found, no E */
    } else if (e_found != NULL) {

	/* firewall - search for two e's */
	e = strrchr(str, 'e');
	if (e_found != e) {
	    dbg(DBG_HIGH, "%s: floating point numbers cannot have more than one e: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}

	/* note e notation */
	item->is_e_notation = true;

    /* case: just E found, no e */
    } else if (cap_e_found != NULL) {

	/* firewall - search for two E's */
	e = strrchr(str, 'E');
	if (cap_e_found != e) {
	    dbg(DBG_HIGH, "%s: floating point numbers cannot have more than one E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}

	/* note e notation */
	item->is_e_notation = true;
    }

    /*
     * perform additional JSON number checks on e notation
     *
     * NOTE: If item->is_e_notation == true, then e points to the e or E
     */
    if (item->is_e_notation == true) {

	/*
	 * JSON spec detail: e notation number cannot start with e or E
	 */
	if (e == str) {
	    dbg(DBG_HIGH, "%s: e notation numbers cannot start with e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot end with e or E
	 */
	} else if (e == &(str[len-1])) {
	    dbg(DBG_HIGH, "%s: e notation numbers cannot end with e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number cannot have e or E after .
	 */
	} else if (e > str && e[-1] == '.') {
	    dbg(DBG_HIGH, "%s: e notation numbers cannot have '.' before e or E: <%s>",
			  __func__, str);
	    return false;	/* processing failed */

	/*
	 * JSON spec detail: e notation number must have digit before e or E
	 */
	} else if (e > str && (!isascii(e[-1]) || !isdigit(e[-1]))) {
	    dbg(DBG_HIGH, "%s: e notation numbers must have digit before e or E: <%s>",
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
		dbg(DBG_HIGH, "%s: :e notation number with e+ or E+ must be followed by a digit <%s>",
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
		dbg(DBG_HIGH, "%s: :e notation number with e- or E- must be followed by a digit <%s>",
			      __func__, str);
		return false;	/* processing failed */
	    }

	/*
	 * JSON spec detail: e notation number must have + or - or digit after e or E
	 */
	} else if (!isascii(e[1]) || !isdigit(e[1])) {
	    dbg(DBG_HIGH, "%s: e notation numbers must follow e or E with + or - or digit: <%s>",
			  __func__, str);
	    return false;	/* processing failed */
	}
    }

    /*
     * convert to largest floating point value
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_longdouble = strtold(str, &endptr);
    if (errno == ERANGE || endptr == str || endptr == NULL) {
	dbg(DBG_VVVHIGH, "strtold failed to convert");
	item->converted = false;
	return false;	/* processing failed */
    }
    item->converted = true;
    item->longdouble_sized = true;
    item->as_longdouble_int = (item->as_longdouble == floorl(item->as_longdouble));
    dbg(DBG_VVVHIGH, "strtold for <%s> returned as %%Lg: %.22Lg", str, item->as_longdouble);
    dbg(DBG_VVVHIGH, "strtold for <%s> returned as %%Le: %.22Le", str, item->as_longdouble);
    dbg(DBG_VVVHIGH, "strtold for <%s> returned as %%Lf: %.22Lf", str, item->as_longdouble);
    dbg(DBG_VVVHIGH, "strtold returned an integer value: %s", booltostr(item->as_longdouble_int));

    /*
     * note if value < 0
     */
    if (item->as_longdouble < 0) {
	item->is_negative = true;
    }

    /*
     * convert to double
     */
    errno = 0;			/* pre-clear conversion test */
    item->as_double = strtod(str, &endptr);
    if (errno == ERANGE || endptr == str || endptr == NULL) {
	item->double_sized = false;
	dbg(DBG_VVVHIGH, "strtod for <%s> failed", str);
    } else {
	item->double_sized = true;
	item->as_double_int = (item->as_double == floorl(item->as_double));
	dbg(DBG_VVVHIGH, "strtod for <%s> returned as %%lg: %.22lg", str, item->as_double);
	dbg(DBG_VVVHIGH, "strtod for <%s> returned as %%le: %.22le", str, item->as_double);
	dbg(DBG_VVVHIGH, "strtod for <%s> returned as %%lf: %.22lf", str, item->as_double);
	dbg(DBG_VVVHIGH, "strtod returned an integer value: %s", booltostr(item->as_double_int));
    }

    /*
     * convert to float
     */
    errno = 0;			/* pre-clear conversion test */
    item->as_float = strtof(str, &endptr);
    if (errno == ERANGE || endptr == str || endptr == NULL) {
	item->float_sized = false;
	dbg(DBG_VVVHIGH, "strtof for <%s> failed", str);
    } else {
	item->float_sized = true;
	item->as_float_int = (item->as_longdouble == floorl(item->as_longdouble));
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%g: %.22g", str, item->as_float);
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%e: %.22e", str, item->as_float);
	dbg(DBG_VVVHIGH, "strtof for <%s> returned as %%f: %.22f", str, item->as_float);
	dbg(DBG_VVVHIGH, "strtof returned an integer value: %s", booltostr(item->as_float_int));
    }

    /*
     * processing was successful
     */
    return true;
}


/*
 * json_alloc - allocate and initialize the JSON parse tree element
 *
 * given:
 *	type	a valid struct json type
 *
 * returns:
 *	pointer to an initialized the JSON parse tree element
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_alloc(enum element_type type)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */

    /*
     * inspect the struct json type for unknown types
     */
    switch (type) {
    case JTYPE_UNSET:
    case JTYPE_NUMBER:
    case JTYPE_STRING:
    case JTYPE_BOOL:
    case JTYPE_NULL:
    case JTYPE_MEMBER:
    case JTYPE_OBJECT:
    case JTYPE_ARRAY:
	break;
    default:
	warn(__func__, "called with unknown JSON type: %d", type);
	break;
    }

    /*
     * allocate the JSON parse tree element
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = calloc(1, sizeof(*ret));
    if (ret == NULL) {
	errp(209, __func__, "calloc #0 error allocating %ju bytes", (uintmax_t)sizeof(*ret));
	not_reached();
    }

    /*
     * initialize the JSON parse tree element
     */
    ret->type = type;
    ret->parent = NULL;
    json_dbg(JSON_DBG_VVHIGH, __func__, "allocated json with type: %s", json_element_type_name(ret));

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_number - convert JSON number string to C numeric value
 *
 * A JSON number string is of the form:
 *
 *      ({JSON_INTEGER}|{JSON_INTEGER}{JSON_FRACTION}|{JSON_INTEGER}{JSON_FRACTION}{JSON_EXPONENT}_
 *
 * where {JSON_INTEGER} is of the form:
 *
 *      -?([1-9][0-9]*|0)
 *
 * and where {JSON_FRACTION} is of the form:
 *
 *      \.[0-9]+
 *
 * and where {JSON_EXPONENT} is of the form:
 *
 *      [Ee][-+]?[0-9]+
 *
 * given:
 *	ptr	pointer to buffer containing a JSON number
 *	len	length, starting at ptr, of the JSON number string
 *
 * returns:
 *	allocated JSON parser tree node of the converted JSON number
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_number(char const *ptr, size_t len)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_number *item = NULL;	    /* JSON number element inside JSON parser tree node */
    bool decimal = false;		    /* true ==> ptr points to a base 10 integer in ASCII */
    bool success = false;		    /* true ==> processing was successful */

    /*
     * allocate an initialized JSON parse tree element
     */
    ret = json_alloc(JTYPE_NUMBER);

    /*
     * initialize the JSON element
     */
    item = &(ret->element.number);
    item->as_str = NULL;
    item->first = NULL;
    item->converted = false;
    item->is_negative = false;
    item->is_floating = false;
    item->is_e_notation = false;
    /* integer values */
    item->int8_sized = false;
    item->uint8_sized = false;
    item->int16_sized = false;
    item->uint16_sized = false;
    item->int32_sized = false;
    item->uint32_sized = false;
    item->int64_sized = false;
    item->uint64_sized = false;
    item->int_sized = false;
    item->uint_sized = false;
    item->long_sized = false;
    item->ulong_sized = false;
    item->longlong_sized = false;
    item->ulonglong_sized = false;
    item->ssize_sized = false;
    item->size_sized = false;
    item->off_sized = false;
    item->maxint_sized = false;
    item->umaxint_sized = false;
    /* floating point values */
    item->float_sized = false;
    item->as_float = 0.0;
    item->as_float_int = false;
    item->double_sized = false;
    item->as_double = 0.0;
    item->as_double_int = false;
    item->longdouble_sized = false;
    item->as_longdouble = 0.0;
    item->as_longdouble_int = false;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL ptr");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    item->as_str_len = len;	/* save original length */
    if (ptr[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON integer string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = calloc(len+1+1, sizeof(char));
    if (item->as_str == NULL) {
	errp(210, __func__, "calloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    strncpy(item->as_str, ptr, len);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */

    /*
     * ignore whitespace
     *
     * Find the first ASCII text, ignoring any leading whitespace if found.
     * The length, within the len limit, of ASCII text that is neither
     * whitespace nor NUL byte, is also found.
     *
     * While the common use of this function is via bison/flex produced C code,
     * we want to keep the general case working where this function might
     * someday be called from some other code.  For such a future case we
     * want to trim off leading and trailing whitespace so that the code below
     * checking for < 0 and the code checking the conversion is not confused.
     */
    item->number_len = find_text(item->as_str, item->as_str_len, &(item->first));
    if (item->number_len <= 0) {
	dbg(DBG_HIGH, "%s: called with string containing all whitespace: <%s>", __func__, item->as_str);
	return ret;
    }

    /*
     * case: JSON number is a base 10 integer in ASCII
     */
    decimal = is_decimal(item->first, item->number_len);
    if (decimal == true) {

	/* process JSON number as a base 10 integer in ASCII */
	success = json_process_decimal(item, item->first, item->number_len);
	if (success == false) {
	    warn(__func__, "JSON number as base 10 integer in ASCII processing failed: <%*.*s>",
			   (int)item->number_len, (int)item->number_len, item->first);
	}

    /*
     * case: JSON number is not a base 10 integer in ASCII
     *
     * The JSON number might be a floating point or e-notation number.
     */
    } else {

	/* process JSON number as floating point or e-notation number */
	success = json_process_floating(item, item->first, item->number_len);
	if (success == false) {
	    warn(__func__, "JSON number as floating point or e-notation number failed: <%*.*s>",
			   (int)item->number_len, (int)item->number_len, item->first);
	}
    }


    json_dbg(JSON_DBG_LOW, __func__, "JSON return type: %s", json_element_type_name(ret));

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_number_str - convert JSON number string to C numeric value
 *
 * This is a simplified interface for json_conv_int().  See that function for details.
 *
 * given:
 *	str	JSON number in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	allocated JSON parser tree node converted JSON integer
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_number_str(char const *str, size_t *retlen)
{
    struct json *ret = NULL;	    /* JSON parser tree node to return */
    size_t len = 0;		    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_number() handle the arg firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_number() call
     */
    ret = json_conv_number(str, len);
    if (ret == NULL) {
	err(211, __func__, "json_conv_number() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_string - convert JSON encoded string to C string
 *
 * A JSON string is of the form:
 *
 *      "([^\n"]|\\")*"
 *
 * given:
 *	ptr	pointer to buffer containing a JSON encoded string
 *	len	length, starting at ptr, of the JSON encoded string
 *	quote	true ==> ignore JSON double quotes, both ptr[0] & ptr[len-1]
 *		must be '"'
 *		false ==> the entire ptr is to be converted
 *
 * returns:
 *	allocated JSON parser tree node converted JSON string
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_string(char const *ptr, size_t len, bool quote)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_string *item = NULL;	    /* JSON string element inside JSON parser tree node */

    /*
     * allocate an initialized JSON parse tree element
     */
    ret = json_alloc(JTYPE_STRING);

    /*
     * initialize the JSON element
     */
    item = &(ret->element.string);
    item->as_str = NULL;
    item->str = NULL;
    item->converted = false;
    item->quote = false;
    item->same = false;
    item->has_nul = false;
    item->slash = false;
    item->posix_safe = false;
    item->first_alphanum = false;
    item->upper = false;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL ptr");
	return ret;
    }

    /*
     * case: JSON surrounding '"'s are to be ignored
     */
    item->as_str_len = len;	/* save length of as_str */
    if (quote == true) {

	/*
	 * firewall
	 */
	if (len < 2) {
	    warn(__func__, "quote === true: called with len: %ju < 2", (uintmax_t)len);
	    return ret;
	}
	if (ptr[0] != '"') {
	    warn(__func__, "quote === true: string does NOT start with a \"");
	    return ret;
	}
	if (ptr[len-1] != '"') {
	    warn(__func__, "quote === true: string does NOT end with a \"");
	    return ret;
	}

	/*
	 * ignore JSON surrounding '"'s
	 */
	item->quote = true;
	++ptr;
	len -= 2;
    }

    /*
     * duplicate the JSON string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = calloc(len+1+1, sizeof(char));
    if (item->as_str == NULL) {
	errp(212, __func__, "calloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    strncpy(item->as_str, ptr, len);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */

    /*
     * decode the JSON encoded string
     */
    /* decode the entire string */
    item->str = json_decode_str(item->as_str, &(item->str_len));
    if (item->str == NULL) {
	warn(__func__, "quote === %s: JSON string decode failed for: <%s>",
		       booltostr(quote), item->as_str);
	return ret;
    }
    item->converted = true;	/* JSON decoding successful */

    /*
     * determine if decoded string is identical to the original JSON encoded string
     */
    if (item->as_str_len == item->str_len && memcmp(item->as_str, item->str, item->as_str_len) == 0) {
	item->same = true;	/* decoded string same an original JSON encoded string (perhaps sans '"'s) */
    }

    /*
     * determine if decoded JSON string is a C string
     */
    item->has_nul = is_string(item->str, item->str_len);

    /*
     * determine POSIX state of the decoded string
     */
    posix_safe_chk(item->str, item->str_len, &item->slash, &item->posix_safe, &item->first_alphanum, &item->upper);

    json_dbg(JSON_DBG_LOW, __func__, "JSON return type: %s", json_element_type_name(ret));

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_string_str - convert JSON encoded string to C string
 *
 * This is a simplified interface for json_conv_string(). See that function for details.
 *
 * given:
 *	str	encoded JSON string in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *	quote	true ==> ignore JSON double quotes, both str[0] & str[len-1]
 *		must be '"', false ==> the entire str is to be converted
 *
 * returns:
 *	allocated JSON parser tree node converted JSON string
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_string_str(char const *str, size_t *retlen, bool quote)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    size_t len = 0;			    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_string() handle the arg firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_string() call
     */
    ret = json_conv_string(str, len, quote);
    if (ret == NULL) {
	err(213, __func__, "json_conv_string() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_bool - convert JSON boolean to C bool
 *
 * A JSON boolean is of the form:
 *
 *      true
 *      false
 *
 * given:
 *	ptr	pointer to buffer containing a JSON boolean
 *	len	length, starting at ptr, of the JSON boolean
 *
 * returns:
 *	allocated JSON parser tree node converted JSON boolean
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_bool(char const *ptr, size_t len)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_boolean *item = NULL;	    /* allocated decoding string or NULL */

    /*
     * allocate an initialized JSON parse tree element
     */
    ret = json_alloc(JTYPE_BOOL);

    /*
     * initialize the JSON element
     */
    item = &(ret->element.boolean);
    item->as_str = NULL;
    item->converted = false;
    item->value = false;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL ptr");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    if (ptr[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON encoded string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = malloc(len+1+1);
    if (item->as_str == NULL) {
	errp(214, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    memcpy(item->as_str, ptr, len+1);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */
    item->as_str_len = len;

    /*
     * decode the JSON boolean
     */
    if (strcmp(item->as_str, "true") == 0) {
	item->converted = true;
	item->value = true;
    } else if (strcmp(item->as_str, "false") == 0) {
	item->converted = true;
	item->value = false;
    } else {
	warn(__func__, "JSON boolean string neither true nor false: <%s>", item->as_str);
    }

    json_dbg(JSON_DBG_LOW, __func__, "JSON return type: %s", json_element_type_name(ret));

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_bool_str - convert JSON boolean to C bool
 *
 * This is a simplified interface for json_conv_bool(). See that function for details.
 *
 * given:
 *	str	JSON boolean in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	allocated JSON parser tree node converted JSON boolean
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_bool_str(char const *str, size_t *retlen)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    size_t len = 0;			    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_bool() handle the arg firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_bool() call
     */
    ret = json_conv_bool(str, len);
    if (ret == NULL) {
	err(215, __func__, "json_conv_bool() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_null - convert JSON null to C NULL
 *
 * A JSON null is of the form:
 *
 *      null
 *
 * given:
 *	ptr	pointer to buffer containing a JSON null
 *	len	length, starting at ptr, of the JSON null
 *
 * returns:
 *	allocated JSON parser tree node converted JSON null
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_null(char const *ptr, size_t len)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_null *item = NULL;	    /* allocated decoding string or NULL */

    /*
     * allocate an initialized JSON parse tree element
     */
    ret = json_alloc(JTYPE_NULL);

    /*
     * initialize the JSON element
     */
    item = &(ret->element.null);
    item->as_str = NULL;
    item->value = NULL;

    /*
     * firewall
     */
    if (ptr == NULL) {
	warn(__func__, "called with NULL ptr");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    if (ptr[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON string
     */
    errno = 0;			/* pre-clear errno for errp() */
    item->as_str = malloc(len+1+1);
    if (item->as_str == NULL) {
	errp(216, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    memcpy(item->as_str, ptr, len+1);
    item->as_str[len] = '\0';	/* paranoia */
    item->as_str[len+1] = '\0';	/* paranoia */
    item->as_str_len = len;

    /*
     * decode the JSON null
     */
    if (strcmp(item->as_str, "null") == 0) {
	item->converted = true;
	item->value = NULL;
    } else {
	warn(__func__, "JSON null string is not null: <%s>", item->as_str);
    }

    json_dbg(JSON_DBG_LOW, __func__, "JSON return type: %s", json_element_type_name(ret));

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_null_str - convert JSON null to C NULL
 *
 * This is a simplified interface for json_conv_null(). See that function for details.
 *
 * given:
 *	str	JSON null in the from of a NUL terminated C-style string
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	allocated JSON parser tree node converted JSON null
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_null_str(char const *str, size_t *retlen)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    size_t len = 0;			    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the json_conv_null() handle the arg firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }

    /*
     * convert to json_conv_null() call
     */
    ret = json_conv_null(str, len);
    if (ret == NULL) {
	err(217, __func__, "json_conv_null() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_conv_member - convert JSON member into JSON parser tree node
 *
 * A JSON member is of the form:
 *
 *      name : value
 *
 * where name is a JSON string of the form:
 *
 *      "([^\n"]|\\")*"
 *
 * and where value is any JSON value such as a:
 *
 *      JSON object
 *      JSON array
 *      JSON string
 *      JSON number
 *      JSON boolean
 *      JSON null
 *
 * given:
 *	name	JSON member string
 *	value	JSON member value (object, array, string, number, boolean, null)
 *
 * returns:
 *	allocated JSON parser tree node converted JSON member
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_conv_member(struct json *name, struct json *value)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_member *item = NULL;	    /* allocated JSON member */

    /*
     * allocate an initialized JSON parse tree element
     */
    ret = json_alloc(JTYPE_MEMBER);

    /*
     * initialize the JSON element
     */
    item = &(ret->element.member);
    item->converted = false;
    item->name = NULL;
    item->value = NULL;

    /*
     * firewall
     */
    if (name == NULL) {
	warn(__func__, "called with NULL name");
	return ret;
    }
    if (value == NULL) {
	warn(__func__, "called with NULL value");
	return ret;
    }
    if (name->type != JTYPE_STRING) {
	warn(__func__, "expected JSON string JTYPE_STRING (type: %d) found type: %s (type: %d)",
		JTYPE_STRING, json_element_type_name(name),  name->type);
	return ret;
    }
    switch (value->type) {
    case JTYPE_NUMBER:
    case JTYPE_STRING:
    case JTYPE_BOOL:
    case JTYPE_MEMBER:
    case JTYPE_OBJECT:
    case JTYPE_ARRAY:
	json_dbg(JSON_DBG_LOW, __func__, "%s: JSON name type: %s", __func__, json_element_type_name(name));
	json_dbg(JSON_DBG_LOW, __func__, "%s: JSON value type: %s", __func__, json_element_type_name(value));
	break;
    default:
	warn(__func__, "expected JSON object, array, string, number, boolean or null, found type: %d", value->type);
	return ret;
    }

    /*
     * link JSON parse tree children to this parent node
     */
    name->parent = ret;
    value->parent = ret;

    /*
     * link name and value
     */
    item->name = name;
    item->value = value;
    item->converted = true;

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_create_object - allocate a JSON object
 *
 * JSON object is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON object, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * returns:
 *	allocated JSON parser tree node converted JSON object
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_create_object(void)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_object *item = NULL;	    /* allocated JSON member */

    /*
     * allocate an initialized JSON parse tree element
     */
    ret = json_alloc(JTYPE_OBJECT);

    /*
     * initialize the JSON object
     */
    item = &(ret->element.object);
    item->converted = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON objects
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(218, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;

    json_dbg(JSON_DBG_LOW, __func__, "%s: JSON return type: %s", __func__, json_element_type_name(ret));

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_object_add_member - add a JSON member to a JSON object
 *
 * JSON object is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON object, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * given:
 *	node	JSON node of the JSON object being added to
 *	member	JSON node of the JSON member to add
 *
 * returns:
 *	true ==> JSON member added to JSON object
 *	false ==> wrong type of node, or
 *		  wrong type of member, or
 *		  NULL pointer
 *
 * NOTE: This function will not return on malloc error.
 */
bool
json_object_add_member(struct json *node, struct json *member)
{
    struct json_object *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return false;
    }
    if (member == NULL) {
	warn(__func__, "member is NULL");
	return false;
    }
    if (node->type != JTYPE_OBJECT) {
	warn(__func__, "node type expected to be JTYPE_OBJECT: %d found type: %d",
		       JTYPE_OBJECT, node->type);
	return false;
    }
    if (member->type != JTYPE_MEMBER) {
	warn(__func__, "node type expected to be JTYPE_MEMBER: %d found type: %d",
		       JTYPE_MEMBER, node->type);
	return false;
    }

    /*
     * point to object
     */
    json_dbg(JSON_DBG_LOW, __func__, "%s: JSON object type: %s", __func__, json_element_type_name(node));
    json_dbg(JSON_DBG_LOW, __func__, "%s: JSON member type: %s", __func__, json_element_type_name(member));
    item = &(node->element.object);
    if (item->s == NULL) {
	warn(__func__, "item->s is NULL");
	return false;
    }

    /*
     * link JSON parse tree child to this parent node
     */
    member->parent = node;

    /*
     * append member
     */
    moved = dyn_array_append_value(item->s, member);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s, dyn_array_append_value moved data", __func__);
    }

    /*
     * update accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return true;
}


/*
 * json_object_append_members - add a dynamic array of JSON members to a JSON object
 *
 * JSON object is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON object, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * given:
 *	node	JSON node of the JSON object being added to
 *	members	dynamic array of JSON members
 *
 * returns:
 *	true ==> JSON members added to JSON object
 *	false ==> wrong type of node, or
 *		  wrong type of members in dynamic members, or
 *		  NULL pointer
 *
 * NOTE: This function will not return on malloc error.
 */
bool
json_object_append_members(struct json *node, struct dyn_array *members)
{
    struct json_object *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */
    int i;

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return false;
    }
    if (members == NULL) {
	warn(__func__, "members is NULL");
	return false;
    }
    if (node->type != JTYPE_OBJECT) {
	warn(__func__, "node type expected to be JTYPE_OBJECT: %d found type: %d",
		       JTYPE_OBJECT, node->type);
	return false;
    }
    if (members->data == NULL) {
	warn(__func__, "dynamic array data is NULL");
	return false;
    }
    json_dbg(JSON_DBG_LOW, __func__, "%s: JSON object type: %s", __func__, json_element_type_name(node));
    for (i=0; i < dyn_array_tell(members); ++i) {
	json_dbg(JSON_DBG_LOW, __func__, "%s: JSON members[%d] type: %s", __func__,
				 i, json_element_type_name(dyn_array_value(members, struct json *, i)));
	if (dyn_array_value(members, struct json *, i)->type != JTYPE_MEMBER) {
	    warn(__func__, "members[%d] node type expected to be JTYPE_MEMBER: %d found type: %d",
			   i, JTYPE_MEMBER, dyn_array_value(members, struct json *, i)->type);
	    return false;
	}
    }

    /*
     * point to object
     */
    item = &(node->element.object);
    if (item->s == NULL) {
	warn(__func__, "item->s is NULL");
	return false;
    }

    /*
     * link JSON parse tree children to this parent node
     */
    for (i=0; i < dyn_array_tell(members); ++i) {
	dyn_array_value(members, struct json *, i)->parent = node;
    }

    /*
     * append member
     */
    moved = dyn_array_concat_array(item->s, members);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s, dyn_array_concat_array moved data", __func__);
    }

    /*
     * update accounting for the object
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return true;
}


/*
 * json_create_array - allocate a JSON array
 *
 * A JSON array is of the form:
 *
 *      [ ]
 *      [ values ]
 *
 * The pointer to the i-th JSON value in the JSON array, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * returns:
 *	allocated JSON parser tree node converted JSON array
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json *
json_create_array(void)
{
    struct json *ret = NULL;		    /* JSON parser tree node to return */
    struct json_array *item = NULL;	    /* allocated JSON member */

    /*
     * allocate an initialized JSON parse tree element
     */
    ret = json_alloc(JTYPE_ARRAY);

    /*
     * initialize the JSON array
     */
    item = &(ret->element.array);
    item->converted = false;
    item->len = 0;
    item->set = NULL;
    item->s = NULL;

    /*
     * create a dynamic array to store JSON arrays
     */
    item->s = dyn_array_create(sizeof (struct json *), JSON_CHUNK, JSON_CHUNK, true);
    if (item->s == NULL) {
	errp(219, __func__, "dyn_array_create() returned NULL");
	not_reached();
    }

    /*
     * initialize accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    item->converted = true;

    json_dbg(JSON_DBG_LOW, __func__, "%s: JSON return type: %s", __func__, json_element_type_name(ret));

    /*
     * return the JSON parse tree element
     */
    return ret;
}


/*
 * json_array_add_value - add a JSON value to a JSON array
 *
 * JSON array is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON array, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * given:
 *	node	JSON node of the JSON array being added to
 *	value	JSON node of the JSON value
 *
 * returns:
 *	true ==> JSON value added to JSON array
 *	false ==> wrong type of node, or
 *		  wrong type of value, or
 *		  NULL pointer
 *
 * NOTE: This function will not return on malloc error.
 */
bool
json_array_add_value(struct json *node, struct json *value)
{
    struct json_array *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return false;
    }
    if (value == NULL) {
	warn(__func__, "value is NULL");
	return false;
    }
    if (node->type != JTYPE_ARRAY) {
	warn(__func__, "node type expected to be JTYPE_ARRAY: %d found type: %d",
		       JTYPE_ARRAY, node->type);
	return false;
    }
    json_dbg(JSON_DBG_LOW, __func__, "JSON object type: %s", json_element_type_name(node));
    switch (value->type) {
    case JTYPE_NUMBER:
    case JTYPE_STRING:
    case JTYPE_BOOL:
    case JTYPE_MEMBER:
    case JTYPE_OBJECT:
    case JTYPE_ARRAY:
	json_dbg(JSON_DBG_LOW, __func__, "JSON value type: %s", json_element_type_name(value));
	break;
    default:
	warn(__func__, "expected JSON object, array, string, number, boolean or null, found type: %d", value->type);
	return false;
    }

    /*
     * point to array
     */
    item = &(node->element.array);
    if (item->s == NULL) {
	warn(__func__, "item->s is NULL");
	return false;
    }

    /*
     * link JSON parse tree child to this parent node
     */
    value->parent = node;

    /*
     * append value
     */
    moved = dyn_array_append_value(item->s, value);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s, dyn_array_append_value moved data", __func__);
    }

    /*
     * update accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return true;
}


/*
 * json_array_append_values - add a dynamic array of JSON values to a JSON array
 *
 * JSON array is one of:
 *
 *      { }
 *      { members }
 *
 * The pointer to the i-th JSON member in the JSON array, if i < len, is:
 *
 *      foo.set[i-1]
 *
 * given:
 *	node	JSON node of the JSON array being added to
 *	values	dynamic array of JSON values
 *
 * returns:
 *	true ==> JSON values added to JSON array
 *	false ==> wrong type of node, or
 *		  wrong type of values, or
 *		  NULL pointer
 *
 * NOTE: This function will not return on malloc error.
 */
bool
json_array_append_values(struct json *node, struct dyn_array *values)
{
    struct json_array *item = NULL;	    /* allocated JSON member */
    bool moved = false;			    /* true == dyn_array_append_value() moved data */
    int i;

    /*
     * firewall
     */
    if (node == NULL) {
	warn(__func__, "node is NULL");
	return false;
    }
    if (values == NULL) {
	warn(__func__, "values is NULL");
	return false;
    }
    if (values->data == NULL) {
	warn(__func__, "dynamic array data is NULL");
	return false;
    }
    if (node->type != JTYPE_ARRAY) {
	warn(__func__, "node type expected to be JTYPE_ARRAY: %d found type: %d",
		       JTYPE_ARRAY, node->type);
	return false;
    }
    json_dbg(JSON_DBG_LOW, __func__, "JSON object type: %s", json_element_type_name(node));
    for (i=0; i < dyn_array_tell(values); ++i) {
	json_dbg(JSON_DBG_LOW, __func__, "%s: JSON values[%d] type: %s", __func__,
				 i, json_element_type_name(dyn_array_value(values, struct json *, i)));
	switch (dyn_array_value(values, struct json *, i)->type) {
	case JTYPE_NUMBER:
	case JTYPE_STRING:
	case JTYPE_BOOL:
	case JTYPE_MEMBER:
	case JTYPE_OBJECT:
	case JTYPE_ARRAY:
	    break;
	default:
	    warn(__func__, "expected JSON values[%d], array, string, number, boolean or null, found type: %d",
			   i, dyn_array_value(values, struct json *, i)->type);
	    return false;
	}
    }

    /*
     * point to array
     */
    item = &(node->element.array);
    if (item->s == NULL) {
	warn(__func__, "item->s is NULL");
	return false;
    }

    /*
     * link JSON parse tree child to this parent node
     */
    for (i=0; i < dyn_array_tell(values); ++i) {
	dyn_array_value(values, struct json *, i)->parent = node;
    }

    /*
     * append value
     */
    moved = dyn_array_concat_array(item->s, values);
    if (moved == true) {
	dbg(DBG_HIGH, "in %s, dyn_array_concat_array moved data", __func__);
    }

    /*
     * update accounting for the array
     */
    item->len = dyn_array_tell(item->s);
    item->set = dyn_array_addr(item->s, struct json *, 0);
    return true;
}
