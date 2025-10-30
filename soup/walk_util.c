/*
 * walk_util - utility functions tools walk directory trees and tar listings
 *
 * "This is a serious journey, not a hobbit walking-party."
 *
 *	-- Gandalf to Pippin, the Fellowship of the Ring.
 *
 * IMPORTANT NOTE: While most of the applications for walk relates to walking
 *                 a directory tree, there are applications (related to the
 *                 tools such as txzchk(1)) where processing is perform on
 *                 the tar listing as well.  So a while traversing a file hierarchy
 *                 might be one application of walking, so to is processing
 *                 the listing of a tar(1) command.
 *
 * Copyright (c) 2025 by Landon Curt Noll and Cody Boone Ferguson.  All Rights
 * Reserved.
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
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * The IOCCC soup is a joke between Landon and Cody after Cody misread, being
 * very tired at the time, one word as 'soup', when discussing decreasing the
 * number of files in the top level directory (thus the name of this directory).
 * We might have even left you all some delicious soup somewhere here. :-)
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


/*
 * walk - walk directory trees and tar listings
 */
#include "walk.h"


/*
 * defines
 */
#define DEF_CHUNK (16)	    /* number of elements to allocate at a time and initial allocation */


/*
 * static functions
 */
static void free_walk_rule(struct walk_rule *wrule_p);
static void init_walk_rule(struct walk_rule *wrule_p);
static void free_walk_set(struct walk_set *wset_p);
static void init_walk_set(struct walk_set *wset_p, char const *context);
static void free_item(struct item *i_p);
static struct item *alloc_item(char const *fts_path, off_t st_size, mode_t st_mode, int_least32_t fts_level);
static void free_items_in_dyn_array(struct dyn_array *dyn_array_p);
static bool chk_walk_stat(struct walk_stat *wstat_p);
static bool match_walk_rule(struct walk_rule *rule_p, struct item *i_p, int indx);
static void fprintf_dyn_array_item(FILE *stream, char const *element_name, struct dyn_array *dyn_array_p);
static void record_fts_err(struct walk_stat *wstat_p, char const *path, off_t st_size, mode_t st_mode, int_least32_t fts_level);


/*
 * free_walk_rule - free and clear a struct walk_rule
 *
 * given:
 *	wrule_p	    pointer to a struct walk_rule
 *
 * NOTE: This function does not return on error due to invalid pointers.
 */
static void
free_walk_rule(struct walk_rule *wrule_p)
{
    /*
     * firewall - catch NULL ptrs
     */
    if (wrule_p == NULL) {
	err(20, __func__, "called with NULL wrule_p");
	not_reached();
    }

    /*
     * clear match_count
     */
    wrule_p->match_count = 0;

    /*
     * if needed, free pointer to regex_t
     */
    if (wrule_p->p_preg != NULL) {
	regfree(wrule_p->p_preg);
	wrule_p->p_preg = NULL;
    }
    return;
}


/*
 * init_walk_rule - initialize a struct walk_rule
 *
 * given:
 *	wrule_p	    pointer to a struct walk_rule
 *
 * NOTE: This function does not return on an internal error.
 */
static void
init_walk_rule(struct walk_rule *wrule_p)
{
    int ret = 0;	    /* regcomp(3) return value */
    char msgbuf[BUFSIZ+1];  /* error message buffer used by regerror(3) */

    /*
     * firewall - catch NULL ptrs
     */
    if (wrule_p == NULL) {
	err(21, __func__, "called with NULL wrule_p");
	not_reached();
    }
    if (wrule_p->pattern == NULL) {
	err(22, __func__, "wrule_p->pattern is NULL");
	not_reached();
    }

    /*
     * clear match_count
     */
    wrule_p->match_count = 0;

    /*
     * free regex_t pointer if non-NULL
     */
    if (wrule_p->p_preg != NULL) {
	free(wrule_p->p_preg);
	wrule_p->p_preg = NULL;
    }

    /*
     * setup p_preg pointer to regex_t as needed
     */
    msgbuf[0] = '\0';		/* buffer paranoia */
    msgbuf[BUFSIZ] = '\0';	/* buffer paranoia */
    switch (wrule_p->match) {

    /* allocate, for MATCH_REGEX match_type, the regex_t for regcomp(3) with REG_NOSUB */
    case MATCH_REGEX:
	wrule_p->p_preg = calloc(1, sizeof(regex_t));
	if (wrule_p->p_preg == NULL) {
	    errp(23, __func__, "failed to calloc a regex_t #0");
	}
	ret = regcomp(wrule_p->p_preg, wrule_p->pattern, REG_NOSUB);
	if (ret != 0) {
	    (void) regerror(ret, wrule_p->p_preg, msgbuf, BUFSIZ);
	    errp(24, __func__, "regcomp(wrule_p->p_preg, %s, REG_NOSUB) failed: %s",
			       wrule_p->pattern, msgbuf);
	    not_reached();
	}
	break;

    /* allocate, for MATCH_REGEX_ANYCASE match_type, the regex_t for regcomp(3) with REG_NOSUB|REG_ICASE */
    case MATCH_REGEX_ANYCASE:
	wrule_p->p_preg = calloc(1, sizeof(regex_t));
	if (wrule_p->p_preg == NULL) {
	    errp(25, __func__, "failed to calloc a regex_t #1");
	}
	ret = regcomp(wrule_p->p_preg, wrule_p->pattern, REG_NOSUB|REG_ICASE);
	if (ret != 0) {
	    (void) regerror(ret, wrule_p->p_preg, msgbuf, BUFSIZ);
	    errp(26, __func__, "regcomp(wrule_p->p_preg, %s, REG_NOSUB|REG_ICASE) failed: %s",
			       wrule_p->pattern, msgbuf);
	    not_reached();
	}
	break;

    /* allocate, for MATCH_EXT_REGEX match_type, the regex_t for regcomp(3) with REG_EXTENDED|REG_NOSUB */
    case MATCH_EXT_REGEX:
	wrule_p->p_preg = calloc(1, sizeof(regex_t));
	if (wrule_p->p_preg == NULL) {
	    errp(27, __func__, "failed to calloc a regex_t #2");
	}
	ret = regcomp(wrule_p->p_preg, wrule_p->pattern, REG_EXTENDED|REG_NOSUB);
	if (ret != 0) {
	    (void) regerror(ret, wrule_p->p_preg, msgbuf, BUFSIZ);
	    errp(28, __func__, "regcomp(wrule_p->p_preg, %s, REG_EXTENDED|REG_NOSUB) failed: %s",
			       wrule_p->pattern, msgbuf);
	    not_reached();
	}
	break;

    /* allocate, for MATCH_EXT_REGEX_ANYCASE match_type, the regex_t for regcomp(3) with REG_EXTENDED|REG_NOSUB|REG_ICASE */
    case MATCH_EXT_REGEX_ANYCASE:
	wrule_p->p_preg = calloc(1, sizeof(regex_t));
	if (wrule_p->p_preg == NULL) {
	    errp(29, __func__, "failed to calloc a regex_t #3");
	}
	ret = regcomp(wrule_p->p_preg, wrule_p->pattern, REG_EXTENDED|REG_NOSUB|REG_ICASE);
	if (ret != 0) {
	    (void) regerror(ret, wrule_p->p_preg, msgbuf, BUFSIZ);
	    errp(30, __func__, "regcomp(wrule_p->p_preg, %s, REG_EXTENDED|REG_NOSUB|REG_ICASE) failed: %s",
			       wrule_p->pattern, msgbuf);
	    not_reached();
	}
	break;

    /* regex_t is not used */
    default:
	wrule_p->p_preg = NULL;
	break;
    }
    return;
}


/*
 * free_walk_set - free and clear a walk_set
 *
 * given:
 *	wset_p	    pointer to a struct walk_rule
 *
 * NOTE: This function does not return on an internal error.
 */
static void
free_walk_set(struct walk_set *wset_p)
{
    struct walk_rule *rule_p;	    /* pointer to a walk rule */

    /*
     * firewall - catch NULL ptrs
     */
    if (wset_p == NULL) {
	err(31, __func__, "called with NULL wset_p");
	not_reached();
    }

    /*
     * free calloced context
     */
    if (wset_p->context != NULL) {
	free(wset_p->context);
	wset_p->context = NULL;
    }

    /*
     * initialize each walk_rule in the set
     */
    for (rule_p = wset_p->rule; rule_p->pattern != NULL; ++rule_p) {
	free_walk_rule(rule_p);
    }

    /*
     * declare walk_set not ready
     */
    wset_p->ready = false;
    dbg(DBG_V2_HIGH, "walk_set now free");
    return;
}


/*
 * init_walk_set - initialize a walk_set
 *
 * given:
 *	wset_p	    pointer to a struct walk_rule
 *	context	    string describing the context (tool and options) for debugging purposes
 *		    NOTE: The context string arg as duplicated via strdup(3).
 *
 * NOTE: This function does not return on an internal error.
 */
static void
init_walk_set(struct walk_set *wset_p, char const *context)
{
    int count;			    /* length of set NOT counting the final NULL walk_rule.pattern */
    struct walk_rule *rule_p;	    /* pointer to a walk rule */

    /*
     * firewall - catch NULL ptrs
     */
    if (wset_p == NULL) {
	err(32, __func__, "called with NULL wset_p");
	not_reached();
    }
    if (context == NULL) {
	err(33, __func__, "called with NULL context");
	not_reached();
    }
    if (wset_p->rule == NULL) {
	err(34, __func__, "called with NULL wset_p->rule");
	not_reached();
    }

    /*
     * free existing walk_set if walk_set is ready
     */
    if (wset_p->ready) {
	free_walk_set(wset_p);
    }

    /*
     * duplicate the context
     */
    wset_p->context = strdup(context);
    if (wset_p->context == NULL) {
	errp(35,  __func__, "failed to strdup context string");
	not_reached();
    }

    /*
     * initialize each walk_rule in the set
     */
    for (count = 0, rule_p = wset_p->rule; rule_p->pattern != NULL; ++rule_p, ++count) {
	init_walk_rule(rule_p);
    }

    /*
     * save length of walk_rule set NOT counting the final NULL walk_rule.pattern
    */
    wset_p->count = count;

    /*
     * declare walk_set ready
     */
    wset_p->ready = true;
    dbg(DBG_V2_HIGH, "walk_set initialized with a context: %s", wset_p->context);
    return;
}


/*
 * free_item - free and clear an item
 *
 * given:
 *	i_p	    pointer to a calloced struct item
 *
 * NOTE: This function does not return on an internal error.
 */
static void
free_item(struct item *i_p)
{
    /*
     * firewall - catch NULL ptrs
     */
    if (i_p == NULL) {
	err(36, __func__, "called with NULL i_p");
	not_reached();
    }

    /*
     * free fts_path
     */
    if (i_p->fts_path != NULL) {
	free(i_p->fts_path);
	i_p->fts_path = NULL;
    }

    /*
     * free fts_name
     */
    if (i_p->fts_name != NULL) {
	free(i_p->fts_name);
	i_p->fts_name = NULL;
    }

    /*
     * zero the item
     */
    memset(i_p, 0, sizeof(struct item));

    /*
     * free the calloced struct item
     */
    free(i_p);
    return;
}


/*
 * alloc_item - calloc a struct item
 *
 * One of the important side effects of this function is to use basename(3)
 * to obtain an allocated the basename of the original fts_path.
 *
 * All of the struct item are calloced, including the struct item itself, are calloced.
 *
 * given:
 *	fts_path    "root path" from topdir of the item
 *	st_size	    size, in bytes in the form used by stat(2)
 *	st_mode	    inode protection mode in the form used b by stat(2)
 *	fts_level   fts_path depth, 0 ==> topdir, 1 ==> directly under topdir, 2 ==> in sub-dir under topdir
 *
 * returns:
 *	pointer to an calloced and filled out struct item with independently calloced strings.
 *
 * NOTE: This function will NOT return NULL.
 *
 * NOTE: This function does not return on an internal error.
 */
static struct item *
alloc_item(char const *fts_path, off_t st_size, mode_t st_mode, int_least32_t fts_level)
{
    struct item *i_p;		/* struct item pointer that has been calloced */

    /*
     * firewall - catch NULL ptrs
     */
    if (fts_path == NULL) {
	err(37, __func__, "called with NULL fts_path");
	not_reached();
    }

    /*
     * allocate and zeroize a struct item
     */
    i_p = calloc(sizeof(struct item), 1);
    if (i_p == NULL) {
	errp(38, __func__, "failed to calloc 1 struct item");
	not_reached();
    }

    /*
     * duplicate the fts_name
     */
    i_p->fts_path = strdup(fts_path);
    if (i_p->fts_path == NULL) {
	errp(39, __func__, "failed strdup of fts_path");
	not_reached();
    }
    i_p->fts_pathlen = strlen(i_p->fts_path);

    /*
     * obtain the basename of the original path
     */
    i_p->fts_name = base_name(i_p->fts_path);
    i_p->fts_namelen = strlen(i_p->fts_name);

    /*
     * save fts_level
     */
    i_p->fts_level = fts_level;

    /*
     * save st_size
     */
    i_p->st_size = st_size;

    /*
     * save st_mode
     */
    i_p->st_mode = st_mode;

    /*
     * return calloced struct item
     */
    return i_p;
}


/*
 * free_items_in_dyn_array - free the array of allocated items in an dyn_array
 *
 * given:
 *	dyn_array_p pointer to a dynamic array of item pointers
 *
 * NOTE: This function does not return on error due to invalid pointers.
 */
static void
free_items_in_dyn_array(struct dyn_array *dyn_array_p)
{
    struct item *i_p;	    /* pointer to an element in the dynamic array */
    intmax_t len;	    /* number of elements in the dynamic array */
    intmax_t i;

    /*
     * firewall - catch NULL ptrs
     */
    if (dyn_array_p == NULL) {
	err(40, __func__, "called with NULL dyn_array_p");
	not_reached();
    }
    if (dyn_array_p->data == NULL) {
	err(41, __func__, "dyn_array_p->data is NULL");
	not_reached();
    }

    /*
     * free each item
     */
    len = dyn_array_tell(dyn_array_p);
    for (i=0; i < len; ++i) {

	/*
	 * obtain the current item
	 */
	i_p = dyn_array_value(dyn_array_p, struct item *, i);

	/*
	 * free the current item
	 */
	if (i_p != NULL) {
	    free_item(i_p);
	}
    }
    return;
}


/*
 * free_walk_stat - free and clear a walk_stat
 *
 * given:
 *	wstat_p	    pointer to a struct walk_stat
 *
 * NOTE: This function does not return on error due to invalid pointers.
 */
