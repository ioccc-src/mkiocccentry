/*
 * json_search - search JSON parse tree for matches
 *
 * Because sometimes it is harder to see the JSON forest through the JSON trees.
 * By carefully searching the JSON parse tree, we avoid careless playing with matches.
 * Only you can prevent JSON tree fires! :-)
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
 * This JSON search functionality was designed and implemented in support of
 * jprint by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */



#if !defined(INCLUDE_JSON_SEARCH_H)
#    define  INCLUDE_JSON_SEARCH_H


/*
 * JSON defines
 */


/*
 * json_match - how to determine if a given JSON node is a match
 */
struct json_match {
    struct json *node;		/* JSON node to test for a match */
    void *match_info;		/* pointer to data that describes how to match */
    size_t match_info_size;	/* size of the data pointed to by match_info */
};



/*
 * global variables
 */


/*
 * external function declarations
 */
extern int json_search(struct json *node, unsigned int max_depth, struct json_match *match_book, unsigned int match_book_len,
		       bool (*vmatch_node_test)(struct json *, struct json_match *, unsigned int, unsigned int, va_list),
		       void (*vmatch_report)(struct json *, struct json_match *, unsigned int, va_list),
		       ...);
extern int vjson_search(struct json *node, unsigned int max_depth, struct json_match *match_book, unsigned int match_book_len,
			bool (*vmatch_node_test)(struct json *, struct json_match *, unsigned int, unsigned int, va_list),
			void (*vmatch_report)(struct json *, struct json_match *, unsigned int, va_list),
			va_list ap);


#endif /* INCLUDE_JSON_SEARCH_H */
