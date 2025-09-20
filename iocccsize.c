/*
 * iocccsize - IOCCC Source Size Tool
 *
 *	"You are not expected to understand this" :-)
 *
 *	Public Domain 1992, 2015, 2018, 2019, 2021 by Anthony Howe.  All rights released.
 *	With IOCCC mods in 2019-2023, 2025 by chongo (Landon Curt Noll) ^oo^
 *
 * SYNOPSIS
 *
 *	usage: iocccsize [-h] [-i] [-v level] [-V] prog.c
 *	usage: iocccsize [-h] [-i] [-v level] [-V] < prog.c
 *
 *	-i		ignored for backward compatibility
 *	-h		print usage message in stderr and exit 2
 *	-v level	set debug level (def: none)
 *	-V		print version and exit 3
 *
 *	Exit codes:
 *		0   source code is within Rule 2a and Rule 2b limits
 *		1   source code larger than Rule 2a and/or Rule 2b limits
 *		2   -h used and help printed
 *		3   -V used and version printed
 *		4   invalid command line
 *		6   there is no Rule 6!
 *	    >= 10   some internal error occurred
 *
 * DESCRIPTION
 *
 *	By default, the Rule 2b count is written to stdout.
 *	If the debug level is > 0, then the Rule 2a, Rule 2b,
 *	and keyword count is written to stdout instead.
 *
 *	The entry's gross size in bytes must be less than equal to the
 *	RULE_2A_SIZE value as defined in soup/limit_ioccc.h.
 *
 *	The entry's net size in bytes must be less than equal to the
 *	RULE_2B_SIZE value as defined in soup/limit_ioccc.h.
 *
 *	The size tool counts most C reserved words (keyword, secondary,
 *	and selected preprocessor keywords) as 1.  The size tool counts all
 *	other octets as 1 excluding ASCII whitespace, and excluding any
 *	';', '{' or '}' followed by ASCII whitespace, and excluding any
 *	';', '{' or '}' octet immediately before the end of file.
 */

/*
 * IOCCC Judge's remarks:
 *
 * This code contains undocumented features.  On the other hand, this code
 * is RTFS (for certain values of RTFS).  One might say that this code
 * perfectly documents itself.  :-)
 *
 * Many thanks to Anthony Howe for taking the time to put his OCD
 * (Obfuscated Coding Determination) into this code!
 */

#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <locale.h>

/*
 * For use in the https://github.com/ioccc-src/mkiocccentry repo
 */
#if defined(MKIOCCCENTRY_USE)

  /*
   * c_bool - sure that bool, true, and false are understood by the C compiler
   */
  #include "dbg/c_bool.h"

  /*
   * limit_ioccc - IOCCC size and rule related limitations
   */
  #include "soup/limit_ioccc.h"

  /*
   * version - official IOCCC toolkit versions
   */
  #include "soup/version.h"

  /*
   * iocccsize_err.h - debug, warning and error compat routines
   */
  #include "soup/iocccsize_err.h"

  /*
   * location - location/country codes and set the IOCCC locale
   */
  #include "soup/location.h"

  /*
   * iocccsize - IOCCC Source Size Tool
   */
  #include "iocccsize.h"

/*
 * For use in the https://github.com/SirWumpus/iocccsize repo
 */
#else /* MKIOCCCENTRY_USE */

  #include "iocccsize_err.h"

#endif /* MKIOCCCENTRY_USE */


/*
 * usage message, split into strings that are small enough to be supported by C standards
 */
static char usage0[] =
"usage: iocccsize [-h] [-i] [-v level] [-q] [-V] prog.c\n"
"usage: iocccsize [-h] [-i] [-v level] [-q] [-V] < prog.c\n"
"\n"
"\t-i\t\tignored for backward compatibility\n"
"\t-h\t\tprint usage message in stderr and exit\n"
"\t-v level\tset debug level (def: none)\n"
"\t-q\t\tquiet mode: silence msg(), warn(), warnp() if -v 0 (def: not msg_warn_silent)\n"
"\t-V\t\tprint version and exit\n"
"\n";
static char usage1[] =
"\tBy default, the Rule 2b count is written to stdout.\n"
"\tIf the debug level is > 0, then the Rule 2a, Rule 2b,\n"
"\tand keyword count is written to stdout instead.\n"
"\n"
"Exit codes:\n"
"     0   source code is within Rule 2a and Rule 2b limits\n"
"     1   source code larger than Rule 2a and/or Rule 2b limits\n"
"     2   -h used and help printed\n"
"     3   -V used and version printed\n"
"     4   invalid command line\n"
"     6   there is no Rule 6!\n"
" >= 10   some internal error occurred\n";


