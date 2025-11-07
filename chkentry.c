/*
 * chkentry - check JSON files in an IOCCC submission
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * Copyright (c) 2022-2025 by Landon Curt Noll and Cody Boone Ferguson.
 * All Rights Reserved.
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
 * This tool and the JSON parser were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * The concept of the JSON semantics tables was developed by Landon Curt Noll.
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
#include <fcntl.h>
#include <locale.h>

/*
 * chkentry - check JSON files in an IOCCC submission
 */
#include "chkentry.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */


/*
 * globals
 */
static bool quiet = false;		    /* true ==> quiet mode */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] [-b] [-I path] ... [-P] [-s] [-S] [-w] dir\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-q\t\tquiet mode (def: loud :-) )\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-b\t\tBING!!!\n"
    "\t-I path\t\tIgnore path under dir if found\n"
    "\n"
    "\t-P\t\tIOCCC judge use only: ignore permissions, ignore counts, and ignore path depth limits\n"
    "\t-s\t\tIOCCC judge use only: enter special mode\n"
    "\t\t\t    NOTE: -s conflicts with -w\n"
    "\t-S\t\tenter submission mode: used by the chksubmit(1) tool\n"
    "\t\t\t    NOTE: -S conflicts with -I path, -P, -s, and -w\n"
    "\t-w\t\tIOCCC judge use only: winning entry checks\n"
    "\t\t\t    NOTE: -w conflicts with -s\n"
    "\n"
    "\tdir\tthe directory to be checked (entry directory if -w, submission directory otherwise)\n"
    "\n"
    "Exit codes:\n"
    "    0\t\tall is OK\n"
    "    1\t\tsome check failed, invalid JSON, semantic test failure, or other test failure\n"
    "    2\t\t-h and help string printed or -V and version string printed\n"
    "    3\t\tcommand line error\n"
    "    >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";

/*
 * functions
 */
static bool chk_json(FILE *err, char const *submission_dir, char const *filename, struct json_sem *sem);
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));


