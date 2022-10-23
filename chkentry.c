/*
 * chkentry - check JSON files in an IOCCC entry
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * The JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 *
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll.
 * The concept of the JSON semantics table, which this tool will use, was
 * developed by Landon Curt Noll.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
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
static FILE * chdir_open_json(char const *dir, char const *path);
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


/*
 * chdir_open_json - chdir and open a readable JSON file
 *
 * Chdir to the directory, if non-NULL, try to open the file.
 * If dir == NULL, just try to open the file.
 *
 * given:
 *	dir	directory into which we will chdir or NULL ==> do not chdir
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
chdir_open_json(char const *dir, char const *path)
{
    FILE *ret = NULL;		/* open file stream to return */
    int retc = 0;		/* libc function return */

    /*
     * firewall
     */
    if (path == NULL) {
	err(10, __func__, "called with NULL path");
	not_reached();
    }

    /*
     * chdir if dir is non-NULL
     */
    if (dir != NULL) {

	/*
	 * check if we can search / work within the directory
	 */
	if (!exists(dir)) {
	    err(11, __func__, "directory does not exist: %s", dir);
	    not_reached();
	}
	if (!is_dir(dir)) {
	    err(12, __func__, "is not a directory: %s", dir);
	    not_reached();
	}
	if (!is_exec(path)) {
	    err(13, __func__, "directory is not searchable: %s", dir);
	    not_reached();
	}

	/*
	 * chdir to to the directory
	 */
	errno = 0;		/* pre-clear errno for errp() */
	retc = chdir(dir);
	if (retc < 0) {
	    errp(14, __func__, "cannot cd %s", dir);
	    not_reached();
	}
    }

    /*
     * must be a readable file
     */
    if (!exists(path)) {
	err(15, __func__, "JSON does exist: %s", path);
	not_reached();
    }
    if (!is_file(path)) {
	err(16, __func__, "JSON is not a file: %s", path);
	not_reached();
    }
    if (!is_read(path)) {
	err(17, __func__, "JSON is not a readable file: %s", path);
	not_reached();
    }

    /*
     * open the readable JSON file
     */
    errno = 0;		/* pre-clear errno for errp() */
    ret = fopen(path, "r");
    if (ret == NULL) {
	errp(18, __func__, "cannot open JSON file: %s", path);
	not_reached();
    }

    /*
     * return open stream
     */
    return ret;
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
    FILE *info_stream = NULL;		/* open .info.json file stream */
    FILE *auth_stream = NULL;		/* open .author.json file stream */
    struct json *info_tree = NULL;	/* JSON parse tree for .info.json, or NULL ==> not parsed */
    struct json *auth_tree = NULL;	/* JSON parse tree for .author.json, or NULL ==> not parsed */
    bool info_valid = false;		/* .info.json is valid JSON */
    bool auth_valid = false;		/* .author.json is valid JSON */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:J:Vq")) != -1) {
	switch (i) {
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
	info_stream = chdir_open_json(entry_dir, info_filename);
	if (info_stream == NULL) { /* paranoia */
	    err(19, __func__, "chdir_open_json(%s, %s) returned NULL", entry_dir, info_filename);
	    not_reached();
	}

	/*
	 * open the .author.json file under entry_dir
	 */
	info_filename = ".author.json";
	auth_stream = chdir_open_json(entry_dir, auth_filename);
	if (auth_stream == NULL) { /* paranoia */
	    err(20, __func__, "chdir_open_json(%s, %s) returned NULL", entry_dir, auth_filename);
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
	    info_stream = chdir_open_json(NULL, info_filename);
	    if (info_stream == NULL) { /* paranoia */
		err(21, __func__, "chdir_open_json for returned NULL for .info.json path: %s", info_filename);
		not_reached();
	    }
	} else {
	    info_stream = NULL;
	}

	/*
	 * open the .author.json file unless it is .
	 */
	if (strcmp(auth_filename, ".") != 0) {
	    auth_stream = chdir_open_json(NULL, auth_filename);
	    if (auth_stream == NULL) { /* paranoia */
		err(22, __func__, "chdir_open_json for returned NULL for .author.json path: %s", auth_filename);
		not_reached();
	    }
	} else {
	    auth_stream = NULL;
	}

    /*
     * case: paranoia
     */
    } else {
	err(23, __func__, "we should not get here");
	not_reached();
    }

    /*
     * parse .info.json if it is open
     */
    if (info_stream != NULL) {
	info_tree = parse_json_stream(info_stream, &info_valid);
	if (info_valid == false || info_tree == NULL) {
	    err(24, __func__, "failed to JSON parse of .info.json file: %s%s%s",
		   entry_dir == NULL ? "" : entry_dir,
		   entry_dir == NULL ? "" : "/",
		   info_filename);
	    not_reached();
	}
	dbg(DBG_LOW, "successful JSON parse of .info.json file: %s%s%s",
	       entry_dir == NULL ? "" : entry_dir,
	       entry_dir == NULL ? "" : "/",
	       info_filename);
    }

    /*
     * parse .author.json if it is open
     */
    if (auth_stream != NULL) {
	auth_tree = parse_json_stream(auth_stream, &auth_valid);
	if (auth_valid == false || auth_tree == NULL) {
	    err(25, __func__, "failed to JSON parse of .author.json file: %s%s%s",
		   entry_dir == NULL ? "" : entry_dir,
		   entry_dir == NULL ? "" : "/",
		   auth_filename);
	    not_reached();
	}
	dbg(DBG_LOW, "successful JSON parse of .author.json file: %s%s%s",
	       entry_dir == NULL ? "" : entry_dir,
	       entry_dir == NULL ? "" : "/",
	       auth_filename);
    }

    /* XXX - add more code here - XXX */

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(0); /*ooo*/
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
