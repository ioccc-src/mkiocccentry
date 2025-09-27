/*
 * dyn_array_test - test the dynamic array facility
 *
 * Copyright (c) 2014,2015,2022-2025 by Landon Curt Noll.  All Rights Reserved.
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
#include <string.h>
#include <unistd.h>

/*
 * dyn_array_test - test the dynamic array facility
 */
#include "dyn_test.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (0)	/* number of required arguments on the command line */
#define CHUNK (1024)		/* allocate CHUNK elements at a time */
#define DEFAULT_SEED (23209)	/* default seed used when calling srandom() */
#define MAX_RANDOM (0x7fffffff)	/* maximum value returned by random() */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-s seed]\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-s seed\t\tset seed for srandom() (def: %u)\n"
    "\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    1\ttest suite failed\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    " >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "dyn_array library version: %s\n";


/*
 * forward declarations
 */
static int dbl_cmp(const void *pa, const void *pb);
#if defined(NON_STANDARD_SORT)
static int thunk_dbl_cmp(void *pthunk, const void *pa, const void *pb);
#endif /* NON_STANDARD_SORT */
static void usage(int exitcode, char const *str, char const *prog) __attribute__((noreturn));


/*
 * ******************************************************************************************************
 * PLEASE NOTE: For purposes of reading an example, please IGNORE the following !defined(DBG_USE) section
 * ******************************************************************************************************
 */
#if !defined(DBG_USE)

/*
 * dbg defines and forward declarations
 *
 * NOTICE: What follows are declarations lifted from dbg.c in the dbg repo:
 *
 *	https://github.com/lcn2/dbg
 *
 *	These defines and forward declarations are used when DBG_USE is NOT defined.
 *	This allows dyn_test to be compiled without requiring the dbg.h file to be installed.
 *	We reproduce dbg defines and forward declarations  here so that one may run this test
 *	code here without having to have access to the dbg repo source code.
 */

#  if !defined(DBG_NONE)
#  define DBG_NONE (0)		   /* no debugging */
#  endif

#  if !defined(DBG_DEFAULT)
#  define DBG_DEFAULT (DBG_NONE)  /* default debugging level */
#  endif


/*
 * not_reached
 *
 * In the old days of lint, one could give lint and friends a hint by
 * placing the token NOTREACHED immediately between opening and closing
 * comments.  Modern compilers do not honor such commented tokens
 * and instead rely on features such as __builtin_unreachable
 * and __attribute__((noreturn)).
 *
 * The not_reached will either yield a __builtin_unreachable() feature call,
 * or it will call abort from stdlib.
 */
#if __has_builtin(__builtin_unreachable)
#    define not_reached() __builtin_unreachable()
#else
#    define not_reached() abort()
#endif /* __has_builtin(__builtin_unreachable) */

/*
 * globals for debugging
 */
int verbosity_level = DBG_DEFAULT;	/* maximum debug level for debug messages */
bool dbg_output_allowed = true;		/* false ==> disable debug messages */
bool warn_output_allowed = true;	/* false ==> disable warning messages */
bool err_output_allowed = true;		/* false ==> disable error messages */
bool usage_output_allowed = true;	/* false ==> disable usage messages */
bool msg_warn_silent = false;		/* true ==> silence info & warnings if verbosity_level <= 0 */

/*
 * forward declarations
 */
static void fdbg_write(FILE *stream, char const *caller, int level, char const *fmt, va_list ap);
static void fwarn_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap);
static void ferr_write(FILE *stream, int error_code, char const *caller,
		       char const *name, char const *fmt, va_list ap);
static void ferrp_write(FILE *stream, int error_code, char const *caller,
			char const *name, char const *fmt, va_list ap);
static void fwarnp_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap);
static void fusage_write(FILE *stream, int error_code, char const *caller, char const *fmt, va_list ap);
static bool fchk_inval_opt(FILE *stream, char const *prog, int ch, int opt);

