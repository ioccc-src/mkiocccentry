/*
 * jsemtblgen - generate JSON semantics table
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * The concept of this file was developed by:
 *
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * The JSON parser was co-developed in 2022 by:
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
#define STRLEN(s)		(sizeof (s)-1)


/*
 * static globals
 */
static bool h_mode = false;		/* -I - true ==> output as .h include file, false ==> output as .c src */
static char *tbl_name = "sem_tbl";	/* -N name - name of the semantic table */
static char *def_func = NULL;		/* -D def_func - validate with def_func() unless overridden */
static char *prefix = NULL;		/* -P prefix - validate JTYPE_MEMBER with prefix_name() or NULL */
static char *number_func = NULL;	/* -1 func - validate JTYPE_NUMBER JSON nodes or NULL */
static char *string_func = NULL;	/* -S func - validate JTYPE_STRING JSON nodes or NULL */
static char *bool_func = NULL;		/* -B func - validate JTYPE_BOOL JSON nodes or NULL */
static char *null_func = NULL;		/* -0 func - validate JTYPE_NULL JSON nodes or NULL */
static char *member_func = NULL;	/* -M func - validate JTYPE_MEMBER JSON nodes or NULL */
static char *object_func = NULL;	/* -O func - validate JTYPE_OBJECT JSON nodes or NULL */
static char *array_func = NULL;		/* -A func - validate JTYPE_ARRAY JSON nodes or NULL */
static char *unknown_func = NULL;	/* -U func - validate nodes with unknown types or NULL */

/*
 * IOCCC Judge's remarks:
 *
 * The following editorial plea expresses a view shared by more than zero
 * IOCCC judges. It may not represent the opinion of all those involved
 * with this code nor the International Obfuscated C Code Contest as a whole:
 *
 * The long list of reserved words below should be a source
 * of embarrassment to some of those involved in standardizing C.
 * The growing list of reserved words, along with an expanding set of
 * linguistic inventions has the appearance of feature
 * creep that, if left unchecked, risks turning a beautifully elegant
 * language into a steaming pile of biological excretion.
 *
 * The history of the IOCCC has taught us that even minor changes
 * to the language are not always well understood by compiler writers,
 * let alone the standards body who publishes them. We have enormous
 * sympathy for C compiler writers who must keep up with the creeping
 * featurism.  We are aware of some C standards members who share
 * these concerns.  Alas, they seem to be a minority.
 *
 * The C standards body as a whole, before they emit yet more mountains of new
 * standardese, might wish consider the option of moth-balling their committee.
 * Or if they must produce a new standard, consider naming whatever
 * follows c11 as CNC (C's Not C).  :-)
 */

/*
 * C reserved words, plus a few #preprocessor tokens, that count as 1
 *
 * NOTE: For a good list of reserved words in C, see:
 *
 *	http://www.bezem.de/pdf/ReservedWordsInC.pdf
 *
 * by Johan Bezem of JB Enterprises:
 *
 *	See http://www.bezem.de/en/
 */
typedef struct {
	size_t length;
	const char *word;
} Word;

