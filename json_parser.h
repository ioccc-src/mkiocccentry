/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * This tool is currently being worked on by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * ...and it is VERY incomplete! It's easy to trigger a parser error because
 * it's incomplete and there's a lot to change and do as I learn more about flex
 * and bison. It's a work in progress that will be developed over some time.
 *
 */


#if !defined(INCLUDE_JSON_PARSER_H)
#    define  INCLUDE_JSON_PARSER_H



/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"


/*
 * util - utility functions and definitions
 */
#include "util.h"

/*
 * json - json file structs
 */
#include "json.h"

/*
 * sanity - sanity checks on the IOCCC toolkit
 */
#include "sanity.h"

/*
 * IOCCC size and rule related limitations
 */
#include "limit_ioccc.h"

/*
 * globals
 */
int verbosity_level = DBG_DEFAULT;	/* debug level set by -v */
bool msg_output_allowed = true;		/* false ==> disable output from msg() */
bool dbg_output_allowed = true;		/* false ==> disable output from dbg() */
bool warn_output_allowed = true;	/* false ==> disable output from warn() and warnp() */
bool err_output_allowed = true;		/* false ==> disable output from err() and errp() */
bool usage_output_allowed = true;	/* false ==> disable output from vfprintf_usage() */
static char const *program = NULL;	    /* our name */
static bool quiet = false;		    /* true ==> quiet mode */
static bool strict = false;		    /* true ==> be more restrictive on what's allowed */

#endif /* INCLUDE_JSON_PARSER_H */
