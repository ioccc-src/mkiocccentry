/*
 * walk_tbl - how various tools and modes for those tools walk directory trees and tar listings
 *
 * "This is a serious journey, not a hobbit walking-party."
 *
 *	-- Gandalf to Pippin, the Fellowship of the Ring.
 *
 * IMPORTANT NOTE: While most of the applications for walk relates to walking
 *                 a directory tree, there are applications (related to the
 *                 tools such as txzchk(1)) where processing is perform on
 *                 the tar listing as well. So while traversing a file hierarchy
 *                 might be one application of walking, so too is processing
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


/*******************/
/* mkiocccentry(1) */
/*******************/


/*
 * mkiocccentry walk_rule set
 *
 * Path items are compared, in order, to each walk_rule UNTIL a match is found, OR until the NULL entry is found.
 */
static struct walk_rule walk_rule_set_mkiocccentry[] = {

    /*
     * (dot-dot)
     *
     * This struct walk_rule should be first !!!
     *
     * Avoid any complications with path items that are ".." (dot-dot).
     *
     * Regarding types: We match any type, not just ".." (dot-dot) directories, to avoid any potential for
     *			"funny business" (as the expression goes), to be extra safe, and as a safety check
     *			in case calls to functions such as fts(3) are not given the correct options.
     */
    { "..",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR, TYPE_ANY, LEVEL_ANY,			/* match whole string, any type, at any level */
	0, NULL },

    /*
     * (NOT file, NOR directory - symlinks DO match)
     *
     * This struct walk_rule should be second !!!
     *
     * Regarding types: We match symlinks, and we match things other than files & directories,
     *			so they won't be copied under workdir.
     */
    { "*",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_ANY, TYPE_NOT_FILEDIR, LEVEL_ANY,		/* match anything, symlink or other type, at any level */
	0, NULL },

