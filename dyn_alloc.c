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
#include "dyn_alloc.h"


/*
 * external allocation functions
 */
static bool grow_dyn_array(struct dyn_array *array, intmax_t elms_to_allocate);
static bool sum_will_overflow(intmax_t si_a, intmax_t si_b);
static bool multiplication_will_overflow(intmax_t si_a, intmax_t si_b);


/*
 * grow_dyn_array - grow the allocation of a dynamic array by a specified number of elements
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
 * be zeroized according to the type.
 *
 * NOTE: This function does not return on error.
 */
static bool
grow_dyn_array(struct dyn_array *array, intmax_t elms_to_allocate)
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
	err(51, __func__, "elms_to_allocate arg must be > 0: %ld", elms_to_allocate);
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
	err(53, __func__, "elm_size in dynamic array must be > 0: %ld", array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(54, __func__, "chunk in dynamic array must be > 0: %ld", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(55, __func__, "count: %ld in dynamic array must be <= allocated: %ld", array->count, array->allocated);
	not_reached();
    }

    /* firewall - check for overflow */
    old_allocated = array->allocated;
    if (sum_will_overflow(old_allocated, elms_to_allocate) == true) {
	err(56, __func__, "allocating %ld new elements would overflow the allocated counter (now %ld) of the dynamic "
			   "array: %ld + %ld does not fit in a intmax_t",
			   elms_to_allocate, old_allocated, old_allocated, elms_to_allocate);
	not_reached();
    }
    new_allocated = old_allocated + elms_to_allocate;

    /* firewall - check for size overflow */
    old_bytes = old_allocated * array->elm_size;
    if (multiplication_will_overflow(new_allocated, array->elm_size)) {
	err(57, __func__, "the total number of bytes occupied by %ld elements of size %lu would overflow because "
			  "%ld * %lu does not fit in a intmax_t",
			  new_allocated, array->elm_size, new_allocated, array->elm_size);
	not_reached();
    }
    new_bytes = new_allocated * array->elm_size;

    /* firewall - check if new_bytes fits in a size_t variable */
    if (new_bytes > INTMAX_MAX) {
	err(58, __func__, "the total number of bytes occupied by %ld elements of size %lu is too big and does not fit"
			  " the bounds of a size_t variable: requires %ld <= %lu",
			  new_allocated, array->elm_size, new_bytes, INTMAX_MAX);
	not_reached();
    }

    /*
     * Reallocate array
     */
    errno = 0;			/* pre-clear errno for errp() */
    data = realloc(array->data, (size_t) new_bytes);
    if (data == NULL) {
	errp(59, __func__, "failed to reallocate the dynamic array from a size of %ld bytes to a size of %ld bytes",
			   old_bytes, new_bytes);
	not_reached();
    }
    if (array->data != data) {
	moved = true;
    }
    array->data = data;
    array->allocated = new_allocated;
    dbg(DBG_VHIGH, "expanded dynamic array with %ld new allocated elements of %lu bytes: the number of allocated elements "
		   "went from %ld to %ld and the size went from %ld to %ld",
		   elms_to_allocate, array->elm_size, old_allocated, array->allocated, old_bytes, new_bytes);

    /*
     * Zeroize new elements if needed
     */
    if (array->zeroize == true) {
	    p = (unsigned char *) (array->data) + old_bytes;
	    memset(p, 0, elms_to_allocate * array->elm_size);
    }

    return moved;
}


/*
 * sum_will_overflow - check if a sum operation will lead to overflow
 *
 * given:
 *      si_a	- the number to which we want to sum something
 *      si_b	- the addend that we want to add to si_a
 *
 * returns:
 *      1 if the operation would cause number to overflow, 0 otherwise
 */
static bool
sum_will_overflow(intmax_t si_a, intmax_t si_b)
{
    if (((si_b > 0) && (si_a > (INTMAX_MAX - si_b))) ||
	((si_b < 0) && (si_a < (INTMAX_MIN - si_b)))) {
	/* will overflow */
	return true;
    }

    /* will not overflow */
    return false;
}


