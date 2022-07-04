/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jsemtblgen - generate JSON semantics table
 *
 * "Because there is more to JSON than a flawed grammar specification."
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <unistd.h>

/*
 * jsemtblgen - generate JSON semantics table
 */
#include "jsemtblgen.h"


/*
 * definitions
 */
#define REQUIRED_ARGS (1)	/* number of required arguments on the command line */
#define CHUNK (16)		/* allocate CHUNK elements at a time */


/*
 * static functions
 */
static void gen_sem_tbl(struct json *tree, unsigned int max_depth, ...);
static void vupdate_tbl(struct json *node, unsigned int depth, va_list ap);
static int sem_cmp(void const *a, void const *b);
static void print_sem_tbl(struct dyn_array *tbl, char *tbl_name, char *cap_tbl_name);
static void usage(int exitcode, char const *str, char const *prog);


int
main(int argc, char **argv)
{
    struct dyn_array *tbl = NULL;   /* semantic table - array of struct json_sem */
    char const *program = NULL;	    /* our name */
    extern char *optarg;	    /* option argument */
    extern int optind;		    /* argv index of the next arg */
    bool string_flag_used = false;  /* true ==> -S string was used */
    bool valid_json = false;	    /* true ==> JSON parse was valid */
    struct json *tree = NULL;	    /* JSON parse tree or NULL */
    int arg_cnt = 0;		    /* number of args to process */
    char *tbl_name = "sem_tbl";	    /* name of the semantic table */
    char *cap_tbl_name = NULL;	    /* UPPER case copy of tbl_name */
    size_t len = 0;		    /* length pf tbl_name */
    int ret;			    /* libc return code */
    size_t i;

    /*
     * parse args
     */
    program = argv[0];
    while ((i = getopt(argc, argv, "hv:J:qVsN:")) != -1) {
	switch (i) {
	case 'h':		/* -h - print help to stderr and exit 0 */
	    usage(2, "-h help mode", program); /*ooo*/
	    not_reached();
	    break;
	case 'v':		/* -v verbosity */
	    /*
	     * parse verbosity
	     */
	    verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'J': /* -J json_verbosity_level */
	    /*
	     * parse json verbosity level
	     */
	    json_verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'q':
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("JSON parser version: %s\n", JSON_PARSER_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing JSON parser version string: %s", JSEMTBLGEN_VERSION);
	    }
	    errno = 0;		/* pre-clear errno for warnp() */
	    ret = printf("jparse version: %s\n", JSEMTBLGEN_VERSION);
	    if (ret <= 0) {
		warnp(__func__, "printf error printing jparse version string: %s", JSEMTBLGEN_VERSION);
	    }
	    exit(0); /*ooo*/
	    not_reached();
	    break;
	case 's':
	    string_flag_used = true;
	    break;
	case 'N':
	    tbl_name = optarg;
	    break;
	default:
	    usage(2, "invalid -flag or missing option argument", program); /*ooo*/
	    not_reached();
	}
    }
    arg_cnt = argc - optind;
    if (arg_cnt != REQUIRED_ARGS) {
	usage(4, "wrong number of arguments", program); /*ooo*/
	not_reached();
    }

    /*
     * form upper case table name
     */
    errno = 0;		/* pre-clear errno for errp() */
    cap_tbl_name = strdup(tbl_name);
    if (cap_tbl_name == NULL) {
	++num_errors;
	errp(10, program, "strdup of tbl_name failed");
	not_reached();
    }
    len = strlen(tbl_name);
    if (len <= 0) {
	++num_errors;
	errp(11, program, "tbl_name cannot be empty");
	not_reached();
    }
    for (i=0; i < len; ++i) {
	if (isascii(tbl_name[i]) && islower(tbl_name[i])) {
	    cap_tbl_name[i] = (char)toupper(tbl_name[i]);
	}
    }
    dbg(DBG_MED, "tbl_name: <%s>", tbl_name);
    dbg(DBG_MED, "cap_tbl_name: <%s>", cap_tbl_name);

    /*
     * enable bison internal debugging if -J is verbose enough
     */
    if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	ugly_debug = 1;	/* verbose bison debug on */
	dbg(DBG_VHIGH, "ugly_debug: enabled");
    } else {
	ugly_debug = 0;	/* verbose bison debug off */
	dbg(DBG_VHIGH, "ugly_debug: disabled");
    }

    /*
     * case: process -s arg
     */
    if (string_flag_used == true) {

	/* parse arg as a block of json input */
	dbg(DBG_HIGH, "Calling parse_json(\"%s\", %ju, &valid_json):",
		      argv[argc-1], (uintmax_t)strlen(argv[argc-1]));
	tree = parse_json(argv[argc-1], strlen(argv[argc-1]), &valid_json);

    /*
     * case: process file arg
     */
    } else {

	/* parse arg as a json filename */
	dbg(DBG_HIGH, "Calling parse_json_file(\"%s\", &valid_json):", argv[argc-1]);
	tree = parse_json_file(argv[argc-1], &valid_json);
    }

    /*
     * firewall - JSON parser must have returned a valid JSON parse tree
     */
    if (valid_json == false) {
	++num_errors;
	err(1, program, "invalid JSON"); /*ooo*/
	not_reached();
    }
    if (tree == NULL) {
	++num_errors;
	err(12, program, "JSON parse tree is NULL");
	not_reached();
    }

    /*
     * create an empty semantic table
     */
    tbl = dyn_array_create(sizeof(struct json_sem), CHUNK, CHUNK, true);
    if (tbl == NULL) {
	++num_errors;
	err(13, program, "NULL dynamic array");
	not_reached();
    }

    /*
     * generate the semantic table from the parsed JSON node tree
     */
    gen_sem_tbl(tree, JSON_INFINITE_DEPTH, tbl);

    /*
     * print a sorted semantic table as a C structure
     */
    print_sem_tbl(tbl, tbl_name, cap_tbl_name);

    /* XXX - add more code here - XXX */

    /*
     * free the JSON parse tree
     */
    if (tree != NULL) {
	json_tree_free(tree, JSON_INFINITE_DEPTH);
	free(tree);
	tree = NULL;
    }

    /*
     *  exit based on JSON parse success or failure
     */
    if (num_errors > 0) {
	exit(1); /*ooo*/
    }
    exit(0); /*ooo*/
}


