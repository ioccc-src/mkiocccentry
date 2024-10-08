/*
 * mkiocccentry - form IOCCC entry compressed tarball
 *
 * Make an IOCCC compressed tarball for an IOCCC entry.
 *
 * We will form the IOCCC entry compressed tarball "by hand" in C.
 * Not in some high level language, but standard vanilla (with a healthy
 * overdose of chocolate :-) ) C.  Why? Because this is an obfuscated C contest.
 * But then why isn't this code obfuscated?  Because the IOCCC judges prefer to
 * write in robust unobfuscated code.  Besides, the IOCCC was started as an
 * ironic commentary on the Bourne shell source and finger daemon source.
 * Moreover, irony is well baked-in to the IOCCC.  :-)
 *
 * If you do find a problem with this code, please let the judges know.
 * To contact the judges please see:
 *
 *      https://www.ioccc.org/contact.html
 *
 * "Because even printf has a return value worth paying attention to." :-)
 *
 * Many thanks are due to a number of people who provided important
 * and valuable testing, suggestions, issue reports and GitHub pull
 * requests to this code.  Without their time and effort, this tool
 * would not work very well!
 *
 * Among the GitHub users we wish to thank include these fine developers
 * in alphabetical GitHub @user order:
 *
 *	@ilyakurdyukov		Ilya Kurdyukov
 *	@SirWumpus		Anthony Howe
 *	@vog			Volker Diels-Grabsch
 *	@xexyl			Cody Boone Ferguson
 *
 * Copyright (c) 2021-2024 by Landon Curt Noll.  All Rights Reserved.
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


#if !defined(INCLUDE_MKIOCCCENTRY_H)
#    define  INCLUDE_MKIOCCCENTRY_H


/*
 * jparse/version - JSON parser API and tool version
 */
#include "jparse/version.h"


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg/dbg.h"

/*
 * location - location/country codes
 */
#include "soup/location.h"

/*
 * default_handle - translate UTF-8 into handle that is POSIX portable and + chars
 */
#include "soup/default_handle.h"

/*
 * sanity - perform common IOCCC sanity checks
 */
#include "soup/sanity.h"

/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "soup/limit_ioccc.h"

/*
 * iocccsize use
 */
#include "iocccsize.h"

/*
 * version - official IOCCC toolkit versions
 */
#include "soup/version.h"

/*
 * entry_util - utilities supporting mkiocccentry JSON files
 */
#include "soup/entry_util.h"

/*
 * mkiocccentry tool basename
 */
#define MKIOCCCENTRY_BASENAME "mkiocccentry"


/*
 * definitions
 *
 * NOTE: MKIOCCCENTRY_ANSWERS_VERSION and MKIOCCCENTRY_ANSWERS_EOF must be defined
 *	 in mkiocccentry.c and not here.
 */
#define ISO_3166_1_CODE_URL0 "\thttps://en.wikipedia.org/wiki/ISO_3166-1#Officially_assigned_code_elements"
#define ISO_3166_1_CODE_URL1 "\thttps://en.wikipedia.org/wiki/ISO_3166-1_alpha-2"
#define ISO_3166_1_CODE_URL2 "\thttps://www.iso.org/obp/ui/#iso:pub:PUB500001:en"
#define ISO_3166_1_CODE_URL3 "\thttps://www.iso.org/obp/ui/#search"
#define ISO_3166_1_CODE_URL4 "\thttps://www.iso.org/glossary-for-iso-3166.html"

#undef IOCCC_WINNER_HANDLE_READY /* XXX: change to #define when IOCCC winner handles are available and ready */

#define IOCCC_REGISTER_URL "https://register.ioccc.org/NOT/a/real/URL"	/* XXX: change to real URL when ready */
#define IOCCC_SUBMIT_URL "https://submit.ioccc.org/NOT/a/real/URL"	/* XXX: change to real URL when ready */
#define IOCCC_NEWS_URL "https://www.ioccc.org/index.html#news" /* XXX: change to news.html after the Great Fork Merge is done */
#undef IOCCC_REGISTRATION_READY		/* XXX: change to #define when registration process is ready */
#undef IOCCC_SUBMIT_SERVER_READY	/* XXX: change to #define when submit server is ready */

/* NOTE: The next two are for the warn_rule_2a_size() function. Do **NOT** change these values! */
#define RULE_2A_BIG_FILE_WARNING (0)	/* warn that prog.c appears to be too big under Rule 2a */
#define RULE_2A_IOCCCSIZE_MISMATCH (1)	/* warn that prog.c iocccsize size differs from the file size */



/*
 * forward declarations
 */
static void warn_empty_prog(char const *prog_c);
static void warn_rule_2a_size(struct info *infop, char const *prog_c, int mode, RuleCount size);
static void warn_high_bit(char const *prog_c);
static void warn_nul_chars(char const *prog_c);
static void warn_trigraph(char const *prog_c);
static void warn_wordbuf(char const *prog_c);
static void warn_ungetc(char const *prog_c);
static void warn_rule_2b_size(struct info *infop, char const *prog_c);
static RuleCount check_prog_c(struct info *infop, char const *submission_dir, char const *cp, char const *prog_c);
static void mkiocccentry_sanity_chks(struct info *infop, char const *work_dir, char const *tar, char const *cp,
				     char const *ls, char const *txzchk, char const *fnamchk, char const *chkentry);
static char *prompt(char const *str, size_t *lenp);
static char *get_contest_id(bool *testp, bool *read_answers_flag_used);
static int get_submit_slot(struct info *infop);
static char *mk_submission_dir(char const *work_dir, char const *ioccc_id, int submit_slot,
			  char **tarball_path, time_t tstamp, bool test_mode);
static bool inspect_Makefile(char const *Makefile, struct info *infop);
static void warn_Makefile(char const *Makefile, struct info *infop);
static void check_Makefile(struct info *infop, char const *submission_dir, char const *cp, char const *Makefile);
static void check_remarks_md(struct info *infop, char const *submission_dir, char const *cp, char const *remarks_md);
static void check_extra_data_files(struct info *infop, char const *submission_dir, char const *cp, int count, char **args);
static bool yes_or_no(char const *question);
static char *get_title(struct info *infop);
static char *get_abstract(struct info *infop);
static int get_author_info(struct author **author_set_p);
static void verify_submission_dir(char const *submission_dir, char const *ls);
static void write_info(struct info *infop, char const *submission_dir, char const *chkentry, char const *fnamchk);
static void form_auth(struct auth *authp, struct info *infop, int author_count, struct author *authorp);
static void write_auth(struct auth *authp, char const *submission_dir, char const *chkentry, char const *fnamchk);
static void form_tarball(char const *work_dir, char const *submission_dir, char const *tarball_path, char const *tar,
			 char const *ls, char const *txzchk, char const *fnamchk);
static void remind_user(char const *work_dir, char const *submission_dir, char const *tar, char const *tarball_path,
	bool test_mode);
static void show_registration_url(void);
static void show_submit_url(char const *work_dir, char const *tarball_path);
#endif /* INCLUDE_MKIOCCCENTRY_H */
