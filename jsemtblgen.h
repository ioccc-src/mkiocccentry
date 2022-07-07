/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * jsemtblgen - generate JSON semantics table
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This JSON parser was co-developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 */


#if !defined(INCLUDE_JSEMTBLGEN_H)
#    define  INCLUDE_JSEMTBLGEN_H


#include <ctype.h>

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * dyn_array - dynamic array facility
 */
#include "dyn_array.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * jparse - JSON parser demo tool
 */
#include "jparse.h"

/*
 * json_sem - JSON semantics support
 */
#include "json_sem.h"

/*
 * official jsemtblgen version
 */
#define JSEMTBLGEN_VERSION "0.3 2022-07-04"		/* format: major.minor YYYY-MM-DD */


/*
 * static functions
 */
static void gen_sem_tbl(struct json *tree, unsigned int max_depth, ...);
static void vupdate_tbl(struct json *node, unsigned int depth, va_list ap);
static int sem_cmp(void const *a, void const *b);
static void print_sem_tbl(struct dyn_array *tbl, char *tbl_name, char *cap_tbl_name);
static void usage(int exitcode, char const *str, char const *prog);


/*
 * usage message
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-q] [-V] [-s] [-N name] [-D def_func] [-P prefix]\n"
    "\t\t    [-1 func] [-S func] [-B func] [-0 func] [-M func] [-O func] [-A func] [-U func] json_arg\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not quiet)\n"
    "\t-V\t\tprint version string and exit 0\n"
    "\t-s\t\targ is a string (def: arg is a filename)\n"
    "\n"
    "\t-N name\t\tname of the semantics table (def: sem_tbl)\n"
    "\n"
    "\t-D def_func\tvalidate with def_func() unless overridden by another flag (def: NULL)\n"
    "\n"
    "\t-P prefix\tvalidate JTYPE_MEMBER JSON nodes with prefix_name() (def: do not)\n"
    "\n"
    "\t\t\tNOTE: The name is based on the JTYPE_MEMBER JSON decoded name string.\n"
    "\t\t\tNOTE: Underscore (_) replaces any name chars that are not valid in a C function name.\n"
    "\t\t\tNOTE: -P overrides any use of -M.\n"
    "\n"
    "\t-1 func\t\tvalidate JTYPE_NUMBER JSON nodes with func() (def: do not)\n"
    "\t-S func\t\tvalidate JTYPE_STRING JSON nodes with func() (def: do not)\n"
    "\t-B func\t\tvalidate JTYPE_BOOL JSON nodes with func() (def: do not)\n"
    "\t-0 func\t\tvalidate JTYPE_NULL JSON nodes with func() (def: do not)\n"
    "\t-M func\t\tvalidate JTYPE_MEMBER JSON nodes with func() (def: do not)\n"
    "\t-O func\t\tvalidate JTYPE_OBJECT JSON nodes with func() (def: do not)\n"
    "\t-A func\t\tvalidate JTYPE_ARRAY JSON nodes with func() (def: do not)\n"
    "\t-U func\t\tvalidate nodes with unknown types with func() (def: do not)\n"
    "\n"
    "\tjson_arg\tgenerate JSON semantics table for string (if -s), file (w/o -s), or stdin (if arg is -)\n"
    "\n"
    "Exit codes:\n"
    "    0\tJSON is valid\n"
    "    1\tJSON is invalid\n"
    "    2\t-h and help string printed or -V and version string printed\n"
    "    3\tcommand line error\n"
    "    >=4\tinternal error\n"
    "\n"
    "JSON parser version: %s\n"
    "jsemtblgen version: %s\n";


#endif /* INCLUDE_JSEMTBLGEN_H */
