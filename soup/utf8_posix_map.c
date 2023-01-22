/*
 * utf8_posix_map - translate UTF-8 into POSIX portable filename and + chars
 *
 * "Because even POSIX needs an extra plus." :-)
 *
 * An author_handle, for an IOCCC winner, will be used to form
 * a winner_handle.  These winner handles will become part of a
 * JSON filename on the www.ioccc.org website.  For this reason,
 * (and other reasons), we must restrict an author_handle to
 * only lower case POSIX portable filenames, with the addition of the +,
 * and restrictions on leading characters.
 *
 * The author_handle (and winner_handle) must fit the following
 * regular expression:
 *
 *	^[0-9a-z][0-9a-z._+-]*$
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
/* exit code out of numerical order - ignore in sequencing - ooo */
/* exit code change of order - use new value in sequencing - coo */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h> /* strncasecmp */
#include <sys/time.h>
#include <unistd.h>


/*
 * utf8_posix_map - translate UTF-8 into POSIX portable filename and + chars
 */
#include "utf8_posix_map.h"


/*
 * How to translate certain UTF-8 strings into safe lower case
 * POSIX portable filenames plus +.  This table helps us convert
 * certain UTF-8 strings into strings that match this regular expression:
 *
 *	^[0-9a-z][0-9a-z._+-]*$
 *
 * This table is NOT linguistic.  This table is NOT intended to be
 * an accurate translation of alphabets.  This table maps some UTF-8 byte
 * strings into characters that look somewhat like lower case POSIX portable
 * filenames plus +.
 *
 * We use this table to convert author handles into some ASCII that
 * can be used as a POSIX portable filenames plus +, in lower case.
 *
 * With that in mind, feel free to suggest modifications or additions,
 * in the form of a GitHub pull request, to the table below.
 *
 * NOTE: UTF-8 strings that are NOT found in this table are to be ignored.
 *	 In some cases the byte is translated into an empty string,
 *	 which as the same effect if the entry were not in this table.
 *
 * This table ends in a NULL entry.
 */
struct utf8_posix_map hmap[] =
{
    /* \x00 - special case */
    {"\x00", "",  0,  0},		/* NUL */

    /* \x01 -\x0f */
    {"\x01", "", -1, -1},		/* SOH */
    {"\x02", "", -1, -1},		/* STX */
    {"\x03", "", -1, -1},		/* ETX */
    {"\x04", "", -1, -1},		/* EOT */
    {"\x05", "", -1, -1},		/* EOT */
    {"\x06", "", -1, -1},		/* ENQ */
    {"\x07", "", -1, -1},		/* BEL (\a) */
    {"\x08", "", -1, -1},		/* BS (\b) */
    {"\x09", "_", -1, -1},		/* HT (\t) */
    {"\x0a", "_", -1, -1},		/* LF (\n) */
    {"\x0b", "_", -1, -1},		/* VT (\v) */
    {"\x0c", "_", -1, -1},		/* FF (\f) */
    {"\x0d", "_", -1, -1},		/* CR (\r) */
    {"\x0e", "", -1, -1},		/* SO */
    {"\x0f", "", -1, -1},		/* SI */

    /* \x10 -\x1f */
    {"\x10", "", -1, -1},		/* DLE */
    {"\x11", "", -1, -1},		/* DC1 */
    {"\x12", "", -1, -1},		/* DC2 */
    {"\x13", "", -1, -1},		/* DC3 */
    {"\x14", "", -1, -1},		/* DC4 */
    {"\x15", "", -1, -1},		/* NAK */
    {"\x16", "", -1, -1},		/* SYN */
    {"\x17", "", -1, -1},		/* ETB */
    {"\x18", "", -1, -1},		/* CAN */
    {"\x19", "", -1, -1},		/* EM */
    {"\x1a", "", -1, -1},		/* SUB */
    {"\x1b", "", -1, -1},		/* ESC */
    {"\x1c", "", -1, -1},		/* FS */
    {"\x1d", "", -1, -1},		/* GS */
    {"\x1e", "", -1, -1},		/* RS */
    {"\x1f", "", -1, -1},		/* US */

    /* \x20 -\x2f */
    {" ", "_", -1, -1},			/* SP */
    {"!", "", -1, -1},			/* ! */
    {"\"", "", -1, -1},			/* " */
    {"#", "", -1, -1},			/* # */
    {"$", "", -1, -1},			/* $ */
    {"%", "", -1, -1},			/* % */
    {"&", "", -1, -1},			/* & */
    {"\'", "", -1, -1},			/* ' */
    {"(", "", -1, -1},			/* ( */
    {")", "", -1, -1},			/* ) */
    {"*", "", -1, -1},			/* * */
    {"+", "+", -1, -1},			/* + - allowed character */
    {",", "", -1, -1},			/* , */
    {"-", "-", -1, -1},			/* - - allowed character */
    {".", ".", -1, -1},			/* . - allowed character */
    {"/", "", -1, -1},			/* / */

    /* \x30 -\x3f */
    {"0", "0", -1, -1},			/* 0 - allowed character */
    {"1", "1", -1, -1},			/* 1 - allowed character */
    {"2", "2", -1, -1},			/* 2 - allowed character */
    {"3", "3", -1, -1},			/* 3 - allowed character */
    {"4", "4", -1, -1},			/* 4 - allowed character */
    {"5", "5", -1, -1},			/* 5 - allowed character */
    {"6", "6", -1, -1},			/* 6 - allowed character */
    {"7", "7", -1, -1},			/* 7 - allowed character */
    {"8", "8", -1, -1},			/* 8 - allowed character */
    {"9", "9", -1, -1},			/* 9 - allowed character */
    {":", "", -1, -1},			/* & */
    {";", "", -1, -1},			/* ; */
    {"<", "", -1, -1},			/* < */
    {"=", "", -1, -1},			/* = */
    {">", "", -1, -1},			/* > */
    {"?", "", -1, -1},			/* ? */

    /* \x40 -\x4f */
    {"@", "", -1, -1},			/* @ */
    {"A", "a", -1, -1},			/* A - converted to lower case character */
    {"B", "b", -1, -1},			/* B - converted to lower case character */
    {"C", "c", -1, -1},			/* C - converted to lower case character */
    {"D", "d", -1, -1},			/* D - converted to lower case character */
    {"E", "e", -1, -1},			/* E - converted to lower case character */
    {"F", "f", -1, -1},			/* F - converted to lower case character */
    {"G", "g", -1, -1},			/* G - converted to lower case character */
    {"H", "h", -1, -1},			/* H - converted to lower case character */
    {"I", "i", -1, -1},			/* I - converted to lower case character */
    {"J", "j", -1, -1},			/* J - converted to lower case character */
    {"K", "k", -1, -1},			/* K - converted to lower case character */
    {"L", "l", -1, -1},			/* L - converted to lower case character */
    {"M", "m", -1, -1},			/* M - converted to lower case character */
    {"N", "n", -1, -1},			/* N - converted to lower case character */
    {"O", "o", -1, -1},			/* O - converted to lower case character */

    /* \x50 -\x5f */
    {"P", "p", -1, -1},			/* P - converted to lower case character */
    {"Q", "q", -1, -1},			/* Q - converted to lower case character */
    {"R", "r", -1, -1},			/* R - converted to lower case character */
    {"S", "s", -1, -1},			/* S - converted to lower case character */
    {"T", "t", -1, -1},			/* T - converted to lower case character */
    {"U", "u", -1, -1},			/* U - converted to lower case character */
    {"V", "v", -1, -1},			/* V - converted to lower case character */
    {"W", "w", -1, -1},			/* W - converted to lower case character */
    {"X", "x", -1, -1},			/* X - converted to lower case character */
    {"Y", "y", -1, -1},			/* Y - converted to lower case character */
    {"Z", "z", -1, -1},			/* Z - converted to lower case character */
    {"[", "", -1, -1},			/* [ */
    {"\\", "", -1, -1},			/* \ */
    {"]", "", -1, -1},			/* ] */
    {"^", "", -1, -1},			/* ^ */
    {"_", "_", -1, -1},			/* _ - allowed character */

    /* \x60 -\x6f */
    {"`", "", -1, -1},			/* ` */
    {"a", "a", -1, -1},			/* a - allowed character */
    {"b", "b", -1, -1},			/* b - allowed character */
    {"c", "c", -1, -1},			/* c - allowed character */
    {"d", "d", -1, -1},			/* d - allowed character */
    {"e", "e", -1, -1},			/* e - allowed character */
    {"f", "f", -1, -1},			/* f - allowed character */
    {"g", "g", -1, -1},			/* g - allowed character */
    {"h", "h", -1, -1},			/* h - allowed character */
    {"i", "i", -1, -1},			/* i - allowed character */
    {"j", "j", -1, -1},			/* j - allowed character */
    {"k", "k", -1, -1},			/* k - allowed character */
    {"l", "l", -1, -1},			/* l - allowed character */
    {"m", "m", -1, -1},			/* m - allowed character */
    {"n", "n", -1, -1},			/* n - allowed character */
    {"o", "o", -1, -1},			/* o - allowed character */

    /* \x70 -\x7f */
    {"p", "p", -1, -1},			/* p - allowed character */
    {"q", "q", -1, -1},			/* q - allowed character */
    {"r", "r", -1, -1},			/* r - allowed character */
    {"s", "s", -1, -1},			/* s - allowed character */
    {"t", "t", -1, -1},			/* t - allowed character */
    {"u", "u", -1, -1},			/* u - allowed character */
    {"v", "v", -1, -1},			/* v - allowed character */
    {"w", "w", -1, -1},			/* w - allowed character */
    {"x", "x", -1, -1},			/* x - allowed character */
    {"y", "y", -1, -1},			/* y - allowed character */
    {"z", "z", -1, -1},			/* z - allowed character */
    {"{", "", -1, -1},			/* { */
    {"|", "", -1, -1},			/* | */
    {"}", "", -1, -1},			/* } */
    {"~", "", -1, -1},			/* ^ */
    {"\x7f", "", -1, -1},		/* DEL */

