/*
 * try_walk_set - try using one of the static walk_rule sets
 *
 * "This is a serious journey, not a hobbit walking-party."
 *
 *	-- Gandalf to Pippin, the Fellowship of the Ring.
 *
 * Copyright (c) 2025 by Landon Curt Noll.  All Rights Reserved.
 * Minor portability improvements were made by Cody Boone Ferguson.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
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
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <unistd.h>
#include <libgen.h>

/*
 * location - location/country codes and set the IOCCC locale
 */
#include "../soup/location.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "../soup/limit_ioccc.h"

/*
 * walk - walk directory trees and tar listings
 */
#include "../soup/walk.h"


/*
 * definitions
 */
#define TRY_WALK_SET_BASENAME "try_walk_set"
/**/
#define TOPDIR "."		/* default topdir */


/*
 * walk_set numbers
 */
enum walk_set_num {
    WALK_SET_UNSET = -1,	/* not valid set, or walk_set number unset */
    WALK_SET_ALL = 0,		/* try all sets */
    WALK_SET_MKIOCCCENTRY = 1,	/* mkiocccentry walk_set */
    WALK_SET_TXZCHK = 2,	/* txzchk walk_set */
    WALK_SET_CHKSUBMIT = 3,	/* chksubmit walk_set */
    WALK_SET_CHKENTRY_CAPS = 4,	/* chkentry -S walk_set */
    WALK_SET_CHKENTRY_S = 5,	/* chkentry -s walk_set */
    WALK_SET_CHKENTRY_W = 6,	/* chkentry -w walk_set */
    WALK_SET_ANYTHING = 7,	/* anything goes walk_set */
};
#define WALK_SET_DEFAULT (1)	/* default walk set number */

/*
 * type of path to simulate
 */
enum path_type {
    PATH_TYPE_UNSET = '?',	/* not valid type, or path type not set */
    PATH_TYPE_FILE = 'f',	/* simulate regular file */
    PATH_TYPE_DIR = 'd',	/* simulate directory */
    PATH_TYPE_SYMLINK = 's',	/* simulate symlink */
    PATH_TYPE_OTHER = 'o',	/* simulate other type (will simulate named pipe (fifo) */
};

/*
 * parsed_data - type, size, and path
 */
struct parsed_data {
    mode_t st_mode;		/* simulated inode protection mode in struct stat st_mode form */
    off_t st_size;		/* simulated size of item, in bytes */
    char const *path;		/* original part arg */
};


/*
 * globals
 */
static char *program = "try_walk_set";		    /* our name */
static bool sort_cpath = true;			    /* true ==> dynamic arrays by canonicalized path, false ==> do not sort */

/*
 * usage message
 */