void
free_walk_stat(struct walk_stat *wstat_p)
{
    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(42, __func__, "called with NULL wstat_p");
	not_reached();
    }

    /*
     * free walk_set
     */
    if (wstat_p->set != NULL) {
	free_walk_set(wstat_p->set);
	wstat_p->set = NULL;
    }

    /*
     * free topdir
     */
    if (wstat_p->topdir != NULL) {
	free(wstat_p->topdir);
	wstat_p->topdir = NULL;
    }

    /*
     * unset walked
     */
    wstat_p->walked = false;

    /*
     * unset walking
     */
    wstat_p->walking = false;

    /*
     * unset skip
     */
    wstat_p->skip = false;

    /*
     * unset tar_listing_used
     */
    wstat_p->tar_listing_used = false;

    /*
     * clear step count
     */
    wstat_p->steps = 0;

    /*
     * free the all items and the all dynamic array
     */
    if (wstat_p->all != NULL) {

	/* free all items pointed at by the dynamic array */
	free_items_in_dyn_array(wstat_p->all);

	/* free the all dynamic array */
	dyn_array_free(wstat_p->all);
    }

    /*
     * free skip_set dynamic array
     */
    if (wstat_p->skip_set != NULL) {
	dyn_array_free(wstat_p->skip_set);
	wstat_p->skip_set = NULL;
    }

    /*
     * free dyn_array for items by type
     */
    if (wstat_p->file != NULL) {
	dyn_array_free(wstat_p->file);
	wstat_p->file = NULL;
    }
    if (wstat_p->dir != NULL) {
	dyn_array_free(wstat_p->dir);
	wstat_p->dir = NULL;
    }
    if (wstat_p->symlink != NULL) {
	dyn_array_free(wstat_p->symlink);
	wstat_p->symlink = NULL;
    }
    if (wstat_p->not_file != NULL) {
	dyn_array_free(wstat_p->not_file);
	wstat_p->not_file = NULL;
    }
    if (wstat_p->not_dir != NULL) {
	dyn_array_free(wstat_p->not_dir);
	wstat_p->not_dir = NULL;
    }
    if (wstat_p->not_symlink != NULL) {
	dyn_array_free(wstat_p->not_symlink);
	wstat_p->not_symlink = NULL;
    }
    if (wstat_p->filedir != NULL) {
	dyn_array_free(wstat_p->filedir);
	wstat_p->filedir = NULL;
    }
    if (wstat_p->filesym != NULL) {
	dyn_array_free(wstat_p->filesym);
	wstat_p->filesym = NULL;
    }
    if (wstat_p->dirsym != NULL) {
	dyn_array_free(wstat_p->dirsym);
	wstat_p->dirsym = NULL;
    }
    if (wstat_p->filedirsym != NULL) {
	dyn_array_free(wstat_p->filedirsym);
	wstat_p->filedirsym = NULL;
    }
    if (wstat_p->not_filedir != NULL) {
	dyn_array_free(wstat_p->not_filedir);
	wstat_p->not_filedir = NULL;
    }
    if (wstat_p->not_filesym != NULL) {
	dyn_array_free(wstat_p->not_filesym);
	wstat_p->not_filesym = NULL;
    }
    if (wstat_p->not_dirsym != NULL) {
	dyn_array_free(wstat_p->not_dirsym);
	wstat_p->not_dirsym = NULL;
    }
    if (wstat_p->not_filedirsym != NULL) {
	dyn_array_free(wstat_p->not_filedirsym);
	wstat_p->not_filedirsym = NULL;
    }

    /*
     * free dyn_array for items by level
     */
    if (wstat_p->top != NULL) {
	dyn_array_free(wstat_p->top);
	wstat_p->top = NULL;
    }
    if (wstat_p->shallow != NULL) {
	dyn_array_free(wstat_p->shallow);
	wstat_p->shallow = NULL;
    }
    if (wstat_p->too_deep != NULL) {
	dyn_array_free(wstat_p->too_deep);
	wstat_p->too_deep = NULL;
    }
    if (wstat_p->below != NULL) {
	dyn_array_free(wstat_p->below);
	wstat_p->below = NULL;
    }
    /**/
    if (wstat_p->any_level != NULL) {
	dyn_array_free(wstat_p->any_level);
	wstat_p->any_level = NULL;
    }
    if (wstat_p->unknown_level != NULL) {
	dyn_array_free(wstat_p->unknown_level);
	wstat_p->unknown_level = NULL;
    }

    /*
     * free dyn_array for items by walk_rule
     */
    if (wstat_p->required != NULL) {
	dyn_array_free(wstat_p->required);
	wstat_p->required = NULL;
    }
    if (wstat_p->optional != NULL) {
	dyn_array_free(wstat_p->optional);
	wstat_p->optional = NULL;
    }
    /**/
    if (wstat_p->non_empty != NULL) {
	dyn_array_free(wstat_p->non_empty);
	wstat_p->non_empty = NULL;
    }
    if (wstat_p->empty != NULL) {
	dyn_array_free(wstat_p->empty);
	wstat_p->empty = NULL;
    }
    /**/
    if (wstat_p->free != NULL) {
	dyn_array_free(wstat_p->free);
	wstat_p->free = NULL;
    }
    if (wstat_p->counted != NULL) {
	dyn_array_free(wstat_p->counted);
	wstat_p->counted = NULL;
    }
    /**/
    if (wstat_p->counted_file != NULL) {
	dyn_array_free(wstat_p->counted_file);
	wstat_p->counted_file = NULL;
    }
    if (wstat_p->counted_dir != NULL) {
	dyn_array_free(wstat_p->counted_dir);
	wstat_p->counted_dir = NULL;
    }
    if (wstat_p->counted_sym != NULL) {
	dyn_array_free(wstat_p->counted_sym);
	wstat_p->counted_sym = NULL;
    }
    if (wstat_p->counted_other != NULL) {
	dyn_array_free(wstat_p->counted_other);
	wstat_p->counted_other = NULL;
    }
    /**/
    if (wstat_p->ignore != NULL) {
	dyn_array_free(wstat_p->ignore);
	wstat_p->ignore = NULL;
    }
    if (wstat_p->not_ignore != NULL) {
	dyn_array_free(wstat_p->not_ignore);
	wstat_p->not_ignore = NULL;
    }
    /**/
    if (wstat_p->prohibit != NULL) {
	dyn_array_free(wstat_p->prohibit);
	wstat_p->prohibit = NULL;
    }
    if (wstat_p->not_prohibit != NULL) {
	dyn_array_free(wstat_p->not_prohibit);
	wstat_p->not_prohibit = NULL;
    }
    /**/
    if (wstat_p->unmatched != NULL) {
	/* free the dynamic array */
	dyn_array_free(wstat_p->unmatched);
	wstat_p->unmatched = NULL;
    }

    /*
     * free dyn_array by path problem
     */
    if (wstat_p->unsafe != NULL) {
	dyn_array_free(wstat_p->unsafe);
	wstat_p->unsafe = NULL;
    }
    if (wstat_p->abs != NULL) {
	dyn_array_free(wstat_p->abs);
	wstat_p->abs = NULL;
    }
    if (wstat_p->too_long_path != NULL) {
	dyn_array_free(wstat_p->too_long_path);
	wstat_p->too_long_path = NULL;
    }
    if (wstat_p->too_long_name != NULL) {
	dyn_array_free(wstat_p->too_long_name);
	wstat_p->too_long_name = NULL;
    }
    /**/
    if (wstat_p->fts_err != NULL) {
	dyn_array_free(wstat_p->fts_err);
	wstat_p->fts_err = NULL;
    }
    if (wstat_p->safe != NULL) {
	dyn_array_free(wstat_p->safe);
	wstat_p->safe = NULL;
    }

    /*
     * free dyn_array for item descendant state
     */
    if (wstat_p->visit != NULL) {
	dyn_array_free(wstat_p->visit);
	wstat_p->visit = NULL;
    }
    if (wstat_p->prune != NULL) {
	dyn_array_free(wstat_p->prune);
	wstat_p->prune = NULL;
    }

    /*
     * declare walk_stat not ready
     */
    wstat_p->ready = false;
    dbg(DBG_V2_HIGH, "walk_stat now free");
    return;
}


/*
 * init_walk_stat - initialize a walk_stat
 *
 * This init_walk_stat() will initialize a struct walk_stat, given the various arguments
 * that such an initialization requires.
 *
 * given:
 *	wstat_p		    - pointer to a struct walk_stat
 *	topdir		    - path to a topdir, or empty string
 *	set		    - pointer to a walk_set
 *	context		    - string describing the context (tool and options) for debugging purposes
 *			      NOTE: The context string arg as duplicated via strdup(3).
 *
 *      max_path_len        - max canonicalized path length, 0 ==> no limit
 *	max_filename_len    - max length of each component of path, 0 ==> no limit
 *      max_depth           - max depth of subdirectory tree, 0 ==> no limit, <0 ==> reserved for future use
 *
 *	tar_listing_used    - true ==> tarball listing related walk, false ==> walk relating to fts(3) processing
 *
 * NOTE: This function does not return on an internal error.
 */
void
init_walk_stat(struct walk_stat *wstat_p, char const *topdir, struct walk_set *set, char const *context,
	       size_t max_path_len, size_t max_filename_len, int_least32_t max_depth,
	       bool tar_listing_used)
{
    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(43, __func__, "called with NULL wstat_p");
	not_reached();
    }
    if (topdir == NULL) {
	err(44, __func__, "called with NULL topdir");
	not_reached();
    }
    if (set == NULL) {
	err(45, __func__, "called with NULL set");
	not_reached();
    }
    if (context == NULL) {
	err(46, __func__, "called with NULL context");
	not_reached();
    }

    /*
     * zeroize walk_stat
     */
    memset(wstat_p, 0, sizeof(struct walk_stat));

    /*
     * initialize the walk_set
     */
    init_walk_set(set, context);
    wstat_p->set = set;

    /*
     * duplicate topdir
     */
    wstat_p->topdir = strdup(topdir);
    if (wstat_p->topdir == NULL) {
	errp(47,  __func__, "failed to strdup topdir string");
	not_reached();
    }
    wstat_p->topdir_len = strlen(wstat_p->topdir);

    /*
     * unset walked
     */
    wstat_p->walked = false;

    /*
     * unset walking
     */
    wstat_p->walking = false;

    /*
     * unset skip
     */
    wstat_p->skip = false;

    /*
     * set tar_listing_used according to tar_listing_used
     */
    wstat_p->tar_listing_used = tar_listing_used;

    /*
     * set max canonicalized path length
     */
    wstat_p->max_path_len = max_path_len;

    /*
     * set max length of each component of path
     */
    wstat_p->max_filename_len = max_filename_len;

    /*
     * set maximum depth
     */
    wstat_p->max_depth = max_depth;

    /*
     * clear step count
     */
    wstat_p->steps = 0;

    /*
     * allocate the skip_set dynamic array
     */
    wstat_p->skip_set = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);

    /*
     * allocate dyn_array for items by type
     */
    wstat_p->file = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->dir = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->symlink = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->not_file = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->not_dir = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->not_symlink = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->filedir = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->filesym = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->dirsym = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->filedirsym = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->not_filedir = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->not_filesym = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->not_dirsym = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->not_filedirsym = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);

    /*
     * allocate dyn_array for items by level
     */
    wstat_p->top = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->shallow = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->too_deep = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->below = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->any_level = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->unknown_level = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);

    /*
     * allocate dyn_array for items by walk_rule
     */
    wstat_p->required = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->optional = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->non_empty = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->empty = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->free = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->counted = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->counted_file = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->counted_dir = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->counted_sym = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->counted_other = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->ignore = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->not_ignore = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->prohibit = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->not_prohibit = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->unmatched = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);

    /*
     * allocate dyn_array path problem
     */
    wstat_p->unsafe = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->abs = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->too_long_path = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->too_long_name = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    /**/
    wstat_p->fts_err = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->safe = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);

    /*
     * allocate dyn_array for item descendant state
     */
    wstat_p->visit = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);
    wstat_p->prune = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);

    /*
     * allocate dyn_array for all items
     */
    wstat_p->all = dyn_array_create(sizeof(struct item *), DEF_CHUNK, DEF_CHUNK, true);

    /*
     * declare walk_stat ready
     */
    wstat_p->ready = true;
    dbg(DBG_V2_HIGH, "walk_stat initialized with a walk_set context: %s", wstat_p->set->context);
    return;
}


/*
 * chk_walk_stat - perform sanity checks on a struct walk_stat
 *
 * In addition to checking of the walk_stat is ready, we verify that some of the
 * top level pointers are non-NULL.  This just is a quick high level sanity check.
 *
 * given:
 *	wstat_p	    pointer to a struct walk_stat
 *
 * returns:
 *	true ==> *wstat_p is ready and no errors found
 *	false ==> *wstat_p is not ready, or some errors found
 */
static bool
chk_walk_stat(struct walk_stat *wstat_p)
{
    struct walk_rule *rule_p;	    /* pointer to a walk rule */
    struct walk_set *wset_p;	    /* pointer to a walk set */

    /*
     * firewall - NULL ptr catch
     */
    if (wstat_p == NULL) {
	warn(__func__, "wstat_p is NULL");
	return false;
    }

    /*
     * must be ready
     */
    if (! wstat_p->ready) {
	warn(__func__, "*wstat_p is not ready");
	return false;
    }

    /*
     * set must be non-NULL
     */
    wset_p = wstat_p->set;
    if (wset_p == NULL) {
	warn(__func__, "wstat_p has a NULL set");
	return false;
    }

    /*
     * name must be non-NULL
     */
    if (wset_p->name == NULL) {
	warn(__func__, "wset_p has a NULL name");
	return false;
    }

    /*
     * context must be non-NULL
     */
    if (wset_p->context == NULL) {
	warn(__func__, "wset_p has a NULL context");
	return false;
    }

    /*
     * walk set must as a non-NULL rule
     */
    rule_p = wset_p->rule;
    if (rule_p == NULL) {
	warn(__func__, "wset_p has a NULL rule");
	return false;
    }

    /*
     * check skip_set is NULL
     */
    if (wstat_p->skip_set == NULL) {
	warn(__func__, "wstat_p skip_set is NULL");
	return false;
    }

    /*
     * items by type dyn_array must be non-NULL
     */
    if (wstat_p->file == NULL) {
	warn(__func__, "wstat_p file is NULL");
	return false;
    }
    if (wstat_p->dir == NULL) {
	warn(__func__, "wstat_p dir is NULL");
	return false;
    }
    if (wstat_p->symlink == NULL) {
	warn(__func__, "wstat_p symlink is NULL");
	return false;
    }
    if (wstat_p->not_file == NULL) {
	warn(__func__, "wstat_p not_file is NULL");
	return false;
    }
    if (wstat_p->not_dir == NULL) {
	warn(__func__, "wstat_p not_dir is NULL");
	return false;
    }
    if (wstat_p->not_symlink == NULL) {
	warn(__func__, "wstat_p not_symlink is NULL");
	return false;
    }
    if (wstat_p->filedir == NULL) {
	warn(__func__, "wstat_p filedir is NULL");
	return false;
    }
    if (wstat_p->filesym == NULL) {
	warn(__func__, "wstat_p filesym is NULL");
	return false;
    }
    if (wstat_p->dirsym == NULL) {
	warn(__func__, "wstat_p dirsym is NULL");
	return false;
    }
    if (wstat_p->filedirsym == NULL) {
	warn(__func__, "wstat_p filedirsym is NULL");
	return false;
    }
    if (wstat_p->not_filedir == NULL) {
	warn(__func__, "wstat_p not_filedir is NULL");
	return false;
    }
    if (wstat_p->not_filesym == NULL) {
	warn(__func__, "wstat_p not_filesym is NULL");
	return false;
    }
    if (wstat_p->not_dirsym == NULL) {
	warn(__func__, "wstat_p not_dirsym is NULL");
	return false;
    }
    if (wstat_p->not_filedirsym == NULL) {
	warn(__func__, "wstat_p not_filedirsym is NULL");
	return false;
    }

    /*
     * items by level dyn_array must be non-NULL
     */
    if (wstat_p->top == NULL) {
	warn(__func__, "wstat_p top is NULL");
	return false;
    }
    if (wstat_p->shallow == NULL) {
	warn(__func__, "wstat_p shallow is NULL");
	return false;
    }
    if (wstat_p->too_deep == NULL) {
	warn(__func__, "wstat_p too_deep is NULL");
	return false;
    }
    if (wstat_p->below == NULL) {
	warn(__func__, "wstat_p below is NULL");
	return false;
    }
    /**/
    if (wstat_p->any_level == NULL) {
	warn(__func__, "wstat_p any_level is NULL");
	return false;
    }
    if (wstat_p->unknown_level == NULL) {
	warn(__func__, "wstat_p unknown_level is NULL");
	return false;
    }

    /*
     * items by walk_rule dyn_array must be non-NULL
     */
    if (wstat_p->required == NULL) {
	warn(__func__, "wstat_p required is NULL");
	return false;
    }
    if (wstat_p->optional == NULL) {
	warn(__func__, "wstat_p optional is NULL");
	return false;
    }
    /**/
    if (wstat_p->non_empty == NULL) {
	warn(__func__, "wstat_p non_empty is NULL");
	return false;
    }
    if (wstat_p->empty == NULL) {
	warn(__func__, "wstat_p empty is NULL");
	return false;
    }
    /**/
    if (wstat_p->free == NULL) {
	warn(__func__, "wstat_p free is NULL");
	return false;
    }
    if (wstat_p->counted == NULL) {
	warn(__func__, "wstat_p counted is NULL");
	return false;
    }
    /**/
    if (wstat_p->counted_file == NULL) {
	warn(__func__, "wstat_p counted_file is NULL");
	return false;
    }
    if (wstat_p->counted_dir == NULL) {
	warn(__func__, "wstat_p counted_dir is NULL");
	return false;
    }
    if (wstat_p->counted_sym == NULL) {
	warn(__func__, "wstat_p counted_sym is NULL");
	return false;
    }
    if (wstat_p->counted_other == NULL) {
	warn(__func__, "wstat_p counted_other is NULL");
	return false;
    }
    /**/
    if (wstat_p->ignore == NULL) {
	warn(__func__, "wstat_p ignore is NULL");
	return false;
    }
    if (wstat_p->not_ignore == NULL) {
	warn(__func__, "wstat_p not_ignore is NULL");
	return false;
    }
    /**/
    if (wstat_p->prohibit == NULL) {
	warn(__func__, "wstat_p prohibit is NULL");
	return false;
    }
    if (wstat_p->not_prohibit == NULL) {
	warn(__func__, "wstat_p not_prohibit is NULL");
	return false;
    }
    /**/
    if (wstat_p->unmatched == NULL) {
	warn(__func__, "wstat_p unmatched is NULL");
	return false;
    }

    /*
     * items by path problem
     */
    if (wstat_p->unsafe == NULL) {
	warn(__func__, "wstat_p unsafe is NULL");
	return false;
    }
    if (wstat_p->abs == NULL) {
	warn(__func__, "wstat_p abs is NULL");
	return false;
    }
    if (wstat_p->too_long_path == NULL) {
	warn(__func__, "wstat_p too_long_path is NULL");
	return false;
    }
    if (wstat_p->too_long_name == NULL) {
	warn(__func__, "wstat_p too_long_name is NULL");
	return false;
    }
    /**/
    if (wstat_p->fts_err == NULL) {
	warn(__func__, "wstat_p fts_err is NULL");
	return false;
    }
    if (wstat_p->safe == NULL) {
	warn(__func__, "wstat_p safe is NULL");
	return false;
    }

    /*
     * item descendant state problem
     */
    if (wstat_p->visit == NULL) {
	warn(__func__, "wstat_p visit is NULL");
	return false;
    }
    if (wstat_p->prune == NULL) {
	warn(__func__, "wstat_p prune is NULL");
	return false;
    }

    /*
     * all dyn_array must be non-NULL
     */
    if (wstat_p->all == NULL) {
	warn(__func__, "wstat_p all is NULL");
	return false;
    }

    /*
     * topdir must be non-NULL
     */
    if (wstat_p->topdir == NULL) {
	warn(__func__, "wstat_p->topdir is NULL");
	return false;
    }

    /*
     * topdir must be non-empty string
     */
    if (wstat_p->topdir[0] == '\0') {
	warn(__func__, "wstat_p->topdir is am empty string");
	return false;
    }

    /*
     * topdir string length must match topdir_len
     */
    if (strlen(wstat_p->topdir) != wstat_p->topdir_len) {
	warn(__func__, "wstat_p->topdir length: %zu != topdir_len: %zu", strlen(wstat_p->topdir), wstat_p->topdir_len);
	return false;
    }

    /*
     * no errors found under walk_stat
     */
    return true;
}


/*
 * match_walk_rule - attempt to match an item with a walk rule
 *
 * given:
 *	rule_p	    pointer to a walk rule
 *	i_p	    item to match
 *	indx	    walk rule index number (for debugging purposes)
 *
 * returns:
 *	true ==> item matches walk rule
 *	false ==> item does NOT match the walk rule
 */
