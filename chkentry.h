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
 * This tool is being co-developed by Cody Boone Ferguson and Landon Curt Noll
 * and the concept of this file was developed by Landon Curt Noll.
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 */


#if !defined(INCLUDE_CHKENTRY_H)
#    define  INCLUDE_CHKENTRY_H


/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#include "dbg.h"

/*
 * json_util - general JSON parser utility support functions
 */
#include "json_util.h"

/*
 * util - entry common utility functions for the IOCCC toolkit
 */
#include "util.h"

/*
 * time - for time specific things
 */
#include <time.h>

/*
 * non-strict match to 1 part in MATCH_PRECISION
 */
#define MATCH_PRECISION ((long double)(1<<22))


/*
 * usage message
 *
 * Use the usage() function to print the usage_msg([0-9]?)+ strings.
 */
static const char * const usage_msg =
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] [-F fnamchk] entry_dir\n"
    "usage: %s [-h] [-v level] [-J level] [-V] [-q] [-F fnamchk] info.json author.json\n"
    "\n"
    "\t-h\t\tprint help message and exit 0\n"
    "\t-v level\tset verbosity level (def level: %d)\n"
    "\t-J level\tset JSON verbosity level (def level: %d)\n"
    "\t-V\t\tprint version string and exit 3\n"
    "\t-q\t\tquiet mode (def: not quiet)\n"
    "\t\t\t    NOTE: -q will also silence msg(), warn(), warnp() if -v 0\n"
    "\t-F fnamchk\tpath to fnamchk tool (def: %s)\n"
    "\n"
    "entry_dir\tIOCCC entry directory with .info.json and author.json files\n"
    "info.json\tcheck info.json file, . ==> skip IOCCC info check\n"
    "author.json\tcheck author.json file, . ==> skip IOCCC author check\n"
    "\n"
    "Exit codes:\n"
    "    0\tall is OK\n"
    "    3\t-h and help string printed or -V and version string printed\n"
    "    4\tcommand line error\n"
    "    >=5\tinternal error\n"
    "\n"
    "chkentry version: %s\n";


/*
 * globals
 */
bool quiet = false;				/* true ==> quiet mode */


/*
 * externals
 */

/*
 * function prototypes
 */
static void usage(int exitcode, char const *prog, char const *str, int expected, int argc) __attribute__((noreturn));


/* Special stuff you're not supposed to know about. :-( */
static void Bfgrerv(int d, int y);
static int D = -1, Y = -1;
static time_t t;
static char const *Bfgrexbeo[] =
{
    "'yrff' vf yrkvpbtencuvpnyyl terngre guna 'terngre'. '<', ubjrire,\n"
    "vf yrkvpbtencuvpnyyl yrff guna '>'. Fbzrbar fperjrq hc.\n-- Krkly",
    "V qba'g xabj unys bs lbh unys nf jryy nf V fubhyq yvxr;\nnaq V yvxr yrff guna"
    " unys bs lbh unys nf jryy nf lbh qrfreir.\n-- Ovyob Onttvaf",
    "P frpgvba: '{}'\n-- Krkly\n",
    "'Frpbaqyl, gb pryroengr BHE oveguqnlf: zvar naq zl ubabhenoyr naq tnyynag\n"
    "sngure'f.' Hapbzsbegnoyr naq nccerurafvir fvyrapr. 'V nz bayl unys gur zna\n"
    "gung ur vf: V nz 72, ur vf 144. Lbhe ahzoref ner pubfra gb qb ubabhe gb\n"
    "rnpu bs uvf ubabhenoyr lrnef.' Guvf jnf ernyyl qernqshy -- n erthyne\n"
    "oenvagjvfgre, naq fbzr bs gurz sryg vafhygrq, yvxr yrnc-qnlf fubirq\n"
    "va gb svyy hc n pnyraqne.\n-- Ovatb Onttvaf",
    "P havbaf: ybivat pbhcyrf jub rawbl cebtenzzvat va P gbtrgure.\n-- Krkly\n",
    "'Ybbx,' fnvq Neguhe, 'jbhyq vg fnir lbh n ybg bs gvzr vs V whfg tnir\nhc naq jrag znq abj?'\n",
    "Gurer vf na neg, be, engure, n xanpx gb sylvat. Gur xanpx yvrf va\nyrneavat ubj gb guebj lbhefrys ng gur tebhaq naq zvff.\n",
    "SHA SNPG: Va O naq rneyl P, gur += bcrengbe jnf fcryg =+ vafgrnq bs += ;\nguvf zvfgnxr, ercnverq va 1976, jnf vaqhprq ol n frqhpgviryl rnfl jnl bs\nunaqyvat gur svefg sbez va O'f yrkvpny nanylfre.\n\nFrr zber sha snpgf naq uvfgbel ol Qraavf Evgpuvr uvzfrys ng\nuggcf://jjj.oryy-ynof.pbz/hfe/qze/jjj/puvfg.ugzy.\n",
    NULL
};

#endif /* INCLUDE_CHKENTRY_H */
