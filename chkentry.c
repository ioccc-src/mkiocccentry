/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll.
 * The concept of the JSON semantics tables, which this tool will use, was
 * developed by Landon Curt Noll.
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
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
#include "version.h"


/*
 * definitions
 */


/*
 * functions
 */
static FILE * open_json_dir_file(char const *dir, char const *path);
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
 *	path	path of readable JSON file to open
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
open_json_dir_file(char const *dir, char const *path)
{
    FILE *ret_stream = NULL;		/* open file stream to return */
    int ret = 0;		/* libc function return */
    int cwd = -1;		/* current working directory */

    /*
     * firewall
     */
    if (path == NULL) {
	err(10, __func__, "called with NULL path");
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
    if (!exists(path)) {
	err(16, __func__, "JSON does exist: %s", path);
	not_reached();
    }
    if (!is_file(path)) {
	err(17, __func__, "JSON is not a file: %s", path);
	not_reached();
    }
    if (!is_read(path)) {
	err(18, __func__, "JSON is not a readable file: %s", path);
	not_reached();
    }

    /*
     * open the readable JSON file
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret_stream = fopen(path, "r");
    if (ret_stream == NULL) {
	errp(19, __func__, "cannot open JSON file: %s", path);
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
 *      usage(3, "missing required argument(s), program: %s", program);
 *
 * given:
 *	exitcode        value to exit with
 *	prog		our program name
 *	str		top level usage message
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
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): str was NULL, forcing it to be: %s\n", str);
    }

    /*
     * print the formatted usage stream
     */
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog, prog, DBG_DEFAULT, JSON_DBG_DEFAULT, JNUM_CHK_VERSION);
    exit(exitcode);		/*ooo*/
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
    char const *auth_filename = ".";	/* .author.json file to process, or NULL ==> no .author.json to process */
    char *info_path = NULL;		/* full path of .info.json or NULL */
    char *auth_path = NULL;		/* full path of .author.json or NULL */
    FILE *info_stream = NULL;		/* open .info.json file stream */
    FILE *auth_stream = NULL;		/* open .author.json file stream */
    struct json *info_tree = NULL;	/* JSON parse tree for .info.json, or NULL ==> not parsed */
    struct json *auth_tree = NULL;	/* JSON parse tree for .author.json, or NULL ==> not parsed */
    bool info_valid = false;		/* .info.json is valid JSON */
    bool auth_valid = false;		/* .author.json is valid JSON */
    struct dyn_array *info_count_err = NULL;	/* JSON semantic count errors for .info.json */
    struct dyn_array *info_val_err = NULL;	/* JSON semantic validation errors for .info.json */
    struct dyn_array *auth_count_err = NULL;	/* JSON semantic count errors for .author.json */
    struct dyn_array *auth_val_err = NULL;	/* JSON semantic validation errors for .author.json */
    uintmax_t info_count_err_count = 0;	/* semantic count error count from json_sem_check() for .info.json */
    uintmax_t info_val_err_count = 0;	/* semantic validation error count from json_sem_check() for .info.json */
    uintmax_t info_int_err_count = 0;	/* internal error count from json_sem_check() for .info.json */
    uintmax_t info_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .info.json */
    uintmax_t auth_count_err_count = 0;	/* semantic count error count from json_sem_check() for .author.json */
    uintmax_t auth_val_err_count = 0;	/* semantic validation count from json_sem_check() for .author.json */
    uintmax_t auth_int_err_count = 0;	/* internal error count from json_sem_check() for .author.json */
    uintmax_t auth_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .author.json */
    uintmax_t all_count_err_count = 0;	/* semantic count error count from json_sem_check() for .author.json */
    uintmax_t all_val_err_count = 0;	/* semantic validation error count from json_sem_check() for .author.json */
    uintmax_t all_int_err_count = 0;	/* internal error count from json_sem_check() for .author.json */
    uintmax_t all_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .author.json */
    struct json_sem_count_err *sem_count_err = NULL;	/* semantic count error to print */
    struct json_sem_val_err *sem_val_err = NULL;	/* semantic validation error to print */
    uintmax_t i;			/* dynamic array index */
    int c;

    /*
     * parse args
     */
    program = argv[0];
    while ((c = getopt(argc, argv, "hv:J:Vq")) != -1) {
	switch (c) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, program, "-h help mode");	/*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'J':		/* -J json_verbosity */
	    /*
	     * parse JSON verbosity level
	     */
	    json_verbosity_level = parse_verbosity(program, optarg);
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
	default:
	    usage(3, program, "invalid -flag");	/*ooo*/
	    not_reached();
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
	usage(2, program, "expected 1 or 2 argument");	/*ooo*/
	not_reached();
	break;
    }
    if (info_filename != NULL && strcmp(info_filename, ".") == 0 &&
        auth_filename != NULL && strcmp(auth_filename, ".") == 0 && entry_dir == NULL) {
	/* Easter egg */
	vrergfB(-1, -1);
	not_reached();
    }
    if (entry_dir == NULL) {
	dbg(1, "entry_dir is NULL");
    } else {
	dbg(1, "entry_dir: %s", entry_dir);
    }
    if (info_filename == NULL) {
	dbg(1, "info_filename is NULL");
    } else if (strcmp(info_filename, ".") == 0) {
	dbg(1, "info_filename is .");
    } else {
	dbg(1, "info_filename: %s", info_filename);
    }
    if (auth_filename == NULL) {
	dbg(1, "auth_filename is NULL");
    } else if (strcmp(auth_filename, ".") == 0) {
	dbg(1, "auth_filename is .");
    } else {
	dbg(1, "auth_filename: %s", auth_filename);
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
	    err(22, __func__, "open_json_dir_file(%s, %s) returned NULL", entry_dir, info_filename);
	    not_reached();
	}

	/*
	 * open the .author.json file under entry_dir
	 */
	auth_filename = ".author.json";
	auth_stream = open_json_dir_file(entry_dir, auth_filename);
	if (auth_stream == NULL) { /* paranoia */
	    err(23, __func__, "open_json_dir_file(%s, %s) returned NULL", entry_dir, auth_filename);
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
	 * open the .author.json file unless it is .
	 */
	if (strcmp(auth_filename, ".") != 0) {
	    auth_stream = open_json_dir_file(NULL, auth_filename);
	    if (auth_stream == NULL) { /* paranoia */
		err(25, __func__, "open_json_dir_file for returned NULL for .author.json path: %s", auth_filename);
		not_reached();
	    }
	} else {
	    auth_stream = NULL;
	}

    /*
     * case: paranoia
     */
    } else {
	err(26, __func__, "we should not get here");
	not_reached();
    }
    info_path = malloc_path(entry_dir, info_filename);
    if (info_path == NULL) {
	err(27, __func__, "info_path is NULL");
	not_reached();
    }
    auth_path = malloc_path(entry_dir, auth_filename);
    if (auth_path == NULL) {
	err(28, __func__, "auth_path is NULL");
	not_reached();
    }

    /*
     * parse .info.json if it is open
     */
    if (info_stream != NULL) {
	info_tree = parse_json_stream(info_stream, &info_valid);
	if (info_valid == false || info_tree == NULL) {
	    err(29, __func__, "failed to JSON parse of .info.json file: %s", info_path);
	    not_reached();
	}
	dbg(DBG_LOW, "successful JSON parse of .info.json file: %s", info_path);
    }

    /*
     * parse .author.json if it is open
     */
    if (auth_stream != NULL) {
	auth_tree = parse_json_stream(auth_stream, &auth_valid);
	if (auth_valid == false || auth_tree == NULL) {
	    err(30, __func__, "failed to JSON parse of .author.json file: %s", auth_path);
	    not_reached();
	}
	dbg(DBG_LOW, "successful JSON parse of .author.json file: %s", auth_path);
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
	    err(31, __func__, "json_sem_check() left info_count_err as NULL for .info.json file: %s", info_path);
	    not_reached();
	}
	if (dyn_array_tell(info_count_err) < 0) {
	    err(32, __func__, "dyn_array_tell(info_count_err): %jd < 0 "
		   "for .info.json file: %s",
		   dyn_array_tell(info_count_err), info_path);
	    not_reached();
	}
	info_count_err_count = (uintmax_t) dyn_array_tell(info_count_err);
	if (info_val_err == NULL) {
	    err(33, __func__, "json_sem_check() left info_val_err as NULL for .info.json file: %s", info_path);
	    not_reached();
	}
	if (dyn_array_tell(info_val_err) < 0) {
	    err(34, __func__, "dyn_array_tell(info_val_err): %jd < 0 "
		   "for .info.json file: %ss",
		   dyn_array_tell(info_val_err), info_path);
	    not_reached();
	}
	info_val_err_count = (uintmax_t) dyn_array_tell(info_val_err);
	if (info_all_err_count < info_count_err_count+info_val_err_count) {
	    err(35, __func__, "info_all_err_count: %ju < info_count_err_count: %ju + info_val_err_count: %ju "
		   "for .info.json file: %s",
		   info_all_err_count, info_count_err_count, info_val_err_count, info_path);
	    not_reached();
	}
	info_int_err_count = info_all_err_count - (info_count_err_count+info_val_err_count);
    }

    /*
     * check a JSON parse tree against a JSON semantic table for .author.json, if open
     */
    if (auth_stream != NULL) {

	/*
	 * perform JSON semantic analysis on the .author.json JSON parse tree
	 */
	dbg(DBG_HIGH, "about to perform JSON semantic check for .author.json file: %s", auth_path);
	auth_all_err_count = json_sem_check(auth_tree, JSON_DEFAULT_MAX_DEPTH, sem_auth,
					  &auth_count_err, &auth_val_err);

	/*
	 * firewall on json_sem_check() results AND count errors for .author.json
	 */
	if (auth_count_err == NULL) {
	    err(36, __func__, "json_sem_check() left auth_count_err as NULL for .author.json file: %s", auth_path);
	    not_reached();
	}
	if (dyn_array_tell(auth_count_err) < 0) {
	    err(37, __func__, "dyn_array_tell(auth_count_err): %jd < 0 "
		   "for .author.json file: %s", dyn_array_tell(auth_count_err), auth_path);
	    not_reached();
	}
	auth_count_err_count = (uintmax_t) dyn_array_tell(auth_count_err);
	if (auth_val_err == NULL) {
	    err(38, __func__, "json_sem_check() left auth_val_err as NULL for .author.json file: %s", auth_path);
	    not_reached();
	}
	if (dyn_array_tell(auth_val_err) < 0) {
	    err(39, __func__, "dyn_array_tell(auth_val_err): %jd < 0 "
		   "for .author.json file: %s", dyn_array_tell(auth_val_err), auth_path);
	    not_reached();
	}
	auth_val_err_count = (uintmax_t) dyn_array_tell(auth_val_err);
	if (auth_all_err_count < auth_count_err_count+auth_val_err_count) {
	    err(40, __func__, "auth_all_err_count: %ju < auth_count_err_count: %ju + auth_val_err_count: %ju "
		   "for .author.json file: %s",
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
	    fpr(stderr, __func__, "  info_count_err is NULL!!!\n");
	} else {
	    for (i=0; i < info_count_err_count; ++i) {
		sem_count_err = dyn_array_addr(info_count_err, struct json_sem_count_err, i);
		fprint_count_err(stderr, "  .info.json count error ", sem_count_err, "\n");
	    }
	    for (i=0; i < info_val_err_count; ++i) {
		sem_val_err = dyn_array_addr(info_val_err, struct json_sem_val_err, i);
		fprint_val_err(stderr, "  .info.json validation error ", sem_val_err, "\n");
	    }
	}
	if (info_int_err_count > 0) {
	    fpr(stderr, __func__, "  .info.json internal errors found: %ju", info_int_err_count);
	}
    }

    /*
     * report details of any .author.json semantic errors
     */
    if (auth_all_err_count > 0) {
	fpr(stderr, __func__, "What follows are semantic errors for .author.json file: %s\n", auth_path);
	if (auth_count_err == NULL) {
	    fpr(stderr, __func__, "  auth_count_err is NULL!!!\n");
	} else {
	    for (i=0; i < auth_count_err_count; ++i) {
		sem_count_err = dyn_array_addr(auth_count_err, struct json_sem_count_err, i);
		fprint_count_err(stderr, "  .author.json count error ", sem_count_err, "\n");
	    }
	    for (i=0; i < auth_val_err_count; ++i) {
		sem_val_err = dyn_array_addr(auth_val_err, struct json_sem_val_err, i);
		fprint_val_err(stderr, "  .author.json validation error ", sem_val_err, "\n");
	    }
	}
	if (auth_int_err_count > 0) {
	    fpr(stderr, __func__, "  .author.json internal errors found: %ju", auth_int_err_count);
	}
    }

    /*
     * report on semantic count errors
     error: */
    if (all_count_err_count > 0) {
	if (info_count_err_count > 0) {
	    dbg(DBG_LOW, "count errors for   .info.json: %ju", info_count_err_count);
	}
	if (auth_count_err_count > 0) {
	    dbg(DBG_LOW, "count errors for .author.json: %ju", auth_count_err_count);
	}
	dbg(DBG_LOW, "count errors for   both files: %ju", all_count_err_count);
    }

    /*
     * report on semantic validation errors
     */
    if (all_val_err_count > 0) {
	if (info_val_err_count > 0) {
	    dbg(DBG_LOW, "validation errors for   .info.json: %ju", info_val_err_count);
	}
	if (auth_val_err_count > 0) {
	    dbg(DBG_LOW, "validation errors for .author.json: %ju", auth_val_err_count);
	}
	dbg(DBG_LOW, "validation errors for   both files: %ju", all_val_err_count);
    }

    /*
     * report on internal errors
     */
    if (all_int_err_count > 0) {
	if (info_int_err_count > 0) {
	    dbg(DBG_LOW, "internal errors for   .info.json: %ju", info_int_err_count);
	}
	if (auth_int_err_count > 0) {
	    dbg(DBG_LOW, "internal errors for .author.json: %ju", auth_int_err_count);
	}
	dbg(DBG_LOW, "internal errors for   both files: %ju", all_int_err_count);
    }

    /*
     * report on total error counts
     */
    if (all_all_err_count > 0) {
	if (info_all_err_count > 0) {
	    dbg(DBG_LOW, "total semantic errors for   .info.json: %ju", info_all_err_count);
	}
	if (auth_all_err_count > 0) {
	    dbg(DBG_LOW, "total semantic errors for .author.json: %ju", auth_all_err_count);
	}
	dbg(DBG_LOW, "total semantic errors for   both files: %ju", all_all_err_count);
    }

    /*
     * cleanup - except for info_path and auth_path
     */
    if (info_stream != NULL) {
	clearerr_or_fclose(info_stream);
	info_stream = NULL;
    }
    if (auth_stream != NULL) {
	clearerr_or_fclose(auth_stream);
	auth_stream = NULL;
    }
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

    /*
     * All Done!!! - Jessica Noll, age 2
     *
     */
    if (all_all_err_count > 0) {
	if (info_all_err_count > 0) {
	    if (info_path == NULL) {
		werr(1, __func__, "JSON semantic check failed for .info.json file: ((NULL))"); /*ooo*/
	    } else {
		werr(1, __func__, "JSON semantic check failed for .info.json file: %s", info_path); /*ooo*/
		free(info_path);
		info_path = NULL;
	    }
	}
	if (auth_all_err_count > 0) {
	    if (auth_path == NULL) {
		werr(1, __func__, "JSON semantic check failed for .author.json file: ((NULL))"); /*ooo*/
	    } else {
		werr(1, __func__, "JSON semantic check failed for .author.json file: %s", auth_path); /*ooo*/
		free(auth_path);
		auth_path = NULL;
	    }
	}
	/* XXX - change 0, to 1 below so that semantic errors will force a non-zero exit - XXX */
	err(0, __func__, "JSON semantic check failed"); /*ooo*/
	not_reached();
    }
    exit(0); /*ooo*/
}
