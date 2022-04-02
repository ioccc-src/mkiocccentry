/*
 * txzchk: the IOCCC tarball validation checker
 *
 * Invoked by mkiocccentry; txzchk in turn uses fnamchk to make sure that
 * the tarball was correctly named and formed (i.e. the mkiocccentry tool was
 * used).
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * Written in 2022 by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 */


#if !defined(INCLUDE_TXZCHK_H)
#    define  INCLUDE_TXZCHK_H


/*
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * sanity
 */
#include "sanity.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/* variable specific to txzchk */
bool quiet = false;				/* true ==> quiet mode */
/**/
static char const *txzpath = NULL;		/* the current tarball being checked */
static char const *program = NULL;		/* our name */
static bool text_file_flag_used = false;	/* true ==> assume txzpath is a text file */

/*
 * information about the tarball
 */
struct txz_info {
    bool has_info_json;			    /* true ==> has a .info.json file */
    bool empty_info_json;		    /* true ==> .info.json size == 0 */
    bool has_author_json;		    /* true ==> has an .author.json file */
    bool empty_author_json;		    /* true ==> .author.json size == 0 */
    bool has_prog_c;			    /* true ==> has a prog.c file */
    bool empty_prog_c;			    /* true ==> prog.c size == 0 (this is for debugging information only) */
    bool has_remarks_md;		    /* true ==> has a remarks.md file */
    bool empty_remarks_md;		    /* true ==> remarks.md size == 0 */
    bool has_Makefile;			    /* true ==> has a Makefile */
    bool empty_Makefile;		    /* true ==> Makefile size == 0 */
    unsigned invalid_chars;		    /* > 0 ==> invalid characters found in this number of filenames */
    off_t size;				    /* size of the tarball itself */
    off_t file_sizes;			    /* total size of all the files combined */
    off_t rounded_file_size;		    /* file sizes rounded up to 1024 multiple */
    unsigned correct_directory;		    /* number of files in the correct directory */
    unsigned dot_files;			    /* number of dot files that aren't .author.json and .info.json */
    unsigned named_dot;			    /* number of files called just '.' */
    unsigned total_files;		    /* total files in the tarball */
    int total_issues;			    /* number of total issues in tarball */
};

static struct txz_info txz_info;

struct txz_file {
    char *basename;
    char *filename;
    unsigned count;
    struct txz_file *next;
};

static struct txz_file *txz_files;

struct txz_line {
    char *line;
    int line_num;
    struct txz_line *next;
};

static struct txz_line *txz_lines;


/*
 * usage message
 *
 * Use the usage() function to print the usage_msgX strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-q] [-V] [-t tar] [-F fnamchk] [-T] txzpath\n"
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
    "\t-T\t\tassume txzpath is a text file with tar listing (for testing different formats)\n\n"
    "\ttxzpath\t\tpath to an IOCCC compressed tarball\n"
    "\n"
    "txzchk version: %s\n";



/*
 * function prototypes
 */
static void usage(int exitcode, char const *name, char const *str) __attribute__((noreturn));
static void txzchk_sanity_chks(char const *tar, char const *fnamchk);
static void parse_txz_line(char *linep, char *line_dup, char const *dir_name, char const *txzpath, int *dir_count);
static void parse_linux_txz_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr);
static void parse_bsd_txz_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr);
static unsigned check_tarball(char const *tar, char const *fnamchk);
static void show_txz_info(char const *txzpath);
static void check_empty_file(char const *txzpath, off_t size, struct txz_file *file);
static void check_txz_file(char const *txzpath, char *p, char const *dir_name, struct txz_file *file);
static void check_all_txz_files(char const *dir_name);
static void check_directories(struct txz_file *file, char const *dir_name, char const *txzpath);
static bool has_special_bits(char const *str);
static void add_txz_line(char const *str, int line_num);
static void parse_all_txz_lines(char const *dir_name, char const *txzpath);
static void free_txz_lines(void);
static struct txz_file *alloc_txz_file(char const *p);
static void add_txz_file_to_list(struct txz_file *file);
static void free_txz_files_list(void);


#endif /* INCLUDE_TXZCHK_H */
