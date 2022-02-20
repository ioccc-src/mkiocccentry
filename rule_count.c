/*
 * iocccsize - IOCCC Source Size Tool
 *
 *	"You are not expected to understand this" :-)
 *
 *	Public Domain 1992, 2015, 2018, 2019, 2021 by Anthony Howe.  All rights released.
 *	With IOCCC mods in 2019-2021 by chongo (Landon Curt Noll) ^oo^
 */

/*
 * ISO C11 section 5.2.1 defines source character set, specifically:
 *
 *	The representation of each member of the source and execution
 *	basic character sets shall fit in a byte.
 *
 * Note however that string literals and comments could contain non-ASCII
 * (consider non-English developers writing native language comments):
 *
 *	If any other characters are encountered in a source file (except
 *	in an identifier, a character constant, a string literal, a header
 *	name, a comment, or a preprocessing token that is never converted
 *	to a token), the behavior is undefined.
 *
 * Probably best to leave as-is, count them, and let the compiler sort it.
 */
#undef ASCII_ONLY

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

#define STRLEN(s)		(sizeof (s)-1)

#define NO_STRING		0
#define NO_COMMENT		0
#define COMMENT_EOL		1
#define COMMENT_BLOCK		2

extern int verbosity_level;
char const * const iocccsize_version = IOCCCSIZE_VERSION;

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

RuleCount
rule_count(FILE *fp_in)
{
	size_t wordi = 0;
	char word[WORD_BUFFER_SIZE+1];
	RuleCount counts = { 0, 0, 0, false, false, false, false, false };
	int ch, next_ch, quote = NO_STRING, escape = 0, is_comment = NO_COMMENT;

/* If quote == NO_STRING (0) and is_comment == NO_COMMENT (0) then its code. */
#define IS_CODE	(quote == is_comment)

	while ((ch = fgetc(fp_in)) != EOF) {
		if (ch == '\r') {
			/* Discard bare CR and those part of CRLF. */
			counts.rule_2a_size++;
			continue;
		}
		if (ch == '\0') {
			counts.nul_warning = true;
			counts.rule_2a_size++;
			continue;
		}

		/* Future gazing. */
		while ((next_ch = fgetc(fp_in)) != EOF && next_ch == '\r') {
			/* Discard bare CR and those part of CRLF. */
			counts.rule_2a_size++;
		}
#ifdef ASCII_ONLY
		if (!isascii(ch) || (ch >= 0x80)) {
			counts.char_warning = true;
			counts.rule_2a_size++;
			continue;
		}
#endif

#ifdef TRIGRAPHS
		if (ch == '?' && next_ch == '?') {
			/* ISO C11 section 5.2.1.1 Trigraph Sequences */
			const char *t;
			static const char trigraphs[] = "=#([)]'^<{!|>}-~/\\";

			ch = fgetc(fp_in);
			for (t = trigraphs; *t != '\0'; t += 2) {
				if (ch == t[0]) {
					/* Mapped trigraphs count as 1 byte. */
					next_ch = fgetc(fp_in);
					counts.rule_2a_size += 2;
					ch = t[1];
					break;
				}
			}

			/* Unknown trigraph, push back the 3rd character. */
			if (*t == '\0') {
				if (ch != EOF && ungetc(ch, fp_in) == EOF) {
					counts.ungetc_warning = true;
					counts.trigraph_warning = true;
					counts.rule_2a_size++;
					continue;
				}
				ch = '?';
			}
		}
#endif
		if (ch == '\\' && next_ch == '\n') {
			/* ISO C11 section 5.1.1.2 Translation Phases
			 * point 2 discards backslash newlines.
			 */
			counts.rule_2a_size += 2;
			continue;
		}

		if (next_ch != EOF && ungetc(next_ch, fp_in) == EOF) {
			/* ISO C ungetc() guarantees one character (byte) pushback.
			 * How does that relate to UTF8 and wide-character library
			 * handling?  An invalid trigraph results in 2x ungetc().
			 */
			counts.wordbuf_warning = true;
			counts.rule_2a_size++;
			continue;
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
			iocccsize_dbg(1, "~~NO_COMMENT");
			is_comment = NO_COMMENT;
		}

		/* Within comment block? */
		else if (is_comment == COMMENT_BLOCK && ch == '*' && next_ch == '/') {
			iocccsize_dbg(1, "~~NO_COMMENT");
			is_comment = NO_COMMENT;
		}

		/* Start of comment to end of line? */
		else if (is_comment == NO_COMMENT && ch == '/' && next_ch == '/') {
			iocccsize_dbg(1, "~~COMMENT_EOL");
			is_comment = COMMENT_EOL;

			/* Consume next_ch. */
			ch = fgetc(fp_in);
			counts.rule_2a_size++;
			counts.rule_2b_size++;
		}

		/* Start of comment block? */
		else if (is_comment == NO_COMMENT && ch == '/' && next_ch == '*') {
			iocccsize_dbg(1, "~~COMMENT_BLOCK");
			is_comment = COMMENT_BLOCK;

			/* Consume next_ch. */
			ch = fgetc(fp_in);
			counts.rule_2a_size++;
			counts.rule_2b_size++;
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
					(void) fgetc(fp_in);
					counts.rule_2a_size++;
					ch = d[0];
					break;
				}
			}
		}
#endif
		/* Sanity check against file size and wc(1) byte count. */
		counts.rule_2a_size++;

		/* End of possible keyword?  Care with #word as there can
		 * be whitespace or comments between # and word.
		 */
		if ((word[0] != '#' || 1 < wordi) && !isalnum(ch) && ch != '_' && ch != '#') {
			if (find_member(cwords, word) != NULL) {
				/* Count keyword as 1. */
				counts.rule_2b_size = counts.rule_2b_size - wordi + 1;
				counts.keywords++;
				iocccsize_dbg(1, "~~keyword %lu \"%s\"", (unsigned long)counts.keywords, word);
			}
			word[wordi = 0] = '\0';
		}

		/* Ignore all whitespace. */
		if (isspace(ch)) {
			iocccsize_dbg(2, "~~ignore whitespace %#02x", ch);
			continue;
		}

		/* Ignore begin/end block and end of statement. */
		if (strchr("{;}", ch) != NULL && (isspace(next_ch) || next_ch == EOF)) {
			iocccsize_dbg(2, "~~ignore %c", ch);
			continue;
		}

		/* Collect next word not in a string or comment. */
		if (IS_CODE && (isalnum(ch) || ch == '_' || ch == '#')) {
			if (sizeof (word) <= wordi) {
				counts.wordbuf_warning = true;
				wordi = 0;
			}
			word[wordi++] = (char) ch;
			word[wordi] = '\0';
		}

		counts.rule_2b_size++;
	}

	return counts;
}
