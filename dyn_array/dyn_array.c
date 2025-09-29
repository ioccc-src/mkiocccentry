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


/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


/*
 * dyn_array - dynamic array facility
 */
#include "dyn_array.h"


/*
 * internal enum for relative addresses
 */
enum ptr_compare {
    PTR_COMPARE_UNSET = 0,	/* pointer compare not been determined - must be 0 */
    PTR_BELOW_ADDR,		/* pointer is below (before) a given address */
    PTR_EQ,			/* pointer is the same as a given address */
    PTR_ABOVE_ADDR,		/* pointer is above (beyond) a given address */
};

/*
 * internal enum for cases outlined in dyn_array_append_set()
 */
enum move_case {
    MOVE_CASE_UNSET = 0,	/* move case has not been determined - must be 0 */
    MOVE_CASE_OUTSIDE,		/* case 1: data is entirely outside allocated area */
    MOVE_CASE_INSIDE,		/* case 2: data is completely inside allocated area */
    MOVE_CASE_BEFORE_INTO,	/* case 3: data starts before and goes into allocated area */
    MOVE_CASE_BEFORE_IN_BEYOND,	/* case 4: data starts before and goes and then beyond allocated area */
    MOVE_CASE_IN_BEYOND,	/* case 5: data starts inside allocated area and goes beyond it */
};


/*
 * global variables
 */
const char *const dyn_array_version = DYN_ARRAY_VERSION;	/* library version format: major.minor YYYY-MM-DD */


/*
 * external allocation functions
 */
static bool dyn_array_grow(struct dyn_array *array, intmax_t elms_to_allocate);
static enum ptr_compare compare_addr(void *a, void *b);
static enum move_case determine_move_case(void *first_alloc, void *last_alloc, void *first_add, void *last_add);
static char const *move_case_name(enum move_case mv_case);


/*
 * dyn_array_grow - grow the allocation of a dynamic array by a specified number of elements
 *
 * given:
 *      array			pointer to the dynamic array
 *      elms_to_allocate	number of elements to allocate space for
 *
 * returns:
 *	true ==> address of the array of elements moved during realloc()
 *	false ==> address of the elements array did not move
 *
 * The dynamic array allocation will be expanded by new_elem elements.  That data will
 * be zeroized if array->zeroize is true.
 *
 * We will always allocate one more chunk of that requested to serve as a guard chunk.
 * This extra guard chunk will NOT be listed in the total element allocation count.
 *
 * NOTE: This function does not return on error.
 */
