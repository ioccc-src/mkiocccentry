/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * This tool was improved by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * The concept of the JSON semantics tables was developed by Landon Curt Noll.
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 *
 * Copyright (c) 2022-2023 by Landon Curt Noll.  All Rights Reserved.
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
#include <ctype.h>
#include <fcntl.h>		/* for open() */

/*
 * chkentry - check JSON files in an IOCCC entry
 */
#include "chkentry.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "soup/version.h"


/*
 * definitions
 */


/*
 * globals
 */
static bool quiet = false;				/* true ==> quiet mode */

/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] entry_dir\n"
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] info.json auth.json\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\n"
    "\tentry_dir\tIOCCC entry directory with .info.json and auth.json files\n"
    "\tinfo.json\tcheck info.json file, . ==> skip IOCCC .info.json style check\n"
    "\tauth.json\tcheck auth.json file, . ==> skip IOCCC .auth.json style check\n"
    "\n"
    "Exit codes:\n"
    "    0\t\tall is OK\n"
    "    1\t\tJSON files(s) are valid JSON but one or more semantic checks failed\n"
    "    2\t\t-h and help string printed or -V and version string printed\n"
    "    3\t\tcommand line error\n"
    "    4\t\tfile(s) not valid JSON; no semantic checks were performed\n"
    "    >=10\tinternal error\n"
    "\n"
    "chkentry version: %s";


/*
 * functions
 */
static FILE *open_json_dir_file(char const *dir, char const *file);
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


/*
 * open_json_dir_file - open a readable JSON file in a given directory
 *
 * Temporarily chdir to the directory, if non-NULL, try to open the file,
 * and then chdir back to the current directory.
 *
 * If dir == NULL, just try to open the file without a chdir.
 *
 * given:
 *	dir	directory into which we will temporarily chdir or
 *		    NULL ==> do not chdir
 *	file	path of readable JSON file to open
 *
 * returns:
 *	open readable JSON file stream
 *
 * NOTE: This function does not return if path is NULL,
 *	 if we cannot chdir to a non-NULL dir, if not a readable file,
 *	 or if unable to open file.
 *
 * NOTE: This function will NOT return NULL.
 */