/*
 * multiplication_will_overflow - check if a multiplication operation will lead to overflow
 *
 * given:
 *      si_a	- the number that we want to multiply by something
 *      si_b	- the multiplier by which we want to multiply si_a
 *
 * returns:
 *      1 if the operation would cause number to overflow, 0 otherwise
 */
static bool
multiplication_will_overflow(intmax_t si_a, intmax_t si_b)
{
    /* si_a is positive */
    if (si_a > 0) {
	/* si_a and si_b are positive */
	if (si_b > 0) {
	    if (si_a > (INTMAX_MAX / si_b)) {
		return true;	/* will overflow */
	    }
	} else { /* si_a positive, si_b nonpositive */
	    if (si_b < (INTMAX_MIN / si_a)) {
		return true;	/* will overflow */
	    }
	}
    } else { /* si_a is nonpositive */
	if (si_b > 0) { /* si_a is nonpositive, si_b is positive */
	    if (si_a < (INTMAX_MIN / si_b)) {
		return true;	/* will overflow */
	    }
	} else { /* si_a and si_b are nonpositive */
	    if ( (si_a != 0) && (si_b < (INTMAX_MAX / si_a))) {
		return true;	/* will overflow */
	    }
	}
    }

    /* will not overflow */
    return false;
}


/*
 * clear_dyn_array - clears the dynamic array
 *
 * given:
 *      array			pointer to the dynamic array
 *
 * NOTE: This function does not return on error.
 */
void
clear_dyn_array(struct dyn_array *array)
{
    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(60, __func__, "array arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(61, __func__, "data for dynamic array is NULL");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(62, __func__, "elm_size in dynamic array must be > 0: %ld", array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(63, __func__, "chunk in dynamic array must be > 0: %ld", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(64, __func__, "count: %ld in dynamic array must be <= allocated: %ld", array->count, array->allocated);
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

    return;
}


/*
 * create_dyn_array - create a dynamic array
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
 * NOTE: This function does not return on error.
 */
struct dyn_array *
create_dyn_array(size_t elm_size, intmax_t chunk, intmax_t start_elm_count, bool zeroize)
{
    struct dyn_array *ret;		/* Created dynamic array to return */
    intmax_t number_of_bytes;		/* Total number of bytes occupied by the initialized array */

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (elm_size <= 0) {
	err(65, __func__, "elm_size must be > 0: %ld", elm_size);
	not_reached();
    }
    if (chunk <= 0) {
	err(66, __func__, "chunk must be > 0: %ld", chunk);
	not_reached();
    }
    if (start_elm_count <= 0) {
	err(67, __func__, "start_elm_count must be > 0: %ld", start_elm_count);
	not_reached();
    }

    /*
     * Allocate new dynamic array
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = malloc(sizeof(struct dyn_array));
    if (ret == NULL) {
	errp(68, __func__, "cannot malloc of %ld elements of %ld bytes each for dyn_array",
			   (intmax_t) 1, sizeof(struct dyn_array));
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

    /* firewall - check for size overflow */
    if (multiplication_will_overflow(ret->allocated, elm_size) == true) {
	err(69, __func__, "the total number of bytes occupied by %ld elements of size %lu would overflow because "
			  "%ld * %lu does not fit in a intmax_t",
			  ret->allocated, elm_size, ret->allocated, elm_size);
	not_reached();
    }
    number_of_bytes = ret->allocated * elm_size;
    if (number_of_bytes > INTMAX_MAX) {
	err(70, __func__, "the total number of bytes occupied by %ld elements of size %lu is too big and does not fit"
			  " the bounds of a size_t variable: requires %ld <= %lu",
			  ret->allocated, elm_size, number_of_bytes, INTMAX_MAX);
	not_reached();
    }

    errno = 0;			/* pre-clear errno for errp() */
    ret->data = malloc((size_t) number_of_bytes);
    if (ret->data == NULL) {
	errp(71, __func__, "cannot malloc of %ld elements of %ld bytes each for dyn_array->data",
			   ret->allocated, elm_size);
	not_reached();
    }

    /*
     * Zeroize allocated data according to type
     */
    if (ret->zeroize == true) {
	memset(ret->data, 0, ret->allocated * ret->elm_size);
    }

    /*
     * Return newly allocated array
     */
    dbg(DBG_HIGH, "initialized empty dynamic array of %ld elements of %ld bytes per element", ret->allocated, ret->elm_size);

    return ret;
}


/*
 * append_value - append a value of a given type onto the end of the dynamic array
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
append_value(struct dyn_array *array, void *value_to_add)
{
    bool moved = false;		/* true ==> location of the elements array moved during realloc() */
    unsigned char *p;

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(72, __func__, "array arg is NULL");
	not_reached();
    }
    if (value_to_add == NULL) {
	err(73, __func__, "value_to_add arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(74, __func__, "data in dynamic array");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(75, __func__, "elm_size in dynamic array must be > 0: %ld", array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(76, __func__, "chunk in dynamic array must be > 0: %ld", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(77, __func__, "count: %ld in dynamic array must be <= allocated: %ld", array->count, array->allocated);
	not_reached();
    }

    /*
     * Expand dynamic array if needed
     */
    if (array->count == array->allocated) {
	moved = grow_dyn_array(array, array->chunk);
    }

    /*
     * We know the dynamic array has enough room to append, so append the value
     */
    p = (unsigned char *) (array->data) + (array->count * array->elm_size);
    memcpy(p, value_to_add, array->elm_size);
    ++array->count;

    return moved;
}


