/*
 * json_utf8 - JSON UTF-8 encoder/decoder
 *
 * "Because ASCII text is too limited, even for JSON." :-)
 *
 * Copyright (c) 2022-2025 by Cody Boone Ferguson and Landon Curt Noll. All
 * rights reserved.
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
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE OR JSON.
 *
 * This JSON parser, library and tools were co-developed in 2022-2025 by Cody Boone
 * Ferguson and Landon Curt Noll:
 *
 *  @xexyl
 *	https://xexyl.net		Cody Boone Ferguson
 *	https://ioccc.xexyl.net
 * and:
 *	chongo (Landon Curt Noll, http://www.isthe.com/chongo/index.html) /\oo/\
 *
 * "Because sometimes even the IOCCC Judges need some help." :-)
 *
 * "Share and Enjoy!"
 *     --  Sirius Cybernetics Corporation Complaints Division, JSON spec department. :-)
 */


#if !defined(INCLUDE_JSON_UTF8_H)

/*
 * dbg - info, debug, warning, error, and usage message facility
 */
#if defined(INTERNAL_INCLUDE)
#include "../dbg/dbg.h"
#elif defined(INTERNAL_INCLUDE_2)
#include "dbg/dbg.h"
#else
#include <dbg.h>
#endif

/*
 * util - common utility functions for the JSON parser
 */
#include "util.h"

/*
 * official jparse UTF-8 version
 */
#define JPARSE_UTF8_VERSION "2.1.3 2025-07-04"	/* format: major.minor YYYY-MM-DD */

extern size_t utf8len(const char *str, int32_t surrogate);
extern int32_t surrogate_pair_to_codepoint(int32_t hi, int32_t lo);
extern int codepoint_to_unicode(char *str, unsigned int val);
extern uint32_t utf8_to_codepoint(const char *str);
extern bool is_surrogate_pair(const int32_t xa, const int32_t xb);

#endif /* INCLUDE_JSON_UTF8_H */
