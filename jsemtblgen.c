/*
 * jsemtblgen - generate JSON semantics table
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
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
 * static globals
 */
static bool h_mode = false;		/* -I - true ==> output as .h include file, false ==> output as .c src */
static char *tbl_name = "sem_tbl";	/* -N name - name of the semantic table */
static char *def_func = "NULL";		/* -D def_func - validate with def_func() unless overridden */
static char *prefix = NULL;		/* -P prefix - validate JTYPE_MEMBER with prefix_name() or NULL */
static char *number_func = NULL;	/* -1 func - validate JTYPE_NUMBER JSON nodes or NULL */
static char *string_func = NULL;	/* -S func - validate JTYPE_STRING JSON nodes or NULL */
static char *bool_func = NULL;		/* -B func - validate JTYPE_BOOL JSON nodes or NULL */
static char *null_func = NULL;		/* -0 func - validate JTYPE_NULL JSON nodes or NULL */
static char *member_func = NULL;	/* -M func - validate JTYPE_MEMBER JSON nodes or NULL */
static char *object_func = NULL;	/* -O func - validate JTYPE_OBJECT JSON nodes or NULL */
static char *array_func = NULL;		/* -A func - validate JTYPE_ARRAY JSON nodes or NULL */
static char *unknown_func = NULL;	/* -U func - validate nodes with unknown types or NULL */


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
    int arg_count = 0;		    /* number of args to process */
    char *cap_tbl_name = NULL;	    /* UPPER case copy of tbl_name */
    size_t len = 0;		    /* length pf tbl_name */
    size_t i;
    int c;

    /*
     * parse args
     */
    program = argv[0];
    while ((c = getopt(argc, argv, "hv:J:qVsIN:D:P:1:S:B:0:M:O:A:U:")) != -1) {
	switch (c) {
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
	case 'J':		/* -J level - JSON parser verbosity level */
	    /*
	     * parse json verbosity level
	     */
	    json_verbosity_level = parse_verbosity(program, optarg);
	    break;
	case 'q':		/* -q - quiet mode: silence msg(), warn(), warnp() if -v 0 */
	    msg_warn_silent = true;
	    break;
	case 'V':		/* -V - print version and exit */
	    print("%s\n", JSEMTBLGEN_VERSION);
	    exit(2); /*ooo*/
	    not_reached();
	    break;
	case 's':		/* -s - arg is a string */
	    string_flag_used = true;
	    break;
	case 'I':
	    h_mode = true;
	    break;
	case 'N':		/* -N name - name of the semantic table */
	    tbl_name = optarg;
	    break;
	case 'D':		/* -D def_func - validate with def_func() unless overridden */
	    def_func = optarg;
	    break;
	case 'P':		/* -P prefix - validate JTYPE_MEMBER with prefix_name() */
	    prefix = optarg;
	    break;
	case '1':		/* -1 func - validate JTYPE_NUMBER JSON nodes */
	    number_func = optarg;
	    break;
	case 'S':		/* -S func - validate JTYPE_STRING JSON nodes */
	    string_func = optarg;
	    break;
	case 'B':		/* -B func - validate JTYPE_BOOL JSON nodes */
	    bool_func = optarg;
	    break;
	case '0':		/* -0 func - validate JTYPE_NULL JSON nodes */
	    null_func = optarg;
	    break;
	case 'M':		/* -M func - validate JTYPE_MEMBER JSON nodes */
	    member_func = optarg;
	    break;
	case 'O':		/* -O func - validate JTYPE_NULL JSON nodes */
	    object_func = optarg;
	    break;
	case 'A':		/* -A func - validate JTYPE_ARRAY JSON nodes */
	    array_func = optarg;
	    break;
	case 'U':		/* -U func - validate nodes with unknown types */
	    unknown_func = optarg;
	    break;
	default:
	    usage(3, "invalid -flag or missing option argument", program); /*ooo*/
	    not_reached();
	}
    }
    arg_count = argc - optind;
    if (arg_count != REQUIRED_ARGS) {
	usage(3, "wrong number of arguments", program); /*ooo*/
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
    dbg(DBG_MED, "output_mode: %s", h_mode ? ".h include file" : ".c src file");
    dbg(DBG_MED, "tbl_name: <%s>", tbl_name);
    dbg(DBG_MED, "cap_tbl_name: <%s>", cap_tbl_name);

    /*
     * enable bison internal debugging if -J is verbose enough
     */
    if (json_dbg_allowed(JSON_DBG_VHIGH)) {
	yydebug = 1;	/* verbose bison debug on */
	dbg(DBG_VHIGH, "yydebug: enabled");
    } else {
	yydebug = 0;	/* verbose bison debug off */
	dbg(DBG_VHIGH, "yydebug: disabled");
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
	filename = argv[argc-1];
	tree = parse_json_file(filename, &valid_json);
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
    if (h_mode == true) {
	print_sem_h_src(tbl, tbl_name, cap_tbl_name);
    } else {
	print_sem_c_src(tbl, tbl_name, cap_tbl_name);
    }

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
 * vupdate_tbl - tree walk callback to update semantic table for a given JSON node
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
    struct json_sem *p = NULL;	     /* i-th entry in the semantics table */
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
	if (node->type == JTYPE_MEMBER && p->type == JTYPE_MEMBER && p->name != NULL) {
	    struct json_member const *item = &(node->item.member);

	    /*
	     * match decoded name
	     */
	    /* paranoia */
	    if (item->name == NULL) {
		err(17, __func__, "item->name is NULL");
		not_reached();
	    }
	    if (item->name_str_len != p->name_len ||
		memcmp(item->name_str, p->name, p->name_len) != 0) {
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
	    struct json_member const *item = &(node->item.member);
	    new.name = item->name_str;
	    new.name_len = item->name_str_len;
	} else {
	    new.name = NULL;
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
 * Produce a semantic table that is in some canonical order.
 *
 * NOTE: Since the table is not very large, we do not need to
 *	 optimize lookups in this table.  It is better to reverse
 *	 sort by depth for human understanding.  This will help
 *	 put the more common nodes early in the table.
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
    if (first->name != NULL) {
	if (second->name != NULL) {
	    cmp = strcmp(first->name, second->name);
	    if (cmp > 0) {
		return 1;	/* first > second */
	    } else if (cmp < 0) {
		return -1;	/* first < second */
	    }
	} else {
	    return -1;	/* first < NULL second */
	}
    } else if (second->name != NULL) {
	return 1;	/* NULL first > second */
    }
    /* case: name matches */

    /*
     * entries match
     */
    return 0;
}


/*
 * print_c_funct_name - print a string as a C function name
 *
 * Prints str as a valid C function name. Any character that is not
 * alphanumeric is printed as an underscore ("_") character.
 *
 * If the first character of str is a digit,
 * a leading x ("x") will be printed before str is processed.
 *
 * If the first character of str is an underscore ("_"),
 * a leading x ("x") will be printed before str is processed.
 * C names starting with underscore ("_") are reserved, so this
 * rule prevents the function from being a reserved function.
 *
 * An empty string will cause x ("x") to be printed.
 *
 * If a C reserved word would otherwise be printed, a leading
 * x ("x") will be printed before str is processed.
 *
 * Examples:
 *
 *	"foo" ==> "foo"
 *	"23209" ==> "x23209"
 *	"3foo" ==> "x3foo"
 *	"curds&whey++.*" ==> "curds_whey____"
 *	"_foo" ==> "x_foo"
 *	"++" ==> "x__"
 *	"" ==> "x"
 *	"Hello, world!\n" ==> "Hello_world__"
 *	"if" ==> "xif"
 *	"#if" ==> "x_if"
 *	"_Pragma" ==> "x_Pragma"
 *
 * given:
 *	stream	open FILE stream to print on
 *	str	string to print to stream
 *
 * NOTE: This function does not return if given NULL pointers or on error.
 */
static void
print_c_funct_name(FILE *stream, char const *str)
{
    bool reserved = false;	/* true ==> str is a reserved word in C */

    /*
     * firewall
     */
    if (stream == NULL) {
	err(20, __func__, "stream is NULL");
	not_reached();
    }
    if (str == NULL) {
	err(21, __func__, "str is NULL");
	not_reached();
    }

    /*
     * case: str is a C reserved word
     * case: str is an empty string
     * case: str begins with a digit
     * case: str begins with an underscore
     */
    reserved = is_reserved(str);
    if (reserved == true || str[0] == '\0' || (isascii(str[0]) && isdigit(str[0])) || str[0] == '_') {
	/* print a leading x */
	fprstr(stream, "x");
    }

    /*
     * process each character in C, converting non-alphanumeric characters to underscore.
     */
    while (str[0] != '\0') {
	if (!isascii(str[0]) || !isalnum(str[0])) {
	    /* convert non-C name character to underscore */
	    fprstr(stream, "_");
	} else {
	    fprint(stream, "%c", str[0]);
	}
	++str;
    }
    return;
}


/*
 * print_sem_c_src - print a sorted semantic table as a .c src file
 *
 * given:
 *	tbl		dynamic array of semantic table entries
 *	tbl_name	name of the semantic table
 *	cap_tbl_name	UPPER case copy of tbl_name
 *
 * NOTE: This function does not return if given NULL pointers or on error.
 */
static void
print_sem_c_src(struct dyn_array *tbl, char *tbl_name, char *cap_tbl_name)
{
    struct json_sem *p = NULL;	/* current semantic table to print */
    intmax_t len = 0;		/* number of semantic table entries */
    intmax_t i;

    /*
     * firewall
     */
    if (tbl == NULL) {
	err(22, __func__, "tbl is NULL");
	not_reached();
    }
    if (tbl_name == NULL) {
	err(23, __func__, "tbl_name is NULL");
	not_reached();
    }
    if (cap_tbl_name == NULL) {
	err(24, __func__, "cap_tbl_name is NULL");
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
    print("struct json_sem %s[%s_LEN+1] = {\n", tbl_name, cap_tbl_name);
    prstr("/* depth    type        min     max   count   index  name_len validate  name */\n");

    /*
     * print each semantic table entry
     */
    for (i=0; i < len; ++i) {
	char *validate = def_func;	/* validation function name */

	/*
	 * determine the validation function name
	 */
	p = dyn_array_addr(tbl, struct json_sem, i);
	switch (p->type) {
	case JTYPE_NUMBER:
	    if (number_func != NULL) {
		validate = number_func;
	    }
	    break;
	case JTYPE_STRING:
	    if (string_func != NULL) {
		validate = string_func;
	    }
	    break;
	case JTYPE_BOOL:
	    if (bool_func != NULL) {
		validate = bool_func;
	    }
	    break;
	case JTYPE_NULL:
	    if (null_func != NULL) {
		validate = null_func;
	    }
	    break;
	case JTYPE_MEMBER:
	    if (member_func != NULL) {
		validate = member_func;
	    }
	    break;
	case JTYPE_OBJECT:
	    if (object_func != NULL) {
		validate = object_func;
	    }
	    break;
	case JTYPE_ARRAY:
	    if (array_func != NULL) {
		validate = array_func;
	    }
	    break;
	default:
	    if (unknown_func != NULL) {
		validate = unknown_func;
	    }
	    break;
	}

	/*
	 * print a given semantic table entry
	 */
	if (p->name == NULL) {
	    if (p->count == INF) {
		print("  { %u,\t%s,\t1,\tINF,\t%u,\t%ju,\t0,\t",
		      p->depth, json_type_name(p->type), p->count, i);
		print_c_funct_name(stdout, validate);
		prstr("\tNULL },\n");
	    } else {
		print("  { %u,\t%s,\t1,\t%u,\t%u,\t%ju,\t0,\t",
		      p->depth, json_type_name(p->type), p->count, p->count, i);
		print_c_funct_name(stdout, validate);
		prstr(",\tNULL },\n");
	    }
	} else {
	    if (p->count == INF) {
		if (p->type == JTYPE_MEMBER && prefix != NULL) {
		    print("  { %u,\t%s,\t1,\tINF,\t%u,\t%ju,\t%ju,\t",
			  p->depth, json_type_name(p->type), p->count, i, (uintmax_t)p->name_len);
		    print_c_funct_name(stdout, prefix);
		    prstr("_");
		    print_c_funct_name(stdout, p->name);
		    prstr(",\t\"");
		    print_c_funct_name(stdout, p->name);
		    prstr("\" },\n");
		} else {
		    print("  { %u,\t%s,\t1,\tINF,\t%u,\t%ju,\t%ju,\t",
			  p->depth, json_type_name(p->type), p->count, i, (uintmax_t)p->name_len);
		    print_c_funct_name(stdout, p->name);
		    prstr(",\t\"");
		    print_c_funct_name(stdout, validate);
		    prstr("\" },\n");
		}
	    } else {
		if (p->type == JTYPE_MEMBER && prefix != NULL) {
		    print("  { %u,\t%s,\t1,\t%u,\t%u,\t%ju,\t%ju,\t",
			  p->depth, json_type_name(p->type), p->count, p->count, i, (uintmax_t)p->name_len);
		    print_c_funct_name(stdout, prefix);
		    prstr("_");
		    print_c_funct_name(stdout, p->name);
		    prstr(",\t\"");
		    print_c_funct_name(stdout, p->name);
		    prstr("\" },\n");
		} else {
		    print("  { %u,\t%s,\t1,\t%u,\t%u,\r%ju,\t%ju,\t",
			  p->depth, json_type_name(p->type), p->count, p->count, i, (uintmax_t)p->name_len);
		    print_c_funct_name(stdout, p->name);
		    prstr(",\t\"");
		    print_c_funct_name(stdout, validate);
		    prstr("\" },\n");
		}
	    }
	}
    }

    /*
     * print semantic table trailer
     */
    prstr("  { 0,\tJTYPE_UNSET,\t0,\t0,\t0,\t-1,\t0,\tNULL,\tNULL }\n");
    prstr("};\n");

    return;
}


/*
 * print_sem_h_src - print a sorted semantic table as a .h include file
 *
 * given:
 *	tbl		dynamic array of semantic table entries
 *	tbl_name	name of the semantic table
 *	cap_tbl_name	UPPER case copy of tbl_name
 *
 * NOTE: This function does not return if given NULL pointers or on error.
 */
static void
print_sem_h_src(struct dyn_array *tbl, char *tbl_name, char *cap_tbl_name)
{
    struct json_sem *p = NULL;	/* current semantic table to print */
    intmax_t len = 0;		/* number of semantic table entries */
    intmax_t i;

    /*
     * firewall
     */
    if (tbl == NULL) {
	err(25, __func__, "tbl is NULL");
	not_reached();
    }
    if (tbl_name == NULL) {
	err(26, __func__, "tbl_name is NULL");
	not_reached();
    }
    if (cap_tbl_name == NULL) {
	err(27, __func__, "cap_tbl_name is NULL");
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
    print("#if !defined(%s_LEN)\n\n", cap_tbl_name);
    print("#define %s_LEN (%jd)\n\n", cap_tbl_name, len);
    print("extern struct json_sem %s[%s_LEN+1];\n\n", tbl_name, cap_tbl_name);

    /*
     * print each semantic table entry
     */
    for (i=0; i < len; ++i) {
	char *validate = def_func;	/* validation function name */

	/*
	 * determine the validation function name
	 */
	p = dyn_array_addr(tbl, struct json_sem, i);
	switch (p->type) {
	case JTYPE_NUMBER:
	    if (number_func != NULL) {
		validate = number_func;
	    }
	    break;
	case JTYPE_STRING:
	    if (string_func != NULL) {
		validate = string_func;
	    }
	    break;
	case JTYPE_BOOL:
	    if (bool_func != NULL) {
		validate = bool_func;
	    }
	    break;
	case JTYPE_NULL:
	    if (null_func != NULL) {
		validate = null_func;
	    }
	    break;
	case JTYPE_MEMBER:
	    if (member_func != NULL) {
		validate = member_func;
	    }
	    break;
	case JTYPE_OBJECT:
	    if (object_func != NULL) {
		validate = object_func;
	    }
	    break;
	case JTYPE_ARRAY:
	    if (array_func != NULL) {
		validate = array_func;
	    }
	    break;
	default:
	    if (unknown_func != NULL) {
		validate = unknown_func;
	    }
	    break;
	}

	/*
	 * print an extern if we have a non-NULL validation function
	 */
	if (p->name != NULL) {
	    prstr("extern bool ");
	    if (p->type == JTYPE_MEMBER && prefix != NULL && strcmp(p->name, "NULL") != 0) {
		print_c_funct_name(stdout, prefix);
		prstr("_");
		print_c_funct_name(stdout, p->name);
	    } else if (strcmp(validate, "NULL") != 0) {
		print_c_funct_name(stdout, validate);
	    }
	    prstr("(struct json const *node,\n\tunsigned int depth, "
		  "struct json_sem *sem, struct json_sem_val_err **val_err);\n");
	}
    }

    /*
     * print semantic table trailer
     */
    print("\n#endif /* %s_LEN */\n", cap_tbl_name);

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
