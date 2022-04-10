/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * json - JSON functions supporting mkiocccentry code
 *
 * JSON related functions to support formation of .info.json files
 * and .author.json files, their related check tools, test code,
 * and string encoding/decoding tools.
 *
 * "Because JSON embodies a commitment to original design flaws." :-)
 *
 * Copyright (c) 2022 by Landon Curt Noll.  All Rights Reserved.
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


/* special comments for the seqcexit tool */
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>
#include <time.h>
#include <limits.h>
#include <inttypes.h>

/*
 * dbg - debug, warning and error reporting facility
 */
#include "dbg.h"

/*
 * util - utility functions
 */
#include "util.h"

/*
 * IOCCC limits
 */
#include "limit_ioccc.h"

/*
 * JSON - JSON structures and functions
 */
#include "json.h"


/*
 * JSON encoding of an octet in a JSON string
 */
struct encode {
    u_int8_t byte;	    /* 8 bit character to encode */
    size_t len;		    /* length of encoding */
    const char * const enc; /* JSON encoding of val */
};
static struct encode jenc[BYTE_VALUES] = {
    /* \x00 - \x0f */
    {0x00, 6, "\\u0000"}, {0x01, 6, "\\u0001"}, {0x02, 6, "\\u0002"}, {0x03, 6, "\\u0003"},
    {0x04, 6, "\\u0004"}, {0x05, 6, "\\u0005"}, {0x06, 6, "\\u0006"}, {0x07, 6, "\\u0007"},
    {0x08, 2, "\\b"}, {0x09, 2, "\\t"}, {0x0a, 2, "\\n"}, {0x0b, 6, "\\u000b"},
    {0x0c, 2, "\\f"}, {0x0d, 2, "\\r"}, {0x0e, 6, "\\u000e"}, {0x0f, 6, "\\u000f"},

    /* \x10 - \x1f */
    {0x10, 6, "\\u0010"}, {0x11, 6, "\\u0011"}, {0x12, 6, "\\u0012"}, {0x13, 6, "\\u0013"},
    {0x14, 6, "\\u0014"}, {0x15, 6, "\\u0015"}, {0x16, 6, "\\u0016"}, {0x17, 6, "\\u0017"},
    {0x18, 6, "\\u0018"}, {0x19, 6, "\\u0019"}, {0x1a, 6, "\\u001a"}, {0x1b, 6, "\\u001b"},
    {0x1c, 6, "\\u001c"}, {0x1d, 6, "\\u001d"}, {0x1e, 6, "\\u001e"}, {0x1f, 6, "\\u001f"},

    /* \x20 - \x2f */
    {' ', 1, " "}, {'!', 1, "!"}, {'"', 2, "\\\""}, {'#', 1, "#"},
    {'$', 1, "$"}, {'%', 1, "%"}, {'&', 6, "\\u0026"}, {'\'', 1, "'"},
    {'(', 1, "("}, {')', 1, ")"}, {'*', 1, "*"}, {'+', 1, "+"},
    {',', 1, ","}, {'-', 1, "-"}, {'.', 1, "."}, {'/', 2, "\\/"},

    /* \x30 - \x3f */
    {'0', 1, "0"}, {'1', 1, "1"}, {'2', 1, "2"}, {'3', 1, "3"},
    {'4', 1, "4"}, {'5', 1, "5"}, {'6', 1, "6"}, {'7', 1, "7"},
    {'8', 1, "8"}, {'9', 1, "9"}, {':', 1, ":"}, {';', 1, ";"},
    {'<', 6, "\\u003c"}, {'=', 1, "="}, {'>', 6, "\\u003e"}, {'?', 1, "?"},

    /* \x40 - \x4f */
    {'@', 1, "@"}, {'A', 1, "A"}, {'B', 1, "B"}, {'C', 1, "C"},
    {'D', 1, "D"}, {'E', 1, "E"}, {'F', 1, "F"}, {'G', 1, "G"},
    {'H', 1, "H"}, {'I', 1, "I"}, {'J', 1, "J"}, {'K', 1, "K"},
    {'L', 1, "L"}, {'M', 1, "M"}, {'N', 1, "N"}, {'O', 1, "O"},

    /* \x50 - \x5f */
    {'P', 1, "P"}, {'Q', 1, "Q"}, {'R', 1, "R"}, {'S', 1, "S"},
    {'T', 1, "T"}, {'U', 1, "U"}, {'V', 1, "V"}, {'W', 1, "W"},
    {'X', 1, "X"}, {'Y', 1, "Y"}, {'Z', 1, "Z"}, {'[', 1, "["},
    {'\\', 2, "\\\\"}, {']', 1, "]"}, {'^', 1, "^"}, {'_', 1, "_"},

    /* \x60 - \x6f */
    {'`', 1, "`"}, {'a', 1, "a"}, {'b', 1, "b"}, {'c', 1, "c"},
    {'d', 1, "d"}, {'e', 1, "e"}, {'f', 1, "f"}, {'g', 1, "g"},
    {'h', 1, "h"}, {'i', 1, "i"}, {'j', 1, "j"}, {'k', 1, "k"},
    {'l', 1, "l"}, {'m', 1, "m"}, {'n', 1, "n"}, {'o', 1, "o"},

    /* \x70 - \x7f */
    {'p', 1, "p"}, {'q', 1, "q"}, {'r', 1, "r"}, {'s', 1, "s"},
    {'t', 1, "t"}, {'u', 1, "u"}, {'v', 1, "v"}, {'w', 1, "w"},
    {'x', 1, "x"}, {'y', 1, "y"}, {'z', 1, "z"}, {'{', 1, "{"},
    {'|', 1, "|"}, {'}', 1, "}"}, {'~', 1, "~"}, {0x7f, 6, "\\u007f"},

    /* \x80 - \x8f */
    {0x80, 6, "\\u0080"}, {0x81, 6, "\\u0081"}, {0x82, 6, "\\u0082"}, {0x83, 6, "\\u0083"},
    {0x84, 6, "\\u0084"}, {0x85, 6, "\\u0085"}, {0x86, 6, "\\u0086"}, {0x87, 6, "\\u0087"},
    {0x88, 6, "\\u0088"}, {0x89, 6, "\\u0089"}, {0x8a, 6, "\\u008a"}, {0x8b, 6, "\\u008b"},
    {0x8c, 6, "\\u008c"}, {0x8d, 6, "\\u008d"}, {0x8e, 6, "\\u008e"}, {0x8f, 6, "\\u008f"},

    /* \x90 - \x9f */
    {0x90, 6, "\\u0090"}, {0x91, 6, "\\u0091"}, {0x92, 6, "\\u0092"}, {0x93, 6, "\\u0093"},
    {0x94, 6, "\\u0094"}, {0x95, 6, "\\u0095"}, {0x96, 6, "\\u0096"}, {0x97, 6, "\\u0097"},
    {0x98, 6, "\\u0098"}, {0x99, 6, "\\u0099"}, {0x9a, 6, "\\u009a"}, {0x9b, 6, "\\u009b"},
    {0x9c, 6, "\\u009c"}, {0x9d, 6, "\\u009d"}, {0x9e, 6, "\\u009e"}, {0x9f, 6, "\\u009f"},

    /* \xa0 - \xaf */
    {0xa0, 6, "\\u00a0"}, {0xa1, 6, "\\u00a1"}, {0xa2, 6, "\\u00a2"}, {0xa3, 6, "\\u00a3"},
    {0xa4, 6, "\\u00a4"}, {0xa5, 6, "\\u00a5"}, {0xa6, 6, "\\u00a6"}, {0xa7, 6, "\\u00a7"},
    {0xa8, 6, "\\u00a8"}, {0xa9, 6, "\\u00a9"}, {0xaa, 6, "\\u00aa"}, {0xab, 6, "\\u00ab"},
    {0xac, 6, "\\u00ac"}, {0xad, 6, "\\u00ad"}, {0xae, 6, "\\u00ae"}, {0xaf, 6, "\\u00af"},

    /* \xb0 - \xbf */
    {0xb0, 6, "\\u00b0"}, {0xb1, 6, "\\u00b1"}, {0xb2, 6, "\\u00b2"}, {0xb3, 6, "\\u00b3"},
    {0xb4, 6, "\\u00b4"}, {0xb5, 6, "\\u00b5"}, {0xb6, 6, "\\u00b6"}, {0xb7, 6, "\\u00b7"},
    {0xb8, 6, "\\u00b8"}, {0xb9, 6, "\\u00b9"}, {0xba, 6, "\\u00ba"}, {0xbb, 6, "\\u00bb"},
    {0xbc, 6, "\\u00bc"}, {0xbd, 6, "\\u00bd"}, {0xbe, 6, "\\u00be"}, {0xbf, 6, "\\u00bf"},

    /* \xc0 - \xcf */
    {0xc0, 6, "\\u00c0"}, {0xc1, 6, "\\u00c1"}, {0xc2, 6, "\\u00c2"}, {0xc3, 6, "\\u00c3"},
    {0xc4, 6, "\\u00c4"}, {0xc5, 6, "\\u00c5"}, {0xc6, 6, "\\u00c6"}, {0xc7, 6, "\\u00c7"},
    {0xc8, 6, "\\u00c8"}, {0xc9, 6, "\\u00c9"}, {0xca, 6, "\\u00ca"}, {0xcb, 6, "\\u00cb"},
    {0xcc, 6, "\\u00cc"}, {0xcd, 6, "\\u00cd"}, {0xce, 6, "\\u00ce"}, {0xcf, 6, "\\u00cf"},

    /* \xd0 - \xdf */
    {0xd0, 6, "\\u00d0"}, {0xd1, 6, "\\u00d1"}, {0xd2, 6, "\\u00d2"}, {0xd3, 6, "\\u00d3"},
    {0xd4, 6, "\\u00d4"}, {0xd5, 6, "\\u00d5"}, {0xd6, 6, "\\u00d6"}, {0xd7, 6, "\\u00d7"},
    {0xd8, 6, "\\u00d8"}, {0xd9, 6, "\\u00d9"}, {0xda, 6, "\\u00da"}, {0xdb, 6, "\\u00db"},
    {0xdc, 6, "\\u00dc"}, {0xdd, 6, "\\u00dd"}, {0xde, 6, "\\u00de"}, {0xdf, 6, "\\u00df"},

    /* \xe0 - \xef */
    {0xe0, 6, "\\u00e0"}, {0xe1, 6, "\\u00e1"}, {0xe2, 6, "\\u00e2"}, {0xe3, 6, "\\u00e3"},
    {0xe4, 6, "\\u00e4"}, {0xe5, 6, "\\u00e5"}, {0xe6, 6, "\\u00e6"}, {0xe7, 6, "\\u00e7"},
    {0xe8, 6, "\\u00e8"}, {0xe9, 6, "\\u00e9"}, {0xea, 6, "\\u00ea"}, {0xeb, 6, "\\u00eb"},
    {0xec, 6, "\\u00ec"}, {0xed, 6, "\\u00ed"}, {0xee, 6, "\\u00ee"}, {0xef, 6, "\\u00ef"},

    /* \xf0 - \xff */
    {0xf0, 6, "\\u00f0"}, {0xf1, 6, "\\u00f1"}, {0xf2, 6, "\\u00f2"}, {0xf3, 6, "\\u00f3"},
    {0xf4, 6, "\\u00f4"}, {0xf5, 6, "\\u00f5"}, {0xf6, 6, "\\u00f6"}, {0xf7, 6, "\\u00f7"},
    {0xf8, 6, "\\u00f8"}, {0xf9, 6, "\\u00f9"}, {0xfa, 6, "\\u00fa"}, {0xfb, 6, "\\u00fb"},
    {0xfc, 6, "\\u00fc"}, {0xfd, 6, "\\u00fd"}, {0xfe, 6, "\\u00fe"}, {0xff, 6, "\\u00ff"}
};


/*
 * hexval - concert ASCII character to hex value
 *
 * NOTE: -1 means the ASCII character is not a value hex character
 */
static int hexval[BYTE_VALUES] = {
    /* \x00 - \x0f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x10 - \x1f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x20 - \x2f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x30 - \x3f */
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    /* \x40 - \x4f */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x50 - \x5f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x60 - \x6f */
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x70 - \x7f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x80 - \x8f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \x90 - \x9f */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xa0 - \xaf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xb0 - \xbf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xc0 - \xcf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xd0 - \xdf */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xe0 - \xef */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    /* \xf0 - \xff */
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
};


/*
 * JSON warn (NOT error) codes to ignore
 *
 * When a tool is given command line arguments of the form:
 *
 *	.. -W 123 -W 1345 -W 56 ...
 *
 * this means the tool will ignore {JSON-0123}, {JSON-1345}, and {JSON-0056}.
 * The code_ignore_settable holds the JSON codes to ignore.
 *
 * NOTE: A NULL ignore_json_code_set means that the set has not been setup.
 */
struct ignore_json_code *ignore_json_code_set = NULL;


/*
 * Common JSON fields table used to determine if a name is a common field,
 * whether it's been added to the found_common_json_fields list, how many times
 * it has been seen and how many are allowed.
 */
struct json_field common_json_fields[] =
{
    { "ioccc_contest",		    NULL, 0, 1, false, JSON_CHARS, false, NULL },
    { "ioccc_year",		    NULL, 0, 1, false, JSON_NUM, false, NULL },
    { "mkiocccentry_version",	    NULL, 0, 1, false, JSON_CHARS, false, NULL },
    { "fnamchk_version",	    NULL, 0, 1, false, JSON_CHARS, false, NULL },
    { "IOCCC_contest_id",	    NULL, 0, 1, false, JSON_CHARS, false, NULL },
    { "entry_num",		    NULL, 0, 1, false, JSON_NUM, false, NULL },
    { "tarball",		    NULL, 0, 1, false, JSON_CHARS, false, NULL },
    { "formed_timestamp",	    NULL, 0, 1, false, JSON_NUM, false, NULL },
    { "formed_timestamp_usec",	    NULL, 0, 1, false, JSON_NUM, false, NULL },
    { "timestamp_epoch",	    NULL, 0, 1, false, JSON_CHARS, false, NULL },
    { "min_timestamp",		    NULL, 0, 1, false, JSON_NUM, false, NULL },
    { "formed_UTC",		    NULL, 0, 1, false, JSON_CHARS, false, NULL },
    { "test_mode",		    NULL, 0, 1, false, JSON_BOOL,   false, NULL },
    { NULL,			    NULL, 0, 0, false, JSON_EOT,   false, NULL } /* this **MUST** be last! */
};
size_t SIZEOF_COMMON_JSON_FIELDS_TABLE = TBLLEN(common_json_fields);


/*
 * .info.json fields table used to determine if a name belongs in the file,
 * whether it's been added to the found_info_json_fields list, how many times
 * it's been seen and how many are allowed.
 *
 */