static char const * const usage_msg =
    "usage: %s [-h] [-v level] [-V] [-q] [-T] [-o] [-t topdir]\n"
    "\t\t\t[-m max_path] [-M max_depth] [-d max_depth]\n"
    "\t\t\t[-F max_file] [-D max_dir] [-S max_sym] [-O max_other] [-s set] [type size path] ...\n"
    "\n"
    "\t-h\t\tPrint help message and exit\n"
    "\t-v level\tSet verbosity level: (def level: 0)\n"
    "\t-V\t\tPrint version string and exit\n"
    "\t-q\t\tQuiet mode: silence msg(), warn(), warnp() if -v 0 (def: loud :-) )\n"
    "\n"
    "\t-o\t\tkeep paths in original order (def: sort dynamic arrays by canonicalized path)\n"
    "\t-T\t\tprocess as a tarball listing\n"
    "\t-t topdir\tset topdir (def: %s)\n"
    "\n"
    "\t-m max_path\tmax canonicalized path length, 0 ==> no limit (def: %d)\n"
    "\t-M max_file\tmax length of any canonicalized path component, 0 ==> no limit (def: %d)\n"
    "\t-d max_depth\tmax canonicalized path depth where 0 is the topdir, 0 ==> no limit (def: %d)\n"
    "\n"
    "\t-F max_file\tmax counted (non-free) files, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\t-D max_dir\tmax counted (non-free) directories, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\t-S max_sym\tmax counted (non-free) symlinks, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\t-O max_other\tmax counted (non-free) non-file/dir/symlinks, %d ==> no limit, %d ==> none allowed (def: %d)\n"
    "\n"
    "\t-s set\t\twalk_set number to try (def: %d)\n"
    "\t\t\t    l - list sets and exit\n"
    "\t\t\t    0 - try all sets\n"
    "\t\t\t    1 - mkiocccentry walk_set\n"
    "\t\t\t    2 - txzchk walk_set\n"
    "\t\t\t    3 - chksubmit walk_set\n"
    "\t\t\t    4 - chkentry -S walk_set\n"
    "\t\t\t    5 - chkentry -s walk_set\n"
    "\t\t\t    6 - chkentry -w walk_set\n"
    "\t\t\t    7 - anything goes\n"
    "\n"
    "\ttype\t\ttype of path to simulate\n"
    "\t\t\t    f - regular file\n"
    "\t\t\t    d - directory\n"
    "\t\t\t    s - symlink\n"
    "\t\t\t    o - other type (will simulate named pipe (fifo))\n"
    "\n"
    "\tsize\t\tsimulate the size in bytes\n"
    "\n"
    "\tpath\t\tpath from topdir to simulate\n"
    "\n"
    "\tNOTE: If no 'type size path' sets are given on the command line, read sets of 3, 1 set line, from stdin.\n"
    "\n"
    "Exit codes:\n"
    "     0   all is OK\n"
    "     1   some walk erorrs detected\n"
    "     2   -h and help string printed or -V and version string printed\n"
    "     3   invalid command line, invalid option or option missing an argument\n"
    "     4   invalid set number\n"
    "     5   invalid type letter\n"
    "     6   invalid size\n"
    "     7   canonicalization of path failed\n"
    " >= 10   internal error\n"
    "\n"
    "%s version: %s\n";

/*
 * static functions
 */
static void usage(int exitcode, char const *str) __attribute__((noreturn));
static mode_t parse_type_str(char const *type_str);
static off_t parse_size_str(char const *size_str);
static void parse_3_args(struct parsed_data *data_p, char const *type_str, char const *size_str, char const *path);
static void free_parsed_data(struct parsed_data *data_p);
static bool parse_stream(FILE *stream, char **line_p, char **type_str_p, char **size_str_p, char **path_p);
static bool test_walk(int argc, char **argv, FILE *in, FILE *out,
		      char const *topdir, struct walk_set *set, char const *context,
		      size_t max_path_len, size_t max_filename_len, int_least32_t max_depth,
		      int_least32_t max_file, int_least32_t max_dir, int_least32_t max_sym, int_least32_t max_other,
		      bool tar_listing, bool sort_cpath);