/*
 * gen_sem_tbl - update semantic table from the parsed JSON node tree
 *
 * given:
 *      tree        pointer to a JSON parse tree
 *      max_depth   maximum tree depth to descend, or 0 ==> infinite depth
 *                      NOTE: Use JSON_INFINITE_DEPTH for infinite depth
 *      ...         extra args for vcallback, required extra args:
 *
 *		tbl	dynamic array of semantic table entries
 *
 * NOTE: This function does nothing if tree is NULL.
 */
static void
gen_sem_tbl(struct json *tree, unsigned int max_depth, ...)
{
    va_list ap;		/* variable argument list */

    /*
     * firewall - nothing to do for a NULL tree
     */
    if (tree == NULL) {
        return;
    }

    /*
     * stdarg variable argument list setup
     */
    va_start(ap, max_depth);

    /*
     * update semantic table from the parsed JSON node tree
     */
    vjson_tree_walk(tree, max_depth, 0, vupdate_tbl, ap);

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap);
    return;
}


/*
 * vupdate_tbl - tree talk callback to update semantic table for a given JSON node
 *
 * If the JSON node is represented in the semantic table, update the count.
 * If the JSON node is not in the semantic table, this function will add a
 * new entry to the semantic table.
 *
 * Because the semantic table is not expected to be huge, we will perform
 * a simple linear search.  If the semantic table an entry for the JSON node
 * is missing, we will simply append a new entry.
 *
 * given:
 *	node    pointer to a JSON parser tree node to free
 *      depth   current tree depth (0 ==> top of tree)
 *      ap      variable argument list, required ap args:
 *
 *		tbl	dynamic array of semantic table entries
 *
 * NOTE: This function does not return if given NULL pointers or on error.
 */
