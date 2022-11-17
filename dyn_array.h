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


#if !defined(INCLUDE_DYN_ARRAY_H)
#    define  INCLUDE_DYN_ARRAY_H


#include <stdint.h>
#include <sys/types.h>


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
#include "dbg/dbg.h"


/*
 * official version
 */
#define DYN_ALLOC_VERSION "2.0 2022-06-09"	/* format: major.minor YYYY-MM-DD */


/*
 * dynamic array convenience macros
 *
 * Obtain an element in a dynamic array:
 *
 *	struct dyn_array *array_p;
 *	double value;
 *
 *      value = dyn_array_value(array_p, double, i);
 *
 * Obtain the address of an element in a dynamic array:
 *
 *	struct dyn_array *array_p;
 *	struct json *addr;
 *
 *      addr = dyn_array_addr(array_p, struct json, i);
 *
 * Current element count of dynamic array:
 *
 *	struct dyn_array *array_p;
 *	intmax_t pos;
 *
 *	pos = dyn_array_tell(array_p);
 *
 * Address of the element just beyond the elements in use:
 *
 *	struct dyn_array *array_p;
 *	struct json_member *next;
 *
 *	next = dyn_array_beyond(array_p, struct json_member);
 *
 * Number of elements allocated in memory for the dynamic array:
 *
 *	struct dyn_array *array_p;
 *	intmax_t size;
 *
 *	size = dyn_array_alloced(array_p);
 *
 * Number of elements available (allocated but not in use) for the dynamic array:
 *
 *	struct dyn_array *array_p;
 *	intmax_t avail;
 *
 *	avail = dyn_array_avail(array_p);
 *
 * Rewind a dynamic array back to zero elements:
 *
 *	struct dyn_array *array_p;
 *
 *	dyn_array_rewind(array_p);
 */
#define dyn_array_value(array_p, type, index) (((type *)(((struct dyn_array *)(array_p))->data))[(index)])
#define dyn_array_addr(array_p, type, index) (((type *)(((struct dyn_array *)(array_p))->data))+(index))
#define dyn_array_tell(array_p) (((struct dyn_array *)(array_p))->count)
#define dyn_array_beyond(array_p, type) (dyn_array_addr(array_p, type, dyn_array_tell(array_p)))
#define dyn_array_alloced(array_p) (((struct dyn_array *)(array_p))->allocated)
#define dyn_array_avail(array_p) (dyn_array_alloced(array_p) - dyn_array_tell(array_p))
#define dyn_array_rewind(array_p) (dyn_array_seek((struct dyn_array *)(array_p), 0, SEEK_SET))


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
 * external allocation functions
 */
extern struct dyn_array *dyn_array_create(size_t elm_size, intmax_t chunk, intmax_t start_elm_count, bool zeroize);
extern bool dyn_array_append_value(struct dyn_array *array, void *value_to_add);
extern bool dyn_array_append_set(struct dyn_array *array, void *array_to_add_p, intmax_t count_of_elements_to_add);
extern bool dyn_array_concat_array(struct dyn_array *array, struct dyn_array *other);
extern bool dyn_array_seek(struct dyn_array *array, off_t offset, int whence);
extern void dyn_array_clear(struct dyn_array *array);
extern void dyn_array_free(struct dyn_array *array);


#endif		/* INCLUDE_DYN_ARRAY_H */