/*
 * append_array - append the values of a given array (which are of a given type) onto the end of the dynamic array
 *
 * given:
 *      array				- pointer to the dynamic array
 *      array_to_add_p			- pointer to the array to add to the end of the dynamic array
 *      count_of_elements_to_add	- number of element to add to the dynamic array
 *
 * We will add the values of the given array (which are of a given type) onto the
 * end of the dynamic array. We will grow the dynamic array if all allocated values are used.
 * If after adding the values of the array, all allocated values are used, we will grow
 * the dynamic array as a firewall.
 *
 * NOTE: This function does not return on error.
 */
bool
append_array(struct dyn_array *array, void *array_to_add_p, intmax_t count_of_elements_to_add)
{
    intmax_t available_empty_elements;
    intmax_t required_elements_to_allocate;
    bool moved = false;		/* true ==> location of the elements array moved during realloc() */
    unsigned char *p;

    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(78, __func__, "array arg is NULL");
	not_reached();
    }
    if (array_to_add_p == NULL) {
	err(79, __func__, "array_to_add_p arg is NULL");
	not_reached();
    }

    /*
     * Check preconditions (firewall) - sanity check array
     */
    if (array->data == NULL) {
	err(80, __func__, "data in dynamic array");
	not_reached();
    }
    if (array->elm_size <= 0) {
	err(81, __func__, "elm_size in dynamic array must be > 0: %ld", array->elm_size);
	not_reached();
    }
    if (array->chunk <= 0) {
	err(82, __func__, "chunk in dynamic array must be > 0: %ld", array->chunk);
	not_reached();
    }
    if (array->count > array->allocated) {
	err(83, __func__, "count: %ld in dynamic array must be <= allocated: %ld", array->count, array->allocated);
	not_reached();
    }

    /*
     * Expand dynamic array if needed
     */
    available_empty_elements = array->allocated - array->count;
    required_elements_to_allocate = array->chunk *
		    ((count_of_elements_to_add - available_empty_elements + (array->chunk - 1)) / array->chunk);
    if (available_empty_elements <= count_of_elements_to_add) {
	moved = grow_dyn_array(array, required_elements_to_allocate);
    }

    /*
     * We know the dynamic array has enough room to append, so append the new array
     */
    p = (unsigned char *) (array->data) + (array->count * array->elm_size);
    memcpy(p, array_to_add_p, array->elm_size * count_of_elements_to_add);
    array->count += count_of_elements_to_add;

    return moved;
}


/*
 * free_dyn_array - free a dynamic array
 *
 * given:
 *      array           - pointer to the dynamic array
 *
 * NOTE: This function does not return on error.
 */
void
free_dyn_array(struct dyn_array *array)
{
    /*
     * Check preconditions (firewall) - sanity check args
     */
    if (array == NULL) {
	err(84, __func__, "array arg is NULL");
	not_reached();
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
    array->count = 0;
    array->allocated = 0;
    array->chunk = 0;
    return;
}
