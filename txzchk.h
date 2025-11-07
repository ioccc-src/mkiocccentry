/*
 * txzchk - IOCCC submission tarball validation tool
 *
 * txzchk verifies that IOCCC submission tarballs conform to the IOCCC rules (no
 * feathers stuck in the tarballs :-) ).
 *
 * txzchk is invoked by mkiocccentry; txzchk in turn uses fnamchk to make
 * sure that the tarball was correctly named and formed. In other words txzchk
 * makes sure that the mkiocccentry tool was used and there was no screwing
 * around with the resultant tarball.
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson.  All Rights Reserved.
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
 * CODY BOONE FERGUSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT
 * SHALL CODY BOONE FERGUSON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 *
 * This tool was written in 2022-2025 by Cody Boone Ferguson:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Dedicated to:
 *
 *	The many poor souls who have been tarred and feathered:
 *
 *	    "Because sometimes people throw feathers on tar." :-(
 *
 *	And to my wonderful Mum and my dear cousin and friend Dani:
 *
 *	    "Because even feathery balls of tar need some love." :-)
 *
 * Disclaimer:
 *
 *	No pitman or coal mines were harmed in the making of this tool and
 *	neither were any pine trees or birch trees. Whether the fact no coal
 *	mines were harmed is a good or bad thing might be debatable but
 *	nevertheless none were harmed. :-) More importantly, no tar pits -
 *	including the La Brea Tar Pits - were disturbed in the making of this
 *	tool. :-)
 *
 * Share and enjoy! :-)
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_TXZCHK_H)
#    define  INCLUDE_TXZCHK_H

/*
 * jparse/version - JSON parser API and tool version
 */
#include "jparse/version.h"

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg/dbg.h"

/*
 * canon_path - canonicalize paths
 */
#include "cpath/cpath.h"

/*
 * sanity - perform common IOCCC sanity checks
 */
#include "soup/sanity.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "soup/limit_ioccc.h"

/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "soup/entry_util.h"


/*
 * soup/util - certain utils we need
 */
#include "soup/util.h"

/*
 * soup/walk - for walking through the tar pit
 */
#include "soup/walk.h"


/*
 * macros
 */

/*
 * txzchk tool basename
 */
#define TXZCHK_BASENAME "txzchk"

/*
 * utility macros
 *
 * These will work for our purposes but the singular or plural one is in truth
 * much more complicated than what we're making it seem like.
 */
#define SINGULAR_OR_PLURAL(x) ((x)==1?"":"s")


/*
 * information about the tarball
 */
struct tarball
{
    off_t size;				    /* size of the tarball itself */
    off_t total_size;			    /* total size of all the files combined */
    uintmax_t total_feathers;		    /* number of total feathers stuck in tarball (i.e. issues found) */
    off_t previous_files_size;		    /* the previous total size of all files combined */
    uintmax_t invalid_files_count;	    /* > 0 ==> number of times file count went <= 0 */
    uintmax_t negative_files_size;	    /* > 0 ==> number of times the total files reached < 0 */
    uintmax_t files_size_too_big;	    /* > 0 ==> total number of times files size sum > MAX_SUM_FILELEN */
    uintmax_t files_size_shrunk;	    /* > 0 ==> total files size shrunk this many times */
    uintmax_t invalid_perms;                /* total number of files with invalid permissions */
    uintmax_t total_exec_files;             /* total number of executable FILES */
};


/*
 * txz_file - struct for each file
 *
 * This struct is for the txz_files linked list which is all the files the
 * tarball has (according to tar -tJvf or if -T specified what was read from
 * each line of the text file).
 *
 * Once the tar/text file output has been parsed we go through this list to
 * determine if the appropriate files exist, to check that only one of each file
 * exists, the correct directory is found (and no directory changes are there)
 * and check other conditions. See the function check_all_txz_files().
 */
struct txz_file
{
    char *basename;			    /* basename of _this_ file */
    char *filename;			    /* full path of _this_ file */
    char *dirname;                          /* dirname (to compare against fnamchk) */
    char *top_dirname;                      /* top directory name of _this_ file (i.e. up to first '/') */
    uintmax_t count;			    /* number of times _this_ file has been seen */
    bool isfile;			    /* true ==> is normal file (count size and number of files) */
    intmax_t length;			    /* size as determined by string_to_intmax2() */
    bool isdir;                             /* true ==> is a directory */
    char *perms;                            /* permission bits */
    bool isexec;                            /* true ==> executable (+x) file */
    mode_t mode;                            /* perms -> mode_t */
    struct txz_file *next;		    /* the next file in the txz_files list */
};





/*
 * struct txz_line - a line of output from tar -tJvf or the text file
 *
 * This struct defines a line of output for the txz_lines linked list. After all
 * lines from tar or the text file are added to the list, we parse each line (by
 * calling parse_txz_line() which calls either parse_linux_txz_line() or
 * parse_bsd_txz_line() depending on the line format), doing various checks and
 * adds each file to the txz_files list for further checks after the initial
 * tests are complete.
 */
struct txz_line
{
    char *line;				/* copy of the line */
    uintmax_t line_num;			/* line number */
    struct txz_line *next;		/* pointer to the next line or NULL if last line */
};

/*
 * function prototypes
 */
static void txzchk_sanity_chks(char const *tar, char const *fnamchk);
static uintmax_t check_tarball(char const *tar, char const *fnamchk);
static void parse_txz_line(char *linep, char *line_dup, char const *dirname, char const *tarball_path, intmax_t *sum,
        intmax_t *count);
static void parse_linux_txz_line(char *p, char *line, char *line_dup, char const *dirname,
	char const *tarball_path, char **saveptr, bool normal_file, intmax_t *sum, intmax_t *count, bool isdir,
        char *perms, bool isexec);
static void show_tarball_info(char const *tarball_path);
static void check_all_txz_files(void);
static mode_t get_mode(struct txz_file *file);
static void add_txz_line(char const *str, uintmax_t line_num);
static void parse_all_txz_lines(char const *dirname, char const *tarball_path);
static void free_txz_lines(void);
static void check_txz_file(char const *tarball_path, char const *dirname, struct txz_file *file);
static void check_directory(struct txz_file *file, char const *dirname, char const *tarball_path);
static struct txz_file *alloc_txz_file(char const *path, char const *dirname, char *perms, bool isdir,
        bool isfile, bool isexec, intmax_t length);
static void count_and_sum(char const *tarball_path, intmax_t *sum, intmax_t *count, intmax_t length);
static bool has_special_bits(struct txz_file *file);
static void add_txz_file_to_list(struct txz_file *file);
static void free_txz_file(struct txz_file **file);
static void free_txz_files_list(void);


#endif /* INCLUDE_TXZCHK_H */
