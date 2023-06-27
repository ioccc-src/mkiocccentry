/*
 * json_search - search JSON parse tree for matches
 *
 * Because sometimes is harder to see the JSON forest thru the JSON trees.
 * By carefully searching the JSON parse tree, we avoid careless playing with matches.
 * Only you can prevent JSON tree fires! :-)
 *
 * This search function was implemented by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#include <stdarg.h>


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "../dbg/dbg.h"

/*
 * json_parse - JSON parser support code
 */
#include "json_parse.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * json_search - search JSON parse tree for matches
 */
#include "json_search.h"


/*
 * json_search - search JSON parse tree for matches
 *
 * XXX - this function is highly subject to change - XXX
 *
 * given:
 *	tree		pointer to a JSON parse tree
 *	max_depth	maximum tree depth to descend, or 0 ==> infinite depth
 *			    NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *	match_book	pointer to an array of struct json_match data
 *			    If match_book_len == 0, then match_book must == NULL
 *			    If match_book_len > 0,  then match_book must != NULL
 *	match_book_len	length of match_book (0 ==> no match data)
 *
 *	vmatch_node_test	function to test if node is a match under an individual match
 *
 *			    node		JSON parse tree node to check for a match
 *			    match_book		pointer to an array of struct json_match data
 *				If match_book_len == 0, then match_book must == NULL
 *				If match_book_len > 0,  then match_book must != NULL
 *			    match_book_len	length of match_book (0 ==> no match data)
 *			    match_book_index	element of match_book to test
 *				If match_book_len == 0, then match_book_index must == 0
 *				match_book_index must <= match_book_len
 *			    ap			va_list for any additional args
 *
 *			    returns: true ==> match found
 *				     false ==> no match found
 *
 *	vmatch_report	function to call when a match is found
 *
 *			    node		JSON parse tree node that is a match
 *			    match_book		pointer to an array of struct json_match data
 *				If match_book_len == 0, then match_book must == NULL
 *				If match_book_len > 0,  then match_book must != NULL
 *			    match_book_len	length of match_book (0 ==> no match data)
 *			    ap			va_list for any additional args
 *
 *	...		extra args for both vmatch_test and vmatch_report
 *
 * returns:
 *	> 0 ==> number of matches found
 *	0   ==> no matches found
 *	< 0 ==> error encountered during the march search
 */
int
json_search(struct json *node, unsigned int max_depth, struct json_match *match_book, unsigned int match_book_len,
	    bool (*vmatch_test)(struct json *, struct json_match *, unsigned int, unsigned int, va_list),
	    void (*vmatch_report)(struct json *, struct json_match *, unsigned int, va_list),
	    ...)
{
    va_list ap;		/* variable argument list */
    int ret = 0;	/* > 0 ==> number of matches found, 0 ==> no matches found, < 0 ==> error */

    /*
     * firewall - arg check
     */
    if (node == NULL) {
	warn(__func__, "node is NULL, returning -1");
	return -1;
    }
    if (match_book_len > 0 && match_book == NULL) {
	warn(__func__, "match_book is NULL while match_book_len: %d > 0, returning -2", match_book_len);
	return -2;
    }
    if (match_book_len == 0 && match_book != NULL) {
	warn(__func__, "match_book is non-NULL while match_book_len == 0, returning -3");
	return -3;
    }
    if (vmatch_test == NULL) {
	warn(__func__, "vmatch_test is NULL, returning -4");
	return -4;
    }
    if (vmatch_report == NULL) {
	warn(__func__, "vmatch_report is NULL, returning -5");
	return -5;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, vmatch_report);

    /*
     * search JSON tree for matches
     */
    ret = vjson_search(node, max_depth, match_book, match_book_len, vmatch_test, vmatch_report, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return ret;
}


/*
 * vjson_search - search JSON parse tree for matches in va_list form
 *
 * XXX - this function is highly subject to change - XXX
 *
 * given:
 *	tree		pointer to a JSON parse tree
 *	max_depth	maximum tree depth to descend, or 0 ==> infinite depth
 *			    NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *	match_book	pointer to an array of struct json_match data
 *			    If match_book_len == 0, then match_book must == NULL
 *			    If match_book_len > 0,  then match_book must != NULL
 *	match_book_len	length of match_book (0 ==> no match data)
 *
 *	vmatch_node_test	function to test if node is a match under an individual match
 *
 *			    node		JSON parse tree node to check for a match
 *			    match_book		pointer to an array of struct json_match data
 *				If match_book_len == 0, then match_book must == NULL
 *				If match_book_len > 0,  then match_book must != NULL
 *			    match_book_len	length of match_book (0 ==> no match data)
 *			    match_book_index	element of match_book to test
 *				If match_book_len == 0, then match_book_index must == 0
 *				match_book_index must <= match_book_len
 *			    ap			va_list for any additional args
 *
 *			    returns: true ==> match found
 *				     false ==> no match found
 *
 *	vmatch_report	function to call when a match is found
 *
 *			    node		JSON parse tree node that is a match
 *			    match_book		pointer to an array of struct json_match data
 *				If match_book_len == 0, then match_book must == NULL
 *				If match_book_len > 0,  then match_book must != NULL
 *			    match_book_len	length of match_book (0 ==> no match data)
 *			    ap			va_list for any additional args
 *
 *	ap		va_list for vmatch_test and vmatch_report to use
 *
 * returns:
 *	> 0 ==> number of matches found
 *	0   ==> no matches found
 *	< 0 ==> error encountered during the march search
 */
int
vjson_search(struct json *node, unsigned int max_depth, struct json_match *match_book, unsigned int match_book_len,
	     bool (*vmatch_test)(struct json *, struct json_match *, unsigned int, unsigned int, va_list),
	     void (*vmatch_report)(struct json *, struct json_match *, unsigned int, va_list),
	     va_list ap)
{
    int ret = 0;	/* > 0 ==> number of matches found, 0 ==> no matches found, < 0 ==> error */

    UNUSED_ARG(max_depth); /* XXX - use arg - XXX */
    UNUSED_ARG(vmatch_test); /* XXX - use arg - XXX */
    UNUSED_ARG(vmatch_report); /* XXX - use arg - XXX */
    UNUSED_ARG(ap); /* XXX - use arg - XXX */

    /*
     * firewall - arg check
     */
    if (node == NULL) {
	warn(__func__, "node is NULL, returning -6");
	return -6;
    }
    if (match_book_len > 0 && match_book == NULL) {
	warn(__func__, "match_book is NULL while match_book_len: %d > 0, returning -7", match_book_len);
	return -7;
    }
    if (match_book_len == 0 && match_book != NULL) {
	warn(__func__, "match_book is non-NULL while match_book_len == 0, returning -8");
	return -8;
    }
    if (vmatch_test == NULL) {
	warn(__func__, "vmatch_test is NULL, returning -9");
	return -9;
    }
    if (vmatch_report == NULL) {
	warn(__func__, "vmatch_report is NULL, returning -10");
	return -10;
    }

    /* XXX - add code here */

    /*
     * stdarg variable argument list cleanup
     */
    return ret;
}
