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


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

/*
 * Our header file - #includes the header files we need
 */
#include "dyn_array.h"


/*
 * external allocation functions
 */
static bool dyn_array_grow(struct dyn_array *array, intmax_t elms_to_allocate);


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
    unsigned char *p;		/* Pointer to the beginning of the new allocated space */
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
	err(52, __func__, "data for dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(53, __func__, "elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(54, __func__, "chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(55, __func__, "count: %jd in dynamic array must be <= allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * determine the size of the realloc-ed area
     */
    old_allocated = array->allocated;
    new_allocated = old_allocated + elms_to_allocate;
    old_bytes = old_allocated * array->elm_size;
    /* +array->chunk for guard chunk */
    new_bytes = (new_allocated+array->chunk) * array->elm_size;

    /*
     * firewall - check if new_bytes fits in a size_t variable
     */
    if ((double)new_bytes > (double)SIZE_MAX) {
	err(56, __func__, "the total number of bytes occupied by %jd elements of size %ju is too big "
			  "and does not fit  the bounds of a size_t [%jd,%jd]",
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
	errp(57, __func__, "failed to reallocate the dynamic array from a size of %jd bytes "
			   "to a size of %jd bytes",
			   old_bytes, new_bytes);
	not_reached();
    }
    if (array->data != data) {
	moved = true;
    }
    array->data = data;
    array->allocated = new_allocated;
    dbg(DBG_VVVHIGH, "%s(array, %jd): %s: allocated: %jd elements of size: %ju in use: %jd",
		     __func__, elms_to_allocate,
		     (moved == true ? "moved" : "in-place"),
		     dyn_array_alloced(array),
		     (uintmax_t)array->elm_size,
		     dyn_array_tell(array));

    /*
     * Zeroize new elements if requested
     */
    if (array->zeroize == true) {
	    p = (unsigned char *) (array->data) + old_bytes;
	    /* +array->chunk for guard chunk */
	    memset(p, 0, (elms_to_allocate+array->chunk) * array->elm_size);
    }

    return moved;
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
	err(58, __func__, "elm_size must be > 0: %ju", (uintmax_t)elm_size);
	not_reached();
    }
    if (chunk <= 0) {
	err(59, __func__, "chunk must be > 0: %jd", chunk);
	not_reached();
    }
    if (start_elm_count <= 0) {
	err(60, __func__, "start_elm_count must be > 0: %jd", start_elm_count);
	not_reached();
    }

    /*
     * Allocate new dynamic array
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = calloc(1, sizeof(struct dyn_array));
    if (ret == NULL) {
	errp(61, __func__, "cannot calloc %ju bytes for a struct dyn_array",
			   (uintmax_t)sizeof(struct dyn_array));
	not_reached();
    }

    /*
     * Initialize empty dynamic array
     * Start with a dynamic array with allocated enough chunks to hold at least start_elm_count elements
     */
    ret->elm_size = elm_size;
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
    number_of_bytes = (ret->allocated+chunk) * elm_size;

    errno = 0;			/* pre-clear errno for errp() */
    ret->data = malloc((size_t)number_of_bytes);
    if (ret->data == NULL) {
	/* +chunk for guard chunk */
	errp(62, __func__, "cannot malloc of %jd elements of %ju bytes each for dyn_array->data",
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
    dbg(DBG_VVHIGH, "%s(%ju, %jd, %jd, %s): initialized empty dynamic array, allocated: %jd elements of size: %ju",
		    __func__, (uintmax_t)elm_size, chunk, start_elm_count,
		    t_or_f(zeroize),
		    dyn_array_alloced(ret), (uintmax_t)ret->elm_size);
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
    unsigned char *p;

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(63, __func__, "array arg is NULL");
	not_reached();
    }
    if (value_to_add == NULL) {
	err(64, __func__, "value_to_add arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(65, __func__, "data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(66, __func__, "elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(67, __func__, "chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(68, __func__, "count: %jd in dynamic array must be <= allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * Expand dynamic array if needed
     */
    if (array->count >= array->allocated) {
	moved = dyn_array_grow(array, array->chunk);
    }

    /*
     * We know the dynamic array has enough room to append, so append the value
     */
    p = (unsigned char *) (array->data) + (array->count * array->elm_size);
    memcpy(p, value_to_add, array->elm_size);
    ++array->count;
    dbg(DBG_VVVHIGH, "%s(array, buf, value): %s: allocated: %jd elements of size: %ju in use: %jd",
		     __func__,
		     (moved == true ? "moved" : "in-place"),
		     dyn_array_alloced(array),
		     (uintmax_t)array->elm_size,
		     dyn_array_tell(array));

    /* return array moved condition */
    return moved;
}


/*
 * dyn_array_append_array - append the values of a given array onto the end of the dynamic array
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
 * We will add the values of the given array (which are of a given type) onto the
 * end of the dynamic array. We will grow the dynamic array if all allocated values are used.
 * If after adding the values of the array, all allocated values are used, we will grow
 * the dynamic array as a firewall.
 *
 * NOTE: This function does not return on error.
 */
bool
dyn_array_append_array(struct dyn_array *array, void *array_to_add_p, intmax_t count_of_elements_to_add)
{
    intmax_t available_empty_elements;
    intmax_t required_elements_to_allocate;
    bool moved = false;		/* true ==> location of the elements array moved during realloc() */
    unsigned char *p;

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(69, __func__, "array arg is NULL");
	not_reached();
    }
    if (array_to_add_p == NULL) {
	err(70, __func__, "array_to_add_p arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(71, __func__, "data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(72, __func__, "elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(73, __func__, "chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(74, __func__, "count: %jd in dynamic array must be <= allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * Expand dynamic array if needed
     */
    available_empty_elements = dyn_array_avail(array);
    if (count_of_elements_to_add > available_empty_elements) {
	required_elements_to_allocate = array->chunk *
		((count_of_elements_to_add - available_empty_elements + (array->chunk - 1)) / array->chunk);
	moved = dyn_array_grow(array, required_elements_to_allocate);
    }

    /*
     * We know the dynamic array has enough room to append, so append the new array
     */
    p = (unsigned char *) (array->data) + (array->count * array->elm_size);
    memcpy(p, array_to_add_p, array->elm_size * count_of_elements_to_add);
    array->count += count_of_elements_to_add;
    dbg(DBG_VVVHIGH, "%s(array, buf, %jd): %s: allocated: %jd elements of size: %ju in use: %jd",
		     __func__,
		     (intmax_t)count_of_elements_to_add,
		     (moved == true ? "moved" : "in-place"),
		     dyn_array_alloced(array),
		     (uintmax_t)array->elm_size,
		     dyn_array_tell(array));

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
	err(75, __func__, "array arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(76, __func__, "data in dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(77, __func__, "elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(78, __func__, "chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(79, __func__, "count: %jd in dynamic array must be <= allocated: %jd",
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
	err(80, __func__, "whence: %d != SEEK_SET: %d != SEEK_CUR: %d != SEEK_END: %d",
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
	    memset((uint8_t *)array->data + (setpoint * array->elm_size), 0, (array->count - setpoint) * array->elm_size);
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
	    memset((uint8_t *)array->data + (array->count * array->elm_size), 0, (setpoint - array->count) * array->elm_size);
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
    dbg(DBG_VVVHIGH, "%s(array, %jd, %s): %s: allocated: %jd elements of size: %ju in use: %jd",
		     __func__,
		     (intmax_t)offset,
		     (whence == SEEK_SET ? "SEEK_SET" : (whence == SEEK_CUR ? "SEEK_CUR" : "SEEK_END")),
		     (moved == true ? "moved" : "in-place"),
		     dyn_array_alloced(array),
		     (uintmax_t)array->elm_size,
		     dyn_array_tell(array));

    /* return array moved condition */
    return moved;
}


/*
 * dyn_array_clear - clear the dynamic array
 *
 * This function zeroize any element in use (if array->zerosize is true),
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
	err(81, __func__, "array arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(82, __func__, "data for dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(83, __func__, "elm_size in dynamic array must be > 0: %ju", (uintmax_t)array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(84, __func__, "chunk in dynamic array must be > 0: %jd", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(85, __func__, "count: %jd in dynamic array must be <= allocated: %jd",
			  array->count, array->allocated);
	not_reached();
    }

    /*
     * Zeroize the elements currently in the array
     */
    if (array->zeroize == true) {
	memset(array->data, 0, array->count * array->elm_size);
    }

    /*
     * Set the number of elements in the array to zero
     */
    array->count = 0;
    dbg(DBG_VVVHIGH, "%s(array) not-moved: allocated: %jd elements of size: %ju in use: %jd",
		     __func__,
		     dyn_array_alloced(array),
		     (uintmax_t)array->elm_size,
		     dyn_array_tell(array));
    return;
}


/*
 * dyn_array_free - free the contents of a dynamic array
 *
 * This function zeroize any elements in use (if array->zerosize is true),
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
	err(86, __func__, "array arg is NULL");
	not_reached();
    }

    /*
     * Zeroize allocated data
     */
    if (array->zeroize == true && array->data != NULL && array->allocated > 0 && array->elm_size > 0) {
	memset(array->data, 0, array->allocated * array->elm_size);
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
    dbg(DBG_VVVHIGH, "%s(array)", __func__);
    return;
}
