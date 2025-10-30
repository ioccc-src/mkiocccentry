/*
 * walk - walk directory trees and tar listings
 *
 * "This is a serious journey, not a hobbit walking-party."
 *
 *	-- Gandalf to Pippin, the Fellowship of the Ring.
 *
 * IMPORTANT NOTE: While most of the applications for walk relates to walking
 *		   a directory tree, there are applications (related to the
 *		   tools such as txzchk(1)) where processing is perform on
 *		   the tar listing as well.  So a while traversing a file hierarchy
 *		   might be one application of walking, so to is processing
 *		   the listing of a tar(1) command.
 *
 * Copyright (c) 2025 by Landon Curt Noll and Cody Boone Ferguson. All Rights Reserved.
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


#if !defined(INCLUDE_WALK_H)
#    define  INCLUDE_WALK_H


/*
 * system includes
 */
#include <stdio.h>	    /* for NULL */
#include <stdbool.h>	    /* for bool, true, false */
#include <string.h>	    /* needed for string(3) related functions */
#include <regex.h>	    /* needed for regex(3) related functions */
#include <sys/stat.h>	    /* for st_mode and st_size types from stat(2) */
#include <fts.h>	    /* for fts – traverse a file hierarchy */

/*
 * needed for fnmatch(3) related functions and for the FNM_CASEFOLD symbol
 */
#if !defined(_GNU_SOURCE)
  #define _GNU_SOURCE
#endif
#include <fnmatch.h>

/*
 * library related includes
 */
#include "../dbg/dbg.h"			/* dbg - info, debug, warning, error, and usage message facility */
#include "../dyn_array/dyn_array.h"	/* dyn_array - dynamic array facility */
#include "../cpath/cpath.h"		/* canon_path - canonicalize paths */
#include "file_util.h"			/* for fprint() and fprstr() and friends */
#include "../jparse/util.h"		/* for fpr() and friends */

/*
 * util - common utility functions
 */
#include "util.h"


/*
 * item_count - count the number of items in a given dyn_array pointer in the struct walk_stat
 *
 * We pay attention to the ready element is true in the struct walk_stat as well as to of the
 * dyn_array pointer in struct walk_stat is non-NULL.  If any of those are false, a count of 0
 * is returned.  I.e., if the struct walk_stat is not setup, or if the dyn_array is not setup, 0 is returned.
 *
 * Example, the following will determine the number of file and directory items recorded in struct walk_stat wstat:
 *
 *	struct walk_stat wstat;
 *	intmax_t file_count;
 *	intmax_t dir_count;
 *
 *	file_count = item_count(&wstat, file);
 *	dir_count = item_count(&wstat, dir);
 */
#define item_count(wptr, dptr) \
    ((((wptr) == NULL || (wptr)->ready == false || (wptr)->(dptr) == NULL)) ? 0 : dyn_array_tell((wptr)->(dptr)))

/*
 * For functions such as chk_walk(), these COUNTED defines may be used
 * as args for max_files, max_dirs, max_syms, or max_other.
 */
#define ANY_COUNT (0)		/* any number of counted items */
#define NO_COUNT (-1)		/* no counted items */


/*
 * pattern_type - how to match a path item with a pattern
 */
enum pattern_type {
    MATCH_UNSET = 0,		/* not a valid pattern_type and/or pattern_type has not been assigned */

    MATCH_STR,			/* match item with the full item string */
    MATCH_STR_ANYCASE,		/* match item, ignoring case distinctions, the full item string */
    MATCH_PREFIX,		/* match the beginning of the item */
    MATCH_PREFIX_ANYCASE,	/* match the beginning of the item, ignoring case distinctions */
    MATCH_SUFFIX,		/* match the end of the item */
    MATCH_SUFFIX_ANYCASE,	/* match the end of the item, ignoring case distinctions */
    MATCH_FNMATCH,		/* match item, using fnmatch(3) using the flags: FNM_NOESCAPE */
    MATCH_FNMATCH_ANYCASE,	/* match item, using fnmatch(3) using the flags: FNM_NOESCAPE|FNM_CASEFOLD */
    MATCH_REGEX,		/* match item, using regexec(3) using p_preg and flags: REG_BASIC */
    MATCH_REGEX_ANYCASE,	/* match item, using regexec(3) using p_preg and flags: REG_BASIC|REG_ICASE */
    MATCH_EXT_REGEX,		/* match item, using regexec(3) using p_preg and flags: REG_EXTENDED */
    MATCH_EXT_REGEX_ANYCASE,	/* match item, using regexec(3) using p_preg and flags: REG_EXTENDED|REG_ICASE */