static FILE *
open_json_dir_file(char const *dir, char const *file)
{
    FILE *ret_stream = NULL;		/* open file stream to return */
    int ret = 0;		/* libc function return */
    int cwd = -1;		/* current working directory */

    /*
     * firewall
     */
    if (file == NULL) {
	err(10, __func__, "called with NULL file");
	not_reached();
    }

    /*
     * note the current directory so we can restore it later, after the chdir(work_dir) below
     */
    errno = 0;                  /* pre-clear errno for errp() */
    cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
    if (cwd < 0) {
        errp(11, __func__, "cannot open .");
        not_reached();
    }

    /*
     * Temporarily chdir if dir is non-NULL
     */
    if (dir != NULL && cwd >= 0) {

	/*
	 * check if we can search / work within the directory
	 */
	if (!exists(dir)) {
	    err(12, __func__, "directory does not exist: %s", dir);
	    not_reached();
	}
	if (!is_dir(dir)) {
	    err(13, __func__, "is not a directory: %s", dir);
	    not_reached();
	}
	if (!is_exec(dir)) {
	    err(14, __func__, "directory is not searchable: %s", dir);
	    not_reached();
	}

	/*
	 * chdir to to the directory
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = chdir(dir);
	if (ret < 0) {
	    errp(15, __func__, "cannot cd %s", dir);
	    not_reached();
	}
    }

    /*
     * must be a readable file
     */
    if (!exists(file)) {
	err(16, __func__, "file does not exist: %s", file);
	not_reached();
    }
    if (!is_file(file)) {
	err(17, __func__, "file is not a regular file: %s", file);
	not_reached();
    }
    if (!is_read(file)) {
	err(18, __func__, "file is not a readable file: %s", file);
	not_reached();
    }

    /*
     * open the readable JSON file
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret_stream = fopen(file, "r");
    if (ret_stream == NULL) {
	errp(19, __func__, "cannot open file: %s", file);
	not_reached();
    }

    /*
     * if we did a chdir to dir, chdir back to cwd
     */
    if (dir != NULL && cwd >= 0) {

	/*
	 * switch back to the previous current directory
	 */
	errno = 0;                  /* pre-clear errno for errp() */
	ret = fchdir(cwd);
	if (ret < 0) {
	    errp(20, __func__, "cannot fchdir to the previous current directory");
	    not_reached();
	}
	errno = 0;                  /* pre-clear errno for errp() */
	ret = close(cwd);
	if (ret < 0) {
	    errp(21, __func__, "close of previous current directory failed");
	    not_reached();
	}
    }

    /*
     * return open stream
     */
    return ret_stream;
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, program, "wrong number of arguments");
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message
 *	prog		our program name
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str)
{
    /*
     * firewall
     */
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): prog was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    if (*str != '\0') {
	fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    }
    fprintf_usage(exitcode, stderr, usage_msg, prog, prog, DBG_DEFAULT, JSON_DBG_DEFAULT, CHKENTRY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


int
main(int argc, char *argv[])
{
    char const *program = NULL;		/* our name */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char const *entry_dir = ".";	/* entry directory to process, or NULL ==> process files */
    char const *info_filename = ".";	/* .info.json file to process, or NULL ==> no .info.json to process */
    char const *auth_filename = ".";	/* .auth.json file to process, or NULL ==> no .auth.json to process */
    char *info_path = NULL;		/* full path of .info.json or NULL */
    char *auth_path = NULL;		/* full path of .auth.json or NULL */
    FILE *info_stream = NULL;		/* open .info.json file stream */
    FILE *auth_stream = NULL;		/* open .auth.json file stream */
    struct json *info_tree = NULL;	/* JSON parse tree for .info.json, or NULL ==> not parsed */
    struct json *auth_tree = NULL;	/* JSON parse tree for .auth.json, or NULL ==> not parsed */
    bool info_valid = false;		/* .info.json is valid JSON */
    bool auth_valid = false;		/* .auth.json is valid JSON */
    struct dyn_array *info_count_err = NULL;	/* JSON semantic count errors for .info.json */
    struct dyn_array *info_val_err = NULL;	/* JSON semantic validation errors for .info.json */
    struct dyn_array *auth_count_err = NULL;	/* JSON semantic count errors for .auth.json */
    struct dyn_array *auth_val_err = NULL;	/* JSON semantic validation errors for .auth.json */
    uintmax_t info_count_err_count = 0;	/* semantic count error count from json_sem_check() for .info.json */
    uintmax_t info_val_err_count = 0;	/* semantic validation error count from json_sem_check() for .info.json */
    uintmax_t info_int_err_count = 0;	/* internal error count from json_sem_check() for .info.json */
    uintmax_t info_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .info.json */
    uintmax_t auth_count_err_count = 0;	/* semantic count error count from json_sem_check() for .auth.json */
    uintmax_t auth_val_err_count = 0;	/* semantic validation count from json_sem_check() for .auth.json */
    uintmax_t auth_int_err_count = 0;	/* internal error count from json_sem_check() for .auth.json */
    uintmax_t auth_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .auth.json */
    uintmax_t all_count_err_count = 0;	/* semantic count error count from json_sem_check() for .auth.json */
    uintmax_t all_val_err_count = 0;	/* semantic validation error count from json_sem_check() for .auth.json */
    uintmax_t all_int_err_count = 0;	/* internal error count from json_sem_check() for .auth.json */
    uintmax_t all_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .auth.json */
    struct json_sem_count_err *sem_count_err = NULL;	/* semantic count error to print */
    struct json_sem_val_err *sem_val_err = NULL;	/* semantic validation error to print */
    uintmax_t c;			/* dynamic array index */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:Vq")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, "");	/*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -v verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'J':		/* -J json_verbosity */
	    /*
	     * parse JSON verbosity level
	     */
	    json_verbosity_level = parse_verbosity(optarg);
	    if (verbosity_level < 0) {
		usage(3, program, "invalid -J json_verbosity"); /*ooo*/
		not_reached();
	    }
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", CHKENTRY_VERSION);
	    exit(2);		/*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    check_invalid_option(program, i, optopt);
	    usage(3, program, ""); /*ooo*/
	    not_reached();
	    break;
	}
    }
    argc -= optind;
    argv += optind;
    switch (argc) {
    case 1:
	entry_dir = argv[0];
	info_filename = NULL;
	auth_filename = NULL;
	break;
    case 2:
	entry_dir = NULL;
	info_filename = argv[0];
	auth_filename = argv[1];
	break;
    default:
	usage(3, program, "wrong number of arguments");	/*ooo*/
	not_reached();
	break;
    }
    if (info_filename != NULL && strcmp(info_filename, ".") == 0 &&
        auth_filename != NULL && strcmp(auth_filename, ".") == 0 && entry_dir == NULL) {
	vrergfB(-1, -1); /* Easter egg */
	not_reached();
    }
    if (entry_dir == NULL) {
	dbg(DBG_LOW, "entry_dir is NULL");
    } else {
	dbg(DBG_LOW, "entry_dir: %s", entry_dir);
    }
    if (info_filename == NULL) {
	dbg(DBG_LOW, "info_filename is NULL");
    } else if (strcmp(info_filename, ".") == 0) {
	dbg(DBG_LOW, "info_filename is .");
    } else {
	dbg(DBG_LOW, "info_filename: %s", info_filename);
    }
    if (auth_filename == NULL) {
	dbg(DBG_LOW, "auth_filename is NULL");
    } else if (strcmp(auth_filename, ".") == 0) {
	dbg(DBG_LOW, "auth_filename is .");
    } else {
	dbg(DBG_LOW, "auth_filename: %s", auth_filename);
    }

    /*
     * case: 1 arg - directory
     */
    if (entry_dir != NULL && info_filename == NULL && auth_filename == NULL) {

	/*
	 * open the .info.json file under entry_dir
	 */
	info_filename = ".info.json";
	info_stream = open_json_dir_file(entry_dir, info_filename);
	if (info_stream == NULL) { /* paranoia */
	    err(22, __func__, "info_stream = open_json_dir_file(%s, %s) returned NULL", entry_dir, info_filename);
	    not_reached();
	}

	/*
	 * open the .auth.json file under entry_dir
	 */
	auth_filename = ".auth.json";
	auth_stream = open_json_dir_file(entry_dir, auth_filename);
	if (auth_stream == NULL) { /* paranoia */
	    err(23, __func__, "auth_stream = open_json_dir_file(%s, %s) returned NULL", entry_dir, auth_filename);
	    not_reached();
	}

    /*
     * case: 2 args - info path and author path
     */
    } else if (entry_dir == NULL && info_filename != NULL && auth_filename != NULL) {

	/*
	 * open the .info.json file unless it is .
	 */
	if (strcmp(info_filename, ".") != 0) {
	    info_stream = open_json_dir_file(NULL, info_filename);
	    if (info_stream == NULL) { /* paranoia */
		err(24, __func__, "open_json_dir_file for returned NULL for .info.json path: %s", info_filename);
		not_reached();
	    }
	} else {
	    info_stream = NULL;
	}

	/*
	 * open the .auth.json file unless it is .
	 */
	if (strcmp(auth_filename, ".") != 0) {
	    auth_stream = open_json_dir_file(NULL, auth_filename);
	    if (auth_stream == NULL) { /* paranoia */
		err(25, __func__, "open_json_dir_file for returned NULL for .auth.json path: %s", auth_filename);
		not_reached();
	    }
	} else {
	    auth_stream = NULL;
	}

    /*
     * case: paranoia
     */
    } else {
	err(26, __func__, "we should not get here; please report, making sure to use make bug_report");
	not_reached();
    }
    info_path = calloc_path(entry_dir, info_filename);
    if (info_path == NULL) {
	err(27, __func__, "info_path is NULL");
	not_reached();
    }
    auth_path = calloc_path(entry_dir, auth_filename);
    if (auth_path == NULL) {
	err(28, __func__, "auth_path is NULL");
	not_reached();
    }

    /*
     * parse .info.json if it is open
     */
    if (info_stream != NULL) {
	info_tree = parse_json_stream(info_stream, info_path, &info_valid);
	if (info_valid == false || info_tree == NULL) {
	    err(4, __func__, "failed to parse JSON in .info.json file: %s", info_path); /*ooo*/
	    not_reached();
	}
	dbg(DBG_LOW, "successful parse of JSON in .info.json file: %s", info_path);
    }

    /*
     * parse .auth.json if it is open
     */
    if (auth_stream != NULL) {
	auth_tree = parse_json_stream(auth_stream, auth_path, &auth_valid);
	if (auth_valid == false || auth_tree == NULL) {
	    err(4, __func__, "failed to parse JSON in .auth.json file: %s", auth_path); /*ooo*/
	    not_reached();
	}
	dbg(DBG_LOW, "successful parse of JSON in .auth.json file: %s", auth_path);
    }

    /*
     * check a JSON parse tree against a JSON semantic table for .info.json, if open
     */
    if (info_stream != NULL) {

	/*
	 * perform JSON semantic analysis on the .info.json JSON parse tree
	 */
	dbg(DBG_HIGH, "about to perform JSON semantic check for .info.json file: %s", info_path);
	info_all_err_count = json_sem_check(info_tree, JSON_DEFAULT_MAX_DEPTH, sem_info,
					  &info_count_err, &info_val_err);

	/*
	 * firewall on json_sem_check() results AND count errors for .info.json
	 */
	if (info_count_err == NULL) {
	    err(29, __func__, "json_sem_check() left info_count_err as NULL for .info.json file: %s", info_path);
	    not_reached();
	}
	if (dyn_array_tell(info_count_err) < 0) {
	    err(30, __func__, "dyn_array_tell(info_count_err): %jd < 0 "
		   "for .info.json file: %s",
		   dyn_array_tell(info_count_err), info_path);
	    not_reached();
	}
	info_count_err_count = (uintmax_t)dyn_array_tell(info_count_err);
	if (info_val_err == NULL) {
	    err(31, __func__, "json_sem_check() left info_val_err as NULL for .info.json file: %s", info_path);
	    not_reached();
	}
	if (dyn_array_tell(info_val_err) < 0) {
	    err(32, __func__, "dyn_array_tell(info_val_err): %jd < 0 "
		   "for .info.json file: %ss",
		   dyn_array_tell(info_val_err), info_path);
	    not_reached();
	}
	info_val_err_count = (uintmax_t)dyn_array_tell(info_val_err);
	if (info_all_err_count < info_count_err_count+info_val_err_count) {
	    err(33, __func__, "info_all_err_count: %ju < info_count_err_count: %ju + info_val_err_count: %ju "
		   "for .info.json file: %s",
		   info_all_err_count, info_count_err_count, info_val_err_count, info_path);
	    not_reached();
	}
	info_int_err_count = info_all_err_count - (info_count_err_count+info_val_err_count);
    }

    /*
     * check a JSON parse tree against a JSON semantic table for .auth.json, if open
     */
    if (auth_stream != NULL) {

	/*
	 * perform JSON semantic analysis on the .auth.json JSON parse tree
	 */
	dbg(DBG_HIGH, "about to perform JSON semantic check for .auth.json file: %s", auth_path);
	auth_all_err_count = json_sem_check(auth_tree, JSON_DEFAULT_MAX_DEPTH, sem_auth,
					  &auth_count_err, &auth_val_err);

	/*
	 * firewall on json_sem_check() results AND count errors for .auth.json
	 */
	if (auth_count_err == NULL) {
	    err(34, __func__, "json_sem_check() left auth_count_err as NULL for .auth.json file: %s", auth_path);
	    not_reached();
	}
	if (dyn_array_tell(auth_count_err) < 0) {
	    err(35, __func__, "dyn_array_tell(auth_count_err): %jd < 0 "
		   "for .auth.json file: %s", dyn_array_tell(auth_count_err), auth_path);
	    not_reached();
	}
	auth_count_err_count = (uintmax_t) dyn_array_tell(auth_count_err);
	if (auth_val_err == NULL) {
	    err(36, __func__, "json_sem_check() left auth_val_err as NULL for .auth.json file: %s", auth_path);
	    not_reached();
	}
	if (dyn_array_tell(auth_val_err) < 0) {
	    err(37, __func__, "dyn_array_tell(auth_val_err): %jd < 0 "
		   "for .auth.json file: %s", dyn_array_tell(auth_val_err), auth_path);
	    not_reached();
	}
	auth_val_err_count = (uintmax_t)dyn_array_tell(auth_val_err);
	if (auth_all_err_count < auth_count_err_count+auth_val_err_count) {
	    err(38, __func__, "auth_all_err_count: %ju < auth_count_err_count: %ju + auth_val_err_count: %ju "
		   "for .auth.json file: %s",
		   auth_all_err_count, auth_count_err_count, auth_val_err_count, auth_path);
	    not_reached();
	}
	auth_int_err_count = auth_all_err_count - (auth_count_err_count+auth_val_err_count);
    }

    /*
     * count all errors
     */
    all_count_err_count = info_count_err_count + auth_count_err_count;
    all_val_err_count = info_val_err_count + auth_val_err_count;
    all_int_err_count = info_int_err_count + auth_int_err_count;
    all_all_err_count = info_all_err_count + auth_all_err_count;

    /*
     * report details of any .info.json semantic errors
     */
    if (info_all_err_count > 0) {
	fpr(stderr, __func__, "What follows are semantic errors for .info.json file: %s\n", info_path);
	if (info_count_err == NULL) {
	    fpr(stderr, __func__, "info_count_err is NULL!!!\n");
	} else {
	    for (c=0; c < info_count_err_count; ++c) {
		sem_count_err = dyn_array_addr(info_count_err, struct json_sem_count_err, c);
		fprint_count_err(stderr, ".info.json count error ", sem_count_err, "\n");
	    }
	    for (c=0; c < info_val_err_count; ++c) {
		sem_val_err = dyn_array_addr(info_val_err, struct json_sem_val_err, c);
		fprint_val_err(stderr, ".info.json validation error ", sem_val_err, "\n");
	    }
	}
	if (info_int_err_count > 0) {
	    fpr(stderr, __func__, ".info.json internal errors found: %ju", info_int_err_count);
	}
    }

    /*
     * report details of any .auth.json semantic errors
     */
    if (auth_all_err_count > 0) {
	fpr(stderr, __func__, "What follows are semantic errors for .auth.json file: %s\n", auth_path);
	if (auth_count_err == NULL) {
	    fpr(stderr, __func__, "auth_count_err is NULL!!!\n");
	} else {
	    for (c=0; c < auth_count_err_count; ++c) {
		sem_count_err = dyn_array_addr(auth_count_err, struct json_sem_count_err, c);
		fprint_count_err(stderr, ".auth.json count error ", sem_count_err, "\n");
	    }
	    for (c=0; c < auth_val_err_count; ++c) {
		sem_val_err = dyn_array_addr(auth_val_err, struct json_sem_val_err, c);
		fprint_val_err(stderr, ".auth.json validation error ", sem_val_err, "\n");
	    }
	}
	if (auth_int_err_count > 0) {
	    fpr(stderr, __func__, ".auth.json internal errors found: %ju", auth_int_err_count);
	}
    }

    /*
     * report on semantic count errors
     */
    if (all_count_err_count > 0) {
	if (info_count_err_count > 0) {
	    dbg(DBG_LOW, "count errors for .info.json: %ju", info_count_err_count);
	}
	if (auth_count_err_count > 0) {
	    dbg(DBG_LOW, "count errors for .auth.json: %ju", auth_count_err_count);
	}
	dbg(DBG_LOW, "count errors for both files: %ju", all_count_err_count);
    }

    /*
     * report on semantic validation errors
     */
    if (all_val_err_count > 0) {
	if (info_val_err_count > 0) {
	    dbg(DBG_LOW, "validation errors for .info.json: %ju", info_val_err_count);
	}
	if (auth_val_err_count > 0) {
	    dbg(DBG_LOW, "validation errors for .auth.json: %ju", auth_val_err_count);
	}
	dbg(DBG_LOW, "validation errors for both files: %ju", all_val_err_count);
    }

    /*
     * report on internal errors
     */
    if (all_int_err_count > 0) {
	if (info_int_err_count > 0) {
	    dbg(DBG_LOW, "internal errors for .info.json: %ju", info_int_err_count);
	}
	if (auth_int_err_count > 0) {
	    dbg(DBG_LOW, "internal errors for .auth.json: %ju", auth_int_err_count);
	}
	dbg(DBG_LOW, "internal errors for both files: %ju", all_int_err_count);
    }

    /*
     * report on total error counts
     */
    if (all_all_err_count > 0) {
	if (info_all_err_count > 0) {
	    dbg(DBG_LOW, "total semantic errors for .info.json: %ju", info_all_err_count);
	}
	if (auth_all_err_count > 0) {
	    dbg(DBG_LOW, "total semantic errors for .auth.json: %ju", auth_all_err_count);
	}
	dbg(DBG_LOW, "total semantic errors for both files: %ju", all_all_err_count);
    }

    /*
     * summarize the JSON semantic check status
     */
    if (info_all_err_count > 0) {
	if (info_path == NULL) {
	    werr(1, __func__, "JSON semantic check failed for .info.json file: ((NULL))"); /*ooo*/
	} else {
	    werr(1, __func__, "JSON semantic check failed for .info.json file: %s", info_path); /*ooo*/
	}
    }
    if (auth_all_err_count > 0) {
	if (auth_path == NULL) {
	    werr(1, __func__, "JSON semantic check failed for .auth.json file: ((NULL))"); /*ooo*/
	} else {
	    werr(1, __func__, "JSON semantic check failed for .auth.json file: %s", auth_path); /*ooo*/
	}
    }
    if (all_all_err_count == 0) {
	dbg(DBG_LOW, "JSON semantic check OK");
    }

    /*
     * cleanup - except for info_stream and auth_stream.
     *
     * We don't try closing info_stream or auth_stream because the json parser
     * already does that at this point. This is because we no longer use yyin
     * due to complications introduced when making the lexer re-entrant. This is
     * not a problem under macOS but it is under linux.
     */
    if (info_tree != NULL) {
	json_tree_free(info_tree, JSON_DEFAULT_MAX_DEPTH);
	info_tree = NULL;
    }
    if (auth_tree != NULL) {
	json_tree_free(auth_tree, JSON_DEFAULT_MAX_DEPTH);
	auth_tree = NULL;
    }
    if (info_count_err != NULL) {
	free_count_err(info_count_err);
	info_count_err = NULL;
    }
    if (info_val_err != NULL) {
	free_val_err(info_val_err);
	info_val_err = NULL;
    }
    if (auth_count_err != NULL) {
	free_count_err(auth_count_err);
	auth_count_err = NULL;
    }
    if (auth_val_err != NULL) {
	free_val_err(auth_val_err);
	auth_val_err = NULL;
    }
    if (info_path != NULL) {
	free(info_path);
	info_path = NULL;
    }
    if (auth_path != NULL) {
	free(auth_path);
	auth_path = NULL;
    }

    /*
     * All Done!!! - Jessica Noll, age 2
     *
     */
    if (all_all_err_count > 0) {
	err(1, __func__, "JSON semantic check failed"); /*ooo*/
	not_reached();
    }
    exit(0); /*ooo*/
}
