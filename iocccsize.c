/*
 * iocccsize - IOCCC Source Size Tool
 *
 *	"You are not expected to understand this" :-)
 *
 *	Public Domain 1992, 2015, 2018, 2019 by Anthony Howe.  All rights released.
 *	See:
 *		https://github.com/SirWumpus/iocccsize
 *	IOCCC mods in 2019-2022 by chongo (Landon Curt Noll) ^oo^
 *
 * SYNOPSIS
 *
 *	iocccsize [-ihvV] prog.c
 *	iocccsize [-ihvV] < prog.c
 *
 *	-i	ignored for backward compatibility
 *	-h	print usage message in stderr and exit 2
 *	-v	turn on some debugging to stderr; -vv or -vvv for more
 *	-V	print version string and exit 3
 *
 *	The IOCCC net count rule 2b is written to stdout; with -v, net count (Rule 2b),
 *	gross count (Rule 2a), number of keywords counted as 1 byte; -vv or -vvv write
 *	more tool diagnostics.
 *
 * DESCRIPION
 *
 *	Reading a C source file from standard input or a file arg, apply the IOCCC
 *	source size rules as explained in the Guidelines.
 *
 *	The source's Rule 2b length is written to stdout; with -v option the Rule 2b
 *	length, gross (Rule 2a) length, and matched keyword count are written to stdout.
 *
 *	The size tool counts most C reserved words (keyword, secondary,
 *	and selected preprocessor keywords) as 1.  The size tool counts all
 *	other octets as 1 excluding ASCII whitespace, and excluding any
 *	';', '{' or '}' followed by ASCII whitespace, and excluding any
 *	';', '{' or '}' octet immediately before the end of file.
 */

#define DIGRAPHS
#define TRIGRAPHS

/*
 * IOCCC Judge's remarks:
 *
 * This code contains undocumented features.  On the other hand, this code
 * is RTFS (for certain values of RTFS).  One might say that this code
 * perfectly documents itself.  :-)
 *
 * Many thanks to Anthony Howe for taking the time to put his OCD
 */

/*
 * HINT: The algorithm implemented by this code may or not be obfuscated.
 *       The algorithm may not or may appear to be obfuscated.
 *
 * In particular:
 *
 *      We did not invent the algorithm.
 *      The algorithm consistently finds Obfuscation.
 *      The algorithm killed Obfuscation.
 *      The algorithm is banned in C.
 *      The algorithm is from Bell Labs in Jersey.
 *      The algorithm constantly finds Obfuscation.
 *      This is not the algorithm.
 *      This is close.
 */

#include <err.h>
#include <ctype.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define VERSION "28.5 2022-01-16"	/* use format: major.minor YYYY-MM-DD */

#define WORD_BUFFER_SIZE	256
#define RULE_2A_SIZE		5120	/* IOCCC Rule 2a */
#define RULE_2B_SIZE		3217	/* IOCCC Rule 2b */
#define STRLEN(s)		(sizeof (s)-1)

#define NO_STRING		0
#define NO_COMMENT		0
#define COMMENT_EOL		1
#define COMMENT_BLOCK		2

static char usage0[] =
"usage: iocccsize [-h] [-i] [-v ...] [-V] prog.c\n"
"usage: iocccsize [-h] [-i] [-v ...] [-V] < prog.c\n"
"\n"
"-i\t\tignored for backward compatibility\n"
"-h\t\tprint usage message in stderr and exit 2\n"
"-v\t\tturn on some debugging to stderr; -vv or -vvv for more\n"
"-V\t\tprint version and exit 3\n"
"\n";
static char usage1[] =
"The IOCCC net count Rule 2b is written to stdout; with -v, net count (2b),\n"
"gross count (2a), number of keywords counted as 1 byte; -vv or -vvv write\n"
"more tool diagnostics.\n"
"\n"
"Exit codes:\n"
"\t0 - soruce code is within Rule 2a and Rule 2b limits\n"
"\t1 - soruce code larger than Rule 2a and/or Rule 2b limits\n"
"\t2 - -h used and help printed\n"
"\t3 - -V used and version printed\n"
"\t>= 4 - some internal error occurred\n"
;

static int debug;
static char *out_fmt = "%lu\n";
static int char_warning = 0;
static int trigraph_warning = 0;
static int wordbuf_warning = 0;
static int ungetc_warning = 0;
static int rule_2a_warning = 0;
static int rule_2b_warning = 0;

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

static int
read_ch(FILE *fp)
{
	int ch;

	while ((ch = fgetc(fp)) != EOF && ch == '\r') {
		/* Discard bare CR and those part of CRLF. */
	}
	if (ch == '\0' || 128 <= ch) {
		char_warning = 1;
	}
	return ch;
}