bool dbg_allowed(int level);
bool warn_allowed(void);
bool err_allowed(void);
bool usage_allowed(void);
void dbg(int level, char const *fmt, ...);
void fwarn(FILE *stream, char const *name, char const *fmt, ...);
void warnp(char const *name, char const *fmt, ...);
void err(int exitcode, char const *name, char const *fmt, ...);
void errp(int exitcode, char const *name, char const *fmt, ...);
void fprintf_usage(int exitcode, FILE *stream, char const *fmt, ...);
int parse_verbosity(char const *optarg);

#endif /* !DBG_USE */
/*
 * **************************************************************
 * For purposes of an example, end of !DBG_USE section to ignore.
 * **************************************************************
 */


/*
 * dbl_cmp - compare doubles for sorting
 *
 * given:
 *	pa	    pointer to a
 *	pb	    pointer to b
 *
 * returns
 *	-1 ==> a < b
 *	0  ==> a == b
 *	1  ==> a > b
 */
static int
dbl_cmp(const void *pa, const void *pb)
{
    double a, b;	    /* values to compare */

    /*
     * firewall - paranoia
     */
    if (pa == NULL) {
	err(10, __func__, "pa is NULL");    /*ooo*/
	not_reached();
    }
    if (pb == NULL) {
	err(11, __func__, "pb is NULL");
	not_reached();
    }

    /*
     * compare
     */
    a = *(double *)pa;
    b = *(double *)pb;
    if (a < b) {
	return -1;
    } else if (a > b) {
	return 1;
    }
    return 0;
}


#if defined(NON_STANDARD_SORT)

/*
 * NON_STANDARD_SORT
 *
 * It is sad that qsort_r() is not part of the standard C library as of 2025.  Worse yet, clang libc and gnu libc
 * put the thunk argument in different positions in the comparison library.
 *
 * It is sad that both heapsort() and mergesort() part of the standard C library as of 2025.
 */


/*
 * thunk_dbl_cmp - compare doubles for sorting with a thunk value for use by qsort_r(3)
 *
 * given:
 *	pthunk	    pointer to a thunk
 *	pa	    pointer to a
 *	pb	    pointer to b
 *
 * returns
 *	-1 ==> a < b
 *	0  ==> a == b
 *	1  ==> a > b
 */
static int
thunk_dbl_cmp(void *pthunk, const void *pa, const void *pb)
{
    double a, b;	    /* values to compare */

    /*
     * firewall - paranoia
     */
    if (pthunk == NULL) {
	err(12, __func__, "pthunk is NULL");
	not_reached();
    }
    if (pa == NULL) {
	err(13, __func__, "pa is NULL");
	not_reached();
    }
    if (pb == NULL) {
	err(14, __func__, "pb is NULL");
	not_reached();
    }

    /*
     * increment thunk
     */
    ++(*(intmax_t *)pthunk);

    /*
     * compare
     */
    a = *(double *)pa;
    b = *(double *)pb;
    if (a < b) {
	return -1;
    } else if (a > b) {
	return 1;
    }
    return 0;
}

#endif /* NON_STANDARD_SORT */


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    struct dyn_array *array;	/* dynamic array to test */
    double d;			/* test double */
    bool error = false;		/* true ==> test error found */
    intmax_t len = 0;		/* length of the dynamic array */
#if defined(NON_STANDARD_SORT)
    intmax_t thunk = 0;		/* qsort_r(3) compare count */
    int ret;				/* dyn_array_heapsort() or dyn_array_mergesort() return */
