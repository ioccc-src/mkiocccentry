/*
 * jstr_util - common utility functions for the JSON string encoder/decoder
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
 * free_jstring	    - free a single struct jstring *
 *
 * given:
 *	jstr	    - a struct jstring * allocated on the stack
 *
 * This function returns void.
 *
 * NOTE: it is ASSUMED that the string in the struct if != NULL is allocated via
 * calloc()/malloc()/strdup() due to the way the encoding/decoding tools work.
 *
 * NOTE: as the strings CAN have NUL bytes it is entirely possible that there
 * might be a memory leak in some cases.
 *
 * NOTE: we do NOT take a struct jstring ** so as to set the pointer to NULL in
 * the calling function.
 */
void
free_jstring(struct jstring *jstr)
{
    /*
     * firewall
     */
    if (jstr != NULL) {
	/* free the string if not NULL */
	if (jstr->jstr != NULL) {
	    free(jstr->jstr);
	    jstr->jstr = NULL;
	}
	/* free the struct itself */
	free(jstr);
	jstr = NULL;
    }

    return;
}