static Word cwords[] = {
	/* Yes Virginia, we left #define off the list on purpose! */
	{ STRLEN("#elif"), "#elif" } ,
	{ STRLEN("#else"), "#else" } ,
	{ STRLEN("#endif"), "#endif" } ,
	{ STRLEN("#error"), "#error" } ,
	{ STRLEN("#ident"), "#ident" } ,
	{ STRLEN("#if"), "#if" } ,
	{ STRLEN("#ifdef"), "#ifdef" } ,
	{ STRLEN("#ifndef"), "#ifndef" } ,
	{ STRLEN("#include"), "#include" } ,
	{ STRLEN("#line"), "#line" } ,
	{ STRLEN("#pragma"), "#pragma" } ,
	{ STRLEN("#sccs"), "#sccs" } ,
	{ STRLEN("#warning"), "#warning" } ,

	{ STRLEN("_Alignas"), "_Alignas" } ,
	{ STRLEN("_Alignof"), "_Alignof" } ,
	{ STRLEN("_Atomic"), "_Atomic" } ,
	{ STRLEN("_Bool"), "_Bool" } ,
	{ STRLEN("_Complex"), "_Complex" } ,
	{ STRLEN("_Generic"), "_Generic" } ,
	{ STRLEN("_Imaginary"), "_Imaginary" } ,
	{ STRLEN("_Noreturn"), "_Noreturn" } ,
	{ STRLEN("_Pragma"), "_Pragma" } ,
	{ STRLEN("_Static_assert"), "_Static_assert" } ,
	{ STRLEN("_Thread_local"), "_Thread_local" } ,

	{ STRLEN("alignas"), "alignas" } ,
	{ STRLEN("alignof"), "alignof" } ,
	{ STRLEN("and"), "and" } ,
	{ STRLEN("and_eq"), "and_eq" } ,
	{ STRLEN("auto"), "auto" } ,
	{ STRLEN("bitand"), "bitand" } ,
	{ STRLEN("bitor"), "bitor" } ,
	{ STRLEN("bool"), "bool" } ,
	{ STRLEN("break"), "break" } ,
	{ STRLEN("case"), "case" } ,
	{ STRLEN("char"), "char" } ,
	{ STRLEN("compl"), "compl" } ,
	{ STRLEN("const"), "const" } ,
	{ STRLEN("continue"), "continue" } ,
	{ STRLEN("default"), "default" } ,
	{ STRLEN("do"), "do" } ,
	{ STRLEN("double"), "double" } ,
	{ STRLEN("else"), "else" } ,
	{ STRLEN("enum"), "enum" } ,
	{ STRLEN("extern"), "extern" } ,
	{ STRLEN("false"), "false" } ,
	{ STRLEN("float"), "float" } ,
	{ STRLEN("for"), "for" } ,
	{ STRLEN("goto"), "goto" } ,
	{ STRLEN("if"), "if" } ,
	{ STRLEN("inline"), "inline" } ,
	{ STRLEN("int"), "int" } ,
	{ STRLEN("long"), "long" } ,
	{ STRLEN("noreturn"), "noreturn" } ,
	{ STRLEN("not"), "not" } ,
	{ STRLEN("not_eq"), "not_eq" } ,
	{ STRLEN("or"), "or" } ,
	{ STRLEN("or_eq"), "or_eq" } ,
	{ STRLEN("register"), "register" } ,
	{ STRLEN("restrict"), "restrict" } ,
	{ STRLEN("return"), "return" } ,
	{ STRLEN("short"), "short" } ,
	{ STRLEN("signed"), "signed" } ,
	{ STRLEN("sizeof"), "sizeof" } ,
	{ STRLEN("static"), "static" } ,
	{ STRLEN("static_assert"), "static_assert" } ,
	{ STRLEN("struct"), "struct" } ,
	{ STRLEN("switch"), "switch" } ,
	{ STRLEN("thread_local"), "thread_local" } ,
	{ STRLEN("true"), "true" } ,
	{ STRLEN("typedef"), "typedef" } ,
	{ STRLEN("union"), "union" } ,
	{ STRLEN("unsigned"), "unsigned" } ,
	{ STRLEN("void"), "void" } ,
	{ STRLEN("volatile"), "volatile" } ,
	{ STRLEN("while"), "while" } ,
	{ STRLEN("xor"), "xor" } ,
	{ STRLEN("xor_eq"), "xor_eq" } ,

	{ 0, NULL }
};


/*
 * forward declarations
 */