#endif /* NON_STANDARD_SORT */
    unsigned long seed = DEFAULT_SEED;	/* seed for random(3) */
    bool opt_error = false;	/* fchk_inval_opt() return */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:Vs:")) != -1) {
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
	    (void) printf("%s version: %s\n", DYN_TEST_BASENAME, DYN_TEST_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 's':
	    errno = 0;
	    seed = strtoul(optarg, NULL, 0);
	    if (errno != 0) {
		err(15, __func__, "strtoul error");
		not_reached();
	    }
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

    /*
     * create dynamic array
     */
    array = dyn_array_create(sizeof(double), CHUNK, CHUNK, true);
    dbg(DBG_MED, "created array of size %ju", (uintmax_t)CHUNK);

    /*
     * load a million doubles
     */
    dbg(DBG_LOW, "loading a million doubles into array");
    for (d = 0.0; d < 1000000.0; d += 1.0) {
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "moved data after appending d: %f", d);
	}
    }

    /*
     * verify values
     */
    for (i = 0; i < 1000000; ++i) {
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i));
	    error = true;
	}
    }

    /*
     * verify size
     */
    dbg(DBG_HIGH, "determining size of array");
    len = dyn_array_tell(array);
    if (len != 1000000) {
	warn(__func__, "dyn_array_tell(array): %jd != %jd", len, (intmax_t)1000000);
	error = true;
    }
    dbg(DBG_VHIGH, "array size is %ju", (uintmax_t)len);

    /*
     * qsort array that is already sorted
     */
    dbg(DBG_MED, "dyn_array_qsort() array, that is already sorted, using dbl_cmp");
    dyn_array_qsort(array, dbl_cmp);

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_qsort()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of array, that is already sorted, by dyn_array_qsort()");


#if defined(NON_STANDARD_SORT)

/*
 * NON_STANDARD_SORT
 *
 * It is sad that qsort_r() is not part of the standard C library as of 2025.  Worse yet, clang libc and gnu libc
 * put the thunk argument in different positions in the comparison library.
 *
 * It is sad that both heapsort() and mergesort() part of the standard C library as of 2025.
 */

    /*
     * qsort_r array that is already sorted
     */
    thunk = 0;
    dbg(DBG_MED, "dyn_array_qsort_r() array, that is already sorted, using dbl_cmp");
    dyn_array_qsort_r(array, &thunk, thunk_dbl_cmp);

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_qsort_r()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of array, that is already sorted, by dyn_array_qsort_r(), thunk: %jd", thunk);

    /*
     * heapsort array that is already sorted
     */
    dbg(DBG_MED, "dyn_array_heapsort() array, that is already sorted, using dbl_cmp");
    ret = dyn_array_heapsort(array, dbl_cmp);
    if (ret != 0) {
	errp(16, __func__, "dyn_array_heapsort() error: %d", ret);
	not_reached();
    }

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_heapsort()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of array, that is already sorted, by dyn_array_heapsort()");

    /*
     * mergesort array that is already sorted
     */
    dbg(DBG_MED, "dyn_array_mergesort() array, that is already sorted, using dbl_cmp");
    ret = dyn_array_mergesort(array, dbl_cmp);
    if (ret != 0) {
	errp(17, __func__, "dyn_array_mergesort() error: %d", ret);
	not_reached();
    }

