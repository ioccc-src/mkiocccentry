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
#include <stdint.h>

#if defined(MKIOCCCENTRY_USE)
/*
 * limit_ioccc - IOCCC size and rule related limitations
 */
#include "iocccsize_err.h"
#include "limit_ioccc.h"
#include "../iocccsize.h"
#else /* MKIOCCCENTRY_USE */
#include "iocccsize_err.h"
#include "iocccsize.h"
#endif /* MKIOCCCENTRY_USE */

#define STRLEN(s)		(sizeof (s)-1)

#define NO_STRING		0
#define NO_COMMENT		0
#define COMMENT_EOL		1
#define COMMENT_BLOCK		2

/*
 * globals
 */
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
	/* Yes Virginia, we left #define off the list on purpose!  K&R */
	{ STRLEN("#elif"), "#elif" } ,				/* K&R */
	{ STRLEN("#elifdef"), "#elifdef" } ,			/* +C23 */
	{ STRLEN("#elifndef"), "#elifndef" } ,			/* +C23 */
	{ STRLEN("#else"), "#else" } ,				/* K&R */
	{ STRLEN("#embed"), "#embed" } ,			/* +C23 */
	{ STRLEN("#endif"), "#endif" } ,			/* K&R */
	{ STRLEN("#error"), "#error" } ,			/* +C89 */
	{ STRLEN("#ident"), "#ident" } ,			/* gcc */
	{ STRLEN("#if"), "#if" } ,				/* K&R */
	{ STRLEN("#ifdef"), "#ifdef" } ,			/* K&R */
	{ STRLEN("#ifndef"), "#ifndef" } ,			/* K&R */
	{ STRLEN("#include"), "#include" } ,			/* K&R */
	{ STRLEN("#line"), "#line" } ,				/* K*R */
	{ STRLEN("#pragma"), "#pragma" } ,			/* +C89 */
	{ STRLEN("#sccs"), "#sccs" } ,				/* gcc */
	{ STRLEN("#warning"), "#warning" } ,			/* +C23 */
	{ STRLEN("#undef"), "#undef" } ,			/* K&R */

	{ STRLEN("_Alignas"), "_Alignas" } ,			/* +C11 */
	{ STRLEN("_Alignof"), "_Alignof" } ,			/* +C11 */
	{ STRLEN("_Atomic"), "_Atomic" } ,			/* +C11 */
	{ STRLEN("_BitInt"), "_BitInt" } ,			/* +C23 */
	{ STRLEN("_Bool"), "_Bool" } ,				/* +C99 */
	{ STRLEN("_Complex"), "_Complex" } ,			/* +C99 */
	{ STRLEN("_Decimal128"), "_Decimal128" } ,		/* +C23 */
	{ STRLEN("_Decimal64"), "_Decimal64" } ,		/* +C23 */
	{ STRLEN("_Decimal32"), "_Decimal32" } ,		/* +C23 */
	{ STRLEN("_Generic"), "_Generic" } ,			/* +C11 */
	{ STRLEN("_Imaginary"), "_Imaginary" } ,		/* +C99 */
	{ STRLEN("_Noreturn"), "_Noreturn" } ,			/* +C11 */
	{ STRLEN("_Pragma"), "_Pragma" } ,			/* +C99 */
	{ STRLEN("_Static_assert"), "_Static_assert" } ,	/* +C11 */
	{ STRLEN("_Thread_local"), "_Thread_local" } ,		/* +C11 */

	{ STRLEN("alignas"), "alignas" } ,			/* +C23 */
	{ STRLEN("alignof"), "alignof" } ,			/* +C23 */
	{ STRLEN("and"), "and" } ,				/* +C89 iso646.h */
	{ STRLEN("and_eq"), "and_eq" } ,			/* +C89 iso646.h */
	{ STRLEN("auto"), "auto" } ,				/* K&R */
	{ STRLEN("bitand"), "bitand" } ,			/* +C89 iso646.h */
	{ STRLEN("bitor"), "bitor" } ,				/* +C89 iso646.h */
	{ STRLEN("bool"), "bool" } ,				/* +C23 */
	{ STRLEN("break"), "break" } ,				/* K&R */
	{ STRLEN("case"), "case" } ,				/* K&R */
	{ STRLEN("char"), "char" } ,				/* K&R */
	{ STRLEN("compl"), "compl" } ,				/* +C89 iso646.h */
	{ STRLEN("const"), "const" } ,				/* +C89 */
	{ STRLEN("continue"), "continue" } ,			/* K&R */
	{ STRLEN("default"), "default" } ,			/* K&R */
	{ STRLEN("do"), "do" } ,				/* K&R */
	{ STRLEN("double"), "double" } ,			/* K&R */
	{ STRLEN("else"), "else" } ,				/* K&R */
	{ STRLEN("enum"), "enum" } ,				/* +C89 */
	{ STRLEN("extern"), "extern" } ,			/* K&R */
	{ STRLEN("false"), "false" } ,				/* +C23 */
	{ STRLEN("float"), "float" } ,				/* K&R */
	{ STRLEN("for"), "for" } ,				/* K&R */
	{ STRLEN("goto"), "goto" } ,				/* K&R */
	{ STRLEN("if"), "if" } ,				/* K&R */
	{ STRLEN("inline"), "inline" } ,			/* +C99 */
	{ STRLEN("int"), "int" } ,				/* K&R */
	{ STRLEN("long"), "long" } ,				/* K&R */
	{ STRLEN("noreturn"), "noreturn" } ,			/* +C23 */
	{ STRLEN("not"), "not" } ,				/* +C89 iso646.h */
	{ STRLEN("not_eq"), "not_eq" } ,			/* +C89 iso646.h */
	{ STRLEN("or"), "or" } ,				/* +C89 iso646.h */
	{ STRLEN("or_eq"), "or_eq" } ,				/* +C89 iso646.h */
	{ STRLEN("register"), "register" } ,			/* K&R */
	{ STRLEN("restrict"), "restrict" } ,			/* +C99 */
	{ STRLEN("return"), "return" } ,			/* K&R */
	{ STRLEN("short"), "short" } ,				/* K&R */
	{ STRLEN("signed"), "signed" } ,			/* K&R */
	{ STRLEN("sizeof"), "sizeof" } ,			/* K&R */
	{ STRLEN("static"), "static" } ,			/* K&R */
	{ STRLEN("static_assert"), "static_assert" } ,		/* +C23 */
	{ STRLEN("struct"), "struct" } ,			/* K&R */
	{ STRLEN("switch"), "switch" } ,			/* K&R */
	{ STRLEN("thread_local"), "thread_local" } ,		/* +C23 */
	{ STRLEN("true"), "true" } ,				/* +C23 */
	{ STRLEN("typedef"), "typedef" } ,			/* K&R */
	{ STRLEN("typeof"), "typeof" } ,			/* +C23 */
	{ STRLEN("typeof_unequal"), "typeof_unequal" } ,	/* +C23 */
	{ STRLEN("union"), "union" } ,				/* K&R */
	{ STRLEN("unsigned"), "unsigned" } ,			/* K&R */
	{ STRLEN("void"), "void" } ,				/* +C89 */
	{ STRLEN("volatile"), "volatile" } ,			/* +C89 */
	{ STRLEN("while"), "while" } ,				/* K&R */
	{ STRLEN("xor"), "xor" } ,				/* +C89 iso646.h */
	{ STRLEN("xor_eq"), "xor_eq" } ,			/* +C89 iso646.h */

	{ 0, NULL }
};


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
 * rule_count - count data on file stream, according to Rule 2a and Rule 2b
 *
 * In addition to the Rule 2a and Rule 2b counting, count keywords.
 * We also warn about various situations such found high-bit or non-ASCII character,
 * found NUL byte, found an unknown Tri-Graph, word buffer overflow,
 * or ungetc called too many times.
 *
 * given:
 *	fp_in		open file stream to count data on
 *
 * returns:
 *	RuleCount information
 */