    MATCH_ANY,			/* match any item, pattern is ignored, a pattern of "*" is recommended */
};


/*
 * allowed_type - determine the file type(s) that are required
 *
 * With regards to symlinks and file walking: continuing the walk where the symlink points to or not
 * is up to the tool implementation.
 *
 * A symlink is an item independent of the target of that symlink and even if the target does not exist.
 * In terms of the fts_open(3) function, FTS_PHYSICAL should be used.  In terms of a tarball listing,
 * what a symlink points to has no effect on allowed_type as symlinks are treated as just symlinks.
 */
enum allowed_type {
    TYPE_UNSET = 0,		/* not a valid allowed_type and/or allowed_type has not been assigned */

    TYPE_FILE,			/* only items that exist and that are files match */
    TYPE_DIR,			/* only items that exist and that are directory match */
    TYPE_SYMLINK,		/* only items that exist and that are symlinks match */

    TYPE_NOT_FILE,		/* only items that exist and NOT file */
    TYPE_NOT_DIR,		/* only items that exist and NOT directory */
    TYPE_NOT_SYMLINK,		/* only items that exist and NOT symlink */

    TYPE_FILEDIR,		/* only items that exist and that are files, or directories match */
    TYPE_FILESYM,		/* only items that exist and that are files, or symlinks match */
    TYPE_DIRSYM,		/* only items that exist and that are directories, or symlinks match */
    TYPE_FILEDIRSYM,		/* only items that exist and that are files, directories, or directories match */

    TYPE_NOT_FILEDIR,		/* only items that exist and NOT file, NOR directory - symlinks DO match */
    TYPE_NOT_FILESYM,		/* only items that exist and NOT file, NOR symlink */
    TYPE_NOT_DIRSYM,		/* only items that exist and NOT directories, NOR symlink */
    TYPE_NOT_FILEDIRSYM,	/* only items that exist and NOT file, NOR directory, NOR symlink */

    TYPE_ANY,			/* any items, regardless of type AND those that do not exist, match */
};


/*
 * allowed_level - where a walk_rule applies, relative to topdir
 */
enum allowed_level {
    LEVEL_UNSET = 0,		/* a valid allowed_level and/or allowed_level has not been assigned */
    LEVEL_TOP,			/* walk_rule applies only to items directly in topdir */
    LEVEL_BELOW,		/* walk_rule applies only to items below but NOT under topdir */

    LEVEL_ANY,			/* walk_rule applies only to items at or below topdir at any level */
};


/*
 * item - describe a file, directory or other file system member
 */
struct item {
    char *fts_path;             /* malloced copy of the "root path" from topdir - typicality canonicalized */
    size_t fts_pathlen;         /* strlen(fts_path) */
    char *fts_name;             /* malloced copy of the "file name", i.e., basename of fts_path */
    size_t fts_namelen;         /* strlen(fts_name) */
    int_least32_t fts_level;	/* fts_path depth, 0 ==> topdir, 1 ==> directly under topdir, 2 ==> in sub-dir under topdir */
    off_t st_size;              /* file size, in bytes in struct stat st_size form */
    mode_t st_mode;             /* inode protection mode in struct stat st_mode form */
};


