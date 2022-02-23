/*
 * iocccsize - IOCCC Source Size Tool
 *
 * Public Domain 1992, 2015, 2018, 2019, 2021, 2022 by Anthony Howe.  All rights released.
 */


#if !defined(INCLUDE_IOCCCSIZE_H)
#    define  INCLUDE_IOCCCSIZE_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <sys/types.h>


/*
 * standard truth :-)
 */
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
/* have a C99 compiler - we should expect to have <stdbool.h> */
#include <stdbool.h>
#elif !defined(__cplusplus)
/* do not have a C99 compiler - fake a <stdbool.h> header file */
typedef unsigned char bool;
#undef true
#define true ((bool)(1))
#undef false
#define false ((bool)(0))
#endif


#ifndef IOCCCSIZE_VERSION
#define IOCCCSIZE_VERSION "unofficial iocccsize version"
#endif

#ifndef WORD_BUFFER_SIZE
#define WORD_BUFFER_SIZE	256
#endif

#ifndef RULE_2A_SIZE
#define RULE_2A_SIZE		4096	/* IOCCC Rule 2a */
#endif

#ifndef RULE_2B_SIZE
#define RULE_2B_SIZE		2503	/* IOCCC Rule 2b */
#endif

#if defined(MKIOCCCENTRY_USE)
#undef DIGRAPHS		/* digraphs count a 1 for Rule 2b */
#else
#define DIGRAPHS	/* digraphs count a 2 for Rule 2b */
#endif

#if defined(MKIOCCCENTRY_USE)
#undef TRIGRAPHS	/* trigraphs count a 1 for Rule 2b */
#else
#define TRIGRAPHS	/* trigraphs count a 3 for Rule 2b */
#endif


/*
 * rule_count() processing results
 */
typedef struct {
	off_t rule_2a_size;	/* official IOCCC Rule 2a calculated size */
	size_t rule_2b_size;	/* official IOCCC Rule 2b calculated size */
	size_t keywords;	/* keyword count - for -v mode */
	bool char_warning;	/* true ==> found high-bit or non-ASCII character */
	bool nul_warning;	/* true ==> found NUL */
	bool trigraph_warning;	/* true ==> found an unknown Tri-Graph */
	bool wordbuf_warning;	/* true ==> word buffer overflow detected */
	bool ungetc_warning;	/* true ==> ungetc called too many times */
} RuleCount;


/*
 * external functions
 */
extern RuleCount rule_count(FILE *fp_in);

/*
 * global variables
 */
extern char const * const iocccsize_version;


#ifdef  __cplusplus
}
#endif


#endif /* INCLUDE_IOCCCSIZE_H */
