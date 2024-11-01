/*
 * jstr_util - common utility functions for the JSON string decoder/encoder
 * tools
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This JSON parser was co-developed in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */

/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "jstr_util.h"

/*
 * alloc_jstr	    -	allocate a struct jstring for the jstr tools
 *
 * given:
 *	string	    pointer to an ALLOCATED char *
 *	bufsiz	    size of the buffer
 *
 * NOTE: it is ASSUMED that the string is a calloc()/malloc()/strdup()d char *
 * so we do NOT strdup() it nor can we due to the fact that NUL bytes can exist.
 * This is why we have the bufsiz, for the cases where it must be written or
 * read.
 */
struct jstring *
alloc_jstr(char *string, size_t bufsiz)
{
    struct jstring *jstr = NULL; /* for jstring list */

    /*
     * firewall
     */
    if (string == NULL) {
	err(10, __func__, "string is NULL");
	not_reached();
    }

    /* allocate jstring struct */
    errno = 0;			    /* pre-clear errno for warnp() */
    jstr = calloc(1, sizeof *jstr);
    if (jstr == NULL) {
	warnp(__func__, "calloc() returned NULL for jstring struct");

	if (string != NULL) {
	    free(string);
	    string = NULL;
	}

	return NULL;
    }

    /*
     * set up allocated jstring struct
     */
    jstr->jstr = string;
    jstr->bufsiz = bufsiz;
    jstr->next = NULL;

    return jstr;
}

/*
 * parse_entertainment - parse -E optarg
 *
 * given:
 *	optarg		entertainment string, must be an integer >= 0
 *
 * returns:
 *	parsed entertainment or -1 on conversion error or NULL arg
 */
int
parse_entertainment(char const *optarg)
{
    int entertainment = -1;	/* parsed entertainment level or -1 */

    /*
     * firewall
     */
    if (optarg == NULL) {
	return -1;
    }

    /*
     * parse entertainment
     */
    errno = 0;		/* pre-clear errno for warnp() */
    entertainment = (int)strtol(optarg, NULL, 0);
    if (errno != 0) {
	warnp(__func__, "error converting entertainment value");

	return -1;
    }

    if (entertainment < 0) {
	return -1;
    }

    return entertainment;
}

/*
 * free_jstring	    - free a single struct jstring *
 *
 * given:
 *	jstr	    - a struct jstring * allocated on the stack
 *
 * This function returns void.
 *
 * NOTE: it is ASSUMED that the string in the struct if != NULL is allocated via
 * calloc()/malloc()/strdup() due to the way the decoding/encoding tools work.
 *
 * NOTE: as the strings CAN have NUL bytes it is entirely possible that there
 * might be a memory leak in some cases.
 */
void
free_jstring(struct jstring **jstr)
{
    /*
     * firewall
     */
    if (jstr != NULL && *jstr != NULL) {
	/* free the string if not NULL */
	if ((*jstr)->jstr != NULL) {
	    free((*jstr)->jstr);
	    (*jstr)->jstr = NULL;
	}
	/* free the struct itself */
	free(*jstr);
	*jstr = NULL;
	jstr = NULL; /* not really needed unless for some reason extra code was added below */
    }

    return;
}

/*
 * free_jstring_list	    - free a struct jstring * linked list
 *
 * given:
 *
 *  jstring_list    pointer to pointer to list to free
 *
 * This function takes a struct jstring **, a pointer to a linked list of struct
 * jstring *.
 *
 * NOTE: it is ASSUMED that the string in each struct jstring * is allocated on
 * the stack due to how the encoding/decoding works. If this is not the case
 * then expect errors.
 *
 * NOTE: if jstring_list is NULL then nothing is done.
 */
void
free_jstring_list(struct jstring **jstring_list)
{
    struct jstring *jstr = NULL;    /* current in list */
    struct jstring *jstr_next = NULL;	/* next in list */


    for (jstr = jstring_list != NULL ? *jstring_list : NULL; jstr != NULL; jstr = jstr_next) {
	jstr_next = jstr->next;		/* get next in list before we free the current */

	/* free current json string */
	free_jstring(&jstr);
	jstr = NULL;
    }

    if (jstring_list != NULL) {
        *jstring_list = NULL;
    }

    jstring_list = NULL;
}
