/*
 * iocccsize - IOCCC Source Size Tool
 *
 *	"You are not expected to understand this" :-)
 *
 *	Public Domain 1992, 2015, 2018, 2019, 2021 by Anthony Howe.  All rights released.
 *	With IOCCC mods in 2019-2021 by chongo (Landon Curt Noll) ^oo^
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
 *		0 - source code is within Rule 2a and Rule 2b limits
 *		1 - source code larger than Rule 2a and/or Rule 2b limits
 *		2 - -h used and help printed
 *		3 - -V used and version printed
 *		4 - invalid command line
 *		>= 5 - some internal error occurred
 *
 * DESCRIPTION
 *
 *	By default,the Rule 2b count is written to stdout.
 *	If the debug level is > 0, then the Rule 2a, Rule 2b,
 *	and keyword count is written to stdout instead.
 *
 *	The entry's gross size in bytes must be less than equal to 4096
 *	bytes in length.
 *
 *	The entry's net size in bytes must be less than equal to 2503
 *	bytes (first prime after 2048).  The net size is computed as
 *	follows:
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

#include "iocccsize_err.h"
#if defined(MKIOCCCENTRY_USE)
#include "limit_ioccc.h"
#endif /* MKIOCCCENTRY_USE */
#include "iocccsize.h"

/*
 * usage message, split into strings that are small enough to be supported by C standards
 */
static char usage0[] =
"usage: iocccsize [-h] [-i] [-v level] [-V] prog.c\n"
"usage: iocccsize [-h] [-i] [-v level] [-V] < prog.c\n"
"\n"
"-i\t\tignored for backward compatibility\n"
"-h\t\tprint usage message in stderr and exit 2\n"
"-v level\tset debug level (def: none)\n"
"-V\t\tprint version and exit 3\n"
"\n";
static char usage1[] =
"\tBy default,the Rule 2b count is written to stdout.\n"
"\tIf the debug level is > 0, then the Rule 2a, Rule 2b,\n"
"\tand keyword count is written to stdout instead.\n"
"\n"
"Exit codes:\n"
"\t0 - source code is within Rule 2a and Rule 2b limits\n"
"\t1 - source code larger than Rule 2a and/or Rule 2b limits\n"
"\t2 - -h used and help printed\n"
"\t3 - -V used and version printed\n"
"\t4 - invalid command line\n"
"\t>= 5 - some internal error occurred\n";

int verbosity_level = 0;

int
main(int argc, char **argv)
{
	extern char *optarg;		/* option argument */
	FILE *fp = stdin;		/* stream from which to determine sizes */
	RuleCount count;		/* rule_count() processing results */
	int ch;

	while ((ch = getopt(argc, argv, "6ihv:V")) != -1) {
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

		case 'V':
			printf("%s\n", iocccsize_version);
			exit(3); /*ooo*/

		case '6': /* You're a RTFS master!  Congrats. */
			iocccsize_errx(6, "There is NO... Rule 6!  I'm not a number!  I'm a free(void *man)!"); /*ooo*/
			not_reached();

		case 'h':
			fprintf(stderr, "%s%s", usage0, usage1);
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
			fprintf(stderr, "fopen(%s) failed: %s\n", argv[optind], strerror(errno));
			exit(6); /*ooo*/
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
		(void) printf("%ld\n", (unsigned long)count.rule_2b_size);
	} else {
		(void) printf("%ld %ld %ld\n", (unsigned long)count.rule_2b_size, (unsigned long)count.rule_2a_size,
					       (unsigned long)count.keywords);
	}

	/*
	 * issue warnings
	 */
	if (count.char_warning) {
		iocccsize_warnx("Warning: character(s) with high bit set found! Be careful you don't violate rule 13!");
	}
	if (count.nul_warning) {
		iocccsize_warnx("Warning: NUL character(s) found! Be careful you don't violate rule 13!");
	}
	if (count.trigraph_warning) {
		iocccsize_warnx("Warning: unknown or invalid trigraph(s) found! Is that a bug in, or a feature of your code?");
	}
	if (count.wordbuf_warning) {
		iocccsize_warnx("Warning: word buffer overflow! Is that a bug in, or a feature of your code?");
	}
	if (count.ungetc_warning) {
		iocccsize_warnx("Warning: ungetc error: @SirWumpus goofed. The count on stdout may be invalid under rule 2!");
	}
	if (count.rule_2a_size > RULE_2A_SIZE) {
		iocccsize_warnx("Warning: your source under Rule 2a: %lu exceeds Rule 2a limit: %lu: Rule 2a violation!\n",
			        (unsigned long)count.rule_2a_size, (unsigned long)RULE_2A_SIZE);
	}
	if (count.rule_2b_size > RULE_2B_SIZE) {
		iocccsize_warnx("Warning: your source under Rule 2b: %lu exceeds Rule 2b limit: %lu: Rule 2b violation!\n",
				(unsigned long)count.rule_2b_size, (unsigned long)RULE_2B_SIZE);
	}

	/*
	 * All Done!!! All Done!!! -- Jessica Noll, age 2
	 */
	if ((count.rule_2a_size > RULE_2A_SIZE) || (count.rule_2b_size > RULE_2B_SIZE)) {
		exit(1); /*ooo*/
	}
	exit(0); /*ooo*/
}