RuleCount
rule_count(FILE *fp_in)
{
	size_t wordi = 0;
	char word[WORD_BUFFER_SIZE];
	RuleCount counts = { 0, 0, 0, false, false, false, false, false };
	int ch, next_ch, quote = NO_STRING, escape = 0, is_comment = NO_COMMENT;

/* If quote == NO_STRING (0) and is_comment == NO_COMMENT (0) then its code. */
#define IS_CODE	(quote == is_comment)

	/* Paranoia and to keep valgrind happy. */
	(void) memset(word, 0, sizeof (word));

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

			/* Unknown trigraph, push back the third character. */
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
			/*
			 * ISO C11 section 5.1.1.2 Translation Phases
			 * point 2 discards backslash newlines.
			 */
			counts.rule_2a_size += 2;
			continue;
		}

		if (next_ch != EOF && ungetc(next_ch, fp_in) == EOF) {
			/*
			 * ISO C ungetc() guarantees one character (byte) pushback.
			 * How does that relate to UTF8 and wide-character library
			 * handling?  An invalid trigraph results in 2x ungetc().
			 */
			counts.ungetc_warning = true;
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
		/*
		 * ISO C11 section 6.4.6 Punctuators, digraphs handled during
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

		/*
		 * End of possible keyword?  Care with #word as there can
		 * be whitespace or comments between # and word.
		 */
		if ((word[0] != '#' || 1 < wordi) && !isalnum(ch) && ch != '_' && ch != '#') {
			if (find_member(cwords, word) != NULL) {
				/* Count keyword as 1. */
				counts.rule_2b_size = counts.rule_2b_size - wordi + 1;
				counts.keywords++;
				iocccsize_dbg(1, "~~keyword %ju \"%s\"", (uintmax_t)counts.keywords, word);
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
			word[wordi++] = (char) ch;
			if (sizeof (word) <= wordi) {
				/* ISO C11 section 5.2.4.1 Translation limits, identifiers
				 * can have 63 significant initial characters, which can be
				 * multibyte.  The C keywords are all ASCII, longest is 14
				 * bytes.
				 *
				 * We only care about the C keywords and not the identifiers,
				 * so the buffer can overflow regularly as long words or
				 * identifiers are ignored.
				 */
				counts.wordbuf_warning = true;
				wordi = 0;
			}
			word[wordi] = '\0';
		}

		counts.rule_2b_size++;
	}

	return counts;
}


/*
 * is_reserved - if string is a reserved word in C
 *
 * given:
 *	string	the string to check
 *
 * returns:
 *	true ==> string is a reserved word in C
 *	false ==> string is NOT a reserved word in C or is NULL
 */
bool
is_reserved(const char *string)
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