/*
 * walk_rule - rules for what to do when walking trees
 *
 * The p_preg element is only valid for a match of MATCH_REGEX,
 * MATCH_REGEX_ANYCASE, MATCH_EXT_REGEX, or MATCH_EXT_REGEX_ANYCASE. Such preg
 * values are initialized by regcomp(3) and later used by regexec(). In all
 * other match cases, p_preg is ignored.
 *
 * When initializing an array of walk_rules, the p_preg value may be initialized
 * to NULL. Before using an array of walk_rules, a walk_rule_setup() function
 * should be called so that any NULL p_preg may be replaced by a calls to
 * regcomp(&w->p_preg, w->pattern, cflags). For those pattern_type that do not
 * involve the regexec(3) facility, *p_preg should be set to NULL.
 *
 * In regards to cases where a fatal error is to be thrown, we recommend that
 * tools be able to report the problem on standard error AND exit non-zero AFTER
 * walking the tree. The choice of when to report the problem on standard error
 * is up to the tool implementation: right away or just before exiting non-zero.
 * A tool may wish to have mode to immediately print on stderr and exit non-zero
 * when first fatal error is thrown. The default (abort on first fatal error,
 * or collect all errors and report and exit after finishing the walk) is tool
 * implementation. You may wish to add a command line option to the tool to
 * switch to the non-default abort mode.
 *
 * When walking a tree and a walk_rule with ignore == true is matches for a
 * directory, ALL files under that ignored directory should be ignored. In
 * terms of fts(3) style tree walking, such an ignored directory may be used to
 * "prune" the tree walk, skipping over all paths under the ignored directory.
 *
 * When walking a tree and a walk_rule with ignore == true is matches for
 * non-directory such as a file, in terms of fts(3) style tree walking, it will
 * also be "pruned". Because the non-directory has nothing under it to "prune",
 * that action has no effect on the walk. Nevertheless the non-directory will be
 * indicated to the calling application as something to ignore. So there is
 * usefulness in declaring a file as something to ignore.
 *
 * When walking a tree and a walk_rule with prohibit == true is matches for a
 * directory, ALL files under that ignored directory should be ignored. In terms
 * of fts(3) style tree walking, such an ignored directory may be used to
 * "prune" the tree walk, skipping over all paths under the ignored directory.
 *
 * In the same way, when walking a tree and a walk_rule with ignore == true
 * matches for non-directory such as a file, it will also be "pruned". Because
 * the non-directory has nothing under it to "prune", that action has no effect
 * on the walk.
 *
 * NOTE: A pattern of "*" when the pattern_type is MATCH_FNMATCH or
 * MATCH_FNMATCH_ANYCASE will match an empty string path.
 *
 * The match_count value is to be incremented when a item matches, even if
 * ignore is true. For rules that are required, a match_count value 0 after
 * process all items is an error as it indicates that the required item is
 * missing.
 *
 * When initializing an array of walk_rules, the match_count values will be
 * initialized to 0.
 *
 * To match any item name, a pattern of "*" with a pattern_type of MATCH_ANY is
 * recommended.
 *
 * If pattern_type is TYPE_ANY, then the pattern is ignored. In this case
 * setting pattern to "*" is required.
 *
 * The idea behind any array of struct walk_rule is on a first match basis.
 * I.e., of a given item, the first struct walk_rule that applies of used. All
 * subsequent element in the struct walk_rule are ignored.
 *
 * Disallow problematic paths containing ".." (dot-dot):
 *
 * To avoid complications with path items containing ".." (dot-dot), use the
 * following early on in the struct walk_rule array to prohibit (and prune the
 * tree walk) such problematic paths:
 *
 *	{ "..",
 *	    false, false, false, false, true,
 *	    MATCH_STR, TYPE_ANY, LEVEL_ANY,
 *	    0, NULL },
 *
 * Suggested "catch all" walk_rules that may precede the final element of a
 * struct walk_rule array:
 *
 * To allow and count any file, use:
 *
 *	{ "*",
 *	   false, false, false, false, false,
 *	   MATCH_ANY, TYPE_FILE, LEVEL_ANY,
 *	   0, NULL },
 *
 * To allow and count any file or directory, use:
 *
 *	{ "*",
 *	   false, false, false, false, false,
 *	   MATCH_ANY, TYPE_FILEDIR, LEVEL_ANY,
 *	   0, NULL },
 *
 * To ignore anything else (i.e., item has no effect other than to prune the
         * tree walk), use:
 *
 *	{ "*",
 *	   false, false, true, false, false,
 *	   MATCH_ANY, TYPE_ANY, LEVEL_ANY,
 *	   0, NULL },
 *
 * To prohibit anything else (and to prune the tree walk), use:
 *
 *	{ "*",
 *	   false, false, false, false, true,
 *	   MATCH_ANY, TYPE_ANY, LEVEL_ANY,
 *	   0, NULL },
 *
 * Final element of a struct walk_rule array:
 *
 * An array of walk_rule structures MUST be terminated by a NULL pattern
 * walk_rule structure. When pattern is NULL, all other struct walk_rule are
 * ignored. To end an array of walk_rule structures, use:
 *
 *	{ NULL,
 *	   false, false, false, false, false,
 *	   MATCH_UNSET, TYPE_UNSET, LEVEL_UNSET,
 *	   0, NULL }
 *
 * When processing an item against an array of walk_rule structures and the
 * final NULL pattern is reached, the item will have no effect. To avoid this,
 * precede the final NULL pattern walk_rule with one of the above suggested
 * "catch all" walk_rule structures.
 */
