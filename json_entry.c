/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json_entry - supporting mkiocccentry JSON files
 *
 * JSON related functions to support formation of .info.json files
 * and .author.json files, their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * util - utility functions
 */
#include "util.h"

/*
 * JSON parse tree supporting functions
 */
#include "json_parse.h"

/*
 * supporting mkiocccentry JSON files
 */
#include "json_entry.h"


/*
 * json_putc - print a UTF-8 character with JSON encoding
 *
 * given:
 *	stream	- string to print on
 *	c	- UTF-8 character to encode
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
bool
json_putc(uint8_t const c, FILE *stream)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * write JSON encoding to stream
     */
    ret = fprintf(stream, "%s", jenc[c].enc);
    if (ret <= 0) {
	warnp(__func__, "fprintf #1 error");
	return false;
    }
    return true;
}


/*
 * json_fprintf_str - print a JSON string
 *
 * Print on stream:
 *
 * If str == NULL:
 *
 *	null
 *
 * else str != NULL:
 *
 *	str with JSON string encoding surrounded by '"'s
 *
 * See:
 *
 *	https://www.json.org/json-en.html
 *
 * given:
 *	stream	- open file stream to print on
 *	str	- the string to JSON encode or NULL
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
bool
json_fprintf_str(FILE *stream, char const *str)
{
    int ret;			/* libc function return */
    char const *p;

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * case: NULL
     */
    if (str == NULL) {
	errno = 0;			/* pre-clear errno for warnp() */
	ret = fprintf(stream, "null");
	if (ret <= 0) {
	    warnp(__func__, "fprintf #0 error for null");
	    return false;
	}
	return true;
    }

    /*
     * print leading double-quote
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #0 error for leading double-quote");
	return false;
    }

    /*
     * print name, JSON encoded
     */
    for (p=str; *p != '\0'; ++p) {
	if (json_putc(*p, stream) != true) {
	    warnp(__func__, "json_putc #0 error");
	    return false;
	}
    }

    /*
     * print trailing double-quote
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fputc('"', stream);
    if (ret == EOF) {
	warnp(__func__, "fputc #1 error for trailing double-quote");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_string - print name value (as a string) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded " middle "value_encoded" tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value string to JSON encode or NULL
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
bool
json_fprintf_value_string(FILE *stream, char const *lead, char const *name, char const *middle, char const *value,
			  char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warn(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warn(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON encoded string
     */
    if (json_fprintf_str(stream, value) != true) {
	warn(__func__, "json_fprintf_str for value as a string");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warn(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_long - print name value (as a long integer) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle long_value tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as long
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 *
 * This function does not return on error.
 */
bool
json_fprintf_value_long(FILE *stream, char const *lead, char const *name, char const *middle, long value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warnp(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON long
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%ld", value);
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a long");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * json_fprintf_value_bool - print name value (as a boolean) pair
 *
 * On a stream, we will print:
 *
 *	lead "name_encoded" middle true tail
 * or:
 *	lead "name_encoded" middle false tail
 *
 * given:
 *	stream	- open file stream to print on
 *	lead	- leading whitespace string to print
 *	name	- name string to JSON encode or NULL
 *	middle	- middle string (often " : " )l
 *	value	- value as boolean
 *	tail	- tailing string to print (often ",\n")
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
bool
json_fprintf_value_bool(FILE *stream, char const *lead, char const *name, char const *middle, bool value,
			char const *tail)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL || lead == NULL || middle == NULL || tail == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * print leading string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", lead);
    if (ret < 0) { /* compare < 0 only in case lead is an empty string */
	warnp(__func__, "fprintf printing lead");
	return false;
    }

    /*
     * print name as a JSON encoded string
     */
    if (json_fprintf_str(stream, name) != true) {
	warn(__func__, "json_fprintf_str error printing name");
	return false;
    }

    /*
     * print middle string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", middle);
    if (ret < 0) { /* compare < 0 only in case middle is an empty string */
	warnp(__func__, "fprintf printing middle");
	return false;
    }

    /*
     * print value as a JSON boolean
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", booltostr(value));
    if (ret <= 0) {
	warnp(__func__, "fprintf for value as a boolean");
	return false;
    }

    /*
     * print trailing string
     */
    errno = 0;			/* pre-clear errno for warnp() */
    ret = fprintf(stream, "%s", tail);
    if (ret < 0) { /* compare < 0 only in case tail is an empty string */
	warnp(__func__, "fprintf printing end of line");
	return false;
    }
    return true;
}


/*
 * free_info - free info and related sub-elements
 *
 * given:
 *      infop   - pointer to info structure to free
 *
 * This function does not return on error.
 */
void
free_info(struct info *infop)
{
    int i;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(162, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free version values
     */
    if (infop->common.mkiocccentry_ver != NULL) {
	free(infop->common.mkiocccentry_ver);
	infop->common.mkiocccentry_ver = NULL;
    }

    /*
     * free entry values
     */
    if (infop->common.ioccc_id != NULL) {
	free(infop->common.ioccc_id);
	infop->common.ioccc_id = NULL;
    }
    if (infop->title != NULL) {
	free(infop->title);
	infop->title = NULL;
    }
    if (infop->abstract != NULL) {
	free(infop->abstract);
	infop->abstract = NULL;
    }

    /*
     * free filenames
     */
    if (infop->prog_c != NULL) {
	free(infop->prog_c);
	infop->prog_c = NULL;
    }
    if (infop->Makefile != NULL) {
	free(infop->Makefile);
	infop->Makefile = NULL;
    }
    if (infop->remarks_md != NULL) {
	free(infop->remarks_md);
	infop->remarks_md = NULL;
    }
    if (infop->extra_file != NULL) {
	for (i = 0; i < infop->extra_count; ++i) {
	    if (infop->extra_file[i] != NULL) {
		free(infop->extra_file[i]);
		infop->extra_file[i] = NULL;
	    }
	}
	free(infop->extra_file);
	infop->extra_file = NULL;
    }

    if (infop->common.tarball != NULL) {
	free(infop->common.tarball);
	infop->common.tarball = NULL;
    }

    /*
     * free time values
     */
    if (infop->common.epoch != NULL) {
	free(infop->common.epoch);
	infop->common.epoch = NULL;
    }
    if (infop->common.utctime != NULL) {
	free(infop->common.utctime);
	infop->common.utctime = NULL;
    }
    memset(infop, 0, sizeof *infop);

    return;
}


/*
 * free_author_array - free storage related to a struct author *
 *
 * given:
 *      author_set      - pointer to a struct author array
 *      author_count    - length of author array
 */
void
free_author_array(struct author *author_set, int author_count)
{
    int i;

    /*
     * firewall
     */
    if (author_set == NULL) {
	err(163, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(164, __func__, "author_count: %d < 0", author_count);
	not_reached();
    }

    /*
     * free elements of each array member
     */
    for (i = 0; i < author_count; ++i) {
	if (author_set[i].name != NULL) {
	    free(author_set[i].name);
	    author_set[i].name = NULL;
	}
	if (author_set[i].location_code != NULL) {
	    free(author_set[i].location_code);
	    author_set[i].location_code = NULL;
	}
	if (author_set[i].email != NULL) {
	    free(author_set[i].email);
	    author_set[i].email = NULL;
	}
	if (author_set[i].url != NULL) {
	    free(author_set[i].url);
	    author_set[i].url = NULL;
	}
	if (author_set[i].twitter != NULL) {
	    free(author_set[i].twitter);
	    author_set[i].twitter = NULL;
	}
	if (author_set[i].github != NULL) {
	    free(author_set[i].github);
	    author_set[i].github = NULL;
	}
	if (author_set[i].affiliation != NULL) {
	    free(author_set[i].affiliation);
	    author_set[i].affiliation = NULL;
	}
	if (author_set[i].author_handle != NULL) {
	    free(author_set[i].author_handle);
	    author_set[i].author_handle = NULL;
	}
    }

    memset(author_set, 0, sizeof *author_set);
    return;
}