static bool
dyn_array_grow(struct dyn_array *array, intmax_t elms_to_allocate)
{
    void *data;			/* Reallocated array */
    intmax_t old_allocated;	/* Old number of elements allocated */
    intmax_t new_allocated;	/* New number of elements allocated */
    intmax_t old_bytes;		/* Old size of data in dynamic array */
    intmax_t new_bytes;		/* New size of data in dynamic array after allocation */
    uint8_t *p;			/* Pointer to the beginning of the new allocated space */
    bool moved = false;		/* true ==> location of the elements array moved during realloc() */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(50, __func__, "array arg is NULL");
	not_reached();
    }
    if (elms_to_allocate <= 0) {
	err(51, __func__, "elms_to_allocate arg must be > 0: %jd", elms_to_allocate);
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(52, __func__, "array->data for dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(53, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(54, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(55, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(56, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * determine the size of the realloced area
     */
    old_allocated = array->allocated;
    new_allocated = old_allocated + elms_to_allocate;
    old_bytes = old_allocated * (intmax_t)array->elm_size;
    /* +array->chunk for guard chunk */
    new_bytes = (new_allocated+array->chunk) * (intmax_t)array->elm_size;

    /*
     * firewall - check if new_bytes fits in a size_t variable
     */
    if ((double)new_bytes > (double)SIZE_MAX) {
	err(57, __func__, "the total number of bytes occupied by %jd elements of size %ju is too big "
			  "and does not fit the bounds of a size_t [%jd,%jd]",
			  new_allocated, (uintmax_t)array->elm_size,
			  (intmax_t)SIZE_MIN, (intmax_t)SIZE_MAX);
	not_reached();
    }

    /*
     * reallocate array
     */
    errno = 0;			/* pre-clear errno for errp() */
    data = realloc(array->data, (size_t)new_bytes);
    if (data == NULL) {
	errp(58, __func__, "failed to reallocate the dynamic array from a size of %jd bytes "
			   "to a size of %jd bytes",
			   old_bytes, new_bytes);
	not_reached();
    }
    if (array->data != data) {
	moved = true;
    }
    array->data = data;
    array->allocated = new_allocated;
    if (moved == true && dbg_allowed(DBG_V5_HIGH)) {
	dbg(DBG_V5_HIGH, "in %s(array, %jd): moved-place: allocated: %jd elements of size: %ju in use: %jd",
			  __func__, elms_to_allocate,
			  dyn_array_alloced(array),
			  (uintmax_t)array->elm_size,
			  dyn_array_tell(array));
    } else if (dbg_allowed(DBG_V6_HIGH)) {
	dbg(DBG_V6_HIGH, "in %s(array, %jd): in-place: allocated: %jd elements of size: %ju in use: %jd",
			   __func__, elms_to_allocate,
			   dyn_array_alloced(array),
			   (uintmax_t)array->elm_size,
			   dyn_array_tell(array));
    }

    /*
     * Zeroize new elements if requested
     */
    if (array->zeroize == true) {
	    p = (uint8_t *) (array->data) + old_bytes;
	    /* +array->chunk for guard chunk */
	    memset(p, 0, (elms_to_allocate+array->chunk) * (intmax_t)array->elm_size);
    }

    return moved;
}


/*
 * compare_addr - compare two addresses
 *
 * given:
 *	a	- First address to compare
 *	b	- Second address to compare
 *
 * returns:
 *	PTR_BELOW_ADDR if a < b,
 *	PTR_EQ if a == b,
 *	else PTR_ABOVE_ADDR if a > b
 */
static enum ptr_compare
compare_addr(void *a, void *b)
{
    /*
     * compare addresses
     */
    if (a < b) {
	return PTR_BELOW_ADDR;
    } else if (a == b) {
	return PTR_EQ;
    }
    return PTR_ABOVE_ADDR;
}


/*
 * determine_move_case - determine data our move case
 *
 * We need to be very careful if the data we are adding is also part of the allocated data
 * in the dynamic array.  This is because if as part of growing data (via the realloc() inside
 * the dyn_array_grow() function), we move the data, then we must handle the case that all
 * or part of the data we are adding will also move.
 *
 * If we did not handle that case, then calls such as:
 *
 *	dyn_array_append_set(array, array->data, array->count)
 *
 * could result in a core dump due to the fact that the caller's pointer, array->data,
 * no longer points to the correct location due to the data moving.
 *
 * So we must first determine if the data that is being added is within the existing
 * allocated data of the dynamic array, and if it is, change from where we are moving it.
 *
 * We must consider 5 cases:
 *
 * case 1 (MOVE_CASE_OUTSIDE): data completely outside of the dynamic array's allocated data
 * case 2 (MOVE_CASE_INSIDE): data completely inside of the dynamic array's allocated data
 * case 3 (MOVE_CASE_BEFORE_INTO): data starts outside and ends inside of the dynamic array's allocated data
 * case 4 (MOVE_CASE_BEFORE_IN_BEYOND) : data starts before, includes all, and goes beyond allocated data
 * case 5 (MOVE_CASE_IN_BEYOND): data starts inside of allocated data, and goes beyond allocated data
 *
 * The following assumes that dyn_array_grow() moves the dynamic array's allocated data:
 *
 * MOVE_CASE_OUTSIDE:
 * For case 1, any possible realloc does not impact the data being added.  No special handling needed.
 *
 * MOVE_CASE_INSIDE:
 * For case 2, we need to note the start of data, relative to the start of the allocated data.
 * Then if data is moved, recompute a new start of data to be added.
 *
 * MOVE_CASE_BEFORE_INTO:
 * For case 3, the data outside and before the dynamic array's allocated data can be added as it is.
 * However for the data that is inside the dynamic array's allocated data, we
 * need to add the rest of the data from the moved location.
 *
 * MOVE_CASE_BEFORE_IN_BEYOND:
 * For case 4, the data outside and before the dynamic array's allocated data can be added as it is.
 * The data that is beyond the end of allocated data can be added as it is.
 * However we need to add the dynamic array's allocated data from the moved location.
 *
 * MOVE_CASE_IN_BEYOND:
 * For case 5, the data that is beyond the end of allocated data can be added as it is.
 * However the data that is inside of the dynamic array's allocated data needs to be
 * added from the moved location.
 *
 * MOVE_CASE_UNSET:
 * BTW: case 0 is reserved for the actual case not being set.
 *
 * Because we will not know if dyn_array_grow() will move the data, we must determine our case now,
 * and if the data does move, make changes to the memmove() call later for cases
 * 1 through 4.
 *
 * given:
 *	first_alloc	starting address of allocated data from a dynamic array
 *	last_alloc	address of last byte allocated data from a dynamic array
 *	first_add	starting address of data to add
 *	last_add	address of last byte of data to add
 *
 * returns:
 *	MOVE_CASE_OUTSIDE ==> move case 1
 *	MOVE_CASE_INSIDE ==> move case 2
 *	MOVE_CASE_BEFORE_INTO ==> move case 3
 *	MOVE_CASE_BEFORE_IN_BEYOND ==> move case 4
 *	MOVE_CASE_IN_BEYOND ==> move case 5
 */
static enum move_case
determine_move_case(void *first_alloc, void *last_alloc, void *first_add, void *last_add)
{
    enum move_case ret = MOVE_CASE_UNSET;		/* move case to return */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (first_alloc == NULL) {
	err(59, __func__, "first_alloc arg is NULL");
	not_reached();
    }
    if (last_alloc == NULL) {
	err(60, __func__, "last_alloc arg is NULL");
	not_reached();
    }
    if (first_add == NULL) {
	err(61, __func__, "first_add arg is NULL");
	not_reached();
    }
    if (last_add == NULL) {
	err(62, __func__, "last_add arg is NULL");
	not_reached();
    }
    if (dbg_allowed(DBG_V5_HIGH)) {
	dbg(DBG_V5_HIGH, "in %s: first_alloc: %p last_alloc: %p first_add: %p last_add: %p",
		      __func__, first_alloc, last_alloc, first_add, last_add);
    }
    if (first_alloc > last_alloc) {
	err(63, __func__, "first_alloc: %p > last_alloc: %p", first_alloc, last_alloc);
	not_reached();
    }
    if (first_add > last_add) {
	err(64, __func__, "first_add: %p > last_add: %p", first_add, last_add);
	not_reached();
    }

    /*
     * determine move case
     */
    switch (compare_addr(first_add, first_alloc)) {

    case PTR_BELOW_ADDR:	/* data to add starts before allocated */

	/*
	 * case: data to add starts before allocated
	 */
	if (dbg_allowed(DBG_V5_HIGH)) {
	    dbg(DBG_V5_HIGH, "in %s: first_add: %p < first_alloc: %p",
			   __func__, first_add, first_alloc);
	}
	switch (compare_addr(last_add, first_alloc)) {

	case PTR_BELOW_ADDR:	/* all of data is before allocated */
	    ret = MOVE_CASE_OUTSIDE;
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: ret #0: %s: last_add: %p < first_alloc: %p",
			       __func__, move_case_name(ret), last_add, first_alloc);
	    }
	    break;

	case PTR_EQ:		/* only last byte of data is allocated */
	    ret = MOVE_CASE_BEFORE_INTO;
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: ret #1: %s: last_add: %p == first_alloc: %p",
			       __func__, move_case_name(ret), last_add, first_alloc);
	    }
	    break;

	case PTR_ABOVE_ADDR:	/* last byte to add is beyond first allocated */

	    /*
	     * case: data to add starts before allocated AND last byte to add is beyond first allocated
	     */
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: last_add: %p > first_alloc: %p",
			       __func__, last_add, first_alloc);
	    }
	    switch (compare_addr(last_add, last_alloc)) {

	    case PTR_BELOW_ADDR:	/* data ends before end of allocated */
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: last_add: %p < last_alloc: %p",
				  __func__, last_add, last_alloc);
		}
		ret = MOVE_CASE_BEFORE_INTO;
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: ret #2: %s: last_add: %p < last_alloc: %p",
				  __func__, move_case_name(ret), last_add, last_alloc);
		}
		break;

	    case PTR_EQ:		/* end of data matches end of allocated */
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: last_add: %p == last_alloc: %p",
				  __func__, last_add, last_alloc);
		}
		ret = MOVE_CASE_BEFORE_INTO;
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: ret #3: %s: last_add: %p == last_alloc: %p",
				  __func__, move_case_name(ret), last_add, last_alloc);
		}
		break;

	    case PTR_ABOVE_ADDR:	/* data ends beyond end of allocated */
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: last_add: %p > last_alloc: %p",
				  __func__, last_add, last_alloc);
		}
		ret = MOVE_CASE_BEFORE_IN_BEYOND;
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: ret #4: %s: last_add: %p > last_alloc: %p",
				  __func__, move_case_name(ret), last_add, last_alloc);
		}
		break;

	    default:
		err(65, __func__, "compare_addr(%p, %p) #0 returned unknown enum value", last_add, last_alloc);
		not_reached();
	    }
	    break;

	default:
	    err(66, __func__, "compare_addr(%p, %p) #1 returned unknown enum value", last_add, first_alloc);
	    not_reached();
	}
	break;

    case PTR_EQ:		/* data to add matches allocated */

	/*
	 * case: data starts at beginning of allocated
	 */
	if (dbg_allowed(DBG_V5_HIGH)) {
	    dbg(DBG_V5_HIGH, "in %s: first_add: %p == first_alloc: %p",
			  __func__, first_add, first_alloc);
	}
	switch (compare_addr(last_add, last_alloc)) {

	case PTR_BELOW_ADDR:	/* data ends before end of allocated */
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: last_add: %p < last_alloc: %p",
			      __func__, last_add, last_alloc);
	    }
	    ret = MOVE_CASE_INSIDE;
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: ret #5: %s: last_add: %p < last_alloc: %p",
			      __func__, move_case_name(ret), last_add, last_alloc);
	    }
	    break;

	case PTR_EQ:		/* data ends at end of allocated */
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: last_add: %p == last_alloc: %p",
			      __func__, last_add, last_alloc);
	    }
	    ret = MOVE_CASE_INSIDE;
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: ret #6: %s: last_add: %p == last_alloc: %p",
			       __func__, move_case_name(ret), last_add, last_alloc);
	    }
	    break;

	case PTR_ABOVE_ADDR:	/* data ends beyond end of allocated */
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: last_add: %p > last_alloc: %p",
			      __func__, last_add, last_alloc);
	    }
	    ret = MOVE_CASE_IN_BEYOND;
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: ret #7: %s: last_add: %p > last_alloc: %p",
			      __func__, move_case_name(ret), last_add, last_alloc);
	    }
	    break;

	default:
	    err(67, __func__, "compare_addr(%p, %p) #2 returned unknown enum value", last_add, last_alloc);
	    not_reached();
	}
	break;

    case PTR_ABOVE_ADDR:	/* data to add starts after allocated */

	/*
	 * case: data to add starts after allocated starts
	 */
	if (dbg_allowed(DBG_V5_HIGH)) {
	    dbg(DBG_V5_HIGH, "in %s: first_add: %p > first_alloc: %p",
			  __func__, first_add, first_alloc);
	}
	switch (compare_addr(first_add, last_alloc)) {

	case PTR_BELOW_ADDR:	/* data starts before end of allocated */
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: first_add: %p < last_alloc: %p",
			      __func__, first_add, last_alloc);
	    }
	    switch (compare_addr(last_add, last_alloc)) {
	    case PTR_BELOW_ADDR:	/* data ends before end of allocated */
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: last_add: %p < last_alloc: %p",
				  __func__, last_add, last_alloc);
		}
		ret = MOVE_CASE_INSIDE;
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: ret #8: %s: last_add: %p < last_alloc: %p",
				   __func__, move_case_name(ret), last_add, last_alloc);
		}
		break;

	    case PTR_EQ:		/* data ends matches end of allocated */
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: last_add: %p == last_alloc: %p",
				  __func__, last_add, last_alloc);
		}
		ret = MOVE_CASE_INSIDE;
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: ret #9: %s: last_add: %p == last_alloc: %p",
				   __func__, move_case_name(ret), last_add, last_alloc);
		}
		break;

	    case PTR_ABOVE_ADDR:	/* data ends beyond end of allocated */
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: last_add: %p > last_alloc: %p",
				  __func__, last_add, last_alloc);
		}
		ret = MOVE_CASE_IN_BEYOND;
		if (dbg_allowed(DBG_V5_HIGH)) {
		    dbg(DBG_V5_HIGH, "in %s: ret #10: %s: last_add: %p > last_alloc: %p",
				   __func__, move_case_name(ret), last_add, last_alloc);
		}
		break;

	    default:
		err(68, __func__, "compare_addr(%p, %p) #3 returned unknown enum value", first_add, last_alloc);
		not_reached();
	    }
	    break;

	case PTR_EQ:		/* data start matches end of allocated */
	    ret = MOVE_CASE_INSIDE;
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: ret #11: %s: first_add: %p == last_alloc: %p",
			       __func__, move_case_name(ret), first_add, last_alloc);
	    }
	    break;

	case PTR_ABOVE_ADDR:	/* data start beyond end of allocated */
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: first_add: %p > last_alloc: %p",
			      __func__, first_add, last_alloc);
	    }
	    ret = MOVE_CASE_OUTSIDE;
	    if (dbg_allowed(DBG_V5_HIGH)) {
		dbg(DBG_V5_HIGH, "in %s: ret #12: %s: first_add: %p > last_alloc: %p",
			      __func__, move_case_name(ret), first_add, last_alloc);
	    }
	    break;

	default:
	    err(69, __func__, "compare_addr(%p, %p) #4 returned unknown enum value", first_add, last_alloc);
	    not_reached();
	}
	break;

    default:
	err(70, __func__, "compare_addr(%p, %p) #5 returned unknown enum value", first_add, first_alloc);
	not_reached();
    }

    /*
     * return move case
     */
    return ret;
}