    /*
     * COPYING*
     *
     * We ignore this name do that it won't be copied under workdir.  The mkiocccentry tool will ask the
     * submitter if they are willing to contribute under Rule 18.  We don't want such a file that
     * might be incompatible with Rule 18, so we will ignore anything we find under this name to be safe.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "COPYING",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * COPYRIGHT*
     *
     * We ignore this name do that it won't be copied under workdir.  The mkiocccentry tool will ask the
     * submitter if they are willing to contribute under Rule 18.  We don't want such a file that
     * might be incompatible with Rule 18, so we will ignore anything we find under this name to be safe.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "COPYRIGHT",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * LICENSE*
     *
     * We ignore this name do that it won't be copied under workdir.  The mkiocccentry tool will ask the
     * submitter if they are willing to contribute under Rule 18.  We don't want such a file that
     * might be incompatible with Rule 18, so we will ignore anything we find under this name to be safe.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "LICENSE",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * Makefile
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "Makefile",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * README.md
     *
     * This file is reserved for winning entries and it created by IOCCC judges.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "README.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * index.html
     *
     * This file is reserved for winning entries and it generated by IOCCC judges from README.md.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "index.html",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog
     *
     * We ignore so that those developing submission don't have to make clobber before running the mkiocccentry tool.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.alt
     *
     * We ignore so that those developing submission don't have to make clobber before running the mkiocccentry tool.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.alt",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.alt.c
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.c",
	false, false, true, false, false,		/* optional, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.alt.o
     *
     * We ignore so that those developing submission don't have to make clobber before running the mkiocccentry tool.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.o",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.c
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.c",
	true, false, true, false, false,		/* required, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.o
     *
     * We ignore so that those developing submission don't have to make clobber before running the mkiocccentry tool.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.o",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.orig.c
     *
     * This file is reserved for winning entries and it created by IOCCC judges.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.orig.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string, any type, at top level only */
	0, NULL },

    /*
     * remarks.md
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "remarks.md",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "try.alt.sh",
	false, true, true, false, false,		/* optional, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.sh
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "try.sh",
	false, true, true, false, false,		/* optional, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * . ((dot))
     *
     * In case the tree walking or tarball list functions encounter a "." (dot) directory, we ignore it.
     *
     * This will mean that the ".*" (dot star) walk_rule below will not match any "." (dot) directory.
     *
     * Regarding types: We match directories only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR, TYPE_DIR, LEVEL_ANY,			/* match whole string, directory, at any level */
	0, NULL },

    /*
     * .* (dot star)
     *
     * We things that start with "." (dot), for the convenience of submission development, so that they
     * do not get copied under the workdir by the mkiocccentry tool.  This also helps avoid passing
     * dot-files to later tools that might interfere with the IOCCC judging process.
     *
     * The mkiocccentry tool will explicitly create the .auth.json and .info.json files under workdir.
     * Should that workdir be "recycled" as a submission directory, this walk_rule will ignore them,
     * allowing the mkiocccentry tool to create new JSON files.
     *
     * NOTE: ".." (dot-dot) is processed first in this struct walk_rule array, and the directory "." (dot)
     *	     is processed above.
     *
     * Regarding types: We match .* (dot star) files, directories and symlinks only, and let "((anything))"
     *			rule below deal with any other types to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX, TYPE_FILEDIRSYM, LEVEL_ANY,	/* match beginning, file or directory or symlink, at any level */
	0, NULL },

    /*
     * 2YYY_*.tar.bz2
     *
     * We match any bzip2 compressed tarball file, at they are reserved for winning entries and it created by
     * bin tools that the IOCCC judges execute.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "2[0-9][0-9][0-9]_*.tar.bz2",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_FNMATCH_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* file glob any case, any type, at top level only */
	0, NULL },

    /*
     * Makefile ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.alt.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.alt.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * remarks.md ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "remarks.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * Makefile ((empty file))
     *
     * Regarding types: We match files.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * remarks.md ((empty file))
     *
     * Regarding types: We match files.
     */
    { "remarks.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 3RD to LAST !!!
     *
     * We match any file or directory.
     */
    { "*",
	false, false, false, false, false,		/* optional */
	MATCH_ANY, TYPE_FILEDIR, LEVEL_ANY,		/* match anything, file or directory type, at any level */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 2ND to LAST !!!
     *
     * We prohibit anything else and to prune the tree walk.
     *
     * Regarding types: Anything needs to match any type, as a number of the more type restrictive walk_rule
     *		        above will count on this walk_rule "catching" other types.
     */
    { "*",
	false, false, false, false, true,		/* prohibited */
	MATCH_ANY, TYPE_ANY, LEVEL_ANY,			/* match anything, any type, at any level */
	0, NULL },

    /*
     * NULL
     *
     * This MUST be LAST !!!
     *
     * This is a mandatory end an array of walk_rule structures.
     *
     * Regarding types: A NULL pattern means that all of the other elements of struct walk_rule,
     *			including the type, are ignored.
     */
    { NULL,
	false, false, false, false, false,		/* end of array */
	MATCH_UNSET, TYPE_UNSET, LEVEL_UNSET,		/* everything UNSET */
	0, NULL }
};


/*
 * mkiocccentry walk_set
 */
struct walk_set walk_mkiocccentry = {
    "mkiocccentry walk_rule set",	/* walk_set name - for debugging purposes */
    false,				/* true ==> walk_set initialized and setup */
    NULL,				/* NUL terminated string describing context (tool + options) for debugging purposes */
    0,					/* length of set NOT counting the final NULL walk_rule.pattern */
    walk_rule_set_mkiocccentry		/* array of walk rules, ending with a NULL walk_rule.pattern */
};


/*************/
/* txzchk(1) */
/*************/


/*
 * txzchk walk_rule set
 *
 * Path items are compared, in order, to each walk_rule UNTIL a match is found, OR until the NULL entry is found.
 */
static struct walk_rule walk_rule_set_txzchk[] = {

    /*
     * (dot-dot)
     *
     * This struct walk_rule should be first !!!
     *
     * Avoid any complications with path items that are ".." (dot-dot).
     *
     * Regarding types: We match any type, not just ".." (dot-dot) directories, to avoid any potential for
     *			"funny business" (as the expression goes) involving attempts to bypass checks
     *			made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     */
    { "..",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR, TYPE_ANY, LEVEL_ANY,			/* match whole string, any type, at any level */
	0, NULL },

    /*
     * (NOT file, NOR directory - symlinks DO match)
     *
     * This struct walk_rule should be second !!!
     *
     * Regarding types: We match symlinks, and we match things avoid any potential for
     *			"funny business" (as the expression goes) involving attempts to bypass checks
     *			made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     */
    { "*",
	false, false, false, false, true,		/* prohibited */
	MATCH_ANY, TYPE_NOT_FILEDIR, LEVEL_ANY,		/* match anything, symlink or other type, at any level */
	0, NULL },

    /*
     * COPYING*
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "COPYING",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * COPYRIGHT*
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "COPYRIGHT",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * LICENSE*
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "LICENSE",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * Makefile
     *
     * The mkiocccentry tool checks should have required something to match this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "Makefile",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * README.md
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "README.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * index.html
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "index.html",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.alt
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.alt",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.alt.c
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.c",
	false, false, true, false, false,		/* optional, free */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.alt.o
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.o",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.c
     *
     * The mkiocccentry tool checks should have required something to match this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.c",
	true, false, true, false, false,		/* required, free */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.o
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.o",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * prog.orig.c
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "prog.orig.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_ANY, LEVEL_TOP,		/* match whole string any case, any type, at top level only */
	0, NULL },

    /*
     * remarks.md
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "remarks.md",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "try.alt.sh",
	false, true, true, false, false,		/* optional, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.sh
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "try.sh",
	false, true, true, false, false,		/* optional, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * .auth.json
     *
     * This file is created by the mkiocccentry tool.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".auth.json",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * .info.json
     *
     * This file is created by the mkiocccentry tool.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".info.json",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * . ((dot))
     *
     * In case the tree walking or tarball list functions encounter a "." (dot) directory, we ignore it.
     *
     * This will mean that the ".*" (dot star) walk_rule below will not match any "." (dot) directory.
     *
     * Regarding types: We match directories only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR, TYPE_DIR, LEVEL_ANY,			/* match whole string, directory, at any level */
	0, NULL },

    /*
     * .* (dot star)
     *
     * The mkiocccentry tool creates a select set of ".*" (dot star) files that are managed by walk_rule entries
     * above, therefore the mkiocccentry tool checks should have prevented other ".*" (dot star) items.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * NOTE: ".." (dot-dot) is processed first in this struct walk_rule array, and the directory "." (dot)
     *	     is processed above.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX, TYPE_ANY, LEVEL_ANY,		/* match beginning, any type, at any level */
	0, NULL },

    /*
     * 2YYY_*.tar.bz2
     *
     * We match any bzip2 compressed tarball file, at they are reserved for winning entries and it created by
     * bin tools that the IOCCC judges execute.
     *
     * The mkiocccentry tool checks should have required something to match this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "2[0-9][0-9][0-9]_*.tar.bz2",
	false, false, false, false, true,		/* prohibited */
	MATCH_FNMATCH_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* file glob any case, any type, at top level only */
	0, NULL },

    /*
     * Makefile ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.alt.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.alt.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * remarks.md ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "remarks.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * .auth.json ((not a file))
     *
     * This file is created by the mkiocccentry tool.
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { ".auth.json",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * .info.json ((not a file))
     *
     * This file is created by the mkiocccentry tool.
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { ".info.json",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * Makefile ((empty file))
     *
     * Regarding types: We match files.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * remarks.md ((empty file))
     *
     * Regarding types: We match files.
     */
    { "remarks.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 3RD to LAST !!!
     *
     * We match any file or directory.
     */
    { "*",
	false, false, false, false, false,		/* optional */
	MATCH_ANY, TYPE_FILEDIR, LEVEL_ANY,		/* match anything, file or directory type, at any level */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 2ND to LAST !!!
     *
     * We prohibit anything else and to prune the tree walk.
     *
     * Regarding types: Anything needs to match any type, as a number of the more type restrictive walk_rule
     *		        above will count on this walk_rule "catching" other types.
     */
    { "*",
	false, false, false, false, true,		/* prohibited */
	MATCH_ANY, TYPE_ANY, LEVEL_ANY,			/* match anything, any type, at any level */
	0, NULL },

    /*
     * NULL
     *
     * This MUST be LAST !!!
     *
     * This is a mandatory end an array of walk_rule structures.
     *
     * Regarding types: A NULL pattern means that all of the other elements of struct walk_rule,
     *			including the type, are ignored.
     */
    { NULL,
	false, false, false, false, false,		/* end of array */
	MATCH_UNSET, TYPE_UNSET, LEVEL_UNSET,		/* everything UNSET */
	0, NULL }
};


/*
 * txzchk walk_set
 */
struct walk_set walk_txzchk = {
    "txzchk walk_rule set",		/* walk_set name - for debugging purposes */
    false,				/* true ==> walk_set initialized and setup */
    NULL,				/* NUL terminated string describing context (tool + options) for debugging purposes */
    0,					/* length of set NOT counting the final NULL walk_rule.pattern */
    walk_rule_set_txzchk		/* array of walk rules, ending with a NULL walk_rule.pattern */
};


/*******************************/
/* chksubmit(1) or chkentry -S */
/*******************************/


/*
 * chkentry -S walk_set
 *
 * Because chksubmit(1) calls chkentry -S, the chkentry -S case applies here.
 *
 * NOTE: The chkentry -S walk_rule set uses the same walk_rule set as the txzchk(1) tool.
 */
struct walk_set walk_chkentry_S = {
    "chkentry -S walk_rule set",	/* walk_set name - for debugging purposes */
    false,				/* true ==> walk_set initialized and setup */
    NULL,				/* NUL terminated string describing context (tool + options) for debugging purposes */
    0,					/* length of set NOT counting the final NULL walk_rule.pattern */
    walk_rule_set_txzchk		/* array of walk rules, ending with a NULL walk_rule.pattern */
};


/***************/
/* chkentry -s */
/***************/


/*
 * chkentry -s walk_rule set
 *
 * Path items are compared, in order, to each walk_rule UNTIL a match is found, OR until the NULL entry is found.
 */
static struct walk_rule walk_rule_set_chkentry_s[] = {

    /*
     * (dot-dot)
     *
     * This struct walk_rule should be first !!!
     *
     * Avoid any complications with path items that are ".." (dot-dot).
     *
     * Regarding types: We match any type, not just ".." (dot-dot) directories, to avoid any potential for
     *			"funny business" (as the expression goes) involving attempts to bypass checks
     *			made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     */
    { "..",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR, TYPE_ANY, LEVEL_ANY,			/* match whole string, any type, at any level */
	0, NULL },

    /*
     * (symlink)
     *
     * This struct walk_rule should be second !!!
     *
     * Regarding types: We match symlinks, which IOCCC judges may use while judging submissions.
     */
    { "*",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_ANY, TYPE_SYMLINK, LEVEL_ANY,		/* match anything, symlink type, at any level */
	0, NULL },

    /*
     * (NOT file, NOR directory, NOR symlink)
     *
     * This struct walk_rule should be third !!!
     *
     * Regarding types: We match symlinks, and we match things avoid any potential for
     *			"funny business" (as the expression goes) involving attempts to bypass checks
     *			made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     */
    { "*",
	false, false, false, false, true,		/* prohibited */
	MATCH_ANY, TYPE_NOT_FILEDIRSYM, LEVEL_ANY,	/* match anything, other type, at any level */
	0, NULL },

    /*
     * COPYING*
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "COPYING",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * COPYRIGHT*
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "COPYRIGHT",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * LICENSE*
     *
     * The mkiocccentry tool checks should have prevented anything from matching this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "LICENSE",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * Makefile
     *
     * The mkiocccentry tool checks should have required something to match this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "Makefile",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * README.md
     *
     * The IOCCC judges may add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "README.md",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * index.html
     *
     * The IOCCC judges may add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "index.html",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog
     *
     * The IOCCC judges may add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.alt
     *
     * The IOCCC judges may add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.alt.c
     *
     * The IOCCC judges may add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.c",
	false, false, true, false, false,		/* optional, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.alt.o
     *
     * The IOCCC judges may add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.o",
	false, false, true, false, false,		/* optional, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.c
     *
     * The mkiocccentry tool checks should have required something to match this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.c",
	true, false, true, false, false,		/* required, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.o
     *
     * The IOCCC judges may add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.o",
	false, false, true, false, false,		/* optional, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.orig.c
     *
     * The IOCCC judges MUST add this to a submission during judging rounds.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.orig.c",
	true, false, true, false, false,		/* required, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * remarks.md
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "remarks.md",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "try.alt.sh",
	false, true, true, false, false,		/* optional, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.sh
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "try.sh",
	false, true, true, false, false,		/* optional, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * .auth.json*
     *
     * The IOCCC judging process converted the non-prefixed file (see the walk_rule above) into this file.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".auth.json",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_PREFIX_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match beginning any case, file type, at top level only */
	0, NULL },

    /*
     * .info.json
     *
     * This file is created by the mkiocccentry tool.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".info.json",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * . ((dot))
     *
     * In case the tree walking or tarball list functions encounter a "." (dot) directory, we ignore it.
     *
     * This will mean that the ".*" (dot star) walk_rule below will not match any "." (dot) directory.
     *
     * Regarding types: We match directories only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR, TYPE_DIR, LEVEL_ANY,			/* match whole string, directory, at any level */
	0, NULL },

    /*
     * .* (dot star)
     *
     * The IOCCC judging process will create various ".*" (dot star) files, directories and symlinks.
     *
     * NOTE: ".." (dot-dot) is processed first in this struct walk_rule array, and the directory "." (dot)
     *	     is processed above.
     *
     * Regarding types: We match .* (dot star) files, directories and symlinks only, and let "((anything))"
     *			rule below deal with any other types to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX, TYPE_FILEDIRSYM, LEVEL_ANY,	/* match beginning, file or directory or symlink, at any level */
	0, NULL },

    /*
     * 2YYY_*.tar.bz2
     *
     * We match any bzip2 compressed tarball file, at they are reserved for winning entries and it created by
     * bin tools that the IOCCC judges execute.
     *
     * The mkiocccentry tool checks should have required something to match this walk_rule.
     * We match is again, in case of "funny business" (as the expression goes) involving attempts to bypass checks
     * made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     *
     * Regarding types: We match symlinks, and we match things avoid any potential for
     *			"funny business" (as the expression goes).
     */
    { "2[0-9][0-9][0-9]_*.tar.bz2",
	false, false, false, false, true,		/* prohibited */
	MATCH_FNMATCH_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* file glob any case, any type, at top level only */
	0, NULL },

    /*
     * Makefile ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.alt.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.alt.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * remarks.md ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "remarks.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * .auth.json ((not a file))
     *
     * This file is created by the mkiocccentry tool.
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { ".auth.json",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * .info.json ((not a file))
     *
     * This file is created by the mkiocccentry tool.
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { ".info.json",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * Makefile ((empty file))
     *
     * Regarding types: We match files.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * remarks.md ((empty file))
     *
     * Regarding types: We match files.
     */
    { "remarks.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 3RD to LAST !!!
     *
     * We match any file or directory.
     */
    { "*",
	false, false, false, false, false,		/* optional */
	MATCH_ANY, TYPE_FILEDIR, LEVEL_ANY,		/* match anything, file or directory type, at any level */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 2ND to LAST !!!
     *
     * We prohibit anything else and to prune the tree walk.
     *
     * Regarding types: Anything needs to match any type, as a number of the more type restrictive walk_rule
     *		        above will count on this walk_rule "catching" other types.
     */
    { "*",
	false, false, false, false, true,		/* prohibited */
	MATCH_ANY, TYPE_ANY, LEVEL_ANY,			/* match anything, any type, at any level */
	0, NULL },

    /*
     * NULL
     *
     * This MUST be LAST !!!
     *
     * This is a mandatory end an array of walk_rule structures.
     *
     * Regarding types: A NULL pattern means that all of the other elements of struct walk_rule,
     *			including the type, are ignored.
     */
    { NULL,
	false, false, false, false, false,		/* end of array */
	MATCH_UNSET, TYPE_UNSET, LEVEL_UNSET,		/* everything UNSET */
	0, NULL }
};


/*
 * chkentry -s walk_set
 */
struct walk_set walk_chkentry_s = {
    "chkentry -s walk_rule set",	/* walk_set name - for debugging purposes */
    false,				/* true ==> walk_set initialized and setup */
    NULL,				/* NUL terminated string describing context (tool + options) for debugging purposes */
    0,					/* length of set NOT counting the final NULL walk_rule.pattern */
    walk_rule_set_chkentry_s		/* array of walk rules, ending with a NULL walk_rule.pattern */
};


/***************/
/* chkentry -w */
/***************/


/*
 * The intent for chkentry(1) is that without -S, without -s, the -w option is implied and is default
 *
 * The primary focus for the chkentry -w walk_rule set is for winning entries managed by the IOCCC judges.
 * As such, this walk_rule set differs in a number of ways from the walk_rule sets above.
 * For example: everything that is not prohibited is free.
 *
 * The chkentry -w walk_rule set assumes that a make clobber has been performed.  The reason for this
 * more restrictive case is the help with inventory checking and inventory control for a winning entry.
 *
 * There are a number of (dot star), not mentioned in other walk_rule sets, that are prohibited in this walk_rule set.
 * These dot-files may be added to a submission by IOCCC judges that, when the submission becomes a winning entry,
 * MUST NOT be present.
 *
 * NOTE: The term submission is reserved for things submitted to the IOCCC for judging, where as
 *	 the term entry is reserved for things that have won the IOCCC.
 */


/*
 * chkentry -w walk_rule set
 *
 * Path items are compared, in order, to each walk_rule UNTIL a match is found, OR until the NULL entry is found.
 */
static struct walk_rule walk_rule_set_chkentry_w[] = {

    /*
     * (dot-dot)
     *
     * This struct walk_rule should be first !!!
     *
     * Avoid any complications with path items that are ".." (dot-dot).
     *
     * Regarding types: We match any type, not just ".." (dot-dot) directories, to avoid any potential for
     *			"funny business" (as the expression goes) involving attempts to bypass checks
     *			made by the mkiocccentry tool, and in case the tarball has been modified after formation.
     */
    { "..",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR, TYPE_ANY, LEVEL_ANY,			/* match whole string, any type, at any level */
	0, NULL },

    /*
     * (NOT file, NOR directory)
     *
     * This struct walk_rule should be second !!!
     *
     * Regarding types: We match symlinks, and we match things avoid any prohibited item file types
     *			in the web site repo.
     */
    { "*",
	false, false, false, false, true,		/* prohibited */
	MATCH_ANY, TYPE_NOT_FILEDIR, LEVEL_ANY,	/* match anything, other type, at any level */
	0, NULL },

    /*
     * COPYING*
     *
     * The IOCCC judges might, at their discretion, add such a file to a winning entry.  And while
     * this would be a rare event, and such a file would be compatible with the overall web site license,
     * we add this walk_rule that is optionally permissive.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "COPYING",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * COPYRIGHT*
     *
     * The IOCCC judges might, at their discretion, add such a file to a winning entry.  And while
     * this would be a rare event, and such a file would be compatible with the overall web site license,
     * we add this walk_rule that is optionally permissive.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "COPYRIGHT",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * LICENSE*
     *
     * The IOCCC judges might, at their discretion, add such a file to a winning entry.  And while
     * this would be a rare event, and such a file would be compatible with the overall web site license,
     * we add this walk_rule that is optionally permissive.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "LICENSE",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * Makefile
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "Makefile",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * README.md
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "README.md",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * index.html
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "index.html",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog
     *
     * The IOCCC judges MUST perform a make clobber before using this walk_set.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.alt
     *
     * The IOCCC judges MUST perform a make clobber before using this walk_set.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.alt.c
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.c",
	false, false, true, false, false,		/* optional, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.alt.o
     *
     * The IOCCC judges MUST perform a make clobber before using this walk_set.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.alt.o",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.c
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.c",
	true, false, true, false, false,		/* required, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.o
     *
     * The IOCCC judges MUST perform a make clobber before using this walk_set.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.o",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * prog.orig.c
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "prog.orig.c",
	true, false, true, false, false,		/* required, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * remarks.md
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { "remarks.md",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* match beginning any case, any type, at top level only */
	0, NULL },

    /*
     * .auth.json*
     *
     * In the process of converting a submission into a winning entry, this file MUST be removed.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { ".auth.json",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* match beginning any case, any type, at top level only */
	0, NULL },

    /*
     * .info.json*
     *
     * In the process of converting a submission into a winning entry, this file MUST be removed.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { ".info.json",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* match beginning any case, any type, at top level only */
	0, NULL },

    /*
     * .entry.json
     *
     * In the process of converting a submission into a winning entry, this file MUST be created.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".entry.json",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * .num*
     *
     * In the process of converting a submission into a winning entry, this file MUST be removed.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { ".num",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* match beginning any case, any type, at top level only */
	0, NULL },

    /*
     * .orig*
     *
     * In the process of converting a submission into a winning entry, this file MUST be removed.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { ".orig",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* match beginning any case, any type, at top level only */
	0, NULL },

    /*
     * .path
     *
     * In the process of converting a submission into a winning entry, this file MUST be created.
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".path",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * .submit*
     *
     * In the process of converting a submission into a winning entry, this file MUST be removed.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { ".submit",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* match beginning any case, any type, at top level only */
	0, NULL },

    /*
     * .txz*
     *
     * In the process of converting a submission into a winning entry, this file MUST be removed.
     *
     * Regarding types: We match any type, not just files, to avoid "funny business" (as the expression goes).
     */
    { ".txz",
	false, false, false, false, true,		/* prohibited */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* match beginning any case, any type, at top level only */
	0, NULL },

    /*
     * . ((dot))
     *
     * In case the tree walking or tarball list functions encounter a "." (dot) directory, we ignore it.
     *
     * This will mean that the ".*" (dot star) walk_rule below will not match any "." (dot) directory.
     *
     * Regarding types: We match directories only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_STR, TYPE_DIR, LEVEL_ANY,			/* match whole string, directory, at any level */
	0, NULL },

    /*
     * .* (dot star)
     *
     * The IOCCC judges may add additional (dot star) to an IOCCC winning entry when needed, so long
     * at the walk_rule that relate to other (dot star) patterns are enforced.
     *
     * NOTE: ".." (dot-dot) is processed first in this struct walk_rule array, and the directory "." (dot)
     *	     is processed above.
     *
     * Regarding types: We match .* (dot star) files, directories and symlinks only, and let "((anything))"
     *			rule below deal with any other types to avoid "funny business" (as the expression goes).
     */
    { ".",
	false, false, true, true, false,		/* optional, free, ignore */
	MATCH_PREFIX_ANYCASE, TYPE_ANY, LEVEL_ANY,	/* match beginning any case, any type, at any level */
	0, NULL },

    /*
     * 2YYY_*.tar.bz2
     *
     * Regarding types: We match files only and let "((anything))" rule below deal with any other
     *			types to avoid "funny business" (as the expression goes).
     */
    { "2[0-9][0-9][0-9]_*.tar.bz2",
	true, true, true, false, false,			/* required, non-empty, free */
	MATCH_FNMATCH_ANYCASE, TYPE_ANY, LEVEL_TOP,	/* file glob any case, any type, at top level only */
	0, NULL },

    /*
     * Makefile ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.alt.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.alt.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * prog.c ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "prog.c",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * try.sh ((not a file))
     *
     * Regarding types: We match anything that is NOT a file.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_NOT_FILE, LEVEL_TOP,	/* match whole string any case, not file type, at top level only */
	0, NULL },

    /*
     * Makefile ((empty file))
     *
     * Regarding types: We match files.
     */
    { "Makefile",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.alt.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.alt.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * try.sh ((empty file))
     *
     * Regarding types: We match files.
     */
    { "try.sh",
	false, false, false, false, true,		/* prohibited */
	MATCH_STR_ANYCASE, TYPE_FILE, LEVEL_TOP,	/* match whole string any case, file type, at top level only */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 3RD to LAST !!!
     *
     * Any file or directory is free.
     *
     * We match any file or directory.
     */
    { "*",
	false, false, true, false, false,		/* optional, free */
	MATCH_ANY, TYPE_FILEDIR, LEVEL_ANY,		/* match anything, file or directory type, at any level */
	0, NULL },

    /*
     * ((anything))
     *
     * This MUST be 2ND to LAST !!!
     *
     * In theory, nothing should reach this walk_rule, however we put this 2ND to LAST just in case (paranoia).
     *
     * We prohibit anything that is not a file, nor a directory.
     *
     * Regarding types: Anything needs to match any type, as a number of the more type restrictive walk_rule
     *		        above will count on this walk_rule "catching" other types.
     */
    { "*",
	false, false, false, false, true,		/* prohibited */
	MATCH_ANY, TYPE_NOT_FILEDIR, LEVEL_ANY,		/* match anything, any type, at any level */
	0, NULL },

    /*
     * NULL
     *
     * This MUST be LAST !!!
     *
     * This is a mandatory end an array of walk_rule structures.
     *
     * Regarding types: A NULL pattern means that all of the other elements of struct walk_rule,
     *			including the type, are ignored.
     */
    { NULL,
	false, false, false, false, false,		/* end of array */
	MATCH_UNSET, TYPE_UNSET, LEVEL_UNSET,		/* everything UNSET */
	0, NULL }
};


/*
 * chkentry -w walk_set
 */
struct walk_set walk_chkentry_w = {
    "chkentry -w walk_rule set",	/* walk_set name - for debugging purposes */
    false,				/* true ==> walk_set initialized and setup */
    NULL,				/* NUL terminated string describing context (tool + options) for debugging purposes */
    0,					/* length of set NOT counting the final NULL walk_rule.pattern */
    walk_rule_set_chkentry_w		/* array of walk rules, ending with a NULL walk_rule.pattern */
};