int
main(int argc, char *argv[])
{
    char const *program = NULL;		/* our name */
    char *submission_dir = NULL;        /* directory from which files are to be checked */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    bool opt_error = false;		/* fchk_inval_opt() return */
    /**/
    bool winning_entry_mode = false;    /* chkentry -w - true ==> -w used, do other checks */
    bool special_mode = false;          /* chkentry -s - special features for the judges :-) */
    bool submission_mode = false;       /* chkentry -S - also used by chksubmit(1) */
    /**/
    struct walk_stat wstat;		/* walk_stat being processed */
    struct walk_set *wset_p = NULL;	/* pointer to a walk set */
    char const *context = NULL;		/* string describing the context (tool and options) for debugging purposes */
    char const *json_filename = NULL;	/* name of JSON file under submission_dir to check */
    bool json_ok = true;		/* true ==> no JSON errors found, false ==> some JSON errors found */
    bool walk_ok = true;		/* true ==> no walk errors found, false ==> some walk errors found */
    /**/
    struct item *i_p;			/* pointer to an element in the dynamic array */
    intmax_t len;			/* number of elements in the dynamic array */
    /**/
    bool cap_I_seen = false;		/* The -I path was seen on the command line */
    bool skip_add_ret = false;		/* return from skip_add() */
    /**/
    int cd_ret = -1;			/* chdir(2) return */
    /**/
    int i;
    intmax_t j;

    /* IOCCC requires use of C locale */
    set_ioccc_locale();

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:VqbI:PwsS")) != -1) {
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
	    print("%s version: %s\n", CHKENTRY_BASENAME, CHKENTRY_VERSION);
	    print("jparse utils version: %s\n", JPARSE_UTILS_VERSION);
	    print("jparse UTF-8 version: %s\n", JPARSE_UTF8_VERSION);
	    print("jparse library version: %s\n", JPARSE_LIBRARY_VERSION);
	    exit(2);		/*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	case 'b':
	    msg("This is the option that goes \aBING!!!");
	    exit(0); /*ooo*/
	case 'I':
	    cap_I_seen = true;
	    break;
        case 'P':		/* -P - ignore permissions of paths */
            ignore_permissions = true;
            break;
        case 'w':
            winning_entry_mode = true;
            break;
        case 's':
            special_mode = true;
            break;
        case 'S':
            submission_mode = true;
            break;
	case ':':   /* option requires an argument */
	case '?':   /* illegal option */
	default:    /* anything else but should not actually happen */
	    opt_error = fchk_inval_opt(stderr, program, i, optopt);
	    if (opt_error) {
		usage(3, program, ""); /*ooo*/
		not_reached();
	    } else {
		fwarn(stderr, __func__, "getopt() return: %c optopt: %c", (char)i, (char)optopt);
	    }
	    break;
	}
    }
    switch (argc-optind) {
    case 1:
        /*
         * get absolute path of submission dir
         */
        errno = 0; /* pre-clear errno for errp() */
        /*
         * chdir
         */
        if (chdir(argv[optind]) != 0) {
            errp(44, __func__, "failed to chdir() into submission dir: %s", argv[optind]);
            not_reached();
        }
        submission_dir = getcwd(NULL, 0);
        if (submission_dir == NULL) {
            err(44, __func__, "failed to get absolute path for: %s", argv[optind]);
            not_reached();
        }
	break;
    case 2:
	/* how dotty! */
        if (!strcmp(argv[optind], ".") && !strcmp(argv[optind+1], ".")) {
            vrergfB(-1, -1); /* Easter egg */
            not_reached();
        }
        /*fallthrough*/
    default:
	usage(3, program, "wrong number of arguments");	/*ooo*/
	not_reached();
	break;
    }

    /*
     * set the struct walk_set and context string
     */

    /*
     * case: chkentry -S
     *
     * This case is usually via the chksubmit tool.
     */
    if (submission_mode) {

	/* set walk_set and context */
	wset_p = &walk_chkentry_S;
	context = "chkentry -S";

	/* check for conflicting options */
	if (special_mode) {
	    usage(3, program, "cannot use -S with the -s option"); /*ooo*/
	    not_reached();
	} else if (winning_entry_mode) {
	    usage(3, program, "cannot use -S with the -w option"); /*ooo*/
	    not_reached();
	} else if (ignore_permissions) {
	    usage(3, program, "cannot use -S with the -P option"); /*ooo*/
	    not_reached();
	} else if (cap_I_seen) {
	    usage(3, program, "cannot use -S with the -I path option"); /*ooo*/
	    not_reached();
	}

    /*
     * case: chkentry -s
     */
    } else if (special_mode) {

	/* set walk_set and context */
	wset_p = &walk_chkentry_s;
	context = "chkentry -s";

	/* check for conflicting options */
	if (submission_mode) {
	    usage(3, program, "cannot use -s with the -S option"); /*ooo*/
	    not_reached();
	} else if (winning_entry_mode) {
	    usage(3, program, "cannot use -s with the -w option"); /*ooo*/
	    not_reached();
	}

    /*
     * case: chkentry -w
     */
    } else if (winning_entry_mode) {

	/* set walk_set and context */
	wset_p = &walk_chkentry_w;
	context = "chkentry -w";

	/* check for conflicting options */
	if (submission_mode) {
	    usage(3, program, "cannot use -s with the -S option"); /*ooo*/
	    not_reached();
	} else if (special_mode) {
	    usage(3, program, "cannot use -s with the -s option"); /*ooo*/
	    not_reached();
	}

    /*
     * case: neither -S, nor -s, nor -w used
     *
     * We will assume -S is used.
     */
    } else {

	/* set walk_set and context */
	wset_p = &walk_chkentry_S;
	context = "chkentry -S";
	submission_mode = true;

	/* check for conflicting options */
	if (ignore_permissions) {
	    usage(3, program, "cannot use -P option when neither -S, nor -s, nor -w are used"); /*ooo*/
	    not_reached();
	} else if (cap_I_seen) {
	    usage(3, program, "cannot use -I path option when neither -S, nor -s, nor -w are used"); /*ooo*/
	    not_reached();
	}
    }
    dbg(DBG_LOW, "submission_dir: %s", submission_dir);
    dbg(DBG_LOW, "will use context: %s name: %s",
		 context, (wset_p == NULL || wset_p->name == NULL) ? "((NULL))" : wset_p->name);

    /*
     * init_walk_stat - initialize a walk_stat
     */
    if (ignore_permissions) {
	init_walk_stat(&wstat,
		       submission_dir, wset_p, context,
		       0, 0, 0,
		       false);
    } else {
	init_walk_stat(&wstat,
		       submission_dir, wset_p, context,
		       MAX_PATH_LEN, MAX_FILENAME_LEN, MAX_PATH_DEPTH,
		       false);
    }

    /*
     * if we had -I path, rescan the command line looking for and processing -I path options
     */
    if (cap_I_seen) {

	optind = 1;	/* reset getopt(3) processing */
	while ((i = getopt(argc, argv, ":hv:J:VqI:PwsSb")) != -1) {
	    switch (i) {
	    case 'h':		/* -h - print help to stderr and exit 0 */
	    case 'v':		/* -v verbosity */
	    case 'J':		/* -J json_verbosity */
	    case 'V':		/* -V - print version and exit */
	    case 'q':
	    case 'P':		/* -P - ignore permissions of paths */
	    case 'w':
	    case 's':
	    case 'S':
	    case 'b':
		/* already processed, nothing to do now */
		break;
	    case 'I':
		skip_add_ret = skip_add(&wstat, optarg);
		if (skip_add_ret) {
		    dbg(DBG_LOW, "context will ignore, when canonicalized: %s", optarg);
		} else {
		    dbg(DBG_MED, "path: %s is already marked for skipping", optarg);
		}
		break;
	    case ':':   /* option requires an argument */
	    case '?':   /* illegal option */
	    default:    /* anything else but should not actually happen */
		opt_error = fchk_inval_opt(stderr, program, i, optopt);
		if (opt_error) {
		    usage(3, program, ""); /*ooo*/
		    not_reached();
		} else {
		    fwarn(stderr, __func__, "getopt() 2nd return: %c optopt: %c", (char)i, (char)optopt);
		}
		break;
	    }
	}
    }

    /*
     * walk a file system tree, recording steps
     */
    walk_ok = fts_walk(&wstat);
    if (walk_ok == false) {
	err(1, CHKENTRY_BASENAME, "failed to scan: %s", submission_dir); /*ooo*/
	not_reached();
    }

    /*
     * sort walk_stat arrays by canonicalized path in a case independent way
     */
    sort_walk_istat(&wstat);

    /*
     * end walk and check if the walk was successful
     */
    if (winning_entry_mode) {
	walk_ok = chk_walk(&wstat, stderr, ANY_COUNT, ANY_COUNT, NO_COUNT, NO_COUNT, true);
    } else {
	walk_ok = chk_walk(&wstat, stderr, MAX_EXTRA_FILE_COUNT, MAX_EXTRA_DIR_COUNT, NO_COUNT, NO_COUNT, true);
    }
    if (walk_ok) {
	dbg(DBG_LOW, "%s walk was successful for: %s", context, submission_dir);
    } else {
	dbg(DBG_LOW, "%s some walk errors were detected for: %s", context, submission_dir);
    }

    /*
     * check permissions unless ignore_permissions
     */
    if (!ignore_permissions) {

	/*
	 * scan files for permissions
	 */
	len = dyn_array_tell(wstat.file);
	for (j=0; j < len; ++j) {

	    /*
	     * obtain file permission
	     */
	    i_p = dyn_array_value(wstat.file, struct item *, j);
	    if (i_p == NULL) {
		err(10, CHKENTRY_BASENAME, "wstat.file[%jd] is NULL\n", j); /*coo*/
		not_reached();
	    } else if (i_p->fts_path == NULL) {
		err(11, CHKENTRY_BASENAME, "wstat.file[%jd].fts_path is NULL\n", j);
		not_reached();
	    } else if (i_p->fts_name == NULL) {
		err(12, CHKENTRY_BASENAME, "wstat.file[%jd].fts_name is NULL\n", j);
		not_reached();
	    }

	    /*
	     * case: file ends in .sh in any case
	     */
	    if (i_p->fts_namelen >= LITLEN(".sh") &&
		strcasecmp(i_p->fts_name + i_p->fts_namelen - LITLEN(".sh"), ".sh") == 0) {

		/*
		 * verify mode 0555
		 */
		if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0555) {
		    dbg(DBG_LOW, "shell file: %s permission: %03o != %03o",
				  i_p->fts_path, ITEM_PERM(i_p->st_mode), ITEM_PERM_0555);
		    walk_ok = false;
		}

	    /*
	     * case: file does NOT end in .sh
	     */
	    } else {

		/*
		 * verify mode 0444
		 */
		if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0444) {
		    dbg(DBG_LOW, "non-shell file: %s permission: %03o != %03o",
				  i_p->fts_path, ITEM_PERM(i_p->st_mode), ITEM_PERM_0444);
		    walk_ok = false;
		}
	    }
	}

	/*
	 * scan directories for permissions
	 */
	len = dyn_array_tell(wstat.dir);
	for (j=0; j < len; ++j) {

	    /*
	     * obtain file permission
	     */
	    i_p = dyn_array_value(wstat.dir, struct item *, j);
	    if (i_p == NULL) {
		err(13, CHKENTRY_BASENAME, "wstat.dir[%jd] is NULL\n", j);
		not_reached();
	    } else if (i_p->fts_path == NULL) {
		err(14, CHKENTRY_BASENAME, "wstat.dir[%jd].fts_path is NULL\n", j);
		not_reached();
	    }

	    /*
	     * verify mode 0755
	     */
	    if (ITEM_PERM(i_p->st_mode) != ITEM_PERM_0755) {
		dbg(DBG_LOW, "directory: %s permission: %03o != %03o",
			      i_p->fts_path, ITEM_PERM(i_p->st_mode), ITEM_PERM_0755);
		walk_ok = false;
	    }
	}
    }

    /*
     * cd to the submission directory for any final JSON file verification
     *
     * At this time we no longer need to remain at the current directory,
     * so the cd below is OK.
     */
    errno = 0;	    /* pre-clear errno for errp() */
    cd_ret = chdir(submission_dir);
    if (cd_ret < 0) {

	/* report failure to cd to submission_dir */
        errp(1, CHKENTRY_BASENAME, "cannot cd: %s", submission_dir); /*ooo*/
	not_reached();

    /*
     * case: verify .entry.json JSON file
     */
    } else if (winning_entry_mode) {

	/*
	 * check .entry.json
	 *
	 * TODO: until we have a struct json_sem for .entry.json, we pass NULL
	 */
	json_filename = ".entry.json";
	json_ok = chk_json(stderr, ".", json_filename, NULL);
	if (json_ok == false) {
	    dbg(DBG_LOW, "invalid JSON file: %s/%s", submission_dir, json_filename);
	    walk_ok = false;
	}

    /*
     * case: verify .auth.json and .info.json JSON files
     */
    } else {

	/*
	 * check .auth.json
	 */
	json_filename = ".auth.json";
	json_ok = chk_json(stderr, ".", json_filename, sem_auth);
	if (json_ok == false) {
	    dbg(DBG_LOW, "invalid JSON file: %s/%s", submission_dir, json_filename);
	    walk_ok = false;
	}

	/*
	 * check .info.json
	 */
	json_filename = ".info.json";
	json_ok = chk_json(stderr, ".", json_filename, sem_info);
	if (json_ok == false) {
	    dbg(DBG_LOW, "invalid JSON file: %s/%s", submission_dir, json_filename);
	    walk_ok = false;
	}
    }

    /*
     * free submission directory
     */
    if (submission_dir != NULL) {
        free(submission_dir);
        submission_dir = NULL;
    }

    /*
     * free the walk_stat
     */
    free_walk_stat(&wstat);

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     */
    if (!walk_ok) {
	err(1, CHKENTRY_BASENAME, "check failed for: %s", submission_dir); /*ooo*/
    }
    exit(0); /*ooo*/
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
	prog = CHKENTRY_BASENAME;
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
    fprintf_usage(exitcode, stderr, usage_msg, prog, DBG_DEFAULT, JSON_DBG_DEFAULT,
	    CHKENTRY_BASENAME, CHKENTRY_VERSION, JPARSE_UTILS_VERSION, JPARSE_UTF8_VERSION, JPARSE_LIBRARY_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}