int
main(int argc, char *argv[])
{
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    /**/
    char *topdir = TOPDIR;			    /* simulated topdir */
    bool tar_listing = false;			    /* true ==> process tarball listing, false ==> path processing */
    /**/
    int_least32_t max_file = MAX_EXTRA_FILE_COUNT;  /* max counted files, 0 ==> unlimited, -1 ==> none */
    int_least32_t max_dir = MAX_EXTRA_DIR_COUNT;    /* max counted directories, 0 ==> unlimited, -1 ==> none */
    int_least32_t max_sym = ANY_COUNT;            /* max counted symlinks, 0 ==> unlimited, -1 ==> none */
    int_least32_t max_other = NO_COUNT;           /* max counted non-file/dir/symlinks, 0 ==> unlimited, -1 ==> none */
    /**/
    size_t max_path_len = MAX_PATH_LEN;		    /* max canonicalized path length, 0 ==> no limit */
    size_t max_filename_len = MAX_FILENAME_LEN;	    /* max length of each component of path, 0 ==> no limit */
    int_least32_t max_depth = MAX_PATH_DEPTH;	    /* max canonicalized path depth where 0 is the topdir, 0 ==> no limit */
    /**/
    bool opt_error = false;		/* fchk_inval_opt() return */
    int set_num = WALK_SET_DEFAULT;	/* set arg as a number */
    bool walk_ok = true;		/* true ==> no errors found, false ==> some errors found */
    int exit_code = 0;			/* exit with this code */
    int i;

    /* IOCCC requires use of C locale */
    set_ioccc_locale();

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:VqoTt:m:M:d:s:F:D:S:O:")) != -1) {
	switch (i) {
	case 'h':	/* -h - print help to stderr and exit 0 */
	    usage(2, NULL);
	    not_reached();
	    break;
	case 'v':	/* -v verbosity */
	    /* parse verbosity */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':	/* -V - print version and exit */
            print("%s version: %s\n", TRY_WALK_SET_BASENAME, TRY_WALK_SET_VERSION);
            exit(2); /*ooo*/
            not_reached();
            break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 't':
	    topdir = optarg;
	    break;
	case 'o':
	    sort_cpath = false;
	    break;
	case 'T':
	    tar_listing = true;
	    break;
	case 'm':
	    errno = 0;
	    max_path_len = (size_t) strtoumax(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -m %s into a size_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'M':
	    errno = 0;
	    max_filename_len = (size_t) strtoumax(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -M %s into a size_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'd':
	    errno = 0;
	    max_depth = (int_least32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -d %s into an int_least32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 's':
	    if (strcmp(optarg, "l") == 0) {
		(void) printf("Allowed Walk Sets\n"
			      "\n"
			      "0 - try all sets\n"
			      "1 - mkiocccentry walk_set\n"
			      "2 - txzchk walk_set\n"
			      "3 - chksubmit walk_set\n"
			      "4 - chkentry -S walk_set\n"
			      "5 - chkentry -s walk_set\n"
			      "6 - chkentry -w walk_set\n"
			      "7 - anything goes walk_set\n");
		exit(0); /*ooo*/
		break;
	    }
	    errno = 0;
	    set_num = (int) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, __func__, "cannot convert set: %s into a set number", optarg);
		not_reached();
	    }
	    break;
	case 'F':
	    errno = 0;
	    max_file = (int_least32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -F %s into an int_least32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'D':
	    errno = 0;
	    max_dir = (int_least32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -S %s into an int_least32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'S':
	    errno = 0;
	    max_sym = (int_least32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -S %s into an int_least32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case 'O':
	    errno = 0;
	    max_depth = (int_least32_t) strtol(optarg, NULL, 0);
	    if (errno != 0) {
		errp(3, program, "unable to convert -O %s into an int_least32_t", optarg); /*ooo*/
		not_reached();
	    }
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, "invalid command line option"); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, __func__, "getopt() return: %c optopt: %c", (char)i, (char)optopt);
	    }
	    break;
	}
    }
    /* advance command line beyond options */
    argc -= optind;
    argv += optind;
    /* must have 3 args */
    if (argc % 3 != 0) {
	usage(3, "number of arguments must be a multiple of 3");	/*ooo*/
	not_reached();
    }
    /* parse set_num */
    switch (set_num) {
    case WALK_SET_ALL:
	dbg(DBG_MED, "set number WALK_SET_ALL: %d", set_num);
	break;
    case WALK_SET_MKIOCCCENTRY:
	dbg(DBG_MED, "set number WALK_SET_MKIOCCCENTRY: %d", set_num);
	break;
    case WALK_SET_TXZCHK:
	dbg(DBG_MED, "set number WALK_SET_TXZCHK: %d", set_num);
	break;
    case WALK_SET_CHKSUBMIT:
	dbg(DBG_MED, "set number WALK_SET_CHKSUBMIT: %d", set_num);
	break;
    case WALK_SET_CHKENTRY_CAPS:
	dbg(DBG_MED, "set number WALK_SET_CHKENTRY_CAPS: %d", set_num);
	break;
    case WALK_SET_CHKENTRY_S:
	dbg(DBG_MED, "set number WALK_SET_CHKENTRY_S: %d", set_num);
	break;
    case WALK_SET_CHKENTRY_W:
	dbg(DBG_MED, "set number WALK_SET_CHKENTRY_W: %d", set_num);
	break;
    case WALK_SET_ANYTHING:
	dbg(DBG_MED, "set number WALK_SET_ANYTHING: %d", set_num);
	break;
    default:
	err(4,  __func__, "unknown set number: %d", set_num); /*ooo*/
	not_reached();
    }

    /*
     * mkiocccentry walk_set WALK_SET_MKIOCCCENTRY or WALK_SET_ALL
     */
    if (set_num == WALK_SET_MKIOCCCENTRY || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_MKIOCCCENTRY */
	walk_ok = test_walk(argc, argv, stdin, stdout,
			    topdir, &walk_mkiocccentry, "mkiocccentry",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    tar_listing, sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * txzchk walk_set WALK_SET_TXZCHK or WALK_SET_ALL
     */
    if (set_num == WALK_SET_TXZCHK || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_TXZCHK */
	walk_ok = test_walk(argc, argv, stdin, stdout,
			    topdir, &walk_txzchk, "txzchk",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    tar_listing, sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * chkentry -S walk_set WALK_SET_CHKSUBMIT or WALK_SET_CHKENTRY_CAPS or WALK_SET_ALL
     */
    if (set_num == WALK_SET_CHKSUBMIT || set_num == WALK_SET_CHKENTRY_CAPS || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_CHKENTRY_CAPS */
	walk_ok = test_walk(argc, argv, stdin, stdout,
			    topdir, &walk_chkentry_S, "chkentry -S",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    tar_listing, sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * chkentry -s walk_set WALK_SET_CHKENTRY_S or WALK_SET_ALL
     */
    if (set_num == WALK_SET_CHKENTRY_S || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_CHKENTRY_S */
	walk_ok = test_walk(argc, argv, stdin, stdout,
			    topdir, &walk_chkentry_s, "chkentry -s",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    tar_listing, sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * chkentry -w walk_set (WALK_SET_CHKENTRY_W) or WALK_SET_ALL
     */
    if (set_num == WALK_SET_CHKENTRY_W || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_CHKENTRY_S */
	walk_ok = test_walk(argc, argv, stdin, stdout,
			    topdir, &walk_chkentry_w, "chkentry -w",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    tar_listing, sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * anything goes walk_set (WALK_SET_ANYTHING) or WALK_SET_ALL
     */
    if (set_num == WALK_SET_ANYTHING || set_num == WALK_SET_ALL) {

	/* test walk_set WALK_SET_ANYTHING */
	walk_ok = test_walk(argc, argv, stdin, stdout,
			    topdir, &walk_anything, "anything goes",
			    max_path_len, max_filename_len, max_depth,
			    max_file, max_dir, max_sym, max_other,
			    tar_listing, sort_cpath);
	exit_code = (walk_ok == false) ? 1 : exit_code;
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    exit(exit_code); /*ooo*/
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(2, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message if str is non-NULL and not an empty string
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *str)
{
    /*
     * print the formatted usage stream
     */
    if (str != NULL && str[0] != '\0') {
	(void) fprintf(stderr, "%s: %s\n", TRY_WALK_SET_BASENAME, str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, TRY_WALK_SET_BASENAME, TOPDIR,
			    MAX_PATH_LEN, MAX_FILENAME_LEN, MAX_PATH_DEPTH,
			    ANY_COUNT, NO_COUNT, MAX_EXTRA_FILE_COUNT,
			    ANY_COUNT, NO_COUNT, MAX_EXTRA_DIR_COUNT,
			    ANY_COUNT, NO_COUNT, ANY_COUNT,
			    ANY_COUNT, NO_COUNT, NO_COUNT,
			    WALK_SET_DEFAULT, TRY_WALK_SET_BASENAME, TRY_WALK_SET_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * parse_type_str - parse the file type string
 *
 * given:
 *	type_str    - type arg as a single character string
 *
 * returns:
 *	st_mode related to the type arg as a single character string
 *
 * NOTE: This function does not return if type_str is NULL or valid file type string.
 */
static mode_t
parse_type_str(char const *type_str)
{
    mode_t st_mode = 0;				/* inode protection mode in struct stat st_mode form */

    /*
     * firewall
     */
    if (type_str == NULL) {
	err(5,  __func__, "type_str is NULL"); /*ooo*/
	not_reached();
    }
    dbg(DBG_V1_HIGH, "%s: type: %s", __func__, type_str);
    if (strlen(type_str) != 1) {
	err(5,  __func__, "type must be a single letter (one of [fdso])"); /*ooo*/
	not_reached();
    }

    /*
     * parse type_str
     */
    switch (type_str[0]) {
    case PATH_TYPE_FILE:
	st_mode = S_IFREG | S_IRUSR | S_IRGRP | S_IROTH;    /* file mode 0444 */
	break;
    case PATH_TYPE_DIR:
	st_mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;	/* dir mode 0755 */
	break;
    case PATH_TYPE_SYMLINK:
	st_mode = S_IFLNK | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;	/* symlink mode 0755 */
	break;
    case PATH_TYPE_OTHER:
	st_mode = S_IFIFO | S_IRUSR | S_IRGRP | S_IROTH;    /* named pipe (fifo) mode 0444 */
	break;
    default:
	err(5,  __func__, "unknown path type"); /*ooo*/
        not_reached();
    }

    /*
     * return the st_mode related to the type arg as a single character string
     */
    return st_mode;
}


/*
 * parse_size_str - parse the item size string
 *
 * given:
 *	size_str    - item size as a string
 *
 * returns:
 *	st_mode related to the type arg as a single character string
 *
 * NOTE: This function does not return if size_str is NULL or valid integer.
 */
static off_t
parse_size_str(char const *size_str)
{
    off_t st_size = 0;			/* size, in bytes */

    /*
     * firewall
     */
    if (size_str == NULL) {
	err(6,  __func__, "size_str is NULL"); /*ooo*/
	not_reached();
    }
    dbg(DBG_V1_HIGH, "%s: size_str: %s", __func__, size_str);

    /*
     * parse size_str
     */
    errno = 0;
    st_size = (off_t) strtoul(size_str, NULL, 0);
    if (errno != 0) {
	errp(6, __func__, "cannot convert size_str: %s into a size", size_str); /*ooo*/
	not_reached();
    }

    /*
     * return item size
     */
    return st_size;
}


/*
 * parse_3_args - parse type size path
 *
 * given:
 *	data_p	    - pointer to parsed_data to modify
 *	type_str    - type arg as a single character string
 *	size_str    - item size as a string
 *	path	    - path arg as a string (pre-canonicalized)
 *
 * NOTE: This function does not return on error.
 */
static void
parse_3_args(struct parsed_data *data_p, char const *type_str, char const *size_str, char const *path)
{
    /*
     * firewall
     *
     * NOTE: The parse_type_str() call will firewall the type_str arg
     * NOTE: The parse_size_str() call will firewall the size_str
     */
    if (size_str == NULL) {
	err(10,  __func__, "data_p is NULL"); /*coo*/
	not_reached();
    }
    if (path == NULL) {
	err(11,  __func__, "path is NULL");
	not_reached();
    }
    memset(data_p, 0, sizeof(struct parsed_data)); /* paranoia */
    data_p->path = path;
    dbg(DBG_HIGH, "%s: file: %s", __func__, data_p->path);

    /*
     * validate type_str arg and then parse type_str
     */
    data_p->st_mode = parse_type_str(type_str);
    dbg(DBG_HIGH, "%s: simulate type file: %c mode: %07o (%s)",
		 __func__, (char)type_str[0], data_p->st_mode, file_type_name(data_p->st_mode));

    /*
     * validate size_str arg and then parse size_str
     */
    data_p->st_size = parse_size_str(size_str);
    dbg(DBG_HIGH, "%s: size: %lld", __func__, (long long) data_p->st_size);
    return;
}


/*
 * free_parsed_data - free malloced data in struct parsed_data created via parse_3_args()
 *
 * This function ONLY frees data that was allocated by functions such as parse_3_args()
 * within is struct parsed_data.  This function does NOT free the struct parsed_data itself.
 *
 * given:
 *	data_p	    - pointer to parsed_data
 *
 * NOTE: This function does not return on error.
 */
static void
free_parsed_data(struct parsed_data *data_p)
{
    /*
     * firewall
     */
    if (data_p == NULL) {
	err(12,  __func__, "data_p is NULL");
	not_reached();
    }

    /*
     * free non-NULL pointers within the struct parsed_data
     */
    if (data_p->path == NULL) {
	free((void *)data_p->path);
	data_p->path = NULL;
    }
    dbg(DBG_V1_HIGH, "free 3 arg parsed data");
    return;
}


/*
 * parse_stream - parse type size path from a stream
 *
 * A line from a stream is parsed for 3 words, where ASCII spaces and ASCII tabs
 * are used as a deliminator.  Multiple ASCII spaces and/or multiple ASCII tabs
 * are treated as a single separator.  Leading ASCII spaces and ASCII tabs on
 * the stream read line is ignored.
 *
 * We ignore empty lines and lines that start with "#" (pound sign).
 *
 * given:
 *	stream	    - file stream to read from
 *	line_p	    - malloced line from stream
 *	type_str_p  - pointer to type arg as a single character string
 *	size_str_p  - pointer to item size as a string
 *	path_p	    - pointer to path arg as a string (pre-canonicalized)
 *
 * returns:
 *	true ==> type size path parsed from stream, *type_str_p, *size_str_p, *path_p updated
 *	false ==> EOF on stream, or read error
 *
 * This function does not return on read error that is NOT an EOF.
 */
static bool
parse_stream(FILE *stream, char **line_p, char **type_str_p, char **size_str_p, char **path_p)
{
    char *line = NULL;		/* data from stream */
    ssize_t len = 0;		/* readline() return - characters in the line with newline removed */
    char *type_str = NULL;	/* parsed type string */
    char *size_str = NULL;	/* parsed size string */
    char *path_str = NULL;	/* parsed path string */

    /*
     * firewall
     */
    if (stream == NULL) {
	err(13,  __func__, "stream is NULL");
	not_reached();
    }
    if (line_p == NULL) {
	err(14,  __func__, "line_p is NULL");
	not_reached();
    }
    if (type_str_p == NULL) {
	err(15,  __func__, "type_str_p is NULL");
	not_reached();
    }
    if (size_str_p == NULL) {
	err(16,  __func__, "size_str_p is NULL");
	not_reached();
    }
    if (path_p == NULL) {
	err(17,  __func__, "path_p is NULL");
	not_reached();
    }

    /*
     * read a line
     */
    do {

	/*
	 * read a line
	 */
	len = readline(&line, stream);
	if (len < 0 || line == NULL) {
	    dbg(DBG_HIGH, "%s: EOF on stream", __func__);
	    if (line != NULL) {
		free(line);
		line = NULL;
	    }
	    return false;
	}

	/*
	 * ignore and read again, if line is empty
	 */
	if (len == 0) {
	    dbg(DBG_V2_HIGH, "%s: line is empty", __func__);
	    continue;
	}

	/*
	 * ignore and read again, if line starts with "#" (pound sign)
	 */
	if (line[0] == '#') {
	    dbg(DBG_V2_HIGH, "%s: line starts with #", __func__);
	    continue;
	}

	/*
	 * parse first word
	 */
	type_str = strtok(line, " \t");
	if (type_str == NULL) {
	    dbg(DBG_V2_HIGH, "%s: line 1st field not found", __func__);
	    continue;
	}

	/*
	 * parse 2nd word
	 */
	size_str = strtok(NULL, " \t");
	if (size_str == NULL) {
	    dbg(DBG_V2_HIGH, "%s: line 2nd field not found", __func__);
	    continue;
	}

	/*
	 * parse 3rd word
	 */
	path_str = strtok(NULL, " \t");
	if (path_str == NULL) {
	    dbg(DBG_V2_HIGH, "%s: line 3rd field not found", __func__);
	    continue;
	}

	/*
	 * save the 3 words and return
	 */
	dbg(DBG_V2_HIGH, "%s: type_str: %s size_str: %s path_str: %s",
			 __func__, type_str, size_str, path_str);
	*line_p = line;
	*type_str_p = type_str;
	*size_str_p = size_str;
	*path_p = path_str;
	return true;

    } while(line != NULL);
    dbg(DBG_HIGH, "%s: fell out of readline loop", __func__);
    return false;
}


/*
 * test_walk - test a walk_set with either args or stream data
 *
 * Perform a test of a walk_set with either sets of 3 args from the command line,
 * or set of 3 strings parsed from the input stream.
 *
 * We initialize a walk_stat, process triples (type, size, path) by recording
 * each set of 3 as a wall step, and when all triples are processed print details
 * if debug level is at least DBG_LOW, and free all storage.
 *
 * given:
 *	argc		- arg count after options,
 *		          must be a multiple of 3,
 *		          0 ==> parse data from FILE stream in
 *	argv		- args to process
 *		          NULL ==> read from FILE stream in
 *	in		- stream read data from, must be non-NULL if argc == 0
 *			  NULL ==> argc > 0
 *	out		- stream to print walk_stat details on
 *
 *	topdir		- simulated topdir
 *	set		- pointer to walk_set being used
 *	context		- context (tool and options) for debugging purposes
 *
 *      max_path_len        - max canonicalized path length, 0 ==> no limit
 *      max_filename_len    - max length of each component of path, 0 ==> no limit
 *      max_depth           - max depth of subdirectory tree, 0 ==> no limit, <0 ==> reserved for future use
 *
 *      max_file            - max counted files, 0 ==> unlimited, -1 ==> none
 *      max_dir             - max counted directories, 0 ==> unlimited, -1 ==> none
 *      max_sym             - max counted symlinks, 0 ==> unlimited, -1 ==> none
 *      max_other           - max counted non-file/dir/symlinks, 0 ==> unlimited, -1 ==> none
 *
 *      tar_listing	    - true ==> process tarball listing, false ==> path processing
 *      sort_cpath	    - true ==> dynamic arrays by canonicalized path, false ==> do not sort
 *
 * returns:
 *	true ==> no walk errors found
 *	false ==> some walk errors found
 */
static bool
test_walk(int argc, char **argv, FILE *in, FILE *out,
	  char const *topdir, struct walk_set *set, char const *context,
          size_t max_path_len, size_t max_filename_len, int_least32_t max_depth,
          int_least32_t max_file, int_least32_t max_dir, int_least32_t max_sym, int_least32_t max_other,
          bool tar_listing, bool sort_cpath)
{
    struct walk_stat wstat;	    /* walk_stat being processed */
    char *line_p = NULL;	    /* malloced line read by parse_stream() */
    bool process = false;	    /* true ==> process item, false ==> ignore item and skip walk */
    struct parsed_data parsed;	    /* parsed canonical path, simulated type, simulated size */
    char *type_str = NULL;	    /* arg as a single character string */
    char *size_str = NULL;	    /* item size as a string */
    char *path = NULL;		    /* path arg as a string (pre-canonicalized) */
    char const *cpath = NULL;	    /* canonicalized path unless error in which case is just path */
    bool walk_ok = true;	    /* true ==> no walk errors found, false ==> some walk errors found */
    bool dup = false;		    /* true ==> entry is a duplicate, false ==> entry is NOT a duplicate */
    int i;

    /*
     * firewall
     */
    if (argc % 3 != 0 || argc < 0) {
	err(18,  __func__, "argc: %d not a multiple of 3 >= 0", argc);
	not_reached();
    }
    if (argc > 0 && argv == NULL) {
	err(19,  __func__, "argc %d > 0 and argv is NULL", argc);
	not_reached();
    }
    if (argc == 0 && in == NULL) {
	err(20,  __func__, "argc is 0 and in is NULL");
	not_reached();
    }
    if (out == NULL) {
	err(21,  __func__, "out is NULL");
	not_reached();
    }
    if (set == NULL) {
	err(22,  __func__, "set is NULL");
	not_reached();
    }
    if (context == NULL) {
	err(23,  __func__, "context is NULL");
	not_reached();
    }

    /*
     * initialize a walk_stat
     */
    init_walk_stat(&wstat, topdir, set, context,
		   max_path_len, max_filename_len, max_depth,
		   tar_listing);
    dbg(DBG_V2_HIGH, "%s: initialized walk_stat for context: %s", __func__, context);
    memset(&parsed, 0, sizeof(parsed));	/* paranoia */

    /*
     * case: process sets of 3 args from the command line
     */
    if (argc > 0) {

	/*
	 * process each triple (type, size, path) on the command line
	 */
	for (i=0; i+2 < argc; i+=3) {

	    /*
	     * parse the next (type size path) arg set
	     */
	    parse_3_args(&parsed, argv[i], argv[i+1], argv[i+2]);

	    /*
	     * take a step with the data from the arguments
	     */
	    process = record_step(&wstat, parsed.path, parsed.st_size, parsed.st_mode, &dup, &cpath);
	    cpath = (cpath == NULL) ? "((NULL))" : cpath;   /* paranoia */
	    if (dup) {
		dbg(DBG_MED, "item[%d]: %s duplicate skipped: %s", (i/3), context, cpath);
	    } else if (process) {
		dbg(DBG_MED, "item[%d]: %s should process: %s", (i/3), context, cpath);
	    } else {
		dbg(DBG_MED, "item[%d]: %s should ignore: %s", (i/3), context, cpath);
	    }

	    /*
	     * free allocated storage related to parsed args
	     */
	    free_parsed_data(&parsed);
	}

    /*
     * case: set of 3 strings parsed from stdin
     */
    } else {

	/*
	 * process lines from stdin
	 */
	for (i=0; parse_stream(in, &line_p, &type_str, &size_str, &path); i+=3) {

	    /*
	     * parse the next (type size path) strings on input
	     */
	    parse_3_args(&parsed, type_str, size_str, path);

	    /*
	     * take a step with the data from the arguments
	     */
	    process = record_step(&wstat, parsed.path, parsed.st_size, parsed.st_mode, &dup, &cpath);
	    cpath = (cpath == NULL) ? "((NULL))" : cpath;   /* paranoia */
	    if (dup) {
		dbg(DBG_MED, "item[%d]: %s duplicate skipped: %s", (i/3), context, cpath);
	    } else if (process) {
		dbg(DBG_MED, "item[%d]: %s should process: %s", (i/3), context, cpath);
	    } else {
		dbg(DBG_MED, "item[%d]: %s should ignore: %s", (i/3), context, cpath);
	    }

	    /*
	     * free allocated storage related to parsed args
	     */
	    free_parsed_data(&parsed);
	}
    }

    /*
     * sort walk_stat arrays by canonicalized path in a case independent way unless -o
     */
    if (sort_cpath) {
	dbg(DBG_MED, "%s: sorting walk_stat arrays by canonicalized path", __func__);
	sort_walk_istat(&wstat);
    }

    /*
     * print walk_stat details on stdout if at least -v 1
     */
    if (dbg_allowed(DBG_HIGH)) {
	fprintf_walk_stat(out, &wstat);
    }

    /*
     * end talk and check if the walk was successful
     */
    walk_ok = chk_walk(&wstat, stderr, max_file, max_dir, max_sym, max_other, true);

    /*
     * free the walk_stat
     */
    free_walk_stat(&wstat);

    /*
     * exit if walk was not successful
     */
    if (walk_ok) {
	dbg(DBG_LOW, "%s walk was successful", context);
    } else {
	dbg(DBG_LOW, "%s some walk errors were detected", context);
    }
    return walk_ok;
}