/*
 * move_case_name - return read_only move case name string
 *
 * given:
 *	mv_case		enum move_case
 *
 * returns:
 *	constant read-only string with the enum move_case name
 */
static char const *
move_case_name(enum move_case mv_case)
{
    char const *ret = "((unknown move_case enum))";		/* enum move_case name to return */

    /*
     * examine move case
     */
    switch (mv_case) {
    case MOVE_CASE_OUTSIDE:
	ret = "MOVE_CASE_OUTSIDE";
	break;
    case MOVE_CASE_INSIDE:
	ret = "MOVE_CASE_INSIDE";
	break;
    case MOVE_CASE_BEFORE_INTO:
	ret = "MOVE_CASE_BEFORE_INTO";
	break;
    case MOVE_CASE_BEFORE_IN_BEYOND:
	ret = "MOVE_CASE_BEFORE_IN_BEYOND";
	break;
    case MOVE_CASE_IN_BEYOND:
	ret = "MOVE_CASE_IN_BEYOND";
	break;
    default:
	warn(__func__, "unknown enum move_case value: %d", (int)mv_case);
	break;
    }

    /*
     * return move case name
     */
    return ret;
}


/*
 * dyn_array_create - create a dynamic array
 *
 * given:
 *      elm_size        - size of an element
 *      chunk           - fixed number of elements to expand by when allocating
 *      start_elm_count - starting number of elements to allocate
 *      zeroize         - true ==> always zero newly allocated chunks, false ==> don't
 *
 * returns:
 *      initialized (to zero) empty dynamic array
 *
 * We will always allocate one more chunk of that requested to serve as a guard chunk.
 * This extra guard chunk will NOT be listed in the total element allocation count.
 *
 * NOTE: This function does not return on error.
 */