#endif /* NON_STANDARD_SORT */

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_mergesort()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of array, that is already sorted, by dyn_array_mergesort()");

    /*
     * concatenate the array onto itself
     */
    dbg(DBG_LOW, "concatenating array on to itself");
    if (dyn_array_concat_array(array, array)) {
	dbg(DBG_LOW, "moved data after concatenation");
    }

    /*
     * verify new size
     */
    dbg(DBG_HIGH, "determining new size of array");
    len = dyn_array_tell(array);
    if (len != 2000000) {
	warn(__func__, "dyn_array_tell(array): %jd != %jd", len, (intmax_t)2000000);
	error = true;
    }
    dbg(DBG_VHIGH, "new size of array is %ju", (uintmax_t)len);

    /*
     * verify values again
     */
    dbg(DBG_HIGH, "verifying values again");
    for (i = 0; i < 1000000; ++i) {
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i));
	    error = true;
	}
	if ((intmax_t)i != (intmax_t)dyn_array_value(array, double, i+1000000)) {
	    warn(__func__, "value mismatch %d != %f", i, dyn_array_value(array, double, i+1000000));
	    error = true;
	}
    }

    /*
     * append random double values
     */
    dbg(DBG_LOW, "appending a million doubles onto the array");
    srandom((unsigned) seed);
    for (i = 0; i < 1000000; ++i) {
	d = ((double)1000000.0 * (double)random() / ((double)MAX_RANDOM)) + ((double)random() / ((double)MAX_RANDOM));
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "#0: moved data after append %d of d: %f", i, d);
	}
    }
    len = dyn_array_tell(array);
    dbg(DBG_LOW, "array size after appending random double values: %jd", len);

    /*
     * qsort array that as quasi-sorted
     */
    dbg(DBG_LOW, "calling dyn_array_qsort() array, that as quasi-sorted, using dbl_cmp");
    dyn_array_qsort(array, dbl_cmp);

    /*
     * append more random double values
     */
    dbg(DBG_LOW, "appending a million more doubles onto the array");
    srandom((unsigned) seed);
    for (i = 0; i < 1000000; ++i) {
	d = ((double)1000000.0 * (double)random() / ((double)MAX_RANDOM)) + ((double)random() / ((double)MAX_RANDOM));
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "#1: moved data after append %d of d: %f", i, d);
	}
    }
    len = dyn_array_tell(array);
    dbg(DBG_LOW, "array size after appending more random double values: %jd", len);

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_qsort()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of that as quasi-sorted, by dyn_array_qsort()");


#if defined(NON_STANDARD_SORT)

/*
 * NON_STANDARD_SORT
 *
 * It is sad that qsort_r() is not part of the standard C library as of 2025.  Worse yet, clang libc and gnu libc
 * put the thunk argument in different positions in the comparison library.
 *
 * It is sad that both heapsort() and mergesort() part of the standard C library as of 2025.
 */

    /*
     * append yet more random double values
     */
    dbg(DBG_LOW, "appending a 2nd million more doubles onto the array");
    srandom((unsigned) seed);
    for (i = 0; i < 1000000; ++i) {
	d = ((double)1000000.0 * (double)random() / ((double)MAX_RANDOM)) + ((double)random() / ((double)MAX_RANDOM));
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "#1: moved data after append %d of d: %f", i, d);
	}
    }
    len = dyn_array_tell(array);
    dbg(DBG_LOW, "array size after appending yet more random double values: %jd", len);

    /*
     * qsort_r array that as quasi-sorted
     */
    thunk = 0;
    dbg(DBG_LOW, "calling dyn_array_qsort_r() array, that as quasi-sorted, using dbl_cmp");
    dyn_array_qsort_r(array, &thunk, thunk_dbl_cmp);

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_qsort_r()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of that as quasi-sorted, by dyn_array_qsort_r(), thunk: %jd", thunk);

    /*
     * append even more random double values
     */
    dbg(DBG_LOW, "appending another million more doubles onto the array");
    srandom((unsigned) seed);
    for (i = 0; i < 1000000; ++i) {
	d = ((double)1000000.0 * (double)random() / ((double)MAX_RANDOM)) + ((double)random() / ((double)MAX_RANDOM));
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "#2: moved data after append %d of d: %f", i, d);
	}
    }
    len = dyn_array_tell(array);
    dbg(DBG_LOW, "array size after appending even more random double values: %jd", len);

    /*
     * heapsort array that as quasi-sorted
     */
    dbg(DBG_LOW, "calling dyn_array_heapsort() array, that as quasi-sorted, using dbl_cmp");
    ret = dyn_array_heapsort(array, dbl_cmp);
    if (ret != 0) {
	errp(18, __func__, "dyn_array_heapsort() error: %d", ret);
	not_reached();
    }

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_heapsort()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of that as quasi-sorted, by dyn_array_heapsort()");

    /*
     * append yet even more random double values
     */
    dbg(DBG_LOW, "appending yet another million more doubles onto the array");
    srandom((unsigned) seed);
    for (i = 0; i < 1000000; ++i) {
	d = ((double)1000000.0 * (double)random() / ((double)MAX_RANDOM)) + ((double)random() / ((double)MAX_RANDOM));
	if (dyn_array_append_value(array, &d)) {
	    dbg(DBG_LOW, "#3: moved data after append %d of d: %f", i, d);
	}
    }
    len = dyn_array_tell(array);
    dbg(DBG_LOW, "array size after appending yet even more random double values: %jd", len);

    /*
     * mergesort array that as quasi-sorted
     */
    dbg(DBG_LOW, "calling dyn_array_mergesort() array, that as quasi-sorted, using dbl_cmp");
    ret = dyn_array_mergesort(array, dbl_cmp);
    if (ret != 0) {
	errp(19, __func__, "dyn_array_mergesort() error: %d", ret);
	not_reached();
    }

    /*
     * verify array is sorted
     */
    dbg(DBG_HIGH, "verify array is sorted after dyn_array_mergesort()");
    for (i = 1; i < 1000000; ++i) {
	if ((intmax_t)dyn_array_value(array, double, i) < (intmax_t)dyn_array_value(array, double, i-1)) {
	    warn(__func__, "array not sorted: array[%d]: %f < array[%d]: %f",
			   i, dyn_array_value(array, double, i), i-1, dyn_array_value(array, double, i-1));
	}
    }
    dbg(DBG_LOW, "successful sort of that as quasi-sorted, by dyn_array_mergesort()");

