/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
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
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll
 * and the concept of this file was developed by Landon Curt Noll.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h> /* for open() */

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

/*
 * verify_entry_dir	- verify entry_dir
 *
 * Verify we can cd into the directory and that the proper files exist in that
 * directory.
 *
 * given:
 *
 *	entry_dir   - the entry directory to check files under
 *	info_json   - if != NULL the .info.json file to check
 *	author_json - if != NULL the .author.json file to check
 *
 * NOTE: It's expected that chkentry_sanity_chks() will call this function if
 * entry_dir != NULL and is a directory but this code is preliminary so it might
 * be modified or reused in some way later on.
 *
 * XXX If the directory is NULL this is okay as long as the files are
 * readable in the current working directory. However the fact this function is
 * not processing the files but just verifying they exist makes the function
 * problematic. This is because although the purpose is to prevent code
 * duplication in the chkentry_sanity_chks() function it will actually result in
 * code duplication elsewhere because we have to use chdir(2) to change to the
 * directory simply for verifying the files exist - and then later we have to do
 * that again in order to actually process the files. However since the
 * functions that do this do not yet exist we can get by with this for now.
 * Possible fixes would be to have the changing directory code in a separate
 * function and then having both functions make use of that (possibly another
 * function to return to the previous directory?); or we could have the check
 * for the files existing right before processing them i.e. removing the check
 * for files from the sanity check function. These questions can be answered at
 * a later time.
 *
 * NOTE: If entry_dir != NULL it is expected that info_json and author_json ARE
 * NULL and we will directly name them _after_ changing into the directory.
 *
 * This function does not return on error or if files are not readable.
 */
static void
verify_entry_dir(char const *entry_dir, char const *info_json, char const *author_json)
{
    int cwd;			/* current working directory */
    int ret;
    bool switched_dir = false;	/* if we used chdir(2) */

    /*
     * firewall
     */
    if (entry_dir == NULL && info_json == NULL && author_json == NULL)
    {
	err(30, __func__, "called with NULL arg(s)");
	not_reached();
    }
    else if (entry_dir != NULL)
    {
	if (!is_dir(entry_dir))
	{
	    err(31, __func__, "called on path that is not a directory");
	    not_reached();
	}
	/*
	 * note the current directory so we can restore it later, after the
	 * chdir(entry_dir) below
	 */
	errno = 0;			/* pre-clear errno for errp() */
	cwd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
	if (cwd < 0) {
	    errp(32, __func__, "cannot open .");
	    not_reached();
	}
	/*
	 * cd into the entry_dir to check that the .info.json and .author.json
	 * files exist.
	 *
	 * XXX Once again this will result in code duplication unless this part
	 * is moved to another function or the processing directory function
	 * (which does not exist yet) does the checks on files and not the
	 * sanity check functions.
	 */
	errno = 0;			/* pre-clear errno for errp() */
	ret = chdir(entry_dir);
	if (ret < 0) {
	    errp(33, __func__, "cannot cd %s", entry_dir);
	    not_reached();
	}

	switched_dir = true;

	/*
	 * set the filenames so that the tests below can work whether or not we
	 * check in a directory or direct paths. If user specifies "." it will
	 * be skipped because we only update the strings if the directory is not
	 * NULL (and we could cd into it) which at that point the filenames are
	 * NULL.
	 */

	info_json = ".info.json";
	author_json = ".author.json";
    }

    if (info_json != NULL && strcmp(info_json, "."))
    {
	if (!exists(info_json))
	{
	    fpara(stderr,
		  "",
		  "The directory does not have a .info.json file."
		  "Please check the path and try again."
		  "",
		  "    chkentry [options] entry_dir",
		  "    chkentry [options] info.json author.json"
		  "",
		  NULL);
	    err(34, __func__, "info.json does not exist: %s%s%s", entry_dir!=NULL?entry_dir:"",entry_dir!=NULL?"/":"", info_json);
	    not_reached();
	}
	if (!is_file(info_json))
	{
	    fpara(stderr,
		  "",
		  "The .info.json, while it exists, is not a regular file.",
		  "",
		  "Perhaps you need to use another path:",
		  "",
		  "    chkentry [options] entry_dir",
		  "    chkentry [options] info.json author.json"
		  "",
		  NULL);
	    err(35, __func__, "info.json is not a regular file: %s%s%s", entry_dir!=NULL?entry_dir:"",entry_dir!=NULL?"/":"", info_json);
	    not_reached();
	}
	if (!is_read(info_json))
	{
	    fpara(stderr,
		  "",
		  "The info.json, while it is a file, is not readable.",
		  "",
		  "We suggest you check the permissions on the path or use another path:",
		  "",
		  "    chkentry [options] entry_dir",
		  "    chkentry [options] info.json author.json"
		  "",
		  NULL);
	    err(36, __func__, "info.json is not readable: %s%s%s", entry_dir!=NULL?entry_dir:"",entry_dir!=NULL?"/":"", info_json);
	    not_reached();
	}
    }

    if (author_json != NULL && strcmp(author_json, "."))
    {
	/* first .author.json */
	if (!exists(author_json))
	{
	    fpara(stderr,
		  "",
		  "The directory does not have a .author.json file.",
		  "Please check the path and try again."
		  "",
		  "    chkentry [options] entry_dir",
		  "    chkentry [options] info.json author.json"
		  "",
		  NULL);
	    err(37, __func__, "author.json does not exist: %s%s%s", entry_dir!=NULL?entry_dir:"",entry_dir!=NULL?"/":"", author_json);
	    not_reached();
	}
	if (!is_file(author_json))
	{
	    fpara(stderr,
		  "",
		  "The .author.json, while it exists, is not a regular file.",
		  "",
		  "Perhaps you need to use another path:",
		  "",
		  "    chkentry [options] entry_dir",
		  "    chkentry [options] info.json author.json"
		  "",
		  NULL);
	    err(38, __func__, "author.json does is not a regular file: %s%s%s", entry_dir!=NULL?entry_dir:"",entry_dir!=NULL?"/":"", author_json);
	    not_reached();
	}
	if (!is_read(author_json))
	{
	    fpara(stderr,
		  "",
		  "The author.json, while it is a file, is not readable.",
		  "",
		  "We suggest you check the permissions on the path or use another path:",
		  "",
		  "    chkentry [options] entry_dir",
		  "    chkentry [options] info.json author.json"
		  "",
		  NULL);
	    err(39, __func__, "author.json does is not readable: %s%s%s", entry_dir!=NULL?entry_dir:"",entry_dir!=NULL?"/":"", author_json);
	    not_reached();
	}
    }

    if (entry_dir != NULL && switched_dir)
    {
	/*
	 * switch back to the previous current directory if we changed
	 * directories
	 */
	errno = 0;			/* pre-clear errno for errp() */
	ret = fchdir(cwd);
	if (ret < 0) {
	    errp(40, __func__, "cannot fchdir to the previous current directory");
	    not_reached();
	}
	errno = 0;			/* pre-clear errno for errp() */
	ret = close(cwd);
	if (ret < 0) {
	    errp(41, __func__, "close of previous current directory failed");
	    not_reached();
	}

    }
}
/*
 * chkentry_sanity_chks - perform basic sanity checks
 *
 * We perform basic sanity checks on paths as well as some of the IOCCC tables.
 *
 * given:
 *
 *	entry_dir	- if != NULL the entry directory with .info.json and .author.json (can be just ".")
 *	info_json	- if != NULL the .info.json file to check (can be just ".")
 *	author_json	- if != NULL the .author.json file to check (can be just ".")
 *	fnamchk		- path to the fnamchk utility
 *
 * NOTE: This function does not return on error or if things are not sane.
 *
 * NOTE: In some cases entry_dir, info_json and author_json can be NULL but
 *       fnamchk can never be NULL.
 */