void
rule_count(FILE *fp)
{
	char word[WORD_BUFFER_SIZE+1];
	size_t gross_count = 0, net_count = 0, wordi = 0;
	int keywords = 0;
	int ch, next_ch, quote = NO_STRING, escape = 0, is_comment = NO_COMMENT;

/* If quote == NO_STRING (0) and is_comment == NO_COMMENT (0) then its code. */
#define IS_CODE	(quote == is_comment)

	while ((ch = read_ch(fp)) != EOF) {
		/* Future gazing. */
		next_ch = read_ch(fp);

#ifdef TRIGRAPHS
		if (ch == '?' && next_ch == '?') {
			/* ISO C11 section 5.2.1.1 Trigraph Sequences */
			const char *t;
			static const char trigraphs[] = "=#([)]'^<{!|>}-~/\\";

			ch = fgetc(fp);
			for (t = trigraphs; *t != '\0'; t += 2) {
				if (ch == t[0]) {
					/* Mapped trigraphs count as 1 byte. */
					next_ch = fgetc(fp);
					gross_count += 2;
					ch = t[1];
					break;
				}
			}

			/* Unknown trigraph, push back the 3rd character. */
			if (*t == '\0') {
				if (ch != EOF && ungetc(ch, fp) == EOF) {
					trigraph_warning = 1;
				}
				ch = '?';
			}
		}
#endif
		if (ch == '\\' && next_ch == '\n') {
			/* ISO C11 section 5.1.1.2 Translation Phases
			 * point 2 discards backslash newlines.
			 */
			gross_count += 2;
			continue;
		}

		if (next_ch != EOF && ungetc(next_ch, fp) == EOF) {
			/* ISO C ungetc() guarantees one character (byte) pushback.
			 * How does that relate to UTF8 and wide-character library
			 * handling?  An invalid trigraph results in 2x ungetc().
			 */
			ungetc_warning = 1;
		}

		/* Within quoted string? */
		if (quote != NO_STRING) {
			/* Escape _this_ character. */
			if (escape) {
				escape = 0;
			}

			/* Escape next character. */
			else if (ch == '\\') {
				escape = 1;
			}

			/* Close matching quote? */
			else if (ch == quote) {
				quote = NO_STRING;
			}
		}

		/* Within comment to end of line? */
		else if (is_comment == COMMENT_EOL && ch == '\n') {
			if (debug > 1) {
				(void) fprintf(stderr, "~~NO_COMMENT\n");
			}
			is_comment = NO_COMMENT;
		}

		/* Within comment block? */
		else if (is_comment == COMMENT_BLOCK && ch == '*' && next_ch == '/') {
			if (debug > 1) {
				(void) fprintf(stderr, "~~NO_COMMENT\n");
			}
			is_comment = NO_COMMENT;
		}

		/* Start of comment to end of line? */
		else if (is_comment == NO_COMMENT && ch == '/' && next_ch == '/') {
			if (debug > 1) {
				(void) fprintf(stderr, "~~COMMENT_EOL\n");
			}
			is_comment = COMMENT_EOL;

			/* Consume next_ch. */
			ch = fgetc(fp);
			gross_count++;
			net_count++;
		}

		/* Start of comment block? */
		else if (is_comment == NO_COMMENT && ch == '/' && next_ch == '*') {
			if (debug > 1) {
				(void) fprintf(stderr, "~~COMMENT_BLOCK\n");
			}
			is_comment = COMMENT_BLOCK;

			/* Consume next_ch. */
			ch = fgetc(fp);
			gross_count++;
			net_count++;
		}

		/* Open single or double quote? */
		else if (is_comment == NO_COMMENT && (ch == '\'' || ch == '"')) {
			quote = ch;
		}

#ifdef DIGRAPHS
		/* ISO C11 section 6.4.6 Punctuators, digraphs handled during
		 * tokenization, but map here and count as 1 byte, like their
		 * ASCII counter parts.
		 */
		if (IS_CODE) {
			const char *d;
			static const char digraphs[] = "[<:]:>{<%}%>#%:";
			for (d = digraphs; *d != '\0'; d += 3) {
				if (ch == d[1] && next_ch == d[2]) {
					(void) fgetc(fp);
					gross_count++;
					ch = d[0];
					break;
				}
			}
		}
#endif
		/* Sanity check against file size and wc(1) byte count. */
		gross_count++;

		/* End of possible keyword?  Care with #word as there can
		 * be whitespace or comments between # and word.
		 */
		if ((word[0] != '#' || 1 < wordi) && !isalnum(ch) && ch != '_' && ch != '#') {
			if (find_member(cwords, word) != NULL) {
				/* Count keyword as 1. */
				net_count = net_count - wordi + 1;
				keywords++;
				if (debug > 1) {
					(void) fprintf(stderr, "~~keyword %d \"%s\"\n", keywords, word);
				}
			}
			word[wordi = 0] = '\0';
		}

		/* Ignore all whitespace. */
		if (isspace(ch)) {
			if (debug > 2) {
				(void) fprintf(stderr, "~~ignore whitespace %#02x\n", ch);
			}
			continue;
		}

		/* Ignore begin/end block and end of statement. */
		if (strchr("{;}", ch) != NULL && (isspace(next_ch) || next_ch == EOF)) {
			if (debug > 2) {
				(void) fprintf(stderr, "~~ignore %c\n", ch);
			}
			continue;
		}

		/* Collect next word not in a string or comment. */
		if (IS_CODE && (isalnum(ch) || ch == '_' || ch == '#')) {
			if (sizeof (word) <= wordi) {
				wordbuf_warning = 1;
				wordi = 0;
			}
			word[wordi++] = (char) ch;
			word[wordi] = '\0';
		}

		net_count++;
	}

	/*
	 * The original author was not entirely in agreement with printing
	 * these warnings, since he believes that its the programmer's job to
	 * be cognisant of the rules, guidelines, and the state of their work.
	 *
	 * The IOCCC judges observe that enough IOCCC submitters are not so
	 * cognizant (cognisant) and so make these warnings manditory in the
	 * hopes it will reduce the number of entries that violate the IOCCC
	 * size rules.
	 */
	if (RULE_2A_SIZE < gross_count) {
		rule_2a_warning = 1;
	}
	if (RULE_2B_SIZE < net_count) {
		rule_2b_warning = 1;
	}

	(void) printf(out_fmt, net_count, gross_count, keywords);

	return;
}