struct dyn_array *
dyn_array_create(size_t elm_size, intmax_t chunk, intmax_t start_elm_count, bool zeroize)
{
    struct dyn_array *ret;		/* Created dynamic array to return */
    intmax_t number_of_bytes;		/* Total number of bytes occupied by the initialized array */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (elm_size <= 0) {
	err(71, __func__, "elm_size must be > 0: %ju", (uintmax_t)elm_size);
	not_reached();
    }
    if (chunk <= 0) {
	err(72, __func__, "chunk must be > 0: %jd", chunk);
	not_reached();
    }
    if (start_elm_count <= 0) {
	err(73, __func__, "start_elm_count must be > 0: %jd", start_elm_count);
	not_reached();
    }

    /*
     * Allocate new dynamic array
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = calloc(1, sizeof(struct dyn_array));
    if (ret == NULL) {
	errp(74, __func__, "cannot calloc %ju bytes for a struct dyn_array",
			   (uintmax_t)sizeof(struct dyn_array));
	not_reached();
    }

    /*
     * Initialize empty dynamic array
     * Start with a dynamic array with allocated enough chunks to hold at least start_elm_count elements
     */
    ret->elm_size = (intmax_t)elm_size;
    ret->zeroize = zeroize;
    /* Allocated array is empty */
    ret->count = 0;
    /* Allocate a number of elements multiple of chunk */
    ret->allocated = chunk * ((start_elm_count + (chunk - 1)) / chunk);
    ret->chunk = chunk;

    /*
     * determine the size of the allocated area
     */
    /* +chunk for guard chunk */
    number_of_bytes = (ret->allocated+chunk) * (intmax_t)elm_size;

    errno = 0;			/* pre-clear errno for errp() */
    ret->data = malloc((size_t)number_of_bytes);
    if (ret->data == NULL) {
	/* +chunk for guard chunk */
	errp(75, __func__, "cannot malloc %jd elements of %ju bytes each for dyn_array->data",
			   (ret->allocated+chunk), (uintmax_t)elm_size);
	not_reached();
    }

    /*
     * Zeroize allocated data
     */
    if (ret->zeroize == true) {
	memset(ret->data, 0, number_of_bytes);
    }

    /*
     * Return newly allocated array
     */
    if (dbg_allowed(DBG_V5_HIGH)) {
	dbg(DBG_V5_HIGH, "in %s(%ju, %jd, %jd, %s): initialized empty dynamic array, allocated: %jd elements of size: %ju",
		      __func__, (uintmax_t)elm_size, chunk, start_elm_count,
		      booltostr(zeroize),
		      dyn_array_alloced(ret), (uintmax_t)ret->elm_size);
    }
    return ret;
}


/*
 * dyn_array_append_value - append a value of a given type onto the end of the dynamic array
 *
 * given:
 *      array           - pointer to the dynamic array
 *      value_to_add    - pointer to the value to add to the end of the dynamic array
 *
 * returns:
 *	true ==> address of the array of elements moved during realloc()
 *	false ==> address of the elements array did not move
 *
 * We will add a value of a given type onto the end of the dynamic array.
 * We will grow the dynamic array if all allocated values are used.
 * If after adding the value, all allocated values are used, we will grow
 * the dynamic array as a firewall.
 *
 * NOTE: This function does not return on error.
 */