static void
chkentry_sanity_chks(char const *entry_dir, char const *info_json, char const *author_json, char const *fnamchk)
{
    /*
     * firewall
     */
    if (fnamchk == NULL)
    {
	err(42, __func__, "called with NULL fnamchk");
	not_reached();
    }

    /*
     * fnamchk must be executable
     */
    if (!exists(fnamchk))
    {
	fpara(stderr,
	      "",
	      "We cannot find fnamchk.",
	      "",
	      "This tool is required for various tests on the JSON files."
	      "Perhaps you need to use:",
	      "",
	      "    chkentry -F /path/to/fnamchk ...",
	      NULL);
	err(43, __func__, "fnamchk does not exist: %s", fnamchk);
	not_reached();
    }
    if (!is_file(fnamchk))
    {
	fpara(stderr,
	      "",
	      "The fnamchk, while it exists, is not a regular file.",
	      "",
	      "Perhaps you need to use another path:",
	      "",
	      "    chkentry -F /path/to/fnamchk ...",
	      NULL);
	err(44, __func__, "fnamchk is not a regular file: %s", fnamchk);
	not_reached();
    }
    if (!is_exec(fnamchk))
    {
	fpara(stderr,
	      "",
	      "The fnamchk, while it is a file, is not executable.",
	      "",
	      "We suggest you check the permissions on the fnamchk program, or use another path:",
	      "",
	      "    chkentry -F /path/to/fnamchk ...",
	      NULL);
	err(45, __func__, "fnamchk is not an executable program: %s", fnamchk);
	not_reached();
    }


    /*
     * if != NULL, we must be able to cd to entry_dir
     */
    if (entry_dir != NULL)
    {
	if (!exists(entry_dir))
	{
	    fpara(stderr,
		  "",
		  "The entry_dir path specified does not exist. Perhaps you made a typo?",
		  "Please check the path and try again."
		  "",
		  "    chkentry [-h] [-v level] [-J level] [-V] [-q] [-F fnamchk] entry_dir"
		  "",
		  NULL);
	    err(46, __func__, "entry_dir does not exist: %s", entry_dir);
	    not_reached();
	}
	if (!is_dir(entry_dir))
	{
	    fpara(stderr,
		  "",
		  "The entry_dir specified, while it exists, is not a directory.",
		  "",
		  "Perhaps you need to use another path:",
		  "",
		  "    chkentry [-h] [-v level] [-J level] [-V] [-q] [-F fnamchk] entry_dir"
		  "",
		  NULL);
	    err(47, __func__, "entry_dir is not a directory: %s", entry_dir);
	    not_reached();
	}

	verify_entry_dir(entry_dir, info_json, author_json);
    }
    else /* entry_dir == NULL */
    {
	/* although one or the other can be NULL, both cannot be NULL */
	if (info_json == NULL && author_json == NULL)
	{
	    err(48, __func__, "called with NULL entry_dir, info_json and author_json");
	    not_reached();
	}

	/*
	 * it's okay if entry_dir is NULL: see that function comments for
	 * details.
	 */
	verify_entry_dir(entry_dir, info_json, author_json);
    }
    /* we also check that all the tables across the IOCCC toolkit are sane */
    ioccc_sanity_chks();

    return;
}