static void
vupdate_tbl(struct json *node, unsigned int depth, va_list ap)
{
    struct dyn_array *tbl = NULL;    /* semantic table - array of struct json_sem */
    struct json_sem *p = NULL;	     /* i-th entey in the semantic */
    struct json_sem new;	     /* new semantic table entry */
    va_list ap2;		     /* copy of va_list ap */
    intmax_t len = 0;		     /* number of semantic table entries */
    bool match = false;		     /* true ==> semantic table entry match found */
    intmax_t i;

    /*
     * firewall
     */
    if (node == NULL) {
	err(14, __func__, "node is NULL");
	not_reached();
    }

    /*
     * duplicate va_list ap
     */
    va_copy(ap2, ap);

    /*
     * obtain the array of struct json_sem
     */
    tbl = va_arg(ap2, struct dyn_array *);
    if (tbl == NULL) {
	err(15, __func__, "tbl va_arg is NULL");
	not_reached();
    }
    len = dyn_array_tell(tbl);

    /*
     * scan the semantic table
     */
    for (i = 0; i < len; ++i) {

	/*
	 * note the semantic table entry we will work with
	 */
	p = dyn_array_addr(tbl, struct json_sem, i);
	/* paranoia */
	if (p == NULL) {
	    err(16, __func__, "semantic tbl[%jd] is NULL", i);
	    not_reached();
	}

	/*
	 * match tree depth
	 */
	if (depth != p->depth) {
	    continue;
	}

	/*
	 * match node type
	 */
	if (node->type != p->type) {
	    continue;
	}

	/*
	 * case: if JTYPE_MEMBER - match non-NULL non-empty JSON decoded name
	 */
	if (node->type == JTYPE_MEMBER && p->type == JTYPE_MEMBER && p->name_str != NULL) {
	    struct json_member *item = &(node->item.member);

	    /*
	     * match decoded name
	     */
	    /* paranoia */
	    if (item->name_str == NULL) {
		err(17, __func__, "item->name_str is NULL");
		not_reached();
	    }
	    if (item->name_str_len != p->name_str_len ||
		memcmp(item->name_str, p->name_str, p->name_str_len) != 0) {
		continue;
	    }
	}

	/*
	 * we have a match in the semantic table, increment count
	 */
	++p->count;
	match = true;
	break;
    }

    /*
     * case: no semantic table match - add a new entry
     */
    if (match == false) {

	/*
	 * construct a new semantic table entry for this node
	 */
	memset(&new, 0, sizeof(struct json_sem));
	new.depth = depth;
	new.type = node->type;
	if (node->type == JTYPE_MEMBER) {
	    struct json_member *item = &(node->item.member);
	    new.name_str = item->name_str;
	    new.name_str_len = item->name_str_len;
	} else {
	    new.name_str = NULL;
	}
	new.validate = NULL;
	new.count = 1;

	/*
	 * append new new semantic table entry to semantic table
	 */
	(void) dyn_array_append_value(tbl, &new);
    }

    /*
     * stdarg variable argument list cleanup
     */
    va_end(ap2);
    return;
}


/*
 * sem_cmp - compare two semantic table entries
 *
 * given:
 *	a	pointer to first semantic table entry to compare
 *	b	pointer to second semantic table entry to compare
 *
 * returns:
 *      -1      a < b
 *      0       a == b
 *      1       a > b
 */
