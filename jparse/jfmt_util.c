/*
 * jfmt_util - utility functions for JSON printer jfmt
 *
 * "Because specs w/o version numbers are forced to commit to their original design flaws." :-)
 *
 * This tool is being developed by:
 *
 *	@xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 *
 * The JSON parser was co-developed in 2022 by Cody and Landon.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */

/* special comments for the seqcexit tool */
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */

#include "jfmt_util.h"

/* alloc_jfmt	    - allocate a struct jfmt *, clear it out and set defaults
 *
 * This function returns a newly allocated and cleared struct jfmt *.
 *
 * This function will never return a NULL pointer.
 */
struct jfmt *
alloc_jfmt(void)
{
    /* allocate our struct jfmt */
    struct jfmt *jfmt = calloc(1, sizeof *jfmt);

    /* verify jfmt != NULL */
    if (jfmt == NULL) {
	err(22, __func__, "failed to allocate jfmt struct");
	not_reached();
    }

    /* explicitly clear everything out and set defaults */

    /* JSON file member variables */
    jfmt->common.is_stdin = false;			/* true if it's stdin */
    jfmt->common.file_contents = NULL;			/* file.json contents */
    jfmt->common.json_file = NULL;			/* JSON file * */
    jfmt->common.json_file_path = NULL;			/* path to JSON file to read */

    jfmt->common.outfile_path = NULL;			/* assume no -o used */
    jfmt->common.outfile = stdout;			/* default stdout */
    jfmt->common.outfile_not_stdout = false;		/* by default we write to stdout */

    /* number range options, see struct jfmt_number_range in jfmt_util.h for details */

    /* levels number range */
    jfmt->common.json_util_levels.number = 0;
    jfmt->common.json_util_levels.exact = false;
    jfmt->common.json_util_levels.range.min = 0;
    jfmt->common.json_util_levels.range.max = 0;
    jfmt->common.json_util_levels.range.less_than_equal = false;
    jfmt->common.json_util_levels.range.greater_than_equal = false;
    jfmt->common.json_util_levels.range.inclusive = false;
    jfmt->common.levels_constrained = false;

    /* print related options */
    jfmt->common.print_json_levels = false;			/* -L specified */
    jfmt->common.num_level_spaces = 4;			/* number of spaces or tab for -L */
    jfmt->common.print_level_tab = false;			/* -L tab option */
    jfmt->common.indent_levels = false;			/* -I used */
    jfmt->common.indent_spaces = 4;				/* -I number of tabs or spaces */
    jfmt->common.indent_tab = false;				/* -I <num>[{t|s}] specified */

    /* parsing related */
    jfmt->common.max_depth = JSON_DEFAULT_MAX_DEPTH;		/* max depth to traverse set by -m depth */

    return jfmt;
}

/*
 * free_jfmt	    - free jfmt struct
 *
 * given:
 *
 *	jfmt	    - a struct jfmt **
 *
 * This function will do nothing other than warn on NULL pointer (even though
 * it's safe to free a NULL pointer though if jfmt itself was NULL it would be
 * an error to dereference it).
 *
 * We pass a struct jfmt ** so that in the caller jfmt can be set to NULL to
 * remove the need to repeatedly set it to NULL each time this function is
 * called. This way we remove the need to do more than just call this function.
 */
void
free_jfmt(struct jfmt **jfmt)
{
    /* firewall */
    if (jfmt == NULL || *jfmt == NULL) {
	warn(__func__, "passed NULL struct jfmt ** or *jfmt is NULL");
	return;
    }

    /* flush output file if open and then close it */
    if ((*jfmt)->common.outfile != NULL && (*jfmt)->common.outfile != stdout) {
	fflush((*jfmt)->common.outfile);
	fclose((*jfmt)->common.outfile);
	(*jfmt)->common.outfile = NULL;
	(*jfmt)->common.outfile_path = NULL;
    }

    free(*jfmt);
    *jfmt = NULL;
}