static void gen_sem_tbl(struct json *tree, unsigned int max_depth, ...);
static void vupdate_tbl(struct json *node, unsigned int depth, va_list ap);
static int sem_cmp(void const *a, void const *b);
static char *alloc_c_funct_name(char const *prefix, char const *str);
static bool append_unique_str(struct dyn_array *tbl, char *str);
static void print_sem_c_src(struct dyn_array *tbl, char *tbl_name, char *cap_tbl_name);
static void print_sem_h_src(struct dyn_array *tbl, char *tbl_name, char *cap_tbl_name);
static Word *find_member(Word *table, const char *string);
static bool test_reserved(const char *string);
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
    int arg_count = 0;		    /* number of args to process */
    char *cap_tbl_name = NULL;	    /* UPPER case copy of tbl_name */
    size_t len = 0;		    /* length of tbl_name */
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
	errp(10, program, "strdup of tbl_name failed");
	not_reached();
    }
    len = strlen(tbl_name);
    if (len <= 0) {
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
	dbg(DBG_HIGH, "Calling parse_json(\"%s\", %ju, NULL, &valid_json):",
		      argv[argc-1], (uintmax_t)strlen(argv[argc-1]));
	tree = parse_json(argv[argc-1], strlen(argv[argc-1]), NULL, &valid_json);

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
    if (!valid_json) {
	err(1, program, "invalid JSON"); /*ooo*/
	not_reached();
    }
    if (tree == NULL) {
	err(1, program, "JSON parse tree is NULL"); /*ooo*/
	not_reached();
    }

    /*
     * create an empty semantic table
     */
    tbl = dyn_array_create(sizeof(struct json_sem), CHUNK, CHUNK, true);
    if (tbl == NULL) {
	err(12, program, "NULL dynamic array");
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
     * free the capitalised name
     */
    if (cap_tbl_name != NULL) {
	free(cap_tbl_name);
	cap_tbl_name = NULL;
    }


    /*
     * exit based on JSON parse success or failure
     *
     * NOTE: this check is not strictly needed because if the JSON is not valid
     * it's a fatal error and so we'd never get here.
     */
    if (!valid_json) {
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
	err(13, __func__, "node is NULL");
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
	err(14, __func__, "tbl va_arg is NULL");
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
	    err(15, __func__, "semantic tbl[%jd] is NULL", i);
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
		err(16, __func__, "item->name is NULL");
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
    const struct json_sem *first = NULL;	/* 1st entry to compare */
    const struct json_sem *second = NULL;	/* 2nd entry to compare */
    int cmp = 0;				/* byte string comparison */

    /*
     * firewall
     */
    if (a == NULL) {
	err(17, __func__, "a is NULL");
	not_reached();
    }
    if (b == NULL) {
	err(18, __func__, "b is NULL");
	not_reached();
    }
    first = (const struct json_sem *)a;
    second = (const struct json_sem *)b;

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
 * alloc_c_funct_name - allocate a string as a C function name
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
 * Examples (how both prefix and str are processed):
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
 * If prefix is NULL, then the string formed is just str as processed.
 * If prefix is non-NULL, then the string formed starts with a processed
 * prefix, followed by _ (underscore), followed by str as processed.
 *
 * given:
 *	prefix	!= NULL ==> prefix to add (before str), along with a trailing _
 *	str	string to process
 *
 * returns:
 *	allocated string as a C function name
 *
 * NOTE: This function will not return on error.
 *
 * NOTE: This function does NOT return NULL.
 */
static char *
alloc_c_funct_name(char const *prefix, char const *str)
{
    bool prefix_is_reserved = false;	/* true ==> prefix is a reserved word in C */
    bool str_is_reserved = false;	/* true ==> str is a reserved word in C */
    size_t len = 0;			/* length of allocated string */
    char *ret = NULL;			/* allocated string to return */
    char *p = NULL;			/* next character to add */

    /*
     * firewall
     */
    if (str == NULL) {
	err(19, __func__, "str is NULL");
	not_reached();
    }

    /*
     * allocated space for the string
     */
    if (prefix != NULL) {
	prefix_is_reserved = test_reserved(prefix);
	len = strlen(prefix) + (prefix_is_reserved ? 1 : 0) + 1;	/* + 1 for _ after prefix */
    }
    str_is_reserved = test_reserved(str);
    len += strlen(str) + (str_is_reserved ? 1 : 0) + 1;	/* + 1 for NUL */
    ret = calloc(len + 1, sizeof(char));		/* + 1 for guard byte paranoia */
    if (ret == NULL) {
	errp(20, __func__, "calloc of %ju bytes failed", (uintmax_t)ret);
	not_reached();
    }
    p = ret;

    /*
     * if we have a prefix, process each prefix character in C, converting non-alphanumeric characters to underscore.
     */
    if (prefix != NULL) {

	/*
	 * case: prefix is a C reserved word
	 * case: prefix is an empty string
	 * case: prefix begins with a digit
	 * case: prefix begins with an underscore
	 */
	if (prefix_is_reserved == true || prefix[0] == '\0' || (isascii(prefix[0]) && isdigit(prefix[0])) || prefix[0] == '_') {
	    /* add x due to the cases mentioned above */
	    *p++ = 'x';
	}

	/*
	 * process prefix
	 */
	while (prefix[0] != '\0') {
	    if (!isascii(prefix[0]) || !isalnum(prefix[0])) {
		/* convert non-C name character to underscore */
		*p++ = '_';
	    } else {
		*p++ = prefix[0];
	    }
	    ++prefix;
	}

	/*
	 * append _ after prefix
	 */
	*p++ = '_';
    }

    /*
     * case: str is a C reserved word
     * case: str is an empty string
     * case: str begins with a digit
     * case: str begins with an underscore
     */
    if (str_is_reserved == true || str[0] == '\0' || (isascii(str[0]) && isdigit(str[0])) || str[0] == '_') {
	/* add x due to the cases mentioned above */
	*p++ = 'x';
    }

    /*
     * process each str character in C, converting non-alphanumeric characters to underscore.
     */
    while (str[0] != '\0') {
	if (!isascii(str[0]) || !isalnum(str[0])) {
	    /* convert non-C name character to underscore */
	    *p++ = '_';
	} else {
	    *p++ = str[0];
	}
	++str;
    }
    *p = '\0';	/* paranoia */

    /*
     * return calloced string
     */
    return ret;
}


/*
 * append_unique_str - append string pointer to dynamic array if not already found
 *
 * Given a pointer to string, we search a dynamic array of pointers to strings.
 * If an exact match is found (i.e, the string is already in the dynamic array),
 * nothing is done other than to return false.  If no match is found, the pointer
 * to the string is appended to the dynamic array and we return true.
 *
 * given:
 *	tbl		dynamic array of pointers to strings
 *	str		string to search tbl and append if not already found
 *
 * returns:
 *	true		str was not already in dynamic array and has now been appended
 *	false		str was already in the dynamic array, table is unchanged
 *
 * NOTE: This function does not return if given NULL pointers or on other errors.
 */
static bool
append_unique_str(struct dyn_array *tbl, char *str)
{
    intmax_t unique_len = 0;	/* number of unique function name entries */
    char *u = NULL;		/* unique name pointer */
    intmax_t i;

    /*
     * firewall
     */
    if (tbl == NULL) {
	err(21, __func__, "tbl is NULL");
	not_reached();
    }
    if (str == NULL) {
	err(22, __func__, "str is NULL");
	not_reached();
    }

    /*
     * search tbl for the string
     *
     * NOTE: We realize calling the function with unique strings will
     *	     cause the execution time to grow as O(n^2).  However the
     *	     usual number of strings in a unique function name dynamic array
     *	     is almost certainly small.  Therefore we do not need to
     *	     employ a more optimized dynamic array search mechanism.
     */
    unique_len = dyn_array_tell(tbl);
    for (i=0; i < unique_len; ++i) {

	/* get next string pointer */
	u = dyn_array_value(tbl, char *, i);
	if (u == NULL) {	/* paranoia */
	    err(23, __func__, "found NULL pointer in function name dynamic array element: %ju", (uintmax_t)i);
	    not_reached();
	}

	/* look for match */
	if (strcmp(str, u) == 0) {
	    /* str found in function name dynamic array, not unique */
	    return false;
	}
    }

    /*
     * function name is unique, append to function name dynamic array
     */
    (void) dyn_array_append_value(tbl, &str);
    return true;	/* pointer to string was appended */
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
    char *func_name = NULL;	/* function name (allocated) to print */
    intmax_t len = 0;		/* number of semantic table entries */
    intmax_t i;

    /*
     * firewall
     */
    if (tbl == NULL) {
	err(24, __func__, "tbl is NULL");
	not_reached();
    }
    if (tbl_name == NULL) {
	err(25, __func__, "tbl_name is NULL");
	not_reached();
    }
    if (cap_tbl_name == NULL) {
	err(26, __func__, "cap_tbl_name is NULL");
	not_reached();
    }

    /*
     * sort the semantic table
     */
    qsort(tbl->data, (size_t)dyn_array_tell(tbl), sizeof(struct json_sem), sem_cmp);

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
	char *validate;		/* validation function name */

	/*
	 * determine the validation function name
	 */
	validate = def_func;	/* start with default name, which may be NULL */
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
	 * print start of semantic table element, entry depth, type, min,
	 */
	print("  { %u,\t%s,\t1,", p->depth, json_type_name(p->type));
	/* print max, count, sem_index, name_len, */
	if (p->count == INF) {
	    print("\tINF,\t%u,\t%ju,\t%ju,", p->count, i, (uintmax_t)p->name_len);
	} else {
	    print("\t%u,\t%u,\t%ju,\t%ju,", p->count, p->count, i, (uintmax_t)p->name_len);
	}

	/*
	 * case: JSON MEMBER
	 *
	 * The JTYPE_MEMBER type is a special case where -M member_func will override
	 * any member named function.
	 *
	 * print validate_function,
	 */
	prstr("\t");
	if (p->type == JTYPE_MEMBER) {

	    /* if -M member_func print member_func */
	    if (member_func != NULL) {

		/* print -M member_func */
		func_name = alloc_c_funct_name(prefix, member_func);
		if (func_name != NULL) {
		    print("%s", func_name);
		    free(func_name);
		    func_name = NULL;
		}

	    /* without -M member_func print member name if we have one */
	    } else if (p->name != NULL) {

		/* print member name */
		func_name = alloc_c_funct_name(prefix, p->name);
		if (func_name != NULL) {
		    print("%s", func_name);
		    free(func_name);
		    func_name = NULL;
		}

	    /* otherwise print NULL */
	    } else {
		prstr("NULL");
	    }
	    prstr(",");

	/*
	 * case: JSON non-MEMBER with a validation function
	 *
	 * In the non-JTYPE_MEMBER type case, we print either a general validation function,
	 * or default validation function, or just NULL, where is no validation function.
	 *
	 * print validate_function,
	 */
	} else if (validate != NULL) {

	    /* print validate_function */
	    func_name = alloc_c_funct_name(prefix, validate);
	    if (func_name != NULL) {
		print("%s", func_name);
		free(func_name);
		func_name = NULL;
	    }
	    prstr(",");

	/*
	 * case: JSON non-MEMBER without a validation function
	 *
	 * print NULL,
	 */
	} else {
	    prstr("NULL,");
	}

	/*
	 * print , name and end of element
	 */
	if (p->name != NULL) {
	    /* print quoted member name w/o any prefix */
	    prstr("\t\"");
	    func_name = alloc_c_funct_name(NULL, p->name);
	    if (func_name != NULL) {
		print("%s", func_name);
		free(func_name);
		func_name = NULL;
	    }
	    prstr("\" },\n");
	} else {
	    /* print NULL for no name */
	    prstr("\tNULL },\n");
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
    char *func_name = NULL;	/* function name (allocated) to print */
    struct dyn_array *unique_tbl = NULL;	/* dynamic array of unique function names */
    char *u = NULL;		/* unique name pointer */
    intmax_t unique_len = 0;	/* number of unique function name entries */
    bool is_unique = false;	/* true ==> function name was appended to unique_tbl */
    intmax_t len = 0;		/* number of semantic table entries */
    intmax_t i;

    /*
     * firewall
     */
    if (tbl == NULL) {
	err(27, __func__, "tbl is NULL");
	not_reached();
    }
    if (tbl_name == NULL) {
	err(28, __func__, "tbl_name is NULL");
	not_reached();
    }
    if (cap_tbl_name == NULL) {
	err(29, __func__, "cap_tbl_name is NULL");
	not_reached();
    }

    /*
     * sort the semantic table
     */
    qsort(tbl->data, (size_t)dyn_array_tell(tbl), sizeof(struct json_sem), sem_cmp);

    /*
     * allocate empty dynamic array of unique function names
     */
    unique_tbl = dyn_array_create(sizeof(char *), CHUNK, CHUNK, true);
    if (unique_tbl == NULL) {
	err(30, __func__, "dyn_array_create failed");
	not_reached();
    }

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
	char *validate;		/* validation function name */

	/*
	 * determine the validation function name
	 */
	validate = def_func;	/* start with default name, which may be NULL */
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
	 * case: JSON MEMBER
	 *
	 * The JTYPE_MEMBER type is a special case where -M member_func will override
	 * any member named function.
	 *
	 * print extern for the validate_function if we have one
	 */
	if (p->type == JTYPE_MEMBER) {

	    /* if -M member_func print member_func */
	    if (member_func != NULL) {

		/* form function name based on -M member_func */
		func_name = alloc_c_funct_name(prefix, member_func);
		if (func_name == NULL) {
		    err(31, __func__, "alloc_c_funct_name for member_func returned NULL");
		    not_reached();
		}
		is_unique = append_unique_str(unique_tbl, func_name);
		if (is_unique == false) {
		    /* we previously saw this function name, no need to reprint function decl */
		    free(func_name);
		    func_name = NULL;
		    continue;
		}

		/* print start of function declaration */
		prstr("extern bool ");

		/* print -M func name */
		print("%s", func_name);

		/* print end of function declaration */
		prstr("(struct json const *node,\n\tunsigned int depth, "
		      "struct json_sem *sem, struct json_sem_val_err **val_err);\n");

	    /* without -M member_func print member name if we have one */
	    } else if (p->name != NULL) {

		/* form function name based on JSON member name */
		func_name = alloc_c_funct_name(prefix, p->name);
		if (func_name == NULL) {
		    err(32, __func__, "alloc_c_funct_name for JSON member name returned NULL");
		    not_reached();
		}
		is_unique = append_unique_str(unique_tbl, func_name);
		if (is_unique == false) {
		    /* we previously saw this function name, no need to reprint function decl */
		    free(func_name);
		    func_name = NULL;
		    continue;
		}

		/* print start of function declaration */
		prstr("extern bool ");

		/* print function name */
		print("%s", func_name);

		/* print end of function declaration */
		prstr("(struct json const *node,\n\tunsigned int depth, "
		      "struct json_sem *sem, struct json_sem_val_err **val_err);\n");

	    }

	    /* otherwise nothing to do */

	/*
	 * case: JSON non-MEMBER with a validation function
	 *
	 * In the non-JTYPE_MEMBER type case, we print either a general validation function,
	 * or default validation function, or just NULL, where is no validation function.
	 *
	 * print extern for the validate_function if we have one
	 */
	} else if (validate != NULL) {

	    /* form function name based validation function */
	    func_name = alloc_c_funct_name(prefix, validate);
	    if (func_name == NULL) {
		err(33, __func__, "alloc_c_funct_name for validation function name returned NULL");
		not_reached();
	    }
	    is_unique = append_unique_str(unique_tbl, func_name);
	    if (is_unique == false) {
		/* we previously saw this function name, no need to reprint function decl */
		free(func_name);
		func_name = NULL;
		continue;
	    }

	    /* print start of function declaration */
	    prstr("extern bool ");

	    /* print validation function name */
	    print("%s", func_name);

	    /* print end of function declaration */
	    prstr("(struct json const *node,\n\tunsigned int depth, "
		  "struct json_sem *sem, struct json_sem_val_err **val_err);\n");
	}

	/*
	 * case: JSON non-MEMBER without a validation function
	 *
	 * nothing to do
	 */
    }

    /*
     * free dynamic array of unique function names
     */
    unique_len = dyn_array_tell(unique_tbl);
    for (i=0; i < unique_len; ++i) {
	u = dyn_array_value(unique_tbl, char *, i);
	if (u != NULL) {
	    free(u);
	}
    }
    dyn_array_free(unique_tbl);

    /*
     * print semantic table trailer
     */
    print("\n#endif /* %s_LEN */\n", cap_tbl_name);

    return;
}


/*
 * find_member - find an entry in a word table
 *
 * given:
 *	table		table of words to scan
 *	string		word to find in the table
 *
 * returns:
 *	NULL ==> string is not in the table,
 *	!= NULL ==> table entry that matches string
 */
static Word *
find_member(Word *table, const char *string)
{
	Word *w;
	for (w = table; w->length != 0; w++) {
		if (strcmp(string, w->word) == 0) {
			return w;
		}
	}
	return NULL;
}


/*
 * test_reserved - if string is a reserved word in C
 *
 * given:
 *	string	the string to check
 *
 * returns:
 *	true ==> string is a reserved word in C
 *	false ==> string is NOT a reserved word in C or is NULL
 */
static bool
test_reserved(const char *string)
{
    static Word *found = NULL;

    /*
     * firewall
     */
    if (string == NULL) {
	/* NULL pointer is not a reserved word */
	return false;
    }

    /*
     * search the reserved word table
     */
    found = find_member(cwords, string);
    if (found == NULL) {
	/* case: string is not a reserved word */
	return false;
    }
    /* case: string is a reserved word */
    return true;
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
		  DBG_DEFAULT, json_verbosity_level, json_parser_version, JSEMTBLGEN_VERSION);
    exit(exitcode); /*ooo*/
    not_reached();
}
