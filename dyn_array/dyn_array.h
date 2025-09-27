/*
 * dyn_array - dynamic array facility
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


#if !defined(INCLUDE_DYN_ARRAY_H)
#    define  INCLUDE_DYN_ARRAY_H


#include <stdint.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <sys/errno.h>


/*
 * size_t MAX and MIN
 */
#if !defined(SIZE_MAX)
#define SIZE_MAX (~((size_t)0))
#endif /* SIZE_MAX */
#if !defined(SIZE_MIN)
#define SIZE_MIN ((size_t)(0))
#endif /* SIZE_MIN */


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/c_bool.h"
#include "../dbg/c_compat.h"
#include "../dbg/dbg.h"
#else
#include <c_bool.h>
#include <c_compat.h>
#include <dbg.h>
#endif


/*
 * official version
 */
#define DYN_ARRAY_VERSION "2.5.0 2024-09-26"	/* format: major.minor YYYY-MM-DD */


/*
 * dynamic array convenience macros
 *
 * Obtain an element in a dynamic array:
 *
 *	struct dyn_array *array;
 *	double value;
 *
 *      value = dyn_array_value(array, double, i);
 *
 * Obtain the address of an element in a dynamic array:
 *
 *	struct dyn_array *array;
 *	struct json *addr;
 *
 *      addr = dyn_array_addr(array, struct json, i);
 *
 * Current element count of dynamic array:
 *
 *	struct dyn_array *array;
 *	intmax_t pos;
 *
 *	pos = dyn_array_tell(array);
 *
 * Address of the first element:
 *
 *	struct dyn_array *array;
 *	struct json_member *next;
 *
 *	next = dyn_array_first(array, struct json_member);
 *
 *	WARNING: Do NOT reference unless the dynamic array contains 1 or more elements.
 *
 * Address of the element just beyond the elements in use:
 *
 *	struct dyn_array *array;
 *	struct json_member *next;
 *
 *	next = dyn_array_beyond(array, struct json_member);
 *
 *	WARNING: Do NOT reference this this address.
 *
 * Number of elements allocated in memory for the dynamic array:
 *
 *	struct dyn_array *array;
 *	intmax_t size;
 *
 *	size = dyn_array_alloced(array);
 *
 * Number of elements available (allocated but not in use) for the dynamic array:
 *
 *	struct dyn_array *array;
 *	intmax_t avail;
 *
 *	avail = dyn_array_avail(array);
 *
 * Rewind a dynamic array back to zero elements:
 *
 *	struct dyn_array *array;
 *
 *	dyn_array_rewind(array);
 *
 * Append a new element onto the end of a dynamic array ("push on the stack"):
 *
 *	struct dyn_array *array;
 *	bool data_moved;
 *	type value;
 *
 *	data_moved = dyn_array_push(array, value);
 */
#define dyn_array_value(array, type, index) (((type *)(((struct dyn_array *)(array))->data))[(index)])
#define dyn_array_addr(array, type, index) (((type *)(((struct dyn_array *)(array))->data))+(index))
#define dyn_array_tell(array) (((struct dyn_array *)(array))->count)
#define dyn_array_first(array, type) (dyn_array_addr(array, type, 0))
#define dyn_array_beyond(array, type) (dyn_array_addr(array, type, dyn_array_tell(array)))
#define dyn_array_alloced(array) (((struct dyn_array *)(array))->allocated)
#define dyn_array_avail(array) (dyn_array_alloced(array) - dyn_array_tell(array))
#define dyn_array_rewind(array) (dyn_array_seek((struct dyn_array *)(array), 0, SEEK_SET))
#define dyn_array_push(array, value_to_push) (dyn_array_append_value((array), ((void *)&(value_to_push))))


/*
 * dyn_array - a dynamic array of elements of the same type
 *
 * The dynamic array maintains both an allocated element count
 * and a number of elements in use.
 *
 * The actual storage allocated will be a number of elements
 * beyond the allocated element count, to serve as a guard chunk.
 *
 * If zeroize is true, then all allocated elements will be
 * zeroized when first allocated, and zeroized when dyn_array_free()
 * is called.
 */
struct dyn_array
{
    size_t elm_size;		/* Number of bytes for a single element */
    bool zeroize;		/* true ==> always zero newly allocated chunks, false ==> don't */
    intmax_t count;		/* Number of elements in use */
    intmax_t allocated;		/* Number of elements allocated (>= count) */
    intmax_t chunk;		/* Number of elements to expand by when allocating */
    void *data;			/* allocated dynamic array of identical things or NULL */
};


/*
 * global variables
 */
extern const char *const dyn_array_version;	/* library version format: major.minor YYYY-MM-DD */


/*
 * external allocation functions
 */
extern struct dyn_array *dyn_array_create(size_t elm_size, intmax_t chunk, intmax_t start_elm_count, bool zeroize);
extern bool dyn_array_append_value(struct dyn_array *array, void *value_to_add);
extern bool dyn_array_append_set(struct dyn_array *array, void *array_to_add_p, intmax_t count_of_elements_to_add);
extern bool dyn_array_concat_array(struct dyn_array *array, struct dyn_array *other);
extern bool dyn_array_seek(struct dyn_array *array, off_t offset, int whence);
extern void dyn_array_clear(struct dyn_array *array);
extern void dyn_array_free(struct dyn_array *array);
/**/
extern void dyn_array_qsort(struct dyn_array *array, int (*compar)(const void *, const void *));
/**/
extern intmax_t dyn_array_top(struct dyn_array *array, void *fetched_value);
extern intmax_t dyn_array_pop(struct dyn_array *array, void *fetched_value);


#if defined(NON_STANDARD_SORT)

/*
 * NON_STANDARD_SORT
 *
 * It is sad that qsort_r() is not part of the standard C library as of 2025.  Worse yet, clang libc and gnu libc
 * put the thunk argument in different positions in the comparison library.
 *
 * It is sad that both heapsort() and mergesort() part of the standard C library as of 2025.
 */
extern void dyn_array_qsort_r(struct dyn_array *array, void *thunk, int (*compar)(void *, const void *, const void *));
extern int dyn_array_heapsort(struct dyn_array *array, int (*compar)(const void *, const void *));
extern int dyn_array_mergesort(struct dyn_array *array, int (*compar)(const void *, const void *));

#endif /* NON_STANDARD_SORT */


#endif		/* INCLUDE_DYN_ARRAY_H */