struct walk_rule {
    char const *pattern;	    /* how to match path item, ignored if match == MATCH_ANY, NULL if last in a walk_rule array */

    bool const required;	    /* true ==> something MUST match this walk_rule, else a fatal error MUST be thrown */
    bool const non_empty;	    /* true ==> the item MUST have a size > 0 */
    bool const free;		    /* true ==> items matching this walk_rule do NOT add to max file or max dir limits */
    bool const ignore;		    /* true ==> items matching this walk_rule should NOT be processed by the tool */
    bool const prohibit;	    /* true ==> items matching this walk_rule MUST result in a fatal error being thrown */

    enum pattern_type const match;	    /* how to match item against pattern */
    enum allowed_type const allowed_type;   /* only items of type will match */
    enum allowed_level const allowed_level; /* when evaluating an item relative to topdir, which directly levels to apply */

    int match_count;		    /* number of times in a tree walk, this walk_rule is matched */
    regex_t *restrict p_preg;	    /* pointer to a regex_t pointer compiled by regcomp(3) */
};


/*
 * walk_set - set of 1 or more walk_rule for a given application and if needed application option set
 */
struct walk_set {
    char const *name;		    /* walk_set name as a NUL terminated string, for debugging purposes */
    bool ready;			    /* true ==> walk_set setup: p_preg setup if needed, count set, context string set, etc. */
    char *context;		    /* malloced string describing context (tool + options) for debugging purposes */
    int count;			    /* length of set NOT counting the final NULL walk_rule.pattern */
    struct walk_rule * const rule;  /* array of walk rules, ending with a NULL walk_rule.pattern */
};


/*
 * walk_stat - record information and stats about a complete walk
 *
 * In all cases, the ready boolean is a gate.  If ready == false, then none of the other elements
 * of this structure are valid.  When ready == false, elements that are pointers may be NULL.
 *
 * The topdir may be am empty string when there is no identifiable topdir, such as what
 * might happen when "walking" a tarball listing.
 *
 * The many struct dyn_array pointers are intended to record walk information for a given item category.
 * These are in the form of paths relative topdir.  The "tell" macros may be used to determine the count,
 * i.e., the number of items that belonging to that given category.  In some cases the application may
 * wish to use a non-zero category as an indication of an error (such as prohibited items found) and
 * throw a fatal error if/as needed.
 *
 * The main "walk" functions record items in order that they were found.  No attempt is made
 * to prevent an item from being recorded multiple times.  The fts(3) traverse a file hierarchy,
 * if called properly with reasonable options, shouldn't produce duplicate items.  It is theoretically
 * possible that a damaged file system could produce duplicate items.
 *
 * Nevertheless the "walk" functions, when appending to a struct dyn_array for a given category, will not
 * attempt to de-duplicate.  It is worth noting that duplicate item circumstances should be very rare,
 * and or a sign that someone is "playing games" with data and deserve what they get.  So we recommend
 * letting not to worry about the very rare possibility of duplicate items and letting such cases be.
 *
 * The "qsort" functions may be used fast the walk to "order" the category paths.  The "walk" functions
 * do NOT sort items during the walk.  All application is free to provide a sort comparison function
 * to the walk_qsort() function if sorted order is desired.
 *
 * For services such as the "fts(3) traverse a file hierarchy", tree pruning may be performed.
 * In such cases where parts of the file hierarchy are skipped due to tree pruning, no stats about
 * what may have been skipped will be recorded.
 *
 * Due to the number of dynamic arrays used, it is recommended that the application use the
 * walk_free() function when it no longer needs any of the walk_stat information.  This call
 * will free any non-NULL struct dyn_array pointers, re-initialize the set, change name and topdir to NULL,
 * and set all booleans to false.  The use of the walk_free() function should be used with care so
 * that the application does not use any data from the struct walk_stat directly, or if it does
 * use a duplicated and independent copy of that data.
 *
 * Because growing an dynamic array may result in the realloc(3) facility moving data, the
 * application should NOT assume that a pointers within struct walk_stat, or the pointer to
 * the data of a dynamic array, will be unchanged after a walk step.  This is because a walk step
 * may append data to the end of a dynamic array, that in turn may call the realloc(3) facility,
 * and in turn may move the very data that was pointed to before the step.  Therefore an application should
 * always re-access/re-acquire referenced data between each step, or make independent duplicate copies of
 * such data (that won't be subject to realloc(3) facility moving data), or wait until the walk is complete.
 * For this reason, the walking boolean was added.
 */