bool
dyn_array_append_value(struct dyn_array *array, void *value_to_add)
{
    bool moved = false;		/* true ==> location of the elements array moved during realloc() */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(76, __func__, "array arg is NULL");
	not_reached();
    }
    if (value_to_add == NULL) {
	err(77, __func__, "value_to_add arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(78, __func__, "array->data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(79, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(80, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(81, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(82, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * copy data as if we have a set of 1
     */
    moved = dyn_array_append_set(array, value_to_add, 1);

    /* return array moved condition */
    return moved;
}


/*
 * dyn_array_append_set - append a set values of a given array onto the end of the dynamic array
 *
 * given:
 *      array				- pointer to the dynamic array
 *      array_to_add_p			- pointer to the array to add to the end of the dynamic array
 *      count_of_elements_to_add	- number of element to add to the dynamic array
 *
 * returns:
 *	true ==> address of the array of elements moved during realloc()
 *	false ==> address of the elements array did not move
 *
 * We will add a set (non-dynamic array) of values of the given array (which are of a given type)
 * onto the end of the dynamic array. We will grow the dynamic array if all allocated values are used.
 * If after adding the values of the array, all allocated values are used, we will grow
 * the dynamic array as a firewall.
 *
 * NOTE: This function does not return on error.
 */
bool
dyn_array_append_set(struct dyn_array *array, void *array_to_add_p, intmax_t count_of_elements_to_add)
{
    intmax_t available_empty_elements;
    intmax_t required_elements_to_allocate;
    enum move_case mv_case = MOVE_CASE_UNSET;	/* assume no special move case */
    uint8_t *last_add_byte = NULL;		/* last byte of the data to add, if != NULL */
    uint8_t *last_alloc_byte = NULL;		/* last byte of the allocated data of the dynamic array */
    intmax_t data_first_offset = 0;		/* offset of array_to_add_p from dynamic array allocated data */
    intmax_t data_last_offset = 0;		/* offset of end of data to add from dynamic array allocated data */
    intmax_t data_size = 0;			/* length in bytes, of the data to move */
    intmax_t alloc_size = 0;			/* initial size of dynamic array allocated data */
    bool moved = false;				/* true ==> location of the elements array moved during realloc() */
    uint8_t *beyond = NULL;			/* next address in dynamic array available for adding data */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(83, __func__, "array arg is NULL");
	not_reached();
    }
    if (array_to_add_p == NULL) {
	err(84, __func__, "array_to_add_p arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(85, __func__, "array->data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(86, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(87, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(88, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(89, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * Expand dynamic array if needed
     */
    available_empty_elements = dyn_array_avail(array);
    data_size = (intmax_t)array->elm_size * count_of_elements_to_add;
    alloc_size = (intmax_t)array->elm_size * array->allocated;
    if (count_of_elements_to_add > available_empty_elements) {

	/*
	 * determine our move case
	 */
	if (dbg_allowed(DBG_V4_HIGH)) {
	    dbg(DBG_V4_HIGH, "in %s(array: %p, array_to_add_p: %p, %jd)", __func__,
			 (void *)array, (void *)array_to_add_p, count_of_elements_to_add);
	}
	last_add_byte = (uint8_t *)(array_to_add_p) + (data_size - 1);
	last_alloc_byte = (uint8_t *)(array->data) + (alloc_size - 1);
	data_first_offset = (uint8_t *)(array_to_add_p) - (uint8_t *)(array->data);
	data_last_offset = (uint8_t *)(last_add_byte) - (uint8_t *)(array->data);
	if (dbg_allowed(DBG_V5_HIGH)) {
	    dbg(DBG_V5_HIGH, "in %s: array->data: %p", __func__, (void *)array->data);
	    dbg(DBG_V5_HIGH, "in %s: alloc_size: %jd", __func__, alloc_size);
	    dbg(DBG_V5_HIGH, "in %s: last_alloc_byte: %p", __func__, (void *)last_alloc_byte);
	    dbg(DBG_V5_HIGH, "in %s: array_to_add_p: %p", __func__, (void *)array_to_add_p);
	    dbg(DBG_V5_HIGH, "in %s: data_size: %jd", __func__, data_size);
	    dbg(DBG_V5_HIGH, "in %s: last_add_byte: %p", __func__, (void *)last_add_byte);
	    dbg(DBG_V5_HIGH, "in %s: data_first_offset: %ju", __func__, data_first_offset);
	    dbg(DBG_V5_HIGH, "in %s: data_last_offset: %ju", __func__, data_last_offset);
	}
	mv_case = determine_move_case(array->data, last_alloc_byte, array_to_add_p, last_add_byte);
	if (dbg_allowed(DBG_V5_HIGH)) {
	    dbg(DBG_V5_HIGH, "in %s: move case: %s", __func__, move_case_name(mv_case));
	}

	/*
	 * determine the new allocated data size that as need
	 */
	required_elements_to_allocate = array->chunk *
		((count_of_elements_to_add - available_empty_elements + (array->chunk - 1)) / array->chunk);

	/*
	 * expand the allocated data and note of the data moved
	 */
	moved = dyn_array_grow(array, required_elements_to_allocate);
    }

    /*
     * We know the dynamic array has enough room to append, so append the new array
     *
     * We append depending on if the data moved during a possible dyn_array_grow() / realloc() call
     * as well of the move type.
     */
    beyond = (uint8_t *)(array->data) + (array->count * (intmax_t)array->elm_size);
    if (moved == false || mv_case == MOVE_CASE_UNSET || mv_case == MOVE_CASE_OUTSIDE) {

	/*
	 * Either the dynamic array had enough allocated memory to receive the new data,
	 * or the expanded dynamic array did not require the data to be moved,
	 * or the data was not within the allocated data of the dynamic array.
	 * Therefore, we can simply move the data to the end of the dynamic array.
	 */

	/* append data to the end of the dynamic array */
	memmove(beyond, array_to_add_p, data_size);

    } else {

	uint8_t *new_array_to_add_p = NULL;	/* newly moved data to append, location */
	intmax_t pre_length = 0;		/* length of data before dynamic array allocated data, or 0 */
	intmax_t in_length = 0;			/* length of dynamic array allocated data, or 0 */
	intmax_t post_length = 0;		/* length of data after dynamic array allocated data, or 0 */

	/*
	 * move data based on the move case
	 *
	 * The dynamic array had to be expanded via dyn_array_grow(), and the
	 * resulting realloc() moved the data
	 */
	switch (mv_case) {

	case MOVE_CASE_UNSET:
	    err(90, __func__, "mv_case: %s but previous if says this is impossible",
			       move_case_name(mv_case));
	    not_reached();
	    break;

	case MOVE_CASE_OUTSIDE:
	    err(91, __func__, "mv_case: %s but previous if says this is impossible",
			       move_case_name(mv_case));
	    not_reached();
	    break;

	case MOVE_CASE_INSIDE:

	    /* firewall */
	    if (data_first_offset < 0) {
		err(92, __func__, "mv_case == %s but data_first_offset < 0: %ju",
				  move_case_name(mv_case), data_first_offset);
		not_reached();
	    }

	    /* append the entire chunk of data */
	    new_array_to_add_p = (uint8_t *)(array->data) + data_first_offset;	/* new starting location */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p, new_array_to_add_p: %p, data_size: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, (void *)new_array_to_add_p, data_size);
	    }
	    memmove((void *)beyond, (void *)new_array_to_add_p, data_size);
	    break;

	case MOVE_CASE_BEFORE_INTO:

	    /* firewall */
	    if (data_first_offset >= 0) {
		err(93, __func__, "mv_case: %s but data_first_offset >= 0: %ju",
				  move_case_name(mv_case), data_first_offset);
		not_reached();
	    }
	    pre_length = -data_first_offset;
	    if (pre_length <= 0) {
		err(94, __func__, "mv_case: %s but pre_length: %jd <= 0",
				  move_case_name(mv_case), pre_length);
		not_reached();
	    }
	    in_length = data_size - pre_length;
	    if (in_length <= 0) {
		err(95, __func__, "mv_case: %s but in_length: %jd <= 0",
				  move_case_name(mv_case), in_length);
		not_reached();
	    }

	    /* append data that is before the dynamic array allocated data */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p, new_array_to_add_p: %p, pre_length: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, (void *)new_array_to_add_p, pre_length);
	    }
	    memmove((void *)beyond, (void *)array_to_add_p, pre_length);

	    /* append data that moved with the dynamic array allocated data */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p, array->data: %p, in_length: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, array->data, in_length);
	    }
	    memmove((void *)beyond, array->data, in_length);
	    break;

	case MOVE_CASE_BEFORE_IN_BEYOND:

	    /* firewall */
	    if (data_first_offset >= 0) {
		err(96, __func__, "mv_case: %s but data_first_offset >= 0: %ju",
				  move_case_name(mv_case), data_first_offset);
		not_reached();
	    }
	    pre_length = -data_first_offset;
	    if (pre_length <= 0) {
		err(97, __func__, "mv_case: %s but pre_length: %jd <= 0",
				  move_case_name(mv_case), pre_length);
		not_reached();
	    } else if (pre_length >= data_size) {
		err(98, __func__, "mv_case: %s but pre_length: %jd >= data_size: %jd",
				  move_case_name(mv_case), pre_length, data_size);
		not_reached();
	    }
	    post_length = data_size - pre_length - alloc_size;
	    if (post_length <= 0) {
		err(99, __func__, "mv_case: %s but post_length: %jd <= 0",
				  move_case_name(mv_case), post_length);
		not_reached();
	    }

	    /* append data that is before the dynamic array allocated data */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p array_to_add_p: %p, pre_length: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, (void *)array_to_add_p, pre_length);
	    }
	    memmove((void *)beyond, (void *)array_to_add_p, pre_length);

	    /* append the section that was part of the previous dynamic array allocated data */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p array->data: %p, alloc_size: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, array->data, alloc_size);
	    }
	    memmove((void *)beyond, array->data, alloc_size);

	    /* append the data that was beyond the end of the previous dynamic array allocated data */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p array_to_add_p+pre_length+data_size: %p, post_length: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, (void *)((uint8_t *)(array_to_add_p) + pre_length+data_size), post_length);
	    }
	    memmove((void *)beyond, (void *)((uint8_t *)(array_to_add_p) + pre_length+data_size), post_length);
	    break;

	case MOVE_CASE_IN_BEYOND:

	    /* firewall */
	    if (data_first_offset < 0) {
		err(100, __func__, "mv_case: %s but data_first_offset < 0: %ju",
				  move_case_name(mv_case), data_first_offset);
		not_reached();
	    }
	    in_length = data_size - (alloc_size - data_first_offset);
	    if (in_length <= 0) {
		err(101, __func__, "mv_case: %s but in_length: %jd <= 0",
				  move_case_name(mv_case), in_length);
		not_reached();
	    }
	    post_length = data_size - in_length;
	    if (post_length <= 0) {
		err(102, __func__, "mv_case: %s but post_length: %jd <= 0",
				  move_case_name(mv_case), post_length);
		not_reached();
	    }

	    /* append the data that stated inside the previous dynamic array allocated data */
	    new_array_to_add_p = (uint8_t *)(array->data) + data_first_offset;	/* new stating location */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p new_array_to_add_p: %p, in_length: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, (void *)new_array_to_add_p, in_length);
	    }
	    memmove((void *)beyond, (void *)new_array_to_add_p, in_length);

	    /* append the data that was beyond the end of the previous dynamic array allocated data */
	    if (dbg_allowed(DBG_V4_HIGH)) {
		dbg(DBG_V4_HIGH, "mv_case: %s special "
			     "memmove(beyond: %p array_to_add_p+in_length: %p, post_length: %ju)",
			     move_case_name(mv_case),
			     (void *)beyond, (void *)((uint8_t *)(array_to_add_p) + in_length), post_length);
	    }
	    memmove((void *)beyond, (void *)((uint8_t *)(array_to_add_p) + in_length), post_length);
	    break;

	default:
	    err(103, __func__, "mv_case is an unknown enum value: %d", (int)mv_case);
	    not_reached();
	    break;
	}
    }

    /*
     * increase the amount of data that is in use, in the dynamic array
     */
    array->count += count_of_elements_to_add;
    if (dbg_allowed(DBG_V6_HIGH)) {
	dbg(DBG_V6_HIGH, "in %s(array: %p, array_to_add_p %p, %jd): %s: allocated: %jd elements of size: %ju in use: %jd",
		       __func__, (void *)array, (void *)array_to_add_p,
		       (intmax_t)count_of_elements_to_add,
		       (moved == true ? "moved" : "in-place"),
		       dyn_array_alloced(array),
		       (uintmax_t)array->elm_size,
		       dyn_array_tell(array));
    }

    /* return array moved condition */
    return moved;
}