static bool
match_walk_rule(struct walk_rule *rule_p, struct item *i_p, int indx)
{
    size_t patternlen;		/* length of pattern */

    /*
     * firewall - NULL ptr catch
     */
    if (rule_p == NULL) {
	warn(__func__, "rule_p is NULL");
	return false;
    }
    if (i_p == NULL) {
	warn(__func__, "i_p is NULL");
	return false;
    }

    /*
     * check size if non_empty is true
     */
    if (rule_p->non_empty && i_p->st_size == 0) {
	dbg(DBG_V3_HIGH, "%s: rule[%d]: non_empty is true however st_size is 0",
			__func__, indx);
	return false;
    }

    /*
     * check level unless allowed_level is LEVEL_ANY
     */
    switch (rule_p->allowed_level) {
    case LEVEL_TOP:
	if (i_p->fts_level != 1) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_level LEVEL_TOP: %d however fts_level: %d != 0",
			    __func__, indx, rule_p->allowed_level, i_p->fts_level);
	    return false;
	}
	break;

    case LEVEL_BELOW:
	if (i_p->fts_level <= 1) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_level LEVEL_BELOW: %d however fts_level: %d <= 0",
			    __func__, indx, rule_p->allowed_level, i_p->fts_level);
	    return false;
	}
	break;

    case LEVEL_ANY:
	dbg(DBG_V4_HIGH, "%s: rule[%d]: allowed_level LEVEL_ANY: %d allows fts_level: %d",
			 __func__, indx, rule_p->allowed_level, i_p->fts_level);
	break;

    default:
	dbg(DBG_V4_HIGH, "%s: rule[%d]: assume unknown allowed_level: %d does NOT disallow fts_level: %d",
			 __func__, indx, rule_p->allowed_level, i_p->fts_level);
	break;
    }

    /*
     * check type unless allowed_type is TYPE_ANY
     */
    switch (rule_p->allowed_type) {
    case TYPE_FILE:
	if (! ITEM_IS_FILE(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_FILE: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_DIR:
	if (! ITEM_IS_DIR(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_DIR: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_SYMLINK:
	if (! ITEM_IS_SYMLINK(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_SYMLINK: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_NOT_FILE:
	if (! ITEM_IS_NOT_FILE(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_NOT_FILE: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_NOT_DIR:
	if (! ITEM_IS_NOT_DIR(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_NOT_DIR: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_NOT_SYMLINK:
	if (! ITEM_IS_NOT_SYMLINK(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_NOT_SYMLINK: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_FILEDIR:
	if (! ITEM_IS_FILEDIR(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_FILEDIR: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_FILESYM:
	if (! ITEM_IS_FILESYM(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_FILESYM: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_DIRSYM:
	if (! ITEM_IS_DIRSYM(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_DIRSYM: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_FILEDIRSYM:
	if (! ITEM_IS_FILEDIRSYM(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_FILEDIRSYM: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_NOT_FILEDIR:
	if (! ITEM_IS_NOT_FILEDIR(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_NOT_FILEDIR: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_NOT_FILESYM:
	if (! ITEM_IS_NOT_FILESYM(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_NOT_FILESYM: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_NOT_DIRSYM:
	if (! ITEM_IS_NOT_DIRSYM(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_NOT_DIRSYM: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_NOT_FILEDIRSYM:
	if (! ITEM_IS_NOT_FILEDIRSYM(i_p->st_mode)) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: allowed_type TYPE_NOT_FILEDIRSYM: %d doesn't match ITEM_TYPE(st_mode): %07o (%s)",
			     __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			     file_type_name(i_p->st_mode));
	    return false;
	}
	break;

    case TYPE_ANY:
	dbg(DBG_V4_HIGH, "%s: rule[%d]: allowed_type TYPE_ANY: %d allows ITEM_TYPE(st_mode): %07o (%s)",
			 __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			 file_type_name(i_p->st_mode));
	break;

    default:
	dbg(DBG_V4_HIGH, "%s: rule[%d]: assume unknown allowed_type: %d doesn't disallow ITEM_TYPE(st_mode): %07o (%s)",
			 __func__, indx, rule_p->allowed_type, ITEM_TYPE(i_p->st_mode),
			 file_type_name(i_p->st_mode));
	break;
    }

    /*
     * check for pattern match
     */
    switch (rule_p->match) {

    case MATCH_STR:
	if (strcmp(rule_p->pattern, i_p->fts_name) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_STR: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_STR_ANYCASE:
	if (strcasecmp(rule_p->pattern, i_p->fts_name) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_STR_ANYCASE: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_PREFIX:
	patternlen = strlen(rule_p->pattern);
	if (patternlen > i_p->fts_namelen ||
	    strncmp(rule_p->pattern, i_p->fts_name, patternlen) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_PREFIX: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_PREFIX_ANYCASE:
	patternlen = strlen(rule_p->pattern);
	if (patternlen > i_p->fts_namelen ||
	    strncasecmp(rule_p->pattern, i_p->fts_name, patternlen) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_PREFIX_ANYCASE: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_SUFFIX:
	patternlen = strlen(rule_p->pattern);
	if (patternlen > i_p->fts_namelen ||
	    strncmp(rule_p->pattern, i_p->fts_name + i_p->fts_namelen - patternlen, patternlen) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_SUFFIX: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	}
	break;

    case MATCH_SUFFIX_ANYCASE:
	patternlen = strlen(rule_p->pattern);
	if (patternlen > i_p->fts_namelen ||
	    strncasecmp(rule_p->pattern, i_p->fts_name + i_p->fts_namelen - patternlen, patternlen) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_SUFFIX_ANYCASE: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	}
	break;

    case MATCH_FNMATCH:
	if (fnmatch(rule_p->pattern, i_p->fts_name, 0) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_FNMATCH: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_FNMATCH_ANYCASE:
	if (fnmatch(rule_p->pattern, i_p->fts_name, FNM_CASEFOLD) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_FNMATCH_ANYCASE: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_REGEX:
	if (rule_p->p_preg != NULL &&
	    regexec(rule_p->p_preg, i_p->fts_name, (size_t) 0, NULL, 0) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_REGEX: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_REGEX_ANYCASE:
	if (rule_p->p_preg != NULL &&
	    regexec(rule_p->p_preg, i_p->fts_name, (size_t) 0, NULL, 0) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_REGEX_ANYCASE: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_EXT_REGEX:
	if (rule_p->p_preg != NULL &&
	    regexec(rule_p->p_preg, i_p->fts_name, (size_t) 0, NULL, 0) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_EXT_REGEX: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_EXT_REGEX_ANYCASE:
	if (rule_p->p_preg != NULL &&
	    regexec(rule_p->p_preg, i_p->fts_name, (size_t) 0, NULL, 0) != 0) {
	    dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_EXT_REGEX_ANYCASE: pattern: %s doesn't match fts_name: %s",
			     __func__, indx, rule_p->pattern, i_p->fts_name);
	    return false;
	}
	break;

    case MATCH_ANY:
	dbg(DBG_V3_HIGH, "%s: rule[%d]: match MATCH_ANY pattern: %s matches fts_name: %s",
			 __func__, indx, rule_p->pattern, i_p->fts_name);
	break;

    default:
	dbg(DBG_V4_HIGH, "%s: rule[%d]: assume unknown pattern: %d doesn't disallow fts_name: %s",
			 __func__, indx, rule_p->match, i_p->fts_name);
	break;
    }

    /*
     * no match failures found, must be a match
     */
    return true;
}


/*
 * canonicalize_path - canonicalize a path using limits from a struct walk_stat
 *
 * We canonicalize a path using the path length (wstat_p->max_path_len and wstat_p->max_filename_len) limits
 * and the depth (wstat_p->max_depth) limits from the a struct walk_stat.
 *
 * The *sanity_p, *len_p, and *depth_p pointers, if non-NULL, are updated with the
 * enum path_sanity error code (or PATH_OK), path length, and path depth as determined by
 * the canon_path() function.
 *
 * We use this canonicalize_path() function instead of just hard coding calls to the
 * the canon_path() function, in order to be sure that paths are canonicalized in the
 * same way throughout this walk code.
 *
 * In the case of these canonicalization problems:
 *
 *	PATH_ERR_PATH_TOO_LONG
 *	PATH_ERR_NOT_RELATIVE
 *	PATH_ERR_NAME_TOO_LONG
 *	PATH_ERR_PATH_TOO_DEEP
 *	PATH_ERR_NOT_SAFE
 *	PATH_ERR_DOTDOT_OVER_TOPDIR
 *
 * A malloced but unmodified copy of fts_path is returned.  Like other canon_path() function
 * detected problems, *sanity_p will be set if non-NULL.
 *
 * It is the responsibility of the calling function to free any non-NULL return by this function,
 * after the calling function no longer needs the canonicalized path.
 *
 * given:
 *	wstat_p	    pointer to a struct walk_stat
 *	fts_path    "root path" from topdir of the item
 *
 *      sanity_p    NULL ==> don't save canon_path path_sanity error, or PATH_OK
 *                  != NULL ==> save enum path_sanity in *sanity_p
 *      len_p       NULL ==> don't save canonical path length,
 *                  != NULL ==> save canonical path length in *len_p
 *      depth_p     NULL ==> don't save canonical depth,
 *		    != NULL ==> record canonical depth in *depth_p
 *
 * returns:
 *      NULL ==> invalid path, internal error, critical canon_path() function error, or NULL pointer used
 *      != NULL ==> malloced canonicalized path
 *
 * NOTE: When NULL is returned:
 *
 *	    if sanity_p is non-NULL, then
 *		*sanity_p will be set some value that is != PATH_OK
 *
 * NOTE: When a non-NULL malloced path is returned:
 *
 *          if sanity_p is non-NULL, then
 *
 *		if all was OK, then
 *		    *sanity_p will be set to PATH_OK
 *		else
 *		    *sanity_p will be set to non-critical canon_path() function error code that is != PATH_OK
 *
 * NOTE: This function will call warn(3) on internal errors and critical canon_path() function errors.
 *	 In doing do, it will set values (*sanity_p, *len_p, *depth_p) as needed, and then return NULL.
 */
char const *
canonicalize_path(struct walk_stat *wstat_p, char const *fts_path,
		  enum path_sanity *sanity_p, size_t *len_p, int_least32_t *depth_p)
{
    char const *cpath = NULL;	    /* canonicalized path arg as a string */
    enum path_sanity sanity = PATH_ERR_UNSET;	    /* canon_path() error code, or PATH_ERR_UNSET, or PATH_OK */
    size_t path_len = 0;	    /* canonicalized path length */
    int_least32_t deep = -1;	    /* canonicalized stack depth */

    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {

	/* warn about internal error, update non-NULL values and return NULL */
	warn(__func__, "called with NULL wstat_p");
	if (sanity_p != NULL) {
	    *sanity_p = PATH_ERR_UNSET;
	}
	if (len_p != NULL) {
	    *len_p = path_len;
	}
	if (depth_p != NULL) {
	    *depth_p = deep;
	}
	return NULL;
    }
    if (fts_path == NULL) {

	/* warn about internal error, update non-NULL values and return NULL */
	warn(__func__, "called with NULL fts_path");
	if (sanity_p != NULL) {
	    *sanity_p = PATH_ERR_PATH_IS_NULL;
	}
	if (len_p != NULL) {
	    *len_p = path_len;
	}
	if (depth_p != NULL) {
	    *depth_p = deep;
	}
	return NULL;
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {

	/* warn about internal error, update non-NULL values and return NULL */
	warn(__func__, "wstat_p failed the chk_walk_stat function test suite");
	if (sanity_p != NULL) {
	    *sanity_p = PATH_ERR_UNSET;
	}
	if (len_p != NULL) {
	    *len_p = path_len;
	}
	if (depth_p != NULL) {
	    *depth_p = deep;
	}
	return NULL;
    }

    /*
     * canonicalize path
     */
    dbg(DBG_HIGH, "%s: fts_path: %s", __func__, fts_path);
    cpath = canon_path(fts_path, wstat_p->max_path_len, wstat_p->max_filename_len, wstat_p->max_depth,
		       &sanity, &path_len, &deep, true, true, true, true, NULL);
    if (cpath == NULL || sanity != PATH_OK) {

	/*
	 * process the error we encountered while trying to canonicalize the path
	 *
	 * In for some internal errors from the canon_path() function, we have to abort.
	 *
	 * In other cases we can process the original path as a "path problem".
	 * We form a fake calloced canonicalized path so that we can later file
	 * a "path problem".
	 */
	switch (sanity) {
	case PATH_OK:

	    /* warn about critical canon_path() function error, update non-NULL values and return NULL */
	    warn(__func__, "invalid PATH_OK error: %s: %s", path_sanity_name(sanity), path_sanity_error(sanity));
	    if (sanity_p != NULL) {
		*sanity_p = PATH_ERR_UNSET;
	    }
	    if (len_p != NULL) {
		*len_p = path_len;
	    }
	    if (depth_p != NULL) {
		*depth_p = deep;
	    }
	    return NULL;

	case PATH_ERR_PATH_EMPTY:

	    /* empty paths canonicalize to . (dot) */
	    cpath = strdup(".");
	    if (cpath == NULL) {

		/* warn about internal error, update non-NULL values and return NULL */
		warn(__func__, "strdup of . (dot) failed");
		if (sanity_p != NULL) {
		    *sanity_p = PATH_ERR_MALLOC;
		}
		if (len_p != NULL) {
		    *len_p = path_len;
		}
		if (depth_p != NULL) {
		    *depth_p = deep;
		}
		return NULL;
	    }
	    break;

	case PATH_ERR_PATH_TOO_LONG:
	case PATH_ERR_NOT_RELATIVE:
	case PATH_ERR_NAME_TOO_LONG:
	case PATH_ERR_PATH_TOO_DEEP:
	case PATH_ERR_NOT_SAFE:
	case PATH_ERR_DOTDOT_OVER_TOPDIR:

	    /* just duplicate fts_path unmodified due to path problems found by canon_path() */
	    cpath = strdup(fts_path);
	    if (cpath == NULL) {

		/* warn about internal error, update non-NULL values and return NULL */
		warn(__func__, "strdup of fts_path failed: %s", fts_path);
		if (sanity_p != NULL) {
		    *sanity_p = PATH_ERR_MALLOC;
		}
		if (len_p != NULL) {
		    *len_p = path_len;
		}
		if (depth_p != NULL) {
		    *depth_p = deep;
		}
		return NULL;
	    }
	    break;

	case PATH_ERR_UNSET:
	case PATH_ERR_PATH_IS_NULL:
	case PATH_ERR_MALLOC:
	case PATH_ERR_NULL_COMPONENT:
	case PATH_ERR_WRONG_LEN:

	    /* warn about critical canon_path() function error, update non-NULL values and return NULL */
	    warn(__func__, "canon_path function reported critical error %s (%s) for: %s",
			   path_sanity_name(sanity), path_sanity_error(sanity), fts_path);
	    if (sanity_p != NULL) {
		*sanity_p = sanity;
	    }
	    if (len_p != NULL) {
		*len_p = path_len;
	    }
	    if (depth_p != NULL) {
		*depth_p = deep;
	    }
	    return NULL;

	default:

	    /* warn about critical canon_path() function error, update non-NULL values and return NULL */
	    warn(__func__, "canon_path function set an unknown sanity value: %d for: %s", sanity, fts_path);
	    if (sanity_p != NULL) {
		*sanity_p = PATH_ERR_UNSET;
	    }
	    if (len_p != NULL) {
		*len_p = path_len;
	    }
	    if (depth_p != NULL) {
		*depth_p = deep;
	    }
	    return NULL;
	}
    }

    /*
     * paranoia - cpath CANNOT be NULL at this point
     */
    if (cpath == NULL) {

	/* report critical canon_path() function error */
	warn(__func__, "cpath was NULL after the canon_path function processed: %s", fts_path);
	if (sanity_p != NULL) {
	    *sanity_p = PATH_ERR_PATH_IS_NULL;
	}
	if (len_p != NULL) {
	    *len_p = path_len;
	}
	if (depth_p != NULL) {
	    *depth_p = deep;
	}
	return NULL;
    }

    /*
     * fill in return stats as required
     */
    if (sanity_p != NULL) {
        *sanity_p = sanity;
    }
    if (len_p != NULL) {
        *len_p = path_len;
    }
    if (depth_p != NULL) {
        *depth_p = deep;
    }

    /*
     * return canonicalized path
     */
    return cpath;
}


/*
 * skip_add - add a canonicalized path to skip_set dynamic array
 *
 * For a given struct walk_stat, add canonicalized fts_path as an pointer to as
 * "fake" struct item to wstat_p->skip_set.  By "fake", the st_size and st_mode
 * will be set to 0.
 *
 * The skip_set dynamic array is used to skip record_step() processing for certain selected paths.
 * For more information about how the skip_set dynamic array is used,
 * see the comments in the record_step() function below.
 *
 * given:
 *	wstat_p	    pointer to a struct walk_stat
 *	fts_path    "root path" from topdir of the item
 *
 * returns:
 *	true ==> canonicalized fts_path was added to the skip_set dynamic array
 *	false ==> canonicalized fts_path was already in the skip_set dynamic array
 *
 * NOTE: This function does not return on an internal error.
 */
bool
skip_add(struct walk_stat *wstat_p, char const *fts_path)
{
    char const *cpath = NULL;	    /* canonicalized path arg as a string */
    int_least32_t deep = -1;	    /* canonicalized stack depth */
    struct item *i_p = NULL;	    /* allocated item */

    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(48, __func__, "called with NULL wstat_p");
	not_reached();
    }
    if (fts_path == NULL) {
	err(49, __func__, "called with NULL fts_path");
	not_reached();
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(50, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }

    /*
     * canonicalize path
     */
    dbg(DBG_MED, "%s: fts_path: %s", __func__, fts_path);
    cpath = canonicalize_path(wstat_p, fts_path, NULL, NULL, &deep);
    /* paranoia */
    if (cpath == NULL) {
	err(51, __func__, "canonicalize_path had an internal failure and returned NULL");
	not_reached();
    }

    /*
     * see if canonicalized path is already in the skip_set dynamic array
     */
    i_p = path_in_item_array(wstat_p->skip_set, cpath);
    if (i_p != NULL) {
	dbg(DBG_MED, "%s: fts_path already in skip_set: %s", __func__, fts_path);
	return false;
    }

    /*
     * form an allocated item
     */
    i_p = alloc_item(cpath, 0, 0, deep);
    (void) dyn_array_append_value(wstat_p->skip_set, &i_p);

    /*
     * canonicalized fts_path added to skip_set
     */
    dbg(DBG_LOW, "%s: fts_path added to skip_set: %s", __func__, fts_path);
    return true;
}


/*
 * record_step - record an item in a struct walk_stat
 *
 * Record the step of a walk, given the original path.  We attempt to canonicalize the path.
 * If the path was successfully canonicalized, process the result.  If the path could
 * not be canonicalized, record the problem accordingly.
 *
 * This function WILL always:
 *
 *	- err(3) WILL be called if wstat_p is NULL
 *	- err(3) WILL be called if fts_path is NULL
 *	- err(3) WILL be called if chk_walk_stat(wstat_p) returns false
 *	- canonicalize_path() WILL be called to canonicalized the fts_path
 *
 *	  NOTE: using length and depth restrictions that were set when init_walk_stat() was called
 *
 *	- fts_path will be checked if it is a duplicate under wstat_p->all
 *	- If dup_p is non-NULL, *dup_p will be set
 *
 *	  NOTE: *dup_p set according to if canonicalized was already recorded in struct walk_stat or not
 *
 *	- if cpath_ret is non-NULL, *cpath_ret WILL be set to canonicalized path of fts_path
 *
 * If a path in the skip_set dynamic array of canonicalized paths matches the canonicalized path of fts_path:
 *
 *	- ALL of the "This function WILL always" actions WILL be performed
 *
 *	- path will NOT be added to ANY struct walk_stat dynamic array
 *	- path will NOT be added to the all dynamic array
 *	- path will NOT be added to the prune dynamic array
 *
 *	- wstat_p->walking will NOT be modified
 *	- wstat_p->steps will NOT be modified
 *
 *	- return false
 *
 * given:
 *	wstat_p	    pointer to a struct walk_stat
 *	fts_path    "root path" from topdir of the item
 *	st_size	    size, in bytes in the form used by stat(2)
 *	st_mode	    inode protection mode in the form used by stat(2)
 *
 *	dup_p       != NULL ==> set *dup_p according to if canonicalized was already recorded in struct walk_stat
 *		    NULL ==> do not check for duplicates
 *	cpath_ret   != NULL ==> update with pointer to the canonical path
 *		    NULL ==> ignore
 *	skip_set   != NULL ==> dynamic_array of canonicalized paths from topdir to ignore
 *		    NULL ==> ignore
 *
 * returns:
 *	true ==> fts_path is to be processed: if walking a hierarchy, any descendants should be visited
 *	false ==> if walking a hierarchy prune walk below fts_path: do NOT visit descendants of fts_path
 *
 * NOTE: This function does not return on an internal error.
 */
bool
record_step(struct walk_stat *wstat_p, char const *fts_path, off_t st_size, mode_t st_mode,
	    bool *dup_p, char const **cpath_ret)
{
    struct walk_rule *rule_p;	    /* pointer to a walk rule */
    struct walk_set *wset_p;	    /* pointer to a walk set */
    struct item *i_p = NULL;	    /* allocated item */
    char const *cpath = NULL;	    /* canonicalized path arg as a string */
    enum path_sanity sanity = PATH_ERR_UNSET;	    /* canonicalize_path() error code, or PATH_OK */
    size_t path_len = 0;	    /* canonicalized path length */
    int_least32_t deep = -1;	    /* canonicalized stack depth */
    bool matched = false;	    /* true ==> walk rule matched */
    bool visit_descendants = true;  /* true ==> OK to visit descendants, false ==> do NOT visit descendants */
    int count;

    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(52, __func__, "called with NULL wstat_p");
	not_reached();
    }
    if (fts_path == NULL) {
	err(53, __func__, "called with NULL fts_path");
	not_reached();
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(54, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }

    /*
     d canonicalize path
     */
    dbg(DBG_HIGH, "%s: fts_path: %s", __func__, fts_path);
    cpath = canonicalize_path(wstat_p, fts_path, &sanity, &path_len, &deep);
    /* paranoia */
    if (cpath == NULL) {
	err(55, __func__, "canonicalize_path had an internal failure and returned NULL");
	not_reached();
    }

    /*
     * if requested, check for duplicates
     */
    if (dup_p != NULL) {

	/*
	 * check for duplicates
	 */
	i_p = path_in_walk_stat(wstat_p, cpath);
	if (i_p == NULL) {
	    dbg(DBG_V2_HIGH, "%s: not a duplicate path: %s", __func__, fts_path);
	    *dup_p = false;
	} else{
	    dbg(DBG_MED, "%s: duplicate path found: %s", __func__, fts_path);
	    *dup_p = true;
	    return false;
	}
    }

    /*
     * update *cpath_ret with the canonicalized path if cpath_ret is non-NULL
     */
    if (cpath_ret != NULL) {
	*cpath_ret = cpath;
    }

    /*
     * check for a match with the skip_set
     */
    i_p = path_in_item_array(wstat_p->skip_set, cpath);
    if (i_p != NULL) {

	/* canonicalized path is in the skip_set, skip any further processing and just return false */
	dbg(DBG_MED, "%s: skipping path: %s", __func__, fts_path);
	if (cpath != NULL) {
	    free((void *)cpath);
	    cpath = NULL;
	}
	return false;
    }

    /*
     * form an allocated item
     */
    i_p = alloc_item(cpath, st_size, st_mode, deep);

    /*
     * first, record every item in the all dynamic array
     *
     * NOTE: This is how free_walk_stat() will free the above allocated item.
     */
    (void) dyn_array_append_value(wstat_p->all, &i_p);

    /*
     * record regular files
     */
    if (ITEM_IS_FILE(st_mode)) {
	(void) dyn_array_append_value(wstat_p->file, &i_p);
    }

    /*
     * record directories
     */
    if (ITEM_IS_DIR(st_mode)) {
	(void) dyn_array_append_value(wstat_p->dir, &i_p);
    }

    /*
     * record symlinks
     */
    if (ITEM_IS_SYMLINK(st_mode)) {
	(void) dyn_array_append_value(wstat_p->symlink, &i_p);
    }

    /*
     * record NOT regular files
     */
    if (ITEM_IS_NOT_FILE(st_mode)) {
	(void) dyn_array_append_value(wstat_p->not_file, &i_p);
    }

    /*
     * record NOT directories
     */
    if (ITEM_IS_NOT_DIR(st_mode)) {
	(void) dyn_array_append_value(wstat_p->not_dir, &i_p);
    }

    /*
     * record NOT symlinks
     */
    if (ITEM_IS_NOT_SYMLINK(st_mode)) {
	(void) dyn_array_append_value(wstat_p->not_symlink, &i_p);
    }

    /*
     * record file and directory
     */
    if (ITEM_IS_FILEDIR(st_mode)) {
	(void) dyn_array_append_value(wstat_p->filedir, &i_p);
    }

    /*
     * record file and symlink
     */
    if (ITEM_IS_FILESYM(st_mode)) {
	(void) dyn_array_append_value(wstat_p->filesym, &i_p);
    }

    /*
     * record directory and symlink
     */
    if (ITEM_IS_DIRSYM(st_mode)) {
	(void) dyn_array_append_value(wstat_p->dirsym, &i_p);
    }

    /*
     * record file, directory and symlink
     */
    if (ITEM_IS_FILEDIRSYM(st_mode)) {
	(void) dyn_array_append_value(wstat_p->filedirsym, &i_p);
    }

    /*
     * record file and directory
     */
    if (ITEM_IS_NOT_FILEDIR(st_mode)) {
	(void) dyn_array_append_value(wstat_p->not_filedir, &i_p);
    }

    /*
     * record file and symlink
     */
    if (ITEM_IS_NOT_FILESYM(st_mode)) {
	(void) dyn_array_append_value(wstat_p->not_filesym, &i_p);
    }

    /*
     * record directory and symlink
     */
    if (ITEM_IS_NOT_DIRSYM(st_mode)) {
	(void) dyn_array_append_value(wstat_p->not_dirsym, &i_p);
    }

    /*
     * record file, directory and symlink
     */
    if (ITEM_IS_NOT_FILEDIRSYM(st_mode)) {
	(void) dyn_array_append_value(wstat_p->not_filedirsym, &i_p);
    }

    /*
     * record top level
     */
    if (deep == 0) {
        /* We include the shallow vs too_deep check in case max_depth < 0 */
	if (sanity == PATH_ERR_PATH_TOO_DEEP || (wstat_p->max_depth > 0 && deep > wstat_p->max_depth)) {
	    (void) dyn_array_append_value(wstat_p->too_deep, &i_p);
	    dbg(DBG_HIGH, "too deep #0: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	    visit_descendants = false;
	} else {
	    (void) dyn_array_append_value(wstat_p->shallow, &i_p);
	}
	(void) dyn_array_append_value(wstat_p->top, &i_p);

    /*
     * record below top level
     */
    } else if (deep > 0) {
	if (sanity == PATH_ERR_PATH_TOO_DEEP || (wstat_p->max_depth > 0 && deep > wstat_p->max_depth)) {
	    (void) dyn_array_append_value(wstat_p->too_deep, &i_p);
	    dbg(DBG_HIGH, "too deep #1: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	    visit_descendants = false;
	} else {
	    (void) dyn_array_append_value(wstat_p->shallow, &i_p);
	}
	(void) dyn_array_append_value(wstat_p->below, &i_p);

    /*
     * record unknown level or above topdir
     */
    } else {
	(void) dyn_array_append_value(wstat_p->unknown_level, &i_p);
    }
    (void) dyn_array_append_value(wstat_p->any_level, &i_p);

    /*
     * record that we have stepped
     */
    ++wstat_p->steps;		/* count this step */
    wstat_p->walking = true;	/* note that we are walking */

    /*
     * scan each rule until we find a match
     */
    wset_p = wstat_p->set;
    rule_p = wset_p->rule;
    for (count = 0; rule_p->pattern != NULL; ++rule_p, ++count) {

	/*
	 * try to match
	 */
	matched = match_walk_rule(rule_p, i_p, count);
	if (matched) {

	    /*
	     * record that this rule has been matched
	     *
	     * This is especially important for rules that are required
	     * because any required rule that was NOT matched by at
	     * least 1 item is an error.
	     */
	    ++rule_p->match_count;
	    dbg(DBG_V1_HIGH, "%s: rule[%d]: %s matched item fts_path: %s fts_name: %s",
			    __func__, count, rule_p->pattern, i_p->fts_path, i_p->fts_name);
	    dbg(DBG_V1_HIGH, "%s: item fts_level: %d st_size: %lld st_mode: %07o (%s)",
			    __func__, i_p->fts_level, (long long)i_p->st_size,
			    (int)i_p->st_mode, file_type_name(i_p->st_mode));
	    break;
	}
    }

    /*
     * firewall - in case of no rules matching
     *
     * Canonically the last of a walk rule set is:
     *
     *	    { NULL,
     *		false, false, false, false, false,
     *		0, NULL }
     *
     * In particular, ignore is false.  This we assume that the current
     * item should NOT be pruned.
     */
    if (rule_p->pattern == NULL) {

	/*
	 * append to unmatched
	 *
	 * Normally we should not get here as normally because the 2nd to last pattern
	 * should be something of the form:
	 *
	 *	{ "*",
	 *	    false, false, false, false, true,
	 *	    MATCH_ANY, TYPE_ANY, LEVEL_ANY,
	 *	    0, NULL },
	 */
	(void) dyn_array_append_value(wstat_p->unmatched, &i_p);

	/*
	 * report not pruning
	 *
	 * So getting here to the unmatched is not usual.  So we will warn because
	 * the item was not recorded into any foo / not_foo lists other than all.
	 */
	warn(__func__, "fts_path: %s did NOT match any rule!", i_p->fts_path);
	return false;
    }

    /*
     * record match
     *
     * assertion: we found that *rule_p matches the item - we will now process the match
     */

    /*
     * record according to walk_rule required boolean
     */
    if (rule_p->required) {
	(void) dyn_array_append_value(wstat_p->required, &i_p);
    } else {
	(void) dyn_array_append_value(wstat_p->optional, &i_p);
    }

    /*
     * record according to walk_rule non_empty boolean
     */
    if (rule_p->non_empty) {
	(void) dyn_array_append_value(wstat_p->non_empty, &i_p);
    } else {
	(void) dyn_array_append_value(wstat_p->empty, &i_p);
    }

    /*
     * record according to walk_rule free boolean
     *
     * When walk_rule free boolean is false, and thus we are counting,
     * also count according to if the item is a file or a directory or otherwise.
     */
    if (rule_p->free) {
	(void) dyn_array_append_value(wstat_p->free, &i_p);
    } else {
	(void) dyn_array_append_value(wstat_p->counted, &i_p);
	if (ITEM_IS_FILE(st_mode)) {
	    (void) dyn_array_append_value(wstat_p->counted_file, &i_p);
	}
	if (ITEM_IS_DIR(st_mode)) {
	    (void) dyn_array_append_value(wstat_p->counted_dir, &i_p);
	}
	if (ITEM_IS_SYMLINK(st_mode)) {
	    (void) dyn_array_append_value(wstat_p->counted_sym, &i_p);
	}
	if (ITEM_IS_NOT_FILEDIRSYM(st_mode)) {
	    (void) dyn_array_append_value(wstat_p->counted_other, &i_p);
	}
    }

    /*
     * record according to walk_rule prohibit boolean
     */
    if (rule_p->prohibit) {
	(void) dyn_array_append_value(wstat_p->prohibit, &i_p);
	dbg(DBG_HIGH, "prohibit: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	visit_descendants = false;
    } else {
	(void) dyn_array_append_value(wstat_p->not_prohibit, &i_p);
    }

    /*
     * record according to walk_rule ignore boolean
     */
    if (rule_p->ignore) {
	(void) dyn_array_append_value(wstat_p->ignore, &i_p);
	dbg(DBG_HIGH, "ignore: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	visit_descendants = false;
    } else {
	(void) dyn_array_append_value(wstat_p->not_ignore, &i_p);
    }

    /*
     * record path problem or lack of a problem
     */
    switch (sanity) {
    case PATH_OK:
	(void) dyn_array_append_value(wstat_p->safe, &i_p);
	break;

    case PATH_ERR_DOTDOT_OVER_TOPDIR:
    case PATH_ERR_NOT_SAFE:
	dbg(DBG_HIGH, "not safe: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	(void) dyn_array_append_value(wstat_p->unsafe, &i_p);
	visit_descendants = false;
	break;

    case PATH_ERR_NOT_RELATIVE:
	dbg(DBG_HIGH, "absolute path: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	(void) dyn_array_append_value(wstat_p->abs, &i_p);
	visit_descendants = false;
	break;

    case PATH_ERR_PATH_TOO_LONG:
	dbg(DBG_HIGH, "path too long: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	(void) dyn_array_append_value(wstat_p->too_long_path, &i_p);
	visit_descendants = false;
	break;

    case PATH_ERR_NAME_TOO_LONG:
	dbg(DBG_HIGH, "path with a too long element: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
	(void) dyn_array_append_value(wstat_p->too_long_name, &i_p);
	visit_descendants = false;
	break;

    default:
	break;
    }

    /*
     * informing the calling application if it should ignore this item or not
     */
    if (visit_descendants) {
	(void) dyn_array_append_value(wstat_p->visit, &i_p);
	dbg(DBG_MED, "process: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
    } else {
	(void) dyn_array_append_value(wstat_p->prune, &i_p);
	wstat_p->skip = true;	/* note that we skipped something */
	dbg(DBG_MED, "prune and process: fts_path: %s fts_name: %s", i_p->fts_path, i_p->fts_name);
    }
    return visit_descendants;
}


/*
 * fprintf_dyn_array_item - print information about a dynamic array of item pointers
 *
 * given:
 *	stream		the stream on which to print information
 *	element_name	name of the struct dyn_array pointer for debugging purposes
 *	dyn_array_p	pointer to dynamic array of item pointers
 *
 * NOTE: This function does not return on an internal error.
 */
static void
fprintf_dyn_array_item(FILE *stream, char const *element_name, struct dyn_array *dyn_array_p)
{
    struct item *i_p;	    /* pointer to an element in the dynamic array */
    intmax_t len;	    /* number of elements in the dynamic array */
    intmax_t i;

    /*
     * firewall - catch NULL ptrs
     */
    if (stream == NULL) {
	err(56, __func__, "called with NULL stream");
	not_reached();
    }
    if (element_name == NULL) {
	err(57, __func__, "called with NULL element_name");
	not_reached();
    }
    if (dyn_array_p == NULL) {
	err(58, __func__, "called with NULL dyn_array_p");
	not_reached();
    }
    if (dyn_array_p->data == NULL) {
	err(59, __func__, "called with NULL dyn_array_p->data for: %s", element_name);
	not_reached();
    }

    /*
     * print name and length
     */
    len = dyn_array_tell(dyn_array_p);
    if (len > 0) {
	fprint(stream, "%s length: %jd\n", element_name, len);
    }

    /*
     * print the fts_path of each item
     */
    for (i=0; i < len; ++i) {

	/*
	 * print the fts_path of the current item
	 */
	i_p = dyn_array_value(dyn_array_p, struct item *, i);
	if (i_p == NULL) {
	    fprint(stream, "    item[%jd] is NULL\n", i);
	} else if (i_p->fts_path == NULL) {
	    fprint(stream, "    item[%jd] fts_path is NULL\n", i);
	} else {
	    fprint(stream, "    item[%jd] fts_path: %s\n", i, i_p->fts_path);
	}
    }
    if (len > 0) {
	fprstr(stream, "\n");
    }
    return;
}


/*
 * fprintf_walk_stat - print information about a struct walk_stat
 *
 * given:
 *	stream	    the stream on which to print information
 *	wstat_p	    pointer to a struct walk_stat
 *
 * NOTE: This function does not return on an internal error.
 */
void
fprintf_walk_stat(FILE *stream, struct walk_stat *wstat_p)
{
    struct walk_set *wset_p;	    /* pointer to a walk set */

    /*
     * firewall - catch NULL ptrs
     */
    if (stream == NULL) {
	err(60, __func__, "called with NULL stream");
	not_reached();
    }
    if (wstat_p == NULL) {
	err(61, __func__, "called with NULL wstat_p");
	not_reached();
    }

    /*
     * if not ready, exit early, before we do the walk_stat check
     */
    if (! wstat_p->ready) {
	fprstr(stream, "walk_stat not ready\n");
	/* nothing else to do */
	return;
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(62, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }
    wset_p = wstat_p->set;

    /*
     * print information a walk that may have started (or not), may be in progress (or not), may have finished (or not)
     */
    fprint(stream, "name: %s\n", wset_p->name);
    fprint(stream, "context: %s\n", wset_p->context);
    fprint(stream, "topdir: %s\n", wstat_p->topdir);
    fprint(stream, "walked: %s\n", booltostr(wstat_p->walked));
    fprint(stream, "walking: %s\n", booltostr(wstat_p->walking));
    fprint(stream, "skip: %s\n", booltostr(wstat_p->skip));
    fprint(stream, "tar_listing_used: %s\n", booltostr(wstat_p->tar_listing_used));
    if (wstat_p->max_path_len > 0) {
	fprint(stream, "max_path_len: %d\n", wstat_p->max_path_len);
    } else {
	fprstr(stream, "max_path_len: unlimited\n");
    }
    if (wstat_p->max_filename_len > 0) {
	fprint(stream, "max_filename_len: %d\n", wstat_p->max_filename_len);
    } else {
	fprstr(stream, "max_filename_len: unlimited\n");
    }
    if (wstat_p->max_depth > 0) {
	fprint(stream, "max_depth: %d\n", wstat_p->max_depth);
    } else {
	fprstr(stream, "max_depth: unlimited\n");
    }
    fprint(stream, "steps: %ju\n", wstat_p->steps);
    fprstr(stream, "\n");

    /*
     * print info about skip_set dynamic array
     */
    fprintf_dyn_array_item(stream, "skip_set", wstat_p->skip_set);

    /*
     * items by type
     */
    fprintf_dyn_array_item(stream, "file", wstat_p->file);
    fprintf_dyn_array_item(stream, "dir", wstat_p->dir);
    fprintf_dyn_array_item(stream, "symlink", wstat_p->symlink);
    /**/
    fprintf_dyn_array_item(stream, "not_file", wstat_p->not_file);
    fprintf_dyn_array_item(stream, "not_dir", wstat_p->not_dir);
    fprintf_dyn_array_item(stream, "not_symlink", wstat_p->not_symlink);
    /**/
    fprintf_dyn_array_item(stream, "filedir", wstat_p->filedir);
    fprintf_dyn_array_item(stream, "filesym", wstat_p->filesym);
    fprintf_dyn_array_item(stream, "dirsym", wstat_p->dirsym);
    fprintf_dyn_array_item(stream, "filedirsym", wstat_p->filedirsym);
    /**/
    fprintf_dyn_array_item(stream, "not_filedir", wstat_p->not_filedir);
    fprintf_dyn_array_item(stream, "not_filesym", wstat_p->not_filesym);
    fprintf_dyn_array_item(stream, "not_dirsym", wstat_p->not_dirsym);
    fprintf_dyn_array_item(stream, "not_filedirsym", wstat_p->not_filedirsym);

    /*
     * items by level
     */
    fprintf_dyn_array_item(stream, "top", wstat_p->top);
    fprintf_dyn_array_item(stream, "shallow", wstat_p->shallow);
    fprintf_dyn_array_item(stream, "too_deep", wstat_p->too_deep);
    fprintf_dyn_array_item(stream, "below", wstat_p->below);
    /**/
    fprintf_dyn_array_item(stream, "any_level", wstat_p->any_level);
    fprintf_dyn_array_item(stream, "unknown_level", wstat_p->unknown_level);

    /*
     * items by walk_rule
     */
    fprintf_dyn_array_item(stream, "required", wstat_p->required);
    fprintf_dyn_array_item(stream, "optional", wstat_p->optional);
    /**/
    fprintf_dyn_array_item(stream, "non_empty", wstat_p->non_empty);
    fprintf_dyn_array_item(stream, "empty", wstat_p->empty);
    /**/
    fprintf_dyn_array_item(stream, "free", wstat_p->free);
    fprintf_dyn_array_item(stream, "counted", wstat_p->counted);
    /**/
    fprintf_dyn_array_item(stream, "counted_file", wstat_p->counted_file);
    fprintf_dyn_array_item(stream, "counted_dir", wstat_p->counted_dir);
    fprintf_dyn_array_item(stream, "counted_sym", wstat_p->counted_sym);
    fprintf_dyn_array_item(stream, "counted_other", wstat_p->counted_other);
    /**/
    fprintf_dyn_array_item(stream, "ignore", wstat_p->ignore);
    fprintf_dyn_array_item(stream, "not_ignore", wstat_p->not_ignore);
    /**/
    fprintf_dyn_array_item(stream, "prohibit", wstat_p->prohibit);
    fprintf_dyn_array_item(stream, "not_prohibit", wstat_p->not_prohibit);
    /**/
    fprintf_dyn_array_item(stream, "unmatched", wstat_p->unmatched);

    /*
     * items by level
     */
    fprintf_dyn_array_item(stream, "unsafe", wstat_p->unsafe);
    fprintf_dyn_array_item(stream, "absolute", wstat_p->abs);
    fprintf_dyn_array_item(stream, "too_long_path", wstat_p->too_long_path);
    fprintf_dyn_array_item(stream, "too_long_name", wstat_p->too_long_name);
    /**/
    fprintf_dyn_array_item(stream, "fts_err", wstat_p->fts_err);
    fprintf_dyn_array_item(stream, "safe", wstat_p->safe);

    /*
     * items by descendant state
     */
    fprintf_dyn_array_item(stream, "visit", wstat_p->visit);
    fprintf_dyn_array_item(stream, "prune", wstat_p->prune);

    /*
     * all items
     */
    fprintf_dyn_array_item(stream, "all", wstat_p->all);
    return;
}


/*
 * fprintf_walk_set - print information about a struct walk_set
 *
 * given:
 *	stream	    the stream on which to print information
 *	wset_p	    pointer to a struct walk_set
 *
 * NOTE: This function does not return on an internal error.
 */
void
fprintf_walk_set(FILE *stream, struct walk_set *wset_p)
{
    struct walk_rule * rule_p;	    /* pointer to a walk rule */
    int i;

    /*
     * firewall - catch NULL ptrs
     */
    if (stream == NULL) {
	err(63, __func__, "called with NULL stream");
	not_reached();
    }
    if (wset_p == NULL) {
	err(64, __func__, "called with NULL wset_p");
	not_reached();
    }
    if (wset_p->name == NULL) {
	err(65, __func__, "called with NULL wset_p->name");
	not_reached();
    }
    if (wset_p->context == NULL) {
	err(66, __func__, "called with NULL wset_p->context");
	not_reached();
    }
    if (wset_p->rule == NULL) {
	err(67, __func__, "called with NULL wset_p->rule");
	not_reached();
    }

    /*
     * if not ready, exit early, before we do the walk_set check
     */
    if (! wset_p->ready) {
	fprstr(stream, "wset_p not ready\n");
	/* nothing else to do */
	return;
    }

    /*
     * print information the walk_set
     */
    fprint(stream, "name: %s\n", wset_p->name);
    fprint(stream, "context: %s\n", wset_p->context);
    fprint(stream, "count: %d\n", wset_p->count);
    fprstr(stream, "\n");

    /*
     * print each walk_rule
     */
    for (rule_p = wset_p->rule, i=0; rule_p->pattern != NULL; ++rule_p, ++i) {

	/* print walk_rule pattern */
	fprint(stream, "pattern[%d]: %s", i, rule_p->pattern);

	/* print walk_rule boolean information */
	fprint(stream, "    %s, %s, %s, %s, %s,\n",
		(rule_p->required ? "required" : "optional"),
		(rule_p->non_empty ? "non_empty" : "empty_ok"),
		(rule_p->free ? "free" : "counted"),
		(rule_p->ignore ? "ignore" : "not_ignore"),
		(rule_p->prohibit ? "prohibit" : "not_prohibit"));

	/* print walk_rule match */
	switch (rule_p->match) {
	case MATCH_UNSET:
	    fprstr(stream, "    MATCH_UNSET, ");
	    break;

	case MATCH_STR:
	    fprstr(stream, "    MATCH_STR, ");
	    break;
	case MATCH_STR_ANYCASE:
	    fprstr(stream, "    MATCH_STR_ANYCASE, ");
	    break;
	case MATCH_PREFIX:
	    fprstr(stream, "    MATCH_PREFIX, ");
	    break;
	case MATCH_PREFIX_ANYCASE:
	    fprstr(stream, "    MATCH_PREFIX_ANYCASE, ");
	    break;
	case MATCH_SUFFIX:
	    fprstr(stream, "    MATCH_SUFFIX, ");
	    break;
	case MATCH_SUFFIX_ANYCASE:
	    fprstr(stream, "    MATCH_SUFFIX_ANYCASE, ");
	    break;
	case MATCH_FNMATCH:
	    fprstr(stream, "    MATCH_FNMATCH, ");
	    break;
	case MATCH_FNMATCH_ANYCASE:
	    fprstr(stream, "    MATCH_FNMATCH_ANYCASE, ");
	    break;
	case MATCH_REGEX:
	    fprstr(stream, "    MATCH_REGEX, ");
	    break;
	case MATCH_REGEX_ANYCASE:
	    fprstr(stream, "    MATCH_REGEX_ANYCASE, ");
	    break;
	case MATCH_EXT_REGEX:
	    fprstr(stream, "    MATCH_EXT_REGEX, ");
	    break;
	case MATCH_EXT_REGEX_ANYCASE:
	    fprstr(stream, "    MATCH_EXT_REGEX_ANYCASE, ");
	    break;

	case MATCH_ANY:
	    fprstr(stream, "    MATCH_ANY, ");
	    break;

	default:
	    fprint(stream, "    ((unknown_pattern_type:%d)), ", rule_p->match);
	    break;
	}

	/* print walk_rule allowed_type */
	switch (rule_p->allowed_type) {
	case TYPE_UNSET:
	    fprstr(stream, "TYPE_UNSET, ");
	    break;

	case TYPE_FILE:
	    fprstr(stream, "TYPE_FILE, ");
	    break;
	case TYPE_DIR:
	    fprstr(stream, "TYPE_DIR, ");
	    break;
	case TYPE_SYMLINK:
	    fprstr(stream, "TYPE_SYMLINK, ");
	    break;

	case TYPE_NOT_FILE:
	    fprstr(stream, "TYPE_NOT_FILE, ");
	    break;
	case TYPE_NOT_DIR:
	    fprstr(stream, "TYPE_NOT_DIR, ");
	    break;
	case TYPE_NOT_SYMLINK:
	    fprstr(stream, "TYPE_NOT_SYMLINK, ");
	    break;

	case TYPE_FILEDIR:
	    fprstr(stream, "TYPE_FILEDIR, ");
	    break;
	case TYPE_FILESYM:
	    fprstr(stream, "TYPE_FILESYM, ");
	    break;
	case TYPE_DIRSYM:
	    fprstr(stream, "TYPE_DIRSYM, ");
	    break;
	case TYPE_FILEDIRSYM:
	    fprstr(stream, "TYPE_FILEDIRSYM, ");
	    break;

	case TYPE_NOT_FILEDIR:
	    fprstr(stream, "TYPE_NOT_FILEDIR, ");
	    break;
	case TYPE_NOT_FILESYM:
	    fprstr(stream, "TYPE_NOT_FILESYM, ");
	    break;
	case TYPE_NOT_DIRSYM:
	    fprstr(stream, "TYPE_NOT_DIRSYM, ");
	    break;
	case TYPE_NOT_FILEDIRSYM:
	    fprstr(stream, "TYPE_NOT_FILEDIRSYM, ");
	    break;

	case TYPE_ANY:
	    fprstr(stream, "TYPE_ANY, ");
	    break;

	default:
	    fprint(stream, "((allowed_type:%d)), ", rule_p->allowed_type);
	    break;
	}

	/* print walk_rule allowed_level */
	switch (rule_p->allowed_level) {
	case LEVEL_UNSET:
	    fprstr(stream, "LEVEL_UNSET,\n");
	    break;

	case LEVEL_TOP:
	    fprstr(stream, "LEVEL_TOP,\n");
	    break;
	case LEVEL_BELOW:
	    fprstr(stream, "LEVEL_BELOW,\n");
	    break;

	case LEVEL_ANY:
	    fprstr(stream, "LEVEL_ANY,\n");
	    break;

	default:
	    fprint(stream, "((allowed_level:%d)),\n", rule_p->allowed_level);
	    break;
	}

	/* print match_count and p_preg */
	fprint(stream, "    %d, %p\n", rule_p->match_count, (void *)rule_p->p_preg);
    }
    return;
}


/*
 * path_cmp - compare two case dependent paths
 *
 * given:
 *      pa          pointer to string a
 *      pb          pointer to string b
 *
 * returns
 *      -1 ==> a < b
 *      0  ==> a == b
 *      1  ==> a > b
 *
 * NOTE: NULL pointers are sorted later than all strings.
 */
int
path_cmp(const void *pa, const void *pb)
{
    int cmp;			/* string compare return value */

    /*
     * firewall
     *
     * If we find NULL printers, we will sort them to the end, and thus they come out greater (last).
     */
    if (pa == NULL) {
	if (pb == NULL) {
	    warn(__func__, "both pa and pb are NULL, assuming they are equal");
	    return 0;
	} else {
	    warn(__func__, "pa is NULL but pb is NOT, assuming a is greater");
	    return 1;
	}
    } else if (pb == NULL) {
	warn(__func__, "pb is NULL but pa is NOT, assuming b is greater");
	return -1;
    }

    /*
     * compare the paths in a case dependent way
     */
    cmp = strcmp((char const *)pa, (char const *)pb);
    if (cmp < 0) {
	return -1;
    } else if (cmp > 0) {
	return 1;
    }
    return 0;
}


/*
 * item_cmp - compare two struct item case dependent fts_paths
 *
 * given:
 *      pa          pointer to struct item a pointer
 *      pb          pointer to struct item b pointer
 *
 * returns
 *      -1 ==> a < b
 *      0  ==> a == b
 *      1  ==> a > b
 *
 * NOTE: NULL pointers are sorted later than all strings.
 */
int
item_cmp(void const *pa, void const *pb)
{
    struct item const *p_a;	/* pointer to 1st struct item */
    struct item const *p_b;	/* pointer to 2nd struct item */
    int cmp;			/* string compare return value */

    /*
     * firewall
     *
     * If we find NULL printers, we will sort them to the end, and thus they come out greater (last).
     */
    if (pa == NULL) {
	if (pb == NULL) {
	    warn(__func__, "both pa and pb are NULL, assuming they are equal");
	    return 0;
	} else {
	    warn(__func__, "pa is NULL but pb is NOT, assuming a is greater");
	    return 1;
	}
    } else if (pb == NULL) {
	warn(__func__, "pb is NULL but pa is NOT, assuming b is greater");
	return -1;
    }


    /*
     * deference the item pointers
     */
    p_a = *(struct item const **)pa;
    p_b = *(struct item const **)pb;

    /*
     * compare the fts_path item elements in a case dependent way
     */
    cmp = path_cmp(p_a->fts_path, p_b->fts_path);
    if (cmp < 0) {
	return -1;
    } else if (cmp > 0) {
	return 1;
    }
    return 0;
}


/*
 * path_icmp - compare two case independent paths
 *
 * given:
 *      pa          pointer to string a
 *      pb          pointer to string b
 *
 * returns
 *      -1 ==> a < b
 *      0  ==> a == b
 *      1  ==> a > b
 *
 * NOTE: NULL pointers are sorted later than all strings.
 */
int
path_icmp(const void *pa, const void *pb)
{
    int cmp;			/* string compare return value */

    /*
     * firewall
     *
     * If we find NULL printers, we will sort them to the end, and thus they come out greater (last).
     */
    if (pa == NULL) {
	if (pb == NULL) {
	    warn(__func__, "both pa and pb are NULL, assuming they are equal");
	    return 0;
	} else {
	    warn(__func__, "pa is NULL but pb is NOT, assuming a is greater");
	    return 1;
	}
    } else if (pb == NULL) {
	warn(__func__, "pb is NULL but pa is NOT, assuming b is greater");
	return -1;
    }

    /*
     * compare the paths in a case independent way
     */
    cmp = strcasecmp((char const *)pa, (char const *)pb);
    if (cmp < 0) {
	return -1;
    } else if (cmp > 0) {
	return 1;
    }
    return 0;
}


/*
 * item_icmp - compare two struct item case independent fts_paths
 *
 * given:
 *      pa          pointer to struct item a pointer
 *      pb          pointer to struct item b pointer
 *
 * returns
 *      -1 ==> a < b
 *      0  ==> a == b
 *      1  ==> a > b
 *
 * NOTE: NULL pointers are sorted later than all strings.
 */
int
item_icmp(void const *pa, void const *pb)
{
    struct item const *p_a;	/* pointer to 1st struct item */
    struct item const *p_b;	/* pointer to 2nd struct item */
    int cmp;			/* string compare return value */

    /*
     * firewall
     *
     * If we find NULL printers, we will sort them to the end, and thus they come out greater (last).
     */
    if (pa == NULL) {
	if (pb == NULL) {
	    warn(__func__, "both pa and pb are NULL, assuming they are equal");
	    return 0;
	} else {
	    warn(__func__, "pa is NULL but pb is NOT, assuming a is greater");
	    return 1;
	}
    } else if (pb == NULL) {
	warn(__func__, "pb is NULL but pa is NOT, assuming b is greater");
	return -1;
    }

    /*
     * deference the item pointers
     */
    p_a = *(struct item const **)pa;
    p_b = *(struct item const **)pb;

    /*
     * compare the fts_path item elements in a case independent way
     */
    cmp = path_icmp(p_a->fts_path, p_b->fts_path);
    if (cmp < 0) {
	return -1;
    } else if (cmp > 0) {
	return 1;
    }
    return 0;
}


/*
 * sort_walk_stat - sort paths in a case dependent way in each dynamic array in a walk_stat
 *
 * given:
 *	wset_p	    pointer to a struct walk_set
 *
 * NOTE: This function does not return on an internal error.
 *
 * NOTE: NULL pointers are sorted later than all strings.
 */
void
sort_walk_stat(struct walk_stat *wstat_p)
{
    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(68, __func__, "called with NULL wstat_p");
	not_reached();
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(69, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }

    /*
     * sort by canonicalized paths, items by type - See enum allowed_type
     */
    dyn_array_qsort(wstat_p->file, item_cmp);
    dyn_array_qsort(wstat_p->dir, item_cmp);
    dyn_array_qsort(wstat_p->symlink, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->not_file, item_cmp);
    dyn_array_qsort(wstat_p->not_dir, item_cmp);
    dyn_array_qsort(wstat_p->not_symlink, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->filedir, item_cmp);
    dyn_array_qsort(wstat_p->filesym, item_cmp);
    dyn_array_qsort(wstat_p->dirsym, item_cmp);
    dyn_array_qsort(wstat_p->filedirsym, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->not_filedir, item_cmp);
    dyn_array_qsort(wstat_p->not_filesym, item_cmp);
    dyn_array_qsort(wstat_p->not_dirsym, item_cmp);
    dyn_array_qsort(wstat_p->not_filedirsym, item_cmp);

    /*
     * sort by canonicalized paths, items by level - See enum level
     */
    dyn_array_qsort(wstat_p->top, item_cmp);
    dyn_array_qsort(wstat_p->shallow, item_cmp);
    dyn_array_qsort(wstat_p->too_deep, item_cmp);
    dyn_array_qsort(wstat_p->below, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->any_level, item_cmp);
    dyn_array_qsort(wstat_p->unknown_level, item_cmp);

    /*
     * sort by canonicalized paths, items by walk_rule - see booleans in struct walk_rule
     */
    dyn_array_qsort(wstat_p->required, item_cmp);
    dyn_array_qsort(wstat_p->optional, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->non_empty, item_cmp);
    dyn_array_qsort(wstat_p->empty, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->free, item_cmp);
    dyn_array_qsort(wstat_p->counted, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->counted_file, item_cmp);
    dyn_array_qsort(wstat_p->counted_dir, item_cmp);
    dyn_array_qsort(wstat_p->counted_sym, item_cmp);
    dyn_array_qsort(wstat_p->counted_other, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->ignore, item_cmp);
    dyn_array_qsort(wstat_p->not_ignore, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->prohibit, item_cmp);
    dyn_array_qsort(wstat_p->not_prohibit, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->unmatched, item_cmp);

    /*
     * sort by path problem
     */
    dyn_array_qsort(wstat_p->unsafe, item_cmp);
    dyn_array_qsort(wstat_p->abs, item_cmp);
    dyn_array_qsort(wstat_p->too_long_path, item_cmp);
    dyn_array_qsort(wstat_p->too_long_path, item_cmp);
    /**/
    dyn_array_qsort(wstat_p->fts_err, item_cmp);
    dyn_array_qsort(wstat_p->safe, item_cmp);

    /*
     * sort by descendant state
     */
    dyn_array_qsort(wstat_p->visit, item_cmp);
    dyn_array_qsort(wstat_p->prune, item_cmp);

    /*
     * sort by canonicalized paths, all items
     */
    dyn_array_qsort(wstat_p->all, item_cmp);
    return;
}


/*
 * sort_walk_istat - sort paths in a case independent way in each dynamic array in a walk_stat
 *
 * given:
 *	wset_p	    pointer to a struct walk_set
 *
 * NOTE: This function does not return on an internal error.
 *
 * NOTE: NULL pointers are sorted later than all strings.
 */
void
sort_walk_istat(struct walk_stat *wstat_p)
{
    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(70, __func__, "called with NULL wstat_p");
	not_reached();
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(71, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }

    /*
     * sort by canonicalized paths, items by type - See enum allowed_type
     */
    dyn_array_qsort(wstat_p->file, item_icmp);
    dyn_array_qsort(wstat_p->dir, item_icmp);
    dyn_array_qsort(wstat_p->symlink, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->not_file, item_icmp);
    dyn_array_qsort(wstat_p->not_dir, item_icmp);
    dyn_array_qsort(wstat_p->not_symlink, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->filedir, item_icmp);
    dyn_array_qsort(wstat_p->filesym, item_icmp);
    dyn_array_qsort(wstat_p->dirsym, item_icmp);
    dyn_array_qsort(wstat_p->filedirsym, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->not_filedir, item_icmp);
    dyn_array_qsort(wstat_p->not_filesym, item_icmp);
    dyn_array_qsort(wstat_p->not_dirsym, item_icmp);
    dyn_array_qsort(wstat_p->not_filedirsym, item_icmp);

    /*
     * sort by canonicalized paths, items by level - See enum level
     */
    dyn_array_qsort(wstat_p->top, item_icmp);
    dyn_array_qsort(wstat_p->shallow, item_icmp);
    dyn_array_qsort(wstat_p->too_deep, item_icmp);
    dyn_array_qsort(wstat_p->below, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->any_level, item_icmp);
    dyn_array_qsort(wstat_p->unknown_level, item_icmp);

    /*
     * sort by canonicalized paths, items by walk_rule - see booleans in struct walk_rule
     */
    dyn_array_qsort(wstat_p->required, item_icmp);
    dyn_array_qsort(wstat_p->optional, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->non_empty, item_icmp);
    dyn_array_qsort(wstat_p->empty, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->free, item_icmp);
    dyn_array_qsort(wstat_p->counted, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->counted_file, item_icmp);
    dyn_array_qsort(wstat_p->counted_dir, item_icmp);
    dyn_array_qsort(wstat_p->counted_sym, item_icmp);
    dyn_array_qsort(wstat_p->counted_other, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->ignore, item_icmp);
    dyn_array_qsort(wstat_p->not_ignore, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->prohibit, item_icmp);
    dyn_array_qsort(wstat_p->not_prohibit, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->unmatched, item_icmp);

    /*
     * sort by path problem
     */
    dyn_array_qsort(wstat_p->unsafe, item_icmp);
    dyn_array_qsort(wstat_p->abs, item_icmp);
    dyn_array_qsort(wstat_p->too_long_path, item_icmp);
    dyn_array_qsort(wstat_p->too_long_path, item_icmp);
    /**/
    dyn_array_qsort(wstat_p->fts_err, item_icmp);
    dyn_array_qsort(wstat_p->safe, item_icmp);

    /*
     * sort by descendant state
     */
    dyn_array_qsort(wstat_p->visit, item_icmp);
    dyn_array_qsort(wstat_p->prune, item_icmp);

    /*
     * sort, by canonicalized paths, all items
     */
    dyn_array_qsort(wstat_p->all, item_icmp);
    return;
}


/*
 * chk_walk - check the walk_stat the walk_rule's of the walk_set
 *
 * given:
 *	wset_p	    pointer to a struct walk_set
 *	stream	    != NULL ==> print error messages in err
 *		    NULL ==> do not print any error information
 *	max_file    max counted files, ANY_COUNT ==> unlimited, NO_COUNT ==> none allowed
 *	max_dir     max counted directories, ANY_COUNT ==> unlimited, NO_COUNT ==> none allowed
 *	max_sym     max counted symlinks, ANY_COUNT ==> unlimited, NO_COUNT ==> none allowed
 *	max_other   max counted non-file/dir/symlinks, ANY_COUNT ==> unlimited, NO_COUNT ==> none allowed
 *	walk_done   true ==> mark walked as true, walking as false
 *		    false ==> mark nothing
 *
 * return:
 *	true ==> no errors found
 *	false ==> some errors found
 *
 * NOTE: This function does not return on an internal error.
 */
bool
chk_walk(struct walk_stat *wstat_p, FILE *stream,
	 int_least32_t max_file, int_least32_t max_dir, int_least32_t max_sym, int_least32_t max_other, bool walk_done)
{
    struct walk_set *wset_p;	    /* pointer to a walk set */
    struct walk_rule *rule_p;	    /* pointer to a walk rule */
    intmax_t prohibit_count = 0;    /* number of prohibited items found */
    intmax_t too_deep_count = 0;    /* number of items whose path is too deep */
    intmax_t unsafe_count = 0;	    /* number of items whose path unsafe */
    intmax_t abs_count = 0;	    /* number of items whose path is absolute */
    intmax_t too_long_path_count = 0;	/* number of items whose path is too deep */
    intmax_t too_long_name_count = 0;	/* number of items whose path is too deep */
    intmax_t fts_err_count = 0;		/* number of items causing fts_read(3) errors. */
    intmax_t counted_file_count = 0;	/* number of counted (non-free) files */
    intmax_t counted_dir_count = 0;	/* number of counted (non-free) directories */
    intmax_t counted_sym_count = 0;	/* number of counted (non-free) symlinks */
    intmax_t counted_other_count = 0;	/* number of counted (non-free) non-file/dir/symlink items */
    struct item *i_p;		    /* pointer to an element in the dynamic array */
    bool ret = true;		    /* true ==> no errors found, false ==> some errors found */
    int i;

    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(72, __func__, "called with NULL wstat_p");
	not_reached();
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(73, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }
    wset_p = wstat_p->set;

    /*
     * scan for missing required items
     */
    for (rule_p = wset_p->rule, i=0; rule_p->pattern != NULL; ++rule_p, ++i) {

	/*
	 * look for a required rule
	 */
	if (rule_p->required) {

	    /*
	     * every rule that is required MUST have a match_count > 0
	     */
	    if (rule_p->match_count <= 0) {

		/* NOTE: required item not found */
		if (stream != NULL) {
		    fmsg(stream, "%s required: %s not found", wset_p->context, rule_p->pattern);
		    dbg(DBG_LOW, "%s required item[%d]: %s not found", wset_p->context, i, rule_p->pattern);
		}
		ret = false;
	    }
	}
    }

    /*
     * scan for prohibited items
     */
    prohibit_count = dyn_array_tell(wstat_p->prohibit);
    if (prohibit_count > 0) {

	/* NOTE: 1 or more prohibited items found */
	if (stream != NULL) {

	    /*
	     * report the prohibited count and item paths
	     */
	    fmsg(stream, "%s found %jd prohibited items", wset_p->context, prohibit_count);
	    for (i=0; i < prohibit_count; ++i) {

		/*
		 * print the fts_path of the current item
		 */
		i_p = dyn_array_value(wstat_p->prohibit, struct item *, i);
		if (i_p != NULL) {
		    fmsg(stream, "   prohibited %s%s: %s",
				 (i_p->st_size ? "" : "empty "), file_type_name(i_p->st_mode), i_p->fts_path);
		} else {
		    err(74, __func__, "item[%d] fts_path #0 is NULL", i);
		    not_reached();
		}
	    }
	}
	ret = false;
    }

    /*
     * scan for items too deep
     */
    too_deep_count = dyn_array_tell(wstat_p->too_deep);
    if (too_deep_count > 0) {

	/* NOTE: 1 or more items that are too deep was found */
	if (stream != NULL) {

	    /*
	     * report the too deep count and item paths
	     */
	    fmsg(stream, "%s found %jd items whose directory level is too deep", wset_p->context, too_deep_count);
	    for (i=0; i < too_deep_count; ++i) {

		/*
		 * print the fts_path of the current item
		 */
		i_p = dyn_array_value(wstat_p->too_deep, struct item *, i);
		if (i_p != NULL) {
		    fmsg(stream, "   %s is too deep: %s", file_type_name(i_p->st_mode), i_p->fts_path);
		} else {
		    err(75, __func__, "item[%d] fts_path #1 is NULL", i);
		    not_reached();
		}
	    }
	}
	ret = false;
    }

    /*
     * scan for unsafe paths
     */
    unsafe_count = dyn_array_tell(wstat_p->unsafe);
    if (unsafe_count > 0) {

	/* NOTE: 1 or more paths that are unsafe */
	if (stream != NULL) {

	    /*
	     * report the path too long
	     */
	    fmsg(stream, "%s found %jd unsafe paths", wset_p->context, unsafe_count);
	    for (i=0; i < unsafe_count; ++i) {

		/*
		 * print the fts_path of the current item
		 */
		i_p = dyn_array_value(wstat_p->unsafe, struct item *, i);
		if (i_p != NULL) {
		    fmsg(stream, "   %s has an unsafe path: %s", file_type_name(i_p->st_mode), i_p->fts_path);
		} else {
		    err(76, __func__, "item[%d] fts_path #2 is NULL", i);
		    not_reached();
		}
	    }
	}
	ret = false;
    }

    /*
     * scan for absolute paths
     */
    abs_count = dyn_array_tell(wstat_p->abs);
    if (abs_count > 0) {

	/* NOTE: 1 or more paths that are absolute (not relative) */
	if (stream != NULL) {

	    /*
	     * report the path too long
	     */
	    fmsg(stream, "%s found %jd non-relative paths", wset_p->context, abs_count);
	    for (i=0; i < abs_count; ++i) {

		/*
		 * print the fts_path of the current item
		 */
		i_p = dyn_array_value(wstat_p->abs, struct item *, i);
		if (i_p != NULL) {
		    fmsg(stream, "   %s has an absolute path: %s", file_type_name(i_p->st_mode), i_p->fts_path);
		} else {
		    err(77, __func__, "item[%d] fts_path #3 is NULL", i);
		    not_reached();
		}
	    }
	}
	ret = false;
    }

    /*
     * scan for paths too long
     */
    too_long_path_count = dyn_array_tell(wstat_p->too_long_path);
    if (too_long_path_count > 0) {

	/* NOTE: 1 or more paths that are too long */
	if (stream != NULL) {

	    /*
	     * report the path too long
	     */
	    fmsg(stream, "%s found %jd too long path", wset_p->context, too_long_path_count);
	    for (i=0; i < too_long_path_count; ++i) {

		/*
		 * print the fts_path of the current item
		 */
		i_p = dyn_array_value(wstat_p->too_long_path, struct item *, i);
		if (i_p != NULL) {
		    fmsg(stream, "   %s path is too long: %s", file_type_name(i_p->st_mode), i_p->fts_path);
		} else {
		    err(78, __func__, "item[%d] fts_path #4 is NULL", i);
		    not_reached();
		}
	    }
	}
	ret = false;
    }

    /*
     * scan for path with a path element too long
     */
    too_long_name_count = dyn_array_tell(wstat_p->too_long_name);
    if (too_long_name_count > 0) {

	/* NOTE: 1 or more paths with path elements too long */
	if (stream != NULL) {

	    /*
	     * report the paths with a path element too long
	     */
	    fmsg(stream, "%s found %jd items with path elements too long", wset_p->context, too_long_name_count);
	    for (i=0; i < too_long_name_count; ++i) {

		/*
		 * print the fts_path of the current item
		 */
		i_p = dyn_array_value(wstat_p->too_long_name, struct item *, i);
		if (i_p != NULL) {
		    fmsg(stream, "   %s has path element that is too long: %s", file_type_name(i_p->st_mode), i_p->fts_path);
		} else {
		    err(79, __func__, "item[%d] fts_path #5 is NULL", i);
		    not_reached();
		}
	    }
	}
	ret = false;
    }

    /*
     * scan for path with a item that caused an fts(3) error, e.g., cannot read dir, cannot stat(2), etc.
     */
    fts_err_count = dyn_array_tell(wstat_p->fts_err);
    if (fts_err_count > 0) {

	/* NOTE: 1 or more paths with path elements too long */
	if (stream != NULL) {

	    /*
	     * report the paths with a path element too long
	     */
	    fmsg(stream, "%s found %jd items that caused hierarchy traverse errors", wset_p->context, fts_err_count);
	    for (i=0; i < fts_err_count; ++i) {

		/*
		 * print the fts_path of the current item
		 */
		i_p = dyn_array_value(wstat_p->fts_err, struct item *, i);
		if (i_p != NULL) {
		    fmsg(stream, "   %s causing a hierarchy traverse error: %s", file_type_name(i_p->st_mode), i_p->fts_path);
		} else {
		    err(80, __func__, "item[%d] fts_path #6 is NULL", i);
		    not_reached();
		}
	    }
	}
	ret = false;
    }

    /*
     * verify we do not have too many counted (non-free) files
     */
    counted_file_count = dyn_array_tell(wstat_p->counted_file);
    if (max_file > 0 && counted_file_count > max_file) {
	fmsg(stream, "files: number of counted (non-free): %jd > maximum allowed counted: %d",
		     counted_file_count, max_file);
	ret = false;
    } else if (max_file == NO_COUNT && counted_file_count > 0) {
	fmsg(stream, "files: no counted (non-free) allowed, found: %jd", counted_file_count);
	ret = false;
    }

    /*
     * verify we do not have too many counted (non-free) dirs
     */
    counted_dir_count = dyn_array_tell(wstat_p->counted_dir);
    if (max_dir > 0 && counted_dir_count > max_dir) {
	fmsg(stream, "directories: number of counted (non-free): %jd > maximum allowed counted: %d",
		     counted_dir_count, max_dir);
	ret = false;
    } else if (max_dir == NO_COUNT && counted_dir_count > 0) {
	fmsg(stream, "directories: no counted (non-free) allowed, found: %jd", counted_dir_count);
	ret = false;
    }

    /*
     * verify we do not have too many counted (non-free) syms
     */
    counted_sym_count = dyn_array_tell(wstat_p->counted_sym);
    if (max_sym > 0 && counted_sym_count > max_sym) {
	fmsg(stream, "symlinks: number of counted (non-free): %jd > maximum allowed counted: %d",
		     counted_sym_count, max_sym);
	ret = false;
    } else if (max_sym == NO_COUNT && counted_sym_count > 0) {
	fmsg(stream, "symlinks: no counted (non-free) allowed, found: %jd", counted_sym_count);
	ret = false;
    }

    /*
     * verify we do not have too many counted (non-free) others
     */
    counted_other_count = dyn_array_tell(wstat_p->counted_other);
    if (max_other > 0 && counted_other_count > max_other) {
	fmsg(stream, "non-files/dirs/symlinks: number of counted (non-free): %jd > maximum allowed counted: %d",
		     counted_other_count, max_other);
	ret = false;
    } else if (max_other == NO_COUNT && counted_other_count > 0) {
	fmsg(stream, "non-files/dirs/symlinks: no counted (non-free) allowed, found: %jd", counted_other_count);
	ret = false;
    }

    /*
     * if requested, mark the end of the walk
     */
    if (walk_done) {
	wstat_p->walking = false;
	wstat_p->walked = true;
    }

    /*
     * report check results
     */
    return ret;
}


/*
 * fts_cmp - compare FTSENT fts_name strings case dependently via strcmp(3)
 *
 * NOTE: The fts_accpath, fts_path and fts_pathlen fields of the FTSENT may
 *	 never be used in this comparison.  We compare with fts_name instead.
 *
 * given:
 *	a	pointer to pointer to 1st FTSENT to compare
 *	b	pointer to pointer to 2nd FTSENT to compare
 *
 * returns:
 *	-1 if  a < b
 *	0  if  a == b
 *	1  if  a > b
 */
int
fts_cmp(const FTSENT **a, const FTSENT **b)
{
    int cmp = 0;	/* FTSENT comparison */

    /*
     * firewall
     */
    if (a == NULL) {
	fprintf(stderr, "%s: a is NULL\n", __func__);
	exit(81);
    }
    if (b == NULL) {
	fprintf(stderr, "%s: b is NULL\n", __func__);
	exit(82);
    }
    if (*a == NULL) {
	fprintf(stderr, "%s: *a is NULL\n", __func__);
	exit(83);
    }
    if (*b == NULL) {
	fprintf(stderr, "%s: *b is NULL\n", __func__);
	exit(84);
    }

    /*
     * compare FTSENTs
     *
     * Empty fts_name's are sorted AFTER non-empty fts_name's.
     * We adopt this storing style do that the sorted list will
     * have empty names at the end, similar to the "convention" that
     * a list of string pointers may end with a NULL pointer.
     */
    /* case: a is empty */
    if (*(*a)->fts_name == '\0') {

	/* a is empty */
	if (*(*b)->fts_name == '\0') {

	    /* a and b are both empty, and thus equal  */
	    return 0;

	} else {

	    /* empty a > non-empty b */
	    return 1;
	}

    /* case: b is empty, but a is non-empty */
    } else if (*(*b)->fts_name == '\0') {

	/* non-empty a < empty b */
	return -1;
    }

    /*
     * string compare FTSENTs
     */
    cmp = strcmp((*a)->fts_name, (*b)->fts_name);
    /* convert strcmp(3) return into -1, 0, or 1 to simplify debugging */
    cmp = ((cmp <= -1) ? -1 : ((cmp == 0) ? 0 : 1));

    /* return comparison result */
    return cmp;
}


/*
 * fts_icmp - compare FTSENT fts_name strings case dependently via strcasecmp(3)
 *
 * NOTE: The fts_accpath, fts_path and fts_pathlen fields of the FTSENT may
 *	 never be used in this comparison.  We compare with fts_name instead.
 *
 * given:
 *	a	pointer to pointer to 1st FTSENT to compare
 *	b	pointer to pointer to 2nd FTSENT to compare
 *
 * returns:
 *	-1 if  a < b
 *	0  if  a == b
 *	1  if  a > b
 */
int
fts_icmp(const FTSENT **a, const FTSENT **b)
{
    int cmp = 0;	/* FTSENT comparison */

    /*
     * firewall
     */
    if (a == NULL) {
	fprintf(stderr, "%s: a is NULL\n", __func__);
	exit(85);
    }
    if (b == NULL) {
	fprintf(stderr, "%s: b is NULL\n", __func__);
	exit(86);
    }
    if (*a == NULL) {
	fprintf(stderr, "%s: *a is NULL\n", __func__);
	exit(87);
    }
    if (*b == NULL) {
	fprintf(stderr, "%s: *b is NULL\n", __func__);
	exit(88);
    }

    /*
     * compare FTSENTs
     *
     * Empty fts_name's are sorted AFTER non-empty fts_name's.
     * We adopt this storing style do that the sorted list will
     * have empty names at the end, similar to the "convention" that
     * a list of string pointers may end with a NULL pointer.
     */
    /* case: a is empty */
    if (*(*a)->fts_name == '\0') {

	/* a is empty */
	if (*(*b)->fts_name == '\0') {

	    /* a and b are both empty, and thus equal  */
	    return 0;

	} else {

	    /* empty a > non-empty b */
	    return 1;
	}

    /* case: b is empty, but a is non-empty */
    } else if (*(*b)->fts_name == '\0') {

	/* non-empty a < empty b */
	return -1;
    }

    /*
     * string compare FTSENTs
     */
    cmp = strcmp((*a)->fts_name, (*b)->fts_name);
    /* convert strcmp(3) return into -1, 0, or 1 to simplify debugging */
    cmp = ((cmp <= -1) ? -1 : ((cmp == 0) ? 0 : 1));

    /* return comparison result */
    return cmp;
}


/*
 * record_fts_err - record an fts(3) error
 *
 * Add a path to a walk_stat's fts_err, prune and all.  Record it as a step.
 *
 * given:
 *	wstat_p	    - pointer to a struct walk_stat
 *	path	    - path that caused an fts(3) error
 *	st_size	    - size, in bytes in the form used by stat(2)
 *	st_mode	    - inode protection mode in the form used b by stat(2)
 *	fts_level   - canonicalized stack depth
 *
 * NOTE: This function does not return on an internal error.
 */
static void
record_fts_err(struct walk_stat *wstat_p, char const *path, off_t st_size, mode_t st_mode, int_least32_t fts_level)
{
    struct item *i_p = NULL;	    /* allocated item */

    /*
     * firewall - catch NULL ptrs
     */
    if (wstat_p == NULL) {
	err(89, __func__, "called with NULL wstat_p");
	not_reached();
    }
    if (wstat_p->fts_err == NULL) {
        err(90, __func__, "called with NULL wstat_p->fts_err");
        not_reached();
    }
    if (wstat_p->prune == NULL) {
        err(91, __func__, "called with NULL wstat_p->prune");
        not_reached();
    }
    if (wstat_p->all == NULL) {
        err(92, __func__, "called with NULL wstat_p->all");
        not_reached();
    }
    if (path == NULL) {
	err(93, __func__, "called with path topdir");
	not_reached();
    }

    /*
     * form an allocated item
     */
    i_p = alloc_item(path, st_size, st_mode, fts_level);

    /*
     * add path to fts_err
     */
    (void) dyn_array_append_value(wstat_p->fts_err, &i_p);

    /*
     * add path to fts_err
     */
    (void) dyn_array_append_value(wstat_p->prune, &i_p);

    /*
     * add path to all
     */
    (void) dyn_array_append_value(wstat_p->all, &i_p);

    /*
     * record that we have stepped
     */
    ++wstat_p->steps;		/* count this step */
    wstat_p->walking = true;	/* note that we are walking */
    dbg(DBG_MED, "fts prune: path: %s name: %s", i_p->fts_name, i_p->fts_name);
    return;
}


/*
 * fts_walk - walk a file system tree
 *
 * Given an initialized struct walk_stat, traverse a file system tree recording
 * matches against the struct walk_rule array of the struct walk_set.
 *
 * NOTE: You need to call init_walk_stat() first in order to setup the struct walk_stat first.
 *
 * given:
 *	wstat_p		    - pointer to a struct walk_stat to create
 *
 * return:
 *	true ==> no errors found
 *	false ==> some errors found
 *
 * NOTE: This function does not return on an internal error.
 */
bool
fts_walk(struct walk_stat *wstat_p)
{
    char const **path_set;	    /* NULL terminated array of character pointers */
    FTS *ftsp = NULL;		    /* file hierarchy structure */
    FTSENT *ftsent = NULL;	    /* information on a file under the file hierarchy structure */
    char *subpath = NULL;	    /* path that is below canonicalized topdir + / (slash) */
    int fts_set_ret = -1;	    /* fts_set() return */
    int fts_close_ret = -1;	    /* fts_close() return */
    bool process = false;	    /* false ==> prune walk below the path, true ==> continue to walk path */

    /*
     * firewall
     */
    if (wstat_p == NULL) {
	err(94, __func__, "wstat_p is NULL");
	not_reached();
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(95, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }

    /*
     * form a NULL terminated array of strings
     */
    errno = 0;	/* pre-clear for warnp() */
    path_set = calloc(2, sizeof(char const *));
    if (path_set == NULL) {

	/* warn about calloc failure */
	warnp(__func__, "calloc of 2 string pointers failed");
	return false;
    }
    path_set[0] = wstat_p->topdir;
    path_set[1] = NULL;

    /*
     * "open" the file hierarchy
     *
     * NOTE: If wstat_p->topdir is a symlink, we will reference where is points, however for
     *	     any symlink we find lower down, we will see the symbolic links themselves.
     */
    errno = 0;	/* pre-clear for warnp() */
    ftsp = fts_open((char *const *)path_set, FTS_COMFOLLOW | FTS_NOCHDIR | FTS_PHYSICAL, fts_cmp);
    if (ftsp == NULL || errno != 0) {

	/* warn about fts_open failure */
	warnp(__func__, "filed to open for file hierarchy traversal: %s", strerror(errno));

	/* free storage and return indicating walk error */
	if (path_set != NULL) {
	    free(path_set);
	    path_set = NULL;
	}
	return false;
    }

    /*
     * traverse the entire file hierarchy
     */
    while ((ftsent = fts_read(ftsp)) != NULL) {

	/*
	 * paranoia - avoid NULL fts_path
	 *
	 * NOTE: This should never happpen, but in case there a fts_read(3) bug, we check.
	 */
	if (ftsent->fts_path == NULL) {

	    /* warn about fts_path failure */
	    warn(__func__, "fts_path was NULL during file hierarchy traversal of: %s,", wstat_p->topdir);

	    /* close file hierarchy traversal */
	    errno = 0;	/* pre-clear for warnp() */
	    fts_close_ret = fts_close(ftsp);
	    if (fts_close_ret != 0) {

		/* report problem on stderr, free storage and return indicating an traversing error */
		warnp(__func__, "fts_close() #0: failed: %d", fts_close_ret);
	    }

	    /* free storage and return indicating walk error */
	    if (path_set != NULL) {
		free(path_set);
		path_set = NULL;
	    }
	    return false;

	/*
	 * paranoia - avoid fts_path that is shorter than the canonicalized wstat_p->topdir length
	 *
	 * NOTE: This should never happen, but in case there a fts_read(3) bug, we check.
	 */
	} else if (ftsent->fts_pathlen < wstat_p->topdir_len) {

	    /* warn about fts_path length shorter than canonicalized wstat_p->topdir length */
	    warn(__func__, "fts_read produced fts_pathlen: %d < %zu for: %s",
			   ftsent->fts_pathlen, wstat_p->topdir_len, ftsent->fts_path);

	    /* close file hierarchy traversal */
	    errno = 0;	/* pre-clear for warnp() */
	    fts_close_ret = fts_close(ftsp);
	    if (fts_close_ret != 0) {

		/* report problem on stderr, free storage and return indicating an traversing error */
		warnp(__func__, "fts_close() #1: failed: %d", fts_close_ret);
	    }

	    /* free storage and return indicating walk error */
	    if (path_set != NULL) {
		free(path_set);
		path_set = NULL;
	    }
	    return false;

	/*
	 * skip wstat_p->topdir itself
	 *
	 * NOTE: Due to directory pre-order and post-order traversal, this may happen twice.
	 */
	} else if (ftsent->fts_pathlen == wstat_p->topdir_len && strcmp(wstat_p->topdir, ftsent->fts_path) == 0) {

	    /* silently skip wstat_p->topdir  */
	    dbg(DBG_V1_HIGH, "%s: skipping wstat_p->topdir: %s", __func__, wstat_p->topdir);
	    continue;

	/*
	 * paranoia - avoid fts_path that is same as canonicalized wstat_p->topdir length but isn't wstat_p->topdir
	 *
	 * NOTE: This should never happen, but in case there a fts_read(3) bug, we check.
	 */
	} else if (ftsent->fts_pathlen == wstat_p->topdir_len) {

	    /* warn about fts_path length same as canonicalized wstat_p->topdir length but isn't wstat_p->topdir */
	    warn(__func__, "fts_read() produced fts_pathlen: %d == %zu for wstat_p->topdir: %s != %s",
		 ftsent->fts_pathlen, wstat_p->topdir_len, wstat_p->topdir, ftsent->fts_path);

	    /* close file hierarchy traversal */
	    errno = 0;	/* pre-clear for warnp() */
	    fts_close_ret = fts_close(ftsp);
	    if (fts_close_ret != 0) {

		/* report problem on stderr, free storage and return indicating an traversing error */
		warnp(__func__, "fts_close() #2: failed: %d", fts_close_ret);
	    }

	    /* free storage and return indicating walk error */
	    if (path_set != NULL) {
		free(path_set);
		path_set = NULL;
	    }
	    return false;

	/* assertion: ftsent->fts_pathlen > wstat_p->topdir_len */

	/*
	 * paranoia - avoid fts_path that does NOT start with canonicalized wstat_p->topdir followed by / (slash)
	 *
	 * NOTE: This should never happen, but in case there is a fts_read(3) bug, we check.
	 */
	} else if (ftsent->fts_path[wstat_p->topdir_len] != '/' ||
		   strncmp(wstat_p->topdir, ftsent->fts_path, wstat_p->topdir_len) != 0) {

	    /* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
	    warn(__func__, "fts_read() produced fts_pathlen: %d == %zu for non-wstat_p->topdir: %s",
		 ftsent->fts_pathlen, wstat_p->topdir_len, ftsent->fts_path);

	    /* close file hierarchy traversal */
	    errno = 0;	/* pre-clear for warnp() */
	    fts_close_ret = fts_close(ftsp);
	    if (fts_close_ret != 0) {

		/* report problem on stderr, free storage and return indicating an traversing error */
		warnp(__func__, "fts_close() #3: failed: %d", fts_close_ret);
	    }

	    /* free storage and return indicating walk error */
	    if (path_set != NULL) {
		free(path_set);
		path_set = NULL;
	    }
	    return false;
	}

	/* assertion: ftsent->fts_path starts with wstat_p->topdir */

	/*
	 * determine the part of fts_path that is beyond the wstat_p->topdir
	 */
	subpath = &ftsent->fts_path[wstat_p->topdir_len+1];

	/*
	 * print information about the file
	 */
	switch (ftsent->fts_info) {

	case FTS_D:

	    /* debugging */
	    dbg(DBG_V1_HIGH, "%s: pre-order directory: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size: %lld", __func__, (long long)(ftsent->fts_statp ? ftsent->fts_statp->st_size : 0));
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /* record walk step with sub-path below canonicalized wstat_p->topdir */
	    process = record_step(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, NULL, NULL);
	    if (process == false) {

		/* prune walk below the path that caused an fts error */
		fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
		if (fts_set_ret != 0) {

		    /* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		    warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		    /* close file hierarchy traversal */
		    errno = 0;	/* pre-clear for warnp() */
		    fts_close_ret = fts_close(ftsp);
		    if (fts_close_ret != 0) {

			/* report problem on stderr, free storage and return indicating an traversing error */
			warnp(__func__, "fts_close() #7: failed: %d", fts_close_ret);
		    }

		    /* free storage and return indicating walk error */
		    if (path_set != NULL) {
			free(path_set);
			path_set = NULL;
		    }
		    return false;
		}
	    }
	    break;

	case FTS_DC:

	    /* debugging */
	    warn(__func__, "cycle causing directory fot: %s", ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "%s: cycle causing directory found: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size: %lld", __func__, (long long)(ftsent->fts_statp ? ftsent->fts_statp->st_size : 0));
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /* record walk step with sub-path below canonicalized wstat_p->topdir */
	    record_fts_err(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, ftsent->fts_level);
	    break;

	case FTS_DEFAULT:

	    /* debugging */
	    dbg(DBG_V1_HIGH, "%s: other type: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size: %lld", __func__, (long long)(ftsent->fts_statp ? ftsent->fts_statp->st_size : 0));
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /* record walk step with sub-path below canonicalized wstat_p->topdir */
	    process = record_step(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, NULL, NULL);
	    if (process == false) {

		/* prune walk below the path that caused an fts error */
		fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
		if (fts_set_ret != 0) {

		    /* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		    warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		    /* close file hierarchy traversal */
		    errno = 0;	/* pre-clear for warnp() */
		    fts_close_ret = fts_close(ftsp);
		    if (fts_close_ret != 0) {

			/* report problem on stderr, free storage and return indicating an traversing error */
			warnp(__func__, "fts_close() #7: failed: %d", fts_close_ret);
		    }

		    /* free storage and return indicating walk error */
		    if (path_set != NULL) {
			free(path_set);
			path_set = NULL;
		    }
		    return false;
		}
	    }
	    break;

	case FTS_DNR:

	    /* debugging */
	    warn(__func__, "non-readable directory for: %s", ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "%s: non-readable directory found: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size withheld due to FTS_DNR", __func__);
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);
	    dbg(DBG_V1_HIGH, "  %s: fts_errno: %d\n", __func__, ftsent->fts_errno);

	    /* record fts error */
	    record_fts_err(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, ftsent->fts_level);

	    /* prune walk below the path that caused an fts error */
	    fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
	    if (fts_set_ret != 0) {

		/* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		/* close file hierarchy traversal */
		errno = 0;	/* pre-clear for warnp() */
		fts_close_ret = fts_close(ftsp);
		if (fts_close_ret != 0) {

		    /* report problem on stderr, free storage and return indicating an traversing error */
		    warnp(__func__, "fts_close() #4: failed: %d", fts_close_ret);
		}

		/* free storage and return indicating walk error */
		if (path_set != NULL) {
		    free(path_set);
		    path_set = NULL;
		}
		return false;
	    }
	    break;

	case FTS_DOT:

	    /*
	     * This should never happen because we do not use FTS_SEEDOT with fts_open(3).
	     * We handle these as errors in case there is a fts_read(3) bug.
	     *
	     * We will silently "forgive" . (dot), but all other cases we return indicating walk error.
	     */

	    /*
	     * case: . (dot)
	     */
	    if (strcmp(ftsent->fts_name, ".") == 0) {

		/* debugging */
		dbg(DBG_V1_HIGH, "%s: ignoring FTS_DOT: . (dot) without FTS_SEEDOT: %s", __func__, ftsent->fts_path);
		dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
		dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
		dbg(DBG_V1_HIGH, "  %s: size: %lld", __func__, (long long)(ftsent->fts_statp ? ftsent->fts_statp->st_size : 0));
		dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /*
	     * case: .. (dot-dot)
	     */
	    } else if (strcmp(ftsent->fts_name, "..") == 0) {

		/* warn about .. returned when we did not set FTS_SEEDOT */
		warn(__func__, "FTS_DOT returned .. (dot-dot) without FTS_SEEDOT: %s", ftsent->fts_path);

		/* close file hierarchy traversal */
		errno = 0;	/* pre-clear for warnp() */
		fts_close_ret = fts_close(ftsp);
		if (fts_close_ret != 0) {

		    /* report problem on stderr, free storage and return indicating an traversing error */
		    warnp(__func__, "fts_close() #5: failed: %d", fts_close_ret);
		}

		/* free storage and return indicating walk error */
		if (path_set != NULL) {
		    free(path_set);
		    path_set = NULL;
		}
		return false;

	    /*
	     * case: FTS_DOT w/o . (dot) and w/o .. (dot-dot)
	     */
	    } else {

		/* warn about .. returned when we did not set FTS_SEEDOT */
		warn(__func__, "FTS_DOT returned neither . (dot) nor .. (dot-dot) without FTS_SEEDOT: %s", ftsent->fts_path);

		/* close file hierarchy traversal */
		errno = 0;	/* pre-clear for warnp() */
		fts_close_ret = fts_close(ftsp);
		if (fts_close_ret != 0) {

		    /* report problem on stderr, free storage and return indicating an traversing error */
		    warnp(__func__, "fts_close() #6: failed: %d", fts_close_ret);
		}

		/* free storage and return indicating walk error */
		if (path_set != NULL) {
		    free(path_set);
		    path_set = NULL;
		}
		return false;
	    }
	    break;

	case FTS_DP:

	    /*
	     * A directory being visited in post-order if normal, so we just silently ignore this case.
	     */

	    /* debugging */
	    dbg(DBG_V1_HIGH, "%s: ignoring FTS_DP: post-order directory: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size: %lld", __func__, (long long)(ftsent->fts_statp ? ftsent->fts_statp->st_size : 0));
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);
	    break;

	case FTS_ERR:

	    /* debugging */
	    warn(__func__, "fts error for: %s", ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "%s: error code: %d", __func__, ftsent->fts_errno);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name withheld due to FTS_ERR", __func__);
	    dbg(DBG_V1_HIGH, "  %s: size withheld due to FTS_ERR", __func__);
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);
	    dbg(DBG_V1_HIGH, "  %s: fts_errno: %d\n", __func__, ftsent->fts_errno);

	    /* record fts error */
	    record_fts_err(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, ftsent->fts_level);

	    /* prune walk below the path that caused an fts error */
	    fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
	    if (fts_set_ret != 0) {

		/* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		/* close file hierarchy traversal */
		errno = 0;	/* pre-clear for warnp() */
		fts_close_ret = fts_close(ftsp);
		if (fts_close_ret != 0) {

		    /* report problem on stderr, free storage and return indicating an traversing error */
		    warnp(__func__, "fts_close() #7: failed: %d", fts_close_ret);
		}

		/* free storage and return indicating walk error */
		if (path_set != NULL) {
		    free(path_set);
		    path_set = NULL;
		}
		return false;
	    }
	    break;

	case FTS_F:

	    /* debugging */
	    dbg(DBG_V1_HIGH, "%s: file: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size: %lld", __func__, (long long)(ftsent->fts_statp ? ftsent->fts_statp->st_size : 0));
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /* record walk step with sub-path below canonicalized wstat_p->topdir */
	    process = record_step(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, NULL, NULL);
	    if (process == false) {

		/* prune walk below the path that caused an fts error */
		fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
		if (fts_set_ret != 0) {

		    /* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		    warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		    /* close file hierarchy traversal */
		    errno = 0;	/* pre-clear for warnp() */
		    fts_close_ret = fts_close(ftsp);
		    if (fts_close_ret != 0) {

			/* report problem on stderr, free storage and return indicating an traversing error */
			warnp(__func__, "fts_close() #7: failed: %d", fts_close_ret);
		    }

		    /* free storage and return indicating walk error */
		    if (path_set != NULL) {
			free(path_set);
			path_set = NULL;
		    }
		    return false;
		}
	    }
	    break;

	case FTS_NS:

	    /* debugging */
	    warn(__func__, "stat failed for: %s", ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "%s: cannot stat: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size withheld due to FTS_NS", __func__);
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);
	    dbg(DBG_V1_HIGH, "  %s: fts_errno: %d\n", __func__, ftsent->fts_errno);

	    /*
	     * record fts error
	     *
	     * For FTS_NS, ftsent->fts_statp is not valid, so return 0 for size and mode.
	     */
	    record_fts_err(wstat_p, subpath, 0, 0, ftsent->fts_level);

	    /* prune walk below the path that caused an fts error */
	    fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
	    if (fts_set_ret != 0) {

		/* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		/* close file hierarchy traversal */
		errno = 0;	/* pre-clear for warnp() */
		fts_close_ret = fts_close(ftsp);
		if (fts_close_ret != 0) {

		    /* report problem on stderr, free storage and return indicating an traversing error */
		    warnp(__func__, "fts_close() #8: failed: %d", fts_close_ret);
		}

		/* free storage and return indicating walk error */
		if (path_set != NULL) {
		    free(path_set);
		    path_set = NULL;
		}
		return false;
	    }
	    break;

	case FTS_NSOK:

	    /*
	     * We should never see FTS_NSOK because we to not use FTS_NOSTAT with fts_open(3)
	     */

	    /* debugging */
	    warn(__func__, "stat not returned without FTS_NOSTAT for: %s", ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "%s: file: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size withheld due to FTS_NSOK", __func__);
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /*
	     * record fts error
	     *
	     * For FTS_NSOK, ftsent->fts_statp is not valid, so return 0 for size and mode.
	     */
	    record_fts_err(wstat_p, subpath, 0, 0, ftsent->fts_level);

	    /* prune walk below the path that caused an fts error */
	    fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
	    if (fts_set_ret != 0) {

		/* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		/* close file hierarchy traversal */
		errno = 0;	/* pre-clear for warnp() */
		fts_close_ret = fts_close(ftsp);
		if (fts_close_ret != 0) {

		    /* report problem on stderr, free storage and return indicating an traversing error */
		    warnp(__func__, "fts_close() #9: failed: %d", fts_close_ret);
		}

		/* free storage and return indicating walk error */
		if (path_set != NULL) {
		    free(path_set);
		    path_set = NULL;
		}
		return false;
	    }
	    break;

	case FTS_SL:

	    /* debugging */
	    dbg(DBG_V1_HIGH, "%s: symlink: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size: %lld", __func__, (long long)(ftsent->fts_statp ? ftsent->fts_statp->st_size : 0));
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /* record walk step with sub-path below canonicalized wstat_p->topdir */
	    process = record_step(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, NULL, NULL);
	    if (process == false) {

		/* prune walk below the path that caused an fts error */
		fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
		if (fts_set_ret != 0) {

		    /* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		    warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		    /* close file hierarchy traversal */
		    errno = 0;	/* pre-clear for warnp() */
		    fts_close_ret = fts_close(ftsp);
		    if (fts_close_ret != 0) {

			/* report problem on stderr, free storage and return indicating an traversing error */
			warnp(__func__, "fts_close() #7: failed: %d", fts_close_ret);
		    }

		    /* free storage and return indicating walk error */
		    if (path_set != NULL) {
			free(path_set);
			path_set = NULL;
		    }
		    return false;
		}
	    }
	    break;

	case FTS_SLNONE:

	    /* debugging */
	    dbg(DBG_V1_HIGH, "%s: symlink with missing target: %s", __func__, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name: %s", __func__, ftsent->fts_name);
	    dbg(DBG_V1_HIGH, "  %s: size withheld due to FTS_SLNONE", __func__);
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /* record walk step with sub-path below canonicalized wstat_p->topdir */
	    process = record_step(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, NULL, NULL);
	    if (process == false) {

		/* prune walk below the path that caused an fts error */
		fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
		if (fts_set_ret != 0) {

		    /* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		    warn(__func__, "fts_set() returned error: %d for path:  %s", fts_set_ret, ftsent->fts_path);

		    /* close file hierarchy traversal */
		    errno = 0;	/* pre-clear for warnp() */
		    fts_close_ret = fts_close(ftsp);
		    if (fts_close_ret != 0) {

			/* report problem on stderr, free storage and return indicating an traversing error */
			warnp(__func__, "fts_close() #7: failed: %d", fts_close_ret);
		    }

		    /* free storage and return indicating walk error */
		    if (path_set != NULL) {
			free(path_set);
			path_set = NULL;
		    }
		    return false;
		}
	    }
	    break;

	default:

	    /* debugging */
	    warn(__func__, "unknown fts_info value: %d for: %s", ftsent->fts_info, ftsent->fts_path);
	    dbg(DBG_V1_HIGH, "%s: unknown fts_info value: %d", __func__, ftsent->fts_info);
	    dbg(DBG_V1_HIGH, "  %s: subpath: %s", __func__, subpath);
	    dbg(DBG_V1_HIGH, "  %s: name withheld due unknown fts_info", __func__);
	    dbg(DBG_V1_HIGH, "  %s: size withheld due unknown fts_info", __func__);
	    dbg(DBG_V1_HIGH, "  %s: depth: %d\n", __func__, ftsent->fts_level);

	    /* record fts error */
	    record_fts_err(wstat_p, subpath, ftsent->fts_statp->st_size, ftsent->fts_statp->st_mode, ftsent->fts_level);

	    /* prune walk below the path that caused an fts error */
	    fts_set_ret = fts_set(ftsp, ftsent, FTS_SKIP);
	    if (fts_set_ret != 0) {

		/* warn about fts_path not starting with canonicalized wstat_p->topdir + / (slash) */
		warn(__func__, "fts_set() returned error: %d for path: %s",
		     fts_set_ret, ftsent->fts_path);

		/* close file hierarchy traversal */
		errno = 0;	/* pre-clear for warnp() */
		fts_close_ret = fts_close(ftsp);
		if (fts_close_ret != 0) {

		    /* report problem on stderr, free storage and return indicating an traversing error */
		    warnp(__func__, "fts_close() #8: failed: %d", fts_close_ret);
		}

		/* free storage and return indicating walk error */
		if (path_set != NULL) {
		    free(path_set);
		    path_set = NULL;
		}
		return false;
	    }
	    break;
	}
    }

    /*
     * close the file hierarchy structure
     */
    errno = 0;	/* pre-clear for warnp() */
    fts_close_ret = fts_close(ftsp);
    if (fts_close_ret != 0) {

	/* report problem on stderr, free storage and return indicating an traversing error */
	warnp(__func__, "fts_close() failed: %d", fts_close_ret);
	if (path_set != NULL) {
	    free(path_set);
	    path_set = NULL;
	}
	return false;
    }

    /*
     * free storage
     */
    if (path_set != NULL) {
	free(path_set);
	path_set = NULL;
    }

    /*
     * walk successful
     */
    return true;
}


/*
 * path_in_item_array
 *
 * Given a dynamic array of struct items for a struct item with a fts_path that is identical to c_path.
 *
 * NOTE: Because the functions such as record_step() work on canonicalized paths,
 *	 the c_path should be called with a canonicalized path using canonicalize_path():
 *
 *	    struct walk_stat wstat;
 *	    char const *c_path;
 *	    chat *path;
 *	    enum path_sanity sanity_p = PATH_ERR_UNSET;
 *	    size_t path_len = 0;
 *	    int_least32_t deep = -1;
 *	    struct item *i_p;
 *
 *	    ... call init_walk_stat() as neeed ...
 *
 *	    memset(&wstat, 0, sizeof(wstat));
 *	    init_walk_stat(&wstat, ...);
 *
 *	    ... set path as needed ...
 *
 *	    c_path = canonicalize_path(&wstat, path, &sanity, &path_len, &deep);
 *	    if (cpath == NULL) {
 *		... report that canonicalize_path had an internal failure and returned NULL ...
 *	    }
 *
 *	    ... look canonicalized path in the file dynamic array of struct item pointers ...
 *
 *	    i_p = path_in_item_array(c_path->file, c_path);
 *	    if (i_p == NULL) {
 *		.. process c_path NOT found in the file items dynamic array ..
 *	    } else {
 *		.. process c_path found in the file items dynamic array ..
 *	    }
 *
 *	    if (c_path != NULL) {
 *		free(c_path);
 *		c_path = NULL;
 *	    }
 *
 * given:
 *	item_array	    - dynamic array of pointers to struct item
 *	c_path		    - canonicalized path to search for on the struct walk_stat all dynamic array
 *
 * returns:
 *	!= NULL ==> struct item pointer where fts_path matches path
 *	NULL ==> path not found on the struct walk_stat all dynamic array, or internal error
 *
 * NOTE: This function does not return on an internal error.
 */
struct item *
path_in_item_array(struct dyn_array *item_array, char const *c_path)
{
    struct item *i_p;	    /* pointer to an element in the dynamic array */
    size_t c_path_len;	    /* length of c_path */
    intmax_t len;	    /* number of elements in the dynamic array */
    intmax_t i;

    /*
     * firewall
     */
    if (item_array == NULL) {
	err(96, __func__, "item_array is NULL");
	not_reached();
    }
    if (c_path == NULL) {
	err(97, __func__, "c_path is NULL");
	not_reached();
    }
    if (item_array->elm_size != sizeof(struct item *)) {
	err(98, __func__, "item_array->elm_size: %zu != sizeof(struct item *): %zu",
			  item_array->elm_size, sizeof(struct item *));
	not_reached();
    }

    /*
     * scan the all array
     */
    c_path_len = strlen(c_path);
    len = dyn_array_tell(item_array);
    for (i=0; i < len; ++i) {

	/*
	 * compare the fts_path of the current item
	 */
	i_p = dyn_array_value(item_array, struct item *, i);
	if (i_p == NULL) {
	    dbg(DBG_V1_HIGH, "%s: item_array[%zu] is NULL", __func__, i);
	} else if (c_path_len == i_p->fts_pathlen && strcmp(c_path, i_p->fts_path) == 0) {
	    /* report match found */
	    dbg(DBG_V1_HIGH, "%s: item_array[%zu] matches: %s", __func__, i, c_path);
	    return i_p;
	}
    }

    /*
     * report no match
     */
    dbg(DBG_V1_HIGH, "%s: c_path not found: %s", __func__, c_path);
    return NULL;
}


/*
 * path_in_walk_stat
 *
 * Given a struct walk_stat *wstat_p, search the wstat_p->all dynamic array of struct items
 * for a struct item with a fts_path that is identical to c_path.  We can the wstat_p->all
 * dynamic array because functions such as record_step() stores all valid items on this
 * dynamic array, regardless of the type of path.
 *
 * NOTE: Because the functions such as record_step() work on canonicalized paths,
 *	 the c_path should be called with a canonicalized path using canonicalize_path():
 *
 *	    struct walk_stat wstat;
 *	    char const *c_path;
 *	    chat *path;
 *	    enum path_sanity sanity_p = PATH_ERR_UNSET;
 *	    size_t path_len = 0;
 *	    int_least32_t deep = -1;
 *
 *	    ... call init_walk_stat() as neeed ...
 *
 *	    memset(&wstat, 0, sizeof(wstat));
 *	    init_walk_stat(&wstat, ...);
 *
 *	    ... set path as needed ...
 *
 *	    c_path = canonicalize_path(&wstat, path, &sanity, &path_len, &deep);
 *	    if (cpath == NULL) {
 *		... report that canonicalize_path had an internal failure and returned NULL ...
 *	    }
 *
 *	Do not forget that c_path is a calloced canonicalized path, so when finished
 *	calling this function, free c_path:
 *
 *	    struct item *i_p;
 *
 *	    ...
 *
 *	    i_p = path_in_walk_stat(&wstat, c_path);
 *	    if (i_p == NULL) {
 *		.. process c_path NOT found ..
 *	    } else {
 *		.. process c_path found ..
 *	    }
 *
 *	    if (c_path != NULL) {
 *		free(c_path);
 *		c_path = NULL;
 *	    }
 *
 * given:
 *	wstat_p		    - pointer to a struct walk_stat to create
 *	c_path		    - canonicalized path to search for on the struct walk_stat all dynamic array
 *
 * returns:
 *	!= NULL ==> struct item pointer where fts_path matches path
 *	NULL ==> path not found on the struct walk_stat all dynamic array, or internal error
 *
 * NOTE: This function does not return on an internal error.
 */
struct item *
path_in_walk_stat(struct walk_stat *wstat_p, char const *c_path)
{
    struct item *i_p;	    /* pointer to an element in the dynamic array */

    /*
     * firewall
     */
    if (wstat_p == NULL) {
	err(99, __func__, "wstat_p is NULL");
	not_reached();
    }
    if (c_path == NULL) {
	err(100, __func__, "c_path is NULL");
	not_reached();
    }

    /*
     * if walk_stat check fails, abort
     *
     * NOTE: Passing this check means that we do not have to do as much checking elsewhere
     *       in this function.
     */
    if (! chk_walk_stat(wstat_p)) {
	err(101, __func__, "wstat_p failed the chk_walk_stat function test suite");
	not_reached();
    }

    /*
     * scan the all array
     */
    i_p = path_in_item_array(wstat_p->all, c_path);
    if (i_p != NULL) {

	/* report match found */
	dbg(DBG_V1_HIGH, "%s: c_path found: %s", __func__, c_path);
	return i_p;
    }

    /*
     * report no match
     */
    dbg(DBG_V1_HIGH, "%s: c_path not found: %s", __func__, c_path);
    return NULL;
}