/*
 * chk_json - check a JSON file
 *
 * given:
 *	err		    stream on which to report errors
 *	submission_dir	    directory from which files are to be checked
 *	filename	    name of JSON file in submission_dir
 *	sem		    != NULL ==> pointer to a JSON semantic table
 *			    NULL => do not perform any semantic table checks
 *
 * returns:
 *	true ==> all is OK
 *	false ==> something failed
 *
 * The file, .auth.json, under the submission_dir will opened, checked for proper JSON
 * syntax, and finally JSON semantic analysis will be performed.
 */
static bool
chk_json(FILE *err, char const *submission_dir, char const *filename, struct json_sem *sem)
{
    FILE *stream = NULL;				/* file stream for the JSON file or NULL */
    struct json *tree = NULL;				/* JSON parse tree or NULL ==> not parsed or invalid JSON */
    bool json_valid = false;				/* true ==> JSON is valid, false ==> JSON is invalid */
    struct dyn_array *count_err = NULL;			/* JSON semantic count errors */
    struct dyn_array *val_err = NULL;			/* JSON semantic validation errors */
    intmax_t all_err_count = 0;				/* number of errors (count+validation+internal) from semantic check */
    intmax_t count_err_count = 0;			/* semantic count error count */
    intmax_t int_err_count = 0;				/* internal error count */
    intmax_t val_err_count = 0;				/* semantic validation count */
    struct json_sem_count_err *sem_count_err = NULL;    /* semantic count error to print */
    struct json_sem_val_err *sem_val_err = NULL;        /* semantic validation error to print */
    intmax_t c;						/* dynamic array index and ignored lists iterator */

    /*
     * firewall
     */
    if (err == NULL) {
	fwarn(stderr, __func__, "err is NULL");
	return false;
    }
    if (submission_dir == NULL) {
	fwarn(err, __func__, "submission_dir is NULL");
	return false;
    }
    if (filename == NULL) {
	fwarn(err, __func__, "filename is NULL");
	return false;
    }

    /*
     * open the JSON file under submission_dir
     */
    stream = open_dir_file(submission_dir, filename);
    if (stream == NULL) {
	fwarn(err, __func__, "failed to open: %s", filename);
	return false;
    }

    /*
     * parse the JSON file and check for valid JSON syntax
     */
    tree = parse_json_stream(stream, filename, &json_valid);
    if (json_valid == false || tree == NULL) {
	fwarn(err, __func__, "file contains invalid JSON: %s", filename);
	return false;
    }

    /*
     * perform JSON semantic checks if requested
     */
    if (sem != NULL) {

	/*
	 * perform JSON semantic analysis on the .auth.json JSON parse tree
	 */
	all_err_count = json_sem_check(tree, JSON_DEFAULT_MAX_DEPTH, sem, &count_err, &val_err, NULL);

	/*
	 * firewall for semantic analysis
	 */
	if (count_err == NULL) {

	    /* report failure */
	    fwarn(err, __func__, "left count_err as NULL for JSON file: %s", filename);

	    /* free storage */
	    if (val_err == NULL) {
		free_count_err(val_err);
		val_err = NULL;
	    }
	    if (tree == NULL) {
		json_tree_free(tree, JSON_DEFAULT_MAX_DEPTH);
		tree = NULL;
	    }
	    return false;
	}
	if (val_err == NULL) {

	    /* report failure */
	    fwarn(err, __func__, "cleft val_err as NULL for JSON file: %s", filename);

	    /* free storage */
	    if (count_err != NULL) {
		free_count_err(count_err);
		count_err = NULL;
	    }
	    if (tree != NULL) {
		json_tree_free(tree, JSON_DEFAULT_MAX_DEPTH);
		tree = NULL;
	    }
	    return false;
	}

	/*
	 * determine semantic error count
	 */
	count_err_count = dyn_array_tell(count_err);
	val_err_count = dyn_array_tell(val_err);
	if (all_err_count < count_err_count + val_err_count) {

	    /* report failure */
	    fwarn(err, __func__, "all_err_count: %jd < count_err_count: %jd + val_err_count: %jd for JSON file: %s",
		       all_err_count, count_err_count, val_err_count, filename);

	    /* free storage */
	    if (count_err != NULL) {
		free_count_err(count_err);
		count_err = NULL;
	    }
	    if (val_err != NULL) {
		free_count_err(val_err);
		val_err = NULL;
	    }
	    if (tree != NULL) {
		json_tree_free(tree, JSON_DEFAULT_MAX_DEPTH);
		tree = NULL;
	    }
	    return false;
	}
	int_err_count = all_err_count - (count_err_count + val_err_count);

	/*
	 * report in details of the semantic analysis
	 */
	if (all_err_count > 0) {

	    /* report errors */
	    fpr(err, __func__, "follows are errors for JSON file: %s\n", filename);
	    for (c=0; c < count_err_count; ++c) {
		sem_count_err = dyn_array_addr(count_err, struct json_sem_count_err, c);
		fprint_count_err(err, "JSON semantic count error ", sem_count_err, "\n");
	    }
	    for (c=0; c < val_err_count; ++c) {
		sem_val_err = dyn_array_addr(val_err, struct json_sem_val_err, c);
		fprint_val_err(err, "JSON semantic validation error ", sem_val_err, "\n");
	    }
	    if (int_err_count > 0) {
		fpr(err, __func__, "%s internal JSON semantic error(s) found: %ju\n", filename, int_err_count);
	    }
	    fpr(err, __func__, "%s total JSON semantic error(s) found: %ju\n", filename, all_err_count);

	    /* free storage */
	    if (count_err != NULL) {
		free_count_err(count_err);
		count_err = NULL;
	    }
	    if (val_err != NULL) {
		free_count_err(val_err);
		val_err = NULL;
	    }
	    if (tree != NULL) {
		json_tree_free(tree, JSON_DEFAULT_MAX_DEPTH);
		tree = NULL;
	    }
	    return false;
	}

	/*
	 * free storage
	 */
	if (count_err != NULL) {
	    free_count_err(count_err);
	    count_err = NULL;
	}
	if (val_err != NULL) {
	    free_count_err(val_err);
	    val_err = NULL;
	}
    }
    if (tree != NULL) {
	json_tree_free(tree, JSON_DEFAULT_MAX_DEPTH);
	tree = NULL;
    }
    dbg(DBG_LOW, "JSON file is OK: %s", filename);
    return true;
}
