/* vim: set tabstop=8 softtabstop=4 shiftwidth=4 noexpandtab : */
/*
 * utf8_posix_map - translate certain UTF-8 into POSIX+ safe filenames
 *
 * "Because even POSIX needs an extra plus." :-)
 *
 * An author_handle, for an IOCCC winner, will be used to form
 * a winner_handle.  These winner_handle's will become part of a
 * JSON filename on the www.ioccc.org web site.  For this reason,
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

/*
 * utf8_posix_map include
 */
#include "utf8_posix_map.h"


/*
 * How to translate certain UTF-8 strings into safe lower case
 * POSIX portable filenames plus +.  This table helps us convert
 * cerain UTF-8 strings into strings that match this regular expression:
 *
 *	^[0-9a-z][0-9a-z._+-]*$
 *
 * The hmap[] may be used in forming author handles.
 *
 * NOTE: In some cases the byte is translated into an empty string.
 *
 * This table ends in a NULL entry.
 */
struct utf8_posix_map hmap[] =
{
    /* \x00 -\x0f */
    {"\x00", ""},		/* NUL */
    {"\x01", ""},		/* SOH */
    {"\x02", ""},		/* STX */
    {"\x03", ""},		/* ETX */
    {"\x04", ""},		/* EOT */
    {"\x05", ""},		/* EOT */
    {"\x06", ""},		/* ENQ */
    {"\x07", ""},		/* BEL (\a) */
    {"\x08", ""},		/* BS (\b) */
    {"\x09", "_"},		/* HT (\t) */
    {"\x0a", "_"},		/* LF (\n) */
    {"\x0b", "_"},		/* VT (\v) */
    {"\x0c", "_"},		/* FF (\f) */
    {"\x0d", "_"},		/* CR (\r) */
    {"\x0e", ""},		/* SO */
    {"\x0f", ""},		/* SI */

    /* \x10 -\x1f */
    {"\x10", ""},		/* DLE */
    {"\x11", ""},		/* DC1 */
    {"\x12", ""},		/* DC2 */
    {"\x13", ""},		/* DC3 */
    {"\x14", ""},		/* DC4 */
    {"\x15", ""},		/* NAK */
    {"\x16", ""},		/* SYN */
    {"\x17", ""},		/* ETB */
    {"\x18", ""},		/* CAN */
    {"\x19", ""},		/* EM */
    {"\x1a", ""},		/* SUB */
    {"\x1b", ""},		/* ESC */
    {"\x1c", ""},		/* FS */
    {"\x1d", ""},		/* GS */
    {"\x1e", ""},		/* RS */
    {"\x1f", ""},		/* US */

    /* \x20 -\x2f */
    {" ", "_"},			/* SP */
    {"!", ""},			/* ! */
    {"\"", ""},			/* " */
    {"#", ""},			/* # */
    {"$", ""},			/* $ */
    {"%", ""},			/* % */
    {"&", ""},			/* & */
    {"\'", ""},			/* ' */
    {"(", ""},			/* ( */
    {")", ""},			/* ) */
    {"*", ""},			/* * */
    {"+", "+"},			/* + - allowed character */
    {",", ""},			/* , */
    {"-", "-"},			/* - - allowed character */
    {".", "."},			/* . - allowed character */
    {"/", ""},			/* / */

    /* \x30 -\x3f */
    {"0", "0"},			/* 0 - allowed character */
    {"1", "1"},			/* 1 - allowed character */
    {"2", "2"},			/* 2 - allowed character */
    {"3", "3"},			/* 3 - allowed character */
    {"4", "4"},			/* 4 - allowed character */
    {"5", "5"},			/* 5 - allowed character */
    {"6", "6"},			/* 6 - allowed character */
    {"7", "7"},			/* 7 - allowed character */
    {"8", "8"},			/* 8 - allowed character */
    {"9", "9"},			/* 9 - allowed character */
    {":", ""},			/* & */
    {";", ""},			/* ; */
    {"<", ""},			/* < */
    {"=", ""},			/* = */
    {">", ""},			/* > */
    {"?", ""},			/* ? */

