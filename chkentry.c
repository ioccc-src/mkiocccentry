/*
 * chkentry - check JSON files in an IOCCC submission
 *
 * "Because grammar and syntax alone do not make a complete language." :-)
 *
 * This tool and the JSON parser were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *	@xexyl
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

/*
 * chkentry - check JSON files in an IOCCC submission
 */
#include "chkentry.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */
#define CHUNK (39)              /* allocate CHUNK elements at a time */


/*
 * globals
 */
static bool quiet = false;		    /* true ==> quiet mode */
static bool ignore_info = false;            /* true ==> skip .info.json checks */
static bool ignore_auth = false;            /* true ==> skip .auth.json checks */
static bool ignore_entry = false;           /* true ==> skip .entry.json checks */
static bool ignore_README_md = false;           /* true ==> skip README.md checks */
static bool ignore_index = false;           /* true ==> skip index.html checks */
static bool ignore_Makefile = false;         /* true ==> skip Makefile checks */
static bool ignore_prog_c = false;         /* true ==> skip prog.c checks */
static bool ignore_prog = false;         /* true ==> skip prog checks */
static bool ignore_remarks_md = false;         /* true ==> skip remarks.md checks */


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] [-i file] auth.json info.json\n"
    "\n"
    "\t-h\t\tprint help message and exit\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit\n"
    "\t-q\t\tquiet mode (def: loud :-) )\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-i file\t\tadd file to ignore list\n"
    "\t-w\t\twinning entry checks\n"
    "\tauth.json\tcheck auth.json file, . ==> skip IOCCC .auth.json style check\n"
    "\tinfo.json\tcheck info.json file, . ==> skip IOCCC .info.json style check\n"
    "\n"
    "Exit codes:\n"
    "    0\t\tall is OK\n"
    "    1\t\tJSON files(s) are valid JSON but one or more semantic checks failed\n"
    "    2\t\t-h and help string printed or -V and version string printed\n"
    "    3\t\tcommand line error\n"
    "    4\t\tfile(s) not valid JSON; no semantic checks were performed\n"
    "    >=10\tinternal error\n"
    "\n"
    "%s version: %s\n"
    "jparse utils version: %s\n"
    "jparse UTF-8 version: %s\n"
    "jparse library version: %s";

/*
 * NOTE: this MUST end with { NULL, NULL }!
 */
char *abbrevs[][2] =
{
    { "auth"        , AUTH_JSON_FILENAME    },
    { "info"        , INFO_JSON_FILENAME    },
    { "prog.c"      , PROG_C_FILENAME       },
    { "Makefile"    , MAKEFILE_FILENAME     },
    { "try"         , TRY_SH                },
    { "try.alt"     , TRY_ALT_SH            },
    { "remarks"     , REMARKS_FILENAME      },
    { "README"      , README_MD_FILENAME    },
    { "index"       , INDEX_HTML_FILENAME   },
    { "prog"        , PROG_FILENAME         },
    { "prog.orig.c" , PROG_ORIG_C_FILENAME  },
    { "prog.orig"   , PROG_ORIG_FILENAME    },
    { "entry"       , ENTRY_JSON_FILENAME   },
    { NULL          , NULL                  } /* MUST be last! */
};

/*
 * functions
 */
static void usage(int exitcode, char const *prog, char const *str) __attribute__((noreturn));
static void add_ignore_path(char *path);

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
 * add_ignore_path - add path to ignored paths list
 *
 * This function is used instead of just append_path() (although it uses this)
 * because some names have an abbreviation.
 *
 * given:
 *
 *      path    - path to add to ignore list
 *
 * NOTE: for some paths abbreviations are allowed. However as it is impossible
 * to know all the possible paths in a submission/entry, in those cases it must
 * be the exact path.
 *
 * NOTE: these paths are from UNDER the topdir. So if one has the file:
 *
 *      topdir/foo/bar/baz
 *
 * and they run the command:
 *
 *      chkentry topdir
 *
 * and want to ignore the path baz they should NOT do:
 *
 *      chkentry -i topdir/foo/bar/baz topdir
 *
 * Rather they should do:
 *
 *      chkentry -i foo/bar/baz topdir
 *
 * NOTE: this function uses the static char *abbrevs[][2]  to find abbreviations.
 * In the case nothing is found then the path will be added to the list as is.
 *
 * NOTE: this function will not add a path to the array more than once so if you
 * were to do:
 *
 *      chkentry -i foo -i foo topdir
 *
 * the ignored list would only have the path 'foo' once.
 */