    /* U+0080 through U+058f */
    { "\xc2\x80", "" , -1, -1},		/*  U+0080 -   - <control> */
    { "\xc2\x81", "" , -1, -1},		/*  U+0081 -   - <control> */
    { "\xc2\x82", "" , -1, -1},		/*  U+0082 -   - <control> */
    { "\xc2\x83", "" , -1, -1},		/*  U+0083 -   - <control> */
    { "\xc2\x84", "" , -1, -1},		/*  U+0084 -   - <control> */
    { "\xc2\x85", "" , -1, -1},		/*  U+0085 -   - <control> */
    { "\xc2\x86", "" , -1, -1},		/*  U+0086 -   - <control> */
    { "\xc2\x87", "" , -1, -1},		/*  U+0087 -   - <control> */
    { "\xc2\x88", "" , -1, -1},		/*  U+0088 -   - <control> */
    { "\xc2\x89", "" , -1, -1},		/*  U+0089 -   - <control> */
    { "\xc2\x8a", "" , -1, -1},		/*  U+008A -   - <control> */
    { "\xc2\x8b", "" , -1, -1},		/*  U+008B -   - <control> */
    { "\xc2\x8c", "" , -1, -1},		/*  U+008C -   - <control> */
    { "\xc2\x8d", "" , -1, -1},		/*  U+008D -   - <control> */
    { "\xc2\x8e", "" , -1, -1},		/*  U+008E -   - <control> */
    { "\xc2\x8f", "" , -1, -1},		/*  U+008F -   - <control> */
    { "\xc2\x90", "" , -1, -1},		/*  U+0090 -   - <control> */
    { "\xc2\x91", "" , -1, -1},		/*  U+0091 -   - <control> */
    { "\xc2\x92", "" , -1, -1},		/*  U+0092 -   - <control> */
    { "\xc2\x93", "" , -1, -1},		/*  U+0093 -   - <control> */
    { "\xc2\x94", "" , -1, -1},		/*  U+0094 -   - <control> */
    { "\xc2\x95", "" , -1, -1},		/*  U+0095 -   - <control> */
    { "\xc2\x96", "" , -1, -1},		/*  U+0096 -   - <control> */
    { "\xc2\x97", "" , -1, -1},		/*  U+0097 -   - <control> */
    { "\xc2\x98", "" , -1, -1},		/*  U+0098 -   - <control> */
    { "\xc2\x99", "" , -1, -1},		/*  U+0099 -   - <control> */
    { "\xc2\x9a", "" , -1, -1},		/*  U+009A -   - <control> */
    { "\xc2\x9b", "" , -1, -1},		/*  U+009B -   - <control> */
    { "\xc2\x9c", "" , -1, -1},		/*  U+009C -   - <control> */
    { "\xc2\x9d", "" , -1, -1},		/*  U+009D -   - <control> */
    { "\xc2\x9e", "" , -1, -1},		/*  U+009E -   - <control> */
    { "\xc2\x9f", "" , -1, -1},		/*  U+009F -   - <control> */
    { "\xc2\xa0", "_" , -1, -1},	/*  U+00A0 -   - NO-BREAK SPACE */
    { "\xc2\xa1", "" , -1, -1},		/*  U+00A1 - ¡ - INVERTED EXCLAMATION MARK */
    { "\xc2\xa2", "c" , -1, -1},	/*  U+00A2 - ¢ - CENT SIGN */
    { "\xc2\xa3", "f" , -1, -1},	/*  U+00A3 - £ - POUND SIGN */
    { "\xc2\xa4", "o" , -1, -1},	/*  U+00A4 - ¤ - CURRENCY SIGN */
    { "\xc2\xa5", "y" , -1, -1},	/*  U+00A5 - ¥ - YEN SIGN */
    { "\xc2\xa6", "" , -1, -1},		/*  U+00A6 - ¦ - BROKEN BAR */
    { "\xc2\xa7", "s" , -1, -1},	/*  U+00A7 - § - SECTION SIGN */
    { "\xc2\xa8", "" , -1, -1},		/*  U+00A8 - ¨ - DIAERESIS */
    { "\xc2\xa9", "o" , -1, -1},	/*  U+00A9 - © - COPYRIGHT SIGN */
    { "\xc2\xaa", "o" , -1, -1},	/*  U+00AA - ª - FEMININE ORDINAL INDICATOR */
    { "\xc2\xab", "" , -1, -1},		/*  U+00AB - « - LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { "\xc2\xac", "-" , -1, -1},	/*  U+00AC - ¬ - NOT SIGN */
    { "\xc2\xad", "-" , -1, -1},	/*  U+00AD - ­ - SOFT HYPHEN */
    { "\xc2\xae", "" , -1, -1},		/*  U+00AE - ® - REGISTERED SIGN */
    { "\xc2\xaf", "-" , -1, -1},	/*  U+00AF - ¯ - MACRON */
    { "\xc2\xb0", "o" , -1, -1},	/*  U+00B0 - ° - DEGREE SIGN */
    { "\xc2\xb1", "+" , -1, -1},	/*  U+00B1 - ± - PLUS-MINUS SIGN */
    { "\xc2\xb2", "2" , -1, -1},	/*  U+00B2 - ² - SUPERSCRIPT TWO */
    { "\xc2\xb3", "3" , -1, -1},	/*  U+00B3 - ³ - SUPERSCRIPT THREE */
    { "\xc2\xb4", "" , -1, -1},		/*  U+00B4 - ´ - ACUTE ACCENT */
    { "\xc2\xb5", "u" , -1, -1},	/*  U+00B5 - µ - MICRO SIGN */
    { "\xc2\xb6", "p" , -1, -1},	/*  U+00B6 - ¶ - PILCROW SIGN */
    { "\xc2\xb7", "." , -1, -1},	/*  U+00B7 - · - MIDDLE DOT */
    { "\xc2\xb8", "" , -1, -1},		/*  U+00B8 - ¸ - CEDILLA */
    { "\xc2\xb9", "1" , -1, -1},	/*  U+00B9 - ¹ - SUPERSCRIPT ONE */
    { "\xc2\xba", "o" , -1, -1},	/*  U+00BA - º - MASCULINE ORDINAL INDICATOR */
    { "\xc2\xbb", "" , -1, -1},		/*  U+00BB - » - RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { "\xc2\xbc", "1" , -1, -1},	/*  U+00BC - ¼ - VULGAR FRACTION ONE QUARTER */
    { "\xc2\xbd", "2" , -1, -1},	/*  U+00BD - ½ - VULGAR FRACTION ONE HALF */
    { "\xc2\xbe", "3" , -1, -1},	/*  U+00BE - ¾ - VULGAR FRACTION THREE QUARTERS */
    { "\xc2\xbf", "" , -1, -1},		/*  U+00BF - ¿ - INVERTED QUESTION MARK */
    { "\xc3\x80", "a" , -1, -1},	/*  U+00C0 - À - LATIN CAPITAL LETTER A WITH GRAVE */
    { "\xc3\x81", "a" , -1, -1},	/*  U+00C1 - Á - LATIN CAPITAL LETTER A WITH ACUTE */
    { "\xc3\x82", "a" , -1, -1},	/*  U+00C2 - Â - LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
    { "\xc3\x83", "a" , -1, -1},	/*  U+00C3 - Ã - LATIN CAPITAL LETTER A WITH TILDE */
    { "\xc3\x84", "a" , -1, -1},	/*  U+00C4 - Ä - LATIN CAPITAL LETTER A WITH DIAERESIS */
    { "\xc3\x85", "a" , -1, -1},	/*  U+00C5 - Å - LATIN CAPITAL LETTER A WITH RING ABOVE */
    { "\xc3\x86", "ae" , -1, -1},	/*  U+00C6 - Æ - LATIN CAPITAL LETTER AE */
    { "\xc3\x87", "c" , -1, -1},	/*  U+00C7 - Ç - LATIN CAPITAL LETTER C WITH CEDILLA */
    { "\xc3\x88", "e" , -1, -1},	/*  U+00C8 - È - LATIN CAPITAL LETTER E WITH GRAVE */
    { "\xc3\x89", "e" , -1, -1},	/*  U+00C9 - É - LATIN CAPITAL LETTER E WITH ACUTE */
    { "\xc3\x8a", "e" , -1, -1},	/*  U+00CA - Ê - LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
    { "\xc3\x8b", "e" , -1, -1},	/*  U+00CB - Ë - LATIN CAPITAL LETTER E WITH DIAERESIS */
    { "\xc3\x8c", "i" , -1, -1},	/*  U+00CC - Ì - LATIN CAPITAL LETTER I WITH GRAVE */
    { "\xc3\x8d", "i" , -1, -1},	/*  U+00CD - Í - LATIN CAPITAL LETTER I WITH ACUTE */
    { "\xc3\x8e", "i" , -1, -1},	/*  U+00CE - Î - LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
    { "\xc3\x8f", "i" , -1, -1},	/*  U+00CF - Ï - LATIN CAPITAL LETTER I WITH DIAERESIS */
    { "\xc3\x90", "d" , -1, -1},	/*  U+00D0 - Ð - LATIN CAPITAL LETTER ETH */
    { "\xc3\x91", "n" , -1, -1},	/*  U+00D1 - Ñ - LATIN CAPITAL LETTER N WITH TILDE */
    { "\xc3\x92", "o" , -1, -1},	/*  U+00D2 - Ò - LATIN CAPITAL LETTER O WITH GRAVE */
    { "\xc3\x93", "o" , -1, -1},	/*  U+00D3 - Ó - LATIN CAPITAL LETTER O WITH ACUTE */
    { "\xc3\x94", "o" , -1, -1},	/*  U+00D4 - Ô - LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
    { "\xc3\x95", "o" , -1, -1},	/*  U+00D5 - Õ - LATIN CAPITAL LETTER O WITH TILDE */
    { "\xc3\x96", "o" , -1, -1},	/*  U+00D6 - Ö - LATIN CAPITAL LETTER O WITH DIAERESIS */
    { "\xc3\x97", "x" , -1, -1},	/*  U+00D7 - × - MULTIPLICATION SIGN */
    { "\xc3\x98", "0" , -1, -1},	/*  U+00D8 - Ø - LATIN CAPITAL LETTER O WITH STROKE */
    { "\xc3\x99", "u" , -1, -1},	/*  U+00D9 - Ù - LATIN CAPITAL LETTER U WITH GRAVE */
    { "\xc3\x9a", "u" , -1, -1},	/*  U+00DA - Ú - LATIN CAPITAL LETTER U WITH ACUTE */
    { "\xc3\x9b", "u" , -1, -1},	/*  U+00DB - Û - LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
    { "\xc3\x9c", "u" , -1, -1},	/*  U+00DC - Ü - LATIN CAPITAL LETTER U WITH DIAERESIS */
    { "\xc3\x9d", "y" , -1, -1},	/*  U+00DD - Ý - LATIN CAPITAL LETTER Y WITH ACUTE */
    { "\xc3\x9e", "p" , -1, -1},	/*  U+00DE - Þ - LATIN CAPITAL LETTER THORN */
    { "\xc3\x9f", "s" , -1, -1},	/*  U+00DF - ß - LATIN SMALL LETTER SHARP S */
    { "\xc3\xa0", "a" , -1, -1},	/*  U+00E0 - à - LATIN SMALL LETTER A WITH GRAVE */
    { "\xc3\xa1", "a" , -1, -1},	/*  U+00E1 - á - LATIN SMALL LETTER A WITH ACUTE */
    { "\xc3\xa2", "a" , -1, -1},	/*  U+00E2 - â - LATIN SMALL LETTER A WITH CIRCUMFLEX */
    { "\xc3\xa3", "a" , -1, -1},	/*  U+00E3 - ã - LATIN SMALL LETTER A WITH TILDE */
    { "\xc3\xa4", "a" , -1, -1},	/*  U+00E4 - ä - LATIN SMALL LETTER A WITH DIAERESIS */
    { "\xc3\xa5", "a" , -1, -1},	/*  U+00E5 - å - LATIN SMALL LETTER A WITH RING ABOVE */
    { "\xc3\xa6", "ae" , -1, -1},	/*  U+00E6 - æ - LATIN SMALL LETTER AE */
    { "\xc3\xa7", "c" , -1, -1},	/*  U+00E7 - ç - LATIN SMALL LETTER C WITH CEDILLA */
    { "\xc3\xa8", "e" , -1, -1},	/*  U+00E8 - è - LATIN SMALL LETTER E WITH GRAVE */
    { "\xc3\xa9", "e" , -1, -1},	/*  U+00E9 - é - LATIN SMALL LETTER E WITH ACUTE */
    { "\xc3\xaa", "e" , -1, -1},	/*  U+00EA - ê - LATIN SMALL LETTER E WITH CIRCUMFLEX */
    { "\xc3\xab", "e" , -1, -1},	/*  U+00EB - ë - LATIN SMALL LETTER E WITH DIAERESIS */
    { "\xc3\xac", "i" , -1, -1},	/*  U+00EC - ì - LATIN SMALL LETTER I WITH GRAVE */
    { "\xc3\xad", "i" , -1, -1},	/*  U+00ED - í - LATIN SMALL LETTER I WITH ACUTE */
    { "\xc3\xae", "i" , -1, -1},	/*  U+00EE - î - LATIN SMALL LETTER I WITH CIRCUMFLEX */
    { "\xc3\xaf", "i" , -1, -1},	/*  U+00EF - ï - LATIN SMALL LETTER I WITH DIAERESIS */
    { "\xc3\xb0", "o" , -1, -1},	/*  U+00F0 - ð - LATIN SMALL LETTER ETH */
    { "\xc3\xb1", "n" , -1, -1},	/*  U+00F1 - ñ - LATIN SMALL LETTER N WITH TILDE */
    { "\xc3\xb2", "o" , -1, -1},	/*  U+00F2 - ò - LATIN SMALL LETTER O WITH GRAVE */
    { "\xc3\xb3", "o" , -1, -1},	/*  U+00F3 - ó - LATIN SMALL LETTER O WITH ACUTE */
    { "\xc3\xb4", "o" , -1, -1},	/*  U+00F4 - ô - LATIN SMALL LETTER O WITH CIRCUMFLEX */
    { "\xc3\xb5", "o" , -1, -1},	/*  U+00F5 - õ - LATIN SMALL LETTER O WITH TILDE */
    { "\xc3\xb6", "o" , -1, -1},	/*  U+00F6 - ö - LATIN SMALL LETTER O WITH DIAERESIS */
    { "\xc3\xb7", "+" , -1, -1},	/*  U+00F7 - ÷ - DIVISION SIGN */
    { "\xc3\xb8", "0" , -1, -1},	/*  U+00F8 - ø - LATIN SMALL LETTER O WITH STROKE */
    { "\xc3\xb9", "u" , -1, -1},	/*  U+00F9 - ù - LATIN SMALL LETTER U WITH GRAVE */
    { "\xc3\xba", "u" , -1, -1},	/*  U+00FA - ú - LATIN SMALL LETTER U WITH ACUTE */
    { "\xc3\xbb", "u" , -1, -1},	/*  U+00FB - û - LATIN SMALL LETTER U WITH CIRCUMFLEX */
    { "\xc3\xbc", "u" , -1, -1},	/*  U+00FC - ü - LATIN SMALL LETTER U WITH DIAERESIS */
    { "\xc3\xbd", "y" , -1, -1},	/*  U+00FD - ý - LATIN SMALL LETTER Y WITH ACUTE */
    { "\xc3\xbe", "b" , -1, -1},	/*  U+00FE - þ - LATIN SMALL LETTER THORN */
    { "\xc3\xbf", "y" , -1, -1},	/*  U+00FF - ÿ - LATIN SMALL LETTER Y WITH DIAERESIS */
    { "\xc4\x80", "a" , -1, -1},	/*  U+0100 - Ā - LATIN CAPITAL LETTER A WITH MACRON */
    { "\xc4\x81", "a" , -1, -1},	/*  U+0101 - ā - LATIN SMALL LETTER A WITH MACRON */
    { "\xc4\x82", "a" , -1, -1},	/*  U+0102 - Ă - LATIN CAPITAL LETTER A WITH BREVE */
    { "\xc4\x83", "a" , -1, -1},	/*  U+0103 - ă - LATIN SMALL LETTER A WITH BREVE */
    { "\xc4\x84", "a" , -1, -1},	/*  U+0104 - Ą - LATIN CAPITAL LETTER A WITH OGONEK */
    { "\xc4\x85", "a" , -1, -1},	/*  U+0105 - ą - LATIN SMALL LETTER A WITH OGONEK */
    { "\xc4\x86", "c" , -1, -1},	/*  U+0106 - Ć - LATIN CAPITAL LETTER C WITH ACUTE */
    { "\xc4\x87", "c" , -1, -1},	/*  U+0107 - ć - LATIN SMALL LETTER C WITH ACUTE */
    { "\xc4\x88", "c" , -1, -1},	/*  U+0108 - Ĉ - LATIN CAPITAL LETTER C WITH CIRCUMFLEX */
    { "\xc4\x89", "c" , -1, -1},	/*  U+0109 - ĉ - LATIN SMALL LETTER C WITH CIRCUMFLEX */
    { "\xc4\x8a", "c" , -1, -1},	/*  U+010A - Ċ - LATIN CAPITAL LETTER C WITH DOT ABOVE */
    { "\xc4\x8b", "c" , -1, -1},	/*  U+010B - ċ - LATIN SMALL LETTER C WITH DOT ABOVE */
    { "\xc4\x8c", "c" , -1, -1},	/*  U+010C - Č - LATIN CAPITAL LETTER C WITH CARON */
    { "\xc4\x8d", "c" , -1, -1},	/*  U+010D - č - LATIN SMALL LETTER C WITH CARON */
    { "\xc4\x8e", "d" , -1, -1},	/*  U+010E - Ď - LATIN CAPITAL LETTER D WITH CARON */
    { "\xc4\x8f", "d" , -1, -1},	/*  U+010F - ď - LATIN SMALL LETTER D WITH CARON */
    { "\xc4\x90", "d" , -1, -1},	/*  U+0110 - Đ - LATIN CAPITAL LETTER D WITH STROKE */
    { "\xc4\x91", "d" , -1, -1},	/*  U+0111 - đ - LATIN SMALL LETTER D WITH STROKE */
    { "\xc4\x92", "e" , -1, -1},	/*  U+0112 - Ē - LATIN CAPITAL LETTER E WITH MACRON */
    { "\xc4\x93", "e" , -1, -1},	/*  U+0113 - ē - LATIN SMALL LETTER E WITH MACRON */
    { "\xc4\x94", "e" , -1, -1},	/*  U+0114 - Ĕ - LATIN CAPITAL LETTER E WITH BREVE */
    { "\xc4\x95", "e" , -1, -1},	/*  U+0115 - ĕ - LATIN SMALL LETTER E WITH BREVE */
    { "\xc4\x96", "e" , -1, -1},	/*  U+0116 - Ė - LATIN CAPITAL LETTER E WITH DOT ABOVE */
    { "\xc4\x97", "e" , -1, -1},	/*  U+0117 - ė - LATIN SMALL LETTER E WITH DOT ABOVE */
    { "\xc4\x98", "e" , -1, -1},	/*  U+0118 - Ę - LATIN CAPITAL LETTER E WITH OGONEK */
    { "\xc4\x99", "e" , -1, -1},	/*  U+0119 - ę - LATIN SMALL LETTER E WITH OGONEK */
    { "\xc4\x9a", "e" , -1, -1},	/*  U+011A - Ě - LATIN CAPITAL LETTER E WITH CARON */
    { "\xc4\x9b", "e" , -1, -1},	/*  U+011B - ě - LATIN SMALL LETTER E WITH CARON */
    { "\xc4\x9c", "g" , -1, -1},	/*  U+011C - Ĝ - LATIN CAPITAL LETTER G WITH CIRCUMFLEX */
    { "\xc4\x9d", "g" , -1, -1},	/*  U+011D - ĝ - LATIN SMALL LETTER G WITH CIRCUMFLEX */
    { "\xc4\x9e", "g" , -1, -1},	/*  U+011E - Ğ - LATIN CAPITAL LETTER G WITH BREVE */
    { "\xc4\x9f", "g" , -1, -1},	/*  U+011F - ğ - LATIN SMALL LETTER G WITH BREVE */
    { "\xc4\xa0", "g" , -1, -1},	/*  U+0120 - Ġ - LATIN CAPITAL LETTER G WITH DOT ABOVE */
    { "\xc4\xa1", "g" , -1, -1},	/*  U+0121 - ġ - LATIN SMALL LETTER G WITH DOT ABOVE */
    { "\xc4\xa2", "g" , -1, -1},	/*  U+0122 - Ģ - LATIN CAPITAL LETTER G WITH CEDILLA */
    { "\xc4\xa3", "g" , -1, -1},	/*  U+0123 - ģ - LATIN SMALL LETTER G WITH CEDILLA */
    { "\xc4\xa4", "h" , -1, -1},	/*  U+0124 - Ĥ - LATIN CAPITAL LETTER H WITH CIRCUMFLEX */
    { "\xc4\xa5", "h" , -1, -1},	/*  U+0125 - ĥ - LATIN SMALL LETTER H WITH CIRCUMFLEX */
    { "\xc4\xa6", "h" , -1, -1},	/*  U+0126 - Ħ - LATIN CAPITAL LETTER H WITH STROKE */
    { "\xc4\xa7", "h" , -1, -1},	/*  U+0127 - ħ - LATIN SMALL LETTER H WITH STROKE */
    { "\xc4\xa8", "i" , -1, -1},	/*  U+0128 - Ĩ - LATIN CAPITAL LETTER I WITH TILDE */
    { "\xc4\xa9", "i" , -1, -1},	/*  U+0129 - ĩ - LATIN SMALL LETTER I WITH TILDE */
    { "\xc4\xaa", "i" , -1, -1},	/*  U+012A - Ī - LATIN CAPITAL LETTER I WITH MACRON */
    { "\xc4\xab", "i" , -1, -1},	/*  U+012B - ī - LATIN SMALL LETTER I WITH MACRON */
    { "\xc4\xac", "i" , -1, -1},	/*  U+012C - Ĭ - LATIN CAPITAL LETTER I WITH BREVE */
    { "\xc4\xad", "i" , -1, -1},	/*  U+012D - ĭ - LATIN SMALL LETTER I WITH BREVE */
    { "\xc4\xae", "i" , -1, -1},	/*  U+012E - Į - LATIN CAPITAL LETTER I WITH OGONEK */
    { "\xc4\xaf", "i" , -1, -1},	/*  U+012F - į - LATIN SMALL LETTER I WITH OGONEK */
    { "\xc4\xb0", "i" , -1, -1},	/*  U+0130 - İ - LATIN CAPITAL LETTER I WITH DOT ABOVE */
    { "\xc4\xb1", "i" , -1, -1},	/*  U+0131 - ı - LATIN SMALL LETTER DOTLESS I */
    { "\xc4\xb2", "ij" , -1, -1},	/*  U+0132 - Ĳ - LATIN CAPITAL LIGATURE IJ */
    { "\xc4\xb3", "ij" , -1, -1},	/*  U+0133 - ĳ - LATIN SMALL LIGATURE IJ */
    { "\xc4\xb4", "j" , -1, -1},	/*  U+0134 - Ĵ - LATIN CAPITAL LETTER J WITH CIRCUMFLEX */
    { "\xc4\xb5", "j" , -1, -1},	/*  U+0135 - ĵ - LATIN SMALL LETTER J WITH CIRCUMFLEX */
    { "\xc4\xb6", "k" , -1, -1},	/*  U+0136 - Ķ - LATIN CAPITAL LETTER K WITH CEDILLA */
    { "\xc4\xb7", "k" , -1, -1},	/*  U+0137 - ķ - LATIN SMALL LETTER K WITH CEDILLA */
    { "\xc4\xb8", "k" , -1, -1},	/*  U+0138 - ĸ - LATIN SMALL LETTER KRA */
    { "\xc4\xb9", "l" , -1, -1},	/*  U+0139 - Ĺ - LATIN CAPITAL LETTER L WITH ACUTE */
    { "\xc4\xba", "l" , -1, -1},	/*  U+013A - ĺ - LATIN SMALL LETTER L WITH ACUTE */
    { "\xc4\xbb", "l" , -1, -1},	/*  U+013B - Ļ - LATIN CAPITAL LETTER L WITH CEDILLA */
    { "\xc4\xbc", "l" , -1, -1},	/*  U+013C - ļ - LATIN SMALL LETTER L WITH CEDILLA */
    { "\xc4\xbd", "l" , -1, -1},	/*  U+013D - Ľ - LATIN CAPITAL LETTER L WITH CARON */
    { "\xc4\xbe", "l" , -1, -1},	/*  U+013E - ľ - LATIN SMALL LETTER L WITH CARON */
    { "\xc4\xbf", "l" , -1, -1},	/*  U+013F - Ŀ - LATIN CAPITAL LETTER L WITH MIDDLE DOT */
    { "\xc5\x80", "l" , -1, -1},	/*  U+0140 - ŀ - LATIN SMALL LETTER L WITH MIDDLE DOT */
    { "\xc5\x81", "l" , -1, -1},	/*  U+0141 - Ł - LATIN CAPITAL LETTER L WITH STROKE */
    { "\xc5\x82", "l" , -1, -1},	/*  U+0142 - ł - LATIN SMALL LETTER L WITH STROKE */
    { "\xc5\x83", "n" , -1, -1},	/*  U+0143 - Ń - LATIN CAPITAL LETTER N WITH ACUTE */
    { "\xc5\x84", "n" , -1, -1},	/*  U+0144 - ń - LATIN SMALL LETTER N WITH ACUTE */
    { "\xc5\x85", "n" , -1, -1},	/*  U+0145 - Ņ - LATIN CAPITAL LETTER N WITH CEDILLA */
    { "\xc5\x86", "n" , -1, -1},	/*  U+0146 - ņ - LATIN SMALL LETTER N WITH CEDILLA */
    { "\xc5\x87", "n" , -1, -1},	/*  U+0147 - Ň - LATIN CAPITAL LETTER N WITH CARON */
    { "\xc5\x88", "n" , -1, -1},	/*  U+0148 - ň - LATIN SMALL LETTER N WITH CARON */
    { "\xc5\x89", "n" , -1, -1},	/*  U+0149 - ŉ - LATIN SMALL LETTER N PRECEDED BY APOSTROPHE */
    { "\xc5\x8a", "n" , -1, -1},	/*  U+014A - Ŋ - LATIN CAPITAL LETTER ENG */
    { "\xc5\x8b", "n" , -1, -1},	/*  U+014B - ŋ - LATIN SMALL LETTER ENG */
    { "\xc5\x8c", "o" , -1, -1},	/*  U+014C - Ō - LATIN CAPITAL LETTER O WITH MACRON */
    { "\xc5\x8d", "o" , -1, -1},	/*  U+014D - ō - LATIN SMALL LETTER O WITH MACRON */
    { "\xc5\x8e", "o" , -1, -1},	/*  U+014E - Ŏ - LATIN CAPITAL LETTER O WITH BREVE */
    { "\xc5\x8f", "o" , -1, -1},	/*  U+014F - ŏ - LATIN SMALL LETTER O WITH BREVE */
    { "\xc5\x90", "o" , -1, -1},	/*  U+0150 - Ő - LATIN CAPITAL LETTER O WITH DOUBLE ACUTE */
    { "\xc5\x91", "o" , -1, -1},	/*  U+0151 - ő - LATIN SMALL LETTER O WITH DOUBLE ACUTE */
    { "\xc5\x92", "oe" , -1, -1},	/*  U+0152 - Œ - LATIN CAPITAL LIGATURE OE */
    { "\xc5\x93", "oe" , -1, -1},	/*  U+0153 - œ - LATIN SMALL LIGATURE OE */
    { "\xc5\x94", "r" , -1, -1},	/*  U+0154 - Ŕ - LATIN CAPITAL LETTER R WITH ACUTE */
    { "\xc5\x95", "r" , -1, -1},	/*  U+0155 - ŕ - LATIN SMALL LETTER R WITH ACUTE */
    { "\xc5\x96", "r" , -1, -1},	/*  U+0156 - Ŗ - LATIN CAPITAL LETTER R WITH CEDILLA */
    { "\xc5\x97", "r" , -1, -1},	/*  U+0157 - ŗ - LATIN SMALL LETTER R WITH CEDILLA */
    { "\xc5\x98", "r" , -1, -1},	/*  U+0158 - Ř - LATIN CAPITAL LETTER R WITH CARON */
    { "\xc5\x99", "r" , -1, -1},	/*  U+0159 - ř - LATIN SMALL LETTER R WITH CARON */
    { "\xc5\x9a", "s" , -1, -1},	/*  U+015A - Ś - LATIN CAPITAL LETTER S WITH ACUTE */
    { "\xc5\x9b", "s" , -1, -1},	/*  U+015B - ś - LATIN SMALL LETTER S WITH ACUTE */
    { "\xc5\x9c", "s" , -1, -1},	/*  U+015C - Ŝ - LATIN CAPITAL LETTER S WITH CIRCUMFLEX */
    { "\xc5\x9d", "s" , -1, -1},	/*  U+015D - ŝ - LATIN SMALL LETTER S WITH CIRCUMFLEX */
    { "\xc5\x9e", "s" , -1, -1},	/*  U+015E - Ş - LATIN CAPITAL LETTER S WITH CEDILLA */
    { "\xc5\x9f", "s" , -1, -1},	/*  U+015F - ş - LATIN SMALL LETTER S WITH CEDILLA */
    { "\xc5\xa0", "s" , -1, -1},	/*  U+0160 - Š - LATIN CAPITAL LETTER S WITH CARON */
    { "\xc5\xa1", "s" , -1, -1},	/*  U+0161 - š - LATIN SMALL LETTER S WITH CARON */
    { "\xc5\xa2", "t" , -1, -1},	/*  U+0162 - Ţ - LATIN CAPITAL LETTER T WITH CEDILLA */
    { "\xc5\xa3", "t" , -1, -1},	/*  U+0163 - ţ - LATIN SMALL LETTER T WITH CEDILLA */
    { "\xc5\xa4", "t" , -1, -1},	/*  U+0164 - Ť - LATIN CAPITAL LETTER T WITH CARON */
    { "\xc5\xa5", "t" , -1, -1},	/*  U+0165 - ť - LATIN SMALL LETTER T WITH CARON */
    { "\xc5\xa6", "t" , -1, -1},	/*  U+0166 - Ŧ - LATIN CAPITAL LETTER T WITH STROKE */
    { "\xc5\xa7", "t" , -1, -1},	/*  U+0167 - ŧ - LATIN SMALL LETTER T WITH STROKE */
    { "\xc5\xa8", "u" , -1, -1},	/*  U+0168 - Ũ - LATIN CAPITAL LETTER U WITH TILDE */
    { "\xc5\xa9", "u" , -1, -1},	/*  U+0169 - ũ - LATIN SMALL LETTER U WITH TILDE */
    { "\xc5\xaa", "u" , -1, -1},	/*  U+016A - Ū - LATIN CAPITAL LETTER U WITH MACRON */
    { "\xc5\xab", "u" , -1, -1},	/*  U+016B - ū - LATIN SMALL LETTER U WITH MACRON */
    { "\xc5\xac", "u" , -1, -1},	/*  U+016C - Ŭ - LATIN CAPITAL LETTER U WITH BREVE */
    { "\xc5\xad", "u" , -1, -1},	/*  U+016D - ŭ - LATIN SMALL LETTER U WITH BREVE */
    { "\xc5\xae", "u" , -1, -1},	/*  U+016E - Ů - LATIN CAPITAL LETTER U WITH RING ABOVE */
    { "\xc5\xaf", "u" , -1, -1},	/*  U+016F - ů - LATIN SMALL LETTER U WITH RING ABOVE */
    { "\xc5\xb0", "u" , -1, -1},	/*  U+0170 - Ű - LATIN CAPITAL LETTER U WITH DOUBLE ACUTE */
    { "\xc5\xb1", "u" , -1, -1},	/*  U+0171 - ű - LATIN SMALL LETTER U WITH DOUBLE ACUTE */
    { "\xc5\xb2", "u" , -1, -1},	/*  U+0172 - Ų - LATIN CAPITAL LETTER U WITH OGONEK */
    { "\xc5\xb3", "u" , -1, -1},	/*  U+0173 - ų - LATIN SMALL LETTER U WITH OGONEK */
    { "\xc5\xb4", "w" , -1, -1},	/*  U+0174 - Ŵ - LATIN CAPITAL LETTER W WITH CIRCUMFLEX */
    { "\xc5\xb5", "w" , -1, -1},	/*  U+0175 - ŵ - LATIN SMALL LETTER W WITH CIRCUMFLEX */
    { "\xc5\xb6", "y" , -1, -1},	/*  U+0176 - Ŷ - LATIN CAPITAL LETTER Y WITH CIRCUMFLEX */
    { "\xc5\xb7", "y" , -1, -1},	/*  U+0177 - ŷ - LATIN SMALL LETTER Y WITH CIRCUMFLEX */
    { "\xc5\xb8", "y" , -1, -1},	/*  U+0178 - Ÿ - LATIN CAPITAL LETTER Y WITH DIAERESIS */
    { "\xc5\xb9", "z" , -1, -1},	/*  U+0179 - Ź - LATIN CAPITAL LETTER Z WITH ACUTE */
    { "\xc5\xba", "z" , -1, -1},	/*  U+017A - ź - LATIN SMALL LETTER Z WITH ACUTE */
    { "\xc5\xbb", "z" , -1, -1},	/*  U+017B - Ż - LATIN CAPITAL LETTER Z WITH DOT ABOVE */
    { "\xc5\xbc", "z" , -1, -1},	/*  U+017C - ż - LATIN SMALL LETTER Z WITH DOT ABOVE */
    { "\xc5\xbd", "z" , -1, -1},	/*  U+017D - Ž - LATIN CAPITAL LETTER Z WITH CARON */
    { "\xc5\xbe", "z" , -1, -1},	/*  U+017E - ž - LATIN SMALL LETTER Z WITH CARON */
    { "\xc5\xbf", "f" , -1, -1},	/*  U+017F - ſ - LATIN SMALL LETTER LONG S */
    { "\xc6\x80", "b" , -1, -1},	/*  U+0180 - ƀ - LATIN SMALL LETTER B WITH STROKE */
    { "\xc6\x81", "b" , -1, -1},	/*  U+0181 - Ɓ - LATIN CAPITAL LETTER B WITH HOOK */
    { "\xc6\x82", "b" , -1, -1},	/*  U+0182 - Ƃ - LATIN CAPITAL LETTER B WITH TOPBAR */
    { "\xc6\x83", "b" , -1, -1},	/*  U+0183 - ƃ - LATIN SMALL LETTER B WITH TOPBAR */
    { "\xc6\x84", "b" , -1, -1},	/*  U+0184 - Ƅ - LATIN CAPITAL LETTER TONE SIX */
    { "\xc6\x85", "b" , -1, -1},	/*  U+0185 - ƅ - LATIN SMALL LETTER TONE SIX */
    { "\xc6\x86", "c" , -1, -1},	/*  U+0186 - Ɔ - LATIN CAPITAL LETTER OPEN O */
    { "\xc6\x87", "c" , -1, -1},	/*  U+0187 - Ƈ - LATIN CAPITAL LETTER C WITH HOOK */
    { "\xc6\x88", "c" , -1, -1},	/*  U+0188 - ƈ - LATIN SMALL LETTER C WITH HOOK */
    { "\xc6\x89", "d" , -1, -1},	/*  U+0189 - Ɖ - LATIN CAPITAL LETTER AFRICAN D */
    { "\xc6\x8a", "d" , -1, -1},	/*  U+018A - Ɗ - LATIN CAPITAL LETTER D WITH HOOK */
    { "\xc6\x8b", "d" , -1, -1},	/*  U+018B - Ƌ - LATIN CAPITAL LETTER D WITH TOPBAR */
    { "\xc6\x8c", "d" , -1, -1},	/*  U+018C - ƌ - LATIN SMALL LETTER D WITH TOPBAR */
    { "\xc6\x8d", "o" , -1, -1},	/*  U+018D - ƍ - LATIN SMALL LETTER TURNED DELTA */
    { "\xc6\x8e", "e" , -1, -1},	/*  U+018E - Ǝ - LATIN CAPITAL LETTER REVERSED E */
    { "\xc6\x8f", "e" , -1, -1},	/*  U+018F - Ə - LATIN CAPITAL LETTER SCHWA */
    { "\xc6\x90", "e" , -1, -1},	/*  U+0190 - Ɛ - LATIN CAPITAL LETTER OPEN E */
    { "\xc6\x91", "f" , -1, -1},	/*  U+0191 - Ƒ - LATIN CAPITAL LETTER F WITH HOOK */
    { "\xc6\x92", "f" , -1, -1},	/*  U+0192 - ƒ - LATIN SMALL LETTER F WITH HOOK */
    { "\xc6\x93", "g" , -1, -1},	/*  U+0193 - Ɠ - LATIN CAPITAL LETTER G WITH HOOK */
    { "\xc6\x94", "y" , -1, -1},	/*  U+0194 - Ɣ - LATIN CAPITAL LETTER GAMMA */
    { "\xc6\x95", "hu" , -1, -1},	/*  U+0195 - ƕ - LATIN SMALL LETTER HV */
    { "\xc6\x96", "l" , -1, -1},	/*  U+0196 - Ɩ - LATIN CAPITAL LETTER IOTA */
    { "\xc6\x97", "i" , -1, -1},	/*  U+0197 - Ɨ - LATIN CAPITAL LETTER I WITH STROKE */
    { "\xc6\x98", "k" , -1, -1},	/*  U+0198 - Ƙ - LATIN CAPITAL LETTER K WITH HOOK */
    { "\xc6\x99", "k" , -1, -1},	/*  U+0199 - ƙ - LATIN SMALL LETTER K WITH HOOK */
    { "\xc6\x9a", "l" , -1, -1},	/*  U+019A - ƚ - LATIN SMALL LETTER L WITH BAR */
    { "\xc6\x9b", "l" , -1, -1},	/*  U+019B - ƛ - LATIN SMALL LETTER LAMBDA WITH STROKE */
    { "\xc6\x9c", "w" , -1, -1},	/*  U+019C - Ɯ - LATIN CAPITAL LETTER TURNED M */
    { "\xc6\x9d", "n" , -1, -1},	/*  U+019D - Ɲ - LATIN CAPITAL LETTER N WITH LEFT HOOK */
    { "\xc6\x9e", "n" , -1, -1},	/*  U+019E - ƞ - LATIN SMALL LETTER N WITH LONG RIGHT LEG */
    { "\xc6\x9f", "0" , -1, -1},	/*  U+019F - Ɵ - LATIN CAPITAL LETTER O WITH MIDDLE TILDE */
    { "\xc6\xa0", "o" , -1, -1},	/*  U+01A0 - Ơ - LATIN CAPITAL LETTER O WITH HORN */
    { "\xc6\xa1", "o" , -1, -1},	/*  U+01A1 - ơ - LATIN SMALL LETTER O WITH HORN */
    { "\xc6\xa2", "oi" , -1, -1},	/*  U+01A2 - Ƣ - LATIN CAPITAL LETTER OI */
    { "\xc6\xa3", "oi" , -1, -1},	/*  U+01A3 - ƣ - LATIN SMALL LETTER OI */
    { "\xc6\xa4", "p" , -1, -1},	/*  U+01A4 - Ƥ - LATIN CAPITAL LETTER P WITH HOOK */
    { "\xc6\xa5", "p" , -1, -1},	/*  U+01A5 - ƥ - LATIN SMALL LETTER P WITH HOOK */
    { "\xc6\xa6", "r" , -1, -1},	/*  U+01A6 - Ʀ - LATIN LETTER YR */
    { "\xc6\xa7", "s" , -1, -1},	/*  U+01A7 - Ƨ - LATIN CAPITAL LETTER TONE TWO */
    { "\xc6\xa8", "s" , -1, -1},	/*  U+01A8 - ƨ - LATIN SMALL LETTER TONE TWO */
    { "\xc6\xa9", "e" , -1, -1},	/*  U+01A9 - Ʃ - LATIN CAPITAL LETTER ESH */
    { "\xc6\xaa", "l" , -1, -1},	/*  U+01AA - ƪ - LATIN LETTER REVERSED ESH LOOP */
    { "\xc6\xab", "t" , -1, -1},	/*  U+01AB - ƫ - LATIN SMALL LETTER T WITH PALATAL HOOK */
    { "\xc6\xac", "t" , -1, -1},	/*  U+01AC - Ƭ - LATIN CAPITAL LETTER T WITH HOOK */
    { "\xc6\xad", "e" , -1, -1},	/*  U+01AD - ƭ - LATIN SMALL LETTER T WITH HOOK */
    { "\xc6\xae", "t" , -1, -1},	/*  U+01AE - Ʈ - LATIN CAPITAL LETTER T WITH RETROFLEX HOOK */
    { "\xc6\xaf", "u" , -1, -1},	/*  U+01AF - Ư - LATIN CAPITAL LETTER U WITH HORN */
    { "\xc6\xb0", "u" , -1, -1},	/*  U+01B0 - ư - LATIN SMALL LETTER U WITH HORN */
    { "\xc6\xb1", "v" , -1, -1},	/*  U+01B1 - Ʊ - LATIN CAPITAL LETTER UPSILON */
    { "\xc6\xb2", "v" , -1, -1},	/*  U+01B2 - Ʋ - LATIN CAPITAL LETTER V WITH HOOK */
    { "\xc6\xb3", "y" , -1, -1},	/*  U+01B3 - Ƴ - LATIN CAPITAL LETTER Y WITH HOOK */
    { "\xc6\xb4", "y" , -1, -1},	/*  U+01B4 - ƴ - LATIN SMALL LETTER Y WITH HOOK */
    { "\xc6\xb5", "z" , -1, -1},	/*  U+01B5 - Ƶ - LATIN CAPITAL LETTER Z WITH STROKE */
    { "\xc6\xb6", "z" , -1, -1},	/*  U+01B6 - ƶ - LATIN SMALL LETTER Z WITH STROKE */
    { "\xc6\xb7", "3" , -1, -1},	/*  U+01B7 - Ʒ - LATIN CAPITAL LETTER EZH */
    { "\xc6\xb8", "e" , -1, -1},	/*  U+01B8 - Ƹ - LATIN CAPITAL LETTER EZH REVERSED */
    { "\xc6\xb9", "e" , -1, -1},	/*  U+01B9 - ƹ - LATIN SMALL LETTER EZH REVERSED */
    { "\xc6\xba", "3" , -1, -1},	/*  U+01BA - ƺ - LATIN SMALL LETTER EZH WITH TAIL */
    { "\xc6\xbb", "2" , -1, -1},	/*  U+01BB - ƻ - LATIN LETTER TWO WITH STROKE */
    { "\xc6\xbc", "5" , -1, -1},	/*  U+01BC - Ƽ - LATIN CAPITAL LETTER TONE FIVE */
    { "\xc6\xbd", "5" , -1, -1},	/*  U+01BD - ƽ - LATIN SMALL LETTER TONE FIVE */
    { "\xc6\xbe", "+" , -1, -1},	/*  U+01BE - ƾ - LATIN LETTER INVERTED GLOTTAL STOP WITH STROKE */
    { "\xc6\xbf", "p" , -1, -1},	/*  U+01BF - ƿ - LATIN LETTER WYNN */
    { "\xc7\x80", "l" , -1, -1},	/*  U+01C0 - ǀ - LATIN LETTER DENTAL CLICK */
    { "\xc7\x81", "ll" , -1, -1},	/*  U+01C1 - ǁ - LATIN LETTER LATERAL CLICK */
    { "\xc7\x82", "+" , -1, -1},	/*  U+01C2 - ǂ - LATIN LETTER ALVEOLAR CLICK */
    { "\xc7\x83", "" , -1, -1},		/*  U+01C3 - ǃ - LATIN LETTER RETROFLEX CLICK */
    { "\xc7\x84", "dz" , -1, -1},	/*  U+01C4 - Ǆ - LATIN CAPITAL LETTER DZ WITH CARON */
    { "\xc7\x85", "dz" , -1, -1},	/*  U+01C5 - ǅ - LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON */
    { "\xc7\x86", "dz" , -1, -1},	/*  U+01C6 - ǆ - LATIN SMALL LETTER DZ WITH CARON */
    { "\xc7\x87", "lj" , -1, -1},	/*  U+01C7 - Ǉ - LATIN CAPITAL LETTER LJ */
    { "\xc7\x88", "lj" , -1, -1},	/*  U+01C8 - ǈ - LATIN CAPITAL LETTER L WITH SMALL LETTER J */
    { "\xc7\x89", "lj" , -1, -1},	/*  U+01C9 - ǉ - LATIN SMALL LETTER LJ */
    { "\xc7\x8a", "nj" , -1, -1},	/*  U+01CA - Ǌ - LATIN CAPITAL LETTER NJ */
    { "\xc7\x8b", "nj" , -1, -1},	/*  U+01CB - ǋ - LATIN CAPITAL LETTER N WITH SMALL LETTER J */
    { "\xc7\x8c", "nj" , -1, -1},	/*  U+01CC - ǌ - LATIN SMALL LETTER NJ */
    { "\xc7\x8d", "a" , -1, -1},	/*  U+01CD - Ǎ - LATIN CAPITAL LETTER A WITH CARON */
    { "\xc7\x8e", "a" , -1, -1},	/*  U+01CE - ǎ - LATIN SMALL LETTER A WITH CARON */
    { "\xc7\x8f", "i" , -1, -1},	/*  U+01CF - Ǐ - LATIN CAPITAL LETTER I WITH CARON */
    { "\xc7\x90", "i" , -1, -1},	/*  U+01D0 - ǐ - LATIN SMALL LETTER I WITH CARON */
    { "\xc7\x91", "o" , -1, -1},	/*  U+01D1 - Ǒ - LATIN CAPITAL LETTER O WITH CARON */
    { "\xc7\x92", "o" , -1, -1},	/*  U+01D2 - ǒ - LATIN SMALL LETTER O WITH CARON */
    { "\xc7\x93", "u" , -1, -1},	/*  U+01D3 - Ǔ - LATIN CAPITAL LETTER U WITH CARON */
    { "\xc7\x94", "u" , -1, -1},	/*  U+01D4 - ǔ - LATIN SMALL LETTER U WITH CARON */
    { "\xc7\x95", "u" , -1, -1},	/*  U+01D5 - Ǖ - LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON */
    { "\xc7\x96", "u" , -1, -1},	/*  U+01D6 - ǖ - LATIN SMALL LETTER U WITH DIAERESIS AND MACRON */
    { "\xc7\x97", "u" , -1, -1},	/*  U+01D7 - Ǘ - LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE */
    { "\xc7\x98", "u" , -1, -1},	/*  U+01D8 - ǘ - LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE */
    { "\xc7\x99", "u" , -1, -1},	/*  U+01D9 - Ǚ - LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON */
    { "\xc7\x9a", "u" , -1, -1},	/*  U+01DA - ǚ - LATIN SMALL LETTER U WITH DIAERESIS AND CARON */
    { "\xc7\x9b", "u" , -1, -1},	/*  U+01DB - Ǜ - LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE */
    { "\xc7\x9c", "u" , -1, -1},	/*  U+01DC - ǜ - LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE */
    { "\xc7\x9d", "e" , -1, -1},	/*  U+01DD - ǝ - LATIN SMALL LETTER TURNED E */
    { "\xc7\x9e", "a" , -1, -1},	/*  U+01DE - Ǟ - LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON */
    { "\xc7\x9f", "a" , -1, -1},	/*  U+01DF - ǟ - LATIN SMALL LETTER A WITH DIAERESIS AND MACRON */
    { "\xc7\xa0", "a" , -1, -1},	/*  U+01E0 - Ǡ - LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON */
    { "\xc7\xa1", "a" , -1, -1},	/*  U+01E1 - ǡ - LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON */
    { "\xc7\xa2", "ae" , -1, -1},	/*  U+01E2 - Ǣ - LATIN CAPITAL LETTER AE WITH MACRON */
    { "\xc7\xa3", "ae" , -1, -1},	/*  U+01E3 - ǣ - LATIN SMALL LETTER AE WITH MACRON */
    { "\xc7\xa4", "g" , -1, -1},	/*  U+01E4 - Ǥ - LATIN CAPITAL LETTER G WITH STROKE */
    { "\xc7\xa5", "g" , -1, -1},	/*  U+01E5 - ǥ - LATIN SMALL LETTER G WITH STROKE */
    { "\xc7\xa6", "g" , -1, -1},	/*  U+01E6 - Ǧ - LATIN CAPITAL LETTER G WITH CARON */
    { "\xc7\xa7", "g" , -1, -1},	/*  U+01E7 - ǧ - LATIN SMALL LETTER G WITH CARON */
    { "\xc7\xa8", "k" , -1, -1},	/*  U+01E8 - Ǩ - LATIN CAPITAL LETTER K WITH CARON */
    { "\xc7\xa9", "k" , -1, -1},	/*  U+01E9 - ǩ - LATIN SMALL LETTER K WITH CARON */
    { "\xc7\xaa", "o" , -1, -1},	/*  U+01EA - Ǫ - LATIN CAPITAL LETTER O WITH OGONEK */
    { "\xc7\xab", "o" , -1, -1},	/*  U+01EB - ǫ - LATIN SMALL LETTER O WITH OGONEK */
    { "\xc7\xac", "o" , -1, -1},	/*  U+01EC - Ǭ - LATIN CAPITAL LETTER O WITH OGONEK AND MACRON */
    { "\xc7\xad", "o" , -1, -1},	/*  U+01ED - ǭ - LATIN SMALL LETTER O WITH OGONEK AND MACRON */
    { "\xc7\xae", "3" , -1, -1},	/*  U+01EE - Ǯ - LATIN CAPITAL LETTER EZH WITH CARON */
    { "\xc7\xaf", "3" , -1, -1},	/*  U+01EF - ǯ - LATIN SMALL LETTER EZH WITH CARON */
    { "\xc7\xb0", "j" , -1, -1},	/*  U+01F0 - ǰ - LATIN SMALL LETTER J WITH CARON */
    { "\xc7\xb1", "dz" , -1, -1},	/*  U+01F1 - Ǳ - LATIN CAPITAL LETTER DZ */
    { "\xc7\xb2", "dz" , -1, -1},	/*  U+01F2 - ǲ - LATIN CAPITAL LETTER D WITH SMALL LETTER Z */
    { "\xc7\xb3", "dz" , -1, -1},	/*  U+01F3 - ǳ - LATIN SMALL LETTER DZ */
    { "\xc7\xb4", "g" , -1, -1},	/*  U+01F4 - Ǵ - LATIN CAPITAL LETTER G WITH ACUTE */
    { "\xc7\xb5", "g" , -1, -1},	/*  U+01F5 - ǵ - LATIN SMALL LETTER G WITH ACUTE */
    { "\xc7\xb6", "hu" , -1, -1},	/*  U+01F6 - Ƕ - LATIN CAPITAL LETTER HWAIR */
    { "\xc7\xb7", "p" , -1, -1},	/*  U+01F7 - Ƿ - LATIN CAPITAL LETTER WYNN */
    { "\xc7\xb8", "n" , -1, -1},	/*  U+01F8 - Ǹ - LATIN CAPITAL LETTER N WITH GRAVE */
    { "\xc7\xb9", "n" , -1, -1},	/*  U+01F9 - ǹ - LATIN SMALL LETTER N WITH GRAVE */
    { "\xc7\xba", "a" , -1, -1},	/*  U+01FA - Ǻ - LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE */
    { "\xc7\xbb", "a" , -1, -1},	/*  U+01FB - ǻ - LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE */
    { "\xc7\xbc", "ae" , -1, -1},	/*  U+01FC - Ǽ - LATIN CAPITAL LETTER AE WITH ACUTE */
    { "\xc7\xbd", "ae" , -1, -1},	/*  U+01FD - ǽ - LATIN SMALL LETTER AE WITH ACUTE */
    { "\xc7\xbe", "0" , -1, -1},	/*  U+01FE - Ǿ - LATIN CAPITAL LETTER O WITH STROKE AND ACUTE */
    { "\xc7\xbf", "0" , -1, -1},	/*  U+01FF - ǿ - LATIN SMALL LETTER O WITH STROKE AND ACUTE */
    { "\xc8\x80", "a" , -1, -1},	/*  U+0200 - Ȁ - LATIN CAPITAL LETTER A WITH DOUBLE GRAVE */
    { "\xc8\x81", "a" , -1, -1},	/*  U+0201 - ȁ - LATIN SMALL LETTER A WITH DOUBLE GRAVE */
    { "\xc8\x82", "a" , -1, -1},	/*  U+0202 - Ȃ - LATIN CAPITAL LETTER A WITH INVERTED BREVE */
    { "\xc8\x83", "a" , -1, -1},	/*  U+0203 - ȃ - LATIN SMALL LETTER A WITH INVERTED BREVE */
    { "\xc8\x84", "e" , -1, -1},	/*  U+0204 - Ȅ - LATIN CAPITAL LETTER E WITH DOUBLE GRAVE */
    { "\xc8\x85", "e" , -1, -1},	/*  U+0205 - ȅ - LATIN SMALL LETTER E WITH DOUBLE GRAVE */
    { "\xc8\x86", "e" , -1, -1},	/*  U+0206 - Ȇ - LATIN CAPITAL LETTER E WITH INVERTED BREVE */
    { "\xc8\x87", "e" , -1, -1},	/*  U+0207 - ȇ - LATIN SMALL LETTER E WITH INVERTED BREVE */
    { "\xc8\x88", "i" , -1, -1},	/*  U+0208 - Ȉ - LATIN CAPITAL LETTER I WITH DOUBLE GRAVE */
    { "\xc8\x89", "i" , -1, -1},	/*  U+0209 - ȉ - LATIN SMALL LETTER I WITH DOUBLE GRAVE */
    { "\xc8\x8a", "i" , -1, -1},	/*  U+020A - Ȋ - LATIN CAPITAL LETTER I WITH INVERTED BREVE */
    { "\xc8\x8b", "i" , -1, -1},	/*  U+020B - ȋ - LATIN SMALL LETTER I WITH INVERTED BREVE */
    { "\xc8\x8c", "o" , -1, -1},	/*  U+020C - Ȍ - LATIN CAPITAL LETTER O WITH DOUBLE GRAVE */
    { "\xc8\x8d", "o" , -1, -1},	/*  U+020D - ȍ - LATIN SMALL LETTER O WITH DOUBLE GRAVE */
    { "\xc8\x8e", "o" , -1, -1},	/*  U+020E - Ȏ - LATIN CAPITAL LETTER O WITH INVERTED BREVE */
    { "\xc8\x8f", "o" , -1, -1},	/*  U+020F - ȏ - LATIN SMALL LETTER O WITH INVERTED BREVE */
    { "\xc8\x90", "r" , -1, -1},	/*  U+0210 - Ȑ - LATIN CAPITAL LETTER R WITH DOUBLE GRAVE */
    { "\xc8\x91", "r" , -1, -1},	/*  U+0211 - ȑ - LATIN SMALL LETTER R WITH DOUBLE GRAVE */
    { "\xc8\x92", "r" , -1, -1},	/*  U+0212 - Ȓ - LATIN CAPITAL LETTER R WITH INVERTED BREVE */
    { "\xc8\x93", "r" , -1, -1},	/*  U+0213 - ȓ - LATIN SMALL LETTER R WITH INVERTED BREVE */
    { "\xc8\x94", "u" , -1, -1},	/*  U+0214 - Ȕ - LATIN CAPITAL LETTER U WITH DOUBLE GRAVE */
    { "\xc8\x95", "u" , -1, -1},	/*  U+0215 - ȕ - LATIN SMALL LETTER U WITH DOUBLE GRAVE */
    { "\xc8\x96", "u" , -1, -1},	/*  U+0216 - Ȗ - LATIN CAPITAL LETTER U WITH INVERTED BREVE */
    { "\xc8\x97", "u" , -1, -1},	/*  U+0217 - ȗ - LATIN SMALL LETTER U WITH INVERTED BREVE */
    { "\xc8\x98", "s" , -1, -1},	/*  U+0218 - Ș - LATIN CAPITAL LETTER S WITH COMMA BELOW */
    { "\xc8\x99", "s" , -1, -1},	/*  U+0219 - ș - LATIN SMALL LETTER S WITH COMMA BELOW */
    { "\xc8\x9a", "t" , -1, -1},	/*  U+021A - Ț - LATIN CAPITAL LETTER T WITH COMMA BELOW */
    { "\xc8\x9b", "t" , -1, -1},	/*  U+021B - ț - LATIN SMALL LETTER T WITH COMMA BELOW */
    { "\xc8\x9c", "3" , -1, -1},	/*  U+021C - Ȝ - LATIN CAPITAL LETTER YOGH */
    { "\xc8\x9d", "3" , -1, -1},	/*  U+021D - ȝ - LATIN SMALL LETTER YOGH */
    { "\xc8\x9e", "h" , -1, -1},	/*  U+021E - Ȟ - LATIN CAPITAL LETTER H WITH CARON */
    { "\xc8\x9f", "h" , -1, -1},	/*  U+021F - ȟ - LATIN SMALL LETTER H WITH CARON */
    { "\xc8\xa0", "n" , -1, -1},	/*  U+0220 - Ƞ - LATIN CAPITAL LETTER N WITH LONG RIGHT LEG */
    { "\xc8\xa1", "d" , -1, -1},	/*  U+0221 - ȡ - LATIN SMALL LETTER D WITH CURL */
    { "\xc8\xa2", "y" , -1, -1},	/*  U+0222 - Ȣ - LATIN CAPITAL LETTER OU */
    { "\xc8\xa3", "y" , -1, -1},	/*  U+0223 - ȣ - LATIN SMALL LETTER OU */
    { "\xc8\xa4", "z" , -1, -1},	/*  U+0224 - Ȥ - LATIN CAPITAL LETTER Z WITH HOOK */
    { "\xc8\xa5", "z" , -1, -1},	/*  U+0225 - ȥ - LATIN SMALL LETTER Z WITH HOOK */
    { "\xc8\xa6", "a" , -1, -1},	/*  U+0226 - Ȧ - LATIN CAPITAL LETTER A WITH DOT ABOVE */
    { "\xc8\xa7", "a" , -1, -1},	/*  U+0227 - ȧ - LATIN SMALL LETTER A WITH DOT ABOVE */
    { "\xc8\xa8", "e" , -1, -1},	/*  U+0228 - Ȩ - LATIN CAPITAL LETTER E WITH CEDILLA */
    { "\xc8\xa9", "e" , -1, -1},	/*  U+0229 - ȩ - LATIN SMALL LETTER E WITH CEDILLA */
    { "\xc8\xaa", "o" , -1, -1},	/*  U+022A - Ȫ - LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON */
    { "\xc8\xab", "o" , -1, -1},	/*  U+022B - ȫ - LATIN SMALL LETTER O WITH DIAERESIS AND MACRON */
    { "\xc8\xac", "o" , -1, -1},	/*  U+022C - Ȭ - LATIN CAPITAL LETTER O WITH TILDE AND MACRON */
    { "\xc8\xad", "o" , -1, -1},	/*  U+022D - ȭ - LATIN SMALL LETTER O WITH TILDE AND MACRON */
    { "\xc8\xae", "o" , -1, -1},	/*  U+022E - Ȯ - LATIN CAPITAL LETTER O WITH DOT ABOVE */
    { "\xc8\xaf", "o" , -1, -1},	/*  U+022F - ȯ - LATIN SMALL LETTER O WITH DOT ABOVE */
    { "\xc8\xb0", "o" , -1, -1},	/*  U+0230 - Ȱ - LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON */
    { "\xc8\xb1", "o" , -1, -1},	/*  U+0231 - ȱ - LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON */
    { "\xc8\xb2", "y" , -1, -1},	/*  U+0232 - Ȳ - LATIN CAPITAL LETTER Y WITH MACRON */
    { "\xc8\xb3", "y" , -1, -1},	/*  U+0233 - ȳ - LATIN SMALL LETTER Y WITH MACRON */
    { "\xc8\xb4", "l" , -1, -1},	/*  U+0234 - ȴ - LATIN SMALL LETTER L WITH CURL */
    { "\xc8\xb5", "n" , -1, -1},	/*  U+0235 - ȵ - LATIN SMALL LETTER N WITH CURL */
    { "\xc8\xb6", "t" , -1, -1},	/*  U+0236 - ȶ - LATIN SMALL LETTER T WITH CURL */
    { "\xc8\xb7", "j" , -1, -1},	/*  U+0237 - ȷ - LATIN SMALL LETTER DOTLESS J */
    { "\xc8\xb8", "db" , -1, -1},	/*  U+0238 - ȸ - LATIN SMALL LETTER DB DIGRAPH */
    { "\xc8\xb9", "qp" , -1, -1},	/*  U+0239 - ȹ - LATIN SMALL LETTER QP DIGRAPH */
    { "\xc8\xba", "a" , -1, -1},	/*  U+023A - Ⱥ - LATIN CAPITAL LETTER A WITH STROKE */
    { "\xc8\xbb", "c" , -1, -1},	/*  U+023B - Ȼ - LATIN CAPITAL LETTER C WITH STROKE */
    { "\xc8\xbc", "c" , -1, -1},	/*  U+023C - ȼ - LATIN SMALL LETTER C WITH STROKE */
    { "\xc8\xbd", "l" , -1, -1},	/*  U+023D - Ƚ - LATIN CAPITAL LETTER L WITH BAR */
    { "\xc8\xbe", "7" , -1, -1},	/*  U+023E - Ⱦ - LATIN CAPITAL LETTER T WITH DIAGONAL STROKE */
    { "\xc8\xbf", "s" , -1, -1},	/*  U+023F - ȿ - LATIN SMALL LETTER S WITH SWASH TAIL */
    { "\xc9\x80", "z" , -1, -1},	/*  U+0240 - ɀ - LATIN SMALL LETTER Z WITH SWASH TAIL */
    { "\xc9\x81", "2" , -1, -1},	/*  U+0241 - Ɂ - LATIN CAPITAL LETTER GLOTTAL STOP */
    { "\xc9\x82", "2" , -1, -1},	/*  U+0242 - ɂ - LATIN SMALL LETTER GLOTTAL STOP */
    { "\xc9\x83", "b" , -1, -1},	/*  U+0243 - Ƀ - LATIN CAPITAL LETTER B WITH STROKE */
    { "\xc9\x84", "u" , -1, -1},	/*  U+0244 - Ʉ - LATIN CAPITAL LETTER U BAR */
    { "\xc9\x85", "a" , -1, -1},	/*  U+0245 - Ʌ - LATIN CAPITAL LETTER TURNED V */
    { "\xc9\x86", "e" , -1, -1},	/*  U+0246 - Ɇ - LATIN CAPITAL LETTER E WITH STROKE */
    { "\xc9\x87", "e" , -1, -1},	/*  U+0247 - ɇ - LATIN SMALL LETTER E WITH STROKE */
    { "\xc9\x88", "j" , -1, -1},	/*  U+0248 - Ɉ - LATIN CAPITAL LETTER J WITH STROKE */
    { "\xc9\x89", "j" , -1, -1},	/*  U+0249 - ɉ - LATIN SMALL LETTER J WITH STROKE */
    { "\xc9\x8a", "q" , -1, -1},	/*  U+024A - Ɋ - LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL */
    { "\xc9\x8b", "q" , -1, -1},	/*  U+024B - ɋ - LATIN SMALL LETTER Q WITH HOOK TAIL */
    { "\xc9\x8c", "r" , -1, -1},	/*  U+024C - Ɍ - LATIN CAPITAL LETTER R WITH STROKE */
    { "\xc9\x8d", "r" , -1, -1},	/*  U+024D - ɍ - LATIN SMALL LETTER R WITH STROKE */
    { "\xc9\x8e", "y" , -1, -1},	/*  U+024E - Ɏ - LATIN CAPITAL LETTER Y WITH STROKE */
    { "\xc9\x8f", "y" , -1, -1},	/*  U+024F - ɏ - LATIN SMALL LETTER Y WITH STROKE */
    { "\xc9\x90", "a" , -1, -1},	/*  U+0250 - ɐ - LATIN SMALL LETTER TURNED A */
    { "\xc9\x91", "a" , -1, -1},	/*  U+0251 - ɑ - LATIN SMALL LETTER ALPHA */
    { "\xc9\x92", "a" , -1, -1},	/*  U+0252 - ɒ - LATIN SMALL LETTER TURNED ALPHA */
    { "\xc9\x93", "g" , -1, -1},	/*  U+0253 - ɓ - LATIN SMALL LETTER B WITH HOOK */
    { "\xc9\x94", "c" , -1, -1},	/*  U+0254 - ɔ - LATIN SMALL LETTER OPEN O */
    { "\xc9\x95", "c" , -1, -1},	/*  U+0255 - ɕ - LATIN SMALL LETTER C WITH CURL */
    { "\xc9\x96", "d" , -1, -1},	/*  U+0256 - ɖ - LATIN SMALL LETTER D WITH TAIL */
    { "\xc9\x97", "d" , -1, -1},	/*  U+0257 - ɗ - LATIN SMALL LETTER D WITH HOOK */
    { "\xc9\x98", "e" , -1, -1},	/*  U+0258 - ɘ - LATIN SMALL LETTER REVERSED E */
    { "\xc9\x99", "e" , -1, -1},	/*  U+0259 - ə - LATIN SMALL LETTER SCHWA */
    { "\xc9\x9a", "e" , -1, -1},	/*  U+025A - ɚ - LATIN SMALL LETTER SCHWA WITH HOOK */
    { "\xc9\x9b", "e" , -1, -1},	/*  U+025B - ɛ - LATIN SMALL LETTER OPEN E */
    { "\xc9\x9c", "e" , -1, -1},	/*  U+025C - ɜ - LATIN SMALL LETTER REVERSED OPEN E */
    { "\xc9\x9d", "e" , -1, -1},	/*  U+025D - ɝ - LATIN SMALL LETTER REVERSED OPEN E WITH HOOK */
    { "\xc9\x9e", "g" , -1, -1},	/*  U+025E - ɞ - LATIN SMALL LETTER CLOSED REVERSED OPEN E */
    { "\xc9\x9f", "f" , -1, -1},	/*  U+025F - ɟ - LATIN SMALL LETTER DOTLESS J WITH STROKE */
    { "\xc9\xa0", "g" , -1, -1},	/*  U+0260 - ɠ - LATIN SMALL LETTER G WITH HOOK */
    { "\xc9\xa1", "g" , -1, -1},	/*  U+0261 - ɡ - LATIN SMALL LETTER SCRIPT G */
    { "\xc9\xa2", "g" , -1, -1},	/*  U+0262 - ɢ - LATIN LETTER SMALL CAPITAL G */
    { "\xc9\xa3", "y" , -1, -1},	/*  U+0263 - ɣ - LATIN SMALL LETTER GAMMA */
    { "\xc9\xa4", "y" , -1, -1},	/*  U+0264 - ɤ - LATIN SMALL LETTER RAMS HORN */
    { "\xc9\xa5", "u" , -1, -1},	/*  U+0265 - ɥ - LATIN SMALL LETTER TURNED H */
    { "\xc9\xa6", "h" , -1, -1},	/*  U+0266 - ɦ - LATIN SMALL LETTER H WITH HOOK */
    { "\xc9\xa7", "h" , -1, -1},	/*  U+0267 - ɧ - LATIN SMALL LETTER HENG WITH HOOK */
    { "\xc9\xa8", "i" , -1, -1},	/*  U+0268 - ɨ - LATIN SMALL LETTER I WITH STROKE */
    { "\xc9\xa9", "l" , -1, -1},	/*  U+0269 - ɩ - LATIN SMALL LETTER IOTA */
    { "\xc9\xaa", "i" , -1, -1},	/*  U+026A - ɪ - LATIN LETTER SMALL CAPITAL I */
    { "\xc9\xab", "l" , -1, -1},	/*  U+026B - ɫ - LATIN SMALL LETTER L WITH MIDDLE TILDE */
    { "\xc9\xac", "l" , -1, -1},	/*  U+026C - ɬ - LATIN SMALL LETTER L WITH BELT */
    { "\xc9\xad", "l" , -1, -1},	/*  U+026D - ɭ - LATIN SMALL LETTER L WITH RETROFLEX HOOK */
    { "\xc9\xae", "l3" , -1, -1},	/*  U+026E - ɮ - LATIN SMALL LETTER LEZH */
    { "\xc9\xaf", "w" , -1, -1},	/*  U+026F - ɯ - LATIN SMALL LETTER TURNED M */
    { "\xc9\xb0", "w" , -1, -1},	/*  U+0270 - ɰ - LATIN SMALL LETTER TURNED M WITH LONG LEG */
    { "\xc9\xb1", "m" , -1, -1},	/*  U+0271 - ɱ - LATIN SMALL LETTER M WITH HOOK */
    { "\xc9\xb2", "n" , -1, -1},	/*  U+0272 - ɲ - LATIN SMALL LETTER N WITH LEFT HOOK */
    { "\xc9\xb3", "n" , -1, -1},	/*  U+0273 - ɳ - LATIN SMALL LETTER N WITH RETROFLEX HOOK */
    { "\xc9\xb4", "n" , -1, -1},	/*  U+0274 - ɴ - LATIN LETTER SMALL CAPITAL N */
    { "\xc9\xb5", "o" , -1, -1},	/*  U+0275 - ɵ - LATIN SMALL LETTER BARRED O */
    { "\xc9\xb6", "ae" , -1, -1},	/*  U+0276 - ɶ - LATIN LETTER SMALL CAPITAL OE */
    { "\xc9\xb7", "n" , -1, -1},	/*  U+0277 - ɷ - LATIN SMALL LETTER CLOSED OMEGA */
    { "\xc9\xb8", "o" , -1, -1},	/*  U+0278 - ɸ - LATIN SMALL LETTER PHI */
    { "\xc9\xb9", "r" , -1, -1},	/*  U+0279 - ɹ - LATIN SMALL LETTER TURNED R */
    { "\xc9\xba", "r" , -1, -1},	/*  U+027A - ɺ - LATIN SMALL LETTER TURNED R WITH LONG LEG */
    { "\xc9\xbb", "r" , -1, -1},	/*  U+027B - ɻ - LATIN SMALL LETTER TURNED R WITH HOOK */
    { "\xc9\xbc", "r" , -1, -1},	/*  U+027C - ɼ - LATIN SMALL LETTER R WITH LONG LEG */
    { "\xc9\xbd", "t" , -1, -1},	/*  U+027D - ɽ - LATIN SMALL LETTER R WITH TAIL */
    { "\xc9\xbe", "r" , -1, -1},	/*  U+027E - ɾ - LATIN SMALL LETTER R WITH FISHHOOK */
    { "\xc9\xbf", "r" , -1, -1},	/*  U+027F - ɿ - LATIN SMALL LETTER REVERSED R WITH FISHHOOK */
    { "\xca\x80", "r" , -1, -1},	/*  U+0280 - ʀ - LATIN LETTER SMALL CAPITAL R */
    { "\xca\x81", "r" , -1, -1},	/*  U+0281 - ʁ - LATIN LETTER SMALL CAPITAL INVERTED R */
    { "\xca\x82", "e" , -1, -1},	/*  U+0282 - ʂ - LATIN SMALL LETTER S WITH HOOK */
    { "\xca\x83", "f" , -1, -1},	/*  U+0283 - ʃ - LATIN SMALL LETTER ESH */
    { "\xca\x84", "f" , -1, -1},	/*  U+0284 - ʄ - LATIN SMALL LETTER DOTLESS J WITH STROKE AND HOOK */
    { "\xca\x85", "l" , -1, -1},	/*  U+0285 - ʅ - LATIN SMALL LETTER SQUAT REVERSED ESH */
    { "\xca\x86", "f" , -1, -1},	/*  U+0286 - ʆ - LATIN SMALL LETTER ESH WITH CURL */
    { "\xca\x87", "t" , -1, -1},	/*  U+0287 - ʇ - LATIN SMALL LETTER TURNED T */
    { "\xca\x88", "t" , -1, -1},	/*  U+0288 - ʈ - LATIN SMALL LETTER T WITH RETROFLEX HOOK */
    { "\xca\x89", "u" , -1, -1},	/*  U+0289 - ʉ - LATIN SMALL LETTER U BAR */
    { "\xca\x8a", "u" , -1, -1},	/*  U+028A - ʊ - LATIN SMALL LETTER UPSILON */
    { "\xca\x8b", "v" , -1, -1},	/*  U+028B - ʋ - LATIN SMALL LETTER V WITH HOOK */
    { "\xca\x8c", "a" , -1, -1},	/*  U+028C - ʌ - LATIN SMALL LETTER TURNED V */
    { "\xca\x8d", "m" , -1, -1},	/*  U+028D - ʍ - LATIN SMALL LETTER TURNED W */
    { "\xca\x8e", "y" , -1, -1},	/*  U+028E - ʎ - LATIN SMALL LETTER TURNED Y */
    { "\xca\x8f", "y" , -1, -1},	/*  U+028F - ʏ - LATIN LETTER SMALL CAPITAL Y */
    { "\xca\x90", "z" , -1, -1},	/*  U+0290 - ʐ - LATIN SMALL LETTER Z WITH RETROFLEX HOOK */
    { "\xca\x91", "z" , -1, -1},	/*  U+0291 - ʑ - LATIN SMALL LETTER Z WITH CURL */
    { "\xca\x92", "3" , -1, -1},	/*  U+0292 - ʒ - LATIN SMALL LETTER EZH */
    { "\xca\x93", "3" , -1, -1},	/*  U+0293 - ʓ - LATIN SMALL LETTER EZH WITH CURL */
    { "\xca\x94", "" , -1, -1},		/*  U+0294 - ʔ - LATIN LETTER GLOTTAL STOP */
    { "\xca\x95", "" , -1, -1},		/*  U+0295 - ʕ - LATIN LETTER PHARYNGEAL VOICED FRICATIVE */
    { "\xca\x96", "" , -1, -1},		/*  U+0296 - ʖ - LATIN LETTER INVERTED GLOTTAL STOP */
    { "\xca\x97", "c" , -1, -1},	/*  U+0297 - ʗ - LATIN LETTER STRETCHED C */
    { "\xca\x98", "0" , -1, -1},	/*  U+0298 - ʘ - LATIN LETTER BILABIAL CLICK */
    { "\xca\x99", "b" , -1, -1},	/*  U+0299 - ʙ - LATIN LETTER SMALL CAPITAL B */
    { "\xca\x9a", "g" , -1, -1},	/*  U+029A - ʚ - LATIN SMALL LETTER CLOSED OPEN E */
    { "\xca\x9b", "g" , -1, -1},	/*  U+029B - ʛ - LATIN LETTER SMALL CAPITAL G WITH HOOK */
    { "\xca\x9c", "h" , -1, -1},	/*  U+029C - ʜ - LATIN LETTER SMALL CAPITAL H */
    { "\xca\x9d", "j" , -1, -1},	/*  U+029D - ʝ - LATIN SMALL LETTER J WITH CROSSED-TAIL */
    { "\xca\x9e", "k" , -1, -1},	/*  U+029E - ʞ - LATIN SMALL LETTER TURNED K */
    { "\xca\x9f", "l" , -1, -1},	/*  U+029F - ʟ - LATIN LETTER SMALL CAPITAL L */
    { "\xca\xa0", "p" , -1, -1},	/*  U+02A0 - ʠ - LATIN SMALL LETTER Q WITH HOOK */
    { "\xca\xa1", "" , -1, -1},		/*  U+02A1 - ʡ - LATIN LETTER GLOTTAL STOP WITH STROKE */
    { "\xca\xa2", "" , -1, -1},		/*  U+02A2 - ʢ - LATIN LETTER REVERSED GLOTTAL STOP WITH STROKE */
    { "\xca\xa3", "dz" , -1, -1},	/*  U+02A3 - ʣ - LATIN SMALL LETTER DZ DIGRAPH */
    { "\xca\xa4", "dz" , -1, -1},	/*  U+02A4 - ʤ - LATIN SMALL LETTER DEZH DIGRAPH */
    { "\xca\xa5", "dz" , -1, -1},	/*  U+02A5 - ʥ - LATIN SMALL LETTER DZ DIGRAPH WITH CURL */
    { "\xca\xa6", "ts" , -1, -1},	/*  U+02A6 - ʦ - LATIN SMALL LETTER TS DIGRAPH */
    { "\xca\xa7", "tf" , -1, -1},	/*  U+02A7 - ʧ - LATIN SMALL LETTER TESH DIGRAPH */
    { "\xca\xa8", "tc" , -1, -1},	/*  U+02A8 - ʨ - LATIN SMALL LETTER TC DIGRAPH WITH CURL */
    { "\xca\xa9", "fn" , -1, -1},	/*  U+02A9 - ʩ - LATIN SMALL LETTER FENG DIGRAPH */
    { "\xca\xaa", "ls" , -1, -1},	/*  U+02AA - ʪ - LATIN SMALL LETTER LS DIGRAPH */
    { "\xca\xab", "lz" , -1, -1},	/*  U+02AB - ʫ - LATIN SMALL LETTER LZ DIGRAPH */
    { "\xca\xac", "ww" , -1, -1},	/*  U+02AC - ʬ - LATIN LETTER BILABIAL PERCUSSIVE */
    { "\xca\xad", "nn" , -1, -1},	/*  U+02AD - ʭ - LATIN LETTER BIDENTAL PERCUSSIVE */
    { "\xca\xae", "y" , -1, -1},	/*  U+02AE - ʮ - LATIN SMALL LETTER TURNED H WITH FISHHOOK */
    { "\xca\xaf", "y" , -1, -1},	/*  U+02AF - ʯ - LATIN SMALL LETTER TURNED H WITH FISHHOOK AND TAIL */
    { "\xca\xb0", "" , -1, -1},		/*  U+02B0 - ʰ - MODIFIER LETTER SMALL H */
    { "\xca\xb1", "" , -1, -1},		/*  U+02B1 - ʱ - MODIFIER LETTER SMALL H WITH HOOK */
    { "\xca\xb2", "" , -1, -1},		/*  U+02B2 - ʲ - MODIFIER LETTER SMALL J */
    { "\xca\xb3", "" , -1, -1},		/*  U+02B3 - ʳ - MODIFIER LETTER SMALL R */
    { "\xca\xb4", "" , -1, -1},		/*  U+02B4 - ʴ - MODIFIER LETTER SMALL TURNED R */
    { "\xca\xb5", "" , -1, -1},		/*  U+02B5 - ʵ - MODIFIER LETTER SMALL TURNED R WITH HOOK */
    { "\xca\xb6", "" , -1, -1},		/*  U+02B6 - ʶ - MODIFIER LETTER SMALL CAPITAL INVERTED R */
    { "\xca\xb7", "" , -1, -1},		/*  U+02B7 - ʷ - MODIFIER LETTER SMALL W */
    { "\xca\xb8", "" , -1, -1},		/*  U+02B8 - ʸ - MODIFIER LETTER SMALL Y */
    { "\xca\xb9", "" , -1, -1},		/*  U+02B9 - ʹ - MODIFIER LETTER PRIME */
    { "\xca\xba", "" , -1, -1},		/*  U+02BA - ʺ - MODIFIER LETTER DOUBLE PRIME */
    { "\xca\xbb", "" , -1, -1},		/*  U+02BB - ʻ - MODIFIER LETTER TURNED COMMA */
    { "\xca\xbc", "" , -1, -1},		/*  U+02BC - ʼ - MODIFIER LETTER APOSTROPHE */
    { "\xca\xbd", "" , -1, -1},		/*  U+02BD - ʽ - MODIFIER LETTER REVERSED COMMA */
    { "\xca\xbe", "" , -1, -1},		/*  U+02BE - ʾ - MODIFIER LETTER RIGHT HALF RING */
    { "\xca\xbf", "" , -1, -1},		/*  U+02BF - ʿ - MODIFIER LETTER LEFT HALF RING */
    { "\xcb\x80", "" , -1, -1},		/*  U+02C0 - ˀ - MODIFIER LETTER GLOTTAL STOP */
    { "\xcb\x81", "" , -1, -1},		/*  U+02C1 - ˁ - MODIFIER LETTER REVERSED GLOTTAL STOP */
    { "\xcb\x82", "" , -1, -1},		/*  U+02C2 - ˂ - MODIFIER LETTER LEFT ARROWHEAD */
    { "\xcb\x83", "" , -1, -1},		/*  U+02C3 - ˃ - MODIFIER LETTER RIGHT ARROWHEAD */
    { "\xcb\x84", "" , -1, -1},		/*  U+02C4 - ˄ - MODIFIER LETTER UP ARROWHEAD */
    { "\xcb\x85", "" , -1, -1},		/*  U+02C5 - ˅ - MODIFIER LETTER DOWN ARROWHEAD */
    { "\xcb\x86", "" , -1, -1},		/*  U+02C6 - ˆ - MODIFIER LETTER CIRCUMFLEX ACCENT */
    { "\xcb\x87", "" , -1, -1},		/*  U+02C7 - ˇ - CARON */
    { "\xcb\x88", "" , -1, -1},		/*  U+02C8 - ˈ - MODIFIER LETTER VERTICAL LINE */
    { "\xcb\x89", "" , -1, -1},		/*  U+02C9 - ˉ - MODIFIER LETTER MACRON */
    { "\xcb\x8a", "" , -1, -1},		/*  U+02CA - ˊ - MODIFIER LETTER ACUTE ACCENT */
    { "\xcb\x8b", "" , -1, -1},		/*  U+02CB - ˋ - MODIFIER LETTER GRAVE ACCENT */
    { "\xcb\x8c", "" , -1, -1},		/*  U+02CC - ˌ - MODIFIER LETTER LOW VERTICAL LINE */
    { "\xcb\x8d", "" , -1, -1},		/*  U+02CD - ˍ - MODIFIER LETTER LOW MACRON */
    { "\xcb\x8e", "" , -1, -1},		/*  U+02CE - ˎ - MODIFIER LETTER LOW GRAVE ACCENT */
    { "\xcb\x8f", "" , -1, -1},		/*  U+02CF - ˏ - MODIFIER LETTER LOW ACUTE ACCENT */
    { "\xcb\x90", "" , -1, -1},		/*  U+02D0 - ː - MODIFIER LETTER TRIANGULAR COLON */
    { "\xcb\x91", "" , -1, -1},		/*  U+02D1 - ˑ - MODIFIER LETTER HALF TRIANGULAR COLON */
    { "\xcb\x92", "" , -1, -1},		/*  U+02D2 - ˒ - MODIFIER LETTER CENTRED RIGHT HALF RING */
    { "\xcb\x93", "" , -1, -1},		/*  U+02D3 - ˓ - MODIFIER LETTER CENTRED LEFT HALF RING */
    { "\xcb\x94", "" , -1, -1},		/*  U+02D4 - ˔ - MODIFIER LETTER UP TACK */
    { "\xcb\x95", "" , -1, -1},		/*  U+02D5 - ˕ - MODIFIER LETTER DOWN TACK */
    { "\xcb\x96", "" , -1, -1},		/*  U+02D6 - ˖ - MODIFIER LETTER PLUS SIGN */
    { "\xcb\x97", "" , -1, -1},		/*  U+02D7 - ˗ - MODIFIER LETTER MINUS SIGN */
    { "\xcb\x98", "" , -1, -1},		/*  U+02D8 - ˘ - BREVE */
    { "\xcb\x99", "" , -1, -1},		/*  U+02D9 - ˙ - DOT ABOVE */
    { "\xcb\x9a", "" , -1, -1},		/*  U+02DA - ˚ - RING ABOVE */
    { "\xcb\x9b", "" , -1, -1},		/*  U+02DB - ˛ - OGONEK */
    { "\xcb\x9c", "" , -1, -1},		/*  U+02DC - ˜ - SMALL TILDE */
    { "\xcb\x9d", "" , -1, -1},		/*  U+02DD - ˝ - DOUBLE ACUTE ACCENT */
    { "\xcb\x9e", "" , -1, -1},		/*  U+02DE - ˞ - MODIFIER LETTER RHOTIC HOOK */
    { "\xcb\x9f", "" , -1, -1},		/*  U+02DF - ˟ - MODIFIER LETTER CROSS ACCENT */
    { "\xcb\xa0", "" , -1, -1},		/*  U+02E0 - ˠ - MODIFIER LETTER SMALL GAMMA */
    { "\xcb\xa1", "" , -1, -1},		/*  U+02E1 - ˡ - MODIFIER LETTER SMALL L */
    { "\xcb\xa2", "" , -1, -1},		/*  U+02E2 - ˢ - MODIFIER LETTER SMALL S */
    { "\xcb\xa3", "" , -1, -1},		/*  U+02E3 - ˣ - MODIFIER LETTER SMALL X */
    { "\xcb\xa4", "" , -1, -1},		/*  U+02E4 - ˤ - MODIFIER LETTER SMALL REVERSED GLOTTAL STOP */
    { "\xcb\xa5", "" , -1, -1},		/*  U+02E5 - ˥ - MODIFIER LETTER EXTRA-HIGH TONE BAR */
    { "\xcb\xa6", "" , -1, -1},		/*  U+02E6 - ˦ - MODIFIER LETTER HIGH TONE BAR */
    { "\xcb\xa7", "" , -1, -1},		/*  U+02E7 - ˧ - MODIFIER LETTER MID TONE BAR */
    { "\xcb\xa8", "" , -1, -1},		/*  U+02E8 - ˨ - MODIFIER LETTER LOW TONE BAR */
    { "\xcb\xa9", "" , -1, -1},		/*  U+02E9 - ˩ - MODIFIER LETTER EXTRA-LOW TONE BAR */
    { "\xcb\xaa", "" , -1, -1},		/*  U+02EA - ˪ - MODIFIER LETTER YIN DEPARTING TONE MARK */
    { "\xcb\xab", "" , -1, -1},		/*  U+02EB - ˫ - MODIFIER LETTER YANG DEPARTING TONE MARK */
    { "\xcb\xac", "" , -1, -1},		/*  U+02EC - ˬ - MODIFIER LETTER VOICING */
    { "\xcb\xad", "" , -1, -1},		/*  U+02ED - ˭ - MODIFIER LETTER UNASPIRATED */
    { "\xcb\xae", "" , -1, -1},		/*  U+02EE - ˮ - MODIFIER LETTER DOUBLE APOSTROPHE */
    { "\xcb\xaf", "" , -1, -1},		/*  U+02EF - ˯ - MODIFIER LETTER LOW DOWN ARROWHEAD */
    { "\xcb\xb0", "" , -1, -1},		/*  U+02F0 - ˰ - MODIFIER LETTER LOW UP ARROWHEAD */
    { "\xcb\xb1", "" , -1, -1},		/*  U+02F1 - ˱ - MODIFIER LETTER LOW LEFT ARROWHEAD */
    { "\xcb\xb2", "" , -1, -1},		/*  U+02F2 - ˲ - MODIFIER LETTER LOW RIGHT ARROWHEAD */
    { "\xcb\xb3", "" , -1, -1},		/*  U+02F3 - ˳ - MODIFIER LETTER LOW RING */
    { "\xcb\xb4", "" , -1, -1},		/*  U+02F4 - ˴ - MODIFIER LETTER MIDDLE GRAVE ACCENT */
    { "\xcb\xb5", "" , -1, -1},		/*  U+02F5 - ˵ - MODIFIER LETTER MIDDLE DOUBLE GRAVE ACCENT */
    { "\xcb\xb6", "" , -1, -1},		/*  U+02F6 - ˶ - MODIFIER LETTER MIDDLE DOUBLE ACUTE ACCENT */
    { "\xcb\xb7", "" , -1, -1},		/*  U+02F7 - ˷ - MODIFIER LETTER LOW TILDE */
    { "\xcb\xb8", "" , -1, -1},		/*  U+02F8 - ˸ - MODIFIER LETTER RAISED COLON */
    { "\xcb\xb9", "" , -1, -1},		/*  U+02F9 - ˹ - MODIFIER LETTER BEGIN HIGH TONE */
    { "\xcb\xba", "" , -1, -1},		/*  U+02FA - ˺ - MODIFIER LETTER END HIGH TONE */
    { "\xcb\xbb", "" , -1, -1},		/*  U+02FB - ˻ - MODIFIER LETTER BEGIN LOW TONE */
    { "\xcb\xbc", "" , -1, -1},		/*  U+02FC - ˼ - MODIFIER LETTER END LOW TONE */
    { "\xcb\xbd", "" , -1, -1},		/*  U+02FD - ˽ - MODIFIER LETTER SHELF */
    { "\xcb\xbe", "" , -1, -1},		/*  U+02FE - ˾ - MODIFIER LETTER OPEN SHELF */
    { "\xcb\xbf", "" , -1, -1},		/*  U+02FF - ˿ - MODIFIER LETTER LOW LEFT ARROW */
    { "\xcc\x80", "" , -1, -1},		/*  U+0300 - >̀ - COMBINING GRAVE ACCENT */
    { "\xcc\x81", "" , -1, -1},		/*  U+0301 - >́ - COMBINING ACUTE ACCENT */
    { "\xcc\x82", "" , -1, -1},		/*  U+0302 - >̂ - COMBINING CIRCUMFLEX ACCENT */
    { "\xcc\x83", "" , -1, -1},		/*  U+0303 - >̃ - COMBINING TILDE */
    { "\xcc\x84", "" , -1, -1},		/*  U+0304 - >̄ - COMBINING MACRON */
    { "\xcc\x85", "" , -1, -1},		/*  U+0305 - >̅ - COMBINING OVERLINE */
    { "\xcc\x86", "" , -1, -1},		/*  U+0306 - >̆ - COMBINING BREVE */
    { "\xcc\x87", "" , -1, -1},		/*  U+0307 - >̇ - COMBINING DOT ABOVE */
    { "\xcc\x88", "" , -1, -1},		/*  U+0308 - >̈ - COMBINING DIAERESIS */
    { "\xcc\x89", "" , -1, -1},		/*  U+0309 - >̉ - COMBINING HOOK ABOVE */
    { "\xcc\x8a", "" , -1, -1},		/*  U+030A - >̊ - COMBINING RING ABOVE */
    { "\xcc\x8b", "" , -1, -1},		/*  U+030B - >̋ - COMBINING DOUBLE ACUTE ACCENT */
    { "\xcc\x8c", "" , -1, -1},		/*  U+030C - >̌ - COMBINING CARON */
    { "\xcc\x8d", "" , -1, -1},		/*  U+030D - >̍ - COMBINING VERTICAL LINE ABOVE */
    { "\xcc\x8e", "" , -1, -1},		/*  U+030E - >̎ - COMBINING DOUBLE VERTICAL LINE ABOVE */
    { "\xcc\x8f", "" , -1, -1},		/*  U+030F - >̏ - COMBINING DOUBLE GRAVE ACCENT */
    { "\xcc\x90", "" , -1, -1},		/*  U+0310 - >̐ - COMBINING CANDRABINDU */
    { "\xcc\x91", "" , -1, -1},		/*  U+0311 - >̑ - COMBINING INVERTED BREVE */
    { "\xcc\x92", "" , -1, -1},		/*  U+0312 - >̒ - COMBINING TURNED COMMA ABOVE */
    { "\xcc\x93", "" , -1, -1},		/*  U+0313 - >̓ - COMBINING COMMA ABOVE */
    { "\xcc\x94", "" , -1, -1},		/*  U+0314 - >̔ - COMBINING REVERSED COMMA ABOVE */
    { "\xcc\x95", "" , -1, -1},		/*  U+0315 - >̕ - COMBINING COMMA ABOVE RIGHT */
    { "\xcc\x96", "" , -1, -1},		/*  U+0316 - >̖ - COMBINING GRAVE ACCENT BELOW */
    { "\xcc\x97", "" , -1, -1},		/*  U+0317 - >̗ - COMBINING ACUTE ACCENT BELOW */
    { "\xcc\x98", "" , -1, -1},		/*  U+0318 - >̘ - COMBINING LEFT TACK BELOW */
    { "\xcc\x99", "" , -1, -1},		/*  U+0319 - >̙ - COMBINING RIGHT TACK BELOW */
    { "\xcc\x9a", "" , -1, -1},		/*  U+031A - >̚ - COMBINING LEFT ANGLE ABOVE */
    { "\xcc\x9b", "" , -1, -1},		/*  U+031B - >̛ - COMBINING HORN */
    { "\xcc\x9c", "" , -1, -1},		/*  U+031C - >̜ - COMBINING LEFT HALF RING BELOW */
    { "\xcc\x9d", "" , -1, -1},		/*  U+031D - >̝ - COMBINING UP TACK BELOW */
    { "\xcc\x9e", "" , -1, -1},		/*  U+031E - >̞ - COMBINING DOWN TACK BELOW */
    { "\xcc\x9f", "" , -1, -1},		/*  U+031F - >̟ - COMBINING PLUS SIGN BELOW */
    { "\xcc\xa0", "" , -1, -1},		/*  U+0320 - >̠ - COMBINING MINUS SIGN BELOW */
    { "\xcc\xa1", "" , -1, -1},		/*  U+0321 - >̡ - COMBINING PALATALIZED HOOK BELOW */
    { "\xcc\xa2", "" , -1, -1},		/*  U+0322 - >̢ - COMBINING RETROFLEX HOOK BELOW */
    { "\xcc\xa3", "" , -1, -1},		/*  U+0323 - >̣ - COMBINING DOT BELOW */
    { "\xcc\xa4", "" , -1, -1},		/*  U+0324 - >̤ - COMBINING DIAERESIS BELOW */
    { "\xcc\xa5", "" , -1, -1},		/*  U+0325 - >̥ - COMBINING RING BELOW */
    { "\xcc\xa6", "" , -1, -1},		/*  U+0326 - >̦ - COMBINING COMMA BELOW */
    { "\xcc\xa7", "" , -1, -1},		/*  U+0327 - >̧ - COMBINING CEDILLA */
    { "\xcc\xa8", "" , -1, -1},		/*  U+0328 - >̨ - COMBINING OGONEK */
    { "\xcc\xa9", "" , -1, -1},		/*  U+0329 - >̩ - COMBINING VERTICAL LINE BELOW */
    { "\xcc\xaa", "" , -1, -1},		/*  U+032A - >̪ - COMBINING BRIDGE BELOW */
    { "\xcc\xab", "" , -1, -1},		/*  U+032B - >̫ - COMBINING INVERTED DOUBLE ARCH BELOW */
    { "\xcc\xac", "" , -1, -1},		/*  U+032C - >̬ - COMBINING CARON BELOW */
    { "\xcc\xad", "" , -1, -1},		/*  U+032D - >̭ - COMBINING CIRCUMFLEX ACCENT BELOW */
    { "\xcc\xae", "" , -1, -1},		/*  U+032E - >̮ - COMBINING BREVE BELOW */
    { "\xcc\xaf", "" , -1, -1},		/*  U+032F - >̯ - COMBINING INVERTED BREVE BELOW */
    { "\xcc\xb0", "" , -1, -1},		/*  U+0330 - >̰ - COMBINING TILDE BELOW */
    { "\xcc\xb1", "" , -1, -1},		/*  U+0331 - >̱ - COMBINING MACRON BELOW */
    { "\xcc\xb2", "" , -1, -1},		/*  U+0332 - >̲ - COMBINING LOW LINE */
    { "\xcc\xb3", "" , -1, -1},		/*  U+0333 - >̳ - COMBINING DOUBLE LOW LINE */
    { "\xcc\xb4", "" , -1, -1},		/*  U+0334 - >̴ - COMBINING TILDE OVERLAY */
    { "\xcc\xb5", "" , -1, -1},		/*  U+0335 - >̵ - COMBINING SHORT STROKE OVERLAY */
    { "\xcc\xb6", "" , -1, -1},		/*  U+0336 - >̶ - COMBINING LONG STROKE OVERLAY */
    { "\xcc\xb7", "" , -1, -1},		/*  U+0337 - >̷ - COMBINING SHORT SOLIDUS OVERLAY */
    { "\xcc\xb8", "" , -1, -1},		/*  U+0338 - ≯ - COMBINING LONG SOLIDUS OVERLAY */
    { "\xcc\xb9", "" , -1, -1},		/*  U+0339 - >̹ - COMBINING RIGHT HALF RING BELOW */
    { "\xcc\xba", "" , -1, -1},		/*  U+033A - >̺ - COMBINING INVERTED BRIDGE BELOW */
    { "\xcc\xbb", "" , -1, -1},		/*  U+033B - >̻ - COMBINING SQUARE BELOW */
    { "\xcc\xbc", "" , -1, -1},		/*  U+033C - >̼ - COMBINING SEAGULL BELOW */
    { "\xcc\xbd", "" , -1, -1},		/*  U+033D - >̽ - COMBINING X ABOVE */
    { "\xcc\xbe", "" , -1, -1},		/*  U+033E - >̾ - COMBINING VERTICAL TILDE */
    { "\xcc\xbf", "" , -1, -1},		/*  U+033F - >̿ - COMBINING DOUBLE OVERLINE */
    { "\xcd\x80", "" , -1, -1},		/*  U+0340 - >̀ - COMBINING GRAVE TONE MARK */
    { "\xcd\x81", "" , -1, -1},		/*  U+0341 - >́ - COMBINING ACUTE TONE MARK */
    { "\xcd\x82", "" , -1, -1},		/*  U+0342 - >͂ - COMBINING GREEK PERISPOMENI */
    { "\xcd\x83", "" , -1, -1},		/*  U+0343 - >̓ - COMBINING GREEK KORONIS */
    { "\xcd\x84", "" , -1, -1},		/*  U+0344 - >̈́ - COMBINING GREEK DIALYTIKA TONOS */
    { "\xcd\x85", "" , -1, -1},		/*  U+0345 - >ͅ - COMBINING GREEK YPOGEGRAMMENI */
    { "\xcd\x86", "" , -1, -1},		/*  U+0346 - >͆ - COMBINING BRIDGE ABOVE */
    { "\xcd\x87", "" , -1, -1},		/*  U+0347 - >͇ - COMBINING EQUALS SIGN BELOW */
    { "\xcd\x88", "" , -1, -1},		/*  U+0348 - >͈ - COMBINING DOUBLE VERTICAL LINE BELOW */
    { "\xcd\x89", "" , -1, -1},		/*  U+0349 - >͉ - COMBINING LEFT ANGLE BELOW */
    { "\xcd\x8a", "" , -1, -1},		/*  U+034A - >͊ - COMBINING NOT TILDE ABOVE */
    { "\xcd\x8b", "" , -1, -1},		/*  U+034B - >͋ - COMBINING HOMOTHETIC ABOVE */
    { "\xcd\x8c", "" , -1, -1},		/*  U+034C - >͌ - COMBINING ALMOST EQUAL TO ABOVE */
    { "\xcd\x8d", "" , -1, -1},		/*  U+034D - >͍ - COMBINING LEFT RIGHT ARROW BELOW */
    { "\xcd\x8e", "" , -1, -1},		/*  U+034E - >͎ - COMBINING UPWARDS ARROW BELOW */
    { "\xcd\x8f", "" , -1, -1},		/*  U+034F - >͏ - COMBINING GRAPHEME JOINER */
    { "\xcd\x90", "" , -1, -1},		/*  U+0350 - >͐ - COMBINING RIGHT ARROWHEAD ABOVE */
    { "\xcd\x91", "" , -1, -1},		/*  U+0351 - >͑ - COMBINING LEFT HALF RING ABOVE */
    { "\xcd\x92", "" , -1, -1},		/*  U+0352 - >͒ - COMBINING FERMATA */
    { "\xcd\x93", "" , -1, -1},		/*  U+0353 - >͓ - COMBINING X BELOW */
    { "\xcd\x94", "" , -1, -1},		/*  U+0354 - >͔ - COMBINING LEFT ARROWHEAD BELOW */
    { "\xcd\x95", "" , -1, -1},		/*  U+0355 - >͕ - COMBINING RIGHT ARROWHEAD BELOW */
    { "\xcd\x96", "" , -1, -1},		/*  U+0356 - >͖ - COMBINING RIGHT ARROWHEAD AND UP ARROWHEAD BELOW */
    { "\xcd\x97", "" , -1, -1},		/*  U+0357 - >͗ - COMBINING RIGHT HALF RING ABOVE */
    { "\xcd\x98", "" , -1, -1},		/*  U+0358 - >͘ - COMBINING DOT ABOVE RIGHT */
    { "\xcd\x99", "" , -1, -1},		/*  U+0359 - >͙ - COMBINING ASTERISK BELOW */
    { "\xcd\x9a", "" , -1, -1},		/*  U+035A - >͚ - COMBINING DOUBLE RING BELOW */
    { "\xcd\x9b", "" , -1, -1},		/*  U+035B - >͛ - COMBINING ZIGZAG ABOVE */
    { "\xcd\x9c", "" , -1, -1},		/*  U+035C - >͜ - COMBINING DOUBLE BREVE BELOW */
    { "\xcd\x9d", "" , -1, -1},		/*  U+035D - >͝ - COMBINING DOUBLE BREVE */
    { "\xcd\x9e", "" , -1, -1},		/*  U+035E - >͞ - COMBINING DOUBLE MACRON */
    { "\xcd\x9f", "" , -1, -1},		/*  U+035F - >͟ - COMBINING DOUBLE MACRON BELOW */
    { "\xcd\xa0", "" , -1, -1},		/*  U+0360 - >͠ - COMBINING DOUBLE TILDE */
    { "\xcd\xa1", "" , -1, -1},		/*  U+0361 - >͡ - COMBINING DOUBLE INVERTED BREVE */
    { "\xcd\xa2", "" , -1, -1},		/*  U+0362 - >͢ - COMBINING DOUBLE RIGHTWARDS ARROW BELOW */
    { "\xcd\xa3", "" , -1, -1},		/*  U+0363 - >ͣ - COMBINING LATIN SMALL LETTER A */
    { "\xcd\xa4", "" , -1, -1},		/*  U+0364 - >ͤ - COMBINING LATIN SMALL LETTER E */
    { "\xcd\xa5", "" , -1, -1},		/*  U+0365 - >ͥ - COMBINING LATIN SMALL LETTER I */
    { "\xcd\xa6", "" , -1, -1},		/*  U+0366 - >ͦ - COMBINING LATIN SMALL LETTER O */
    { "\xcd\xa7", "" , -1, -1},		/*  U+0367 - >ͧ - COMBINING LATIN SMALL LETTER U */
    { "\xcd\xa8", "" , -1, -1},		/*  U+0368 - >ͨ - COMBINING LATIN SMALL LETTER C */
    { "\xcd\xa9", "" , -1, -1},		/*  U+0369 - >ͩ - COMBINING LATIN SMALL LETTER D */
    { "\xcd\xaa", "" , -1, -1},		/*  U+036A - >ͪ - COMBINING LATIN SMALL LETTER H */
    { "\xcd\xab", "" , -1, -1},		/*  U+036B - >ͫ - COMBINING LATIN SMALL LETTER M */
    { "\xcd\xac", "" , -1, -1},		/*  U+036C - >ͬ - COMBINING LATIN SMALL LETTER R */
    { "\xcd\xad", "" , -1, -1},		/*  U+036D - >ͭ - COMBINING LATIN SMALL LETTER T */
    { "\xcd\xae", "" , -1, -1},		/*  U+036E - >ͮ - COMBINING LATIN SMALL LETTER V */
    { "\xcd\xaf", "" , -1, -1},		/*  U+036F - >ͯ - COMBINING LATIN SMALL LETTER X */
    { "\xcd\xb0", "i" , -1, -1},	/*  U+0370 - Ͱ - GREEK CAPITAL LETTER HETA */
    { "\xcd\xb1", "i" , -1, -1},	/*  U+0371 - ͱ - GREEK SMALL LETTER HETA */
    { "\xcd\xb2", "t" , -1, -1},	/*  U+0372 - Ͳ - GREEK CAPITAL LETTER ARCHAIC SAMPI */
    { "\xcd\xb3", "r" , -1, -1},	/*  U+0373 - ͳ - GREEK SMALL LETTER ARCHAIC SAMPI */
    { "\xcd\xb4", "" , -1, -1},		/*  U+0374 - ʹ - GREEK NUMERAL SIGN */
    { "\xcd\xb5", "" , -1, -1},		/*  U+0375 - ͵ - GREEK LOWER NUMERAL SIGN */
    { "\xcd\xb6", "n" , -1, -1},	/*  U+0376 - Ͷ - GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA */
    { "\xcd\xb7", "n" , -1, -1},	/*  U+0377 - ͷ - GREEK SMALL LETTER PAMPHYLIAN DIGAMMA */
    { "\xcd\xb8", "" , -1, -1},		/*  U+0378 - ͸ -  */
    { "\xcd\xb9", "" , -1, -1},		/*  U+0379 - ͹ -  */
    { "\xcd\xba", "." , -1, -1},	/*  U+037A - ͺ - GREEK YPOGEGRAMMENI */
    { "\xcd\xbb", "c" , -1, -1},	/*  U+037B - ͻ - GREEK SMALL REVERSED LUNATE SIGMA SYMBOL */
    { "\xcd\xbc", "c" , -1, -1},	/*  U+037C - ͼ - GREEK SMALL DOTTED LUNATE SIGMA SYMBOL */
    { "\xcd\xbd", "c" , -1, -1},	/*  U+037D - ͽ - GREEK SMALL REVERSED DOTTED LUNATE SIGMA SYMBOL */
    { "\xcd\xbe", "" , -1, -1},		/*  U+037E - ; - GREEK QUESTION MARK */
    { "\xcd\xbf", "j" , -1, -1},	/*  U+037F - Ϳ - GREEK CAPITAL LETTER YOT */
    { "\xce\x80", "" , -1, -1},		/*  U+0380 - ΀ -  */
    { "\xce\x81", "" , -1, -1},		/*  U+0381 - ΁ -  */
    { "\xce\x82", "" , -1, -1},		/*  U+0382 - ΂ -  */
    { "\xce\x83", "" , -1, -1},		/*  U+0383 - ΃ -  */
    { "\xce\x84", "" , -1, -1},		/*  U+0384 - ΄ - GREEK TONOS */
    { "\xce\x85", ".." , -1, -1},	/*  U+0385 - ΅ - GREEK DIALYTIKA TONOS */
    { "\xce\x86", "a" , -1, -1},	/*  U+0386 - Ά - GREEK CAPITAL LETTER ALPHA WITH TONOS */
    { "\xce\x87", "." , -1, -1},	/*  U+0387 - · - GREEK ANO TELEIA */
    { "\xce\x88", "e" , -1, -1},	/*  U+0388 - Έ - GREEK CAPITAL LETTER EPSILON WITH TONOS */
    { "\xce\x89", "h" , -1, -1},	/*  U+0389 - Ή - GREEK CAPITAL LETTER ETA WITH TONOS */
    { "\xce\x8a", "i" , -1, -1},	/*  U+038A - Ί - GREEK CAPITAL LETTER IOTA WITH TONOS */
    { "\xce\x8b", "" , -1, -1},		/*  U+038B - ΋ -  */
    { "\xce\x8c", "o" , -1, -1},	/*  U+038C - Ό - GREEK CAPITAL LETTER OMICRON WITH TONOS */
    { "\xce\x8d", "" , -1, -1},		/*  U+038D - ΍ -  */
    { "\xce\x8e", "y" , -1, -1},	/*  U+038E - Ύ - GREEK CAPITAL LETTER UPSILON WITH TONOS */
    { "\xce\x8f", "n" , -1, -1},	/*  U+038F - Ώ - GREEK CAPITAL LETTER OMEGA WITH TONOS */
    { "\xce\x90", "l" , -1, -1},	/*  U+0390 - ΐ - GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS */
    { "\xce\x91", "a" , -1, -1},	/*  U+0391 - Α - GREEK CAPITAL LETTER ALPHA */
    { "\xce\x92", "b" , -1, -1},	/*  U+0392 - Β - GREEK CAPITAL LETTER BETA */
    { "\xce\x93", "c" , -1, -1},	/*  U+0393 - Γ - GREEK CAPITAL LETTER GAMMA */
    { "\xce\x94", "d" , -1, -1},	/*  U+0394 - Δ - GREEK CAPITAL LETTER DELTA */
    { "\xce\x95", "e" , -1, -1},	/*  U+0395 - Ε - GREEK CAPITAL LETTER EPSILON */
    { "\xce\x96", "z" , -1, -1},	/*  U+0396 - Ζ - GREEK CAPITAL LETTER ZETA */
    { "\xce\x97", "h" , -1, -1},	/*  U+0397 - Η - GREEK CAPITAL LETTER ETA */
    { "\xce\x98", "o" , -1, -1},	/*  U+0398 - Θ - GREEK CAPITAL LETTER THETA */
    { "\xce\x99", "i" , -1, -1},	/*  U+0399 - Ι - GREEK CAPITAL LETTER IOTA */
    { "\xce\x9a", "k" , -1, -1},	/*  U+039A - Κ - GREEK CAPITAL LETTER KAPPA */
    { "\xce\x9b", "l" , -1, -1},	/*  U+039B - Λ - GREEK CAPITAL LETTER LAMBDA */
    { "\xce\x9c", "m" , -1, -1},	/*  U+039C - Μ - GREEK CAPITAL LETTER MU */
    { "\xce\x9d", "n" , -1, -1},	/*  U+039D - Ν - GREEK CAPITAL LETTER NU */
    { "\xce\x9e", "-" , -1, -1},	/*  U+039E - Ξ - GREEK CAPITAL LETTER XI */
    { "\xce\x9f", "o" , -1, -1},	/*  U+039F - Ο - GREEK CAPITAL LETTER OMICRON */
    { "\xce\xa0", "p" , -1, -1},	/*  U+03A0 - Π - GREEK CAPITAL LETTER PI */
    { "\xce\xa1", "r" , -1, -1},	/*  U+03A1 - Ρ - GREEK CAPITAL LETTER RHO */
    { "\xce\xa2", "" , -1, -1},		/*  U+03A2 - ΢ -  */
    { "\xce\xa3", "e" , -1, -1},	/*  U+03A3 - Σ - GREEK CAPITAL LETTER SIGMA */
    { "\xce\xa4", "t" , -1, -1},	/*  U+03A4 - Τ - GREEK CAPITAL LETTER TAU */
    { "\xce\xa5", "y" , -1, -1},	/*  U+03A5 - Υ - GREEK CAPITAL LETTER UPSILON */
    { "\xce\xa6", "o" , -1, -1},	/*  U+03A6 - Φ - GREEK CAPITAL LETTER PHI */
    { "\xce\xa7", "x" , -1, -1},	/*  U+03A7 - Χ - GREEK CAPITAL LETTER CHI */
    { "\xce\xa8", "y" , -1, -1},	/*  U+03A8 - Ψ - GREEK CAPITAL LETTER PSI */
    { "\xce\xa9", "n" , -1, -1},	/*  U+03A9 - Ω - GREEK CAPITAL LETTER OMEGA */
    { "\xce\xaa", "i" , -1, -1},	/*  U+03AA - Ϊ - GREEK CAPITAL LETTER IOTA WITH DIALYTIKA */
    { "\xce\xab", "y" , -1, -1},	/*  U+03AB - Ϋ - GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA */
    { "\xce\xac", "a" , -1, -1},	/*  U+03AC - ά - GREEK SMALL LETTER ALPHA WITH TONOS */
    { "\xce\xad", "e" , -1, -1},	/*  U+03AD - έ - GREEK SMALL LETTER EPSILON WITH TONOS */
    { "\xce\xae", "n" , -1, -1},	/*  U+03AE - ή - GREEK SMALL LETTER ETA WITH TONOS */
    { "\xce\xaf", "i" , -1, -1},	/*  U+03AF - ί - GREEK SMALL LETTER IOTA WITH TONOS */
    { "\xce\xb0", "v" , -1, -1},	/*  U+03B0 - ΰ - GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS */
    { "\xce\xb1", "a" , -1, -1},	/*  U+03B1 - α - GREEK SMALL LETTER ALPHA */
    { "\xce\xb2", "b" , -1, -1},	/*  U+03B2 - β - GREEK SMALL LETTER BETA */
    { "\xce\xb3", "y" , -1, -1},	/*  U+03B3 - γ - GREEK SMALL LETTER GAMMA */
    { "\xce\xb4", "o" , -1, -1},	/*  U+03B4 - δ - GREEK SMALL LETTER DELTA */
    { "\xce\xb5", "e" , -1, -1},	/*  U+03B5 - ε - GREEK SMALL LETTER EPSILON */
    { "\xce\xb6", "z" , -1, -1},	/*  U+03B6 - ζ - GREEK SMALL LETTER ZETA */
    { "\xce\xb7", "n" , -1, -1},	/*  U+03B7 - η - GREEK SMALL LETTER ETA */
    { "\xce\xb8", "o" , -1, -1},	/*  U+03B8 - θ - GREEK SMALL LETTER THETA */
    { "\xce\xb9", "l" , -1, -1},	/*  U+03B9 - ι - GREEK SMALL LETTER IOTA */
    { "\xce\xba", "k" , -1, -1},	/*  U+03BA - κ - GREEK SMALL LETTER KAPPA */
    { "\xce\xbb", "l" , -1, -1},	/*  U+03BB - λ - GREEK SMALL LETTER LAMBDA */
    { "\xce\xbc", "u" , -1, -1},	/*  U+03BC - μ - GREEK SMALL LETTER MU */
    { "\xce\xbd", "v" , -1, -1},	/*  U+03BD - ν - GREEK SMALL LETTER NU */
    { "\xce\xbe", "e" , -1, -1},	/*  U+03BE - ξ - GREEK SMALL LETTER XI */
    { "\xce\xbf", "o" , -1, -1},	/*  U+03BF - ο - GREEK SMALL LETTER OMICRON */
    { "\xcf\x80", "t" , -1, -1},	/*  U+03C0 - π - GREEK SMALL LETTER PI */
    { "\xcf\x81", "p" , -1, -1},	/*  U+03C1 - ρ - GREEK SMALL LETTER RHO */
    { "\xcf\x82", "c" , -1, -1},	/*  U+03C2 - ς - GREEK SMALL LETTER FINAL SIGMA */
    { "\xcf\x83", "o" , -1, -1},	/*  U+03C3 - σ - GREEK SMALL LETTER SIGMA */
    { "\xcf\x84", "t" , -1, -1},	/*  U+03C4 - τ - GREEK SMALL LETTER TAU */
    { "\xcf\x85", "v" , -1, -1},	/*  U+03C5 - υ - GREEK SMALL LETTER UPSILON */
    { "\xcf\x86", "o" , -1, -1},	/*  U+03C6 - φ - GREEK SMALL LETTER PHI */
    { "\xcf\x87", "x" , -1, -1},	/*  U+03C7 - χ - GREEK SMALL LETTER CHI */
    { "\xcf\x88", "w" , -1, -1},	/*  U+03C8 - ψ - GREEK SMALL LETTER PSI */
    { "\xcf\x89", "w" , -1, -1},	/*  U+03C9 - ω - GREEK SMALL LETTER OMEGA */
    { "\xcf\x8a", "i" , -1, -1},	/*  U+03CA - ϊ - GREEK SMALL LETTER IOTA WITH DIALYTIKA */
    { "\xcf\x8b", "v" , -1, -1},	/*  U+03CB - ϋ - GREEK SMALL LETTER UPSILON WITH DIALYTIKA */
    { "\xcf\x8c", "o" , -1, -1},	/*  U+03CC - ό - GREEK SMALL LETTER OMICRON WITH TONOS */
    { "\xcf\x8d", "v" , -1, -1},	/*  U+03CD - ύ - GREEK SMALL LETTER UPSILON WITH TONOS */
    { "\xcf\x8e", "w" , -1, -1},	/*  U+03CE - ώ - GREEK SMALL LETTER OMEGA WITH TONOS */
    { "\xcf\x8f", "k" , -1, -1},	/*  U+03CF - Ϗ - GREEK CAPITAL KAI SYMBOL */
    { "\xcf\x90", "b" , -1, -1},	/*  U+03D0 - ϐ - GREEK BETA SYMBOL */
    { "\xcf\x91", "g" , -1, -1},	/*  U+03D1 - ϑ - GREEK THETA SYMBOL */
    { "\xcf\x92", "y" , -1, -1},	/*  U+03D2 - ϒ - GREEK UPSILON WITH HOOK SYMBOL */
    { "\xcf\x93", "y" , -1, -1},	/*  U+03D3 - ϓ - GREEK UPSILON WITH ACUTE AND HOOK SYMBOL */
    { "\xcf\x94", "y" , -1, -1},	/*  U+03D4 - ϔ - GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL */
    { "\xcf\x95", "o" , -1, -1},	/*  U+03D5 - ϕ - GREEK PHI SYMBOL */
    { "\xcf\x96", "w" , -1, -1},	/*  U+03D6 - ϖ - GREEK PI SYMBOL */
    { "\xcf\x97", "x" , -1, -1},	/*  U+03D7 - ϗ - GREEK KAI SYMBOL */
    { "\xcf\x98", "o" , -1, -1},	/*  U+03D8 - Ϙ - GREEK LETTER ARCHAIC KOPPA */
    { "\xcf\x99", "o" , -1, -1},	/*  U+03D9 - ϙ - GREEK SMALL LETTER ARCHAIC KOPPA */
    { "\xcf\x9a", "c" , -1, -1},	/*  U+03DA - Ϛ - GREEK LETTER STIGMA */
    { "\xcf\x9b", "c" , -1, -1},	/*  U+03DB - ϛ - GREEK SMALL LETTER STIGMA */
    { "\xcf\x9c", "f" , -1, -1},	/*  U+03DC - Ϝ - GREEK LETTER DIGAMMA */
    { "\xcf\x9d", "f" , -1, -1},	/*  U+03DD - ϝ - GREEK SMALL LETTER DIGAMMA */
    { "\xcf\x9e", "h" , -1, -1},	/*  U+03DE - Ϟ - GREEK LETTER KOPPA */
    { "\xcf\x9f", "z" , -1, -1},	/*  U+03DF - ϟ - GREEK SMALL LETTER KOPPA */
    { "\xcf\xa0", "ti" , -1, -1},	/*  U+03E0 - Ϡ - GREEK LETTER SAMPI */
    { "\xcf\xa1", "y" , -1, -1},	/*  U+03E1 - ϡ - GREEK SMALL LETTER SAMPI */
    { "\xcf\xa2", "w" , -1, -1},	/*  U+03E2 - Ϣ - COPTIC CAPITAL LETTER SHEI */
    { "\xcf\xa3", "w" , -1, -1},	/*  U+03E3 - ϣ - COPTIC SMALL LETTER SHEI */
    { "\xcf\xa4", "q" , -1, -1},	/*  U+03E4 - Ϥ - COPTIC CAPITAL LETTER FEI */
    { "\xcf\xa5", "q" , -1, -1},	/*  U+03E5 - ϥ - COPTIC SMALL LETTER FEI */
    { "\xcf\xa6", "b" , -1, -1},	/*  U+03E6 - Ϧ - COPTIC CAPITAL LETTER KHEI */
    { "\xcf\xa7", "e" , -1, -1},	/*  U+03E7 - ϧ - COPTIC SMALL LETTER KHEI */
    { "\xcf\xa8", "8" , -1, -1},	/*  U+03E8 - Ϩ - COPTIC CAPITAL LETTER HORI */
    { "\xcf\xa9", "e" , -1, -1},	/*  U+03E9 - ϩ - COPTIC SMALL LETTER HORI */
    { "\xcf\xaa", "x" , -1, -1},	/*  U+03EA - Ϫ - COPTIC CAPITAL LETTER GANGIA */
    { "\xcf\xab", "x" , -1, -1},	/*  U+03EB - ϫ - COPTIC SMALL LETTER GANGIA */
    { "\xcf\xac", "o" , -1, -1},	/*  U+03EC - Ϭ - COPTIC CAPITAL LETTER SHIMA */
    { "\xcf\xad", "." , -1, -1},	/*  U+03ED - ϭ - COPTIC SMALL LETTER SHIMA */
    { "\xcf\xae", "t" , -1, -1},	/*  U+03EE - Ϯ - COPTIC CAPITAL LETTER DEI */
    { "\xcf\xaf", "t" , -1, -1},	/*  U+03EF - ϯ - COPTIC SMALL LETTER DEI */
    { "\xcf\xb0", "x" , -1, -1},	/*  U+03F0 - ϰ - GREEK KAPPA SYMBOL */
    { "\xcf\xb1", "g" , -1, -1},	/*  U+03F1 - ϱ - GREEK RHO SYMBOL */
    { "\xcf\xb2", "c" , -1, -1},	/*  U+03F2 - ϲ - GREEK LUNATE SIGMA SYMBOL */
    { "\xcf\xb3", "j" , -1, -1},	/*  U+03F3 - ϳ - GREEK LETTER YOT */
    { "\xcf\xb4", "o" , -1, -1},	/*  U+03F4 - ϴ - GREEK CAPITAL THETA SYMBOL */
    { "\xcf\xb5", "e" , -1, -1},	/*  U+03F5 - ϵ - GREEK LUNATE EPSILON SYMBOL */
    { "\xcf\xb6", "e" , -1, -1},	/*  U+03F6 - ϶ - GREEK REVERSED LUNATE EPSILON SYMBOL */
    { "\xcf\xb7", "p" , -1, -1},	/*  U+03F7 - Ϸ - GREEK CAPITAL LETTER SHO */
    { "\xcf\xb8", "p" , -1, -1},	/*  U+03F8 - ϸ - GREEK SMALL LETTER SHO */
    { "\xcf\xb9", "c" , -1, -1},	/*  U+03F9 - Ϲ - GREEK CAPITAL LUNATE SIGMA SYMBOL */
    { "\xcf\xba", "m" , -1, -1},	/*  U+03FA - Ϻ - GREEK CAPITAL LETTER SAN */
    { "\xcf\xbb", "m" , -1, -1},	/*  U+03FB - ϻ - GREEK SMALL LETTER SAN */
    { "\xcf\xbc", "p" , -1, -1},	/*  U+03FC - ϼ - GREEK RHO WITH STROKE SYMBOL */
    { "\xcf\xbd", "c" , -1, -1},	/*  U+03FD - Ͻ - GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL */
    { "\xcf\xbe", "e" , -1, -1},	/*  U+03FE - Ͼ - GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL */
    { "\xcf\xbf", "e" , -1, -1},	/*  U+03FF - Ͽ - GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL */
    { "\xd0\x80", "e" , -1, -1},	/*  U+0400 - Ѐ - CYRILLIC CAPITAL LETTER IE WITH GRAVE */
    { "\xd0\x81", "e" , -1, -1},	/*  U+0401 - Ё - CYRILLIC CAPITAL LETTER IO */
    { "\xd0\x82", "h" , -1, -1},	/*  U+0402 - Ђ - CYRILLIC CAPITAL LETTER DJE */
    { "\xd0\x83", "t" , -1, -1},	/*  U+0403 - Ѓ - CYRILLIC CAPITAL LETTER GJE */
    { "\xd0\x84", "e" , -1, -1},	/*  U+0404 - Є - CYRILLIC CAPITAL LETTER UKRAINIAN IE */
    { "\xd0\x85", "s" , -1, -1},	/*  U+0405 - Ѕ - CYRILLIC CAPITAL LETTER DZE */
    { "\xd0\x86", "i" , -1, -1},	/*  U+0406 - І - CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I */
    { "\xd0\x87", "i" , -1, -1},	/*  U+0407 - Ї - CYRILLIC CAPITAL LETTER YI */
    { "\xd0\x88", "k" , -1, -1},	/*  U+0408 - Ј - CYRILLIC CAPITAL LETTER JE */
    { "\xd0\x89", "jb" , -1, -1},	/*  U+0409 - Љ - CYRILLIC CAPITAL LETTER LJE */
    { "\xd0\x8a", "hb" , -1, -1},	/*  U+040A - Њ - CYRILLIC CAPITAL LETTER NJE */
    { "\xd0\x8b", "h" , -1, -1},	/*  U+040B - Ћ - CYRILLIC CAPITAL LETTER TSHE */
    { "\xd0\x8c", "k" , -1, -1},	/*  U+040C - Ќ - CYRILLIC CAPITAL LETTER KJE */
    { "\xd0\x8d", "n" , -1, -1},	/*  U+040D - Ѝ - CYRILLIC CAPITAL LETTER I WITH GRAVE */
    { "\xd0\x8e", "y" , -1, -1},	/*  U+040E - Ў - CYRILLIC CAPITAL LETTER SHORT U */
    { "\xd0\x8f", "u" , -1, -1},	/*  U+040F - Џ - CYRILLIC CAPITAL LETTER DZHE */
    { "\xd0\x90", "a" , -1, -1},	/*  U+0410 - А - CYRILLIC CAPITAL LETTER A */
    { "\xd0\x91", "b" , -1, -1},	/*  U+0411 - Б - CYRILLIC CAPITAL LETTER BE */
    { "\xd0\x92", "b" , -1, -1},	/*  U+0412 - В - CYRILLIC CAPITAL LETTER VE */
    { "\xd0\x93", "t" , -1, -1},	/*  U+0413 - Г - CYRILLIC CAPITAL LETTER GHE */
    { "\xd0\x94", "a" , -1, -1},	/*  U+0414 - Д - CYRILLIC CAPITAL LETTER DE */
    { "\xd0\x95", "e" , -1, -1},	/*  U+0415 - Е - CYRILLIC CAPITAL LETTER IE */
    { "\xd0\x96", "x" , -1, -1},	/*  U+0416 - Ж - CYRILLIC CAPITAL LETTER ZHE */
    { "\xd0\x97", "3" , -1, -1},	/*  U+0417 - З - CYRILLIC CAPITAL LETTER ZE */
    { "\xd0\x98", "n" , -1, -1},	/*  U+0418 - И - CYRILLIC CAPITAL LETTER I */
    { "\xd0\x99", "n" , -1, -1},	/*  U+0419 - Й - CYRILLIC CAPITAL LETTER SHORT I */
    { "\xd0\x9a", "k" , -1, -1},	/*  U+041A - К - CYRILLIC CAPITAL LETTER KA */
    { "\xd0\x9b", "n" , -1, -1},	/*  U+041B - Л - CYRILLIC CAPITAL LETTER EL */
    { "\xd0\x9c", "m" , -1, -1},	/*  U+041C - М - CYRILLIC CAPITAL LETTER EM */
    { "\xd0\x9d", "h" , -1, -1},	/*  U+041D - Н - CYRILLIC CAPITAL LETTER EN */
    { "\xd0\x9e", "o" , -1, -1},	/*  U+041E - О - CYRILLIC CAPITAL LETTER O */
    { "\xd0\x9f", "n" , -1, -1},	/*  U+041F - П - CYRILLIC CAPITAL LETTER PE */
    { "\xd0\xa0", "p" , -1, -1},	/*  U+0420 - Р - CYRILLIC CAPITAL LETTER ER */
    { "\xd0\xa1", "c" , -1, -1},	/*  U+0421 - С - CYRILLIC CAPITAL LETTER ES */
    { "\xd0\xa2", "t" , -1, -1},	/*  U+0422 - Т - CYRILLIC CAPITAL LETTER TE */
    { "\xd0\xa3", "y" , -1, -1},	/*  U+0423 - У - CYRILLIC CAPITAL LETTER U */
    { "\xd0\xa4", "o" , -1, -1},	/*  U+0424 - Ф - CYRILLIC CAPITAL LETTER EF */
    { "\xd0\xa5", "x" , -1, -1},	/*  U+0425 - Х - CYRILLIC CAPITAL LETTER HA */
    { "\xd0\xa6", "u" , -1, -1},	/*  U+0426 - Ц - CYRILLIC CAPITAL LETTER TSE */
    { "\xd0\xa7", "y" , -1, -1},	/*  U+0427 - Ч - CYRILLIC CAPITAL LETTER CHE */
    { "\xd0\xa8", "w" , -1, -1},	/*  U+0428 - Ш - CYRILLIC CAPITAL LETTER SHA */
    { "\xd0\xa9", "w" , -1, -1},	/*  U+0429 - Щ - CYRILLIC CAPITAL LETTER SHCHA */
    { "\xd0\xaa", "b" , -1, -1},	/*  U+042A - Ъ - CYRILLIC CAPITAL LETTER HARD SIGN */
    { "\xd0\xab", "bl" , -1, -1},	/*  U+042B - Ы - CYRILLIC CAPITAL LETTER YERU */
    { "\xd0\xac", "b" , -1, -1},	/*  U+042C - Ь - CYRILLIC CAPITAL LETTER SOFT SIGN */
    { "\xd0\xad", "e" , -1, -1},	/*  U+042D - Э - CYRILLIC CAPITAL LETTER E */
    { "\xd0\xae", "10" , -1, -1},	/*  U+042E - Ю - CYRILLIC CAPITAL LETTER YU */
    { "\xd0\xaf", "r" , -1, -1},	/*  U+042F - Я - CYRILLIC CAPITAL LETTER YA */
    { "\xd0\xb0", "a" , -1, -1},	/*  U+0430 - а - CYRILLIC SMALL LETTER A */
    { "\xd0\xb1", "g" , -1, -1},	/*  U+0431 - б - CYRILLIC SMALL LETTER BE */
    { "\xd0\xb2", "b" , -1, -1},	/*  U+0432 - в - CYRILLIC SMALL LETTER VE */
    { "\xd0\xb3", "t" , -1, -1},	/*  U+0433 - г - CYRILLIC SMALL LETTER GHE */
    { "\xd0\xb4", "a" , -1, -1},	/*  U+0434 - д - CYRILLIC SMALL LETTER DE */
    { "\xd0\xb5", "e" , -1, -1},	/*  U+0435 - е - CYRILLIC SMALL LETTER IE */
    { "\xd0\xb6", "x" , -1, -1},	/*  U+0436 - ж - CYRILLIC SMALL LETTER ZHE */
    { "\xd0\xb7", "e" , -1, -1},	/*  U+0437 - з - CYRILLIC SMALL LETTER ZE */
    { "\xd0\xb8", "n" , -1, -1},	/*  U+0438 - и - CYRILLIC SMALL LETTER I */
    { "\xd0\xb9", "n" , -1, -1},	/*  U+0439 - й - CYRILLIC SMALL LETTER SHORT I */
    { "\xd0\xba", "k" , -1, -1},	/*  U+043A - к - CYRILLIC SMALL LETTER KA */
    { "\xd0\xbb", "n" , -1, -1},	/*  U+043B - л - CYRILLIC SMALL LETTER EL */
    { "\xd0\xbc", "m" , -1, -1},	/*  U+043C - м - CYRILLIC SMALL LETTER EM */
    { "\xd0\xbd", "h" , -1, -1},	/*  U+043D - н - CYRILLIC SMALL LETTER EN */
    { "\xd0\xbe", "o" , -1, -1},	/*  U+043E - о - CYRILLIC SMALL LETTER O */
    { "\xd0\xbf", "n" , -1, -1},	/*  U+043F - п - CYRILLIC SMALL LETTER PE */
    { "\xd1\x80", "p" , -1, -1},	/*  U+0440 - р - CYRILLIC SMALL LETTER ER */
    { "\xd1\x81", "c" , -1, -1},	/*  U+0441 - с - CYRILLIC SMALL LETTER ES */
    { "\xd1\x82", "t" , -1, -1},	/*  U+0442 - т - CYRILLIC SMALL LETTER TE */
    { "\xd1\x83", "y" , -1, -1},	/*  U+0443 - у - CYRILLIC SMALL LETTER U */
    { "\xd1\x84", "o" , -1, -1},	/*  U+0444 - ф - CYRILLIC SMALL LETTER EF */
    { "\xd1\x85", "x" , -1, -1},	/*  U+0445 - х - CYRILLIC SMALL LETTER HA */
    { "\xd1\x86", "u" , -1, -1},	/*  U+0446 - ц - CYRILLIC SMALL LETTER TSE */
    { "\xd1\x87", "y" , -1, -1},	/*  U+0447 - ч - CYRILLIC SMALL LETTER CHE */
    { "\xd1\x88", "w" , -1, -1},	/*  U+0448 - ш - CYRILLIC SMALL LETTER SHA */
    { "\xd1\x89", "w" , -1, -1},	/*  U+0449 - щ - CYRILLIC SMALL LETTER SHCHA */
    { "\xd1\x8a", "b" , -1, -1},	/*  U+044A - ъ - CYRILLIC SMALL LETTER HARD SIGN */
    { "\xd1\x8b", "bl" , -1, -1},	/*  U+044B - ы - CYRILLIC SMALL LETTER YERU */
    { "\xd1\x8c", "b" , -1, -1},	/*  U+044C - ь - CYRILLIC SMALL LETTER SOFT SIGN */
    { "\xd1\x8d", "e" , -1, -1},	/*  U+044D - э - CYRILLIC SMALL LETTER E */
    { "\xd1\x8e", "10" , -1, -1},	/*  U+044E - ю - CYRILLIC SMALL LETTER YU */
    { "\xd1\x8f", "r" , -1, -1},	/*  U+044F - я - CYRILLIC SMALL LETTER YA */
    { "\xd1\x90", "e" , -1, -1},	/*  U+0450 - ѐ - CYRILLIC SMALL LETTER IE WITH GRAVE */
    { "\xd1\x91", "e" , -1, -1},	/*  U+0451 - ё - CYRILLIC SMALL LETTER IO */
    { "\xd1\x92", "h" , -1, -1},	/*  U+0452 - ђ - CYRILLIC SMALL LETTER DJE */
    { "\xd1\x93", "r" , -1, -1},	/*  U+0453 - ѓ - CYRILLIC SMALL LETTER GJE */
    { "\xd1\x94", "e" , -1, -1},	/*  U+0454 - є - CYRILLIC SMALL LETTER UKRAINIAN IE */
    { "\xd1\x95", "s" , -1, -1},	/*  U+0455 - ѕ - CYRILLIC SMALL LETTER DZE */
    { "\xd1\x96", "i" , -1, -1},	/*  U+0456 - і - CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I */
    { "\xd1\x97", "i" , -1, -1},	/*  U+0457 - ї - CYRILLIC SMALL LETTER YI */
    { "\xd1\x98", "j" , -1, -1},	/*  U+0458 - ј - CYRILLIC SMALL LETTER JE */
    { "\xd1\x99", "jb" , -1, -1},	/*  U+0459 - љ - CYRILLIC SMALL LETTER LJE */
    { "\xd1\x9a", "hb" , -1, -1},	/*  U+045A - њ - CYRILLIC SMALL LETTER NJE */
    { "\xd1\x9b", "h" , -1, -1},	/*  U+045B - ћ - CYRILLIC SMALL LETTER TSHE */
    { "\xd1\x9c", "k" , -1, -1},	/*  U+045C - ќ - CYRILLIC SMALL LETTER KJE */
    { "\xd1\x9d", "n" , -1, -1},	/*  U+045D - ѝ - CYRILLIC SMALL LETTER I WITH GRAVE */
    { "\xd1\x9e", "y" , -1, -1},	/*  U+045E - ў - CYRILLIC SMALL LETTER SHORT U */
    { "\xd1\x9f", "u" , -1, -1},	/*  U+045F - џ - CYRILLIC SMALL LETTER DZHE */
    { "\xd1\xa0", "o" , -1, -1},	/*  U+0460 - Ѡ - CYRILLIC CAPITAL LETTER OMEGA */
    { "\xd1\xa1", "w" , -1, -1},	/*  U+0461 - ѡ - CYRILLIC SMALL LETTER OMEGA */
    { "\xd1\xa2", "b" , -1, -1},	/*  U+0462 - Ѣ - CYRILLIC CAPITAL LETTER YAT */
    { "\xd1\xa3", "b" , -1, -1},	/*  U+0463 - ѣ - CYRILLIC SMALL LETTER YAT */
    { "\xd1\xa4", "ie" , -1, -1},	/*  U+0464 - Ѥ - CYRILLIC CAPITAL LETTER IOTIFIED E */
    { "\xd1\xa5", "ie" , -1, -1},	/*  U+0465 - ѥ - CYRILLIC SMALL LETTER IOTIFIED E */
    { "\xd1\xa6", "a" , -1, -1},	/*  U+0466 - Ѧ - CYRILLIC CAPITAL LETTER LITTLE YUS */
    { "\xd1\xa7", "a" , -1, -1},	/*  U+0467 - ѧ - CYRILLIC SMALL LETTER LITTLE YUS */
    { "\xd1\xa8", "ha" , -1, -1},	/*  U+0468 - Ѩ - CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS */
    { "\xd1\xa9", "ha" , -1, -1},	/*  U+0469 - ѩ - CYRILLIC SMALL LETTER IOTIFIED LITTLE YUS */
    { "\xd1\xaa", "x" , -1, -1},	/*  U+046A - Ѫ - CYRILLIC CAPITAL LETTER BIG YUS */
    { "\xd1\xab", "x" , -1, -1},	/*  U+046B - ѫ - CYRILLIC SMALL LETTER BIG YUS */
    { "\xd1\xac", "hx" , -1, -1},	/*  U+046C - Ѭ - CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS */
    { "\xd1\xad", "hx" , -1, -1},	/*  U+046D - ѭ - CYRILLIC SMALL LETTER IOTIFIED BIG YUS */
    { "\xd1\xae", "3" , -1, -1},	/*  U+046E - Ѯ - CYRILLIC CAPITAL LETTER KSI */
    { "\xd1\xaf", "3" , -1, -1},	/*  U+046F - ѯ - CYRILLIC SMALL LETTER KSI */
    { "\xd1\xb0", "y" , -1, -1},	/*  U+0470 - Ѱ - CYRILLIC CAPITAL LETTER PSI */
    { "\xd1\xb1", "y" , -1, -1},	/*  U+0471 - ѱ - CYRILLIC SMALL LETTER PSI */
    { "\xd1\xb2", "o" , -1, -1},	/*  U+0472 - Ѳ - CYRILLIC CAPITAL LETTER FITA */
    { "\xd1\xb3", "o" , -1, -1},	/*  U+0473 - ѳ - CYRILLIC SMALL LETTER FITA */
    { "\xd1\xb4", "v" , -1, -1},	/*  U+0474 - Ѵ - CYRILLIC CAPITAL LETTER IZHITSA */
    { "\xd1\xb5", "v" , -1, -1},	/*  U+0475 - ѵ - CYRILLIC SMALL LETTER IZHITSA */
    { "\xd1\xb6", "v" , -1, -1},	/*  U+0476 - Ѷ - CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT */
    { "\xd1\xb7", "v" , -1, -1},	/*  U+0477 - ѷ - CYRILLIC SMALL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT */
    { "\xd1\xb8", "oy" , -1, -1},	/*  U+0478 - Ѹ - CYRILLIC CAPITAL LETTER UK */
    { "\xd1\xb9", "oy" , -1, -1},	/*  U+0479 - ѹ - CYRILLIC SMALL LETTER UK */
    { "\xd1\xba", "o" , -1, -1},	/*  U+047A - Ѻ - CYRILLIC CAPITAL LETTER ROUND OMEGA */
    { "\xd1\xbb", "o" , -1, -1},	/*  U+047B - ѻ - CYRILLIC SMALL LETTER ROUND OMEGA */
    { "\xd1\xbc", "" , -1, -1},		/*  U+047C - Ѽ - CYRILLIC CAPITAL LETTER OMEGA WITH TITLO */
    { "\xd1\xbd", "" , -1, -1},		/*  U+047D - ѽ - CYRILLIC SMALL LETTER OMEGA WITH TITLO */
    { "\xd1\xbe", "" , -1, -1},		/*  U+047E - Ѿ - CYRILLIC CAPITAL LETTER OT */
    { "\xd1\xbf", "w" , -1, -1},	/*  U+047F - ѿ - CYRILLIC SMALL LETTER OT */
    { "\xd2\x80", "c" , -1, -1},	/*  U+0480 - Ҁ - CYRILLIC CAPITAL LETTER KOPPA */
    { "\xd2\x81", "c" , -1, -1},	/*  U+0481 - ҁ - CYRILLIC SMALL LETTER KOPPA */
    { "\xd2\x82", "+" , -1, -1},	/*  U+0482 - ҂ - CYRILLIC THOUSANDS SIGN */
    { "\xd2\x83", "" , -1, -1},		/*  U+0483 - >҃ - COMBINING CYRILLIC TITLO */
    { "\xd2\x84", "" , -1, -1},		/*  U+0484 - >҄ - COMBINING CYRILLIC PALATALIZATION */
    { "\xd2\x85", "t" , -1, -1},	/*  U+0485 - >҅ - COMBINING CYRILLIC DASIA PNEUMATA */
    { "\xd2\x86", "t" , -1, -1},	/*  U+0486 - >҆ - COMBINING CYRILLIC PSILI PNEUMATA */
    { "\xd2\x87", "" , -1, -1},		/*  U+0487 - >҇ - COMBINING CYRILLIC POKRYTIE */
    { "\xd2\x88", "" , -1, -1},		/*  U+0488 - >҈ - COMBINING CYRILLIC HUNDRED THOUSANDS SIGN */
    { "\xd2\x89", "" , -1, -1},		/*  U+0489 - >҉ - COMBINING CYRILLIC MILLIONS SIGN */
    { "\xd2\x8a", "n" , -1, -1},	/*  U+048A - Ҋ - CYRILLIC CAPITAL LETTER SHORT I WITH TAIL */
    { "\xd2\x8b", "n" , -1, -1},	/*  U+048B - ҋ - CYRILLIC SMALL LETTER SHORT I WITH TAIL */
    { "\xd2\x8c", "b" , -1, -1},	/*  U+048C - Ҍ - CYRILLIC CAPITAL LETTER SEMISOFT SIGN */
    { "\xd2\x8d", "b" , -1, -1},	/*  U+048D - ҍ - CYRILLIC SMALL LETTER SEMISOFT SIGN */
    { "\xd2\x8e", "p" , -1, -1},	/*  U+048E - Ҏ - CYRILLIC CAPITAL LETTER ER WITH TICK */
    { "\xd2\x8f", "p" , -1, -1},	/*  U+048F - ҏ - CYRILLIC SMALL LETTER ER WITH TICK */
    { "\xd2\x90", "r" , -1, -1},	/*  U+0490 - Ґ - CYRILLIC CAPITAL LETTER GHE WITH UPTURN */
    { "\xd2\x91", "r" , -1, -1},	/*  U+0491 - ґ - CYRILLIC SMALL LETTER GHE WITH UPTURN */
    { "\xd2\x92", "f" , -1, -1},	/*  U+0492 - Ғ - CYRILLIC CAPITAL LETTER GHE WITH STROKE */
    { "\xd2\x93", "f" , -1, -1},	/*  U+0493 - ғ - CYRILLIC SMALL LETTER GHE WITH STROKE */
    { "\xd2\x94", "h" , -1, -1},	/*  U+0494 - Ҕ - CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK */
    { "\xd2\x95", "h" , -1, -1},	/*  U+0495 - ҕ - CYRILLIC SMALL LETTER GHE WITH MIDDLE HOOK */
    { "\xd2\x96", "x" , -1, -1},	/*  U+0496 - Җ - CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER */
    { "\xd2\x97", "x" , -1, -1},	/*  U+0497 - җ - CYRILLIC SMALL LETTER ZHE WITH DESCENDER */
    { "\xd2\x98", "3" , -1, -1},	/*  U+0498 - Ҙ - CYRILLIC CAPITAL LETTER ZE WITH DESCENDER */
    { "\xd2\x99", "3" , -1, -1},	/*  U+0499 - ҙ - CYRILLIC SMALL LETTER ZE WITH DESCENDER */
    { "\xd2\x9a", "k" , -1, -1},	/*  U+049A - Қ - CYRILLIC CAPITAL LETTER KA WITH DESCENDER */
    { "\xd2\x9b", "k" , -1, -1},	/*  U+049B - қ - CYRILLIC SMALL LETTER KA WITH DESCENDER */
    { "\xd2\x9c", "k" , -1, -1},	/*  U+049C - Ҝ - CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE */
    { "\xd2\x9d", "k" , -1, -1},	/*  U+049D - ҝ - CYRILLIC SMALL LETTER KA WITH VERTICAL STROKE */
    { "\xd2\x9e", "k" , -1, -1},	/*  U+049E - Ҟ - CYRILLIC CAPITAL LETTER KA WITH STROKE */
    { "\xd2\x9f", "k" , -1, -1},	/*  U+049F - ҟ - CYRILLIC SMALL LETTER KA WITH STROKE */
    { "\xd2\xa0", "k" , -1, -1},	/*  U+04A0 - Ҡ - CYRILLIC CAPITAL LETTER BASHKIR KA */
    { "\xd2\xa1", "k" , -1, -1},	/*  U+04A1 - ҡ - CYRILLIC SMALL LETTER BASHKIR KA */
    { "\xd2\xa2", "h" , -1, -1},	/*  U+04A2 - Ң - CYRILLIC CAPITAL LETTER EN WITH DESCENDER */
    { "\xd2\xa3", "h" , -1, -1},	/*  U+04A3 - ң - CYRILLIC SMALL LETTER EN WITH DESCENDER */
    { "\xd2\xa4", "hr" , -1, -1},	/*  U+04A4 - Ҥ - CYRILLIC CAPITAL LIGATURE EN GHE */
    { "\xd2\xa5", "hr" , -1, -1},	/*  U+04A5 - ҥ - CYRILLIC SMALL LIGATURE EN GHE */
    { "\xd2\xa6", "tb" , -1, -1},	/*  U+04A6 - Ҧ - CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK */
    { "\xd2\xa7", "tb" , -1, -1},	/*  U+04A7 - ҧ - CYRILLIC SMALL LETTER PE WITH MIDDLE HOOK */
    { "\xd2\xa8", "c" , -1, -1},	/*  U+04A8 - Ҩ - CYRILLIC CAPITAL LETTER ABKHASIAN HA */
    { "\xd2\xa9", "c" , -1, -1},	/*  U+04A9 - ҩ - CYRILLIC SMALL LETTER ABKHASIAN HA */
    { "\xd2\xaa", "c" , -1, -1},	/*  U+04AA - Ҫ - CYRILLIC CAPITAL LETTER ES WITH DESCENDER */
    { "\xd2\xab", "c" , -1, -1},	/*  U+04AB - ҫ - CYRILLIC SMALL LETTER ES WITH DESCENDER */
    { "\xd2\xac", "t" , -1, -1},	/*  U+04AC - Ҭ - CYRILLIC CAPITAL LETTER TE WITH DESCENDER */
    { "\xd2\xad", "t" , -1, -1},	/*  U+04AD - ҭ - CYRILLIC SMALL LETTER TE WITH DESCENDER */
    { "\xd2\xae", "y" , -1, -1},	/*  U+04AE - Ү - CYRILLIC CAPITAL LETTER STRAIGHT U */
    { "\xd2\xaf", "v" , -1, -1},	/*  U+04AF - ү - CYRILLIC SMALL LETTER STRAIGHT U */
    { "\xd2\xb0", "y" , -1, -1},	/*  U+04B0 - Ұ - CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE */
    { "\xd2\xb1", "v" , -1, -1},	/*  U+04B1 - ұ - CYRILLIC SMALL LETTER STRAIGHT U WITH STROKE */
    { "\xd2\xb2", "x" , -1, -1},	/*  U+04B2 - Ҳ - CYRILLIC CAPITAL LETTER HA WITH DESCENDER */
    { "\xd2\xb3", "x" , -1, -1},	/*  U+04B3 - ҳ - CYRILLIC SMALL LETTER HA WITH DESCENDER */
    { "\xd2\xb4", "ti" , -1, -1},	/*  U+04B4 - Ҵ - CYRILLIC CAPITAL LIGATURE TE TSE */
    { "\xd2\xb5", "ti" , -1, -1},	/*  U+04B5 - ҵ - CYRILLIC SMALL LIGATURE TE TSE */
    { "\xd2\xb6", "y" , -1, -1},	/*  U+04B6 - Ҷ - CYRILLIC CAPITAL LETTER CHE WITH DESCENDER */
    { "\xd2\xb7", "y" , -1, -1},	/*  U+04B7 - ҷ - CYRILLIC SMALL LETTER CHE WITH DESCENDER */
    { "\xd2\xb8", "y" , -1, -1},	/*  U+04B8 - Ҹ - CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE */
    { "\xd2\xb9", "y" , -1, -1},	/*  U+04B9 - ҹ - CYRILLIC SMALL LETTER CHE WITH VERTICAL STROKE */
    { "\xd2\xba", "h" , -1, -1},	/*  U+04BA - Һ - CYRILLIC CAPITAL LETTER SHHA */
    { "\xd2\xbb", "h" , -1, -1},	/*  U+04BB - һ - CYRILLIC SMALL LETTER SHHA */
    { "\xd2\xbc", "e" , -1, -1},	/*  U+04BC - Ҽ - CYRILLIC CAPITAL LETTER ABKHASIAN CHE */
    { "\xd2\xbd", "e" , -1, -1},	/*  U+04BD - ҽ - CYRILLIC SMALL LETTER ABKHASIAN CHE */
    { "\xd2\xbe", "e" , -1, -1},	/*  U+04BE - Ҿ - CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER */
    { "\xd2\xbf", "e" , -1, -1},	/*  U+04BF - ҿ - CYRILLIC SMALL LETTER ABKHASIAN CHE WITH DESCENDER */
    { "\xd3\x80", "i" , -1, -1},	/*  U+04C0 - Ӏ - CYRILLIC LETTER PALOCHKA */
    { "\xd3\x81", "x" , -1, -1},	/*  U+04C1 - Ӂ - CYRILLIC CAPITAL LETTER ZHE WITH BREVE */
    { "\xd3\x82", "x" , -1, -1},	/*  U+04C2 - ӂ - CYRILLIC SMALL LETTER ZHE WITH BREVE */
    { "\xd3\x83", "k" , -1, -1},	/*  U+04C3 - Ӄ - CYRILLIC CAPITAL LETTER KA WITH HOOK */
    { "\xd3\x84", "k" , -1, -1},	/*  U+04C4 - ӄ - CYRILLIC SMALL LETTER KA WITH HOOK */
    { "\xd3\x85", "n" , -1, -1},	/*  U+04C5 - Ӆ - CYRILLIC CAPITAL LETTER EL WITH TAIL */
    { "\xd3\x86", "n" , -1, -1},	/*  U+04C6 - ӆ - CYRILLIC SMALL LETTER EL WITH TAIL */
    { "\xd3\x87", "h" , -1, -1},	/*  U+04C7 - Ӈ - CYRILLIC CAPITAL LETTER EN WITH HOOK */
    { "\xd3\x88", "h" , -1, -1},	/*  U+04C8 - ӈ - CYRILLIC SMALL LETTER EN WITH HOOK */
    { "\xd3\x89", "h" , -1, -1},	/*  U+04C9 - Ӊ - CYRILLIC CAPITAL LETTER EN WITH TAIL */
    { "\xd3\x8a", "h" , -1, -1},	/*  U+04CA - ӊ - CYRILLIC SMALL LETTER EN WITH TAIL */
    { "\xd3\x8b", "y" , -1, -1},	/*  U+04CB - Ӌ - CYRILLIC CAPITAL LETTER KHAKASSIAN CHE */
    { "\xd3\x8c", "y" , -1, -1},	/*  U+04CC - ӌ - CYRILLIC SMALL LETTER KHAKASSIAN CHE */
    { "\xd3\x8d", "m" , -1, -1},	/*  U+04CD - Ӎ - CYRILLIC CAPITAL LETTER EM WITH TAIL */
    { "\xd3\x8e", "m" , -1, -1},	/*  U+04CE - ӎ - CYRILLIC SMALL LETTER EM WITH TAIL */
    { "\xd3\x8f", "l" , -1, -1},	/*  U+04CF - ӏ - CYRILLIC SMALL LETTER PALOCHKA */
    { "\xd3\x90", "a" , -1, -1},	/*  U+04D0 - Ӑ - CYRILLIC CAPITAL LETTER A WITH BREVE */
    { "\xd3\x91", "a" , -1, -1},	/*  U+04D1 - ӑ - CYRILLIC SMALL LETTER A WITH BREVE */
    { "\xd3\x92", "a" , -1, -1},	/*  U+04D2 - Ӓ - CYRILLIC CAPITAL LETTER A WITH DIAERESIS */
    { "\xd3\x93", "a" , -1, -1},	/*  U+04D3 - ӓ - CYRILLIC SMALL LETTER A WITH DIAERESIS */
    { "\xd3\x94", "ae" , -1, -1},	/*  U+04D4 - Ӕ - CYRILLIC CAPITAL LIGATURE A IE */
    { "\xd3\x95", "ae" , -1, -1},	/*  U+04D5 - ӕ - CYRILLIC SMALL LIGATURE A IE */
    { "\xd3\x96", "e" , -1, -1},	/*  U+04D6 - Ӗ - CYRILLIC CAPITAL LETTER IE WITH BREVE */
    { "\xd3\x97", "e" , -1, -1},	/*  U+04D7 - ӗ - CYRILLIC SMALL LETTER IE WITH BREVE */
    { "\xd3\x98", "e" , -1, -1},	/*  U+04D8 - Ә - CYRILLIC CAPITAL LETTER SCHWA */
    { "\xd3\x99", "e" , -1, -1},	/*  U+04D9 - ә - CYRILLIC SMALL LETTER SCHWA */
    { "\xd3\x9a", "e" , -1, -1},	/*  U+04DA - Ӛ - CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS */
    { "\xd3\x9b", "e" , -1, -1},	/*  U+04DB - ӛ - CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS */
    { "\xd3\x9c", "x" , -1, -1},	/*  U+04DC - Ӝ - CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS */
    { "\xd3\x9d", "x" , -1, -1},	/*  U+04DD - ӝ - CYRILLIC SMALL LETTER ZHE WITH DIAERESIS */
    { "\xd3\x9e", "3" , -1, -1},	/*  U+04DE - Ӟ - CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS */
    { "\xd3\x9f", "3" , -1, -1},	/*  U+04DF - ӟ - CYRILLIC SMALL LETTER ZE WITH DIAERESIS */
    { "\xd3\xa0", "3" , -1, -1},	/*  U+04E0 - Ӡ - CYRILLIC CAPITAL LETTER ABKHASIAN DZE */
    { "\xd3\xa1", "3" , -1, -1},	/*  U+04E1 - ӡ - CYRILLIC SMALL LETTER ABKHASIAN DZE */
    { "\xd3\xa2", "n" , -1, -1},	/*  U+04E2 - Ӣ - CYRILLIC CAPITAL LETTER I WITH MACRON */
    { "\xd3\xa3", "n" , -1, -1},	/*  U+04E3 - ӣ - CYRILLIC SMALL LETTER I WITH MACRON */
    { "\xd3\xa4", "n" , -1, -1},	/*  U+04E4 - Ӥ - CYRILLIC CAPITAL LETTER I WITH DIAERESIS */
    { "\xd3\xa5", "n" , -1, -1},	/*  U+04E5 - ӥ - CYRILLIC SMALL LETTER I WITH DIAERESIS */
    { "\xd3\xa6", "o" , -1, -1},	/*  U+04E6 - Ӧ - CYRILLIC CAPITAL LETTER O WITH DIAERESIS */
    { "\xd3\xa7", "o" , -1, -1},	/*  U+04E7 - ӧ - CYRILLIC SMALL LETTER O WITH DIAERESIS */
    { "\xd3\xa8", "0" , -1, -1},	/*  U+04E8 - Ө - CYRILLIC CAPITAL LETTER BARRED O */
    { "\xd3\xa9", "0" , -1, -1},	/*  U+04E9 - ө - CYRILLIC SMALL LETTER BARRED O */
    { "\xd3\xaa", "0" , -1, -1},	/*  U+04EA - Ӫ - CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS */
    { "\xd3\xab", "0" , -1, -1},	/*  U+04EB - ӫ - CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS */
    { "\xd3\xac", "3" , -1, -1},	/*  U+04EC - Ӭ - CYRILLIC CAPITAL LETTER E WITH DIAERESIS */
    { "\xd3\xad", "3" , -1, -1},	/*  U+04ED - ӭ - CYRILLIC SMALL LETTER E WITH DIAERESIS */
    { "\xd3\xae", "y" , -1, -1},	/*  U+04EE - Ӯ - CYRILLIC CAPITAL LETTER U WITH MACRON */
    { "\xd3\xaf", "y" , -1, -1},	/*  U+04EF - ӯ - CYRILLIC SMALL LETTER U WITH MACRON */
    { "\xd3\xb0", "y" , -1, -1},	/*  U+04F0 - Ӱ - CYRILLIC CAPITAL LETTER U WITH DIAERESIS */
    { "\xd3\xb1", "y" , -1, -1},	/*  U+04F1 - ӱ - CYRILLIC SMALL LETTER U WITH DIAERESIS */
    { "\xd3\xb2", "y" , -1, -1},	/*  U+04F2 - Ӳ - CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE */
    { "\xd3\xb3", "y" , -1, -1},	/*  U+04F3 - ӳ - CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE */
    { "\xd3\xb4", "y" , -1, -1},	/*  U+04F4 - Ӵ - CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS */
    { "\xd3\xb5", "y" , -1, -1},	/*  U+04F5 - ӵ - CYRILLIC SMALL LETTER CHE WITH DIAERESIS */
    { "\xd3\xb6", "r" , -1, -1},	/*  U+04F6 - Ӷ - CYRILLIC CAPITAL LETTER GHE WITH DESCENDER */
    { "\xd3\xb7", "r" , -1, -1},	/*  U+04F7 - ӷ - CYRILLIC SMALL LETTER GHE WITH DESCENDER */
    { "\xd3\xb8", "bl" , -1, -1},	/*  U+04F8 - Ӹ - CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS */
    { "\xd3\xb9", "bl" , -1, -1},	/*  U+04F9 - ӹ - CYRILLIC SMALL LETTER YERU WITH DIAERESIS */
    { "\xd3\xba", "f" , -1, -1},	/*  U+04FA - Ӻ - CYRILLIC CAPITAL LETTER GHE WITH STROKE AND HOOK */
    { "\xd3\xbb", "f" , -1, -1},	/*  U+04FB - ӻ - CYRILLIC SMALL LETTER GHE WITH STROKE AND HOOK */
    { "\xd3\xbc", "x" , -1, -1},	/*  U+04FC - Ӽ - CYRILLIC CAPITAL LETTER HA WITH HOOK */
    { "\xd3\xbd", "x" , -1, -1},	/*  U+04FD - ӽ - CYRILLIC SMALL LETTER HA WITH HOOK */
    { "\xd3\xbe", "x" , -1, -1},	/*  U+04FE - Ӿ - CYRILLIC CAPITAL LETTER HA WITH STROKE */
    { "\xd3\xbf", "x" , -1, -1},	/*  U+04FF - ӿ - CYRILLIC SMALL LETTER HA WITH STROKE */
    { "\xd4\x80", "d" , -1, -1},	/*  U+0500 - Ԁ - CYRILLIC CAPITAL LETTER KOMI DE */
    { "\xd4\x81", "d" , -1, -1},	/*  U+0501 - ԁ - CYRILLIC SMALL LETTER KOMI DE */
    { "\xd4\x82", "du" , -1, -1},	/*  U+0502 - Ԃ - CYRILLIC CAPITAL LETTER KOMI DJE */
    { "\xd4\x83", "du" , -1, -1},	/*  U+0503 - ԃ - CYRILLIC SMALL LETTER KOMI DJE */
    { "\xd4\x84", "r" , -1, -1},	/*  U+0504 - Ԅ - CYRILLIC CAPITAL LETTER KOMI ZJE */
    { "\xd4\x85", "r" , -1, -1},	/*  U+0505 - ԅ - CYRILLIC SMALL LETTER KOMI ZJE */
    { "\xd4\x86", "r" , -1, -1},	/*  U+0506 - Ԇ - CYRILLIC CAPITAL LETTER KOMI DZJE */
    { "\xd4\x87", "r" , -1, -1},	/*  U+0507 - ԇ - CYRILLIC SMALL LETTER KOMI DZJE */
    { "\xd4\x88", "ju" , -1, -1},	/*  U+0508 - Ԉ - CYRILLIC CAPITAL LETTER KOMI LJE */
    { "\xd4\x89", "ju" , -1, -1},	/*  U+0509 - ԉ - CYRILLIC SMALL LETTER KOMI LJE */
    { "\xd4\x8a", "hu" , -1, -1},	/*  U+050A - Ԋ - CYRILLIC CAPITAL LETTER KOMI NJE */
    { "\xd4\x8b", "hu" , -1, -1},	/*  U+050B - ԋ - CYRILLIC SMALL LETTER KOMI NJE */
    { "\xd4\x8c", "g" , -1, -1},	/*  U+050C - Ԍ - CYRILLIC CAPITAL LETTER KOMI SJE */
    { "\xd4\x8d", "g" , -1, -1},	/*  U+050D - ԍ - CYRILLIC SMALL LETTER KOMI SJE */
    { "\xd4\x8e", "tj" , -1, -1},	/*  U+050E - Ԏ - CYRILLIC CAPITAL LETTER KOMI TJE */
    { "\xd4\x8f", "tj" , -1, -1},	/*  U+050F - ԏ - CYRILLIC SMALL LETTER KOMI TJE */
    { "\xd4\x90", "e" , -1, -1},	/*  U+0510 - Ԑ - CYRILLIC CAPITAL LETTER REVERSED ZE */
    { "\xd4\x91", "e" , -1, -1},	/*  U+0511 - ԑ - CYRILLIC SMALL LETTER REVERSED ZE */
    { "\xd4\x92", "n" , -1, -1},	/*  U+0512 - Ԓ - CYRILLIC CAPITAL LETTER EL WITH HOOK */
    { "\xd4\x93", "n" , -1, -1},	/*  U+0513 - ԓ - CYRILLIC SMALL LETTER EL WITH HOOK */
    { "\xd4\x94", "x" , -1, -1},	/*  U+0514 - Ԕ - CYRILLIC CAPITAL LETTER LHA */
    { "\xd4\x95", "x" , -1, -1},	/*  U+0515 - ԕ - CYRILLIC SMALL LETTER LHA */
    { "\xd4\x96", "pk" , -1, -1},	/*  U+0516 - Ԗ - CYRILLIC CAPITAL LETTER RHA */
    { "\xd4\x97", "pk" , -1, -1},	/*  U+0517 - ԗ - CYRILLIC SMALL LETTER RHA */
    { "\xd4\x98", "re" , -1, -1},	/*  U+0518 - Ԙ - CYRILLIC CAPITAL LETTER YAE */
    { "\xd4\x99", "re" , -1, -1},	/*  U+0519 - ԙ - CYRILLIC SMALL LETTER YAE */
    { "\xd4\x9a", "q" , -1, -1},	/*  U+051A - Ԛ - CYRILLIC CAPITAL LETTER QA */
    { "\xd4\x9b", "q" , -1, -1},	/*  U+051B - ԛ - CYRILLIC SMALL LETTER QA */
    { "\xd4\x9c", "w" , -1, -1},	/*  U+051C - Ԝ - CYRILLIC CAPITAL LETTER WE */
    { "\xd4\x9d", "w" , -1, -1},	/*  U+051D - ԝ - CYRILLIC SMALL LETTER WE */
    { "\xd4\x9e", "kx" , -1, -1},	/*  U+051E - Ԟ - CYRILLIC CAPITAL LETTER ALEUT KA */
    { "\xd4\x9f", "kx" , -1, -1},	/*  U+051F - ԟ - CYRILLIC SMALL LETTER ALEUT KA */
    { "\xd4\xa0", "th" , -1, -1},	/*  U+0520 - Ԡ - CYRILLIC CAPITAL LETTER EL WITH MIDDLE HOOK */
    { "\xd4\xa1", "th" , -1, -1},	/*  U+0521 - ԡ - CYRILLIC SMALL LETTER EL WITH MIDDLE HOOK */
    { "\xd4\xa2", "hh" , -1, -1},	/*  U+0522 - Ԣ - CYRILLIC CAPITAL LETTER EN WITH MIDDLE HOOK */
    { "\xd4\xa3", "hh" , -1, -1},	/*  U+0523 - ԣ - CYRILLIC SMALL LETTER EN WITH MIDDLE HOOK */
    { "\xd4\xa4", "n" , -1, -1},	/*  U+0524 - Ԥ - CYRILLIC CAPITAL LETTER PE WITH DESCENDER */
    { "\xd4\xa5", "n" , -1, -1},	/*  U+0525 - ԥ - CYRILLIC SMALL LETTER PE WITH DESCENDER */
    { "\xd4\xa6", "h" , -1, -1},	/*  U+0526 - Ԧ - CYRILLIC CAPITAL LETTER SHHA WITH DESCENDER */
    { "\xd4\xa7", "h" , -1, -1},	/*  U+0527 - ԧ - CYRILLIC SMALL LETTER SHHA WITH DESCENDER */
    { "\xd4\xa8", "h" , -1, -1},	/*  U+0528 - Ԩ - CYRILLIC CAPITAL LETTER EN WITH LEFT HOOK */
    { "\xd4\xa9", "h" , -1, -1},	/*  U+0529 - ԩ - CYRILLIC SMALL LETTER EN WITH LEFT HOOK */
    { "\xd4\xaa", "ak" , -1, -1},	/*  U+052A - Ԫ - CYRILLIC CAPITAL LETTER DZZHE */
    { "\xd4\xab", "ak" , -1, -1},	/*  U+052B - ԫ - CYRILLIC SMALL LETTER DZZHE */
    { "\xd4\xac", "yk" , -1, -1},	/*  U+052C - Ԭ - CYRILLIC CAPITAL LETTER DCHE */
    { "\xd4\xad", "ya" , -1, -1},	/*  U+052D - ԭ - CYRILLIC SMALL LETTER DCHE */
    { "\xd4\xae", "n" , -1, -1},	/*  U+052E - Ԯ - CYRILLIC CAPITAL LETTER EL WITH DESCENDER */
    { "\xd4\xaf", "n" , -1, -1},	/*  U+052F - ԯ - CYRILLIC SMALL LETTER EL WITH DESCENDER */
    { "\xd4\xb0", "" , -1, -1},		/*  U+0530 - ԰ -  */
    { "\xd4\xb1", "u" , -1, -1},	/*  U+0531 - Ա - ARMENIAN CAPITAL LETTER AYB */
    { "\xd4\xb2", "f" , -1, -1},	/*  U+0532 - Բ - ARMENIAN CAPITAL LETTER BEN */
    { "\xd4\xb3", "q" , -1, -1},	/*  U+0533 - Գ - ARMENIAN CAPITAL LETTER GIM */
    { "\xd4\xb4", "r" , -1, -1},	/*  U+0534 - Դ - ARMENIAN CAPITAL LETTER DA */
    { "\xd4\xb5", "t" , -1, -1},	/*  U+0535 - Ե - ARMENIAN CAPITAL LETTER ECH */
    { "\xd4\xb6", "on" , -1, -1},	/*  U+0536 - Զ - ARMENIAN CAPITAL LETTER ZA */
    { "\xd4\xb7", "t" , -1, -1},	/*  U+0537 - Է - ARMENIAN CAPITAL LETTER EH */
    { "\xd4\xb8", "r" , -1, -1},	/*  U+0538 - Ը - ARMENIAN CAPITAL LETTER ET */
    { "\xd4\xb9", "p" , -1, -1},	/*  U+0539 - Թ - ARMENIAN CAPITAL LETTER TO */
    { "\xd4\xba", "d" , -1, -1},	/*  U+053A - Ժ - ARMENIAN CAPITAL LETTER ZHE */
    { "\xd4\xbb", "r" , -1, -1},	/*  U+053B - Ի - ARMENIAN CAPITAL LETTER INI */
    { "\xd4\xbc", "l" , -1, -1},	/*  U+053C - Լ - ARMENIAN CAPITAL LETTER LIWN */
    { "\xd4\xbd", "tu" , -1, -1},	/*  U+053D - Խ - ARMENIAN CAPITAL LETTER XEH */
    { "\xd4\xbe", "y" , -1, -1},	/*  U+053E - Ծ - ARMENIAN CAPITAL LETTER CA */
    { "\xd4\xbf", "y" , -1, -1},	/*  U+053F - Կ - ARMENIAN CAPITAL LETTER KEN */
    { "\xd5\x80", "c" , -1, -1},	/*  U+0540 - Հ - ARMENIAN CAPITAL LETTER HO */
    { "\xd5\x81", "g" , -1, -1},	/*  U+0541 - Ձ - ARMENIAN CAPITAL LETTER JA */
    { "\xd5\x82", "r" , -1, -1},	/*  U+0542 - Ղ - ARMENIAN CAPITAL LETTER GHAD */
    { "\xd5\x83", "o" , -1, -1},	/*  U+0543 - Ճ - ARMENIAN CAPITAL LETTER CHEH */
    { "\xd5\x84", "u" , -1, -1},	/*  U+0544 - Մ - ARMENIAN CAPITAL LETTER MEN */
    { "\xd5\x85", "3" , -1, -1},	/*  U+0545 - Յ - ARMENIAN CAPITAL LETTER YI */
    { "\xd5\x86", "u" , -1, -1},	/*  U+0546 - Ն - ARMENIAN CAPITAL LETTER NOW */
    { "\xd5\x87", "t" , -1, -1},	/*  U+0547 - Շ - ARMENIAN CAPITAL LETTER SHA */
    { "\xd5\x88", "n" , -1, -1},	/*  U+0548 - Ո - ARMENIAN CAPITAL LETTER VO */
    { "\xd5\x89", "o" , -1, -1},	/*  U+0549 - Չ - ARMENIAN CAPITAL LETTER CHA */
    { "\xd5\x8a", "m" , -1, -1},	/*  U+054A - Պ - ARMENIAN CAPITAL LETTER PEH */
    { "\xd5\x8b", "o" , -1, -1},	/*  U+054B - Ջ - ARMENIAN CAPITAL LETTER JHEH */
    { "\xd5\x8c", "n" , -1, -1},	/*  U+054C - Ռ - ARMENIAN CAPITAL LETTER RA */
    { "\xd5\x8d", "u" , -1, -1},	/*  U+054D - Ս - ARMENIAN CAPITAL LETTER SEH */
    { "\xd5\x8e", "y" , -1, -1},	/*  U+054E - Վ - ARMENIAN CAPITAL LETTER VEW */
    { "\xd5\x8f", "s" , -1, -1},	/*  U+054F - Տ - ARMENIAN CAPITAL LETTER TIWN */
    { "\xd5\x90", "r" , -1, -1},	/*  U+0550 - Ր - ARMENIAN CAPITAL LETTER REH */
    { "\xd5\x91", "8" , -1, -1},	/*  U+0551 - Ց - ARMENIAN CAPITAL LETTER CO */
    { "\xd5\x92", "r" , -1, -1},	/*  U+0552 - Ւ - ARMENIAN CAPITAL LETTER YIWN */
    { "\xd5\x93", "0" , -1, -1},	/*  U+0553 - Փ - ARMENIAN CAPITAL LETTER PIWR */
    { "\xd5\x94", "p" , -1, -1},	/*  U+0554 - Ք - ARMENIAN CAPITAL LETTER KEH */
    { "\xd5\x95", "o" , -1, -1},	/*  U+0555 - Օ - ARMENIAN CAPITAL LETTER OH */
    { "\xd5\x96", "s" , -1, -1},	/*  U+0556 - Ֆ - ARMENIAN CAPITAL LETTER FEH */
    { "\xd5\x97", "" , -1, -1},		/*  U+0557 - ՗ -  */
    { "\xd5\x98", "" , -1, -1},		/*  U+0558 - ՘ -  */
    { "\xd5\x99", "" , -1, -1},		/*  U+0559 - ՙ - ARMENIAN MODIFIER LETTER LEFT HALF RING */
    { "\xd5\x9a", "" , -1, -1},		/*  U+055A - ՚ - ARMENIAN APOSTROPHE */
    { "\xd5\x9b", "" , -1, -1},		/*  U+055B - ՛ - ARMENIAN EMPHASIS MARK */
    { "\xd5\x9c", "" , -1, -1},		/*  U+055C - ՜ - ARMENIAN EXCLAMATION MARK */
    { "\xd5\x9d", "" , -1, -1},		/*  U+055D - ՝ - ARMENIAN COMMA */
    { "\xd5\x9e", "" , -1, -1},		/*  U+055E - ՞ - ARMENIAN QUESTION MARK */
    { "\xd5\x9f", "" , -1, -1},		/*  U+055F - ՟ - ARMENIAN ABBREVIATION MARK */
    { "\xd5\xa0", "" , -1, -1},		/*  U+0560 - ՠ - ARMENIAN SMALL LETTER TURNED AYB */
    { "\xd5\xa1", "w" , -1, -1},	/*  U+0561 - ա - ARMENIAN SMALL LETTER AYB */
    { "\xd5\xa2", "r" , -1, -1},	/*  U+0562 - բ - ARMENIAN SMALL LETTER BEN */
    { "\xd5\xa3", "q" , -1, -1},	/*  U+0563 - գ - ARMENIAN SMALL LETTER GIM */
    { "\xd5\xa4", "n" , -1, -1},	/*  U+0564 - դ - ARMENIAN SMALL LETTER DA */
    { "\xd5\xa5", "t" , -1, -1},	/*  U+0565 - ե - ARMENIAN SMALL LETTER ECH */
    { "\xd5\xa6", "q" , -1, -1},	/*  U+0566 - զ - ARMENIAN SMALL LETTER ZA */
    { "\xd5\xa7", "t" , -1, -1},	/*  U+0567 - է - ARMENIAN SMALL LETTER EH */
    { "\xd5\xa8", "n" , -1, -1},	/*  U+0568 - ը - ARMENIAN SMALL LETTER ET */
    { "\xd5\xa9", "p" , -1, -1},	/*  U+0569 - թ - ARMENIAN SMALL LETTER TO */
    { "\xd5\xaa", "d" , -1, -1},	/*  U+056A - ժ - ARMENIAN SMALL LETTER ZHE */
    { "\xd5\xab", "h" , -1, -1},	/*  U+056B - ի - ARMENIAN SMALL LETTER INI */
    { "\xd5\xac", "l" , -1, -1},	/*  U+056C - լ - ARMENIAN SMALL LETTER LIWN */
    { "\xd5\xad", "hu" , -1, -1},	/*  U+056D - խ - ARMENIAN SMALL LETTER XEH */
    { "\xd5\xae", "o" , -1, -1},	/*  U+056E - ծ - ARMENIAN SMALL LETTER CA */
    { "\xd5\xaf", "u" , -1, -1},	/*  U+056F - կ - ARMENIAN SMALL LETTER KEN */
    { "\xd5\xb0", "g" , -1, -1},	/*  U+0570 - հ - ARMENIAN SMALL LETTER HO */
    { "\xd5\xb1", "d" , -1, -1},	/*  U+0571 - ձ - ARMENIAN SMALL LETTER JA */
    { "\xd5\xb2", "n" , -1, -1},	/*  U+0572 - ղ - ARMENIAN SMALL LETTER GHAD */
    { "\xd5\xb3", "6" , -1, -1},	/*  U+0573 - ճ - ARMENIAN SMALL LETTER CHEH */
    { "\xd5\xb4", "u" , -1, -1},	/*  U+0574 - մ - ARMENIAN SMALL LETTER MEN */
    { "\xd5\xb5", "k" , -1, -1},	/*  U+0575 - յ - ARMENIAN SMALL LETTER YI */
    { "\xd5\xb6", "u" , -1, -1},	/*  U+0576 - ն - ARMENIAN SMALL LETTER NOW */
    { "\xd5\xb7", "2" , -1, -1},	/*  U+0577 - շ - ARMENIAN SMALL LETTER SHA */
    { "\xd5\xb8", "n" , -1, -1},	/*  U+0578 - ո - ARMENIAN SMALL LETTER VO */
    { "\xd5\xb9", "" , -1, -1},		/*  U+0579 - չ - ARMENIAN SMALL LETTER CHA */
    { "\xd5\xba", "w" , -1, -1},	/*  U+057A - պ - ARMENIAN SMALL LETTER PEH */
    { "\xd5\xbb", "q" , -1, -1},	/*  U+057B - ջ - ARMENIAN SMALL LETTER JHEH */
    { "\xd5\xbc", "n" , -1, -1},	/*  U+057C - ռ - ARMENIAN SMALL LETTER RA */
    { "\xd5\xbd", "u" , -1, -1},	/*  U+057D - ս - ARMENIAN SMALL LETTER SEH */
    { "\xd5\xbe", "u" , -1, -1},	/*  U+057E - վ - ARMENIAN SMALL LETTER VEW */
    { "\xd5\xbf", "un" , -1, -1},	/*  U+057F - տ - ARMENIAN SMALL LETTER TIWN */
    { "\xd6\x80", "n" , -1, -1},	/*  U+0580 - ր - ARMENIAN SMALL LETTER REH */
    { "\xd6\x81", "g" , -1, -1},	/*  U+0581 - ց - ARMENIAN SMALL LETTER CO */
    { "\xd6\x82", "l" , -1, -1},	/*  U+0582 - ւ - ARMENIAN SMALL LETTER YIWN */
    { "\xd6\x83", "yh" , -1, -1},	/*  U+0583 - փ - ARMENIAN SMALL LETTER PIWR */
    { "\xd6\x84", "p" , -1, -1},	/*  U+0584 - ք - ARMENIAN SMALL LETTER KEH */
    { "\xd6\x85", "o" , -1, -1},	/*  U+0585 - օ - ARMENIAN SMALL LETTER OH */
    { "\xd6\x86", "s" , -1, -1},	/*  U+0586 - ֆ - ARMENIAN SMALL LETTER FEH */
    { "\xd6\x87", "u" , -1, -1},	/*  U+0587 - և - ARMENIAN SMALL LIGATURE ECH YIWN */
    { "\xd6\x88", "" , -1, -1},		/*  U+0588 - ֈ - ARMENIAN SMALL LETTER YI WITH STROKE */
    { "\xd6\x89", "" , -1, -1},		/*  U+0589 - ։ - ARMENIAN FULL STOP */
    { "\xd6\x8a", "" , -1, -1},		/*  U+058A - ֊ - ARMENIAN HYPHEN */
    { "\xd6\x8b", "" , -1, -1},		/*  U+058B - ֋ -  */
    { "\xd6\x8c", "" , -1, -1},		/*  U+058C - ֌ -  */
    { "\xd6\x8d", "" , -1, -1},		/*  U+058D - ֍ - RIGHT-FACING ARMENIAN ETERNITY SIGN */
    { "\xd6\x8e", "" , -1, -1},		/*  U+058E - ֎ - LEFT-FACING ARMENIAN ETERNITY SIGN */
    { "\xd6\x8f", "" , -1, -1},		/*  U+058F - ֏ - ARMENIAN DRAM SIGN */