int
main(int argc, char **argv)
{
	FILE *fp = stdin;
	int ch;

	while ((ch = getopt(argc, argv, "6ihvV")) != -1) {
		switch (ch) {
		case 'i': /* ignored for backward compatibility */
			break;

		case 'v':
			debug++;
			out_fmt = "%lu %lu %lu\n";
			break;

		case 'V':
			printf("%s\n", VERSION);
			exit(3);

		case '6': /* You're a RTFS master!  Congrats. */
			errx(6, "There is NO... Rule 6!  I'm not a number!  I'm a free(void *man)!");

		case 'h':
			fprintf(stderr, "%s%s", usage0, usage1);
			exit(2);
			break;

		default:
			fprintf(stderr, "unknown -option\n");
			fprintf(stderr, "%s%s", usage0, usage1);
			exit(4);
			break;
		}
	}

	if (optind + 1 == argc) {
		/* Redirect stdin to file path argument. */
		errno = 0;
		fp = fopen(argv[optind], "r");
		if (fp == NULL) {
			fprintf(stderr, "freopen(%s) failed: %s\n", argv[optind], strerror(errno));
			exit(5);
		}
	} else if (optind != argc) {
		/* Too many arguments. */
		fprintf(stderr, "%s%s", usage0, usage1);
		exit(4);
	}

	(void) setvbuf(fp, NULL, _IOLBF, 0);

	/* The Count - 1 Muha .. 2 Muhaha .. 3 Muhahaha ... */
	rule_count(fp);

	/*
	 * issue warnings
	 */
	if (char_warning) {
		fprintf(stderr, "Warning: character(s) with high bit set or NUL found! Be careful you don't violate rule 13!\n");
	}
	if (trigraph_warning) {
		fprintf(stderr, "Warning: unknown or invalid trigraph(s) found! Be careful!\n");
	}
	if (wordbuf_warning) {
		fprintf(stderr, "Warning: word buffer overflow! The size on stdout may be invalid under rule 2!");
	}
	if (ungetc_warning) {
		fprintf(stderr, "Warning: ungetc error: @SirWumpus goofe The size on stdout may be invalid under rule 2!\n");
	}
	if (rule_2a_warning) {
		(void) fprintf(stderr, "Warning: your source exceeds Rule 2a limit: %u! Rulr 2a violation!\n", RULE_2A_SIZE);
	}
	if (rule_2b_warning) {
		(void) fprintf(stderr, "Warning: your source exceeds Rule 2b limit: %u! Rule 2b violation!\n", RULE_2B_SIZE);
	}

	/*
	 * All Done!!! All Done!!! -- Jessica Noll, age 2
	 */
	if (rule_2a_warning || rule_2b_warning) {
		exit(1);
	}
	exit(0);
}