#endif /* NON_STANDARD_SORT */

    /*
     * free dynamic array
     */
    if (array != NULL) {
	dyn_array_free(array);
	array = NULL;
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

    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, DEFAULT_SEED,
						     DYN_TEST_BASENAME, DYN_TEST_VERSION,
						     dyn_array_version);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * ******************************************************************************************************
 * PLEASE NOTE: For purposes of reading an example, please IGNORE the following !defined(DBG_USE) section
 * ******************************************************************************************************
 * */
#if !defined(DBG_USE)

/*
 * NOTICE: What follows is code lifted from dbg.c in the dbg repo:
 *
 *	https://github.com/lcn2/dbg
 *
 * We reproduce it here so that one may run this test code here without having to have
 * access to the dbg repo source code.
 */


/*
 * fdbg_write - write a diagnostic message to a stream
 *
 * Write a formatted debug diagnostic message to a stream. The diagnostic is followed by
 * a newline and then the stream is flushed.
 *
 * given:
 *	stream	open stream on which to write
 *	caller	name of the calling function
 *	level	debug level
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
fdbg_write(FILE *stream, char const *caller, int level, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write debug header
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stream, "debug[%d]: ", level);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fprintf error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * write diagnostic to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): vfprintf error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fputc error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fflush error\n",
		      __func__, caller, level, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwarn_write - write a warning to a stream
 *
 * Write a warming message to a stream. The diagnostic is followed by
 * a newline and then the stream is flushed.
 *
 * given:
 *	stream	open stream on which to write
 *	caller	name of the calling function
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 */
static void
fwarn_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;			/* libc function return code */
    int saved_errno;		/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write warning header to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, "Warning: %s: ", name);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): vfprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fputc returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fflush(stream);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fflush returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * ferr_write - write an error diagnostic, to a stream
 *
 * Write a formatted an error diagnostic to a stream.
 *
 * given:
 *	stream		open stream on which to write
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the error diagnostic
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: If stderr is NULL, this function will not issue warnings about print errors.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
ferr_write(FILE *stream, int error_code, char const *caller,
	   char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write error diagnostic header to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stream, "ERROR[%d]: %s: ", error_code, name);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf error\n",
			       __func__, caller, error_code, name, fmt);
    }

    /*
     * write error diagnostic warning to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): vfprintf error\n",
			       __func__, caller, error_code, name, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fputc error\n",
			       __func__, caller, error_code, name, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fflush error\n",
			       __func__, caller, error_code, name, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * ferrp_write - write an error diagnostic with errno details, to a stream
 *
 * Write a formatted an error diagnostic with errno details to a stream.
 *
 * given:
 *	stream		open stream on which to write
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the error diagnostic
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed a other NULL pointers.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
ferrp_write(FILE *stream, int error_code, char const *caller,
	    char const *name, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */


    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write error diagnostic warning header to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stream, "ERROR[%d]: %s: ", error_code, name);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf #0 error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * write error diagnostic warning to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): vfprintf error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * write errno details plus newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fprintf(stream, ": errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fprintf #1 error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, %s, ap): fflush error\n",
		      __func__, caller, error_code, name, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwarnp_write - write a warning message with errno details, to a stream
 *
 * Write a warming message with errno info to a stream. The diagnostic is followed by
 * a newline and then the stream is flushed.
 *
 * given:
 *	stream	open stream on which to write
 *	caller	name of the calling function
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	ap	variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 *
 * NOTE: This function does nothing (just returns) if passed other NULL pointers.
 */