    /* more UTF-8 table entries can be added here */

    {NULL, NULL, 0, 0}		/* MUST BE LAST */
};


/*
 * random state info
 */
#define STATE_LEN (256)		/* size of random() state machine */
#define RANDOM_VAL_LEN (8)	/* hex characters produced by 31-bit random() values */

/*
 * file variables
 */
static bool utf8_posix_map_checked = false;	/* true ==> check_utf8_posix_map() run was successfully */
static bool seeded = false;			/* true ==> default_handle() was seeded for random() */
static char state[STATE_LEN+1];			/* random() state */


/*
 * check_utf8_posix_map	- fill in string lengths and sanity check hmap[]
 *
 * This function verifies that the only NULL element in the table is the very
 * last element: if it's not there or there's another NULL element it's a
 * problem that has to be fixed. It also checks that the table is not empty and
 * additionally it sets the length of each utf8_str and posix_str in the table.
 */
void
check_utf8_posix_map(void)
{
    size_t max = 0;		/* number of elements in hmap[] */
    size_t i;

    /*
     * firewall - check if we already ran to completion
     */
    max = TBLLEN(hmap);
    if (utf8_posix_map_checked == true) {
	dbg(DBG_VVVHIGH, "hmap[0..%ju] was already setup, returning", (uintmax_t)(max-1));
	return;
    }

    /*
     * check hmap[] for non-NULL through next to last table entry
     * and fill in table lengths
     */
    if (max <= 0) {
	err(10, __func__, "bogus hmap length: %ju <= 0", (uintmax_t)max);
	not_reached();
    }
    for (i = 0; i < max - 1; ++i) {

	/* non-NULL check */
	if (hmap[i].utf8_str == NULL) {
	    err(11, __func__, "found utf8_str NULL pointer not at end (hmap[%ju]) of hmap[%ju]; "
			      "fix table in %s and recompile",
			      (uintmax_t)max, (uintmax_t)i, __FILE__);
	    not_reached();
	}
	if (hmap[i].posix_str == NULL) {
	    err(12, __func__, "found posix_str NULL pointer not at end (hmap[%ju]) of hmap[%ju]; "
			      "fix table in %s and recompile",
			      (uintmax_t)max, (uintmax_t)i, __FILE__);
	    not_reached();
	}

	/* fill in string lengths */
	hmap[i].utf8_str_len = (int)strlen(hmap[i].utf8_str);
	hmap[i].posix_str_len = (int)strlen(hmap[i].posix_str);

	/* POSIX portable plus + check on posix_str if string is not empty */
	if (hmap[i].posix_str_len > 0 && posix_plus_safe(hmap[i].posix_str, true, false, false) == false) {
	    err(13, __func__, "hmap[%ju] = '%s' is not POSIX portable plus + safe; "
			      "fix table in %s and recompile", (uintmax_t)i, hmap[i].posix_str, __FILE__);
	    not_reached();
	}
    }

    /*
     * check final NULL at end of table
     */
    if (hmap[max-1].utf8_str != NULL || hmap[max-1].posix_str != NULL) {
	err(14, __func__, "no final NULL element at hmap[%ju]; fix table in %s and recompile", (uintmax_t)(max-1), __FILE__);
	not_reached();
    }
    utf8_posix_map_checked = true;
    dbg(DBG_VVHIGH, "hmap[0..%ju] sane and ready in %s", (uintmax_t)(max-1), __FILE__);
    return;
}


