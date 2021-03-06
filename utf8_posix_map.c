/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
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
/*ooo*/ /* exit code out of numerical order - ignore in sequencing */
/*coo*/ /* exit code change of order - use new value in sequencing */


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
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
    { "\xc2\x80", "" , -1, -1},		/*  U+0080 - ?? - <control> */
    { "\xc2\x81", "" , -1, -1},		/*  U+0081 - ?? - <control> */
    { "\xc2\x82", "" , -1, -1},		/*  U+0082 - ?? - <control> */
    { "\xc2\x83", "" , -1, -1},		/*  U+0083 - ?? - <control> */
    { "\xc2\x84", "" , -1, -1},		/*  U+0084 - ?? - <control> */
    { "\xc2\x85", "" , -1, -1},		/*  U+0085 - ?? - <control> */
    { "\xc2\x86", "" , -1, -1},		/*  U+0086 - ?? - <control> */
    { "\xc2\x87", "" , -1, -1},		/*  U+0087 - ?? - <control> */
    { "\xc2\x88", "" , -1, -1},		/*  U+0088 - ?? - <control> */
    { "\xc2\x89", "" , -1, -1},		/*  U+0089 - ?? - <control> */
    { "\xc2\x8a", "" , -1, -1},		/*  U+008A - ?? - <control> */
    { "\xc2\x8b", "" , -1, -1},		/*  U+008B - ?? - <control> */
    { "\xc2\x8c", "" , -1, -1},		/*  U+008C - ?? - <control> */
    { "\xc2\x8d", "" , -1, -1},		/*  U+008D - ?? - <control> */
    { "\xc2\x8e", "" , -1, -1},		/*  U+008E - ?? - <control> */
    { "\xc2\x8f", "" , -1, -1},		/*  U+008F - ?? - <control> */
    { "\xc2\x90", "" , -1, -1},		/*  U+0090 - ?? - <control> */
    { "\xc2\x91", "" , -1, -1},		/*  U+0091 - ?? - <control> */
    { "\xc2\x92", "" , -1, -1},		/*  U+0092 - ?? - <control> */
    { "\xc2\x93", "" , -1, -1},		/*  U+0093 - ?? - <control> */
    { "\xc2\x94", "" , -1, -1},		/*  U+0094 - ?? - <control> */
    { "\xc2\x95", "" , -1, -1},		/*  U+0095 - ?? - <control> */
    { "\xc2\x96", "" , -1, -1},		/*  U+0096 - ?? - <control> */
    { "\xc2\x97", "" , -1, -1},		/*  U+0097 - ?? - <control> */
    { "\xc2\x98", "" , -1, -1},		/*  U+0098 - ?? - <control> */
    { "\xc2\x99", "" , -1, -1},		/*  U+0099 - ?? - <control> */
    { "\xc2\x9a", "" , -1, -1},		/*  U+009A - ?? - <control> */
    { "\xc2\x9b", "" , -1, -1},		/*  U+009B - ?? - <control> */
    { "\xc2\x9c", "" , -1, -1},		/*  U+009C - ?? - <control> */
    { "\xc2\x9d", "" , -1, -1},		/*  U+009D - ?? - <control> */
    { "\xc2\x9e", "" , -1, -1},		/*  U+009E - ?? - <control> */
    { "\xc2\x9f", "" , -1, -1},		/*  U+009F - ?? - <control> */
    { "\xc2\xa0", "_" , -1, -1},	/*  U+00A0 - ?? - NO-BREAK SPACE */
    { "\xc2\xa1", "" , -1, -1},		/*  U+00A1 - ?? - INVERTED EXCLAMATION MARK */
    { "\xc2\xa2", "c" , -1, -1},	/*  U+00A2 - ?? - CENT SIGN */
    { "\xc2\xa3", "f" , -1, -1},	/*  U+00A3 - ?? - POUND SIGN */
    { "\xc2\xa4", "o" , -1, -1},	/*  U+00A4 - ?? - CURRENCY SIGN */
    { "\xc2\xa5", "y" , -1, -1},	/*  U+00A5 - ?? - YEN SIGN */
    { "\xc2\xa6", "" , -1, -1},		/*  U+00A6 - ?? - BROKEN BAR */
    { "\xc2\xa7", "s" , -1, -1},	/*  U+00A7 - ?? - SECTION SIGN */
    { "\xc2\xa8", "" , -1, -1},		/*  U+00A8 - ?? - DIAERESIS */
    { "\xc2\xa9", "o" , -1, -1},	/*  U+00A9 - ?? - COPYRIGHT SIGN */
    { "\xc2\xaa", "o" , -1, -1},	/*  U+00AA - ?? - FEMININE ORDINAL INDICATOR */
    { "\xc2\xab", "" , -1, -1},		/*  U+00AB - ?? - LEFT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { "\xc2\xac", "-" , -1, -1},	/*  U+00AC - ?? - NOT SIGN */
    { "\xc2\xad", "-" , -1, -1},	/*  U+00AD - ?? - SOFT HYPHEN */
    { "\xc2\xae", "" , -1, -1},		/*  U+00AE - ?? - REGISTERED SIGN */
    { "\xc2\xaf", "-" , -1, -1},	/*  U+00AF - ?? - MACRON */
    { "\xc2\xb0", "o" , -1, -1},	/*  U+00B0 - ?? - DEGREE SIGN */
    { "\xc2\xb1", "+" , -1, -1},	/*  U+00B1 - ?? - PLUS-MINUS SIGN */
    { "\xc2\xb2", "2" , -1, -1},	/*  U+00B2 - ?? - SUPERSCRIPT TWO */
    { "\xc2\xb3", "3" , -1, -1},	/*  U+00B3 - ?? - SUPERSCRIPT THREE */
    { "\xc2\xb4", "" , -1, -1},		/*  U+00B4 - ?? - ACUTE ACCENT */
    { "\xc2\xb5", "u" , -1, -1},	/*  U+00B5 - ?? - MICRO SIGN */
    { "\xc2\xb6", "p" , -1, -1},	/*  U+00B6 - ?? - PILCROW SIGN */
    { "\xc2\xb7", "." , -1, -1},	/*  U+00B7 - ?? - MIDDLE DOT */
    { "\xc2\xb8", "" , -1, -1},		/*  U+00B8 - ?? - CEDILLA */
    { "\xc2\xb9", "1" , -1, -1},	/*  U+00B9 - ?? - SUPERSCRIPT ONE */
    { "\xc2\xba", "o" , -1, -1},	/*  U+00BA - ?? - MASCULINE ORDINAL INDICATOR */
    { "\xc2\xbb", "" , -1, -1},		/*  U+00BB - ?? - RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK */
    { "\xc2\xbc", "1" , -1, -1},	/*  U+00BC - ?? - VULGAR FRACTION ONE QUARTER */
    { "\xc2\xbd", "2" , -1, -1},	/*  U+00BD - ?? - VULGAR FRACTION ONE HALF */
    { "\xc2\xbe", "3" , -1, -1},	/*  U+00BE - ?? - VULGAR FRACTION THREE QUARTERS */
    { "\xc2\xbf", "" , -1, -1},		/*  U+00BF - ?? - INVERTED QUESTION MARK */
    { "\xc3\x80", "a" , -1, -1},	/*  U+00C0 - ?? - LATIN CAPITAL LETTER A WITH GRAVE */
    { "\xc3\x81", "a" , -1, -1},	/*  U+00C1 - ?? - LATIN CAPITAL LETTER A WITH ACUTE */
    { "\xc3\x82", "a" , -1, -1},	/*  U+00C2 - ?? - LATIN CAPITAL LETTER A WITH CIRCUMFLEX */
    { "\xc3\x83", "a" , -1, -1},	/*  U+00C3 - ?? - LATIN CAPITAL LETTER A WITH TILDE */
    { "\xc3\x84", "a" , -1, -1},	/*  U+00C4 - ?? - LATIN CAPITAL LETTER A WITH DIAERESIS */
    { "\xc3\x85", "a" , -1, -1},	/*  U+00C5 - ?? - LATIN CAPITAL LETTER A WITH RING ABOVE */
    { "\xc3\x86", "ae" , -1, -1},	/*  U+00C6 - ?? - LATIN CAPITAL LETTER AE */
    { "\xc3\x87", "c" , -1, -1},	/*  U+00C7 - ?? - LATIN CAPITAL LETTER C WITH CEDILLA */
    { "\xc3\x88", "e" , -1, -1},	/*  U+00C8 - ?? - LATIN CAPITAL LETTER E WITH GRAVE */
    { "\xc3\x89", "e" , -1, -1},	/*  U+00C9 - ?? - LATIN CAPITAL LETTER E WITH ACUTE */
    { "\xc3\x8a", "e" , -1, -1},	/*  U+00CA - ?? - LATIN CAPITAL LETTER E WITH CIRCUMFLEX */
    { "\xc3\x8b", "e" , -1, -1},	/*  U+00CB - ?? - LATIN CAPITAL LETTER E WITH DIAERESIS */
    { "\xc3\x8c", "i" , -1, -1},	/*  U+00CC - ?? - LATIN CAPITAL LETTER I WITH GRAVE */
    { "\xc3\x8d", "i" , -1, -1},	/*  U+00CD - ?? - LATIN CAPITAL LETTER I WITH ACUTE */
    { "\xc3\x8e", "i" , -1, -1},	/*  U+00CE - ?? - LATIN CAPITAL LETTER I WITH CIRCUMFLEX */
    { "\xc3\x8f", "i" , -1, -1},	/*  U+00CF - ?? - LATIN CAPITAL LETTER I WITH DIAERESIS */
    { "\xc3\x90", "d" , -1, -1},	/*  U+00D0 - ?? - LATIN CAPITAL LETTER ETH */
    { "\xc3\x91", "n" , -1, -1},	/*  U+00D1 - ?? - LATIN CAPITAL LETTER N WITH TILDE */
    { "\xc3\x92", "o" , -1, -1},	/*  U+00D2 - ?? - LATIN CAPITAL LETTER O WITH GRAVE */
    { "\xc3\x93", "o" , -1, -1},	/*  U+00D3 - ?? - LATIN CAPITAL LETTER O WITH ACUTE */
    { "\xc3\x94", "o" , -1, -1},	/*  U+00D4 - ?? - LATIN CAPITAL LETTER O WITH CIRCUMFLEX */
    { "\xc3\x95", "o" , -1, -1},	/*  U+00D5 - ?? - LATIN CAPITAL LETTER O WITH TILDE */
    { "\xc3\x96", "o" , -1, -1},	/*  U+00D6 - ?? - LATIN CAPITAL LETTER O WITH DIAERESIS */
    { "\xc3\x97", "x" , -1, -1},	/*  U+00D7 - ?? - MULTIPLICATION SIGN */
    { "\xc3\x98", "0" , -1, -1},	/*  U+00D8 - ?? - LATIN CAPITAL LETTER O WITH STROKE */
    { "\xc3\x99", "u" , -1, -1},	/*  U+00D9 - ?? - LATIN CAPITAL LETTER U WITH GRAVE */
    { "\xc3\x9a", "u" , -1, -1},	/*  U+00DA - ?? - LATIN CAPITAL LETTER U WITH ACUTE */
    { "\xc3\x9b", "u" , -1, -1},	/*  U+00DB - ?? - LATIN CAPITAL LETTER U WITH CIRCUMFLEX */
    { "\xc3\x9c", "u" , -1, -1},	/*  U+00DC - ?? - LATIN CAPITAL LETTER U WITH DIAERESIS */
    { "\xc3\x9d", "y" , -1, -1},	/*  U+00DD - ?? - LATIN CAPITAL LETTER Y WITH ACUTE */
    { "\xc3\x9e", "p" , -1, -1},	/*  U+00DE - ?? - LATIN CAPITAL LETTER THORN */
    { "\xc3\x9f", "s" , -1, -1},	/*  U+00DF - ?? - LATIN SMALL LETTER SHARP S */
    { "\xc3\xa0", "a" , -1, -1},	/*  U+00E0 - ?? - LATIN SMALL LETTER A WITH GRAVE */
    { "\xc3\xa1", "a" , -1, -1},	/*  U+00E1 - ?? - LATIN SMALL LETTER A WITH ACUTE */
    { "\xc3\xa2", "a" , -1, -1},	/*  U+00E2 - ?? - LATIN SMALL LETTER A WITH CIRCUMFLEX */
    { "\xc3\xa3", "a" , -1, -1},	/*  U+00E3 - ?? - LATIN SMALL LETTER A WITH TILDE */
    { "\xc3\xa4", "a" , -1, -1},	/*  U+00E4 - ?? - LATIN SMALL LETTER A WITH DIAERESIS */
    { "\xc3\xa5", "a" , -1, -1},	/*  U+00E5 - ?? - LATIN SMALL LETTER A WITH RING ABOVE */
    { "\xc3\xa6", "ae" , -1, -1},	/*  U+00E6 - ?? - LATIN SMALL LETTER AE */
    { "\xc3\xa7", "c" , -1, -1},	/*  U+00E7 - ?? - LATIN SMALL LETTER C WITH CEDILLA */
    { "\xc3\xa8", "e" , -1, -1},	/*  U+00E8 - ?? - LATIN SMALL LETTER E WITH GRAVE */
    { "\xc3\xa9", "e" , -1, -1},	/*  U+00E9 - ?? - LATIN SMALL LETTER E WITH ACUTE */
    { "\xc3\xaa", "e" , -1, -1},	/*  U+00EA - ?? - LATIN SMALL LETTER E WITH CIRCUMFLEX */
    { "\xc3\xab", "e" , -1, -1},	/*  U+00EB - ?? - LATIN SMALL LETTER E WITH DIAERESIS */
    { "\xc3\xac", "i" , -1, -1},	/*  U+00EC - ?? - LATIN SMALL LETTER I WITH GRAVE */
    { "\xc3\xad", "i" , -1, -1},	/*  U+00ED - ?? - LATIN SMALL LETTER I WITH ACUTE */
    { "\xc3\xae", "i" , -1, -1},	/*  U+00EE - ?? - LATIN SMALL LETTER I WITH CIRCUMFLEX */
    { "\xc3\xaf", "i" , -1, -1},	/*  U+00EF - ?? - LATIN SMALL LETTER I WITH DIAERESIS */
    { "\xc3\xb0", "o" , -1, -1},	/*  U+00F0 - ?? - LATIN SMALL LETTER ETH */
    { "\xc3\xb1", "n" , -1, -1},	/*  U+00F1 - ?? - LATIN SMALL LETTER N WITH TILDE */
    { "\xc3\xb2", "o" , -1, -1},	/*  U+00F2 - ?? - LATIN SMALL LETTER O WITH GRAVE */
    { "\xc3\xb3", "o" , -1, -1},	/*  U+00F3 - ?? - LATIN SMALL LETTER O WITH ACUTE */
    { "\xc3\xb4", "o" , -1, -1},	/*  U+00F4 - ?? - LATIN SMALL LETTER O WITH CIRCUMFLEX */
    { "\xc3\xb5", "o" , -1, -1},	/*  U+00F5 - ?? - LATIN SMALL LETTER O WITH TILDE */
    { "\xc3\xb6", "o" , -1, -1},	/*  U+00F6 - ?? - LATIN SMALL LETTER O WITH DIAERESIS */
    { "\xc3\xb7", "+" , -1, -1},	/*  U+00F7 - ?? - DIVISION SIGN */
    { "\xc3\xb8", "0" , -1, -1},	/*  U+00F8 - ?? - LATIN SMALL LETTER O WITH STROKE */
    { "\xc3\xb9", "u" , -1, -1},	/*  U+00F9 - ?? - LATIN SMALL LETTER U WITH GRAVE */
    { "\xc3\xba", "u" , -1, -1},	/*  U+00FA - ?? - LATIN SMALL LETTER U WITH ACUTE */
    { "\xc3\xbb", "u" , -1, -1},	/*  U+00FB - ?? - LATIN SMALL LETTER U WITH CIRCUMFLEX */
    { "\xc3\xbc", "u" , -1, -1},	/*  U+00FC - ?? - LATIN SMALL LETTER U WITH DIAERESIS */
    { "\xc3\xbd", "y" , -1, -1},	/*  U+00FD - ?? - LATIN SMALL LETTER Y WITH ACUTE */
    { "\xc3\xbe", "b" , -1, -1},	/*  U+00FE - ?? - LATIN SMALL LETTER THORN */
    { "\xc3\xbf", "y" , -1, -1},	/*  U+00FF - ?? - LATIN SMALL LETTER Y WITH DIAERESIS */
    { "\xc4\x80", "a" , -1, -1},	/*  U+0100 - ?? - LATIN CAPITAL LETTER A WITH MACRON */
    { "\xc4\x81", "a" , -1, -1},	/*  U+0101 - ?? - LATIN SMALL LETTER A WITH MACRON */
    { "\xc4\x82", "a" , -1, -1},	/*  U+0102 - ?? - LATIN CAPITAL LETTER A WITH BREVE */
    { "\xc4\x83", "a" , -1, -1},	/*  U+0103 - ?? - LATIN SMALL LETTER A WITH BREVE */
    { "\xc4\x84", "a" , -1, -1},	/*  U+0104 - ?? - LATIN CAPITAL LETTER A WITH OGONEK */
    { "\xc4\x85", "a" , -1, -1},	/*  U+0105 - ?? - LATIN SMALL LETTER A WITH OGONEK */
    { "\xc4\x86", "c" , -1, -1},	/*  U+0106 - ?? - LATIN CAPITAL LETTER C WITH ACUTE */
    { "\xc4\x87", "c" , -1, -1},	/*  U+0107 - ?? - LATIN SMALL LETTER C WITH ACUTE */
    { "\xc4\x88", "c" , -1, -1},	/*  U+0108 - ?? - LATIN CAPITAL LETTER C WITH CIRCUMFLEX */
    { "\xc4\x89", "c" , -1, -1},	/*  U+0109 - ?? - LATIN SMALL LETTER C WITH CIRCUMFLEX */
    { "\xc4\x8a", "c" , -1, -1},	/*  U+010A - ?? - LATIN CAPITAL LETTER C WITH DOT ABOVE */
    { "\xc4\x8b", "c" , -1, -1},	/*  U+010B - ?? - LATIN SMALL LETTER C WITH DOT ABOVE */
    { "\xc4\x8c", "c" , -1, -1},	/*  U+010C - ?? - LATIN CAPITAL LETTER C WITH CARON */
    { "\xc4\x8d", "c" , -1, -1},	/*  U+010D - ?? - LATIN SMALL LETTER C WITH CARON */
    { "\xc4\x8e", "d" , -1, -1},	/*  U+010E - ?? - LATIN CAPITAL LETTER D WITH CARON */
    { "\xc4\x8f", "d" , -1, -1},	/*  U+010F - ?? - LATIN SMALL LETTER D WITH CARON */
    { "\xc4\x90", "d" , -1, -1},	/*  U+0110 - ?? - LATIN CAPITAL LETTER D WITH STROKE */
    { "\xc4\x91", "d" , -1, -1},	/*  U+0111 - ?? - LATIN SMALL LETTER D WITH STROKE */
    { "\xc4\x92", "e" , -1, -1},	/*  U+0112 - ?? - LATIN CAPITAL LETTER E WITH MACRON */
    { "\xc4\x93", "e" , -1, -1},	/*  U+0113 - ?? - LATIN SMALL LETTER E WITH MACRON */
    { "\xc4\x94", "e" , -1, -1},	/*  U+0114 - ?? - LATIN CAPITAL LETTER E WITH BREVE */
    { "\xc4\x95", "e" , -1, -1},	/*  U+0115 - ?? - LATIN SMALL LETTER E WITH BREVE */
    { "\xc4\x96", "e" , -1, -1},	/*  U+0116 - ?? - LATIN CAPITAL LETTER E WITH DOT ABOVE */
    { "\xc4\x97", "e" , -1, -1},	/*  U+0117 - ?? - LATIN SMALL LETTER E WITH DOT ABOVE */
    { "\xc4\x98", "e" , -1, -1},	/*  U+0118 - ?? - LATIN CAPITAL LETTER E WITH OGONEK */
    { "\xc4\x99", "e" , -1, -1},	/*  U+0119 - ?? - LATIN SMALL LETTER E WITH OGONEK */
    { "\xc4\x9a", "e" , -1, -1},	/*  U+011A - ?? - LATIN CAPITAL LETTER E WITH CARON */
    { "\xc4\x9b", "e" , -1, -1},	/*  U+011B - ?? - LATIN SMALL LETTER E WITH CARON */
    { "\xc4\x9c", "g" , -1, -1},	/*  U+011C - ?? - LATIN CAPITAL LETTER G WITH CIRCUMFLEX */
    { "\xc4\x9d", "g" , -1, -1},	/*  U+011D - ?? - LATIN SMALL LETTER G WITH CIRCUMFLEX */
    { "\xc4\x9e", "g" , -1, -1},	/*  U+011E - ?? - LATIN CAPITAL LETTER G WITH BREVE */
    { "\xc4\x9f", "g" , -1, -1},	/*  U+011F - ?? - LATIN SMALL LETTER G WITH BREVE */
    { "\xc4\xa0", "g" , -1, -1},	/*  U+0120 - ?? - LATIN CAPITAL LETTER G WITH DOT ABOVE */
    { "\xc4\xa1", "g" , -1, -1},	/*  U+0121 - ?? - LATIN SMALL LETTER G WITH DOT ABOVE */
    { "\xc4\xa2", "g" , -1, -1},	/*  U+0122 - ?? - LATIN CAPITAL LETTER G WITH CEDILLA */
    { "\xc4\xa3", "g" , -1, -1},	/*  U+0123 - ?? - LATIN SMALL LETTER G WITH CEDILLA */
    { "\xc4\xa4", "h" , -1, -1},	/*  U+0124 - ?? - LATIN CAPITAL LETTER H WITH CIRCUMFLEX */
    { "\xc4\xa5", "h" , -1, -1},	/*  U+0125 - ?? - LATIN SMALL LETTER H WITH CIRCUMFLEX */
    { "\xc4\xa6", "h" , -1, -1},	/*  U+0126 - ?? - LATIN CAPITAL LETTER H WITH STROKE */
    { "\xc4\xa7", "h" , -1, -1},	/*  U+0127 - ?? - LATIN SMALL LETTER H WITH STROKE */
    { "\xc4\xa8", "i" , -1, -1},	/*  U+0128 - ?? - LATIN CAPITAL LETTER I WITH TILDE */
    { "\xc4\xa9", "i" , -1, -1},	/*  U+0129 - ?? - LATIN SMALL LETTER I WITH TILDE */
    { "\xc4\xaa", "i" , -1, -1},	/*  U+012A - ?? - LATIN CAPITAL LETTER I WITH MACRON */
    { "\xc4\xab", "i" , -1, -1},	/*  U+012B - ?? - LATIN SMALL LETTER I WITH MACRON */
    { "\xc4\xac", "i" , -1, -1},	/*  U+012C - ?? - LATIN CAPITAL LETTER I WITH BREVE */
    { "\xc4\xad", "i" , -1, -1},	/*  U+012D - ?? - LATIN SMALL LETTER I WITH BREVE */
    { "\xc4\xae", "i" , -1, -1},	/*  U+012E - ?? - LATIN CAPITAL LETTER I WITH OGONEK */
    { "\xc4\xaf", "i" , -1, -1},	/*  U+012F - ?? - LATIN SMALL LETTER I WITH OGONEK */
    { "\xc4\xb0", "i" , -1, -1},	/*  U+0130 - ?? - LATIN CAPITAL LETTER I WITH DOT ABOVE */
    { "\xc4\xb1", "i" , -1, -1},	/*  U+0131 - ?? - LATIN SMALL LETTER DOTLESS I */
    { "\xc4\xb2", "ij" , -1, -1},	/*  U+0132 - ?? - LATIN CAPITAL LIGATURE IJ */
    { "\xc4\xb3", "ij" , -1, -1},	/*  U+0133 - ?? - LATIN SMALL LIGATURE IJ */
    { "\xc4\xb4", "j" , -1, -1},	/*  U+0134 - ?? - LATIN CAPITAL LETTER J WITH CIRCUMFLEX */
    { "\xc4\xb5", "j" , -1, -1},	/*  U+0135 - ?? - LATIN SMALL LETTER J WITH CIRCUMFLEX */
    { "\xc4\xb6", "k" , -1, -1},	/*  U+0136 - ?? - LATIN CAPITAL LETTER K WITH CEDILLA */
    { "\xc4\xb7", "k" , -1, -1},	/*  U+0137 - ?? - LATIN SMALL LETTER K WITH CEDILLA */
    { "\xc4\xb8", "k" , -1, -1},	/*  U+0138 - ?? - LATIN SMALL LETTER KRA */
    { "\xc4\xb9", "l" , -1, -1},	/*  U+0139 - ?? - LATIN CAPITAL LETTER L WITH ACUTE */
    { "\xc4\xba", "l" , -1, -1},	/*  U+013A - ?? - LATIN SMALL LETTER L WITH ACUTE */
    { "\xc4\xbb", "l" , -1, -1},	/*  U+013B - ?? - LATIN CAPITAL LETTER L WITH CEDILLA */
    { "\xc4\xbc", "l" , -1, -1},	/*  U+013C - ?? - LATIN SMALL LETTER L WITH CEDILLA */
    { "\xc4\xbd", "l" , -1, -1},	/*  U+013D - ?? - LATIN CAPITAL LETTER L WITH CARON */
    { "\xc4\xbe", "l" , -1, -1},	/*  U+013E - ?? - LATIN SMALL LETTER L WITH CARON */
    { "\xc4\xbf", "l" , -1, -1},	/*  U+013F - ?? - LATIN CAPITAL LETTER L WITH MIDDLE DOT */
    { "\xc5\x80", "l" , -1, -1},	/*  U+0140 - ?? - LATIN SMALL LETTER L WITH MIDDLE DOT */
    { "\xc5\x81", "l" , -1, -1},	/*  U+0141 - ?? - LATIN CAPITAL LETTER L WITH STROKE */
    { "\xc5\x82", "l" , -1, -1},	/*  U+0142 - ?? - LATIN SMALL LETTER L WITH STROKE */
    { "\xc5\x83", "n" , -1, -1},	/*  U+0143 - ?? - LATIN CAPITAL LETTER N WITH ACUTE */
    { "\xc5\x84", "n" , -1, -1},	/*  U+0144 - ?? - LATIN SMALL LETTER N WITH ACUTE */
    { "\xc5\x85", "n" , -1, -1},	/*  U+0145 - ?? - LATIN CAPITAL LETTER N WITH CEDILLA */
    { "\xc5\x86", "n" , -1, -1},	/*  U+0146 - ?? - LATIN SMALL LETTER N WITH CEDILLA */
    { "\xc5\x87", "n" , -1, -1},	/*  U+0147 - ?? - LATIN CAPITAL LETTER N WITH CARON */
    { "\xc5\x88", "n" , -1, -1},	/*  U+0148 - ?? - LATIN SMALL LETTER N WITH CARON */
    { "\xc5\x89", "n" , -1, -1},	/*  U+0149 - ?? - LATIN SMALL LETTER N PRECEDED BY APOSTROPHE */
    { "\xc5\x8a", "n" , -1, -1},	/*  U+014A - ?? - LATIN CAPITAL LETTER ENG */
    { "\xc5\x8b", "n" , -1, -1},	/*  U+014B - ?? - LATIN SMALL LETTER ENG */
    { "\xc5\x8c", "o" , -1, -1},	/*  U+014C - ?? - LATIN CAPITAL LETTER O WITH MACRON */
    { "\xc5\x8d", "o" , -1, -1},	/*  U+014D - ?? - LATIN SMALL LETTER O WITH MACRON */
    { "\xc5\x8e", "o" , -1, -1},	/*  U+014E - ?? - LATIN CAPITAL LETTER O WITH BREVE */
    { "\xc5\x8f", "o" , -1, -1},	/*  U+014F - ?? - LATIN SMALL LETTER O WITH BREVE */
    { "\xc5\x90", "o" , -1, -1},	/*  U+0150 - ?? - LATIN CAPITAL LETTER O WITH DOUBLE ACUTE */
    { "\xc5\x91", "o" , -1, -1},	/*  U+0151 - ?? - LATIN SMALL LETTER O WITH DOUBLE ACUTE */
    { "\xc5\x92", "oe" , -1, -1},	/*  U+0152 - ?? - LATIN CAPITAL LIGATURE OE */
    { "\xc5\x93", "oe" , -1, -1},	/*  U+0153 - ?? - LATIN SMALL LIGATURE OE */
    { "\xc5\x94", "r" , -1, -1},	/*  U+0154 - ?? - LATIN CAPITAL LETTER R WITH ACUTE */
    { "\xc5\x95", "r" , -1, -1},	/*  U+0155 - ?? - LATIN SMALL LETTER R WITH ACUTE */
    { "\xc5\x96", "r" , -1, -1},	/*  U+0156 - ?? - LATIN CAPITAL LETTER R WITH CEDILLA */
    { "\xc5\x97", "r" , -1, -1},	/*  U+0157 - ?? - LATIN SMALL LETTER R WITH CEDILLA */
    { "\xc5\x98", "r" , -1, -1},	/*  U+0158 - ?? - LATIN CAPITAL LETTER R WITH CARON */
    { "\xc5\x99", "r" , -1, -1},	/*  U+0159 - ?? - LATIN SMALL LETTER R WITH CARON */
    { "\xc5\x9a", "s" , -1, -1},	/*  U+015A - ?? - LATIN CAPITAL LETTER S WITH ACUTE */
    { "\xc5\x9b", "s" , -1, -1},	/*  U+015B - ?? - LATIN SMALL LETTER S WITH ACUTE */
    { "\xc5\x9c", "s" , -1, -1},	/*  U+015C - ?? - LATIN CAPITAL LETTER S WITH CIRCUMFLEX */
    { "\xc5\x9d", "s" , -1, -1},	/*  U+015D - ?? - LATIN SMALL LETTER S WITH CIRCUMFLEX */
    { "\xc5\x9e", "s" , -1, -1},	/*  U+015E - ?? - LATIN CAPITAL LETTER S WITH CEDILLA */
    { "\xc5\x9f", "s" , -1, -1},	/*  U+015F - ?? - LATIN SMALL LETTER S WITH CEDILLA */
    { "\xc5\xa0", "s" , -1, -1},	/*  U+0160 - ?? - LATIN CAPITAL LETTER S WITH CARON */
    { "\xc5\xa1", "s" , -1, -1},	/*  U+0161 - ?? - LATIN SMALL LETTER S WITH CARON */
    { "\xc5\xa2", "t" , -1, -1},	/*  U+0162 - ?? - LATIN CAPITAL LETTER T WITH CEDILLA */
    { "\xc5\xa3", "t" , -1, -1},	/*  U+0163 - ?? - LATIN SMALL LETTER T WITH CEDILLA */
    { "\xc5\xa4", "t" , -1, -1},	/*  U+0164 - ?? - LATIN CAPITAL LETTER T WITH CARON */
    { "\xc5\xa5", "t" , -1, -1},	/*  U+0165 - ?? - LATIN SMALL LETTER T WITH CARON */
    { "\xc5\xa6", "t" , -1, -1},	/*  U+0166 - ?? - LATIN CAPITAL LETTER T WITH STROKE */
    { "\xc5\xa7", "t" , -1, -1},	/*  U+0167 - ?? - LATIN SMALL LETTER T WITH STROKE */
    { "\xc5\xa8", "u" , -1, -1},	/*  U+0168 - ?? - LATIN CAPITAL LETTER U WITH TILDE */
    { "\xc5\xa9", "u" , -1, -1},	/*  U+0169 - ?? - LATIN SMALL LETTER U WITH TILDE */
    { "\xc5\xaa", "u" , -1, -1},	/*  U+016A - ?? - LATIN CAPITAL LETTER U WITH MACRON */
    { "\xc5\xab", "u" , -1, -1},	/*  U+016B - ?? - LATIN SMALL LETTER U WITH MACRON */
    { "\xc5\xac", "u" , -1, -1},	/*  U+016C - ?? - LATIN CAPITAL LETTER U WITH BREVE */
    { "\xc5\xad", "u" , -1, -1},	/*  U+016D - ?? - LATIN SMALL LETTER U WITH BREVE */
    { "\xc5\xae", "u" , -1, -1},	/*  U+016E - ?? - LATIN CAPITAL LETTER U WITH RING ABOVE */
    { "\xc5\xaf", "u" , -1, -1},	/*  U+016F - ?? - LATIN SMALL LETTER U WITH RING ABOVE */
    { "\xc5\xb0", "u" , -1, -1},	/*  U+0170 - ?? - LATIN CAPITAL LETTER U WITH DOUBLE ACUTE */
    { "\xc5\xb1", "u" , -1, -1},	/*  U+0171 - ?? - LATIN SMALL LETTER U WITH DOUBLE ACUTE */
    { "\xc5\xb2", "u" , -1, -1},	/*  U+0172 - ?? - LATIN CAPITAL LETTER U WITH OGONEK */
    { "\xc5\xb3", "u" , -1, -1},	/*  U+0173 - ?? - LATIN SMALL LETTER U WITH OGONEK */
    { "\xc5\xb4", "w" , -1, -1},	/*  U+0174 - ?? - LATIN CAPITAL LETTER W WITH CIRCUMFLEX */
    { "\xc5\xb5", "w" , -1, -1},	/*  U+0175 - ?? - LATIN SMALL LETTER W WITH CIRCUMFLEX */
    { "\xc5\xb6", "y" , -1, -1},	/*  U+0176 - ?? - LATIN CAPITAL LETTER Y WITH CIRCUMFLEX */
    { "\xc5\xb7", "y" , -1, -1},	/*  U+0177 - ?? - LATIN SMALL LETTER Y WITH CIRCUMFLEX */
    { "\xc5\xb8", "y" , -1, -1},	/*  U+0178 - ?? - LATIN CAPITAL LETTER Y WITH DIAERESIS */
    { "\xc5\xb9", "z" , -1, -1},	/*  U+0179 - ?? - LATIN CAPITAL LETTER Z WITH ACUTE */
    { "\xc5\xba", "z" , -1, -1},	/*  U+017A - ?? - LATIN SMALL LETTER Z WITH ACUTE */
    { "\xc5\xbb", "z" , -1, -1},	/*  U+017B - ?? - LATIN CAPITAL LETTER Z WITH DOT ABOVE */
    { "\xc5\xbc", "z" , -1, -1},	/*  U+017C - ?? - LATIN SMALL LETTER Z WITH DOT ABOVE */
    { "\xc5\xbd", "z" , -1, -1},	/*  U+017D - ?? - LATIN CAPITAL LETTER Z WITH CARON */
    { "\xc5\xbe", "z" , -1, -1},	/*  U+017E - ?? - LATIN SMALL LETTER Z WITH CARON */
    { "\xc5\xbf", "f" , -1, -1},	/*  U+017F - ?? - LATIN SMALL LETTER LONG S */
    { "\xc6\x80", "b" , -1, -1},	/*  U+0180 - ?? - LATIN SMALL LETTER B WITH STROKE */
    { "\xc6\x81", "b" , -1, -1},	/*  U+0181 - ?? - LATIN CAPITAL LETTER B WITH HOOK */
    { "\xc6\x82", "b" , -1, -1},	/*  U+0182 - ?? - LATIN CAPITAL LETTER B WITH TOPBAR */
    { "\xc6\x83", "b" , -1, -1},	/*  U+0183 - ?? - LATIN SMALL LETTER B WITH TOPBAR */
    { "\xc6\x84", "b" , -1, -1},	/*  U+0184 - ?? - LATIN CAPITAL LETTER TONE SIX */
    { "\xc6\x85", "b" , -1, -1},	/*  U+0185 - ?? - LATIN SMALL LETTER TONE SIX */
    { "\xc6\x86", "c" , -1, -1},	/*  U+0186 - ?? - LATIN CAPITAL LETTER OPEN O */
    { "\xc6\x87", "c" , -1, -1},	/*  U+0187 - ?? - LATIN CAPITAL LETTER C WITH HOOK */
    { "\xc6\x88", "c" , -1, -1},	/*  U+0188 - ?? - LATIN SMALL LETTER C WITH HOOK */
    { "\xc6\x89", "d" , -1, -1},	/*  U+0189 - ?? - LATIN CAPITAL LETTER AFRICAN D */
    { "\xc6\x8a", "d" , -1, -1},	/*  U+018A - ?? - LATIN CAPITAL LETTER D WITH HOOK */
    { "\xc6\x8b", "d" , -1, -1},	/*  U+018B - ?? - LATIN CAPITAL LETTER D WITH TOPBAR */
    { "\xc6\x8c", "d" , -1, -1},	/*  U+018C - ?? - LATIN SMALL LETTER D WITH TOPBAR */
    { "\xc6\x8d", "o" , -1, -1},	/*  U+018D - ?? - LATIN SMALL LETTER TURNED DELTA */
    { "\xc6\x8e", "e" , -1, -1},	/*  U+018E - ?? - LATIN CAPITAL LETTER REVERSED E */
    { "\xc6\x8f", "e" , -1, -1},	/*  U+018F - ?? - LATIN CAPITAL LETTER SCHWA */
    { "\xc6\x90", "e" , -1, -1},	/*  U+0190 - ?? - LATIN CAPITAL LETTER OPEN E */
    { "\xc6\x91", "f" , -1, -1},	/*  U+0191 - ?? - LATIN CAPITAL LETTER F WITH HOOK */
    { "\xc6\x92", "f" , -1, -1},	/*  U+0192 - ?? - LATIN SMALL LETTER F WITH HOOK */
    { "\xc6\x93", "g" , -1, -1},	/*  U+0193 - ?? - LATIN CAPITAL LETTER G WITH HOOK */
    { "\xc6\x94", "y" , -1, -1},	/*  U+0194 - ?? - LATIN CAPITAL LETTER GAMMA */
    { "\xc6\x95", "hu" , -1, -1},	/*  U+0195 - ?? - LATIN SMALL LETTER HV */
    { "\xc6\x96", "l" , -1, -1},	/*  U+0196 - ?? - LATIN CAPITAL LETTER IOTA */
    { "\xc6\x97", "i" , -1, -1},	/*  U+0197 - ?? - LATIN CAPITAL LETTER I WITH STROKE */
    { "\xc6\x98", "k" , -1, -1},	/*  U+0198 - ?? - LATIN CAPITAL LETTER K WITH HOOK */
    { "\xc6\x99", "k" , -1, -1},	/*  U+0199 - ?? - LATIN SMALL LETTER K WITH HOOK */
    { "\xc6\x9a", "l" , -1, -1},	/*  U+019A - ?? - LATIN SMALL LETTER L WITH BAR */
    { "\xc6\x9b", "l" , -1, -1},	/*  U+019B - ?? - LATIN SMALL LETTER LAMBDA WITH STROKE */
    { "\xc6\x9c", "w" , -1, -1},	/*  U+019C - ?? - LATIN CAPITAL LETTER TURNED M */
    { "\xc6\x9d", "n" , -1, -1},	/*  U+019D - ?? - LATIN CAPITAL LETTER N WITH LEFT HOOK */
    { "\xc6\x9e", "n" , -1, -1},	/*  U+019E - ?? - LATIN SMALL LETTER N WITH LONG RIGHT LEG */
    { "\xc6\x9f", "0" , -1, -1},	/*  U+019F - ?? - LATIN CAPITAL LETTER O WITH MIDDLE TILDE */
    { "\xc6\xa0", "o" , -1, -1},	/*  U+01A0 - ?? - LATIN CAPITAL LETTER O WITH HORN */
    { "\xc6\xa1", "o" , -1, -1},	/*  U+01A1 - ?? - LATIN SMALL LETTER O WITH HORN */
    { "\xc6\xa2", "oi" , -1, -1},	/*  U+01A2 - ?? - LATIN CAPITAL LETTER OI */
    { "\xc6\xa3", "oi" , -1, -1},	/*  U+01A3 - ?? - LATIN SMALL LETTER OI */
    { "\xc6\xa4", "p" , -1, -1},	/*  U+01A4 - ?? - LATIN CAPITAL LETTER P WITH HOOK */
    { "\xc6\xa5", "p" , -1, -1},	/*  U+01A5 - ?? - LATIN SMALL LETTER P WITH HOOK */
    { "\xc6\xa6", "r" , -1, -1},	/*  U+01A6 - ?? - LATIN LETTER YR */
    { "\xc6\xa7", "s" , -1, -1},	/*  U+01A7 - ?? - LATIN CAPITAL LETTER TONE TWO */
    { "\xc6\xa8", "s" , -1, -1},	/*  U+01A8 - ?? - LATIN SMALL LETTER TONE TWO */
    { "\xc6\xa9", "e" , -1, -1},	/*  U+01A9 - ?? - LATIN CAPITAL LETTER ESH */
    { "\xc6\xaa", "l" , -1, -1},	/*  U+01AA - ?? - LATIN LETTER REVERSED ESH LOOP */
    { "\xc6\xab", "t" , -1, -1},	/*  U+01AB - ?? - LATIN SMALL LETTER T WITH PALATAL HOOK */
    { "\xc6\xac", "t" , -1, -1},	/*  U+01AC - ?? - LATIN CAPITAL LETTER T WITH HOOK */
    { "\xc6\xad", "e" , -1, -1},	/*  U+01AD - ?? - LATIN SMALL LETTER T WITH HOOK */
    { "\xc6\xae", "t" , -1, -1},	/*  U+01AE - ?? - LATIN CAPITAL LETTER T WITH RETROFLEX HOOK */
    { "\xc6\xaf", "u" , -1, -1},	/*  U+01AF - ?? - LATIN CAPITAL LETTER U WITH HORN */
    { "\xc6\xb0", "u" , -1, -1},	/*  U+01B0 - ?? - LATIN SMALL LETTER U WITH HORN */
    { "\xc6\xb1", "v" , -1, -1},	/*  U+01B1 - ?? - LATIN CAPITAL LETTER UPSILON */
    { "\xc6\xb2", "v" , -1, -1},	/*  U+01B2 - ?? - LATIN CAPITAL LETTER V WITH HOOK */
    { "\xc6\xb3", "y" , -1, -1},	/*  U+01B3 - ?? - LATIN CAPITAL LETTER Y WITH HOOK */
    { "\xc6\xb4", "y" , -1, -1},	/*  U+01B4 - ?? - LATIN SMALL LETTER Y WITH HOOK */
    { "\xc6\xb5", "z" , -1, -1},	/*  U+01B5 - ?? - LATIN CAPITAL LETTER Z WITH STROKE */
    { "\xc6\xb6", "z" , -1, -1},	/*  U+01B6 - ?? - LATIN SMALL LETTER Z WITH STROKE */
    { "\xc6\xb7", "3" , -1, -1},	/*  U+01B7 - ?? - LATIN CAPITAL LETTER EZH */
    { "\xc6\xb8", "e" , -1, -1},	/*  U+01B8 - ?? - LATIN CAPITAL LETTER EZH REVERSED */
    { "\xc6\xb9", "e" , -1, -1},	/*  U+01B9 - ?? - LATIN SMALL LETTER EZH REVERSED */
    { "\xc6\xba", "3" , -1, -1},	/*  U+01BA - ?? - LATIN SMALL LETTER EZH WITH TAIL */
    { "\xc6\xbb", "2" , -1, -1},	/*  U+01BB - ?? - LATIN LETTER TWO WITH STROKE */
    { "\xc6\xbc", "5" , -1, -1},	/*  U+01BC - ?? - LATIN CAPITAL LETTER TONE FIVE */
    { "\xc6\xbd", "5" , -1, -1},	/*  U+01BD - ?? - LATIN SMALL LETTER TONE FIVE */
    { "\xc6\xbe", "+" , -1, -1},	/*  U+01BE - ?? - LATIN LETTER INVERTED GLOTTAL STOP WITH STROKE */
    { "\xc6\xbf", "p" , -1, -1},	/*  U+01BF - ?? - LATIN LETTER WYNN */
    { "\xc7\x80", "l" , -1, -1},	/*  U+01C0 - ?? - LATIN LETTER DENTAL CLICK */
    { "\xc7\x81", "ll" , -1, -1},	/*  U+01C1 - ?? - LATIN LETTER LATERAL CLICK */
    { "\xc7\x82", "+" , -1, -1},	/*  U+01C2 - ?? - LATIN LETTER ALVEOLAR CLICK */
    { "\xc7\x83", "" , -1, -1},		/*  U+01C3 - ?? - LATIN LETTER RETROFLEX CLICK */
    { "\xc7\x84", "dz" , -1, -1},	/*  U+01C4 - ?? - LATIN CAPITAL LETTER DZ WITH CARON */
    { "\xc7\x85", "dz" , -1, -1},	/*  U+01C5 - ?? - LATIN CAPITAL LETTER D WITH SMALL LETTER Z WITH CARON */
    { "\xc7\x86", "dz" , -1, -1},	/*  U+01C6 - ?? - LATIN SMALL LETTER DZ WITH CARON */
    { "\xc7\x87", "lj" , -1, -1},	/*  U+01C7 - ?? - LATIN CAPITAL LETTER LJ */
    { "\xc7\x88", "lj" , -1, -1},	/*  U+01C8 - ?? - LATIN CAPITAL LETTER L WITH SMALL LETTER J */
    { "\xc7\x89", "lj" , -1, -1},	/*  U+01C9 - ?? - LATIN SMALL LETTER LJ */
    { "\xc7\x8a", "nj" , -1, -1},	/*  U+01CA - ?? - LATIN CAPITAL LETTER NJ */
    { "\xc7\x8b", "nj" , -1, -1},	/*  U+01CB - ?? - LATIN CAPITAL LETTER N WITH SMALL LETTER J */
    { "\xc7\x8c", "nj" , -1, -1},	/*  U+01CC - ?? - LATIN SMALL LETTER NJ */
    { "\xc7\x8d", "a" , -1, -1},	/*  U+01CD - ?? - LATIN CAPITAL LETTER A WITH CARON */
    { "\xc7\x8e", "a" , -1, -1},	/*  U+01CE - ?? - LATIN SMALL LETTER A WITH CARON */
    { "\xc7\x8f", "i" , -1, -1},	/*  U+01CF - ?? - LATIN CAPITAL LETTER I WITH CARON */
    { "\xc7\x90", "i" , -1, -1},	/*  U+01D0 - ?? - LATIN SMALL LETTER I WITH CARON */
    { "\xc7\x91", "o" , -1, -1},	/*  U+01D1 - ?? - LATIN CAPITAL LETTER O WITH CARON */
    { "\xc7\x92", "o" , -1, -1},	/*  U+01D2 - ?? - LATIN SMALL LETTER O WITH CARON */
    { "\xc7\x93", "u" , -1, -1},	/*  U+01D3 - ?? - LATIN CAPITAL LETTER U WITH CARON */
    { "\xc7\x94", "u" , -1, -1},	/*  U+01D4 - ?? - LATIN SMALL LETTER U WITH CARON */
    { "\xc7\x95", "u" , -1, -1},	/*  U+01D5 - ?? - LATIN CAPITAL LETTER U WITH DIAERESIS AND MACRON */
    { "\xc7\x96", "u" , -1, -1},	/*  U+01D6 - ?? - LATIN SMALL LETTER U WITH DIAERESIS AND MACRON */
    { "\xc7\x97", "u" , -1, -1},	/*  U+01D7 - ?? - LATIN CAPITAL LETTER U WITH DIAERESIS AND ACUTE */
    { "\xc7\x98", "u" , -1, -1},	/*  U+01D8 - ?? - LATIN SMALL LETTER U WITH DIAERESIS AND ACUTE */
    { "\xc7\x99", "u" , -1, -1},	/*  U+01D9 - ?? - LATIN CAPITAL LETTER U WITH DIAERESIS AND CARON */
    { "\xc7\x9a", "u" , -1, -1},	/*  U+01DA - ?? - LATIN SMALL LETTER U WITH DIAERESIS AND CARON */
    { "\xc7\x9b", "u" , -1, -1},	/*  U+01DB - ?? - LATIN CAPITAL LETTER U WITH DIAERESIS AND GRAVE */
    { "\xc7\x9c", "u" , -1, -1},	/*  U+01DC - ?? - LATIN SMALL LETTER U WITH DIAERESIS AND GRAVE */
    { "\xc7\x9d", "e" , -1, -1},	/*  U+01DD - ?? - LATIN SMALL LETTER TURNED E */
    { "\xc7\x9e", "a" , -1, -1},	/*  U+01DE - ?? - LATIN CAPITAL LETTER A WITH DIAERESIS AND MACRON */
    { "\xc7\x9f", "a" , -1, -1},	/*  U+01DF - ?? - LATIN SMALL LETTER A WITH DIAERESIS AND MACRON */
    { "\xc7\xa0", "a" , -1, -1},	/*  U+01E0 - ?? - LATIN CAPITAL LETTER A WITH DOT ABOVE AND MACRON */
    { "\xc7\xa1", "a" , -1, -1},	/*  U+01E1 - ?? - LATIN SMALL LETTER A WITH DOT ABOVE AND MACRON */
    { "\xc7\xa2", "ae" , -1, -1},	/*  U+01E2 - ?? - LATIN CAPITAL LETTER AE WITH MACRON */
    { "\xc7\xa3", "ae" , -1, -1},	/*  U+01E3 - ?? - LATIN SMALL LETTER AE WITH MACRON */
    { "\xc7\xa4", "g" , -1, -1},	/*  U+01E4 - ?? - LATIN CAPITAL LETTER G WITH STROKE */
    { "\xc7\xa5", "g" , -1, -1},	/*  U+01E5 - ?? - LATIN SMALL LETTER G WITH STROKE */
    { "\xc7\xa6", "g" , -1, -1},	/*  U+01E6 - ?? - LATIN CAPITAL LETTER G WITH CARON */
    { "\xc7\xa7", "g" , -1, -1},	/*  U+01E7 - ?? - LATIN SMALL LETTER G WITH CARON */
    { "\xc7\xa8", "k" , -1, -1},	/*  U+01E8 - ?? - LATIN CAPITAL LETTER K WITH CARON */
    { "\xc7\xa9", "k" , -1, -1},	/*  U+01E9 - ?? - LATIN SMALL LETTER K WITH CARON */
    { "\xc7\xaa", "o" , -1, -1},	/*  U+01EA - ?? - LATIN CAPITAL LETTER O WITH OGONEK */
    { "\xc7\xab", "o" , -1, -1},	/*  U+01EB - ?? - LATIN SMALL LETTER O WITH OGONEK */
    { "\xc7\xac", "o" , -1, -1},	/*  U+01EC - ?? - LATIN CAPITAL LETTER O WITH OGONEK AND MACRON */
    { "\xc7\xad", "o" , -1, -1},	/*  U+01ED - ?? - LATIN SMALL LETTER O WITH OGONEK AND MACRON */
    { "\xc7\xae", "3" , -1, -1},	/*  U+01EE - ?? - LATIN CAPITAL LETTER EZH WITH CARON */
    { "\xc7\xaf", "3" , -1, -1},	/*  U+01EF - ?? - LATIN SMALL LETTER EZH WITH CARON */
    { "\xc7\xb0", "j" , -1, -1},	/*  U+01F0 - ?? - LATIN SMALL LETTER J WITH CARON */
    { "\xc7\xb1", "dz" , -1, -1},	/*  U+01F1 - ?? - LATIN CAPITAL LETTER DZ */
    { "\xc7\xb2", "dz" , -1, -1},	/*  U+01F2 - ?? - LATIN CAPITAL LETTER D WITH SMALL LETTER Z */
    { "\xc7\xb3", "dz" , -1, -1},	/*  U+01F3 - ?? - LATIN SMALL LETTER DZ */
    { "\xc7\xb4", "g" , -1, -1},	/*  U+01F4 - ?? - LATIN CAPITAL LETTER G WITH ACUTE */
    { "\xc7\xb5", "g" , -1, -1},	/*  U+01F5 - ?? - LATIN SMALL LETTER G WITH ACUTE */
    { "\xc7\xb6", "hu" , -1, -1},	/*  U+01F6 - ?? - LATIN CAPITAL LETTER HWAIR */
    { "\xc7\xb7", "p" , -1, -1},	/*  U+01F7 - ?? - LATIN CAPITAL LETTER WYNN */
    { "\xc7\xb8", "n" , -1, -1},	/*  U+01F8 - ?? - LATIN CAPITAL LETTER N WITH GRAVE */
    { "\xc7\xb9", "n" , -1, -1},	/*  U+01F9 - ?? - LATIN SMALL LETTER N WITH GRAVE */
    { "\xc7\xba", "a" , -1, -1},	/*  U+01FA - ?? - LATIN CAPITAL LETTER A WITH RING ABOVE AND ACUTE */
    { "\xc7\xbb", "a" , -1, -1},	/*  U+01FB - ?? - LATIN SMALL LETTER A WITH RING ABOVE AND ACUTE */
    { "\xc7\xbc", "ae" , -1, -1},	/*  U+01FC - ?? - LATIN CAPITAL LETTER AE WITH ACUTE */
    { "\xc7\xbd", "ae" , -1, -1},	/*  U+01FD - ?? - LATIN SMALL LETTER AE WITH ACUTE */
    { "\xc7\xbe", "0" , -1, -1},	/*  U+01FE - ?? - LATIN CAPITAL LETTER O WITH STROKE AND ACUTE */
    { "\xc7\xbf", "0" , -1, -1},	/*  U+01FF - ?? - LATIN SMALL LETTER O WITH STROKE AND ACUTE */
    { "\xc8\x80", "a" , -1, -1},	/*  U+0200 - ?? - LATIN CAPITAL LETTER A WITH DOUBLE GRAVE */
    { "\xc8\x81", "a" , -1, -1},	/*  U+0201 - ?? - LATIN SMALL LETTER A WITH DOUBLE GRAVE */
    { "\xc8\x82", "a" , -1, -1},	/*  U+0202 - ?? - LATIN CAPITAL LETTER A WITH INVERTED BREVE */
    { "\xc8\x83", "a" , -1, -1},	/*  U+0203 - ?? - LATIN SMALL LETTER A WITH INVERTED BREVE */
    { "\xc8\x84", "e" , -1, -1},	/*  U+0204 - ?? - LATIN CAPITAL LETTER E WITH DOUBLE GRAVE */
    { "\xc8\x85", "e" , -1, -1},	/*  U+0205 - ?? - LATIN SMALL LETTER E WITH DOUBLE GRAVE */
    { "\xc8\x86", "e" , -1, -1},	/*  U+0206 - ?? - LATIN CAPITAL LETTER E WITH INVERTED BREVE */
    { "\xc8\x87", "e" , -1, -1},	/*  U+0207 - ?? - LATIN SMALL LETTER E WITH INVERTED BREVE */
    { "\xc8\x88", "i" , -1, -1},	/*  U+0208 - ?? - LATIN CAPITAL LETTER I WITH DOUBLE GRAVE */
    { "\xc8\x89", "i" , -1, -1},	/*  U+0209 - ?? - LATIN SMALL LETTER I WITH DOUBLE GRAVE */
    { "\xc8\x8a", "i" , -1, -1},	/*  U+020A - ?? - LATIN CAPITAL LETTER I WITH INVERTED BREVE */
    { "\xc8\x8b", "i" , -1, -1},	/*  U+020B - ?? - LATIN SMALL LETTER I WITH INVERTED BREVE */
    { "\xc8\x8c", "o" , -1, -1},	/*  U+020C - ?? - LATIN CAPITAL LETTER O WITH DOUBLE GRAVE */
    { "\xc8\x8d", "o" , -1, -1},	/*  U+020D - ?? - LATIN SMALL LETTER O WITH DOUBLE GRAVE */
    { "\xc8\x8e", "o" , -1, -1},	/*  U+020E - ?? - LATIN CAPITAL LETTER O WITH INVERTED BREVE */
    { "\xc8\x8f", "o" , -1, -1},	/*  U+020F - ?? - LATIN SMALL LETTER O WITH INVERTED BREVE */
    { "\xc8\x90", "r" , -1, -1},	/*  U+0210 - ?? - LATIN CAPITAL LETTER R WITH DOUBLE GRAVE */
    { "\xc8\x91", "r" , -1, -1},	/*  U+0211 - ?? - LATIN SMALL LETTER R WITH DOUBLE GRAVE */
    { "\xc8\x92", "r" , -1, -1},	/*  U+0212 - ?? - LATIN CAPITAL LETTER R WITH INVERTED BREVE */
    { "\xc8\x93", "r" , -1, -1},	/*  U+0213 - ?? - LATIN SMALL LETTER R WITH INVERTED BREVE */
    { "\xc8\x94", "u" , -1, -1},	/*  U+0214 - ?? - LATIN CAPITAL LETTER U WITH DOUBLE GRAVE */
    { "\xc8\x95", "u" , -1, -1},	/*  U+0215 - ?? - LATIN SMALL LETTER U WITH DOUBLE GRAVE */
    { "\xc8\x96", "u" , -1, -1},	/*  U+0216 - ?? - LATIN CAPITAL LETTER U WITH INVERTED BREVE */
    { "\xc8\x97", "u" , -1, -1},	/*  U+0217 - ?? - LATIN SMALL LETTER U WITH INVERTED BREVE */
    { "\xc8\x98", "s" , -1, -1},	/*  U+0218 - ?? - LATIN CAPITAL LETTER S WITH COMMA BELOW */
    { "\xc8\x99", "s" , -1, -1},	/*  U+0219 - ?? - LATIN SMALL LETTER S WITH COMMA BELOW */
    { "\xc8\x9a", "t" , -1, -1},	/*  U+021A - ?? - LATIN CAPITAL LETTER T WITH COMMA BELOW */
    { "\xc8\x9b", "t" , -1, -1},	/*  U+021B - ?? - LATIN SMALL LETTER T WITH COMMA BELOW */
    { "\xc8\x9c", "3" , -1, -1},	/*  U+021C - ?? - LATIN CAPITAL LETTER YOGH */
    { "\xc8\x9d", "3" , -1, -1},	/*  U+021D - ?? - LATIN SMALL LETTER YOGH */
    { "\xc8\x9e", "h" , -1, -1},	/*  U+021E - ?? - LATIN CAPITAL LETTER H WITH CARON */
    { "\xc8\x9f", "h" , -1, -1},	/*  U+021F - ?? - LATIN SMALL LETTER H WITH CARON */
    { "\xc8\xa0", "n" , -1, -1},	/*  U+0220 - ?? - LATIN CAPITAL LETTER N WITH LONG RIGHT LEG */
    { "\xc8\xa1", "d" , -1, -1},	/*  U+0221 - ?? - LATIN SMALL LETTER D WITH CURL */
    { "\xc8\xa2", "y" , -1, -1},	/*  U+0222 - ?? - LATIN CAPITAL LETTER OU */
    { "\xc8\xa3", "y" , -1, -1},	/*  U+0223 - ?? - LATIN SMALL LETTER OU */
    { "\xc8\xa4", "z" , -1, -1},	/*  U+0224 - ?? - LATIN CAPITAL LETTER Z WITH HOOK */
    { "\xc8\xa5", "z" , -1, -1},	/*  U+0225 - ?? - LATIN SMALL LETTER Z WITH HOOK */
    { "\xc8\xa6", "a" , -1, -1},	/*  U+0226 - ?? - LATIN CAPITAL LETTER A WITH DOT ABOVE */
    { "\xc8\xa7", "a" , -1, -1},	/*  U+0227 - ?? - LATIN SMALL LETTER A WITH DOT ABOVE */
    { "\xc8\xa8", "e" , -1, -1},	/*  U+0228 - ?? - LATIN CAPITAL LETTER E WITH CEDILLA */
    { "\xc8\xa9", "e" , -1, -1},	/*  U+0229 - ?? - LATIN SMALL LETTER E WITH CEDILLA */
    { "\xc8\xaa", "o" , -1, -1},	/*  U+022A - ?? - LATIN CAPITAL LETTER O WITH DIAERESIS AND MACRON */
    { "\xc8\xab", "o" , -1, -1},	/*  U+022B - ?? - LATIN SMALL LETTER O WITH DIAERESIS AND MACRON */
    { "\xc8\xac", "o" , -1, -1},	/*  U+022C - ?? - LATIN CAPITAL LETTER O WITH TILDE AND MACRON */
    { "\xc8\xad", "o" , -1, -1},	/*  U+022D - ?? - LATIN SMALL LETTER O WITH TILDE AND MACRON */
    { "\xc8\xae", "o" , -1, -1},	/*  U+022E - ?? - LATIN CAPITAL LETTER O WITH DOT ABOVE */
    { "\xc8\xaf", "o" , -1, -1},	/*  U+022F - ?? - LATIN SMALL LETTER O WITH DOT ABOVE */
    { "\xc8\xb0", "o" , -1, -1},	/*  U+0230 - ?? - LATIN CAPITAL LETTER O WITH DOT ABOVE AND MACRON */
    { "\xc8\xb1", "o" , -1, -1},	/*  U+0231 - ?? - LATIN SMALL LETTER O WITH DOT ABOVE AND MACRON */
    { "\xc8\xb2", "y" , -1, -1},	/*  U+0232 - ?? - LATIN CAPITAL LETTER Y WITH MACRON */
    { "\xc8\xb3", "y" , -1, -1},	/*  U+0233 - ?? - LATIN SMALL LETTER Y WITH MACRON */
    { "\xc8\xb4", "l" , -1, -1},	/*  U+0234 - ?? - LATIN SMALL LETTER L WITH CURL */
    { "\xc8\xb5", "n" , -1, -1},	/*  U+0235 - ?? - LATIN SMALL LETTER N WITH CURL */
    { "\xc8\xb6", "t" , -1, -1},	/*  U+0236 - ?? - LATIN SMALL LETTER T WITH CURL */
    { "\xc8\xb7", "j" , -1, -1},	/*  U+0237 - ?? - LATIN SMALL LETTER DOTLESS J */
    { "\xc8\xb8", "db" , -1, -1},	/*  U+0238 - ?? - LATIN SMALL LETTER DB DIGRAPH */
    { "\xc8\xb9", "qp" , -1, -1},	/*  U+0239 - ?? - LATIN SMALL LETTER QP DIGRAPH */
    { "\xc8\xba", "a" , -1, -1},	/*  U+023A - ?? - LATIN CAPITAL LETTER A WITH STROKE */
    { "\xc8\xbb", "c" , -1, -1},	/*  U+023B - ?? - LATIN CAPITAL LETTER C WITH STROKE */
    { "\xc8\xbc", "c" , -1, -1},	/*  U+023C - ?? - LATIN SMALL LETTER C WITH STROKE */
    { "\xc8\xbd", "l" , -1, -1},	/*  U+023D - ?? - LATIN CAPITAL LETTER L WITH BAR */
    { "\xc8\xbe", "7" , -1, -1},	/*  U+023E - ?? - LATIN CAPITAL LETTER T WITH DIAGONAL STROKE */
    { "\xc8\xbf", "s" , -1, -1},	/*  U+023F - ?? - LATIN SMALL LETTER S WITH SWASH TAIL */
    { "\xc9\x80", "z" , -1, -1},	/*  U+0240 - ?? - LATIN SMALL LETTER Z WITH SWASH TAIL */
    { "\xc9\x81", "2" , -1, -1},	/*  U+0241 - ?? - LATIN CAPITAL LETTER GLOTTAL STOP */
    { "\xc9\x82", "2" , -1, -1},	/*  U+0242 - ?? - LATIN SMALL LETTER GLOTTAL STOP */
    { "\xc9\x83", "b" , -1, -1},	/*  U+0243 - ?? - LATIN CAPITAL LETTER B WITH STROKE */
    { "\xc9\x84", "u" , -1, -1},	/*  U+0244 - ?? - LATIN CAPITAL LETTER U BAR */
    { "\xc9\x85", "a" , -1, -1},	/*  U+0245 - ?? - LATIN CAPITAL LETTER TURNED V */
    { "\xc9\x86", "e" , -1, -1},	/*  U+0246 - ?? - LATIN CAPITAL LETTER E WITH STROKE */
    { "\xc9\x87", "e" , -1, -1},	/*  U+0247 - ?? - LATIN SMALL LETTER E WITH STROKE */
    { "\xc9\x88", "j" , -1, -1},	/*  U+0248 - ?? - LATIN CAPITAL LETTER J WITH STROKE */
    { "\xc9\x89", "j" , -1, -1},	/*  U+0249 - ?? - LATIN SMALL LETTER J WITH STROKE */
    { "\xc9\x8a", "q" , -1, -1},	/*  U+024A - ?? - LATIN CAPITAL LETTER SMALL Q WITH HOOK TAIL */
    { "\xc9\x8b", "q" , -1, -1},	/*  U+024B - ?? - LATIN SMALL LETTER Q WITH HOOK TAIL */
    { "\xc9\x8c", "r" , -1, -1},	/*  U+024C - ?? - LATIN CAPITAL LETTER R WITH STROKE */
    { "\xc9\x8d", "r" , -1, -1},	/*  U+024D - ?? - LATIN SMALL LETTER R WITH STROKE */
    { "\xc9\x8e", "y" , -1, -1},	/*  U+024E - ?? - LATIN CAPITAL LETTER Y WITH STROKE */
    { "\xc9\x8f", "y" , -1, -1},	/*  U+024F - ?? - LATIN SMALL LETTER Y WITH STROKE */
    { "\xc9\x90", "a" , -1, -1},	/*  U+0250 - ?? - LATIN SMALL LETTER TURNED A */
    { "\xc9\x91", "a" , -1, -1},	/*  U+0251 - ?? - LATIN SMALL LETTER ALPHA */
    { "\xc9\x92", "a" , -1, -1},	/*  U+0252 - ?? - LATIN SMALL LETTER TURNED ALPHA */
    { "\xc9\x93", "g" , -1, -1},	/*  U+0253 - ?? - LATIN SMALL LETTER B WITH HOOK */
    { "\xc9\x94", "c" , -1, -1},	/*  U+0254 - ?? - LATIN SMALL LETTER OPEN O */
    { "\xc9\x95", "c" , -1, -1},	/*  U+0255 - ?? - LATIN SMALL LETTER C WITH CURL */
    { "\xc9\x96", "d" , -1, -1},	/*  U+0256 - ?? - LATIN SMALL LETTER D WITH TAIL */
    { "\xc9\x97", "d" , -1, -1},	/*  U+0257 - ?? - LATIN SMALL LETTER D WITH HOOK */
    { "\xc9\x98", "e" , -1, -1},	/*  U+0258 - ?? - LATIN SMALL LETTER REVERSED E */
    { "\xc9\x99", "e" , -1, -1},	/*  U+0259 - ?? - LATIN SMALL LETTER SCHWA */
    { "\xc9\x9a", "e" , -1, -1},	/*  U+025A - ?? - LATIN SMALL LETTER SCHWA WITH HOOK */
    { "\xc9\x9b", "e" , -1, -1},	/*  U+025B - ?? - LATIN SMALL LETTER OPEN E */
    { "\xc9\x9c", "e" , -1, -1},	/*  U+025C - ?? - LATIN SMALL LETTER REVERSED OPEN E */
    { "\xc9\x9d", "e" , -1, -1},	/*  U+025D - ?? - LATIN SMALL LETTER REVERSED OPEN E WITH HOOK */
    { "\xc9\x9e", "g" , -1, -1},	/*  U+025E - ?? - LATIN SMALL LETTER CLOSED REVERSED OPEN E */
    { "\xc9\x9f", "f" , -1, -1},	/*  U+025F - ?? - LATIN SMALL LETTER DOTLESS J WITH STROKE */
    { "\xc9\xa0", "g" , -1, -1},	/*  U+0260 - ?? - LATIN SMALL LETTER G WITH HOOK */
    { "\xc9\xa1", "g" , -1, -1},	/*  U+0261 - ?? - LATIN SMALL LETTER SCRIPT G */
    { "\xc9\xa2", "g" , -1, -1},	/*  U+0262 - ?? - LATIN LETTER SMALL CAPITAL G */
    { "\xc9\xa3", "y" , -1, -1},	/*  U+0263 - ?? - LATIN SMALL LETTER GAMMA */
    { "\xc9\xa4", "y" , -1, -1},	/*  U+0264 - ?? - LATIN SMALL LETTER RAMS HORN */
    { "\xc9\xa5", "u" , -1, -1},	/*  U+0265 - ?? - LATIN SMALL LETTER TURNED H */
    { "\xc9\xa6", "h" , -1, -1},	/*  U+0266 - ?? - LATIN SMALL LETTER H WITH HOOK */
    { "\xc9\xa7", "h" , -1, -1},	/*  U+0267 - ?? - LATIN SMALL LETTER HENG WITH HOOK */
    { "\xc9\xa8", "i" , -1, -1},	/*  U+0268 - ?? - LATIN SMALL LETTER I WITH STROKE */
    { "\xc9\xa9", "l" , -1, -1},	/*  U+0269 - ?? - LATIN SMALL LETTER IOTA */
    { "\xc9\xaa", "i" , -1, -1},	/*  U+026A - ?? - LATIN LETTER SMALL CAPITAL I */
    { "\xc9\xab", "l" , -1, -1},	/*  U+026B - ?? - LATIN SMALL LETTER L WITH MIDDLE TILDE */
    { "\xc9\xac", "l" , -1, -1},	/*  U+026C - ?? - LATIN SMALL LETTER L WITH BELT */
    { "\xc9\xad", "l" , -1, -1},	/*  U+026D - ?? - LATIN SMALL LETTER L WITH RETROFLEX HOOK */
    { "\xc9\xae", "l3" , -1, -1},	/*  U+026E - ?? - LATIN SMALL LETTER LEZH */
    { "\xc9\xaf", "w" , -1, -1},	/*  U+026F - ?? - LATIN SMALL LETTER TURNED M */
    { "\xc9\xb0", "w" , -1, -1},	/*  U+0270 - ?? - LATIN SMALL LETTER TURNED M WITH LONG LEG */
    { "\xc9\xb1", "m" , -1, -1},	/*  U+0271 - ?? - LATIN SMALL LETTER M WITH HOOK */
    { "\xc9\xb2", "n" , -1, -1},	/*  U+0272 - ?? - LATIN SMALL LETTER N WITH LEFT HOOK */
    { "\xc9\xb3", "n" , -1, -1},	/*  U+0273 - ?? - LATIN SMALL LETTER N WITH RETROFLEX HOOK */
    { "\xc9\xb4", "n" , -1, -1},	/*  U+0274 - ?? - LATIN LETTER SMALL CAPITAL N */
    { "\xc9\xb5", "o" , -1, -1},	/*  U+0275 - ?? - LATIN SMALL LETTER BARRED O */
    { "\xc9\xb6", "ae" , -1, -1},	/*  U+0276 - ?? - LATIN LETTER SMALL CAPITAL OE */
    { "\xc9\xb7", "n" , -1, -1},	/*  U+0277 - ?? - LATIN SMALL LETTER CLOSED OMEGA */
    { "\xc9\xb8", "o" , -1, -1},	/*  U+0278 - ?? - LATIN SMALL LETTER PHI */
    { "\xc9\xb9", "r" , -1, -1},	/*  U+0279 - ?? - LATIN SMALL LETTER TURNED R */
    { "\xc9\xba", "r" , -1, -1},	/*  U+027A - ?? - LATIN SMALL LETTER TURNED R WITH LONG LEG */
    { "\xc9\xbb", "r" , -1, -1},	/*  U+027B - ?? - LATIN SMALL LETTER TURNED R WITH HOOK */
    { "\xc9\xbc", "r" , -1, -1},	/*  U+027C - ?? - LATIN SMALL LETTER R WITH LONG LEG */
    { "\xc9\xbd", "t" , -1, -1},	/*  U+027D - ?? - LATIN SMALL LETTER R WITH TAIL */
    { "\xc9\xbe", "r" , -1, -1},	/*  U+027E - ?? - LATIN SMALL LETTER R WITH FISHHOOK */
    { "\xc9\xbf", "r" , -1, -1},	/*  U+027F - ?? - LATIN SMALL LETTER REVERSED R WITH FISHHOOK */
    { "\xca\x80", "r" , -1, -1},	/*  U+0280 - ?? - LATIN LETTER SMALL CAPITAL R */
    { "\xca\x81", "r" , -1, -1},	/*  U+0281 - ?? - LATIN LETTER SMALL CAPITAL INVERTED R */
    { "\xca\x82", "e" , -1, -1},	/*  U+0282 - ?? - LATIN SMALL LETTER S WITH HOOK */
    { "\xca\x83", "f" , -1, -1},	/*  U+0283 - ?? - LATIN SMALL LETTER ESH */
    { "\xca\x84", "f" , -1, -1},	/*  U+0284 - ?? - LATIN SMALL LETTER DOTLESS J WITH STROKE AND HOOK */
    { "\xca\x85", "l" , -1, -1},	/*  U+0285 - ?? - LATIN SMALL LETTER SQUAT REVERSED ESH */
    { "\xca\x86", "f" , -1, -1},	/*  U+0286 - ?? - LATIN SMALL LETTER ESH WITH CURL */
    { "\xca\x87", "t" , -1, -1},	/*  U+0287 - ?? - LATIN SMALL LETTER TURNED T */
    { "\xca\x88", "t" , -1, -1},	/*  U+0288 - ?? - LATIN SMALL LETTER T WITH RETROFLEX HOOK */
    { "\xca\x89", "u" , -1, -1},	/*  U+0289 - ?? - LATIN SMALL LETTER U BAR */
    { "\xca\x8a", "u" , -1, -1},	/*  U+028A - ?? - LATIN SMALL LETTER UPSILON */
    { "\xca\x8b", "v" , -1, -1},	/*  U+028B - ?? - LATIN SMALL LETTER V WITH HOOK */
    { "\xca\x8c", "a" , -1, -1},	/*  U+028C - ?? - LATIN SMALL LETTER TURNED V */
    { "\xca\x8d", "m" , -1, -1},	/*  U+028D - ?? - LATIN SMALL LETTER TURNED W */
    { "\xca\x8e", "y" , -1, -1},	/*  U+028E - ?? - LATIN SMALL LETTER TURNED Y */
    { "\xca\x8f", "y" , -1, -1},	/*  U+028F - ?? - LATIN LETTER SMALL CAPITAL Y */
    { "\xca\x90", "z" , -1, -1},	/*  U+0290 - ?? - LATIN SMALL LETTER Z WITH RETROFLEX HOOK */
    { "\xca\x91", "z" , -1, -1},	/*  U+0291 - ?? - LATIN SMALL LETTER Z WITH CURL */
    { "\xca\x92", "3" , -1, -1},	/*  U+0292 - ?? - LATIN SMALL LETTER EZH */
    { "\xca\x93", "3" , -1, -1},	/*  U+0293 - ?? - LATIN SMALL LETTER EZH WITH CURL */
    { "\xca\x94", "" , -1, -1},		/*  U+0294 - ?? - LATIN LETTER GLOTTAL STOP */
    { "\xca\x95", "" , -1, -1},		/*  U+0295 - ?? - LATIN LETTER PHARYNGEAL VOICED FRICATIVE */
    { "\xca\x96", "" , -1, -1},		/*  U+0296 - ?? - LATIN LETTER INVERTED GLOTTAL STOP */
    { "\xca\x97", "c" , -1, -1},	/*  U+0297 - ?? - LATIN LETTER STRETCHED C */
    { "\xca\x98", "0" , -1, -1},	/*  U+0298 - ?? - LATIN LETTER BILABIAL CLICK */
    { "\xca\x99", "b" , -1, -1},	/*  U+0299 - ?? - LATIN LETTER SMALL CAPITAL B */
    { "\xca\x9a", "g" , -1, -1},	/*  U+029A - ?? - LATIN SMALL LETTER CLOSED OPEN E */
    { "\xca\x9b", "g" , -1, -1},	/*  U+029B - ?? - LATIN LETTER SMALL CAPITAL G WITH HOOK */
    { "\xca\x9c", "h" , -1, -1},	/*  U+029C - ?? - LATIN LETTER SMALL CAPITAL H */
    { "\xca\x9d", "j" , -1, -1},	/*  U+029D - ?? - LATIN SMALL LETTER J WITH CROSSED-TAIL */
    { "\xca\x9e", "k" , -1, -1},	/*  U+029E - ?? - LATIN SMALL LETTER TURNED K */
    { "\xca\x9f", "l" , -1, -1},	/*  U+029F - ?? - LATIN LETTER SMALL CAPITAL L */
    { "\xca\xa0", "p" , -1, -1},	/*  U+02A0 - ?? - LATIN SMALL LETTER Q WITH HOOK */
    { "\xca\xa1", "" , -1, -1},		/*  U+02A1 - ?? - LATIN LETTER GLOTTAL STOP WITH STROKE */
    { "\xca\xa2", "" , -1, -1},		/*  U+02A2 - ?? - LATIN LETTER REVERSED GLOTTAL STOP WITH STROKE */
    { "\xca\xa3", "dz" , -1, -1},	/*  U+02A3 - ?? - LATIN SMALL LETTER DZ DIGRAPH */
    { "\xca\xa4", "dz" , -1, -1},	/*  U+02A4 - ?? - LATIN SMALL LETTER DEZH DIGRAPH */
    { "\xca\xa5", "dz" , -1, -1},	/*  U+02A5 - ?? - LATIN SMALL LETTER DZ DIGRAPH WITH CURL */
    { "\xca\xa6", "ts" , -1, -1},	/*  U+02A6 - ?? - LATIN SMALL LETTER TS DIGRAPH */
    { "\xca\xa7", "tf" , -1, -1},	/*  U+02A7 - ?? - LATIN SMALL LETTER TESH DIGRAPH */
    { "\xca\xa8", "tc" , -1, -1},	/*  U+02A8 - ?? - LATIN SMALL LETTER TC DIGRAPH WITH CURL */
    { "\xca\xa9", "fn" , -1, -1},	/*  U+02A9 - ?? - LATIN SMALL LETTER FENG DIGRAPH */
    { "\xca\xaa", "ls" , -1, -1},	/*  U+02AA - ?? - LATIN SMALL LETTER LS DIGRAPH */
    { "\xca\xab", "lz" , -1, -1},	/*  U+02AB - ?? - LATIN SMALL LETTER LZ DIGRAPH */
    { "\xca\xac", "ww" , -1, -1},	/*  U+02AC - ?? - LATIN LETTER BILABIAL PERCUSSIVE */
    { "\xca\xad", "nn" , -1, -1},	/*  U+02AD - ?? - LATIN LETTER BIDENTAL PERCUSSIVE */
    { "\xca\xae", "y" , -1, -1},	/*  U+02AE - ?? - LATIN SMALL LETTER TURNED H WITH FISHHOOK */
    { "\xca\xaf", "y" , -1, -1},	/*  U+02AF - ?? - LATIN SMALL LETTER TURNED H WITH FISHHOOK AND TAIL */
    { "\xca\xb0", "" , -1, -1},		/*  U+02B0 - ?? - MODIFIER LETTER SMALL H */
    { "\xca\xb1", "" , -1, -1},		/*  U+02B1 - ?? - MODIFIER LETTER SMALL H WITH HOOK */
    { "\xca\xb2", "" , -1, -1},		/*  U+02B2 - ?? - MODIFIER LETTER SMALL J */
    { "\xca\xb3", "" , -1, -1},		/*  U+02B3 - ?? - MODIFIER LETTER SMALL R */
    { "\xca\xb4", "" , -1, -1},		/*  U+02B4 - ?? - MODIFIER LETTER SMALL TURNED R */
    { "\xca\xb5", "" , -1, -1},		/*  U+02B5 - ?? - MODIFIER LETTER SMALL TURNED R WITH HOOK */
    { "\xca\xb6", "" , -1, -1},		/*  U+02B6 - ?? - MODIFIER LETTER SMALL CAPITAL INVERTED R */
    { "\xca\xb7", "" , -1, -1},		/*  U+02B7 - ?? - MODIFIER LETTER SMALL W */
    { "\xca\xb8", "" , -1, -1},		/*  U+02B8 - ?? - MODIFIER LETTER SMALL Y */
    { "\xca\xb9", "" , -1, -1},		/*  U+02B9 - ?? - MODIFIER LETTER PRIME */
    { "\xca\xba", "" , -1, -1},		/*  U+02BA - ?? - MODIFIER LETTER DOUBLE PRIME */
    { "\xca\xbb", "" , -1, -1},		/*  U+02BB - ?? - MODIFIER LETTER TURNED COMMA */
    { "\xca\xbc", "" , -1, -1},		/*  U+02BC - ?? - MODIFIER LETTER APOSTROPHE */
    { "\xca\xbd", "" , -1, -1},		/*  U+02BD - ?? - MODIFIER LETTER REVERSED COMMA */
    { "\xca\xbe", "" , -1, -1},		/*  U+02BE - ?? - MODIFIER LETTER RIGHT HALF RING */
    { "\xca\xbf", "" , -1, -1},		/*  U+02BF - ?? - MODIFIER LETTER LEFT HALF RING */
    { "\xcb\x80", "" , -1, -1},		/*  U+02C0 - ?? - MODIFIER LETTER GLOTTAL STOP */
    { "\xcb\x81", "" , -1, -1},		/*  U+02C1 - ?? - MODIFIER LETTER REVERSED GLOTTAL STOP */
    { "\xcb\x82", "" , -1, -1},		/*  U+02C2 - ?? - MODIFIER LETTER LEFT ARROWHEAD */
    { "\xcb\x83", "" , -1, -1},		/*  U+02C3 - ?? - MODIFIER LETTER RIGHT ARROWHEAD */
    { "\xcb\x84", "" , -1, -1},		/*  U+02C4 - ?? - MODIFIER LETTER UP ARROWHEAD */
    { "\xcb\x85", "" , -1, -1},		/*  U+02C5 - ?? - MODIFIER LETTER DOWN ARROWHEAD */
    { "\xcb\x86", "" , -1, -1},		/*  U+02C6 - ?? - MODIFIER LETTER CIRCUMFLEX ACCENT */
    { "\xcb\x87", "" , -1, -1},		/*  U+02C7 - ?? - CARON */
    { "\xcb\x88", "" , -1, -1},		/*  U+02C8 - ?? - MODIFIER LETTER VERTICAL LINE */
    { "\xcb\x89", "" , -1, -1},		/*  U+02C9 - ?? - MODIFIER LETTER MACRON */
    { "\xcb\x8a", "" , -1, -1},		/*  U+02CA - ?? - MODIFIER LETTER ACUTE ACCENT */
    { "\xcb\x8b", "" , -1, -1},		/*  U+02CB - ?? - MODIFIER LETTER GRAVE ACCENT */
    { "\xcb\x8c", "" , -1, -1},		/*  U+02CC - ?? - MODIFIER LETTER LOW VERTICAL LINE */
    { "\xcb\x8d", "" , -1, -1},		/*  U+02CD - ?? - MODIFIER LETTER LOW MACRON */
    { "\xcb\x8e", "" , -1, -1},		/*  U+02CE - ?? - MODIFIER LETTER LOW GRAVE ACCENT */
    { "\xcb\x8f", "" , -1, -1},		/*  U+02CF - ?? - MODIFIER LETTER LOW ACUTE ACCENT */
    { "\xcb\x90", "" , -1, -1},		/*  U+02D0 - ?? - MODIFIER LETTER TRIANGULAR COLON */
    { "\xcb\x91", "" , -1, -1},		/*  U+02D1 - ?? - MODIFIER LETTER HALF TRIANGULAR COLON */
    { "\xcb\x92", "" , -1, -1},		/*  U+02D2 - ?? - MODIFIER LETTER CENTRED RIGHT HALF RING */
    { "\xcb\x93", "" , -1, -1},		/*  U+02D3 - ?? - MODIFIER LETTER CENTRED LEFT HALF RING */
    { "\xcb\x94", "" , -1, -1},		/*  U+02D4 - ?? - MODIFIER LETTER UP TACK */
    { "\xcb\x95", "" , -1, -1},		/*  U+02D5 - ?? - MODIFIER LETTER DOWN TACK */
    { "\xcb\x96", "" , -1, -1},		/*  U+02D6 - ?? - MODIFIER LETTER PLUS SIGN */
    { "\xcb\x97", "" , -1, -1},		/*  U+02D7 - ?? - MODIFIER LETTER MINUS SIGN */
    { "\xcb\x98", "" , -1, -1},		/*  U+02D8 - ?? - BREVE */
    { "\xcb\x99", "" , -1, -1},		/*  U+02D9 - ?? - DOT ABOVE */
    { "\xcb\x9a", "" , -1, -1},		/*  U+02DA - ?? - RING ABOVE */
    { "\xcb\x9b", "" , -1, -1},		/*  U+02DB - ?? - OGONEK */
    { "\xcb\x9c", "" , -1, -1},		/*  U+02DC - ?? - SMALL TILDE */
    { "\xcb\x9d", "" , -1, -1},		/*  U+02DD - ?? - DOUBLE ACUTE ACCENT */
    { "\xcb\x9e", "" , -1, -1},		/*  U+02DE - ?? - MODIFIER LETTER RHOTIC HOOK */
    { "\xcb\x9f", "" , -1, -1},		/*  U+02DF - ?? - MODIFIER LETTER CROSS ACCENT */
    { "\xcb\xa0", "" , -1, -1},		/*  U+02E0 - ?? - MODIFIER LETTER SMALL GAMMA */
    { "\xcb\xa1", "" , -1, -1},		/*  U+02E1 - ?? - MODIFIER LETTER SMALL L */
    { "\xcb\xa2", "" , -1, -1},		/*  U+02E2 - ?? - MODIFIER LETTER SMALL S */
    { "\xcb\xa3", "" , -1, -1},		/*  U+02E3 - ?? - MODIFIER LETTER SMALL X */
    { "\xcb\xa4", "" , -1, -1},		/*  U+02E4 - ?? - MODIFIER LETTER SMALL REVERSED GLOTTAL STOP */
    { "\xcb\xa5", "" , -1, -1},		/*  U+02E5 - ?? - MODIFIER LETTER EXTRA-HIGH TONE BAR */
    { "\xcb\xa6", "" , -1, -1},		/*  U+02E6 - ?? - MODIFIER LETTER HIGH TONE BAR */
    { "\xcb\xa7", "" , -1, -1},		/*  U+02E7 - ?? - MODIFIER LETTER MID TONE BAR */
    { "\xcb\xa8", "" , -1, -1},		/*  U+02E8 - ?? - MODIFIER LETTER LOW TONE BAR */
    { "\xcb\xa9", "" , -1, -1},		/*  U+02E9 - ?? - MODIFIER LETTER EXTRA-LOW TONE BAR */
    { "\xcb\xaa", "" , -1, -1},		/*  U+02EA - ?? - MODIFIER LETTER YIN DEPARTING TONE MARK */
    { "\xcb\xab", "" , -1, -1},		/*  U+02EB - ?? - MODIFIER LETTER YANG DEPARTING TONE MARK */
    { "\xcb\xac", "" , -1, -1},		/*  U+02EC - ?? - MODIFIER LETTER VOICING */
    { "\xcb\xad", "" , -1, -1},		/*  U+02ED - ?? - MODIFIER LETTER UNASPIRATED */
    { "\xcb\xae", "" , -1, -1},		/*  U+02EE - ?? - MODIFIER LETTER DOUBLE APOSTROPHE */
    { "\xcb\xaf", "" , -1, -1},		/*  U+02EF - ?? - MODIFIER LETTER LOW DOWN ARROWHEAD */
    { "\xcb\xb0", "" , -1, -1},		/*  U+02F0 - ?? - MODIFIER LETTER LOW UP ARROWHEAD */
    { "\xcb\xb1", "" , -1, -1},		/*  U+02F1 - ?? - MODIFIER LETTER LOW LEFT ARROWHEAD */
    { "\xcb\xb2", "" , -1, -1},		/*  U+02F2 - ?? - MODIFIER LETTER LOW RIGHT ARROWHEAD */
    { "\xcb\xb3", "" , -1, -1},		/*  U+02F3 - ?? - MODIFIER LETTER LOW RING */
    { "\xcb\xb4", "" , -1, -1},		/*  U+02F4 - ?? - MODIFIER LETTER MIDDLE GRAVE ACCENT */
    { "\xcb\xb5", "" , -1, -1},		/*  U+02F5 - ?? - MODIFIER LETTER MIDDLE DOUBLE GRAVE ACCENT */
    { "\xcb\xb6", "" , -1, -1},		/*  U+02F6 - ?? - MODIFIER LETTER MIDDLE DOUBLE ACUTE ACCENT */
    { "\xcb\xb7", "" , -1, -1},		/*  U+02F7 - ?? - MODIFIER LETTER LOW TILDE */
    { "\xcb\xb8", "" , -1, -1},		/*  U+02F8 - ?? - MODIFIER LETTER RAISED COLON */
    { "\xcb\xb9", "" , -1, -1},		/*  U+02F9 - ?? - MODIFIER LETTER BEGIN HIGH TONE */
    { "\xcb\xba", "" , -1, -1},		/*  U+02FA - ?? - MODIFIER LETTER END HIGH TONE */
    { "\xcb\xbb", "" , -1, -1},		/*  U+02FB - ?? - MODIFIER LETTER BEGIN LOW TONE */
    { "\xcb\xbc", "" , -1, -1},		/*  U+02FC - ?? - MODIFIER LETTER END LOW TONE */
    { "\xcb\xbd", "" , -1, -1},		/*  U+02FD - ?? - MODIFIER LETTER SHELF */
    { "\xcb\xbe", "" , -1, -1},		/*  U+02FE - ?? - MODIFIER LETTER OPEN SHELF */
    { "\xcb\xbf", "" , -1, -1},		/*  U+02FF - ?? - MODIFIER LETTER LOW LEFT ARROW */
    { "\xcc\x80", "" , -1, -1},		/*  U+0300 - >?? - COMBINING GRAVE ACCENT */
    { "\xcc\x81", "" , -1, -1},		/*  U+0301 - >?? - COMBINING ACUTE ACCENT */
    { "\xcc\x82", "" , -1, -1},		/*  U+0302 - >?? - COMBINING CIRCUMFLEX ACCENT */
    { "\xcc\x83", "" , -1, -1},		/*  U+0303 - >?? - COMBINING TILDE */
    { "\xcc\x84", "" , -1, -1},		/*  U+0304 - >?? - COMBINING MACRON */
    { "\xcc\x85", "" , -1, -1},		/*  U+0305 - >?? - COMBINING OVERLINE */
    { "\xcc\x86", "" , -1, -1},		/*  U+0306 - >?? - COMBINING BREVE */
    { "\xcc\x87", "" , -1, -1},		/*  U+0307 - >?? - COMBINING DOT ABOVE */
    { "\xcc\x88", "" , -1, -1},		/*  U+0308 - >?? - COMBINING DIAERESIS */
    { "\xcc\x89", "" , -1, -1},		/*  U+0309 - >?? - COMBINING HOOK ABOVE */
    { "\xcc\x8a", "" , -1, -1},		/*  U+030A - >?? - COMBINING RING ABOVE */
    { "\xcc\x8b", "" , -1, -1},		/*  U+030B - >?? - COMBINING DOUBLE ACUTE ACCENT */
    { "\xcc\x8c", "" , -1, -1},		/*  U+030C - >?? - COMBINING CARON */
    { "\xcc\x8d", "" , -1, -1},		/*  U+030D - >?? - COMBINING VERTICAL LINE ABOVE */
    { "\xcc\x8e", "" , -1, -1},		/*  U+030E - >?? - COMBINING DOUBLE VERTICAL LINE ABOVE */
    { "\xcc\x8f", "" , -1, -1},		/*  U+030F - >?? - COMBINING DOUBLE GRAVE ACCENT */
    { "\xcc\x90", "" , -1, -1},		/*  U+0310 - >?? - COMBINING CANDRABINDU */
    { "\xcc\x91", "" , -1, -1},		/*  U+0311 - >?? - COMBINING INVERTED BREVE */
    { "\xcc\x92", "" , -1, -1},		/*  U+0312 - >?? - COMBINING TURNED COMMA ABOVE */
    { "\xcc\x93", "" , -1, -1},		/*  U+0313 - >?? - COMBINING COMMA ABOVE */
    { "\xcc\x94", "" , -1, -1},		/*  U+0314 - >?? - COMBINING REVERSED COMMA ABOVE */
    { "\xcc\x95", "" , -1, -1},		/*  U+0315 - >?? - COMBINING COMMA ABOVE RIGHT */
    { "\xcc\x96", "" , -1, -1},		/*  U+0316 - >?? - COMBINING GRAVE ACCENT BELOW */
    { "\xcc\x97", "" , -1, -1},		/*  U+0317 - >?? - COMBINING ACUTE ACCENT BELOW */
    { "\xcc\x98", "" , -1, -1},		/*  U+0318 - >?? - COMBINING LEFT TACK BELOW */
    { "\xcc\x99", "" , -1, -1},		/*  U+0319 - >?? - COMBINING RIGHT TACK BELOW */
    { "\xcc\x9a", "" , -1, -1},		/*  U+031A - >?? - COMBINING LEFT ANGLE ABOVE */
    { "\xcc\x9b", "" , -1, -1},		/*  U+031B - >?? - COMBINING HORN */
    { "\xcc\x9c", "" , -1, -1},		/*  U+031C - >?? - COMBINING LEFT HALF RING BELOW */
    { "\xcc\x9d", "" , -1, -1},		/*  U+031D - >?? - COMBINING UP TACK BELOW */
    { "\xcc\x9e", "" , -1, -1},		/*  U+031E - >?? - COMBINING DOWN TACK BELOW */
    { "\xcc\x9f", "" , -1, -1},		/*  U+031F - >?? - COMBINING PLUS SIGN BELOW */
    { "\xcc\xa0", "" , -1, -1},		/*  U+0320 - >?? - COMBINING MINUS SIGN BELOW */
    { "\xcc\xa1", "" , -1, -1},		/*  U+0321 - >?? - COMBINING PALATALIZED HOOK BELOW */
    { "\xcc\xa2", "" , -1, -1},		/*  U+0322 - >?? - COMBINING RETROFLEX HOOK BELOW */
    { "\xcc\xa3", "" , -1, -1},		/*  U+0323 - >?? - COMBINING DOT BELOW */
    { "\xcc\xa4", "" , -1, -1},		/*  U+0324 - >?? - COMBINING DIAERESIS BELOW */
    { "\xcc\xa5", "" , -1, -1},		/*  U+0325 - >?? - COMBINING RING BELOW */
    { "\xcc\xa6", "" , -1, -1},		/*  U+0326 - >?? - COMBINING COMMA BELOW */
    { "\xcc\xa7", "" , -1, -1},		/*  U+0327 - >?? - COMBINING CEDILLA */
    { "\xcc\xa8", "" , -1, -1},		/*  U+0328 - >?? - COMBINING OGONEK */
    { "\xcc\xa9", "" , -1, -1},		/*  U+0329 - >?? - COMBINING VERTICAL LINE BELOW */
    { "\xcc\xaa", "" , -1, -1},		/*  U+032A - >?? - COMBINING BRIDGE BELOW */
    { "\xcc\xab", "" , -1, -1},		/*  U+032B - >?? - COMBINING INVERTED DOUBLE ARCH BELOW */
    { "\xcc\xac", "" , -1, -1},		/*  U+032C - >?? - COMBINING CARON BELOW */
    { "\xcc\xad", "" , -1, -1},		/*  U+032D - >?? - COMBINING CIRCUMFLEX ACCENT BELOW */
    { "\xcc\xae", "" , -1, -1},		/*  U+032E - >?? - COMBINING BREVE BELOW */
    { "\xcc\xaf", "" , -1, -1},		/*  U+032F - >?? - COMBINING INVERTED BREVE BELOW */
    { "\xcc\xb0", "" , -1, -1},		/*  U+0330 - >?? - COMBINING TILDE BELOW */
    { "\xcc\xb1", "" , -1, -1},		/*  U+0331 - >?? - COMBINING MACRON BELOW */
    { "\xcc\xb2", "" , -1, -1},		/*  U+0332 - >?? - COMBINING LOW LINE */
    { "\xcc\xb3", "" , -1, -1},		/*  U+0333 - >?? - COMBINING DOUBLE LOW LINE */
    { "\xcc\xb4", "" , -1, -1},		/*  U+0334 - >?? - COMBINING TILDE OVERLAY */
    { "\xcc\xb5", "" , -1, -1},		/*  U+0335 - >?? - COMBINING SHORT STROKE OVERLAY */
    { "\xcc\xb6", "" , -1, -1},		/*  U+0336 - >?? - COMBINING LONG STROKE OVERLAY */
    { "\xcc\xb7", "" , -1, -1},		/*  U+0337 - >?? - COMBINING SHORT SOLIDUS OVERLAY */
    { "\xcc\xb8", "" , -1, -1},		/*  U+0338 - >?? - COMBINING LONG SOLIDUS OVERLAY */
    { "\xcc\xb9", "" , -1, -1},		/*  U+0339 - >?? - COMBINING RIGHT HALF RING BELOW */
    { "\xcc\xba", "" , -1, -1},		/*  U+033A - >?? - COMBINING INVERTED BRIDGE BELOW */
    { "\xcc\xbb", "" , -1, -1},		/*  U+033B - >?? - COMBINING SQUARE BELOW */
    { "\xcc\xbc", "" , -1, -1},		/*  U+033C - >?? - COMBINING SEAGULL BELOW */
    { "\xcc\xbd", "" , -1, -1},		/*  U+033D - >?? - COMBINING X ABOVE */
    { "\xcc\xbe", "" , -1, -1},		/*  U+033E - >?? - COMBINING VERTICAL TILDE */
    { "\xcc\xbf", "" , -1, -1},		/*  U+033F - >?? - COMBINING DOUBLE OVERLINE */
    { "\xcd\x80", "" , -1, -1},		/*  U+0340 - >?? - COMBINING GRAVE TONE MARK */
    { "\xcd\x81", "" , -1, -1},		/*  U+0341 - >?? - COMBINING ACUTE TONE MARK */
    { "\xcd\x82", "" , -1, -1},		/*  U+0342 - >?? - COMBINING GREEK PERISPOMENI */
    { "\xcd\x83", "" , -1, -1},		/*  U+0343 - >?? - COMBINING GREEK KORONIS */
    { "\xcd\x84", "" , -1, -1},		/*  U+0344 - >?? - COMBINING GREEK DIALYTIKA TONOS */
    { "\xcd\x85", "" , -1, -1},		/*  U+0345 - >?? - COMBINING GREEK YPOGEGRAMMENI */
    { "\xcd\x86", "" , -1, -1},		/*  U+0346 - >?? - COMBINING BRIDGE ABOVE */
    { "\xcd\x87", "" , -1, -1},		/*  U+0347 - >?? - COMBINING EQUALS SIGN BELOW */
    { "\xcd\x88", "" , -1, -1},		/*  U+0348 - >?? - COMBINING DOUBLE VERTICAL LINE BELOW */
    { "\xcd\x89", "" , -1, -1},		/*  U+0349 - >?? - COMBINING LEFT ANGLE BELOW */
    { "\xcd\x8a", "" , -1, -1},		/*  U+034A - >?? - COMBINING NOT TILDE ABOVE */
    { "\xcd\x8b", "" , -1, -1},		/*  U+034B - >?? - COMBINING HOMOTHETIC ABOVE */
    { "\xcd\x8c", "" , -1, -1},		/*  U+034C - >?? - COMBINING ALMOST EQUAL TO ABOVE */
    { "\xcd\x8d", "" , -1, -1},		/*  U+034D - >?? - COMBINING LEFT RIGHT ARROW BELOW */
    { "\xcd\x8e", "" , -1, -1},		/*  U+034E - >?? - COMBINING UPWARDS ARROW BELOW */
    { "\xcd\x8f", "" , -1, -1},		/*  U+034F - >?? - COMBINING GRAPHEME JOINER */
    { "\xcd\x90", "" , -1, -1},		/*  U+0350 - >?? - COMBINING RIGHT ARROWHEAD ABOVE */
    { "\xcd\x91", "" , -1, -1},		/*  U+0351 - >?? - COMBINING LEFT HALF RING ABOVE */
    { "\xcd\x92", "" , -1, -1},		/*  U+0352 - >?? - COMBINING FERMATA */
    { "\xcd\x93", "" , -1, -1},		/*  U+0353 - >?? - COMBINING X BELOW */
    { "\xcd\x94", "" , -1, -1},		/*  U+0354 - >?? - COMBINING LEFT ARROWHEAD BELOW */
    { "\xcd\x95", "" , -1, -1},		/*  U+0355 - >?? - COMBINING RIGHT ARROWHEAD BELOW */
    { "\xcd\x96", "" , -1, -1},		/*  U+0356 - >?? - COMBINING RIGHT ARROWHEAD AND UP ARROWHEAD BELOW */
    { "\xcd\x97", "" , -1, -1},		/*  U+0357 - >?? - COMBINING RIGHT HALF RING ABOVE */
    { "\xcd\x98", "" , -1, -1},		/*  U+0358 - >?? - COMBINING DOT ABOVE RIGHT */
    { "\xcd\x99", "" , -1, -1},		/*  U+0359 - >?? - COMBINING ASTERISK BELOW */
    { "\xcd\x9a", "" , -1, -1},		/*  U+035A - >?? - COMBINING DOUBLE RING BELOW */
    { "\xcd\x9b", "" , -1, -1},		/*  U+035B - >?? - COMBINING ZIGZAG ABOVE */
    { "\xcd\x9c", "" , -1, -1},		/*  U+035C - >?? - COMBINING DOUBLE BREVE BELOW */
    { "\xcd\x9d", "" , -1, -1},		/*  U+035D - >?? - COMBINING DOUBLE BREVE */
    { "\xcd\x9e", "" , -1, -1},		/*  U+035E - >?? - COMBINING DOUBLE MACRON */
    { "\xcd\x9f", "" , -1, -1},		/*  U+035F - >?? - COMBINING DOUBLE MACRON BELOW */
    { "\xcd\xa0", "" , -1, -1},		/*  U+0360 - >?? - COMBINING DOUBLE TILDE */
    { "\xcd\xa1", "" , -1, -1},		/*  U+0361 - >?? - COMBINING DOUBLE INVERTED BREVE */
    { "\xcd\xa2", "" , -1, -1},		/*  U+0362 - >?? - COMBINING DOUBLE RIGHTWARDS ARROW BELOW */
    { "\xcd\xa3", "" , -1, -1},		/*  U+0363 - >?? - COMBINING LATIN SMALL LETTER A */
    { "\xcd\xa4", "" , -1, -1},		/*  U+0364 - >?? - COMBINING LATIN SMALL LETTER E */
    { "\xcd\xa5", "" , -1, -1},		/*  U+0365 - >?? - COMBINING LATIN SMALL LETTER I */
    { "\xcd\xa6", "" , -1, -1},		/*  U+0366 - >?? - COMBINING LATIN SMALL LETTER O */
    { "\xcd\xa7", "" , -1, -1},		/*  U+0367 - >?? - COMBINING LATIN SMALL LETTER U */
    { "\xcd\xa8", "" , -1, -1},		/*  U+0368 - >?? - COMBINING LATIN SMALL LETTER C */
    { "\xcd\xa9", "" , -1, -1},		/*  U+0369 - >?? - COMBINING LATIN SMALL LETTER D */
    { "\xcd\xaa", "" , -1, -1},		/*  U+036A - >?? - COMBINING LATIN SMALL LETTER H */
    { "\xcd\xab", "" , -1, -1},		/*  U+036B - >?? - COMBINING LATIN SMALL LETTER M */
    { "\xcd\xac", "" , -1, -1},		/*  U+036C - >?? - COMBINING LATIN SMALL LETTER R */
    { "\xcd\xad", "" , -1, -1},		/*  U+036D - >?? - COMBINING LATIN SMALL LETTER T */
    { "\xcd\xae", "" , -1, -1},		/*  U+036E - >?? - COMBINING LATIN SMALL LETTER V */
    { "\xcd\xaf", "" , -1, -1},		/*  U+036F - >?? - COMBINING LATIN SMALL LETTER X */
    { "\xcd\xb0", "i" , -1, -1},	/*  U+0370 - ?? - GREEK CAPITAL LETTER HETA */
    { "\xcd\xb1", "i" , -1, -1},	/*  U+0371 - ?? - GREEK SMALL LETTER HETA */
    { "\xcd\xb2", "t" , -1, -1},	/*  U+0372 - ?? - GREEK CAPITAL LETTER ARCHAIC SAMPI */
    { "\xcd\xb3", "r" , -1, -1},	/*  U+0373 - ?? - GREEK SMALL LETTER ARCHAIC SAMPI */
    { "\xcd\xb4", "" , -1, -1},		/*  U+0374 - ?? - GREEK NUMERAL SIGN */
    { "\xcd\xb5", "" , -1, -1},		/*  U+0375 - ?? - GREEK LOWER NUMERAL SIGN */
    { "\xcd\xb6", "n" , -1, -1},	/*  U+0376 - ?? - GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA */
    { "\xcd\xb7", "n" , -1, -1},	/*  U+0377 - ?? - GREEK SMALL LETTER PAMPHYLIAN DIGAMMA */
    { "\xcd\xb8", "" , -1, -1},		/*  U+0378 - ?? -  */
    { "\xcd\xb9", "" , -1, -1},		/*  U+0379 - ?? -  */
    { "\xcd\xba", "." , -1, -1},	/*  U+037A - ?? - GREEK YPOGEGRAMMENI */
    { "\xcd\xbb", "c" , -1, -1},	/*  U+037B - ?? - GREEK SMALL REVERSED LUNATE SIGMA SYMBOL */
    { "\xcd\xbc", "c" , -1, -1},	/*  U+037C - ?? - GREEK SMALL DOTTED LUNATE SIGMA SYMBOL */
    { "\xcd\xbd", "c" , -1, -1},	/*  U+037D - ?? - GREEK SMALL REVERSED DOTTED LUNATE SIGMA SYMBOL */
    { "\xcd\xbe", "" , -1, -1},		/*  U+037E - ?? - GREEK QUESTION MARK */
    { "\xcd\xbf", "j" , -1, -1},	/*  U+037F - ?? - GREEK CAPITAL LETTER YOT */
    { "\xce\x80", "" , -1, -1},		/*  U+0380 - ?? -  */
    { "\xce\x81", "" , -1, -1},		/*  U+0381 - ?? -  */
    { "\xce\x82", "" , -1, -1},		/*  U+0382 - ?? -  */
    { "\xce\x83", "" , -1, -1},		/*  U+0383 - ?? -  */
    { "\xce\x84", "" , -1, -1},		/*  U+0384 - ?? - GREEK TONOS */
    { "\xce\x85", ".." , -1, -1},	/*  U+0385 - ?? - GREEK DIALYTIKA TONOS */
    { "\xce\x86", "a" , -1, -1},	/*  U+0386 - ?? - GREEK CAPITAL LETTER ALPHA WITH TONOS */
    { "\xce\x87", "." , -1, -1},	/*  U+0387 - ?? - GREEK ANO TELEIA */
    { "\xce\x88", "e" , -1, -1},	/*  U+0388 - ?? - GREEK CAPITAL LETTER EPSILON WITH TONOS */
    { "\xce\x89", "h" , -1, -1},	/*  U+0389 - ?? - GREEK CAPITAL LETTER ETA WITH TONOS */
    { "\xce\x8a", "i" , -1, -1},	/*  U+038A - ?? - GREEK CAPITAL LETTER IOTA WITH TONOS */
    { "\xce\x8b", "" , -1, -1},		/*  U+038B - ?? -  */
    { "\xce\x8c", "o" , -1, -1},	/*  U+038C - ?? - GREEK CAPITAL LETTER OMICRON WITH TONOS */
    { "\xce\x8d", "" , -1, -1},		/*  U+038D - ?? -  */
    { "\xce\x8e", "y" , -1, -1},	/*  U+038E - ?? - GREEK CAPITAL LETTER UPSILON WITH TONOS */
    { "\xce\x8f", "n" , -1, -1},	/*  U+038F - ?? - GREEK CAPITAL LETTER OMEGA WITH TONOS */
    { "\xce\x90", "l" , -1, -1},	/*  U+0390 - ?? - GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS */
    { "\xce\x91", "a" , -1, -1},	/*  U+0391 - ?? - GREEK CAPITAL LETTER ALPHA */
    { "\xce\x92", "b" , -1, -1},	/*  U+0392 - ?? - GREEK CAPITAL LETTER BETA */
    { "\xce\x93", "c" , -1, -1},	/*  U+0393 - ?? - GREEK CAPITAL LETTER GAMMA */
    { "\xce\x94", "d" , -1, -1},	/*  U+0394 - ?? - GREEK CAPITAL LETTER DELTA */
    { "\xce\x95", "e" , -1, -1},	/*  U+0395 - ?? - GREEK CAPITAL LETTER EPSILON */
    { "\xce\x96", "z" , -1, -1},	/*  U+0396 - ?? - GREEK CAPITAL LETTER ZETA */
    { "\xce\x97", "h" , -1, -1},	/*  U+0397 - ?? - GREEK CAPITAL LETTER ETA */
    { "\xce\x98", "o" , -1, -1},	/*  U+0398 - ?? - GREEK CAPITAL LETTER THETA */
    { "\xce\x99", "i" , -1, -1},	/*  U+0399 - ?? - GREEK CAPITAL LETTER IOTA */
    { "\xce\x9a", "k" , -1, -1},	/*  U+039A - ?? - GREEK CAPITAL LETTER KAPPA */
    { "\xce\x9b", "l" , -1, -1},	/*  U+039B - ?? - GREEK CAPITAL LETTER LAMBDA */
    { "\xce\x9c", "m" , -1, -1},	/*  U+039C - ?? - GREEK CAPITAL LETTER MU */
    { "\xce\x9d", "n" , -1, -1},	/*  U+039D - ?? - GREEK CAPITAL LETTER NU */
    { "\xce\x9e", "-" , -1, -1},	/*  U+039E - ?? - GREEK CAPITAL LETTER XI */
    { "\xce\x9f", "o" , -1, -1},	/*  U+039F - ?? - GREEK CAPITAL LETTER OMICRON */
    { "\xce\xa0", "p" , -1, -1},	/*  U+03A0 - ?? - GREEK CAPITAL LETTER PI */
    { "\xce\xa1", "r" , -1, -1},	/*  U+03A1 - ?? - GREEK CAPITAL LETTER RHO */
    { "\xce\xa2", "" , -1, -1},		/*  U+03A2 - ?? -  */
    { "\xce\xa3", "e" , -1, -1},	/*  U+03A3 - ?? - GREEK CAPITAL LETTER SIGMA */
    { "\xce\xa4", "t" , -1, -1},	/*  U+03A4 - ?? - GREEK CAPITAL LETTER TAU */
    { "\xce\xa5", "y" , -1, -1},	/*  U+03A5 - ?? - GREEK CAPITAL LETTER UPSILON */
    { "\xce\xa6", "o" , -1, -1},	/*  U+03A6 - ?? - GREEK CAPITAL LETTER PHI */
    { "\xce\xa7", "x" , -1, -1},	/*  U+03A7 - ?? - GREEK CAPITAL LETTER CHI */
    { "\xce\xa8", "y" , -1, -1},	/*  U+03A8 - ?? - GREEK CAPITAL LETTER PSI */
    { "\xce\xa9", "n" , -1, -1},	/*  U+03A9 - ?? - GREEK CAPITAL LETTER OMEGA */
    { "\xce\xaa", "i" , -1, -1},	/*  U+03AA - ?? - GREEK CAPITAL LETTER IOTA WITH DIALYTIKA */
    { "\xce\xab", "y" , -1, -1},	/*  U+03AB - ?? - GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA */
    { "\xce\xac", "a" , -1, -1},	/*  U+03AC - ?? - GREEK SMALL LETTER ALPHA WITH TONOS */
    { "\xce\xad", "e" , -1, -1},	/*  U+03AD - ?? - GREEK SMALL LETTER EPSILON WITH TONOS */
    { "\xce\xae", "n" , -1, -1},	/*  U+03AE - ?? - GREEK SMALL LETTER ETA WITH TONOS */
    { "\xce\xaf", "i" , -1, -1},	/*  U+03AF - ?? - GREEK SMALL LETTER IOTA WITH TONOS */
    { "\xce\xb0", "v" , -1, -1},	/*  U+03B0 - ?? - GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS */
    { "\xce\xb1", "a" , -1, -1},	/*  U+03B1 - ?? - GREEK SMALL LETTER ALPHA */
    { "\xce\xb2", "b" , -1, -1},	/*  U+03B2 - ?? - GREEK SMALL LETTER BETA */
    { "\xce\xb3", "y" , -1, -1},	/*  U+03B3 - ?? - GREEK SMALL LETTER GAMMA */
    { "\xce\xb4", "o" , -1, -1},	/*  U+03B4 - ?? - GREEK SMALL LETTER DELTA */
    { "\xce\xb5", "e" , -1, -1},	/*  U+03B5 - ?? - GREEK SMALL LETTER EPSILON */
    { "\xce\xb6", "z" , -1, -1},	/*  U+03B6 - ?? - GREEK SMALL LETTER ZETA */
    { "\xce\xb7", "n" , -1, -1},	/*  U+03B7 - ?? - GREEK SMALL LETTER ETA */
    { "\xce\xb8", "o" , -1, -1},	/*  U+03B8 - ?? - GREEK SMALL LETTER THETA */
    { "\xce\xb9", "l" , -1, -1},	/*  U+03B9 - ?? - GREEK SMALL LETTER IOTA */
    { "\xce\xba", "k" , -1, -1},	/*  U+03BA - ?? - GREEK SMALL LETTER KAPPA */
    { "\xce\xbb", "l" , -1, -1},	/*  U+03BB - ?? - GREEK SMALL LETTER LAMBDA */
    { "\xce\xbc", "u" , -1, -1},	/*  U+03BC - ?? - GREEK SMALL LETTER MU */
    { "\xce\xbd", "v" , -1, -1},	/*  U+03BD - ?? - GREEK SMALL LETTER NU */
    { "\xce\xbe", "e" , -1, -1},	/*  U+03BE - ?? - GREEK SMALL LETTER XI */
    { "\xce\xbf", "o" , -1, -1},	/*  U+03BF - ?? - GREEK SMALL LETTER OMICRON */
    { "\xcf\x80", "t" , -1, -1},	/*  U+03C0 - ?? - GREEK SMALL LETTER PI */
    { "\xcf\x81", "p" , -1, -1},	/*  U+03C1 - ?? - GREEK SMALL LETTER RHO */
    { "\xcf\x82", "c" , -1, -1},	/*  U+03C2 - ?? - GREEK SMALL LETTER FINAL SIGMA */
    { "\xcf\x83", "o" , -1, -1},	/*  U+03C3 - ?? - GREEK SMALL LETTER SIGMA */
    { "\xcf\x84", "t" , -1, -1},	/*  U+03C4 - ?? - GREEK SMALL LETTER TAU */
    { "\xcf\x85", "v" , -1, -1},	/*  U+03C5 - ?? - GREEK SMALL LETTER UPSILON */
    { "\xcf\x86", "o" , -1, -1},	/*  U+03C6 - ?? - GREEK SMALL LETTER PHI */
    { "\xcf\x87", "x" , -1, -1},	/*  U+03C7 - ?? - GREEK SMALL LETTER CHI */
    { "\xcf\x88", "w" , -1, -1},	/*  U+03C8 - ?? - GREEK SMALL LETTER PSI */
    { "\xcf\x89", "w" , -1, -1},	/*  U+03C9 - ?? - GREEK SMALL LETTER OMEGA */
    { "\xcf\x8a", "i" , -1, -1},	/*  U+03CA - ?? - GREEK SMALL LETTER IOTA WITH DIALYTIKA */
    { "\xcf\x8b", "v" , -1, -1},	/*  U+03CB - ?? - GREEK SMALL LETTER UPSILON WITH DIALYTIKA */
    { "\xcf\x8c", "o" , -1, -1},	/*  U+03CC - ?? - GREEK SMALL LETTER OMICRON WITH TONOS */
    { "\xcf\x8d", "v" , -1, -1},	/*  U+03CD - ?? - GREEK SMALL LETTER UPSILON WITH TONOS */
    { "\xcf\x8e", "w" , -1, -1},	/*  U+03CE - ?? - GREEK SMALL LETTER OMEGA WITH TONOS */
    { "\xcf\x8f", "k" , -1, -1},	/*  U+03CF - ?? - GREEK CAPITAL KAI SYMBOL */
    { "\xcf\x90", "b" , -1, -1},	/*  U+03D0 - ?? - GREEK BETA SYMBOL */
    { "\xcf\x91", "g" , -1, -1},	/*  U+03D1 - ?? - GREEK THETA SYMBOL */
    { "\xcf\x92", "y" , -1, -1},	/*  U+03D2 - ?? - GREEK UPSILON WITH HOOK SYMBOL */
    { "\xcf\x93", "y" , -1, -1},	/*  U+03D3 - ?? - GREEK UPSILON WITH ACUTE AND HOOK SYMBOL */
    { "\xcf\x94", "y" , -1, -1},	/*  U+03D4 - ?? - GREEK UPSILON WITH DIAERESIS AND HOOK SYMBOL */
    { "\xcf\x95", "o" , -1, -1},	/*  U+03D5 - ?? - GREEK PHI SYMBOL */
    { "\xcf\x96", "w" , -1, -1},	/*  U+03D6 - ?? - GREEK PI SYMBOL */
    { "\xcf\x97", "x" , -1, -1},	/*  U+03D7 - ?? - GREEK KAI SYMBOL */
    { "\xcf\x98", "o" , -1, -1},	/*  U+03D8 - ?? - GREEK LETTER ARCHAIC KOPPA */
    { "\xcf\x99", "o" , -1, -1},	/*  U+03D9 - ?? - GREEK SMALL LETTER ARCHAIC KOPPA */
    { "\xcf\x9a", "c" , -1, -1},	/*  U+03DA - ?? - GREEK LETTER STIGMA */
    { "\xcf\x9b", "c" , -1, -1},	/*  U+03DB - ?? - GREEK SMALL LETTER STIGMA */
    { "\xcf\x9c", "f" , -1, -1},	/*  U+03DC - ?? - GREEK LETTER DIGAMMA */
    { "\xcf\x9d", "f" , -1, -1},	/*  U+03DD - ?? - GREEK SMALL LETTER DIGAMMA */
    { "\xcf\x9e", "h" , -1, -1},	/*  U+03DE - ?? - GREEK LETTER KOPPA */
    { "\xcf\x9f", "z" , -1, -1},	/*  U+03DF - ?? - GREEK SMALL LETTER KOPPA */
    { "\xcf\xa0", "ti" , -1, -1},	/*  U+03E0 - ?? - GREEK LETTER SAMPI */
    { "\xcf\xa1", "y" , -1, -1},	/*  U+03E1 - ?? - GREEK SMALL LETTER SAMPI */
    { "\xcf\xa2", "w" , -1, -1},	/*  U+03E2 - ?? - COPTIC CAPITAL LETTER SHEI */
    { "\xcf\xa3", "w" , -1, -1},	/*  U+03E3 - ?? - COPTIC SMALL LETTER SHEI */
    { "\xcf\xa4", "q" , -1, -1},	/*  U+03E4 - ?? - COPTIC CAPITAL LETTER FEI */
    { "\xcf\xa5", "q" , -1, -1},	/*  U+03E5 - ?? - COPTIC SMALL LETTER FEI */
    { "\xcf\xa6", "b" , -1, -1},	/*  U+03E6 - ?? - COPTIC CAPITAL LETTER KHEI */
    { "\xcf\xa7", "e" , -1, -1},	/*  U+03E7 - ?? - COPTIC SMALL LETTER KHEI */
    { "\xcf\xa8", "8" , -1, -1},	/*  U+03E8 - ?? - COPTIC CAPITAL LETTER HORI */
    { "\xcf\xa9", "e" , -1, -1},	/*  U+03E9 - ?? - COPTIC SMALL LETTER HORI */
    { "\xcf\xaa", "x" , -1, -1},	/*  U+03EA - ?? - COPTIC CAPITAL LETTER GANGIA */
    { "\xcf\xab", "x" , -1, -1},	/*  U+03EB - ?? - COPTIC SMALL LETTER GANGIA */
    { "\xcf\xac", "o" , -1, -1},	/*  U+03EC - ?? - COPTIC CAPITAL LETTER SHIMA */
    { "\xcf\xad", "." , -1, -1},	/*  U+03ED - ?? - COPTIC SMALL LETTER SHIMA */
    { "\xcf\xae", "t" , -1, -1},	/*  U+03EE - ?? - COPTIC CAPITAL LETTER DEI */
    { "\xcf\xaf", "t" , -1, -1},	/*  U+03EF - ?? - COPTIC SMALL LETTER DEI */
    { "\xcf\xb0", "x" , -1, -1},	/*  U+03F0 - ?? - GREEK KAPPA SYMBOL */
    { "\xcf\xb1", "g" , -1, -1},	/*  U+03F1 - ?? - GREEK RHO SYMBOL */
    { "\xcf\xb2", "c" , -1, -1},	/*  U+03F2 - ?? - GREEK LUNATE SIGMA SYMBOL */
    { "\xcf\xb3", "j" , -1, -1},	/*  U+03F3 - ?? - GREEK LETTER YOT */
    { "\xcf\xb4", "o" , -1, -1},	/*  U+03F4 - ?? - GREEK CAPITAL THETA SYMBOL */
    { "\xcf\xb5", "e" , -1, -1},	/*  U+03F5 - ?? - GREEK LUNATE EPSILON SYMBOL */
    { "\xcf\xb6", "e" , -1, -1},	/*  U+03F6 - ?? - GREEK REVERSED LUNATE EPSILON SYMBOL */
    { "\xcf\xb7", "p" , -1, -1},	/*  U+03F7 - ?? - GREEK CAPITAL LETTER SHO */
    { "\xcf\xb8", "p" , -1, -1},	/*  U+03F8 - ?? - GREEK SMALL LETTER SHO */
    { "\xcf\xb9", "c" , -1, -1},	/*  U+03F9 - ?? - GREEK CAPITAL LUNATE SIGMA SYMBOL */
    { "\xcf\xba", "m" , -1, -1},	/*  U+03FA - ?? - GREEK CAPITAL LETTER SAN */
    { "\xcf\xbb", "m" , -1, -1},	/*  U+03FB - ?? - GREEK SMALL LETTER SAN */
    { "\xcf\xbc", "p" , -1, -1},	/*  U+03FC - ?? - GREEK RHO WITH STROKE SYMBOL */
    { "\xcf\xbd", "c" , -1, -1},	/*  U+03FD - ?? - GREEK CAPITAL REVERSED LUNATE SIGMA SYMBOL */
    { "\xcf\xbe", "e" , -1, -1},	/*  U+03FE - ?? - GREEK CAPITAL DOTTED LUNATE SIGMA SYMBOL */
    { "\xcf\xbf", "e" , -1, -1},	/*  U+03FF - ?? - GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL */
    { "\xd0\x80", "e" , -1, -1},	/*  U+0400 - ?? - CYRILLIC CAPITAL LETTER IE WITH GRAVE */
    { "\xd0\x81", "e" , -1, -1},	/*  U+0401 - ?? - CYRILLIC CAPITAL LETTER IO */
    { "\xd0\x82", "h" , -1, -1},	/*  U+0402 - ?? - CYRILLIC CAPITAL LETTER DJE */
    { "\xd0\x83", "t" , -1, -1},	/*  U+0403 - ?? - CYRILLIC CAPITAL LETTER GJE */
    { "\xd0\x84", "e" , -1, -1},	/*  U+0404 - ?? - CYRILLIC CAPITAL LETTER UKRAINIAN IE */
    { "\xd0\x85", "s" , -1, -1},	/*  U+0405 - ?? - CYRILLIC CAPITAL LETTER DZE */
    { "\xd0\x86", "i" , -1, -1},	/*  U+0406 - ?? - CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I */
    { "\xd0\x87", "i" , -1, -1},	/*  U+0407 - ?? - CYRILLIC CAPITAL LETTER YI */
    { "\xd0\x88", "k" , -1, -1},	/*  U+0408 - ?? - CYRILLIC CAPITAL LETTER JE */
    { "\xd0\x89", "jb" , -1, -1},	/*  U+0409 - ?? - CYRILLIC CAPITAL LETTER LJE */
    { "\xd0\x8a", "hb" , -1, -1},	/*  U+040A - ?? - CYRILLIC CAPITAL LETTER NJE */
    { "\xd0\x8b", "h" , -1, -1},	/*  U+040B - ?? - CYRILLIC CAPITAL LETTER TSHE */
    { "\xd0\x8c", "k" , -1, -1},	/*  U+040C - ?? - CYRILLIC CAPITAL LETTER KJE */
    { "\xd0\x8d", "n" , -1, -1},	/*  U+040D - ?? - CYRILLIC CAPITAL LETTER I WITH GRAVE */
    { "\xd0\x8e", "y" , -1, -1},	/*  U+040E - ?? - CYRILLIC CAPITAL LETTER SHORT U */
    { "\xd0\x8f", "u" , -1, -1},	/*  U+040F - ?? - CYRILLIC CAPITAL LETTER DZHE */
    { "\xd0\x90", "a" , -1, -1},	/*  U+0410 - ?? - CYRILLIC CAPITAL LETTER A */
    { "\xd0\x91", "b" , -1, -1},	/*  U+0411 - ?? - CYRILLIC CAPITAL LETTER BE */
    { "\xd0\x92", "b" , -1, -1},	/*  U+0412 - ?? - CYRILLIC CAPITAL LETTER VE */
    { "\xd0\x93", "t" , -1, -1},	/*  U+0413 - ?? - CYRILLIC CAPITAL LETTER GHE */
    { "\xd0\x94", "a" , -1, -1},	/*  U+0414 - ?? - CYRILLIC CAPITAL LETTER DE */
    { "\xd0\x95", "e" , -1, -1},	/*  U+0415 - ?? - CYRILLIC CAPITAL LETTER IE */
    { "\xd0\x96", "x" , -1, -1},	/*  U+0416 - ?? - CYRILLIC CAPITAL LETTER ZHE */
    { "\xd0\x97", "3" , -1, -1},	/*  U+0417 - ?? - CYRILLIC CAPITAL LETTER ZE */
    { "\xd0\x98", "n" , -1, -1},	/*  U+0418 - ?? - CYRILLIC CAPITAL LETTER I */
    { "\xd0\x99", "n" , -1, -1},	/*  U+0419 - ?? - CYRILLIC CAPITAL LETTER SHORT I */
    { "\xd0\x9a", "k" , -1, -1},	/*  U+041A - ?? - CYRILLIC CAPITAL LETTER KA */
    { "\xd0\x9b", "n" , -1, -1},	/*  U+041B - ?? - CYRILLIC CAPITAL LETTER EL */
    { "\xd0\x9c", "m" , -1, -1},	/*  U+041C - ?? - CYRILLIC CAPITAL LETTER EM */
    { "\xd0\x9d", "h" , -1, -1},	/*  U+041D - ?? - CYRILLIC CAPITAL LETTER EN */
    { "\xd0\x9e", "o" , -1, -1},	/*  U+041E - ?? - CYRILLIC CAPITAL LETTER O */
    { "\xd0\x9f", "n" , -1, -1},	/*  U+041F - ?? - CYRILLIC CAPITAL LETTER PE */
    { "\xd0\xa0", "p" , -1, -1},	/*  U+0420 - ?? - CYRILLIC CAPITAL LETTER ER */
    { "\xd0\xa1", "c" , -1, -1},	/*  U+0421 - ?? - CYRILLIC CAPITAL LETTER ES */
    { "\xd0\xa2", "t" , -1, -1},	/*  U+0422 - ?? - CYRILLIC CAPITAL LETTER TE */
    { "\xd0\xa3", "y" , -1, -1},	/*  U+0423 - ?? - CYRILLIC CAPITAL LETTER U */
    { "\xd0\xa4", "o" , -1, -1},	/*  U+0424 - ?? - CYRILLIC CAPITAL LETTER EF */
    { "\xd0\xa5", "x" , -1, -1},	/*  U+0425 - ?? - CYRILLIC CAPITAL LETTER HA */
    { "\xd0\xa6", "u" , -1, -1},	/*  U+0426 - ?? - CYRILLIC CAPITAL LETTER TSE */
    { "\xd0\xa7", "y" , -1, -1},	/*  U+0427 - ?? - CYRILLIC CAPITAL LETTER CHE */
    { "\xd0\xa8", "w" , -1, -1},	/*  U+0428 - ?? - CYRILLIC CAPITAL LETTER SHA */
    { "\xd0\xa9", "w" , -1, -1},	/*  U+0429 - ?? - CYRILLIC CAPITAL LETTER SHCHA */
    { "\xd0\xaa", "b" , -1, -1},	/*  U+042A - ?? - CYRILLIC CAPITAL LETTER HARD SIGN */
    { "\xd0\xab", "bl" , -1, -1},	/*  U+042B - ?? - CYRILLIC CAPITAL LETTER YERU */
    { "\xd0\xac", "b" , -1, -1},	/*  U+042C - ?? - CYRILLIC CAPITAL LETTER SOFT SIGN */
    { "\xd0\xad", "e" , -1, -1},	/*  U+042D - ?? - CYRILLIC CAPITAL LETTER E */
    { "\xd0\xae", "10" , -1, -1},	/*  U+042E - ?? - CYRILLIC CAPITAL LETTER YU */
    { "\xd0\xaf", "r" , -1, -1},	/*  U+042F - ?? - CYRILLIC CAPITAL LETTER YA */
    { "\xd0\xb0", "a" , -1, -1},	/*  U+0430 - ?? - CYRILLIC SMALL LETTER A */
    { "\xd0\xb1", "g" , -1, -1},	/*  U+0431 - ?? - CYRILLIC SMALL LETTER BE */
    { "\xd0\xb2", "b" , -1, -1},	/*  U+0432 - ?? - CYRILLIC SMALL LETTER VE */
    { "\xd0\xb3", "t" , -1, -1},	/*  U+0433 - ?? - CYRILLIC SMALL LETTER GHE */
    { "\xd0\xb4", "a" , -1, -1},	/*  U+0434 - ?? - CYRILLIC SMALL LETTER DE */
    { "\xd0\xb5", "e" , -1, -1},	/*  U+0435 - ?? - CYRILLIC SMALL LETTER IE */
    { "\xd0\xb6", "x" , -1, -1},	/*  U+0436 - ?? - CYRILLIC SMALL LETTER ZHE */
    { "\xd0\xb7", "e" , -1, -1},	/*  U+0437 - ?? - CYRILLIC SMALL LETTER ZE */
    { "\xd0\xb8", "n" , -1, -1},	/*  U+0438 - ?? - CYRILLIC SMALL LETTER I */
    { "\xd0\xb9", "n" , -1, -1},	/*  U+0439 - ?? - CYRILLIC SMALL LETTER SHORT I */
    { "\xd0\xba", "k" , -1, -1},	/*  U+043A - ?? - CYRILLIC SMALL LETTER KA */
    { "\xd0\xbb", "n" , -1, -1},	/*  U+043B - ?? - CYRILLIC SMALL LETTER EL */
    { "\xd0\xbc", "m" , -1, -1},	/*  U+043C - ?? - CYRILLIC SMALL LETTER EM */
    { "\xd0\xbd", "h" , -1, -1},	/*  U+043D - ?? - CYRILLIC SMALL LETTER EN */
    { "\xd0\xbe", "o" , -1, -1},	/*  U+043E - ?? - CYRILLIC SMALL LETTER O */
    { "\xd0\xbf", "n" , -1, -1},	/*  U+043F - ?? - CYRILLIC SMALL LETTER PE */
    { "\xd1\x80", "p" , -1, -1},	/*  U+0440 - ?? - CYRILLIC SMALL LETTER ER */
    { "\xd1\x81", "c" , -1, -1},	/*  U+0441 - ?? - CYRILLIC SMALL LETTER ES */
    { "\xd1\x82", "t" , -1, -1},	/*  U+0442 - ?? - CYRILLIC SMALL LETTER TE */
    { "\xd1\x83", "y" , -1, -1},	/*  U+0443 - ?? - CYRILLIC SMALL LETTER U */
    { "\xd1\x84", "o" , -1, -1},	/*  U+0444 - ?? - CYRILLIC SMALL LETTER EF */
    { "\xd1\x85", "x" , -1, -1},	/*  U+0445 - ?? - CYRILLIC SMALL LETTER HA */
    { "\xd1\x86", "u" , -1, -1},	/*  U+0446 - ?? - CYRILLIC SMALL LETTER TSE */
    { "\xd1\x87", "y" , -1, -1},	/*  U+0447 - ?? - CYRILLIC SMALL LETTER CHE */
    { "\xd1\x88", "w" , -1, -1},	/*  U+0448 - ?? - CYRILLIC SMALL LETTER SHA */
    { "\xd1\x89", "w" , -1, -1},	/*  U+0449 - ?? - CYRILLIC SMALL LETTER SHCHA */
    { "\xd1\x8a", "b" , -1, -1},	/*  U+044A - ?? - CYRILLIC SMALL LETTER HARD SIGN */
    { "\xd1\x8b", "bl" , -1, -1},	/*  U+044B - ?? - CYRILLIC SMALL LETTER YERU */
    { "\xd1\x8c", "b" , -1, -1},	/*  U+044C - ?? - CYRILLIC SMALL LETTER SOFT SIGN */
    { "\xd1\x8d", "e" , -1, -1},	/*  U+044D - ?? - CYRILLIC SMALL LETTER E */
    { "\xd1\x8e", "10" , -1, -1},	/*  U+044E - ?? - CYRILLIC SMALL LETTER YU */
    { "\xd1\x8f", "r" , -1, -1},	/*  U+044F - ?? - CYRILLIC SMALL LETTER YA */
    { "\xd1\x90", "e" , -1, -1},	/*  U+0450 - ?? - CYRILLIC SMALL LETTER IE WITH GRAVE */
    { "\xd1\x91", "e" , -1, -1},	/*  U+0451 - ?? - CYRILLIC SMALL LETTER IO */
    { "\xd1\x92", "h" , -1, -1},	/*  U+0452 - ?? - CYRILLIC SMALL LETTER DJE */
    { "\xd1\x93", "r" , -1, -1},	/*  U+0453 - ?? - CYRILLIC SMALL LETTER GJE */
    { "\xd1\x94", "e" , -1, -1},	/*  U+0454 - ?? - CYRILLIC SMALL LETTER UKRAINIAN IE */
    { "\xd1\x95", "s" , -1, -1},	/*  U+0455 - ?? - CYRILLIC SMALL LETTER DZE */
    { "\xd1\x96", "i" , -1, -1},	/*  U+0456 - ?? - CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I */
    { "\xd1\x97", "i" , -1, -1},	/*  U+0457 - ?? - CYRILLIC SMALL LETTER YI */
    { "\xd1\x98", "j" , -1, -1},	/*  U+0458 - ?? - CYRILLIC SMALL LETTER JE */
    { "\xd1\x99", "jb" , -1, -1},	/*  U+0459 - ?? - CYRILLIC SMALL LETTER LJE */
    { "\xd1\x9a", "hb" , -1, -1},	/*  U+045A - ?? - CYRILLIC SMALL LETTER NJE */
    { "\xd1\x9b", "h" , -1, -1},	/*  U+045B - ?? - CYRILLIC SMALL LETTER TSHE */
    { "\xd1\x9c", "k" , -1, -1},	/*  U+045C - ?? - CYRILLIC SMALL LETTER KJE */
    { "\xd1\x9d", "n" , -1, -1},	/*  U+045D - ?? - CYRILLIC SMALL LETTER I WITH GRAVE */
    { "\xd1\x9e", "y" , -1, -1},	/*  U+045E - ?? - CYRILLIC SMALL LETTER SHORT U */
    { "\xd1\x9f", "u" , -1, -1},	/*  U+045F - ?? - CYRILLIC SMALL LETTER DZHE */
    { "\xd1\xa0", "o" , -1, -1},	/*  U+0460 - ?? - CYRILLIC CAPITAL LETTER OMEGA */
    { "\xd1\xa1", "w" , -1, -1},	/*  U+0461 - ?? - CYRILLIC SMALL LETTER OMEGA */
    { "\xd1\xa2", "b" , -1, -1},	/*  U+0462 - ?? - CYRILLIC CAPITAL LETTER YAT */
    { "\xd1\xa3", "b" , -1, -1},	/*  U+0463 - ?? - CYRILLIC SMALL LETTER YAT */
    { "\xd1\xa4", "ie" , -1, -1},	/*  U+0464 - ?? - CYRILLIC CAPITAL LETTER IOTIFIED E */
    { "\xd1\xa5", "ie" , -1, -1},	/*  U+0465 - ?? - CYRILLIC SMALL LETTER IOTIFIED E */
    { "\xd1\xa6", "a" , -1, -1},	/*  U+0466 - ?? - CYRILLIC CAPITAL LETTER LITTLE YUS */
    { "\xd1\xa7", "a" , -1, -1},	/*  U+0467 - ?? - CYRILLIC SMALL LETTER LITTLE YUS */
    { "\xd1\xa8", "ha" , -1, -1},	/*  U+0468 - ?? - CYRILLIC CAPITAL LETTER IOTIFIED LITTLE YUS */
    { "\xd1\xa9", "ha" , -1, -1},	/*  U+0469 - ?? - CYRILLIC SMALL LETTER IOTIFIED LITTLE YUS */
    { "\xd1\xaa", "x" , -1, -1},	/*  U+046A - ?? - CYRILLIC CAPITAL LETTER BIG YUS */
    { "\xd1\xab", "x" , -1, -1},	/*  U+046B - ?? - CYRILLIC SMALL LETTER BIG YUS */
    { "\xd1\xac", "hx" , -1, -1},	/*  U+046C - ?? - CYRILLIC CAPITAL LETTER IOTIFIED BIG YUS */
    { "\xd1\xad", "hx" , -1, -1},	/*  U+046D - ?? - CYRILLIC SMALL LETTER IOTIFIED BIG YUS */
    { "\xd1\xae", "3" , -1, -1},	/*  U+046E - ?? - CYRILLIC CAPITAL LETTER KSI */
    { "\xd1\xaf", "3" , -1, -1},	/*  U+046F - ?? - CYRILLIC SMALL LETTER KSI */
    { "\xd1\xb0", "y" , -1, -1},	/*  U+0470 - ?? - CYRILLIC CAPITAL LETTER PSI */
    { "\xd1\xb1", "y" , -1, -1},	/*  U+0471 - ?? - CYRILLIC SMALL LETTER PSI */
    { "\xd1\xb2", "o" , -1, -1},	/*  U+0472 - ?? - CYRILLIC CAPITAL LETTER FITA */
    { "\xd1\xb3", "o" , -1, -1},	/*  U+0473 - ?? - CYRILLIC SMALL LETTER FITA */
    { "\xd1\xb4", "v" , -1, -1},	/*  U+0474 - ?? - CYRILLIC CAPITAL LETTER IZHITSA */
    { "\xd1\xb5", "v" , -1, -1},	/*  U+0475 - ?? - CYRILLIC SMALL LETTER IZHITSA */
    { "\xd1\xb6", "v" , -1, -1},	/*  U+0476 - ?? - CYRILLIC CAPITAL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT */
    { "\xd1\xb7", "v" , -1, -1},	/*  U+0477 - ?? - CYRILLIC SMALL LETTER IZHITSA WITH DOUBLE GRAVE ACCENT */
    { "\xd1\xb8", "oy" , -1, -1},	/*  U+0478 - ?? - CYRILLIC CAPITAL LETTER UK */
    { "\xd1\xb9", "oy" , -1, -1},	/*  U+0479 - ?? - CYRILLIC SMALL LETTER UK */
    { "\xd1\xba", "o" , -1, -1},	/*  U+047A - ?? - CYRILLIC CAPITAL LETTER ROUND OMEGA */
    { "\xd1\xbb", "o" , -1, -1},	/*  U+047B - ?? - CYRILLIC SMALL LETTER ROUND OMEGA */
    { "\xd1\xbc", "" , -1, -1},		/*  U+047C - ?? - CYRILLIC CAPITAL LETTER OMEGA WITH TITLO */
    { "\xd1\xbd", "" , -1, -1},		/*  U+047D - ?? - CYRILLIC SMALL LETTER OMEGA WITH TITLO */
    { "\xd1\xbe", "" , -1, -1},		/*  U+047E - ?? - CYRILLIC CAPITAL LETTER OT */
    { "\xd1\xbf", "w" , -1, -1},	/*  U+047F - ?? - CYRILLIC SMALL LETTER OT */
    { "\xd2\x80", "c" , -1, -1},	/*  U+0480 - ?? - CYRILLIC CAPITAL LETTER KOPPA */
    { "\xd2\x81", "c" , -1, -1},	/*  U+0481 - ?? - CYRILLIC SMALL LETTER KOPPA */
    { "\xd2\x82", "+" , -1, -1},	/*  U+0482 - ?? - CYRILLIC THOUSANDS SIGN */
    { "\xd2\x83", "" , -1, -1},		/*  U+0483 - >?? - COMBINING CYRILLIC TITLO */
    { "\xd2\x84", "" , -1, -1},		/*  U+0484 - >?? - COMBINING CYRILLIC PALATALIZATION */
    { "\xd2\x85", "t" , -1, -1},	/*  U+0485 - >?? - COMBINING CYRILLIC DASIA PNEUMATA */
    { "\xd2\x86", "t" , -1, -1},	/*  U+0486 - >?? - COMBINING CYRILLIC PSILI PNEUMATA */
    { "\xd2\x87", "" , -1, -1},		/*  U+0487 - >?? - COMBINING CYRILLIC POKRYTIE */
    { "\xd2\x88", "" , -1, -1},		/*  U+0488 - >?? - COMBINING CYRILLIC HUNDRED THOUSANDS SIGN */
    { "\xd2\x89", "" , -1, -1},		/*  U+0489 - >?? - COMBINING CYRILLIC MILLIONS SIGN */
    { "\xd2\x8a", "n" , -1, -1},	/*  U+048A - ?? - CYRILLIC CAPITAL LETTER SHORT I WITH TAIL */
    { "\xd2\x8b", "n" , -1, -1},	/*  U+048B - ?? - CYRILLIC SMALL LETTER SHORT I WITH TAIL */
    { "\xd2\x8c", "b" , -1, -1},	/*  U+048C - ?? - CYRILLIC CAPITAL LETTER SEMISOFT SIGN */
    { "\xd2\x8d", "b" , -1, -1},	/*  U+048D - ?? - CYRILLIC SMALL LETTER SEMISOFT SIGN */
    { "\xd2\x8e", "p" , -1, -1},	/*  U+048E - ?? - CYRILLIC CAPITAL LETTER ER WITH TICK */
    { "\xd2\x8f", "p" , -1, -1},	/*  U+048F - ?? - CYRILLIC SMALL LETTER ER WITH TICK */
    { "\xd2\x90", "r" , -1, -1},	/*  U+0490 - ?? - CYRILLIC CAPITAL LETTER GHE WITH UPTURN */
    { "\xd2\x91", "r" , -1, -1},	/*  U+0491 - ?? - CYRILLIC SMALL LETTER GHE WITH UPTURN */
    { "\xd2\x92", "f" , -1, -1},	/*  U+0492 - ?? - CYRILLIC CAPITAL LETTER GHE WITH STROKE */
    { "\xd2\x93", "f" , -1, -1},	/*  U+0493 - ?? - CYRILLIC SMALL LETTER GHE WITH STROKE */
    { "\xd2\x94", "h" , -1, -1},	/*  U+0494 - ?? - CYRILLIC CAPITAL LETTER GHE WITH MIDDLE HOOK */
    { "\xd2\x95", "h" , -1, -1},	/*  U+0495 - ?? - CYRILLIC SMALL LETTER GHE WITH MIDDLE HOOK */
    { "\xd2\x96", "x" , -1, -1},	/*  U+0496 - ?? - CYRILLIC CAPITAL LETTER ZHE WITH DESCENDER */
    { "\xd2\x97", "x" , -1, -1},	/*  U+0497 - ?? - CYRILLIC SMALL LETTER ZHE WITH DESCENDER */
    { "\xd2\x98", "3" , -1, -1},	/*  U+0498 - ?? - CYRILLIC CAPITAL LETTER ZE WITH DESCENDER */
    { "\xd2\x99", "3" , -1, -1},	/*  U+0499 - ?? - CYRILLIC SMALL LETTER ZE WITH DESCENDER */
    { "\xd2\x9a", "k" , -1, -1},	/*  U+049A - ?? - CYRILLIC CAPITAL LETTER KA WITH DESCENDER */
    { "\xd2\x9b", "k" , -1, -1},	/*  U+049B - ?? - CYRILLIC SMALL LETTER KA WITH DESCENDER */
    { "\xd2\x9c", "k" , -1, -1},	/*  U+049C - ?? - CYRILLIC CAPITAL LETTER KA WITH VERTICAL STROKE */
    { "\xd2\x9d", "k" , -1, -1},	/*  U+049D - ?? - CYRILLIC SMALL LETTER KA WITH VERTICAL STROKE */
    { "\xd2\x9e", "k" , -1, -1},	/*  U+049E - ?? - CYRILLIC CAPITAL LETTER KA WITH STROKE */
    { "\xd2\x9f", "k" , -1, -1},	/*  U+049F - ?? - CYRILLIC SMALL LETTER KA WITH STROKE */
    { "\xd2\xa0", "k" , -1, -1},	/*  U+04A0 - ?? - CYRILLIC CAPITAL LETTER BASHKIR KA */
    { "\xd2\xa1", "k" , -1, -1},	/*  U+04A1 - ?? - CYRILLIC SMALL LETTER BASHKIR KA */
    { "\xd2\xa2", "h" , -1, -1},	/*  U+04A2 - ?? - CYRILLIC CAPITAL LETTER EN WITH DESCENDER */
    { "\xd2\xa3", "h" , -1, -1},	/*  U+04A3 - ?? - CYRILLIC SMALL LETTER EN WITH DESCENDER */
    { "\xd2\xa4", "hr" , -1, -1},	/*  U+04A4 - ?? - CYRILLIC CAPITAL LIGATURE EN GHE */
    { "\xd2\xa5", "hr" , -1, -1},	/*  U+04A5 - ?? - CYRILLIC SMALL LIGATURE EN GHE */
    { "\xd2\xa6", "tb" , -1, -1},	/*  U+04A6 - ?? - CYRILLIC CAPITAL LETTER PE WITH MIDDLE HOOK */
    { "\xd2\xa7", "tb" , -1, -1},	/*  U+04A7 - ?? - CYRILLIC SMALL LETTER PE WITH MIDDLE HOOK */
    { "\xd2\xa8", "c" , -1, -1},	/*  U+04A8 - ?? - CYRILLIC CAPITAL LETTER ABKHASIAN HA */
    { "\xd2\xa9", "c" , -1, -1},	/*  U+04A9 - ?? - CYRILLIC SMALL LETTER ABKHASIAN HA */
    { "\xd2\xaa", "c" , -1, -1},	/*  U+04AA - ?? - CYRILLIC CAPITAL LETTER ES WITH DESCENDER */
    { "\xd2\xab", "c" , -1, -1},	/*  U+04AB - ?? - CYRILLIC SMALL LETTER ES WITH DESCENDER */
    { "\xd2\xac", "t" , -1, -1},	/*  U+04AC - ?? - CYRILLIC CAPITAL LETTER TE WITH DESCENDER */
    { "\xd2\xad", "t" , -1, -1},	/*  U+04AD - ?? - CYRILLIC SMALL LETTER TE WITH DESCENDER */
    { "\xd2\xae", "y" , -1, -1},	/*  U+04AE - ?? - CYRILLIC CAPITAL LETTER STRAIGHT U */
    { "\xd2\xaf", "v" , -1, -1},	/*  U+04AF - ?? - CYRILLIC SMALL LETTER STRAIGHT U */
    { "\xd2\xb0", "y" , -1, -1},	/*  U+04B0 - ?? - CYRILLIC CAPITAL LETTER STRAIGHT U WITH STROKE */
    { "\xd2\xb1", "v" , -1, -1},	/*  U+04B1 - ?? - CYRILLIC SMALL LETTER STRAIGHT U WITH STROKE */
    { "\xd2\xb2", "x" , -1, -1},	/*  U+04B2 - ?? - CYRILLIC CAPITAL LETTER HA WITH DESCENDER */
    { "\xd2\xb3", "x" , -1, -1},	/*  U+04B3 - ?? - CYRILLIC SMALL LETTER HA WITH DESCENDER */
    { "\xd2\xb4", "ti" , -1, -1},	/*  U+04B4 - ?? - CYRILLIC CAPITAL LIGATURE TE TSE */
    { "\xd2\xb5", "ti" , -1, -1},	/*  U+04B5 - ?? - CYRILLIC SMALL LIGATURE TE TSE */
    { "\xd2\xb6", "y" , -1, -1},	/*  U+04B6 - ?? - CYRILLIC CAPITAL LETTER CHE WITH DESCENDER */
    { "\xd2\xb7", "y" , -1, -1},	/*  U+04B7 - ?? - CYRILLIC SMALL LETTER CHE WITH DESCENDER */
    { "\xd2\xb8", "y" , -1, -1},	/*  U+04B8 - ?? - CYRILLIC CAPITAL LETTER CHE WITH VERTICAL STROKE */
    { "\xd2\xb9", "y" , -1, -1},	/*  U+04B9 - ?? - CYRILLIC SMALL LETTER CHE WITH VERTICAL STROKE */
    { "\xd2\xba", "h" , -1, -1},	/*  U+04BA - ?? - CYRILLIC CAPITAL LETTER SHHA */
    { "\xd2\xbb", "h" , -1, -1},	/*  U+04BB - ?? - CYRILLIC SMALL LETTER SHHA */
    { "\xd2\xbc", "e" , -1, -1},	/*  U+04BC - ?? - CYRILLIC CAPITAL LETTER ABKHASIAN CHE */
    { "\xd2\xbd", "e" , -1, -1},	/*  U+04BD - ?? - CYRILLIC SMALL LETTER ABKHASIAN CHE */
    { "\xd2\xbe", "e" , -1, -1},	/*  U+04BE - ?? - CYRILLIC CAPITAL LETTER ABKHASIAN CHE WITH DESCENDER */
    { "\xd2\xbf", "e" , -1, -1},	/*  U+04BF - ?? - CYRILLIC SMALL LETTER ABKHASIAN CHE WITH DESCENDER */
    { "\xd3\x80", "i" , -1, -1},	/*  U+04C0 - ?? - CYRILLIC LETTER PALOCHKA */
    { "\xd3\x81", "x" , -1, -1},	/*  U+04C1 - ?? - CYRILLIC CAPITAL LETTER ZHE WITH BREVE */
    { "\xd3\x82", "x" , -1, -1},	/*  U+04C2 - ?? - CYRILLIC SMALL LETTER ZHE WITH BREVE */
    { "\xd3\x83", "k" , -1, -1},	/*  U+04C3 - ?? - CYRILLIC CAPITAL LETTER KA WITH HOOK */
    { "\xd3\x84", "k" , -1, -1},	/*  U+04C4 - ?? - CYRILLIC SMALL LETTER KA WITH HOOK */
    { "\xd3\x85", "n" , -1, -1},	/*  U+04C5 - ?? - CYRILLIC CAPITAL LETTER EL WITH TAIL */
    { "\xd3\x86", "n" , -1, -1},	/*  U+04C6 - ?? - CYRILLIC SMALL LETTER EL WITH TAIL */
    { "\xd3\x87", "h" , -1, -1},	/*  U+04C7 - ?? - CYRILLIC CAPITAL LETTER EN WITH HOOK */
    { "\xd3\x88", "h" , -1, -1},	/*  U+04C8 - ?? - CYRILLIC SMALL LETTER EN WITH HOOK */
    { "\xd3\x89", "h" , -1, -1},	/*  U+04C9 - ?? - CYRILLIC CAPITAL LETTER EN WITH TAIL */
    { "\xd3\x8a", "h" , -1, -1},	/*  U+04CA - ?? - CYRILLIC SMALL LETTER EN WITH TAIL */
    { "\xd3\x8b", "y" , -1, -1},	/*  U+04CB - ?? - CYRILLIC CAPITAL LETTER KHAKASSIAN CHE */
    { "\xd3\x8c", "y" , -1, -1},	/*  U+04CC - ?? - CYRILLIC SMALL LETTER KHAKASSIAN CHE */
    { "\xd3\x8d", "m" , -1, -1},	/*  U+04CD - ?? - CYRILLIC CAPITAL LETTER EM WITH TAIL */
    { "\xd3\x8e", "m" , -1, -1},	/*  U+04CE - ?? - CYRILLIC SMALL LETTER EM WITH TAIL */
    { "\xd3\x8f", "l" , -1, -1},	/*  U+04CF - ?? - CYRILLIC SMALL LETTER PALOCHKA */
    { "\xd3\x90", "a" , -1, -1},	/*  U+04D0 - ?? - CYRILLIC CAPITAL LETTER A WITH BREVE */
    { "\xd3\x91", "a" , -1, -1},	/*  U+04D1 - ?? - CYRILLIC SMALL LETTER A WITH BREVE */
    { "\xd3\x92", "a" , -1, -1},	/*  U+04D2 - ?? - CYRILLIC CAPITAL LETTER A WITH DIAERESIS */
    { "\xd3\x93", "a" , -1, -1},	/*  U+04D3 - ?? - CYRILLIC SMALL LETTER A WITH DIAERESIS */
    { "\xd3\x94", "ae" , -1, -1},	/*  U+04D4 - ?? - CYRILLIC CAPITAL LIGATURE A IE */
    { "\xd3\x95", "ae" , -1, -1},	/*  U+04D5 - ?? - CYRILLIC SMALL LIGATURE A IE */
    { "\xd3\x96", "e" , -1, -1},	/*  U+04D6 - ?? - CYRILLIC CAPITAL LETTER IE WITH BREVE */
    { "\xd3\x97", "e" , -1, -1},	/*  U+04D7 - ?? - CYRILLIC SMALL LETTER IE WITH BREVE */
    { "\xd3\x98", "e" , -1, -1},	/*  U+04D8 - ?? - CYRILLIC CAPITAL LETTER SCHWA */
    { "\xd3\x99", "e" , -1, -1},	/*  U+04D9 - ?? - CYRILLIC SMALL LETTER SCHWA */
    { "\xd3\x9a", "e" , -1, -1},	/*  U+04DA - ?? - CYRILLIC CAPITAL LETTER SCHWA WITH DIAERESIS */
    { "\xd3\x9b", "e" , -1, -1},	/*  U+04DB - ?? - CYRILLIC SMALL LETTER SCHWA WITH DIAERESIS */
    { "\xd3\x9c", "x" , -1, -1},	/*  U+04DC - ?? - CYRILLIC CAPITAL LETTER ZHE WITH DIAERESIS */
    { "\xd3\x9d", "x" , -1, -1},	/*  U+04DD - ?? - CYRILLIC SMALL LETTER ZHE WITH DIAERESIS */
    { "\xd3\x9e", "3" , -1, -1},	/*  U+04DE - ?? - CYRILLIC CAPITAL LETTER ZE WITH DIAERESIS */
    { "\xd3\x9f", "3" , -1, -1},	/*  U+04DF - ?? - CYRILLIC SMALL LETTER ZE WITH DIAERESIS */
    { "\xd3\xa0", "3" , -1, -1},	/*  U+04E0 - ?? - CYRILLIC CAPITAL LETTER ABKHASIAN DZE */
    { "\xd3\xa1", "3" , -1, -1},	/*  U+04E1 - ?? - CYRILLIC SMALL LETTER ABKHASIAN DZE */
    { "\xd3\xa2", "n" , -1, -1},	/*  U+04E2 - ?? - CYRILLIC CAPITAL LETTER I WITH MACRON */
    { "\xd3\xa3", "n" , -1, -1},	/*  U+04E3 - ?? - CYRILLIC SMALL LETTER I WITH MACRON */
    { "\xd3\xa4", "n" , -1, -1},	/*  U+04E4 - ?? - CYRILLIC CAPITAL LETTER I WITH DIAERESIS */
    { "\xd3\xa5", "n" , -1, -1},	/*  U+04E5 - ?? - CYRILLIC SMALL LETTER I WITH DIAERESIS */
    { "\xd3\xa6", "o" , -1, -1},	/*  U+04E6 - ?? - CYRILLIC CAPITAL LETTER O WITH DIAERESIS */
    { "\xd3\xa7", "o" , -1, -1},	/*  U+04E7 - ?? - CYRILLIC SMALL LETTER O WITH DIAERESIS */
    { "\xd3\xa8", "0" , -1, -1},	/*  U+04E8 - ?? - CYRILLIC CAPITAL LETTER BARRED O */
    { "\xd3\xa9", "0" , -1, -1},	/*  U+04E9 - ?? - CYRILLIC SMALL LETTER BARRED O */
    { "\xd3\xaa", "0" , -1, -1},	/*  U+04EA - ?? - CYRILLIC CAPITAL LETTER BARRED O WITH DIAERESIS */
    { "\xd3\xab", "0" , -1, -1},	/*  U+04EB - ?? - CYRILLIC SMALL LETTER BARRED O WITH DIAERESIS */
    { "\xd3\xac", "3" , -1, -1},	/*  U+04EC - ?? - CYRILLIC CAPITAL LETTER E WITH DIAERESIS */
    { "\xd3\xad", "3" , -1, -1},	/*  U+04ED - ?? - CYRILLIC SMALL LETTER E WITH DIAERESIS */
    { "\xd3\xae", "y" , -1, -1},	/*  U+04EE - ?? - CYRILLIC CAPITAL LETTER U WITH MACRON */
    { "\xd3\xaf", "y" , -1, -1},	/*  U+04EF - ?? - CYRILLIC SMALL LETTER U WITH MACRON */
    { "\xd3\xb0", "y" , -1, -1},	/*  U+04F0 - ?? - CYRILLIC CAPITAL LETTER U WITH DIAERESIS */
    { "\xd3\xb1", "y" , -1, -1},	/*  U+04F1 - ?? - CYRILLIC SMALL LETTER U WITH DIAERESIS */
    { "\xd3\xb2", "y" , -1, -1},	/*  U+04F2 - ?? - CYRILLIC CAPITAL LETTER U WITH DOUBLE ACUTE */
    { "\xd3\xb3", "y" , -1, -1},	/*  U+04F3 - ?? - CYRILLIC SMALL LETTER U WITH DOUBLE ACUTE */
    { "\xd3\xb4", "y" , -1, -1},	/*  U+04F4 - ?? - CYRILLIC CAPITAL LETTER CHE WITH DIAERESIS */
    { "\xd3\xb5", "y" , -1, -1},	/*  U+04F5 - ?? - CYRILLIC SMALL LETTER CHE WITH DIAERESIS */
    { "\xd3\xb6", "r" , -1, -1},	/*  U+04F6 - ?? - CYRILLIC CAPITAL LETTER GHE WITH DESCENDER */
    { "\xd3\xb7", "r" , -1, -1},	/*  U+04F7 - ?? - CYRILLIC SMALL LETTER GHE WITH DESCENDER */
    { "\xd3\xb8", "bl" , -1, -1},	/*  U+04F8 - ?? - CYRILLIC CAPITAL LETTER YERU WITH DIAERESIS */
    { "\xd3\xb9", "bl" , -1, -1},	/*  U+04F9 - ?? - CYRILLIC SMALL LETTER YERU WITH DIAERESIS */
    { "\xd3\xba", "f" , -1, -1},	/*  U+04FA - ?? - CYRILLIC CAPITAL LETTER GHE WITH STROKE AND HOOK */
    { "\xd3\xbb", "f" , -1, -1},	/*  U+04FB - ?? - CYRILLIC SMALL LETTER GHE WITH STROKE AND HOOK */
    { "\xd3\xbc", "x" , -1, -1},	/*  U+04FC - ?? - CYRILLIC CAPITAL LETTER HA WITH HOOK */
    { "\xd3\xbd", "x" , -1, -1},	/*  U+04FD - ?? - CYRILLIC SMALL LETTER HA WITH HOOK */
    { "\xd3\xbe", "x" , -1, -1},	/*  U+04FE - ?? - CYRILLIC CAPITAL LETTER HA WITH STROKE */
    { "\xd3\xbf", "x" , -1, -1},	/*  U+04FF - ?? - CYRILLIC SMALL LETTER HA WITH STROKE */
    { "\xd4\x80", "d" , -1, -1},	/*  U+0500 - ?? - CYRILLIC CAPITAL LETTER KOMI DE */
    { "\xd4\x81", "d" , -1, -1},	/*  U+0501 - ?? - CYRILLIC SMALL LETTER KOMI DE */
    { "\xd4\x82", "du" , -1, -1},	/*  U+0502 - ?? - CYRILLIC CAPITAL LETTER KOMI DJE */
    { "\xd4\x83", "du" , -1, -1},	/*  U+0503 - ?? - CYRILLIC SMALL LETTER KOMI DJE */
    { "\xd4\x84", "r" , -1, -1},	/*  U+0504 - ?? - CYRILLIC CAPITAL LETTER KOMI ZJE */
    { "\xd4\x85", "r" , -1, -1},	/*  U+0505 - ?? - CYRILLIC SMALL LETTER KOMI ZJE */
    { "\xd4\x86", "r" , -1, -1},	/*  U+0506 - ?? - CYRILLIC CAPITAL LETTER KOMI DZJE */
    { "\xd4\x87", "r" , -1, -1},	/*  U+0507 - ?? - CYRILLIC SMALL LETTER KOMI DZJE */
    { "\xd4\x88", "ju" , -1, -1},	/*  U+0508 - ?? - CYRILLIC CAPITAL LETTER KOMI LJE */
    { "\xd4\x89", "ju" , -1, -1},	/*  U+0509 - ?? - CYRILLIC SMALL LETTER KOMI LJE */
    { "\xd4\x8a", "hu" , -1, -1},	/*  U+050A - ?? - CYRILLIC CAPITAL LETTER KOMI NJE */
    { "\xd4\x8b", "hu" , -1, -1},	/*  U+050B - ?? - CYRILLIC SMALL LETTER KOMI NJE */
    { "\xd4\x8c", "g" , -1, -1},	/*  U+050C - ?? - CYRILLIC CAPITAL LETTER KOMI SJE */
    { "\xd4\x8d", "g" , -1, -1},	/*  U+050D - ?? - CYRILLIC SMALL LETTER KOMI SJE */
    { "\xd4\x8e", "tj" , -1, -1},	/*  U+050E - ?? - CYRILLIC CAPITAL LETTER KOMI TJE */
    { "\xd4\x8f", "tj" , -1, -1},	/*  U+050F - ?? - CYRILLIC SMALL LETTER KOMI TJE */
    { "\xd4\x90", "e" , -1, -1},	/*  U+0510 - ?? - CYRILLIC CAPITAL LETTER REVERSED ZE */
    { "\xd4\x91", "e" , -1, -1},	/*  U+0511 - ?? - CYRILLIC SMALL LETTER REVERSED ZE */
    { "\xd4\x92", "n" , -1, -1},	/*  U+0512 - ?? - CYRILLIC CAPITAL LETTER EL WITH HOOK */
    { "\xd4\x93", "n" , -1, -1},	/*  U+0513 - ?? - CYRILLIC SMALL LETTER EL WITH HOOK */
    { "\xd4\x94", "x" , -1, -1},	/*  U+0514 - ?? - CYRILLIC CAPITAL LETTER LHA */
    { "\xd4\x95", "x" , -1, -1},	/*  U+0515 - ?? - CYRILLIC SMALL LETTER LHA */
    { "\xd4\x96", "pk" , -1, -1},	/*  U+0516 - ?? - CYRILLIC CAPITAL LETTER RHA */
    { "\xd4\x97", "pk" , -1, -1},	/*  U+0517 - ?? - CYRILLIC SMALL LETTER RHA */
    { "\xd4\x98", "re" , -1, -1},	/*  U+0518 - ?? - CYRILLIC CAPITAL LETTER YAE */
    { "\xd4\x99", "re" , -1, -1},	/*  U+0519 - ?? - CYRILLIC SMALL LETTER YAE */
    { "\xd4\x9a", "q" , -1, -1},	/*  U+051A - ?? - CYRILLIC CAPITAL LETTER QA */
    { "\xd4\x9b", "q" , -1, -1},	/*  U+051B - ?? - CYRILLIC SMALL LETTER QA */
    { "\xd4\x9c", "w" , -1, -1},	/*  U+051C - ?? - CYRILLIC CAPITAL LETTER WE */
    { "\xd4\x9d", "w" , -1, -1},	/*  U+051D - ?? - CYRILLIC SMALL LETTER WE */
    { "\xd4\x9e", "kx" , -1, -1},	/*  U+051E - ?? - CYRILLIC CAPITAL LETTER ALEUT KA */
    { "\xd4\x9f", "kx" , -1, -1},	/*  U+051F - ?? - CYRILLIC SMALL LETTER ALEUT KA */
    { "\xd4\xa0", "th" , -1, -1},	/*  U+0520 - ?? - CYRILLIC CAPITAL LETTER EL WITH MIDDLE HOOK */
    { "\xd4\xa1", "th" , -1, -1},	/*  U+0521 - ?? - CYRILLIC SMALL LETTER EL WITH MIDDLE HOOK */
    { "\xd4\xa2", "hh" , -1, -1},	/*  U+0522 - ?? - CYRILLIC CAPITAL LETTER EN WITH MIDDLE HOOK */
    { "\xd4\xa3", "hh" , -1, -1},	/*  U+0523 - ?? - CYRILLIC SMALL LETTER EN WITH MIDDLE HOOK */
    { "\xd4\xa4", "n" , -1, -1},	/*  U+0524 - ?? - CYRILLIC CAPITAL LETTER PE WITH DESCENDER */
    { "\xd4\xa5", "n" , -1, -1},	/*  U+0525 - ?? - CYRILLIC SMALL LETTER PE WITH DESCENDER */
    { "\xd4\xa6", "h" , -1, -1},	/*  U+0526 - ?? - CYRILLIC CAPITAL LETTER SHHA WITH DESCENDER */
    { "\xd4\xa7", "h" , -1, -1},	/*  U+0527 - ?? - CYRILLIC SMALL LETTER SHHA WITH DESCENDER */
    { "\xd4\xa8", "h" , -1, -1},	/*  U+0528 - ?? - CYRILLIC CAPITAL LETTER EN WITH LEFT HOOK */
    { "\xd4\xa9", "h" , -1, -1},	/*  U+0529 - ?? - CYRILLIC SMALL LETTER EN WITH LEFT HOOK */
    { "\xd4\xaa", "ak" , -1, -1},	/*  U+052A - ?? - CYRILLIC CAPITAL LETTER DZZHE */
    { "\xd4\xab", "ak" , -1, -1},	/*  U+052B - ?? - CYRILLIC SMALL LETTER DZZHE */
    { "\xd4\xac", "yk" , -1, -1},	/*  U+052C - ?? - CYRILLIC CAPITAL LETTER DCHE */
    { "\xd4\xad", "ya" , -1, -1},	/*  U+052D - ?? - CYRILLIC SMALL LETTER DCHE */
    { "\xd4\xae", "n" , -1, -1},	/*  U+052E - ?? - CYRILLIC CAPITAL LETTER EL WITH DESCENDER */
    { "\xd4\xaf", "n" , -1, -1},	/*  U+052F - ?? - CYRILLIC SMALL LETTER EL WITH DESCENDER */
    { "\xd4\xb0", "" , -1, -1},		/*  U+0530 - ?? -  */
    { "\xd4\xb1", "u" , -1, -1},	/*  U+0531 - ?? - ARMENIAN CAPITAL LETTER AYB */
    { "\xd4\xb2", "f" , -1, -1},	/*  U+0532 - ?? - ARMENIAN CAPITAL LETTER BEN */
    { "\xd4\xb3", "q" , -1, -1},	/*  U+0533 - ?? - ARMENIAN CAPITAL LETTER GIM */
    { "\xd4\xb4", "r" , -1, -1},	/*  U+0534 - ?? - ARMENIAN CAPITAL LETTER DA */
    { "\xd4\xb5", "t" , -1, -1},	/*  U+0535 - ?? - ARMENIAN CAPITAL LETTER ECH */
    { "\xd4\xb6", "on" , -1, -1},	/*  U+0536 - ?? - ARMENIAN CAPITAL LETTER ZA */
    { "\xd4\xb7", "t" , -1, -1},	/*  U+0537 - ?? - ARMENIAN CAPITAL LETTER EH */
    { "\xd4\xb8", "r" , -1, -1},	/*  U+0538 - ?? - ARMENIAN CAPITAL LETTER ET */
    { "\xd4\xb9", "p" , -1, -1},	/*  U+0539 - ?? - ARMENIAN CAPITAL LETTER TO */
    { "\xd4\xba", "d" , -1, -1},	/*  U+053A - ?? - ARMENIAN CAPITAL LETTER ZHE */
    { "\xd4\xbb", "r" , -1, -1},	/*  U+053B - ?? - ARMENIAN CAPITAL LETTER INI */
    { "\xd4\xbc", "l" , -1, -1},	/*  U+053C - ?? - ARMENIAN CAPITAL LETTER LIWN */
    { "\xd4\xbd", "tu" , -1, -1},	/*  U+053D - ?? - ARMENIAN CAPITAL LETTER XEH */
    { "\xd4\xbe", "y" , -1, -1},	/*  U+053E - ?? - ARMENIAN CAPITAL LETTER CA */
    { "\xd4\xbf", "y" , -1, -1},	/*  U+053F - ?? - ARMENIAN CAPITAL LETTER KEN */
    { "\xd5\x80", "c" , -1, -1},	/*  U+0540 - ?? - ARMENIAN CAPITAL LETTER HO */
    { "\xd5\x81", "g" , -1, -1},	/*  U+0541 - ?? - ARMENIAN CAPITAL LETTER JA */
    { "\xd5\x82", "r" , -1, -1},	/*  U+0542 - ?? - ARMENIAN CAPITAL LETTER GHAD */
    { "\xd5\x83", "o" , -1, -1},	/*  U+0543 - ?? - ARMENIAN CAPITAL LETTER CHEH */
    { "\xd5\x84", "u" , -1, -1},	/*  U+0544 - ?? - ARMENIAN CAPITAL LETTER MEN */
    { "\xd5\x85", "3" , -1, -1},	/*  U+0545 - ?? - ARMENIAN CAPITAL LETTER YI */
    { "\xd5\x86", "u" , -1, -1},	/*  U+0546 - ?? - ARMENIAN CAPITAL LETTER NOW */
    { "\xd5\x87", "t" , -1, -1},	/*  U+0547 - ?? - ARMENIAN CAPITAL LETTER SHA */
    { "\xd5\x88", "n" , -1, -1},	/*  U+0548 - ?? - ARMENIAN CAPITAL LETTER VO */
    { "\xd5\x89", "o" , -1, -1},	/*  U+0549 - ?? - ARMENIAN CAPITAL LETTER CHA */
    { "\xd5\x8a", "m" , -1, -1},	/*  U+054A - ?? - ARMENIAN CAPITAL LETTER PEH */
    { "\xd5\x8b", "o" , -1, -1},	/*  U+054B - ?? - ARMENIAN CAPITAL LETTER JHEH */
    { "\xd5\x8c", "n" , -1, -1},	/*  U+054C - ?? - ARMENIAN CAPITAL LETTER RA */
    { "\xd5\x8d", "u" , -1, -1},	/*  U+054D - ?? - ARMENIAN CAPITAL LETTER SEH */
    { "\xd5\x8e", "y" , -1, -1},	/*  U+054E - ?? - ARMENIAN CAPITAL LETTER VEW */
    { "\xd5\x8f", "s" , -1, -1},	/*  U+054F - ?? - ARMENIAN CAPITAL LETTER TIWN */
    { "\xd5\x90", "r" , -1, -1},	/*  U+0550 - ?? - ARMENIAN CAPITAL LETTER REH */
    { "\xd5\x91", "8" , -1, -1},	/*  U+0551 - ?? - ARMENIAN CAPITAL LETTER CO */
    { "\xd5\x92", "r" , -1, -1},	/*  U+0552 - ?? - ARMENIAN CAPITAL LETTER YIWN */
    { "\xd5\x93", "0" , -1, -1},	/*  U+0553 - ?? - ARMENIAN CAPITAL LETTER PIWR */
    { "\xd5\x94", "p" , -1, -1},	/*  U+0554 - ?? - ARMENIAN CAPITAL LETTER KEH */
    { "\xd5\x95", "o" , -1, -1},	/*  U+0555 - ?? - ARMENIAN CAPITAL LETTER OH */
    { "\xd5\x96", "s" , -1, -1},	/*  U+0556 - ?? - ARMENIAN CAPITAL LETTER FEH */
    { "\xd5\x97", "" , -1, -1},		/*  U+0557 - ?? -  */
    { "\xd5\x98", "" , -1, -1},		/*  U+0558 - ?? -  */
    { "\xd5\x99", "" , -1, -1},		/*  U+0559 - ?? - ARMENIAN MODIFIER LETTER LEFT HALF RING */
    { "\xd5\x9a", "" , -1, -1},		/*  U+055A - ?? - ARMENIAN APOSTROPHE */
    { "\xd5\x9b", "" , -1, -1},		/*  U+055B - ?? - ARMENIAN EMPHASIS MARK */
    { "\xd5\x9c", "" , -1, -1},		/*  U+055C - ?? - ARMENIAN EXCLAMATION MARK */
    { "\xd5\x9d", "" , -1, -1},		/*  U+055D - ?? - ARMENIAN COMMA */
    { "\xd5\x9e", "" , -1, -1},		/*  U+055E - ?? - ARMENIAN QUESTION MARK */
    { "\xd5\x9f", "" , -1, -1},		/*  U+055F - ?? - ARMENIAN ABBREVIATION MARK */
    { "\xd5\xa0", "" , -1, -1},		/*  U+0560 - ?? - ARMENIAN SMALL LETTER TURNED AYB */
    { "\xd5\xa1", "w" , -1, -1},	/*  U+0561 - ?? - ARMENIAN SMALL LETTER AYB */
    { "\xd5\xa2", "r" , -1, -1},	/*  U+0562 - ?? - ARMENIAN SMALL LETTER BEN */
    { "\xd5\xa3", "q" , -1, -1},	/*  U+0563 - ?? - ARMENIAN SMALL LETTER GIM */
    { "\xd5\xa4", "n" , -1, -1},	/*  U+0564 - ?? - ARMENIAN SMALL LETTER DA */
    { "\xd5\xa5", "t" , -1, -1},	/*  U+0565 - ?? - ARMENIAN SMALL LETTER ECH */
    { "\xd5\xa6", "q" , -1, -1},	/*  U+0566 - ?? - ARMENIAN SMALL LETTER ZA */
    { "\xd5\xa7", "t" , -1, -1},	/*  U+0567 - ?? - ARMENIAN SMALL LETTER EH */
    { "\xd5\xa8", "n" , -1, -1},	/*  U+0568 - ?? - ARMENIAN SMALL LETTER ET */
    { "\xd5\xa9", "p" , -1, -1},	/*  U+0569 - ?? - ARMENIAN SMALL LETTER TO */
    { "\xd5\xaa", "d" , -1, -1},	/*  U+056A - ?? - ARMENIAN SMALL LETTER ZHE */
    { "\xd5\xab", "h" , -1, -1},	/*  U+056B - ?? - ARMENIAN SMALL LETTER INI */
    { "\xd5\xac", "l" , -1, -1},	/*  U+056C - ?? - ARMENIAN SMALL LETTER LIWN */
    { "\xd5\xad", "hu" , -1, -1},	/*  U+056D - ?? - ARMENIAN SMALL LETTER XEH */
    { "\xd5\xae", "o" , -1, -1},	/*  U+056E - ?? - ARMENIAN SMALL LETTER CA */
    { "\xd5\xaf", "u" , -1, -1},	/*  U+056F - ?? - ARMENIAN SMALL LETTER KEN */
    { "\xd5\xb0", "g" , -1, -1},	/*  U+0570 - ?? - ARMENIAN SMALL LETTER HO */
    { "\xd5\xb1", "d" , -1, -1},	/*  U+0571 - ?? - ARMENIAN SMALL LETTER JA */
    { "\xd5\xb2", "n" , -1, -1},	/*  U+0572 - ?? - ARMENIAN SMALL LETTER GHAD */
    { "\xd5\xb3", "6" , -1, -1},	/*  U+0573 - ?? - ARMENIAN SMALL LETTER CHEH */
    { "\xd5\xb4", "u" , -1, -1},	/*  U+0574 - ?? - ARMENIAN SMALL LETTER MEN */
    { "\xd5\xb5", "k" , -1, -1},	/*  U+0575 - ?? - ARMENIAN SMALL LETTER YI */
    { "\xd5\xb6", "u" , -1, -1},	/*  U+0576 - ?? - ARMENIAN SMALL LETTER NOW */
    { "\xd5\xb7", "2" , -1, -1},	/*  U+0577 - ?? - ARMENIAN SMALL LETTER SHA */
    { "\xd5\xb8", "n" , -1, -1},	/*  U+0578 - ?? - ARMENIAN SMALL LETTER VO */
    { "\xd5\xb9", "" , -1, -1},		/*  U+0579 - ?? - ARMENIAN SMALL LETTER CHA */
    { "\xd5\xba", "w" , -1, -1},	/*  U+057A - ?? - ARMENIAN SMALL LETTER PEH */
    { "\xd5\xbb", "q" , -1, -1},	/*  U+057B - ?? - ARMENIAN SMALL LETTER JHEH */
    { "\xd5\xbc", "n" , -1, -1},	/*  U+057C - ?? - ARMENIAN SMALL LETTER RA */
    { "\xd5\xbd", "u" , -1, -1},	/*  U+057D - ?? - ARMENIAN SMALL LETTER SEH */
    { "\xd5\xbe", "u" , -1, -1},	/*  U+057E - ?? - ARMENIAN SMALL LETTER VEW */
    { "\xd5\xbf", "un" , -1, -1},	/*  U+057F - ?? - ARMENIAN SMALL LETTER TIWN */
    { "\xd6\x80", "n" , -1, -1},	/*  U+0580 - ?? - ARMENIAN SMALL LETTER REH */
    { "\xd6\x81", "g" , -1, -1},	/*  U+0581 - ?? - ARMENIAN SMALL LETTER CO */
    { "\xd6\x82", "l" , -1, -1},	/*  U+0582 - ?? - ARMENIAN SMALL LETTER YIWN */
    { "\xd6\x83", "yh" , -1, -1},	/*  U+0583 - ?? - ARMENIAN SMALL LETTER PIWR */
    { "\xd6\x84", "p" , -1, -1},	/*  U+0584 - ?? - ARMENIAN SMALL LETTER KEH */
    { "\xd6\x85", "o" , -1, -1},	/*  U+0585 - ?? - ARMENIAN SMALL LETTER OH */
    { "\xd6\x86", "s" , -1, -1},	/*  U+0586 - ?? - ARMENIAN SMALL LETTER FEH */
    { "\xd6\x87", "u" , -1, -1},	/*  U+0587 - ?? - ARMENIAN SMALL LIGATURE ECH YIWN */
    { "\xd6\x88", "" , -1, -1},		/*  U+0588 - ?? - ARMENIAN SMALL LETTER YI WITH STROKE */
    { "\xd6\x89", "" , -1, -1},		/*  U+0589 - ?? - ARMENIAN FULL STOP */
    { "\xd6\x8a", "" , -1, -1},		/*  U+058A - ?? - ARMENIAN HYPHEN */
    { "\xd6\x8b", "" , -1, -1},		/*  U+058B - ?? -  */
    { "\xd6\x8c", "" , -1, -1},		/*  U+058C - ?? -  */
    { "\xd6\x8d", "" , -1, -1},		/*  U+058D - ?? - RIGHT-FACING ARMENIAN ETERNITY SIGN */
    { "\xd6\x8e", "" , -1, -1},		/*  U+058E - ?? - LEFT-FACING ARMENIAN ETERNITY SIGN */
    { "\xd6\x8f", "" , -1, -1},		/*  U+058F - ?? - ARMENIAN DRAM SIGN */

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
			      "fix table in utf8_posix_map.c and recompile",
			      (uintmax_t)max, (uintmax_t)i);
	    not_reached();
	}
	if (hmap[i].posix_str == NULL) {
	    err(12, __func__, "found posix_str NULL pointer not at end (hmap[%ju]) of hmap[%ju]; "
			      "fix table in utf8_posix_map.c and recompile",
			      (uintmax_t)max, (uintmax_t)i);
	    not_reached();
	}

	/* fill in string lengths */
	hmap[i].utf8_str_len = strlen(hmap[i].utf8_str);
	hmap[i].posix_str_len = strlen(hmap[i].posix_str);

	/* POSIX portable plus + check on posix_str if string is not empty */
	if (hmap[i].posix_str_len > 0 && posix_plus_safe(hmap[i].posix_str, true, false, false) == false) {
	    err(13, __func__, "hmap[%ju] = '%s' is not POSIX portable plus + safe; "
			      "fix table in utf8_posix_map.c and recompile", (uintmax_t)i, hmap[i].posix_str);
	    not_reached();
	}
    }

    /*
     * check final NULL at end of table
     */
    if (hmap[max-1].utf8_str != NULL || hmap[max-1].posix_str != NULL) {
	err(14, __func__, "no final NULL element at hmap[%ju]; fix table in utf8_posix_map.c and recompile", (uintmax_t)(max-1));
	not_reached();
    }
    utf8_posix_map_checked = true;
    dbg(DBG_VVHIGH, "hmap[0..%ju] sane and ready in utf8_posix_map.c", (uintmax_t)(max-1));
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
	    if (i + m->utf8_str_len > namelen) {
		continue;
	    }

	    /* skip if there isn't match with the rest of the string */
	    if (strncasecmp(m->utf8_str, name+i, m->utf8_str_len) != 0) {
		continue;
	    }

	    /* match found: add to default handle length */
	    def_len += m->posix_str_len;

	    /*
	     * advance string position
	     */
	    i += m->utf8_str_len;

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
		if (i + m->utf8_str_len > namelen) {
		    continue;
		}

		/* skip if there isn't a match with the rest of the string */
		if (strncasecmp(m->utf8_str, name+i, m->utf8_str_len) != 0) {
		    continue;
		}

		/*
		 * match found: copy translated chars to handle, if there are any
		 */
		if (m->posix_str_len > 0) {

		    /*
		     * firewall - do not copy beyond end of allocated buffer
		     */
		    if (cur_len + m->posix_str_len > def_len) {
			err(22, __func__, "attempt to copy to buf[%ju] %ju bytes: would go beyond allocated len: %ju",
					   (uintmax_t)cur_len, (uintmax_t)m->posix_str_len, (uintmax_t)def_len);
			not_reached();
		    }

		    /*
		     * copy translation into the default author handle buffer
		     */
		    errno = 0;		/* pre-clear errno for errp() */
		    pret = strncpy(ret+cur_len, m->posix_str, m->posix_str_len);
		    if (pret == NULL) {
			errp(23, __func__, "strncpy() returned NULL");
			not_reached();
		    }

		    /*
		     * advance our copy position
		     */
		    cur_len += m->posix_str_len;
		}

		/*
		 * advance our string position
		 */
		i += m->utf8_str_len;

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