static void
fwarnp_write(FILE *stream, char const *caller, char const *name, char const *fmt, va_list ap)
{
    int ret;			/* libc function return code */
    int saved_errno;		/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || name == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write warning header to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, "Warning: %s: ", name);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write warning to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): vfprintf returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * write errno details plus newline to stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fprintf(stream, ": errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in vwarnp(%s, %s, ap): fprintf with errno returned error: %s\n",
			       caller, name, fmt, strerror(errno));
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for strerror() */
    ret = fflush(stream);
    if (ret < 0) {
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: in %s(stream, %s, %s, %s, ap): fflush returned error: %s\n",
			       caller, __func__, caller, name, fmt, strerror(errno));
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fusage_write - write the usage message, to a stream
 *
 * Write a formatted the usage message to a stream. Checks for write
 * errors and call warnp() with a write error diagnostic.
 *
 * given:
 *	stream		open stream on which to write
 *	error_code	error code
 *	caller		name of the calling function
 *	name		name of function issuing the usage message
 *	fmt		format of the warning
 *	ap		variable argument list
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just exits).
 *
 * NOTE: This function does nothing (just returns) if passed a NULL pointer.
 *
 * NOTE: We call warnp() with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 */
static void
fusage_write(FILE *stream, int error_code, char const *caller, char const *fmt, va_list ap)
{
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * firewall - just return if given a NULL ptr
     */
    if (stream == NULL || caller == NULL || fmt == NULL) {
	return;
    }

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * write the usage message to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = vfprintf(stream, fmt, ap);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): vfprintf error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * write final newline to stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fputc('\n', stream);
    if (ret != '\n') {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fputc error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * flush the stream
     */
    errno = 0;		/* pre-clear errno for warnp() */
    ret = fflush(stream);
    if (ret < 0) {
	warnp(caller, "\nin %s(stream, %s, %d, %s, ap): fflush error\n",
		      __func__, caller, error_code, fmt);
    }

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * dbg_allowed - determine if verbosity level allows for debug messages are allowed
 *
 * given:
 *	level	write message if >= verbosity level
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
dbg_allowed(int level)
{
    /*
     * determine if verbosity level allows for debug messages
     */
    if (dbg_output_allowed == false || level > verbosity_level) {
	return false;
    }
    return true;
}


/*
 * warn_allowed - determine if warning messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
warn_allowed(void)
{
    /*
     * determine if warning messages are allowed
     */
    if (warn_output_allowed == false || (msg_warn_silent == true && verbosity_level <= 0)) {
	return false;
    }
    return true;
}


/*
 * err_allowed - determine if fatal error messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
err_allowed(void)
{
    /*
     * determine if fatal error messages are allowed
     */
    if (err_output_allowed == false) {
	return false;
    }
    return true;
}


/*
 * usage_allowed - determine if command line usage messages are allowed
 *
 * returns:
 *	true ==> allowed, false ==> disabled
 */
bool
usage_allowed(void)
{
    /*
     * determine if conditions allow command line usage messages
     */
    if (usage_output_allowed == false) {
	return false;
    }
    return true;
}