int
main(int argc, char *argv[])
{
    char const *program = NULL;	/* our name */
    extern char *optarg;	/* option argument */
    extern int optind;		/* argv index of the next arg */
    bool test = false;		/* true ==> JSON conversion test case result */
    bool error = false;		/* true ==> JSON conversion test suite error */
    char *fnamchk = FNAMCHK_PATH_0;	/* path to fnamchk executable */
    bool fnamchk_flag_used = false;	/* true ==> -F fnamchk used */
    struct json *node = NULL;	/* allocated JSON parser tree node */
    char const *entry_dir = ".";	/* entry directory to process, or NULL ==> process files */
    char const *info_json = ".";	/* .info.json file to process, or NULL ==> no .info.json to process */
    char const *author_json = ".";	/* .author.json file to process, or NULL ==> no .author.json to process */
    int i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:J:VqF:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(3, program, "-h help mode", -1, -1); /*ooo*/
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
	    print("%s", CHKENTRY_VERSION);
	    exit(3); /*ooo*/
	    not_reached();
	    break;
	case 'q':
	    quiet = true;
	    msg_warn_silent = true;
	    break;
	 case 'F':
            fnamchk_flag_used = true;
            fnamchk = optarg;
            break;
	default:
	    usage(4, program, "invalid -flag", -1, -1); /*ooo*/
	    not_reached();
	 }
    }
    argc -= optind;
    argv += optind;
    switch (argc) {
    case 1:
	entry_dir = argv[0];
	info_json = NULL;
	author_json = NULL;
	break;
    case 2:
	entry_dir = NULL;
	info_json = argv[0];
	author_json = argv[1];
	break;
    default:
	if (argc >= 2) {
	    vrergfB(atoi(argv[0]), atoi(argv[1])); /* XXX - keep this line around for testing purposes for now - XXX */
	} else {
	    usage(5, program, "invalid number of arguments", -1, -1); /*ooo*/
	}
	not_reached();
	break;
    }
    if (info_json != NULL && author_json != NULL && entry_dir == NULL) {
	if (strcmp(info_json, ".") == 0 && strcmp(author_json, ".") == 0) {
	    vrergfB(-1, -1);
	    not_reached();
	}
    }

    /* find fnamchk */
    find_utils(false, NULL, false, NULL, false, NULL, false, NULL,
	       fnamchk_flag_used, &fnamchk, false, NULL);

    chkentry_sanity_chks(entry_dir, info_json, author_json, fnamchk);


    /* XXX - fake code below needs to be removed - XXX */
    if (node == NULL) {
	error = true; /* XXX */
    }
    if (!test) {
	test = true; /* XXX */
    }
    if (error) {
	error = false; /* XXX */
    }
    /* XXX - add more code here - XXX */

    /*
     * All Done!!! - Jessica Noll, age 2
     */
    exit(error != false);
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
 *	expected	>= 0 ==> expected args, < 0 ==> ignored
 *	argc		>= 0 ==> argument count, < 0 ==> ignored
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
static void
usage(int exitcode, char const *prog, char const *str, int expected, int argc)
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
    fprintf_usage(DO_NOT_EXIT, stderr, str, expected, expected+1, argc);
    fprintf_usage(exitcode, stderr, usage_msg, prog, prog, DBG_DEFAULT, JSON_DBG_DEFAULT, FNAMCHK_PATH_0, JNUM_CHK_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