struct walk_stat {

    /* determine if other elements are valid */
    bool ready;				/* true ==> walk_set initialized, false ==> NONE of the other elements are valid */

    /* information supplied at the start of the walk */
    struct walk_set *set;		/* pointer 1 or more walk_rules */

    /* information a walk that may have started (or not), may be in progress (or not), may have finished (or not) */
    char *topdir;			/* malloced path to a topdir, or empty string */
    size_t topdir_len;			/* length of topdir in bytes */
    bool walked;			/* true ==> walk has completed, false ==> no walk has been performed */
    bool walking;			/* true ==> walk is in progress, false ==> walk is complete or no walk was ever started */
    bool skip;				/* true ==> some tree pruning was observed, false ==> no known tree pruning */
    bool tar_listing_used;		/* true ==> tarball listing walk, false ==> walk via fts(3) */
    int_least32_t max_path_len;		/* max canonicalized path length, 0 ==> no limit */
    int_least32_t max_filename_len;	/* max length of each component of path, 0 ==> no limit */
    int_least32_t max_depth;		/* max depth of subdirectory tree, 0 ==> no limit, <0 ==> reserved for future use */
    uintmax_t steps;			/* number of steps taken during the walk */

    /* skip certain canonicalized paths */
    struct dyn_array *skip_set;		/* skip processing any canonicalized path that matches a skip_set item */

    /* items by type - See enum allowed_type */
    struct dyn_array *file;		/* file items */
    struct dyn_array *dir;		/* directory items */
    struct dyn_array *symlink;		/* symlink items */
    /**/
    struct dyn_array *not_file;		/* NOT file items */
    struct dyn_array *not_dir;		/* NOT directory items */
    struct dyn_array *not_symlink;	/* NOT symlink items */
    /**/
    struct dyn_array *filedir;		/* file, or directory items */
    struct dyn_array *filesym;		/* file, or symlink items */
    struct dyn_array *dirsym;		/* directory, or symlink items */
    struct dyn_array *filedirsym;	/* file, or directory, or symlink items */
    /**/
    struct dyn_array *not_filedir;	/* an item that is neither a file, nor a directory */
    struct dyn_array *not_filesym;	/* an item that is neither a file, nor a symlink */
    struct dyn_array *not_dirsym;	/* an item that is neither a directory, nor a symlink */
    struct dyn_array *not_filedirsym;	/* an item that is neither a file, nor a directory, nor a symlink */

    /* items by level - See enum level */
    struct dyn_array *top;		/* items directly found in the topdir directory */
    struct dyn_array *shallow;		/* items at or below the topdir directory but NOT deeper than the depth limit */
    struct dyn_array *too_deep;		/* items deeper than the depth limit */
    struct dyn_array *below;		/* items below but NOT in topdir (i.e., not directly found in the topdir directory) */
    /**/
    struct dyn_array *any_level;	/* items at or below topdir at any level (i.e., at or below topdir) */
    struct dyn_array *unknown_level;	/* items of an unknown level or above the topdir directory */