/*
 * dbg - write a verbosity level allowed debug message, to stderr
 *
 * given:
 *	level	write message if >= verbosity level
 *	fmt	printf format
 *	...
 *
 * Example:
 *
 *	dbg(1, "foobar information: %d", value);
 *
 * NOTE: If stderr is NULL, this function does nothing (just returns).
 */
void
dbg(int level, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - do nothing if stderr is NULL
     */
    if (stderr == NULL) {
	return;
    }

    /*
     * stage 0: determine if conditions allow function to write, return if not
     */
    allowed = dbg_allowed(level);
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	fwarn(stderr, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
    }

    /*
     * stage 4: write the diagnostic
     */
    fdbg_write(stderr, __func__, level, fmt, ap);

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * fwarn - write a warning message, to a stream
 *
 * given:
 *	stream	open stream to use
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	fwarn(stderr, __func__, "unexpected foobar: %d", value);
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just returns).
 */
void
fwarn(FILE *stream, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - if stream is NULL, try stderr, unless that is also NULL
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	    stream = stderr;
        } else {
	    return;
	}
    }

    /*
     * stage 0: determine if conditions allow function to write, return if not
     */
    allowed = warn_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: name is NULL, forcing name to be: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stream, "\nWarning: %s: fmt is NULL, forcing fmt to be: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning
     */
    fwarn_write(stream, __func__, name, fmt, ap);

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * warnp - write a warning message with errno details, to stderr
 *
 * given:
 *	name	name of function issuing the warning
 *	fmt	format of the warning
 *	...	optional format args
 *
 * Example:
 *
 *	warnp(__func__, "unexpected foobar: %d", value);
 *
 * NOTE: If stderr is NULL, this function does nothing (just returns).
 */
