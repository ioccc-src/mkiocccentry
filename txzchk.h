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

#ifndef TXZCHK_H
#define TXZCHK_H

/* 
 * some of the identifiers below share the name of identifiers in other files so
 * only define/declare the below for txzchk.c
 */
#ifdef TXZCHK_C

/*
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */



/* variable specific to txzchk */
char const *txzpath = NULL;		    /* the current tarball being checked */
char const *program = NULL;		    /* our name */
int verbosity_level = DBG_DEFAULT;	    /* debug level set by -v */
bool quiet = false;			    /* true ==> only show errors and warnings */
bool text_file_flag_used = false;	    /* true ==> assume txzpath is a text file */

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
    unsigned invalid_chars;		    /* > 0 ==> invalid characters found in one or more files */
    off_t size;				    /* size of the tarball itself */
    off_t file_sizes;			    /* total size of all the files combined */
    off_t rounded_file_size;		    /* file sizes rounded up to 1024 multiple */
    unsigned correct_directory;		    /* number of files in the correct directory */
    unsigned dot_files;			    /* number of dot files that aren't .author.json and .info.json */
    unsigned named_dot;			    /* number of files called just '.' */
    unsigned total_files;		    /* total files in the tarball */
    int total_issues;			    /* number of total issues in tarball */
} txz_info;

struct file {
    char *basename;
    char *filename;
    unsigned count;
    struct file *next;
};

struct file *files;

struct line {
    char *line;
    int line_num;
    struct line *next;
};

struct line *lines;

/*
 * txzchk version
 */
#define TXZCHK_VERSION "0.7 2022-02-12"    /* use format: major.minor YYYY-MM-DD */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msgX strings.
 */
const char * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-t tar] [-F fnamchk] [-T] txzpath\n"
    "\n"
    "\t-h\t\t\tprint help message and exit 0\n"
    "\t-v level\t\tset verbosity level: (def level: %d)\n"
    "\t-V\t\t\tprint version string and exit\n"
    "\n"
    "\t-t /path/to/tar\t\tpath to tar executable that supports the -J (xz) option (def: %s)\n"
    "\t-F /path/to/fnamchk\tpath to tool that checks if txzpath is a valid compressed tarball name\n"
    "\t\t\t\tfilename (def: %s)\n\n"
    "\t-T\t\t\tassume txzpath is a text file with tar listing (for testing different formats)\n\n"
    "\ttxzpath\t\t\tpath to an IOCCC compressed tarball\n"
    "\n"
    "txzchk version: %s\n";



/*
 * function prototypes
 */
void usage(int exitcode, char const *name, char const *str, char const *tar, char const *fnamchk) __attribute__((noreturn));
void sanity_chk(char const *tar, char const *fnamchk);
void parse_line(char *linep, char *line_dup, char const *dir_name, char const *txzpath, int *dir_count);
void parse_linux_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr);
void parse_bsd_line(char *p, char *line, char *line_dup, char const *dir_name, char const *txzpath, char **saveptr);
unsigned check_tarball(char const *tar, char const *fnamchk);
void show_txz_info(char const *txzpath);
void check_empty_file(char const *txzpath, off_t size, struct file *file);
void check_file(char const *txzpath, char *p, char const *dir_name, struct file *file);
void check_all_files(char const *dir_name);
void check_directories(struct file *file, char const *dir_name, char const *txzpath);
bool has_special_bits(char const *str);
void add_line(char const *str, int line_num);
void parse_all_lines(char const *dir_name, char const *txzpath);
void free_lines(void);
struct file *alloc_file(char const *p);
void add_file_to_list(struct file *file);
void free_file_list(void);


#endif /* TXZCHK_C */
#endif /* TXZCHK_H */