static void
add_ignore_path(char *path)
{
    size_t i = 0;

    if (path == NULL || *path == '\0') {
        /*
         * ignore NULL path or empty string
         */
        return;
    }
    for (i = 0; abbrevs[i][0] != NULL; ++i) {
        if (!strcmp(abbrevs[i][0], path)) {
            /*
             * we have to determine if this is the .auth.json file to ignore
             */
            if (!strcmp(path, "auth")) {
                if (ignore_auth) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_auth = true;
            }

            /*
             * we also have to check for .info.json
             */
            else if (!strcmp(path, "info")) {
                if (ignore_info) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_info = true;
            }
            /*
             * we also need to record if .entry.json is here
             */
            else if (!strcmp(path, "entry")) {
                if (ignore_entry) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_entry = true;
            }
            /*
             * remarks.md
             */
            else if (!strcmp(path, "remarks")) {
                if (ignore_remarks_md) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_remarks_md = true;
            }
            /*
             * README.md
             */
            else if (!strcmp(path, "README")) {
                if (ignore_README_md) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_README_md = true;
            }
            /*
             * prog
             */
            else if (!strcmp(path, PROG_FILENAME)) {
                if (ignore_prog) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_prog = true;
            }
            /*
             * prog.c
             */
            else if (!strcmp(path, PROG_C_FILENAME)) {
                if (ignore_prog_c) {
                    /*
                     * skip
                     */
                     return;
                }
                ignore_prog_c = true;
            }
            else if (!strcmp(path, MAKEFILE_FILENAME)) {
                if (ignore_Makefile) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_Makefile = true;
            }
            else if (!strcmp(path, "index")) {
                if (ignore_index) {
                    /*
                     * skip
                     */
                    return;
                }
                ignore_index = true;
            }

            /* we can append the file now */
            append_path(&ignored_paths, abbrevs[i][1], true, false, false);
            return;
        }
    }

    /*
     * we have to determine if this is a certain file
     */
    if (!strcmp(path, AUTH_JSON_FILENAME)) {
        if (ignore_auth) {
            /*
             * skip
             */
            return;
        }
        ignore_auth = true;
    }

    /*
     * we also have to check for .info.json
     */
    else if (!strcmp(path, INFO_JSON_FILENAME)) {
        if (ignore_info) {
            /*
             * skip
             */
            return;
        }
        ignore_info = true;
    }

    /*
     * we also have to check for .entry.json
     */
    else if (!strcmp(path, ENTRY_JSON_FILENAME)) {
        if (ignore_entry) {
            /*
             * skip
             */
            return;
        }
        ignore_entry = true;
    }
    /*
     * we need to check for README.md
     */
    else if (!strcmp(path, README_MD_FILENAME)) {
        if (ignore_README_md) {
            /*
             * skip
             */
            return;
        }
        ignore_README_md = true;
    }
    /*
     * index.html
     */
    else if (!strcmp(path, INDEX_HTML_FILENAME)) {
        if (ignore_index) {
            /*
             * skip
             */
            return;
        }
        ignore_index = true;
    }
    /*
     * Makefile
     */
    else if (!strcmp(path, MAKEFILE_FILENAME)) {
        if (ignore_Makefile) {
            /*
             * skip
             */
            return;
        }
        ignore_Makefile = true;
    }
    /*
     * prog.c
     */
    else if (!strcmp(path, PROG_C_FILENAME)) {
        if (ignore_prog_c) {
            /*
             * skip
             */
            return;
        }
        ignore_prog_c = true;
    }
    /*
     * prog
     */
    else if (!strcmp(path, PROG_FILENAME)) {
        if (ignore_prog) {
            /*
             * skip
             */
            return;
        }
        ignore_prog = true;
    }
    /*
     * remarks.md
     */
    else if (!strcmp(path, REMARKS_FILENAME)) {
        if (ignore_remarks_md) {
            /*
             * skip
             */
            return;
        }
        ignore_remarks_md = true;
    }

    /*
     * this will not add the path if it is already there
     */
    append_path(&ignored_paths, path, true, false, false);
}