/*
 * default_handle - compute a default handle given name
 *
 * A default author handle attempted to be computed from the author name, using
 * the utf8_posix_map hmap[] to translate certain UTF-8 strings in the name
 * into POSIX+ safe strings.  If for some reason the translation results in
 * an empty string, a string using the author_num, entry_num and ioccc_id
 * will be returned.  If the translation results in a string that would
 * be too long, the string will be truncated.
 *
 * In all cases the return will be a non-NULL allocated string that
 * is NUL terminated, not empty, and consisting of only POSIX portable
 * filename and + chars.
 *
 * given:
 *	name		- name of the author
 *
 * returns:
 *	allocated default handle
 *
 * Does not return on error nor NULL pointers nor invalid args.
 */
char *
default_handle(char const *name)
{
    size_t def_len = 0;		/* default handle length */
    size_t namelen = 0;		/* length of name */
    char *ret = NULL;		/* calloc string to return */
    struct utf8_posix_map *m;	/* pointer into hmap[] table */
    bool safe = false;		/* true ==> default handle has safe characters */
    size_t cur_len = 0;		/* current default handle length that is bring formed */
    size_t len = 0;		/* string length of computed default handle */
    int cret;			/* gettimeofday() or snprintf() return value */
    char *pret;			/* strncpy() return */
    size_t i;

    /*
     * firewall
     */
    if (name == NULL) {
	err(15, __func__, "passed NULL arg");
	not_reached();
    }
    namelen = strlen(name);
    if (namelen <= 0) {
	err(16, __func__, "empty name");
	not_reached();
    }

    /*
     * setup hmap[] - OK to call if already set up
     */
    check_utf8_posix_map();

    /*
     * determine length of default handle
     */
    def_len = 0;
    i = 0;
    while (i < namelen) {

	/*
	 * search for a hmap[] match
	 */
	for (m=hmap; m->utf8_str != NULL; ++m) {

	    /* skip special cases */
	    if (m->utf8_str_len <= 0) {
		continue;
	    }

	    /* skip if there is not a posix_str for this entry */
	    if (m->posix_str == NULL) {
		continue;
	    }

	    /* skip if map entry is too long for remainder of string */
	    if (i + (uintmax_t)m->utf8_str_len > namelen) {
		continue;
	    }

	    /* skip if there isn't match with the rest of the string */
	    if (strncasecmp(m->utf8_str, name+i, (uintmax_t)m->utf8_str_len) != 0) {
		continue;
	    }

	    /* match found: add to default handle length */
	    def_len += (uintmax_t)m->posix_str_len;

	    /*
	     * advance string position
	     */
	    i += (uintmax_t)m->utf8_str_len;

	    /* stop hmap[] scan on this match */
	    break;
	}

	/*
	 * if no hmap[] match was found, skip this string byte
	 */
        if (m->utf8_str == NULL) {
	    ++i;
	}
    }
    dbg(DBG_VVHIGH, "estimated default handle <%s> length: %ju", name, (uintmax_t)def_len);

    /*
     * case: estimated default handle is empty
     *
     * If the hmap[] translation of name would be empty, then
     * for a allocated string of hex characters our of 3 calls random().
     */
    if (def_len <= 0) {

	long a;		/* first call to random() */
	long b;		/* second call to random() */
	long c;		/* third call to random() */

        /*
	 * NOTE: We do not need a very strong, let alone, cryptographically
	 *	 sound, random number generator.  We just need to generate
	 *	 some characters that are unlikely to be pseudo-randomly
	 *	 picked by another user.  Even if there happens to be a
	 *	 match, this is OK as the IOCCC judges can sort it out
	 *	 when it comes time to judge entries.  We just need a
	 *	 somewhat reasonable j-random string in the event
	 *	 that we cannot use a translation of the name.
	 */

	/*
	 * if needed, first seed our PRNG
	 */
	if (seeded == false) {
	    struct timeval tp;	/* the time, now */
	    uintmax_t tmp;	/* temp value from time for xor folding */
	    char *iret;		/* initstate() return value */

	    /*
	     * determine now
	     */
	    errno = 0;		/* pre-clear errno for errp() */
	    cret = gettimeofday(&tp, NULL);
	    if (cret < 0) {
		errp(17, __func__, "gettimeofday failed");
		not_reached();
	    }

	    /*
	     * initialize our random state from stuff that hopefully is somewhat variable
	     */
	    tmp = (uintmax_t)tp.tv_sec + (uintmax_t)tp.tv_usec + (uintmax_t)getpid() + (uintmax_t)getppid();
	    errno = 0;		/* pre-clear errno for errp() */
	    iret = initstate((unsigned)tmp, state, STATE_LEN);
	    if (iret == NULL) {
		errp(18, __func__, "srandom failed");
		not_reached();
	    }

	    /*
	     * note that we are now seeded
	     */
	    seeded = true;
	}

	/*
	 * obtain our 3 random() values
	 */
	a = random();
	b = random();
	c = random();

	/*
	 * calloc the random default author handle
	 */
	def_len = LITLEN("jrandom+") + 4*RANDOM_VAL_LEN;
	errno = 0;		/* pre-clear errno for errp() */
	ret = calloc(def_len+1, sizeof(char));
	if (ret == NULL) {
	    errp(19, __func__, "calloc failed for %ju bytes", (uintmax_t)(def_len+1));
	    not_reached();
	}

	/*
	 * format the random handle
	 */
	cret = snprintf(ret, def_len, "jrandom+%08lx%08lx%08lx", a, b, c);
	if (cret <= 0) {
	    errp(20, __func__, "snprintf failed, returned: %d", cret);
	    not_reached();
	}

    /*
     * case: we can translate into a default random handle
     */
    } else {

	/*
	 * calloc the default default author handle
	 */
	errno = 0;		/* pre-clear errno for errp() */
	ret = calloc(def_len+1, sizeof(char));
	if (ret == NULL) {
	    errp(21, __func__, "calloc failed for %ju bytes", (uintmax_t)(def_len+1));
	    not_reached();
	}

	/*
	 * form default handle from hmap[] translations
	 */
	cur_len = 0;
	i = 0;
	while (i < namelen) {

	    /*
	     * search for a hmap[] match
	     */
	    for (m=hmap; m->utf8_str != NULL; ++m) {

		/* skip special cases */
		if (m->utf8_str_len <= 0) {
		    continue;
		}

		/* skip if there is not a posix_str for this entry */
		if (m->posix_str == NULL) {
		    continue;
		}

		/* skip if map entry is too long for remainder of string */
		if (i + (uintmax_t)m->utf8_str_len > namelen) {
		    continue;
		}

		/* skip if there isn't a match with the rest of the string */
		if (strncasecmp(m->utf8_str, name+i, (uintmax_t)m->utf8_str_len) != 0) {
		    continue;
		}

		/*
		 * match found: copy translated chars to handle, if there are any
		 */
		if (m->posix_str_len > 0) {

		    /*
		     * firewall - do not copy beyond end of allocated buffer
		     */
		    if (cur_len + (uintmax_t)m->posix_str_len > def_len) {
			err(22, __func__, "attempt to copy to buf[%ju] %ju bytes: would go beyond allocated len: %ju",
					   (uintmax_t)cur_len, (uintmax_t)m->posix_str_len, (uintmax_t)def_len);
			not_reached();
		    }

		    /*
		     * copy translation into the default author handle buffer
		     */
		    errno = 0;		/* pre-clear errno for errp() */
		    pret = strncpy(ret+cur_len, m->posix_str, (uintmax_t)m->posix_str_len);
		    if (pret == NULL) {
			errp(23, __func__, "strncpy() returned NULL");
			not_reached();
		    }

		    /*
		     * advance our copy position
		     */
		    cur_len += (uintmax_t)m->posix_str_len;
		}

		/*
		 * advance our string position
		 */
		i += (uintmax_t)m->utf8_str_len;

		/* stop hmap[] scan on this match */
		break;
	    }

	    /*
	     * if no hmap[] match was found, skip this string byte
	     */
	    if (m->utf8_str == NULL) {
		++i;
	    }
	}
	ret[cur_len] = '\0';	/* paranoia */
	dbg(DBG_VVHIGH, "current default handle length: %ju", (uintmax_t)cur_len);
	dbg(DBG_VHIGH, "current computed default handle: <%s>", ret);

	/*
	 * truncate translated default author handle if needed
	 */
	if (cur_len > MAX_HANDLE) {
	    cur_len = MAX_HANDLE;
	    ret[cur_len] = '\0';
	    dbg(DBG_VVHIGH, "default handle truncated to length: %ju", (uintmax_t)cur_len);
	}
    }
    dbg(DBG_VHIGH, "computed default handle: <%s>", ret);

    /*
     * sanity check: default author handle cannot be empty
     */
    len = strlen(ret);
    if (len <= 0) {
	err(24, __func__, "default author handle length: %ju <= 0", (uintmax_t)len);
	not_reached();
    }
    dbg(DBG_VVHIGH, "actual default handle length: %ju", (uintmax_t)len);

    /*
     * sanity check: default author handle cannot be too long
     */
    if (len > MAX_HANDLE) {
	err(25, __func__, "default author handle length: %ju > MAX_HANDLE: %d",
			   (uintmax_t)len, MAX_HANDLE);
	not_reached();
    }

    /*
     * sanity check: default author handle must have only POSIX portable safe
     * plus + chars
     */
    safe = posix_plus_safe(ret, true, false, true);
    if (safe == false) {
	err(26, __func__, "default author handle contains unsafe chars: <%s>", ret);
	not_reached();
    }

    /*
     * return default author handle
     */
    dbg(DBG_HIGH, "returning default handle: <%s>", ret);
    return ret;
}