static int
sem_cmp(void const *a, void const *b)
{
    struct json_sem *first = NULL;	/* 1st entry to compare */
    struct json_sem *second = NULL;	/* 2nd entry to compare */
    int cmp = 0;			/* byte string comparison */

    /*
     * firewall
     */
    if (a == NULL) {
	err(18, __func__, "a is NULL");
	not_reached();
    }
    if (b == NULL) {
	err(19, __func__, "b is NULL");
	not_reached();
    }
    first = (struct json_sem *)a;
    second = (struct json_sem *)b;

    /*
     * compare depth in reverse order
     */
    if (first->depth < second->depth) {
	return 1;	/* first > second */
    } else if (first->depth > second->depth) {
	return -1;	/* first < second */
    }
    /* case: depth matches */

    /*
     * compare numeric type of JSON node
     */
    if ((int)first->type > (int)second->type) {
	return 1;	/* first > second */
    } else if ((int)first->type < (int)second->type) {
	return -1;	/* first < second */
    }
    /* case: type matches */

    /*
     * compare match decoded name if not NULL
     */
    if (first->name_str != NULL) {
	if (second->name_str != NULL) {
	    cmp = memcmp(first->name_str, second->name_str, first->name_str_len);
	    if (cmp > 0) {
		return 1;	/* first > second */
	    } else if (cmp < 0) {
		return -1;	/* first < second */
	    }
	} else {
	    return -1;	/* first < NULL second */
	}
    } else if (second->name_str != NULL) {
	return 1;	/* NULL first > second */
    }
    /* case: name matches */

    /*
     * compare counts
     */
    if (first->count > second->count) {
	return 1;	/* first > second */
    } else if (first->count < second->count) {
	return -1;	/* first < second */
    }
    /* case: count matches */

    /*
     * entries match
     */
    return 0;
}


/*
 * print_sem_tbl - print a sorted semantic table as a C structure
 *
 * given:
 *	tbl		dynamic array of semantic table entries
 *	tbl_name	name of the semantic table
 *	cap_tbl_name	UPPER case copy of tbl_name
 *
 * NOTE: This function does not return if given NULL pointers or on error.
 */
static void
print_sem_tbl(struct dyn_array *tbl, char *tbl_name, char *cap_tbl_name)
{
    struct json_sem *p = NULL;	/* current semantic table to print */
    intmax_t len = 0;		/* number of semantic table entries */
    intmax_t i;

    /*
     * firewall
     */
    if (tbl == NULL) {
	err(20, __func__, "tbl is NULL");
	not_reached();
    }
    if (tbl_name == NULL) {
	err(21, __func__, "tbl_name is NULL");
	not_reached();
    }
    if (cap_tbl_name == NULL) {
	err(22, __func__, "cap_tbl_name is NULL");
	not_reached();
    }

    /*
     * sort the semantic table
     */
    qsort(tbl->data, dyn_array_tell(tbl), sizeof(struct json_sem), sem_cmp);

    /*
     * print semantic table header
     */
    len = dyn_array_tell(tbl);
    print("#define %s_LEN (%jd)\n", cap_tbl_name, len);
    print("struct struct json_sem [%s_LEN] = {\n", cap_tbl_name);

    /*
     * print each semantic table entry
     */
    for (i=0; i < len; ++i) {

	/*
	 * print a given semantic table entry
	 *
	 * XXX - add code for non-NULL validate functions - XXX
	 */
	p = dyn_array_addr(tbl, struct json_sem, i);
	if (p->name_str == NULL) {
	    print("    { %u,\t%s,\t%u,\t%u,\tNULL,\t0,\tNULL,\t0 },\n",
		  p->depth, json_type_name(p->type), p->count, p->count);
	} else {
	    print("    { %u,\t%s,\t%u,\t%u,\t\"%s\",\t%ju,\tNULL,\t0 },\n",
		  p->depth, json_type_name(p->type), p->count, p->count,
		  p->name_str, (uintmax_t)p->name_str_len);
	}
    }

    /*
     * print semantic tailer
     */
    prstr("};\n");

    return;
}


/*
 * usage - print usage to stderr
 *
 * Example:
 *      usage(3, "missing required argument(s), program: %s", program);
 *
 * given:
 *	exitcode        value to exit with
 *	str		top level usage message
 *	program		our program name
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *       Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 *
 */
static void
usage(int exitcode, char const *str, char const *prog)
{
    /*
     * firewall
     */
    if (str == NULL) {
	str = "((NULL str))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", str);
    }
    if (prog == NULL) {
	prog = "((NULL prog))";
	warn(__func__, "\nin usage(): program was NULL, forcing it to be: %s\n", prog);
    }

    /*
     * print the formatted usage stream
     */
    fprintf_usage(DO_NOT_EXIT, stderr, "%s\n", str);
    fprintf_usage(exitcode, stderr, usage_msg, prog,
		  DBG_DEFAULT, json_verbosity_level, JSON_PARSER_VERSION, JSEMTBLGEN_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