void
warnp(char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - do nothing if stderr is NULL
     */
    if (stderr == NULL) {
	return;
    }

    /*
     * stage 0: determine if conditions allow function to write, return if not
     */
    allowed = warn_allowed();
    if (allowed == false) {
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (name == NULL) {
	name = "((NULL name))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: called with NULL name, forcing name: %s\n",
			       __func__, name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	/* we cannot call warn() because that would produce an infinite loop! */
	(void) fprintf(stderr, "\nWarning: %s: called with NULL fmt, forcing fmt: %s\n",
			       __func__, fmt);
    }

    /*
     * stage 4: write the warning with errno details
     */
    fwarnp_write(stderr, __func__, name, fmt, ap);

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: restore previous errno value
     */
    errno = saved_errno;
    return;
}


/*
 * err - write a fatal error message to stderr before exiting
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the error
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	err(1, __func__, "bad foobar: %s", message);
 *
 * This function does not return.
 *
 * NOTE: If stderr is NULL, this function does nothing (just exits).
 */
void
err(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
    if (allowed == false) {
	exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	not_reached();
    }

    /* stage 1: we will not return so we do not need to save errno */

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (exitcode < 0) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nexitcode < 0: %d\n", exitcode);
        }
	exitcode = 255;
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
        }
    } else if (exitcode > 255) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nexitcode > 255: %d\n", exitcode);
        }
	exitcode = 255;
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
        }
    }
    if (name == NULL) {
	name = "((NULL name))";
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nname is NULL, forcing name to be: %s", name);
        }
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
        }
    }

    /*
     * stage 4: write error diagnostic
     */
    if (stderr != NULL) {
	ferr_write(stderr, exitcode, __func__, name, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: do not restore errno, just exit
     */
    exit(exitcode);
    not_reached();
}


/*
 * errp - write a fatal error message with errno details to stderr before exiting
 *
 * given:
 *	exitcode	value to exit with
 *	name		name of function issuing the warning
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	errp(1, __func__, "bad foobar: %s", message);
 *
 * This function does not return.
 *
 * NOTE: If stderr is NULL, this function does nothing (just exits).
 */
void
errp(int exitcode, char const *name, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * stage 0: determine if conditions allow function to write, exit if not
     */
    allowed = err_allowed();
    if (allowed == false) {
	exit((exitcode < 0 || exitcode > 255) ? 255 : exitcode);
	not_reached();
    }

    /* stage 1: we will not return so we do not need to save errno */

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (exitcode < 0) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nexitcode < 0: %d\n", exitcode);
	}
	exitcode = 255;
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    } else if (exitcode > 255) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nexitcode > 255: %d\n", exitcode);
	}
	exitcode = 255;
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nforcing use of exit code: %d\n", exitcode);
	}
    }
    if (name == NULL) {
	name = "((NULL name))";
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nname is NULL, forcing name to be: %s", name);
	}
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
	}
    }

    /*
     * stage 4: write error diagnostic with errno details
     */
    if (stderr != NULL) {
	ferrp_write(stderr, exitcode, __func__, name, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: do not restore errno, just exit
     */
    exit(exitcode);
    not_reached();
}


/*
 * fprintf_usage - write command line usage to a stream and, depending on exitcode, exit
 *
 * given:
 *	exitcode	- >= 0, exit with this code
 *			  < 0, just return
 *	stream		- stream to write on
 *	fmt		- format of the usage message
 *	...		- potential args for usage message
 *
 * NOTE: If stream is NULL, stderr will be used.  If stderr is also NULL,
 *	 this function does nothing (just exits).
 */
void
fprintf_usage(int exitcode, FILE *stream, char const *fmt, ...)
{
    va_list ap;			/* variable argument list */
    int saved_errno;		/* errno at function start */
    bool allowed = false;	/* true ==> output is allowed */

    /*
     * firewall - if stream is NULL, try stderr
     */
    if (stream == NULL) {
	if (stderr != NULL) {
	    fwarn(stderr, __func__, "called with NULL stream, will use stderr");
	}
	stream = stderr;
    }

    /*
     * stage 0: determine if conditions allow function to write, exit or return as required
     */
    allowed = usage_allowed();
    if (allowed == false) {
	if (exitcode >= 0) {
	    exit(exitcode);
	    not_reached();
	}
	return;
    }

    /*
     * stage 1: save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * stage 2: stdarg variable argument list setup
     */
    va_start(ap, fmt);

    /*
     * stage 3: firewall checks
     */
    if (fmt == NULL) {
	fmt = "no usage message given";
	if (stream != NULL) {
	    fwarn(stream, __func__, "\nfmt is NULL, forcing fmt to be: %s", fmt);
	}
    }

    /*
     * stage 4: write command line usage
     */
    if (stream != NULL) {
	fusage_write(stream, exitcode, __func__, fmt, ap);
    }

    /*
     * stage 5: stdarg variable argument list cleanup
     */
    va_end(ap);

    /*
     * stage 6: exit if exitcode >= 0, else restore errno
     */
    if (exitcode >= 0) {
	exit(exitcode);
	not_reached();
    } else {
	errno = saved_errno;
    }
    return;
}


/*
 * parse_verbosity - parse -v optarg for our tools
 *
 * given:
 *	optarg		verbosity string, must be an integer >= 0
 *
 * returns:
 *	parsed verbosity or DBG_INVALID on conversion error
 */
int
parse_verbosity(char const *optarg)
{
    int verbosity = DBG_NONE;	/* parsed verbosity or DBG_NONE */

    /*
     * firewall
     */
    if (optarg == NULL) {
	return DBG_INVALID;
    }

    /*
     * parse verbosity
     */
    errno = 0;		/* pre-clear errno for warnp() */
    verbosity = (int)strtol(optarg, NULL, 0);
    if (errno != 0) {
	return DBG_INVALID;
    }
    if (verbosity < 0) {
	return DBG_INVALID;
    }
    return verbosity;
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
static bool
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


#endif /* !DBG_USE */
/*
 * **************************************************************
 * For purposes of an example, end of !DBG_USE section to ignore.
 * **************************************************************
 */