    /* items by walk_rule - see booleans in struct walk_rule */
    struct dyn_array *required;	        /* item matching a walk_rule that is required */
    struct dyn_array *optional;		/* item matching a walk_rule that is NOT required (i.e., optional) */
    /**/
    struct dyn_array *non_empty;	/* item matching a walk_rule must have st_size > 0 */
    struct dyn_array *empty;		/* item matching a walk_rule and st_size is 0 */
    /**/
    struct dyn_array *free;		/* item matching a walk_rule that is free (i.e., not counted) */
    struct dyn_array *counted;		/* item matching a walk_rule that is NOT free (i.e., counted) */
    /**/
    struct dyn_array *counted_file;	/* file item matching a walk_rule that is free (not-counted) */
    struct dyn_array *counted_dir;	/* directory item matching a walk_rule that is free (not-counted) */
    struct dyn_array *counted_sym;	/* symlink item matching a walk_rule that is free (not-counted) */
    struct dyn_array *counted_other;	/* non-file/dir/symlink item matching a walk_rule that is free (not-counted) */
    /**/
    struct dyn_array *ignore;		/* item matching a walk_rule that is ignore */
    struct dyn_array *not_ignore;	/* item matching a walk_rule that is NOT ignore (i.e., processed) */
    /**/
    struct dyn_array *prohibit;		/* item matching a walk_rule that is prohibit */
    struct dyn_array *not_prohibit;	/* item matching a walk_rule that is NOT prohibit (i.e., allowed) */
    /**/
    struct dyn_array *unmatched;	/* item that did not match any walk_rule */

    /* items by path problem */
    struct dyn_array *unsafe;		/* path contains unsafe path elements, or .. (dot-dot) over topdir error */
    struct dyn_array *abs;		/* path is absolute, not a relative path */
    struct dyn_array *too_long_path;	/* path is too long */
    struct dyn_array *too_long_name;	/* element in path is too long */
    /**/
    struct dyn_array *fts_err;		/* caused an fts(3) error e.g., cannot read dir, cannot stat(2), etc. */
    struct dyn_array *safe;		/* all path elements are safe */

    /* item descendant state */
    struct dyn_array *visit;		/* OK to visit descendants */
    struct dyn_array *prune;		/* don't visit descendants, prune any walk at item */

    /* all items recorded */
    struct dyn_array *all;		/* all items - to allow them to be freed by free_walk_stat() */
};


/*
 * externals
 */
extern struct walk_set walk_mkiocccentry;	/* mkiocccentry walk_set */
extern struct walk_set walk_txzchk;		/* txzchk walk_set */
extern struct walk_set walk_chksubmit;		/* chksubmit walk_set */
extern struct walk_set walk_chkentry_S;		/* chkentry -S walk_set */
extern struct walk_set walk_chkentry_s;		/* chkentry -s walk_set */
extern struct walk_set walk_chkentry_w;		/* chkentry -w walk_set */


/*
 * walk_util.c external functions
 */
extern void free_walk_stat(struct walk_stat *wstat_p);
extern void init_walk_stat(struct walk_stat *wstat_p, char const *topdir, struct walk_set *set, char const *context,
			   size_t max_path_len, size_t max_filename_len, int_least32_t max_depth,
			   bool tar_listing_used);
extern bool record_step(struct walk_stat *wstat_p, char const *fts_path, off_t st_size, mode_t st_mode,
		        bool *dup_p, char const **cpath_ret);
extern void fprintf_walk_stat(FILE *stream, struct walk_stat *wstat_p);
extern void fprintf_walk_set(FILE *stream, struct walk_set *wset_p);
int path_cmp(const void *pa, const void *pb);
int path_icmp(const void *pa, const void *pb);
int item_cmp(void const *pa, void const *pb);
int item_icmp(void const *pa, void const *pb);
extern void sort_walk_stat(struct walk_stat *wstat_p);
extern void sort_walk_istat(struct walk_stat *wstat_p);
extern bool chk_walk(struct walk_stat *wstat_p, FILE *stream,
		     int_least32_t max_file, int_least32_t max_dir, int_least32_t max_sym, int_least32_t max_other, bool walk_done);
extern int fts_cmp(const FTSENT **a, const FTSENT **b);
extern int fts_icmp(const FTSENT **a, const FTSENT **b);
extern bool fts_walk(struct walk_stat *wstat_p);
extern char const *canonicalize_path(struct walk_stat *wstat_p, char const *fts_path,
                                     enum path_sanity *sanity_p, size_t *len_p, int_least32_t *depth_p);
extern bool skip_add(struct walk_stat *wstat_p, char const *fts_path);
extern struct item *path_in_item_array(struct dyn_array *item_array, char const *c_path);
extern struct item *path_in_walk_stat(struct walk_stat *wstat_p, char const *c_path);


#endif /* INCLUDE_WALK_H */