int
main(int argc, char *argv[])
{
    char const *program = NULL;		/* our name */
    char **topdir = NULL;               /* directory from which files are to be checked */
    extern char *optarg;		/* option argument */
    extern int optind;			/* argv index of the next arg */
    char const *auth_filename = ".";	/* .auth.json file to process, or NULL ==> no .auth.json to process */
    char const *info_filename = ".";	/* .info.json file to process, or NULL ==> no .info.json to process */
    char *auth_path = NULL;		/* full path of .auth.json or NULL */
    char *info_path = NULL;		/* full path of .info.json or NULL */
    FILE *auth_stream = NULL;		/* open .auth.json file stream */
    FILE *info_stream = NULL;		/* open .info.json file stream */
    struct json *auth_tree = NULL;	/* JSON parse tree for .auth.json, or NULL ==> not parsed */
    struct json *info_tree = NULL;	/* JSON parse tree for .info.json, or NULL ==> not parsed */
    bool auth_valid = false;		/* .auth.json is valid JSON */
    bool info_valid = false;		/* .info.json is valid JSON */
    struct dyn_array *auth_count_err = NULL;	/* JSON semantic count errors for .auth.json */
    struct dyn_array *auth_val_err = NULL;	/* JSON semantic validation errors for .auth.json */
    struct dyn_array *info_count_err = NULL;	/* JSON semantic count errors for .info.json */
    struct dyn_array *info_val_err = NULL;	/* JSON semantic validation errors for .info.json */
    uintmax_t auth_count_err_count = 0;	/* semantic count error count from json_sem_check() for .auth.json */
    uintmax_t auth_val_err_count = 0;	/* semantic validation count from json_sem_check() for .auth.json */
    uintmax_t auth_int_err_count = 0;	/* internal error count from json_sem_check() for .auth.json */
    uintmax_t auth_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .auth.json */
    uintmax_t info_count_err_count = 0;	/* semantic count error count from json_sem_check() for .info.json */
    uintmax_t info_val_err_count = 0;	/* semantic validation error count from json_sem_check() for .info.json */
    uintmax_t info_int_err_count = 0;	/* internal error count from json_sem_check() for .info.json */
    uintmax_t info_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .info.json */
    uintmax_t all_count_err_count = 0;	/* semantic count error count from json_sem_check() for .auth.json */
    uintmax_t all_val_err_count = 0;	/* semantic validation error count from json_sem_check() for .auth.json */
    uintmax_t all_int_err_count = 0;	/* internal error count from json_sem_check() for .auth.json */
    uintmax_t all_all_err_count = 0;	/* number of errors (count+validation+internal) from json_sem_check() for .auth.json */
    uintmax_t all_extra_err_count = 0;  /* depending on options and conditions, we might need this */
    struct json_sem_count_err *sem_count_err = NULL;	/* semantic count error to print */
    struct json_sem_val_err *sem_val_err = NULL;	/* semantic validation error to print */
    char *path = NULL;                  /* specific checks depending on the option (like -i entry/-i .entry.json) */
    uintmax_t c;			/* dynamic array index */
    int cwd = -1;                       /* for when we have to use find_path() */
    int cwd2 = -1;                      /* cwd of program when we start */
    struct fts fts;                     /* for when we have to use find_path() */
    int i;
    bool winning_entry_mode = false;    /* true ==> -w used, do other checks */


    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, ":hv:J:Vqi:w")) != -1) {
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
        case 'i':
            add_ignore_path(optarg);
            break;
        case 'w':
            winning_entry_mode = true;
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
        topdir = argv;
	break;
    case 2:
        if (!strcmp(argv[0], ".") && !strcmp(argv[1], ".")) {
            vrergfB(-1, -1); /* Easter egg */
            not_reached();
        }
        /*fallthrough*/
    default:
	usage(3, program, "wrong number of arguments");	/*ooo*/
	not_reached();
	break;
    }

    if (topdir != NULL && *topdir != NULL) {
	/*
	 * check if we can search / work within the directory
	 */
	if (!exists(*topdir)) {
	    err(39, __func__, "directory does not exist: %s", *topdir);
	    not_reached();
	}
	if (!is_dir(*topdir)) {
	    err(40, __func__, "is not a directory: %s", *topdir);
	    not_reached();
	}
	if (!is_exec(*topdir)) {
	    err(41, __func__, "directory is not searchable: %s", *topdir);
	    not_reached();
	}

        /*
         * note the current directory so we can restore it later if necessary
         */
        errno = 0;			/* pre-clear errno for errp() */
        cwd2 = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        if (cwd2 < 0) {
            errp(42, __func__, "cannot open .");
            not_reached();
        }
        /*
         * specific checks require the FTS API
         */
        reset_fts(&fts);
        fts.depth = 1; /* we need depth 1 only */
        fts.match_case = true; /* we must need to match case */
        fts.type = FTS_TYPE_FILE; /* regular files only */
        if (winning_entry_mode) {
            /*
             * make sure .entry.json exists unless -i entry
             */
            if (!ignore_entry) {
                path = find_path(ENTRY_JSON_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -w used and -i entry not used", ENTRY_JSON_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }

            /*
             * unless -i auth .auth.json must NOT exist
             */
            if (!ignore_auth) {
                path = find_path(AUTH_JSON_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -w used and -i auth not used", AUTH_JSON_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i info .info.json must NOT exist
             */
            if (!ignore_info) {
                /*
                 * .info.json
                 */
                path = find_path(INFO_JSON_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -w used and -i info not used", INFO_JSON_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i README README.md MUST exist
             */
            if (!ignore_README_md) {
                /*
                 * README.md
                 */
                path = find_path(README_MD_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -w used and -i README not used", README_MD_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i index index.html MUST exist
             */
            if (!ignore_index) {
                /*
                 * index.html
                 */
                path = find_path(INDEX_HTML_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -w used and -i index not used", INDEX_HTML_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }

            /*
             * unless -i Makefile Makefile MUST exist
             */
            if (!ignore_Makefile) {
                /*
                 * Makefile
                 */
                path = find_path(MAKEFILE_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -w used and -i Makefile not used", MAKEFILE_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i prog.c prog.c MUST exist
             */
            if (!ignore_prog_c) {
                /*
                 * prog.c
                 */
                path = find_path(PROG_C_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -w used and -i prog.c not used", PROG_C_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i prog prog must NOT exist
             */
            if (!ignore_prog) {
                /*
                 * prog
                 */
                path = find_path(PROG_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -w used and -i prog not used", PROG_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i remarks remarks.md must NOT exist
             */
            if (!ignore_remarks_md) {
                /*
                 * remarks.md
                 */
                path = find_path(REMARKS_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -w used and -i remarks not used", REMARKS_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
        } else {
            /*
             * case: -w NOT used
             */

            /*
             * additional checks: we have to verify the file permissions are correct
             * of files if not ignored. Yes this does mean there is some duplicated
             * obtaining of paths. That also happens when -i info is not used. This
             * could be said to be a mis-feature or it could be said to be defence
             * in depth. You may choose your poison, keeping in mind that:
             *
             *      osis sola facit venenum
             *
             *      -- Paracelsus
             *
             *  :-)
             */

            /*
             * make sure .entry.json does NOT exist unless -i entry
             */
            if (!ignore_entry) {
                path = find_path(ENTRY_JSON_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -w and -i entry not used and ", ENTRY_JSON_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }

            /*
             * unless -i auth .auth.json MUST exist
             */
            if (!ignore_auth) {
                path = find_path(AUTH_JSON_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i auth not used", AUTH_JSON_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i info .info.json MUST exist
             */
            if (!ignore_info) {
                /*
                 * .info.json
                 */
                path = find_path(INFO_JSON_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i info not used", INFO_JSON_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i README README.md must NOT exist
             */
            if (!ignore_README_md) {
                /*
                 * README.md
                 */
                path = find_path(README_MD_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -i README not used", README_MD_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i index index.html must NOT exist
             */
            if (!ignore_index) {
                /*
                 * index.html
                 */
                path = find_path(INDEX_HTML_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -i index not used", INDEX_HTML_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }

            /*
             * unless -i Makefile Makefile MUST exist
             */
            if (!ignore_Makefile) {
                /*
                 * Makefile
                 */
                path = find_path(MAKEFILE_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i Makefile not used", MAKEFILE_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i prog.c prog.c MUST exist
             */
            if (!ignore_prog_c) {
                /*
                 * prog.c
                 */
                path = find_path(PROG_C_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i prog.c not used", PROG_C_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i prog prog must NOT exist
             */
            if (!ignore_prog) {
                /*
                 * prog
                 */
                path = find_path(PROG_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path != NULL) {
                    dbg(DBG_LOW, "%s exists and -i prog not used", PROG_FILENAME);
                    ++all_extra_err_count;
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
            /*
             * unless -i remarks remarks.md MUST exist
             */
            if (!ignore_remarks_md) {
                /*
                 * remarks.md
                 */
                path = find_path(REMARKS_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i remarks not used", REMARKS_FILENAME);
                    ++all_extra_err_count;
                } else {
                    /*
                     * now check the permissions are correct
                     */
                    if (check_manifest_path(path, path, S_IRUSR | S_IRGRP | S_IROTH) != MAN_PATH_OK) {
                        ++all_extra_err_count;
                    }
                    free(path);
                    path = NULL;
                }

                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }
        }

        /*
         * open the .auth.json file under topdir if !ignore_auth && -w not used
         */
        if (!ignore_auth && !winning_entry_mode) {
            auth_filename = ".auth.json";
            auth_stream = open_dir_file(*topdir, auth_filename);
            if (auth_stream == NULL) { /* paranoia */
                err(43, __func__, "auth_stream = open_dir_file(%s, %s) returned NULL", *topdir, auth_filename);
                not_reached();
            }
            auth_path = calloc_path(*topdir, auth_filename);
            if (auth_path == NULL) {
                err(44, __func__, "auth_path is NULL");
                not_reached();
            }
        }

        /*
         * open the .info.json file under topdir if !ignore_info and -w not used
         */
        if (!ignore_info && !winning_entry_mode) {
            info_filename = ".info.json";
            info_stream = open_dir_file(*topdir, info_filename);
            if (info_stream == NULL) { /* paranoia */
                err(45, __func__, "info_stream = open_dir_file(%s, %s) returned NULL", *topdir, info_filename);
                not_reached();
            }
            info_path = calloc_path(*topdir, info_filename);
            if (info_path == NULL) {
                err(46, __func__, "info_path is NULL");
                not_reached();
            }
        }

        /*
         * parse .auth.json if it is open
         */
        if (auth_stream != NULL && auth_path != NULL) {
            auth_tree = parse_json_stream(auth_stream, auth_path, &auth_valid);
            if (auth_valid == false || auth_tree == NULL) {
                err(4, __func__, "failed to parse JSON in .auth.json file: %s", auth_path); /*ooo*/
                not_reached();
            }
            dbg(DBG_LOW, "successful parse of JSON in .auth.json file: %s", auth_path);
        }

        /*
         * parse .info.json if it is open
         */
        if (info_stream != NULL && info_path != NULL) {
            info_tree = parse_json_stream(info_stream, info_path, &info_valid);
            if (info_valid == false || info_tree == NULL) {
                err(4, __func__, "failed to parse JSON in .info.json file: %s", info_path); /*ooo*/
                not_reached();
            }
            dbg(DBG_LOW, "successful parse of JSON in .info.json file: %s", info_path);
        }

        /*
         * check a JSON parse tree against a JSON semantic table for .auth.json, if open
         */
        if (auth_stream != NULL && auth_path != NULL) {

            /*
             * perform JSON semantic analysis on the .auth.json JSON parse tree
             */
            dbg(DBG_HIGH, "about to perform JSON semantic check for .auth.json file: %s", auth_path);
            auth_all_err_count = json_sem_check(auth_tree, JSON_DEFAULT_MAX_DEPTH, sem_auth,
                                              &auth_count_err, &auth_val_err, NULL);

            /*
             * firewall on json_sem_check() results AND count errors for .auth.json
             */
            if (auth_count_err == NULL) {
                err(47, __func__, "json_sem_check() left auth_count_err as NULL for .auth.json file: %s", auth_path);
                not_reached();
            }
            if (dyn_array_tell(auth_count_err) < 0) {
                err(48, __func__, "dyn_array_tell(auth_count_err): %jd < 0 "
                       "for .auth.json file: %s", dyn_array_tell(auth_count_err), auth_path);
                not_reached();
            }
            auth_count_err_count = (uintmax_t) dyn_array_tell(auth_count_err);
            if (auth_val_err == NULL) {
                err(49, __func__, "json_sem_check() left auth_val_err as NULL for .auth.json file: %s", auth_path);
                not_reached();
            }
            if (dyn_array_tell(auth_val_err) < 0) {
                err(50, __func__, "dyn_array_tell(auth_val_err): %jd < 0 "
                       "for .auth.json file: %s", dyn_array_tell(auth_val_err), auth_path);
                not_reached();
            }
            auth_val_err_count = (uintmax_t)dyn_array_tell(auth_val_err);
            if (auth_all_err_count < auth_count_err_count+auth_val_err_count) {
                err(51, __func__, "auth_all_err_count: %ju < auth_count_err_count: %ju + auth_val_err_count: %ju "
                       "for .auth.json file: %s",
                       auth_all_err_count, auth_count_err_count, auth_val_err_count, auth_path);
                not_reached();
            }
            auth_int_err_count = auth_all_err_count - (auth_count_err_count+auth_val_err_count);
        }

        /*
         * check a JSON parse tree against a JSON semantic table for .info.json, if open
         */
        if (info_stream != NULL && info_path != NULL) {

            /*
             * perform JSON semantic analysis on the .info.json JSON parse tree
             */
            dbg(DBG_HIGH, "about to perform JSON semantic check for .info.json file: %s", info_path);
            info_all_err_count = json_sem_check(info_tree, JSON_DEFAULT_MAX_DEPTH, sem_info,
                                              &info_count_err, &info_val_err, *topdir);

            /*
             * firewall on json_sem_check() results AND count errors for .info.json
             */
            if (info_count_err == NULL) {
                err(52, __func__, "json_sem_check() left info_count_err as NULL for .info.json file: %s", info_path);
                not_reached();
            }
            if (dyn_array_tell(info_count_err) < 0) {
                err(53, __func__, "dyn_array_tell(info_count_err): %jd < 0 "
                       "for .info.json file: %s",
                       dyn_array_tell(info_count_err), info_path);
                not_reached();
            }
            info_count_err_count = (uintmax_t)dyn_array_tell(info_count_err);
            if (info_val_err == NULL) {
                err(54, __func__, "json_sem_check() left info_val_err as NULL for .info.json file: %s", info_path);
                not_reached();
            }
            if (dyn_array_tell(info_val_err) < 0) {
                err(55, __func__, "dyn_array_tell(info_val_err): %jd < 0 "
                       "for .info.json file: %ss",
                       dyn_array_tell(info_val_err), info_path);
                not_reached();
            }
            info_val_err_count = (uintmax_t)dyn_array_tell(info_val_err);
            if (info_all_err_count < info_count_err_count+info_val_err_count) {
                err(56, __func__, "info_all_err_count: %ju < info_count_err_count: %ju + info_val_err_count: %ju "
                       "for .info.json file: %s",
                       info_all_err_count, info_count_err_count, info_val_err_count, info_path);
                not_reached();
            }
            info_int_err_count = info_all_err_count - (info_count_err_count+info_val_err_count);
        }

        /*
         * count all errors
         */
        all_count_err_count = info_count_err_count + auth_count_err_count;
        all_val_err_count = info_val_err_count + auth_val_err_count;
        all_int_err_count = info_int_err_count + auth_int_err_count;
        all_all_err_count = info_all_err_count + auth_all_err_count;

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
         * report on semantic count errors
         */
        if (all_count_err_count > 0) {
            if (auth_count_err_count > 0) {
                dbg(DBG_LOW, "count errors for .auth.json: %ju", auth_count_err_count);
            }
            if (info_count_err_count > 0) {
                dbg(DBG_LOW, "count errors for .info.json: %ju", info_count_err_count);
            }
            dbg(DBG_LOW, "count errors for both files: %ju", all_count_err_count);
        }

        /*
         * report on semantic validation errors
         */
        if (all_val_err_count > 0) {
            if (auth_val_err_count > 0) {
                dbg(DBG_LOW, "validation errors for .auth.json: %ju", auth_val_err_count);
            }
            if (info_val_err_count > 0) {
                dbg(DBG_LOW, "validation errors for .info.json: %ju", info_val_err_count);
            }
            dbg(DBG_LOW, "validation errors for both files: %ju", all_val_err_count);
        }

        /*
         * report on internal errors
         */
        if (all_int_err_count > 0) {
            if (auth_int_err_count > 0) {
                dbg(DBG_LOW, "internal errors for .auth.json: %ju", auth_int_err_count);
            }
            if (info_int_err_count > 0) {
                dbg(DBG_LOW, "internal errors for .info.json: %ju", info_int_err_count);
            }
            dbg(DBG_LOW, "internal errors for both files: %ju", all_int_err_count);
        }

        /*
         * report on total error counts
         */
        if (all_all_err_count > 0) {
            if (auth_all_err_count > 0) {
                dbg(DBG_LOW, "total semantic errors for .auth.json: %ju", auth_all_err_count);
            }
            if (info_all_err_count > 0) {
                dbg(DBG_LOW, "total semantic errors for .info.json: %ju", info_all_err_count);
            }
            dbg(DBG_LOW, "total semantic errors for both files: %ju", all_all_err_count);
        }

        /*
         * summarize the JSON semantic check status
         */
        if (auth_all_err_count > 0) {
            if (auth_path == NULL) {
                werr(1, __func__, "JSON semantic check failed for .auth.json file: ((NULL))"); /*ooo*/
            } else {
                werr(1, __func__, "JSON semantic check failed for .auth.json file: %s", auth_path); /*ooo*/
            }
        }
        if (info_all_err_count > 0) {
            if (info_path == NULL) {
                werr(1, __func__, "JSON semantic check failed for .info.json file: ((NULL))"); /*ooo*/
            } else {
                werr(1, __func__, "JSON semantic check failed for .info.json file: %s", info_path); /*ooo*/
            }
        }
        if (all_all_err_count == 0) {
            dbg(DBG_LOW, "JSON semantic check OK");
        }
        /*
         * before we continue, to be on the safe side, we will fchdir(2) to the
         * original directory. We might be able to determine where we are but
         * depending on the options and what or what not failed above, we cannot
         * be sure where we are.
         */
        errno = 0; /* pre-clear errno for errp() */
        if (fchdir(cwd2) != 0) {
            errp(57, __func__, "failed to change back to original directory");
            not_reached();
        }

        if (paths_in_array(ignored_paths) > 0) {
            /*
             * specific checks require the FTS API
             */
            reset_fts(&fts);
            fts.depth = 1; /* we need depth 1 only */
            fts.match_case = true; /* we must need to match case */
            /*
             * if the ignored_paths has .entry.json in it then the file
             * MUST exist
             */
            if (array_has_path(ignored_paths, ENTRY_JSON_FILENAME, true, NULL)) {
                /*
                 * make sure .entry.json exists
                 */
                path = find_path(ENTRY_JSON_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i entry used", ENTRY_JSON_FILENAME);
                    ++all_extra_err_count;
                } else {
                    free(path);
                    path = NULL;
                }
                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }

            /*
             * if the ignored_paths has README.md in it then the file
             * MUST exist
             */
            if (array_has_path(ignored_paths, README_MD_FILENAME, true, NULL)) {
                /*
                 * make sure README.md exists
                 */
                path = find_path(README_MD_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i README used", README_MD_FILENAME);
                    ++all_extra_err_count;
                } else {
                    free(path);
                    path = NULL;
                }
                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }

            /*
             * if the ignored_paths has index.html in it then the file
             * MUST exist
             */
            if (array_has_path(ignored_paths, INDEX_HTML_FILENAME, true, NULL)) {
                /*
                 * make sure index.html exists
                 */
                path = find_path(INDEX_HTML_FILENAME, *topdir, -1, &cwd, false, &fts);
                if (path == NULL) {
                    dbg(DBG_LOW, "%s does not exist and -i index used", INDEX_HTML_FILENAME);
                    ++all_extra_err_count;
                } else {
                    free(path);
                    path = NULL;
                }
                /*
                 * move back to previous directory
                 */
                (void) read_fts(NULL, -1, &cwd, NULL);
            }

        }
        /*
         * cleanup - except for info_stream and auth_stream.
         *
         * We don't try closing info_stream or auth_stream because the json parser
         * already does that at this point. This is because we no longer use yyin
         * due to complications introduced when making the lexer re-entrant. This is
         * not a problem under macOS but it is under linux.
         */
        if (auth_tree != NULL) {
            json_tree_free(auth_tree, JSON_DEFAULT_MAX_DEPTH);
            auth_tree = NULL;
        }
        if (info_tree != NULL) {
            json_tree_free(info_tree, JSON_DEFAULT_MAX_DEPTH);
            info_tree = NULL;
        }
        if (auth_count_err != NULL) {
            free_count_err(auth_count_err);
            auth_count_err = NULL;
        }
        if (auth_val_err != NULL) {
            free_val_err(auth_val_err);
            auth_val_err = NULL;
        }
        if (info_count_err != NULL) {
            free_count_err(info_count_err);
            info_count_err = NULL;
        }
        if (info_val_err != NULL) {
            free_val_err(info_val_err);
            info_val_err = NULL;
        }
        if (auth_path != NULL) {
            free(auth_path);
            auth_path = NULL;
        }
        if (info_path != NULL) {
            free(info_path);
            info_path = NULL;
        }
        if (ignored_paths != NULL) {
            free_paths_array(&ignored_paths, false);
            ignored_paths = NULL;
        }

        /*
         * make sure we're in the original directory
         */
        errno = 0; /* pre-clear errno for errp() */
        if (fchdir(cwd2) != 0) {
            errp(58, __func__, "failed to change back to previous directory");
            not_reached();
        }
        /*
         * close FD
         *
         * NOTE: read_fts() will have already closed cwd so we don't need to do
         * that
         */
        errno = 0; /* pre-clear errno for errp */
        if (close(cwd2) != 0) {
            errp(59, __func__, "failed to close original directory FD");
            not_reached();
        }
    } else {
	usage(3, program, "invalid command line");/*ooo*/
    }

    /*
     * All Done!!! All Done!!! -- Jessica Noll, Age 2
     *
     */
    if (all_all_err_count > 0) {
	err(1, __func__, "JSON semantic check or file checks failed"); /*ooo*/
	not_reached();
    }
    if (all_extra_err_count > 0) {
        err(1, __func__, "missing files or files that should not exist found in topdir");/*ooo*/
        not_reached();
    }
    exit(0); /*ooo*/
}