    /* \x40 -\x4f */
    {"@", ""},			/* @ */
    {"A", "a"},			/* A - converted to lower case character */
    {"B", "b"},			/* B - converted to lower case character */
    {"C", "c"},			/* C - converted to lower case character */
    {"D", "d"},			/* D - converted to lower case character */
    {"E", "e"},			/* E - converted to lower case character */
    {"F", "f"},			/* F - converted to lower case character */
    {"G", "g"},			/* G - converted to lower case character */
    {"H", "h"},			/* H - converted to lower case character */
    {"I", "i"},			/* I - converted to lower case character */
    {"J", "j"},			/* J - converted to lower case character */
    {"K", "k"},			/* K - converted to lower case character */
    {"L", "l"},			/* L - converted to lower case character */
    {"M", "m"},			/* M - converted to lower case character */
    {"N", "n"},			/* N - converted to lower case character */
    {"O", "o"},			/* O - converted to lower case character */

    /* \x50 -\x5f */
    {"P", "p"},			/* P - converted to lower case character */
    {"Q", "q"},			/* Q - converted to lower case character */
    {"R", "r"},			/* R - converted to lower case character */
    {"S", "s"},			/* S - converted to lower case character */
    {"T", "t"},			/* T - converted to lower case character */
    {"U", "u"},			/* U - converted to lower case character */
    {"V", "v"},			/* V - converted to lower case character */
    {"W", "w"},			/* W - converted to lower case character */
    {"X", "x"},			/* X - converted to lower case character */
    {"Y", "y"},			/* Y - converted to lower case character */
    {"Z", "z"},			/* Z - converted to lower case character */
    {"[", ""},			/* [ */
    {"\\", ""},			/* \ */
    {"]", ""},			/* ] */
    {"^", ""},			/* ^ */
    {"_", "_"},			/* _ - allowed character */

    /* \x60 -\x6f */
    {"`", ""},			/* ` */
    {"a", "a"},			/* a - allowed character */
    {"b", "b"},			/* b - allowed character */
    {"c", "c"},			/* c - allowed character */
    {"d", "d"},			/* d - allowed character */
    {"e", "e"},			/* e - allowed character */
    {"f", "f"},			/* f - allowed character */
    {"g", "g"},			/* g - allowed character */
    {"h", "h"},			/* h - allowed character */
    {"i", "i"},			/* i - allowed character */
    {"j", "j"},			/* j - allowed character */
    {"k", "k"},			/* k - allowed character */
    {"l", "l"},			/* l - allowed character */
    {"m", "m"},			/* m - allowed character */
    {"n", "n"},			/* n - allowed character */
    {"o", "o"},			/* o - allowed character */

    /* \x70 -\x7f */
    {"p", "p"},			/* p - allowed character */
    {"q", "q"},			/* q - allowed character */
    {"r", "r"},			/* r - allowed character */
    {"s", "s"},			/* s - allowed character */
    {"t", "t"},			/* t - allowed character */
    {"u", "u"},			/* u - allowed character */
    {"v", "v"},			/* v - allowed character */
    {"w", "w"},			/* w - allowed character */
    {"x", "x"},			/* x - allowed character */
    {"y", "y"},			/* y - allowed character */
    {"z", "z"},			/* z - allowed character */
    {"{", ""},			/* { */
    {"|", ""},			/* | */
    {"}", ""},			/* } */
    {"~", ""},			/* ^ */
    {"\x7f", ""},		/* DEL */

    {"\xc3\x84", "A"},		/* A with two dots */
    {"\xc3\xa4", "a"},		/* a with two dots */

    /* XXX - more UTF-8 table entries needed */

    {0, NULL}		/* MUST BE LAST */
};