/*
 * dyn_array_concat_array - concatenate a dynamic array with another dynamic array
 *
 * given:
 *      array			- pointer to the dynamic array
 *	other			- other dynamic array to concatenate onto array
 *
 * returns:
 *	true ==> address of the array of elements moved during realloc()
 *	false ==> address of the elements array did not move
 *
 * We will add a set (non-dynamic array) of values of the given array (which are of a given type)
 * onto the end of the dynamic array. We will grow the dynamic array if all allocated values are used.
 *
 * We will take the contents of the other dynamic array and concatenate its values onto
 * the first dynamic array.  The contents of the other dynamic array will be duplicated onto the
 * first dynamic array.
 *
 * This function does nothing if the other dynamic array is empty.
 *
 * The contents of the other dynamic array is not modified, nor freed by this function.
 *
 * If after adding the values of the array, all allocated values are used, we will grow
 * the dynamic array as a firewall.
 *
 * NOTE: This function does not return on error.
 */
bool
dyn_array_concat_array(struct dyn_array *array, struct dyn_array *other)
{
    bool moved = false;		/* true ==> location of the elements array moved during realloc() */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(104, __func__, "array arg is NULL");
	not_reached();
    }
    if (other == NULL) {
	err(105, __func__, "other arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(106, __func__, "array->data in first dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(107, __func__, "array->elm_size in first dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(108, __func__, "array->chunk in first dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(109, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(110, __func__, "array->count: %jd in first dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check other
     */
    if (other->data == NULL) {
	err(111, __func__, "other->data in second dynamic array is NULL");
	not_reached();
    }
    if (other->elm_size <= 0) {
	err(112, __func__, "other->elm_size in second dynamic array must be > 0: %ju", (uintmax_t)other->elm_size);
	not_reached();
    }
    if (other->chunk <= 0) {
	err(113, __func__, "other->chunk in second dynamic array must be > 0: %jd", other->chunk);
	not_reached();
    }
    if (other->allocated <= 0) {
	err(114, __func__, "other->chunk in dynamic array must be > 0: %jd", other->allocated);
	not_reached();
    }
    if (other->count > other->allocated) {
	err(115, __func__, "other->count: %jd in second dynamic array must be <= other->allocated: %jd",
			  other->count, other->allocated);
	not_reached();
    }

    /*
     * concatenate other dynamic array
     */
    moved = dyn_array_append_set(array, other->data, other->count);

    /* return array moved condition */
    return moved;
}


/*
 * dyn_array_seek - set the elements in use on a dynamic array
 *
 * given:
 *      array		- pointer to the dynamic array
 *	offset		- offset in elements
 *	whence		- SEEK_SET ==> offset from the dynamic array beginning
 *			  SEEK_CUR ==> offset from the current elements in use
 *			  SEEK_END ==> offset from the end of allocated elements
 *
 * returns:
 *	true ==> address of the array of elements moved during realloc()
 *	false ==> address of the elements array did not move
 *
 * Attempting to "seek" to or before the beginning of the array will have the effect
 * of calling dyn_array_clear().
 *
 * NOTE: This function does not return on error.
 */
bool
dyn_array_seek(struct dyn_array *array, off_t offset, int whence)
{
    bool moved = false;		/* true ==> location of the elements array moved during realloc() */
    intmax_t setpoint = 0;	/* calculated new amount of elements in use */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(116, __func__, "array arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(117, __func__, "array->data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(118, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(119, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(120, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(121, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * process seek
     */
    switch (whence) {

    /*
     * case SEEK_SET: offset from the dynamic array beginning
     */
    case SEEK_SET:
	setpoint = offset;
	break;

    /*
     * case SEEK_CUR: offset from the current elements in use
     */
    case SEEK_CUR:
	setpoint = array->count + offset;
	break;

    /*
     * case SEEK_END: offset from the end of allocated elements
     */
    case SEEK_END:
	setpoint = array->allocated + offset;
	break;

    default:
	err(122, __func__, "whence: %d != SEEK_SET: %d != SEEK_CUR: %d != SEEK_END: %d",
			  whence, SEEK_SET, SEEK_CUR, SEEK_END);
	not_reached();
	break;
    }

    /*
     * case: setpoint before beginning
     *
     * Convert from before beginning to just the beginning (empty the array).
     */
    if (setpoint < 0) {
	setpoint = 0;	/* before beginning turns in to empty */
    }

    /*
     * case: set to beginning or before beginning
     *
     * Just clear the array and set length to 0.
     */
    if (setpoint <= 0) {

	/* reduce array in use count to 0 */
	dyn_array_clear(array);

    /*
     * case: shrink to before in use point
     *
     * Shrink the array.
     */
    } else if (setpoint < array->count) {

	/* zeroize elements after new shrink point if requested */
	if (array->zeroize == true) {
	    memset((uint8_t *)array->data + (setpoint * (intmax_t)array->elm_size), 0,
		   (array->count - setpoint) * (intmax_t)array->elm_size);
	}

    /*
     * case: no change in size
     *
     * Nothing to do.
     */
    } else if (setpoint == array->count) {

	/* no change in the in use count */

    /*
     * case: expand in use below allocated size
     *
     * Expand the array within current allocation size.
     */
    } else if (setpoint <= array->allocated) {

	/* zeroize new elements beyond current in use point */
	if (array->zeroize == true) {
	    memset((uint8_t *)array->data + (array->count * (intmax_t)array->elm_size), 0,
		   (setpoint - array->count) * (intmax_t)array->elm_size);
	}

    /*
     * case: expand beyond current allocation
     *
     * Grow the array.
     */
    } else {

	/* grow the array */
	moved = dyn_array_grow(array, setpoint - array->allocated);
    }

    /* set new in use count */
    array->count = setpoint;
    if (dbg_allowed(DBG_V6_HIGH)) {
	dbg(DBG_V6_HIGH, "in %s(array, %jd, %s): %s: allocated: %jd elements of size: %ju in use: %jd",
		       __func__,
		       (intmax_t)offset,
		       (whence == SEEK_SET ? "SEEK_SET" : (whence == SEEK_CUR ? "SEEK_CUR" : "SEEK_END")),
		       (moved == true ? "moved" : "in-place"),
		       dyn_array_alloced(array),
		       (uintmax_t)array->elm_size,
		       dyn_array_tell(array));
    }

    /* return array moved condition */
    return moved;
}


/*
 * dyn_array_clear - clear the dynamic array
 *
 * This function zeroize any element in use (if array->zeroize is true),
 * and then set the number of element in use to 0.
 *
 * This function does NOT free allocated storage.
 *
 * See also dyn_array_free().
 *
 * given:
 *      array		- pointer to the dynamic array
 *
 * NOTE: This function does not return on error.
 */
void
dyn_array_clear(struct dyn_array *array)
{
    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(123, __func__, "array arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(124, __func__, "array->data for dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(125, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(126, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(128, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(129, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * Zeroize the elements currently in the array
     */
    if (array->zeroize == true) {
	memset(array->data, 0, array->count * (intmax_t)array->elm_size);
    }

    /*
     * Set the number of elements in the array to zero
     */
    array->count = 0;
    if (dbg_allowed(DBG_V3_HIGH)) {
	dbg(DBG_V3_HIGH, "in %s(array) not-moved: allocated: %jd elements of size: %ju in use: %jd",
		     __func__,
		     dyn_array_alloced(array),
		     (uintmax_t)array->elm_size,
		     dyn_array_tell(array));
    }
    return;
}


/*
 * dyn_array_free - free the contents of a dynamic array
 *
 * This function zeroize any elements in use (if array->zeroize is true),
 * free the data storage, and set the dynamic array to empty.
 *
 * This function does NOT free the struct dyn_array itself.
 * This function only frees any allocated storage.
 *
 * See also dyn_array_clear().
 *
 * given:
 *      array           - pointer to the dynamic array
 *
 * NOTE: This function does not return on error.
 */
void
dyn_array_free(struct dyn_array *array)
{
    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(130, __func__, "array arg is NULL");
	not_reached();
    }

    /*
     * Zeroize allocated data
     */
    if (array->zeroize == true && array->data != NULL && array->allocated > 0 && array->elm_size > 0) {
	memset(array->data, 0, array->allocated * (intmax_t)array->elm_size);
    }

    /*
     * Free any storage this dynamic array might have
     */
    if (array->data != NULL) {
	free(array->data);
	array->data = NULL;
    }

    /*
     * Zero the count and allocation
     */
    array->elm_size = 0;
    array->zeroize = false;
    array->count = 0;
    array->allocated = 0;
    array->chunk = 0;
    if (dbg_allowed(DBG_V4_HIGH)) {
	dbg(DBG_V4_HIGH, "in %s(array)", __func__);
    }

    free(array);
    array = NULL;

    return;
}


/*
 * dyn_array_qsort - use the qsort(3) facility on a dynamic array
 *
 * The contents of the array base are sorted in ascending order according to a comparison function pointed
 * to by compar, which requires two arguments pointing to the objects being compared.
 (
 * The comparison function must return an integer less than, equal to, or greater than zero if the first
 * argument is considered to be respectively less than, equal to, or greater than the second.
 *
 * given:
 *      array           - pointer to the dynamic array
 *	compar		- comparison function pointing to the objects being compared
 *
 * NOTE: This function does not return on error.
 */
void
dyn_array_qsort(struct dyn_array *array, int (*compar)(const void *, const void *))
{
    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(131, __func__, "array arg is NULL");
	not_reached();
    }
    if (compar == NULL) {
	err(132, __func__, "compar arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(133, __func__, "array->data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(134, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(135, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(136, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(137, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * quick return - less than 2 elements means nothing to sort
     */
    if (array->count < 2) {
	return;
    }

    /*
     * sort the dynamic array according to the comparison function order
     */
    qsort(array->data, (size_t)(array->count), (size_t)(array->elm_size), compar);
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
 * dyn_array_qsort_r - use the qsort_r(3) facility on a dynamic array
 *
 * The contents of the array base are sorted in ascending order according to a comparison function pointed
 * to by compar, which requires a pointer to thunk followed by two arguments pointing to the objects being compared.
 *
 * The thunk value allows the comparison function to access additional data without using global variables,
 * making function suitable for use in functions which must be reentrant.
 *
 * The comparison function must return an integer less than, equal to, or greater than zero if the first
 * argument is considered to be respectively less than, equal to, or greater than the second.
 *
 * given:
 *      array           - pointer to the dynamic array
 *	compar		- comparison function pointing to thunk and the objects being compared
 *	thunk		- additional unchanged data that as passed as the 1st argument to compar
 *
 * NOTE: This function does not return on error.
 */
void
dyn_array_qsort_r(struct dyn_array *array, void *thunk, int (*compar)(void *, const void *, const void *))
{
    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(138, __func__, "array arg is NULL");
	not_reached();
    }
    if (compar == NULL) {
	err(139, __func__, "compar arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(140, __func__, "array->data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(141, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(142, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(143, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(144, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * quick return - less than 2 elements means nothing to sort
     */
    if (array->count < 2) {
	return;
    }

    /*
     * sort the dynamic array according to the comparison function order
     */
    qsort_r(array->data, (size_t)(array->count), (size_t)(array->elm_size), thunk, compar);
}


/*
 * dyn_array_heapsort - use the heapsort(3) facility on a dynamic array
 *
 * The contents of the array base are sorted in ascending order according to a comparison function pointed
 * to by compar, which requires two arguments pointing to the objects being compared.
 (
 * The comparison function must return an integer less than, equal to, or greater than zero if the first
 * argument is considered to be respectively less than, equal to, or greater than the second.
 *
 * given:
 *      array           - pointer to the dynamic array
 *	compar		- comparison function pointing to the objects being compared
 *
 * returns:
 *	0 ==> sort successful (errno set to 0)
 *	-1 ==> sort error (errno is also set)
 *
 * NOTE: This function does not return when given invalid arguments.
 */
int
dyn_array_heapsort(struct dyn_array *array, int (*compar)(const void *, const void *))
{
    int ret;		    /* heapsort return value */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(145, __func__, "array arg is NULL");
	not_reached();
    }
    if (compar == NULL) {
	err(146, __func__, "compar arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(147, __func__, "array->data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(148, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(149, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(150, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(151, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * quick return - less than 2 elements means nothing to sort
     */
    errno = 0;
    if (array->count < 2) {
	return 0;
    }

    /*
     * sort the dynamic array according to the comparison function order
     */
    ret = heapsort(array->data, (size_t)(array->count), (size_t)(array->elm_size), compar);
    return ret;
}


/*
 * dyn_array_mergesort - use the mergesort(3) facility on a dynamic array
 *
 * The contents of the array base are sorted in ascending order according to a comparison function pointed
 * to by compar, which requires two arguments pointing to the objects being compared.
 (
 * The comparison function must return an integer less than, equal to, or greater than zero if the first
 * argument is considered to be respectively less than, equal to, or greater than the second.
 *
 * given:
 *      array           - pointer to the dynamic array
 *	compar		- comparison function pointing to the objects being compared
 *
 * returns:
 *	0 ==> sort successful
 *	-1 ==> sort error (errno is also set)
 *
 * NOTE: This function does not return when given invalid arguments.
 */
int
dyn_array_mergesort(struct dyn_array *array, int (*compar)(const void *, const void *))
{
    int ret;		    /* mergesort return value */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(152, __func__, "array arg is NULL");
	not_reached();
    }
    if (compar == NULL) {
	err(153, __func__, "compar arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(154, __func__, "array->data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(155, __func__, "array->elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(156, __func__, "array->chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->allocated <= 0) {
	err(157, __func__, "array->allocated in dynamic array must be > 0: %jd", array->allocated);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(158, __func__, "array->count: %jd in dynamic array must be <= array->allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * quick return - less than 2 elements means nothing to sort
     */
    errno = 0;
    if (array->count < 2) {
	return 0;
    }

    /*
     * sort the dynamic array according to the comparison function order
     */
    ret = mergesort(array->data, (size_t)(array->count), (size_t)(array->elm_size), compar);
    return ret;
}

#endif /* NON_STANDARD_SORT */


/*
 * dyn_array_top - obtain the last element ("top of stack"), if dynamic array isn't empty
 *
 * given:
 *      array           - pointer to the dynamic array
 *      fetched_value   - NULL ==> do not store the last element,
 *			  != NULL ==> where to copy the value of the last element
 *
 * returns:
 *	>0 ==> element count of the dynamic array
 *	0 ==> stack underflow, stack was empty
 *	<0 ==> array is NULL, or invalid array byte size of a single element
 */
intmax_t
dyn_array_top(struct dyn_array *array, void *fetched_value)
{
    uint8_t *last;		/* pointer to 1st byte of the last element */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	warn("%s: array is NULL", __func__);
	return -1;
    }
    if (array->elm_size <= 0) {
	warn("%s: array element size <= 0", __func__);
	return -1;
    }

    /*
     * firewall - check for stack underflow
     */
    if (array->count <= 0) {
	/* report stack underflow */
	return 0;
    }

    /*
     * copy the last element if value_to_add is non-NULL
     */
    if (fetched_value != NULL) {
	last = (uint8_t *)(array->data) + ((array->count-1) * (intmax_t)array->elm_size);
	memmove(fetched_value, last, array->elm_size);
    }

    /*
     * return current count of the dynamic array
     */
    return array->count;
}


/*
 * dyn_array_pop - remove the last element ("pop the stack"), if dynamic array isn't empty
 *
 * given:
 *      array           - pointer to the dynamic array
 *      fetched_value   - NULL ==> do not store the last element,
 *			  != NULL ==> where to copy the value of the last element
 *
 * returns:
 *	>0 ==> element count of the dynamic array
 *	0 ==> stack underflow, stack was empty
 *	<0 ==> array is NULL, or invalid array byte size of a single element
 */
intmax_t
dyn_array_pop(struct dyn_array *array, void *fetched_value)
{
    uint8_t *last;		/* pointer to 1st byte of the last element */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	warn("%s: array is NULL", __func__);
	return -1;
    }
    if (array->elm_size <= 0) {
	warn("%s: array element size <= 0", __func__);
	return -1;
    }

    /*
     * firewall - check for stack underflow
     */
    if (array->count <= 0) {
	/* report stack underflow */
	return 0;
    }

    /*
     * copy the last element if value_to_add is non-NULL
     */
    if (fetched_value != NULL) {
	last = (uint8_t *)(array->data) + ((array->count-1) * (intmax_t)array->elm_size);
	memmove(fetched_value, last, array->elm_size);
    }

    /*
     * decrement stack size
     */
    --array->count;

    /*
     * return current count of the dynamic array
     */
    return array->count;
}
