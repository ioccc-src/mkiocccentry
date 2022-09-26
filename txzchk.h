/*
 * txzchk - IOCCC tarball validation tool
 *
 * txzchk verifies that the tarball does not have any feathers stuck in it (i.e.
 * the tarball conforms to the IOCCC tarball rules). Invoked by mkiocccentry;
 * txzchk in turn uses fnamchk to make sure that the tarball was correctly named
 * and formed. In other words txzchk makes sure that the mkiocccentry tool was
 * used and there was no screwing around with the resultant tarball.
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
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */



#if !defined(INCLUDE_TXZCHK_H)
#    define  INCLUDE_TXZCHK_H


/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * sanity - perform common IOCCC sanity checks
 */
#include "sanity.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "entry_util.h"


/*
 * macros
 */
#define has_does_not_have(b) ((b)?"has":"does not have")
#define singular_or_plural(x) ((x)==1?"":"s")

/* globals */
bool quiet = false;				/* true ==> quiet mode */
/* globals specific to txzchk */
static char const *txzpath = NULL;		/* the current tarball being checked */
static char const *program = NULL;		/* our name */
static bool text_file_flag_used = false;	/* true ==> assume txzpath is a text file */
static char const *ext = "txz";			/* force extension in fnamchk to be this value */
static bool suppress_error_messages = false;	/* true ==> suppress error messages (-e used for tests but should be changed) */
static char const *tok_sep = " \t";		/* token separators for strtok_r */

/*
 * information about the tarball
 */
struct txz_info
{
    bool has_info_json;			    /* true ==> has a .info.json file */
    bool empty_info_json;		    /* true ==> .info.json size == 0 */
    off_t info_json_size;		    /* .info.json file size */
    bool has_author_json;		    /* true ==> has an .author.json file */
    off_t author_json_size;		    /* .author.json file size */
    bool empty_author_json;		    /* true ==> .author.json size == 0 */
    bool has_prog_c;			    /* true ==> has a prog.c file */
    bool empty_prog_c;			    /* true ==> prog.c size == 0 (this is for debugging information only) */
    off_t prog_c_size;			    /* prog.c file size */
    bool has_remarks_md;		    /* true ==> has a remarks.md file */
    bool empty_remarks_md;		    /* true ==> remarks.md size == 0 */
    off_t remarks_md_size;		    /* remarks.md file size */
    bool has_Makefile;			    /* true ==> has a Makefile */
    bool empty_Makefile;		    /* true ==> Makefile size == 0 */
    off_t Makefile_size;		    /* Makefile file size */
    uintmax_t unsafe_chars;		    /* > 0 ==> unsafe characters found in this number of filenames (posix_safe_plus()) */
    off_t size;				    /* size of the tarball itself */
    off_t files_size;			    /* total size of all the files combined */
    off_t previous_files_size;		    /* the previous total size of all files combined */
    uintmax_t invalid_files_count;	    /* > 0 ==> number of times file count went <= 0 */
    uintmax_t negative_files_size;	    /* > 0 ==> number of times the total files reached < 0 */
    uintmax_t files_size_too_big;	    /* > 0 ==> total number of times files size sum > MAX_DIR_KSIZE */
    uintmax_t files_size_shrunk;	    /* > 0 ==> total files size shrunk this many times */
    uintmax_t correct_directory;	    /* number of files in the correct directory */
    uintmax_t invalid_dot_files;	    /* number of dot files that aren't .author.json and .info.json */
    uintmax_t named_dot;		    /* number of files called just '.' */
    uintmax_t total_files;		    /* total files in the tarball */
    uintmax_t abnormal_files;		    /* total number of abnormal files in tarball (i.e. not regular files) */
    uintmax_t total_feathers;		    /* number of total feathers stuck in tarball (i.e. issues found) */
};

static struct txz_info txz_info;	    /* all the information collected from txzpath */

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
    uintmax_t count;			    /* number of times _this_ file has been seen */
    bool is_file;			    /* true ==> is normal file (count size and number of files) */
    intmax_t length;			    /* size as determined by string_to_intmax2() */
    struct txz_file *next;		    /* the next file in the txz_files list */
};

static struct txz_file *txz_files;	    /* linked list of the files in the tarball */

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
    int line_num;			/* line number */
    struct txz_line *next;		/* pointer to the next line or NULL if last line */
};

static struct txz_line *txz_lines;	/* all the lines read */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-t tar] [-F fnamchk] [-T] [-E ext] [-e] txzpath\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level: (def level: %d)\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-V\t\tprint version string and exit\n"
    "\n"
    "\t-t tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-F fnamchk\tpath to tool that checks if txzpath is a valid compressed tarball name\n"
    "\t\t\tfilename (def: %s)\n\n"
    "\t-T\t\tassume txzpath is a text file with tar listing (for testing different formats)\n"
    "\t-E ext\t\tchange extension to test (def: txz)\n"
    "\t-e\t\tsuppress error messages\n\n"
    "\ttxzpath\t\tpath to an IOCCC compressed tarball\n"
    "\n"
    "txzchk version: %s\n";


/*
 * function prototypes
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void txzchk_sanity_chks(char const *tar, char const *fnamchk);
static void parse_txz_line(char *linep, char *line_dup, char const *dir_name, char const *txzpath, int *dir_count,
			   intmax_t *sum, intmax_t *count);
static void parse_linux_txz_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr,
		    bool normal_file, intmax_t *sum, intmax_t *count);
static void parse_bsd_txz_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr,
		    bool normal_file, intmax_t *sum, intmax_t *count);
static uintmax_t check_tarball(char const *tar, char const *fnamchk);
static void show_txz_info(char const *txzpath);
static void check_file_size(char const *txzpath, off_t size, struct txz_file *file);
static void count_and_sum(char const *txzpath, intmax_t *sum, intmax_t *count, intmax_t length);
static void check_txz_file(char const *txzpath, char const *dir_name, struct txz_file *file);
static void check_all_txz_files(char const *dir_name);
static void check_directories(struct txz_file *file, char const *dir_name, char const *txzpath);
static bool has_special_bits(char const *str);
static void add_txz_line(char const *str, int line_num);
static void parse_all_txz_lines(char const *dir_name, char const *txzpath);
static void free_txz_lines(void);
static struct txz_file *alloc_txz_file(char const *path, intmax_t length);
static void add_txz_file_to_list(struct txz_file *file);
static void free_txz_files_list(void);


#endif /* INCLUDE_TXZCHK_H */