struct json_field info_json_fields[] =
{
    { "IOCCC_info_version",	NULL, 0, 1, false, JSON_CHARS,		false, NULL },
    { "jinfochk_version",	NULL, 0, 1, false, JSON_CHARS,		false, NULL },
    { "iocccsize_version",	NULL, 0, 1, false, JSON_CHARS,		false, NULL },
    { "txzchk_version",		NULL, 0, 1, false, JSON_CHARS,		false, NULL },
    { "title",			NULL, 0, 1, false, JSON_CHARS,		false, NULL },
    { "abstract",		NULL, 0, 1, false, JSON_CHARS,		false, NULL },
    { "rule_2a_size",		NULL, 0, 1, false, JSON_NUM,		false, NULL },
    { "rule_2b_size",		NULL, 0, 1, false, JSON_NUM,		false, NULL },
    { "empty_override",		NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "rule_2a_override",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "rule_2a_mismatch",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "rule_2b_override",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "highbit_warning",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "nul_warning",		NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "trigraph_warning",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "wordbuf_warning",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "ungetc_warning",		NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "Makefile_override",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "first_rule_is_all",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "found_all_rule",		NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "found_clean_rule",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "found_clobber_rule",	NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "found_try_rule",		NULL, 0, 1, false, JSON_BOOL,		false, NULL },
    { "manifest",		NULL, 0, 1, false, JSON_ARRAY,		false, NULL },
    { "info_JSON",		NULL, 0, 1, false, JSON_ARRAY_CHARS,	false,	NULL },
    { "author_JSON",		NULL, 0, 1, false, JSON_ARRAY_CHARS,	false,	NULL },
    { "c_src",			NULL, 0, 1, false, JSON_ARRAY_CHARS,	false,	NULL },
    { "Makefile",		NULL, 0, 1, false, JSON_ARRAY_CHARS,	false,	NULL },
    { "remarks",		NULL, 0, 1, false, JSON_ARRAY_CHARS,	false,	NULL },
    { "extra_file",		NULL, 0, 0, false, JSON_ARRAY_CHARS,	false,	NULL },
    { NULL,			NULL, 0, 0, false, JSON_EOT,		false,	NULL } /* this **MUST** be last */
};
size_t SIZEOF_INFO_JSON_FIELDS_TABLE = TBLLEN(info_json_fields);


/*
 * .author.json fields table used to determine if a name belongs in the file,
 * whether it's been added to the found_author_json_fields list, how many times
 * it's been seen and how many are allowed.
 *
 * XXX - As of 4 March 2022 all fields are in the table but because arrays
 * are not yet parsed not all of these values will be dealt with: that is they
 * won't be checked. Additionally as of some days back (it's now 16 March 2022)
 * it was decided that a more general JSON parser should be implemented via
 * flex(1) and bison(1) so the parsers in jinfochk.c and jauthchk.c will be
 * removed and a single parser will be made (probably in this file).
 */
struct json_field author_json_fields[] =
{
    { "IOCCC_author_version",	NULL, 0, 1, false, JSON_CHARS,		false,	NULL },
    { "jauthchk_version",	NULL, 0, 1, false, JSON_CHARS,		false,	NULL },
    { "author_count",		NULL, 0, 1, false, JSON_NUM,		false,  NULL },
    { "authors",		NULL, 0, 1, false, JSON_ARRAY,		false,	NULL },
    { "name",			NULL, 0, 5, false, JSON_ARRAY_CHARS,	false,  NULL },
    { "location_code",		NULL, 0, 5, false, JSON_ARRAY_CHARS,	false,	NULL },
    { "email",			NULL, 0, 5, false, JSON_ARRAY_CHARS,	true,	NULL },
    { "url",			NULL, 0, 5, false, JSON_ARRAY_CHARS,	true,	NULL },
    { "twitter",		NULL, 0, 5, false, JSON_ARRAY_CHARS,	true,	NULL },
    { "github",			NULL, 0, 5, false, JSON_ARRAY_CHARS,	true,	NULL },
    { "affiliation",		NULL, 0, 5, false, JSON_ARRAY_CHARS,	true,	NULL },
    { "past_winner",		NULL, 0, 1, false, JSON_BOOL,		true,	NULL },
    { "default_handle",		NULL, 0, 1, false, JSON_BOOL,		true,	NULL },
    { "author_handle",		NULL, 0, 5, false, JSON_ARRAY_CHARS,	true,	NULL },
    { "author_number",		NULL, 0, 5, false, JSON_ARRAY_NUMBER,	false,	NULL },
    { NULL,			NULL, 0, 0, false, JSON_EOT,		false,	NULL } /* this **MUST** be last */
};
size_t SIZEOF_AUTHOR_JSON_FIELDS_TABLE = TBLLEN(author_json_fields);


/*
 * global for jwarn(): -w in jinfochk/jauthchk says to show full warning
 *
 * XXX This currently is not used because there are some problems that have to
 * be resolved first that will take more time and thought.
 */
bool show_full_json_warnings = false;


/*
 * static functions
 */
static void alloc_json_code_ignore_set(void);
static int cmp_codes(const void *a, const void *b);
static void expand_json_code_ignore_set(void);


/*
 * malloc_json_encode - return a JSON encoding of a block of memory
 *
 * JSON string encoding:
 *
 * These escape characters are required by JSON:
 *
 *      old			new
 *      ----------------------------
 *	<backspace>		\b	(\x08)
 *	<horizontal_tab>	\t	(\x09)
 *	<newline>		\n	(\x0a)
 *	<form_feed>		\f	(\x0c)
 *	<enter>			\r	(\x0d)
 *	"			\"	(\x22)
 *	/			\/	(\x2f)
 *	\			\\	(\x5c)
 *
 * These escape characters are implied by JSON due to
 * HTML and XML encoding, although not strictly required:
 *
 *      old		new
 *      --------------------
 *	&		\u0026		(\x26)
 *	<		\u003c		(\x3c)
 *	>		\u003e		(\x3e)
 *
 * These escape characters are implied by JSON to let humans
 * view JSON without worrying about characters that might
 * not display / might not be printable:
 *
 *      old			new
 *      ----------------------------
 *	\x00-\x07		\u0000 - \u0007
 *	\x0b			\u000b <vertical_tab>
 *	\x0e-\x1f		\u000e - \x001f
 *	\x7f-\xff		\u007f - \u00ff
 *
 * See:
 *
 *	https://developpaper.com/escape-and-unicode-encoding-in-json-serialization/
 *
 * NOTE: We chose to not escape '%' as was suggested by the above URL
 *	 because it is neither required by JSON nor implied by JSON.
 *
 * NOTE: While there exist C escapes for characters such as '\v',
 *	 due to flaws in the JSON spec, we must encode such characters
 *	 using the \uffff notation.
 *
 * given:
 *	ptr	start of memory block to encode
 *	len	length of block to encode in bytes
 *	retlen	address of where to store malloced length, if retlen != NULL
 *
 * returns:
 *	malloced JSON encoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
malloc_json_encode(char const *ptr, size_t len, size_t *retlen)
{
    char *ret = NULL;	    /* malloced encoding string or NULL */
    char *beyond = NULL;    /* beyond the end of the malloced encoding string */
    size_t mlen = 0;	    /* length of malloced encoded string */
    char *p;		    /* next place to encode */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %ju must be > 0", (uintmax_t) len);
	return NULL;
    }

    /*
     * count the bytes that will be in the encoded malloced string
     */
    for (i=0; i < len; ++i) {
	mlen += jenc[(uint8_t)(ptr[i])].len;
    }

    /*
     * malloc the encoded string
     */
    ret = malloc(mlen + 1 + 1);
    if (ret == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "malloc of %ju bytes failed", (uintmax_t)(mlen + 1 + 1));
	return NULL;
    }
    ret[mlen] = '\0';   /* terminate string */
    ret[mlen + 1] = '\0';   /* paranoia */
    beyond = &(ret[mlen]);

    /*
     * JSON encode each byte
     */
    for (i=0, p=ret; i < len; ++i) {
	if (p+jenc[(uint8_t)(ptr[i])].len > beyond) {
	    /* error - clear malloced length */
	    if (retlen != NULL) {
		*retlen = 0;
	    }
	    warn(__func__, "encoding ran beyond end of malloced encoded string");
	    return NULL;
	}
	strcpy(p, jenc[(uint8_t)(ptr[i])].enc);
	p += jenc[(uint8_t)(ptr[i])].len;
    }

    /*
     * return result
     */
    dbg(DBG_VVVHIGH, "returning from malloc_json_encode(ptr, %ju, *%ju)",
		     (uintmax_t)len, (uintmax_t)mlen);
    if (retlen != NULL) {
	*retlen = mlen;
    }
    return ret;
}


/*
 * malloc_json_encode_str - return a JSON encoding of a string
 *
 * This is an simplified interface for malloc_json_encode().
 *
 * given:
 *	str	a string to encode
 *	retlen	address of where to store malloced length, if retlen != NULL
 *
 * returns:
 *	malloced JSON encoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
malloc_json_encode_str(char const *str, size_t *retlen)
{
    void *ret = NULL;	    /* malloced encoding string or NULL */
    size_t len = 0;	    /* length of string to encode */

    /*
     * firewall
     */
    if (str == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    len = strlen(str);
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %ju must be > 0", (uintmax_t) len);
	return NULL;
    }

    /*
     * convert to malloc_json_encode() call
     */
    ret = malloc_json_encode(str, len, retlen);
    if (ret == NULL) {
	dbg(DBG_VVHIGH, "returning NULL for encoding of: <%s>", str);
    } else {
	dbg(DBG_VVHIGH, "string: JSON encoded as: <%s>", (char *)ret);
    }

    /*
     * return encoded result or NULL
     */
    return ret;
}


/*
 * jencchk - validate the contents of the jenc[] table
 *
 * This function performs various sanity checks on the jenc[] table.
 *
 * This function does not return on error.
 */
