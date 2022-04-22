/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_util - general JSON utility support functions
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
#include <stdarg.h>
#include <ctype.h>

/*
 * json_ckk - support jinfochk and jauthchk services
 */
#include "json_util.h"


/*
 * static declaractions
 */
static void alloc_json_code_ignore_set(void);
static int cmp_codes(const void *a, const void *b);
static void expand_json_code_ignore_set(void);
static struct ignore_json_code *ignore_json_code_set;


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
 *	filename    filename of json file, "stdin" or NULL
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
 *	filename    filename of json file, "stdin" or NULL
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
 *	filename	file with the problem
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
 *	filename	file with the problem
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
	errp(189, __func__, "failed to malloc struct ignore_json_code");
	not_reached();
    }

    /*
     * allocate an initial block of ignore codes
     */
    errno = 0;			/* pre-clear errno for errp() */
    tbl->code = malloc((JSON_CODE_IGNORE_CHUNK+1+1) * sizeof(int));
    if (tbl->code == NULL) {
	errp(190, __func__, "cannot allocate %d ignore codes", JSON_CODE_IGNORE_CHUNK+1+1);
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
	err(191, __func__, "NULL arg(s)");
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
	err(192, __func__, "ignore_json_code_set is NULL after allocation");
	not_reached();
    }

    /*
     * if no room, expand the table
     */
    if (ignore_json_code_set->next_free >= ignore_json_code_set->alloc) {
	p = realloc(ignore_json_code_set->code, (ignore_json_code_set->alloc+JSON_CODE_IGNORE_CHUNK+1) * sizeof(int));
	errno = 0;			/* pre-clear errno for errp() */
	if (p == NULL) {
	    errp(193, __func__, "cannot expand ignore_json_code_set from %d to %d codes",
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
	err(194, __func__, "code %d < 0", code);
	not_reached();
    }

    /*
     * setup ignore_json_code_set if needed
     */
    alloc_json_code_ignore_set();
    if (ignore_json_code_set == NULL) {
	err(195, __func__, "ignore_json_code_set is NULL after allocation");
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
	err(196, __func__, "code %d < 0", code);
	not_reached();
    }

    /*
     * allocate or expand the json_code_ignore_set[] if needed
     */
    if (ignore_json_code_set == NULL || ignore_json_code_set->next_free >= ignore_json_code_set->alloc) {
	expand_json_code_ignore_set();
    }
    if (ignore_json_code_set == NULL) {
	err(197, __func__, "ignore_json_code_set is NULL after allocation or expansion");
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
