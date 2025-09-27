/*
 * example - allocate dynamic array, store malloced string pointers, print strings, free storage
 *
 * After installing dbg (see https://github.com/lcn2/dbg ),
 * and after installing dyn_array (see https://github.com/lcn2/dyn_array ),
 * compile this example via the command:
 *
 *     cc example.c -ldyn_array -ldbg -o example
 *
 * See also:
 *
 *      man 3 dyn_array
 *
 * Copyright (c) 2025 by Landon Curt Noll.  All Rights Reserved.
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
 * chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) ^oo^
 *
 * Share and enjoy! :-)
 */

#include <stdio.h>
#include <inttypes.h>
#include <dbg.h>                /* for message, debug and error messages */
#include <dyn_array.h>          /* for dyn_array macros and function declarations */

#define INITIAL_SIZE (32)       /* initially allocate this many pointers to strings */
#define CHUNK_SIZE (8)          /* grow dynamic array by this many pointers at a time */

int
main(void)
{
    struct dyn_array *array;    /* dynamic array of pointers to strings */
    char *p;                    /* dynamic array element */
    char **q;                   /* address of a dynamic array string element */
    char buf[BUFSIZ+1];         /* generic buffer */
    char *str;                  /* strdup-ed (malloced) string pointer to add to the dynamic array */
    intmax_t len;               /* length of the dynamic array in elements */
    int *integer_p;             /* address of a dynamic array integer element */
    int integer;                /* integer element value */
    intmax_t i;

    /*
     * allocate an dynamic array of pointers to strings
     */
    msg("allocate an dynamic array of pointers to strings");
    array = dyn_array_create(sizeof(char *), CHUNK_SIZE, INITIAL_SIZE, true);

    /*
     * grow the dynamic array by appending pointers to strings
     *
     * We grow the dynamic array of pointers to strings by appending pointers
     * to successive strings.
     */
    msg("grow the dynamic array by appending pointers to strings");
    str = strdup("this is the first string");
    if (str == NULL) {
        errp(10, "in %s: failed to strdup first string", __func__);
        not_reached();
    }
    dyn_array_append_value(array, &str);    /* append pointer to the string, not the string itself */
    /**/
    buf[BUFSIZ] = '\0'; /* paranoia */
    for (i=0; i < 25; ++i) {

        /* load a string number into the generic buffer */
        snmsg(buf, BUFSIZ, "string number %" PRIdMAX, i);

        /* duplicate generic buffer as a strdup-ed (malloced) string */
        str = strdup(buf);
        if (str == NULL) {
            errp(11, "in %s: failed to strdup string for %" PRIdMAX, __func__, i);
            not_reached();
        }

        /* append the pointer to the string onto the end of the dynamic array */
        (void) dyn_array_append_value(array, &str);
    }
    /**/
    str = strdup("this is the last string");
    if (str == NULL) {
        errp(12, "in %s: failed to strdup last string", __func__);
        not_reached();
    }
    dyn_array_append_value(array, &str);    /* append pointer to the string, not the string itself */

    /*
     * print the dynamic array by using element indices
     *
     * We use a sequence of indexes where dyn_array_tell() is the count of dynamic array elements,
     * and thus is beyond the last index we will process.  We use dyn_array_value(array, char *, i)
     * to fetch the string pointer stored in the dynamic array at index i.
     */
    msg("print the dynamic array by using element indices");
    for (i = 0, p = dyn_array_value(array, char *, i);
         i < dyn_array_tell(array);
         ++i, p = dyn_array_value(array, char *, i)) {
        /* because indexes such as i are of type intmax_t, we must use PRIdMAX to format their value */
        if (p == NULL) {
            msg("    array[%" PRIdMAX "] is NULL", i); /* paranoia */
        } else {
            msg("    array[%" PRIdMAX "]: %s", i, p);
        }
    }

    /*
     * free the strdup-ed (malloced) strings that were stored in the dynamic array
     *
     * As a demo to show a different way to process the pointers stored in the dynamic array, we
     * start with the dyn_array_first() address, and free up to, but not including dyn_array_beyond().
     * Notice that q is a char **, because we are referencing the address of the string pointer that
     * is stored in the dynamic array.
     */
    msg("free the strdup-ed (malloced) strings that were stored in the dynamic array");
    for (q = dyn_array_first(array, char *); q < dyn_array_beyond(array, char *); ++q) {
        if (q != NULL && *q != NULL) {
            free(*q);       /* free the strdup-ed (malloced) string */
        }
    }

    /*
     * free the dynamic array structure
     *
     * NOTE: Because the dynamic array was allocated with a zeroize value true,
     *       the array of pointers to strings will be zeroized before the
     *       dynamic array structure if finally freed.
     */
    msg("free the dynamic array structure");
    dyn_array_free(array);

    /*
     * allocate an dynamic array of integers
     */
    msg("\nallocate an dynamic array of integers");
    array = dyn_array_create(sizeof(int), CHUNK_SIZE, INITIAL_SIZE, true);

    /*
     * push integers onto the dynamic array
     */
    msg("push 3 integers onto the dynamic array stack");
    integer = 10;
    (void) dyn_array_push(array, integer);
    integer = 20;
    (void) dyn_array_push(array, integer);
    integer = 30;
    (void) dyn_array_push(array, integer);

    /*
     * print the stack
     */
    msg("print dynamic array stack of integer values");
    for (integer_p = dyn_array_first(array, int); integer_p < dyn_array_beyond(array, int); ++integer_p) {
        if (integer_p == NULL) {
            msg("    integer stack pointer is NULL"); /* paranoia */
        } else {
            msg("    stack value: %d", *integer_p);
        }
    }

    /*
     * read the top of stack
     */
    len = dyn_array_top(array, &integer);
    if (len > 0) {
        msg("top of integer stack: %d", integer);
    } else {
        warn("%s: stack underflow error #0", __func__);
    }

    /*
     * pop the stack and toss the value
     */
    msg("pop the stack and toss the value");
    (void) dyn_array_pop(array, NULL);

    /*
     * pop the stack and print the popped value
     */
    msg("pop the stack and print the popped value");
    len = dyn_array_pop(array, &integer);
    if (len > 0) {
        msg("top of integer stack: %d", integer);
    } else {
        warn("%s: stack underflow error #1", __func__);
    }

    /*
     * print the stack again
     */
    msg("the dynamic array stack of integer values is now");
    for (integer_p = dyn_array_first(array, int); integer_p < dyn_array_beyond(array, int); ++integer_p) {
        if (integer_p == NULL) {
            msg("    integer stack pointer is NULL"); /* paranoia */
        } else {
            msg("    stack value: %d", *integer_p);
        }
    }

    /*
     * free the dynamic array structure
     *
     * NOTE: Because the dynamic array was allocated with a zeroize value true,
     *       the array of pointers to strings will be zeroized before the
     *       dynamic array structure if finally freed.
     */
    msg("free the dynamic array structure");
    dyn_array_free(array);

    /*
     * exit 0
     */
    return 0;
}