void
jencchk(void)
{
    unsigned int hexval;/* hex value from xxxx part of \uxxxx */
    char guard;		/* scanf guard to catch excess amount of input */
    int indx;		/* test index */
    char const *encstr;	/* encoding string */
    int ret;		/* libc function return value */
    char str[2];	/* character string to encode */
    char *mstr = NULL;	/* malloced encoding string */
    size_t mlen = 0;	/* length of malloced encoding string */
    char *mstr2 = NULL;	/* malloced strict decoding string */
    size_t mlen2 = 0;	/* length of malloced strict decoding string */
    char *mstr3 = NULL;	/* malloced non-strict decoding string */
    size_t mlen3 = 0;	/* length of malloced non-strict decoding string */
    unsigned int i;

    /*
     * assert: bits in byte must be 8
     */
    if (BITS_IN_BYTE != 8) {
	err(100, __func__, "BITS_IN_BYTE: %d != 8", BITS_IN_BYTE);
	not_reached();
    }

    /*
     * assert: table must be 256 elements long
     */
    if (sizeof(jenc)/sizeof(jenc[0]) != BYTE_VALUES) {
	err(101, __func__, "jenc table as %ju elements instead of %d",
			   (uintmax_t)sizeof(jenc)/sizeof(jenc[0]), BYTE_VALUES);
	not_reached();
    }

    /*
     * assert: byte must be an index from 0 to 256
     */
    for (i=0; i < BYTE_VALUES; ++i) {
	if (jenc[i].byte != i) {
	    err(102, __func__, "jenc[0x%02x].byte: %d != %d", i, jenc[i].byte, i);
	    not_reached();
	}
    }

    /*
     * assert: enc string must be non-NULL
     */
    for (i=0; i < BYTE_VALUES; ++i) {
	if (jenc[i].enc == NULL) {
	    err(103, __func__, "jenc[0x%02x].enc == NULL", i);
	    not_reached();
	}
    }

    /*
     * assert: length of enc string must match len
     */
    for (i=0; i < BYTE_VALUES; ++i) {
	if (strlen(jenc[i].enc) != jenc[i].len) {
	    err(104, __func__, "jenc[0x%02x].enc length: %ju != jenc[0x%02x].len: %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       i, (uintmax_t)jenc[i].len);
	    not_reached();
	}
    }

    /*
     * assert: \x00-\x07 encodes to \uxxxx
     */
    for (i=0x00; i <= 0x07; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(105, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       (uintmax_t)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &hexval, &guard);
	if (ret != 1) {
	    err(106, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != hexval) {
	    err(107, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * assert: \x08 encodes to \b
     */
    indx = 0x08;
    encstr = "\\b";
    if (jenc[indx].len != LITLEN("\\b")) {
	err(108, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(109, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x09 encodes to \t
     */
    indx = 0x09;
    encstr = "\\t";
    if (jenc[indx].len != LITLEN("\\b")) {
	err(110, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(111, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0a encodes to \n
     */
    indx = 0x0a;
    encstr = "\\n";
    if (jenc[indx].len != strlen(encstr)) {
	err(112, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(113, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0b encodes to \u000b
     */
    indx = 0x0b;
    encstr = "\\u000b";
    if (jenc[indx].len != strlen(encstr)) {
	err(114, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(115, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0c encodes to \f
     */
    indx = 0x0c;
    encstr = "\\f";
    if (jenc[indx].len != strlen(encstr)) {
	err(116, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(117, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0d encodes to \r
     */
    indx = 0x0d;
    encstr = "\\r";
    if (jenc[indx].len != strlen(encstr)) {
	err(118, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(119, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x0e-\x1f encodes to \uxxxx
     */
    for (i=0x0e; i <= 0x1f; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(120, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       (uintmax_t)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &hexval, &guard);
	if (ret != 1) {
	    err(121, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != hexval) {
	    err(122, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * assert: \x20-\x21 encodes to the character
     */
    for (i=0x20; i <= 0x21; ++i) {
	if (jenc[i].len != 1) {
	    err(123, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(124, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x22 encodes to \"
     */
    indx = 0x22;
    encstr = "\\\"";
    if (jenc[indx].len != strlen(encstr)) {
	err(125, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(126, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x23-\x25 encodes to the character
     */
    for (i=0x23; i <= 0x25; ++i) {
	if (jenc[i].len != 1) {
	    err(128, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(129, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x26 encodes to \u0026
     */
    indx = 0x26;
    encstr = "\\u0026";
    if (jenc[indx].len != strlen(encstr)) {
	err(130, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(131, __func__, "jenc[0x%02x].enc: <%s> is not <\\b> form", indx, encstr);
	not_reached();
    }

    /*
     * assert: \x27-\x2e encodes to the character
     */
    for (i=0x27; i <= 0x2e; ++i) {
	if (jenc[i].len != 1) {
	    err(132, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(133, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x2f encodes to \/
     */
    indx = 0x2f;
    encstr = "\\/";
    if (jenc[indx].len != strlen(encstr)) {
	err(134, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(135, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x30-\x3b encodes to the character
     */
    for (i=0x30; i <= 0x3b; ++i) {
	if (jenc[i].len != 1) {
	    err(136, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(137, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x3c encodes to \u003c
     */
    indx = 0x3c;
    encstr = "\\u003c";
    if (jenc[indx].len != strlen(encstr)) {
	err(138, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(139, __func__, "jenc[0x%02x].enc: <%s> is not <\\b> form", indx, encstr);
	not_reached();
    }

    /*
     * assert: \x3d-\x3d encodes to the character
     */
    for (i=0x3d; i <= 0x3d; ++i) {
	if (jenc[i].len != 1) {
	    err(140, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(141, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x3e encodes to \u003e
     */
    indx = 0x3e;
    encstr = "\\u003e";
    if (jenc[indx].len != strlen(encstr)) {
	err(142, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(143, __func__, "jenc[0x%02x].enc: <%s> is not in <\\b> form", indx, encstr);
	not_reached();
    }

    /*
     * assert: \x3f-\x5b encodes to the character
     */
    for (i=0x3f; i <= 0x5b; ++i) {
	if (jenc[i].len != 1) {
	    err(144, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(145, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x5c encodes to \\
     */
    indx = 0x5c;
    encstr = "\\\\";
    if (jenc[indx].len != strlen(encstr)) {
	err(146, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			   indx, (uintmax_t)strlen(jenc[indx].enc),
			   (uintmax_t)strlen(encstr));
	not_reached();
    }
    if (strcmp(jenc[indx].enc, encstr) != 0) {
	err(147, __func__, "jenc[0x%02x].enc: <%s> != <%s>", indx, jenc[indx].enc, encstr);
	not_reached();
    }

    /*
     * assert: \x5d-\x7e encodes to the character
     */
    for (i=0x5d; i <= 0x7e; ++i) {
	if (jenc[i].len != 1) {
	    err(148, __func__, "jenc[0x%02x].enc length: %ju != %d",
			       i, (uintmax_t)strlen(jenc[i].enc), 1);
	    not_reached();
	}
	if ((unsigned int)(jenc[i].enc[0]) != i) {
	    err(149, __func__, "jenc[0x%02x].enc: <%s> is not <%c>", i, jenc[i].enc, (char)i);
	    not_reached();
	}
    }

    /*
     * assert: \x7f-\xff encodes to \uxxxx
     */
    for (i=0x7f; i <= 0xff; ++i) {
	if (jenc[i].len != LITLEN("\\uxxxx")) {
	    err(150, __func__, "jenc[0x%02x].enc length: %ju != %ju",
			       i, (uintmax_t)strlen(jenc[i].enc),
			       (uintmax_t)LITLEN("\\uxxxx"));
	    not_reached();
	}
	ret = sscanf(jenc[i].enc, "\\u%04x%c", &hexval, &guard);
	if (ret != 1) {
	    err(151, __func__, "jenc[0x%02x].enc: <%s> is not in <\\uxxxx> form", i, jenc[i].enc);
	    not_reached();
	}
	if (i != hexval) {
	    err(152, __func__, "jenc[0x%02x].enc: <%s> != <\\u%04x> form", i, jenc[i].enc, i);
	    not_reached();
	}
    }

    /*
     * special test for encoding a NUL byte
     */
    dbg(DBG_VVVHIGH, "testing malloc_json_encode(0x00, 1, *mlen)");
    memset(str, 0, sizeof(str));    /* clear all bytes in str, including the final \0 */
    mstr = malloc_json_encode(str, 1,  &mlen);
    if (mstr == NULL) {
	err(153, __func__, "malloc_json_encode(0x00, 1, *mlen: %ju) == NULL", (uintmax_t)mlen);
	not_reached();
    }
    if (mlen != jenc[0].len) {
	err(154, __func__, "malloc_json_encode(0x00, 1, *mlen: %ju != %ju)",
			   (uintmax_t)mlen, (uintmax_t)(jenc[0].len));
	not_reached();
    }
    if (strcmp(jenc[0].enc, mstr) != 0) {
	err(155, __func__, "malloc_json_encode(0x00, 1, *mlen: %ju) != <%s>",
			   (uintmax_t)mlen, jenc[0].enc);
	not_reached();
    }
    /* free the malloced encoded string */
    if (mstr != NULL) {
	free(mstr);
	mstr = NULL;
    }

    /*
     * finally try to encode every possible string with a single non-NUL character
     */
    for (i=1; i < BYTE_VALUES; ++i) {

	/*
	 * test JSON encoding
	 */
	dbg(DBG_VVVHIGH, "testing malloc_json_encode_str(0x%02x, *mlen)", i);
	/* load input string */
	str[0] = (char)i;
	/* test malloc_json_encode_str() */
	mstr = malloc_json_encode_str(str, &mlen);
	/* check encoding result */
	if (mstr == NULL) {
	    err(156, __func__, "malloc_json_encode_str(0x%02x, *mlen: %ju) == NULL",
			       i, (uintmax_t)mlen);
	    not_reached();
	}
	if (mlen != jenc[i].len) {
	    err(157, __func__, "malloc_json_encode_str(0x%02x, *mlen %ju != %ju)",
			       i, (uintmax_t)mlen, (uintmax_t)jenc[i].len);
	    not_reached();
	}
	if (strcmp(jenc[i].enc, mstr) != 0) {
	    err(158, __func__, "malloc_json_encode_str(0x%02x, *mlen: %ju) != <%s>", i,
			       (uintmax_t)mlen, jenc[i].enc);
	    not_reached();
	}
	dbg(DBG_VVHIGH, "testing malloc_json_encode_str(0x%02x, *mlen) encoded to <%s>", i, mstr);

	/*
	 * test strict decoding the JSON encoded string
	 */
	dbg(DBG_VVVHIGH, "testing malloc_json_decode_str(<%s>, *mlen, true)", mstr);
	/* test malloc_json_decode_str() */
	mstr2 = malloc_json_decode_str(mstr, &mlen2, true);
	if (mstr2 == NULL) {
	    err(159, __func__, "malloc_json_decode_str(<%s>, *mlen: %ju, true) == NULL",
			       mstr, (uintmax_t)mlen2);
	    not_reached();
	}
	if (mlen2 != 1) {
	    err(160, __func__, "malloc_json_decode_str(<%s>, *mlen2 %ju != 1, true)",
			       mstr, (uintmax_t)mlen2);
	    not_reached();
	}
	if ((uint8_t)(mstr2[0]) != i) {
	    err(161, __func__, "malloc_json_decode_str(<%s>, *mlen: %ju, true): 0x%02x != 0x%02x",
			       mstr, (uintmax_t)mlen2, (uint8_t)(mstr2[0]), i);
	    not_reached();
	}

	/*
	 * test non-strict decoding the JSON encoded string
	 */
	dbg(DBG_VVVHIGH, "testing malloc_json_decode_str(<%s>, *mlen, false)", mstr);
	/* test malloc_json_decode_str() */
	mstr3 = malloc_json_decode_str(mstr, &mlen3, false);
	if (mstr3 == NULL) {
	    err(162, __func__, "malloc_json_decode_str(<%s>, *mlen: %ju, false) == NULL",
			       mstr, (uintmax_t)mlen3);
	    not_reached();
	}
	if (mlen3 != 1) {
	    err(163, __func__, "malloc_json_decode_str(<%s>, *mlen3 %ju != 1, false)",
			       mstr, (uintmax_t)mlen3);
	    not_reached();
	}
	if ((uint8_t)(mstr3[0]) != i) {
	    err(164, __func__, "malloc_json_decode_str(<%s>, *mlen: %ju, false): 0x%02x != 0x%02x",
			       mstr, (uintmax_t)mlen3, (uint8_t)(mstr3[0]), i);
	    not_reached();
	}

	/* free the malloced encoded string */
	if (mstr != NULL) {
	    free(mstr);
	    mstr = NULL;
	}
	if (mstr2 != NULL) {
	    free(mstr2);
	    mstr2 = NULL;
	}
	if (mstr3 != NULL) {
	    free(mstr3);
	    mstr3 = NULL;
	}
    }

    /*
     * all seems well win the jenc[] table
     */
    dbg(DBG_VVHIGH, "jenc[] table passes");
    return;
}


/*
 * json_putc - print a UTF-8 character with JSON encoding
 *
 * given:
 *	stream	- string to print on
 *	c	- UTF-8 character to encode
 *
 * returns:
 *	true ==> stream print was OK,
 *	false ==> error printing to stream
 */
bool
json_putc(uint8_t const c, FILE *stream)
{
    int ret;			/* libc function return */

    /*
     * firewall
     */
    if (stream == NULL) {
	warn(__func__, "called with NULL arg(s)");
	return false;
    }

    /*
     * write JSON encoding to stream
     */
    ret = fprintf(stream, "%s", jenc[c].enc);
    if (ret <= 0) {
	warnp(__func__, "fprintf #1 error");
	return false;
    }
    return true;
}


/*
 * malloc_json_decode - return a decoding of a block of JSON encoded memory
 *
 * given:
 *	ptr	start of memory block to decode
 *	len	length of block to decode in bytes
 *	retlen	address of where to store malloced length, if retlen != NULL
 *	strict	true ==> strict decoding based on how malloc_json_encode() encodes
 *	        false ==> permit a wider use of \-escaping and un-encoded char
 *
 *		    NOTE: strict == false implies a strict reading of the JSON spec
 *
 * returns:
 *	malloced JSON decoding of a block, or NULL ==> error
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 */
char *
malloc_json_decode(char const *ptr, size_t len, size_t *retlen, bool strict)
{
    char *ret = NULL;	    /* malloced encoding string or NULL */
    char *beyond = NULL;    /* beyond the end of the malloced encoding string */
    size_t mlen = 0;	    /* length of malloced encoded string */
    char *p = NULL;	    /* next place to encode */
    char n = 0;		    /* next character beyond a \\ */
    char a = 0;		    /* 1st hex character after \u */
    int xa = 0;		    /* 1st hex character numeric value */
    char b = 0;		    /* 2nd hex character after \u */
    int xb = 0;		    /* 2nd hex character numeric value */
    char c = 0;		    /* character to decode or 3rd hex character after \u */
    int xc = 0;		    /* 3nd hex character numeric value */
    char d = 0;		    /* 4th hex character after \u */
    int xd = 0;		    /* 4th hex character numeric value */
    size_t i;

    /*
     * firewall
     */
    if (ptr == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %ju must be > 0", (uintmax_t) len);
	return NULL;
    }

    /*
     * count the bytes that will be in the decoded malloced string
     */
    for (i=0; i < len; ++i) {

	/*
	 * examine the current character
	 */
	c = (char)((uint8_t)(ptr[i]));

	/*
	 * valid non-\-escaped characters count as 1
	 */
        if (c != '\\') {

	    /*
	     * case: strict encoding based on malloc_json_encode() \-escapes
	     */
	    if (strict == true) {

		/*
		 * disallow characters that should have been escaped
		 */
		if (c <= 0x1f || c >= 0x7f) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "strict encoding at %ju found unescaped char: 0x%02x", (uintmax_t)i, (uint8_t)c);
		    return NULL;
		}
		switch (c) {
		case '"':   /*fallthru*/
		case '/':   /*fallthru*/
		case '\\':  /*fallthru*/
		case '&':   /*fallthru*/
		case '<':   /*fallthru*/
		case '>':
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "strict encoding at %ju found unescaped char: %c", (uintmax_t)i, c);
		    return NULL;
		    break;

		/*
		 * count a valid character
		 */
		default:
		    ++mlen;
		    break;
		}

	    /*
	     * case: non-strict encoding - only what JSON mandates
	     */
	    } else {

		/*
		 * disallow characters that should have been escaped
		 */
		switch (c) {
		case '\b':  /*fallthru*/
		case '\t':  /*fallthru*/
		case '\n':  /*fallthru*/
		case '\f':  /*fallthru*/
		case '\r':
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "non-strict encoding found \\-escaped char: 0x%02x", (uint8_t)c);
		    return NULL;
		    break;
		case '"':   /*fallthru*/
		case '/':   /*fallthru*/
		case '\\':
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "non-strict encoding found \\-escaped char: %c", c);
		    return NULL;
		    break;

		/*
		 * count a valid character
		 */
		default:
		    ++mlen;
		    break;
		}
	    }

	/*
	 * valid \-escaped characters count as 2 or 6
	 */
	} else {

	    /*
	     * there must be at least 1 more character beyond \
	     */
	    if (i+1 >= len) {
		/* error - clear malloced length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found \\ at end of buffer, missing next character");
		return NULL;
	    }

	    /*
	     * look at the next character beyond \
	     */
	    n = (char)((uint8_t)(ptr[i+1]));

	    /*
	     * process single \c escaped pairs
	     */
	    switch (n) {
	    case 'b':	/*fallthru*/
	    case 't':	/*fallthru*/
	    case 'n':	/*fallthru*/
	    case 'f':	/*fallthru*/
	    case 'r':	/*fallthru*/
	    case '"':	/*fallthru*/
	    case '/':	/*fallthru*/
	    case '\\':
		/*
		 * count \c escaped pair as 1 character
		 */
		++mlen;
		++i;
		break;

	    /*
	     * process \uxxxx where x is a hex character
	     */
	    case 'u':

		/*
		 * there must be at least 5 more characters beyond \
		 */
		if (i+5 >= len) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "found \\u, but not enough for 4 hex chars at end of buffer");
		    return NULL;
		}
		a = (char)((uint8_t)(ptr[i+2]));
		b = (char)((uint8_t)(ptr[i+3]));
		c = (char)((uint8_t)(ptr[i+4]));
		d = (char)((uint8_t)(ptr[i+5]));

		/*
		 * the next 4 characters beyond \u must be hex characters
		 */
		if (isxdigit(a) && isxdigit(b) && isxdigit(c) && isxdigit(d)) {

		    /*
		     * case: strict encoding - the 4 hex characters must be lower case
		     */
		    if (strict == true &&
		        (isupper(a) || isupper(b) || isupper(c) || isupper(d))) {
			/* error - clear malloced length */
			if (retlen != NULL) {
			    *retlen = 0;
			}
			warn(__func__, "strict mode found \\u, some of the  4 hex chars are UPPERCASE: 0x%02X%02X%02X%02X",
				       (uint8_t)a, (uint8_t)b, (uint8_t)c, (uint8_t)d);
			return NULL;
		    }

		    /*
		     * case: \u00xx is 1 character
		     */
		    if (a == '0' && b == '0') {
			++mlen;
			i += 5;

		    /*
		     * case: count \uxxxx as 2 characters
		     */
		    } else {
			mlen += 2;
			i += 5;
		    }
		    break;

		/*
		 * case: \uxxxx is invalid because xxxx is not HEX
		 */
		} else {
			warn(__func__, "\\u, not foolowed by 4 hex chars");
			return NULL;
		}
		break;

	    /*
	     * valid C escape sequence but unusual JSON \-escape character
	     */
	    case 'a':	/* ASCII bell */ /*fallthru*/
	    case 'v':	/* ASCII vertical tab */ /*fallthru*/
	    case 'e':	/* ASCII escape */

		/*
		 * case: strict mode - invalid unusual \-escape character
		 */
		if (strict == true) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "strict mode found an unusual JSON \\-escape: \\%c", (uint8_t)c);
		    return NULL;
		}

		/*
		 * case: non-strict - count \c escaped pair as 1 character
		 */
		++mlen;
		++i;
		break;

	    /*
	     * found invalid JSON \-escape character
	     */
	    default:
		/* error - clear malloced length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found invalid JSON \\-escape: followed by 0x%02x", (uint8_t)c);
		return NULL;
	    }
	}
    }

    /*
     * malloced decoded string
     */
    ret = malloc(mlen + 1 + 1);
    if (ret == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "malloc of %ju bytes failed", (uintmax_t)(mlen + 1 + 1));
	return NULL;
    }
    ret[mlen] = '\0';   /* terminate string */
    ret[mlen + 1] = '\0';   /* paranoia */
    beyond = &(ret[mlen]);

    /*
     * JSON string decode
     *
     * In the above counting code, prior to the malloc for the decoded string,
     * we determined that the JSON encoded string is valid.  We can now safely
     * decode regardless of strict mode.
     */
    for (i=0, p=ret; i < len; ++i) {

	/*
	 * examine the current character
	 */
	c = (char)((uint8_t)(ptr[i]));

	/*
	 * paranoia
	 */
	if (p >= beyond) {
	    /* error - clear malloced length */
	    if (retlen != NULL) {
		*retlen = 0;
	    }
	    warn(__func__, "ran beyond end of decoded string");
	    return NULL;
	}

	/*
	 * case: JSON decode non \-escape character
	 */
	if (c != '\\') {
	    /* no translation encoding */
	    *p++ = c;

	/*
	 * case: JSON decode \-escape character
	 */
	} else {

	    /*
	     * look at the next character beyond \
	     */
	    n = (char)((uint8_t)(ptr[i+1]));

	    /*
	     * decode single \c escaped pairs
	     */
	    switch (n) {
	    case 'b':	/* ASCII backspace */
		++i;
		*p++ = '\b';
		break;
	    case 't':	/* ASCII horizontal tab */
		++i;
		*p++ = '\t';
		break;
	    case 'n':	/* ASCII line feed */
		++i;
		*p++ = '\n';
		break;
	    case 'f':	/* ASCII form feed */
		++i;
		*p++ = '\f';
		break;
	    case 'r':	/* ASCII carriage return */
		++i;
		*p++ = '\r';
		break;
	    case 'a':	/* ASCII bell */
		++i;
		*p++ = '\a';
		break;
	    case 'v':	/* ASCII vertical tab */
		++i;
		*p++ = '\v';
		break;
	    case 'e':	/* ASCII escape */
		++i;
		*p++ = 0x0b;  /* no all C compilers understand /e */
		break;
	    case '"':	/*fallthru*/
	    case '/':	/*fallthru*/
	    case '\\':
		++i;
		*p++ = n;	/* escape decodes to itself */
		break;

	    /*
	     * decode \uxxxx
	     */
	    case 'u':

		/*
		 * there must be at least 5 more characters beyond \
		 */
		if (i+5 >= len) {
		    /* error - clear malloced length */
		    if (retlen != NULL) {
			*retlen = 0;
		    }
		    warn(__func__, "found \\u while decoding, but not enough for 4 hex chars at end of buffer");
		    return NULL;
		}
		xa = hexval[(uint8_t)(ptr[i+2])];
		xb = hexval[(uint8_t)(ptr[i+3])];
		xc = hexval[(uint8_t)(ptr[i+4])];
		xd = hexval[(uint8_t)(ptr[i+5])];

		/*
		 * case: \u00xx
		 */
		if (xa == 0 && xb == 0) {
		    /* single byte \u00xx */
		    i += 5;
		    *p++ = (char)((xc << 4) | xd);

		/*
		 * case: \uxxxx
		 */
		} else {

		    /*
		     * paranoia
		     */
		    if (p+1 >= beyond) {
			/* error - clear malloced length */
			if (retlen != NULL) {
			    *retlen = 0;
			}
			warn(__func__, "ran beyond end of decoded string for non-UTF-8 \\u encoding");
			return NULL;
		    }
		    /* double byte \uxxxx */
		    i += 5;
		    *p++ = (char)((xa << 4) | xb);
		    *p++ = (char)((xc << 4) | xd);
		}
		break;

	    /*
	     * unknown \c escaped pairs
	     */
	    default:
		/* error - clear malloced length */
		if (retlen != NULL) {
		    *retlen = 0;
		}
		warn(__func__, "found invalid JSON \\-escape while decoding: followed by 0x%02x", (uint8_t)c);
		return NULL;
	    }
	}
     }

    /*
     * return result
     */
    dbg(DBG_VVVHIGH, "returning from malloc_json_decode(ptr, %ju, *%ju)",
		     (uintmax_t)len, (uintmax_t)mlen);
    if (retlen != NULL) {
	*retlen = mlen;
    }
    return ret;
}


/*
 * malloc_json_decode_str - return a JSON decoding of a string
 *
 * This is a simplified interface for malloc_json_decode().
 *
 * given:
 *	str	a string to decode
 *	retlen	address of where to store malloced length, if retlen != NULL
 *	strict	true ==> strict decoding based on how malloc_json_decode() decodes
 *	        false ==> permit a wider use of \-escaping and un-decoded char
 *
 *		    NOTE: strict == false implies a strict reading of the JSON spec
 *
 * returns:
 *	malloced JSON decoding of a block, or NULL ==> error
 *
 * NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function assumes that str is a valid C-style string with a terminating
 *	 NUL byte.  This code assumes that the first NUL byte found is the end of
 *	 the string.  When working with raw binary data, or data that could have
 *	 a NUL byte inside the block of memory (instead at the very end), this
 *	 function should NOT be used.  In such cases, malloc_json_decode() should
 *	 be used instead!
 */
char *
malloc_json_decode_str(char const *str, size_t *retlen, bool strict)
{
    void *ret = NULL;	    /* malloced decoding string or NULL */
    size_t len = 0;	    /* length of string to decode */

    /*
     * firewall
     */
    if (str == NULL) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "called with NULL ptr");
	return NULL;
    }
    len = strlen(str);
    if (len <= 0) {
	/* error - clear malloced length */
	if (retlen != NULL) {
	    *retlen = 0;
	}
	warn(__func__, "len: %ju must be > 0", (uintmax_t) len);
	return NULL;
    }

    /*
     * convert to malloc_json_decode() call
     */
    ret = malloc_json_decode(str, len, retlen, strict);
    if (ret == NULL) {
	dbg(DBG_VVHIGH, "returning NULL for decoding of: <%s>", str);
    } else {
	dbg(DBG_VVHIGH, "string: <%s> JSON decoded", str);
    }

    /*
     * return decoded result or NULL
     */
    return ret;
}

/*
 * jwarn - issue a JSON warning message
 *
 * given:
 *	code	    warning code
 *	program	    name of program e.g. jinfochk, jauthchk etc.
 *	name	    name of function issuing the warning
 *	filename    filename of json file, "stdin" or NULL
 *	line	    JSON line
 *	line_num    the offending line number in the json file
 *	fmt	    format of the warning
 *	...	    optional format args
 *
 * Example:
 *
 *	jwarn(JSON_CODE(1), program, __func__, file, line, __LINE__, "unexpected foobar: %d", value);
 *
 * XXX As of 13 March 2022 the line will be empty but in time this should be
 * changed to be the offending JSON text and the offending JSON line number
 * (which currently is done but the way it's done might change). Instead the
 * field and values are usually shown as part of the optional format args.
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return on code < JSON_CODE_RESERVED_MIN (0).
 */
void
jwarn(int code, char const *program, char const *name, char const *filename, char const *line, int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    if (is_json_code_ignored(code))
	return;

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (program == NULL) {
	program = "((NULL program))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL program, forcing name: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (code < JSON_CODE_RESERVED_MIN) {
	err(165, __func__, "invalid JSON code passed to jwarn(): %d", code);
	not_reached();
    }
    if (line == NULL) {
	/* currently line will be NULL so we make it empty */
	line = "";
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jwarn(): called with NULL filename, forcing filename: %s\n", filename);
    }


    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "{JSON-%04d}: %s: %d: ", code, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}

/*
 * jwarnp - issue a JSON warning message with errno information
 *
 * given:
 *	code	    warning code
 *	program	    name of program e.g. jinfochk, jauthchk etc.
 *	name	    name of function issuing the warning
 *	filename    filename of json file, "stdin" or NULL
 *	line	    JSON line
 *	line_num    the offending line number in the json file
 *	fmt	    format of the warning
 *	...	    optional format args
 *
 * Example:
 *
 *	jwarn(JSON_CODE(1), program, __func__, file, line, __LINE__, "unexpected foobar: %d", value);
 *
 * XXX As of 13 March 2022 the line will be empty but in time this should be
 * changed to be the offending JSON text and the offending JSON line number
 * (which currently is done but the way it's done might change). Instead the
 * field and values are usually shown as part of the optional format args.
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return on code < JSON_CODE_RESERVED_MIN (0).
 */
void
jwarnp(int code, char const *program, char const *name, char const *filename, char const *line,
       int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
    int saved_errno;	/* errno at function start */

    if (is_json_code_ignored(code))
	return;

    /*
     * save errno so we can restore it before returning
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (program == NULL) {
	program = "((NULL program))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL program, forcing name: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nWarning: in jwarn(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (code < JSON_CODE_RESERVED_MIN) {
	err(166, __func__, "invalid JSON code passed to jwarn(): %d", code);
	not_reached();
    }
    if (line == NULL) {
	/* currently line will be NULL so we make it empty */
	line = "";
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jwarn(): called with NULL filename, forcing filename: %s\n", filename);
    }


    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "{JSON-%04d}: %s: %d: ", code, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "errno[%d]: %s\n", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf with errno returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jwarn(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", code, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * restore previous errno value
     */
    errno = saved_errno;
    return;
}

/*
 * jerr - issue a fatal JSON error message and exit
 *
 * given:
 *	exitcode	value to exit with
 *	program		program or NULL (will be set to "jchk")
 *	name		name of function issuing the error
 *	filename	file with the problem
 *	line		line with the problem (or NULL)
 *	line_num	line number with the problem or -1
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	jerr(JSON_CODE(1), __func__, program, file, line, line_num, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
void
jerr(int exitcode, char const *program, char const *name, char const *filename, char const *line,
     int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /*
     * firewall
     */
    if (exitcode < 0) {
	warn(__func__, "\nin jerr(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin jerr(): forcing exit code: %d\n", exitcode);
    }
    if (program == NULL) {
	program = "jchk";
	dbg(DBG_VHIGH, "\nin jerr(): called with NULL program, forcing program: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin jerr(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin jerr(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jerr(): called with NULL filename, forcing filename: %s\n", filename);
    }
    if (line == NULL) {
	line = "";
	dbg(DBG_VHIGH, "jerr(): called with NULL line, making \"\"");
    }

    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "JSON[%04d]: %s: %d: ", exitcode, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "vfprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerr(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/*
 * jerrp - issue a fatal error message with errno information and exit
 *
 * given:
 *	exitcode	value to exit with
 *	program		program or NULL (will be set to "jchk")
 *	name		name of function issuing the warning
 *	filename	file with the problem
 *	line		line with the problem or NULL
 *	line_num	line number of the problem or -1
 *	fmt		format of the warning
 *	...		optional format args
 *
 * Example:
 *
 *	jerrp(JSON_CODE(1), program, __func__, file, line, line_num, "bad foobar: %s", message);
 *
 * NOTE: We warn with extra newlines to help internal fault messages stand out.
 *	 Normally one should NOT include newlines in warn messages.
 *
 * This function does not return.
 */
void
jerrp(int exitcode, char const *program, char const *name, char const *filename, char const *line,
      int line_num, char const *fmt, ...)
{
    va_list ap;		/* argument pointer */
    int ret;		/* libc function return code */
    int saved_errno;	/* errno value when called */

    /*
     * save errno in case we need it for strerror()
     */
    saved_errno = errno;

    /*
     * start the var arg setup and fetch our first arg
     */
    va_start(ap, fmt);

    /* firewall */
    if (exitcode < 0) {
	warn(__func__, "\nin jerrp(): called with exitcode <0: %d\n", exitcode);
	exitcode = 255;
	warn(__func__, "\nin jerrp(): forcing exit code: %d\n", exitcode);
    }
    if (program == NULL) {
	program = "jchk";
	dbg(DBG_VHIGH, "\nin jerrp(): called with NULL program, forcing program: %s\n", program);
    }
    if (name == NULL) {
	name = "((NULL name))";
	warn(__func__, "\nin jerrp(): called with NULL name, forcing name: %s\n", name);
    }
    if (fmt == NULL) {
	fmt = "((NULL fmt))";
	warn(__func__, "\nin jerrp(): called with NULL fmt, forcing fmt: %s\n", fmt);
    }
    if (filename == NULL) {
	filename = "((NULL))";
	dbg(DBG_VHIGH, "jerrp(): called with NULL filename, forcing filename: %s\n", filename);
    }
    if (line == NULL) {
	line = "";
	dbg(DBG_VHIGH, "jerrp(): called with NULL line, making \"\"");
    }


    errno = 0;
    ret = fprintf(stderr, "# program: %s %s\n", program, name);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, "JSON[%04d]: %s: %d: ", exitcode, filename, line_num);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = vfprintf(stderr, fmt, ap);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "vfprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fprintf(stderr, " errno[%d]: %s", saved_errno, strerror(saved_errno));
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fprintf returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fputc('\n', stderr);
    if (ret != '\n') {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fputc returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    errno = 0;
    ret = fflush(stderr);
    if (ret < 0) {
	(void) fprintf(stderr, "\nWarning: in jerrp(%d, %s, %s, %s, %s, %d, %s, ...): "
			       "fflush returned error: %s\n", exitcode, program, name,
		filename, line, line_num, fmt, strerror(errno));
    }

    /*
     * clean up stdarg stuff
     */
    va_end(ap);

    /*
     * terminate with exit code
     */
    exit(exitcode);
    not_reached();
}


/* find_json_field_in_table	    - find field 'name' in json table
 *
 * given:
 *
 *	table			    - the table to check
 *	name			    - the name to check
 *	loc			    - if != NULL set to index
 *
 * NOTE: The table MUST end with NULL!
 *
 * NOTE: If loc != NULL and the name is not found in the table then in the
 * calling function loc will be the number of entries in the table. To get the
 * number of entries in the table pass in a NULL pointer or zero length name.
 *
 * This function will return NULL if the field is not in the table. It will not
 * return if table is NULL.
 */
struct json_field *
find_json_field_in_table(struct json_field *table, char const *name, size_t *loc)
{
    struct json_field *field = NULL;	/* the found field or NULL if not found */
    size_t i = 0;			/* current index */
    /*
     * firewall
     */
    if (table == NULL) {
	jerr(JSON_CODE_RESERVED(5), NULL, __func__, __FILE__, NULL, __LINE__, "passed NULL table");
	not_reached();
    }

    /* zero length name is equivalent to NULL */
    if (name != NULL && strlen(name) <= 0) {
	name = NULL;
    }

    /*
     * While the current index's name is not what we're looking for, continue to
     * the next field in the table.
     */
    for (i = 0; table[i].name != NULL; ++i) {
	if (name && !strcmp(table[i].name, name)) {
	    field = &table[i];
	    break;
	}
    }

    if (loc != NULL) {
	*loc = i;
    }

    return field;
}

/* check_json_fields_tables	- check the three JSON fields tables
 *
 * This function calls the functions check_common_json_fields_table(),
 * check_info_json_fields_table() and check_author_json_fields_table() to make
 * sure that they're all sane.
 *
 * This means that the only element with the JSON_EOT type is the last element,
 * that the field types are valid (see json.h), that there are no embedded NULL
 * elements (name == NULL) and that the final element _IS_ NULL.
 *
 * This function does not return if any of these conditions are not met in any
 * of the tables.
 */
void
check_json_fields_tables(void)
{
    dbg(DBG_MED, "Running sanity checks on common_json_fields table ...");
    check_common_json_fields_table();
    dbg(DBG_MED, "... all OK.");

    dbg(DBG_MED, "Running sanity checks on info_json_fields table ...");
    check_info_json_fields_table();
    dbg(DBG_MED, "... all OK.");

    dbg(DBG_MED, "Running sanity checks on author_json_fields table ...");
    check_author_json_fields_table();
    dbg(DBG_MED, "... all OK.");
}

/* check_common_json_fields_table	    - perform some sanity checks on the
 *					      common_json_fields table
 *
 * This function checks if JSON_EOT is used on any field other than the NULL
 * field. It also makes sure that each field_type is valid. Additionally it
 * makes sure that there are no NULL elements before the final element and that
 * the final element _IS_ NULL. It also makes sure the table is not empty.
 *
 * These sanity checks are performed on the common_json_fields table.
 *
 * This function does not return on error.
 */
void
check_common_json_fields_table(void)
{
    size_t i;
    size_t max = SIZEOF_COMMON_JSON_FIELDS_TABLE;

    if (max <= 0) {
	err(167, __func__, "bogus common_json_fields table length: %ju <= 0", (uintmax_t)max);
	not_reached();
    }

    for (i = 0; i < max-1 && common_json_fields[i].name != NULL; ++i) {
	switch (common_json_fields[i].field_type) {
	    case JSON_EOT:
		if (common_json_fields[i].name != NULL) {
		    jerr(JSON_CODE_RESERVED(1), NULL, __func__, __FILE__, NULL, __LINE__,
						"found JSON_EOT element with non NULL name '%s' location %ju "
						"in common_json_fields table; fix table and recompile",
                            common_json_fields[i].name, (uintmax_t)i);
		    not_reached();
		}
		break;
	    case JSON_NUM:
	    case JSON_BOOL:
	    case JSON_CHARS:
	    case JSON_ARRAY:
	    case JSON_ARRAY_NUMBER:
	    case JSON_ARRAY_BOOL:
	    case JSON_ARRAY_CHARS:
		/* these are all the valid types */
		break;
	    default:
		jerr(JSON_CODE_RESERVED(2), NULL, __func__, __FILE__, NULL, __LINE__,
					    "found invalid data_type in common_json_fields table location %ju; "
					    "fix table and recompile", (uintmax_t)i);
		not_reached();
		break;
	}
    }
    if (max - 1 != i) {
	jerr(JSON_CODE_RESERVED(3), NULL, __func__, __FILE__, NULL, __LINE__,
				    "found embedded NULL element in common_json_fields table at location %ju; "
				    "fix table and recompile", (uintmax_t)i);
	not_reached();
    }
    if (common_json_fields[i].name != NULL) {
	jerr(JSON_CODE_RESERVED(4), NULL, __func__, __FILE__, NULL, __LINE__,
				    "no final NULL element found in common_json_fields table; "
				    "fix table and recompile");
	not_reached();
    }
}

/*
 * check_info_json_fields_table	 - sanity check info_json_fields table
 *
 * This function checks if JSON_EOT is used on any field other than the NULL
 * field. It also makes sure that each field_type is valid. Additionally it
 * makes sure that there are no NULL elements before the final element and that
 * the final element _IS_ NULL. It also makes sure the table is not empty.
 *
 * These sanity checks are performed on the info_json_fields table.
 *
 * This function does not return on error.
 */
void
check_info_json_fields_table(void)
{
    size_t i;
    size_t max = SIZEOF_INFO_JSON_FIELDS_TABLE;
    bool found_manifest = false;

    if (max <= 0) {
	err(168, __func__, "bogus info_json_fields table length: %ju <= 0", (uintmax_t)max);
	not_reached();
    }

    for (i = 0; i < max - 1 && info_json_fields[i].name != NULL; ++i) {
	if (info_json_fields[i].name != NULL && !strcmp(info_json_fields[i].name, "manifest")) {
	    found_manifest = true;
	}

	switch (info_json_fields[i].field_type) {
	    case JSON_EOT:
		if (info_json_fields[i].name != NULL) {
		    jerr(JSON_CODE_RESERVED(1), NULL, __func__, __FILE__, NULL, __LINE__,
						"found JSON_EOT element with non NULL name '%s' location %ju "
						"in info_json_fields table; fix table and recompile",
			    info_json_fields[i].name, (uintmax_t)i);
		    not_reached();
		}
		break;
	    case JSON_NUM:
	    case JSON_BOOL:
	    case JSON_CHARS:
	    case JSON_ARRAY:
	    case JSON_ARRAY_NUMBER:
	    case JSON_ARRAY_BOOL:
	    case JSON_ARRAY_CHARS:
		/* these are all the valid types */
		break;
	    default:
		jerr(JSON_CODE_RESERVED(2), NULL, __func__, __FILE__, NULL, __LINE__,
					    "found invalid data_type in info_json_fields table location %ju; "
					    "fix table and recompile", (uintmax_t)i);
		not_reached();
		break;
	}
    }

    if (max - 1 != i) {
	jerr(JSON_CODE_RESERVED(3), NULL, __func__, __FILE__, NULL, __LINE__,
				    "found embedded NULL element in info_json_fields table at location %ju; "
				    "fix table and recompile", (uintmax_t)i);
	not_reached();
    }

    if (info_json_fields[i].name != NULL) {
	jerr(JSON_CODE_RESERVED(4), NULL, __func__, __FILE__, NULL, __LINE__,
				    "no final NULL element found in info_json_fields table; "
				    "fix table and recompile");
	not_reached();
    }
    if (!found_manifest) {
	err(169, __func__, "'manifest' field not found in info_json_fields table; "
			   "fix table in %s and recompile", __FILE__);
	not_reached();
    }
}

/*
 * check_author_json_fields_table - perform author_json_fields table sanity checks
 *
 * This function checks if JSON_EOT is used on any field other than the NULL
 * field. It also makes sure that each field_type is valid.  Additionally it
 * makes sure that there are no NULL elements before the final element and that
 * the final element _IS_ NULL. It also makes sure the table is not empty.
 *
 * The sanity checks are performed on the author_json_fields table.
 *
 * This function does not return on error.
 */
void
check_author_json_fields_table(void)
{
    size_t i;
    size_t max = SIZEOF_AUTHOR_JSON_FIELDS_TABLE;
    bool found_authors = false;

    if (max <= 0) {
	err(170, __func__, "bogus author_json_fields table length: %ju <= 0", (uintmax_t)max);
	not_reached();
    }

    for (i = 0; i < max - 1 && author_json_fields[i].name != NULL; ++i) {
	if (author_json_fields[i].name != NULL && !strcmp(author_json_fields[i].name, "authors")) {
	    found_authors = true;
	}

	switch (author_json_fields[i].field_type) {
	    case JSON_EOT:
		if (author_json_fields[i].name != NULL) {
		    jerr(JSON_CODE_RESERVED(1), NULL, __func__, __FILE__, NULL, __LINE__,
						"found JSON_EOT element with non NULL name '%s' location %ju "
						"in author_json_fields table; fix table and recompile",
                            author_json_fields[i].name, (uintmax_t)i);
		    not_reached();
		}
		break;
	    case JSON_NUM:
	    case JSON_BOOL:
	    case JSON_CHARS:
	    case JSON_ARRAY:
	    case JSON_ARRAY_NUMBER:
	    case JSON_ARRAY_BOOL:
	    case JSON_ARRAY_CHARS:
		/* these are all the valid types */
		break;
	    default:
		jerr(JSON_CODE_RESERVED(2), NULL, __func__, __FILE__, NULL, __LINE__,
					    "found invalid data_type in author_json_fields table location %ju; "
					    "fix table and recompile", (uintmax_t)i);
		not_reached();
		break;
	}
    }

    if (max - 1 != i) {
	jerr(JSON_CODE_RESERVED(3), NULL, __func__, __FILE__, NULL, __LINE__,
				    "found embedded NULL element in author_json_fields table at location %ju; "
				    "fix table and recompile", (uintmax_t)i);
	not_reached();
    }
    if (author_json_fields[i].name != NULL) {
	jerr(JSON_CODE_RESERVED(4), NULL, __func__, __FILE__, NULL, __LINE__,
				    "no final NULL element found in author_json_fields table; "
				    "fix table and recompile");
	not_reached();
    }
    if (!found_authors) {
	err(171, __func__, "'authors' field not found in authors_json_fields table; fix table in %s and recompile", __FILE__);
	not_reached();
    }

}


/*
 * json_filename - determine basename of JSON type
 *
 * This function will return ".info.json", ".author.json" or "null" depending on
 * the value of type.
 *
 * given:
 *
 *	type	    - INFO_JSON or AUTHOR_JSON
 *
 * Returns a char * depending on type or "null" (NOT a NULL pointer!) if invalid
 * type.
 *
 * This function never returns NULL.
 *
 */
char const *
json_filename(int type)
{
    char const *name = INVALID_JSON_FILENAME; /* "null" */

    switch (type) {
	case INFO_JSON:
	    return INFO_JSON_FILENAME; /* ".info.json" */
	    break; /* in case the return is ever removed */
	case AUTHOR_JSON:
	    return AUTHOR_JSON_FILENAME; /* ".author.json" */
	    break; /* in case the return is ever removed */
	default:
	    break; /* in case the return is ever removed ... which it ironically was. */
    }
    return name;
}


/*
 * check_first_json_char - check if first char is ch
 *
 * given:
 *
 *	file		- path to file
 *	data		- the data read in from the file
 *	strict		- true ==> disallow anything (including whitespace) before the first '{'.
 *	first		- if != NULL set *first to the first character
 *
 * Returns 0 if first character is ch and 1 if it is not.
 *
 * Sets *first to the first character (for debugging purposes).
 *
 * Does not return on NULL.
 *
 * NOTE: don't use jerr() here because this function will very possibly be
 * removed once a proper json parser is implemented so we don't want to use up
 * any json error codes.
 */
int
check_first_json_char(char const *file, char *data, bool strict, char **first, char ch)
{
    /*
     * firewall
     */
    if (data == NULL || strlen(data) == 0) {
	err(172, __func__, "passed NULL or zero length data");
	not_reached();
    } else if (file == NULL || first == NULL) {
	err(173, __func__, "passed NULL arg(s)");
	not_reached();
    }

    if (!strict) {
	while (*data && isspace(*data))
	    ++data;
    }
    if (first != NULL) {
	*first = data;
    }

    if (*data != ch)
	return 1;
    return 0;
}


/*
 * check_last_json_char - check if last char is ch
 *
 * given:
 *
 *	file		- path to file
 *	data		- the data read in from the file
 *	strict		- true ==> permit only a single trailing newline ("\n") after the last '}'.
 *	last		- if != NULL set *last to last char
 *	ch		- the char to check that the last char is
 *
 * Returns 0 if last character is ch and 1 if it is not.
 *
 * Does not return on error.
 *
 * NOTE: don't use jerr() here because this function will very possibly be
 * removed once a proper json parser is implemented so we don't want to use up
 * any json error codes.
 */
int
check_last_json_char(char const *file, char *data, bool strict, char **last, char ch)
{
    char *p;

    /*
     * firewall
     */
    if (data == NULL || strlen(data) == 0) {
	err(174, __func__, "passed NULL or zero length data");
	not_reached();
    } else if (file == NULL || last == NULL) {
	err(175, __func__, "passed NULL arg(s)");
	not_reached();
    }

    p = data + strlen(data) - 1;

    if (!strict) {
	while (*p && isspace(*p))
	    --p;
    }
    else if (*p && *p == '\n') {
	--p;
    }
    if (last != NULL) {
	*last = p;
    }
    if (*p != ch)
	return 1;

    return 0;
}


/*
 * add_found_common_json_field - add to found_common_json_fields list
 *
 * This function will add a common JSON field name and value to the found_common_json_fields list.
 *
 * given:
 *
 *	json_filename	- json file being parsed (the path to)
 *	name		- name of the field
 *	val		- value of the field
 *	line_num	- 'line number' of the value
 *
 * Allocates a struct json_field * and add it to the found_json_common_fields
 * list. This will be used for reporting errors after parsing the file as well
 * as (later on) verifying that these fields are present in both .info.json and
 * .author.json (this detail is not yet completely thought out).
 *
 * This function returns the newly allocated struct json_field OR if the field
 * was already in the list, the previous one with the count incremented and the
 * value added to the values list.
 */
struct json_field *
add_found_common_json_field(char const *json_filename, char const *name, char const *val, int line_num)
{
    struct json_field *field; /* newly allocated field */
    struct json_field *field_in_table = NULL;
    size_t loc = 0; /* location in table */

    /*
     * firewall
     */
    if (json_filename == NULL || name == NULL || val == NULL) {
	jerr(JSON_CODE_RESERVED(6), NULL, __func__, __FILE__, NULL, __LINE__, "passed NULL arg(s)");
	not_reached();
    }

    field_in_table = find_json_field_in_table(common_json_fields, name, &loc);
    if (field_in_table == NULL) {
	jerr(JSON_CODE_RESERVED(12), NULL, __func__, json_filename, NULL, __LINE__,
				    "couldn't add value '%s' to field '%s' file %s",
				    val, name, json_filename);
	err(176, __func__, "called add_found_common_json_field() on uncommon field '%s'", name);
	not_reached();
    }
    /*
     * Set in table that it's found and increment the number of times it's been
     * seen.
     */
    common_json_fields[loc].found = true;
    common_json_fields[loc].count++;
    for (field = found_common_json_fields; field != NULL; field = field->next) {
	if (field->name && !strcmp(field->name, name)) {
	    field->count++;
	    if (add_json_value(json_filename, field, val, line_num) == NULL) {
		jerr(JSON_CODE_RESERVED(7), NULL, __func__, __FILE__, NULL, __LINE__,
					    "couldn't add value '%s' to field '%s' file %s",
					    val, field->name, json_filename);
		not_reached();
	    }
	    return field;
	}
    }

    field = new_json_field(json_filename, name, val, line_num);
    if (field == NULL) {
	/* this should NEVER be reached but we check just to be sure */
	jerr(JSON_CODE_RESERVED(8), NULL, __func__, __FILE__, NULL, __LINE__,
				    "error creating new struct json_field * for field '%s' value '%s' file %s",
				    name, val, json_filename);
	not_reached();
    }

    /* add field to found_common_json_fields list */
    field->next = found_common_json_fields;
    found_common_json_fields = field;

    dbg(DBG_HIGH, "added field '%s' value '%s' to found_common_json_fields", field->name, val);
    return field;
}


/*
 * add_common_json_field - add name, if it is common, to the common list
 *
 * This function will determine of a name is common to both the .info.json
 * as well as to .author.json.  If it is, then it will be added to the
 * common list.
 *
 * given:
 *
 *	program		- which util called this (jinfochk or jauthchk)
 *	json_filename	- the file being parsed (path to)
 *	name		- the field name
 *	val		- the value of the field
 *	line_num	- the 'line number' of the value
 *
 * returns:
 *	true ==> if the name is common to both files and the field was added to
 *		 the found list
 *	false ==> if it's not one of the common field names
 *
 * NOTE: Does not return on error (NULL pointers).
 */
bool
add_common_json_field(char const *program, char const *json_filename, char *name, char *val, int line_num)
{
    bool ret = true;	/* return value: true ==> known field and added to list, false ==> not a common field */
    struct json_field *field = NULL; /* the field in the common_json_fields table if found */
    size_t loc = 0; /* location in the common_json_fields table */

    /*
     * firewall
     */
    if (program == NULL || json_filename == NULL || name == NULL || val == NULL) {
	err(177, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * check if the name is an expected common field
     */
    field = find_json_field_in_table(common_json_fields, name, &loc);
    if (field != NULL) {
	dbg(DBG_HIGH, "found common field '%s' value '%s'", field->name, val);
	add_found_common_json_field(json_filename, field->name, val, line_num);
    } else {
	ret = false;
    }
    return ret;
}


/*
 * check_found_common_json_fields - found_common_json_fields table checks
 *
 * The found_common_json_fields table will be checked to determine if
 * all fields have a valid value. It also checks that the expected fields are in
 * the file.
 *
 * given:
 *
 *	program		- which util called this (jinfochk or jauthchk)
 *	json_filename	- the file being parsed (path to)
 *	fnamchk		- path to fnamchk util
 *
 * returns:
 *	>0 ==> the number of issues found
 *	0 ==> if no issues were found
 *
 * NOTE: Does not return on error (NULL pointers).
 */
int
check_found_common_json_fields(char const *program, char const *json_filename, char const *fnamchk, bool test)
{
    int year = 0;	/* ioccc_year: IOCCC year as an integer */
    int entry_num = -1;	/* entry_num: entry number as an integer */
    long ts = 0;	/* formed_timestamp_usec: microseconds as an integer */
    struct tm tm;	/* formed_timestamp: formatted as a time structure */
    int exit_code = 0;	/* tarball: exit code from fnamchk command */
    int issues = 0;	/* number of issues found */
    char *p;
    struct json_field *field; /* current field in found_common_json_fields list */
    struct json_value *value; /* current value in current field's values list */
    struct json_field *common_field = NULL; /* element in the common_json_fields table */
    struct json_field *tarball_field = NULL; /* for checks after initial loop */
    char *tarball_val = NULL; /* for checks after initial loop */
    char *entry_num_val = NULL; /* for checks after initial loop */
    char *contest_id_val = NULL; /* for checks after initial loop */
    char *formed_timestamp_val = NULL; /* for checks after initial loop */
    size_t loc = 0;	/* location in the common_json_fields table */
    size_t val_length = 0; /* current value length */
    bool test_mode = false; /* will be set to true if test_mode value is true */

    /*
     * firewall
     */
    if (program == NULL || json_filename == NULL || fnamchk == NULL) {
	err(178, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /*
     * pre-clear time before strptime() use
     */
    memset(&tm, 0, sizeof tm);

    for (field = found_common_json_fields; field != NULL; field = field->next) {
	/*
	 * process a given common field
	 */

	/*
	 * first make sure the name != NULL and strlen() > 0
	 */
	if (field->name == NULL || !strlen(field->name)) {
	    jerr(JSON_CODE_RESERVED(9), NULL, __func__, __FILE__, NULL, __LINE__,
					"found NULL or empty field in found_common_json_fields list in file %s",
					json_filename);
	    not_reached();
	}

	/* now make sure it's allowed to be in this table. */
	loc = 0;
	common_field = find_json_field_in_table(common_json_fields, field->name, &loc);

	/*
	 * If the field is not allowed in the list then it suggests there is a
	 * problem in the code because only common fields should be added to the
	 * list in the first place. Thus it's an error if a field that's in the
	 * common list is not a common field name.
	 */
	if (common_field == NULL) {
	    jerr(JSON_CODE_RESERVED(10), NULL, __func__, __FILE__, NULL, __LINE__,
					 "illegal field name '%s' in found_common_json_fields list in file %s",
					 field->name, json_filename);
	    not_reached();
	}

	/*
	 * Next we test if the field has been seen more times than allowed. In
	 * the case of the common fields each is only allowed once but in
	 * uncommon fields some are allowed more than once.
	 */
	for (value = field->values; value != NULL; value = value->next) {
	    char *val = value->value;

	    if (val == NULL) {
		jerr(JSON_CODE_RESERVED(11), NULL, __func__, __FILE__, NULL, __LINE__,
					     "NULL pointer val for field '%s' in file %s",
					     field->name, json_filename);
		not_reached();
	    }

	    if (common_field->max_count > 0 && common_field->count > common_field->max_count) {
		jwarn(JSON_CODE(1), program, __func__, __FILE__, NULL, value->line_num,
				    "field '%s' found %ju times but is only allowed %ju time%s in file %s",
				    common_field->name, (uintmax_t)common_field->count,
				    (uintmax_t)common_field->max_count, common_field->max_count==1?"":"s",
				    json_filename);
		++issues;
	    }


	    val_length = strlen(val);

	    if (!val_length) {
		warn(__func__, "empty value found for field '%s' in file %s: '%s'", field->name, json_filename, val);
		/* don't increase issues because the below checks will do that
		 * too: this warning only notes the reason the test will fail.
		 */
	    }
	    /* first we do checks on the field type */
	    switch (common_field->field_type) {
		case JSON_BOOL:
		    if (strcmp(val, "false") && strcmp(val, "true")) {
			warn(__func__, "bool field '%s' has non boolean value in file %s: '%s'",
				       common_field->name, json_filename, val);
			++issues;
			continue;
		    }
		    break;
		case JSON_NUM:
		    if (!is_number(val)) {
			warn(__func__, "number field '%s' has non-number value in file %s: '%s'",
				       common_field->name, json_filename, val);
			++issues;
			continue;
		    }
		    break;
		case JSON_ARRAY_BOOL: /* there aren't any arrays common to both .info.json and .author.json */
		case JSON_ARRAY_NUMBER: /* there aren't any arrays common to both .info.json and .author.json */
		case JSON_ARRAY_CHARS: /* there aren't any arrays common to both .info.json and .author.json */
		default:
		    break;
	    }

	    if (!strcmp(field->name, "ioccc_contest")) {
		if (strcmp(val, IOCCC_CONTEST)) {
		    warn(__func__, "ioccc_contest != \"%s\" in file %s: \"%s\"", IOCCC_CONTEST, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "ioccc_year")) {
		year = string_to_int(val);
		if (year != IOCCC_YEAR) {
		    warn(__func__, "ioccc_year != IOCCC_YEAR %d in file %s: '%s' (%d)", IOCCC_YEAR, json_filename, val, year);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "mkiocccentry_version")) {
		if (!test && strcmp(val, MKIOCCCENTRY_VERSION)) {
		    warn(__func__, "mkiocccentry_version != MKIOCCCENTRY_VERSION \"%s\" in file %s: \"%s\"",
				   MKIOCCCENTRY_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "fnamchk_version")) {
		if (!test && strcmp(val, FNAMCHK_VERSION)) {
		    warn(__func__, "fnamchk_version != FNAMCHK_VERSION \"%s\" in file %s: \"%s\"",
				   FNAMCHK_VERSION, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "IOCCC_contest_id")) {
		/*
		 * We need this for verifying that the tarball matches the contest id, entry num and formed timestamp
		 */
		contest_id_val = contest_id_val != NULL ? contest_id_val : val;
		if (!valid_contest_id(val)) {
		    warn(__func__, "IOCCC_contest_id is invalid in file %s: \"%s\"", json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "min_timestamp")) {
		ts = string_to_long(val);
		if (ts != MIN_TIMESTAMP) {
		    warn(__func__, "min_timestamp != MIN_TIMESTAMP '%ld' in file %s: '%s' (%ld)",
				   MIN_TIMESTAMP, json_filename, val, ts);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "timestamp_epoch")) {
		if (strcmp(val, TIMESTAMP_EPOCH)) {
		    warn(__func__, "timestamp_epoch != TIMESTAMP_EPOCH \"%s\" in file %s: \"%s\"",
				   TIMESTAMP_EPOCH, json_filename, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "formed_timestamp_usec")) {
		errno = 0;
		ts = string_to_long(val);
		if (ts < 0 || ts > 999999) {
		    warnp(__func__, "formed_timestamp_usec out of range of >= 0 && <= 999999 in file %s: '%ld'", json_filename, ts);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "entry_num")) {
		/*
		 * We need this for verifying that the tarball matches the contest id, entry num and formed timestamp
		 */
		entry_num_val = entry_num_val != NULL ? entry_num_val : val;
		entry_num = string_to_int(val);
		if (!(entry_num >= 0 && entry_num <= MAX_ENTRY_NUM)) {
		    warn(__func__, "entry number out of range in file %s: %d", json_filename, entry_num);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "formed_UTC")) {
		p = strptime(val, FORMED_UTC_FMT, &tm);
		if (p == NULL) {
		    warn(__func__, "formed_UTC does not match FORMED_UTC_FMT \"%s\": \"%s\"", FORMED_UTC_FMT, val);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "formed_timestamp")) {
		/*
		 * We need this for verifying that the tarball matches the contest id, entry num and formed timestamp
		 */
		formed_timestamp_val = formed_timestamp_val != NULL ? formed_timestamp_val : val;
		ts = string_to_long(val);
		if (ts < MIN_TIMESTAMP) {
		    warn(__func__, "formed_timestamp < MIN_TIMESTAMP '%ld' in file %s: '%s' (%ld)",
				   MIN_TIMESTAMP, json_filename, val, ts);
		    ++issues;
		}
	    } else if (!strcmp(field->name, "tarball")) {
		/*
		 * We have to do special tests on this after we find if
		 * test_mode == true
		 */
		tarball_field = field;
		/*
		 * We need this for verifying that the tarball matches the contest id, entry num and formed timestamp
		 */
		tarball_val = tarball_val != NULL ? tarball_val: val;
	    } else if (!strcmp(field->name, "test_mode")) {
		/*
		 * Only set to the value if !test_mode: this is so that if
		 * there's more than one of "test_mode" in the file it will not
		 * set back to false if already seen as true (we start out as
		 * false so this isn't a problem).
		 *
		 * Note that if this field is seen more than once in the file
		 * that still will be reported after the loop.
		 */
		if (!test_mode) {
		    test_mode = string_to_bool(val);
		    dbg(DBG_LOW, "set test_mode to %s", test_mode?"true":"false");
		}

	    } else {
		/*
		 * This should never actually be reached but if it is it
		 * suggests that a field was not added to be checked: if it's
		 * not a valid field we would have already detected and aborted
		 * earlier in this loop so we don't have to check for that.
		 */
		warn(__func__, "found unhandled common field in file %s: '%s'", json_filename, field->name);
		/*
		 * NOTE: Don't increment issues because this doesn't mean
		 * there's anything wrong with the json file but rather that the
		 * field isn't verified.
		 */
	    }

	}
    }

    /*
     * Now that we've checked each field by name, we still have to make sure
     * that each field expected is actually there. Note that in the above loop
     * we already tested if each field has been seen more times than allowed so
     * we don't do that here. This is because the fields that are in the list
     * are those that will potentially have more than allowed whereas here we're
     * making sure every field that is required is actually in the list.
     */
    for (loc = 0; common_json_fields[loc].name != NULL; ++loc) {
	if (!common_json_fields[loc].found && common_json_fields[loc].max_count > 0) {
	    warn(__func__, "required field not found in found_common_json_fields list in file %s: '%s'",
			   json_filename, common_json_fields[loc].name);
	    ++issues;
	}
    }

    /* test consistency of test_mode and tarball field */
    if (tarball_field == NULL) {
	warn(__func__, "required field not found in found_common_json_fields list in file %s: 'tarball'", json_filename);
	++issues;
    } else if (test_mode) {
	/*
	 * test_mode was set: execute the fnamchk command with -t.
	 *
	 * Note: Although having the tarball path more than once is considered
	 * an issue we still check each value to be complete.
	 */
	for (value = tarball_field->values; value != NULL; value = value->next) {
	    char const *val = value->value;

	    exit_code = shell_cmd(__func__, true, "% -t % >/dev/null", fnamchk, val);
	    if (exit_code != 0) {
		warn(__func__, "%s: %s %s > /dev/null: failed with exit code: %d",
				    program, fnamchk, val, WEXITSTATUS(exit_code));
		++issues;
	    }
	}
    } else {
	/*
	 * test_mode false: execute the fnamchk command with -u.
	 *
	 * Note: Although having the tarball path more than once is considered
	 * an issue we still check each value to be complete.
	 */
	for (value = tarball_field->values; value != NULL; value = value->next) {
	    char const *val = value->value;

	    exit_code = shell_cmd(__func__, true, "% -u % >/dev/null", fnamchk, val);
	    if (exit_code != 0) {
		warn(__func__, "%s: %s %s > /dev/null: failed with exit code: %d",
				    program, fnamchk, val, WEXITSTATUS(exit_code));
		++issues;
	    }
	}

    }

    /*
     * We have to verify that tarball matches the contest id, entry num and
     * formed timestamp.
     *
     * NOTE: This resolves test_JSON/info.json/bad/info.tarball-0.json as well
     * as some others.
     */
    if (tarball_val != NULL && contest_id_val != NULL && entry_num_val != NULL && formed_timestamp_val != NULL) {
	int ret;
	char *str = NULL;

	errno = 0;
	str = calloc(1, strlen(tarball_val) + strlen(contest_id_val) + strlen(entry_num_val) + strlen(formed_timestamp_val) + 1);
	if (str == NULL) {
	    err(179, __func__, "couldn't allocate memory to verify that contest_id and entry_num matches the tarball");
	    not_reached();
	}

	errno = 0;
	ret = sprintf(str, "entry.%s-%s.%s.txz", contest_id_val, entry_num_val, formed_timestamp_val);
	if (ret <= 0) {
	    warn(__func__, "couldn't write entry.%s-%s.%s.txz to string for tarball match test",
			   contest_id_val, entry_num_val, formed_timestamp_val);
	    ++issues;
	} else {
	    if (strcmp(str, tarball_val)) {
		warn(__func__, "tarball '%s' does not match entry.%s-%s.%s.txz",
			       tarball_val, contest_id_val, entry_num_val, formed_timestamp_val);
		++issues;
	    }
	}
	free(str);
	str = NULL;
    } else {
	warn(__func__, "couldn't verify tarball matches IOCCC_contest_id, entry_num and "
		       "formed_timestamp due to one or more missing fields");
	++issues;
    }

    return issues;
}


/*
 * new_json_field - allocate a new json_field with the value passed in
 *
 * given:
 *
 *	name	    - the field name
 *	val	    - the field's value
 *	line_num    - the 'line number' of the value
 *
 * Returns the newly allocated struct json_field *.
 *
 * NOTE: This function does not return on NULL and it does not check any list:
 * as long as no NULL pointers are encountered it will return a newly allocated
 * struct json_field *. This means that it is the caller's responsibility to
 * check if the field is already in the list.
 */
struct json_field *
new_json_field(char const *json_filename, char const *name, char const *val, int line_num)
{
    struct json_field *field; /* the new field */

    /*
     * firewall
     */
    if (json_filename == NULL || name == NULL || val == NULL) {
	err(180, __func__, "passed NULL arg(s)");
	not_reached();
    }

    errno = 0;
    field = calloc(1, sizeof *field);
    if (field == NULL) {
	errp(181, __func__, "error allocating new struct json_field * for field '%s' and value '%s': %s",
		  name, val, strerror(errno));
	not_reached();
    }

    errno = 0;
    field->name = strdup(name);
    if (field->name == NULL) {
	errp(182, __func__, "unable to strdup() field name '%s': %s", name, strerror(errno));
	not_reached();
    }

    if (add_json_value(json_filename, field, val, line_num) == NULL) {
	err(183, __func__, "error adding value '%s' to field '%s'", val, name);
	not_reached();
    }

    field->count = 1;

    return field;
}


/*
 * add_json_value - add a value to a struct json_field
 *
 * given:
 *
 *	json_filename	- path to the json file being parsed
 *	field		- the field to add to
 *	val		- the value to add
 *	line_num	- the 'line number' of the value
 *
 * This function returns the newly allocated struct json_value * with the value
 * strdup()d and added to the struct json_field * values list.
 *
 * NOTE: If the value of the field is already in the field we still add the
 * value.
 *
 * This function does not return on error.
 *
 */
struct json_value *
add_json_value(char const *json_filename, struct json_field *field, char const *val, int line_num)
{
    struct json_value *new_value = NULL;    /* the newly allocated value */
    struct json_value *value = NULL;	    /* the current list of values in field */

    /*
     * firewall
     */
    if (json_filename == NULL || field == NULL || val == NULL) {
	err(184, __func__, "passed NULL arg(s)");
	not_reached();
    }

    /* allocate new json_value */
    errno = 0;
    new_value = calloc(1, sizeof *new_value);
    if (new_value == NULL) {
	errp(185, __func__, "error allocating new value '%s' for field '%s' in file %s: %s",
		  val, field->name, json_filename, strerror(errno));
	not_reached();
    }
    errno = 0;
    new_value->value = strdup(val);
    if (new_value->value == NULL) {
	errp(186, __func__, "error strdup()ing value '%s' for field '%s': %s", val, field->name, strerror(errno));
	not_reached();
    }

    new_value->line_num = line_num;

    /* find end of list */
    for (value = field->values; value != NULL && value->next != NULL; value = value->next)
	; /* satisfy warnings */

    /* append new value to values list (field->values) */
    if (!value) {
	field->values = new_value;
    } else {
	value->next = new_value;
    }

    return new_value;
}


/*
 * free_json_field_values - free a field's values list
 *
 * given:
 *
 *	field			- field to free the values list
 *
 * Returns void.
 *
 * NOTE: This function does not return on error (NULL field passed in).
 */
void
free_json_field_values(struct json_field *field)
{
    struct json_value *value, *next_value; /* to free the values of the field */

    /*
     * firewall
     */
    if (field == NULL) {
	err(187, __func__, "passed NULL field");
	not_reached();
    }

    /* free each value */
    for (value = field->values; value != NULL; value = next_value) {
	next_value = value->next;

	if (value->value != NULL) {
	    free(value->value);
	    value->value = NULL;
	}
	value = NULL;
    }
}


/*
 * free_found_common_json_fields  - free the common json fields list
 *
 * This function returns void.
 */
void
free_found_common_json_fields(void)
{
    struct json_field *field, *next_field;


    for (field = found_common_json_fields; field != NULL; field = next_field) {
	next_field = field->next;

	if (field->name) {
	    free(field->name);
	    field->name = NULL;
	}
	if (field->values) {
	    free_json_field_values(field);
	    field->values = NULL; /* redundant but for safety anyway */
	}
	free(field);
	field = NULL;
    }

    found_common_json_fields = NULL;
}


/*
 * free_json_field	- release memory in a struct json_field *
 *
 * given:
 *
 *	field	    - the field to free
 *
 * NOTE: This function does not return on NULL.
 *
 * NOTE: It is the caller's responsibility to remove it from the list(s) it is in!
 */
void
free_json_field(struct json_field *field)
{
    /*
     * firewall
     */
    if (field == NULL) {
	err(188, __func__, "passed NULL field");
	not_reached();
    }

    /* free the field's values list */
    free_json_field_values(field);
    field->values = NULL; /* redundant but for safety */

    if (field->name) {
	free(field->name);
	field->name = NULL;
    }

    free(field);
    field = NULL;
}


/*
 * free_info - free info and related sub-elements
 *
 * given:
 *      infop   - pointer to info structure to free
 *
 * This function does not return on error.
 */
void
free_info(struct info *infop)
{
    int i;

    /*
     * firewall
     */
    if (infop == NULL) {
	err(189, __func__, "called with NULL arg(s)");
	not_reached();
    }

    /*
     * free version values
     */
    if (infop->common.mkiocccentry_ver != NULL) {
	free(infop->common.mkiocccentry_ver);
	infop->common.mkiocccentry_ver = NULL;
    }

    /*
     * free entry values
     */
    if (infop->common.ioccc_id != NULL) {
	free(infop->common.ioccc_id);
	infop->common.ioccc_id = NULL;
    }
    if (infop->title != NULL) {
	free(infop->title);
	infop->title = NULL;
    }
    if (infop->abstract != NULL) {
	free(infop->abstract);
	infop->abstract = NULL;
    }

    /*
     * free filenames
     */
    if (infop->prog_c != NULL) {
	free(infop->prog_c);
	infop->prog_c = NULL;
    }
    if (infop->Makefile != NULL) {
	free(infop->Makefile);
	infop->Makefile = NULL;
    }
    if (infop->remarks_md != NULL) {
	free(infop->remarks_md);
	infop->remarks_md = NULL;
    }
    if (infop->extra_file != NULL) {
	for (i = 0; i < infop->extra_count; ++i) {
	    if (infop->extra_file[i] != NULL) {
		free(infop->extra_file[i]);
		infop->extra_file[i] = NULL;
	    }
	}
	free(infop->extra_file);
	infop->extra_file = NULL;
    }

    if (infop->common.tarball != NULL) {
	free(infop->common.tarball);
	infop->common.tarball = NULL;
    }

    /*
     * free time values
     */
    if (infop->common.epoch != NULL) {
	free(infop->common.epoch);
	infop->common.epoch = NULL;
    }
    if (infop->common.utctime != NULL) {
	free(infop->common.utctime);
	infop->common.utctime = NULL;
    }
    memset(infop, 0, sizeof *infop);

    return;
}


/*
 * free_author_array - free storage related to a struct author *
 *
 * given:
 *      author_set      - pointer to a struct author array
 *      author_count    - length of author array
 */
void
free_author_array(struct author *author_set, int author_count)
{
    int i;

    /*
     * firewall
     */
    if (author_set == NULL) {
	err(190, __func__, "called with NULL arg(s)");
	not_reached();
    }
    if (author_count < 0) {
	err(191, __func__, "author_count: %d < 0", author_count);
	not_reached();
    }

    /*
     * free elements of each array member
     */
    for (i = 0; i < author_count; ++i) {
	if (author_set[i].name != NULL) {
	    free(author_set[i].name);
	    author_set[i].name = NULL;
	}
	if (author_set[i].location_code != NULL) {
	    free(author_set[i].location_code);
	    author_set[i].location_code = NULL;
	}
	if (author_set[i].email != NULL) {
	    free(author_set[i].email);
	    author_set[i].email = NULL;
	}
	if (author_set[i].url != NULL) {
	    free(author_set[i].url);
	    author_set[i].url = NULL;
	}
	if (author_set[i].twitter != NULL) {
	    free(author_set[i].twitter);
	    author_set[i].twitter = NULL;
	}
	if (author_set[i].github != NULL) {
	    free(author_set[i].github);
	    author_set[i].github = NULL;
	}
	if (author_set[i].affiliation != NULL) {
	    free(author_set[i].affiliation);
	    author_set[i].affiliation = NULL;
	}
	if (author_set[i].author_handle != NULL) {
	    free(author_set[i].author_handle);
	    author_set[i].author_handle = NULL;
	}
    }

    memset(author_set, 0, sizeof *author_set);
    return;
}


/*
 * alloc_json_code_ignore_set - allocate the initial JSON ignore code set
 *
 * This function will setup the ignore_json_code_set table.  If the ignore_json_code_set
 * table is already setup, this function will do nothing.
 *
 * NOTE: This function does not return on error.
 */
static void
alloc_json_code_ignore_set(void)
{
    struct ignore_json_code *tbl = NULL;	/* allocated struct ignore_json_code */
    int i;

    /*
     * firewall - do nothing if already setup
     */
    if (ignore_json_code_set != NULL) {
	dbg(DBG_VVHIGH, "ignore_json_code_set already set up");
	return;
    }

    /*
     * allocate the initial ignore_json_code_set[]
     */
    errno = 0;			/* pre-clear errno for errp() */
    tbl = malloc(sizeof(struct ignore_json_code));
    if (tbl == NULL) {
	errp(192, __func__, "failed to malloc struct ignore_json_code");
	not_reached();
    }

    /*
     * allocate an initial block of ignore codes
     */
    errno = 0;			/* pre-clear errno for errp() */
    tbl->code = malloc((JSON_CODE_IGNORE_CHUNK+1+1) * sizeof(int));
    if (tbl->code == NULL) {
	errp(193, __func__, "cannot allocate %d ignore codes", JSON_CODE_IGNORE_CHUNK+1+1);
	not_reached();
    }

    /*
     * initialize
     */
    tbl->next_free = 0;
    tbl->alloc = JSON_CODE_IGNORE_CHUNK + 1;		/* report one less for the guard code */
    for (i=0; i < tbl->alloc; ++i) {
	tbl->code[i] = -1;	/* -1 is not a valid ignore code */
    }
    ignore_json_code_set = tbl;
}


/*
 * cmp_codes - compare two codes for reverse sorting ignore_json_code_set[]
 *
 * This function will allow qsort() to reverse sort the codes in ignore_json_code_set[].
 *
 * given:
 *	a	- pointer to 1st code to compare
 *	b	- pointer to 2nd code to compare
 *
 * returns:
 *	-1	a > b
 *	0	a == b
 *	1	a < b
 */
static int
cmp_codes(const void *a, const void *b)
{
    /*
     * firewall
     */
    if (a == NULL || b == NULL) {
	err(194, __func__, "NULL arg(s)");
	not_reached();
    }

    /*
     * compare for reverse sort
     */
    if (*(int *)a < *(int *)b) {
	return 1;	/* a < b */
    } else if (*(int *)a > *(int *)b) {
	return -1;	/* a > b */
    }
    return 0;	/* a == b */
}


/*
 * expand_json_code_ignore_set - expand the size of alloc json_code_ignore_set[]
 *
 * This function will expand the allocated  json_code_ignore_set[] by
 * JSON_CODE_IGNORE_CHUNK JSON error codes, and set those new codes (in the end
 * of the table) to -1 to indicate that they are not valid codes.
 *
 * NOTE: This function does not return on error.
 */
static void
expand_json_code_ignore_set(void)
{
    int *p;	/* reallocated code set */
    int i;

    /*
     * setup ignore_json_code_set if needed
     */
    alloc_json_code_ignore_set();
    if (ignore_json_code_set == NULL) {
	err(195, __func__, "ignore_json_code_set is NULL after allocation");
	not_reached();
    }

    /*
     * if no room, expand the table
     */
    if (ignore_json_code_set->next_free >= ignore_json_code_set->alloc) {
	p = realloc(ignore_json_code_set->code, (ignore_json_code_set->alloc+JSON_CODE_IGNORE_CHUNK+1) * sizeof(int));
	errno = 0;			/* pre-clear errno for errp() */
	if (p == NULL) {
	    errp(196, __func__, "cannot expand ignore_json_code_set from %d to %d codes",
				ignore_json_code_set->alloc+1, ignore_json_code_set->alloc+JSON_CODE_IGNORE_CHUNK+1);
	    not_reached();
	}
	for (i=ignore_json_code_set->alloc; i < ignore_json_code_set->alloc+JSON_CODE_IGNORE_CHUNK; ++i) {
	    p[i] = JSON_CODE_INVALID; /* JSON_CODE_INVALID (JSON_CODE_RESERVED_MIN - 1) is not a valid ignore code */
	}
	ignore_json_code_set->code = p;
	/* report one less for the guard code */
	ignore_json_code_set->alloc = ignore_json_code_set->alloc + JSON_CODE_IGNORE_CHUNK;
    }
    return;
}


/*
 * is_json_code_ignored - determine of a JSON warn/error code is to be ignored
 *
 * given:
 *	code	- code to test if code should be ignored
 *
 * returns:
 *	true ==> code is in ignore_json_code_set[] and should be ignored
 *	false ==> code is not in ignore_json_code_set[] and should NOT be ignored
 */
bool
is_json_code_ignored(int code)
{
    int i;

    /*
     * firewall
     */
    if (code < 0) {
	err(197, __func__, "code %d < 0", code);
	not_reached();
    }

    /*
     * setup ignore_json_code_set if needed
     */
    alloc_json_code_ignore_set();
    if (ignore_json_code_set == NULL) {
	err(198, __func__, "ignore_json_code_set is NULL after allocation");
	not_reached();
    }

    /*
     * search ignore_json_code_set[] for the code
     */
    for (i=0; i < ignore_json_code_set->next_free; ++i) {

	/* look for match */
	if (ignore_json_code_set->code[i] == code) {
	    dbg(DBG_VVVHIGH, "code %d is in ignore_json_code_set[]", code);
	    return true;	/* report code should be ignored */
	}

	/* look for going beyond sorted values */
	if (ignore_json_code_set->code[i] <= code) {
	     break;
	}
    }
    return false;	/* report code should NOT be ignored */
}


/*
 * add_ignore_json_code - add a JSON error code to be ignored
 *
 * If code >= 0 and is not in json_code_ignore_set[], then this function will
 * add it and then reverse sort the json_code_ignore_set[] table.
 *
 * NOTE: The json_code_ignore_set[] will be initialized or expanded as needed.
 *
 * given:
 *	code	- code to ignore
 *
 * NOTE: This function does not return on error.
 */
void
ignore_json_code(int code)
{
    /*
     * firewall
     */
    if (code < 0) {
	err(199, __func__, "code %d < 0", code);
	not_reached();
    }

    /*
     * allocate or expand the json_code_ignore_set[] if needed
     */
    if (ignore_json_code_set == NULL || ignore_json_code_set->next_free >= ignore_json_code_set->alloc) {
	expand_json_code_ignore_set();
    }
    if (ignore_json_code_set == NULL) {
	err(200, __func__, "ignore_json_code_set is NULL after allocation or expansion");
	not_reached();
    }

    /*
     * verify that code is not already in json_code_ignore_set[]
     */
    if (is_json_code_ignored(code)) {
	/* nothing to do it code is already added */
	dbg(DBG_VVVHIGH, "code %d is already in ignore_json_code_set[]", code);
	return;
    }

    /*
     * add the code to the json_code_ignore_set[]
     */
    ignore_json_code_set->code[ignore_json_code_set->next_free] = code;
    ++ignore_json_code_set->next_free;

    /*
     * reverse sort the json_code_ignore_set[]
     */
    qsort(ignore_json_code_set->code, ignore_json_code_set->next_free, sizeof(int), cmp_codes);
    dbg(DBG_VHIGH, "code %d added to ignore_json_code_set[]", code);
    return;
}


/*
 * malloc_json_conv_int - convert JSON integer string to C integer value
 *
 * given:
 *	str	a JSON integer string
 *	len	length starting at str of the JSON string
 *
 * returns:
 *	malloced struct json_integer with C integer values based on JSON string
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json_integer *
malloc_json_conv_int(char const *str, size_t len)
{
    struct json_integer *ret = NULL;	    /* malloced decoding string or NULL */
    char *endptr;		    /* first invalid character or str */
    size_t digits = 0;		    /* number of digits in JSON integer not including leading sign */
    size_t i;

    /*
     * malloc the return integer
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = malloc(sizeof(struct json_integer));
    if (ret == NULL) {
	errp(201, __func__, "malloc #0 error allocating %ju bytes", (uintmax_t)sizeof(struct json_integer));
	not_reached();
    }

    /*
     * initialize the return integer
     */
    memset(ret, 0, sizeof(struct json_integer));
    ret->as_str = NULL;
    ret->converted = false;
    ret->is_negative = false;
    ret->int8_sized = false;
    ret->uint8_sized = false;
    ret->int16_sized = false;
    ret->uint16_sized = false;
    ret->int32_sized = false;
    ret->uint32_sized = false;
    ret->int64_sized = false;
    ret->uint64_sized = false;
    ret->int_sized = false;
    ret->uint_sized = false;
    ret->long_sized = false;
    ret->ulong_sized = false;
    ret->longlong_sized = false;
    ret->ulonglong_sized = false;
    ret->ssize_sized = false;
    ret->size_sized = false;
    ret->off_sized = false;
    ret->maxint_sized = false;
    ret->umaxint_sized = false;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    ret->orig_len = len;	/* save original length */
    if (str[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON integer string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret->as_str = malloc(len+1+1);
    if (ret->as_str == NULL) {
	errp(202, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    strncpy(ret->as_str, str, len+1);
    ret->as_str[len] = '\0';	/* paranoia */
    ret->as_str[len+1] = '\0';	/* paranoia */

    /*
     * paranoia
     *
     * While the common use of this function is via bison/flex produced C code,
     * we want to keep the general case working where this function might
     * someday be called from some other code.  For such a future case we
     * want to trim off leading and trailing whitespace so that the code below
     * checking for < 0 and the code checking the conversion into uintmax_t
     * or into intmax_t is not confused.
     */
    /* skip over any leading space */
    for (i=0; i < len; ++i) {
	if (!isascii(ret->as_str[i]) || !isspace(ret->as_str[i])) {
	    break;
	}
    }
    /* trim any leading space */
    len -= i;
    ret->as_str += i;
    if (len <= 0) {
	warn(__func__, "called with string containing only whitespace");
	return ret;
    }
    /* trim off any trailing whitespace */
    while (len > 0 && isascii(ret->as_str[len-1]) && isspace(ret->as_str[len-1])) {
	/* trim trailing whitespace */
	ret->as_str[len-1] = '\0';
	--len;
    }
    if (len <= 0) {
	warn(__func__, "called with string with all whitespace");
	return ret;
    }
    ret->as_str_len = len;	/* save length of as_str */

    /*
     * determine if JSON integer negative
     */
    if (ret->as_str[0] == '-') {

	/* parse JSON integer that is < 0 */
	ret->is_negative = true;

	/*
	 * paranoia
	 *
	 * The only characters beyond the - remaining should be only digits.
	 * This shouldn't happen via the bison / flex code that has an integer
	 * regexp, but we check anyway as a matter of defense in depth.
	 */
	digits = strspn(ret->as_str+1, "0123456789");
	if (digits != len-1) {
	    warn(__func__, "called with string with - followed by non-digits: <%s>", ret->as_str);
	    return ret;
	}

    /* case: JSON integer is >= 0 */
    } else {

	/* parse JSON integer that is >= 0 */
	ret->is_negative = false;

	/*
	 * paranoia
	 *
	 * Integers with leading + are not allowed in JSON due to fundamental design
	 * flaws by the JSON designers.  We check for and skip a leading + just in
	 * case this code is called by some other code.
	 */
	if (ret->as_str[0] == '+') {
	    /* skip leading + */
	    ret->as_str++;
	    len--;
	    if (len <= 0) {
		warn(__func__, "called with string with only a +");
		return ret;
	    }
	}

	/*
	 * paranoia
	 *
	 * The only characters beyond the remaining should be only digits.
	 * This shouldn't happen via the bison / flex code, but we check anyway
	 * as a matter of defense in depth.
	 */
	digits = strspn(ret->as_str, "0123456789");
	if (digits != len) {
	    warn(__func__, "called with string containing non-digits: <%s>", ret->as_str);
	    return ret;
	}
    }

    /*
     * attempt to convert to the largest possible integer
     */
    if (ret->is_negative) {

	/* case: negative, try for largest signed integer */
	errno = 0;			/* pre-clear errno for errp() */
	ret->as_maxint = strtoimax(ret->as_str, &endptr, 10);
	if (errno != 0 || endptr == ret->as_str || endptr == NULL || *endptr != '\0') {
	    dbg(DBG_VVHIGH, "strtoimax failed to convert");
	    ret->converted = false;
	    return ret;
	}
	ret->converted = true;
	ret->maxint_sized = true;
	dbg(DBG_VVHIGH, "strtoimax for <%s> returned: %jd", ret->as_str, ret->as_maxint);

	/* case int8_t: range check */
	if (ret->as_maxint >= (intmax_t)INT8_MIN && ret->as_maxint <= (intmax_t)INT8_MAX) {
	    ret->int8_sized = true;
	    ret->as_int8 = (int8_t)ret->as_maxint;
	}

	/* case uint8_t: cannot be because JSON string is < 0 */
	ret->uint8_sized = false;

	/* case int16_t: range check */
	if (ret->as_maxint >= (intmax_t)INT16_MIN && ret->as_maxint <= (intmax_t)INT16_MAX) {
	    ret->int16_sized = true;
	    ret->as_int16 = (int16_t)ret->as_maxint;
	}

	/* case uint16_t: cannot be because JSON string is < 0 */
	ret->uint16_sized = false;

	/* case int32_t: range check */
	if (ret->as_maxint >= (intmax_t)INT32_MIN && ret->as_maxint <= (intmax_t)INT32_MAX) {
	    ret->int32_sized = true;
	    ret->as_int32 = (int32_t)ret->as_maxint;
	}

	/* case uint32_t: cannot be because JSON string is < 0 */
	ret->uint32_sized = false;

	/* case int64_t: range check */
	if (ret->as_maxint >= (intmax_t)INT64_MIN && ret->as_maxint <= (intmax_t)INT64_MAX) {
	    ret->int64_sized = true;
	    ret->as_int64 = (int64_t)ret->as_maxint;
	}

	/* case uint64_t: cannot be because JSON string is < 0 */
	ret->uint64_sized = false;

	/* case int: range check */
	if (ret->as_maxint >= (intmax_t)INT_MIN && ret->as_maxint <= (intmax_t)INT_MAX) {
	    ret->int_sized = true;
	    ret->as_int = (int)ret->as_maxint;
	}

	/* case unsigned int: cannot be because JSON string is < 0 */
	ret->uint_sized = false;

	/* case long: range check */
	if (ret->as_maxint >= (intmax_t)LONG_MIN && ret->as_maxint <= (intmax_t)LONG_MAX) {
	    ret->long_sized = true;
	    ret->as_long = (long)ret->as_maxint;
	}

	/* case unsigned long: cannot be because JSON string is < 0 */
	ret->ulong_sized = false;

	/* case long long: range check */
	if (ret->as_maxint >= (intmax_t)LLONG_MIN && ret->as_maxint <= (intmax_t)LLONG_MAX) {
	    ret->longlong_sized = true;
	    ret->as_longlong = (long long)ret->as_maxint;
	}

	/* case unsigned long long: cannot be because JSON string is < 0 */
	ret->ulonglong_sized = false;

	/* case size_t: cannot be because JSON string is < 0 */
	ret->size_sized = false;

	/* case ssize_t: range check */
	if (ret->as_maxint >= (intmax_t)SSIZE_MIN && ret->as_maxint <= (intmax_t)SSIZE_MAX) {
	    ret->ssize_sized = true;
	    ret->as_ssize = (ssize_t)ret->as_maxint;
	}

	/* case off_t: range check */
	if (ret->as_maxint >= (intmax_t)OFF_MIN && ret->as_maxint <= (intmax_t)OFF_MAX) {
	    ret->off_sized = true;
	    ret->as_off = (off_t)ret->as_maxint;
	}

	/* case intmax_t: was handled by the above call to strtoimax() */

	/* case uintmax_t: cannot be because JSON string is < 0 */
	ret->umaxint_sized = false;

    } else {

	/* case: non-negative, try for largest unsigned integer */
	errno = 0;			/* pre-clear errno for errp() */
	ret->as_umaxint = strtoumax(ret->as_str, &endptr, 10);
	if (errno != 0 || endptr == ret->as_str || endptr == NULL || *endptr != '\0') {
	    dbg(DBG_VVHIGH, "strtoumax failed to convert");
	    ret->converted = false;
	    return ret;
	}
	ret->converted = true;
	ret->umaxint_sized = true;
	dbg(DBG_VVHIGH, "strtoumax for <%s> returned: %ju", ret->as_str, ret->as_umaxint);

	/* case int8_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)INT8_MAX) {
	    ret->int8_sized = true;
	    ret->as_int8 = (int8_t)ret->as_umaxint;
	}

	/* case uint8_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)UINT8_MAX) {
	    ret->uint8_sized = true;
	    ret->as_uint8 = (uint8_t)ret->as_umaxint;
	}

	/* case int16_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)INT16_MAX) {
	    ret->int16_sized = true;
	    ret->as_int16 = (int16_t)ret->as_umaxint;
	}

	/* case uint16_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)UINT16_MAX) {
	    ret->uint16_sized = true;
	    ret->as_uint16 = (uint16_t)ret->as_umaxint;
	}

	/* case int32_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)INT32_MAX) {
	    ret->int32_sized = true;
	    ret->as_int32 = (int32_t)ret->as_umaxint;
	}

	/* case uint32_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)UINT32_MAX) {
	    ret->uint32_sized = true;
	    ret->as_uint32 = (uint32_t)ret->as_umaxint;
	}

	/* case int64_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)INT64_MAX) {
	    ret->int64_sized = true;
	    ret->as_int64 = (int64_t)ret->as_umaxint;
	}

	/* case uint64_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)UINT64_MAX) {
	    ret->uint64_sized = true;
	    ret->as_uint64 = (uint64_t)ret->as_umaxint;
	}

	/* case int: bounds check */
	if (ret->as_umaxint <= (uintmax_t)INT_MAX) {
	    ret->int_sized = true;
	    ret->as_int = (int)ret->as_umaxint;
	}

	/* case unsigned int: bounds check */
	if (ret->as_umaxint <= (uintmax_t)UINT_MAX) {
	    ret->uint_sized = true;
	    ret->as_uint = (unsigned int)ret->as_umaxint;
	}

	/* case long: bounds check */
	if (ret->as_umaxint <= (uintmax_t)LONG_MAX) {
	    ret->long_sized = true;
	    ret->as_long = (long)ret->as_umaxint;
	}

	/* case unsigned long: bounds check */
	if (ret->as_umaxint <= (uintmax_t)ULONG_MAX) {
	    ret->ulong_sized = true;
	    ret->as_ulong = (unsigned long)ret->as_umaxint;
	}

	/* case long long: bounds check */
	if (ret->as_umaxint <= (uintmax_t)LLONG_MAX) {
	    ret->longlong_sized = true;
	    ret->as_longlong = (long long)ret->as_umaxint;
	}

	/* case unsigned long long: bounds check */
	if (ret->as_umaxint <= (uintmax_t)ULLONG_MAX) {
	    ret->ulonglong_sized = true;
	    ret->as_ulonglong = (unsigned long long)ret->as_umaxint;
	}

	/* case ssize_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)SSIZE_MAX) {
	    ret->ssize_sized = true;
	    ret->as_ssize = (ssize_t)ret->as_umaxint;
	}

	/* case size_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)SIZE_MAX) {
	    ret->size_sized = true;
	    ret->as_size = (size_t)ret->as_umaxint;
	}

	/* case off_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)OFF_MAX) {
	    ret->off_sized = true;
	    ret->as_off = (off_t)ret->as_umaxint;
	}

	/* case intmax_t: bounds check */
	if (ret->as_umaxint <= (uintmax_t)INTMAX_MAX) {
	    ret->maxint_sized = true;
	    ret->as_maxint = (intmax_t)ret->as_umaxint;
	}

	/* case uintmax_t: was handled by the above call to strtoumax() */
    }

    /*
     * return converted integer
     */
    return ret;
}


/*
 * malloc_json_conv_int_str - convert JSON integer string to C integer value
 *
 * This is an simplified interface for malloc_json_conv_int().
 *
 * given:
 *	str	a JSON integer string to convert
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	malloced struct json_integer with C integer values based on JSON string
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json_integer *
malloc_json_conv_int_str(char const *str, size_t *retlen)
{
    struct json_integer *ret = NULL;	    /* malloced encoding string or NULL */
    size_t len = 0;		    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the malloc_json_conv_int() handle the arg firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
    }

    /*
     * convert to malloc_json_conv_int() call
     */
    ret = malloc_json_conv_int(str, len);
    if (ret == NULL) {
	err(203, __func__, "malloc_json_conv_int() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return encoded result or NULL
     */
    return ret;
}


/*
 * malloc_json_conv_float - convert JSON integer string to C floating point value
 *
 * given:
 *	str	a JSON integer floating point string
 *	len	length starting at str of the JSON string
 *
 * returns:
 *	malloced struct json_floating with C floating point values based on JSON string
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json_floating *
malloc_json_conv_float(char const *str, size_t len)
{
    struct json_floating *ret = NULL;	    /* malloced decoding string or NULL */
    char *endptr;		    /* first invalid character or str */
    size_t i;

    /*
     * malloc the return integer
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret = malloc(sizeof(struct json_floating));
    if (ret == NULL) {
	errp(204, __func__, "malloc #0 error allocating %ju bytes", (uintmax_t)sizeof(struct json_floating));
	not_reached();
    }

    /*
     * initialize the return integer
     */
    memset(ret, 0, sizeof(struct json_integer));
    ret->as_str = NULL;
    ret->converted = false;
    ret->is_negative = false;
    ret->float_sized = false;
    ret->as_float = 0.0;
    ret->double_sized = false;
    ret->as_double = 0.0;
    ret->longdouble_sized = false;
    ret->as_longdouble = 0.0;

    /*
     * firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
	return ret;
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
	return ret;
    }
    if (str[0] == '\0') {
	warn(__func__, "called with empty string");
	return ret;
    }

    /*
     * duplicate the JSON floating point string
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret->as_str = malloc(len+1+1);
    if (ret->as_str == NULL) {
	errp(205, __func__, "malloc #1 error allocating %ju bytes", (uintmax_t)(len+1+1));
	not_reached();
    }
    strncpy(ret->as_str, str, len+1);
    ret->as_str[len] = '\0';	/* paranoia */
    ret->as_str[len+1] = '\0';	/* paranoia */

    /*
     * paranoia
     *
     * While the common use of this function is via bison/flex produced C code,
     * we want to keep the general case working where this function might
     * someday be called from some other code.
     */
    /* skip over any leading space */
    for (i=0; i < len; ++i) {
	if (!isascii(ret->as_str[i]) || !isspace(ret->as_str[i])) {
	    break;
	}
    }
    /* trim any leading space */
    len -= i;
    ret->as_str += i;
    if (len <= 0) {
	warn(__func__, "called with string containing only whitespace");
	return ret;
    }
    /* trim off any trailing whitespace */
    while (len > 0 && isascii(ret->as_str[len-1]) && isspace(ret->as_str[len-1])) {
	/* trim trailing whitespace */
	ret->as_str[len-1] = '\0';
	--len;
    }
    if (len <= 0) {
	warn(__func__, "called with string with all whitespace");
	return ret;
    }

    /*
     * convert to largest floating point value
     */
    errno = 0;			/* pre-clear errno for errp() */
    ret->as_longdouble = strtold(ret->as_str, &endptr);
    if (errno != 0 || endptr == ret->as_str || endptr == NULL || *endptr != '\0') {
	dbg(DBG_VVHIGH, "strtold failed to convert");
	ret->converted = false;
	return ret;
    }
    ret->converted = true;
    ret->longdouble_sized = true;
    dbg(DBG_VVHIGH, "strtold for <%s> returned as %%Lg: %.22Lg", ret->as_str, ret->as_longdouble);
    dbg(DBG_VVHIGH, "strtold for <%s> returned as %%Le: %.22Le", ret->as_str, ret->as_longdouble);
    dbg(DBG_VVHIGH, "strtold for <%s> returned as %%Lf: %.22Lf", ret->as_str, ret->as_longdouble);

    /*
     * note if value < 0
     */
    if (ret->as_longdouble < 0) {
	ret->is_negative = true;
    }

    /*
     * convert to double
     */
    errno = 0;			/* pre-clear conversion test */
    ret->as_double = strtod(ret->as_str, &endptr);
    if (errno != 0 || endptr == ret->as_str || endptr == NULL || *endptr != '\0') {
	ret->double_sized = false;
	dbg(DBG_VVHIGH, "strtod for <%s> failed", ret->as_str);
    } else {
	ret->double_sized = true;
	dbg(DBG_VVHIGH, "strtod for <%s> returned as %%lg: %.22lg", ret->as_str, ret->as_double);
	dbg(DBG_VVHIGH, "strtod for <%s> returned as %%le: %.22le", ret->as_str, ret->as_double);
	dbg(DBG_VVHIGH, "strtod for <%s> returned as %%lf: %.22lf", ret->as_str, ret->as_double);
    }

    /*
     * convert to float
     */
    errno = 0;			/* pre-clear conversion test */
    ret->as_float = strtof(ret->as_str, &endptr);
    if (errno != 0 || endptr == ret->as_str || endptr == NULL || *endptr != '\0') {
	ret->float_sized = false;
	dbg(DBG_VVHIGH, "strtof for <%s> failed", ret->as_str);
    } else {
	ret->float_sized = true;
	dbg(DBG_VVHIGH, "strtof for <%s> returned as %%g: %.22g", ret->as_str, ret->as_float);
	dbg(DBG_VVHIGH, "strtof for <%s> returned as %%e: %.22e", ret->as_str, ret->as_float);
	dbg(DBG_VVHIGH, "strtof for <%s> returned as %%f: %.22f", ret->as_str, ret->as_float);
    }

    /*
     * return converted floating point value
     */
    return ret;
}


/*
 * malloc_json_conv_float_str - convert JSON floating point string to C integer value
 *
 * This is an simplified interface for malloc_json_conv_float().
 *
 * given:
 *	str	a JSON floating point string to convert
 *	retlen	address of where to store length of str, if retlen != NULL
 *
 * returns:
 *	malloced struct json_floating with C floating point values based on JSON string
 *	NOTE: retlen, if non-NULL, is set to 0 on error
 *
 * NOTE: This function will not return on malloc error.
 * NOTE: This function will not return NULL.
 */
struct json_floating *
malloc_json_conv_float_str(char const *str, size_t *retlen)
{
    struct json_floating *ret = NULL;	    /* malloced encoding string or NULL */
    size_t len = 0;		    /* length of string to encode */

    /*
     * firewall
     *
     * NOTE: We will let the malloc_json_conv_float() handle the arg firewall
     */
    if (str == NULL) {
	warn(__func__, "called with NULL str");
    } else {
	len = strlen(str);
    }
    if (len <= 0) {
	warn(__func__, "called with len: %ju <= 0", (uintmax_t)len);
    }

    /*
     * convert to malloc_json_conv_float() call
     */
    ret = malloc_json_conv_float(str, len);
    if (ret == NULL) {
	err(206, __func__, "malloc_json_conv_float() returned NULL");
	not_reached();
    }

    /*
     * save length if allowed
     */
    if (retlen != NULL) {
	*retlen = len;
    }

    /*
     * return encoded result or NULL
     */
    return ret;
}