int
main(int argc, char **argv)
{
	extern char *optarg;		/* option argument */
	FILE *fp = stdin;		/* stream from which to determine sizes */
	RuleCount count;		/* rule_count() processing results */
	int ch;

#if defined(MKIOCCCENTRY_USE)
	/* IOCCC requires use of C locale */
	set_ioccc_locale();
#endif /* MKIOCCCENTRY_USE */

	while ((ch = getopt(argc, argv, "6ihv:aV")) != -1) {
		switch (ch) {
		case 'i': /* ignored for backward compatibility */
			break;

		case 'v':
			errno = 0;
			verbosity_level = (int)strtol(optarg, NULL, 0);
			if (errno != 0) {
			    iocccsize_errx(4, "cannot parse -v arg: %s", optarg);
			    not_reached();
			}
			break;

		case 'q':
			msg_warn_silent = true;
			break;

		case 'V':
			printf("%s\n", iocccsize_version);
			exit(3); /*ooo*/

		case '6': /* You're a RTFS master!  Congrats. */
			iocccsize_errx(6, "There is NO... Rule 6!  I'm not a number!  I'm a free(void *man)!"); /*ooo*/
			not_reached();

		case 'h':
			fprintf(stderr, "%s%s", usage0, usage1);
			fprintf(stderr, "\niocccsize version: %s\n", iocccsize_version);
			exit(2); /*ooo*/
			break;

		default:
			fprintf(stderr, "unknown -option\n");
			fprintf(stderr, "%s%s", usage0, usage1);
			exit(4); /*ooo*/
			break;
		}
	}

	if (optind + 1 == argc) {
		/* Redirect stdin to file path argument. */
		errno = 0;
		fp = fopen(argv[optind], "r");
		if (fp == NULL) {
			iocccsize_errx(6, "fopen(%s) failed", argv[optind]); /*ooo*/
			not_reached();
		}
	} else if (optind != argc) {
		/* Too many arguments. */
		fprintf(stderr, "%s%s", usage0, usage1);
		exit(4); /*ooo*/
		not_reached();
	}

	(void) setvbuf(fp, NULL, _IOLBF, 0);

	/* The Count - 1 Muha .. 2 Muhaha .. 3 Muhahaha ... */
	count = rule_count(fp);
	if (verbosity_level == 0) {
		(void) printf("%ju\n", (uintmax_t)count.rule_2b_size);
	} else {
		(void) printf("%ju %ju %ju\n", (uintmax_t)count.rule_2b_size, (uintmax_t)count.rule_2a_size,
					       (uintmax_t)count.keywords);
	}

	/*
	 * issue warnings
	 */
	if (1 < verbosity_level && 0 < count.char_warning) {
		iocccsize_warnx("Warning: character(s) with high bit set found! Be careful you don't violate rule 13!");
	}
        if (1 < verbosity_level && count.nul_warning) {
		iocccsize_warnx("Warning: NUL character(s) found! Be careful you don't violate rule 13!");
	}
	if (count.trigraph_warning) {
		iocccsize_warnx("Warning: unknown or invalid trigraph(s) found! Is that a bug in, or a feature of your code?");
	}
        if (1 < verbosity_level && 0 < count.wordbuf_warning) {
		iocccsize_warnx("Warning: word buffer overflow! Is that a bug in, or a feature of your code?");
	}
	if (count.ungetc_warning) {
		iocccsize_warnx("Warning: ungetc error: @SirWumpus goofed. The count on stdout may be invalid under rule 2!");
	}
	if (count.rule_2a_size > RULE_2A_SIZE) {
		iocccsize_warnx("Warning: your source under Rule 2a: %ju exceeds Rule 2a limit: %ju: Rule 2a violation!\n",
			        (uintmax_t)count.rule_2a_size, (uintmax_t)RULE_2A_SIZE);
	}
	if (count.rule_2b_size > RULE_2B_SIZE) {
		iocccsize_warnx("Warning: your source under Rule 2b: %ju exceeds Rule 2b limit: %ju: Rule 2b violation!\n",
				(uintmax_t)count.rule_2b_size, (uintmax_t)RULE_2B_SIZE);
	}

	/*
	 * All Done!!! All Done!!! -- Jessica Noll, Age 2
	 */
	if ((count.rule_2a_size > RULE_2A_SIZE) || (count.rule_2b_size > RULE_2B_SIZE)) {
		exit(1); /*ooo*/
	}
	exit(0); /*ooo*/
}
