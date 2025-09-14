/*
 * iocccsize - IOCCC Source Size Tool
 *
 * Public Domain 1992, 2015, 2018, 2019, 2021 by Anthony Howe.  All rights released.
 * With IOCCC mods in 2019-2025 by chongo (Landon Curt Noll) ^oo^
 */


#if !defined(INCLUDE_IOCCCSIZE_H)
#    define  INCLUDE_IOCCCSIZE_H


#ifdef __cplusplus
extern "C" {
#endif


#include <stddef.h>
#include <sys/types.h>

#ifndef WORD_BUFFER_SIZE
  #define WORD_BUFFER_SIZE	16
#endif

#undef DIGRAPHS	    /* digraphs count a 2 for Rule 2b */
#undef TRIGRAPHS    /* trigraphs count a 3 for Rule 2b */


/*
 * rule_count() processing results
 */
typedef struct
{
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
extern bool is_reserved(const char *string);

/*
 * global variables
 */
extern char const * const iocccsize_version;


#ifdef  __cplusplus
}
#endif


#endif /* INCLUDE_IOCCCSIZE_H */
