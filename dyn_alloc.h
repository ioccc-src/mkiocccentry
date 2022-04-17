/*
 * dyn_array - dynamic array facility
 *
 * Copyright (c) 2014,2015,2022 by Landon Curt Noll.  All Rights Reserved.
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


#if !defined(INCLUDE_DYN_ALLOC_H)
#    define  INCLUDE_DYN_ALLOC_H


#include <stdint.h>


/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * convenience utilities for adding or reading values of a given type to a dynamic array
 *
 *      array_p                 - pointer to a struct dyn_array
 *      type                    - type of element held in the dyn_array->data
 *      index                   - index of data to fetch (no bounds checking)
 *
 * Example:
 *      p_value = get_value(state->p_val[test_num], double, i);
 *      stat = addr_value(state->stats[test_num], struct private_stats, i);
 */
#define get_value(array_p, type, index) (((type *)(((struct dyn_array *)(array_p))->data))[(index)])
#define addr_value(array_p, type, index) (((type *)(((struct dyn_array *)(array_p))->data))+(index))


/*
 * array - a dynamic array of identical things
 *
 * Rather than write values, such as floating point values
 * as ASCII formatted numbers into a file and then later
 * same in the run (after closing the file) reopen and re-parse
 * those same files, we will append them to an array
 * and return that pointer to the array.  Then later
 * in the same run, the array can be written as ASCII formatted
 * numbers to a file if needed.
 */
struct dyn_array {
    size_t elm_size;		/* Number of bytes for a single element */
    bool zeroize;		/* true ==> always zero newly allocated chunks, false ==> don't */
    intmax_t count;		/* Number of elements in use */
    intmax_t allocated;		/* Number of elements allocated (>= count) */
    intmax_t chunk;		/* Number of elements to expand by when allocating */
    void *data;			/* allocated dynamic array of identical things or NULL */
};


/*
 * external allocation functions
 */
extern struct dyn_array *create_dyn_array(size_t elm_size, intmax_t chunk, intmax_t start_elm_count, bool zeroize);
extern bool append_value(struct dyn_array *array, void *value_to_add);
extern bool append_array(struct dyn_array *array, void *array_to_add_p, intmax_t count_of_elements_to_add);
extern void free_dyn_array(struct dyn_array *array);
extern void clear_dyn_array(struct dyn_array *array);


#endif		/* INCLUDE_DYN_ALLOC_H */
